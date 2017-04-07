#!/bin/bash

echo Running Quicksort Tests

testLocation=quickSortTests

testFile=( 
		qs1.txt
		qs2.txt
		qs3.txt
		)
testOutput=( 
		qs1Res.txt
		qs2Res.txt
		qs3Res.txt
	)
testName=( 
		quickSort_length_10
		quickSort_length_100
		quickSort_length_1000
	)
testHighMem=(
		4183
		4903
		12103
	)

gcc -o cmpFiles cmpFiles.c 

make clean
make assembler
make simulator

echo 
for i in {0..2}
	do 
		./assembler $testLocation/${testFile[$i]} > $testLocation/assemblerOutput.txt
		./simulator $testLocation/assemblerOutput.txt ${testHighMem[$i]} > $testLocation/simulatorOutput.txt
		./cmpFiles $testLocation/${testOutput[$i]} $testLocation/simulatorOutput.txt ${testName[$i]}
	done

echo finished
echo
