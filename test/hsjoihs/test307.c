struct A{int a; int *b; int c;}; struct B{char d; struct A e;}; int test307(){struct A a; a.a = 174; struct B b; b.e = a; return b.e.a;}
