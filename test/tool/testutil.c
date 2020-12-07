#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

void assert(int expected, int actual, char *code) {
  if (expected == actual) {
    printf("\033[32m");
    printf("%s => %d\n", code, actual);
    printf("\033[39m");
  } else {
    printf("\033[31m");
    printf("%s => %d expected but got %d\n", code, expected, actual);
    printf("\033[39m");
  }
}
