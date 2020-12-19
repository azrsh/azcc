#ifndef STDLIB_H
#define STDLIB_H

#define NULL 0

typedef int size_t;
void *calloc(size_t nmemb, size_t size);
void *realloc(void *ptr, size_t size);
int strtol(const char *s, char **endptr, int base);
int exit(int status);
#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0

/*
#ifdef __STDC__
#include <stdarg.h>
typedef struct __FILE FILE;
int fprintf(FILE *restrict, const char *restrict, ...);
int printf(const char *restrict, ...);
int sprintf(char *restrict s, const char *restrict format, ...);
int vfprintf(FILE *stream, const char *format, va_list arg);
int vprintf(const char *format, va_list arg);
#endif
#ifndef __STDC__
int fprintf();
int printf();
int sprintf();
int vfprintf(struct __FILE *stream, const char *format,
             struct va_list_tag arg[1]);
int vprintf(const char *format, struct va_list_tag arg[1]);
#endif*/

#endif
