# Justfile

# Caminho do executável do compilador
binary := "build/compiler"

# Compila os arquivos C do compilador
build:
    mkdir -p build
    gcc -o {{binary}} src/compiler/*.c

# Executa o compilador com Programa.lng
run:
    just build
    {{binary}} examples/Programa.lng

# Compila e executa de uma vez
all: run

# Limpa arquivos gerados
clean:
    rm -rf build
