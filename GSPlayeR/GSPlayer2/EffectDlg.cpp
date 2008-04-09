#include "GSPlayer2.h"
#include "EffectDlg.h"
#include "SmallSlider.h" 

CEffectDlg::CEffectDlg()
{
	m_hMap = NULL;
	InitializeSmallSliderControl();
}

CEffectDlg::~CEffectDlg()
{
	UninitializeSmallSliderControl();
}

void CEffectDlg::ShowEffectDlg(HWND hwndParent, HANDLE hMap)
{
	m_hMap = hMap;
	CTempStr strTitle(IDS_TITLE_EFFECT);

	PROPSHEETPAGE psp[4];
	memset(psp, 0, sizeof(psp));
	psp[0].dwSize = sizeof(PROPSHEETPAGE);
	psp[0].dwFlags = PSP_DEFAULT;
	psp[0].pszTemplate = MAKEINTRESOURCE(IDD_PAGE_EQUALIZER);
	psp[0].pfnDlgProc = (DLGPROC)EqualizerProc;
	psp[0].hInstance = GetInst();
	psp[0].pszIcon = NULL;
	psp[0].lParam = (LPARAM)this;
	psp[1].dwSize = sizeof(PROPSHEETPAGE);
	psp[1].dwFlags = PSP_DEFAULT;
	psp[1].pszTemplate = MAKEINTRESOURCE(IDD_PAGE_EFFECT);
	psp[1].pfnDlgProc = (DLGPROC)EffectProc;
	psp[1].hInstance = GetInst();
	psp[1].pszIcon = NULL;
	psp[1].lParam = (LPARAM)this;
	psp[2].dwSize = sizeof(PROPSHEETPAGE);
	psp[2].dwFlags = PSP_DEFAULT;
	psp[2].pszTemplate = MAKEINTRESOURCE(IDD_PAGE_REVERB);
	psp[2].pfnDlgProc = (DLGPROC)ReverbProc;
	psp[2].hInstance = GetInst();
	psp[2].pszIcon = NULL;
	psp[2].lParam = (LPARAM)this;
	psp[3].dwSize = sizeof(PROPSHEETPAGE);
	psp[3].dwFlags = PSP_DEFAULT;
	psp[3].pszTemplate = MAKEINTRESOURCE(IDD_PAGE_EFFECT_SAVE);
	psp[3].pfnDlgProc = (DLGPROC)PresetProc;
	psp[3].hInstance = GetInst();
	psp[3].pszIcon = NULL;
	psp[3].lParam = (LPARAM)this;
	
	PROPSHEETHEADER psh;
	psh.dwSize = sizeof(PROPSHEETHEADER);
	psh.dwFlags = PSH_PROPSHEETPAGE | PSH_NOAPPLYNOW | PSH_USECALLBACK;
	psh.pfnCallback = PropSheetCallback;
	psh.nPages = sizeof(psp) / sizeof(PROPSHEETPAGE);
	psh.nStartPage = 0;
	psh.pszIcon = NULL;
	psh.pszCaption = strTitle;
	psh.hwndParent = hwndParent;
	psh.hInstance = GetInst();
	psh.ppsp = psp;
	PropertySheet(&psh);
}

BOOL CALLBACK CEffectDlg::EqualizerProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static CEffectDlg* pDlg;
	switch (uMsg) {
		case WM_INITDIALOG:
		{
			pDlg = (CEffectDlg*)((LPPROPSHEETPAGE)lParam)->lParam;
			pDlg->EqualizerOnInitDialog(hwndDlg);
			return TRUE;
		}
		case WM_COMMAND:
			return pDlg->EqualizerOnCommand(hwndDlg, wParam, lParam);
		case WM_HSCROLL:
		case WM_VSCROLL:
			pDlg->EqualizerOnScroll(hwndDlg);
			return TRUE;
		case WM_NOTIFY:
		{
			NMHDR* phdr = (NMHDR*)lParam;
			if (phdr->code == PSN_SETACTIVE) {
				pDlg->EqualizerOnActivate(hwndDlg);
				return TRUE;
			}
			return FALSE;
		}
		default:
			return FALSE;
	}
}

BOOL CALLBACK CEffectDlg::EffectProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static CEffectDlg* pDlg;
	switch (uMsg) {
		case WM_INITDIALOG:
			pDlg = (CEffectDlg*)((LPPROPSHEETPAGE)lParam)->lParam;
			pDlg->EffectOnInitDialog(hwndDlg);
			return TRUE;
		case WM_COMMAND:
			return pDlg->EffectOnCommand(hwndDlg, wParam, lParam);
		case WM_HSCROLL:
		case WM_VSCROLL:
			pDlg->EffectOnScroll(hwndDlg);
			return TRUE;
		case WM_NOTIFY:
		{
			NMHDR* phdr = (NMHDR*)lParam;
			if (phdr->code == PSN_SETACTIVE) {
				pDlg->EffectOnActivate(hwndDlg);
				return TRUE;
			}
			return FALSE;
		}
		default:
			return FALSE;
	}
}

BOOL CALLBACK CEffectDlg::ReverbProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static CEffectDlg* pDlg;
	switch (uMsg) {
		case WM_INITDIALOG:
			pDlg = (CEffectDlg*)((LPPROPSHEETPAGE)lParam)->lParam;
			pDlg->ReverbOnInitDialog(hwndDlg);
			return TRUE;
		case WM_COMMAND:
			return pDlg->ReverbOnCommand(hwndDlg, wParam, lParam);
		case WM_HSCROLL:
		case WM_VSCROLL:
			pDlg->ReverbOnScroll(hwndDlg);
			return TRUE;
		case WM_NOTIFY:
		{
			NMHDR* phdr = (NMHDR*)lParam;
			if (phdr->code == PSN_SETACTIVE) {
				pDlg->ReverbOnActivate(hwndDlg);
				return TRUE;
			}
			return FALSE;
		}
		default:
			return FALSE;
	}
}

BOOL CALLBACK CEffectDlg::PresetProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static CEffectDlg* pDlg;
	switch (uMsg) {
		case WM_INITDIALOG:
			pDlg = (CEffectDlg*)((LPPROPSHEETPAGE)lParam)->lParam;
			pDlg->PresetOnInitDialog(hwndDlg);
			return TRUE;
		case WM_COMMAND:
			return pDlg->PresetOnCommand(hwndDlg, wParam, lParam);
		case WM_NOTIFY:
			return pDlg->PresetOnNotify(hwndDlg, (NMHDR*)lParam);
		default:
			return FALSE;
	}
}

void CEffectDlg::EqualizerOnInitDialog(HWND hwndDlg)
{
	SendMessage(GetDlgItem(hwndDlg, IDC_SLIDER_EQ1), TBM_SETRANGE, FALSE, MAKELONG(EQUALIZER_DATA_MAX, EQUALIZER_DATA_MIN));
	SendMessage(GetDlgItem(hwndDlg, IDC_SLIDER_EQ2), TBM_SETRANGE, FALSE, MAKELONG(EQUALIZER_DATA_MAX, EQUALIZER_DATA_MIN));
	SendMessage(GetDlgItem(hwndDlg, IDC_SLIDER_EQ3), TBM_SETRANGE, FALSE, MAKELONG(EQUALIZER_DATA_MAX, EQUALIZER_DATA_MIN));
	SendMessage(GetDlgItem(hwndDlg, IDC_SLIDER_EQ4), TBM_SETRANGE, FALSE, MAKELONG(EQUALIZER_DATA_MAX, EQUALIZER_DATA_MIN));
	SendMessage(GetDlgItem(hwndDlg, IDC_SLIDER_EQ5), TBM_SETRANGE, FALSE, MAKELONG(EQUALIZER_DATA_MAX, EQUALIZER_DATA_MIN));
	SendMessage(GetDlgItem(hwndDlg, IDC_SLIDER_EQ6), TBM_SETRANGE, FALSE, MAKELONG(EQUALIZER_DATA_MAX, EQUALIZER_DATA_MIN));
	SendMessage(GetDlgItem(hwndDlg, IDC_SLIDER_EQ7), TBM_SETRANGE, FALSE, MAKELONG(EQUALIZER_DATA_MAX, EQUALIZER_DATA_MIN));
	SendMessage(GetDlgItem(hwndDlg, IDC_SLIDER_EQ8), TBM_SETRANGE, FALSE, MAKELONG(EQUALIZER_DATA_MAX, EQUALIZER_DATA_MIN));
	SendMessage(GetDlgItem(hwndDlg, IDC_SLIDER_EQ9), TBM_SETRANGE, FALSE, MAKELONG(EQUALIZER_DATA_MAX, EQUALIZER_DATA_MIN));
	SendMessage(GetDlgItem(hwndDlg, IDC_SLIDER_EQ10), TBM_SETRANGE, FALSE, MAKELONG(EQUALIZER_DATA_MAX, EQUALIZER_DATA_MIN));
	SendMessage(GetDlgItem(hwndDlg, IDC_SLIDER_PREAMP), TBM_SETRANGE, FALSE, MAKELONG(EQUALIZER_DATA_MAX, EQUALIZER_DATA_MIN));	
	SendMessage(GetDlgItem(hwndDlg, IDC_SLIDER_BASSBOOST), TBM_SETRANGE, FALSE, MAKELONG(0, 20));

	SendMessage(GetDlgItem(hwndDlg, IDC_SLIDER_EQ1), TBM_SETTIC, 0, 31);
	SendMessage(GetDlgItem(hwndDlg, IDC_SLIDER_EQ2), TBM_SETTIC, 0, 31);
	SendMessage(GetDlgItem(hwndDlg, IDC_SLIDER_EQ3), TBM_SETTIC, 0, 31);
	SendMessage(GetDlgItem(hwndDlg, IDC_SLIDER_EQ4), TBM_SETTIC, 0, 31);
	SendMessage(GetDlgItem(hwndDlg, IDC_SLIDER_EQ5), TBM_SETTIC, 0, 31);
	SendMessage(GetDlgItem(hwndDlg, IDC_SLIDER_EQ6), TBM_SETTIC, 0, 31);
	SendMessage(GetDlgItem(hwndDlg, IDC_SLIDER_EQ7), TBM_SETTIC, 0, 31);
	SendMessage(GetDlgItem(hwndDlg, IDC_SLIDER_EQ8), TBM_SETTIC, 0, 31);
	SendMessage(GetDlgItem(hwndDlg, IDC_SLIDER_EQ9), TBM_SETTIC, 0, 31);
	SendMessage(GetDlgItem(hwndDlg, IDC_SLIDER_EQ10), TBM_SETTIC, 0, 31);
	SendMessage(GetDlgItem(hwndDlg, IDC_SLIDER_PREAMP), TBM_SETTIC, 0, 31);
	SendMessage(GetDlgItem(hwndDlg, IDC_SLIDER_SURROUND), TBM_SETTIC, 0, 50);
	SendMessage(GetDlgItem(hwndDlg, IDC_SLIDER_BASSBOOST), TBM_SETTIC, 0, 10);
}

void CEffectDlg::EffectOnInitDialog(HWND hwndDlg)
{
	int i;
	SendMessage(GetDlgItem(hwndDlg, IDC_SLIDER_SURROUND), TBM_SETRANGE, FALSE, MAKELONG(0, 100));
	for (i = 0; i < 100; i += 20)
		SendMessage(GetDlgItem(hwndDlg, IDC_SLIDER_SURROUND), TBM_SETTIC, 0, i);
	
	SendMessage(GetDlgItem(hwndDlg, IDC_SLIDER_3DCHORUS), TBM_SETRANGE, FALSE, MAKELONG(0, 100));
	for (i = 0; i < 100; i += 20)
		SendMessage(GetDlgItem(hwndDlg, IDC_SLIDER_3DCHORUS), TBM_SETTIC, 0, i);
}

void CEffectDlg::ReverbOnInitDialog(HWND hwndDlg)
{
	int i;
	SendMessage(GetDlgItem(hwndDlg, IDC_SLIDER_REVERB), TBM_SETRANGE, FALSE, MAKELONG(0, 6));
	for (i = 0; i < 7; i += 1)
		SendMessage(GetDlgItem(hwndDlg, IDC_SLIDER_REVERB), TBM_SETTIC, 0, i);

	SendMessage(GetDlgItem(hwndDlg, IDC_SLIDER_ECHO), TBM_SETRANGE, FALSE, MAKELONG(0, 6));
	for (i = 0; i < 7; i += 1)
		SendMessage(GetDlgItem(hwndDlg, IDC_SLIDER_ECHO), TBM_SETTIC, 0, i);
}

BOOL CEffectDlg::EqualizerOnCommand(HWND hwndDlg, WPARAM wParam, LPARAM lParam)
{
	EQUALIZER eq;
	switch (LOWORD(wParam)) {
		case IDOK:
		case IDCANCEL:
			EndDialog(hwndDlg, LOWORD(wParam));
			return TRUE;
		case IDC_CHECK_EQUALIZER:
			if (HIWORD(wParam) == BN_CLICKED) {
				MAP_GetEqualizer(m_hMap, &eq);
				eq.fEnable = SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_EQUALIZER), BM_GETCHECK, 0, 0)?TRUE:FALSE;
				MAP_SetEqualizer(m_hMap, &eq);
			}
			return TRUE;
		case IDC_BUTTON_FLAT:
			SendMessage(GetDlgItem(hwndDlg, IDC_SLIDER_PREAMP), TBM_SETPOS, TRUE, 31);
			SendMessage(GetDlgItem(hwndDlg, IDC_SLIDER_EQ1), TBM_SETPOS, TRUE, 31);
			SendMessage(GetDlgItem(hwndDlg, IDC_SLIDER_EQ2), TBM_SETPOS, TRUE, 31);
			SendMessage(GetDlgItem(hwndDlg, IDC_SLIDER_EQ3), TBM_SETPOS, TRUE, 31);
			SendMessage(GetDlgItem(hwndDlg, IDC_SLIDER_EQ4), TBM_SETPOS, TRUE, 31);
			SendMessage(GetDlgItem(hwndDlg, IDC_SLIDER_EQ5), TBM_SETPOS, TRUE, 31);
			SendMessage(GetDlgItem(hwndDlg, IDC_SLIDER_EQ6), TBM_SETPOS, TRUE, 31);
			SendMessage(GetDlgItem(hwndDlg, IDC_SLIDER_EQ7), TBM_SETPOS, TRUE, 31);
			SendMessage(GetDlgItem(hwndDlg, IDC_SLIDER_EQ8), TBM_SETPOS, TRUE, 31);
			SendMessage(GetDlgItem(hwndDlg, IDC_SLIDER_EQ9), TBM_SETPOS, TRUE, 31);
			SendMessage(GetDlgItem(hwndDlg, IDC_SLIDER_EQ10), TBM_SETPOS, TRUE, 31);
			PostMessage(hwndDlg, WM_HSCROLL, 0, (LPARAM)GetDlgItem(hwndDlg, IDC_SLIDER_EQ1));
			return TRUE;
	}
	return FALSE;
}

BOOL CEffectDlg::EffectOnCommand(HWND hwndDlg, WPARAM wParam, LPARAM lParam)
{
	switch (LOWORD(wParam))
	{
		case IDOK:
		case IDCANCEL:
			EndDialog(hwndDlg, LOWORD(wParam));
			return TRUE;
		case IDC_CHECK_SURROUND:
			if (HIWORD(wParam) == BN_CLICKED) {
				EFFECT effect;
				MAP_GetEffect(m_hMap, EFFECT_SURROUND, &effect);
				effect.fEnable = SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_SURROUND), BM_GETCHECK, 0, 0)?TRUE:FALSE;
				MAP_SetEffect(m_hMap, EFFECT_SURROUND, &effect);
			}
			return TRUE;
		case IDC_CHECK_3DCHORUS:
			if (HIWORD(wParam) == BN_CLICKED) {
				EFFECT effect;
				MAP_GetEffect(m_hMap, EFFECT_3DCHORUS, &effect);
				effect.fEnable = SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_3DCHORUS), BM_GETCHECK, 0, 0)?TRUE:FALSE;
				MAP_SetEffect(m_hMap, EFFECT_3DCHORUS, &effect);
			}
			return TRUE;
	}
	return FALSE;
}

BOOL CEffectDlg::ReverbOnCommand(HWND hwndDlg, WPARAM wParam, LPARAM lParam)
{
	switch (LOWORD(wParam))
	{
		case IDOK:
		case IDCANCEL:
			EndDialog(hwndDlg, LOWORD(wParam));
			return TRUE;
		case IDC_CHECK_REVERB:
			if (HIWORD(wParam) == BN_CLICKED) {
				EFFECT effect;
				MAP_GetEffect(m_hMap, EFFECT_REVERB, &effect);
				effect.fEnable = SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_REVERB), BM_GETCHECK, 0, 0)?TRUE:FALSE;
				MAP_SetEffect(m_hMap, EFFECT_REVERB, &effect);
			}
			return TRUE;
		case IDC_CHECK_ECHO:
			if (HIWORD(wParam) == BN_CLICKED) {
				EFFECT effect;
				MAP_GetEffect(m_hMap, EFFECT_ECHO, &effect);
				effect.fEnable = SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_ECHO), BM_GETCHECK, 0, 0)?TRUE:FALSE;
				MAP_SetEffect(m_hMap, EFFECT_ECHO, &effect);
			}
			return TRUE;
	}
	return FALSE;
}

BOOL CEffectDlg::PresetOnCommand(HWND hwndDlg, WPARAM wParam, LPARAM lParam)
{
	switch (LOWORD(wParam))
	{
		case IDOK:
		case IDCANCEL:
			EndDialog(hwndDlg, LOWORD(wParam));
			return TRUE;
		case IDC_SAVE:
			PresetOnSave(hwndDlg);
			return TRUE;
		case IDC_LOAD:
			PresetOnLoad(hwndDlg);
			return TRUE;
		case IDC_DEL:
			PresetOnDelete(hwndDlg);
			return TRUE;
	}
	return FALSE;
}

void CEffectDlg::EqualizerOnScroll(HWND hwndDlg)
{
	EQUALIZER eq;
	MAP_GetEqualizer(m_hMap, &eq);
	eq.preamp = SendMessage(GetDlgItem(hwndDlg, IDC_SLIDER_PREAMP), TBM_GETPOS, 0, 0);
	eq.data[0] = SendMessage(GetDlgItem(hwndDlg, IDC_SLIDER_EQ1), TBM_GETPOS, 0, 0);
	eq.data[1] = SendMessage(GetDlgItem(hwndDlg, IDC_SLIDER_EQ2), TBM_GETPOS, 0, 0);
	eq.data[2] = SendMessage(GetDlgItem(hwndDlg, IDC_SLIDER_EQ3), TBM_GETPOS, 0, 0);
	eq.data[3] = SendMessage(GetDlgItem(hwndDlg, IDC_SLIDER_EQ4), TBM_GETPOS, 0, 0);
	eq.data[4] = SendMessage(GetDlgItem(hwndDlg, IDC_SLIDER_EQ5), TBM_GETPOS, 0, 0);
	eq.data[5] = SendMessage(GetDlgItem(hwndDlg, IDC_SLIDER_EQ6), TBM_GETPOS, 0, 0);
	eq.data[6] = SendMessage(GetDlgItem(hwndDlg, IDC_SLIDER_EQ7), TBM_GETPOS, 0, 0);
	eq.data[7] = SendMessage(GetDlgItem(hwndDlg, IDC_SLIDER_EQ8), TBM_GETPOS, 0, 0);
	eq.data[8] = SendMessage(GetDlgItem(hwndDlg, IDC_SLIDER_EQ9), TBM_GETPOS, 0, 0);
	eq.data[9] = SendMessage(GetDlgItem(hwndDlg, IDC_SLIDER_EQ10), TBM_GETPOS, 0, 0);
	MAP_SetEqualizer(m_hMap, &eq);

	int nLevel = 20 - SendMessage(GetDlgItem(hwndDlg, IDC_SLIDER_BASSBOOST), TBM_GETPOS, 0, 0);
	MAP_SetBassBoostLevel(m_hMap, nLevel);
}

void CEffectDlg::EffectOnScroll(HWND hwndDlg)
{
	EFFECT effect;
	MAP_GetEffect(m_hMap, EFFECT_SURROUND, &effect);
	effect.nRate = SendMessage(GetDlgItem(hwndDlg, IDC_SLIDER_SURROUND), TBM_GETPOS, 0, 0);
	MAP_SetEffect(m_hMap, EFFECT_SURROUND, &effect);
	MAP_GetEffect(m_hMap, EFFECT_3DCHORUS, &effect);
	effect.nRate = SendMessage(GetDlgItem(hwndDlg, IDC_SLIDER_3DCHORUS), TBM_GETPOS, 0, 0);
	MAP_SetEffect(m_hMap, EFFECT_3DCHORUS, &effect);
}

void CEffectDlg::ReverbOnScroll(HWND hwndDlg)
{
	static const s_nReverbRate[] = {20, 22, 24, 26, 28, 30, 32};

	int n;
	EFFECT effect;

	n = SendMessage(GetDlgItem(hwndDlg, IDC_SLIDER_REVERB), TBM_GETPOS, 0, 0);
	MAP_GetEffect(m_hMap, EFFECT_REVERB, &effect);
	effect.nDelay = n * 25 + 50;
	effect.nRate = s_nReverbRate[n];
	MAP_SetEffect(m_hMap, EFFECT_REVERB, &effect);

	n = SendMessage(GetDlgItem(hwndDlg, IDC_SLIDER_ECHO), TBM_GETPOS, 0, 0);
	MAP_GetEffect(m_hMap, EFFECT_ECHO, &effect);
	effect.nDelay = n * 25 + 50;
	effect.nRate = s_nReverbRate[n];
	MAP_SetEffect(m_hMap, EFFECT_ECHO, &effect);
}

void CEffectDlg::EqualizerOnActivate(HWND hwndDlg)
{
	EQUALIZER eq;

	MAP_GetEqualizer(m_hMap, &eq);
	SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_EQUALIZER), BM_SETCHECK, eq.fEnable ? BST_CHECKED : BST_UNCHECKED, 0);
	SendMessage(GetDlgItem(hwndDlg, IDC_SLIDER_PREAMP), TBM_SETPOS, TRUE, eq.preamp);
	SendMessage(GetDlgItem(hwndDlg, IDC_SLIDER_EQ1), TBM_SETPOS, TRUE, eq.data[0]);
	SendMessage(GetDlgItem(hwndDlg, IDC_SLIDER_EQ2), TBM_SETPOS, TRUE, eq.data[1]);
	SendMessage(GetDlgItem(hwndDlg, IDC_SLIDER_EQ3), TBM_SETPOS, TRUE, eq.data[2]);
	SendMessage(GetDlgItem(hwndDlg, IDC_SLIDER_EQ4), TBM_SETPOS, TRUE, eq.data[3]);
	SendMessage(GetDlgItem(hwndDlg, IDC_SLIDER_EQ5), TBM_SETPOS, TRUE, eq.data[4]);
	SendMessage(GetDlgItem(hwndDlg, IDC_SLIDER_EQ6), TBM_SETPOS, TRUE, eq.data[5]);
	SendMessage(GetDlgItem(hwndDlg, IDC_SLIDER_EQ7), TBM_SETPOS, TRUE, eq.data[6]);
	SendMessage(GetDlgItem(hwndDlg, IDC_SLIDER_EQ8), TBM_SETPOS, TRUE, eq.data[7]);
	SendMessage(GetDlgItem(hwndDlg, IDC_SLIDER_EQ9), TBM_SETPOS, TRUE, eq.data[8]);
	SendMessage(GetDlgItem(hwndDlg, IDC_SLIDER_EQ10), TBM_SETPOS, TRUE, eq.data[9]);

	int nLevel = MAP_GetBassBoostLevel(m_hMap);
	SendMessage(GetDlgItem(hwndDlg, IDC_SLIDER_BASSBOOST), TBM_SETPOS, TRUE, 20 - nLevel);
}

void CEffectDlg::EffectOnActivate(HWND hwndDlg)
{
	EFFECT effect;
	
	MAP_GetEffect(m_hMap, EFFECT_SURROUND, &effect);
	SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_SURROUND), BM_SETCHECK, effect.fEnable ? BST_CHECKED : BST_UNCHECKED, 0);
	SendMessage(GetDlgItem(hwndDlg, IDC_SLIDER_SURROUND), TBM_SETPOS, TRUE, effect.nRate);

	MAP_GetEffect(m_hMap, EFFECT_3DCHORUS, &effect);
	SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_3DCHORUS), BM_SETCHECK, effect.fEnable ? BST_CHECKED : BST_UNCHECKED, 0);
	SendMessage(GetDlgItem(hwndDlg, IDC_SLIDER_3DCHORUS), TBM_SETPOS, TRUE, effect.nRate);
}

void CEffectDlg::ReverbOnActivate(HWND hwndDlg)
{
	EFFECT effect;
	MAP_GetEffect(m_hMap, EFFECT_REVERB, &effect);
	SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_REVERB), BM_SETCHECK, effect.fEnable ? BST_CHECKED : BST_UNCHECKED, 0);
	SendMessage(GetDlgItem(hwndDlg, IDC_SLIDER_REVERB), TBM_SETPOS, TRUE, (effect.nDelay - 50) / 25);

	MAP_GetEffect(m_hMap, EFFECT_ECHO, &effect);
	SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_ECHO), BM_SETCHECK, effect.fEnable ? BST_CHECKED : BST_UNCHECKED, 0);
	SendMessage(GetDlgItem(hwndDlg, IDC_SLIDER_ECHO), TBM_SETPOS, TRUE, (effect.nDelay - 50) / 25);
}
