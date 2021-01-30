static void f(int *p) {
  *p = 174;
  return;
}
int test213(void) {
  int a;
  f(&a);
  return a;
}
