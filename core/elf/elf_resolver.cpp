#include <elf/elf_resolver.h>
#include <elf/elf.h>
#include <utils/log.h>
#include <utils/string.h>

using namespace elf;

namespace elf {
	elf_symbol_resolver* kernel_resolver = nullptr;
	elf_symbol_resolver* other_resolver[512] = {0};
	int other_resolver_count = 0;
}

elf_symbol_resolver::elf_symbol_resolver(void* raw_elf_file) {
	Elf64_Ehdr* ehdr = (Elf64_Ehdr*) raw_elf_file;

	if(__builtin_bswap32(ehdr->e_ident.i) != elf::MAGIC) {
		return; // no elf
	}
	if(ehdr->e_ident.c[elf::EI_CLASS] != elf::ELFCLASS64) {
		return; // not 64 bit
	}

	Elf64_Shdr* shdr = (Elf64_Shdr*) ((char*) raw_elf_file + ehdr->e_shoff);
	this->sym_info.shdr = shdr;
	this->sym_info.shdr_count = ehdr->e_shnum;

	int section_sting_index = ehdr->e_shstrndx;

	for (int i = 0; i < this->sym_info.shdr_count; i++) {
		Elf64_Shdr* shdr = &this->sym_info.shdr[i];
		if (shdr->sh_type == 0x2) { // symtab 
			this->sym_info.sym_entries = (Elf64_Sym*) ((char*) raw_elf_file + shdr->sh_offset);
			this->sym_info.sym_count = shdr->sh_size / sizeof(Elf64_Sym);
		} else if (shdr->sh_type == 0x3) { // strtab
			if (section_sting_index == i) {
				this->sym_info.sect_str_table = (char*) raw_elf_file + shdr->sh_offset;
				this->sym_info.sect_str_table_size = shdr->sh_size;
			} else {
				this->sym_info.sym_str_table = (char*) raw_elf_file + shdr->sh_offset;
				this->sym_info.sym_str_table_size = shdr->sh_size;
			}
		}
	}
}

void* elf_symbol_resolver::resolve(char* symbol_name) {
	for (int i = 0; i < this->sym_info.sym_count; i++) {
		Elf64_Sym* sym = &this->sym_info.sym_entries[i];
		if (sym->st_value == 0) {
			continue;
		}

		if(strcmp(&this->sym_info.sym_str_table[sym->st_name], symbol_name) == 0) {
			return (void*) sym->st_value;
		}
	}

	return nullptr;
}

char* elf_symbol_resolver::resolve(void* symbol_addr) {
	for (int i = 0; i < this->sym_info.sym_count; i++) {
		Elf64_Sym* sym = &this->sym_info.sym_entries[i];
		if (sym->st_value == 0) {
			continue;
		}

		if((uint64_t) symbol_addr >= (uint64_t) sym->st_value && (uint64_t) symbol_addr < (uint64_t) sym->st_value + sym->st_size) {
			return &this->sym_info.sym_str_table[sym->st_name] ;
		}
	}

	return (char*) "<unknown function>";
}

void elf::setup(stivale2_struct* bootinfo) {
	debugf("Setting up kernel elf resolver...\n");

	stivale2_struct_tag_kernel_file* kernel_file = stivale2_tag_find<stivale2_struct_tag_kernel_file>(bootinfo, STIVALE2_STRUCT_TAG_KERNEL_FILE_ID);
	kernel_resolver = new elf_symbol_resolver((void*) kernel_file->kernel_file);
}

uint64_t elf::resolve_symbol(char* name) {
	uint64_t sym_addr = (uint64_t) kernel_resolver->resolve(name);
	if (sym_addr == 0) {
		for (int i = 0; i < other_resolver_count; i++) {
			sym_addr = (uint64_t) other_resolver[i]->resolve(name);
			if (sym_addr != 0) {
				break;
			}
		}
	}

	return sym_addr;
}

char* elf::resolve_symbol(uint64_t address) {
	char* sym_name = kernel_resolver->resolve((void*) address);
	if (strcmp(sym_name, "<unknown function>") == 0) {
		for (int i = 0; i < other_resolver_count; i++) {
			sym_name = other_resolver[i]->resolve((void*) address);
			if (strcmp(sym_name, "<unknown function>") != 0) {
				break;
			}
		}
	}

	return sym_name;
}