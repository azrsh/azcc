#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

void test_assert(int expected, int actual, char *code) {
  if (expected == actual) {
    printf("\033[32m");
    printf("PASS ");
    printf("\033[39m");
    printf("%s => %d\n", code, actual);
  } else {
    printf("\033[31m");
    printf("FAIL ");
    printf("\033[39m");
    printf("%s => %d expected but got %d\n", code, expected, actual);
    exit(EXIT_FAILURE);
  }
}
