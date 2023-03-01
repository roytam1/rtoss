/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is Mozilla Universal charset detector code.
 *
 * The Initial Developer of the Original Code is
 * Netscape Communications Corporation.
 * Portions created by the Initial Developer are Copyright (C) 2001
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *          Kohei TAKETA <k-tak@void.in>
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */
#ifndef nsDummyPrmem_h__
#define nsDummyPrmem_h__

#include <stdlib.h>

#if defined(WIN32) && defined(SUPERTINY)
	#include <windows.h>
	inline void* __cdecl operator new(size_t siz)
	{
	#ifdef USE_LOCALALLOC
		return ::LocalAlloc(LMEM_FIXED, siz);
	#else
		return ::HeapAlloc(GetProcessHeap(), 0, siz);
	#endif
	}

	inline void __cdecl operator delete(void* ptr)
	{
	#ifdef USE_LOCALALLOC
		::LocalFree(ptr);
	#else
		::HeapFree(GetProcessHeap(), 0, ptr);
	#endif
	}

	inline void* PR_Malloc(size_t len)
	{
	#ifdef USE_LOCALALLOC
		return ::LocalAlloc(LMEM_FIXED, len);
	#else
		return ::HeapAlloc(GetProcessHeap(), 0, len);
	#endif
	}
	inline void PR_Free(void *ptr)
	{
	#ifdef USE_LOCALALLOC
		::LocalFree(ptr);
	#else
		::HeapFree(GetProcessHeap(), 0, ptr);
	#endif
	}

#else // WIN32+SUPERTINY
	inline void* PR_Malloc(size_t len)
	{
	    return malloc(len);
	}
	inline void PR_Free(void *ptr)
	{
	    free(ptr);
	}
#endif

#define PR_FREEIF(p) do { if (p) PR_Free((void*)p); } while(0)


#endif
