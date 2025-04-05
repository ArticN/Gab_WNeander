#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "codegen.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <arquivo.lng>\n", argv[0]);
        return 1;
    }

    const char *input_file = argv[1];
    ASTNode *program = parse_lng_file(input_file);
    if (!program) {
        fprintf(stderr, "Erro ao parsear o arquivo %s\n", input_file);
        return 1;
    }

    char asm_file[256];
    snprintf(asm_file, sizeof(asm_file), "build/Programa.asm");
    FILE *out = fopen(asm_file, "w");
    if (!out) {
        perror("Erro ao criar arquivo ASM");
        return 1;
    }

    generate_code(out, program);
    fclose(out);

    free_ast(program);
    printf("Arquivo ASM gerado com sucesso: %s\n", asm_file);
    return 0;
}
