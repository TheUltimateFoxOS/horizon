#include <syscall/syscall.h>

#include <utils/assert.h>
#include <input/mouse_input.h>

using namespace syscall;

void syscall::sys_mouse_reset(interrupts::s_registers* regs) {
	assert(input::default_mouse_input_device != nullptr);
	input::default_mouse_input_device->reset();
}