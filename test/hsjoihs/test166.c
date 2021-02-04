int printf();
int puts();
static int a(int N) { return 3; }
int test166() {
  int i;
  for (i = 1; i <= 12; i++) {
    printf("%d %d", i, a(i));
    puts("");
  }
  return 0;
}
