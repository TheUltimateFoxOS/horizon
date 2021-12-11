#include <syscall/syscall.h>

#include <scheduler/scheduler.h>

using namespace syscall;

void syscall::sys_exit(interrupts::s_registers* regs) {
	scheduler::kill_self();
}
