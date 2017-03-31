#define NUMINSTR 16
#define NUMOPQ 4
#define NUMBRANCH 7
#define NUMMOVE 7

#define HALT 0
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
#define IADDQ 14
#define LEAVE 15


int instrBytes[NUMINSTR] = {1, 1, 1, 2, 10, 10, 10, 2, 9, 2, 9, 1, 2, 2, 10, 1};