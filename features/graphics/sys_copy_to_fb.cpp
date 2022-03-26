#include <graphic_syscalls.h>

#include <utils/string.h>
#include <renderer/renderer.h>

using namespace syscall;

void syscall::sys_copy_to_fb(interrupts::s_registers* regs) {
	void* user_address = (void*) regs->rbx;

	memcpy(renderer::default_framebuffer.base_address, user_address, renderer::default_framebuffer.buffer_size);
}