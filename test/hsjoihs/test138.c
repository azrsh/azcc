static char a;
static char foo(char *p) {
  *p = 5;
  a = 3;
  return a;
}
int test138() {
  char q;
  char r;
  r = foo(&q);
  return 172 - r + q;
}
