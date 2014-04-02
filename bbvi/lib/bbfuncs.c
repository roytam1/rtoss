#define FAST_FUNC
#define ENABLE_DEBUG 0
#define bb_msg_memory_exhausted "memory exhausted"
#define bb_error_msg_and_die(s) do {fprintf(stderr,(s)); exit(1);} while(0);
#define bb_perror_msg(s) fprintf(stderr,(s))
#define bb_putchar(ch) putchar(ch)

void* FAST_FUNC xmalloc(size_t size)
{
        void *ptr = malloc(size);
        if (ptr == NULL && size != 0)
                bb_error_msg_and_die(bb_msg_memory_exhausted);
        return ptr;
}

// Die if we can't allocate and zero size bytes of memory.
void* FAST_FUNC xzalloc(size_t size)
{
        void *ptr = xmalloc(size);
        memset(ptr, 0, size);
        return ptr;
}

// Die if we can't copy a string to freshly allocated memory.
char* FAST_FUNC xstrdup(const char *s)
{
        char *t;

        if (s == NULL)
                return NULL;

        t = strdup(s);

        if (t == NULL)
                bb_error_msg_and_die(bb_msg_memory_exhausted);

        return t;
}

// Die if we can't allocate n+1 bytes (space for the null terminator) and copy
// the (possibly truncated to length n) string into it.
char* FAST_FUNC xstrndup(const char *s, int n)
{
        int m;
        char *t;

        if (ENABLE_DEBUG && s == NULL)
                bb_error_msg_and_die("xstrndup bug");

        /* We can just xmalloc(n+1) and strncpy into it, */
        /* but think about xstrndup("abc", 10000) wastage! */
        m = n;
        t = (char*) s;
        while (m) {
                if (!*t) break;
                m--;
                t++;
        }
        n -= m;
        t = xmalloc(n + 1);
        t[n] = '\0';

        return memcpy(t, s, n);
}

// Die if we can't resize previously allocated memory.  (This returns a pointer
// to the new memory, which may or may not be the same as the old memory.
// It'll copy the contents to a new chunk and free the old one if necessary.)
void* FAST_FUNC xrealloc(void *ptr, size_t size)
{
        ptr = realloc(ptr, size);
        if (ptr == NULL && size != 0)
                bb_error_msg_and_die(bb_msg_memory_exhausted);
        return ptr;
}

static int wh_helper(int value, int def_val, const char *env_name, int *err)
{
        if (value == 0) {
                char *s = getenv(env_name);
                if (s) {
                        value = atoi(s);
                        /* If LINES/COLUMNS are set, pretend that there is
                         * no error getting w/h, this prevents some ugly
                         * cursor tricks by our callers */
                        *err = 0;
                }
        }
        if (value <= 1 || value >= 30000)
                value = def_val;
        return value;
}

/* It is perfectly ok to pass in a NULL for either width or for
 * height, in which case that value will not be set.  */
int FAST_FUNC get_terminal_width_height(int fd, unsigned *width, unsigned *height)
{
        struct winsize win;
        int err;

        win.ws_row = 0;
        win.ws_col = 0;
        /* I've seen ioctl returning 0, but row/col is (still?) 0.
         * We treat that as an error too.  */
        err = ioctl(fd, TIOCGWINSZ, &win) != 0 || win.ws_row == 0;
        if (height)
                *height = wh_helper(win.ws_row, 24, "LINES", &err);
        if (width)
                *width = wh_helper(win.ws_col, 80, "COLUMNS", &err);
        return err;
}

int FAST_FUNC tcsetattr_stdin_TCSANOW(const struct termios *tp)
{
        return tcsetattr(STDIN_FILENO, TCSANOW, tp);
}

int FAST_FUNC fflush_all(void)
{
        return fflush(NULL);
}

