# Caminhos
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -I./pkg

# Compiladores
COMPILER_SRCS = cmd/compiler/main.c pkg/compiler/parser.c
ASSEMBLER_SRCS = cmd/assembler/main.c pkg/assembler/assembler.c

# Binaries
BIN_COMPILER = compiler.out
BIN_ASSEMBLER = assembler.out

all: $(BIN_COMPILER) $(BIN_ASSEMBLER)

$(BIN_COMPILER): $(COMPILER_SRCS)
	$(CC) $(CFLAGS) $(COMPILER_SRCS) -o $(BIN_COMPILER)

$(BIN_ASSEMBLER): $(ASSEMBLER_SRCS)
	$(CC) $(CFLAGS) $(ASSEMBLER_SRCS) -o $(BIN_ASSEMBLER)

clean:
	rm -f *.out asm/*.asm bin/*.bin

.PHONY: all clean
