struct A {
  int a;
  int b;
  int *p;
};
static struct A f(int j) {
  struct A u;
  u.a = 100;
  u.b = 72 + j;
  u.p = 0;
  return u;
}
static int g(struct A *p) { return p->a + p->b; }
int test320(void) {
  struct A u = f(2);
  struct A *p = &u;
  if (u.p) {
    return 3;
  } else {
    return g(p);
  }
}
