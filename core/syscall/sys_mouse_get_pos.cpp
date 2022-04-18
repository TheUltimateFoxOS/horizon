#include <syscall/syscall.h>

#include <utils/assert.h>
#include <input/mouse_input.h>

using namespace syscall;

void syscall::sys_mouse_get_pos(interrupts::s_registers* regs) {
	assert(input::default_mouse_input_device != nullptr);

	regs->rbx = input::default_mouse_input_device->last_x;
	regs->rcx = input::default_mouse_input_device->last_y;
}