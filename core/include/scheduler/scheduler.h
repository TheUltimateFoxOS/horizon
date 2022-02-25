#pragma once

#include <utils/queue.h>
#include <stdint.h>
#include <interrupts/interrupts.h>

namespace scheduler {
	#define TASK_STACK_PAGES 4

	typedef void (*signal_handler)(uint8_t signum);
	typedef void (*pipe)(char* buffer, uint64_t size);

	struct task_t {
		interrupts::s_registers registers;
		char fxsr_state[512] __attribute__((aligned(16)));
		uint64_t stack;

		bool first_sched; // is true if the task just got created
		bool kill_me; // if true task gets killed
		bool lock; // if true task is locked and doesent get scheduled

		bool is_elf; // if true task is started from an elf
		char** argv;
		char** envp;

		void* offset; // offset of the elf
		int page_count; // number of pages the elf needs

		bool* on_exit; // if it isn't a nullptr gets set to true when the task exits
		int* exit_code; // if it isn't a nullptr gets set to the exit code of the task

		pipe stdin_pipe;
		pipe stdout_pipe;
		pipe stderr_pipe;
		bool pipe_enabled;

		signal_handler signals[32];

		char cwd[128];
	};

	void setup();
	void start();

	task_t* create_task(void* entry);

	void kill_self();

	extern queue<task_t*>* task_queue[128];
	extern bool is_scheduler_running;

	bool handle_signal(int signum);
	void register_signal_handler_self(int signum, uint64_t handler);

	void set_cwd_self(const char* cwd);
	const char* get_cwd_self();

	extern "C" void task_entry();
}