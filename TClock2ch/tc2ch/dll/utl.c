/*-------------------------------------------
  utl.c
  misc functions
  KAZUBON 1997-1999
---------------------------------------------*/

#include "tcdll.h"

extern HANDLE hmod;

BOOL g_bIniSetting = FALSE;
char g_inifile[MAX_PATH];

int _strncmp(const char* d, const char* s, size_t n)
{
	unsigned int i;
	for(i = 0; i < n; i++)
	{
		if(*s == 0 && *d == 0) break;
		if(*d != *s) return (*d - *s);
		d++; s++;
	}
	return 0;
}

/*-------------------------------------------
　パス名にファイル名をつける
---------------------------------------------*/
void add_title(char *path, char *title)
{
	char *p;

	p = path;

	if(*title && *(title + 1) == ':') ;
	else if(*title == '\\')
	{
		if(*p && *(p + 1) == ':') p += 2;
	}
	else
	{
		while(*p)
		{
			if((*p == '\\' || *p == '/') && *(p + 1) == 0)
			{
				break;
			}
			p = CharNext(p);
		}
		*p++ = '\\';
	}
	while(*title) *p++ = *title++;
	*p = 0;
}

/*-------------------------------------------
　パス名からファイル名をとりのぞく
---------------------------------------------*/
void del_title(char *path)
{
	char *p, *ep;

	p = ep = path;
	while(*p)
	{
		if(*p == '\\' || *p == '/')
		{
			if(p > path && *(p - 1) == ':') ep = p + 1;
			else ep = p;
		}
		p = CharNext(p);
	}
	*ep = 0;
}

/*------------------------------------------------
	カンマで区切られた文字列を取り出す
--------------------------------------------------*/
void parse(char *dst, char *src, int n)
{
	char *dp;
	int i;

	for(i = 0; i < n; i++)
	{
		while(*src && *src != ',') src++;
		if(*src == ',') src++;
	}
	if(*src == 0)
	{
		*dst = 0; return;
	}

	while(*src == ' ') src++;

	dp = dst;
	while(*src && *src != ',') *dst++ = *src++;
	*dst = 0;

	while(dst != dp)
	{
		dst--;
		if(*dst == ' ') *dst = 0;
		else break;
	}
}

/*-------------------------------------------
  returns a resource string
---------------------------------------------*/
char* MyString(UINT id)
{
	static char buf[80];

	if(LoadString(hmod, id, buf, 80) == 0)
		buf[0] = 0;

	return buf;
}

char mykey[] = "Software\\Kazubon\\TClock";

/*------------------------------------------------
　自分のレジストリから文字列を得る
--------------------------------------------------*/
int GetMyRegStr(char* section, char* entry, char* val, int cbData,
	char* defval)
{
	char key[80];
	HKEY hkey;
	DWORD regtype;
	DWORD size;
	BOOL b;
	int r = 0;

	if(g_bIniSetting) key[0] = 0;
	else strcpy(key, mykey);

	if(section && *section)
	{
		if(!g_bIniSetting) strcat(key, "\\");
		strcat(key, section);
	}
	else
	{
		if(g_bIniSetting) strcpy(key, "Main");
	}

	if(g_bIniSetting)
	{
		r = GetPrivateProfileString(key, entry, defval, val,
			cbData, g_inifile);
	}
	else
	{
		b = FALSE;
		if(RegOpenKey(HKEY_CURRENT_USER, key, &hkey) == 0)
		{
			size = cbData;
			if(RegQueryValueEx(hkey, entry, 0, &regtype,
				(LPBYTE)val, &size) == 0)
			{
				if(size == 0) *val = 0;
				r = size;
				b = TRUE;
			}
			RegCloseKey(hkey);
		}
		if(b == FALSE)
		{
			strcpy(val, defval);
			r = strlen(defval);
		}
	}

	return r;
}

/*------------------------------------------------
　自分のレジストリから文字列を得る
--------------------------------------------------*/
int GetMyRegStrEx(char* section, char* entry, char* val, int cbData,
	char* defval, int confno)
{
	char key[80];
	char sectionEx[80];
	HKEY hkey;
	DWORD regtype;
	DWORD size;
	BOOL b;
	int r = 0;

	if(g_bIniSetting) key[0] = 0;
	else strcpy(key, mykey);

	if (confno > 1)
	{
		if(section && *section)
		{
			if(!g_bIniSetting)
				wsprintf(sectionEx,"\\%s%d",section,confno);
			else
				wsprintf(sectionEx,"%s%d",section,confno);
			strcat(key, sectionEx);
		}
		else
		{
			if(!g_bIniSetting)
				wsprintf(sectionEx,"\\%s%d","Main",confno);
			else
				wsprintf(sectionEx,"%s%d","Main",confno);
			strcat(key, sectionEx);
		}
	}
	else
	{
		if(section && *section)
		{
			if(!g_bIniSetting) strcat(key, "\\");
			strcat(key, section);
		}
		else
		{
			if(g_bIniSetting) strcpy(key, "Main");
		}
	}


	if(g_bIniSetting)
	{
		r = GetPrivateProfileString(key, entry, defval, val,
			cbData, g_inifile);
	}
	else
	{
		b = FALSE;
		if(RegOpenKey(HKEY_CURRENT_USER, key, &hkey) == 0)
		{
			size = cbData;
			if(RegQueryValueEx(hkey, entry, 0, &regtype,
				(LPBYTE)val, &size) == 0)
			{
				if(size == 0) *val = 0;
				r = size;
				b = TRUE;
			}
			RegCloseKey(hkey);
		}
		if(b == FALSE)
		{
			strcpy(val, defval);
			r = strlen(defval);
		}
	}

	return r;
}

/*------------------------------------------------
　自分のレジストリからLONG値を得る
--------------------------------------------------*/
LONG GetMyRegLong(char* section, char* entry, LONG defval)
{
	char key[80];
	HKEY hkey;
	DWORD regtype;
	DWORD size;
	BOOL b;
	LONG r = 0;

	if(g_bIniSetting) key[0] = 0;
	else strcpy(key, mykey);

	if(section && *section)
	{
		if(!g_bIniSetting) strcat(key, "\\");
		strcat(key, section);
	}
	else
	{
		if(g_bIniSetting) strcpy(key, "Main");
	}

	if(g_bIniSetting)
	{
		r = GetPrivateProfileInt(key, entry, defval, g_inifile);
	}
	else
	{
		b = FALSE;
		if(RegOpenKey(HKEY_CURRENT_USER, key, &hkey) == 0)
		{
			size = 4;
			if(RegQueryValueEx(hkey, entry, 0, &regtype,
				(LPBYTE)&r, &size) == 0)
			{
				if(size == 4) b = TRUE;
			}
			RegCloseKey(hkey);
		}
		if(b == FALSE) r = defval;
	}
	return r;
}

/*------------------------------------------------
　自分のレジストリからLONG値を得るの拡張版
--------------------------------------------------*/
LONG GetMyRegLongEx(char* section, char* entry, LONG defval, int confno)
{
	char key[80];
	char sectionEx[80];
	HKEY hkey;
	DWORD regtype;
	DWORD size;
	BOOL b;
	LONG r = 0;

	if(g_bIniSetting) key[0] = 0;
	else strcpy(key, mykey);

	if (confno > 1)
	{
		if(section && *section)
		{
			if(!g_bIniSetting)
				wsprintf(sectionEx,"\\%s%d",section,confno);
			else
				wsprintf(sectionEx,"%s%d",section,confno);
			strcat(key, sectionEx);
		}
		else
		{
			if(!g_bIniSetting)
				wsprintf(sectionEx,"\\%s%d","Main",confno);
			else
				wsprintf(sectionEx,"%s%d","Main",confno);
			strcat(key, sectionEx);
		}
	}
	else
	{
		if(section && *section)
		{
			if(!g_bIniSetting) strcat(key, "\\");
			strcat(key, section);
		}
		else
		{
			if(g_bIniSetting) strcpy(key, "Main");
		}
	}

	if(g_bIniSetting)
	{
		r = GetPrivateProfileInt(key, entry, defval, g_inifile);
	}
	else
	{
		b = FALSE;
		if(RegOpenKey(HKEY_CURRENT_USER, key, &hkey) == 0)
		{
			size = 4;
			if(RegQueryValueEx(hkey, entry, 0, &regtype,
				(LPBYTE)&r, &size) == 0)
			{
				if(size == 4) b = TRUE;
			}
			RegCloseKey(hkey);
		}
		if(b == FALSE) r = defval;
	}
	return r;
}

/*------------------------------------------------
  get DWORD value from registry
--------------------------------------------------*/
LONG GetRegLong(HKEY rootkey, char*subkey, char* entry, LONG defval)
{
	HKEY hkey;
	DWORD regtype;
	DWORD size;
	BOOL b;
	int r = 0;

	b = FALSE;
	if(RegOpenKey(rootkey, subkey, &hkey) == 0)
	{
		size = 4;
		regtype = REG_DWORD;
		if(RegQueryValueEx(hkey, entry, 0, &regtype,
			(LPBYTE)&r, &size) == 0)
		{
			if(size == 4) b = TRUE;
		}
		RegCloseKey(hkey);
	}
	if(b == FALSE) r = defval;
	return r;
}

/*------------------------------------------------
　レジストリから文字列を得る
--------------------------------------------------*/
int GetRegStr(HKEY rootkey, char*subkey, char* entry,
	char* val, int cbData, char* defval)
{
	HKEY hkey;
	DWORD regtype;
	DWORD size;
	BOOL b;
	int r = 0;

	b = FALSE;
	if(RegOpenKey(rootkey, subkey, &hkey) == 0)
	{
		size = cbData;
		if(RegQueryValueEx(hkey, entry, 0, &regtype,
			(LPBYTE)val, &size) == 0)
		{
			if(size == 0) *val = 0;
			b = TRUE;
		}
		RegCloseKey(hkey);
	}
	if(b == FALSE)
	{
		strcpy(val, defval);
	}
	r = strlen(val);
	return r;
}

/*-------------------------------------------
　レジストリに文字列を書き込む
---------------------------------------------*/
BOOL SetMyRegStr(char* section, char* entry, char* val)
{
	HKEY hkey;
	BOOL r;
	char key[80];

	if(g_bIniSetting) key[0] = 0;
	else strcpy(key, mykey);

	if(section && *section)
	{
		if(!g_bIniSetting) strcat(key, "\\");
		strcat(key, section);
	}
	else
	{
		if(g_bIniSetting) strcpy(key, "Main");
	}

	if(g_bIniSetting)
	{
		char *chk_val;
		BOOL b_chkflg = FALSE;
		char saveval[1024];

		r = FALSE;
		chk_val = val;
		while(*chk_val)
		{
			if (*chk_val == '\"' || *chk_val == '\'' || *chk_val == ' '  )
				b_chkflg = TRUE;
			chk_val++;
		}

		if (b_chkflg)
		{
			strcpy(saveval,"\"");
			strcat(saveval,val);
			strcat(saveval,"\"");
		}
		else
			strcpy(saveval,val);

		if(WritePrivateProfileString(key, entry, saveval, g_inifile))
			r = TRUE;
	}
	else
	{
		r = FALSE;
		if(RegCreateKey(HKEY_CURRENT_USER, key, &hkey) == 0)
		{
			if(RegSetValueEx(hkey, entry, 0, REG_SZ,
				(CONST BYTE*)val, strlen(val)) == 0)
			{
				r = TRUE;
			}
			RegCloseKey(hkey);
		}
	}
	return r;
}

/*-------------------------------------------
　レジストリに文字列を書き込む
---------------------------------------------*/
BOOL SetMyRegStrEx(char* section, char* entry, char* val, int confno)
{
	HKEY hkey;
	BOOL r;
	char key[80];
	char sectionEx[80];

	if(g_bIniSetting) key[0] = 0;
	else strcpy(key, mykey);

	if (confno > 1)
	{
		if(section && *section)
		{
			if(!g_bIniSetting)
				wsprintf(sectionEx,"\\%s%d",section,confno);
			else
				wsprintf(sectionEx,"%s%d",section,confno);
			strcat(key, sectionEx);
		}
		else
		{
			if(!g_bIniSetting)
				wsprintf(sectionEx,"\\%s%d","Main",confno);
			else
				wsprintf(sectionEx,"%s%d","Main",confno);
			strcat(key, sectionEx);
		}
	}
	else
	{
		if(section && *section)
		{
			if(!g_bIniSetting) strcat(key, "\\");
			strcat(key, section);
		}
		else
		{
			if(g_bIniSetting) strcpy(key, "Main");
		}
	}

	if(g_bIniSetting)
	{
		char *chk_val;
		BOOL b_chkflg = FALSE;
		char saveval[1024];

		r = FALSE;
		chk_val = val;
		while(*chk_val)
		{
			if (*chk_val == '\"' || *chk_val == '\'' || *chk_val == ' '  )
				b_chkflg = TRUE;
			chk_val++;
		}

		if (b_chkflg)
		{
			strcpy(saveval,"\"");
			strcat(saveval,val);
			strcat(saveval,"\"");
		}
		else
			strcpy(saveval,val);

		if(WritePrivateProfileString(key, entry, saveval, g_inifile))
			r = TRUE;
	}
	else
	{
		r = FALSE;
		if(RegCreateKey(HKEY_CURRENT_USER, key, &hkey) == 0)
		{
			if(RegSetValueEx(hkey, entry, 0, REG_SZ,
				(CONST BYTE*)val, strlen(val)) == 0)
			{
				r = TRUE;
			}
			RegCloseKey(hkey);
		}
	}
	return r;
}

/*-------------------------------------------
　レジストリにDWORD値を書き込む
---------------------------------------------*/
BOOL SetMyRegLong(char* section, char* entry, DWORD val)
{
	HKEY hkey;
	BOOL r;
	char key[80];

	if(g_bIniSetting) key[0] = 0;
	else strcpy(key, mykey);

	if(section && *section)
	{
		if(!g_bIniSetting) strcat(key, "\\");
		strcat(key, section);
	}
	else
	{
		if(g_bIniSetting) strcpy(key, "Main");
	}

	if(g_bIniSetting)
	{
		char s[20];
		wsprintf(s, "%d", val);
		r = FALSE;
		if(WritePrivateProfileString(key, entry, s, g_inifile))
			r = TRUE;
	}
	else
	{
		r = FALSE;
		if(RegCreateKey(HKEY_CURRENT_USER, key, &hkey) == 0)
		{
			if(RegSetValueEx(hkey, entry, 0, REG_DWORD,
				(CONST BYTE*)&val, 4) == 0)
			{
				r = TRUE;
			}
			RegCloseKey(hkey);
		}
	}
	return r;
}

/*-------------------------------------------
　レジストリにDWORD値を書き込む
---------------------------------------------*/
BOOL SetMyRegLongEx(char* section, char* entry, DWORD val, int confno)
{
	HKEY hkey;
	BOOL r;
	char key[80];
	char sectionEx[80];

	if(g_bIniSetting) key[0] = 0;
	else strcpy(key, mykey);

	if (confno > 1)
	{
		if(section && *section)
		{
			if(!g_bIniSetting)
				wsprintf(sectionEx,"\\%s%d",section,confno);
			else
				wsprintf(sectionEx,"%s%d",section,confno);
			strcat(key, sectionEx);
		}
		else
		{
			if(!g_bIniSetting)
				wsprintf(sectionEx,"\\%s%d","Main",confno);
			else
				wsprintf(sectionEx,"%s%d","Main",confno);
			strcat(key, sectionEx);
		}
	}
	else
	{
		if(section && *section)
		{
			if(!g_bIniSetting) strcat(key, "\\");
			strcat(key, section);
		}
		else
		{
			if(g_bIniSetting) strcpy(key, "Main");
		}
	}

	if(g_bIniSetting)
	{
		char s[20];
		wsprintf(s, "%d", val);
		r = FALSE;
		if(WritePrivateProfileString(key, entry, s, g_inifile))
			r = TRUE;
	}
	else
	{
		r = FALSE;
		if(RegCreateKey(HKEY_CURRENT_USER, key, &hkey) == 0)
		{
			if(RegSetValueEx(hkey, entry, 0, REG_DWORD,
				(CONST BYTE*)&val, 4) == 0)
			{
				r = TRUE;
			}
			RegCloseKey(hkey);
		}
	}
	return r;
}

/*------------------------------------------------
　デバッグ用
--------------------------------------------------*/
void WriteDebug(LPSTR s)
{
	HFILE hf;
	char fname[MAX_PATH], title[] = "DEBUG.TXT";

	GetModuleFileName(hmod, fname, MAX_PATH);
	del_title(fname);
	add_title(fname, title);
	hf = _lopen(fname, OF_WRITE);
	if(hf == HFILE_ERROR) hf = _lcreat(fname, 0);
	if(hf == HFILE_ERROR) return;
	_llseek(hf, 0, 2);
	_lwrite(hf, s, lstrlen(s));
	_lwrite(hf, "\x0d\x0a", 2);
	_lclose(hf);
}

// 与えられたファイル名が相対パスならば
// TClockのフォルダからの絶対パスに変換
PSTR CreateFullPathName(HINSTANCE hmod, PSTR fname)
{
	int len;
	int tlen;
	char szTClockPath[MAX_PATH];
	PSTR pstr;

	if (hmod == NULL) {
		return NULL;
	}
	if (fname == NULL) {
		return NULL;
	}
	if (*fname == '\0') {
		return NULL;
	}

	// \\NAME\C\path\path\filename.txt
	// C:\path\path\filename.txt
	// 以上の絶対パス以外を相対パスと判断して
	// その前にTClockのパスを基準ディレクトリとして付加
	len = strlen(fname);
	if (len >= 2) {
		if ((*fname == '\\') && (*(fname + 1) == '\\')) {
			//UNC name
			return NULL;
		} else if (*(fname + 1) == ':') {
			return NULL;
		}
	}

	// TClockの位置を基準パスとして指定文字列を相対パスとして追加
	if (GetModuleFileName(hmod, szTClockPath, MAX_PATH) == 0) {
		return NULL;
	}
	del_title(szTClockPath);
	tlen = strlen(szTClockPath);

	pstr = malloc(tlen + len + 2);
	if (pstr == NULL) {
		return NULL;
	}
	strcpy(pstr, szTClockPath);
	add_title(pstr, fname);

	return pstr;
}
