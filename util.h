#ifndef UTIL_H
#define UTIL_H

#include <stdbool.h>

bool start_with(const char *p, const char *q);

//エラーを報告するための関数
// printfと同じ引数をとる
_Noreturn void error(const char *fmt, ...);

extern const char *user_input;
extern const char *filename; // 入力ファイル名

//エラーを報告するための関数
// printfと同じ引数に加えてエラーの位置をとる
_Noreturn void error_at(const char *location, const char *fmt, ...);

const char *read_file(const char *path);

#endif
