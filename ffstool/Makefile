CC = gcc
CSTRICT	= -Werror=return-type -Werror=uninitialized
CFLAGS	= -ggdb3 -c $(CSTRICT) -fno-stack-protector -I./ -I../  -DDEBUG

OBJS = ffs.o\
		ffstool.o\
		fileblock.o

all: $(OBJS)
	$(CC) -o ffstool $(OBJS)

ffs.o: ../fs/ffs.c ../fs/ffs.h
	$(CC) $(CFLAGS) -o ffs.o ../fs/ffs.c

ffstool.o: ffstool.c ../fs/ffs.h
	$(CC) $(CFLAGS) -o ffstool.o ffstool.c

fileblock.o: fileblock.c fileblock.h
	$(CC) $(CFLAGS) -o fileblock.o fileblock.c

clean:
	-rm *.o
	-rm ffstool
