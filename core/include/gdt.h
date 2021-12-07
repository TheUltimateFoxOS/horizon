#pragma once

#include <stdint.h>

struct gdt_descriptor_t {
	uint16_t size;
	uint64_t offset;
} __attribute__((packed));

struct gdt_entry_t {
	uint16_t limit0;
	uint16_t base0;
	uint8_t base1;
	uint8_t access_byte;
	uint8_t limit1_flags;
	uint8_t base2;
}__attribute__((packed));

struct gdt_t {
	gdt_entry_t null;
	gdt_entry_t kernel_code;
	gdt_entry_t kernel_data;
	gdt_entry_t user_null;
	gdt_entry_t user_code;
	gdt_entry_t user_data;
} __attribute__((packed)) 
__attribute((aligned(0x1000)));

extern gdt_t default_gdt;
extern "C" void load_gdt(gdt_descriptor_t* gdt_descriptor);

void setup_gdt();