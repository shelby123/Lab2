#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>

// initial state includes util and state
#include "initialState.h"


#define FILENAME 1






/* drivers */
state* runSimulation(char* fileName);

void test1() {

}

void test2() {
	
}


void (*pFuncs[2])() = {test1, test2};
