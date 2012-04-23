/*
	termutil.h
	System dependent terminal utility header

	Copyright (c) 2006 Ken Kato
*/

#ifndef _TERMUTIL_H_
#define _TERMUTIL_H_

#include "vmint.h"

#ifdef __cplusplus
extern "C" {
#endif	/* __cplusplus */

/* display utilities */
void cursor_up(int rows);
void erase_eol(void);

/* keyboard utilities */
void peekkey_prep(void);
void peekkey_end(void);
int peekkey(void);

#ifdef __cplusplus
}
#endif	/* __cplusplus */

#endif	/* _TERMUTIL_H_ */
