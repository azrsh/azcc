#include "stdio.h"

int foo() {
  printf("OK\n");
  return 0;
}

int foo1(int x) {
  printf("%d\n", x);
  return 0;
}

int foo2(int x, int y) {
  printf("%d\n", x + y);
  return 0;
}

int foo3(int x, int y, int z) {
  printf("%d\n", x + y + z);
  return 0;
}
