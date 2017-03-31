/*
 * Note: 
 *  - currently doesn't account for multiple memory 
 *  values for the same memory location.
 *
 */


#include "y86-simulator.h"
 
#define DEBUG 1



/* 
 * run the simulater with the file to read from as 
 * the first argument of the command line.
 * (Wrapper for command line start)
 */ 
// int main(int argc, char** argv ) {
//     assert(argc >= 2);
//     if(DEBUG)
//         printf("Starting the simulator\n");
//     char* fileName = argv[FILENAME];
//     runSimulation(fileName);
// }

/*
 * Run the simulation, given the input file name.
 */
state* runSimulation(char* fileName) {
    FILE *file = fopen(fileName, "r");

    state *s = stalloc();
    loadInMemValues(file, s);

    while(s->status == NORMAL) {
    	executeInstruction(s);
    }

    fclose(file);
    return s;
}


/*
 *	pre: s != null
 *	post: update state after executing one
 *	instruction starting at pc.
 *	pc will be updated. 
 */
void executeInstruction(state *s) {
	unsigned char instrCode = getInstructionCode(s);
	if(DEBUG) 
		printf("\n-instruction number %d about to execute \n", instrCode);
	if(instrCode >= NUMINSTR) {
		char message[] = "instruction code %d invalid. Halting the execution.";
		char copyTo[30];
		sprintf(copyTo, message, instrCode);
		warning(copyTo);
	}
	(*pFuncs[instrCode])(s);

}


/**********************************************************
helpers
***********************************************************/

/*
 * 	pre: s != null
 *	post: returns an integer indicating the 
 *	instruction code. 
 */
unsigned char getInstructionCode(state *s) {
	uint64_t pc = s->pc;
	unsigned char byte = s->memory[pc];
	unsigned char mask = 0xF0;	
	return (byte & mask)>>4;
}

/*
 * 	pre: s != null. 
 *	post: returns an array representing the 
 *	two register numbers. will return what is 
 *	located at the current pc
 */
char* getRegisters(state *s) {
	char* registers = calloc(2, sizeof(char));
	unsigned char value = s->memory[s->pc];
	registers[REGA] = (value & 0xF0) >> 4;
	registers[REGB] = (value & 0x0F);
	return registers;
}

/*
 * 	pre: s!= null
 *	post: read in 8 bytes from the current 
 *	pc. Does NOT update the PC.
 */
uint64_t getQuadWord(state *s) {
	uint64_t acc = 0;
	uint64_t readIn = s->pc;
	for(int i = 0; i < 8; i++) {
		acc += (s->memory[readIn]) << i*8;
		readIn++;
	}
	return acc;
}




/************************************************************
instruction simulation functions
************************************************************/

/* 
 * 	pre: s != null, pc still pointing to the location
 * 	in memory with the byte code instruction.
 * 	post: set the s status to halt
 */
void halt(state *s) {
	s->status = HALT;
}

/* 
 * 	pre: s != null, pc still pointing to the location
 * 	in memory with the byte code instruction.
 * 	post: do not alter any memory or flags. update
 *	the program counter by one byte.
 */
void nop(state *s) {
	if(DEBUG) 
		printf("executing a nop isntruction. Old pc was %lld", 
			(long long int) s->pc);
	s->pc++;
	if(DEBUG) 
		printf(" New pc is %lld\n", 
			(long long int) s->pc);
}

/* 
 * 	pre: s != null, pc still pointing to the location
 * 	in memory with the byte code instruction.
 * 	post: do not alter any memory or flags. update
 *	the program counter by two bytes
 */
void rrmovq(state *s) {
	// advance the pc after getting the instruction code
	s->pc++;
	
	char* registers = getRegisters(s);
	// advance the pc past the register byte
	s->pc++;

	//put what is in register A into register B.
	if(DEBUG) 
		printf("putting the contents of register %d into %d\n", registers[REGA], registers[REGB]);
	
	s->registers[registers[REGB]] = s->registers[registers[REGA]];

	free(registers);
}

/* 
 * 	pre: s != null, pc still pointing to the location
 * 	in memory with the byte code instruction.
 * 	post: do not alter any memory or flags. Update the value of 
 *	register b to be the immediate. Update the pc by 10
 */
void irmovq(state *s) {
	// advance the pc after getting the instruction code
	s->pc++;
	
	char* registers = getRegisters(s);
	// advance the pc past the register byte
	s->pc++;

	if(registers[REGA] != 0xF) {
		warning("executing an irmovq instruction, but regA location not 0xF");
	}

	char dest = registers[REGB];
	free(registers);

	// read in the immediate from memory.
	uint64_t immediate = getQuadWord(s);
	s->pc += 8;

	// put the immediate into the register
	if(DEBUG) 
		printf("putting immediate %lld into register %d\n", (long long int) immediate, dest);

	s->registers[dest] = immediate;

}















