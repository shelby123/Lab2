#include <stdint.h>
#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include "instructions.h"



#define MEMSIZE 16777215
#define BYTESIZE 1
#define NUMREG 15

#define NUMFLAGS 3
#define ZFLAG 0
#define SFLAG 1
#define OFLAG 2

#define NORMAL 0
#define HALT 1

#define RSP 4
#define INITIALSTACKPOS (MEMSIZE)


typedef struct st {

	uint64_t status;
	
	uint64_t flags[NUMFLAGS];

	uint64_t registers[NUMREG];

	char memory[MEMSIZE];

	uint64_t pc;


} state;


state* stalloc() {
	state *res = calloc(1, sizeof(state));
	res->registers[RSP] = INITIALSTACKPOS;
	return res;
}

void printRegisters(state *s) {
	for(int i = 0; i < NUMREG; i++)  {
		printf("%s : %"PRId64"\n", registerNames[i], s->registers[i]);
	}
}
















