#include <pc_speaker.h>

#include <utils/log.h>
#include <utils/port.h>
#include <scheduler/scheduler.h>
#include <timer/timer.h>

uint16_t notes_to_freq_table[7][12] = {
	{ 36485, 34437, 32505, 30680, 28958, 27333, 25799, 24351, 22984, 21694, 20477, 19327 },
	{ 18243, 17219, 16252, 15340, 14479, 13666, 12899, 12175, 11492, 10847, 10238, 9664 },
	{ 9121, 8609, 8126, 7670, 7240, 6833, 6450, 6088, 5746, 5424, 5119, 4832 },
	{ 4561, 4305, 4063, 3835, 3620, 3417, 3225, 3044, 2873, 2712, 2560, 2416 },
	{ 2280, 2152, 2032, 1918, 1810, 1708, 1612, 1522, 1437, 1356, 1280, 1208 },
	{ 1140, 1076, 1016, 959, 905, 854, 806, 761, 718, 678, 640, 604},
	{ 570, 538, 508, 479, 452, 427, 403, 380, 359, 339, 320, 302 }
};

char* play_task_argv[] = {
	(char*) "pc_speaker_queue",
	nullptr
};

using namespace sound;

pc_speaker_driver::pc_speaker_driver() {
}

int pc_speaker_driver::get_channel_count() {
	return 1;
}


void pc_speaker_driver::activate() {
	debugf("Initializing pc speaker driver...\n");

	if (!default_sound_device) {
		default_sound_device = this;
	} else {
		if (get_channel_count() > default_sound_device->get_channel_count()) {
			debugf("default sound device has less channels than this device, using this sound device\n");
			default_sound_device = this;
		} else {
			debugf("default sound device has more channels than this device, using default sound device\n");
		}
	}

	scheduler::task_t* task = scheduler::create_task((void*) (void (*)()) []() {
		register uint64_t driver_reg asm("rbx");
		pc_speaker_driver* driver = (pc_speaker_driver*) driver_reg;

		debugf("pc_speaker_queue task started\n");

		while (true) {
			if (driver->notes.len != 0) {
				atomic_acquire_spinlock(driver->notes_lock);
				queued_note_t note = driver->notes.list[0];
				driver->notes.remove_first();
				int new_len = driver->notes.len;
				atomic_release_spinlock(driver->notes_lock);

				//debugf("Playing note %d on channel %d for %d ms\n", note.note, note.channel, note.duration_ms);

				if (note.note == 0) {
					driver->turn_off();
					driver->set_frequency(0);
				} else {
					driver->turn_on();
					driver->set_frequency(driver->note_to_frequency(note.note));
				}

				timer::global_timer->sleep(note.duration_ms);

				if (new_len == 0) {
					driver->turn_off();
				}
			}
		}
	});

	task->argv = play_task_argv;
	task->registers.rbx = (uint64_t) this;

#if 0
	for (int i = 0; i < 7; i++) {
		for (int j = 0; j < 12; j++) {
			notes.add({
				.note = ENCODE_NOTE(i, j),
				.channel = 0,
				.duration_ms = 50
			});
		}
	}
#endif
}

bool pc_speaker_driver::is_presend() {
	return true;
}

char* pc_speaker_driver::get_name() {
	return (char*) "pc_speaker";
}

void pc_speaker_driver::turn_off() {
	outb(0x61, (inb(0x61) & 0xFC));
}

void pc_speaker_driver::turn_on() {
	outb(0x61, (inb(0x61) | 3));
}

void pc_speaker_driver::set_frequency(uint16_t frequency) {
	outb(0x43, 0xB6);
	outb(0x42, (uint8_t) frequency);
	outb(0x42, (uint8_t) (frequency >> 8));
}

uint16_t pc_speaker_driver::note_to_frequency(uint8_t note) {
	uint8_t octave = DECODE_NOTE_OCTAVE(note);
	uint8_t note_number = DECODE_NOTE_NOTE(note);
	return notes_to_freq_table[octave][note_number];
}