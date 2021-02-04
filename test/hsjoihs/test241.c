int test241(){int a[1][2];int (*p)[2];p = a;int *q;q = *p; *q=174; return **a;}
