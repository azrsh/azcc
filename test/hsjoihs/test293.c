struct A{char a; int b;}; int test293(){struct A a; a.a = 74; struct A b = a; return b.a;}
