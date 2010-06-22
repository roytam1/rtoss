/* ===========================================================================
                           TTB Plugin Template(VC++)

                                 ttbasesdk.c

   =========================================================================== */

// --------------------------------------------------------
//    本体側エクスポート関数
// --------------------------------------------------------
PLUGIN_INFO* (WINAPI *TTBPlugin_GetPluginInfo)(DWORD hPlugin);
void (WINAPI *TTBPlugin_SetPluginInfo)(DWORD hPlugin, PLUGIN_INFO *PluginInfo);
void (WINAPI *TTBPlugin_FreePluginInfo)(PLUGIN_INFO *PluginInfo);
void (WINAPI *TTBPlugin_SetMenuProperty)(DWORD hPlugin, int CommandID, DWORD CheckFlag, DWORD Flag);
PLUGIN_INFO** (WINAPI *TTBPlugin_GetAllPluginInfo)(void);
void (WINAPI *TTBPlugin_FreePluginInfoArray)(PLUGIN_INFO **PluginInfoArray);

/* ローカル関数定義 */
void TTBGetVersion(char *, DWORD *, DWORD *);

/* グローバル変数 */
char *PLUGIN_FILENAME;	// プラグインのファイル名。TTBaseフォルダからの相対パス
DWORD PLUGIN_HANDLE;	// TTBaseがプラグインを識別するためのコード

// ****************************************************************
// *
// *         プラグイン イベント
// *
// --------------------------------------------------------
//    プラグイン情報構造体のセット
// --------------------------------------------------------
PLUGIN_INFO* WINAPI TTBEvent_InitPluginInfo(char *PluginFilename)
{
	unsigned i;
	PLUGIN_COMMAND_INFO *pCI;


	PLUGIN_INFO		*result;	/* 返値	*/

	/* ファイル名（相対パス）をコピー */
	PLUGIN_FILENAME = (char *)malloc(strlen(PluginFilename) + 1);
	if (PLUGIN_FILENAME != NULL)
		strcpy(PLUGIN_FILENAME, PluginFilename);

	/* プラグイン情報構造体の生成 */
	result = (PLUGIN_INFO *)malloc(sizeof(PLUGIN_INFO));
	if (result == NULL) return NULL;

	/* プラグインの名前 */
	result->Name = (char *)malloc(strlen(PLUGIN_NAME) + 1);
	if (result->Name != NULL)
		strcpy(result->Name, PLUGIN_NAME);
	/* プラグインファイル名 */
	result->Filename = (char *)malloc(strlen(PLUGIN_FILENAME) + 1);
	if (result->Filename != NULL)
		strcpy(result->Filename, PLUGIN_FILENAME);
	/* プラグインタイプ */
	result->PluginType = PLUGIN_TYPE;
	/* バージョン情報の取得 */
	TTBGetVersion(PLUGIN_FILENAME, &result->VersionMS, &result->VersionLS);
	/* コマンドの数 */
	result->CommandCount = TTBPImplementGetCommandInfoStart();
	/* コマンド情報構造体配列の作成 */
	result->Commands = (PLUGIN_COMMAND_INFO *)malloc(
				sizeof(PLUGIN_COMMAND_INFO) * result->CommandCount);
	if (result->Commands != NULL) {
		/* コマンド情報構造体の作成 */
		for (i = 0; i < result->CommandCount; i++) {
			PLUGIN_COMMAND_INFO pcitemp;
			pCI = &result->Commands[i];
			TTBPImplementGetCommandInfo(i, &pcitemp);
			*pCI = pcitemp;
			/* コマンド名		*/
			pCI->Name = (char *)malloc(strlen(pcitemp.Name) + 1);
			if (pCI->Name != NULL)
				strcpy(pCI->Name, pcitemp.Name);
			pCI->Caption = (char *)malloc(strlen(pcitemp.Caption) + 1);
			if (pCI->Caption != NULL)
				strcpy(pCI->Caption, pcitemp.Caption);
		}
	}
	TTBPImplementGetCommandInfoEnd();

	return result;
}

// --------------------------------------------------------
//    プラグイン情報構造体の破棄
// --------------------------------------------------------
void WINAPI TTBEvent_FreePluginInfo(PLUGIN_INFO *PluginInfo)
{
	DWORD i;
	PLUGIN_COMMAND_INFO *pCI;

	for (i = 0; i < PluginInfo->CommandCount; i++) {
		pCI = &PluginInfo->Commands[i];
		free(pCI->Name);
		free(pCI->Caption);
	}
	free(PluginInfo->Commands);
	free(PluginInfo->Filename);
	free(PluginInfo->Name);
	free(PluginInfo);
}

// --------------------------------------------------------
//    プラグイン初期化
// --------------------------------------------------------
BOOL WINAPI TTBEvent_Init(char *PluginFilename, DWORD hPlugin)
{
	HMODULE hModule;
	// メッセージ定義
	//RegisterMessages();
	// キャッシュのために、TTBPlugin_InitPluginInfoは呼ばれない場合がある
	// そのため、InitでもPLUGIN_FILENAMEの初期化を行う
	if (PLUGIN_FILENAME != NULL) free(PLUGIN_FILENAME);
	PLUGIN_FILENAME = (char *)malloc(strlen(PluginFilename) + 1);
	if (PLUGIN_FILENAME != NULL)
		strcpy(PLUGIN_FILENAME, PluginFilename);

	// TTBaseがプラグインを識別するためのコード
	PLUGIN_HANDLE = hPlugin;

	// API関数の取得
	hModule = GetModuleHandle(NULL);
	*(FARPROC *)&TTBPlugin_GetPluginInfo = GetProcAddress(hModule, "TTBPlugin_GetPluginInfo");
	*(FARPROC *)&TTBPlugin_SetPluginInfo = GetProcAddress(hModule, "TTBPlugin_SetPluginInfo");
	*(FARPROC *)&TTBPlugin_FreePluginInfo = GetProcAddress(hModule, "TTBPlugin_FreePluginInfo");
	*(FARPROC *)&TTBPlugin_SetMenuProperty = GetProcAddress(hModule, "TTBPlugin_SetMenuProperty");
	*(FARPROC *)&TTBPlugin_GetAllPluginInfo = GetProcAddress(hModule, "TTBPlugin_GetAllPluginInfo");
	*(FARPROC *)&TTBPlugin_FreePluginInfoArray = GetProcAddress(hModule, "TTBPlugin_FreePluginInfo");

	return TTBPImplementInit();
}

// --------------------------------------------------------
//    プラグインアンロード時の処理
// --------------------------------------------------------
void WINAPI TTBEvent_Unload(void)
{
	TTBPImplementUnload();
}

// --------------------------------------------------------
//    コマンド実行
// --------------------------------------------------------
BOOL WINAPI TTBEvent_Execute(int CommandID, HWND hWnd)
{
	return TTBPImplementExecute(CommandID, hWnd);
}

// --------------------------------------------------------
//    フック（ShellHook,MouseHook)
// --------------------------------------------------------
void WINAPI TTBEvent_WindowsHook(UINT Msg, DWORD wParam, DWORD lParam)
{
	TTBPImplementHook(Msg, wParam, lParam);
}

// ****************************************************************
// *
// *         ユーティリティルーチン
// *
// --------------------------------------------------------
//    バージョン情報を返す
// --------------------------------------------------------
void TTBGetVersion(char *Filename, DWORD *VersionMS, DWORD *VersionLS)
{
	DWORD	Size;
	LPDWORD	hVersion;
	LPVOID	pVersionInfo;
	VS_FIXEDFILEINFO	*FixedFileInfo;
	UINT	ItemLen;


	*VersionMS = 0;
	*VersionLS = 0;

	hVersion = NULL;

	Size = GetFileVersionInfoSize(Filename, hVersion);
	if (Size == 0) return;

	pVersionInfo = (LPVOID)malloc(Size);
	if (pVersionInfo == NULL) return;

	GetFileVersionInfo(Filename, 0, Size, pVersionInfo);

	if (VerQueryValue(pVersionInfo, "\\", (void **)&FixedFileInfo, &ItemLen)) {
		*VersionMS = FixedFileInfo->dwFileVersionMS;
		*VersionLS = FixedFileInfo->dwFileVersionLS;
	}

	free(pVersionInfo);
}

// --------------------------------------------------------
//    プラグイン情報構造体のSrcをコピーして返す
// --------------------------------------------------------
PLUGIN_INFO* CopyPluginInfo(PLUGIN_INFO *PluginInfo)
{
	PLUGIN_INFO *Result;
	PLUGIN_COMMAND_INFO *Command, *SrcCommand;
	DWORD i;

	Result = (PLUGIN_INFO *)malloc(sizeof(PLUGIN_INFO));
	*Result = *PluginInfo;
	Result->Name = (char *)malloc(strlen(PluginInfo->Name) + 1);
	strcpy(Result->Name, PluginInfo->Name);
	Result->Filename = (char *)malloc(strlen(PluginInfo->Filename) + 1);
	strcpy(Result->Filename, PluginInfo->Filename);

	Result->Commands = (PLUGIN_COMMAND_INFO *)malloc(
		sizeof(PLUGIN_INFO) * PluginInfo->CommandCount);
	for (i = 0; i < PluginInfo->CommandCount; i++) {
		Command = &Result->Commands[i];
		SrcCommand = &PluginInfo->Commands[i];
		*Command = *SrcCommand;
		Command->Name = (char *)malloc(strlen(SrcCommand->Name) + 1);
		strcpy(Command->Name, SrcCommand->Name);
		Command->Caption = (char *)malloc(strlen(SrcCommand->Caption) + 1);
		strcpy(Command->Caption, SrcCommand->Caption);
	}

	return Result;
}

// --------------------------------------------------------
//  プラグイン側で作成されたプラグイン情報構造体を破棄する
// --------------------------------------------------------
void FreePluginInfo(PLUGIN_INFO *PluginInfo)
{
	TTBEvent_FreePluginInfo(PluginInfo);
}
