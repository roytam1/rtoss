#ifndef __WCEHELPER_H__
#define __WCEHELPER_H__

// ------------------------------------------------ //
#if _WIN32_WCE >= 300 // PocketPC ??
#include <aygshell.h>
#else
#define SHIDIM_FLAGS				0x0001
#define SHIDIF_DONEBUTTON			0x0001
#define SHIDIF_SIZEDLG				0x0002
#define SHIDIF_SIZEDLGFULLSCREEN	0x0004
#define SHIDIF_SIPDOWN				0x0008
#define SHIDIF_FULLSCREENNOMENUBAR	0x0010
typedef struct tagSHINITDIALOG{ 
	DWORD dwMask; 
	HWND hDlg; 
	DWORD dwFlags; 
} SHINITDLGINFO, *PSHINITDLGINFO;

#define SHCMBF_EMPTYBAR				0x0001
#define SHCMBF_HIDDEN				0x0002
#define SHCMBF_HIDESIPBUTTON		0x0004
typedef struct tagSHMENUBARINFO{
	DWORD cbSize; 
	HWND hwndParent; 
	DWORD dwFlags; 
	UINT nToolBarId; 
	HINSTANCE hInstRes; 
	int nBmpId; 
	int cBmpImages; 
	HWND hwndMB;
} SHMENUBARINFO, *PSHMENUBARINFO;

#define SIPF_OFF	0x00000000
#define SIPF_ON 	0x00000001
#define SIPF_DOCKED	0x00000002
#define SIPF_LOCKED	0x00000004
typedef struct {
	DWORD cbSize;
	DWORD fdwFlags;
	RECT rcVisibleDesktop;
	RECT rcSipRect; 
	DWORD dwImDataSize;
	VOID *pvImData;
} SIPINFO;
#endif
// ------------------------------------------------ //

#define MAX_LOADSTRING 256
extern HINSTANCE g_hInst;

// global helpers
void SetFormatSize(DWORD dwSize, TCHAR szBuff[64], LPTSTR pszFmtKB, LPTSTR pszFmtMB);
void SetFormatDouble(LPTSTR pszDouble, UINT nSize);
void SetFormatDateTime(SYSTEMTIME* pst, LPTSTR pszBuff, UINT nSize);
HFONT CreatePointFont(int nPointSize, LPCTSTR pszFaceName, BOOL fBold);

// class CWinceHelper
class CWinceHelper
{
public:
	CWinceHelper();
	~CWinceHelper();
	BOOL IsPocketPC() {return (m_hAygShell && m_pSHInitDialog);}

	BOOL DefDlgPaintProc(HWND hDlg, WPARAM wParam, LPARAM lParam);
	BOOL DefDlgCtlColorStaticProc(HWND hDlg, WPARAM wParam, LPARAM lParam);
	void SHInitDialog(HWND hwndDlg);
	HWND SHCreateMenuBar(HWND hwndParent, int nMenuID, int nBmpID, int cBmpImages);
	BOOL IsSipPanelVisible();

protected:
	HINSTANCE m_hAygShell;
	BOOL (*m_pSHInitDialog)(PSHINITDLGINFO pshidi);
	BOOL (*m_pSHCreateMenuBar)(PSHMENUBARINFO pmb);

	HINSTANCE m_hCoreDll;
	BOOL (*m_pSipGetInfo)(SIPINFO *pSipInfo);
};

#endif // __WCEHELPER_H__