#ifndef _SYSCALL_H_
#define _SYSCALL_H_
#include <syscall/unistd.h>
#include <config.h>
#include <lib/klib.h>


typedef void* DIR;
struct dirent;
typedef unsigned long time_t;

void tty_set_fg_color(unsigned color);
void tty_set_bg_color(unsigned color);
void tty_init();

#define DECLEAR_SYSCALL0(name) \
	int name();

#define DECLEAR_SYSCALL1(name, type0, arg0) \
	int name(type0 arg0);

#define DECLEAR_SYSCALL2(name, type0, arg0, type1, arg1) \
	int name(type0 arg0, type1 arg1);

#define DECLEAR_SYSCALL3(name, type0, arg0, type1, arg1, type2, arg2) \
	int name(type0 arg0, type1 arg1, type2 arg2);


DECLEAR_SYSCALL1(exit, unsigned, status);
DECLEAR_SYSCALL0(fork)
DECLEAR_SYSCALL3(read, unsigned, fd, const char*, buf, unsigned, len)
DECLEAR_SYSCALL3(write, unsigned, fd, const char*, buf, unsigned, len)
DECLEAR_SYSCALL1(open, const char*, path)
DECLEAR_SYSCALL1(close, unsigned, fd)
DECLEAR_SYSCALL3(waitpid, unsigned, pid, int*, status, int ,option);
DECLEAR_SYSCALL3(execve,const char*, path, char** const, argv, char** const, env)
DECLEAR_SYSCALL0(getpid);
DECLEAR_SYSCALL1(uname, struct utsname*, utname);
DECLEAR_SYSCALL0(sched_yield);
DECLEAR_SYSCALL2(stat, const char*, path, struct stat*, buf);
DECLEAR_SYSCALL2(readdir, unsigned, dir, struct dirent*, ent);
DECLEAR_SYSCALL1(brk, unsigned, addr);
DECLEAR_SYSCALL1(time, time_t*, t);
DECLEAR_SYSCALL2(getcwd, const char*, name, unsigned, len);
DECLEAR_SYSCALL1(chdir, const char*, path);
DECLEAR_SYSCALL3(ioctl, unsigned, fd, unsigned, cmd, void*, buf);
DECLEAR_SYSCALL2(creat, const char*, path, unsigned, mode);
DECLEAR_SYSCALL1(rmdir, const char*, path);
DECLEAR_SYSCALL2(mkdir, const char*, path, unsigned, mode);
DECLEAR_SYSCALL1(reboot, unsigned, cmd);
DECLEAR_SYSCALL0(pause);
#endif
