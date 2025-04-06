#include "neander_expr.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

static int precedence(char op) {
    switch (op) {
        case '*': return 2;
        case '+':
        case '-': return 1;
        default: return 0;
    }
}

static bool is_operator(char c) {
    return c == '+' || c == '-' || c == '*';
}

bool infix_to_postfix(const char* expr, TokenList* output) {
    char stack[MAX_TOKENS];
    int top = -1;
    output->count = 0;

    char token[MAX_TOKEN_LENGTH] = {0};
    int tpos = 0;

    for (int i = 0; expr[i]; i++) {
        char c = expr[i];

        if (isspace(c)) continue;

        if (isalnum(c) || c == '-') {
            token[tpos++] = c;
        } else {
            if (tpos > 0) {
                token[tpos] = '\0';
                strcpy(output->items[output->count++], token);
                tpos = 0;
                memset(token, 0, sizeof(token));
            }

            if (c == '(') {
                stack[++top] = c;
            } else if (c == ')') {
                while (top >= 0 && stack[top] != '(') {
                    char op[2] = {stack[top--], '\0'};
                    strcpy(output->items[output->count++], op);
                }
                if (top >= 0 && stack[top] == '(') top--; // discard '('
            } else if (is_operator(c)) {
                while (top >= 0 && precedence(stack[top]) >= precedence(c)) {
                    char op[2] = {stack[top--], '\0'};
                    strcpy(output->items[output->count++], op);
                }
                stack[++top] = c;
            }
        }
    }

    if (tpos > 0) {
        token[tpos] = '\0';
        strcpy(output->items[output->count++], token);
    }

    while (top >= 0) {
        char op[2] = {stack[top--], '\0'};
        strcpy(output->items[output->count++], op);
    }

    return true;
}

void print_token_list(const TokenList* list) {
    printf("Postfix: ");
    for (int i = 0; i < list->count; i++) {
        printf("%s ", list->items[i]);
    }
    printf("\n");
}
