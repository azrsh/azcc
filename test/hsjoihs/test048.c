static int foo(void){ return 2;} static int bar(void){ return 7;} int test048() {int a; int b; int c; a = 3;b = 5;c = 2;if(a) {b = foo();} else { c = bar();}    return 172+b;}
