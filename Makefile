# Makefile

CC = gcc
SRC = src/compiler/*.c
BIN = build/compiler

all: build run

build:
	mkdir -p build
	$(CC) -o $(BIN) $(SRC)

run:
	$(BIN) examples/Programa.lng

clean:
	rm -rf build
