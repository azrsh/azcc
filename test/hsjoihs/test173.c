int printf();
int puts();
static int count;
int test173() {
  int i;
  int hist[20];
  for (i = 1; i < 11; i++) {
    printf("%s", (i == 1 ? "a" : "b"));
    puts("");
  }
  return 0;
}
