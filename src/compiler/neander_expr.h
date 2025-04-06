#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <stdbool.h>

#define MAX_TOKENS 64
#define MAX_TOKEN_LENGTH 32

typedef struct {
    char items[MAX_TOKENS][MAX_TOKEN_LENGTH];
    int count;
} TokenList;

bool infix_to_postfix(const char* expr, TokenList* output);
void print_token_list(const TokenList* list);

#endif
