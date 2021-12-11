#include <scheduler/scheduler.h>

#include <apic/apic.h>
#include <acpi/madt.h>

#include <utils/lock.h>
#include <utils/string.h>
#include <utils/log.h>

#include <interrupts/interrupt_handler.h>

#include <memory/page_frame_allocator.h>

using namespace scheduler;

namespace scheduler {
	queue<task_t*>* task_queue[128];
	bool is_scheduler_running;
}

define_spinlock(task_queue_lock);

void scheduler_interrupt(interrupts::s_registers* registers) {
	LAPIC_ID(id);

	atomic_acquire_spinlock(task_queue_lock);

	task_t* current_task;

	if (task_queue[id]->len == 0) {
		goto out;
	}

	current_task = task_queue[id]->list[0];


	if (!current_task->first_sched) {
		current_task->registers.rax = registers->rax;
		current_task->registers.rbx = registers->rbx;
		current_task->registers.rcx = registers->rcx;
		current_task->registers.rdx = registers->rdx;
		current_task->registers.r8 = registers->r8;
		current_task->registers.r9 = registers->r9;
		current_task->registers.r10 = registers->r10;
		current_task->registers.r11 = registers->r11;
		current_task->registers.r12 = registers->r12;
		current_task->registers.r13 = registers->r13;
		current_task->registers.r14 = registers->r14;
		current_task->registers.r15 = registers->r15;
		current_task->registers.rip = registers->rip;
		current_task->registers.rsp = registers->rsp;
		current_task->registers.rbp = registers->rbp;
		current_task->registers.rsi = registers->rsi;
		current_task->registers.rdi = registers->rdi;
		current_task->registers.rflags = registers->rflags;
	}

next_task:
	current_task = task_queue[id]->next();

	if (current_task->kill_me) {
		memory::global_allocator.free_pages((void*) current_task->stack, TASK_STACK_PAGES);
		delete current_task;

		task_queue[id]->remove_first();
		goto next_task;
	}

	if (current_task->lock) {
		goto next_task;
	}

	registers->rax = current_task->registers.rax;
	registers->rbx = current_task->registers.rbx;
	registers->rcx = current_task->registers.rcx;
	registers->rdx = current_task->registers.rdx;
	registers->r8 = current_task->registers.r8;
	registers->r9 = current_task->registers.r9;
	registers->r10 = current_task->registers.r10;
	registers->r11 = current_task->registers.r11;
	registers->r12 = current_task->registers.r12;
	registers->r13 = current_task->registers.r13;
	registers->r14 = current_task->registers.r14;
	registers->r15 = current_task->registers.r15;
	registers->rip = current_task->registers.rip;
	registers->rsp = current_task->registers.rsp;
	registers->rbp = current_task->registers.rbp;
	registers->rsi = current_task->registers.rsi;
	registers->rdi = current_task->registers.rdi;
	registers->rflags = current_task->registers.rflags;

	current_task->first_sched = false;

	atomic_release_spinlock(task_queue_lock);

out:
	if (id == apic::bsp_id) { // send interrupt 0x20 to all APs except the BSP (we are the BSP)
		for (int i = 0; i < acpi::madt::lapic_count; i++) {
			if (acpi::madt::lapic_ids[i] == id || !apic::cpu_started[i]) {
				continue;
			}

			apic::lapic_ipi(i, 0x20);
		}
	}
}

void scheduler::setup() {
	LAPIC_ID(id);

	for (int i = 0; i < acpi::madt::lapic_count; i++) {
		if (apic::cpu_started[i]) {
			debugf("Setting up scheduler for CPU %d\n", i);

			task_queue[i] = new queue<task_t*>(10);

			task_t* idle_task = new task_t();
			memset(idle_task, 0, sizeof(task_t));

			idle_task->stack = (uint64_t) memory::global_allocator.request_pages(TASK_STACK_PAGES);
			idle_task->registers.rip = (uint64_t) (void (*)()) []() { while(1) { __asm__ __volatile__ ("sti; hlt"); } };
			idle_task->registers.rsp = idle_task->stack + (TASK_STACK_PAGES * 4096) - sizeof(uint64_t);
			idle_task->first_sched = true;

			task_queue[i]->add(idle_task);
		}
	}

	debugf("Scheduler setup complete\n");
}

void scheduler::start() {
	debugf("Starting scheduler\n");

	interrupts::register_interrupt_handler(0x20, scheduler_interrupt);

	is_scheduler_running = true;

	while(true) {
		__asm__ __volatile__ ("sti; hlt");
	}
}

void scheduler::kill_self() {
	LAPIC_ID(id);

	atomic_acquire_spinlock(task_queue_lock);

	task_queue[id]->list[0]->kill_me = true;

	atomic_release_spinlock(task_queue_lock);

	while (true) {
		__asm__ __volatile__ ("sti; hlt");
	}
}

task_t* scheduler::create_task(void* entry) {
	task_t* task = new task_t();
	memset(task, 0, sizeof(task_t));

	task->stack = (uint64_t) memory::global_allocator.request_pages(TASK_STACK_PAGES);
	task->registers.rip = (uint64_t) task_entry;
	task->registers.rsp = task->stack + (TASK_STACK_PAGES * 4096) - sizeof(uint64_t);
	task->first_sched = true;
	task->registers.rax = (uint64_t) entry;

	atomic_acquire_spinlock(task_queue_lock);

	uint64_t min = 0xf0f0;
	uint64_t min_id = 0;

	for (int i = 0; i < acpi::madt::lapic_count; i++) {
		if (apic::cpu_started[i]) {
			if (task_queue[i]->len < min) {
				min = task_queue[i]->len;
				min_id = i;
			}
		}
	}

	task_queue[min_id]->add(task);

	atomic_release_spinlock(task_queue_lock);

	return task;
}