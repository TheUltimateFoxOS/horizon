#include <syscall/syscall.h>
#include <sound/sound.h>

using namespace syscall;

void syscall::sys_sound_push_note(interrupts::s_registers* regs) {
	uint8_t note = regs->rbx;
	uint8_t channel = regs->rcx;
	uint16_t duration_ms = regs->rdx;

	sound::default_sound_device->push_note(note, channel, duration_ms);
}