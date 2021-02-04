int printf();
int puts();
static int a(int b, int c) { return 3; }
int test153() {
  int i;
  for (i = 1; i <= 12; i++) {
    int j;
    j = a(0, i);
    printf("%d %d", i, j);
    puts("");
  }
  return 0;
}
