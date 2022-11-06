#include <syscall/syscall.h>

#include <timer/timer.h>

using namespace syscall;

void syscall::sys_clock(interrupts::s_registers* regs) {
	regs->rbx = timer::global_timer->get_ticks();
}

void syscall::sys_clock_ticks_per_second(interrupts::s_registers* regs) {
	regs->rbx = timer::global_timer->get_ticks_per_second();
}
