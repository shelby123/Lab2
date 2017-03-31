

#include "tester.h"

#define NUMTESTS 1
#define DEBUGTESTER 1

char* assemblerFiles[NUMTESTS] = {"tests/test1.txt"};
char* expectedFiles[NUMTESTS] = {"tests/test1Res.txt"};
char* testNames[NUMTESTS] = {"Test 1"};



int main() {
	for(int testNum = 0; testNum < NUMTESTS; testNum++) {
		runTest(assemblerFiles[testNum], 
				expectedFiles[testNum],
				testNames[testNum]);
	}
}

void runTest(char *assembler, char *expected, char* testName) {
	if(DEBUGTESTER) 
		printf("\nAbout to run simulation on assembler file\n");
	state *resultingState = runSimulation(assembler);
	state *expectedState = stalloc();
	FILE *file = fopen(expected, "r");
	if(DEBUGTESTER)
		printf("\nLoad expected values into memory\n");

	loadInMemValues(file, expectedState);
	loadRegisters(file, expectedState);
	loadFlags(file, expectedState);
	loadStatusAndPC(file, expectedState);

	if(DEBUGTESTER) 
		printf("%s: checking if equal results\n", testName);
	if(statesEqual(resultingState, expectedState)) {
		printf("passed test %s\n", testName);
	} else {
		printf("FAILED TEST %s\n", testName);
	}
	free(expectedState);
	free(resultingState);
	fclose(file);
}

int statesEqual(state *actual, state *expected) {
	if(registersEqual(actual, expected)) {
		if(memoriesEqual(actual, expected)) {
			if(infoEqual(actual, expected)) {
				return 1;
			}
		}
	}
	return 0;
}


int registersEqual(state *actual, state *expected) {
	uint64_t *act = actual->registers;

	uint64_t *exp = expected->registers;
	for(int i = 0; i < NUMREG; i++) {
		if(act[i] != exp[i]) {
			if(DEBUGTESTER) {
				printf("\tactual register %d: %lld, expected register val: %lld\n",
					i,
					(long long int) act[i], 
					(long long int) exp[i]);
			}
			return 0;
		}
	}
	return 1;
}

int memoriesEqual(state *actual, state *expected) {
	for(int i = 0; i < MEMSIZE; i++) {
		if(actual->memory[i] != expected->memory[i]){ 
			if(DEBUGTESTER) {
				printf("\tactual memory location %d: %lld, expected memory location: %lld\n",
					i,
					(long long int) actual->memory[i], 
					(long long int) expected->memory[i]);
			}
			return 0;
		}
	} 
	return 1;
}

int infoEqual(state *actual, state *expected) {
	if(actual->status != expected->status) {
		if(DEBUGTESTER) {
			printf("\tactual status: %lld, expected status: %lld\n",
				(long long int) actual->status, 
				(long long int) expected->status);
		}
	} else if(actual->flags[ZFLAG] != expected->flags[ZFLAG]) {
		if(DEBUGTESTER) {
			printf("\tactual zflag: %lld, expected zflag: %lld\n",
				(long long int) actual->flags[ZFLAG], 
				(long long int) expected->flags[ZFLAG]);
		}
	} else if(actual->flags[SFLAG] != expected->flags[SFLAG]) {
		if(DEBUGTESTER) {
			printf("\tactual sflag: %lld, expected sflag: %lld\n",
				(long long int) actual->flags[SFLAG], 
				(long long int) expected->flags[SFLAG]);
		}
	} else if(actual->flags[OFLAG] != expected->flags[OFLAG]) {
		if(DEBUGTESTER) {
			printf("\tactual oflag: %lld, expected oflag: %lld\n",
				(long long int) actual->flags[OFLAG], 
				(long long int) expected->flags[OFLAG]);
		}
	} else if(actual->pc != expected->pc) {
		if(DEBUGTESTER) {
			printf("\tactual pc: %lld, expected pc: %lld\n",
				(long long int) actual->pc, 
				(long long int) expected->pc);
		}
	} else {
		return 1;
	}
	return 0;

}







































