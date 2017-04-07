	.section	__TEXT,__text,regular,pure_instructions
	.macosx_version_min 10, 12
	.globl	_swap
	.align	4, 0x90
_swap:                                  ## @swap
	.cfi_startproc
## BB#0:
	pushq	%rbp
Ltmp0:
	.cfi_def_cfa_offset 16
Ltmp1:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
Ltmp2:
	.cfi_def_cfa_register %rbp
	movl	(%rdi), %eax
	movl	(%rsi), %ecx
	movl	%ecx, (%rdi)
	movl	%eax, (%rsi)
	popq	%rbp
	retq
	.cfi_endproc

	.globl	_partition
	.align	4, 0x90
_partition:                             ## @partition
	.cfi_startproc
## BB#0:
	pushq	%rbp
Ltmp3:
	.cfi_def_cfa_offset 16
Ltmp4:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
Ltmp5:
	.cfi_def_cfa_register %rbp
	movslq	%edx, %r8
	movl	(%rdi,%r8,4), %r11d
	leal	-1(%rsi), %eax
	movl	%r8d, %ecx
	subl	%esi, %ecx
	jle	LBB1_13
## BB#1:
	movslq	%esi, %r9
	leal	-1(%rdx), %r10d
	testb	$1, %cl
	je	LBB1_5
## BB#2:
	movl	(%rdi,%r9,4), %ecx
	cmpl	%r11d, %ecx
	jg	LBB1_4
## BB#3:
	movl	(%rdi,%r9,4), %eax
	movl	%ecx, (%rdi,%r9,4)
	movl	%eax, (%rdi,%r9,4)
	movl	%esi, %eax
LBB1_4:
	incq	%r9
LBB1_5:
	cmpl	%esi, %r10d
	je	LBB1_12
## BB#6:
	subl	%r9d, %edx
	leaq	4(%rdi,%r9,4), %rsi
	.align	4, 0x90
LBB1_7:                                 ## =>This Inner Loop Header: Depth=1
	movl	-4(%rsi), %ecx
	cmpl	%r11d, %ecx
	jg	LBB1_9
## BB#8:                                ##   in Loop: Header=BB1_7 Depth=1
	movslq	%eax, %r9
	incl	%eax
	movl	4(%rdi,%r9,4), %r10d
	movl	%ecx, 4(%rdi,%r9,4)
	movl	%r10d, -4(%rsi)
LBB1_9:                                 ##   in Loop: Header=BB1_7 Depth=1
	movl	(%rsi), %ecx
	cmpl	%r11d, %ecx
	jg	LBB1_11
## BB#10:                               ##   in Loop: Header=BB1_7 Depth=1
	movslq	%eax, %r9
	incl	%eax
	movl	4(%rdi,%r9,4), %r10d
	movl	%ecx, 4(%rdi,%r9,4)
	movl	%r10d, (%rsi)
LBB1_11:                                ##   in Loop: Header=BB1_7 Depth=1
	addq	$8, %rsi
	addl	$-2, %edx
	jne	LBB1_7
LBB1_12:
	movl	(%rdi,%r8,4), %r11d
LBB1_13:
	movslq	%eax, %rcx
	incl	%eax
	movl	4(%rdi,%rcx,4), %edx
	movl	%r11d, 4(%rdi,%rcx,4)
	movl	%edx, (%rdi,%r8,4)
	popq	%rbp
	retq
	.cfi_endproc

	.globl	_quickSort
	.align	4, 0x90
_quickSort:                             ## @quickSort
	.cfi_startproc
## BB#0:
	pushq	%rbp
Ltmp6:
	.cfi_def_cfa_offset 16
Ltmp7:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
Ltmp8:
	.cfi_def_cfa_register %rbp
	pushq	%r15
	pushq	%r14
	pushq	%r13
	pushq	%r12
	pushq	%rbx
	subq	$24, %rsp
Ltmp9:
	.cfi_offset %rbx, -56
Ltmp10:
	.cfi_offset %r12, -48
Ltmp11:
	.cfi_offset %r13, -40
Ltmp12:
	.cfi_offset %r14, -32
Ltmp13:
	.cfi_offset %r15, -24
	movl	%edx, %r14d
	movq	%rdi, %r13
	cmpl	%r14d, %esi
	jge	LBB2_14
## BB#1:
	movslq	%r14d, %r12
	leal	-1(%r14), %eax
	movl	%eax, -44(%rbp)         ## 4-byte Spill
	leaq	4(%r13), %rax
	movq	%rax, -56(%rbp)         ## 8-byte Spill
	.align	4, 0x90
LBB2_2:                                 ## =>This Loop Header: Depth=1
                                        ##     Child Loop BB2_8 Depth 2
	movl	(%r13,%r12,4), %r8d
	leal	-1(%rsi), %r9d
	movslq	%esi, %rdi
	movl	%r14d, %edx
	subl	%edi, %edx
	testb	$1, %dl
	je	LBB2_6
## BB#3:                                ##   in Loop: Header=BB2_2 Depth=1
	movl	(%r13,%rdi,4), %edx
	cmpl	%r8d, %edx
	jg	LBB2_5
## BB#4:                                ##   in Loop: Header=BB2_2 Depth=1
	movl	(%r13,%rdi,4), %ecx
	movl	%edx, (%r13,%rdi,4)
	movl	%ecx, (%r13,%rdi,4)
	movl	%esi, %r9d
LBB2_5:                                 ##   in Loop: Header=BB2_2 Depth=1
	incq	%rdi
LBB2_6:                                 ##   in Loop: Header=BB2_2 Depth=1
	cmpl	%esi, -44(%rbp)         ## 4-byte Folded Reload
	je	LBB2_13
## BB#7:                                ##   in Loop: Header=BB2_2 Depth=1
	movl	%r14d, %edx
	subl	%edi, %edx
	movq	-56(%rbp), %rax         ## 8-byte Reload
	leaq	(%rax,%rdi,4), %rdi
	.align	4, 0x90
LBB2_8:                                 ##   Parent Loop BB2_2 Depth=1
                                        ## =>  This Inner Loop Header: Depth=2
	movl	-4(%rdi), %ebx
	cmpl	%r8d, %ebx
	jg	LBB2_10
## BB#9:                                ##   in Loop: Header=BB2_8 Depth=2
	movslq	%r9d, %rax
	incl	%r9d
	movl	4(%r13,%rax,4), %ecx
	movl	%ebx, 4(%r13,%rax,4)
	movl	%ecx, -4(%rdi)
LBB2_10:                                ##   in Loop: Header=BB2_8 Depth=2
	movl	(%rdi), %ebx
	cmpl	%r8d, %ebx
	jg	LBB2_12
## BB#11:                               ##   in Loop: Header=BB2_8 Depth=2
	movslq	%r9d, %rax
	incl	%r9d
	movl	4(%r13,%rax,4), %ecx
	movl	%ebx, 4(%r13,%rax,4)
	movl	%ecx, (%rdi)
LBB2_12:                                ##   in Loop: Header=BB2_8 Depth=2
	addq	$8, %rdi
	addl	$-2, %edx
	jne	LBB2_8
LBB2_13:                                ##   in Loop: Header=BB2_2 Depth=1
	movl	(%r13,%r12,4), %eax
	movslq	%r9d, %r15
	movl	4(%r13,%r15,4), %ecx
	movl	%eax, 4(%r13,%r15,4)
	movl	%ecx, (%r13,%r12,4)
	movq	%r13, %rdi
	movl	%r15d, %edx
	callq	_quickSort
	addl	$2, %r15d
	cmpl	%r14d, %r15d
	movl	%r15d, %esi
	jl	LBB2_2
LBB2_14:
	addq	$24, %rsp
	popq	%rbx
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%r15
	popq	%rbp
	retq
	.cfi_endproc


.subsections_via_symbols
