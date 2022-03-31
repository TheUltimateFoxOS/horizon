#include <syscall/syscall.h>
#include <sound/sound.h>

using namespace syscall;

void syscall::sys_sound_get_channel_count(interrupts::s_registers* regs) {
	regs->rbx = sound::default_sound_device->get_channel_count();
}