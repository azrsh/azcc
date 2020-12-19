#ifndef STDIO_H
#define STDIO_H

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2
#define EOF -1

#include <stdlib.h>

extern struct __FILE *stderr;
typedef struct __FILE FILE;

int fprintf();
int printf();
int sprintf();

FILE *fopen(const char *filename, const char *mode);
int fseek(FILE *stream, /*long*/ int offset,
          int origin); // azccの内部では整数値は常に64bitかつlong未実装のため
int ftell(FILE *stream);
size_t fread(void *ptr, size_t size, size_t n, FILE *stream);
int fclose(FILE *stream);

int getchar();

#endif
