static int *foo(int *p) {
  *p = 4;
  return p;
}
int test113() {
  int x;
  int y;
  int **z;
  *foo(&x) += 170;
  return x;
}
