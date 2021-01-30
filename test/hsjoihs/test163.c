static int foo(char *a, int b, int c) { return 0; }
static int a(int N) { return 3; }
int test163() {
  int i;
  foo("%d %d", i, a(i));
  return 0;
}
