#include <syscall/syscall.h>

#include <utils/assert.h>
#include <input/mouse_input.h>

using namespace syscall;

void syscall::sys_mouse_get_buttons(interrupts::s_registers* regs) {
	assert(input::default_mouse_input_device != nullptr);

	regs->rbx = input::default_mouse_input_device->button_left;
	regs->rcx = input::default_mouse_input_device->button_right;
	regs->rdx = input::default_mouse_input_device->button_middle;
}