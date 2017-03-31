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

    fclose(file);
    return s;
}





















