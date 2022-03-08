#pragma once

#include <output/output.h>

#include <config.h>

namespace log {
	extern output::output_device* debug_device;
	extern output::output_device* stdout_device;
}

void debugf_intrnl(const char* fmt, ...);

#ifdef DEBUG
#define debugf(fmt, ...) debugf_intrnl("[%s:%d] ", __FILE__, __LINE__); debugf_intrnl(fmt, ##__VA_ARGS__)
#define debugf_raw(fmt, ...) debugf_intrnl(fmt, ##__VA_ARGS__)
#else
#define debugf(fmt, ...)
#define debugf_raw(fmt, ...)
#endif

void printf(const char *fmt, ...);