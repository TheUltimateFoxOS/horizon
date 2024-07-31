#include <boot/stivale2.h>
#include <boot/boot.h>

#include <output/output.h>
#include <utils/log.h>

static unsigned char stack[0x4000 * 16];

static stivale2_header_tag_terminal terminal_tag = {
	.tag = {
		.identifier = STIVALE2_HEADER_TAG_TERMINAL_ID,
		.next = 0,
	},
	.flags = 0
};

static stivale2_header_tag_framebuffer framebuffer_tag = {
	.tag = {
		.identifier = STIVALE2_HEADER_TAG_FRAMEBUFFER_ID,
		.next = (uint64_t) &terminal_tag,
	},
	.framebuffer_width = 0,
	.framebuffer_height = 0,
	.framebuffer_bpp = 32
};

static stivale2_header_tag_smp smp_tag = {
	.tag = {
		.identifier = STIVALE2_HEADER_TAG_SMP_ID,
		.next = (uint64_t) &framebuffer_tag
	},
	.flags = 0 // DO NOT use x2APIC
};

extern "C" void _start_stivale2();

__attribute__((section(".stivale2hdr"), used))
static stivale2_header __stivale2_header = {
	.entry_point = (uintptr_t) _start_stivale2,
	.stack = (uintptr_t) stack + sizeof(stack),
	.flags = (1 << 2) | (1 << 3) | (1 << 4),
	.tags = (uint64_t) &smp_tag
};

typedef void (*term_write_t)(const char*, int len);
term_write_t stivale2_term_write;

class stivale2_output_device : public output::output_device {
public:
	virtual void putstring(const char *str) {
		int len = 0;
		for (int i = 0; str[i] != 0; i++) {
			len++;
		}

		stivale2_term_write(str, len);
	}

	virtual void putchar(char c) {
		stivale2_term_write(&c, 1);
	}
};

extern "C" void main();


int stivale2_memmap_entry_conv(int stivale2_id) {
	switch(stivale2_id) {
		case STIVALE2_MMAP_USABLE:
			return MMAP_USABLE;
		case STIVALE2_MMAP_RESERVED:
			return MMAP_RESERVED;
		case STIVALE2_MMAP_ACPI_RECLAIMABLE:
			return MMAP_ACPI_RECLAIMABLE;
		case STIVALE2_MMAP_ACPI_NVS:
			return MMAP_ACPI_NVS;
		case STIVALE2_MMAP_BAD_MEMORY:
			return MMAP_BAD_MEMORY;
		case STIVALE2_MMAP_BOOTLOADER_RECLAIMABLE:
			return MMAP_BOOTLOADER_RECLAIMABLE;
		case STIVALE2_MMAP_KERNEL_AND_MODULES:
			return MMAP_KERNEL_AND_MODULES;
		case STIVALE2_MMAP_FRAMEBUFFER:
			return MMAP_FRAMEBUFFER;
		default:
			return MMAP_RESERVED;
	}
}

stivale2_output_device stivale2_output_device_;

extern uint64_t kernel_start;
extern uint64_t kernel_end;

extern "C" void stivale2_entry(stivale2_struct* bootinfo) {
	stivale2_struct_tag_terminal* terminal_tag = stivale2_tag_find<stivale2_struct_tag_terminal>(bootinfo, STIVALE2_STRUCT_TAG_TERMINAL_ID);
	stivale2_term_write = (term_write_t) terminal_tag->term_write;

	log::debug_device = &stivale2_output_device_;
	log::stdout_device = &stivale2_output_device_;

	stivale2_struct_tag_framebuffer* framebuffer_tag = stivale2_tag_find<stivale2_struct_tag_framebuffer>(bootinfo, STIVALE2_STRUCT_TAG_FRAMEBUFFER_ID);

	if (framebuffer_tag->framebuffer_bpp != 32) {
		stivale2_term_write("Framebuffer must be 32bpp\n", 27);
		halt_cpu();
	}

	boot::boot_info.framebuffer = {
		.base_address = (void*) framebuffer_tag->framebuffer_addr,
		.buffer_size = (size_t) framebuffer_tag->framebuffer_width * framebuffer_tag->framebuffer_height * 4,
		.width = framebuffer_tag->framebuffer_width,
		.height = framebuffer_tag->framebuffer_height,
		.pitch = framebuffer_tag->framebuffer_pitch,
	};

	stivale2_struct_tag_cmdline* cmdline_tag = stivale2_tag_find<stivale2_struct_tag_cmdline>(bootinfo, STIVALE2_STRUCT_TAG_CMDLINE_ID);
	boot::boot_info.command_line = (char*) cmdline_tag->cmdline;

	stivale2_struct_tag_rsdp* rsdp_tag = stivale2_tag_find<stivale2_struct_tag_rsdp>(bootinfo, STIVALE2_STRUCT_TAG_RSDP_ID);
	boot::boot_info.rsdp = (acpi::rsdp2_t*) rsdp_tag->rsdp;

	stivale2_struct_tag_kernel_file* kernel_file_tag = stivale2_tag_find<stivale2_struct_tag_kernel_file>(bootinfo, STIVALE2_STRUCT_TAG_KERNEL_FILE_ID);
	boot::boot_info.kernel_file = (void*) kernel_file_tag->kernel_file;

	stivale2_struct_tag_memmap* memmap_tag = stivale2_tag_find<stivale2_struct_tag_memmap>(bootinfo, STIVALE2_STRUCT_TAG_MEMMAP_ID);
	boot::boot_info_memmap_entry_t memmap[memmap_tag->entries];

	for (int i = 0; i < memmap_tag->entries; i++) {
		memmap[i].type = stivale2_memmap_entry_conv(memmap_tag->memmap[i].type);
		memmap[i].base = memmap_tag->memmap[i].base;
		memmap[i].length = (size_t) memmap_tag->memmap[i].length;
	}

	boot::boot_info.memmap = memmap;
	boot::boot_info.memmap_entries = memmap_tag->entries;

	stivale2_struct_tag_modules* modules_tag = stivale2_tag_find<stivale2_struct_tag_modules>(bootinfo, STIVALE2_STRUCT_TAG_MODULES_ID);
	boot::boot_info_module_t modules[modules_tag->module_count];

	for (int i = 0; i < modules_tag->module_count; i++) {
		modules[i].begin = modules_tag->modules[i].begin;
		modules[i].end = modules_tag->modules[i].end;
		for (int j = 0; modules_tag->modules[i].string[j] != 0; j++) {
			modules[i].string[j] = modules_tag->modules[i].string[j];
			modules[i].string[j + 1] = 0;
		}
	}

	boot::boot_info.modules = modules;
	boot::boot_info.module_count = modules_tag->module_count;

	stivale2_struct_tag_kernel_base_address* kernel_base_address_tag = stivale2_tag_find<stivale2_struct_tag_kernel_base_address>(bootinfo, STIVALE2_STRUCT_TAG_KERNEL_BASE_ADDRESS_ID);
	boot::boot_info.physical_base_address = (void*) kernel_base_address_tag->physical_base_address;
	boot::boot_info.virtual_base_address = (void*) kernel_base_address_tag->virtual_base_address;

	stivale2_struct_tag_hhdm* hhdm_tag = stivale2_tag_find<stivale2_struct_tag_hhdm>(bootinfo, STIVALE2_STRUCT_TAG_HHDM_ID);
	boot::boot_info.hhdm_base_address = (void*) hhdm_tag->addr;

	boot::boot_info.boot_protocol_name = (char*) "stivale2";

	stivale2_struct_tag_smbios* smbios_tag = stivale2_tag_find<stivale2_struct_tag_smbios>(bootinfo, STIVALE2_STRUCT_TAG_SMBIOS_ID);
	boot::boot_info.smbios_entry_32 = (void*) smbios_tag->smbios_entry_32;
	boot::boot_info.smbios_entry_64 = (void*) smbios_tag->smbios_entry_64;

	stivale2_struct_tag_smp* smp_tag = stivale2_tag_find<stivale2_struct_tag_smp>(bootinfo, STIVALE2_STRUCT_TAG_SMP_ID);

	boot::boot_smp_core_t smp[smp_tag->cpu_count];
	for (int i = 0; i < smp_tag->cpu_count; i++) {
		smp[i] = {
			.processor_id = smp_tag->smp_info[i].processor_id,
			.lapic_id = smp_tag->smp_info[i].lapic_id,
			.target_stack = &smp_tag->smp_info[i].target_stack,
			.goto_address = &smp_tag->smp_info[i].goto_address,
		};
	}


	boot::boot_info.smp_entries = smp_tag->cpu_count;
	boot::boot_info.smp = smp;


	boot::print_boot_info(&boot::boot_info, [](char* str) {
		int len = 0;
		for (int i = 0; str[i] != 0; i++) {
			len++;
		}

		stivale2_term_write(str, len);
	});

	main();

	while(1) {
		__asm__ __volatile__("hlt");
	}
}