static int a[10][10];
static int foo(int p[10][10]) {
  int q;
  q = ((p += 1) - 1)[0][0];
  return q + p[0][0];
}
int test265() {
  a[0][0] = 100;
  a[1][0] = 74;
  return foo(a);
}
