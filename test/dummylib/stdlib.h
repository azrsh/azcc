#ifndef STDLIB_H
#define STDLIB_H

#define NULL 0

typedef int size_t;
void *calloc(size_t nmemb, size_t size);
int strtol(const char *s, char **endptr, int base);
#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0

#endif
