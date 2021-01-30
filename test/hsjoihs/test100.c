static int foo(int* p){*p = 172; return *p+2;} int test100(){int x; return foo(&x);}
