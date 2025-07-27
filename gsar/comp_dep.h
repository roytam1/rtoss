/* comp_dep.h ************************************* updated: 960801.15:00 TT
 *
 * This file contains a few constants which some compilers do not have
 * defined at all ( even though they're ANSI ).
 */

#ifndef FILENAME_MAX
#define FILENAME_MAX 255
#endif

#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS 0
#endif
#ifndef EXIT_FAILURE
#define EXIT_FAILURE 1
#endif

#ifndef SEEK_SET
#define SEEK_SET  0  /* seek from start of file */
#endif
#ifndef SEEK_CUR
#define SEEK_CUR  1  /* relative to current position */
#endif
#ifndef SEEK_END
#define SEEK_END  2  /* relative to end of file */
#endif

#ifndef S_ISREG
#define S_ISREG(m)  (((m)&(S_IFMT))==S_IFREG)
#endif
