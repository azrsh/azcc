static int a(int b) { return b; }
int test169() {
  int i;
  i = 1;
  a(i == 1 ? 1 : 2);
  return 0;
}
