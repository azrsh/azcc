int assert(int, int, char *);
int printf();

void test(int a) {
  void *b = &a;
  int *c = b;
  int d = *c;
  assert(a, d, "void *b = &a; int *c = b; int d = *c;");
}

void return_test() { return; }

int main() {
  test(1);
  test(10);
  test(1245);
  test(2);
  test(67);
  test(240);

  // void a = test(240); //ちゃんとエラーになる
  // int a = test(240); //ちゃんとエラーになる

  return_test();

  printf("OK\n");
  return 0;
}
