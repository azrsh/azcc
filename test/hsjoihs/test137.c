static char foo(char *p) {
  *p = 5;
  char a;
  a = 3;
  return a;
}
int test137() {
  char q;
  char r;
  r = foo(&q);
  return 172 - r + q;
}
