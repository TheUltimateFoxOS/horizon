#include <utils/string.h>

#include <utils/vsprintf.h>

#include <output/stivale2_terminal.h>

char* strcpy(char* dest, const char* src) {
	do {
		*dest++ = *src++;
	} while (*src != 0);
	return 0;
}

int strlen(char* src) {
	int i = 0;
	while (*src++)
		i++;
	return i;
}

int strnlen(const char *s, int maxlen) {
	int i;
	for (i = 0; i < maxlen; ++i)
	if (s[i] == '\0')
		break;
	return i;
}

int sprintf(char *buf, const char *fmt, ...) {
	va_list args;
	int i;

	va_start(args, fmt);
	i = vsprintf(buf, fmt, args);
	va_end(args);
	return i;
}

void debugf(const char *fmt, ...) {
	va_list args;
	char buf[1024];

	va_start(args, fmt);
	vsprintf(buf, fmt, args);
	va_end(args);

	output::global_terminal->putstring(buf);
}
void printf(const char *fmt, ...) {
	va_list args;
	char buf[1024];

	va_start(args, fmt);
	vsprintf(buf, fmt, args);
	va_end(args);

	output::global_terminal->putstring(buf);
}