#include <utils/log.h>
#include <utils/vsprintf.h>

#include <output/serial_port.h>
#include <output/stivale2_terminal.h>

#include <stdarg.h>

void debugf(const char *fmt, ...) {
	va_list args;
	char buf[1024];

	va_start(args, fmt);
	vsprintf(buf, fmt, args);
	va_end(args);

	output::global_serial_port->putstring(buf);
}

void printf(const char *fmt, ...) {
	va_list args;
	char buf[1024];

	va_start(args, fmt);
	vsprintf(buf, fmt, args);
	va_end(args);

	output::global_terminal->putstring(buf);
}