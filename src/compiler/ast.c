#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "ast.h"

ASTNode *create_number_node(int value) {
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = AST_NUMBER;
    node->number = value;
    return node;
}

ASTNode *create_variable_node(const char *name) {
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = AST_VARIABLE;
    node->var = strdup(name); // copia o nome da variável
    return node;
}

ASTNode *create_binary_node(BinOpType op, ASTNode *left, ASTNode *right) {
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = AST_BINARY_OP;
    node->binary.op = op;
    node->binary.left = left;
    node->binary.right = right;
    return node;
}

Assignment *create_assignment(const char *name, ASTNode *value) {
    Assignment *assign = malloc(sizeof(Assignment));
    assign->name = strdup(name);
    assign->value = value;
    return assign;
}

Program *create_program() {
    Program *prog = malloc(sizeof(Program));
    prog->assignments = NULL;
    prog->assignment_count = 0;
    return prog;
}

void add_assignment(Program *program, Assignment *assign) {
    program->assignment_count++;
    program->assignments = realloc(
        program->assignments,
        program->assignment_count * sizeof(Assignment *)
    );
    program->assignments[program->assignment_count - 1] = assign;
}

void free_ast(ASTNode *node) {
    if (!node) return;
    switch (node->type) {
        case AST_NUMBER:
            break;
        case AST_VARIABLE:
            free(node->var);
            break;
        case AST_BINARY_OP:
            free_ast(node->binary.left);
            free_ast(node->binary.right);
            break;
    }
    free(node);
}

void free_program(Program *program) {
    if (!program) return;
    for (int i = 0; i < program->assignment_count; i++) {
        Assignment *a = program->assignments[i];
        free(a->name);
        free_ast(a->value);
        free(a);
    }
    free(program->assignments);
    free(program);
}
