#include <syscall/syscall.h>

#include <utils/ipc.h>

using namespace syscall;

void syscall::sys_ipc_get_hid(interrupts::s_registers* regs) {
	char* name = (char*)regs->rbx;
	
	int hid = ipc::global_ipc_manager->get_hid(name);

	regs->rdx = hid;
}