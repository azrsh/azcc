int printf();
int puts();
static int A[200][200];
int test152() {
  int i;
  for (i = 1; i <= 12; i++) {
    printf("%d %d", i, i);
    puts("");
  }
  return 0;
}
