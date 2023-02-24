#pragma once

#include <scheduler/scheduler.h>

namespace fexec {
	scheduler::task_t* load_fexec(void* ptr, const char** argv, const char** envp);
	scheduler::task_t* load_fexec(char* filename, const char** argv, const char** envp);
}