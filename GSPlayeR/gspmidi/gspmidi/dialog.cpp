#include "gspmidi.h"
#include "config.h"
#include "commctrl.h"
#ifdef _WIN32_WCE
#include "wce\resource.h"
#include "wce\wcehelper.h"
#else
#include "win32\resource.h"
#endif
#include <commdlg.h>

extern HINSTANCE g_hInst;
extern BOOL g_fUpdateConfig;
extern TCHAR g_szConfigFile[MAX_PATH];
extern int g_nSampleRate;
extern int g_nBitsPerSample;
extern int g_nChannels;
extern int g_nVoices;
extern int g_nAmp;
extern BOOL g_fAdjustPanning;
extern BOOL g_fFreeInst;
extern BOOL g_fAntialiasing;

const int s_nSampleRate[] = {8000, 11025, 16000, 22050, 32000, 44100, 48000, 0};

BOOL ConfigDialogOnInitDialog(HWND hwndDlg)
{
	int i, n;
	TCHAR sz[MAX_PATH];
	SetWindowText(GetDlgItem(hwndDlg, IDC_EDIT_CONFIG), g_szConfigFile);
	
	for (i = 0; s_nSampleRate[i]; i++) {
		wsprintf(sz, _T("%d"), s_nSampleRate[i]);
		n = SendMessage(GetDlgItem(hwndDlg, IDC_CMB_RATE), CB_ADDSTRING, 0, (LPARAM)sz);
		if (s_nSampleRate[i] == g_nSampleRate)
			SendMessage(GetDlgItem(hwndDlg, IDC_CMB_RATE), CB_SETCURSEL, n, 0);
	}
		
	SendMessage(GetDlgItem(hwndDlg, IDC_SPIN_VOICES), UDM_SETRANGE32, 1, MAX_VOICES);
	SendMessage(GetDlgItem(hwndDlg, IDC_SPIN_VOICES), UDM_SETPOS, 0, MAKELONG((short)g_nVoices, 0));
	SendMessage(GetDlgItem(hwndDlg, IDC_SPIN_AMP), UDM_SETRANGE32, 0, MAX_AMPLIFICATION);
	SendMessage(GetDlgItem(hwndDlg, IDC_SPIN_AMP), UDM_SETPOS, 0, MAKELONG((short)g_nAmp, 0));

	if (g_nBitsPerSample == 16)
		SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_16BITS), BM_SETCHECK, 1, 0);
	
	if (g_nChannels == 2)
		SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_STEREO), BM_SETCHECK, 1, 0);

	if (g_fAdjustPanning)
		SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_PAN), BM_SETCHECK, 1, 0);

	if (g_fFreeInst)
		SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_FREEINST), BM_SETCHECK, 1, 0);

	if (g_fAntialiasing)
		SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_ANTIALIAS), BM_SETCHECK, 1, 0);

	return TRUE;
}

BOOL ConfigDialogOnOK(HWND hwndDlg)
{
	int n;
	TCHAR sz[MAX_PATH];

	GetWindowText(GetDlgItem(hwndDlg, IDC_EDIT_CONFIG), g_szConfigFile, MAX_PATH);
	GetWindowText(GetDlgItem(hwndDlg, IDC_CMB_RATE), sz, MAX_PATH);
	g_nSampleRate = _tcstol(sz, NULL, 10);
	GetWindowText(GetDlgItem(hwndDlg, IDC_EDIT_VOICES), sz, MAX_PATH);
	n = _tcstol(sz, NULL, 10);
	if (n >= 1 && n <= MAX_VOICES)
		g_nVoices = n;

	GetWindowText(GetDlgItem(hwndDlg, IDC_EDIT_AMP), sz, MAX_PATH);
	n = _tcstol(sz, NULL, 10);
	if (n <= MAX_AMPLIFICATION)
		g_nAmp = n;

	if (SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_16BITS), BM_GETCHECK, 0, 0))
		g_nBitsPerSample = 16;
	else
		g_nBitsPerSample = 8;

	if (SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_STEREO), BM_GETCHECK, 0, 0))
		g_nChannels = 2;
	else
		g_nChannels = 1;

	if (SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_PAN), BM_GETCHECK, 0, 0))
		g_fAdjustPanning = TRUE;
	else
		g_fAdjustPanning = FALSE;

	if (SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_FREEINST), BM_GETCHECK, 0, 0))
		g_fFreeInst = TRUE;
	else
		g_fFreeInst = FALSE;

	if (SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_ANTIALIAS), BM_GETCHECK, 0, 0))
		g_fAntialiasing = TRUE;
	else
		g_fAntialiasing = FALSE;

	g_fUpdateConfig = TRUE;

	EndDialog(hwndDlg, IDOK);
	return TRUE;
}

BOOL ConfigDialogOnBrowse(HWND hwndDlg)
{
	TCHAR szFile[MAX_PATH];
	TCHAR szFilter[MAX_PATH];
	TCHAR szInitDir[MAX_PATH];
	LPTSTR psz;
	BOOL fRet;

	*szFile = NULL;

	LoadString(g_hInst, IDS_FILTER, szFilter, MAX_PATH);
	psz = szFilter;
	while (psz = _tcschr(psz, _T('|'))) {
		*psz++ = NULL;
	}

	_tcscpy(szInitDir, g_szConfigFile);
	psz = _tcsrchr(szInitDir, _T('\\'));
	if (psz) *psz = NULL;

	OPENFILENAME ofn;
	memset(&(ofn), 0, sizeof(ofn));
	ofn.lStructSize	= sizeof(ofn);
	ofn.hwndOwner   = hwndDlg;
	ofn.lpstrFile   = szFile;
	ofn.nMaxFile    = MAX_PATH;	
	ofn.lpstrFilter = szFilter;
	ofn.Flags       = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	ofn.lpstrInitialDir = szInitDir;

#ifdef _WIN32_WCE
	BOOL (*gsGetOpenFileName)(OPENFILENAME*) = NULL;
	HINSTANCE hInst = LoadLibrary(_T("gsgetfile.dll"));
	if (hInst) {
		(FARPROC&)gsGetOpenFileName = GetProcAddress(hInst, _T("gsGetOpenFileName"));
	}

	if (gsGetOpenFileName)
		fRet = gsGetOpenFileName(&ofn);
	else 
		fRet = GetOpenFileName(&ofn);

	if (hInst) FreeLibrary(hInst);
#else
	fRet = GetOpenFileName(&ofn);
#endif
	if (fRet)
		SetDlgItemText(hwndDlg, IDC_EDIT_CONFIG, szFile);

	return TRUE;
}

BOOL CALLBACK ConfigDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
#ifdef _WIN32_WCE
	static CWinceHelper* pHelper;
#endif
	switch (uMsg) {
	case WM_INITDIALOG:
#ifdef _WIN32_WCE
		pHelper = (CWinceHelper*)lParam;
		if (pHelper->IsPocketPC())
			pHelper->SHInitDialog(hwndDlg);
#endif
		return ConfigDialogOnInitDialog(hwndDlg);
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK:
			return ConfigDialogOnOK(hwndDlg);
		case IDCANCEL:
			EndDialog(hwndDlg, LOWORD(wParam));
			return TRUE;
		case IDC_BROWSE:
			return ConfigDialogOnBrowse(hwndDlg);
		case IDC_ABOUT:
			{
				TCHAR sz[MAX_PATH];
				LoadString(g_hInst, IDS_ABOUT, sz, MAX_PATH);
				MessageBox(hwndDlg, sz, _T("About"), MB_OK | MB_ICONINFORMATION);
			}
			return TRUE;
		}
#ifdef _WIN32_WCE
	case WM_PAINT:
		return pHelper->DefDlgPaintProc(hwndDlg, wParam, lParam);
	case WM_CTLCOLORSTATIC:
		return pHelper->DefDlgCtlColorStaticProc(hwndDlg, wParam, lParam);
#endif
	}
	return FALSE;
}

void ShowConfigDialog(HWND hwndParent)
{
#ifdef _WIN32_WCE
	CWinceHelper helper;
	if (helper.IsPocketPC())
		DialogBoxParam(g_hInst, MAKEINTRESOURCE(IDD_CONFIG_PPC), hwndParent, ConfigDialogProc, (LPARAM)&helper);
	else
		DialogBoxParam(g_hInst, MAKEINTRESOURCE(IDD_CONFIG), hwndParent, ConfigDialogProc, (LPARAM)&helper);
#else
	DialogBox(g_hInst, MAKEINTRESOURCE(IDD_CONFIG), hwndParent, ConfigDialogProc);
#endif
}

