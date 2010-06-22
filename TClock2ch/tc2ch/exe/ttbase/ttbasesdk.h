/* ===========================================================================
                           TTB Plugin Template(VC++)

                                 Plugin.h

   =========================================================================== */
#ifndef _PLUG_H
#define _PLUG_H
#include <windows.h>

// 構造体アライメント圧縮
#include <pshpack1.h>

/* プラグインのロードタイプ */
#define	ptAlwaysLoad		0
#define	ptLoadAtUse			1
#define	ptSpecViolation		0xFFFF

/* メニュー表示に関する定数 */
#define dmNone 			0	/* 何も出さない 	*/
#define dmSystemMenu	1	/* システムメニュー	*/
#define dmToolMenu		2	/* ツールメニュー	*/
#define dmHotKeyMenu	4	/* ホットキー		*/
#define dmChecked		8	/* メニューのチェックマーク */
#define dmUnchecked		0	/* メニューのチェックマークをつけない */
#define dmEnabled		0	/* メニューをEnableに */
#define dmDisabled		0	/* メニューをDisableする */
#define DISPMENU_MENU		dmToolMenu|dmSystemMenu
#define	DISPMENU_ENABLED	dmDisabled
#define DISPMENU_CHECKED	dmChecked


/* --------------------------------------------------------*/
/*    構造体定義                                           */
/* --------------------------------------------------------*/
/* コマンド情報構造体  */
typedef struct
{
	char	*Name;		/*	コマンドの名前（英名）				*/
	char	*Caption;	/*	コマンドの説明（日本語）			*/
	int		CommandID;	/*	コマンド番号						*/
	int		Attr;		/*	アトリビュート（未使用）			*/
	int		ResID;		/*	リソース番号（未使用）				*/
	int		DispMenu;	/*	メニューに出すかどうか。			*/
						/*		SysMenu:1 ToolMenu:2 None: 0	*/
	DWORD	TimerInterval;	/* タイマー実行間隔[msec]　0: 使用しない	*/
    DWORD	TimerCounter;	/* システム内部で使用				*/
} PLUGIN_COMMAND_INFO;

/* プラグイン情報構造体  */
typedef struct
{
	WORD	NeedVersion;	/* プラグインI/F要求バージョン		*/
	char	*Name;			/* プラグインの説明（日本語）		*/
	char	*Filename;		/* プラグインのファイル名（相対パス）*/
	WORD	PluginType;		/* プラグインのロードタイプ			*/
	DWORD	VersionMS;		/* バージョン						*/
	DWORD	VersionLS;		/* バージョン						*/
	DWORD	CommandCount;	/* コマンド個数						*/
	PLUGIN_COMMAND_INFO	*Commands;		/* コマンド				*/
	/* 以下システムで、TTBase本体で使用する						*/
	DWORD	LoadTime;      /* ロードにかかった時間（msec）		*/
} PLUGIN_INFO;


#ifdef __cplusplus
extern "C" {
#endif

// --------------------------------------------------------
//    プラグイン側エクスポート関数
// --------------------------------------------------------
#ifdef __BORLANDC__
#define DLLEXPORT __declspec(dllexport)
#endif
#if     _MSC_VER > 1000
#define DLLEXPORT
#endif
// 必須
DLLEXPORT PLUGIN_INFO* WINAPI TTBEvent_InitPluginInfo(char *PluginFilename);
DLLEXPORT void WINAPI TTBEvent_FreePluginInfo(PLUGIN_INFO *PluginInfo);
// 任意
DLLEXPORT BOOL WINAPI TTBEvent_Init(char *PluginFilename, DWORD hPlugin);
DLLEXPORT void WINAPI TTBEvent_Unload(void);
DLLEXPORT BOOL WINAPI TTBEvent_Execute(int CommandID, HWND hWnd);
DLLEXPORT void WINAPI TTBEvent_WindowsHook(UINT Msg, DWORD wParam, DWORD lParam);

// --------------------------------------------------------
//    本体側エクスポート関数
// --------------------------------------------------------
extern PLUGIN_INFO* (WINAPI *TTBPlugin_GetPluginInfo)(DWORD hPlugin);
extern void (WINAPI *TTBPlugin_SetPluginInfo)(DWORD hPlugin, PLUGIN_INFO *PluginInfo);
extern void (WINAPI *TTBPlugin_FreePluginInfo)(PLUGIN_INFO *PluginInfo);
extern void (WINAPI *TTBPlugin_SetMenuProperty)(DWORD hPlugin, int CommandID, DWORD CheckFlag, DWORD Flag);
extern PLUGIN_INFO** (WINAPI *TTBPlugin_GetAllPluginInfo)(void);
extern void (WINAPI *TTBPlugin_FreePluginInfoArray)(PLUGIN_INFO **PluginInfoArray);

// --------------------------------------------------------
//    ユーティリティルーチン
// --------------------------------------------------------
extern PLUGIN_INFO* CopyPluginInfo(PLUGIN_INFO *PluginInfo);
extern void FreePluginInfo(PLUGIN_INFO *PluginInfo);

/* 外部変数 */
extern char *PLUGIN_FILENAME;	// プラグインのファイル名。TTBaseフォルダからの相対パス
extern DWORD PLUGIN_HANDLE;		// TTBaseがプラグインを識別するためのコード

#ifdef __cplusplus
};
#endif

#include <poppack.h>
#endif