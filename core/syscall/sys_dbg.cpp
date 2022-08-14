#include <syscall/syscall.h>

#include <utils/log.h>

using namespace syscall;

void syscall::sys_dbg(interrupts::s_registers* regs) {
	char* msg = (char*) regs->rbx;
	debugf("[USER] %s\n", msg);
}
