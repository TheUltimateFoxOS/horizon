#include <syscall/syscall.h>
#include <driver/clock.h>

#include <utils/assert.h>
#include <utils/unix_time.h>

using namespace syscall;

void syscall::sys_time(interrupts::s_registers* regs) {
	assert(driver::default_clock_device != nullptr);
	driver::clock_device::clock_result_t result = driver::default_clock_device->time();

	regs->rbx = to_unix_time(result.year, result.month, result.day, result.hours, result.minutes, result.seconds);
}
