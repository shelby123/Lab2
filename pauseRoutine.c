
#include "pauseRoutine.h"



void runPauseRoutine(state *s) {
	unsigned char cont = 1;
	while(cont) {
		int op = getOp();
		if(op == -1) {
			printf("That is not a valid op. Here are the list of possible operations:\n");
			for(int i = 0; i < PAUSEOPS; i++) {
				printf("%s  ", ops[i]);
			}
			printf("\n");
		} else 
			cont = (*pauseFuncs[op])(s);
	}
}


int getOp(void) {
	char input[MAXSTRLEN];

	printf("Paused. Enter an operation:\n");
	scanf("%s.", input);

	for(int i = 0; i < PAUSEOPS; i++) {
		if(!strcmp(ops[i], input)) {
			return i;
		}
	}
	return -1;
}

/*
 * 	pre: s!= null
 *	post: read in 8 bytes from the current 
 *	pc. Does NOT update the PC.
 */
uint64_t getQuad(state *s, uint64_t location) {
	uint64_t acc = 0;
	uint64_t readIn = location;
	// least significant byte is first. 
	for(int i = 0; i < 8; i++) {
		uint64_t byte = (s->memory[readIn] & 0xFF);
		acc += (byte) << i*8;
		readIn++;
	}
	return acc;
}












