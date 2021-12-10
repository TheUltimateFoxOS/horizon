#pragma once

#include <stdint.h>

namespace timer {
	class timer_device {
		public:
			virtual void sleep(uint32_t ms);
	};

	extern timer_device* global_timer;

	void setup();
}