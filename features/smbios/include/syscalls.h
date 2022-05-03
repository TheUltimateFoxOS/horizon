#pragma once

#include <syscall/syscall.h>

namespace syscall {
	struct cpu_info_t {
		uint64_t id;
		int speed;
		char manufacture[64];
		char version[64];
	};

	void sys_get_cpu_info(interrupts::s_registers* regs);
}