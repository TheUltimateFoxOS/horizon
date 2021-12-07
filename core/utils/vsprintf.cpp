#include <utils/vsprintf.h>

#include <utils/string.h>

int skip_atoi(const char **s) {
	int i = 0;

	while (isdigit(**s))
		i = i * 10 + *((*s)++) - '0';
	return i;
}

char *number_printf(char *str, long num, int base, int size, int precision, int type) {
	static const char digits[17] = "0123456789ABCDEF";

	char tmp[66];
	char c, sign, locase;
	int i;

	locase = (type & SMALL);
	if (type & LEFT) {
		type &= ~ZEROPAD;
	}
	if (base < 2 || base > 16) {
		return 0;
	}
	c = (type & ZEROPAD) ? '0' : ' ';

	sign = 0;

	if (type & SIGN) {
		if (num < 0) {
			sign = '-';
			num = -num;
			size--;
		} else if (type & PLUS) {
			sign = '+';
			size--;
		} else if (type & SPACE) {
			sign = ' ';
			size--;
		}
	}
	if (type & SPECIAL) {
		if (base == 16) {
			size -= 2;
		}
		else if (base == 8) {
			size--;
		}
	}
	i = 0;
	if (num == 0) {
		tmp[i++] = '0';
	}
	else {
		while (num != 0) {
			tmp[i++] = (digits[__do_div(num, base)] | locase);
		}
	}
	if (i > precision) {
		precision = i;
	}
	size -= precision;
	if (!(type & (ZEROPAD + LEFT))) {
		while (size-- > 0) {
			*str++ = ' ';
		}
	}
	if (sign) {
		*str++ = sign;
	}
	if (type & SPECIAL) {
		if (base == 8) {
			*str++ = '0';
		} else if (base == 16) {
			*str++ = '0';
			*str++ = ('X' | locase);
		}
	}
	if (!(type & LEFT)) {
		while (size-- > 0) {
			*str++ = c;
		}
	}
	while (i < precision--) {
		*str++ = '0';
	}
	while (i-- > 0) {
		*str++ = tmp[i];
	}
	while (size-- > 0) {
		*str++ = ' ';
	}
	return str;
}

int vsprintf(char *buf, const char *fmt, va_list args) {
	int len;
	unsigned long num;
	int i, base;
	char *str;
	const char *s;

	int flags;

	int field_width;
	int precision;
	int qualifier;

	for (str = buf; *fmt; ++fmt) {
		if (*fmt != '%') {
			*str++ = *fmt;
			continue;
		}
		flags = 0;
	repeat:
		++fmt;
		switch (*fmt) {
			case '-':
				flags |= LEFT;
				goto repeat;
			case '+':
				flags |= PLUS;
				goto repeat;
			case ' ':
				flags |= SPACE;
				goto repeat;
			case '#':
				flags |= SPECIAL;
				goto repeat;
			case '0':
				flags |= ZEROPAD;
				goto repeat;
		}

		field_width = -1;
		if (isdigit(*fmt)) {
			field_width = skip_atoi(&fmt);
		} else if (*fmt == '*') {
			++fmt;

			field_width = va_arg(args, int);
			if (field_width < 0) {
				field_width = -field_width;
				flags |= LEFT;
			}
		}

		precision = -1;
		if (*fmt == '.') {
			++fmt;
			if (isdigit(*fmt)) {
				precision = skip_atoi(&fmt);
			} else if (*fmt == '*') {
				++fmt;
				precision = va_arg(args, int);
			}
			if (precision < 0) {
				precision = 0;
			}
		}

		qualifier = -1;
		if (*fmt == 'h' || *fmt == 'l' || *fmt == 'L') {
			qualifier = *fmt;
			++fmt;
		}

		base = 10;

		switch (*fmt) {
			case 'c':
				if (!(flags & LEFT)) {
					while (--field_width > 0) {
						*str++ = ' ';
					}
				}
				*str++ = (unsigned char) va_arg(args, int);
				while (--field_width > 0) {
					*str++ = ' ';
				}
				continue;

			case 's':
				s = va_arg(args, char *);
				len = strnlen(s, precision);

				if (!(flags & LEFT)) {
					while (len < field_width--) {
						*str++ = ' ';
					}
				}
				for (i = 0; i < len; ++i) {
					*str++ = *s++;
				}
				while (len < field_width--) {
					*str++ = ' ';
				}
				continue;

			case 'p':
				if (field_width == -1) {
					field_width = 2 * sizeof(void *);
					flags |= ZEROPAD;
				}
				str = number_printf(str, (unsigned long) va_arg(args, void *), 16, field_width, precision, flags);
				continue;

			case 'n':
				if (qualifier == 'l') {
					long *ip = va_arg(args, long *);
					*ip = (str - buf);
				} else {
					int *ip = va_arg(args, int *);
					*ip = (str - buf);
				}
				continue;

			case '%':
				*str++ = '%';
				continue;

			case 'o':
				base = 8;
				break;

			case 'x':
				flags |= SMALL;
			case 'X':
				base = 16;
				break;

			case 'd':
			case 'i':
				flags |= SIGN;
			case 'u':
				break;

			default:
				*str++ = '%';
				if (*fmt) {
					*str++ = *fmt;
				} else {
					--fmt;
				}
				continue;
		}
		if (qualifier == 'l') {
			num = va_arg(args, unsigned long);
		}
		else if (qualifier == 'h') {
			num = (unsigned short)va_arg(args, int);
			if (flags & SIGN) {
				num = (short) num;
			}
		} else if (flags & SIGN) {
			num = va_arg(args, int);
		} else {
			num = va_arg(args, unsigned int);
		}
		str = number_printf(str, num, base, field_width, precision, flags);
	}
	*str = '\0';
	return str - buf;
}
