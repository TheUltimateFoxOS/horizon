#include <syscall/syscall.h>

#include <utils/log.h>

#include <apic/apic.h>
#include <scheduler/scheduler.h>

using namespace syscall;

void syscall::sys_dbg(interrupts::s_registers* regs) {
	char* msg = (char*) regs->rbx;

	if (scheduler::is_scheduler_running) {
		LAPIC_ID(core_id);
		scheduler::task_t* task = scheduler::task_queue[core_id]->list[0];

		if (task->argv[0] != nullptr) {
			debugf_raw("[USER@%s] %s\n", task->argv[0], msg);
			return;
		}
	}
	
	debugf_raw("[USER] %s\n", msg);
}
