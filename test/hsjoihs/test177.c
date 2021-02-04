int test177() {
  int a;
  int *p;
  p = &a;
  *p = 2;
  int *q;
  q = &(*p);
  *q = 174;
  return a;
}
