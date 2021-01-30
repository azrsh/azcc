static int foo(void) { return 3; } static int bar(void) { return 5;} int test368(void) { return (0? foo : bar)(); }
