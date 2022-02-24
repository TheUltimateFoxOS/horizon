#include <syscall/syscall.h>

#include <scheduler/scheduler.h>
#include <apic/apic.h>

using namespace syscall;

void syscall::sys_exit(interrupts::s_registers* regs) {
	LAPIC_ID(core_id);
	scheduler::task_t* task = scheduler::task_queue[core_id]->list[0];

	if (task->exit_code) {
		*task->exit_code = regs->rbx;
	}

	scheduler::kill_self();
}
