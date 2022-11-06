#pragma once

#include <stdint.h>
#include <timer/timer.h>

namespace timer {
	struct hpet_t {
		uint64_t capabilities;
		uint64_t unused0;
		uint64_t general_config;
		uint64_t unused1;
		uint64_t int_status;
		uint64_t unused2;
		uint64_t unused3[24];
		uint64_t counter_value;
		uint64_t unused4;
	} __attribute__((packed));

	class hpet_timer : public timer_device {
		public:
			hpet_timer();
			
			hpet_t* hpet;

			virtual void sleep(uint32_t ms);
			virtual uint64_t get_ticks_per_second();
			virtual uint64_t get_ticks();
			
			static bool is_available();
	};

	extern hpet_timer* global_hpet_timer;
}