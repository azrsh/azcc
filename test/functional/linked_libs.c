#include "testtool.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void memalloc() {
  int *array = calloc(10, sizeof(int));
  for (int i = 0; i < 10; i++) {
    array[i] = i;
    assert(i, array[i], "i == array[i]");
  }
}

int main() {
  memalloc();
  printf("OK\n");
  return 0;
}
