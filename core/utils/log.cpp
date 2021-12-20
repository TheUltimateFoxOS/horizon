#include <utils/log.h>
#include <utils/vsprintf.h>

#include <output/serial_port.h>
#include <output/stivale2_terminal.h>

#include <stdarg.h>

namespace log {
	output::output_device* debug_device;
	output::output_device* stdout_device;
}

void debugf_intrnl(const char *fmt, ...) {
	va_list args;
	char buf[1024] = {0};

	va_start(args, fmt);
	vsprintf(buf, fmt, args);
	va_end(args);

	log::debug_device->putstring(buf);
}

void printf(const char *fmt, ...) {
	va_list args;
	char buf[1024] = {0};

	va_start(args, fmt);
	vsprintf(buf, fmt, args);
	va_end(args);

	log::stdout_device->putstring(buf);
}