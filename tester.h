#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>

#include "y86-simulator.c"

void runTest(char *assembler, char *expected, char* testName);
int statesEqual(state *actual, state *expected);
int registersEqual(state *actual, state *expected);
int memoriesEqual(state *actual, state *expected);
int infoEqual(state *actual, state *expected);
void runFibTests(void);
void runQuickSortTests(void);
int quickSortArraysEqual(state *exp, state *act, int numElems);