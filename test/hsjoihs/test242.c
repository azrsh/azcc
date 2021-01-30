int test242(){int a[74][2];int (*p)[2];p = a;int *q;q = *(p+1); *q=174; return **(a+1);}
