#include <timer/pit.h>

using namespace timer;

namespace timer {
	pit_timer* global_pit_timer;
}

pit_timer::pit_timer() : init(0x43), channel_0(0x40), interrupt_handler(0x20) {
	set_divisor(divisor);
}

void pit_timer::set_divisor(uint16_t new_divisor) {
	divisor = new_divisor;

	init.Write(0x36);
	io_wait();
	channel_0.Write((uint8_t)(divisor & 0x00ff));
	io_wait();
	channel_0.Write((uint8_t)((divisor & 0xff00) >> 8));
}

void pit_timer::handle() {
	ticks_since_boot++;
}

void pit_timer::sleep(uint32_t ms) {
	uint64_t start = get_ticks_ms();
	uint64_t end = start + ms;
	while(get_ticks_ms() < end) {
		__asm__ __volatile__ ("hlt");
	}
}

uint64_t pit_timer::get_ticks_per_second() {
	return (1193180 / divisor);
}

uint64_t pit_timer::get_ticks() {
	return ticks_since_boot;
}

uint64_t pit_timer::get_ticks_ms() {
	return (ticks_since_boot * 1000) / (1193180 / divisor);
}
