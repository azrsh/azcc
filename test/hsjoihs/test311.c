struct A {
  int a;
};
static struct A f(void) {
  struct A u;
  u.a = 174;
  return u;
}
int test311(void) {
  struct A u = f();
  return u.a;
}
