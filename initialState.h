#include <stdio.h>
#include "state.h"
#include "util.h"



#define ADDR 0
#define VAL 1

#define DEBUGINITSTATE 1

int lineNumber = 1;


void loadInMemValues(FILE *file, state* s);
void mapMemValue(FILE *file, state *s);
int* getAddrValPair(FILE *file);



/**************************************************
parse the file
***************************************************/

/* 
 *  pre: file!= null, s != null
 *  post: mem values indicated by file are loaded
 *  into the memory block pointed to by s
 */ 
void loadInMemValues(FILE *file, state* s) { 
	lineNumber = 1;
	if(s->memory == NULL)
        s->memory = calloc(MEMSIZE, BYTESIZE);
    lineNumber = removeWhiteSpaceLines(file, lineNumber);
    if(getc(file) != '(') {
        char message[] = "\'(\' not found while parsing file";
        error(message, lineNumber);
    }
    lineNumber = removeWhiteSpaceLines(file, lineNumber);
    char c = peekChar(file);
    while(c != ')') {
        mapMemValue(file, s);
        lineNumber = removeWhiteSpaceLines(file, lineNumber);
        c = peekChar(file);
    }
   
}

/*
 *  pre: s != null, file != null, 
 *  exists a string of format ( addr . value )
 *  coming up in the file. 
 *      Note: 
 *          - white space removed
 *          - last ) consumed
 *  post: loads one addr->value map into s's memory
 */
void mapMemValue(FILE *file, state *s) {
    // white space removed from previous calls
    if(getc(file) != '(') {
        char message[] = "opening \'(\' not found while parsing a ( addr . value ) mapping";
        error(message, lineNumber);
    }
    lineNumber = removeWhiteSpaceLines(file, lineNumber);
    // call getAddr

    int *map = getAddrValPair(file);

    lineNumber = removeWhiteSpaceLines(file, lineNumber);
    if(getc(file) != ')') {
        char message[] = "\')\' not found while parsing a ( addr . value ) mapping";
        error(message, lineNumber);
    }
    // add the result to memory

    s->memory[map[ADDR]] = map[VAL];

    free(map);
}

/*
 *  pre: file != null, of the format addr . value 
 *  post: return an int array of size two with addr at 
 *  position 0 and value at position 1
 */
int* getAddrValPair(FILE *file) {
    int *res = calloc(2, sizeof(int));
    // get the first number 
    uint64_t addr = getNumber(file);

    lineNumber = removeWhiteSpaceLines(file, lineNumber);
    if(getc(file) != '.') {
        char message[] = "\'.\' not found while parsing a ( addr . value ) mapping";
        error(message, lineNumber);
    }
    lineNumber = removeWhiteSpaceLines(file, lineNumber);

    // get the second number 
    uint64_t value = getNumber(file);

    if(DEBUGINITSTATE) {
        printf("Parsing an addr value pair (%lld . %lld)\n",
            (long long int) addr, (long long int) value);
    }
    res[ADDR] = addr;
    res[VAL] = value;
    return res;
}
