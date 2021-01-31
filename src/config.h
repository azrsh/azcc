#ifndef CONFIG_H
#define CONFIG_H

typedef enum {
  AMD64_LINUX_GNU,       // supported
  AMD64_W64_WINDOWS_GNU, // unsupported
  AARCH64_LINUX_GNU,     // unsupported
  LLVM_IR                // unsupported
} TargetPlatform;

typedef struct Config Config;
struct Config {
  TargetPlatform target;
  char *filename;
};

#endif
