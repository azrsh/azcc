struct A {
  int a;
  int b;
};
static struct A f(int a, int b) {
  struct A q;
  q.a = a;
  q.b = b;
  return q;
}
int test356() {
  struct A (*g)(int, int) = f;
  struct A q = g(100, 74);
  return q.a + q.b;
}
