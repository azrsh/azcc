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
int test326(void) {
  struct A u = f();
  return (1, u).a + 74;
}
