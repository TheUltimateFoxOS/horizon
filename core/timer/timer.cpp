#include <timer/timer.h>

#include <timer/hpet.h>
#include <timer/pit.h>

#include <utils/log.h>

using namespace timer;

namespace timer {
	timer_device* global_timer = nullptr;
}

void timer_device::sleep(uint32_t ms) {
	debugf("timer_device::sleep not implemented");
}

uint64_t timer_device::get_ticks_per_second() {
	debugf("timer_device::get_ticks_per_second not implemented");
	return 0;
}

uint64_t timer_device::get_ticks() {
	debugf("timer_device::get_ticks not implemented");
	return 0;
}

void timer::setup() {
	debugf("Setting up timers...\n");

	if (hpet_timer::is_available()) {
		debugf("Detected HPET timer\n");
		global_hpet_timer = new hpet_timer();
	}

	debugf("Setting up PIT timer\n");
	global_pit_timer = new pit_timer();
	global_timer = global_pit_timer;

	if (global_hpet_timer != nullptr) {
		global_timer = global_hpet_timer;
	}
}