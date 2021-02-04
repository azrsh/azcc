struct A{int a; int b;}; int test295(){struct A a; a.a = 174; struct A b = a; return b.a;}
