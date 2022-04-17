#include <boot/limine.h>
#include <boot/boot.h>

static volatile limine_terminal_request terminal_request = {
    .id = LIMINE_TERMINAL_REQUEST,
    .revision = 0
};

extern "C" void _start_limine();

static volatile limine_entry_point_request entry_point_request = {
	.id = LIMINE_ENTRY_POINT_REQUEST,
	.revision = 0,
	.entry = _start_limine
};

static volatile limine_framebuffer_request framebuffer_request = {
	.id = LIMINE_FRAMEBUFFER_REQUEST,
	.revision = 0
};

static volatile limine_kernel_file_request kernel_file_request = {
	.id = LIMINE_KERNEL_FILE_REQUEST,
	.revision = 0
};

static volatile limine_rsdp_request rsdp_request = {
	.id = LIMINE_RSDP_REQUEST,
	.revision = 0
};

static volatile limine_memmap_request memmap_request = {
	.id = LIMINE_MEMMAP_REQUEST,
	.revision = 0
};

static volatile limine_module_request module_request = {
	.id = LIMINE_MODULE_REQUEST,
	.revision = 0
};

static volatile limine_kernel_address_request kernel_address_request = {
	.id = LIMINE_KERNEL_ADDRESS_REQUEST,
	.revision = 0
}; 

static volatile limine_hhdm_request hhdm_request = {
	.id = LIMINE_HHDM_REQUEST,
	.revision = 0
};

extern "C" void main();

int limine_memmap_entry_conv(int limine_id) {
	switch(limine_id) {
		case LIMINE_MEMMAP_USABLE:
			return MMAP_USABLE;
		case LIMINE_MEMMAP_RESERVED:
			return MMAP_RESERVED;
		case LIMINE_MEMMAP_ACPI_RECLAIMABLE:
			return MMAP_ACPI_RECLAIMABLE;
		case LIMINE_MEMMAP_ACPI_NVS:
			return MMAP_ACPI_NVS;
		case LIMINE_MEMMAP_BAD_MEMORY:
			return MMAP_BAD_MEMORY;
		case LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE:
			return MMAP_BOOTLOADER_RECLAIMABLE;
		case LIMINE_MEMMAP_KERNEL_AND_MODULES:
			return MMAP_KERNEL_AND_MODULES;
		case LIMINE_MEMMAP_FRAMEBUFFER:
			return MMAP_FRAMEBUFFER;
		default:
			return MMAP_RESERVED;
	}
}

extern "C" void limine_entry() {
	if (framebuffer_request.response->framebuffers[0]->bpp != 32) {
		terminal_request.response->write(terminal_request.response->terminals[0], "Not a 32-bit framebuffer\n", 31);
		halt_cpu();
	}

	boot::boot_info.framebuffer = {
		.base_address = framebuffer_request.response->framebuffers[0]->address,
		.buffer_size = (size_t) framebuffer_request.response->framebuffers[0]->width * framebuffer_request.response->framebuffers[0]->height * 4,
		.width = framebuffer_request.response->framebuffers[0]->width,
		.height = framebuffer_request.response->framebuffers[0]->height,
	};

	boot::boot_info.command_line = kernel_file_request.response->kernel_file->cmdline;

	boot::boot_info.rsdp = (acpi::rsdp2_t*) rsdp_request.response->address;

	boot::boot_info.kernel_file = kernel_file_request.response->kernel_file->address;

	boot::boot_info_memmap_entry_t memmap[memmap_request.response->entry_count];

	for (size_t i = 0; i < memmap_request.response->entry_count; i++) {
		memmap[i].type = limine_memmap_entry_conv(memmap_request.response->entries[i]->type);
		memmap[i].base = memmap_request.response->entries[i]->base;
		memmap[i].length = memmap_request.response->entries[i]->length;
	}

	boot::boot_info.memmap = memmap;
	boot::boot_info.memmap_entries = memmap_request.response->entry_count;

	boot::boot_info_module_t modules[module_request.response->module_count];

	for (int i = 0; i < module_request.response->module_count; i++) {
		modules[i].begin = (uint64_t) module_request.response->modules[i]->address;
		modules[i].end = (uint64_t) module_request.response->modules[i]->address + module_request.response->modules[i]->size;
		for (int j = 0; module_request.response->modules[i]->cmdline[j] != 0; j++) {
			modules[i].string[j] = module_request.response->modules[i]->cmdline[j];
			modules[i].string[j + 1] = 0;
		}
	}

	boot::boot_info.modules = modules;
	boot::boot_info.module_count = module_request.response->module_count;

	boot::boot_info.virtual_base_address = (void*) kernel_address_request.response->virtual_base;
	boot::boot_info.physical_base_address = (void*) kernel_address_request.response->physical_base;

	boot::boot_info.hhdm_base_address = (void*) hhdm_request.response->offset;

	boot::boot_info.boot_protocol_name = "limine";

	main();

	while(1) {
		__asm__ __volatile__("hlt");
	}
}
