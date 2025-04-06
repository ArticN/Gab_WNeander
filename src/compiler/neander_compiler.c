#include "compiler/neander_compiler.h"
#include "compiler/neander_expr.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_VARIABLES 128
#define BASE_VAR_ADDRESS 0xC0
#define RES_ADDRESS 0xFF

typedef struct {
    char name[32];
    unsigned char address;
} Variable;

static int tmpCount = 0;
static int labelCount = 0;
static int varCount = 0;

static Variable symbolTable[MAX_VARIABLES];
static unsigned char nextAddress = BASE_VAR_ADDRESS;

static int find_variable(const char* name) {
    for (int i = 0; i < varCount; i++) {
        if (strcmp(symbolTable[i].name, name) == 0) return i;
    }
    return -1;
}

static unsigned char allocate_variable(const char* name) {
    int idx = find_variable(name);
    if (idx != -1) return symbolTable[idx].address;

    strcpy(symbolTable[varCount].name, name);
    symbolTable[varCount].address = nextAddress++;
    return symbolTable[varCount++].address;
}

static void process_line(char* line) {
    while (isspace(*line)) line++;
    if (*line == '\0' || strchr(line, '=') == NULL) return;

    // Extrai nome da variável antes do '='
    char varName[32] = {0};
    sscanf(line, "%31s", varName);
    for (int i = 0; varName[i]; i++) {
        if (varName[i] == '=' || varName[i] == ';') {
            varName[i] = '\0';
            break;
        }
    }

    if (strcmp(varName, "RES") != 0) {
        allocate_variable(varName);
    } else if (find_variable("RES") == -1) {
        strcpy(symbolTable[varCount].name, "RES");
        symbolTable[varCount].address = RES_ADDRESS;
        varCount++;
    }

    // Agora extrai a expressão após o '='
    char* exprStart = strchr(line, '=');
    if (exprStart) {
        exprStart++; // pula '='
        // Remove o ';' final, se houver
        char* semicolon = strchr(exprStart, ';');
        if (semicolon) *semicolon = '\0';

        TokenList postfix;
        if (infix_to_postfix(exprStart, &postfix)) {
            printf("Expressão de %s em postfix: ", varName);
            print_token_list(&postfix);
        } else {
            fprintf(stderr, "Erro ao processar expressão: %s\n", exprStart);
        }
    }
}

static const char* new_tmp() {
    static char buffer[16];
    snprintf(buffer, sizeof(buffer), "TMP%d", tmpCount++);
    return strdup(buffer); // você pode usar malloc aqui se quiser mais controle
}

void generate_asm_from_postfix(FILE* out, const TokenList* postfix, const char* target) {
    const char* stack[64];
    int top = -1;

    for (int i = 0; i < postfix->count; i++) {
        const char* token = postfix->items[i];

        if (strcmp(token, "+") == 0 || strcmp(token, "-") == 0 || strcmp(token, "*") == 0) {
            const char* rhs = stack[top--];
            const char* lhs = stack[top--];
            const char* result = (i == postfix->count - 1) ? target : new_tmp();

            if (strcmp(token, "*") == 0) {
                int localLabel = labelCount++;

                fprintf(out,
                    "        LDA %s\n"
                    "        STA TMP_MUL_LHS\n"
                    "        LDA %s\n"
                    "        STA TMP_MUL_COUNT\n"
                    "        LDA ZERO\n"
                    "        STA %s\n"
                    "MUL_LOOP_%d:\n"
                    "        LDA %s\n"
                    "        ADD TMP_MUL_LHS\n"
                    "        STA %s\n"
                    "        LDA TMP_MUL_COUNT\n"
                    "        SUB ONE\n"
                    "        STA TMP_MUL_COUNT\n"
                    "        JZ MUL_END_%d\n"
                    "        JMP MUL_LOOP_%d\n"
                    "MUL_END_%d:\n",
                    lhs, rhs, result,
                    localLabel, result, result,
                    localLabel, localLabel, localLabel
                );
            } else {
                fprintf(out,
                    "        LDA %s\n"
                    "        %s %s\n"
                    "        STA %s\n",
                    lhs,
                    strcmp(token, "+") == 0 ? "ADD" : "SUB",
                    rhs,
                    result
                );
            }

            stack[++top] = result;
        } else {
            stack[++top] = token;
        }
    }
}

bool parse_lng_file(const char* input_path, const char* output_path) {
    FILE* in = fopen(input_path, "r");
    if (!in) {
        perror("Erro ao abrir arquivo de entrada");
        return false;
    }

    // Primeira passada: construir tabela de símbolos
    char line[256];
    while (fgets(line, sizeof(line), in)) {
        process_line(line);  // Essa função aloca variáveis e reconhece RES
    }
    fclose(in);

    // Segunda passada: gerar código ASM
    FILE* out = fopen(output_path, "w");
    if (!out) {
        perror("Erro ao criar arquivo de saída");
        return false;
    }

    fprintf(out, "        ; Código gerado pelo compilador Neander Gabriel\n\n");

    // Reiniciar contadores temporários
    tmpCount = 0;

    in = fopen(input_path, "r");
    if (!in) {
        perror("Erro ao reabrir arquivo para geração de código");
        return false;
    }

    while (fgets(line, sizeof(line), in)) {
        char* ptr = line;
        while (isspace(*ptr)) ptr++;
        if (*ptr == '\0' || strchr(ptr, '=') == NULL) continue;

        char varName[32] = {0};
        sscanf(ptr, "%31s", varName);

        char* exprStart = strchr(ptr, '=');
        if (!exprStart) continue;
        exprStart++;

        char* semicolon = strchr(exprStart, ';');
        if (semicolon) *semicolon = '\0';

        TokenList postfix;
        if (infix_to_postfix(exprStart, &postfix)) {
            generate_asm_from_postfix(out, &postfix, varName);
        }
    }

    fclose(in);

    // Finaliza com HLT
    fprintf(out, "\n        HLT\n\n");

    // Constantes e temporários de multiplicação
    fprintf(out, "ZERO    OCT 0\n");
    fprintf(out, "ONE     OCT 1\n");
    fprintf(out, "TMP_MUL_LHS   SPACE\n");
    fprintf(out, "TMP_MUL_COUNT SPACE\n");

    // Variáveis do programa
    for (int i = 0; i < varCount; i++) {
        fprintf(out, "%s\tSPACE\n", symbolTable[i].name);
    }

    return true;
}

