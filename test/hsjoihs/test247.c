int (*func(int (*a)[5]))[5]{return a;} int test247(){int a[6][5]; a[1][2] = 174; return func(a)[1][2];}
