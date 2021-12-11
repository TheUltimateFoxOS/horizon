#include <elf/kernel_module.h>

#include <cmos.h>
#include <driver/clock.h>
#include <utils/log.h>

void device_init() {
	debugf("Creating CMOS driver and setting default_clock_device...\n");
	cmos::cmos_driver* cmos_driver = new cmos::cmos_driver();
	driver::default_clock_device = cmos_driver;


	// test code
	// driver::clock_device::clock_result_t result = driver::default_clock_device->time();
	// debugf("Time: %d:%d:%d\n", result.hours, result.minutes, result.seconds);
	// debugf("Date: %d/%d/%d\n", result.day, result.month, result.year);
}

define_module("cmos", null_ptr_func, device_init, null_ptr_func);