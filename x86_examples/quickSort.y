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
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	-8(%rbp), %rsi
	movl	(%rsi), %eax
	movl	%eax, -20(%rbp)
	movq	-16(%rbp), %rsi
	movl	(%rsi), %eax
	movq	-8(%rbp), %rsi
	movl	%eax, (%rsi)
	movl	-20(%rbp), %eax
	movq	-16(%rbp), %rsi
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
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movl	%edx, -16(%rbp)
	movslq	-16(%rbp), %rdi
	movq	-8(%rbp), %rax
	movl	(%rax,%rdi,4), %edx
	movl	%edx, -20(%rbp)
	movl	-12(%rbp), %edx
	subl	$1, %edx
	movl	%edx, -24(%rbp)
	movl	-12(%rbp), %edx
	movl	%edx, -28(%rbp)
LBB1_1:                                 ## =>This Inner Loop Header: Depth=1
	movl	-28(%rbp), %eax
	movl	-16(%rbp), %ecx
	subl	$1, %ecx
	cmpl	%ecx, %eax
	jg	LBB1_6
## BB#2:                                ##   in Loop: Header=BB1_1 Depth=1
	movslq	-28(%rbp), %rax
	movq	-8(%rbp), %rcx
	movl	(%rcx,%rax,4), %edx
	cmpl	-20(%rbp), %edx
	jg	LBB1_4
## BB#3:                                ##   in Loop: Header=BB1_1 Depth=1
	movl	-24(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -24(%rbp)
	movslq	-24(%rbp), %rcx
	shlq	$2, %rcx
	addq	-8(%rbp), %rcx
	movslq	-28(%rbp), %rdx
	shlq	$2, %rdx
	addq	-8(%rbp), %rdx
	movq	%rcx, %rdi
	movq	%rdx, %rsi
	callq	_swap
LBB1_4:                                 ##   in Loop: Header=BB1_1 Depth=1
	jmp	LBB1_5
LBB1_5:                                 ##   in Loop: Header=BB1_1 Depth=1
	movl	-28(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -28(%rbp)
	jmp	LBB1_1
LBB1_6:
	movl	-24(%rbp), %eax
	addl	$1, %eax
	movslq	%eax, %rcx
	shlq	$2, %rcx
	addq	-8(%rbp), %rcx
	movslq	-16(%rbp), %rdx
	shlq	$2, %rdx
	addq	-8(%rbp), %rdx
	movq	%rcx, %rdi
	movq	%rdx, %rsi
	callq	_swap
	movl	-24(%rbp), %eax
	addl	$1, %eax
	addq	$32, %rsp
	popq	%rbp
	retq
	.cfi_endproc

	.globl	_quickSort
	.align	4, 0x90


; **************** QUICKSORT **********************

_quickSort:                             ## @quickSort
	.cfi_startproc
## BB#0:
	( pushq	%rbp)
	(rrmovq	%rsp %rbp)
Ltmp8:
	.cfi_def_cfa_register %rbp
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movl	%edx, -16(%rbp)
	movl	-12(%rbp), %edx
	cmpl	-16(%rbp), %edx
	jge	LBB2_2
## BB#1:
	movq	-8(%rbp), %rdi
	movl	-12(%rbp), %esi
	movl	-16(%rbp), %edx
	callq	_partition
	movl	%eax, -20(%rbp)
	movq	-8(%rbp), %rdi
	movl	-12(%rbp), %esi
	movl	-20(%rbp), %eax
	subl	$1, %eax
	movl	%eax, %edx
	callq	_quickSort
	movq	-8(%rbp), %rdi
	movl	-20(%rbp), %eax
	addl	$1, %eax
	movl	-16(%rbp), %edx
	movl	%eax, %esi
	callq	_quickSort
LBB2_2:
	addq	$32, %rsp
	popq	%rbp
	retq