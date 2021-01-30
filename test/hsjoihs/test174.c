int printf();
int puts();
static int count;
int test174() {
  int i;
  int hist[20];
  for (i = 1; i < 11; i++) {
    printf("%d %s: %d", i, (i == 1 ? " " : "s "), i);
    puts("");
  }
  return 0;
}
