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
assert 1 "test(a){ return a; } main(){ return test(1); }"
assert 3 "test(a,b){ return a + b; } main(){ return test(1, 2); }"
assert 6 "test(a,b,c){ return a + b + c; } main(){ return test(1, 2, 3); }"

execute_func_test 0 "main(){foo();}"
execute_func_test 13 "main(){foo1(13);}"
execute_func_test 25 "main(){foo2(13,12);}"
execute_func_test 36 "main(){foo3(13,12,11);}"

echo OK

