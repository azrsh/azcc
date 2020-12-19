#include "util.h"
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *user_input;
const char *filename; // 入力ファイル名

bool start_with(const char *p, const char *q) {
  return memcmp(p, q, strlen(q)) == 0;
}

// 指定されたファイルの内容を返す
const char *read_path(const char *path) {
  // ファイルを開く
  FILE *fp = fopen(path, "r");
  if (!fp)
    ERROR("cannot open %s: %s", path, strerror(errno));

  // ファイルの長さを調べる
  if (fseek(fp, 0, SEEK_END) == -1)
    ERROR("%s: fseek: %s", path, strerror(errno));
  size_t size = ftell(fp);
  if (fseek(fp, 0, SEEK_SET) == -1)
    ERROR("%s: fseek: %s", path, strerror(errno));

  // ファイル内容を読み込む
  char *buf = calloc(1, size + 2);
  fread(buf, size, 1, fp);

  // ファイルが必ず"\n\0"で終わっているようにする
  if (size == 0 || buf[size - 1] != '\n')
    buf[size++] = '\n';
  buf[size] = '\0';
  fclose(fp);
  return buf;
}

//本来はread_file(FILE *stream)としたいが、stdioを宣言できないのでこの形とする
const char *read_stdin() {
  const int bufferSize = 4096;
  char *buffer = calloc(bufferSize, sizeof(char));
  for (int i = 0;; i++) {
    buffer[i] = getchar();
    if (buffer[i] == EOF) {
      if (i == 0 || buffer[i - 1] != '\n')
        buffer[i++] = '\n';
      buffer[i] = '\0';
      break;
    }

    if (i % bufferSize == bufferSize - 1)
      buffer = realloc(buffer, (i + bufferSize + 2) * sizeof(char));
  }

  return buffer;
}
