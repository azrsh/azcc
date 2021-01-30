static int a(int b) { return b; }
int test170() {
  int i;
  for (i = 1; i < 11; i++) {
    a(i == 1 ? 1 : 2);
  }
  return 0;
}
