#pragma once

#include <stdint.h>

#define PIC_EOI 0x20

#define ICW1_INIT 0x10
#define ICW1_ICW4 0x01
#define ICW4_8086 0x01

namespace interrupts {
	struct s_registers {
		uint64_t cr4;
		uint64_t cr3;
		uint64_t cr2;
		uint64_t cr0;
		uint64_t r15;
		uint64_t r14;
		uint64_t r13;
		uint64_t r12;
		uint64_t r11;
		uint64_t r10;
		uint64_t r9;
		uint64_t r8;
		uint64_t rdi;
		uint64_t rsi;
		uint64_t rbp;
		uint64_t rbx;
		uint64_t rdx;
		uint64_t rcx;
		uint64_t rax;
		uint64_t interrupt_number;
		uint64_t error_code;
		uint64_t rip;
		uint64_t cs;
		uint64_t rflags;
		uint64_t rsp;
	} __attribute__((packed));

	char* get_interrupt_name(int interrupt_number);

	void set_idt_gate(void* handler, uint8_t entry_offset, uint8_t type_attr, uint8_t selector);
	void prepare_interrupts();
}

extern "C" void intr_stub_0(void);
extern "C" void intr_stub_1(void);
extern "C" void intr_stub_2(void);
extern "C" void intr_stub_3(void);
extern "C" void intr_stub_4(void);
extern "C" void intr_stub_5(void);
extern "C" void intr_stub_6(void);
extern "C" void intr_stub_7(void);
extern "C" void intr_stub_8(void);
extern "C" void intr_stub_9(void);
extern "C" void intr_stub_10(void);
extern "C" void intr_stub_11(void);
extern "C" void intr_stub_12(void);
extern "C" void intr_stub_13(void);
extern "C" void intr_stub_14(void);
extern "C" void intr_stub_15(void);
extern "C" void intr_stub_16(void);
extern "C" void intr_stub_17(void);
extern "C" void intr_stub_18(void);

extern "C" void intr_stub_32(void);
extern "C" void intr_stub_33(void);
extern "C" void intr_stub_34(void);
extern "C" void intr_stub_35(void);
extern "C" void intr_stub_36(void);
extern "C" void intr_stub_37(void);
extern "C" void intr_stub_38(void);
extern "C" void intr_stub_39(void);
extern "C" void intr_stub_40(void);
extern "C" void intr_stub_41(void);
extern "C" void intr_stub_42(void);
extern "C" void intr_stub_43(void);
extern "C" void intr_stub_44(void);
extern "C" void intr_stub_45(void);
extern "C" void intr_stub_46(void);
extern "C" void intr_stub_47(void);