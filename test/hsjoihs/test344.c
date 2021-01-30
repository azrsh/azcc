int puts(const char *str);
int atoi(const char *str);
static int f(int a) {
  int (*arr[2])(const char *str);
  arr[0] = puts;
  arr[1] = atoi;
  return arr[a]("123");
}
int test344() {
  f(0);
  return f(1);
}
