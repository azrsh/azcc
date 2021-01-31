#include "config.h"
#include "parse.h"
#include "system_v_amd64_codegen.h"
#include "util.h"

void generate_code(Program *program, Config *config) {
  switch (config->target) {
  case AMD64_LINUX_GNU:
    generate_code_system_v_amd64(program);
    return;
  case AMD64_W64_WINDOWS_GNU:
  case AARCH64_LINUX_GNU:
  case LLVM_IR:
    ERROR("サポートされていないターゲット");
    return;
  }
}
