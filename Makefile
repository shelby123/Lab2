CC = gcc
CFLAGS = -Wall

assembler: y86-assembler.c y86-assembler.h
	$(CC) -o assembler y86-assembler.c
simulator: y86-simulator.c y86-simulator.h state.h
	$(CC) -o simulator y86-simulator.c
tester: y86-simulator.c y86-simulator.h state.h tester.c tester.h
	$(CC) -o tester tester.c 