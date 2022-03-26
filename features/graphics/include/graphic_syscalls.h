#pragma once

#include <syscall/syscall.h>

namespace syscall {
	void sys_copy_from_fb(interrupts::s_registers* regs);
	void sys_copy_to_fb(interrupts::s_registers* regs);
	void sys_fb_info(interrupts::s_registers* regs);
}