#ifndef TESTTOOL_H
#define TESTTOOL_H

void assert(int expected, int actual, char *code);
void alloc4(int **p, int v1, int v2, int v3, int v4);
int foo();
int foo1(int a);
int foo2(int a, int b);
int foo3(int a, int b, int c);
int foo7(int a, int b, int c, int d, int e, int f, int g);
int foo8(int a, int b, int c, int d, int e, int f, int g, int h);
int foo9(int a, int b, int c, int d, int e, int f, int g, int h, int i);
int foo10(int a, int b, int c, int d, int e, int f, int g, int h, int i, int j);

#endif
