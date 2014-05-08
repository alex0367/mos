
ifeq ($(shell uname),Linux)
	CC              = gcc
	ASM             = nasm
	LD              = ld
else
	CC              = /opt/local/bin/i386-elf-gcc
	ASM     		= /opt/local/bin/nasm
	LD              = /opt/local/bin/i386-elf-ld
endif

CSTRICT	= -Werror=return-type -Werror=uninitialized
CFLAGS	= -m32 -c $(CSTRICT) -fno-stack-protector -fno-builtin
ASFLAGS	= -f elf32
LDFILE	= -m elf_i386 -T link.ld 
LDFLAGS	= $(LDFILE)
TARGET	= run
OBJS	= run.o 

run: $(OBJS)
	$(LD) $(LDFLAGS) -o $(TARGET) $(OBJS)

run.o: run.c run.h
	$(CC) $(CFLAGS) -o run.o run.c

