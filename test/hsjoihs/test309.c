struct A{int a; int *b; int c;}; struct B{char d; struct A e;}; int test309(){struct A a; a.a = 174; struct B b; b.e.a = 174; a = b.e; return a.a;}
