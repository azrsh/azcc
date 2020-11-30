 void alloc4(); int main(){ int *a; alloc4(&a, 1, 2, 3, 4); int *p; p = a; return *p + *(p + 1); }
