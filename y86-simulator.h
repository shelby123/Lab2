#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>

// initial state includes util and state
#include "initialState.h"
#include "pauseRoutine.c"


#define FILENAME 1

#define REGA 0
#define REGB 1






/* drivers */
state* runSimulation(char* fileName);
void executeInstruction(state *s);

/* helpers */
unsigned char getInstructionCode(state *s);
unsigned char getFunctionCode(state *s);
char* getRegisters(state *s);
uint64_t getQuadWord(state *s);
uint64_t getQuadWordLoc(state *s, uint64_t location);
void setCondCodes(state *s, uint64_t val1, uint64_t val2, uint64_t res, char func);

/* instruction execution methods */
void halt(state *s);
void nop(state *s);
void rrmovq(state *s);
void irmovq(state *s);
void rmmovq(state *s);
void mrmovq(state *s);
void opq(state *s);
void branch(state *s);
void call(state *s);
void ret(state *s);
void pushq(state *s);
void popq(state *s);
void pause(state *s);

void (*pFuncs[13])() = {halt, nop, rrmovq, irmovq, rmmovq,
			mrmovq, opq, branch, call, ret, pushq, popq, pause};

/* op functions */
uint64_t addq(state *s, uint64_t val1,  uint64_t val2);
uint64_t subq(state *s, uint64_t val1,  uint64_t val2);
uint64_t andq(state *s, uint64_t val1,  uint64_t val2);
uint64_t xorq(state *s, uint64_t val1,  uint64_t val2);
uint64_t (*opFuncs[NUMOPQ])(state *s, uint64_t val1, uint64_t val2) = 
			{addq, subq, andq, xorq};


/* conditional checks */ 
char always(state *s);
char lessThan(state *s);
char lessThanEqual(state *s);
char equal(state *s);
char notEqual(state *s);
char greaterThanEqual(state *s);
char greaterThan(state *s);
char (*conditional[NUMBRANCH])(state *s) = 
			{always, lessThanEqual, lessThan, equal, 
			notEqual, greaterThanEqual, greaterThan};





















