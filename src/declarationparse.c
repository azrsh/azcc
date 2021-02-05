/*
 * 宣言文をパースする。
 * declaration、declaration_specifier、type_nameを公開する。
 */

#include "declarationparse.h"
#include "container.h"
#include "declaration.h"
#include "expressionparse.h"
#include "membercontainer.h"
#include "parseutil.h"
#include "tokenize.h"
#include "type.h"
#include "typecontainer.h"
#include "util.h"
#include "variable.h"
#include <assert.h>
#include <stdlib.h>

//全てのメンバがNullまたはNoneに初期化される
Declaration *new_declaration(void) {
  Declaration *result = calloc(1, sizeof(Declaration));
  return result;
}

Declaration *declaration_clone(Declaration *source) {
  Declaration *result = new_declaration();
  result->storage = source->storage;
  result->type = source->type;
  result->function = source->function;
  result->declarators = source->declarators;
  return result;
}

Vector *vector_wrap(void *element) {
  Vector *result = new_vector(1);
  vector_push_back(result, element);
  return result;
}

// EBNF
// declaration = decralation_specifier init_declarator_list ";"

// declaration_specifier = (storage_class_specifier | type_specifier |
// type_qualifier | function_specifier) declaration_specifier*
// type_specifier
// struct_or_union_specifier
// | type_qualifier)*
// enum_specifier

// init_declarator_list = init_declarator ("," init_declarator)*
// init_declarator = declarator ("=" initializer)?
// declarator = pointer? direct_declarator
// direct_declarator = (identifier | ("(" declarator ")")) ("["
// constant_expression "]" | "(" parameter_type_list | identifier_list ")")*

// pointer = ("*" type_qualifier_list?)*
// type_qualifier_list = "const"*

// parameter_type_list = parameter_list ("," "...")?
// parameter_list = parameter_declaration ("," parameter_declaration)*
// parameter_declaration = declaration_specifier (declarator |
// abstract_declarator?)

// identifier_list = identifier ("," identifier)*

// 仕様と異なるEBNF
// type_name = declaration_specifier abstract_declarator

// abstract_declarator = pointer | (pointer? direct_abstract_declarator)
// abstract_direct_declarator = ("(" abstract_declarator ")")? ("["
// constant_expression "]" | "(" parameter_type_list ")")*

Declaration *declaration(Declaration *base,
                         ParseContext *context); // declarated in header file

Declaration *declaration_specifier(ParseContext *context);
Type *type_specifier(ParseContext *context);
Type *struct_or_union_specifier(ParseContext *context);
Type *enum_specifier(ParseContext *context);
Type *typedef_name(ParseContext *context);

Vector *init_declarator_list(Type *base, ParseContext *context);
Variable *init_declarator(Type *base, ParseContext *context);
Variable *declarator(Type *base, ParseContext *context);
Variable *direct_declarator(Type *type, ParseContext *context);

Type *pointer(Type *base);
bool type_qualifier_list(void);

// Declaration Vector
Vector *parameter_type_list(ParseContext *context);
// Declaration Vector
Vector *parameter_list(ParseContext *context);
Declaration *parameter_declaration(ParseContext *context);

// Declaration Vector
Vector *identifier_list(void);

Type *type_name(ParseContext *context);

Variable *abstract_declarator(Type *base, ParseContext *context);
Variable *direct_abstract_declarator(Type *type, ParseContext *context);

Node *initializer(ParseContext *context);

Declaration *declaration(Declaration *base, ParseContext *context) {
  if (!base)
    return NULL;

  Declaration *result = declaration_clone(base);
  Type *type = result->type;
  if (type) {
    if (consume(";")) {
      result->type = type;
      result->declarators = new_vector(0);
      return result;
    }

    result->declarators = init_declarator_list(type, context);
    expect(";");
    return result;
  }

  // static assert

  return NULL;
}

Declaration *declaration_specifier(ParseContext *context) {
  Declaration *result = new_declaration();
  for (;;) {
    // storage class specifier
    if (consume("typedef")) {
      result->storage = STORAGE_TYPEDEF;
      continue;
    } else if (consume("extern")) {
      result->storage = STORAGE_EXTERN;
      continue;
    } else if (consume("static")) {
      result->storage = STORAGE_STATIC;
      continue;
    } else if (consume("_Thread_local")) {
      result->storage = STORAGE_THREAD_LOCAL;
      ERROR_AT(token->string->head, "_Thread_localはサポートされていません");
      continue;
    } else if (consume("auto")) {
      result->storage = STORAGE_AUTO;
      continue;
    } else if (consume("register")) {
      result->storage = STORAGE_REGISTER;
      ERROR_AT(token->string->head, "registerはサポートされていません");
      continue;
    }

    // type specifier
    {
      Token *head = token;
      Type *typeSpecifier = type_specifier(context);
      if (typeSpecifier) {
        if (!result->type) {
          result->type = typeSpecifier;
          continue;
        }

        // short、long long、long
        {
          if (typeSpecifier->kind == TYPE_INT &&
              (result->type->kind == TYPE_SHORT ||
               result->type->kind == TYPE_LONG ||
               result->type->kind == TYPE_LONG_LONG)) {
            continue;
          }

          if (result->type->kind == TYPE_INT &&
              (typeSpecifier->kind == TYPE_SHORT ||
               typeSpecifier->kind == TYPE_LONG ||
               typeSpecifier->kind == TYPE_LONG_LONG)) {
            result->type->kind = typeSpecifier->kind;
            continue;
          }

          if (result->type->kind == TYPE_LONG &&
              typeSpecifier->kind == TYPE_LONG) {
            result->type->kind = TYPE_LONG_LONG;
            continue;
          }
        }

        /* typedef struct A A; typedef struct A A;への対応
         * 本来はtypedef_nameを特別扱いすべきだが
         * 実装の都合上構造体および共用体の特別扱いとした
         * この実装では、実際にはコンパイルできない
         * typedef int A; typedf unsigned A B;
         * などが許可されてしまう
         */
        if (result->type->kind != TYPE_STRUCT &&
            result->type->kind != TYPE_UNION) {
          //これまでの型指定子と合成する
          //本来ならanalyzeで行うべき
          if (result->type->kind == TYPE_NONE &&
              typeSpecifier->signKind == SIGN_NONE) {
            result->type->kind = typeSpecifier->kind;
          } else if (typeSpecifier->kind == TYPE_NONE &&
                     result->type->signKind == SIGN_NONE) {
            result->type->signKind = typeSpecifier->signKind;
          } else {
            ERROR_AT(token->string->head, "不正な型指定子です");
          }
          continue;
        }

        token = head;
      }
    }

    // type qualifier
    if (type_qualifier_list())
      continue;

    // function specifier
    {
      // これらの関数指定子で挙動を変える必要はないので
      // パースしているが何もしない
      if (consume("inline")) {
        result->function = FUNCTION_INLINE;
        continue;
      } else if (consume("_Noreturn")) {
        result->function = FUNCTION_NORETURN;
        continue;
      }
    }

    // alignment specifier
    if (consume("_Alignas")) {
      // (type-name) | (constant-expression)
      ERROR_AT(token->string->head, "_Alighnasはサポートされていません");
      continue;
    }

    if (!result->type)
      return NULL;

    //デフォルトの値をセットする
    //本来ならanalyzeで行うべき
    if (result->type->kind == TYPE_NONE)
      result->type->kind = TYPE_INT;
    if (result->type->signKind == SIGN_NONE)
      result->type->signKind = SIGN_SIGNED;

    return result;
  }
}

//型指定子をパースする
Type *type_specifier(ParseContext *context) {
  // primitive types
  if (consume("void")) {
    return new_type(TYPE_VOID);
  } else if (consume("char")) {
    return new_type(TYPE_CHAR);
  } else if (consume("short")) {
    return new_type(TYPE_SHORT);
  } else if (consume("int")) {
    return new_type(TYPE_INT);
  } else if (consume("long")) {
    return new_type(TYPE_LONG);
  } else if (consume("float")) {
    // new_type(TYPE_FLOAT);
    ERROR_AT(token->string->head, "サポートされていない型です");
  } else if (consume("double")) {
    // new_type(TYPE_DOUBLE);
    ERROR_AT(token->string->head, "サポートされていない型です");
  } else if (consume("signed")) {
    return new_type_with_sign(TYPE_NONE, SIGN_SIGNED);
  } else if (consume("unsigned")) {
    return new_type_with_sign(TYPE_NONE, SIGN_UNSIGNED);
  } else if (consume("_Bool")) {
    return new_type(TYPE_BOOL);
  } else if (consume("_Complex")) {
    // new_type(TYPE_COMPLEX);
    ERROR_AT(token->string->head, "サポートされていない型です");
  }

  if (consume("atomic-type-specifier")) {
    ERROR("atomic specifier is not supported");
  }

  // struct or union specifier
  {
    Type *structType = struct_or_union_specifier(context);
    if (structType) {
      return structType;
    }
  }

  // enum specifier
  {
    Type *enumType = enum_specifier(context);
    if (enumType) {
      return enumType;
    }
  }

  // typedef name
  {
    Type *typedefType = typedef_name(context);
    if (typedefType) {
      return typedefType;
    }
  }

  return NULL;
}

Type *struct_or_union_specifier(ParseContext *context) {
  TypeKind kind = TYPE_VOID;
  if (consume("struct"))
    kind = TYPE_STRUCT;
  else if (consume("union"))
    kind = TYPE_UNION;
  else
    return NULL;

  Type *result = NULL;

  Token *identifier = consume_identifier();

  // 宣言済み構造体の解決
  if (identifier) {
    Type *type =
        type_container_get(context->scope->tagContainer, identifier->string);
    if (type) {
      if (type->kind == kind) {
        result = type;
      } else {
        ERROR_AT(identifier->string->head,
                 "%sは違う種類のタグとして定義されています",
                 identifier->string->head);
      }
    }
  }

  // 解決できなければ生成
  if (!result) {
    result = new_type(kind);
    if (identifier) {
      result->name = identifier->string;

      //この時点でresultは宣言済みでないことが保証されているので場合分けは不要
      type_container_push(context->scope->tagContainer, result->name, result);
    }
  }

  if (consume("{")) {
    if (!result->isDefined) {
      result->isDefined = true;
      result->members = new_member_container();
    } else {
      ERROR_AT(identifier->string->head,
               "構造体または共用体が多重に定義されています")
    }

    int memberOffset = 0;
    while (!consume("}")) {
      Declaration *memberDeclaration =
          declaration(declaration_specifier(context), context);
      for (int i = 0; i < vector_length(memberDeclaration->declarators); i++) {
        Variable *member = vector_get(memberDeclaration->declarators, i);
        if (!member)
          ERROR_AT(token->string->head,
                   "構造体または共用体のメンバの定義が不正です");

        member->kind = VARIABLE_LOCAL;
        size_t memberAlignment = type_to_align(member->type);
        if (kind == TYPE_STRUCT) {
          memberOffset += (memberAlignment - memberOffset % memberAlignment) %
                          memberAlignment;
          member->offset = memberOffset;
          memberOffset += type_to_size(member->type);
        } else if (kind == TYPE_UNION) {
          member->offset = 0;

          INSERT_COMMENT("name:%.*s offset:%d size:%d", member->name->length,
                         member->name->head, member->offset,
                         type_to_size(member->type));
        } else {
          assert(0); // unreachable
        }

        if (!member_container_push(result->members, member))
          ERROR_AT(member->name->head, "同名のメンバが既に定義されています");
      }
    }
  } else if (!identifier) {
    ERROR_AT(token->string->head,
             "無名構造体または共用体を定義なしで使用することはできません");
  }

  return result;
}

Type *enum_specifier(ParseContext *context) {
  Token *tokenHead = token;

  if (!consume("enum"))
    return NULL;

  Token *identifier = consume_identifier();

  Type *result = NULL;

  // 宣言済み列挙体の解決
  if (identifier) {
    Type *type =
        type_container_get(context->scope->tagContainer, identifier->string);
    if (type) {
      if (type->kind == TYPE_ENUM) {
        result = type;
      } else {
        ERROR_AT(identifier->string->head,
                 "%sは違う種類のタグとして定義されています",
                 identifier->string->head);
      }
    }
  }

  // 解決できなければ生成
  if (!result) {
    result = new_type(TYPE_ENUM);
    if (identifier) {
      result->name = identifier->string;

      //この時点でresultは宣言済みでないことが保証されているので場合分けは不要
      type_container_push(context->scope->tagContainer, result->name, result);
    }
  }

  if (consume("{")) {
    if (!result->isDefined) {
      result->isDefined = true;
    } else {
      ERROR_AT(identifier->string->head, "列挙体が多重に定義されています")
    }

    int count = 0;
    do {
      Token *enumeratorIdentifier = consume_identifier();
      if (!enumeratorIdentifier)
        break;

      Node *constantExpression = NULL;
      if (consume("="))
        constantExpression = constant_expression();
      else {
        constantExpression = new_node_num(count);
        count++;
      }

      Variable *variable =
          new_variable(new_type(TYPE_INT), enumeratorIdentifier->string);
      Variable *enumeratorVariable =
          variable_to_enumerator(variable, constantExpression);
      if (!variable_container_push(context->scope->variableContainer,
                                   enumeratorVariable))
        ERROR_AT(enumeratorIdentifier->string->head,
                 "列挙子%sと同名の識別子が既に定義されています",
                 string_to_char(enumeratorIdentifier->string));
    } while (consume(","));
    expect("}");
  } else if (!identifier) {
    ERROR_AT(tokenHead->string->head,
             "列挙体の名称または列挙子を指定してください");
  }

  return result;
}

Type *typedef_name(ParseContext *context) {
  Token *tokenHead = token;

  Token *identifier = consume_identifier();
  if (!identifier)
    return NULL;

  // typedefされた型指定子の探索
  Type *result =
      type_container_get(context->scope->typedefContainer, identifier->string);
  if (result)
    return result;

  token = tokenHead;
  return NULL;
}

Vector *init_declarator_list(Type *base, ParseContext *context) {
  Vector *declarators = new_vector(8);
  do {
    vector_push_back(declarators, init_declarator(base, context));
  } while (consume(","));
  return declarators;
}

Variable *init_declarator(Type *base, ParseContext *context) {
  // declarator
  Variable *variable = declarator(base, context);
  if (!variable)
    return NULL;

  // initalizer
  if (consume("="))
    variable->initialization = initializer(context);

  return variable;
}

Variable *declarator(Type *base, ParseContext *context) {
  // pointer
  Type *type = pointer(base);

  // direct_declarator
  return direct_declarator(type, context);
}

Variable *direct_declarator(Type *base, ParseContext *context) {
  Variable *result = NULL;

  // 1番外側
  {
    Type *dummy = new_type(TYPE_VOID);
    Token *identifier = consume_identifier();
    if (identifier)
      result = new_variable(dummy, identifier->string);
    else if (consume("(")) {
      result = declarator(dummy, context);
      expect(")");
    }

    // 上の2つの条件を満たすがNULLが返ってきたときも
    // どちらの条件を満たさないときもNlULLを返す
    if (!result) {
      return NULL;
    }
  }

  // 2番に外側
  Type root;
  root.base = base;
  Type *type = &root;
  for (;;) {
    Type *child = NULL;
    if (consume("[")) {
      child = new_type_array(base, expect_number());
      expect("]");
    } else if (consume("(")) {
      // Declaration Vector
      Vector *parameters = NULL;
      if (!consume(")")) {
        parameters = parameter_type_list(context);
        if (!parameters)
          parameters = identifier_list();

        assert(parameters);

        expect(")");
      }
      child = new_type_function(base, parameters);
    } else {
      break;
    }

    if (type->base) {
      type->base = child;
      type = type->base;
    } else if (type->returnType) {
      type->returnType = child;
      type = type->returnType;
    } else {
      assert(0); // unreachable
    }
  }

  // dummyを上書き
  // ただしdummyの保管場所はdeclarationでコピーされて変わっているので
  // 再探索する
  {
    Type *leaf = result->type;
    for (;;) {
      if (leaf->base)
        leaf = leaf->base;
      else if (leaf->returnType)
        leaf = leaf->returnType;
      else
        break;
    }

    *leaf = *root.base;
  }

  return result;
}

Type *pointer(Type *base) {
  Type *type = base;
  while (consume("*")) {
    Type *pointer = new_type(TYPE_PTR);
    pointer->base = type;
    type = pointer;

    type_qualifier_list();
  }
  return type;
}

bool type_qualifier_list(void) {
  bool result = false;
  for (;;) {
    if (consume("const")) {
      result = true;
      continue;
    }

    if (consume("restrict") || consume("volatile") || consume("_Atomic"))
      ERROR_AT(token->string->head, "Unsupported type qualifier");

    return result;
  }
}

// parameter_type_list = parameter_list ("," "...")?
// パラメータが少なくとも1つはあることを期待する
Vector *parameter_type_list(ParseContext *context) {
  Vector *result = parameter_list(context);

  if (consume(",") && consume("...")) {
    ERROR_AT(token->string->head,
             "可変長引数関数の宣言はサポートされていません");
    // vector_push_back(result, );
  }

  return result;
}

// parameter_list = parameter_declaration ("," parameter_declaration)*
// パラメータが少なくとも1つはあることを期待する
Vector *parameter_list(ParseContext *context) {
  Vector *result = new_vector(16);
  do {
    Declaration *parameter = parameter_declaration(context);

    assert(parameter);

    vector_push_back(result, parameter);
  } while (consume(","));

  return result;
}

// parameter_declaration = declaration_specifier (declarator |
// abstract_declarator?)
Declaration *parameter_declaration(ParseContext *context) {
  Declaration *base = declaration_specifier(context);
  if (!base)
    return NULL;

  Token *head = token;
  Variable *element = declarator(base->type, context);
  if (!element) {
    //ロールバック
    //構造体指定子などで多重に名前を登録してうまく動かないと思われる
    token = head;
    element = abstract_declarator(base->type, context);
  }

  base->declarators = vector_wrap(element);

  return base;
}

// identifier_list = identifier ("," identifier)*
// パラメータが少なくとも1つはあることを期待する
Vector *identifier_list(void) {
  Vector *result = new_vector(16);
  do {
    Variable *variable =
        new_variable(new_type(TYPE_INT), expect_identifier()->string);
    Declaration *declaration = new_declaration();
    declaration->type = variable->type;
    declaration->declarators = vector_wrap(variable);
    vector_push_back(result, declaration);
  } while (consume(","));
  return result;
}

Type *type_name(ParseContext *context) {
  Declaration *specifier = declaration_specifier(context);
  if (!specifier)
    return NULL;

  Variable *result = abstract_declarator(specifier->type, context);
  return result ? result->type : specifier->type;
}

Variable *abstract_declarator(Type *base, ParseContext *context) {
  // pointer
  Type *type = pointer(base);

  // direct_abstract_declarator
  return direct_abstract_declarator(type, context);
}

Variable *direct_abstract_declarator(Type *base, ParseContext *context) {
  Variable *result = NULL;

  // 1番外側
  {
    Type *dummy = new_type(TYPE_VOID);
    if (consume("(")) {
      result = abstract_declarator(dummy, context);
      expect(")");
    } else {
      result = new_variable(dummy, NULL);
    }
  }

  // 2番目に外側
  Type root;
  root.base = base;
  Type *type = &root;
  for (;;) {
    Type *child = NULL;
    if (consume("[")) {
      child = new_type_array(base, expect_number());
      expect("]");
    } else if (consume("(")) {
      Vector *parameters = NULL;
      if (!consume(")")) {
        parameters = parameter_type_list(context);
        expect(")");
      }
      child = new_type_function(base, parameters);
    } else {
      break;
    }

    if (type->base) {
      type->base = child;
      type = type->base;
    } else if (type->returnType) {
      type->returnType = child;
      type = type->returnType;
    } else {
      assert(0); // unreachable
    }
  }

  //ダミーを上書き
  {
    Type *leaf = result->type;
    for (;;) {
      if (leaf->base)
        leaf = leaf->base;
      else if (leaf->returnType)
        leaf = leaf->returnType;
      else
        break;
    }

    *leaf = *root.base;
  }

  return result;
}

//配列とプリミティブ型のみに対応
Node *initializer(ParseContext *context) {
  if (consume("{")) {
    Vector *elements = new_vector(16);
    do {
      vector_push_back(elements, assign(context));
    } while (consume(","));
    expect("}");

    Node *result = new_node(NODE_ARRAY, NULL, NULL);
    result->elements = elements;
    return result;
  }

  return assign(context);
}
