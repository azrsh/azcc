static int a;
int test159() {
  int *p;
  p = &a;
  int i;
  for (i = 0; i < 174; i++) {
    ++*p;
  }
  return a;
}
