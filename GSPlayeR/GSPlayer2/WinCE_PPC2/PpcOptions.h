#ifndef __PPCOPTIONS_H__
#define __PPCOPTIONS_H__

#include "options.h"

#define VK_BTN1		0xC1
#define VK_BTN2		0xC2
#define VK_BTN3		0xC3
#define VK_BTN4		0xC4
#define VK_BTN5		0xC5
#define VK_BTN6		0xC6
#define FWIN		0x20

// casio cassiopeia remote control
#define VK_CREMBTN1	0xCA
#define VK_CREMBTN2	0xCB
#define VK_CREMBTN3	0xCC
#define VK_CREMBTN4	0xCD

// toshiba genio-e remote control
#define VK_TREMBTN1		0x8E
#define VK_TREMBTN2		0x8D
#define VK_TREMBTN3		0x8B
#define VK_TREMBTN4		0x8A
#define VK_TREMBTN5		0x8C
#define VK_TREMBTN6		0x89
#define VK_TREMBTN7		0x88

class CPpcMainWnd;
class CPpcOptions : public COptions
{
protected:
	friend CPpcMainWnd;
	BOOL		m_fReleaseKeyMap;
	CMultiBuff	m_listKeyMap;

	int			m_nDispAutoOff;
	BOOL		m_fDispAutoOn;
	BOOL		m_fDispEnableBattery;

public:
	CPpcOptions();
	virtual ~CPpcOptions();

protected:
	void DeleteKeyMap();
	virtual void Save(HANDLE hMap);
	virtual void Load(HANDLE hMap);
	virtual int GetPropPages(PROPSHEETPAGE** ppPage);

	static BOOL CALLBACK KeyMapPageProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void KeyMapPageOnInitDialog(HWND hwndDlg);
	void KeyMapPageOnOK(HWND hwndDlg);
	void KeyMapPageOnHotKey(HWND hwndDlg, int nId, UINT fuModifiers, UINT uVirtKey);
	void KeyMapPageOnShowMenu(HWND hwndDlg, int x, int y);
	void KeyMapPageRegisterHotKeys(HWND hwndDlg);
	void KeyMapPageUnregisterHotKeys(HWND hwndDlg);
	void KeyMapPageDeleteAllMap(HWND hwndDlg);
	int KeyMapPageAddMap(HWND hwndDlg, ACCEL* pAccel);
	void KeyMapPageCreateKeyString(UINT uKey, LPTSTR psz);
	void KeyMapPageCreateCommandString(int nCommand, LPTSTR psz);

	static BOOL CALLBACK DisplayPageProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void DisplayPageOnInitDialog(HWND hwndDlg);
	void DisplayPageOnOK(HWND hwndDlg);
};
#endif // __PPCOPTIONS_H__