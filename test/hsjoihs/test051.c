static int foo(void){ return 2;} static int bar(void){ return 7;} int test051() {int a; int b; int c; a = 3;b = 5;c = 2;if(a) if(0)b=foo();else c = bar();return 162+b+c;}
