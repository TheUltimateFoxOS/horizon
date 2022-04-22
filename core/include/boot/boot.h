#pragma once

#include <stdint.h>
#include <renderer/framebuffer.h>
#include <acpi/acpi_tables.h>

namespace boot {
	struct boot_info_memmap_entry_t {
		uint64_t base;
		uint64_t length;
		uint32_t type;
	};

	struct boot_info_module_t {
		uint64_t begin;
		uint64_t end;
		char string[128];
	};

	struct boot_info_t {
		renderer::framebuffer_t framebuffer;

		char* command_line;

		acpi::rsdp2_t* rsdp;

		void* kernel_file;

		uint64_t memmap_entries;
		boot_info_memmap_entry_t* memmap;

		uint64_t module_count;
		boot_info_module_t* modules;

		void* virtual_base_address;
		void* physical_base_address;
		void* hhdm_base_address;

		char* boot_protocol_name;

		void* smbios_entry_32;
		void* smbios_entry_64;
	};

	#define MMAP_USABLE                 1
	#define MMAP_RESERVED               2
	#define MMAP_ACPI_RECLAIMABLE       3
	#define MMAP_ACPI_NVS               4
	#define MMAP_BAD_MEMORY             5
	#define MMAP_BOOTLOADER_RECLAIMABLE 6
	#define MMAP_KERNEL_AND_MODULES     7
	#define MMAP_FRAMEBUFFER            8

	#define halt_cpu() while (true) { __asm__ __volatile__ ("cli; hlt"); }

	void print_boot_info(boot_info_t* info, void (*write_str)(char* str));

	extern boot_info_t boot_info;
}