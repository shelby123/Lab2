

#include <stdio.h>

#define NUMWHITESPACE 6

char whiteSpace[NUMWHITESPACE] = {' ', '\t', '\n', '\v', '\f', '\r'};

// cmpFiles "$FILE ONE" "$FILE TWO" "TESTNAME"

char peekChar(FILE* file) {
    int c = getc(file);
    ungetc(c, file);
    return c;
}

int isWhiteSpace(char c) {
    for(int i = 0; i < NUMWHITESPACE; i++) {
        if(c == whiteSpace[i]) 
            return 1;
    }
    return 0;
}

void removeWhiteSpace(FILE* file) {
    int c = peekChar(file);
    while(isWhiteSpace(c)) {
        getc(file);
        c = peekChar(file);
    }
}


int main(int argc, char** argv){
	char* fileOne = argv[1];
    FILE *f1 = fopen(fileOne, "r");
	char* fileTwo = argv[2];
    FILE *f2 = fopen(fileTwo, "r");

    removeWhiteSpace(f1);
    removeWhiteSpace(f2);
    int c1 = peekChar(f1);
    int c2 = peekChar(f2);
    while(c1 == c2 &&   c1 != EOF && c2 != EOF) {
    	getc(f1);
    	getc(f2);
    	removeWhiteSpace(f1);
    	removeWhiteSpace(f2);
    	c1 = peekChar(f1);
    	c2 = peekChar(f2);
    }
    if(c1== EOF && c2 == EOF) {
    	printf("passed test %s\n", argv[3]);
    } else {
    	printf("FAILED TEST %s\n", argv[3]);
    }
    
}