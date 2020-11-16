#ifndef UTIL_H
#define UTIL_H

#include <stdbool.h>

bool start_with(char *p, char *q);


//エラーを報告するための関数
// printfと同じ引数に加えてエラーの位置をとる
void error_at(char *location, char *fmt, ...);

//エラーを報告するための関数
// printfと同じ引数をとる
void error(char *fmt, ...);

#endif
