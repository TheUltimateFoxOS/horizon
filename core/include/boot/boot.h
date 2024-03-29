#pragma once

#include <stdint.h>
#include <renderer/framebuffer.h>
#include <acpi/acpi_tables.h>

#include <fs/dev_fs.h>
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

	struct boot_smp_core_t {
		uint32_t processor_id;
		uint32_t lapic_id;
		uint64_t* target_stack;
		uint64_t* goto_address;
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

		uint64_t smp_entries;
		boot_smp_core_t* smp;
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

	class bootinfo_dev_fs_file : public fs::dev_fs_file {
		public:
			void read(fs::file_t* file, void* buffer, size_t size, size_t offset);

			void prepare_file(fs::file_t* file);
			char* get_name();
		
			bootinfo_dev_fs_file(boot_info_t* info);
		private:
			char* data;
			int size;
	};

	extern boot_info_t boot_info;
}