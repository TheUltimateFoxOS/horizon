#pragma once

char* strcpy(char* dest, const char* src);
int strlen(char* src);
int strnlen(const char *s, int maxlen);
int sprintf(char *buf, const char *fmt, ...);

void debugf(const char *fmt, ...);
void printf(const char *fmt, ...);