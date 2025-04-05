#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "parser.h"

// ------------------ utilitários ------------------

static char *read_file(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) return NULL;

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    char *buffer = malloc(size + 1);
    fread(buffer, 1, size, f);
    buffer[size] = '\0';

    fclose(f);
    return buffer;
}

static void skip_spaces(const char **src) {
    while (isspace(**src) && **src != '\n') (*src)++;
}

static void skip_line(const char **src) {
    while (**src && **src != '\n') (*src)++;
    if (**src == '\n') (*src)++;
}

static int match(const char **src, const char *kw) {
    size_t len = strlen(kw);
    if (strncmp(*src, kw, len) == 0) {
        *src += len;
        return 1;
    }
    return 0;
}

static void error(const char *msg) {
    fprintf(stderr, "[Parser error] %s\n", msg);
    exit(1);
}

// ------------------ parsing básico ------------------

static char *parse_identifier(const char **src) {
    skip_spaces(src);
    const char *start = *src;
    if (!isalpha(**src)) error("Esperado identificador");
    while (isalnum(**src)) (*src)++;
    size_t len = *src - start;
    char *id = malloc(len + 1);
    strncpy(id, start, len);
    id[len] = '\0';
    return id;
}

static int parse_number(const char **src) {
    skip_spaces(src);
    int sign = 1;
    if (**src == '-') {
        sign = -1;
        (*src)++;
    }
    if (!isdigit(**src)) error("Esperado número após '-'");
    int value = 0;
    while (isdigit(**src)) {
        value = value * 10 + (**src - '0');
        (*src)++;
    }
    return value * sign;
}

// ------------------ parsing de expressões ------------------

static ASTNode *parse_factor(const char **src);
static ASTNode *parse_term(const char **src);
static ASTNode *parse_expression(const char **src);

static ASTNode *parse_factor(const char **src) {
    skip_spaces(src);
    if (**src == '(') {
        (*src)++; // consume '('
        ASTNode *expr = parse_expression(src);
        if (**src != ')') error("Esperado ')'");
        (*src)++;
        return expr;
    } else if (isdigit(**src) || **src == '-') {
        int value = parse_number(src);
        return create_number_node(value);
    } else {
        char *var = parse_identifier(src);
        ASTNode *node = create_variable_node(var);
        free(var);
        return node;
    }
}

static ASTNode *parse_term(const char **src) {
    ASTNode *node = parse_factor(src);
    skip_spaces(src);
    while (**src == '*') {
        (*src)++; // consume '*'
        ASTNode *right = parse_factor(src);
        node = create_binary_node(OP_MUL, node, right);
        skip_spaces(src);
    }
    return node;
}

static ASTNode *parse_expression(const char **src) {
    ASTNode *node = parse_term(src);
    skip_spaces(src);
    while (**src == '+' || **src == '-') {
        char op = **src;
        (*src)++;
        ASTNode *right = parse_term(src);
        node = create_binary_node(
            op == '+' ? OP_ADD : OP_SUB,
            node,
            right
        );
        skip_spaces(src);
    }
    return node;
}

// ------------------ parsing de atribuições ------------------

static Assignment *parse_assignment(const char **src) {
    skip_spaces(src);
    char *var = parse_identifier(src);
    skip_spaces(src);
    if (**src != '=') error("Esperado '='");
    (*src)++;
    ASTNode *value = parse_expression(src);
    skip_spaces(src);
    if (**src != ';') error("Esperado ';'");
    (*src)++;
    skip_spaces(src);
    if (**src == '\n') (*src)++;
    Assignment *assign = create_assignment(var, value);
    free(var);
    return assign;
}

// ------------------ parsing principal ------------------ 

Program *parse_lng_file(const char *filename) {
    char *src = read_file(filename);
    if (!src) {
        fprintf(stderr, "Não foi possível abrir %s\n", filename);
        return NULL;
    }

    const char *p = src;
    skip_spaces(&p);

    if (!match(&p, "PROGRAMA")) error("Esperado 'PROGRAMA'");
    skip_spaces(&p);

    if (*p != '"') error("Esperado nome do programa entre aspas");
    p++;
    while (*p && *p != '"') p++;
    if (*p != '"') error("Aspas não fechadas no nome");
    p++;
    if (*p != ':') error("Esperado ':' após nome do programa");
    p++;
    if (*p != '\n') error("Esperado quebra de linha após ':'");
    p++;

    if (!match(&p, "INICIO")) error("Esperado 'INICIO'");
    if (*p != '\n') error("Esperado quebra de linha após 'INICIO'");
    p++;

    Program *prog = create_program();

    while (1) {
        skip_spaces(&p);
        if (strncmp(p, "RES", 3) == 0 || strncmp(p, "FIM", 3) == 0) break;
        Assignment *a = parse_assignment(&p);
        add_assignment(prog, a);
    }

    skip_spaces(&p);
    printf("DEBUG - Esperando RES, trecho atual: \"%.20s\"\n", p);
    if (!match(&p, "RES")) error("Esperado 'RES'");
    skip_spaces(&p);
    if (*p != '=') error("Esperado '=' após 'RES'");

    p++;
    ASTNode *res = parse_expression(&p);
    if (*p != ';') error("Esperado ';' após expressão de RES");
    p++;
    if (*p == '\n') p++;

    Assignment *resAssign = create_assignment("RES", res);
    add_assignment(prog, resAssign);

    if (!match(&p, "FIM")) error("Esperado 'FIM' no final do programa");

    free(src);
    return prog;
}