


#define MEMSIZE 1000
#define BYTESIZE 1
#define NUMREG 15

typedef struct st {

	uint64_t status;
	uint64_t zflag, sflag, oflag;

	uint64_t *registers;

	uint64_t *memory;

	uint64_t pc;


} state;


state* stalloc() {
	state *res = calloc(1, sizeof(state));
	res->memory = calloc(MEMSIZE, 1);
	res->registers = calloc(NUMREG, sizeof(uint64_t));
	return res;
}