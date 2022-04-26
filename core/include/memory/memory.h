#pragma once

#include <stdint.h>

namespace memory {
	#define KERNEL_HIGH_VMA 0xFFFFFFFF80000000
	#define HIGH_VMA        0xFFFF800000000000

	uint64_t get_memory_size();
	void prepare_memory();

	void* map_if_necessary(void* virtual_address);
}