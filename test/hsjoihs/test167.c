int printf();
int puts();
static int A[200][200];
static int a(int row, int N) { return 3; }
int test167() {
  int i;
  for (i = 1; i <= 12; i++) {
    printf("%d %d", i, a(0, i));
    puts("");
  }
  return 0;
}
