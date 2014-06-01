	.file	"syscall.c"
	.text
	.type	exit, @function
exit:
.LFB0:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
#APP
# 53 "./syscall.h" 1
	movl 8(%ebp), %ebx
# 0 "" 2
# 53 "./syscall.h" 1
	movl $1, %eax
# 0 "" 2
# 53 "./syscall.h" 1
	int $128
# 0 "" 2
#NO_APP
	popl	%ebp
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE0:
	.size	exit, .-exit
	.type	ioctl, @function
ioctl:
.LFB19:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
#APP
# 72 "./syscall.h" 1
	movl 16(%ebp), %edx
# 0 "" 2
# 72 "./syscall.h" 1
	movl 12(%ebp), %ecx
# 0 "" 2
# 72 "./syscall.h" 1
	movl 8(%ebp), %ebx
# 0 "" 2
# 72 "./syscall.h" 1
	movl $54, %eax
# 0 "" 2
# 72 "./syscall.h" 1
	int $128
# 0 "" 2
#NO_APP
	popl	%ebp
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE19:
	.size	ioctl, .-ioctl
	.local	fg_color
	.comm	fg_color,4,4
	.local	bg_color
	.comm	bg_color,4,4
	.globl	start
	.type	start, @function
start:
.LFB24:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$40, %esp
	call	libc_init
	movl	$bg_color, 4(%esp)
	movl	$fg_color, (%esp)
	call	tty_get_color
	movl	12(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	main
	movl	%eax, -12(%ebp)
	movl	-12(%ebp), %eax
	movl	%eax, (%esp)
	call	exit
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE24:
	.size	start, .-start
	.type	tty_issue_cmd, @function
tty_issue_cmd:
.LFB25:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$12, %esp
	movl	8(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	$0, 4(%esp)
	movl	$1, (%esp)
	call	ioctl
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE25:
	.size	tty_issue_cmd, .-tty_issue_cmd
	.type	tty_set_color, @function
tty_set_color:
.LFB26:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$20, %esp
	movl	$1, -12(%ebp)
	movl	8(%ebp), %eax
	movl	%eax, -8(%ebp)
	movl	12(%ebp), %eax
	movl	%eax, -4(%ebp)
	leal	-12(%ebp), %eax
	movl	%eax, (%esp)
	call	tty_issue_cmd
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE26:
	.size	tty_set_color, .-tty_set_color
	.type	tty_get_color, @function
tty_get_color:
.LFB27:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$20, %esp
	movl	$0, -12(%ebp)
	leal	-12(%ebp), %eax
	movl	%eax, (%esp)
	call	tty_issue_cmd
	movl	-8(%ebp), %edx
	movl	8(%ebp), %eax
	movl	%edx, (%eax)
	movl	-4(%ebp), %edx
	movl	12(%ebp), %eax
	movl	%edx, (%eax)
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE27:
	.size	tty_get_color, .-tty_get_color
	.globl	tty_set_fg_color
	.type	tty_set_fg_color, @function
tty_set_fg_color:
.LFB28:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$8, %esp
	movl	8(%ebp), %eax
	movl	%eax, fg_color
	movl	bg_color, %edx
	movl	fg_color, %eax
	movl	%edx, 4(%esp)
	movl	%eax, (%esp)
	call	tty_set_color
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE28:
	.size	tty_set_fg_color, .-tty_set_fg_color
	.globl	tty_set_bg_color
	.type	tty_set_bg_color, @function
tty_set_bg_color:
.LFB29:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$8, %esp
	movl	8(%ebp), %eax
	movl	%eax, bg_color
	movl	bg_color, %edx
	movl	fg_color, %eax
	movl	%edx, 4(%esp)
	movl	%eax, (%esp)
	call	tty_set_color
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE29:
	.size	tty_set_bg_color, .-tty_set_bg_color
	.ident	"GCC: (Ubuntu/Linaro 4.7.3-1ubuntu1) 4.7.3"
	.section	.note.GNU-stack,"",@progbits
