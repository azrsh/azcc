struct A {int k[15];}; int test350(){struct A s; int (*p)[15] = &s.k; return 35;}
