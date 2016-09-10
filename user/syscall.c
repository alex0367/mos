#include <syscall.h>
#include <klib.h>


static TTY_COLOR fg_color;
static TTY_COLOR bg_color;

static void tty_set_color(TTY_COLOR fg, TTY_COLOR bg);
static void tty_get_color(TTY_COLOR* fg, TTY_COLOR* bg);



static void tty_issue_cmd(tty_command* cmd)
{
    ioctl(1, IOCTL_TTY, cmd);
}

static void tty_set_color(TTY_COLOR fg, TTY_COLOR bg)
{
    tty_command cmd;
    cmd.cmd_id = tty_cmd_set_color;
    cmd.color.fg_color = fg;
    cmd.color.bg_color = bg;
    tty_issue_cmd(&cmd);
}

static void tty_get_color(TTY_COLOR* fg, TTY_COLOR* bg)
{
    tty_command cmd;
    cmd.cmd_id = tty_cmd_get_color;
    tty_issue_cmd(&cmd);
    *fg = cmd.color.fg_color;
    *bg = cmd.color.bg_color;
}

void tty_set_fg_color(unsigned color)
{
    fg_color = color;
    tty_set_color(fg_color, bg_color);
}

void tty_set_bg_color(unsigned color)
{
    bg_color = color;
    tty_set_color(fg_color, bg_color);
}


void tty_init()
{
    tty_get_color(&fg_color, &bg_color);
}

#define SYSCALL0(no) \
	__asm__("movl %0, %%eax" : : "i"(no));\
	__asm__("int %0" : : "i"(SYSCALL_INT_NO));\

#define SYSCALL1(no, arg0) \
	__asm__("movl %0, %%ebx" : : "m"(arg0));\
	SYSCALL0(no);

#define SYSCALL2(no, arg0, arg1) \
	__asm__("movl %0, %%ecx" : : "m"(arg1));\
	SYSCALL1(no, arg0);

#define SYSCALL3(no, arg0, arg1, arg2) \
	__asm__("movl %0, %%edx" : : "m"(arg2));\
	SYSCALL2(no, arg0, arg1);

#define DEFINE_SYSCALL0(name) \
	int name() \
	{ \
		SYSCALL0(__NR_##name); \
	}

#define DEFINE_SYSCALL1(name, type0, arg0) \
	int name(type0 arg0)\
	{ \
		SYSCALL1(__NR_##name, arg0); \
	}

#define DEFINE_SYSCALL2(name, type0, arg0, type1, arg1) \
	int name(type0 arg0, type1 arg1) \
	{ \
		SYSCALL2(__NR_##name, arg0, arg1); \
	}

#define DEFINE_SYSCALL3(name, type0, arg0, type1, arg1, type2, arg2) \
	int name(type0 arg0, type1 arg1, type2 arg2) \
	{ \
		SYSCALL3(__NR_##name, arg0, arg1, arg2);\
	}



DEFINE_SYSCALL1(exit, unsigned, status);
DEFINE_SYSCALL0(fork)
DEFINE_SYSCALL3(read, unsigned, fd, const char*, buf, unsigned, len)
DEFINE_SYSCALL3(write, unsigned, fd, const char*, buf, unsigned, len)
DEFINE_SYSCALL1(open, const char*, path)
DEFINE_SYSCALL1(close, unsigned, fd)
DEFINE_SYSCALL3(waitpid, unsigned, pid, int*, status, int, option);
DEFINE_SYSCALL3(execve, const char*, path, char** const, argv, char** const, env)
DEFINE_SYSCALL0(getpid);
DEFINE_SYSCALL1(uname, struct utsname*, utname);
DEFINE_SYSCALL0(sched_yield);
DEFINE_SYSCALL2(stat, const char*, path, struct stat*, buf);
DEFINE_SYSCALL2(readdir, unsigned, fd, struct dirent*, ent);
DEFINE_SYSCALL1(brk, unsigned, addr);
DEFINE_SYSCALL1(time, time_t*, t);
DEFINE_SYSCALL2(getcwd, const char*, name, unsigned, len);
DEFINE_SYSCALL1(chdir, const char*, path);
DEFINE_SYSCALL3(ioctl, unsigned, fd, unsigned, cmd, void*, buf);
DEFINE_SYSCALL2(creat, const char*, path, unsigned, mode);
DEFINE_SYSCALL1(rmdir, const char*, path);
DEFINE_SYSCALL2(mkdir, const char*, path, unsigned, mode);
DEFINE_SYSCALL1(reboot, unsigned, cmd);
DEFINE_SYSCALL0(pause);
DEFINE_SYSCALL1(quota, struct krnquota*, q);
