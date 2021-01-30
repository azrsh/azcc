static int *foo(int *p) {
  *p = 4;
  return p;
}
int test102() {
  int x;
  int y;
  *foo(&x) += 170;
  return x;
}
