struct A{char a; int b; char c;}; int test296(){struct A a; a.c = 74; struct A b = a; return b.c;}
