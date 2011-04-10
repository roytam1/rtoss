/*****************************************************************************
 *                                                                           *
 * DH_TIME.C                                                                 *
 *                                                                           *
 * Freely redistributable and modifiable.  Use at your own risk.             *
 *                                                                           *
 * Copyright 1994 The Downhill Project                                       *
 *                                                                           *
 *****************************************************************************/


/* Include stuff *************************************************************/
#include "INCLUDE/DH.H"
#include <sys/time.h>


/* Function stuff ************************************************************/

/* Get the time of day ===================================================== */
#if (_DOWNHILL_POSIX_SOURCE == 0)
int gettimeofday(struct timeval* time_Info,struct timezone* timezone_Info)
{
	/* Get the time, if they want it */
	if (time_Info != NULL)
	{
		time_Info->tv_sec = time(NULL);
		time_Info->tv_usec = 0;
	}

	/* Get the timezone, if they want it */
	if (timezone_Info != NULL)
	{
		_tzset();
		timezone_Info->tz_minuteswest = _timezone;
		timezone_Info->tz_dsttime = _daylight;
	}

	/* And return */
	return 0;
}
#endif
