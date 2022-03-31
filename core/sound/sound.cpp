#include <sound/sound.h>

#include <utils/log.h>

namespace sound {
	sound_device* default_sound_device = nullptr;
}

using namespace sound;

sound_device::sound_device() : notes(100) {
	notes_lock = 0;
}

void sound_device::push_note(uint8_t note, uint8_t channel, uint16_t duration_ms) {
	atomic_acquire_spinlock(notes_lock);
	notes.add({
		.note = note,
		.channel = channel,
		.duration_ms = duration_ms
	});
	atomic_release_spinlock(notes_lock);
}

void sound_device::sync() {
	debugf("Waiting for audio sync...\n");
	while (notes.len != 0) {
		__asm__ __volatile__("pause" ::: "memory");
	}
}

int sound_device::get_channel_count() {
	debugf("sound_device::get_channel_count() not implemented");
	return 0;
}