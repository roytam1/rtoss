#define CONFIG_FEATURE_VI_MAX_LEN 8192
#define ENABLE_FEATURE_VI_MAX_LEN 1
#define CONFIG_FEATURE_VI_8BIT 1
#define ENABLE_FEATURE_VI_8BIT 1
#define CONFIG_FEATURE_VI_COLON 1
#define ENABLE_FEATURE_VI_COLON 1
#define CONFIG_FEATURE_VI_YANKMARK 1
#define ENABLE_FEATURE_VI_YANKMARK 1
#define CONFIG_FEATURE_VI_SEARCH 1
#define ENABLE_FEATURE_VI_SEARCH 1
#ifndef __FreeBSD__
#define CONFIG_FEATURE_VI_REGEX_SEARCH 1
#define ENABLE_FEATURE_VI_REGEX_SEARCH 1
#else
#define CONFIG_FEATURE_VI_REGEX_SEARCH 0
#define ENABLE_FEATURE_VI_REGEX_SEARCH 0
#endif
#define CONFIG_FEATURE_VI_USE_SIGNALS 1
#define ENABLE_FEATURE_VI_USE_SIGNALS 1
#define CONFIG_FEATURE_VI_DOT_CMD 1
#define ENABLE_FEATURE_VI_DOT_CMD 1
#define CONFIG_FEATURE_VI_READONLY 1
#define ENABLE_FEATURE_VI_READONLY 1
#define CONFIG_FEATURE_VI_SETOPTS 1
#define ENABLE_FEATURE_VI_SETOPTS 1
#define CONFIG_FEATURE_VI_SET 1
#define ENABLE_FEATURE_VI_SET 1
#define CONFIG_FEATURE_VI_WIN_RESIZE 1
#define ENABLE_FEATURE_VI_WIN_RESIZE 1
#define CONFIG_FEATURE_VI_ASK_TERMINAL 1
#define ENABLE_FEATURE_VI_ASK_TERMINAL 1
#define CONFIG_FEATURE_ALLOW_EXEC 1
#define ENABLE_FEATURE_ALLOW_EXEC 1
# define IF_VI(...) __VA_ARGS__
#define IF_NOT_VI(...)
# define IF_FEATURE_VI_MAX_LEN(...) __VA_ARGS__
# define IF_FEATURE_VI_8BIT(...) __VA_ARGS__
#define IF_NOT_FEATURE_VI_8BIT(...)
# define IF_FEATURE_VI_COLON(...) __VA_ARGS__
#define IF_NOT_FEATURE_VI_COLON(...)
# define IF_FEATURE_VI_YANKMARK(...) __VA_ARGS__
#define IF_NOT_FEATURE_VI_YANKMARK(...)
# define IF_FEATURE_VI_SEARCH(...) __VA_ARGS__
#define IF_NOT_FEATURE_VI_SEARCH(...)
# define IF_FEATURE_VI_REGEX_SEARCH(...) __VA_ARGS__
#define IF_NOT_FEATURE_VI_REGEX_SEARCH(...)
# define IF_FEATURE_VI_USE_SIGNALS(...) __VA_ARGS__
#define IF_NOT_FEATURE_VI_USE_SIGNALS(...)
# define IF_FEATURE_VI_DOT_CMD(...) __VA_ARGS__
#define IF_NOT_FEATURE_VI_DOT_CMD(...)
# define IF_FEATURE_VI_READONLY(...) __VA_ARGS__
#define IF_NOT_FEATURE_VI_READONLY(...)
# define IF_FEATURE_VI_SETOPTS(...) __VA_ARGS__
#define IF_NOT_FEATURE_VI_SETOPTS(...)
# define IF_FEATURE_VI_SET(...) __VA_ARGS__
#define IF_NOT_FEATURE_VI_SET(...)
# define IF_FEATURE_VI_WIN_RESIZE(...) __VA_ARGS__
#define IF_NOT_FEATURE_VI_WIN_RESIZE(...)
# define IF_FEATURE_VI_ASK_TERMINAL(...) __VA_ARGS__
#define IF_NOT_FEATURE_VI_ASK_TERMINAL(...)
# define IF_FEATURE_ALLOW_EXEC(...) __VA_ARGS__
#define IF_NOT_FEATURE_ALLOW_EXEC(...)

#define ENABLE_FEATURE_EDITING_ASK_TERMINAL 0
#define IF_FEATURE_EDITING_ASK_TERMINAL(...)
#define IF_NOT_FEATURE_EDITING_ASK_TERMINAL(...) __VA_ARGS__
#define CONFIG_FEATURE_LESS_ASK_TERMINAL 1
#define ENABLE_FEATURE_LESS_ASK_TERMINAL 1
# define IF_FEATURE_LESS_ASK_TERMINAL(...) __VA_ARGS__
#define IF_NOT_FEATURE_LESS_ASK_TERMINAL(...)

#define BB_VER "0.1.23"
#define BB_BT

#define vi_trivial_usage "[OPTIONS] [FILE]..."
#define vi_full_usage "\n\n" \
       "Edit FILE\n" \
        IF_FEATURE_VI_COLON( \
     "\n        -c CMD  Initial command to run ($EXINIT also available)" \
        ) \
        IF_FEATURE_VI_READONLY( \
     "\n        -R      Read-only" \
        ) \
     "\n        -H      List available features"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <fcntl.h>
#include <signal.h>
#include <setjmp.h>
#include <regex.h>
#include <ctype.h>
#include <assert.h>
#include <errno.h>
#include <err.h>
#include <stdarg.h>
#include <stdint.h>
#include <poll.h>
#include <limits.h>

#include "ptr_to_globals.c"
#include "bbfuncs.c"
#include "read.c"
#include "read_key.c"
#include "safe_poll.c"
#include "skip_whitespace.c"
#include "last_char_is.c"
#include "safe_write.c"
#include "full_write.c"

char* progname;

int main(int argc, char** argv) {
  progname = argv[0];
  return vi_main(argc, argv);
}

void bb_show_usage(void) {
    fprintf(stderr, "%s %s%s\n",progname,vi_trivial_usage,vi_full_usage);
    return;
}

#include "vi.c"
