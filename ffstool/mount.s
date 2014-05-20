	.section	__TEXT,__text,regular,pure_instructions
	.globl	_mount_init
	.align	4, 0x90
_mount_init:                            ## @mount_init
	.cfi_startproc
## BB#0:
	pushq	%rbp
Ltmp2:
	.cfi_def_cfa_offset 16
Ltmp3:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
Ltmp4:
	.cfi_def_cfa_register %rbp
	leaq	_mnt_list(%rip), %rdi
	callq	_InitializeListHead
	leaq	_mnt_lock(%rip), %rdi
	leaq	L_.str(%rip), %rsi
	movl	$0, %edx
	callq	_sema_init
	popq	%rbp
	ret
	.cfi_endproc

	.globl	_do_mount
	.align	4, 0x90
_do_mount:                              ## @do_mount
	.cfi_startproc
## BB#0:
	pushq	%rbp
Ltmp7:
	.cfi_def_cfa_offset 16
Ltmp8:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
Ltmp9:
	.cfi_def_cfa_register %rbp
	subq	$48, %rsp
	movq	%rdi, -16(%rbp)
	movq	%rsi, -24(%rbp)
	movq	-16(%rbp), %rdi
	callq	_mount_lookup
	cmpq	$0, %rax
	je	LBB1_2
## BB#1:
	movl	$0, -4(%rbp)
	jmp	LBB1_3
LBB1_2:
	movabsq	$288, %rdi              ## imm = 0x120
	movb	$0, %al
	callq	_kmalloc
	movslq	%eax, %rdi
	movq	%rdi, -32(%rbp)
	movq	-32(%rbp), %rdi
	addq	$24, %rdi
	movq	-16(%rbp), %rsi
	callq	_strcpy
	leaq	_mnt_lock(%rip), %rdi
	movq	-24(%rbp), %rsi
	movq	-32(%rbp), %rcx
	movq	%rsi, 16(%rcx)
	movq	%rax, -40(%rbp)         ## 8-byte Spill
	callq	_sema_wait
	leaq	_mnt_list(%rip), %rdi
	movq	-32(%rbp), %rsi
	callq	_InsertTailList
	leaq	_mnt_lock(%rip), %rdi
	callq	_sema_trigger
	movl	$1, -4(%rbp)
LBB1_3:
	movl	-4(%rbp), %eax
	addq	$48, %rsp
	popq	%rbp
	ret
	.cfi_endproc

	.globl	_mount_lookup
	.align	4, 0x90
_mount_lookup:                          ## @mount_lookup
	.cfi_startproc
## BB#0:
	pushq	%rbp
Ltmp12:
	.cfi_def_cfa_offset 16
Ltmp13:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
Ltmp14:
	.cfi_def_cfa_register %rbp
	subq	$32, %rsp
	movq	%rdi, -16(%rbp)
	movq	-16(%rbp), %rdi
	callq	_do_mount_lookup
	movq	%rax, -24(%rbp)
	cmpq	$0, -24(%rbp)
	jne	LBB2_2
## BB#1:
	movq	$0, -8(%rbp)
	jmp	LBB2_3
LBB2_2:
	movq	-24(%rbp), %rax
	movq	16(%rax), %rax
	movq	%rax, -32(%rbp)
	movq	-32(%rbp), %rax
	movq	%rax, -8(%rbp)
LBB2_3:
	movq	-8(%rbp), %rax
	addq	$32, %rsp
	popq	%rbp
	ret
	.cfi_endproc

	.globl	_do_umount
	.align	4, 0x90
_do_umount:                             ## @do_umount
	.cfi_startproc
## BB#0:
	pushq	%rbp
Ltmp17:
	.cfi_def_cfa_offset 16
Ltmp18:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
Ltmp19:
	.cfi_def_cfa_register %rbp
	subq	$48, %rsp
	movq	%rdi, -16(%rbp)
	movq	-16(%rbp), %rdi
	callq	_do_mount_lookup
	movq	%rax, -24(%rbp)
	cmpq	$0, -24(%rbp)
	jne	LBB3_2
## BB#1:
	movl	$0, -4(%rbp)
	jmp	LBB3_3
LBB3_2:
	leaq	_mnt_lock(%rip), %rdi
	callq	_sema_wait
	leaq	_mnt_lock(%rip), %rdi
	movq	-24(%rbp), %rax
	movq	%rdi, -32(%rbp)         ## 8-byte Spill
	movq	%rax, %rdi
	movb	$0, %al
	callq	_RemoveEntryList
	movq	-32(%rbp), %rdi         ## 8-byte Reload
	movl	%eax, -36(%rbp)         ## 4-byte Spill
	callq	_sema_trigger
	movq	-24(%rbp), %rdi
	movb	$0, %al
	callq	_kfree
	movl	$1, -4(%rbp)
	movl	%eax, -40(%rbp)         ## 4-byte Spill
LBB3_3:
	movl	-4(%rbp), %eax
	addq	$48, %rsp
	popq	%rbp
	ret
	.cfi_endproc

	.align	4, 0x90
_do_mount_lookup:                       ## @do_mount_lookup
	.cfi_startproc
## BB#0:
	pushq	%rbp
Ltmp22:
	.cfi_def_cfa_offset 16
Ltmp23:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
Ltmp24:
	.cfi_def_cfa_register %rbp
	subq	$48, %rsp
	leaq	_mnt_lock(%rip), %rax
	movq	%rdi, -16(%rbp)
	movq	$0, -32(%rbp)
	movl	$0, -36(%rbp)
	movq	%rax, %rdi
	callq	_sema_wait
	movq	_mnt_list(%rip), %rax
	movq	%rax, -24(%rbp)
LBB4_1:                                 ## =>This Inner Loop Header: Depth=1
	leaq	_mnt_list(%rip), %rax
	cmpq	%rax, -24(%rbp)
	je	LBB4_5
## BB#2:                                ##   in Loop: Header=BB4_1 Depth=1
	movq	-24(%rbp), %rax
	movq	%rax, -32(%rbp)
	movq	-32(%rbp), %rax
	addq	$24, %rax
	movq	-16(%rbp), %rsi
	movq	%rax, %rdi
	callq	_strcmp
	cmpl	$0, %eax
	jne	LBB4_4
## BB#3:
	movl	$1, -36(%rbp)
	jmp	LBB4_5
LBB4_4:                                 ##   in Loop: Header=BB4_1 Depth=1
	movq	-24(%rbp), %rax
	movq	(%rax), %rax
	movq	%rax, -24(%rbp)
	jmp	LBB4_1
LBB4_5:
	leaq	_mnt_lock(%rip), %rdi
	callq	_sema_trigger
	cmpl	$0, -36(%rbp)
	jne	LBB4_7
## BB#6:
	movq	$0, -8(%rbp)
	jmp	LBB4_8
LBB4_7:
	movq	-32(%rbp), %rax
	movq	%rax, -8(%rbp)
LBB4_8:
	movq	-8(%rbp), %rax
	addq	$48, %rsp
	popq	%rbp
	ret
	.cfi_endproc

.zerofill __DATA,__bss,_mnt_list,16,3   ## @mnt_list
.zerofill __DATA,__bss,_mnt_lock,20,2   ## @mnt_lock
	.section	__TEXT,__cstring,cstring_literals
L_.str:                                 ## @.str
	.asciz	 "mount"


.subsections_via_symbols
