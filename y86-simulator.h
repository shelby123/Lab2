#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>

// initial state includes util and state
#include "initialState.h"
#include "instructions.h"


#define FILENAME 1

#define REGA 0
#define REGB 1






/* drivers */
state* runSimulation(char* fileName);
void executeInstruction(state *s);

/* helpers */
unsigned char getInstructionCode(state *s);
char* getRegisters(state *s);
uint64_t getQuadWord(state *s);

/* instruction execution methods */
void halt(state *s);
void nop(state *s);
void rrmovq(state *s);
void irmovq(state *s);

void (*pFuncs[4])() = {halt, nop, rrmovq, irmovq};
