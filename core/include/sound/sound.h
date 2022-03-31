#pragma once

#include <stdint.h>
#include <utils/queue.h>
#include <utils/lock.h>

namespace sound {
	#define ENCODE_NOTE(octave, note) (uint8_t) ((octave << 4) | note)
	#define DECODE_NOTE_OCTAVE(note) (note >> 4)
	#define DECODE_NOTE_NOTE(note) (note & 0x0F)

	#define OCTAVE_1 0
	#define OCTAVE_2 1
	#define OCTAVE_3 2
	#define OCTAVE_4 3
	#define OCTAVE_5 4
	#define OCTAVE_6 5
	#define OCTAVE_7 6

	#define NOTE_C 0
	#define NOTE_CS 1
	#define NOTE_DF NOTE_CS
	#define NOTE_D 2
	#define NOTE_DS 3
	#define NOTE_EF NOTE_DS
	#define NOTE_E 4
	#define NOTE_F 5
	#define NOTE_FS 6
	#define NOTE_GF NOTE_FS
	#define NOTE_G 7
	#define NOTE_GS 8
	#define NOTE_AF NOTE_GS
	#define NOTE_A 9
	#define NOTE_AS 10
	#define NOTE_BF NOTE_AS
	#define NOTE_B 11

	struct queued_note_t {
		uint8_t note;
		uint8_t channel;
		uint16_t duration_ms;
	};

	class sound_device {
		public:
			queue<queued_note_t> notes;
			uint64_t notes_lock;

			sound_device();

			void push_note(uint8_t note, uint8_t channel, uint16_t duration_ms);
			void sync();

			virtual int get_channel_count();
	};

	extern sound_device* default_sound_device;
}