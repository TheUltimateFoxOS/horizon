#include <syscall/syscall.h>

#include <timer/timer.h>

using namespace syscall;

void syscall::sys_delay(interrupts::s_registers* regs) {
	int ms = regs->rbx;
	timer::global_timer->sleep(ms);
}
