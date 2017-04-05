/*
Shelby McGarrah
sm52376
*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>


#define NUMREG 15
#define SHORTREG 2
#define NUMINSTR 24
#define NUMOPQ 4
#define NUMBRANCH 7
#define NUMMOVE 7
#define NUMWHITESPACE 6


#define HALT 0
#define NOP 1
#define NOOP 2
#define RRMOVQ 3
#define IRMOVQ 4
#define RMMOVQ 5
#define MRMOVQ 6
#define OP 7
#define JUMP 8
#define CMOVE 9
#define CALL 10
#define RET 11
#define PUSHQ 12
#define POPQ 13
#define IADDQ 14
#define LEAVE 15

#define ALIGN 16
#define POS 17
#define SPACEINSTR 18

#define BYTE 19
#define QUAD 20
#define CHAR 21
#define STRING 22

#define BEGINSEQUENCE 19
#define ENDSEQUENCE 22

#define R8 8
#define R9 9

#define BITSINBYTE 8


#define FILENAME 1
#define SPACE 32
#define MAXIMMEDIATESIZE 19
#define MAXLABELSIZE 100
#define REGNAMESIZE 3
#define NOTFOUND -1

#define LABEL 1000

char* registers[NUMREG] = { "rax", "rcx", "rdx", "rbx", "rsp", "rbp", 
		"rsi", "rdi", "r08", "r09", "r10", "r11", "r12", "r13", "r14"};


char* instr[NUMINSTR] = {"halt", "nop", "noop", "rrmovq", "irmovq", "rmmovq", "mrmovq", 
		"OP", "JUMP", "CONDMOVE", "call", "ret", "pushq", "popq", "iaddq",
        "leave",
            "align", "pos", "space", "byte", "qword", "char", "string", "pause"};


int instrBytes[NUMINSTR] = {1, 1, 1, 2, 10, 10, 10, 2, 9, 2, 9, 1, 2, 2, 10, 1,
                            0, 0, 0, 0, 0, 0, 0, 1};

char* opq[NUMOPQ] = {"addq", "subq", "andq", "xorq"};

char* branches[NUMBRANCH] = {"jmp", "jle", "jl", "je", "jne", "jge", "jg"};

char* moves[NUMMOVE] = {"rrmovq", "cmovle", "cmovl", "cmove", "cmovne", "cmovge", "cmovg"};

char whiteSpace[NUMWHITESPACE] = {' ', '\t', '\n', '\v', '\f', '\r'};




typedef struct tok {
    int  instr;
    int instrNum;
    int rA;
    int rB;
    int numBytes;
    char* label;
    union  {
        uint64_t offset;
        uint64_t immediate;
        uint64_t destination;
    } data;
    struct tok* nextInstr;
    uint64_t pos;
    struct data* sequence;

} op;

typedef struct sym {
    char* name;
    uint64_t loc;
    struct sym* nextSym;
} symbol;

typedef struct data {
    int type;
    union {
        char* string;
        unsigned char character;
        unsigned char byte;
        uint64_t qword;
    } myData;
    struct data* nextData;
} aData;



void firstPassParser(op* headerOp, symbol* headerSym, FILE* file);
int isDirective(op* instr);
op* nextInstr(FILE *file);
void getInstruction(op *instruction, FILE *file);
int getInstrOp(FILE *file);
int indexOf(char** array, int length, char* word);
void printOp(op* instr);
void printSymbol(symbol* sym);
void processInstruction(op* instruction, FILE* file);
void processImmediate(op* instruction, FILE* file);
void processDestination(op* instruction, FILE *file);
void processRegA(op* instruction, FILE* file);
void processRegB(op* instruction, FILE* file);
int getReg(FILE* file);
void getRegA(op* instruction, FILE* file);
void getRegB(op* instruction, FILE* file);
void regWithOffset(op* instruction, FILE* file, void(*function)(op* instruction, FILE* file));
int getHexDigit(char c);
void removeWhiteSpace(FILE* file);
int isWhiteSpace(char c);
int isValidDigit(char c);
char peekChar(FILE* file);
uint64_t setPos(op* dir, uint64_t curPos);
symbol* getSymbol(FILE* file, uint64_t pos);
int isSequence(op* instr);
void processSequence(op* instruction, FILE* file);
void processNumbers(op* instruction, FILE* file);
void processChars(op* instruction, FILE* file);
void processString(op* instruction, FILE* file);
uint64_t getNumber(FILE* file);
void error(char* errorMessage);
void warning(char* errorMessage);
char getNextChar(FILE *file);
uint64_t lookupSymbol(symbol* header, char* label);
char* getLabel(FILE *file);
int isValidInt(char c);
void toLowerCase(char* str, int n);


void writeOutput(op* headerOp, symbol* headerSym);
void printInstrMap(op* instr, symbol* headerSym);


void printQuadWord(uint64_t start, uint64_t val) {
    uint64_t curPos = start;
    uint64_t mask = 0xFF;
    for(int i = 0; i < 8; i++) {
        uint64_t byte = val & mask;
        byte = byte >> (8*i);
        printf("( %" PRIu64 " . %" PRIu64 " )\n", curPos, byte);
        curPos++;
        mask = mask << 8;
    }


}

void pHalt(op* instr, symbol* sym) {  
    printf("( ");
    printf("%" PRIu64"", instr->pos);
    printf(" . ");
    printf("0 ");
    printf(" )\n");
}
void pNOP(op* instr, symbol* sym) {   
    printf("( ");
    printf("%" PRIu64 "", instr->pos);
    printf(" . ");
    if(instr->instr == NOOP) {
        printf("240 ");
    } else {
        printf("16 ");
    }
    printf(" )\n");
}

void pRRMov(op* instr,  symbol* sym) {  
    printf("( %" PRIu64 " .  32 )\n", instr->pos);
    uint64_t regNum = 16*instr->rA + instr->rB;
    printf("( %" PRIu64 " . %" PRIu64 " )\n", instr->pos + 1, regNum);
}

void pIRMov(op* instr, symbol* sym) {   
    printf("( %" PRIu64 " .  48 )\n", instr->pos);
    uint64_t regNum = 16*15 + instr->rB;
    printf("( %" PRIu64 " . %" PRIu64 " )\n", instr->pos + 1, regNum);

    uint64_t val = instr->data.immediate;
    uint64_t curPos = instr->pos + 2;
    if(instr->label == NULL) {
        val = instr->data.immediate;
        printQuadWord(curPos, val);
    } else {
        val = lookupSymbol(sym, instr->label);
        printQuadWord(curPos, val);
    }
}

void pRMMov(op* instr, symbol* sym) {   
    printf("( %" PRIu64 " .  64 )\n", instr->pos);
    uint64_t regNum = 16*instr->rA + instr->rB;
    printf("( %" PRIu64 " . %" PRIu64 " )\n", instr->pos + 1, regNum);
    uint64_t val;
    uint64_t curPos = instr->pos + 2;

    if(instr->label == NULL) {
        val = instr->data.offset;
        printQuadWord(curPos, val);
    } else {
        val = lookupSymbol(sym, instr->label);
        printQuadWord(curPos, val);
    }

}
void pMRMov(op* instr, symbol* sym) {   
    printf("( %" PRIu64 " .  80 )\n", instr->pos);
    uint64_t regNum = 16*instr->rA + instr->rB;
    printf("( %" PRIu64 " . %" PRIu64 " )\n", instr->pos + 1, regNum);
    uint64_t val;
    uint64_t curPos = instr->pos + 2;

    if(instr->label == NULL) {
        val = instr->data.offset;
        printQuadWord(curPos, val);
    } else {
        val = lookupSymbol(sym, instr->label);
        printQuadWord(curPos, val);
    }
}

void pOPq(op* instr,  symbol* sym) {  
    uint64_t firstByte = 6*16 + instr->instrNum;
    printf("( %" PRIu64 " .  %lld )\n", instr->pos, (long long int)firstByte);
    uint64_t regNum = 16*instr->rA + instr->rB;
    printf("( %" PRIu64 " . %" PRIu64 " )\n", instr->pos + 1, regNum);
}

void pJMP(op* instr,  symbol* sym) {  
    uint64_t firstByte = 7*16 + instr->instrNum;
    printf("( %" PRIu64 " .  %lld )\n", instr->pos, (long long int)firstByte);

    uint64_t dest;
    if(instr->label != NULL) {
        dest = lookupSymbol(sym, instr->label);
    } else {
        dest = instr->data.destination;
    }
    uint64_t curPos = instr->pos + 1;
    printQuadWord(curPos, dest);
}


void pCndMov(op* instr,  symbol* sym) {  
    uint64_t firstByte = 2*16 + instr->instrNum;
    printf("( %" PRIu64 " .  %lld )\n", instr->pos, (long long int)firstByte);
    uint64_t regNum = 16*instr->rA + instr->rB;
    printf("( %" PRIu64 " . %" PRIu64 " )\n", instr->pos + 1, regNum);
}

void pCall(op* instr,  symbol* sym) {  
    uint64_t firstByte = 8*16;
    printf("( %" PRIu64 " .  %lld )\n", instr->pos, (long long int)firstByte);

    uint64_t dest;
    if(instr->label != NULL) {
        dest = lookupSymbol(sym, instr->label);
    } else {
        dest = instr->data.destination;
    }
    uint64_t curPos = instr->pos + 1;
    printQuadWord(curPos, dest);
}

void pRet(op* instr, symbol* sym) {   
    printf("( ");
    printf("%lld ", (long long int)instr->pos);
    printf(" . ");
    printf("144 ");
    printf(" )\n");
}

void pPush(op* instr,  symbol* sym) {  
    printf("( %" PRIu64 " .  160 )\n", instr->pos);
    uint64_t regNum = 16*instr->rA + 0xF;
    printf("( %" PRIu64 " . %" PRIu64 " )\n", instr->pos + 1, regNum);
}

void pPop(op* instr,  symbol* sym) {  
    printf("( %" PRIu64 " .  176 )\n", instr->pos);
    uint64_t regNum = 16*instr->rA + 0xF;
    printf("( %" PRIu64 " . %" PRIu64 " )\n", instr->pos + 1, regNum);
}

void pIAddq(op* instr, symbol* sym) {   
    printf("( %" PRIu64 " .  192 )\n", instr->pos);
    uint64_t regNum = 16*15 + instr->rB;
    printf("( %" PRIu64 " . %" PRIu64 " )\n", instr->pos + 1, regNum);

    uint64_t val = instr->data.immediate;
    uint64_t curPos = instr->pos + 2;
    if(instr->label == NULL) {
        val = instr->data.immediate;
        printQuadWord(curPos, val);
    } else {
        val = lookupSymbol(sym, instr->label);
        printQuadWord(curPos, val);
    }
}

void pLeave(op* instr,  symbol* sym) {  
    printf("( %" PRIu64 " .  208 )\n", instr->pos);
}

void pError(op* instr, symbol* sym) {
    fprintf(stderr, "ERROR: called print on a directive token");
    assert(0);
}

void pBytes(op* instr, symbol* sym) {
    int numElems = instr->numBytes;
    aData* cur = instr->sequence;
    uint64_t curPos = instr->pos;
    for(int i = 0; i < numElems; i++) {
        unsigned char value = cur->myData.byte;
        printf("( %" PRIu64 " . %u )\n", curPos, value);
        curPos++;
        cur = cur->nextData;
    }
    assert(cur == NULL);
}


void pQuad(op* instr, symbol* sym) {
    int numElems = (instr->numBytes)>>3;//divide by 8
    aData* cur = instr->sequence;
    uint64_t curPos = instr->pos;
    for(int i = 0; i < numElems; i++) {
        uint64_t value = cur->myData.qword;
        printQuadWord(curPos, value);
        curPos+= 8;
        cur = cur->nextData;
    }
    assert(cur == NULL);
}

void pChar(op* instr, symbol* sym) {
    int numElems = instr->numBytes;
    aData* cur = instr->sequence;
    uint64_t curPos = instr->pos;
    for(int i = 0; i < numElems; i++) {
        char value = cur->myData.character;
        printf("( %" PRIu64 " . %d )\n", curPos, value);
        curPos++;
        cur = cur->nextData;
    }
    assert(cur == NULL);
}

void pString(op* instr, symbol* sym) {
    int numElems = instr->numBytes;
    aData* cur = instr->sequence;
    char* myString = cur->myData.string;
    uint64_t curPos = instr->pos;
    // 0 to numElements to insert \0 char
    for(int i = 0; i < numElems; i++) {
        char value = myString[i];
        printf("( %" PRIu64 " . %d )\n", curPos, value);
        curPos++;
    }
}

void pPause(op* instr, symbol *sym) {
    printf("( ");
    printf("%" PRIu64"", instr->pos);
    printf(" . ");
    printf("240 ");
    printf(" )\n");
}

void (*pFuncs[NUMINSTR])() = {pHalt, pNOP, pNOP, pRRMov, pIRMov, pRMMov,
            pMRMov, pOPq, pJMP, pCndMov, pCall, pRet, pPush, pPop, pIAddq,
            pLeave,
            pError, pError, pError, 
            pBytes, pQuad, pChar, pString,
            pPause}; 


