#pragma once

#include <utils/list.h>
#include <stdint.h>

#include <interrupts/interrupts.h>

namespace syscall {
	struct syscall_entry_t {
		uint64_t id;
		char name[32];
		void (*handler)(interrupts::s_registers*);
	};

	extern list<syscall_entry_t>* syscall_id_list;

	void setup();

	void register_syscall(void (*handler)(interrupts::s_registers*), const char* name);

	void get_id_syscall(interrupts::s_registers* regs);

	extern "C" uint64_t syscall_table[];
	extern "C" uint64_t syscall_table_size;
	extern "C" void syscall_interrupt_stub();
}