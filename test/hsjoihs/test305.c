int test305(void) {int a[5]; a[3] = 174; int (*p)[5] = &a; return (*p)[3];} 
