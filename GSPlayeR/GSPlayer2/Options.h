#ifndef __OPTIONS_H__
#define __OPTIONS_H__

#define ID_TIMER_BUFINFO	100
#define KEY_SETTINGS	_T("Software\\GreenSoftware\\GSPlayer\\MP3")

class CMainWnd;
class COptions
{
public:
	enum RepeatMode {
		RepeatNone = 0,
		RepeatAll,
		RepeatOne
	};

protected:
	friend	CMainWnd;

	BOOL	m_fTopMost;
	POINT	m_ptInitWnd;
	TCHAR	m_szLastDir[MAX_PATH];
	TCHAR	m_szSkinFile[MAX_PATH];

	BOOL	m_fDrawPeek;
	BOOL	m_fShuffle;
	int		m_fRepeat;
	BOOL	m_fResume;
	BOOL	m_fPlayOnStart;
	BOOL	m_fTimeReverse;
	BOOL	m_fSaveDefList;
	BOOL	m_fAddExisting;
	BOOL	m_fTrayIcon;
	BOOL	m_fSearchSubFolders;
	BOOL	m_fScrollTitle;
	int		m_nSleepMinutes;
	BOOL	m_fSleepPowerOff;
	// RT Hack
	BOOL	m_fShowTrackNo;
	BOOL	m_fShowTime;
	BOOL	m_fFastLoad;
	int		m_fRandomSlopeRate;
	int		m_fRandomSlopeBase;

	int		m_nResumeTrack;
	int		m_nResumeTime;

	MAP_OPTIONS m_Options;
	MAP_STREAMING_OPTIONS m_StreamingOptions;

	HANDLE	m_hMap;
	LPTSTR	m_pszLocation;
	int		m_nSleepTime;
	int		m_dwSleepLast;


public:
	COptions();
	virtual ~COptions();
	virtual void Save(HANDLE hMap);
	virtual void Load(HANDLE hMap);
	virtual void ShowOptionDlg(HWND hwndParent, HANDLE hMap);
	virtual BOOL ShowSleepTimerDlg(HWND hwndParent);

	virtual void ShowAboutDlg(HWND hwndParent, HANDLE hMap);

protected:
	virtual int GetPropPages(PROPSHEETPAGE** ppPage);

	static void UpdateBufferInfo(HWND hwndDlg, HANDLE hMap);
	static BOOL CALLBACK AboutDlgProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static BOOL CALLBACK SleepTimerDlgProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static BOOL CALLBACK PlayerPageProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static BOOL CALLBACK DecoderPageProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static BOOL CALLBACK StreamingPageProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static BOOL CALLBACK SkinPageProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static BOOL CALLBACK AssociatePageProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static BOOL CALLBACK PlugInPageProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

	virtual void PlayerPageOnInitDialog(HWND hwndDlg);
	virtual void PlayerPageOnOK(HWND hwndDlg);
	virtual void DecoderPageOnInitDialog(HWND hwndDlg);
	virtual void DecoderPageOnOK(HWND hwndDlg);
	virtual void DecoderPageOnDeltaPos(HWND hwndDlg, NM_UPDOWN* pNmud);
	virtual void StreamingPageOnInitDialog(HWND hwndDlg);
	virtual void StreamingPageOnOK(HWND hwndDlg);
	virtual void StreamingPageOnDeltaPos(HWND hwndDlg, NM_UPDOWN* pNmud);
	virtual void SkinPageOnInitDialog(HWND hwndDlg);
	virtual void SkinPageOnBrowse(HWND hwndDlg);
	virtual void SkinPageOnOK(HWND hwndDlg);
	virtual void AssociatePageOnInitDialog(HWND hwndDlg);
	virtual void AssociatePageOnOK(HWND hwndDlg);
	virtual void SleepTimerDlgOnInitDialog(HWND hwndDlg);
	virtual BOOL SleepTimerDlgOnCommand(HWND hwndDlg, WPARAM wParam, LPARAM lParam);
	virtual void SleepTimerDlgOnDeltaPos(HWND hwndDlg, NM_UPDOWN* pNmud);
	virtual void PlugInPageOnInitDialog(HWND hwndDlg);
	virtual void PlugInPageOnOK(HWND hwndDlg);
	virtual BOOL PlugInPageOnCommand(HWND hwndDlg, WPARAM wParam, LPARAM lParam);

	virtual int ShowLocationDlg(HWND hwndParent, LPTSTR pszLocation);
	static BOOL CALLBACK LocationDlgProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual void LocationDlgOnInitDialog(HWND hwndDlg);
	virtual void LocationDlgOnOK(HWND hwndDlg);

#define EXT_LEN		5
	virtual void AssociateFile(LPCTSTR pszExt, LPCTSTR pszKey, LPCTSTR pszName, UINT uIconID = IDI_FILE);
	virtual void UnassociateFile(LPCTSTR pszExt, LPCTSTR pszKey);
	virtual BOOL IsAssociatedFile(LPCTSTR pszExt, LPCTSTR pszKey);
};

#endif // __OPTIONS_H__