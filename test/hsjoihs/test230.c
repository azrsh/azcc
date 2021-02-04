static void f(int *p) { *p = 174; }
int test230(void) {
  int a;
  f(&a);
  return a;
}
