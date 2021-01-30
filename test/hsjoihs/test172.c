int printf();
int puts();
static int count;
int test172() {
  int i;
  int hist[20];
  for (i = 1; i < 11; i++) {
    printf(i == 1 ? "a" : "b");
    puts("");
  }
  return 0;
}
