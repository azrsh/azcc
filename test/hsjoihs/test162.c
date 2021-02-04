int printf();
static int a() { return 3; }
int test162() {
  int i;
  printf("%d %d", i, a());
  return 0;
}
