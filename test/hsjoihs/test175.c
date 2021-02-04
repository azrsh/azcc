int printf();
int puts();
static int count;
static int solve(int n, int col, int *hist) {
  if (col == n) {
    count += 1;
    return 0;
  }
  int i;
  int j;
  for (i = 0, j = 0; i < n; i++) {
    for (j = 0; j < col && hist[j] != i && (hist[j] - i) != col - j &&
                (hist[j] - i) != j - col;
         j++) {
    }
    if (j < col)
      continue;
    hist[col] = i;
    solve(n, col + 1, hist);
  }
  return 0;
}
int test175() {
  int i;
  int hist[20];
  for (i = 1; i < 11; i++) {
    count = 0;
    solve(i, 0, hist);
    printf("%d queen%s: %d", i, (i == 1 ? " " : "s "), count);
    puts("");
  }
  return 0;
}
