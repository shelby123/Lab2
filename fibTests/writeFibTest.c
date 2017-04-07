#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main(int argc, char** argv ) {
    int n = atoi(argv[1]);
    printf("(\n"
		"; wrapper to call fib\n"
		"(irmovq %d  %%rdi)\n"
		"(call fib)\n"
		"(halt)\n"




		"; argument is in rdi\n"
		"; return value is in rax\n"
		"; check if value is 1 or 0\n"
		"fib\n"
		"(rrmovq  %%rdi  %%r10) \n"
		"; check if r10 is 0\n"
		"(andq  %%r10  %%r10)\n"
		"(jg checkIfOne)"
		"(rrmovq  %%rdi  %%rax)\n"
		"(jmp done )\n"

		"\ncheckIfOne\n"
		"; check if r10 is 1\n"
		"(irmovq 1  %%r11)\n"
		"(subq  %%r11  %%r10)\n"
		"(jne continue)\n"
		"(rrmovq  %%rdi  %%rax)\n"
		"(jmp done)\n\n"

		"continue\n"
		"; recursive step. \n"
		"; fib n-1 + fib n - 2\n"
		"(irmovq 1  %%r11) \n"
		"(subq  %%r11  %%rdi)\n"
		"(pushq %%rdi)\n"
		"(call fib)\n"
		"(popq %%rdi)\n"
		"(subq  %%r11  %%rdi)\n"
		"(call fib)\n"
		"(popq %%r10)\n"
		"(addq  %%r10  %%rax)\n"


		"done\n"
		"(ret)\n"

		")\n", n);
}