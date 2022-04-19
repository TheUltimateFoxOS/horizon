#include <elf/kernel_module.h>

#include <elf/elf_resolver.h>
#include <elf/elf.h>
#include <fs/vfs.h>
#include <utils/abort.h>
#include <utils/log.h>
#include <utils/string.h>
#include <utils/list.h>

#include <renderer/font_renderer.h>

#include <memory/page_frame_allocator.h>

#include <config.h>

namespace elf {
	char* module_blacklist[128] = {
		nullptr
	};

	list<module_t*>* modules = nullptr;
}

using namespace elf;


void kernel_module_render_status(char* status, uint64_t color) {
#ifndef NICE_BOOT_ANIMATION
	renderer::global_font_renderer->cursor_position.x = renderer::global_font_renderer->target_frame_buffer->width - 8 * (strlen(status) + 4);
	
	uint64_t old_color = renderer::global_font_renderer->color;
	printf("[");
	renderer::global_font_renderer->color = color;
	printf("%s", status);
	renderer::global_font_renderer->color = old_color;
	printf("]\n");
#endif
}

void elf::load_kernel_module(char* path, bool announce) {
	if (announce) {
		printf("Loading kernel module %s", path);
	}

	fs::vfs::file_t* file = fs::vfs::global_vfs->open(path);

	if (file == nullptr) {
		abortf("Could not open kernel module file: %s", path);
	}

	int page_amount = file->size / 0x1000 + 1;
	void* elf_contents = memory::global_allocator.request_pages(page_amount);

	fs::vfs::global_vfs->read(file, elf_contents, file->size, 0);

	load_kernel_module(elf_contents, file->size);

	fs::vfs::global_vfs->close(file);
	memory::global_allocator.free_pages(elf_contents, page_amount);

	if (announce) {
		kernel_module_render_status((char*) "ok", 0xff00ff00);
	}
}

void elf::load_kernel_module(void* module, uint32_t size) {
	if (modules == nullptr) {
		debugf("Allocating modules list\n");
		modules = new list<module_t*>(10);
	}

	Elf64_Ehdr* header = (Elf64_Ehdr*) module;

	if (__builtin_bswap32(header->e_ident.i) != elf::MAGIC) {
		abortf("Invalid ELF magic");
	}

	if (header->e_ident.c[elf::EI_CLASS] != elf::ELFCLASS64) {
		abortf("ELF not 64 bit");
	}

	if(header->e_type != elf::ET_REL) {
		abortf("ELF not relocatable");
	}

	void* base_address = memory::global_allocator.request_pages((uint64_t) size / 0x1000 + 1);

	memcpy(base_address, module, size);

	for (unsigned int i = 0; i < header->e_shnum; ++i) {
		Elf64_Shdr* sh = (Elf64_Shdr*) ((uint64_t) base_address +header->e_shoff + header->e_shentsize * i);

		// print section information
		// debugf("Section %d: %d\n", i, sh->sh_type);

		if (sh->sh_type == 8 /* SHT_NOBITS */) {
			sh->sh_addr = (uint64_t) memory::global_allocator.request_pages(sh->sh_size / 0x1000 + 1);
			memset((void*) sh->sh_addr, 0, sh->sh_size);
		} else {
			sh->sh_addr = (uint64_t) ((uint64_t) base_address + sh->sh_offset);
		}
	}

	module_t* module_data = nullptr;

	for (int i = 0; i < header->e_shnum; i++) {
		Elf64_Shdr* sh = (Elf64_Shdr*) ((uint64_t) base_address + header->e_shoff + header->e_shentsize * i);

		if (sh->sh_type != 2 /* SHT_SYMTAB */) {
			continue;
		}

		Elf64_Shdr* strtab_hdr = (Elf64_Shdr*) ((uint64_t) base_address + header->e_shoff + header->e_shentsize * sh->sh_link);
		char* sym_names = (char*) strtab_hdr->sh_addr;
		Elf64_Sym* sym_table = (Elf64_Sym*) sh->sh_addr;

		for (uint16_t sym = 0; sym < sh->sh_size / sizeof(Elf64_Sym); ++sym) {

			if (sym_table[sym].st_shndx > 0 && sym_table[sym].st_shndx < 0xFF00) {
				Elf64_Shdr* sym_hdr = (Elf64_Shdr*) ((uint64_t) base_address + header->e_shoff + header->e_shentsize * sym_table[sym].st_shndx);
				sym_table[sym].st_value += (uint64_t) sym_hdr->sh_addr;
				
			} else if (sym_table[sym].st_shndx == 0 && sym_table[sym].st_name > 0) {
				char* _symbol = sym_names + sym_table[sym].st_name;
				uint64_t _symbol_addr = resolve_symbol(_symbol);

				if (_symbol_addr == 0) {
					debugf("WARNING: Could not resolve symbol %s\n", _symbol);
					sym_table[sym].st_value = 0;
				} else {
					// debugf("Resolved symbol %s to %x\n", _symbol, _symbol_addr);
					sym_table[sym].st_value = _symbol_addr;
				}

			}
			
			if (sym_table[sym].st_name > 0) {
				if (strcmp(sym_names + sym_table[sym].st_name, (char*) "__module__") == 0) {
					module_data = (module_t*) sym_table[sym].st_value;
				}
			}
		}
	}

	if (module_data == nullptr) {
		abortf("Could not find module data");
	}

	for (int i = 0; i < header->e_shnum; i++) {
		Elf64_Shdr* sh = (Elf64_Shdr*) ((uint64_t) base_address + header->e_shoff + header->e_shentsize * i);

		if (sh->sh_type != 4 /* SHT_REL */) {
			continue;
		}

		Elf64_Rela* rel_table = (Elf64_Rela*) sh->sh_addr;

		Elf64_Shdr* target_section = (Elf64_Shdr*) ((uint64_t) base_address + header->e_shoff + header->e_shentsize * sh->sh_info);

		Elf64_Shdr* symbol_section = (Elf64_Shdr*) ((uint64_t) base_address + header->e_shoff + header->e_shentsize * sh->sh_link);
		Elf64_Sym* symbol_table = (Elf64_Sym*) symbol_section->sh_addr;

		for (unsigned int rela = 0; rela < sh->sh_size / sizeof(Elf64_Rela); ++rela) {
			uintptr_t target = rel_table[rela].r_offset + target_section->sh_addr;
			switch (ELF64_R_TYPE(rel_table[rela].r_info)) {
				case R_X86_64_64:
					*(uint64_t*) target = rel_table[rela].r_addend + symbol_table[ELF64_R_SYM(rel_table[rela].r_info)].st_value;
					break;
				
				case R_X86_64_32:
					*(uint32_t*) target = rel_table[rela].r_addend + symbol_table[ELF64_R_SYM(rel_table[rela].r_info)].st_value;
					break;

				case R_X86_64_PC32:
					*(uint32_t*) target = rel_table[rela].r_addend + symbol_table[ELF64_R_SYM(rel_table[rela].r_info)].st_value - target;
					break;

				case R_X86_64_PLT32:
					*(uint32_t*) target = rel_table[rela].r_addend + symbol_table[ELF64_R_SYM(rel_table[rela].r_info)].st_value - target;
					break;

				case R_X86_64_32S:
					*(int32_t*) target = rel_table[rela].r_addend + symbol_table[ELF64_R_SYM(rel_table[rela].r_info)].st_value;
					break;

				default:
					debugf("WARNING: Unsupported relocation type %d\n", ELF64_R_TYPE(rel_table[rela].r_info));
					break;
			}
		}
	}

	module_data->loaded_pages = size / 0x1000 + 1;
	module_data->base_address = base_address;

	for (int i = 0; module_blacklist[i] != nullptr; i++) {
		if (strcmp((char*) module_data->name, module_blacklist[i]) == 0) {
			debugf("Module %s is blacklisted!\n", module_data->name);
			memory::global_allocator.free_pages(base_address, (uint64_t) size / 0x1000 + 1);
			return;
		}
	}

	elf_symbol_resolver* resolver = new elf_symbol_resolver(base_address);
	other_resolver[other_resolver_count++] = resolver;

	modules->add(module_data);

	debugf("Loaded module %s at %x\n", module_data->name, base_address);

	if (module_data->init != nullptr) {
		__asm__ __volatile__ (
			"movq %0, %%rax;"
			"callq *%%rax;"
			:
			: "r"(module_data->init)
		);
	}
}


void elf::device_init_all() {
	debugf("Calling device_init for all modules\n");

	if (!modules) {
		debugf("No modules loaded\n");
		return;
	}

	modules->foreach([](list<module_t*>::node* n) {
		module_t* module = n->data;
		if (module->device_init != nullptr) {
			debugf("Calling device_init for module %s\n", module->name);
			module->device_init();
		}
	});
}

void elf::fs_init_all() {
	debugf("Calling fs_init for all modules\n");

	if (!modules) {
		debugf("No modules loaded\n");
		return;
	}

	modules->foreach([](list<module_t*>::node* n) {
		module_t* module = n->data;
		if (module->fs_init != nullptr) {
			debugf("Calling fs_init for module %s\n", module->name);
			module->fs_init();
		}
	});
}