#ifndef DECLARATION_H
#define DECLARATION_H

#include "container.h"
#include "type.h"

typedef enum {
  STORAGE_NONE,
  STORAGE_TYPEDEF,
  STORAGE_EXTERN,
  STORAGE_STATIC,
  STORAGE_THREAD_LOCAL,
  STORAGE_AUTO,
  STORAGE_REGISTER
} StorageKind;

typedef enum {
  FUNCTION_NONE,    //未指定
  FUNCTION_INLINE,  //関数指定子inline
  FUNCTION_NORETURN //関数指定子_Noreturnl
} FunctionKind;

typedef struct Declaration Declaration;
struct Declaration {
  StorageKind storage;
  Type *type;
  FunctionKind function;
  Vector *declarators; // init-declarators、Variable Vector
};

#endif
