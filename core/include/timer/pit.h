#include <timer/timer.h>

#include <interrupts/interrupt_handler.h>
#include <utils/port.h>

namespace timer {
	class pit_timer : public timer_device, public interrupts::interrupt_handler {
		public:
			pit_timer();

			virtual void sleep(uint32_t ms);
			virtual uint64_t get_ticks_per_second();
			virtual uint64_t get_ticks();
			uint64_t get_ticks_ms();

			uint16_t divisor = 65535;
			uint64_t ticks_since_boot = 0;

			virtual void handle();

			Port8Bit channel_0;
			Port8Bit init;

			void set_divisor(uint16_t this_divisor);
	};

	extern pit_timer* global_pit_timer;
}