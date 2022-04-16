#pragma once

#include <stivale2.h>

namespace memory {
	#define KERNEL_HIGH_VMA 0xFFFFFFFF80000000
	#define HIGH_VMA        0xFFFF800000000000

	uint64_t get_memory_size(stivale2_struct* bootinfo);
	void prepare_memory(stivale2_struct* bootinfo);
}