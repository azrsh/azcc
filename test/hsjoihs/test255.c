int test255(){int a; int *p = &a; *p = 2; int *q = &(*p); *q = 174; return a;}
