#ifndef PAUSE_H
#define PAUSE_H


#include <stdio.h>
#include <string.h>
#include "initialState.h"


#define PAUSEOPS 3
#define MAXSTRLEN 20

#define CONTINUE 1
#define RETURN 0

char* ops[PAUSEOPS] = {
	"continue",
	"pReg",
	"pStack"
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


unsigned char (*pauseFuncs[PAUSEOPS])(state *s)  = {stop, pReg, pStack};




#endif /* PAUSE_H */