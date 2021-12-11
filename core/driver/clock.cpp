#include <driver/clock.h>

#include <utils/log.h>

using namespace driver;

namespace driver {
	clock_device* default_clock_device = nullptr;
}

clock_device::clock_result_t clock_device::time() {
	debugf("clock_device::time not implemented");
	return {};
}