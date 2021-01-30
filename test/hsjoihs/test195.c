struct A{int a; int *b; int c;}; struct B{char d; struct A e;}; int test195(){return sizeof(struct B [4]);}
