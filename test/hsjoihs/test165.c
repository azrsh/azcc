int printf();
static int a(int N) { return 3; }
int test165() {
  int i;
  printf("%d %d", i, a(i));
  return 0;
}
