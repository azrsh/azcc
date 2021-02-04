struct A {
  int a;
  int b;
  int *q;
  int *t;
  int *p;
};
static struct A f(void) {
  struct A u;
  u.a = 100;
  return u;
}
int test324(void) {
  struct A u = f();
  struct A v;
  return (v = u).a + 74;
}
