#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdlib.h>



// num digits in 2^64 
#define MAXNUMBERSIZE 19

#define NUMWHITESPACE 6
char wSpace[NUMWHITESPACE] = {' ', '\t', '\n', '\v', '\f', '\r'};


void removeWhiteSpace(FILE* file);
int removeWhiteSpaceLines(FILE* file, int lineNumber);
void printBinary(uint64_t num);
char peekChar(FILE* file);
int isWhiteSpace(char c);
void error(char* errorMessage, int lineNumber);
void warning(char* errorMessage);
uint64_t getNumber(FILE* file);





/*****************************************************
Utility methods.
******************************************************/
/*
    char copyTo[100];
    sprintf(copyTo, message, dir->instr);
    error(copyTo);
*/

/* 
 * write an errpr message and kill the assembler.
 */
void error(char* errorMessage, int lineNumber) {
    fprintf(stderr, "ERROR: (Line Number %d) \n\t%s\n", lineNumber, errorMessage);
    exit(-1);
}

/* 
 * write an error message but don't kill the assembler.
 */
void warning(char* errorMessage) {
    fprintf(stderr, "WARNING: \n\t%s\n", errorMessage);
}

/*
 *  pre: none
 *  post: print out a binary representation of a 
 *  uint64_t number
 */
void printBinary(uint64_t num) {
    uint64_t mask = 0x1;
    char res[65];
    res[64] = '\0';
    for(int i = 63; i >= 0; i--) {
        uint64_t temp = mask & num;
        if(temp) 
            res[i] = '1';
        else
            res[i] = '0';
        mask = mask << 1;
    }
    printf("%s\n", res);
}


/*
 *  pre: file not past EOF char
 *  post: advances the file pointer until 
 *  peekChar(file) is not whitespace.
 *
 */
void removeWhiteSpace(FILE* file) {
    removeWhiteSpaceLines(file, 0);
}

/*
 *  pre: file not past EOF char
 *  post: advances the file pointer until 
 *  peekChar(file) is not whitespace. Returns
 *  the new line number
 *
 */
int removeWhiteSpaceLines(FILE* file, int lineNumber) {
    int c = peekChar(file);
    if(c == '\n') 
        lineNumber++;
    while(isWhiteSpace(c)) {
        int removeChar = getc(file);
        c = peekChar(file);
        if(c == '\n') 
            lineNumber++;
    }
    return lineNumber;
}

/*
 *  pre: EOF character not already reached
 *  post: return a character that is the next to 
 *  be processed. (does not advance the file pointer)
 *
 */
char peekChar(FILE* file) {
    int c = getc(file);
    ungetc(c, file);
    return c;
}

/*
 *  pre: c is valid char
 *  post: return true if c is considered "whitespace". 
 *  whitespace defined in hearder file.
 *
 */
int isWhiteSpace(char c) {
    for(int i = 0; i < NUMWHITESPACE; i++) {
        if(c == wSpace[i]) 
            return 1;
    }
    return 0;
}

/*
 *  pre: file not advanced past EOF. Next characters
 *  form an integer number.
 *  post: return the next available number pointed
 *  to by file.
 *
 */
uint64_t getNumber(FILE* file) {
    char* value = calloc(MAXNUMBERSIZE +1, sizeof(char));
    int c = peekChar(file);
    char isNeg = c == '-';
    if(isNeg) {
        getc(file);
        c = peekChar(file);
    }
    int index = 0;
    while('0' <= c && c <= '9') {
        value[index] = c;
        index++;
        getc(file);
        c = peekChar(file);
    }  
    value[index] = '\0';
    uint64_t res = atoi(value);
    if(isNeg) 
        res = (uint64_t)((int64_t) res * -1);
    free(value);
    return res;
}




