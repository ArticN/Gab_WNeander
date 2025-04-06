# Compila o compilador
build-compiler:
    gcc -Wall -Wextra -std=c11 -Isrc \
        cmd/compiler/main.c \
        src/compiler/neander_compiler.c \
        src/compiler/neander_expr.c \
        -o compiler.out

# Executa o compilador no .lng
run-compiler:
    ./compiler.out input/programalng.txt asm/Programa.asm

# Executa tudo
pipeline:
    just build-compiler
    just run-compiler

# Limpa
clean:
    rm -f *.out asm/*.asm bin/*.bin
