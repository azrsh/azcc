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
    cc -o tmp tmp.s funccalltest.o
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

assert 0 "main(){0;}"
assert 42 "main(){42;}"
assert 21 "main(){5+20-4;}"
assert 41 "main(){ 12 + 34 - 5 ;}"
assert 47 "main(){5+6*7;}"
assert 15 "main(){5*(9-6);}"
assert 4 "main(){(3+5)/2;}"
assert 10 "main(){-10+20;}"
assert 2 "main(){-10/-5;}"
assert 1 "main(){10 == 10;}"
assert 0 "main(){10 < 10;}"
assert 1 "main(){20-10*1+2 <= 2*7;}"
assert 0 "main(){80-50 > 20*2;}"
assert 1 "main(){80-50 < 20*2;}"
assert 0 "main(){80-0 <= 20*2;}"
assert 16 "main(){a = 5*(9-6);b=a+1;}"
assert 44 "main(){a=29;b=5*(9-6);a+b;}"
assert 25 "main(){a=5*(9-6);b=26-3+2;}"
assert 44 "main(){asaskjkas=29;bakjehjioaj=5*(9-6);asaskjkas+bakjehjioaj;}"
assert 40 "main(){a=5*(9-6);b=26-3+2;return a + b;}"
assert 15 "main(){return 5*(9-6);b=26-3+2;}"
assert 15 "main(){if(1)return 5*(9-6);b=26-3+2;}"
assert 10 "main(){if(0)return 5*(9-6);b=26-3+2;return 10;}"
assert 15 "main(){if(0)return 0; else return 5*(9-6);b=26-3+2;}"
assert 15 "main(){while(0)return 0; return 15;}"
assert 15 "main(){a = 10;while(a > 0)a = a - 1; return 15;}"
assert 10 "main(){b = 0;for(a = 0;a < 10;a = a + 1)b = b + 1; return b;}"
assert 20 "main(){b = 0;c = 0;for(a = 0;a < 10;a = a + 1){b = b + 1;c = c + 2;} return c;}"
assert_with_funccall 0 "main(){ return foo();}"
assert_with_funccall 13 "main(){ return foo1(13);}"
assert_with_funccall 25 "main(){ return foo2(13,12);}"
assert_with_funccall 36 "main(){ return foo3(13,12,11);}"
assert_with_funccall 70 "main(){ return foo7(13,12,11,10,9,8,7);}"
assert_with_funccall 76 "main(){ return foo8(13,12,11,10,9,8,7,6);}"
assert_with_funccall 81 "main(){ return foo9(13,12,11,10,9,8,7,6,5);}"
assert_with_funccall 85 "main(){ return foo10(13,12,11,10,9,8,7,6,5,4);}"
assert 1 "test(a){ return a; } main(){ return test(1); }"
assert 3 "test(a,b){ return a + b; } main(){ return test(1, 2); }"
assert 6 "test(a,b,c){ return a + b + c; } main(){ return test(1, 2, 3); }"
assert 10 "test(a,b,c,d){ return a + b + c + d; } main(){ return test(1, 2, 3, 4); }"
assert 15 "test(a,b,c,d,e){ return a + b + c + d + e; } main(){ return test(1, 2, 3, 4, 5); }"
assert 21 "test(a,b,c,d,e,f){ return a + b + c + d + e + f; } main(){ return test(1, 2, 3, 4, 5, 6); }"
assert 28 "test(a,b,c,d,e,f,g){ return a + b + c + d + e + f + g; } main(){ return test(1, 2, 3, 4, 5, 6, 7); }"
assert 36 "test(a,b,c,d,e,f,g,h){ return a + b + c + d + e + f + g + h; } main(){ return test(1, 2, 3, 4, 5, 6, 7, 8); }"
assert 45 "test(a,b,c,d,e,f,g,h,i){ return a + b + c + d + e + f + g + h + i; } main(){ return test(1, 2, 3, 4, 5, 6, 7, 8, 9); }"
assert 55 "test(a,b,c,d,e,f,g,h,i,j){ return a + b + c + d + e + f + g + h + i + j; } main(){ return test(1, 2, 3, 4, 5, 6, 7, 8, 9, 10); }"

echo OK

