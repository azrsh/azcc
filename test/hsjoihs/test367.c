static int foo(void) { return 3; } static int bar(void) { return 5;} int test367(void) { return (1? foo : bar)(); }
