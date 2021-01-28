/*
 * 式をパースする。
 * expression、assign、constant_expressionを公開する。
 */

#include "container.h"
#include "declarationparse.h"
#include "node.h"
#include "parseutil.h"
#include "tokenize.h"
#include "type.h"
#include "util.h"
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

Node *expression(ParseContext *context);
Node *assign(ParseContext *context);
Node *constant_expression(ParseContext *context);

Vector *argument_expression_list(ParseContext *context);
Node *conditional(ParseContext *context);
Node *logical_or(ParseContext *context);
Node *logical_and(ParseContext *context);
Node *bitwise_inclusive_or(ParseContext *context);
Node *bitwise_exclusive_or(ParseContext *context);
Node *bitwise_and(ParseContext *context);
Node *equality(ParseContext *context);
Node *relational(ParseContext *context);
Node *add(ParseContext *context);
Node *shift(ParseContext *context);
Node *multiply(ParseContext *context);
Node *unary(ParseContext *context);
Node *postfix(ParseContext *context);
Node *primary(ParseContext *context);
Node *literal(ParseContext *context);

// expression = (assign ("," assign)*) | variable_definition
// argument_expression_list = expression ("," expression)*

// assign = (unary (("=" | "+=" | "-=" | "*=" | "/=") assign)? | conditional)
// conditional = logic_or ("?" expression ":" conditional)?
// logical_or = logic_and ("||" logic_and)*
// logical_and = bitwise_inclusive_or ("&&" bitwise_inclusive_or)*
// bitwise_inclusive_or = bitwise_exclusive_or ("&" bitwise_exclusive_or)*
// bitwise_exclusive_xor = bitwise_and ("^" bitwise_and)*
// bitwise_and = equality ("&" equality)*
// equality = relational ("==" relational | "!=" relational)*
// relational = add ("<" add | "<=" add | ">" add | ">=" add)*
// add = mul ("+" mul | "-" mul)*
// mul = unary ("*" unary | "/" unary)*
// unary = ("+" | "-" | "&" | "*" | "!" | "sizeof" | "_Alignof")? (primary |
// "(" type_specifier ")" ) postfix = primary ("(" argument_expression_list?
// ")" | "[" expression "]" |
// "." identifier)* primary = literal | identity ("("
// argument_expression_list?
// ")" | "[" expression "]")? | "("expression")"
// literal = number | "\"" string"\""

//式をパースする
Node *expression(ParseContext *context) {
  //カンマ演算子
  Node *node = assign(context);
  while (consume(",")) {
    node = new_node(NODE_COMMA, node, assign(context));
  }

  return node;
}

// Node Vector
Vector *argument_expression_list(ParseContext *context) {
  Vector *arguments = new_vector(32);
  do {
    vector_push_back(arguments, assign(context));
  } while (consume(","));
  return arguments;
}

Node *constant_expression(ParseContext *context) {
  return new_node_num(expect_number());
}

//代入をパースする
Node *assign(ParseContext *context) {
  //本来代入の左辺にとれるのはunaryだけなのでフィルタリングする必要があるがやっていない
  Node *node = conditional(context);

  if (consume("=")) {
    return new_node(NODE_ASSIGN, node, assign(context));
  } else if (consume("+=")) {
    Node *addNode = new_node(NODE_ADD, node, assign(context));
    return new_node(NODE_ASSIGN, node, addNode);
  } else if (consume("-=")) {
    Node *subNode = new_node(NODE_SUB, node, assign(context));
    return new_node(NODE_ASSIGN, node, subNode);
  } else if (consume("*=")) {
    Node *mulNode = new_node(NODE_MUL, node, assign(context));
    return new_node(NODE_ASSIGN, node, mulNode);
  } else if (consume("/=")) {
    Node *divNode = new_node(NODE_DIV, node, assign(context));
    return new_node(NODE_ASSIGN, node, divNode);
  } else if (consume("%=")) {
    Node *modNode = new_node(NODE_MOD, node, assign(context));
    return new_node(NODE_ASSIGN, node, modNode);
  } else if (consume("&=")) {
    Node *bandNode = new_node(NODE_BAND, node, assign(context));
    return new_node(NODE_ASSIGN, node, bandNode);
  } else if (consume("^=")) {
    Node *bxorNode = new_node(NODE_BXOR, node, assign(context));
    return new_node(NODE_ASSIGN, node, bxorNode);
  } else if (consume("|=")) {
    Node *borNode = new_node(NODE_BOR, node, assign(context));
    return new_node(NODE_ASSIGN, node, borNode);
  } else if (consume("<<=")) {
    Node *lshftNode = new_node(NODE_LSHIFT, node, assign(context));
    return new_node(NODE_ASSIGN, node, lshftNode);
  } else if (consume(">>=")) {
    Node *rshiftNode = new_node(NODE_RSHIFT, node, assign(context));
    return new_node(NODE_ASSIGN, node, rshiftNode);
  } else {
    return node;
  }
}

Node *conditional(ParseContext *context) {
  Node *node = logical_or(context);

  if (consume("?")) {
    Node *lhs = expression(context);
    expect(":");
    Node *rhs = conditional(context);

    Node *conditionalNode = new_node(NODE_COND, lhs, rhs);
    conditionalNode->condition = node;
    node = conditionalNode;
  }

  return node;
}

Node *logical_or(ParseContext *context) {
  Node *node = logical_and(context);

  for (;;) {
    if (consume("||"))
      node = new_node(NODE_LOR, node, logical_and(context));
    else
      return node;
  }
}

Node *logical_and(ParseContext *context) {
  Node *node = bitwise_inclusive_or(context);

  for (;;) {
    if (consume("&&"))
      node = new_node(NODE_LAND, node, bitwise_inclusive_or(context));
    else
      return node;
  }
}

Node *bitwise_inclusive_or(ParseContext *context) {
  Node *node = bitwise_exclusive_or(context);

  for (;;) {
    if (consume("|"))
      node = new_node(NODE_BOR, node, bitwise_exclusive_or(context));
    else
      return node;
  }
}

Node *bitwise_exclusive_or(ParseContext *context) {
  Node *node = bitwise_and(context);

  for (;;) {
    if (consume("^"))
      node = new_node(NODE_BXOR, node, bitwise_and(context));
    else
      return node;
  }
}

Node *bitwise_and(ParseContext *context) {
  Node *node = equality(context);

  for (;;) {
    if (consume("&"))
      node = new_node(NODE_BAND, node, equality(context));
    else
      return node;
  }
}

//等式をパースする
Node *equality(ParseContext *context) {
  Node *node = relational(context);

  for (;;) {
    if (consume("=="))
      node = new_node(NODE_EQ, node, relational(context));
    else if (consume("!="))
      node = new_node(NODE_NE, node, relational(context));
    else
      return node;
  }
}

//不等式をパースする
Node *relational(ParseContext *context) {
  Node *node = shift(context);

  for (;;) {
    if (consume("<"))
      node = new_node(NODE_LT, node, shift(context));
    else if (consume("<="))
      node = new_node(NODE_LE, node, shift(context));
    else if (consume(">"))
      node = new_node(NODE_LT, shift(context), node);
    else if (consume(">="))
      node = new_node(NODE_LE, shift(context), node);
    else
      return node;
  }
}

Node *shift(ParseContext *context) {
  Node *node = add(context);

  for (;;) {
    if (consume("<<"))
      node = new_node(NODE_LSHIFT, node, add(context));
    else if (consume(">>"))
      node = new_node(NODE_RSHIFT, node, add(context));
    else
      return node;
  }
}

Node *add(ParseContext *context) {
  Node *node = multiply(context);

  for (;;) {
    if (consume("+"))
      node = new_node(NODE_ADD, node, multiply(context));
    else if (consume("-"))
      node = new_node(NODE_SUB, node, multiply(context));
    else
      return node;
  }
}

//乗除算をパースする
Node *multiply(ParseContext *context) {
  Node *node = unary(context);

  for (;;) {
    if (consume("*"))
      node = new_node(NODE_MUL, node, unary(context));
    else if (consume("/"))
      node = new_node(NODE_DIV, node, unary(context));
    else if (consume("%"))
      node = new_node(NODE_MOD, node, unary(context));
    else
      return node;
  }
}

//単項演算子をパースする
Node *unary(ParseContext *context) {
  if (consume("+"))
    return postfix(context);
  if (consume("-"))
    return new_node(NODE_SUB, new_node_num(0), postfix(context));
  if (consume("!"))
    return new_node(NODE_LNOT, unary(context), NULL);
  if (consume("~"))
    return new_node(NODE_BNOT, unary(context), NULL);
  if (consume("&"))
    return new_node(NODE_REF, unary(context), NULL);
  if (consume("*"))
    return new_node(NODE_DEREF, unary(context), NULL);
  if (consume("++")) {
    Node *source = unary(context);
    Node *addNode = new_node(NODE_ADD, source, new_node_num(1));
    return new_node(NODE_ASSIGN, source, addNode);
  }
  if (consume("--")) {
    Node *source = unary(context);
    Node *addNode = new_node(NODE_SUB, source, new_node_num(1));
    return new_node(NODE_ASSIGN, source, addNode);
  }
  if (consume("sizeof")) {
    Token *head = token;
    if (consume("(")) {
      Type *type = type_name(context);
      if (type) {
        expect(")");
        return new_node_num(type_to_size(type));
      }
    }
    token = head;

    bool parentheses = consume("(");
    //識別子に対するsizeofのみを特別に許可する
    Token *identifier = consume_identifier();
    // postfix(context);
    if (!identifier) {
      ERROR_AT(identifier->string->head, "式に対するsizeof演算は未実装です");
    }
    if (parentheses)
      expect(")");

    Type *type =
        new_node_variable(identifier, context->scope->variableContainer)
            ->variable->type;
    if (!type)
      ERROR_AT(token->string->head, "sizeof演算子のオペランドが不正です");
    return new_node_num(type_to_size(type));
  }
  if (consume("_Alignof")) {
    expect("(");

    Type *type = type_name(context);
    if (!type)
      ERROR_AT(token->string->head, "型指定子ではありません");

    expect(")");
    return new_node_num(type_to_align(type));
  }
  return postfix(context);
}

//変数、関数呼び出し、添字付の配列
Node *postfix(ParseContext *context) {
  //----postfixが連続するときの先頭は関数呼び出しかprimary--------
  Node *node = primary(context);

  for (;;) {
    if (consume("(")) {
      Vector *arguments; // Node vector
      if (consume(")")) {
        arguments = new_vector(0);
      } else {
        arguments = argument_expression_list(context);
        expect(")");
      }

      node = new_node_function_call(node);
      node->functionCall->arguments = arguments;
    } else if (consume("[")) {
      //配列の添字をポインタ演算に置き換え
      //ポインタ演算の構文木を生成
      Node *addNode = new_node(NODE_ADD, node, expression(context));
      node = new_node(NODE_DEREF, addNode, NULL);
      expect("]");
    } else if (consume(".")) {
      Token *memberToken = expect_identifier();
      Node *memberNode = new_node_member(memberToken);
      node = new_node(NODE_DOT, node, memberNode);
    } else if (consume("->")) {
      Token *memberToken = expect_identifier();
      Node *memberNode = new_node_member(memberToken);
      node = new_node(NODE_DEREF, node, NULL);
      node = new_node(NODE_DOT, node, memberNode);
    } else if (consume("++")) {
      Node *addNode = new_node(NODE_ADD, node, new_node_num(1));
      Node *assignNode = new_node(NODE_ASSIGN, node, addNode);
      node = new_node(NODE_SUB, assignNode,
                      new_node_num(1)); //戻り値を変えるためにやっているが、最悪
    } else if (consume("--")) {
      Node *addNode = new_node(NODE_SUB, node, new_node_num(1));
      Node *assignNode = new_node(NODE_ASSIGN, node, addNode);
      node = new_node(NODE_ADD, assignNode,
                      new_node_num(1)); //戻り値を変えるためにやっているが、最悪
    } else {
      return node;
    }
  }
}

Node *primary(ParseContext *context) {
  //次のトークンが(なら入れ子になった式
  if (consume("(")) {
    Node *node = expression(context);
    expect(")");
    return node;
  }

  Token *identifier = consume_identifier();
  if (identifier) {
    Node *node =
        new_node_variable(identifier, context->scope->variableContainer);
    //列挙子の解決 ここでやるべきではない
    if (node->variable->kind == VARIABLE_ENUMERATOR)
      return node->variable->initialization;
    else
      return node;
  }

  //そうでなければリテラル
  return literal(context);
}

Node *literal(ParseContext *context) {
  Token *character = consume_character();
  if (character) {
    Node *node = new_node(NODE_CHAR, NULL, NULL);
    if (character->string->length == 1)
      node->val = *character->string->head;
    else if (character->string->head[0] == '\\') {
      switch (character->string->head[1]) {
      case '0':
        node->val = '\0';
        break;
      case 'a':
        node->val = '\a';
        break;
      case 'b':
        node->val = '\b';
        break;
      case 'f':
        node->val = '\f';
        break;
      case 'n':
        node->val = '\n';
        break;
      case 'r':
        node->val = '\r';
        break;
      case 't':
        node->val = '\t';
        break;
      case 'v':
        node->val = '\v';
        break;
      case '\\':
      case '\'':
      case '\"':
      case '\?':
        node->val = character->string->head[1];
        break;
      default:
        ERROR_AT(character->string->head,
                 "予期しない文字のエスケープシーケンスです");
        break;
      }
    } else {
      ERROR_AT(character->string->head, "文字リテラルではありません");
    }
    return node;
  }

  Token *string = consume_string();
  if (string) {
    const String *content = string->string;
    for (;;) {
      Token *next = consume_string();
      if (next)
        content = string_concat(content, next->string);
      else
        break;
    }

    Node *node = new_node(NODE_STRING, NULL, NULL);
    node->val = vector_length(context->translationUnit->stringLiterals);
    //コンテナはポインタしか受け入れられないので
    vector_push_back(context->translationUnit->stringLiterals,
                     string_to_char(content));
    return node;
  }

  //そうでなければ整数
  return new_node_num(expect_number());
}
