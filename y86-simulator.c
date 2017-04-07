
#include "y86-simulator.h"
 
#define DEBUG 0

#define COUNTINSTR 0

#define PRINTRESULT 0

#define COUNTINSTR 0


/* 
 * run the simulater with the file to read from as 
 * the first argument of the command line.
 * (Wrapper for command line start)
//  */ 
// int main(int argc, char** argv ) {
//     assert(argc >= 2);
//     if(DEBUG)
//         printf("Starting the simulator\n");
//     char* fileName = argv[FILENAME];
//     state *s = runSimulation(fileName);
//     if(argc >=3) {
//     	uint64_t memHigh = atoi(argv[FILENAME+1]);
//     	printRegisters(s);
//     	for(int i = 4096; i <= memHigh; i++) {
// 	    	printf("%d %d\n", i, (unsigned char)s->memory[i]);
// 	    }
// 	    // for(int i = 4096; i <= memHigh; i+=8) {
// 	    // 	printf("%d %"PRIu64"\n", i,  getQuadWordLoc(s, i));
// 	    // }

//     }
// }

/*
 * Run the simulation, given the input file name.
 */
state* runSimulation(char* fileName) {
	if(DEBUG) {
		printf("running the simulation on file name %s\n", fileName);
	}
    FILE *file = fopen(fileName, "r");

    state *s = stalloc();
    loadInMemValues(file, s);

    uint64_t numInstr = 0;
    while(s->status == NORMAL) {
    	executeInstruction(s);
    	numInstr++;

    }

    if(COUNTINSTR) 
    	printf("number of instructions : %"PRIu64"\n", numInstr);
    if(PRINTRESULT) 
    	printRegisters(s);

    if(COUNTINSTR) 
    	printf("number of instructions : %"PRIu64"\n", numInstr);

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
	unsigned char res = (byte & mask)>>4;
	if(res == PAUSE)
		res = 12;
	return res;
}

/*
 * 	pre: s != null
 *	post: returns an integer indicating the 
 *	function code. 
 */
unsigned char getFunctionCode(state *s) {
	uint64_t pc = s->pc;
	unsigned char byte = s->memory[pc];
	unsigned char mask = 0x0F;	
	return (byte & mask);
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
	if(DEBUG) {
		printf("reading a quad word at %" PRIu64 "\n",
			  readIn);
	}
	// least significant byte is first. 
	for(int i = 0; i < 8; i++) {
		uint64_t byte = (s->memory[readIn] & 0xFF);
		acc += (byte) << i*8;
		readIn++;
	}
	return acc;
}

/*
 *
 */
uint64_t getQuadWordLoc(state *s, uint64_t location) {
	uint64_t acc = 0;
	uint64_t readIn = location;
	if(DEBUG) {
		printf("reading a quad word at %" PRIu64 "\n",
			  readIn);
	}
	// least significant byte is first. 
	for(int i = 0; i < 8; i++) {
		uint64_t byte = (s->memory[readIn] & 0xFF);
		acc += (byte) << i*8;
		readIn++;
	}
	return acc;
}

/*
 * 	pre: s != null
 * 	post: set the condition flags for s based on the 
 *	two operands and the result. 
 *	ZFLAG: set when result is zero
 *	OFLAG: set when there is a sign change 
 *	SFLAG: set if the highest order bit in res is 1
 *	
 */
void setCondCodes(state *s, uint64_t val1, uint64_t val2, uint64_t res, char func) {
	uint64_t val1bit = val1 & 0x8000000000000000;
	uint64_t val2bit = val2 & 0x8000000000000000;
	uint64_t resbit = res & 0x8000000000000000;

	s->flags[ZFLAG] = res == 0;
	// set overflow if the two values had the same sign bit
	// but the operation changed the bit of the result.
	if(func == SUBQ) {
		// if we're doing neg - pos and we get a positive number, overflow
		if((val2bit && !val1bit) && !resbit) {
			s->flags[OFLAG] = 1;
		} else if( (!val2bit && val1bit) && resbit) {
			// we did pos - neg and got a negative number
			s->flags[OFLAG] = 1;
		} else
			s->flags[OFLAG] = 0;
	} else if(func == ADDQ) {
		if((val2bit && val1bit) && !resbit) {
			// added two negative numbers, got a positive
			s->flags[OFLAG] = 1;
		} else if (((!val1bit) && (!val2bit)) && resbit) {
			// added two positive numbers and got a negative
			s->flags[OFLAG] = 1;
		} else 
			s->flags[OFLAG] = 0;
	} else
		s->flags[OFLAG] = 0;


	s->flags[SFLAG] = resbit;
	if(s->flags[SFLAG]) 
		s->flags[SFLAG] = 1;

	if(DEBUG) {
		printf("after the op, the conditions codes are : \n");
		printf("\tz: %d, o: %d, s: %d\n", 
			(int) s->flags[ZFLAG], 
			(int) s->flags[OFLAG],
			(int) s->flags[SFLAG]);
	}
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
	if(DEBUG) {
		printf("halting. Register values are \n");
		printRegisters(s);
		printf("the pc is %"PRIu64"\n", s->pc);
	}
}

/* 
 * 	pre: s != null, pc still pointing to the location
 * 	in memory with the byte code instruction.
 * 	post: do not alter any memory or flags. update
 *	the program counter by one byte.
 */
void nop(state *s) {
	if(DEBUG) 
		printf("executing a nop isntruction. Old pc was %" PRIu64 "", 
			  s->pc);
	s->pc++;
	if(DEBUG) 
		printf(" New pc is %" PRIu64 "\n", 
			  s->pc);
}

/* 
 * 	pre: s != null, pc still pointing to the location
 * 	in memory with the byte code instruction.
 * 	post: do not alter any memory or flags. update
 *	the program counter by two bytes
 */
void rrmovq(state *s) {
	// get the function code 
	char functionCode = getFunctionCode(s);
	if(functionCode >= NUMMOVE) {
		char message[] = "RRMOVE function code %d invalid. Halting the execution.";
		char copyTo[30];
		sprintf(copyTo, message, functionCode);
		warning(copyTo);
	}
	// advance the pc after getting the instruction code
	s->pc++;
	
	char* registers = getRegisters(s);
	// advance the pc past the register byte
	s->pc++;

	// get values and call the function. 
	char move = 
		(*conditional[functionCode])(s);

	//put what is in register A into register B.
	if(DEBUG) { 
		if(move) {
			printf("putting the contents of register %d into %d\n", registers[REGA], registers[REGB]);
		} else  {
			printf("do not execute move. (condition false)\n");
		}
	}
	if(move)
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
		printf("putting immediate %" PRIu64 " into register %d\n",   immediate, dest);

	s->registers[dest] = immediate;
}

/* 
 * 	pre: s != null, pc still pointing to the location
 * 	in memory with the byte code instruction.
 * 	post: do not alter any memory or flags. Update the value in 
 *	memory pointed to by register b with the value in reg A.
 *	Update the pc by 10
 */
void rmmovq(state *s) {
	// advance the pc after getting the instruction code
	s->pc++;
	
	char* registers = getRegisters(s);
	// advance the pc past the register byte
	s->pc++;

	// read in the immediate from memory.
	uint64_t displacement = getQuadWord(s);
	s->pc += 8;

	uint64_t memoryLocation = s->registers[registers[REGB]] + displacement;

	// put the contents of register A into the memory location
	uint64_t value = s->registers[registers[REGA]];
	uint64_t mask = 0xFF;

	if(DEBUG) {
		printf("rmmovq: \n");
		printf("\tvalue from reg %d: %" PRIu64 "\n", registers[REGA],
			  value);
		printf("\tmemory location from reg %d:", registers[REGB]);
		printf(" %" PRIu64 " (displacement: %" PRIu64 ")\n", 
			  memoryLocation, 
			  displacement);

	}

	for(int i = 0; i < 8; i++) {
		char temp = (char)((value & mask)>>i*8);
		s->memory[memoryLocation] = temp;
		mask = mask << 8;
		memoryLocation++;
	}
	if(value == 65793) {
		printf("value at loc %"PRIu64" is %"PRIu64"\n",
			s->registers[registers[REGB]] + displacement,
			getQuadWordLoc(s, s->registers[registers[REGB]] + displacement));
	}

	free(registers);
}

/* 
 * 	pre: s != null, pc still pointing to the location
 * 	in memory with the byte code instruction.
 * 	post: do not alter any memory or flags. Update the
 * 	contents of register A with the value at location
 * 	pointed to by register B + the offset. 
 */
void mrmovq(state *s) {
	// advance the pc after getting the instruction code
	s->pc++;
	
	char* registers = getRegisters(s);
	// advance the pc past the register byte
	s->pc++;

	// read in the immediate from memory.
	uint64_t displacement = getQuadWord(s);
	s->pc += 8;


	uint64_t memoryLocation = s->registers[registers[REGB]] + displacement;

	// put the contents of register A into the memory location
	uint64_t savePc = s->pc;
	s->pc = memoryLocation;
	uint64_t value = getQuadWord(s);
	s->pc = savePc;

	s->registers[registers[REGA]] = value;

	if(DEBUG) {
		printf("mrmovq: \n");
		printf("\tvalue to reg %s: %" PRIu64 "\n", registerNames[registers[REGA]],
			  value);
		printf("\tmemory location from reg %s:", registerNames[registers[REGB]]);
		printf(" %" PRIu64 " (displacement: %" PRIu64 ")\n", 
			  memoryLocation, 
			  displacement);
	}


	free(registers);
}


/* 
 *	pre: s != null
 *	post: contents of regA are added to the 
 *	contents of regB. Condition flags are 
 *	set. 
 */
uint64_t addq(state *s, uint64_t val1, uint64_t val2) {
	uint64_t res = val1 + val2;
	if(DEBUG) {
		printf("Adding : %" PRIu64 " to : %" PRIu64 ", res: %" PRIu64 "\n",
			  val1,  val2, 
			  res);
	}
	return res;
}

/* 
 *	pre: s != null
 *	post: contents of regA are subtracted from the 
 *	contents of regB. Condition flags are 
 *	set. 
 */
uint64_t subq(state *s, uint64_t val1, uint64_t val2){
	uint64_t res = val2 - val1;
	if(DEBUG) {
		printf("subtracting: %" PRId64 " from : %" PRId64 ", res: %" PRId64 "\n",
			  val1,   val2, 
			  res);
	}
	return res;
}

/* 
 *	pre: s != null
 *	post: contents of regA are and-ed with the 
 *	contents of regB. Condition flags are 
 *	set. 
 */
uint64_t andq(state *s, uint64_t val1, uint64_t val2){
	uint64_t res = val2 & val1;
	if(DEBUG) {
		printf("and-ing : %" PRIu64 " with : %" PRIu64 ", res: %" PRIu64 "\n",
			  val1,   val2, 
			  res);
	}
	return res;
}
/* 
 *	pre: s != null
 *	post: contents of regA are xor-ed with the 
 *	contents of regB. Condition flags are 
 *	set. 
 */
uint64_t xorq(state *s, uint64_t val1, uint64_t val2) {
	uint64_t res = val2 ^ val1;
	if(DEBUG) {
		printf("xor-ing : %" PRIu64 " with : %" PRIu64 ", res: %" PRIu64 "\n",
			  val1,   val2, 
			  res);
	}
	return res;
}

/* 
 * 	pre: s != null, pc still pointing to the location
 * 	in memory with the byte code instruction.
 * 	post: do not alter any memory. Update the
 * 	contents of register A with the value at location
 * 	pointed to by register B + the offset. 
 */
void opq(state *s) {
	// get the function code 
	char functionCode = getFunctionCode(s);
	if(functionCode >= NUMOPQ) {
		char message[] = "OP function code %d invalid. Halting the execution.";
		char copyTo[30];
		sprintf(copyTo, message, functionCode);
		warning(copyTo);
	}

	// advance the pc after getting the instruction code
	s->pc++;
	
	char* registers = getRegisters(s);
	// advance the pc past the register byte
	s->pc++;

	// get values and call the function. 
	uint64_t val1 = s->registers[registers[REGA]];
	uint64_t val2 = s->registers[registers[REGB]];
	uint64_t res =
		(*opFuncs[functionCode])(s, val1, val2);
	setCondCodes(s, val1, val2, res, functionCode);
	s->registers[registers[REGB]] = res;

}

/*	
 * 	pre: s != null. condition codes set. 
 *	post: return jump always.
 */
char always(state *s) {
	return 1; 
}

/*	
 * 	pre: s != null. condition codes set. 
 *	post: return jump if condition codes
 *	indicate less than or equal to zero. 
 */
char lessThanEqual(state *s) {
	return lessThan(s) | equal(s);
}

/*	
 * 	pre: s != null. condition codes set. 
 *	post: return jump if condition codes
 *	indicate less than zero.
 */
char lessThan(state *s) {
	return (s->flags[SFLAG] ^ s->flags[OFLAG]);
}

/*	
 * 	pre: s != null. condition codes set. 
 *	post: return jump if condition codes
 *	indicate equal to zero.
 */
char equal(state *s) {
	return s->flags[ZFLAG];
}

/*	
 * 	pre: s != null. condition codes set. 
 *	post: return jump if condition codes
 *	indicate not equal to zero.
 */
char notEqual(state *s) {
	return !s->flags[ZFLAG];
}

/*	
 * 	pre: s != null. condition codes set. 
 *	post: return jump if condition codes
 *	indicate greater than or qual to zero.
 */
char greaterThanEqual(state *s) {
	return !(s->flags[SFLAG]^s->flags[OFLAG]);
}

/*	
 * 	pre: s != null. condition codes set. 
 *	post: return jump if condition codes
 *	indicate greater than zero.
 */
char greaterThan(state *s) {
	return !(s->flags[SFLAG]^s->flags[OFLAG]) & !s->flags[ZFLAG];
}


/* 
 * 	pre: s != null, pc still pointing to the location
 * 	in memory with the byte code instruction.
 * 	post: do not alter any memory, flags, or reg. Jump
 *	to the destination if the condition is true. 
 *	checks the condition codes. 
 * 	pc = pc + 9 if condition false
 * 	pc = dest if condition true
 */
void branch(state *s) {
	// get the function code 
	char functionCode = getFunctionCode(s);
	if(functionCode >= NUMBRANCH) {
		char message[] = "JMP function code %d invalid. Halting the execution.";
		char copyTo[30];
		sprintf(copyTo, message, functionCode);
		warning(copyTo);
	}

	// advance the pc after getting the instruction code
	s->pc++;
	
	uint64_t dest = getQuadWord(s);

	// get values and call the function. 
	char jump = 
		(*conditional[functionCode])(s);
	if(jump)
		s->pc = dest;
	else 
		s->pc = s->pc + 8;
	if(DEBUG) {
		if(jump)
			printf("making jump to %"PRIu64" \n", dest);
		else 
			printf("no jump, continue to %"PRIu64" \n", s->pc);
	}
}

/* 
 *	pre: s != null
 *	post: pc set to the new destination.
 *	return address pushed onto the the stack.
 */
void call(state *s) {
	// get the new destination. 
	// decrement the stack pointer (by 8)
	// put the pc to return to on the stack. 
	// set pc to destination. 
	s->pc++;

	uint64_t dest = getQuadWord(s);

	// decrement the stack pointer. 
	uint64_t prevStack = s->registers[RSP];
	s->registers[RSP] -= 8;
	if(s->registers[RSP] >= prevStack) {
		char message[] = "call instruction has resulted in a stack overflow.";
		warning(message);
	}

	// push the old pc onto the stack 
	uint64_t returnAddr = s->pc + 8;
	uint64_t mask = 0xFF;
	uint64_t memoryLocation = s->registers[RSP];

	if(DEBUG) {
		printf("making call to funct at %"PRIu64"\n", dest);
		printf("pushing return address %"PRIu64"\n", returnAddr);
		printf("pushing onto stack at loc %"PRIu64"\n", memoryLocation);
	}

	for(int i = 0; i < 8; i++) {
		char temp = (char)((returnAddr & mask)>>i*8);
		s->memory[memoryLocation] = temp;
		mask = mask << 8;
		memoryLocation++;
	}

	// set the pc to the destination
	s->pc = dest;
	if(DEBUG) {
		printf("At the end of call. Register values are \n");
		printRegisters(s);
	}
}

/* 
 *	pre: s != null
 *	post: pc set to the address that is 
 *	located on the stack.
 */
void ret(state *s) {
	// read the value that is currently on 
	// the stack 
	s->pc = s->registers[RSP];
	uint64_t returnTo = getQuadWord(s);


	uint64_t mask = 0xFF;
	uint64_t memoryLocation = s->registers[RSP];

	if(DEBUG) {
		printf("return to location %"PRIu64"\n", returnTo);
	}

	for(int i = 0; i < 8; i++) {
		char temp = 0;
		s->memory[memoryLocation] = temp;
		mask = mask << 8;
		memoryLocation++;
	}

	uint64_t oldStack = s->registers[RSP];
	// increment the stack pointer
	s->registers[RSP] += 8;
	if(oldStack >= s->registers[RSP]) {
		char message[] = "return instruction has resulted in a stack overflow.";
		warning(message);
	}

	// set the pc to the value 
	s->pc = returnTo;

	if(DEBUG) {
		printf("At the end of RETURN. Register values are \n");
		printRegisters(s);
	}
}

int pushCount = 0;
int pushCounts[NUMREG] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int popCount = 0;
int popCounts[NUMREG] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

/* 
 *	pre: s != null
 *	post: push a value from regA onto 
 *	the stack. rsp will be decremented 
 *	by 8, and the value placed at the
 *	decremented location.
 */
void pushq(state *s) {
	// advance the pc after getting the instruction code
	s->pc++;
	
	char* registers = getRegisters(s);
	if(registers[REGB] != 0xF) {
		warning("executing a pushq instruction, but regB location not 0xF");
	}
	// advance the pc past the register byte
	s->pc++;

	// decrement the stack pointer, 
	s->registers[RSP] -= 8;

	// push the item onto the stack 
	uint64_t value = s->registers[registers[REGA]];
	uint64_t mask = 0xFF;
	uint64_t memoryLocation = s->registers[RSP];

	if(DEBUG) {
		pushCount++;
		pushCounts[registers[REGA]]++;
		printf("pushing the value %"PRIu64" from register %s onto the stack\n", value, registerNames[registers[REGA]]);
		printf("memory location %"PRIu64"\n", memoryLocation);
		printf("num times push has been called: %d\n", pushCount);
		for(int i = 0 ; i < NUMREG; i++) {
			printf("%s: %d,", registerNames[i], pushCounts[i]);
		}
		printf("\n");
	}
	int isLocation = memoryLocation == 4944;
	for(int i = 0; i < 8; i++) {
		char temp = (char)((value & mask)>>i*8);
		s->memory[memoryLocation] = temp;
		mask = mask << 8;
		memoryLocation++;
	}
}

/* 
 *	pre: s != null
 *	post: push a value from regA onto 
 *	the stack. rsp will be decremented 
 *	by 8, and the value placed at the
 *	decremented location.
 */
void popq(state *s) {
	// advance the pc after getting the instruction code
	s->pc++;

	char* registers = getRegisters(s);
	if(registers[REGB] != 0xF) {
		warning("executing a popq instruction, but regB location not 0xF");
	}
	// advance the pc past the register byte
	s->pc++;

	// set the pc temporarily to rsp to read the value
	uint64_t savePc = s->pc;
	s->pc = s->registers[RSP];
	uint64_t value = getQuadWord(s);
	s->pc = savePc;

	if(DEBUG) {
		popCount++;
		popCounts[registers[REGA]]++;
		printf("putting value %"PRIu64" into reg %d\n", value, registers[REGA]);
		printf("num times pop has been called: %d\n", popCount);
		for(int i = 0 ; i < NUMREG; i++) {
			printf("%s: %d,", registerNames[i], popCounts[i]);
		}
		printf("\n");
	}


	// zero out the contents of the stack, just in case
	uint64_t mask = 0xFF;
	uint64_t memoryLocation = s->registers[RSP];

	for(int i = 0; i < 8; i++) {
		char temp = 0;
		s->memory[memoryLocation] = temp;
		mask = mask << 8;
		memoryLocation++;
	}

	// increment the stack pointer, 
	s->registers[RSP] += 8;

	// put the value from the stack into regA
	s->registers[registers[REGA]] = value;
}


void pause(state *s) {
	s->pc++;
	runPauseRoutine(s);
}

























