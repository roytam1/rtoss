/*
	termutil.c
	System dependent terminal utility functions

	Copyright (c) 2006 Ken Kato
*/

#include "termutil.h"

#if defined(_WIN32)
/*
	win32 specific utility functions
*/
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <conio.h>

/*	move up cursor */
void cursor_up(int rows)
{
	HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

	if (GetFileType(hStdOut) == FILE_TYPE_CHAR) {
		CONSOLE_SCREEN_BUFFER_INFO console_info;

		if (GetConsoleScreenBufferInfo(hStdOut, &console_info)) {

			console_info.dwCursorPosition.X = 0;
			console_info.dwCursorPosition.Y =
				(short)(console_info.dwCursorPosition.Y - rows);

			SetConsoleCursorPosition(
				hStdOut, console_info.dwCursorPosition);
		}
	}
}

/*	erase to the end of current line */
void erase_eol()
{
	HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

	if (GetFileType(hStdOut) == FILE_TYPE_CHAR) {
		CONSOLE_SCREEN_BUFFER_INFO console_info;

		if (GetConsoleScreenBufferInfo(hStdOut, &console_info)) {
			DWORD result;

			FillConsoleOutputCharacter(
				hStdOut, ' ',
				console_info.dwSize.X - console_info.dwCursorPosition.X,
				console_info.dwCursorPosition,
				&result);
		}
	}
}

/*	prepare peek key operation */
void peekkey_prep()
{
	/* nothing to do */
}

/*	end peek key operation */
void peekkey_end()
{
	/* nothing to do */
}

/*	get pressed key without blocking */
int peekkey()
{
	return (_kbhit() ? _getch() : 0);
}

#elif defined(_MSDOS)
/*
	DOS specific utility functions
*/
#include <conio.h>
#include <disp.h>
#include <io.h>

/*	move cursor up */
void cursor_up(int rows)
{
	if (_isatty(_fileno(stdout))) {
		disp_open();
		disp_move(disp_cursorrow - rows, 0);
		disp_close();
	}
}

/*	erase to the end of current line */
void erase_eol()
{
	if (_isatty(_fileno(stdout))) {
		disp_open();
		disp_eeol();
		disp_close();
	}
}

/*	prepare peek key operation */
void peekkey_prep()
{
	/* nothing to do */
}

/*	end peek key operation */
void peekkey_end()
{
	/* nothing to do */
}

/*	get pressed key without blocking */
int peekkey()
{
	return (_kbhit() ? _getch() : 0);
}

#else
/*
	generic unix systems
*/
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <termios.h>
#include <unistd.h>

/*
	if a key is pressed, return the character code
	otherwise return 0 (do not wait for input)
*/

static struct termios tio_orig;
static int noncanonical = 0;

static int __isatty(int fd)
{
	struct stat st;

	return (fstat(fd, &st) == 0 && S_ISCHR(st.st_mode));
}

/*	prepare peek key operation		*/
/*	set stdin to non-canonical mode */
void peekkey_prep()
{
	if (__isatty(0)) {
		struct termios tio_new;

		if (tcgetattr(0, &tio_orig) == 0) {
			memcpy(&tio_new, &tio_orig, sizeof(struct termios));

			tio_new.c_lflag = 0;
			tio_new.c_cc[VMIN] = 0;
			tio_new.c_cc[VTIME] = 0;

			if (tcsetattr(0, TCSANOW, &tio_new) == 0) {
				noncanonical = 1;
			}
		}
	}
}

/*	end peek key operation */
/*	revert to the original mode */
void peekkey_end()
{
	if (noncanonical) {
		tcsetattr(0, TCSANOW, &tio_orig);
		noncanonical = 0;
	}
}

/*	read keyboard input */
int peekkey()
{
	char c = 0;

	if (noncanonical) {
		read(0, &c, 1);
	}

	return c;
}

/*	move cursor up */
void cursor_up(int rows)
{
	if (__isatty(1)) {
		printf("\x1b[%dA", rows);
	}
}

/*	erase to the end of current line */
void erase_eol()
{
	if (__isatty(1)) {
		/* printf("\x1b[M"); */
		printf("\x1b[K");
	}
}

#endif	/* unix like systems */
