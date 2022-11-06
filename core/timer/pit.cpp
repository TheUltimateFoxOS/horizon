#include <timer/pit.h>

using namespace timer;

namespace timer {
	pit_timer* global_pit_timer;
}

pit_timer::pit_timer() : channel_0(0x40), interrupt_handler(0x20) {
	set_divisor(divisor);
}

void pit_timer::set_divisor(uint16_t this_divisor) {
	if (divisor < 100) {
		divisor = 100;
	}
	divisor = this_divisor;

	channel_0.Write((uint8_t)(divisor & 0x00ff));
	io_wait();
	channel_0.Write((uint8_t)(divisor & 0xff00) >> 8);
}

void pit_timer::handle() {
	ticks_since_boot++;
}

void pit_timer::sleep(uint32_t ms) {
	int frequency = 1193180 / divisor;
	int sleep_time = (ms / 10) * frequency;

	int current_ticks = ticks_since_boot;
	while (ticks_since_boot - current_ticks < sleep_time) {
		__asm__ __volatile__("hlt");
	}
}

uint64_t pit_timer::get_ticks_per_second() {
	return 100 * (1193180 / divisor);
}

uint64_t pit_timer::get_ticks() {
	return ticks_since_boot;
}