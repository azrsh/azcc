static int changeBoard(int board[30][30], int i, int j, int d, int N) {
  int k;
  for (k = 0; k < N; k++) {
    board[i][k] += d;
    board[k][j] += d;
  }
  if (i > j) {
    for (k = 0; k < N - (i - j); k++) {
      board[k + (i - j)][k] += d;
    }
  } else {
    for (k = 0; k < N - (j - i); k++) {
      board[k][k + (j - i)] += d;
    }
  }
  if (i + j < N) {
    for (k = 0; k <= i + j; k++) {
      board[i + j - k][k] += d;
    }
  } else {
    for (k = i + j - N + 1; k < N; k++) {
      board[i + j - k][k] += d;
    }
  }
  return 0;
}
static int setQueen(int board[30][30], int num_placed, int *ptr_sol_num,
                    int N) {
  int j;
  if (num_placed == N) {
    (*ptr_sol_num) += 1;
    return 0;
  }
  for (j = 0; j < N; j++) {
    if (board[num_placed][j] == 0) {
      changeBoard(board, num_placed, j, +1, N);
      setQueen(board, num_placed + 1, ptr_sol_num, N);
      changeBoard(board, num_placed, j, -1, N);
    }
  }
  return 0;
}
static int board_[30][30];
int test131() {
  int sol_num;
  sol_num = 0;
  setQueen(board_, 0, &sol_num, 8);
  return sol_num;
}
