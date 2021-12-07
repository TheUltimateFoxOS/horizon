#pragma once

#include <stivale2.h>

namespace memory {
	uint64_t get_memory_size(stivale2_struct* bootinfo);
	void prepare_memory(stivale2_struct* bootinfo);
}