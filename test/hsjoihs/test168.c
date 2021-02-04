int printf();
int puts();
static int A[200][200];
static int dfs(int row, int N) {
  if (row == N)
    return 1;
  int ret;
  ret = 0;
  int col;
  for (col = 0; col < N; col++) {
    int ok;
    ok = 1;
    int i;
    for (i = 1; i < N; i++) {
      if (row - i >= 0 && col - i >= 0) {
        ok = ok && A[row - i][col - i] == 0;
      }
      if (row - i >= 0) {
        ok = ok && A[row - i][col] == 0;
      }
      if (row - i >= 0 && col + i < N) {
        ok = ok && A[row - i][col + i] == 0;
      }
    }
    if (ok) {
      A[row][col] = 1;
      ret += dfs(row + 1, N);
      A[row][col] = 0;
    }
  }
  return ret;
}
int test168() {
  int i;
  for (i = 1; i < 12; i++) {
    printf("%d queen: %d", i, dfs(0, i));
    puts("");
  }
  return 0;
}
