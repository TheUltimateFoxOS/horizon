#include <syscall/syscall.h>

#include <memory/page_frame_allocator.h>

using namespace syscall;

void syscall::sys_memory_info(interrupts::s_registers* regs) {
	regs->rax = memory::get_memory_size();
	regs->rbx = memory::global_allocator.get_free_RAM();
	regs->rcx = memory::global_allocator.get_used_RAM();
	regs->rdx = memory::global_allocator.get_reserved_RAM();
}
