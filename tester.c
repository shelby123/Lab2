

#include "tester.h"

#define NUMTESTS 13
#define DEBUGTESTER 1

#define ARRAYLOC 4096

char* assemblerFiles[NUMTESTS] = {"tests/test1.txt", 
							"tests/test2.txt",
							"tests/test3.txt",
							"tests/test4.txt",
							"tests/test5.txt",
							"tests/test6.txt",
							"tests/test7.txt",
							"tests/test8.txt",
							"tests/fibTest.txt",
							"tests/fibTest-2.txt",
							"tests/fibTest-3.txt",
							"tests/test9.txt",
							"tests/test10.txt"};

char* expectedFiles[NUMTESTS] = {"tests/test1Res.txt",
							"tests/test2Res.txt",
							"tests/test3Res.txt",
							"tests/test4Res.txt",
							"tests/test5Res.txt",
							"tests/test6Res.txt",
							"tests/test7Res.txt",
							"tests/test8Res.txt",
							"tests/fibTestRes.txt",
							"tests/fibTestRes-2.txt",
							"tests/fibTestRes-3.txt",
							"tests/test9Res.txt",
							"tests/test10Res.txt"};
							
char* testNames[NUMTESTS] = {"1: irmovq + addq",
							"2: irmovq + subq",
							"3: rmmovq + rrmovq + andq",
							"4: irmovq + xorq",
							"5: jmp success cases",
							"6: cmove success cases",
							"7: call + ret",
							"8: push + pop",
							"9: fib base case 0",
							"10: fib base case 1",
							"11: fib with n = 2"
							"12: multiple push/ pop"
							"13: multiple push/ pop"};



int main() {
	// for(int testNum = 0; testNum < NUMTESTS; testNum++) {
	// 	runTest(assemblerFiles[testNum], 
	// 			expectedFiles[testNum],
	// 			testNames[testNum]);
	// }
	// runFibTests();
	runQuickSortTests();
}

#define NUMFIBTESTS 4
char* fibTestFiles[NUMFIBTESTS] = 
	{
		"tests/fibTests/test1.txt",
		"tests/fibTests/test2.txt",
		"tests/fibTests/test3.txt",
		"tests/fibTests/test4.txt"
	};
int fibResults[NUMFIBTESTS] = 
	{
		2, 
		3, 
		5, 
		89
	};

void runFibTests() {
	printf("\n\n\n********************************\nRunning Fib tests\n");
	for(int i = 0; i < NUMFIBTESTS; i++) {
		state *result = runSimulation(fibTestFiles[i]);
		if(result->registers[0] == fibResults[i]) {
			printf("passed fib test %d\n", i);
		} else {
			printf("FAILED FIB TEST %d : \n", i);
			printf("\tExpected: %d, actual: %"PRIu64"\n", 
				fibResults[i], result->registers[0]);
		}
		free(result);
	}
}


#define NUMQUICKSORTTESTS 1
// char* quickSortTestFiles[NUMQUICKSORTTESTS] = 
// 	{
		
// 		"tests/quickSortTests/qsTest3.txt",
// 		"tests/quickSortTests/qsTest4.txt",
// 		"tests/quickSortTests/qsTest5.txt",
// 		"tests/quickSortTests/qsTest6.txt"
// 	};

// char* quickSortResultFiles[NUMQUICKSORTTESTS] = 
// 	{
		
// 		"tests/quickSortTests/qsTest3Res.txt",
// 		"tests/quickSortTests/qsTest4Res.txt",
// 		"tests/quickSortTests/qsTest5Res.txt",
// 		"tests/quickSortTests/qsTest6Res.txt"
// 	};

// int numElems[NUMQUICKSORTTESTS] = 
// 	{
// 		4,
// 		11,
// 		101,
// 		1001
// 	};

char* quickSortTestFiles[NUMQUICKSORTTESTS] = 
	{
		"tests/quickSortTests/qsTest5.txt",
		
	};

char* quickSortResultFiles[NUMQUICKSORTTESTS] = 
	{
	
		"tests/quickSortTests/qsTest5Res.txt",
	};

int numElems[NUMQUICKSORTTESTS] = 
	{
		101
	};

void runQuickSortTests() {
	printf("\n\nRunning Quick Sort tests\n");
	for(int i = 0; i < NUMQUICKSORTTESTS; i++) {
		state *result = runSimulation(quickSortTestFiles[i]);
		state *expectedState = stalloc();
		FILE *file = fopen(quickSortResultFiles[i], "r");
		if(DEBUGTESTER)
			printf("\nLoad expected values into memory\n");

		loadInMemValues(file, expectedState);
		if(quickSortArraysEqual(expectedState, result, numElems[i])) {
			printf("passed quick sort test %d\n", i);
		} else {
			printf("FAILED QUICK SORT TEST %d\n", i);
		}
		free(result);
		free(expectedState);
		fclose(file);
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
				printf("\tactual register %s: %" PRIu64 ", expected register val: %" PRIu64 "\n",
					registerNames[i],
					  act[i], 
					  exp[i]);
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
				printf("\tactual memory location %d: %d, expected memory location: %d\n",
					i,
					  actual->memory[i], 
					  expected->memory[i]);
			}
			return 0;
		}
	} 
	return 1;
}

int infoEqual(state *actual, state *expected) {
	if(actual->status != expected->status) {
		if(DEBUGTESTER) {
			printf("\tactual status: %" PRIu64 ", expected status: %" PRIu64 "\n",
				  actual->status, 
				  expected->status);
		}
	} else if(actual->flags[ZFLAG] != expected->flags[ZFLAG]) {
		if(DEBUGTESTER) {
			printf("\tactual zflag: %" PRIu64 ", expected zflag: %" PRIu64 "\n",
				  actual->flags[ZFLAG], 
				  expected->flags[ZFLAG]);
		}
	} else if(actual->flags[SFLAG] != expected->flags[SFLAG]) {
		if(DEBUGTESTER) {
			printf("\tactual sflag: %" PRIu64 ", expected sflag: %" PRIu64 "\n",
				  actual->flags[SFLAG], 
				  expected->flags[SFLAG]);
		}
	} else if(actual->flags[OFLAG] != expected->flags[OFLAG]) {
		if(DEBUGTESTER) {
			printf("\tactual oflag: %" PRIu64 ", expected oflag: %" PRIu64 "\n",
				  actual->flags[OFLAG], 
				  expected->flags[OFLAG]);
		}
	} else if(actual->pc != expected->pc) {
		if(DEBUGTESTER) {
			printf("\tactual pc: %" PRIu64 ", expected pc: %" PRIu64 "\n",
				  actual->pc, 
				 expected->pc);
		}
	} else {
		return 1;
	}
	return 0;

}

int quickSortArraysEqual(state *exp, state *act, int numElems) {
	uint64_t loc = ARRAYLOC;
	uint64_t stop = numElems*8 + ARRAYLOC;
	printf("%u\n", (unsigned char) act->memory[4096] );
	printf("%u\n", (unsigned char) exp->memory[4096] );
	for(int i = loc; i < stop; i++) {
		unsigned char c  = exp->memory[i];
		unsigned char c1 = act->memory[i];
		if(c != c1) {
			if(DEBUGTESTER) {
				printf("\tthe actual memory location %"PRIu64": %u, expected memory location: %u\n",
					loc,
					  c1, 
					  c);
			}
			return 0;
		}
	}
	return 1;
}





































