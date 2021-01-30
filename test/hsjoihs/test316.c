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
int test316(void) {
  struct A u = f();
  struct A *p = &u;
  if (u.p) {
    return 3;
  } else {
    return p->a + p->b;
  }
}
