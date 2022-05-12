#include <syscall/syscall.h>

#include <utils/ipc.h>

using namespace syscall;

void syscall::sys_ipc_register(interrupts::s_registers* regs) {
	char* name = (char*)regs->rbx;
	ipc::ipc_callback_f callback = (ipc::ipc_callback_f) regs->rcx;

	int hid = ipc::global_ipc_manager->register_callback(name, callback);
	regs->rdx = hid;
}