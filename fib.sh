#!/bin/bash

echo Running Fibonnaci Tests

testFolder= fibTests

gcc -o output fibTests/writeFibTest.c 

make clean
make assembler
make simulator

echo 
for i in {0..20}
	do 
		output $i > fibTests/fibY.txt
		./assembler fibTests/fibY.txt > fibTests/assemblerOutput.txt
		./simulator fibTests/assemblerOutput.txt
	done
echo finished
echo
