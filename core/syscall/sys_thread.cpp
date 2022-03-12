#include <syscall/syscall.h>

#include <utils/log.h>
#include <scheduler/scheduler.h>
#include <apic/apic.h>

void syscall::sys_thread(interrupts::s_registers* regs) {
	LAPIC_ID(core_id);
	scheduler::task_t* me = scheduler::task_queue[core_id]->list[0];

	void* addr = (void*) regs->rbx;

	scheduler::task_t* task = scheduler::create_task(addr);

	if (task == nullptr) {
		debugf("Failed to start task");
	} else {
		task->argv = me->argv;
		task->envp = me->envp;
		memcpy(task->signals, me->signals, sizeof(task->signals));
		task->system_method = me->system_method;
		
		if ((bool) regs->rcx) {
			// clone cwd
			char* self_cwd = (char*) scheduler::get_cwd_self();
			memset(task->cwd, 0, sizeof(task->cwd));
			strcpy(task->cwd, self_cwd);
		}
	}

	regs->rax = (uint64_t) task;
}