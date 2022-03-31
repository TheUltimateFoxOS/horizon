#pragma once

#include <sound/sound.h>
#include <driver/driver.h>

namespace sound {
	class pc_speaker_driver : public driver::device_driver, public sound::sound_device {
		public:
			pc_speaker_driver();

			int get_channel_count();

			virtual void activate();
			virtual bool is_presend();
			virtual char* get_name();

			void turn_off();
			void turn_on();
			void set_frequency(uint16_t frequency);

			uint16_t note_to_frequency(uint8_t note);
	};
}