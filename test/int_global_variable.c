int a;
int test1() {
  int a[2];
  a[0] = 1;
  a[1] = 2;
  int *p;
  p = a;
  return p[0] + p[1];
}
int asdjuwgehij;
int test2() {
  int a[2];
  a[0] = 1;
  a[1] = 2;
  int *p;
  p = a;
  return p[0] + p[1];
}
int test3() {
  a = 1;
  return a;
}
int b;
int test4() {
  a = 1;
  b = 2;
  return a + b;
}

int main() {
  assert(3, test1(),
         "int a; int main(){ int a[2]; a[0] = 1; a[1] = 2; int *p; p = a;..."
         /*"return p[0] + p[1]; }"*/);
  assert(
      3, test2(),
      "int asdjuwgehij; int main(){ int a[2]; a[0] = 1; a[1] = 2; int *p; p..."
      /*"= a; return p[0] + p[1]; }"*/);
  assert(1, test3(), "int a; int main(){ a = 1; return a; }");
  assert(3, test4(), "int a; int b; int main(){ a = 1; b = 2; return a + b; }");
  printf("OK\n");
  return 0;
}
