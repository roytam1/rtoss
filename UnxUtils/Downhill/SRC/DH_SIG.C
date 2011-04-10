/*****************************************************************************
 *                                                                           *
 * DH_SIG.C                                                                  *
 *                                                                           *
 * Freely redistributable and modifiable.  Use at your own risk.             *
 *                                                                           *
 * Copyright 1994 The Downhill Project                                       *
 *                                                                           *
 *****************************************************************************/


/* Include stuff *************************************************************/
#include "INCLUDE/DH.H"
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <errno.h>
#include <sys/signal.h>


/* Define stuff ************************************************************ */

/* Signal groupings ======================================================== */
#define DOWNHILL_SIGNAL_REAL_EXIT   case SIGABRT:               \
                                    case SIGFPE:                \
                                    case SIGILL:                \
                                    case SIGINT:                \
                                    case SIGSEGV:               \
                                    case SIGTERM:
#define DOWNHILL_SIGNAL_REAL_IGNORE
#define DOWNHILL_SIGNAL_FAKE_EXIT   case SIGHUP:                \
                                    case SIGQUIT:
#define DOWNHILL_SIGNAL_FAKE_IGNORE case SIGCHLD:               \
                                    case SIGUSR1:               \
                                    case SIGUSR2:               \
                                    case SIGCONT:               \
                                    case SIGTTOU:               \
                                    case SIGPIPE:
#define DOWNHILL_SIGNAL_ALL         DOWNHILL_SIGNAL_REAL_EXIT   \
                                    DOWNHILL_SIGNAL_REAL_IGNORE \
                                    DOWNHILL_SIGNAL_FAKE_EXIT   \
                                    DOWNHILL_SIGNAL_FAKE_IGNORE


/* Struct stuff **************************************************************/
struct downhill_Signal_Struct
{
	void     (*signal_Handler)(int,...);
	int      signal_Count;
	int      signal_Extra;
	sigset_t signal_Mask;
	int      signal_Flags;
};


/* Static stuff **************************************************************/
static struct downhill_Signal_Struct* downhill_Signal_Info = NULL;
static sigset_t                       downhill_Sigset_Mask = 0;
static int                            downhill_Signal_Interrupt = FALSE;


/* Function stuff ************************************************************/

/* Return if an interrupt has occured ====================================== */
int downhill_Signal_IsInterrupted(void)
{
	downhill_Signal_Interrupt = FALSE;

	Downhill_Signal_Check();

	return downhill_Signal_Interrupt;
}

/* Handle a signal ========================================================= */
static void downhill_Signal_Handle(int signal_Number,...)
{
	va_list va_List;

	/* Start up the variable argument stuff */
	va_start(va_List,signal_Number);

	/* Check to see if we're masking this signal */
	if (sigismember(&downhill_Sigset_Mask,signal_Number))
	{
		downhill_Signal_Info[signal_Number].signal_Count++;
		if (signal_Number == SIGFPE)
		{
			downhill_Signal_Info[signal_Number].signal_Extra =
			 va_arg(va_List,int);
		}
	}
	else
	{
		/* Do a signal's action */
		if (downhill_Signal_Info[signal_Number].signal_Handler ==
		 SIG_DFL)
		{
			switch (signal_Number)
			{
				DOWNHILL_SIGNAL_REAL_EXIT
				DOWNHILL_SIGNAL_FAKE_EXIT
					exit(3);
				DOWNHILL_SIGNAL_REAL_IGNORE
				DOWNHILL_SIGNAL_FAKE_IGNORE
					break;
				default:
					fprintf(stderr,
					 "Unknown signal %d -- ignored\n",
					 signal_Number);
			}
		}
		else if (downhill_Signal_Info[signal_Number].signal_Handler ==
		 SIG_IGN)
		{
			/* Ignore */
		}
		else
		{
			void (*signal_HandlerOld)(int,...);
			
			/* Do we reset the handler? */
			signal_HandlerOld =
			 downhill_Signal_Info[signal_Number].signal_Handler;
#if (_DOWNHILL_POSIX_SOURCE == 0)
			if (downhill_Signal_Info[signal_Number].signal_Flags&
			 SA_RESETHAND)
			{
				downhill_Signal_Info[signal_Number].
				 signal_Handler = SIG_DFL;
			}
#endif

			/* Do the action */
			if ((signal_Number == SIGCHLD) &&
			 (downhill_Signal_Info[signal_Number].signal_Flags&
			 SA_NOCLDSTOP))
			{
				/* Ignore SIGCHLD */
			}
			else
			{
				sigset_t sigset_MaskOriginal =
				          downhill_Sigset_Mask;
				sigset_t sigset_MaskNew = downhill_Signal_Info[
				          signal_Number].signal_Mask;

				/* Set the new signal mask */
				sigaddset(&sigset_MaskNew,signal_Number);
				sigprocmask(SIG_BLOCK,&sigset_MaskNew,NULL);

				/* Execute the handler */
				if (signal_Number == SIGFPE)
				{
					signal_HandlerOld(signal_Number,va_arg(
					 va_List,int));
				}
				else
				{
					signal_HandlerOld(signal_Number);
				}

				/* Restore the signal mask */
				sigprocmask(SIG_SETMASK,&sigset_MaskOriginal,
				 NULL);

				/* Does this signal cause an interrupt? */
#if (_DOWNHILL_POSIX_SOURCE == 0)
				if (downhill_Signal_Info[signal_Number].
				 signal_Flags&SA_INTERRUPT)
				{
					downhill_Signal_Interrupt = TRUE;
				}
#endif
			}
		}
	}
}

/* Init the signal re-direction ============================================ */
static int downhill_Signal_Init(void)
{
	/* Skip this if we've already done it */
	if (downhill_Signal_Info == NULL)
	{
		int signal_Index;

		/* Get some memory */
		downhill_Signal_Info =
		 calloc(sizeof(struct downhill_Signal_Struct),NSIG);
		if (downhill_Signal_Info == NULL)
		{
			errno = ENOMEM;
			return FALSE;
		}

		/* Set everything up */
		for (signal_Index = 1;signal_Index < NSIG;signal_Index++)
		{
			switch (signal_Index)
			{
				DOWNHILL_SIGNAL_REAL_EXIT
				DOWNHILL_SIGNAL_REAL_IGNORE
#undef signal
					downhill_Signal_Info[signal_Index].
					 signal_Handler = signal(signal_Index,
					 downhill_Signal_Handle);
					break;
				default:
					downhill_Signal_Info[signal_Index].
					 signal_Handler = SIG_DFL;
					break;
			}
		}
	}

	return TRUE;
}

/* Set a signal action ===================================================== */
int sigaction(int signal_Number,struct sigaction* sigaction_Info,
     struct sigaction* sigaction_InfoOld)
{
	/* Make sure we're init'd */
	if (!downhill_Signal_Init())
	{
		return -1;
	}

	/* Set the signal */
	switch (signal_Number)
	{
		DOWNHILL_SIGNAL_ALL
			if (sigaction_InfoOld != NULL)
			{
				sigaction_InfoOld->sa_handler =
				 downhill_Signal_Info[signal_Number].
				 signal_Handler;
				sigaction_InfoOld->sa_mask =
				 downhill_Signal_Info[signal_Number].
				 signal_Mask;
				sigaction_InfoOld->sa_flags =
				 downhill_Signal_Info[signal_Number].
				 signal_Flags;
			}
			if (sigaction_Info != NULL)
			{
				downhill_Signal_Info[signal_Number].
				 signal_Handler = sigaction_Info->sa_handler;
				downhill_Signal_Info[signal_Number].
				 signal_Count = 0;
				downhill_Signal_Info[signal_Number].
				 signal_Extra = 0;
				downhill_Signal_Info[signal_Number].
				 signal_Mask = sigaction_Info->sa_mask;
				downhill_Signal_Info[signal_Number].
				 signal_Flags = sigaction_Info->sa_flags;
			}
			break;
		default:
			errno = EINVAL;
			return -1;
	}

	return 0;
}

/* Set the action of a signal ============================================== */
void (*Downhill_Signal_Set(int signal_Number,
 void (*signal_Handler)(int,...)))(int,...)
{
	void (*signal_HandlerOld)(int,...);

	/* Make sure we're init'd */
	if (!downhill_Signal_Init())
	{
		return SIG_ERR;
	}

	/* Make set the signal */
	switch (signal_Number)
	{
		DOWNHILL_SIGNAL_ALL
			signal_HandlerOld =
			 downhill_Signal_Info[signal_Number].signal_Handler;
			downhill_Signal_Info[signal_Number].signal_Handler =
			 signal_Handler;
			downhill_Signal_Info[signal_Number].signal_Count = 0;
			downhill_Signal_Info[signal_Number].signal_Extra = 0;
			downhill_Signal_Info[signal_Number].signal_Mask = 0;
			downhill_Signal_Info[signal_Number].signal_Flags = 0;

			return signal_HandlerOld;
	}

	errno = EINVAL;
	return SIG_ERR;
}

/* Add a signal to a set =================================================== */
int sigaddset(sigset_t* sigset_Info,int signal_Number)
{
	switch (signal_Number)
	{
		DOWNHILL_SIGNAL_ALL
			(*sigset_Info) |= ~1<<signal_Number;
			break;
		default:
			errno = EINVAL;
			return -1;
	}

	return 0;
}

/* Remove a signal from a set ============================================== */
int sigdelset(sigset_t* sigset_Info,int signal_Number)
{
	switch (signal_Number)
	{
		DOWNHILL_SIGNAL_ALL
			*sigset_Info &= (1<<signal_Number);
			break;
		default:
			errno = EINVAL;
			return -1;
	}

	return 0;
}

/* Empty a set ============================================================= */
int sigemptyset(sigset_t* sigset_Info)
{
	*sigset_Info = 0;

	return 0;
}

/* Fill a set ============================================================== */
int sigfillset(sigset_t* sigset_Info)
{
	*sigset_Info = (sigset_t)-1;

	return 0;
}

/* Checks if a signal is in a set ========================================== */
int sigismember(sigset_t* sigset_Info,int signal_Number)
{
	switch (signal_Number)
	{
		DOWNHILL_SIGNAL_ALL
			if (*sigset_Info && (1<<signal_Number))
			{
				return 1;
			}
			break;
		default:
			errno = EINVAL;
			return -1;
	}

	return 0;
}

/* Returns the signals pending ============================================= */
int sigpending(sigset_t* sigset_Info)
{
	int signal_Index;

	/* Check all the pending signals */
	for (signal_Index = 1;signal_Index < NSIG;signal_Index++)
	{
		if (downhill_Signal_Info[signal_Index].signal_Count > 0)
		{
			sigaddset(sigset_Info,signal_Index);
		}
	}

	return 0;
}

/* Change the blocked signals ============================================== */
int sigprocmask(int mask_Function,sigset_t* sigset_Info,
     sigset_t* sigset_InfoOld)
{
	int      signal_Index;
	sigset_t sigset_MaskOld = downhill_Sigset_Mask;

	/* Make sure we're init'd */
	if (!downhill_Signal_Init())
	{
		return -1;
	}

	/* Return the current value */
	if (sigset_InfoOld != NULL)
	{
		*sigset_InfoOld = sigset_MaskOld;
	}

	/* Set the new mask */
	switch (mask_Function)
	{
		case SIG_BLOCK:
			downhill_Sigset_Mask |= (*sigset_Info);
			break;
		case SIG_UNBLOCK:
			/* DOME */
			break;
		case SIG_SETMASK:
			if (sigset_Info != NULL)
			{
				downhill_Sigset_Mask = *sigset_Info;
			}
			break;
		default:
			errno = EINVAL;
			return -1;
	}

	/* And release any signals that were pending */
	for (signal_Index = 1;signal_Index < NSIG;signal_Index++)
	{
		if ((!sigismember(&downhill_Sigset_Mask,signal_Index)) &&
		 (sigismember(&sigset_MaskOld,signal_Index)) &&
		 (downhill_Signal_Info[signal_Index].signal_Count > 0))
		{
			if (signal_Index == SIGFPE)
			{
					downhill_Signal_Handle(signal_Index,
					 downhill_Signal_Info[signal_Index].
					 signal_Extra);
			}
			else
			{
					downhill_Signal_Handle(signal_Index);
			}
		}
	}

	return 0;
}

/* Suspend the process until a signal ====================================== */
int sigsuspend(sigset_t* sigset_Info)
{
	sigset_t sigset_MaskOriginal = downhill_Sigset_Mask;

	/* Set the new mask */
	sigprocmask(SIG_SETMASK,sigset_Info,NULL);

	/* Wait for the signal */
	pause();

	/* Reset the old mask */
	sigprocmask(SIG_SETMASK,&sigset_MaskOriginal,NULL);

	return -1;
}

/* Raise a signal ========================================================== */
int Downhill_Signal_Raise(int signal_Number)
{
	if (!downhill_Signal_Init())
	{
		return -1;
	}

	/* Raise the signal */
	switch (signal_Number)
	{
		DOWNHILL_SIGNAL_ALL
			downhill_Signal_Handle(signal_Number);
			break;
		default:
			errno = EINVAL;
			return -1;
	}

	return 0;
}

/* Set signal mask ========================================================= */
#if (_DOWNHILL_POSIX_SOURCE == 0)
unsigned long sigsetmask(unsigned long signal_MaskNew)
{
	unsigned long signal_MaskOld = downhill_Sigset_Mask;

	if (sigprocmask(SIG_SETMASK,(sigset_t*)(&signal_MaskNew),NULL) == -1)
	{
		return (unsigned long)-1;
	}

	return signal_MaskOld;
}
#endif

/* Add signals to mask ===================================================== */
#if (_DOWNHILL_POSIX_SOURCE == 0)
unsigned long sigblock(unsigned long signal_MaskNew)
{
	/* Block a specific group of signals */
	return sigsetmask(downhill_Sigset_Mask|signal_MaskNew);
}
#endif

/* Hold a signal =========================================================== */
#if (_DOWNHILL_POSIX_SOURCE == 0)
int sighold(int signal_Number)
{
	/* Block a specific signal */
	if (sigblock(sigmask(signal_Number)) == -1)
	{
		return -1;
	}

	return 0;
}
#endif

/* Release a signal ======================================================== */
#if (_DOWNHILL_POSIX_SOURCE == 0)
int sigrelse(int signal_Number)
{
	/* Release a specific signal */
	if (sigsetmask(downhill_Sigset_Mask&(~sigmask(signal_Number))) == -1)
	{
		return -1;
	}

	return 0;
}
#endif

/* Check for faked SIGCHLD signals ========================================= */
void Downhill_Signal_CheckSigchld(void)
{
	pid_t process_Id;

	/* See if any children are no longer there */
	process_Id = downhill_Process_GetStatusAny(NULL);
	if (process_Id > 0)
	{
		if (!downhill_Process_IsSignaled(process_Id))
		{
			downhill_Process_Signal(process_Id);
			Downhill_Signal_Raise(SIGCHLD);
		}
	}
}

/* Check for faked SIGUSR1 signals ========================================= */
void Downhill_Signal_CheckSigusr1(void)
{
}

/* Check for faked SIGUSR2 signals ========================================= */
void Downhill_Signal_CheckSigusr2(void)
{
}

/* Check for faked SIGHUP signals ========================================== */
void Downhill_Signal_CheckSighup(void)
{
}

/* Check for faked SIGQUIT signals ========================================= */
void Downhill_Signal_CheckSigquit(void)
{
}

/* Check for faked SIGCONT signals ========================================= */
void Downhill_Signal_CheckSigcont(void)
{
}

/* Check for faked SIGTTOU signals ========================================= */
void Downhill_Signal_CheckSigttou(void)
{
}

/* Check for faked SIGPIPE signals ========================================= */
void Downhill_Signal_CheckSigpipe(void)
{
}

/* Check for faked signals ================================================= */
void Downhill_Signal_Check(void)
{
	/* Check all the dummied signals */
	Downhill_Signal_CheckSigchld();
	Downhill_Signal_CheckSigusr1();
	Downhill_Signal_CheckSigusr2();
	Downhill_Signal_CheckSighup();
	Downhill_Signal_CheckSigquit();
	Downhill_Signal_CheckSigcont();
	Downhill_Signal_CheckSigttou();
	Downhill_Signal_CheckSigpipe();
}
