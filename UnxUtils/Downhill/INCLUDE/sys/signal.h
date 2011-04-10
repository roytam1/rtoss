/*****************************************************************************
 *                                                                           *
 * sys/signal.h                                                              *
 *                                                                           *
 * Freely redistributable and modifiable.  Use at your own risk.             *
 *                                                                           *
 * Copyright 1994 The Downhill Project                                       *
 *                                                                           *
 *****************************************************************************/


#if !defined(_DOWNHILL_SYS_SIGNAL_H)
#define _DOWNHILL_SYS_SIGNAL_H


/* Include stuff *************************************************************/
#include "DOWNHILL/DH_POSIX.H"
#include "DOWNHILL/DH_INC.H"
#include _DOWNHILL_INCLUDE_DIR/signal.h>


/* Define stuff **************************************************************/

/* Fake signals ============================================================ */
#define SIGCHLD 23
#define SIGUSR1 24
#define SIGUSR2 25
#define SIGHUP  26
#define SIGQUIT 27
#define SIGCONT 28
#define SIGTTOU 29
#define SIGPIPE 30
#undef  NSIG
#define NSIG    31

/* Re-mapped functions ===================================================== */
#if (_DOWNHILL_POSIX_SOURCE == 0)
#define sigmask(signal_Index) (1<<(signal_Index-1))
#endif
#define signal(signal_Number,signal_Handler)                                  \
         Downhill_Signal_Set(signal_Number,signal_Handler)
#define raise(signal_Number)  Downhill_Signal_Raise(signal_Number)

/* Signal mask actions ===================================================== */
#define SIG_BLOCK   0
#define SIG_UNBLOCK 1
#define SIG_SETMASK 2

/* Signal flag actions ===================================================== */
#define SA_NOCLDSTOP 1
#if (_DOWNHILL_POSIX_SOURCE == 0)
#define SA_RESETHAND 2
#define SA_INTERRUPT 4
#endif


/* Typedef stuff *************************************************************/
typedef unsigned long sigset_t;


/* Struct stuff **************************************************************/
struct sigaction
{
	void     (*sa_handler)();
	sigset_t sa_mask;
	int      sa_flags;
};


/* Prototype stuff ***********************************************************/
#if (_DOWNHILL_POSIX_SOURCE == 0)
unsigned long sigsetmask(unsigned long signal_Block_MaskNew);
unsigned long sigblock(unsigned long signal_Block_MaskNew);
int           sighold(int signal_Number);
int           sigrelse(int signal_Number);
#endif
int           sigaction(int signal_Number,struct sigaction* sigaction_Info,
               struct sigaction* signaction_InfoOld);
int           sigaddset(sigset_t* sigset_Info,int signal_Number);
int           sigdelset(sigset_t* sigset_Info,int signal_Number);
int           sigemptyset(sigset_t* sigset_Info);
int           sigfillset(sigset_t* sigset_Info);
int           sigismember(sigset_t* sigset_Info,int signal_Number);
int           sigpending(sigset_t* sigset_Info);
int           sigprocmask(int mask_Function,sigset_t* sigset_Info,
               sigset_t* sigset_InfoOld);
void          (*Downhill_Signal_Set(int signal_Number,
               void (*signal_Handler)(int,...)))(int,...);
int           Downhill_Signal_Raise(int signal_Number);


#endif /* _DOWNHILL_SYS_SIGNAL_H */
