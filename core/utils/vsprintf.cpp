#include <utils/vsprintf.h>

#include <utils/string.h>
#include <stdarg.h>

int print_num(unsigned long x, int base, char* str) {
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

	memcpy(str, p, strlen(p));

	return strlen(p);
}

int vsprintf(char *buf, const char *str, va_list args) {
	const char* s;
	unsigned long n;

	while(*str) {
		if(*str == '%') {
			str++;
			switch(*str) {
				case 's':
					s = va_arg(args, char*);
					while (*s) {
						*buf++ = *s++;
					}
					break;
				case 'c':
					n = va_arg(args, int);
					*buf++ = n;
					break;
				case 'd':
				case 'u':
					n = va_arg(args, unsigned long int);
					buf += print_num(n, 10, buf);
					break;
				case 'x':
				case 'p':
					n = va_arg(args, unsigned long int);
					buf += print_num(n, 16, buf);
					break;
				case '%':
					*buf++ = '%';
					break;
				case '\0':
					goto out;
					break;
				default:
					*buf++ = '%';
					*buf++ = *str;
					break;
			}
		} else {
			*buf++ = *str;
		}
		str++;
	}

out:
	return strlen(buf);
}
