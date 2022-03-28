#include <graphic_syscalls.h>

#include <utils/string.h>
#include <renderer/renderer.h>

using namespace syscall;

void syscall::sys_copy_from_fb(interrupts::s_registers* regs) {
	uint32_t* user_address = (uint32_t*) regs->rbx;

	uint64_t base = (uint64_t) renderer::default_framebuffer.base_address;
	uint64_t bytes_per_scanline = renderer::default_framebuffer.width * 4;
	uint64_t fb_height = renderer::default_framebuffer.height;
	uint64_t fb_size = renderer::default_framebuffer.buffer_size;

	for (int vertical_scanline = 0; vertical_scanline < fb_height; vertical_scanline ++){
		uint64_t pix_ptr_base = base + (bytes_per_scanline * vertical_scanline);
		for (uint32_t* pixPtr = (uint32_t*)pix_ptr_base; pixPtr < (uint32_t*)(pix_ptr_base + bytes_per_scanline); pixPtr ++){
			user_address[vertical_scanline * renderer::default_framebuffer.width + (pixPtr - (uint32_t*)pix_ptr_base)] = *pixPtr;
		}
	}
}
