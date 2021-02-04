struct A{char a; int b;}; int test292(){struct A a; a.a = 74; struct A b; b = a; return b.a;}
