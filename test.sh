#!/bin/bash

echo Running Quicksort Tests

testLocation=quickSortTests

testFile=( 
		qs1.txt
		qs2.txt
		)
testOutput=( 
		qs1Res.txt
		qs1Res.txt
	)
testName=( 
		quickSort_length_101
		quickSort_length_101_2
	)
testHighMem=(
		4903
		4903
	)

gcc -o cmpFiles cmpFiles.c 

make clean
make assembler
make simulator

echo 
for i in {0..1}
	do 
		./assembler $testLocation/${testFile[$i]} > $testLocation/assemblerOutput.txt
		./simulator $testLocation/assemblerOutput.txt ${testHighMem[$i]} > $testLocation/simulatorOutput.txt
		./cmpFiles $testLocation/${testOutput[$i]} $testLocation/simulatorOutput.txt ${testName[$i]}
	done

echo finished
echo
