struct A {
  int a;
  int b;
  int *p;
};
static struct A f(void) {
  struct A u;
  u.a = 100;
  u.b = 74;
  u.p = 0;
  return u;
}
int test312(void) {
  struct A u = f();
  if (u.p) {
    return 3;
  } else {
    return u.a + u.b;
  }
}
