/* arg_func.h 
 *
 * Description : Header file for arg_func.c
 * Author      : Tormod Tjaberg
 */


/* variables and defines used by GetOpt
 */
extern int  OptInx;                 /* index into argv, skip argv[0]; filename */
extern int  CurOpt;                 /* current option checked for validity */
extern char *pOptArg;               /* argument associated with option */

#define  BAD_CHAR    (int) 1  /* option char not in option string */  
#define  MISSING_OPT (int) 2  /* switch character '-' but no option */
#define  MISSING_ARG (int) 3  /* option required argument but none found */
#define  NON_OPT     (int) 4  /* non option found '|' not in option string */

/* function prototypes 
 */
int GetOpt( int argc, char **argv, char *pOptStr );
int GetEnvArgs( int *argc, char ***argv, char *pEnvVar );
void Abort( char *,... );

