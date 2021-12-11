#pragma once

#include <stdint.h>

char* strcpy(char* dest, const char* src);
int strlen(char* src);
int strnlen(const char *s, int maxlen);
char* strcat(char* dest, const char* src); // X
int strcmp(char* str1, char* str2); // x
char* strchr(const char* str, int chr);

int sprintf(char *buf, const char *fmt, ...);

void* memcpy(void* dest, const void* src, int n);
int memcmp(const void * _s1, const void* _s2, int n); // X
void* memset(void* start, uint8_t value, uint64_t num);