#define NUMINSTR 15
#define NUMOPQ 4
#define NUMBRANCH 7
#define NUMMOVE 7

#define HALTINSTR 0
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
#define PAUSE 15

#define ADDQ 0
#define SUBQ 1


//int instrBytes[NUMINSTR] = {1, 1, 1, 2, 10, 10, 10, 2, 9, 2, 9, 1, 2, 2, 10, 1};

char* registerNames[15] = { "rax", "rcx", "rdx", "rbx", "rsp", "rbp", 
		"rsi", "rdi", "r08", "r09", "r10", "r11", "r12", "r13", "r14"};