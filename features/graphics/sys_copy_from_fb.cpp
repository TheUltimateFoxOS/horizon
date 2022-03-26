#include <graphic_syscalls.h>

#include <utils/string.h>
#include <renderer/renderer.h>

using namespace syscall;

void syscall::sys_copy_from_fb(interrupts::s_registers* regs) {
	void* user_address = (void*) regs->rbx;

	memcpy(user_address, renderer::default_framebuffer.base_address, renderer::default_framebuffer.buffer_size);
}
