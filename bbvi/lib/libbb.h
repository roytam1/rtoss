#ifndef __LIBBB_H_
#define __LIBBB_H_
#define MAIN_EXTERNALLY_VISIBLE
#define FALSE 0
#define TRUE 1
#define NOINLINE 
#define UNUSED_PARAM __attribute__ ((__unused__))
# define ALIGN1 __attribute__((aligned(1)))

#define ARRAY_SIZE(x) ((unsigned)(sizeof(x) / sizeof((x)[0])))

#if defined(i386) || defined(__x86_64__) || defined(__mips__) || defined(__cris__)
/* add other arches which benefit from this... */
typedef signed char smallint;
typedef unsigned char smalluint;
#else
/* for arches where byte accesses generate larger code: */
typedef int smallint;
typedef unsigned smalluint;
#endif

/* At least gcc 3.4.6 on mipsel system needs optimization barrier */
#define barrier() __asm__ __volatile__("":::"memory")
#define SET_PTR_TO_GLOBALS(x) do { \
        (*(struct globals**)&ptr_to_globals) = (void*)(x); \
        barrier(); \
} while (0)
#define FREE_PTR_TO_GLOBALS() do { \
        if (ENABLE_FEATURE_CLEAN_UP) { \
                free(ptr_to_globals); \
        } \
} while (0)

/* "Keycodes" that report an escape sequence.
 * We use something which fits into signed char,
 * yet doesn't represent any valid Unicode character.
 * Also, -1 is reserved for error indication and we don't use it. */
enum {
        KEYCODE_UP       =  -2,
        KEYCODE_DOWN     =  -3,
        KEYCODE_RIGHT    =  -4,
        KEYCODE_LEFT     =  -5,
        KEYCODE_HOME     =  -6,
        KEYCODE_END      =  -7,
        KEYCODE_INSERT   =  -8,
        KEYCODE_DELETE   =  -9,
        KEYCODE_PAGEUP   = -10,
        KEYCODE_PAGEDOWN = -11,
        // -12 is reserved for Alt/Ctrl/Shift-TAB
#if 0
        KEYCODE_FUN1     = -13,
        KEYCODE_FUN2     = -14,
        KEYCODE_FUN3     = -15,
        KEYCODE_FUN4     = -16,
        KEYCODE_FUN5     = -17,
        KEYCODE_FUN6     = -18,
        KEYCODE_FUN7     = -19,
        KEYCODE_FUN8     = -20,
        KEYCODE_FUN9     = -21,
        KEYCODE_FUN10    = -22,
        KEYCODE_FUN11    = -23,
        KEYCODE_FUN12    = -24,
#endif
        /* Be sure that last defined value is small enough
         * to not interfere with Alt/Ctrl/Shift bits.
         * So far we do not exceed -31 (0xfff..fffe1),
         * which gives us three upper bits in LSB to play with.
         */
        //KEYCODE_SHIFT_TAB  = (-12)         & ~0x80,
        //KEYCODE_SHIFT_...  = KEYCODE_...   & ~0x80,
        //KEYCODE_CTRL_UP    = KEYCODE_UP    & ~0x40,
        //KEYCODE_CTRL_DOWN  = KEYCODE_DOWN  & ~0x40,
        KEYCODE_CTRL_RIGHT = KEYCODE_RIGHT & ~0x40,
        KEYCODE_CTRL_LEFT  = KEYCODE_LEFT  & ~0x40,
        //KEYCODE_ALT_UP     = KEYCODE_UP    & ~0x20,
        //KEYCODE_ALT_DOWN   = KEYCODE_DOWN  & ~0x20,
        KEYCODE_ALT_RIGHT  = KEYCODE_RIGHT & ~0x20,
        KEYCODE_ALT_LEFT   = KEYCODE_LEFT  & ~0x20,

        KEYCODE_CURSOR_POS = -0x100, /* 0xfff..fff00 */
        /* How long is the longest ESC sequence we know?
         * We want it big enough to be able to contain
         * cursor position sequence "ESC [ 9999 ; 9999 R"
         */
        KEYCODE_BUFFER_SIZE = 16
};

#endif
