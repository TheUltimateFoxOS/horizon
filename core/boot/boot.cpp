#include <boot/boot.h>
#include <utils/string.h>
#include <utils/assert.h>
#include <memory/heap.h>

namespace boot {
	boot_info_t boot_info;
}

using namespace boot;

extern "C" void _start() {
	while (1) {
		__asm__ __volatile__("hlt");
	}
}

char* memmap_id_to_str(int id) {
	switch (id) {
		case MMAP_USABLE:
			return (char*) "MMAP_USABLE";
		case MMAP_RESERVED:
			return (char*) "MMAP_RESERVED";
		case MMAP_ACPI_RECLAIMABLE:
			return (char*) "MMAP_ACPI_RECLAIMABLE";
		case MMAP_ACPI_NVS:
			return (char*) "MMAP_ACPI_NVS";
		case MMAP_BAD_MEMORY:
			return (char*) "MMAP_BAD_MEMORY";
		case MMAP_BOOTLOADER_RECLAIMABLE:
			return (char*) "MMAP_BOOTLOADER_RECLAIMABLE";
		case MMAP_KERNEL_AND_MODULES:
			return (char*) "MMAP_KERNEL_AND_MODULES";
		case MMAP_FRAMEBUFFER:
			return (char*) "MMAP_FRAMEBUFFER";
		default:
			return (char*) "UNKNOWN";
	}
}

void boot::print_boot_info(boot_info_t* info, void (*write_str)(char* str)) {
	char buffer[256];

	memset(buffer, 0, 256);
	sprintf(buffer, "Booted using %s protocol\n", info->boot_protocol_name);
	write_str(buffer);

	memset(buffer, 0, 256);
	sprintf(buffer, "Framebuffer:\n");
	write_str(buffer);

	memset(buffer, 0, 256);
	sprintf(buffer, "> base_address: 0x%x\n", info->framebuffer.base_address);
	write_str(buffer);

	memset(buffer, 0, 256);
	sprintf(buffer, "> buffer_size: %d\n", info->framebuffer.buffer_size);
	write_str(buffer);

	memset(buffer, 0, 256);
	sprintf(buffer, "> width: %d\n", info->framebuffer.width);
	write_str(buffer);

	memset(buffer, 0, 256);
	sprintf(buffer, "> height: %d\n", info->framebuffer.height);
	write_str(buffer);

	memset(buffer, 0, 256);
	sprintf(buffer, "Command line: %s\n", info->command_line);
	write_str(buffer);

	memset(buffer, 0, 256);
	sprintf(buffer, "RSDP: 0x%x\n", info->rsdp);
	write_str(buffer);

	memset(buffer, 0, 256);
	sprintf(buffer, "Kernel file: 0x%x\n", info->kernel_file);
	write_str(buffer);

	memset(buffer, 0, 256);
	sprintf(buffer, "Memory map (%d entries):\n", info->memmap_entries);
	write_str(buffer);

	for (int i = 0; i < info->memmap_entries; i++) {
		memset(buffer, 0, sizeof(buffer));
		sprintf(buffer, "> %d: %s 0x%x -> 0x%x\n", i, memmap_id_to_str(info->memmap[i].type), info->memmap[i].base, info->memmap[i].base + info->memmap[i].length);
		write_str(buffer);
	}

	memset(buffer, 0, sizeof(buffer));
	sprintf(buffer, "Modules (%d entries):\n", info->module_count);
	write_str(buffer);

	for (int i = 0; i < info->module_count; i++) {
		memset(buffer, 0, sizeof(buffer));
		sprintf(buffer, "> %d: %s at 0x%x\n", i, info->modules[i].string, info->modules[i].begin);
		write_str(buffer);
	}

	memset(buffer, 0, sizeof(buffer));
	sprintf(buffer, "Virtual base address: 0x%x\n", info->virtual_base_address);
	write_str(buffer);

	memset(buffer, 0, sizeof(buffer));
	sprintf(buffer, "Physical base address: 0x%x\n", info->physical_base_address);
	write_str(buffer);

	memset(buffer, 0, sizeof(buffer));
	sprintf(buffer, "Higher Half Direct Map base address: 0x%x\n", info->hhdm_base_address);
	write_str(buffer);

	memset(buffer, 0, sizeof(buffer));
	sprintf(buffer, "SMBIOS 64 address: 0x%x\n", info->smbios_entry_64);
	write_str(buffer);

	memset(buffer, 0, sizeof(buffer));
	sprintf(buffer, "SMBIOS 32 address: 0x%x\n", info->smbios_entry_32);
	write_str(buffer);
}

char* __bootinfo_dev_fs_file_data = nullptr;
int __bootinfo_dev_fs_file_size = 0;

bootinfo_dev_fs_file::bootinfo_dev_fs_file(boot_info_t* info) {
	print_boot_info(info, [](char* str) {
		int len = 0;
		for (int i = 0; str[i] != 0; i++) {
			len++;
		}

		if (__bootinfo_dev_fs_file_size == 0) {
			__bootinfo_dev_fs_file_data = (char*) memory::malloc(len);
			memcpy(__bootinfo_dev_fs_file_data, str, len);
			__bootinfo_dev_fs_file_size = len;
		} else {
			__bootinfo_dev_fs_file_data = (char*) memory::realloc(__bootinfo_dev_fs_file_data, __bootinfo_dev_fs_file_size, __bootinfo_dev_fs_file_size + len);
			memcpy(__bootinfo_dev_fs_file_data + __bootinfo_dev_fs_file_size, str, len);
			__bootinfo_dev_fs_file_size += len;
		}
	});

	__bootinfo_dev_fs_file_data = (char*) memory::realloc(__bootinfo_dev_fs_file_data, __bootinfo_dev_fs_file_size, __bootinfo_dev_fs_file_size + 1);
	__bootinfo_dev_fs_file_data[__bootinfo_dev_fs_file_size] = 0;

	this->size = __bootinfo_dev_fs_file_size + 1;
	this->data = __bootinfo_dev_fs_file_data;
}

void bootinfo_dev_fs_file::read(fs::file_t* file, void* buffer, size_t size, size_t offset) {
	assert(offset + size <= this->size);
	memcpy(buffer, this->data + offset, size);
}

void bootinfo_dev_fs_file::prepare_file(fs::file_t* file) {
	file->size = this->size;
}

char* bootinfo_dev_fs_file::get_name() {
	return (char*) "bootinfo";
}