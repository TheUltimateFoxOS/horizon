#include <syscall/syscall.h>

#include <utils/log.h>

using namespace syscall;

void syscall::sys_dbg(interrupts::s_registers* regs) {
	char* msg = (char*) regs->rbx;
	debugf_raw("[USER] %s\n", msg);
}
