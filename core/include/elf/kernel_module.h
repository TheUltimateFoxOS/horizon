#pragma once

#include <stdint.h>

#include <utils/list.h>

namespace elf {

	struct module_t {
		const char* name;
		void (*init)();
		void (*device_init)();
		void (*fs_init)();
		void* base_address;
		uint64_t loaded_pages;
	};

	extern char* module_blacklist[128];
	extern list<module_t*>* modules;


	#define null_ptr_func ((void (*)())0)

	#define define_module(name, init, device_init, fs_init) elf::module_t __module__ { name, init, device_init, fs_init, 0, 0 }

	module_t* load_kernel_module(char* path, bool announce);
	module_t* load_kernel_module(void* module, uint32_t size);

	void device_init_all();
	void fs_init_all();
}