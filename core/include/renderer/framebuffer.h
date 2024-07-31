#pragma once

#include <stdint.h>
#include <stddef.h>

namespace renderer {

	struct framebuffer_t {
		void* base_address;
		size_t buffer_size;
		uint32_t width;
		uint32_t height;
		uint32_t pitch;
	};
}