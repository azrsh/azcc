static int a;
int test160() {
  int *p;
  p = &a;
  int i;
  for (i = 0; i < 174; ((i))++) {
    ++*p;
  }
  return a;
}
