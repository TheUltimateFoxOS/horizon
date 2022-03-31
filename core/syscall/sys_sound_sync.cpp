#include <syscall/syscall.h>
#include <sound/sound.h>

using namespace syscall;

void syscall::sys_sound_sync(interrupts::s_registers* regs) {
	sound::default_sound_device->sync();
}