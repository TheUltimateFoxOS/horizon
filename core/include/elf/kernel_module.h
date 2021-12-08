#pragma once

#include <stdint.h>

namespace elf {

	struct module_t {
		const char* name;
		void (*init)();
		void (*device_init)();
		void (*fs_init)();
		void* base_address;
		uint64_t loaded_pages;
	};

	#define define_module(name, init, device_init, fs_init) elf::module_t __module__ { name, init, device_init, fs_init, 0, 0 }

	void load_kernel_module(char* path);
	void load_kernel_module(void* module, uint32_t size);

	void device_init_all();
	void fs_init_all();
}