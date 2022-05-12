#include <syscall/syscall.h>

#include <utils/ipc.h>

using namespace syscall;

void syscall::sys_ipc_send_message(interrupts::s_registers* regs) {
	int hid = regs->rbx;
	int func = regs->rcx;
	void* data = (void*) regs->rdx;

	ipc::global_ipc_manager->send_message(hid, func, data);
}