# azcc: Azarashi2931's C Compiler
![C/C++ CI](https://github.com/azarashi2931/azcc/workflows/C/C++%20CI/badge.svg?branch=master)
![license](https://img.shields.io/github/license/azarashi2931/azcc)

This is a toy C compiler, developed at seccamp2020.

## How to Build and Test
`make`: build.  
`make test`: build and run tests for 1st generation azcc, which is built by gcc, clang, or such other C Compiler.  
`make test2`: build and run tests for 2nd generation azcc, which is built by 1st generation azcc.  
`make test3`: build and run tests for 3rd generation azcc, which is built by 2nd generation azcc.  
`make test-all`: build and run tests for all generation azccs.  
`make clean`: remove binaries.  
`make all`: run `make clean` and `make test-all`.  

## Usage
Makefile outputs `bin/genX/azcc`(`X` is the generation of azcc).  
```sh
$ cpp source.c > source.i
$ bin/genX/azcc source.i > source.s
$ gcc -o source.out source.s
```

## Thanks
+ Rui Ueyama, instructor at seccamp2020
+ hikalium, instructor at seccamp2020
+ hsjoihs, tutor at seccamp2020, contributor of this project.
+ sksat, tutor at seccamp2020, contributor of this project.
+ Takana-Norimasa, classmate at seccamp2020
+ and more classmates at seccamp2020

## Reference
+ https://www.sigbus.info/compilerbook
  + An online textbook for scratching c compiler, written by Rui Ueyama.
+ https://github.com/rui314/chibicc
  + A C compiler, developed by Rui Ueyama.
+ https://github.com/hsjoihs/c-compiler
  + A C compiler, developed by hsjoihs.
