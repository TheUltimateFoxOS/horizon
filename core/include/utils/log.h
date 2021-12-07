#pragma once

#include <output/output.h>

namespace log {
	extern output::output_device* debug_device;
	extern output::output_device* stdout_device;
}

void debugf(const char* fmt, ...);
void printf(const char *fmt, ...);