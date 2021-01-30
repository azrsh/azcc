#include "hsjoihs_test.h"

void test_assert(int, int, char *);

int main() {
  test_assert(
      3, test365(),
      "int foo(void) { return 3; } int bar(void) { return 5;} int "
      "test365"
      "(void) { int (*foo1)(void) = foo; int (*bar1)(void) = bar; return "
      "(1? foo1 : bar1)(); }");
  test_assert(3, test366(),
              "int foo(void) { return 3; } int "
              "test366"
              "(void) { return (1? foo : 0)(); }");
  test_assert(3, test367(),
              "int foo(void) { return 3; } int bar(void) { return 5;} int "
              "test367"
              "(void) { return (1? foo : bar)(); }");
  test_assert(5, test368(),
              "int foo(void) { return 3; } int bar(void) { return 5;} int "
              "test368"
              "(void) { return (0? foo : bar)(); }");

  test_assert(8, test363(),
              "union A { char a[7]; int b; }; int "
              "test363"
              "(void) { return sizeof(union A); }");
  test_assert(
      0, test364(),
      "union A { char a[4]; int b; }; int "
      "test364"
      "(void) { union A x; x.a[0] = 0x4b; x.a[1] = 0x6f; x.a[2] = 0x72; "
      "x.a[3] = 0x79; return x.b - 0x79726f4b; }");

  test_assert(0, test359(),
              "int "
              "test359"
              "() {goto a; return 3; a: return 0;} ");
  test_assert(
      3, test360(),
      "int "
      "test360"
      "(){ int i = 3; goto a; for (i = 0; i < 10; i++) { a: return i; } }");

  test_assert(0, test357(),
              "int "
              "test357"
              "() {return 0;} //nfsjdgkssfdvc");
  test_assert(0, test358(),
              "int "
              "test358"
              "() {return __func__[0] - 116;} ");

  test_assert(21, test354(),
              "struct A {int k[15];}; struct A f(int a, int b){struct A q; "
              "q.k[0] = a; q.k[14] = b; return q;} int "
              "test354"
              "(){struct A (*g)(int a, int b) = f; struct A q = g(10, 11); "
              "return q.k[0] + q.k[14]; }");
  test_assert(
      3, test355(),
      "struct A3 {char a[3];}; struct A3 deref3(struct A3 *p){ return *p;} int "
      "test355"
      "(){return 3;}");
  test_assert(
      174, test356(),
      "struct A {int a; int b;}; struct A f(int a, int b){struct A q; "
      "q.a = a; q.b = b; return q;} int "
      "test356"
      "(){ struct A (*g)(int, int) = f; struct A q = g(100, 74); return "
      "q.a + q.b;}");

  test_assert(3, test343(),
              "int "
              "test343"
              "(){char a[456]; return a + 3 - a; }");
  test_assert(3, test345(),
              "struct A {int k[15];}; int "
              "test345"
              "(){struct A s; return 3;}");
  test_assert(3, test346(),
              "struct A {int k[15]; int a;}; int "
              "test346"
              "(){struct A s; s.a = 3; return s.a;}");
  test_assert(64, test347(),
              "struct A {int k[15]; int a;}; int "
              "test347"
              "(){return sizeof(struct A);}");
  test_assert(35, test348(),
              "struct A {int k[15];}; int "
              "test348"
              "(){struct A s; void *p = s.k; return 35;}");
  test_assert(35, test349(),
              "struct A {int k[15];}; int "
              "test349"
              "(){struct A s; int *p = s.k; return 35;}");
  test_assert(35, test350(),
              "struct A {int k[15];}; int "
              "test350"
              "(){struct A s; int (*p)[15] = &s.k; return 35;}");
  test_assert(35, test351(),
              "struct A {int k[15];}; int "
              "test351"
              "(){struct A s; s.k[3] = 35; return s.k[3];}");
  test_assert(3, test352(),
              "struct A {int a; int b; int c;}; int "
              "test352"
              "(){struct A a[5]; return a + 3 - a;}");
  test_assert(3, test353(),
              "struct A {int k[15];}; int "
              "test353"
              "(){struct A a[5]; return a + 3 - a;}");

  test_assert(123, test332(),
              "int "
              "test332"
              "(){int a = 1; int *b = a?&a : 0; return 123;}");
  test_assert(123, test333(),
              "int "
              "test333"
              "(){int a = 1; int *b = a? 0 :&a; return 123;}");
  test_assert(123, test334(),
              "int "
              "test334"
              "(){int a = 0; int *b = a?&a : 0; return 123;}");
  test_assert(123, test335(),
              "int "
              "test335"
              "(){int a = 0; int *b = a? 0 :&a; return 123;}");

  test_assert(0, test341(),
              "int printf(); int "
              "test341"
              "(){(**************printf)(\" Hello,"
              "World !\"); return 0;}");
  test_assert(123, test342(),
              "int puts(const char *str); int atoi(const char *str); int f(int "
              "a){int (*arr[2])(const char *str);arr[0] = &puts;arr[1] = "
              "&atoi;return arr[a](\" 123 \");} int "
              "test342"
              "(){f(0); return f(1);}");
  test_assert(123, test344(),
              "int puts(const char *str); int atoi(const char *str); int f(int "
              "a){int (*arr[2])(const char *str);arr[0] = puts;arr[1] = "
              "atoi;return arr[a](\" 123 \");} int "
              "test344"
              "(){f(0); return f(1);}");

  test_assert(123, test329(),
              "int "
              "test329"
              "(){void *null = 0; int (*p)(void) = null; return 123;}");
  test_assert(123, test330(),
              "int "
              "test330"
              "(){void *null = 0; int (*p)(int) = null; return 123;}");

  test_assert(
      174, test323(),
      "struct A{int a; int b; int *q; int *t; int *p;}; struct A f(void) "
      "{struct A u; u.a = 100;return u;} int "
      "test324"
      "(void){struct A u = f(); return (u, 174);}");
  test_assert(
      174, test324(),
      "struct A{int a; int b; int *q; int *t; int *p;}; struct A f(void) "
      "{struct A u; u.a = 100;return u;} int "
      "test325"
      "(void){struct A u = f(); struct A v; return (v = u).a + 74;}");
  test_assert(
      174, test325(),
      "struct A{int a; int b; int *q; int *t; int *p;}; struct A f(void) "
      "{struct A u; u.a = 100;return u;} int "
      "test326"
      "(void){struct A u = f(); return (1,u).a + 74;}");
  test_assert(
      174, test326(),
      "struct A{int a; int b; int *q; int *t; int *p;}; struct A f(void) "
      "{struct A u; u.a = 100;return u;} int "
      "test326"
      "(void){struct A u = f(); return (1,2,u).a + 74;}");
  test_assert(174, test327(),
              "struct A{int a; int b; int *q; int *t; int *p;}; struct A "
              "f(void) {struct "
              "A u; u.a = 100;return u;} int "
              "test327"
              "(void){struct A u = f(); struct A v; v.a = 1; return (1? u : "
              "v).a + 74;}");
  test_assert(75, test328(),
              "struct A{int a; int b; int *q; int *t; int *p;}; struct A "
              "f(void) {struct "
              "A u; u.a = 100;return u;} int "
              "test328"
              "(void){struct A u = f(); struct A v; v.a = 1; return (0? u : "
              "v).a + 74;}");

  test_assert(
      174, test321(),
      "struct A{int a; int b; int *q; int *t; int *p;}; struct A f(void) "
      "{struct A u; u.a = 100;return u;} int "
      "test322"
      "(void){struct A u = f(); return u.a + 74;}");
  test_assert(
      174, test322(),
      "struct A{int a; int b; int *q; int *r; int *s; int *t; int *p;}; struct "
      "A "
      "f(void) {struct A u; u.a = 100; u.b = 74; u.p = 0; return u;} int "
      "test323"
      "(void){struct A u = f(); struct A *p = &u; if (u.p) {return 3;} else "
      "{return p->a + p->b;}}");

  test_assert(174, test316(),
              "struct A{int a; int b; int *p;}; struct A f(void) {struct A u; "
              "u.a = 100; u.b = 74; u.p = 0; return u;} int "
              "test316"
              "(void){struct A u = f(); struct A *p = &u; if (u.p) {return 3;} "
              "else {return p->a + p->b;}}");
  test_assert(174, test317(),
              "struct A{int a; int b; int *p;}; struct A f(void) {struct A u; "
              "u.a = 100; u.b = 74; u.p = 0; return u;} int g (struct A *p) "
              "{return p->a + p->b;} int "
              "test317"
              "(void){struct A u = f(); struct A *p = &u; if (u.p) {return 3;} "
              "else {return g(p);}}");

  test_assert(
      174, test320(),
      "struct A{int a; int b; int *p;}; struct A f(int j) {struct A u; "
      "u.a = 100; u.b = 72 + j; u.p = 0; return u;} int g (struct A *p) "
      "{return p->a + p->b;} int "
      "test320"
      "(void){struct A u = f(2); struct A *p = &u; if (u.p) {return 3;} "
      "else {return g(p);}}");

  test_assert(174, test204(),
              "int "
              "test204"
              "(){int *p; p = 0; if(p) {return 4; } return 174;}");
  test_assert(4, test205(),
              "int "
              "test205"
              "(){int *p; int a; p = &a; if(p) {return 4; } return 174;}");
  test_assert(1, test206(),
              "int "
              "test206"
              "(){int *p; int a; p = &a; return p && &p;}");
  test_assert(1, test207(),
              "int "
              "test207"
              "(){int *p; int a; p = &a; return p || &p;}");
  test_assert(174, test208(),
              "int "
              "test208"
              "(){int *p; int a; p = &a; return p?174:1;}");
  test_assert(1, test209(),
              "int "
              "test209"
              "(){int *p; p = 0; return p?174:1;}");
  test_assert(174, test210(),
              "int "
              "test210"
              "(void){return 174;}");
  test_assert(174, test211(),
              "int "
              "test211"
              "(void){void *p; p = 0; p = p; return 174;}");
  test_assert(
      8, test212(),
      "struct A{int a; int b;}; int "
      "test212"
      "(){ struct A *p; void *q1; void *q2; q1 = p; q2 = p+1; char *r1; "
      "char *r2; r1 = q1; r2 = q2; return r2-r1;}");
  test_assert(174, test213(),
              "void f(int *p){*p = 174; return;} int "
              "test213"
              "(void){ int a; f(&a); return a;}");
  test_assert(174, test214(),
              "int "
              "test214"
              "(void){ foo: return 174;}");
  test_assert(174, test215(),
              "int "
              "test215"
              "(void){ foo: bar: return 174;}");
  test_assert(174, test216(),
              "int "
              "test216"
              "(void){ foo: {baz: hoge: 1;} bar: return 174;}");
  test_assert(174, test217(),
              "int "
              "test217"
              "(void){ int a; a = 174; switch(1){a = 2; 1;} return a;}");
  test_assert(
      174, test218(),
      "int "
      "test218"
      "(void){ int a; a = 174; switch(1){a = 2; break; a = 3;} return a;}");
  test_assert(
      174, test219(),
      "int "
      "test219"
      "(void){ int a; a = 1; int b; b = 0; switch(1){ b = 15; default: a "
      "= 174; break; a = 3;} return a+b ;}");
  test_assert(174, test220(),
              "int "
              "test220"
              "(void){ switch(1){ if(0){ default: return 174; } } return 3; }");
  test_assert(174, test221(),
              "int "
              "test221"
              "(void){ int a; a = 1; switch(1){ default: a = 173; switch(0){ "
              "default: return a+1; } return 5; } return 3; }");
  test_assert(
      174, test222(),
      "int "
      "test222"
      "(void){ int a; a = 1; switch(1){ case 1: a = 174; } return a; }");
  test_assert(
      174, test223(),
      "int "
      "test223"
      "(void){ int a; a = 174; switch(2){ case 1: a = 1; } return a; }");
  test_assert(
      0, test224(),
      "int f(int a){switch(a){case 1: return 3; case 2: return 5; "
      "default: return 8;}} int "
      "test224"
      "(void){ return (f(1)-3) || (f(2)-5) || (f(3)-8) || (f(100)-8);}");
  test_assert(4, test225(),
              "int "
              "test225"
              "(){return _Alignof(int);}");
  test_assert(8, test226(),
              "int "
              "test226"
              "(){return _Alignof(int*);}");
  test_assert(4, test227(),
              "struct A{int a; int b;}; int "
              "test227"
              "(){ return _Alignof(struct A);}");
  test_assert(4, test228(),
              "struct A{int a; char c; char d; int b;}; int "
              "test228"
              "(){ return _Alignof(struct A);}");
  test_assert(8, test229(),
              "struct A{int a; int *b; int c;}; int "
              "test229"
              "(){return _Alignof(struct A [5]);}");
  test_assert(174, test230(),
              "void f(int *p){*p = 174;} int "
              "test230"
              "(void){ int a; f(&a); return a;}");
  test_assert(-82, test231(),
              "int "
              "test231"
              "(void){ char a; a = 0; switch(a){case 0: a = 174; break; case "
              "256: a = 3; break; default: a = 5; break;}  return a;}");
  test_assert(174, test232(),
              "enum A{B, C}; int "
              "test232"
              "(void){ enum A b; return 174; }");
  test_assert(174, test233(),
              "enum A{B, C,}; int "
              "test233"
              "(void){ enum A b; return 174; }");
  test_assert(174, test234(),
              "enum A{B, C,}; int "
              "test234"
              "(void){ enum A b; b = 5; return 174; }");
  test_assert(174, test235(),
              "enum A{B, C,}; int "
              "test235"
              "(void){ enum A b; b = B; return 174+b; }");
  test_assert(174, test236(),
              "enum A{B, C,D}; int "
              "test236"
              "(void){ enum A b; b = D; return 172+b; }");
  test_assert(
      0, test237(),
      "enum A{B, C,D}; int f(enum A b){switch(b){case B: return 1; case "
      "C: return 5; case D: return 8;}} int "
      "test237"
      "(void){ return (f(B) - 1) || (f(C) - 5) || (f(D) - 8);}");
  test_assert(174, test238(),
              "int "
              "test238"
              "(){int a[5]; if(a){return 174;} return 0;}");

  test_assert(174, test313(),
              "int "
              "test313"
              "(void){int p = 0; return (!p)*174; }");
  test_assert(174, test314(),
              "int "
              "test314"
              "(void){int *p = 0; return (!p)*174; }");
  test_assert(174, test315(),
              "int "
              "test315"
              "(void){int q; int *p = &q; return (1+!p)*174;}");

  test_assert(174, test311(),
              "struct A{int a;}; struct A f(void) {struct A u; u.a = 174; "
              "return u;} int "
              "test311"
              "(void){struct A u = f(); return u.a;}");
  test_assert(174, test312(),
              "struct A{int a; int b; int *p;}; struct A f(void) {struct A u; "
              "u.a = 100; "
              "u.b = 74; u.p = 0; return u;} int "
              "test312"
              "(void){struct A u = f(); if (u.p) {return 3;} else {return u.a "
              "+ u.b;}}");

  test_assert(
      174, test310(),
      "struct A{int a;}; int "
      "test310"
      "(){struct A arr[5]; void *p = arr; int *q = p; q[3] = 174; return "
      "arr[3].a;}");

  test_assert(
      174, test307(),
      "struct A{int a; int *b; int c;}; struct B{char d; struct A e;}; int "
      "test307"
      "(){struct A a; a.a = 174; struct B b; b.e = a; return b.e.a;}");
  test_assert(
      174, test308(),
      "struct A{int a; int *b; int c;}; struct B{char d; struct A e;}; int "
      "test308"
      "(){struct A a; a.a = 174; struct B b; b.e = a; return (b.e).a;}");
  test_assert(
      174, test309(),
      "struct A{int a; int *b; int c;}; struct B{char d; struct A e;}; int "
      "test309"
      "(){struct A a; a.a = 174; struct B b; b.e.a = 174; a = b.e; return "
      "a.a;}");

  test_assert(
      174, test305(),
      "int "
      "test305"
      "(void) {int a[5]; a[3] = 174; int (*p)[5] = &a; return (*p)[3];} ");
  test_assert(174, test306(),
              "int "
              "test306"
              "(void) {char a = 74; char *p = &a; return *p+100;} ");

  test_assert(174, test303(),
              "static int hidden() { return 3;} int "
              "test303"
              "(){return 171 + hidden();}");

  test_assert(74, test292(),
              "struct A{char a; int b;}; int "
              "test292"
              "(){struct A a; a.a = 74; struct A b; b = a; return b.a;}");
  test_assert(74, test293(),
              "struct A{char a; int b;}; int "
              "test293"
              "(){struct A a; a.a = 74; struct A b = a; return b.a;}");
  test_assert(174, test294(),
              "struct A{int a; int b;}; int "
              "test294"
              "(){struct A a; a.a = 174; struct A b = a; return b.a;}");
  test_assert(174, test295(),
              "struct A{int a; int b;}; int "
              "test295"
              "(){struct A a; a.a = 174; struct A b = a; return b.a;}");
  test_assert(74, test296(),
              "struct A{char a; int b; char c;}; int "
              "test296"
              "(){struct A a; a.c = 74; struct A b = a; return b.c;}");

  test_assert(
      174, test291(),
      "int *foo(){return 0;} int "
      "test291"
      "(){int *p = foo(); if (p == 0) {return 174;} else {return 1;} }");

  test_assert(
      1, test287(),
      "int "
      "test287"
      "(){int a[5];int *p = a;if (p == 0) {return 174;} else {return 1;}}");
  test_assert(
      174, test288(),
      "int "
      "test288"
      "(){int a[5];int *p = 0;if (p == 0) {return 174;} else {return 1;}}");
  test_assert(
      1, test289(),
      "int "
      "test289"
      "(){int a[5];int *p = 0;if (p != 0) {return 174;} else {return 1;}}");
  test_assert(
      174, test290(),
      "int "
      "test290"
      "(){int a[5];int *p = a;if (p != 0) {return 174;} else {return 1;}}");

  test_assert(174, test286(),
              "int main(); int main(void){return 174;} int "
              "test286"
              "(void);");

  test_assert(
      174, test274(),
      "int "
      "test274"
      "(){int a[5];int *p = a;int *q = a+3;if (p < q) {return 174;} else "
      "{return 1;}}");
  test_assert(
      1, test275(),
      "int "
      "test275"
      "(){int a[5];int *p = a;int *q = a+3;if (p > q) {return 174;} else "
      "{return 1;}}");
  test_assert(174, test276(),
              "int "
              "test276"
              "(){int a[5];int *p = a;int *q = a+3;if (p <= q) {return 174;} "
              "else {return 1;}}");
  test_assert(1, test277(),
              "int "
              "test277"
              "(){int a[5];int *p = a;int *q = a+3;if (p >= q) {return 174;} "
              "else {return 1;}}");
  test_assert(1, test278(),
              "int "
              "test278"
              "(){int a[5];int *p = a;int *q = a;if (p < q) {return 174;} else "
              "{return 1;}}");
  test_assert(1, test279(),
              "int "
              "test279"
              "(){int a[5];int *p = a;int *q = a;if (p > q) {return 174;} else "
              "{return 1;}}");
  test_assert(
      174, test280(),
      "int "
      "test280"
      "(){int a[5];int *p = a;int *q = a;if (p <= q) {return 174;} else "
      "{return 1;}}");
  test_assert(
      174, test281(),
      "int "
      "test281"
      "(){int a[5];int *p = a;int *q = a;if (p >= q) {return 174;} else "
      "{return 1;}}");
  test_assert(
      174, test282(),
      "int "
      "test282"
      "(){int a[5];int *p = a;int *q = a;if (p == q) {return 174;} else "
      "{return 1;}}");
  test_assert(
      1, test283(),
      "int "
      "test283"
      "(){int a[5];int *p = a;int *q = a;if (p != q) {return 174;} else "
      "{return 1;}}");
  test_assert(1, test284(),
              "int "
              "test284"
              "(){int a[5];int *p = a;int *q = a+3;if (p == q) {return 174;} "
              "else {return 1;}}");
  test_assert(174, test285(),
              "int "
              "test285"
              "(){int a[5];int *p = a;int *q = a+3;if (p != q) {return 174;} "
              "else {return 1;}}");

  test_assert(174, test264(),
              "int "
              "test264"
              "(void){int a[5]; a[3] = 174; int *p = a; p += 3; return *p;}");
  test_assert(
      174, test265(),
      "int a[10][10]; int foo(int p[10][10]){int q;q = ((p+=1)-1)[0][0]; "
      "return q+p[0][0];} int "
      "test265"
      "(){a[0][0] = 100; a[1][0] = 74; return foo(a);}");
  test_assert(
      174, test266(),
      "int "
      "test266"
      "(void){int a[5]; a[1] = 174; int *p = a + 3; p -= 2; return *p;}");
  test_assert(
      74, test267(),
      "int "
      "test267"
      "(void){char a[5]; a[1] = 74; char *p = a + 3; p -= 2; return *p;}");
  test_assert(
      174, test268(),
      "int a[10][10]; int foo(int p[10][10]){int q;q = p++[0][0]; return "
      "q+p[0][0];} int "
      "test268"
      "(){a[0][0] = 100; a[1][0] = 74; return foo(a);}");
  test_assert(174, test269(),
              "int "
              "test269"
              "(void){int a[5]; a[3] = 174; int *p = a + 2; p++; return *p;}");
  test_assert(174, test270(),
              "int "
              "test270"
              "(void){int a[5]; a[3] = 174; int *p = a + 2; ++p; return *p;}");
  test_assert(174, test271(),
              "int "
              "test271"
              "(void){int a[5]; a[3] = 174; int *p = a + 2; return *++p;}");
  test_assert(174, test272(),
              "int "
              "test272"
              "(void){int a[5]; a[3] = 174; int *p = a + 3; return *p++;}");
  test_assert(74, test273(),
              "int "
              "test273"
              "(void){char a[5]; a[1] = 74; char *p = a + 2; return *--p;}");

  test_assert(174, test260(),
              "int printf(); int puts(); int "
              "test260"
              "(){printf(\"H\" \"e\" \"l\" \"lo,"
              " \"W\" \"or\" \"ld!\"); puts(\""
              "); return 174;}");

  test_assert(174, test261(),
              "int "
              "test261"
              "(void){int a = 5; return 174;}");
  test_assert(174, test262(),
              "int "
              "test262"
              "(void){int u = 0; for(int a = 0; a < 10; a++){ u += a; } return "
              "174+u-45;}");

  test_assert(174, test252(),
              "int "
              "test252"
              "(void){int a = 5; int *p = &a; return 174;}");
  test_assert(174, test253(),
              "int "
              "test253"
              "(void){int a = 4; int *p = &a; *p += 170; return a;}");
  test_assert(
      174, test254(),
      "int "
      "test254"
      "(){int a; int *p = &a; *p = 2; int *q = &*p; *q = 174; return a;}");
  test_assert(
      174, test255(),
      "int "
      "test255"
      "(){int a; int *p = &a; *p = 2; int *q = &(*p); *q = 174; return a;}");
  test_assert(174, test256(),
              "int "
              "test256"
              "(){int x = 86;int *y = &x; return (*y) + x + 2;}");
  test_assert(174, test257(),
              "int "
              "test257"
              "(){int x = 86;int *y = &x; return (*y) + (*y) + 2;}");
  test_assert(
      174, test258(),
      "int "
      "test258"
      "(){int x = 86;int *y = &x;int **z = &y;return (*y) + (**z) + 2;}");
  test_assert(174, test259(),
              "int "
              "test259"
              "(){int x = 86;int *y = &x;int **z = &y;return*y+**z+2;}");

  test_assert(174, test247(),
              "int (*func(int (*a)[5]))[5]{return a;} int "
              "test247"
              "(){int a[6][5]; a[1][2] = 174; return func(a)[1][2];}");
  test_assert(174, test248(),
              "struct A {int a;};int "
              "test248"
              "(){const struct A *a; return 174;}");
  test_assert(174, test249(),
              "struct A {int a;};int "
              "test249"
              "(){const struct A const *const a; return 174;}");
  test_assert(174, test250(),
              "struct A {int a;};int f(int *const b){return 0;}int "
              "test250"
              "(){const struct A const *const a; return 174;}");
  test_assert(174, test251(),
              "struct A {int a;};const int f(const int *const b){return 0;}int "
              "test251"
              "(){const struct A const *const a; return 174;}");

  test_assert(174, test239(),
              "int *foo(int *(p)){*p = 4;return p;} int "
              "test239"
              "(){int (x);int (y); int (*(*(z))); *foo(&x) += 170;return x;}");
  test_assert(174, test240(),
              "int "
              "test240"
              "(){int a[1][2];int (*p)[2];p = a;int *q;q = *p;return 174;}");
  test_assert(
      174, test241(),
      "int "
      "test241"
      "(){int a[1][2];int (*p)[2];p = a;int *q;q = *p; *q=174; return **a;}");
  test_assert(174, test242(),
              "int "
              "test242"
              "(){int a[74][2];int (*p)[2];p = a;int *q;q = *(p+1); *q=174; "
              "return **(a+1);}");
  test_assert(174, test243(),
              "int "
              "test243"
              "(){int a[5][6];int (*p)[6];p = a;int *q;q = *(p+1); *(2+q)=174; "
              "return *(*(1+a)+2);}");
  test_assert(
      92, test244(),
      "int changeBoard(int (*board)[30], int i, int j, int d, int N){int k;for "
      "(k = 0; k < N; k++) {*(*(board + i) + k) += d;*(*(board + k) + j) += "
      "d;}if (i > j) {for (k = 0; k < N - (i - j); k++) {*(*(board + k + (i - "
      "j)) + k) += d;}} else {for (k = 0; k < N - (j - i); k++) {*(*(board + "
      "k) "
      "+ k + (j - i)) += d;}}if (i + j < N) {for (k = 0; k <= i + j; k++) "
      "{*(*(board + i + j - k) + k) += d;}} else {for (k = i + j - N + 1; k < "
      "N; "
      "k++) {*(*(board + i + j - k) + k) += d;}}return 0;}int setQueen(int "
      "(*board)[30], int num_placed, int *ptr_sol_num, int N){int j;if "
      "(num_placed == N) {(*ptr_sol_num)+=1;return 0;}for (j = 0; j < N; j++) "
      "{if (*(*(board+num_placed)+j) == 0) {changeBoard(board, num_placed, j, "
      "+1, N);setQueen(board, num_placed + 1, ptr_sol_num, "
      "N);changeBoard(board, "
      "num_placed, j, -1, N);}}return 0;}int board_[30][30];int "
      "test244"
      "(){int sol_num;sol_num = 0;setQueen(board_, 0, &sol_num, 8);return "
      "sol_num;}");
  test_assert(
      92, test245(),
      "int changeBoard(int (*board)[30], int i, int j, int d, int N){int k;for "
      "(k = 0; k < N; k++) {board[i][k] += d;board[k][j] += d;}if (i > j) {for "
      "(k = 0; k < N - (i - j); k++) {board [k + (i - j)][k] += d;}} else {for "
      "(k = 0; k < N - (j - i); k++) {board[k][k + (j - i)] += d;}}if (i + j < "
      "N) {for (k = 0; k <= i + j; k++) {board[i + j - k][k] += d;}} else {for "
      "(k = i + j - N + 1; k < N; k++) {board[i + j - k][k] += d;}}return "
      "0;}int "
      "setQueen(int (*board)[30], int num_placed, int *ptr_sol_num, int N){int "
      "j;if (num_placed == N) {(*ptr_sol_num)+=1;return 0;}for (j = 0; j < N; "
      "j++) {if (board[num_placed][j] == 0) {changeBoard(board, num_placed, j, "
      "+1, N);setQueen(board, num_placed + 1, ptr_sol_num, "
      "N);changeBoard(board, "
      "num_placed, j, -1, N);}}return 0;}int board_[30][30];int "
      "test245"
      "(){int sol_num;sol_num = 0;setQueen(board_, 0, &sol_num, 8);return "
      "sol_num;}");
  test_assert(174, test246(),
              "int "
              "test246"
              "(){int a[5][6];int (*p)[6];p = a;int *q;q = p[1]; 2[q]=174; "
              "return 1[a][2];}");

  test_assert(4, test190(),
              "int "
              "test190"
              "(){return sizeof(int);}");
  test_assert(8, test191(),
              "int "
              "test191"
              "(){return sizeof(int*);}");
  test_assert(8, test192(),
              "struct A{int a; int b;}; int "
              "test192"
              "(){ return sizeof(struct A);}");
  test_assert(12, test193(),
              "struct A{int a; char c; char d; int b;}; int "
              "test193"
              "(){ return sizeof(struct A);}");
  test_assert(120, test194(),
              "struct A{int a; int *b; int c;}; int "
              "test194"
              "(){return sizeof(struct A [5]);}");
  test_assert(
      128, test195(),
      "struct A{int a; int *b; int c;}; struct B{char d; struct A e;}; int "
      "test195"
      "(){return sizeof(struct B [4]);}");
  test_assert(174, test196(),
              "int *f(int *p){return p;} int main(){int a[4]; a[0] = 1; "
              "f(a)[0]++; f(a)[1] = 172; return a[1]+a[0];}");
  test_assert(74, test197(),
              "struct A{char a; int b;}; int "
              "test197"
              "(){struct A a; a.a = 74; return a.a;}");
  test_assert(174, test198(),
              "struct A{int a; int b;}; int "
              "test198"
              "(){struct A a; a.a = 174; return a.a;}");
  test_assert(174, test199(),
              "struct A{int a; int b;}; int "
              "test199"
              "(){struct A a; a.a = 174; return a.a;}");

  test_assert(0, test169(),
              "int a(int b){ return b; }int "
              "test169"
              "(){int i; i=1; a(i == 1? 1 : 2); return 0;}");
  test_assert(
      0, test170(),
      "int a(int b){ return b; }int "
      "test170"
      "(){int i; for (i = 1; i < 11; i++) { a(i == 1? 1 : 2); } return 0;}");
  test_assert(174, test171(),
              "int a(int b){ return b; }int "
              "test171"
              "(){int i; i=1; return a(i == 1? 174 : 2);}");

  test_assert(
      0, test172(),
      "int printf();int puts();int count;int "
      "test172"
      "(){int i; int hist[20]; for (i = 1; i < 11; i++) { printf(i == 1? \"a"
      " : \"b\"); puts(\""
      ");} return 0;}");
  test_assert(0, test173(),
              "int printf();int puts();int count;int "
              "test173"
              "(){int i; int hist[20]; for (i = 1; i < 11; i++) { printf(\"%"
              "s\", (i == 1? \"a\" : \"b\")); puts("
              ");} return 0;}");
  test_assert(0, test174(),
              "int printf();int puts();int count;int "
              "test174"
              "(){int i; int hist[20]; for (i = 1; i < 11; i++) { printf(\"%"
              "d %s : %d\", i, (i == 1? : ), i); puts();} return 0;}");
  test_assert(
      0, test175(),
      "int printf();int puts();int count;int solve(int n, int col, int "
      "*hist){if (col == n) {count+=1;return 0;}int i;int j;for (i = 0, "
      "j = 0; i < n; i++) {for (j = 0; j < col && hist [j] != i && (hist "
      "[j] - i) != col - j && (hist[j] - i) != j - col; j++){}if (j < "
      "col)continue;hist[col] = i;solve(n, col + 1, hist);}return 0;}int "
      "test175"
      "(){int i; int hist[20]; for (i = 1; i < 11; i++) { count=0; ");
  test_assert(174, test176(),
              "int "
              "test176"
              "(){int a; int *p; p = &a; *p = 2; int *q; q = &*p; *q = 174; "
              "return a;}");
  test_assert(174, test177(),
              "int "
              "test177"
              "(){int a; int *p; p = &a; *p = 2; int *q; q = &(*p); *q = 174; "
              "return a;}");
  test_assert(174, test178(),
              "char foo(char *p){char a; return a;} int "
              "test178"
              "(){char q; foo(&(q)); return 174;}");
  test_assert(174, test179(),
              "char; char     ; char; int; int ; int; int;int;char foo(char "
              "*p){char a; return a;} int "
              "test179"
              "(){char q; foo(&(q)); return 174;}");
  test_assert(174, test180(),
              " struct A; char; char     ; char; int; int ; int; struct B;  "
              "int;int;  struct C; int "
              "test180"
              "(){return 174;}");
  test_assert(
      174, test181(),
      " struct A{int a; int b;}; char; char     ; char; int; int ; int; "
      "struct B{int c; int b;};  int;int;  struct C; int "
      "test181"
      "(){return 174;}");
  test_assert(174, test182(),
              "int "
              "test182"
              "(){ int; return 174;}");
  test_assert(174, test183(),
              "struct A{int a; int b;}; int "
              "test183"
              "(){ struct A; return 174;}");
  test_assert(174, test184(),
              "struct A{int a; int b;}; int "
              "test184"
              "(){ struct A a; return 174;}");
  test_assert(174, test185(),
              "struct A{int a; int b;}; int "
              "test185"
              "(){ struct A a[10]; return 174;}");
  test_assert(174, test186(),
              "struct A{int a; int b;};  struct A a[10]; int "
              "test186"
              "(){return 174;}");

  test_assert(0, test162(),
              "int printf();int a() {return 3;}int "
              "test162"
              "() {int i; printf(");
  test_assert(
      0, test163(),
      "int foo(char *a, int b, int c){return 0;} int a(int N) {return 3;}int "
      "test163"
      "() {int i; foo(");
  test_assert(0, test165(),
              "int printf();int a(int N) {return 3;}int "
              "test165"
              "() {int i; printf(");
  test_assert(0, test166(),
              "int printf();int puts();int a(int N) {return 3;}int "
              "test166"
              "() {int i; for (i = 1; i <= 12; i++) { printf(");
  test_assert(0, test167(),
              "int printf();int puts();int A[200][200];int a(int row, int N) "
              "{return 3;}int "
              "test167"
              "() {int i; for (i = 1; i <= 12; i++) { printf(");
  test_assert(
      0, test168(),
      "int printf();int puts();int A[200][200];int dfs(int row, int N) { if "
      "(row "
      "== N) return 1; int ret;ret = 0; int col;for (col = 0; col < N; col++) "
      "{ "
      "int ok; ok = 1; int i; for (i = 1; i < N; i++) { if (row - i >= 0 && "
      "col "
      "- i >= 0) { ok = ok && A[row - i][col - i] == 0; } if (row - i >= 0) { "
      "ok "
      "= ok && A[row - i][col] == 0; } if (row - i >= 0 && col + i < N) { ok = "
      "ok && A[row - i][col + i] == 0; } } if (ok) { A[row][col] = 1; ret += "
      "dfs(row + 1, N); A[row][col] = 0; } } return ret;}int "
      "test168"
      "() {int i; for (i = 1; i < 12; i++) { printf(");
  test_assert(174, test133(),
              "char foo(){char a; return a;} int "
              "test133"
              "(){foo(); return 174;}");
  test_assert(174, test134(),
              "char foo(char *p){char a; return a;} int "
              "test134"
              "(){char q; foo(&q); return 174;}");
  test_assert(174, test135(),
              "char foo(char *p){char a; a = 5; return a;} int "
              "test135"
              "(){char q; foo(&q); return 174;}");
  test_assert(174, test136(),
              "int "
              "test136"
              "(){char x[3]; x[0] = -1; x[1] = 2; int y; y = 4; return x[0] + "
              "y + 171;}");
  test_assert(174, test137(),
              "char foo(char *p){*p = 5; char a;a = 3; return a;} int "
              "test137"
              "(){char q; char r; r = foo(&q); return 172-r+q;}");
  test_assert(174, test138(),
              "char a;char foo(char *p){*p = 5; a = 3; return a;} int "
              "test138"
              "(){char q; char r; r = foo(&q); return 172-r+q;}");
  test_assert(174, test139(),
              "int foo(char a){int d;d = 3;char c;c = a+d;return c;} int "
              "test139"
              "(){char f;f=3;return foo(f)*4+150;}");
  test_assert(174, test140(),
              "int foo(char a){int d;d = 3;char c;c = a+d;return c*4;} int "
              "test140"
              "(){char f;f=3;return foo(f)+150;}");

  test_assert(174, test143(),
              "int foo(char a, char b){return 23;} int "
              "test143"
              "(){char f;f=3;return foo(f,4)+151;}");
  test_assert(174, test144(),
              "int foo(char a, char b){return a*4+11;} int "
              "test144"
              "(){char f;f=3;return foo(f,4)+151;}");
  test_assert(174, test145(),
              "int foo(char a, char b){return a*4+12;} int "
              "test145"
              "(){char f;f=3;return foo(f,4)+150;}");
  test_assert(174, test146(),
              "int foo(char a, char b){return (a+3)*4;} int "
              "test146"
              "(){char f;f=3;return foo(f,4)+150;}");
  test_assert(174, test147(),
              "int foo(char a, char b){char c;c = a+3;return c*4;} int "
              "test147"
              "(){char f;f=3;return foo(f,4)+150;}");
  test_assert(
      174, test148(),
      "int foo(char a, char b){int d;d = 3;char c;c = a+d;return c*4;} int "
      "test148"
      "(){char f;f=3;return foo(f,4)+150;}");
  test_assert(
      174, test149(),
      "int foo(char a, char b){int d;d = 3;char c;c = a+d;return c*b;} int "
      "test149"
      "(){char f;f=3;return foo(f,4)+150;}");
  test_assert(174, test150(),
              "char foo() { char *x;x = \"1ab\"; return x[0]; }int ");
  test_assert(174, test151(),
              "int printf();int "
              "test151"
              "(){printf(");
  test_assert(0, test152(),
              "int printf();int puts();int A[200][200];int "
              "test152");
  test_assert(0, test153(),
              "int printf();int puts();int a(int b, int c) {return 3;}int "
              "test153");
  test_assert(
      0, test154(),
      "int printf();int puts();int A[200][200];int dfs(int row, int N) { if "
      "(row "
      "== N) return 1; int ret;ret = 0; int col;for (col = 0; col < N; col++) "
      "{ "
      "int ok; ok = 1; int i; for (i = 1; i < N; i++) { if (row - i >= 0 && "
      "col "
      "- i >= 0) { ok = ok && A[row - i][col - i] == 0; } if (row - i >= 0) { "
      "ok "
      "= ok && A[row - i][col] == 0; } if (row - i >= 0 && col + i < N) { ok = "
      "ok && A[row - i][col + i] == 0; } } if (ok) { A[row][col] = 1; ret += "
      "dfs(row + 1, N); A[row][col] = 0; } } return ret;}int "
      "test154");
  test_assert(
      0, test155(),
      "int printf();int puts();int count;int solve(int n, int col, int "
      "*hist){if (col == n) {count+=1;return 0;}int i;int j;for (i = 0, "
      "j = 0; i < n; i++) {for (j = 0; j < col && hist [j] != i && (hist "
      "[j] - i) != col - j && (hist[j] - i) != j - col; j++){}if (j < "
      "col)continue;hist[col] = i;solve(n, col + 1, hist);}return 0;}int "
      "test155"
      "(){int i; int hist[20]; for (i = 2; i < 11; i++) { count=0; ");
  test_assert(123, test156(),
              "int "
              "test156"
              "(){/**/return 123;}");
  test_assert(123, test157(),
              "int "
              "test157"
              "(){/*u89g3wihu-@w3erolk*/ return (123);}");
  test_assert(123, test158(),
              "int/*/* 0^[o;:._/-*/"
              "test158"
              "(){return ((((123))));}");
  test_assert(174, test159(),
              "int a; int "
              "test159"
              "(){int *p; p = &a; int i; for(i=0;i<174;i++){++*p;} return a;}");
  test_assert(
      174, test160(),
      "int a; int "
      "test160"
      "(){int *p; p = &a; int i; for(i=0;i<174;((i))++){++*p;} return a;}");
  test_assert(
      174, test161(),
      "int "
      "test161"
      "(){int a[10]; a[5] = 173; int b; b = a[5]++; return a[5]*!(a[5]-b-1);}");

  test_assert(174, test118(),
              "int "
              "test118"
              "(){int a[1]; int *p; p = a; *p=2; return 174;}");
  test_assert(174, test119(),
              "int "
              "test119"
              "(){int a[1]; *(a+0)=2;return 174;}");
  test_assert(174, test120(),
              "int x; int *y; int "
              "test120"
              "(){x=3; int a[1]; *a=2; y=a; return x+*y+169;}");
  test_assert(174, test121(),
              "int x; int "
              "test121"
              "(){x=3; int *y; y=&x; return *y+171;}");
  test_assert(174, test122(),
              "int a[1]; int "
              "test122"
              "(){ *a=2;return 174;}");
  test_assert(174, test123(),
              "int "
              "test123"
              "(){int a[1][2];int *q;q = *a;return 174;}");
  test_assert(174, test124(),
              "int "
              "test124"
              "(){int a[1][2];int *q;q = *a; *q=174; return **a;}");
  test_assert(174, test125(),
              "int "
              "test125"
              "(){int a[86][2];int *q;q = *(a+1); *q=174; return **(a+1);}");

  test_assert(
      174, test126(),
      "int "
      "test126"
      "(){int a[5][6];int *q;q = *(a+1); *(2+q)=174; return *(*(1+a)+2);}");
  test_assert(
      92, test127(),
      "int changeBoard(int board[30][30], int i, int j, int d, int N){int "
      "k;for "
      "(k = 0; k < N; k++) {*(*(board + i) + k) += d;*(*(board + k) + j) += "
      "d;}if (i > j) {for (k = 0; k < N - (i - j); k++) {*(*(board + k + (i - "
      "j)) + k) += d;}} else {for (k = 0; k < N - (j - i); k++) {*(*(board + "
      "k) "
      "+ k + (j - i)) += d;}}if (i + j < N) {for (k = 0; k <= i + j; k++) "
      "{*(*(board + i + j - k) + k) += d;}} else {for (k = i + j - N + 1; k < "
      "N; "
      "k++) {*(*(board + i + j - k) + k) += d;}}return 0;}int setQueen(int "
      "board[30][30], int num_placed, int *ptr_sol_num, int N){int j;if "
      "(num_placed == N) {(*ptr_sol_num)+=1;return 0;}for (j = 0; j < N; j++) "
      "{if (*(*(board+num_placed)+j) == 0) {changeBoard(board, num_placed, j, "
      "+1, N);setQueen(board, num_placed + 1, ptr_sol_num, "
      "N);changeBoard(board, "
      "num_placed, j, -1, N);}}return 0;}int board_[30][30];int "
      "test127"
      "(){int sol_num;sol_num = 0;setQueen(board_, 0, &sol_num, 8);return "
      "sol_num;}");
  test_assert(92, test128(),
              "int count;int solve(int n, int col, int *hist){if (col == n) "
              "{count+=1;return 0;}int i;int j;for (i = 0, j = 0; i < n; i++) "
              "{for (j = "
              "0; j < col && *(hist + j) != i && (*(hist + j) - i) != col - j "
              "&& (*(hist "
              "+ j) - i) != j - col; j++){}if (j < col)continue;*(hist+col) = "
              "i;solve(n, "
              "col + 1, hist);}return 0;}int "
              "test128"
              "(){int hist[8];solve(8, 0, hist);return count;}");
  test_assert(
      92, test129(),
      "int count;int solve(int n, int col, int *hist){if (col == n) "
      "{count+=1;return 0;}int i;int j;for (i = 0, j = 0; i < n; i++) {for (j "
      "= "
      "0; j < col && *(hist + j) != i && (hist [j] - i) != col - j && (*(hist "
      "+ "
      "j) - i) != j - col; j++){}if (j < col)continue;*(hist+col) = i;solve(n, "
      "col + 1, hist);}return 0;}int "
      "test129"
      "(){int hist[8];solve(8, 0, hist);return count;}");
  test_assert(
      92, test130(),
      "int count;int solve(int n, int col, int *hist){if (col == n) "
      "{count+=1;return 0;}int i;int j;for (i = 0, j = 0; i < n; i++) "
      "{for (j = 0; j < col && hist [j] != i && (hist [j] - i) != col - "
      "j && (hist[j] - i) != j - col; j++){}if (j < "
      "col)continue;hist[col] = i;solve(n, col + 1, hist);}return 0;}int "
      "test130"
      "(){int hist[8];solve(8, 0, hist);return count;}");
  test_assert(
      92, test131(),
      "int changeBoard(int board[30][30], int i, int j, int d, int N){int "
      "k;for "
      "(k = 0; k < N; k++) {board[i][k] += d;board[k][j] += d;}if (i > j) {for "
      "(k = 0; k < N - (i - j); k++) {board [k + (i - j)][k] += d;}} else {for "
      "(k = 0; k < N - (j - i); k++) {board[k][k + (j - i)] += d;}}if (i + j < "
      "N) {for (k = 0; k <= i + j; k++) {board[i + j - k][k] += d;}} else {for "
      "(k = i + j - N + 1; k < N; k++) {board[i + j - k][k] += d;}}return "
      "0;}int "
      "setQueen(int board[30][30], int num_placed, int *ptr_sol_num, int "
      "N){int "
      "j;if (num_placed == N) {(*ptr_sol_num)+=1;return 0;}for (j = 0; j < N; "
      "j++) {if (board[num_placed][j] == 0) {changeBoard(board, num_placed, j, "
      "+1, N);setQueen(board, num_placed + 1, ptr_sol_num, "
      "N);changeBoard(board, "
      "num_placed, j, -1, N);}}return 0;}int board_[30][30];int "
      "test131"
      "(){int sol_num;sol_num = 0;setQueen(board_, 0, &sol_num, 8);return "
      "sol_num;}");
  test_assert(174, test132(),
              "int "
              "test132"
              "(){int a[5][6];int *q;q = a[1]; 2[q]=174; return 1[a][2];}");

  test_assert(123, test001(),
              "int "
              "test001"
              "(){return 123;}");
  test_assert(123, test002(),
              "int "
              "test002"
              "(){return (123);}");
  test_assert(123, test003(),
              "int "
              "test003"
              "(){return ((((123))));}");
  test_assert(174, test004(),
              "int "
              "test004"
              "(){return 123+51;}");
  test_assert(174, test005(),
              "int "
              "test005"
              "(){return 123+56-5;}");
  test_assert(174, test006(),
              "int "
              "test006"
              "(){return 175-(4-3);}");
  test_assert(174, test007(),
              "int "
              "test007"
              "(){return 181-4-3;}");
  test_assert(174, test008(),
              "int "
              "test008"
              "(){return 0x29*3+7*8-5*1;}");
  test_assert(55, test009(),
              "int "
              "test009"
              "(){return 6*(3+7)-5*1;}");
  test_assert(55, test010(),
              "int "
              "test010"
              "(){return 43,6*(3+7)-5*1;}");
  test_assert(55, test011(),
              "int "
              "test011"
              "(){return 43,6*(3+(4|3))-(5|1)*1;}");
  test_assert(174, test012(),
              "int "
              "test012"
              "(){return 043,41*3+07*010-0Xa/(010%(1+2));}");
  test_assert(174, test013(),
              "int "
              "test013"
              "(){return 7*5,(12,41*3)+7*16/(9,2)-10/(8%3);}");
  test_assert(174, test014(),
              "int main(){return 7*5 	,	(0xC,(41   "
              ")*(4-(011>8)))+7*(((1+2)>=3)<<4)/(9,(4>>(10<=10))+(3<3))-10/"
              "(	  ( 	1  <<3)	%3);}");
  test_assert(174, test015(),
              "int main(){return 35,	((	41|	(8   !=     15))*  "
              "((3==3)+2))+((5|2)*(9&10))   -   (10/(8%3));}");
  test_assert(174, test016(),
              "int "
              "test016"
              "(){return 043,41*3+07*010-0Xa/(010%(!!1+2));}");
  test_assert(174, test017(),
              "int "
              "test017"
              "(){return 7*5 	,	(0xC,(41   "
              ")*(4-(011>8)))+7*(((1-~1)>=3)<<4)/(9,(4>>(10<=10))+(3<3))-10/"
              "(	  ( 	!0  <<3)	%3);}");
  test_assert(174, test018(),
              "int "
              "test018"
              "(){return +174;}");
  test_assert(174, test019(),
              "int "
              "test019"
              "(){return -(1-175);}");
  test_assert(174, test020(),
              "int "
              "test020"
              "(){23; 45+37; ((12-1)*75); return -(1-175);}");
  test_assert(174, test021(),
              "int "
              "test021"
              "(){23; 45+37; return -(1-175); ((12-1)*75);}");
  test_assert(
      174, test022(),
      "int "
      "test022"
      "(){int a; int b; return (a = b = 9, a = 41*3, 55 - (b = 4) + a);}");
  test_assert(
      174, test023(),
      "int main(){int a; int b; int c; int d; int _q432; a = b = c = 9; d = 5; "
      "a = 41*3; return (c, _q432 = 8, d = 11*5) - (b = 4) + a;}");
  test_assert(174, test024(),
              "int "
              "test024"
              "(){return 175^1;}");
  test_assert(174, test025(),
              "int "
              "test025"
              "(){return 2 + (1? 100 + 72 : 17);}");
  test_assert(174, test026(),
              "int "
              "test026"
              "(){return (0? 234 : 2) + (1? 100 + 72 : 17);}");

  test_assert(174, test032(),
              "int "
              "test032"
              "(){3; {5; 7; 11; } return 175^1;}");

  test_assert(174, test034(),
              "int add_(int x, int y){4; return x+y;} int "
              "test034"
              "(){3; return add_(87,87);}");
  test_assert(
      55, test035(),
      "int fib(int n){ return n < 2? n : fib(n - 1) + fib(n - 2); } int "
      "test035"
      "(){3; return fib(10);}");
  test_assert(
      12, test036(),
      "int tarai(int x,int y,int z){ return x <= y? y : tarai(tarai(x-1, "
      "y, z), tarai(y-1, z, x), tarai(z-1, x, y)); } int "
      "test036"
      "(){return tarai(12,6,0);}");
  test_assert(174, test037(),
              "int "
              "test037"
              "() { return (3 && 2 && 5) + 173; }");
  test_assert(
      174, test038(),
      "int "
      "test038"
      "() { return (3 && 2) + !(3 && 0) + !(0 && 3)+ !(0 && 0) + 170; }");
  test_assert(174, test039(),
              "int "
              "test039"
              "() { return (3 || 2 || 5) + 173; }");
  test_assert(174, test040(),
              "int "
              "test040"
              "() { return (3 || 2) + (3 || 0) + (0 || 3)+ !(0 || 0) + 170; }");
  test_assert(174, test041(),
              "int "
              "test041"
              "() {int a; a = 3; a += 5;  return a + 166; }");
  test_assert(174, test042(),
              "int "
              "test042"
              "() {int a; int b; a = 3; b = (a += 5);  return a + b + 158; }");
  test_assert(
      174, test043(),
      "int "
      "test043"
      "() {int a; int b; a = 3; b = 1; b *= (a += 5);  return a + b + 158; }");
  test_assert(174, test044(),
              "int "
              "test044"
              "() {int a; int b; a = 11; a -=5; a /= 2; b = 1; b *= (a += 5);  "
              "return a + b + 158; }");
  test_assert(174, test045(),
              "int "
              "test045"
              "() {int a; int b; int c; a = 7; a &= ~2; a <<= 2; a |=2; a >>= "
              "1; a -=5; "
              "a /= 2; b = 3; c = 8; b ^= (c%=3); b *= (a += 5);  return a + b "
              "+ 158; }");
  test_assert(174, test046(),
              "int foo(){ return 2;} int "
              "test046"
              "() {int a; int b; int c; a = 3;b = 5;c = 2;if(a) {b = foo();} "
              "else { }    return 172+b;}");
  test_assert(174, test047(),
              "int foo(){ return 2;} int "
              "test047"
              "() {int a; int b; int c; a = 3;b = 5;c = 2;if(a) {b = foo();}   "
              "return 172+b;}");
  test_assert(174, test048(),
              "int foo(){ return 2;} int bar(){ return 7;} int "
              "test048"
              "() {int a; int b; int c; a = 3;b = 5;c = 2;if(a) {b = foo();} "
              "else { c = bar();}    return 172+b;}");
  test_assert(174, test049(),
              "int foo(){ return 2;} int bar(){ return 7;} int "
              "test049"
              "() {int a; int b; int c; a = 0;b = 5;c = 2;if(a) {b = foo();} "
              "else { c = bar();}    return 162+b+c;}");
  test_assert(174, test050(),
              "int foo(){ return 2;} int bar(){ return 7;} int "
              "test050"
              "() {int a; int b; int c; a = 3;b = 5;c = 2;if(a) if(0) { b = "
              "foo(); } else {  c = bar(); }    return 162+b+c;}");
  test_assert(174, test051(),
              "int foo(){ return 2;} int bar(){ return 7;} int "
              "test051"
              "() {int a; int b; int c; a = 3;b = 5;c = 2;if(a) "
              "if(0)b=foo();else c = bar();return 162+b+c;}");
  test_assert(
      174, test052(),
      "int "
      "test052"
      "() {int a; a = 4; if(1){return 170+a; a = 7; }else{return 170-a; "
      "a = 9;} a = 5; return a;}");
  test_assert(174, test053(),
              "int foo(){return 1;} int "
              "test053"
              "(){int a; a=0;do{a=3;}while(a==foo());return 174;}");
  test_assert(174, test054(),
              "int "
              "test054"
              "(){int a; a=0;do{a+=1;}while(a && a < 174);return a;}");
  test_assert(174, test055(),
              "int "
              "test055"
              "(){int a; a=-8;do{a+=1;}while(a);return a+174;}");
  test_assert(174, test056(),
              "int foo(){return 3;} int "
              "test056"
              "() {int a; a = 0;while(a == foo()) {a = 3;}return 174;}");
  test_assert(55, test057(),
              "int "
              "test057"
              "(){int a; int b; a = 0; b = 0; while(a <= 10) {b += a; a += "
              "1;}return b;}");
  test_assert(174, test058(),
              "int "
              "test058"
              "(){int a; a = 3;while (a) {a = 2;if (a - 3) {break;}a += "
              "3;}return 174;}");
  test_assert(
      174, test059(),
      "int "
      "test059"
      "(){int a; int b; int c; a = 3; b = 5; c = 0;while(a){while(b) {c "
      "+= b;b-=1;if(b == 3) break;}b = 7;a-=1;if(a == 1) break;} return "
      "a*7+b*15+c*2;}");
  test_assert(174, test060(),
              "int "
              "test060"
              "(){int a; a = 3;while (a) {a = 2;if (a - 3) {break;}a += "
              "3;}return 174;}");
  test_assert(55, test061(),
              "int "
              "test061"
              "(){int a; int b; a=11; b=0; "
              "while(a){a-=1;b+=a;if(a)continue;break; a+=100;} return b;}");
  test_assert(3, test062(),
              "int "
              "test062"
              "(){int a; int b; a =0; b=0; do{a-=1;b+=a;if(a)continue;break; "
              "a+=100;}while(a+3); return -a;}");
  test_assert(6, test063(),
              "int "
              "test063"
              "(){int a; int b; a =0; b=0; do{a-=1;b+=a;if(a)continue;break; "
              "a+=100;}while(a+3); return -b;}");
  test_assert(174, test064(),
              "int "
              "test064"
              "(){int a; int b; a =-3; b=-6; return a*b*10+a+b+3;}");
  test_assert(180, test065(),
              "int "
              "test065"
              "(){int a; int b; a =0; b=0; do{a-=1;b+=a;if(a)continue;break; "
              "a+=100;}while(a+3); return a*b*10;}");
  test_assert(174, test066(),
              "int "
              "test066"
              "(){int a; int b; a =0; b=0; do{a-=1;b+=a;if(a)continue;break; "
              "a+=100;}while(a+3); return a*b*10+a+b+3;}");
  test_assert(18, test067(),
              "int "
              "test067"
              "(){int a; int b; a =0; b=0; do{a-=1;b+=a;if(a)continue;break; "
              "a+=100;}while(a+3); return a*b;}");
  test_assert(18, test068(),
              "int "
              "test068"
              "(){int a; int b; a =0; b=0; do{a-=1;b+=a;if(a)continue;break; "
              "a+=100;}while(a+3); return b*a;}");
  test_assert(180, test069(),
              "int "
              "test069"
              "(){int a; int b; a =0; b=0; do{a-=1;b+=a;if(a)continue;break; "
              "a+=100;}while(a+3); return b*a*10;}");
  test_assert(18, test070(),
              "int "
              "test070"
              "(){int a; int b; a =0; b=0; "
              "do{a-=1;b+=a;if(a)continue;break;}while(a+3); return a*b;}");
  test_assert(
      18, test071(),
      "int "
      "test071"
      "(){int a; int b; a =0; b=0; do{a-=1;b+=a;if(!a)break;}while(a+3); "
      "return a*b;}");
  test_assert(
      18, test072(),
      "int "
      "test072"
      "(){int a; int b; a =0; b=0; do{a-=1;b+=a;}while(a+3); return a*b;}");
  test_assert(
      18, test073(),
      "int "
      "test073"
      "(){int a; int b; a =0; b=0; do{a-=1;b+=a;}while(a+3); return b*a;}");
  test_assert(174, test074(),
              "int "
              "test074"
              "(){int a; int b; a=3; b=0; b+= ++a; return a*b*11-2;}");
  test_assert(174, test075(),
              "int "
              "test075"
              "(){int a; int b; a=3; b=0; b+= a++; return !(b-3)+!(a-4)+172;}");
  test_assert(
      18, test076(),
      "int "
      "test076"
      "(){int a; int b; a =0; b=0; do{b+=--a;}while(a+3); return b*a;}");
  test_assert(174, test077(),
              "int "
              "test077"
              "(){int a; for (a = 3;a;) {a = 2;if (a - 3) {break;}a += "
              "3;}return 174;}");
  test_assert(
      174, test078(),
      "int "
      "test078"
      "(){int a; for (a = 3;;) {a = 2;if (a - 3) {break;}a += 3;}return 174;}");
  test_assert(
      55, test079(),
      "int "
      "test079"
      "(){int a; int b; for(a=0,b=0;a <= 10;) {b += a; a += 1;}return b;}");
  test_assert(
      55, test080(),
      "int "
      "test080"
      "(){int a; int b; for(a=11, b=0;a;){a-=1;b+=a;if(a)continue;break; "
      "a+=100;} return b;}");
  test_assert(55, test081(),
              "int "
              "test081"
              "(){int a; int b; for(a=0,b=0;a <= 10;++a) {b += a;}return b;}");
  test_assert(55, test082(),
              "int "
              "test082"
              "(){int a; int b; for(a=0,b=0;a <= 10;a++) {b += a;}return b;}");
  test_assert(174, test083(),
              "int "
              "test083"
              "(){int a; int b; int c; int d; d=0; b = 5; c = 0;for(a = "
              "3;a;d++){for(;b;++d) {c += b;b-=1;if(b == 3) break;}b = "
              "7;a-=1;if(a == 1) break;} return a*7+b*15+c*2;}");
  test_assert(
      40, test084(),
      "int "
      "test084"
      "(){int a; int b; for(a=0,b=0;a<10;a++){ if(a ==5)continue;b+=a;} "
      "return b;}");
  test_assert(174, test085(),
              "int "
              "test085"
              "(){int a; a = 174; {int a; a = 3;} return a;}");
  test_assert(174, test086(),
              "int "
              "test086"
              "(){int a; a = 3; { a = 174;} return a;}");
  test_assert(174, test087(),
              "int "
              "test087"
              "() {int *b; int a; a = 3; a += 5;  return a + 166; }");
  test_assert(174, test088(),
              "int "
              "test088"
              "() {int *******b; int a; a = 3; a += 5;  return a + 166; }");
  test_assert(174, test089(),
              "int "
              "test089"
              "() {int a; a = 174; int *b; b = &a; return a;}");
  test_assert(174, test090(),
              "int "
              "test090"
              "(){int x;x = 86;int *y;y = &x; return (*y) + x + 2;}");
  test_assert(174, test091(),
              "int "
              "test091"
              "(){int x;x = 86;int *y;y = &x; return (*y) + (*y) + 2;}");
  test_assert(
      174, test092(),
      "int "
      "test092"
      "(){int x;x = 86;int *y;y = &x;int **z;z = &y;return (*y) + (**z) + 2;}");
  test_assert(174, test093(),
              "int "
              "test093"
              "(){int x;x = 86;int *y;y = &x;int **z;z = &y;return*y+**z+2;}");
  test_assert(174, test094(),
              "int "
              "test094"
              "() {int x;int *y;x = 3;y = &x;*y = 174;return x;}");
  test_assert(174, test095(),
              "int "
              "test095"
              "() {int x;int *y;x = 3;y = &x;*y = 171;*y += 3;return x;}");
  test_assert(
      174, test096(),
      "int "
      "test096"
      "(){int x; int y; int *z; int*a; z=&x; a=&y; *z=*a=87; return(x+y);}");
  test_assert(
      174, test097(),
      "int "
      "test097"
      "(){int x; int *y; int **z; z = &y; *z = &x; *y = 174; return x;}");
  test_assert(174, test098(),
              "int foo(int* p){return 3;} int "
              "test098"
              "(){int x; return 174;}");
  test_assert(174, test099(),
              "int foo(int* p){return *p;} int "
              "test099"
              "(){int x; x = 174; return foo(&x);}");
  test_assert(174, test100(),
              "int foo(int* p){*p = 172; return *p+2;} int "
              "test100"
              "(){int x; return foo(&x);}");
  test_assert(174, test101(),
              "int *foo(int *p){*p = 4;return p;} int "
              "test101"
              "(){int x;int *y;y = foo(&x); *y+= 170;return x;}");
  test_assert(174, test102(),
              "int *foo(int *p){*p = 4;return p;} int "
              "test102"
              "(){int x;int y;*foo(&x) += 170;return x;}");

  test_assert(174, test113(),
              "int *foo(int *p){*p = 4;return p;} int "
              "test113"
              "(){int x;int y; int **z; *foo(&x) += 170;return x;}");
  test_assert(174, test114(),
              "int "
              "test114"
              "(){int a[2][3]; return 174;}");
  test_assert(174, test115(),
              "int x; int *y; int "
              "test115"
              "(){return 174;}");
  test_assert(174, test116(),
              "int x; int *y; int "
              "test116"
              "(){return x+174;}");
  test_assert(174, test117(),
              "int x; int *y; int "
              "test117"
              "(){x=3; int a; a=2; y=&a; return x+*y+169;}");
  return 0;
}
