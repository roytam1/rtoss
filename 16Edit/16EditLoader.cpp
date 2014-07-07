#define  WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdlib.h>
#include "Common.h"
#include "OFN.h"
#include "HexEditWnd.h"
#include "macros.H"

#pragma comment(linker,"/SUBSYSTEM:WINDOWS /FILEALIGN:512 /MERGE:.rdata=.text /MERGE:.data=.text /SECTION:.text,EWR /IGNORE:4078")

HexEditWnd HEdit;

int main(int argc, char *argv[]) {
	OFN          ofn;
	char         *szCmdl;
	int			 start = 0, len = 0;

	if (argc > 1) {
		szCmdl = argv[1];
		if (argc > 2) {
			start = atoi(argv[2]);
		}

		if (argc > 3) {
			len = atoi(argv[3]);
		}
	} else {
		if (!ofn.GetOpenFilePath())
			return 1;
		szCmdl = ofn.cPathOpen;
	}

	if (!HEdit.DoSpecifySettings(szCmdl, start, len)) {
		MessageBox(0, "File access error !", "16EditLoader", MB_ICONERROR);
		return 1;
	}

	HEditWindowThread();
	return 0;
}

int WINAPI
WinMain(
	HINSTANCE	hInstance,
	HINSTANCE	hPrevInst,
	LPSTR	lpszCmdLine,
	int		nCmdShow)
{
	int		argc;
	char	**argv;

	int		i;
	char	*pch;
	char	*pszNewCmdLine;
	char	prog[256];
	char	*p;
	int		fIsQuote;

	/*
	 * Ron: added full path name so that the $VIM variable will get set to our
	 * startup path (so the .vimrc file can be found w/o a VIM env. var.)
	 * Remove the ".exe" extension, and find the 1st non-space.
	 */
	GetModuleFileName(NULL, prog, 255);
	p = strrchr(prog, '.');
	if (p != NULL)
		*p = '\0';
	for (p = prog; *p != '\0' && *p == ' '; ++p)
		;

	/*
	 * Add the size of the string, two quotes, the separating space, and a
	 * terminating '\0'.
	 */
	pszNewCmdLine = new char[strlen(lpszCmdLine) + strlen(prog) + 4];
	if (pszNewCmdLine == NULL)
		return 0;

	/* put double quotes around the prog name, it could contain spaces */
	pszNewCmdLine[0] = '"';
	strcpy(pszNewCmdLine + 1, p);
	strcat(pszNewCmdLine, "\" ");
	strcat(pszNewCmdLine, lpszCmdLine);

	/*
	 * Isolate each argument and put it in argv[].
	 */
	pch = pszNewCmdLine;
	argc = 0;
	while ( *pch != '\0' )
	{
		/* Ron: Handle quoted strings in args list */
		fIsQuote = (*pch == '\"');
		if (fIsQuote)
			++pch;

		argc++;			    /* this is an argument */
		if (fIsQuote)
		{
			while (*pch != '\0' && *pch != '\"')
			pch++;		    /* advance until a closing quote */
			if (*pch)
			pch++;
		}
		else
		{
			while ((*pch != '\0') && (*pch != ' '))
			pch++;		    /* advance until a space */
		}
		while (*pch && *pch == ' ' )
			pch++;		    /* advance until a non-space */
	}

	argv = (char **)new char[(argc+1) * sizeof(char*)];
	if (argv == NULL )
		return 0;		   /* malloc error */

	i = 0;
	pch = pszNewCmdLine;

	while ((i < argc) && (*pch != '\0'))
	{
		fIsQuote = (*pch == '\"');
		if (fIsQuote)
			++pch;

		argv[i++] = pch;
		if (fIsQuote)
		{
			while (*pch != '\0' && *pch != '\"')
			pch++;		    /* advance until the closing quote */
		}
		else
		{
			while (*pch != '\0' && *pch != ' ')
			pch++;		    /* advance until a space */
		}
		if (*pch != '\0')
			*(pch++) = '\0';	    /* parse argument here */
		while (*pch && *pch == ' ')
			pch++;		    /* advance until a non-space */
	}

	// ASSERT(i == argc);
	argv[argc] = (char *) NULL;    /* NULL-terminated list */
	return main(argc, argv);
}

