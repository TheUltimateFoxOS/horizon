#pragma once

#include <stdint.h>

namespace timer {
	class timer_device {
		public:
			virtual void sleep(uint32_t ms);

			virtual uint64_t get_ticks_per_second();

			virtual uint64_t get_ticks();
	};

	extern timer_device* global_timer;

	void setup();
}