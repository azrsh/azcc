#ifndef UTIL_H
#define UTIL_H

#include <stdbool.h>

bool start_with(const char *p, const char *q);

//エラーを報告するための関数
// printfと同じ引数をとる
// non-prototyped
void error();

extern const char *user_input;
extern const char *filename; // 入力ファイル名

//エラーを報告するための関数
// printfと同じ引数に加えてエラーの位置をとる
// non-prototyped
void error_at();

const char *read_file(const char *path);

#endif
