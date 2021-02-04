static int *foo() { return 0; }
int test291() {
  int *p = foo();
  if (p == 0) {
    return 174;
  } else {
    return 1;
  }
}
