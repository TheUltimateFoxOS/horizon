#pragma once

#include <stdarg.h>

#define ZEROPAD	1
#define SIGN	2
#define PLUS	4
#define SPACE	8
#define LEFT	16
#define SMALL	32
#define SPECIAL	64

#define __do_div(n, base) ({					\
int __res;										\
__res = ((unsigned long) n) % (unsigned) base;	\
n = ((unsigned long) n) / (unsigned) base;		\
__res; })

#define isdigit(c) (c >= '0' && c <= '9')

int skip_atoi(const char **s);
char *number_printf(char *str, long num, int base, int size, int precision, int type);
int vsprintf(char *buf, const char *fmt, va_list args);