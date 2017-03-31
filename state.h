#include <stdint.h>
#include <inttypes.h>
#include <stdlib.h>


#define MEMSIZE 100
#define BYTESIZE 1
#define NUMREG 15

#define NUMFLAGS 3
#define ZFLAG 0
#define SFLAG 1
#define OFLAG 2

typedef struct st {

	uint64_t status;
	
	uint64_t flags[NUMFLAGS];

	uint64_t registers[NUMREG];

	uint64_t memory[MEMSIZE];

	uint64_t pc;


} state;


state* stalloc() {
	state *res = calloc(1, sizeof(state));
	return res;
}