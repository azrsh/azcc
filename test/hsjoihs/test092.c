int test092(){int x;x = 86;int *y;y = &x;int **z;z = &y;return (*y) + (**z) + 2;}
