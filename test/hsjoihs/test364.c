union A { char a[4]; int b; }; int test364(void) { union A x; x.a[0] = 0x4b; x.a[1] = 0x6f; x.a[2] = 0x72; x.a[3] = 0x79; return x.b - 0x79726f4b; }
