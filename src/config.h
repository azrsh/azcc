#ifndef CONFIG_H
#define CONFIG_H

typedef enum {
  TARGET_AMD64_LINUX_GNU,       // supported
  TARGET_AMD64_W64_WINDOWS_GNU, // unsupported
  TARGET_AARCH64_LINUX_GNU,     // unsupported
  TARGET_LLVM_IR,               // unsupported
  TARGET_UNKNOWN                // unsupported
} TargetPlatform;

typedef struct Config Config;
struct Config {
  TargetPlatform target;
  char *filename;
};

#endif
