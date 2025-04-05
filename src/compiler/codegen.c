#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "codegen.h"

#define MAX_VARS 256

typedef struct {
    char *name;
    char label[16];
} Variable;

static Variable symbol_table[MAX_VARS];
static int var_count = 0;
static int temp_count = 0;
static int label_count = 0;

static const char *get_label(const char *name) {
    for (int i = 0; i < var_count; i++) {
        if (strcmp(symbol_table[i].name, name) == 0)
            return symbol_table[i].label;
    }
    snprintf(symbol_table[var_count].label, sizeof(symbol_table[var_count].label), "VAR_%s", name);
    symbol_table[var_count].name = strdup(name);
    return symbol_table[var_count++].label;
}

static const char *new_temp() {
    static char label[16];
    snprintf(label, sizeof(label), "TMP%d", temp_count++);
    return strdup(label);
}

static const char *new_label(const char *prefix) {
    static char label[16];
    snprintf(label, sizeof(label), "%s%d", prefix, label_count++);
    return strdup(label);
}

// --- Geração de código para expressões ---

static void gen_expr(FILE *out, ASTNode *node, const char *target);

static void gen_binary(FILE *out, ASTNode *node, const char *target) {
    const char *left_temp = new_temp();
    const char *right_temp = new_temp();

    gen_expr(out, node->binary.left, left_temp);
    gen_expr(out, node->binary.right, right_temp);

    // LDA esquerda
    fprintf(out, "LDA %s\n", left_temp);

    switch (node->binary.op) {
        case OP_ADD:
            fprintf(out, "ADD %s\n", right_temp);
            break;
        case OP_SUB:
            fprintf(out, "SUB %s\n", right_temp);
            break;
        case OP_MUL: {
            const char *loop = new_label("LOOP");
            const char *fim = new_label("FIM");

            fprintf(out, "STA __A\n");
            fprintf(out, "LDA %s\n", right_temp);
            fprintf(out, "STA __B\n");
            fprintf(out, "LDA ZERO\n");
            fprintf(out, "STA %s\n", target);
            fprintf(out, "%s:\n", loop);
            fprintf(out, "LDA __B\n");
            fprintf(out, "JZ %s\n", fim);
            fprintf(out, "SUB UM\n");
            fprintf(out, "STA __B\n");
            fprintf(out, "LDA %s\n", target);
            fprintf(out, "ADD __A\n");
            fprintf(out, "STA %s\n", target);
            fprintf(out, "JMP %s\n", loop);
            fprintf(out, "%s:\n", fim);
            return;
        }
    }

    fprintf(out, "STA %s\n", target);
}

static void gen_expr(FILE *out, ASTNode *node, const char *target) {
    switch (node->type) {
        case AST_NUMBER:
            fprintf(out, "LDA CONST_%d\n", node->number);
            fprintf(out, "STA %s\n", target);
            break;
        case AST_VARIABLE:
            fprintf(out, "LDA %s\n", get_label(node->var));
            fprintf(out, "STA %s\n", target);
            break;
        case AST_BINARY_OP:
            gen_binary(out, node, target);
            break;
    }
}

// --- Geração principal ---

void generate_code(FILE *out, Program *program) {
    fprintf(out, "INICIO\n");

    for (int i = 0; i < program->assignment_count; i++) {
        Assignment *a = program->assignments[i];
        const char *label = get_label(a->name);
        const char *temp = new_temp();
        gen_expr(out, a->value, temp);
        fprintf(out, "LDA %s\n", temp);
        fprintf(out, "STA %s\n", label);
    }

    fprintf(out, "LDA %s\n", get_label("RES"));
    fprintf(out, "FIM\n");

    // --- Seção de dados ---
    fprintf(out, "\n; Variáveis\n");
    for (int i = 0; i < var_count; i++) {
        fprintf(out, "%s: 0\n", symbol_table[i].label);
    }

    fprintf(out, "\n; Constantes auxiliares\n");
    fprintf(out, "ZERO: 0\n");
    fprintf(out, "UM: 1\n");
    fprintf(out, "__A: 0\n");
    fprintf(out, "__B: 0\n");
}