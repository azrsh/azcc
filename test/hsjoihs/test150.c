static char foo() {
  char *x;
  x = "1ab";
  return x[0];
}
int test150() {
  char *y;
  y = "a2b";
  int z;
  z = 12;
  char a;
  a = y[1];
  return (a - foo()) * z + 162;
}
