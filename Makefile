CC = gcc
CFLAGS = -Wall
OBJECTS = tester simulator assembler initialState.h.gch state.h.gch  util.h.gch

assembler: y86-assembler.c y86-assembler.h
	$(CC) -o assembler y86-assembler.c
simulator: y86-simulator.c y86-simulator.h state.h y86-simulator.c util.h pauseRoutine.c
	$(CC) -c util.h
	$(CC) -c state.h
	$(CC) -c initialState.h
	$(CC) -c pauseRoutine.c
	$(CC) -o simulator y86-simulator.c
tester: y86-simulator.c y86-simulator.h state.h tester.c tester.h y86-simulator.c util.h pauseRoutine.c
	$(CC) -c util.h
	$(CC) -c state.h
	$(CC) -c initialState.h
	$(CC) -c y86-simulator.c
	$(CC) -c pauseRoutine.c
	$(CC) -o tester tester.c 
clean: 
	rm -f $(OBJECTS)
