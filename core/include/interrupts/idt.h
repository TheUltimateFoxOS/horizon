#pragma once

#include <stdint.h>

#define idt_ta_interrupt_gate	0b10001110
#define idt_ta_call_gate		0b10001100
#define idt_ta_trap_gate		0b10001111

namespace interrupts {
	struct idt_desc_entry_t {
		uint16_t offset_0;
		uint16_t selector;
		uint8_t ist;
		uint8_t type_attr;
		uint16_t offset_1;
		uint32_t offset_2;
		uint32_t ignore;
		void set_offset(uint64_t offset);
		uint64_t get_offset();
	};

	struct idt_t {
		uint16_t limit;
		uint64_t offset;
	} __attribute__((packed));
}