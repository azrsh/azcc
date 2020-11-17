#ifndef UTIL_H
#define UTIL_H

#include <stdbool.h>

bool start_with(const char *p, const char *q);

extern const char *user_input;

//エラーを報告するための関数
// printfと同じ引数に加えてエラーの位置をとる
void error_at(const char *location, const char *fmt, ...);

//エラーを報告するための関数
// printfと同じ引数をとる
void error(const char *fmt, ...);

#endif
