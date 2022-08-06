#include <stddef.h>

#include <utils/string.h>
#include <utils/vsprintf.h>

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

char* strchr(const char* str, int chr) {
	if(str == NULL) {
		return NULL;
	}
	while(*str) {
		if(*str == (char) chr) {
			return (char*) str;
		}
		str++;
	}
	return NULL;
}

char* strcat(char* dest, const char* src) {
	char* d = dest;
	while(*d) {
		d++;
	}
	while(*src) {
		*d++ = *src++;
	}
	*d = 0;
	return dest;
}

int sprintf(char *buf, const char *fmt, ...) {
	va_list args;
	int i;

	va_start(args, fmt);
	i = vsprintf(buf, fmt, args);
	va_end(args);
	return i;
}

__attribute__((naked))
void* memcpy(void* dest, const void* src, int n) {
	__asm__ __volatile__ (	"movq %rdi, %rax;"
							"movq %rdx, %rcx;"
							"shrq $3, %rcx;"
							"andl $7, %edx;"
							"rep movsq;"
							"movl %edx, %ecx;"
							"rep movsb;"
							"ret;");
}

__attribute__((naked))
void* memset(void* start, uint8_t value, uint64_t num) {
	__asm__ __volatile__ (	"movq %rdi, %r9;"
							"movq %rdx, %rcx;"
							"andl $7, %edx;"
							"shrq $3, %rcx;"
							"movzbl %sil, %esi;"
							"movabs $0x0101010101010101, %rax;"
							"imulq %rsi, %rax;"
							"rep stosq;"
							"movl %edx, %ecx;"
							"rep stosb;"
							"movq %r9, %rax;"
							"ret;");
}

int memcmp(const void * _s1, const void* _s2, int n) {
	const unsigned char* s1 = (unsigned char*) _s1;
	const unsigned char* s2 = (unsigned char*) _s2;

	while(n--) {
		if(*s1 != *s2) {
			return *s1 - *s2;
		}
		++s1;
		++s2;
	}
	return 0;
}

int strcmp(char* str1, char* str2) {
	while (*str1 && *str2) {
		if (*str1 != *str2)
			return *str1 - *str2;
		str1++;
		str2++;
	}
	return *str1 - *str2;
}