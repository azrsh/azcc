static int x;
int test121() {
  x = 3;
  int *y;
  y = &x;
  return *y + 171;
}
