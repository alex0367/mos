.global start;
.global libc_init;
.global tty_init;
.global main;

.func start;
start:
	movl %esp, %ebp
	call libc_init
	call tty_init

	pushl 8(%ebp)
	pushl 4(%ebp)
	pushl 0(%ebp)
	call main
	call exit
	ret

.endfunc

