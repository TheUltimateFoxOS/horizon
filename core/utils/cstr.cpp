#include <utils/cstr.h>
#include <utils/string.h>

char tmp_string[128];
const char* num_to_string(uint64_t x, int base) {
	const char* digits = "0123456789abcdefghijklmnopqrstuvwxyz";

	char buf[65];

	char* p;

	if(base > 36) {
		return 0;
	}

	p = buf + 64;
	*p = '\0';

	do {
		*--p = digits[x % base];
		x /= base;
	} while(x);

	memcpy(tmp_string, p, strlen(p));

	return tmp_string;
}

const char* num_to_string(uint64_t x) {
	return num_to_string(x, 10);
}