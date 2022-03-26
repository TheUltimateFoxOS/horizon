#include <graphic_syscalls.h>

#include <utils/string.h>
#include <renderer/renderer.h>

using namespace syscall;

void syscall::sys_fb_info(interrupts::s_registers* regs) {
	renderer::framebuffer_t* user_address = (renderer::framebuffer_t*) regs->rbx;

	*user_address = renderer::default_framebuffer;
}