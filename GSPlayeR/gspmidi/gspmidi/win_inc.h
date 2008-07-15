/*
 *		win_inc.h
 *		timidity port for win32 'windows' mode
 *		based on timidity (Unix ) from Tuukka Toivonen and
 *		win32 console port from  Davide Moretti's 
 *		provided under GNU General Public License
 *	contents : standard include files , fixes RC_NONE both defined in timidity 
 *  source code and windows sandart include   
 *    Nicolas Witczak mai 1996
 */

#ifndef WIN_INC_H
	#define 	WIN_INC_H

	#include <windows.h>
	#include <windowsx.h>
	#include <commctrl.h>
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <memory.h>
	#include <malloc.h>
	#include <stdarg.h>

	#ifdef RC_NONE
		#undef RC_NONE
	#endif
#endif //WIN_INC_H