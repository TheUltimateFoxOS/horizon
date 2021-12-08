#pragma once

#include <stdint.h>

namespace elf {
	struct stack_frame_t {
		struct stack_frame_t*	rbp;
		uint64_t				rip;
	} __attribute__((packed));

	void unwind(int max, uint64_t rbp, void (*callback)(int frame_num, uint64_t rip));
}