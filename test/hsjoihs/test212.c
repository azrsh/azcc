struct A{int a; int b;}; int test212(){ struct A *p; void *q1; void *q2; q1 = p; q2 = p+1; char *r1; char *r2; r1 = q1; r2 = q2; return r2-r1;}
