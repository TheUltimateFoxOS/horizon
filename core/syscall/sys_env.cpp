#include <syscall/syscall.h>

#include <scheduler/scheduler.h>
#include <apic/apic.h>

using namespace syscall;

void syscall::sys_env(interrupts::s_registers* regs) {
	LAPIC_ID(core_id);

	scheduler::task_t* task = scheduler::task_queue[core_id]->list[0];

	switch(regs->rbx) {
		case 0:
			{
				regs->rcx = (uint64_t) task->argv;
			}
			break;
		case 1:
			{
				regs->rcx = (uint64_t) task->envp;
			}
			break;
		
		case 2:
			{
				// errno
				debugf("sys_env: errno not implemented\n");
			}
			break;

		case 3:
			{
				scheduler::register_signal_handler_self(regs->rcx, regs->rdx);
			}
			break;
	}
}