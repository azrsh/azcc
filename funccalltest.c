#include "stdio.h"

int foo() { return 0; }

int foo1(int a) { return a; }

int foo2(int a, int b) { return a + b; }

int foo3(int a, int b, int c) { return a + b + c; }

int foo7(int a, int b, int c, int d, int e, int f, int g) {
  return a + b + c + d + e + f + g;
}

int foo8(int a, int b, int c, int d, int e, int f, int g, int h) {
  return a + b + c + d + e + f + g + h;
}

int foo9(int a, int b, int c, int d, int e, int f, int g, int h, int i) {
  return a + b + c + d + e + f + g + h + i;
}

int foo10(int a, int b, int c, int d, int e, int f, int g, int h, int i,
          int j) {
  return a + b + c + d + e + f + g + h + i + j;
}
