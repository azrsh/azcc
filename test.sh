#!/bin/bash
assert() {
    expected="$1"
    input="$2"

    ./9cc "$input" > tmp.s
    cc -o tmp tmp.s
    ./tmp
    actual="$?"

    if [ "$actual" = "$expected" ]; then
        echo "$input => $actual"
    else
        echo "$input => $expected expected, but got $actual"
        exit 1
    fi
}

assert_with_funccall() {
    expected="$1"
    input="$2"

    ./9cc "$input" > tmp.s
    cc -o tmp tmp.s funccalltest.o alloctesthelper.o
    ./tmp
    actual="$?"

    if [ "$actual" = "$expected" ]; then
        echo "$input => $actual"
    else
        echo "$input => $expected expected, but got $actual"
        exit 1
    fi
}

execute_func_test() {
    expected="$1"
    input="$2"

    echo "epected $expected"
    echo "execute $input"
    ./9cc "$input" > tmp.s
    cc -o tmp tmp.s funccalltest.o
    ./tmp
    actual="$?"
    echo "$input => Pass"
}

assert 0 "int main(){0;}"
assert 42 "int main(){42;}"
assert 21 "int main(){5+20-4;}"
assert 41 "int main(){ 12 + 34 - 5 ;}"
assert 47 "int main(){5+6*7;}"
assert 15 "int main(){5*(9-6);}"
assert 4 "int main(){(3+5)/2;}"
assert 10 "int main(){-10+20;}"
assert 2 "int main(){-10/-5;}"
assert 1 "int main(){10 == 10;}"
assert 0 "int main(){10 < 10;}"
assert 1 "int main(){20-10*1+2 <= 2*7;}"
assert 0 "int main(){80-50 > 20*2;}"
assert 1 "int main(){80-50 < 20*2;}"
assert 0 "int main(){80-0 <= 20*2;}"
assert 16 "int main(){int a; int b; a = 5*(9-6);b=a+1;}"
assert 44 "int main(){int a; int b; a=29;b=5*(9-6);a+b;}"
assert 25 "int main(){int a; int b; a=5*(9-6);b=26-3+2;}"
assert 44 "int main(){int asaskjkas; int bakjehjioaj; asaskjkas=29;bakjehjioaj=5*(9-6);asaskjkas+bakjehjioaj;}"
assert 40 "int main(){int a; int b; a=5*(9-6);b=26-3+2;return a + b;}"
assert 15 "int main(){int b; return 5*(9-6);b=26-3+2;}"
assert 15 "int main(){int b; if(1)return 5*(9-6);b=26-3+2;}"
assert 10 "int main(){int b; if(0)return 5*(9-6);b=26-3+2;return 10;}"
assert 15 "int main(){int b; if(0)return 0; else return 5*(9-6);b=26-3+2;}"
assert 15 "int main(){while(0)return 0; return 15;}"
assert 15 "int main(){int a; a = 10;while(a > 0)a = a - 1; return 15;}"
assert 10 "int main(){int a; int b; b = 0;for(a = 0;a < 10;a = a + 1)b = b + 1; return b;}"
assert 20 "int main(){int a; int b; int c; b = 0;c = 0;for(a = 0;a < 10;a = a + 1){b = b + 1;c = c + 2;} return c;}"
assert_with_funccall 0 "int main(){ return foo();}"
assert_with_funccall 13 "int main(){ return foo1(13);}"
assert_with_funccall 25 "int main(){ return foo2(13,12);}"
assert_with_funccall 36 "int main(){ return foo3(13,12,11);}"
assert_with_funccall 70 "int main(){ return foo7(13,12,11,10,9,8,7);}"
assert_with_funccall 76 "int main(){ return foo8(13,12,11,10,9,8,7,6);}"
assert_with_funccall 81 "int main(){ return foo9(13,12,11,10,9,8,7,6,5);}"
assert_with_funccall 85 "int main(){ return foo10(13,12,11,10,9,8,7,6,5,4);}"
assert 1 "int test(int a){ return a; } int main(){ return test(1); }"
assert 3 "int test(int a, int b){ return a + b; } int main(){ return test(1, 2); }"
assert 6 "int test(int a, int b, int c){ return a + b + c; } int main(){ return test(1, 2, 3); }"
assert 10 "int test(int a, int b, int c, int d){ return a + b + c + d; } int main(){ return test(1, 2, 3, 4); }"
assert 15 "int test(int a, int b, int c, int d, int e){ return a + b + c + d + e; } int main(){ return test(1, 2, 3, 4, 5); }"
assert 21 "int test(int a, int b, int c, int d, int e, int f){ return a + b + c + d + e + f; } int main(){ return test(1, 2, 3, 4, 5, 6); }"
assert 28 "int test(int a, int b, int c, int d, int e, int f, int g){ return a + b + c + d + e + f + g; } int main(){ return test(1, 2, 3, 4, 5, 6, 7); }"
assert 36 "int test(int a, int b, int c, int d, int e, int f, int g, int h){ return a + b + c + d + e + f + g + h; } int main(){ return test(1, 2, 3, 4, 5, 6, 7, 8); }"
assert 45 "int test(int a, int b, int c, int d, int e, int f, int g, int h, int i){ return a + b + c + d + e + f + g + h + i; } int main(){ return test(1, 2, 3, 4, 5, 6, 7, 8, 9); }"
assert 55 "int test(int a, int b, int c, int d, int e, int f, int g, int h, int i, int j){ return a + b + c + d + e + f + g + h + i + j; } int main(){ return test(1, 2, 3, 4, 5, 6, 7, 8, 9, 10); }"
assert 6 "int test(int a){ return a; } int main(){ return test(1) + test(2) + test(3); }"
# assert 110 "int test(int a, int b, int c, int d, int e, int f, int g, int h, int i, int j){ return a + b + c + d + e + f + g + h + i + j; } int main(){ return test(1, 2, 3, 4, 5, 6, 7, 8, 9, 10) + test(1, 2, 3, 4, 5, 6, 7, 8, 9, 10); }"
assert 1 "int main(){int a; int *b; a = 1;b = &a; return *b;}"
assert 2 "int main(){int a; int *b; a = 2;b = &a; return *b;}"
assert 3 "int main(){int x; int *y; y = &x; *y = 3; return x;}"
assert_with_funccall 1 "int main(){ int *p; int *q; alloc4(&p, 1, 2, 4, 8); q = p + 0; return *q; }"
assert_with_funccall 2 "int main(){ int *p; int *q; alloc4(&p, 1, 2, 4, 8); q = p + 1; return *q; }"
assert_with_funccall 4 "int main(){ int *p; int *q; alloc4(&p, 1, 2, 4, 8); q = p + 2; return *q; }"
assert_with_funccall 8 "int main(){ int *p; int *q; alloc4(&p, 1, 2, 4, 8); q = p + 3; return *q; }"
assert_with_funccall 1 "int main(){ int *p; alloc4(&p, 1, 2, 4, 8); p = p + 3; p = p - 3; return *p; }"
assert_with_funccall 2 "int main(){ int *p; alloc4(&p, 1, 2, 4, 8); p = p + 3; p = p - 2; return *p; }"
assert_with_funccall 4 "int main(){ int *p; alloc4(&p, 1, 2, 4, 8); p = p + 3; p = p - 1; return *p; }"
assert_with_funccall 8 "int main(){ int *p; alloc4(&p, 1, 2, 4, 8); p = p + 3; p = p - 0; return *p; }"
assert_with_funccall 1 "int main(){ int *p; alloc4(&p, 1, 2, 4, 8); return *(p + 0); }"
assert_with_funccall 2 "int main(){ int *p; alloc4(&p, 1, 2, 4, 8); return *(p + 1); }"
assert_with_funccall 4 "int main(){ int *p; alloc4(&p, 1, 2, 4, 8); return *(p + 2); }"
assert_with_funccall 8 "int main(){ int *p; alloc4(&p, 1, 2, 4, 8); return *(p + 3); }"
assert_with_funccall 1 "int main(){ int *p; alloc4(&p, 1, 2, 4, 8); return *((p + 0) + 0); }"
assert_with_funccall 2 "int main(){ int *p; alloc4(&p, 1, 2, 4, 8); return *((p + 1) + 0); }"
assert_with_funccall 4 "int main(){ int *p; alloc4(&p, 1, 2, 4, 8); return *((p + 1) + 1); }"
assert_with_funccall 8 "int main(){ int *p; alloc4(&p, 1, 2, 4, 8); return *((p + 2) + 1); }"
assert 4 "int main(){ return sizeof(int);}"
assert 8 "int main(){ return sizeof(int*);}"
assert 0 "int main(){ int a[16]; return 0; }"
assert 64 "int main(){ int a[16]; return sizeof(a); }"
assert_with_funccall 3 "int main(){ int *a; alloc4(&a, 1, 2, 3, 4); int *p; p = a; return *p + *(p + 1); }"
assert 1 "int main(){ int a[2]; *a = 1; return *a; }"
assert 2 "int main(){ int a[2]; *(a + 1) = 2; return *(a + 1); }"
assert 3 "int main(){ int a[2]; *a = 1; *(a + 1) = 2; int *p; p = a; return *p + *(p + 1); }"
assert 3 "int main(){ int a[2]; a[0] = 1; a[1] = 2; int *p; p = a; return p[0] + p[1]; }"
assert 3 "int a; int main(){ int a[2]; a[0] = 1; a[1] = 2; int *p; p = a; return p[0] + p[1]; }"
assert 3 "int asdjuwgehij; int main(){ int a[2]; a[0] = 1; a[1] = 2; int *p; p = a; return p[0] + p[1]; }"
assert 1 "int a; int main(){ a = 1; return a; }"
assert 3 "int a; int b; int main(){ a = 1; b = 2; return a + b; }"

echo OK

