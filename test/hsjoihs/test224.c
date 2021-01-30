static int f(int a) {
  switch (a) {
  case 1:
    return 3;
  case 2:
    return 5;
  default:
    return 8;
  }
}
int test224(void) {
  return (f(1) - 3) || (f(2) - 5) || (f(3) - 8) || (f(100) - 8);
}
