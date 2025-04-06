#include <stdio.h>
#include <stdlib.h>
#include "compiler/neander_compiler.h"

int main(int argc, char* argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Uso: %s entrada.lng saida.asm\n", argv[0]);
        return 1;
    }

    const char* inputPath = argv[1];
    const char* outputPath = argv[2];

    if (!parse_lng_file(inputPath, outputPath)) {
        fprintf(stderr, "Erro ao compilar o arquivo.\n");
        return 1;
    }

    printf("Compilação concluída com sucesso: %s -> %s\n", inputPath, outputPath);
    return 0;
}
