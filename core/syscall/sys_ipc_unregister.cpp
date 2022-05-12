#include <syscall/syscall.h>

#include <utils/ipc.h>

using namespace syscall;

void syscall::sys_ipc_unregister(interrupts::s_registers* regs) {
	int hid = regs->rbx;

	ipc::global_ipc_manager->unregister_callback(hid);
}