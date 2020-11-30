int assert(int, int, char *);
int printf();

typedef struct TupleIntInt TupleIntInt;
struct TupleIntInt {
  int a;
  int b;
};

typedef struct TupleCharChar TupleCharChar;
struct TupleCharChar {
  char a;
  char b;
};

int test1() {
  TupleIntInt instance1;
  TupleIntInt instance2;
  TupleIntInt instance3;
  TupleIntInt instance4;
  instance1.a = 34;
  instance2.a = 2184;
  instance3.a = 134;
  instance4.a = 3809;
  instance1.b = 13789;
  instance2.b = 133;
  instance3.b = 643;
  instance4.b = 1319;
  assert(34, instance1.a, "instance1.a");
  assert(2184, instance2.a, "instance2.a");
  assert(134, instance3.a, "instance3.a");
  assert(3809, instance4.a, "instance4.a");
  assert(13789, instance1.b, "instance1.b");
  assert(133, instance2.b, "instance2.b");
  assert(643, instance3.b, "instance3.b");
  assert(1319, instance4.b, "instance4.b");
  return 0;
}

int test2() {
  TupleCharChar instance1;
  TupleCharChar instance2;
  TupleCharChar instance3;
  TupleCharChar instance4;
  instance1.a = 34;
  instance2.a = 2184;
  instance3.a = 134;
  instance4.a = 3809;
  instance1.b = 13789;
  instance2.b = 133;
  instance3.b = 643;
  instance4.b = 1319;
  assert(34, instance1.a, "instance1.a");
  assert(-120, instance2.a, "instance2.a");
  assert(-122, instance3.a, "instance3.a");
  assert(-31, instance4.a, "instance4.a");
  assert(-35, instance1.b, "instance1.b");
  assert(-123, instance2.b, "instance2.b");
  assert(-125, instance3.b, "instance3.b");
  assert(39, instance4.b, "instance4.b");
  return 0;
}

typedef struct TupleCharCharIntChar TupleCharCharIntChar;
struct TupleCharCharIntChar {
  char a;
  char b;
  int c;
  char d;
};

typedef struct TupleCharCharCharInt TupleCharCharCharInt;
struct TupleCharCharCharInt {
  char a;
  char b;
  char c;
  int d;
};

typedef struct TupleCharIntCharIntChar TupleCharIntCharIntChar;
struct TupleCharIntCharIntChar {
  char a;
  int b;
  char c;
  int d;
  char e;
};

int test3() {
  assert(8, sizeof(TupleIntInt), "sizeof(TupleIntInt)");
  assert(2, sizeof(TupleCharChar), "sizeof(TupleCharChar)");
  assert(12, sizeof(TupleCharCharIntChar), "sizeof(TupleCharCharIntChar)");
  assert(8, sizeof(TupleCharCharCharInt), "sizeof(TupleCharCharCharInt)");
  assert(20, sizeof(TupleCharIntCharIntChar),
         "sizeof(TupleCharIntCharIntChar)");
  return 0;
}

int test4() {
  assert(4, _Alignof(TupleIntInt), "_Alignof(TupleIntInt)");
  assert(1, _Alignof(TupleCharChar), "_Alignof(TupleCharChar)");
  assert(4, _Alignof(TupleCharCharIntChar), "_Alignof(TupleCharCharIntChar)");
  assert(4, _Alignof(TupleCharCharCharInt), "_Alignof(TupleCharCharCharInt)");
  assert(4, _Alignof(TupleCharIntCharIntChar),
         "_Alignof(TupleCharIntCharIntChar)");
  return 0;
}

typedef struct TupleCompound TupleCompound;
struct TupleCompound {
  TupleCharChar a;
  TupleCharCharIntChar b;
  TupleCharIntCharIntChar c;
};

int test5() {
  assert(4, _Alignof(TupleCompound), "_Alignof(TupleCompound)");
  assert(36, sizeof(TupleCompound), "sizeof(TupleCompound)");
  return 0;
}

int main() {
  test1();
  test2();
  test3();
  test4();
  test5();
  printf("OK\n");
  return 0;
}
