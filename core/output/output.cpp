#include <output/output.h>

#include <utils/log.h>

using namespace output;

void output_device::putchar(char c) {
	debugf("output_device::putchar() not implemented");
}

void output_device::putstring(const char* str) {
	while (*str) {
		putchar(*str++);
	}
}