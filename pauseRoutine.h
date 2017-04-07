#ifndef PAUSE_H
#define PAUSE_H


#include <stdio.h>
#include <string.h>
#include "initialState.h"


#define PAUSEOPS 4
#define MAXSTRLEN 20

#define CONTINUE 1
#define RETURN 0

char* ops[PAUSEOPS] = {
	"continue",
	"pReg",
	"pStack", 
	"pMem"
};

void runPauseRoutine(state *s);
int getOp(void);
unsigned char runOp(int op);
uint64_t getQuad(state *s, uint64_t location) ;



unsigned char stop(state *s) {
	return RETURN;
}

unsigned char pReg(state *s) {
	printRegisters(s);
	return CONTINUE;
}

unsigned char pStack(state *s) {
	uint64_t loc = s->registers[RSP];
	while(loc != MEMSIZE) {
		printf("%"PRIu64": %"PRIu64"\n", loc, getQuad(s, loc));
		loc += 8;
	}

	return CONTINUE;
}

unsigned char pMem(state *s) {
	

	uint64_t lower = 0; 
	uint64_t upper = 0;
	printf("enter lower bound:\n");
	scanf("%"PRIu64"", &lower);
	printf("enter an upper bound\n");
	scanf("%"PRIu64"", &upper);
	if( lower > upper ) {
		printf("lower cannot be larger than upper \n");
		return pMem(s);
	}
	printf("\nShowing mem from locations %"PRIu64" to %"PRIu64"\n", lower, upper);
	for(uint64_t temp = lower; temp <= upper; temp++) {
		printf("%"PRIu64" : %d\n", temp, (unsigned char)s->memory[temp]);
	}

	return CONTINUE;
}


unsigned char (*pauseFuncs[PAUSEOPS])(state *s)  = {stop, pReg, pStack, pMem};




#endif /* PAUSE_H */