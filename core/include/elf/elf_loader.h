#pragma once

#include <scheduler/scheduler.h>

namespace elf {
	scheduler::task_t* load_elf(void* ptr, const char** argv, const char** envp);
	scheduler::task_t* load_elf(char* filename, const char** argv, const char** envp);
}