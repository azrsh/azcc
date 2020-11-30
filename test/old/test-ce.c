void alloc4(); int main(){ int *p; alloc4(&p, 1, 2, 4, 8); p = p + 3; p = p - 2; return *p; }
