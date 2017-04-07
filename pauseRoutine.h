#ifndef PAUSE_H
#define PAUSE_H


#include <stdio.h>
#include <string.h>
#include "initialState.h"


#define PAUSEOPS 6
#define MAXSTRLEN 20

#define CONTINUE 1
#define RETURN 0

char* ops[PAUSEOPS] = {
	"continue",
	"pReg",
	"pStack", 
	"pMem",
	"reg",
	"conditions"
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
	printf("pc: %"PRIu64"\n", s->pc);
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

unsigned char reg(state *s) {
	int reg = 0;
	printf("register number\n");
	scanf("%d", &reg);
	printf("\nreg value: %"PRIu64"\n", s->registers[reg]);
	return CONTINUE;
}

unsigned char pCond(state *s) {
	printf("ZFlag %"PRIu64"\n", s->flags[ZFLAG]);
	printf("OFlag %"PRIu64"\n", s->flags[OFLAG]);
	printf("SFlag %"PRIu64"\n", s->flags[SFLAG]);

	return CONTINUE;
}

unsigned char (*pauseFuncs[PAUSEOPS])(state *s)  = {stop, pReg, pStack, pMem,
		reg, pCond};




#endif /* PAUSE_H */





