#pragma once

#include <stivale2.h>
#include <renderer/framebuffer.h>

namespace renderer {
	void setup(stivale2_struct* bootinfo);

	extern framebuffer_t default_framebuffer;
}