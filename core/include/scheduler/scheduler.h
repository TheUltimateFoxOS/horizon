#pragma once

#include <utils/queue.h>
#include <stdint.h>
#include <interrupts/interrupts.h>

namespace scheduler {
	#define TASK_STACK_PAGES 4

	struct task_t {
		interrupts::s_registers registers;
		char fxsr_state[512] __attribute__((aligned(16)));
		uint64_t stack;

		bool first_sched; // is true if the task just got created
		bool kill_me; // if true task gets killed
		bool lock; // if true task is locked and doesent get scheduled
	};

	void setup();
	void start();

	task_t* create_task(void* entry);

	void kill_self();

	extern queue<task_t*>* task_queue[128];
	extern bool is_scheduler_running;

	extern "C" void task_entry();
}