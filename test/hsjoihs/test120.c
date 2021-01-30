static int x;
static int *y;
int test120() {
  x = 3;
  int a[1];
  *a = 2;
  y = a;
  return x + *y + 169;
}
