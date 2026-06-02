C = gcc

all: chip8.o
	$(C) -o chip8 chip8.o

chip8.o: chip8.c
	$(C) -c chip8.c