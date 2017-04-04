/*
Shelby McGarrah
sm52376
*/

/*

General flow: 

main
    - parse input into a linear string of tokens
    - loop through the string of tokens to print out 
    information.

*/


#include "y86-assembler.h"


#define DEBUG 0


int lineNumber = 1;
int instrNum = 1;
int printing = 0;


/*
 *  pre: takes in the file to read from from the command line.
 *  post: parses into a linear string of tokens, then prints out
 *  mapping based on the instructions list to stdout
 */
int main(int argc, char** argv ) {
    assert(argc >= 2);
    if(DEBUG)
        printf("Start program\n");
    char* fileName = argv[FILENAME];
    FILE *file = fopen(argv[FILENAME], "r");
   
    removeWhiteSpace(file);
    getc(file);
    
    op* headerOp = calloc(1, sizeof(op));
    symbol* headerSym = calloc(1, sizeof(symbol));
    
    if(DEBUG)
        printf("Begin processing\n");
    firstPassParser(headerOp, headerSym, file);
    if(DEBUG) 
        printf("finished firstPassParser\n");

    printing = 1;
    writeOutput(headerOp, headerSym);
    
    free(headerSym);
    free(headerOp);
    fclose(file);
    return 0;
}

/*
 *  pre: headerOp is a pointer to the token that is the first
 *  instruction. headerSym is a pointer to the header node for the
 *  symbol table.
 *  post: move through the list of operations (instructions), and 
 *  print out all of the information to stdout.
 *
 */
void writeOutput(op* headerOp, symbol* headerSym) {
    printf("(\n");
    op* curInstr = headerOp->nextInstr;
    while(curInstr != NULL) {
        printInstrMap(curInstr,  headerSym);
        instrNum++;
        curInstr = curInstr->nextInstr;
    }
    printf(")\n");
}


/*
 *  pre: instr != null. headerSym != null. headersym is the 
 *  pointer to the start of the symbol table.
 *  post: print out the information for instruction instr. 
 *  (basically a wrapper to call from a list of subroutines
 *  that are stored in an array in y86-assembler.h)
 *
 */
void printInstrMap(op* instr, symbol* headerSym) {
    if(DEBUG) {
        printf("on instruction number %d\n", instrNum);
    }
    (*pFuncs[instr->instr])(instr, headerSym);
}

/*
 *  pre: headerOp, headerSym != NULL. file points to the 
 *  file to be processed. first '(' must be removed. White
 *  space is tolerated.
 *  post: headerOp points to a built list of instruction 
 *  tokens. Pointer to the file is at the end of the file.
 *
 */
void firstPassParser(op* headerOp, symbol* headerSym, FILE* file) { 
    uint64_t currentPos = 0;
    op* prevOp = headerOp;
    symbol* prevSym = headerSym;
    op* tempOp;
    symbol* tempSym;

    removeWhiteSpace(file);
    int c = peekChar(file);
    while(c != ')') {
        if(DEBUG)
            printf("current Position is %lld\n", (long long int)currentPos);
        if(peekChar(file) == '(') { 
            tempOp = nextInstr(file);
            if(DEBUG) 
                printOp(tempOp);
            if(isDirective(tempOp))  {
                currentPos = setPos(tempOp, currentPos);
                free(tempOp);
            } else {
                tempOp->pos = currentPos;
                currentPos += tempOp->numBytes;
                prevOp->nextInstr = tempOp;
                prevOp = tempOp;
            }
        } else {
            tempSym = getSymbol(file, currentPos);
            prevSym->nextSym = tempSym;
            if(DEBUG) 
                printf("new symbol name is %s\n", tempSym->name);
            prevSym = tempSym;
        }
        removeWhiteSpace(file);
        c = peekChar(file);
    }
}

/*
 *  pre: getc(file) must be a valid character.
 *  post: create a symbol token that maps the label
 *  read from the file to the current pos as given by 
 *  pos.
 *
 */
symbol* getSymbol(FILE* file, uint64_t pos) {
    char* name = calloc(MAXLABELSIZE, sizeof(char));
    int index = 0;
    int c = peekChar(file);
    while(c != ')' && !isWhiteSpace(c)) {
        name[index] = c;
        index++;
        getc(file);
        c = peekChar(file);
    }
    name[index] = '\0';
    if(DEBUG) {
        printf("about to add symbol with name %s\n", name);
    }
    removeWhiteSpace(file);
    symbol* newSymbol = calloc(1, sizeof(symbol));
    newSymbol->name = name;
    newSymbol->loc = pos;
    if(DEBUG)
        printSymbol(newSymbol);
    return newSymbol;
}

/*
 *  pre: dir != null. dir points to valid x86 directive.
 *  post: return the new position based on the previous 
 *  position as given by curPos, and the directive as given
 *  by dir.
 *
 */
uint64_t setPos(op* dir, uint64_t curPos) {
    if(dir->instr == ALIGN) {
        uint64_t alignTo = dir->data.immediate;
        uint64_t newPos = curPos;
        if(curPos % alignTo)
            newPos = ((curPos/alignTo) + 1)*alignTo;
        if(newPos < curPos) {
            char message[] = "position overflow\n";
            error(message);
        }
        return newPos;
    } else if(dir->instr == POS) {
        return dir->data.immediate;
    } else if(dir->instr == SPACEINSTR) {
        uint64_t newPos = curPos + dir->data.immediate;
        if(newPos < curPos) {
            char message[] = "position overflow\n";
            error(message);
        }
        return newPos;
    }
    char message[] = "Unexpected directive label: %d\n";
    char copyTo[100];
    sprintf(copyTo, message, dir->instr);
    error(copyTo);

}

/*
 *  pre: instr != null
 *  post: return whether the formed operator
 *  token is a directive or not. (uses the definitions
 *  declared in header file.)
 *
 */
int isDirective(op* instr) {
    int number = instr->instr;
    int res = (number >= ALIGN) && (number <= SPACEINSTR);
    return res;
}


/*
 *  pre: file has not processed the EOF
 *  post: return an op token that represents the next
 *  instruction pointed to by file.
 *
 */
op* nextInstr(FILE *file) {
    op *instruction = calloc(1, sizeof(op));
    int c = getc(file);
    while(c != '(') 
        c = getc(file);
    getInstruction(instruction, file);
    if(isSequence(instruction)) {
        processSequence(instruction, file);
    } else {
        processInstruction(instruction, file);
    }
    if(DEBUG)
        printOp(instruction);
    return instruction;
}

/*
 *  pre: instruction != null, file hasn't moved past the EOF
 *  post: add data to the instruction file based on 
 *  the type of words to be put into memory.
 *
 */
void processSequence(op* instruction, FILE* file) {
    if(instruction->instr == QUAD || instruction->instr == BYTE) {
        processNumbers(instruction, file);
    } else if( instruction->instr == CHAR) {
        processChars(instruction, file);
    } else {
        processString(instruction, file);
    }
    removeWhiteSpace(file);
    if(getc(file) !=  ')') {
        char message[] = "Syntax error. No parenthesis after a word sequence.\n";
        error(message);
    }
}

/*
 *  pre: instruction != null
 *  post: process an array of numbers, add it to the
 *  data token pointed to in instruction.
 *
 */
void processNumbers(op* instruction, FILE* file) {
    removeWhiteSpace(file);
    aData* prev = NULL;
    int c = peekChar(file);
    int numEntries = 0;
    while(c != ')') {
        uint64_t value = getNumber(file);
        removeWhiteSpace(file);
        if((instruction->instr == BYTE) &&
            (value >> 8 != 0)) {
            error("not a valid byte for byte declaration (in processNumbers)\n");
        } 
        numEntries++;
        aData* temp = calloc(1, sizeof(aData));
        temp->type = instruction->instr;
        if(temp->type == BYTE) {
            char byte =(char)( value & 0xFF);
            temp->myData.byte = byte;
        } else 
            temp->myData.qword = value;
        if(prev == NULL) {
            instruction->sequence = temp;
            prev = temp;
        } else {
            prev->nextData = temp;
            prev = temp;
        }
        removeWhiteSpace(file);
        c = peekChar(file);
    }
    int multFactor = instruction->instr == BYTE ? 1 : 8;
    instruction->numBytes = multFactor* numEntries;
}

/*
 *  pre: file not advanced past EOF. Next characters
 *  form either a hex or integer number.
 *  post: return the next available number pointed
 *  to by file.
 *
 */
uint64_t getNumber(FILE* file) {
    char* value = calloc(MAXIMMEDIATESIZE +1, sizeof(char));
    int c = peekChar(file);
    int isHex = c == '#';
    if(isHex) {
        getc(file); // get rid of the additional #
        getc(file); // get rid of the x
        c = peekChar(file);
    } 
    int index = 0;
    while(isValidDigit(c)) {
        value[index] = c;
        index++;
        getc(file);
        c = peekChar(file);
    }  
    value[index] = '\0';
    if(DEBUG)
        printf("string num is %s, next char is %c\n", value, peekChar(file));
    if(isHex) {
        uint64_t acc = 0;
        uint64_t sixteenFact = 1;
        for(int i = index-1; i >= 0; i--) {
            int hexValue = getHexDigit(value[i]);
            acc += (sixteenFact * hexValue);
            sixteenFact *= 16;
        }
        free(value);
        if(DEBUG) 
            printf("returining hex value %lld\n", (long long int) acc);
        return acc;
    } else {
        uint64_t res = atoi(value);
        free(value);
        return res;
    }

}

/*
 *  (helper method for processChars)
 *  pre: file not advanced past EOF
 *  post: returns the next available non white
 *  space character from the file.
 *
 */
char getNextChar(FILE* file) {
    removeWhiteSpace(file);
    if(peekChar(file) == '\'') {
        getc(file);
        char c = getc(file);
        getc(file);
        return c;
    } else {
        // get # and backslash
        getc(file); getc(file);
        char c = getc(file);
        return c;
    }
    
}

/*
 *  pre: isntruction != null, file not advanced past EOF
 *  post: read in the characters from file and put into 
 *  data token pointed to from instruction.
 *
 */
void processChars(op* instruction, FILE* file) {
    removeWhiteSpace(file);
    int c = peekChar(file);
    aData* prev = NULL;
    int numChars = 0;
    while(c != ')') {
        int value = getNextChar(file);
        aData* temp = calloc(1, sizeof(aData));
        temp->type = CHAR;
        temp->myData.character = value;
        if(prev == NULL) {
            instruction->sequence = temp;
            prev = temp;
        } else {
            prev->nextData = temp;
            prev = temp;
        }
        numChars++;
        removeWhiteSpace(file);
        c = peekChar(file);
    }
    instruction->numBytes = numChars;


}

/*
 *  pre: isntruction != null, file not advanced past EOF
 *  post: read in the string from file and put into 
 *  data token pointed to from instruction.
 *
 */
void processString(op* instruction, FILE* file) {
    char* word = calloc(MAXLABELSIZE, sizeof(char));
    removeWhiteSpace(file);
    if(getc(file) !=  '\"') {
        char message[] = "Syntax error. Improper start to string literal.\n";
        error(message);
    }
    int c = peekChar(file);
    int index = 0;
    while(c != '\"') {
        word[index] = c;
        getc(file);
        index++;
        c = peekChar(file);
    }
    if(getc(file) !=  '\"') {
        char message[] = "Syntax error. Improper end to string literal.\n";
        error(message);
    }
    word[index] = '\0';
    aData* temp = calloc(1, sizeof(aData));
    temp->type = STRING;
    temp->myData.string = word;
    instruction->sequence = temp;
    instruction->numBytes = index;

}



/*
 *  pre: instruction != null 
 *  post: return true if instruction is a sequence of 
 *  chars, numbers, or a string.
 *
 */
int isSequence(op* instruction) {
    return instruction->instr >= BEGINSEQUENCE;
}
    
/*
 *  pre: instruction != null, file no advanced past EOF
 *  post: instruction will be filled in with which instruction
 *  it is. instruction->instr (+instrNum)
 *
 */
void getInstruction(op* instruction, FILE *file) {
    char* instrWord = calloc(25, sizeof(char));
    int index = 0;
    int c = peekChar(file);
    while(c != ')' && !isWhiteSpace(c)) {
        instrWord[index] = c;
        getc(file);
        c = peekChar(file);
        index++;
    }
    instrWord[index] = '\0';
    index = indexOf(instr, NUMINSTR, instrWord);
    if(index == NOTFOUND) {
        index = indexOf(opq, NUMOPQ, instrWord);
        if(index == NOTFOUND) {
            index = indexOf(branches, NUMBRANCH, instrWord);
            if(index == NOTFOUND) {
                index = indexOf(moves, NUMMOVE, instrWord);
                if(index == NOTFOUND) {
                    char message[] = "instruction \"%s\" not found\n";
                    char copyTo[100];
                    sprintf(copyTo, message, instrWord);
                    error(copyTo);
                }
                instruction->instr = CMOVE;
                instruction->instrNum = index;
            } else { 
                instruction->instr = JUMP;
                instruction->instrNum = index;
            }
        } else {
            instruction->instr = OP;
            instruction->instrNum = index;
        }
    } else 
        instruction->instr = index;
    if(DEBUG)
        printf("found instr with code %d\n", instruction->instr);
}

/*
 *  pre: instruction != null, file not advanced past
 *  EOF character.
 *  post: write all the information that may be got from
 *  the file into the instruction token.
 *
 */
void processInstruction(op* instruction, FILE *file) {
    processRegA(instruction, file);
    processImmediate(instruction, file);
    processRegB(instruction, file);
    processDestination(instruction, file);

    instruction->numBytes = instrBytes[instruction->instr];

    removeWhiteSpace(file);

    if(getc(file) !=  ')') {
        char message[] = "Syntax error. No closing ) at the end of instruction.\n";
        error(message);
    }
}

/*
 *  pre: instruction != null, file not advanced past the EOF
 *  post: add information about a destination. either from a label
 *  or a direct address.
 *
 */
void processDestination(op* instruction, FILE *file) {
     if(!(instruction->instr == JUMP ||
            instruction->instr == CALL))
            return;
     removeWhiteSpace(file);

     int c = peekChar(file);
     if(DEBUG) 
            printf("first char in processDest is %c\n", c);
     if(isValidInt(c) || c == '#') {
        instruction->data.destination = getNumber(file);
        instruction->label = NULL;
     } else {
         char* dest = calloc(MAXLABELSIZE, sizeof(char));
         int index = 0; 
         c  = peekChar(file);

         while(!isWhiteSpace(c) && c != ')') {
            dest[index] = c;
            index++;
            getc(file);
            c = peekChar(file);
         }
         removeWhiteSpace(file);
         instruction->label = dest;
    }
    if(DEBUG) 
        printf("returning from processDest next char is %c\n", peekChar(file));
    
}

/*
 *  pre: instruction != null, file not advanced past the EOF
 *  post: add information about an immediate value
 *
 */
void processImmediate(op* instruction, FILE* file) {
    if(!(instruction->instr == IRMOVQ || isDirective(instruction) 
        || instruction->instr == IADDQ))
        return;
    removeWhiteSpace(file);
    int c = peekChar(file);
    if(isValidInt(c) || c == '#') {
        uint64_t temp = getNumber(file);
        if(DEBUG) 
            printf("temp is %" PRIu64 " \n", temp);
        instruction->data.immediate = temp;
        instruction->label = NULL;
    } else {
        int index = 0;
        char* label = calloc(MAXLABELSIZE, sizeof(char));
        int c = peekChar(file);
        while(!isWhiteSpace(c)) {
            label[index] = c;
            index++;
            getc(file);
            c = peekChar(file);
        }
        label[index] = '\0';
        if(DEBUG) 
            printf("found an immediate label of %s\n", label);
        instruction->label = label;
    }
}

/*
 *  pre: instruction != null, file not advanced past the EOF
 *  post: process information about a second (destination)
 *  registera dn put it into instruction 
 *
 */
void processRegB(op* instruction, FILE* file) {
    switch(instruction->instr) {
        case RMMOVQ:
            regWithOffset(instruction, file, getRegB);
            break;
        case RRMOVQ:   case IRMOVQ:
        case MRMOVQ:   case OP:
        case CMOVE:    case IADDQ:
            getRegB(instruction, file);
            break;
        default: 
            instruction->rB = NOTFOUND;
    }
    if(instruction->instr == MRMOVQ) {
        int temp = instruction->rA;
        instruction->rA = instruction->rB;
        instruction->rB = temp;
    }
}

/*
 *  pre: instruction != null, file not advanced past the EOF
 *  post: process information about the first (source)
 *  register and put it into the instruction
 *
 */
void processRegA(op* instruction, FILE* file) {
    switch (instruction->instr) {
        case MRMOVQ: 
            regWithOffset(instruction, file, getRegA);
            break;
        case OP:        case RRMOVQ:
        case RMMOVQ:    case CMOVE:
        case PUSHQ:     case POPQ:
            getRegA(instruction, file);
            if(instruction->instr == PUSHQ || instruction->instr == POPQ) {
                break;
            }
            break;
        default:
            instruction->rA = NOTFOUND;
    }
}

/*
 *  pre: file not advanced past the EOF
 *  post: return register number indicated by 
 *  file in the form %(NAME) or number from 0 to 15.
 *
 */
int getReg(FILE* file) {
    removeWhiteSpace(file);
    int c = peekChar(file);
    int regNum;
    if(isValidInt(c) || c == '#') {
        uint64_t num = getNumber(file);
        if(num >= NUMREG) {
            char message[] = "Register number %d invalid\n";
            char copyTo[100];
            sprintf(copyTo, message, num);
            error(copyTo);
        }
        regNum = (int)num;
    } else {
        int percentHolder = getc(file);
        while(percentHolder != '%' && percentHolder != EOF) {
            percentHolder = getc(file);
        } 
        if(percentHolder == EOF) {
            char message[] = "no % found for register name\n";
            error(message);
        }
        char* name = calloc(REGNAMESIZE+1, sizeof(char));
        for(int i = 0; i < REGNAMESIZE; i++) {
            name[i] = getc(file);
        }
        name[REGNAMESIZE] = '\0';
        toLowerCase(name, REGNAMESIZE);
        if(DEBUG) 
            printf("Reg name is %s\n", name);
        regNum = indexOf(registers, NUMREG, name);
        if(regNum == NOTFOUND) {
            if(!strncmp(name, "r8", 2)) {
                regNum = R8;
                ungetc(name[REGNAMESIZE-1], file);
            } else if (!strncmp(name, "r9", 2)) {
                regNum = R9;
                ungetc(name[REGNAMESIZE-1], file);
            } else {
                char message[] = "register \"%s\" not found\n";
                char copyTo[100];
                sprintf(copyTo, message, name);
                error(copyTo);
            }
        }
    }
    if(DEBUG)
        printf("Returning from getReg, the next char is %c\n", peekChar(file));
    return regNum;
}

/*
 *  pre: instruction != null, file not advanced past the EOF
 *  post: wrapper, get the register for the source.
 *
 */
void getRegA(op* instruction, FILE* file) {
    // get the register's number
    // set it to the op's rA
    instruction->rA = getReg(file);
}

/*
 *  pre: instruction != null, file not advanced past the EOF
 *  post: wrapper, get the reguster for the destination
 *
 */
void getRegB(op* instruction, FILE* file) {
    // get the register's number
    // set it to the op's rB
    instruction->rB = getReg(file);
}

/*
 *  pre: file not advanced past the EOF
 *  post: return a pointer to a string label
 *
 */
char* getLabel(FILE* file) {
    int index = 0;
    char* label = calloc(MAXLABELSIZE, sizeof(char));
    int c = peekChar(file);
    while(!isWhiteSpace(c) && c != '(') {
        label[index] = c;
        index++;
        getc(file);
        c = peekChar(file);
    }
    label[index] = '\0';
    if(DEBUG) 
        printf("found an offset label of %s\n", label);
    return label;
}

/* 
 * Method to get the the offset for a register move
 * returns the register number.
 */
void regWithOffset(op* instruction, FILE* file, void(*function)(op* instruction, FILE* file)) {
    // process the offset.
    removeWhiteSpace(file); 
    int c = peekChar(file);
    int isParens = 0;
    if(DEBUG)
        printf("peeking at offset char %c\n", c);

    if(isValidInt(c) || c == '#') {
        instruction->data.offset = getNumber(file);
        instruction->label = NULL;
        removeWhiteSpace(file);
    } else if(c != '%') {
        instruction->label = getLabel(file);
        removeWhiteSpace(file);
    }
    if(peekChar(file) == '(') {
        isParens = 1;
        getc(file);

    }

    // call to set the register number 
    (function)(instruction, file);

    if(isParens) {
        // there was an offset, so there must have been 
        // one open parenthesis, we need to process it 
        c = getc(file);
        if(c !=  ')') {
            char message[] = "Syntax error. No parenthesis after register with offset.\n";
            error(message);
        }
    }
    // get and set the register 
}

/*
 *  pre: header != null, label != null
 *  post: return the address indicated by a label 
 *  throws an exception if the symbol cannot be found.
 *
 */
uint64_t lookupSymbol(symbol* header, char* label) {
    symbol* cur = header->nextSym;
    if(cur== NULL) {
            char message[] = "Symbol \"%s\" not found\n";
            char copyTo[100];
            sprintf(copyTo, message, label);
            warning(copyTo);
            return 0;
    }
    char* temp = cur->name;
    while(cur != NULL && strcmp(label, temp)) {
        cur = cur->nextSym;
        if(cur != NULL)
            temp = cur->name;
    }
    if(cur== NULL) {
        char message[] = "Symbol \"%s\" not found\n";
        char copyTo[100];
        sprintf(copyTo, message, label);
        warning(copyTo);
        return 0;
    }
    return cur->loc;
}

/***********************************************************************************
Utility methods
***********************************************************************************/




/*
 *  pre: array != null, word != null
 *  post: return the index for word in array. 
 *  return NOTFOUND otherwise.
 *
 */
int indexOf(char** array, int length,  char* word) {
    for(int i = 0; i < length; i++) {
        if(!strcmp(array[i], word)) {
            return i;
        }
    }
    return NOTFOUND;
}


/* 
 * write an errpr message and kill the assembler.
 */
void error(char* errorMessage) {
    if(printing) {
        fprintf(stderr, "ERROR: (instruction number %d) \n\t%s", instrNum, errorMessage);
    } else {
        fprintf(stderr, "ERROR: (line %d) \n\t%s", lineNumber, errorMessage);
    }
    exit(-1);
}

/* 
 * write an error message, but continue assembling.
 */
void warning(char* errorMessage) {
    if(printing) {
        fprintf(stderr, "WARNING: (instruction number %d) \n\t%s", instrNum, errorMessage);
    } else {
        fprintf(stderr, "WARNING: (line %d) \n\t%s", lineNumber, errorMessage);
    }
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
        if(c == whiteSpace[i]) 
            return 1;
    }
    return 0;
}

/*
 *  pre: file not past EOF char
 *  post: advances the file pointer until 
 *  peekChar(file) is not whitespace. Note
 *  that comments are considered white space
 *  until the next newline character
 *
 */
void removeWhiteSpace(FILE* file) {
    int c = peekChar(file);
    while(isWhiteSpace(c) || c == ';') {
        if(c == ';') {
            while(c != '\n' && c != EOF) {
                getc(file);
                c = peekChar(file);
            }
        }
        int removeChar = getc(file);
        if(removeChar == '\n')
            lineNumber++;
        c = peekChar(file);
    }
}

/*
 *  pre: none
 *  post: returns true if c is valid hex 
 *  digit
 *
 */
int getHexDigit(char c) {
    if('0'<= c && c <= '9') 
        return c - '0';
    else if ('a' <= c && c <= 'f')
        return c - 'a' + 10;
    else 
        return c - 'A' + 10;
}

/*
 *  pre: none
 *  post: return true if g is int digit
 *
 */
int isValidInt(char c) {
    return ('0' <= c && c <= '9');
}

/*
 *  pre: none
 *  post: return true if g is valid hex digit.
 *
 */
int isValidDigit(char c) {
    return ('0' <= c && c <= '9') ||
            ('A' <= c && c <= 'F') ||
            ('a' <= c && c <= 'f');
}

/* 
 * pre: str is not null
 * post: str will be a lowercase string
 */
void toLowerCase(char* str, int n) {
    for(int i = 0; i < n; i++) {
        char c = str[i];
        if('A' <= c && c <= 'Z') 
            c = c - ('A' - 'a');
        str[i] = c;
    }
}


/*
 *  pre: ins != null
 *  post: print out information about the instruction 
 *  (Just a debug method.)
 *
 */ 
void printOp(op* ins) {
    printf("instruction: %s", instr[ins->instr]);
    if(ins->instr == OP || ins->instr == JUMP || ins->instr == CMOVE) {
        printf("\ttype: %d", ins->instrNum);
    }
    printf("\tregA: %d", ins->rA);
    printf("\tregB: %d", ins->rB);
    printf("\toffset: %lld",(long long int) ins->data.offset);
    if(ins->label != NULL) 
        printf("\tlabel: %s", ins->label);
    printf("\n");

}

/*
 *  pre: sym != null
 *  post: print out information about the symbol
 *  (just a debug method)
 *
 */
void printSymbol(symbol* sym) {
    printf("Symbol %s\tpos: %lld\n", sym->name, (long long int)sym->loc);
}