static int x;
static int *y;
int test117() {
  x = 3;
  int a;
  a = 2;
  y = &a;
  return x + *y + 169;
}
