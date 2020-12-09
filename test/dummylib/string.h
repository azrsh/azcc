#ifndef STRING_H
#define STRING_H

#include <stdlib.h>

#define NULL 0

size_t strlen(const char *s);
char *strchr(const char *s, int c);
char *strstr(const char *s1, const char *s2);
void *memcpy(void *dest, const void *src, size_t n);
int memcmp(const void *s1, const void *s2, size_t n);

char *strerror(int errnum);

#endif
