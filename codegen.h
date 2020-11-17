#ifndef CODEGEN_H
#define CODEGEN_H

#include "parse.h"

//抽象構文木をもとにコード生成を行う
void generate_code(Program *node);

#endif
