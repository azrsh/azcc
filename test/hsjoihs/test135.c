static char foo(char *p) {
  char a;
  a = 5;
  return a;
}
int test135() {
  char q;
  foo(&q);
  return 174;
}
