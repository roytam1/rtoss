#ifndef __CHECKSYS_H__
#define __CHECKSYS_H__

__inline void CheckSystem()
{
#if defined(_WIN32_WCE) && 0
	LANGID lid = GetSystemDefaultLangID();
	if (PRIMARYLANGID(lid) == LANG_JAPANESE)
		exit(0);
	
#endif
}

#endif // __CHECKSYS_H__