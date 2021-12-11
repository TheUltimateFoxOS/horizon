#include <syscall/syscall.h>

#include <memory/page_frame_allocator.h>
#include <utils/lock.h>

using namespace syscall;

define_spinlock(sys_memory_lock)

void syscall::sys_memory_alloc(interrupts::s_registers* regs) {
	uint64_t size = regs->rbx;

	atomic_acquire_spinlock(sys_memory_lock);
	uint64_t addr = (uint64_t) memory::global_allocator.request_pages(size / 4096 + 1);
	atomic_release_spinlock(sys_memory_lock);

	regs->rbx = addr;
}

void syscall::sys_memory_free(interrupts::s_registers* regs) {
	uint64_t size = regs->rbx;
	uint64_t addr = regs->rcx;

	atomic_acquire_spinlock(sys_memory_lock);
	memory::global_allocator.free_pages((void*) addr, size / 4096 + 1);
	atomic_release_spinlock(sys_memory_lock);
}