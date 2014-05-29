#include <syscall.h>
#include <lib/klib.h>


static TTY_COLOR fg_color;
static TTY_COLOR bg_color;

static void tty_set_color(TTY_COLOR fg, TTY_COLOR bg);
static void tty_get_color(TTY_COLOR* fg, TTY_COLOR* bg);

void start(int argc, char** argv, char** envp)
{
	unsigned status;

	libc_init();

	tty_get_color(&fg_color, &bg_color);

	extern int main(int argc, char** argv);

	status = main(argc, argv);

	exit(status);
}

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
