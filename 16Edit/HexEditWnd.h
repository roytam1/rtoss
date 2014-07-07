#ifndef __HexEditWnd_h__
#define __HexEditWnd_h__

#include <windows.h>
#include <shellapi.h>
#include "File.h"

#ifndef EXTERN_HE_STRUCTS
#define EXTERN_HE_STRUCTS

#define                 ID_TB               0x2000
#define                 ID_TRAYICON         0x2001

#define                 IDT_FIRST           0x1000
#define                 IDT_RESTORE         IDT_FIRST + 1
#define                 IDT_EXIT            IDT_FIRST + 2
#define                 WM_TRAYMENU         WM_USER   + IDT_FIRST

#define					HEDIT_WND_CLASS     "16Edit Main Window"
#define					HEDIT_CLASS     	"16Edit Client Window"
#define					HEDIT_WND_TITLE     "[ 16Edit ]"
#define                 HEDIT_TRAY_TIP      "[16Edit]"
#define                 szMemoryBuff        "memory buffer"
#define                 HEDIT_WND_WIDTH     715
#define					HEDIT_WND_HEIGHT    458
#define                 SB_HEIGHT           20
#define                 TB_HEIGHT           26

#define                 INI_NAME            "16Edit.ini"
#define                 INI_SECTION         "Options"
#define                 INI_WINPOSKEY       "WinPos"
#define                 INI_GO_OFFSET       "GO_OFFSET"
#define                 INI_SEARCH_STRING   "SS_STR"
#define                 INI_REPLACE_STRING  "RS_STR"
#define                 INI_SS_OFFSET       "SS_OFFSET"
#define                 INI_FILECD       	"FileCD"
#define                 INI_SHELL       	"ShellMenu"

#define                 CF_16Edit           "CF_16EDIT"

#define                 STR_NO_MEM          "Not enough memory available !"

#define                 DEF_FONT_HEIGHT     17
#define                 DEF_FONT_WIDTH       8

#define                 RGB_BLACK           RGB(0, 0, 0)
#define                 RGB_RED             RGB(0xFF, 0, 0)
#define                 RGB_BLUE            RGB(0, 0, 0xFF)
#define                 RGB_GREEN           RGB(0, 0xFf, 0)
#define                 RGB_SBGRAY          RGB(0x40, 0x40, 0x40)

#define                 H2                  "%02lX"
#define                 H8                  "%08lX"
#define                 _H8                 "0x%08lX"

#define                 DEF_MAX_LINES       22 // A LOOK UP VALUE
#define                 PAIRS_X             (10 * uFontWidth)
#define                 DIGIT_PAIR_WIDTH    (3  * uFontWidth)
#define                 CHARS_X             (PAIRS_X + 16 * DIGIT_PAIR_WIDTH + uFontWidth)

#define                 WM_MOUSEWHEEL       0x020A
#define                 WHEEL_MOVE_STEPS    4
#define                 SELECT_TIMER	    1
#define					TOP_OFFSET			4
#define					LEFT_OFFSET			8

#define					HEX_STRING		0
#define					ASC_STRING		1
#define					UNI_STRING		2

typedef struct _HE_WIN_POS {
	int                 ix, iy, icx, icy;
} HE_WIN_POS, *PHE_WIN_POS;

typedef struct _HE_DATA_INFO {
	BYTE        *pDataBuff;
	DWORD       dwSize;     // data indicator
	BOOL        bReadOnly;
} HE_DATA_INFO, *PHE_DATA_INFO;

typedef struct _HE_POS
{
	DWORD       dwOffset;
	BOOL        bHiword;       // (opt.) first digit of the pair ? ...or the 2nd one ?
	BOOL        bTextSection;  // (opt.) Caret in the text part ?
} HE_POS, *PHE_POS;

#endif // EXTERN_HE_STRUCTS

typedef enum _EDIT_OPER {
	op_modify,
	op_delete,
	op_add,
	op_cut,
	op_paste
} EDIT_OPER;

class HE_OPER {

public:
	HE_OPER(EDIT_OPER, DWORD, DWORD, DWORD);
	~HE_OPER();

public:
	EDIT_OPER	type;
	DWORD		dwOffset;
	DWORD		dwNewLen;
	DWORD		dwOldLen;
	BYTE		*newData;
	BYTE		*oldData;
};

typedef struct HE_CLIPBOARD_DATA
{
	DWORD               dwDataSize;
	BYTE                byDataStart;
} *PHE_CLIPBOARD_DATA;

// changes to this structure could affect "HexEditWnd::HexEditWnd()"
typedef struct HE_STATUS {
	HE_POS              posLastLButtonDown;
	BOOL                bLastLBDownPos;     // TRUE if posLastLButtonDown is valid
	BOOL                bMouseSelecting;

	HE_POS              posCaret;
	BOOL                bCaret;
	BOOL                bCaretVisible;
	BOOL                bCaretPosValid;     // TRUE if posCaret was at least set one time

	DWORD               dwCurOffset;

	BOOL                bSel;
	DWORD               dwOffSelStart;
	DWORD               dwOffSelEnd;

	int                 iLastLine;
} *PHE_STATUS;

class HE_SEARCHOPTIONS {
public:
	BOOL                bInited;            // TRUE if the struct was set at least one time

	BYTE*               pData;              // buffer (malloced)
	DWORD               dwcBuff;
	DWORD               dwcStr;

	BYTE*				pReplaceData;
	DWORD				dwcReplaceStr;

	BOOL                bASCIIStr;
	BOOL                bWideCharStr;
	BOOL                bCaseSensitive;

	DWORD               dwStartOff;
	BOOL                bDown;

	int                 iDlgSearchFrom;     // 0 - top, 1 - cur pos, 2 - off
	void*               pDlgStr;            // buffer (malloced)
	void*               pReplaceStr;            // buffer (malloced)
};

typedef HE_SEARCHOPTIONS *PHE_SEARCHOPTIONS;

class EditOperList {

public:
	EditOperList();
	~EditOperList();
	EditOperList* getFirst();
	EditOperList* getLast();
	EditOperList* getNext();
	EditOperList* getPrev();
	EditOperList* getCur();
	BOOL before(EditOperList *);
	HE_OPER* getOper();
	EditOperList* addOper(HE_OPER *);
	void releaseAfter();

private:
	EditOperList *prev;
	EditOperList *next;
	HE_OPER* oper;
};

extern UINT  cf16Edit;

//
// class HexEditWnd
//
class HexEditWnd
{

public:
	HFONT              hFont, hFontU;
	UINT               iyHETop;       // top of HexEdit area
	UINT               iyHEBottom;  
	BOOL               bHEOnTop;
	char               cIniPath[MAX_PATH];
	HWND               hMainWnd, hTB, hStatusBar, hClient ;
	
	HexEditWnd();
	~HexEditWnd();
	void          SetHEWndCaption();
	BOOL          CreateMainWndThread();
	void          SetTBHandle(HWND hTB);
	HINSTANCE     GetInstance();
	BOOL          DoEditFile(char* szFilePath, BOOL bForceReadOnly);
	BOOL          PaintText(HWND hWnd);
	void          HEHandleWM_SETFOCUS(HWND hWnd);
	void          HEHandleWM_KILLFOCUS(HWND hWnd);
	BOOL          HESetFont(HFONT hf);
	void          HEHandleWM_SIZE(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL          HEHandleLButton(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	BOOL          HEHandleWM_KEYDOWN(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	BOOL          MakeCaretVisible();
	BOOL          ValidatePos(PHE_POS ppos);
	BOOL          SetCaretSet(BOOL bSet);
	BOOL		  HEHandleWM_TIMER(HWND hWnd, WPARAM wParam);
	BOOL          HEHandleWM_VSCROLL(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void          HEHandleWM_MOUSEWHEEL(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void          HEHandleWM_SHOWWINDOW(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	BOOL          HEHandleWM_COMMAND(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	BOOL          HEHandleWM_NOTIFY(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void          InitGotoDlg(HWND hDlg);
	BOOL          GDHandleWM_COMMAND(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT       HEHandleWM_CHAR(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	BOOL          HEHandleWM_MOUSEMOVE(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void          InitSelBlockDlg(HWND hDlg);
	BOOL          SBHandleWM_COMMAND(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void          SSInitDlg(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void          ReplaceInitDlg(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	BOOL          SSHandleWM_COMMAND(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	BOOL          ReplaceHandleWM_COMMAND(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void          SSEnableItems(HWND hDlg);
	BOOL          SSHandleSS_OK(HWND hDlg);
	BOOL          ReplaceHandleSS_OK(HWND hDlg);
	BOOL          DoSpecifySettings(char *, int start = 0, int len = 0);
	BOOL          IsResizingAllowed();
	BOOL          SetCaret(PHE_POS pos);
	BOOL          SetCaret(DWORD dwOffset);
	BOOL          SetCaret();
	BOOL          SetTopLine(int iNewLine);
	BOOL          SetTopLine(DWORD dwOffset);
	BOOL          SetTopLine();
	BOOL          SetSelection(DWORD dwOffStart, DWORD dwOffEnd);
	void          HEHandleWM_CLOSE(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	BOOL          SetHEWnd2Top(BOOL bTop);
	BOOL          HandleStartCaretPosSel(HWND hWnd);
	BOOL          HEditToTray();
	BOOL          HEditKillTrayIcon();
	BOOL          HEditReturnFromTray();
	void          HEHandleWM_TRAYMENU(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void          HEHandleWM_MOVE(HWND hWnd, WPARAM wParam, LPARAM lParam);
	void		  OptionDlgInit(HWND);
	BOOL		  OptionDlgCommand(HWND, DWORD);
	void          ConfigureTBCCP();
	void          ConfigureTB();

private:
	UINT			   timerId;
	EditOperList	   *operList;
	EditOperList	   *current;
	EditOperList	   *savepoint;
	BOOL			   bSavePointValid;
	DWORD			   dwOldSize;
	HINSTANCE          hInst;
	HE_DATA_INFO       diData;        // working buffer
	HE_DATA_INFO       diOrgData;     // buffer with current file content
	CFile              fInput;
	UINT               uFontHeight, uFontWidth, uMaxLines;
	HE_STATUS          stat;
	HE_SEARCHOPTIONS   search;
	RECT               rctHE;         // rect of HexEdit area relative to HE client area
	BOOL               bResizingAllowed, bMinToTray, bSaveWinPos, bDispMultiByte, bReadOnly;
	BOOL			   bInsert, bFileOffset;
	char               cSBText[256];
	char               cSBInfo[200];
	NOTIFYICONDATA     nidTray;
	HMENU              hmTray;
	char               cInitialDir[MAX_PATH];
	RECT               rctLastPos;

	int			  IsDBCSFirstByte(DWORD dwOffset);
	int			  GetDataStatus(DWORD dwOffset);
	void		  AddOper(HE_OPER*);
	BOOL		  CanCut();
	BOOL		  CanCopy();
	BOOL		  CanPaste();
	BOOL		  CanUndo();
	BOOL		  CanRedo();
	BOOL		  CanSave();
	DWORD		  GetOffset(DWORD dwFileOffset);
	DWORD		  GetFileOffset(DWORD dwVirtualAddress);
	DWORD		  GetVirtualAddress(DWORD dwFileOffset);
	void		  ApplyOper(HE_OPER *);
	void		  UndoOper(HE_OPER *);
	BOOL          IsOffsetVisible(DWORD dwOffset);
	BOOL          PointToPos(IN POINT *pp, OUT PHE_POS ppos);
	void          SetupVScrollbar();
	UINT          GetTotalLineNum();
	void          RepaintClientArea();
	void          RepaintClientAreaNow();
	void          Beep();
	void          ErrMsg(HWND hWnd, char* szText);
	void          ErrMsg(char* szText);
	void          ErrMsg(HWND hWnd, char* szText, char* szCaption);
	BOOL          IsOutOfRange(DWORD dwOffset);
	BOOL          IsOutOfRange(PHE_POS ppos);
    void          SetCaretPosData(PHE_POS ppos);
    void          SetCaretPosData(DWORD dwOffset);
	BOOL          SaveChanges();
	void          SetStatusInfo(char *szFormat, ...);
	void          SetStatusText(char *szFormat, ...);
	void          SetStatusText();
	BOOL          IsReadOnly();
	BOOL          KillSelection();
	BOOL          MouseMoveSelect(LPPOINT ppos);
	BOOL          Point2Selection(LPPOINT ppClient);
	UINT          GetCurrentLine();
	BOOL          SetCurrentLine(UINT iLine);
	BOOL          ValidateLine(int *piLine);
	BOOL          Search(PHE_SEARCHOPTIONS pso, DWORD *pOffset);
	BOOL          PerformStrReplace(PHE_SEARCHOPTIONS pso);
	BOOL          PerformStrReplaceAll(PHE_SEARCHOPTIONS pso);
	BOOL          PerformStrSearch(PHE_SEARCHOPTIONS pso);
	BOOL          PerformSearchAgain(PHE_SEARCHOPTIONS pso, BOOL bDown);
	BOOL          CopySelectedBlock();
	BOOL          CopySelectedBlockAsText();
	BOOL          CutSelectedBlock();
	BOOL          DeleteSelectedBlock();
	BOOL          PasteBlockFromCB();
	BOOL          UndoChanges();
	BOOL          RedoChanges();
	void          SelectAll();
	BOOL          IsAllSelected();
	void          ShowAbout();
	void          InitEdition();
	void          QuitEdition();
	void          HEditQuit();
	BOOL          IsKeyDown(int iVKey);
	void          SetCaretSelInfoToStatus();
	BOOL					IsClipboardFormatOK();
	PHE_CLIPBOARD_DATA   	GetClipboardData();

};

DWORD __stdcall HEditWindowThread();
DWORD file_type(char *base);
int get_va(char *base, DWORD file_offset);
int get_fo(char *base, DWORD va_offset);

#endif
