static int a(int b) { return b; }
int test171() {
  int i;
  i = 1;
  return a(i == 1 ? 174 : 2);
}
