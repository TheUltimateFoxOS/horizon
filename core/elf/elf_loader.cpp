#include <elf/elf_loader.h>

#include <elf/elf.h>
#include <fs/vfs.h>
#include <memory/page_frame_allocator.h>
#include <utils/assert.h>

using namespace elf;

scheduler::task_t* elf::load_elf(char* filename, const char** argv, const char** envp) {
	fs::vfs::file_t* file = fs::vfs::global_vfs->open(filename);
	if (file == nullptr) {
		return nullptr;
	}

	int page_amount = file->size / 0x1000 + 1;
	void* elf_contents = memory::global_allocator.request_pages(page_amount);

	fs::vfs::global_vfs->read(file, elf_contents, file->size, 0);

	scheduler::task_t* task = load_elf(elf_contents, argv, envp);

	fs::vfs::global_vfs->close(file);
	memory::global_allocator.free_pages(elf_contents, page_amount);

	return task;
}

void elf_apply_relocations(void* ptr, Elf64_Ehdr* header, uint64_t vaddr, uint64_t elf_load_offset, uint64_t size) {
	Elf64_Phdr* ph = (Elf64_Phdr*) (((char*) ptr) + header->e_phoff);
	for (int i = 0; i < header->e_phnum; i++, ph++) {
		if (ph->p_type != elf::PT_DYNAMIC) {
			continue;
		}

		debugf("dynamic section found at %x\n", ph);

		uint64_t rela_offset = 0;
		uint64_t rela_size = 0;
		uint64_t rela_ent = 0;

		for (uint16_t j = 0; j < ph->p_filesz / sizeof(Elf64_Dyn); j++) {
			Elf64_Dyn* dyn = (Elf64_Dyn*) (((char*) ptr) + ph->p_offset + j * sizeof(Elf64_Dyn));

			switch (dyn->d_tag) {
				case DT_RELA:
					rela_offset = dyn->d_un;
					break;
				case DT_RELAENT:
					rela_ent = dyn->d_un;
					break;
				case DT_RELASZ:
					rela_size = dyn->d_un;
					break;
			}
		}

		debugf("rela_offset: %x\n", rela_offset);
		debugf("rela_size: %x\n", rela_size);
		debugf("rela_ent: %x\n", rela_ent);

		if (rela_offset != 0) {
			assert(rela_ent == sizeof(Elf64_Rela));

			for (uint64_t off = 0; off < rela_size; off += rela_ent) {
				Elf64_Rela* rela = (Elf64_Rela*) (((char*) ptr) + rela_offset + off);

				if (rela->r_info == R_X86_64_RELATIVE) {
					uint64_t* target = (uint64_t*) (elf_load_offset + rela->r_offset);
					*target = elf_load_offset + rela->r_addend;
				}
			}
		}
	}
}

scheduler::task_t* elf::load_elf(void* ptr, const char** argv, const char** envp) {
	Elf64_Ehdr* header = (Elf64_Ehdr*) ptr;


	if(__builtin_bswap32(header->e_ident.i) != elf::MAGIC) {
		return nullptr; // no elf
	}
	if(header->e_ident.c[elf::EI_CLASS] != elf::ELFCLASS64) {
		return nullptr; // not 64 bit
	}
	if(header->e_type != elf::ET_DYN) {
		return nullptr; // not pic
	}

	Elf64_Phdr* ph = (Elf64_Phdr*) (((char*) ptr) + header->e_phoff);

	void* last_dest;

	for (int i = 0; i < header->e_phnum; i++, ph++) {
		if (ph->p_type != elf::PT_LOAD) {
			continue;
		}
		last_dest = (void*) ((uint64_t) ph->p_vaddr + ph->p_memsz);
	}

	void* offset = memory::global_allocator.request_pages((uint64_t) last_dest / 0x1000 + 1);
	debugf("offset: %x\n", offset);

	ph = (Elf64_Phdr*) (((char*) ptr) + header->e_phoff);
	for (int i = 0; i < header->e_phnum; i++, ph++) {
		void* dest = (void*) ((uint64_t) ph->p_vaddr + (uint64_t) offset);
		void* src = ((char*) ptr) + ph->p_offset;


		if (ph->p_type != elf::PT_LOAD) {
			continue;
		}
		

		memset(dest, 0, ph->p_memsz);
		memcpy(dest, src, ph->p_filesz);

		elf_apply_relocations(ptr, header, (uint64_t) ph->p_vaddr, (uint64_t) offset, ph->p_filesz);
	}
	
	scheduler::task_t* new_task = scheduler::create_task((void*) (header->e_entry + (uint64_t) offset));
	new_task->is_elf = true;
	new_task->offset = offset;
	new_task->page_count = (uint64_t) last_dest / 0x1000 + 1;
	new_task->argv = (char**) argv;
	new_task->envp = (char**) envp;

	return new_task;
}