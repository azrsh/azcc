static int *foo(int *p) {
  *p = 4;
  return p;
}
int test101() {
  int x;
  int *y;
  y = foo(&x);
  *y += 170;
  return x;
}
