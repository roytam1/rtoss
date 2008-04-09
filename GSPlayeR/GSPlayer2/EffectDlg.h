#ifndef __EFFECTDLG_H__
#define __EFFECTDLG_H__

//#define EQUALIZER_DATA_MIN	47
//#define EQUALIZER_DATA_MAX	15
#define EQUALIZER_DATA_MIN	64
#define EQUALIZER_DATA_MAX	0

class CEffectDlg
{
public:
	CEffectDlg();
	virtual ~CEffectDlg();
	virtual void ShowEffectDlg(HWND hwndParent, HANDLE hMap);

protected:
	static BOOL CALLBACK EqualizerProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static BOOL CALLBACK EffectProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static BOOL CALLBACK ReverbProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static BOOL CALLBACK PresetProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

	void EqualizerOnInitDialog(HWND hwndDlg);
	void EffectOnInitDialog(HWND hwndDlg);
	void ReverbOnInitDialog(HWND hwndDlg);
	void PresetOnInitDialog(HWND hwndDlg);
	BOOL EqualizerOnCommand(HWND hwnd, WPARAM wParam, LPARAM lParam);
	BOOL EffectOnCommand(HWND hwnd, WPARAM wParam, LPARAM lParam);
	BOOL ReverbOnCommand(HWND hwnd, WPARAM wParam, LPARAM lParam);
	BOOL PresetOnCommand(HWND hwnd, WPARAM wParam, LPARAM lParam);
	void EqualizerOnScroll(HWND hwndDlg);
	void EffectOnScroll(HWND hwndDlg);
	void ReverbOnScroll(HWND hwndDlg);
	void EqualizerOnActivate(HWND hwndDlg);
	void EffectOnActivate(HWND hwndDlg);
	void ReverbOnActivate(HWND hwndDlg);
	BOOL PresetOnNotify(HWND hwndDlg, NMHDR* phdr);

	void PresetOnSave(HWND hwndDlg);
	void PresetOnLoad(HWND hwndDlg);
	void PresetOnDelete(HWND hwndDlg);
	void SavePresetFile(LPTSTR pszFile, BOOL fEQ = TRUE, BOOL fSurround = TRUE, 
					BOOL f3DChorus = TRUE, BOOL fReverb = TRUE, BOOL fEcho = TRUE, BOOL fBassBoost = TRUE);
	void LoadPresetFile(LPTSTR pszFile);
	static BOOL CALLBACK PresetNameDlgProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
	HANDLE	m_hMap;
	TCHAR	m_szNewName[MAX_PATH];

#define SAVE_PARAM_EQ			0
#define SAVE_PARAM_SURROUND		1
#define SAVE_PARAM_3DCHORUS		2
#define SAVE_PARAM_REVERB		3
#define SAVE_PARAM_ECHO			4
#define SAVE_PARAM_BASSBOOST	5
	BOOL	m_fSaveParam[6];
};

#endif // __EFFECTDLG_H__