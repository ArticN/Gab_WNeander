// src/compiler/ast.h

#ifndef AST_H
#define AST_H

typedef enum {
    AST_NUMBER,
    AST_VARIABLE,
    AST_BINARY_OP
} ASTNodeType;

typedef enum {
    OP_ADD,
    OP_SUB,
    OP_MUL
} BinOpType;

typedef struct ASTNode {
    ASTNodeType type;
    union {
        int number; // AST_NUMBER
        char *var;  // AST_VARIABLE
        struct {
            BinOpType op;
            struct ASTNode *left;
            struct ASTNode *right;
        } binary; // AST_BINARY_OP
    };
} ASTNode;

typedef struct Assignment {
    char *name;
    ASTNode *value;
} Assignment;

typedef struct Program {
    Assignment **assignments;  // lista de atribuições
    int assignment_count;
} Program;

ASTNode *create_number_node(int value);
ASTNode *create_variable_node(const char *name);
ASTNode *create_binary_node(BinOpType op, ASTNode *left, ASTNode *right);
Assignment *create_assignment(const char *name, ASTNode *value);
Program *create_program();
void add_assignment(Program *program, Assignment *assign);
void free_ast(ASTNode *node);
void free_program(Program *program);

#endif
