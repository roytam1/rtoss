//---[s]--- For InternetTime 99/03/16@211 M.Takemura -----

/*-----------------------------------------------------
    format.c
    to make a string to display in the clock
    KAZUBON 1997-1998
-------------------------------------------------------*/

#include "tcdll.h"

int codepage = CP_ACP;
int actdvl[26] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
static char DayOfWeekShort[11], DayOfWeekLong[31];
static char MonthShort[11], MonthLong[31];
static char *DayOfWeekEng[7] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
static char *MonthEng[12] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun",
	"Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
static char AM[11], PM[11], SDate[5], STime[5];
static char EraStr[11];
static int AltYear;

extern BOOL bHour12, bHourZero;
extern BOOL bWin95, bWin2000;

extern int iFreeRes[3], iCPUUsage, iBatteryLife, iVolume;
extern int iCPUClock[];
//extern char sAvailPhysK[], sAvailPhysM[];
//extern MEMORYSTATUS msMemory;
extern MEMORYSTATUSEX msMemory;
extern double temperatures[];
extern double voltages[];
extern double fans[];
//extern int permon[];
extern int CPUUsage[];
extern double net[];
extern double diskFree[];
extern double diskAll[];
extern int blt_h, blt_m, blt_s, pw_mode;
extern USRSTR usrstr[10];

/*------------------------------------------------
  GetLocaleInfo() for 95/NT
--------------------------------------------------*/
int GetLocaleInfoWA(WORD wLanguageID, LCTYPE LCType, char* dst, int n)
{
	int r;
	LCID Locale;

	*dst = 0;
	Locale = MAKELCID(wLanguageID, SORT_DEFAULT);
	if(GetVersion() & 0x80000000) // 95
		r = GetLocaleInfoA(Locale, LCType, dst, n);
	else  // NT
	{
		WCHAR* pw;
		pw = (WCHAR*)GlobalAllocPtr(GHND, sizeof(WCHAR)*(n+1));
		*pw = 0;
		r = GetLocaleInfoW(Locale, LCType, pw, n);
		if(r)
			WideCharToMultiByte(codepage, 0, pw, -1, dst, n,
				NULL, NULL);
		GlobalFreePtr(pw);
	}
	return r;
}

/*------------------------------------------------
  GetDateFormat() for 95/NT
--------------------------------------------------*/
int GetDateFormatWA(WORD wLanguageID, DWORD dwFlags, CONST SYSTEMTIME *t,
	char* fmt, char* dst, int n)
{
	int r;
	LCID Locale;

	*dst = 0;
	Locale = MAKELCID(wLanguageID, SORT_DEFAULT);
	if(GetVersion() & 0x80000000) // 95
		r = GetDateFormatA(Locale, dwFlags, t, fmt, dst, n);

	else  // NT
	{
		WCHAR* pw1, *pw2;
		pw1 = NULL;
		if(fmt)
		{
			pw1 = (WCHAR*)GlobalAllocPtr(GHND,
				sizeof(WCHAR)*(strlen(fmt)+1));
			MultiByteToWideChar(CP_ACP, 0, fmt, -1,
				pw1, strlen(fmt));
		}
		pw2 = (WCHAR*)GlobalAllocPtr(GHND, sizeof(WCHAR)*(n+1));
		r = GetDateFormatW(Locale, dwFlags, t, pw1, pw2, n);
		if(r)
			WideCharToMultiByte(CP_ACP, 0, pw2, -1, dst, n,
				NULL, NULL);
		if(pw1) GlobalFreePtr(pw1);
		GlobalFreePtr(pw2);
	}
	return r;
}

/*------------------------------------------------
  GetTimeFormat() for 95/NT
--------------------------------------------------*/
int GetTimeFormatWA(WORD wLanguageID, DWORD dwFlags, CONST SYSTEMTIME *t,
	char* fmt, char* dst, int n)
{
	int r;
	LCID Locale;

	*dst = 0;
	Locale = MAKELCID(wLanguageID, SORT_DEFAULT);
	if(GetVersion() & 0x80000000) // 95
		r = GetTimeFormatA(Locale, dwFlags, t, fmt, dst, n);

	else  // NT
	{
		WCHAR* pw1, *pw2;
		pw1 = NULL;
		if(fmt)
		{
			pw1 = (WCHAR*)GlobalAllocPtr(GHND,
				sizeof(WCHAR)*(strlen(fmt)+1));
			MultiByteToWideChar(CP_ACP, 0, fmt, -1,
				pw1, strlen(fmt));
		}
		pw2 = (WCHAR*)GlobalAllocPtr(GHND, sizeof(WCHAR)*(n+1));
		r = GetTimeFormatW(Locale, dwFlags, t, pw1, pw2, n);
		if(r)
			WideCharToMultiByte(CP_ACP, 0, pw2, -1, dst, n,
				NULL, NULL);
		if(pw1) GlobalFreePtr(pw1);
		GlobalFreePtr(pw2);
	}
	return r;
}

/*------------------------------------------------
  load strings of day, month
--------------------------------------------------*/
void InitFormat(SYSTEMTIME* lt)
{
	char s[80], *p;
	int i, ilang, ioptcal;

	ilang = GetMyRegLong(NULL, "Locale", (int)GetUserDefaultLangID());

	codepage = CP_ACP;
	if(GetLocaleInfoWA((WORD)ilang, LOCALE_IDEFAULTANSICODEPAGE,
		s, 10) > 0)
	{
		p = s; codepage = 0;
		while('0' <= *p && *p <= '9')
			codepage = codepage * 10 + *p++ - '0';
		if(!IsValidCodePage(codepage)) codepage = CP_ACP;
	}

	i = lt->wDayOfWeek; i--; if(i < 0) i = 6;
	GetLocaleInfoWA((WORD)ilang, LOCALE_SABBREVDAYNAME1 + i,
		DayOfWeekShort, 10);
	GetLocaleInfoWA((WORD)ilang, LOCALE_SDAYNAME1 + i,
		DayOfWeekLong, 30);
	i = lt->wMonth; i--;
	GetLocaleInfoWA((WORD)ilang, LOCALE_SABBREVMONTHNAME1 + i,
		MonthShort, 10);
	GetLocaleInfoWA((WORD)ilang, LOCALE_SMONTHNAME1 + i,
		MonthLong, 30);

	GetLocaleInfoWA((WORD)ilang, LOCALE_S1159, AM, 10);
	GetMyRegStr(NULL, "AMsymbol", s, 80, AM);
	if(s[0] == 0) strcpy(s, "AM");
	strcpy(AM, s);
	GetLocaleInfoWA((WORD)ilang, LOCALE_S2359, PM, 10);
	GetMyRegStr(NULL, "PMsymbol", s, 80, PM);
	if(s[0] == 0) strcpy(s, "PM");
	strcpy(PM, s);

	GetLocaleInfoWA((WORD)ilang, LOCALE_SDATE, SDate, 4);
	GetLocaleInfoWA((WORD)ilang, LOCALE_STIME, STime, 4);

	EraStr[0] = 0;
	AltYear = -1;

	ioptcal = 0;
	if(GetLocaleInfoWA((WORD)ilang, LOCALE_IOPTIONALCALENDAR,
		s, 10))
	{
		ioptcal = 0;
		p = s;
		while('0' <= *p && *p <= '9')
			ioptcal = ioptcal * 10 + *p++ - '0';
	}
	if(ioptcal < 3) ilang = LANG_USER_DEFAULT;

	if(GetDateFormatWA((WORD)ilang,
		DATE_USE_ALT_CALENDAR, lt, "gg", s, 12) != 0);
		strcpy(EraStr, s);

	if(GetDateFormatWA((WORD)ilang,
		DATE_USE_ALT_CALENDAR, lt, "yyyy", s, 6) != 0)
	{
		if(s[0])
		{
			p = s;
			AltYear = 0;
			while('0' <= *p && *p <= '9')
				AltYear = AltYear * 10 + *p++ - '0';
		}
	}
}

BOOL GetNumFormat(char **sp, char x, char c, int *len, int *slen, BOOL *bComma)
{
	char *p;
	int n, ns;

	p = *sp;
	n = 0;
	ns = 0;

	while (*p == '_')
	{
		ns++;
		p++;
	}
	if (*p != x && *p != c) return FALSE;
	while (*p == x)
	{
		n++;
		p++;
	}
	while (*p == c)
	{
		n++;
		p++;
		*bComma = TRUE;
	}

	*len = n+ns;
	*slen = ns;
	*sp = p;
	return TRUE;
}

void SetNumFormat(char **dp, int n, int len, int slen, BOOL bComma)
{
	char *p;
	int minlen,i,ii;
	int int_max_value = 1000000000; // 10^n‚µ‚½‚Æ‚«‚ÉŒ…‚ ‚Ó‚ê‚ð‹N‚±‚³‚¸‚Éˆ—‚Å‚«‚éÅ‘å’l

	p = *dp;

	for (i=10,minlen=1; i<int_max_value +1; i*=10,minlen++)
		if (n < i) break;
	if (bComma)
	{
		if (minlen%3 == 0)
			minlen += minlen/3 - 1;
		else
			minlen += minlen/3;
	}
	while (minlen < len)
	{
		if (slen > 0) { *p++ = ' '; slen--; }
		else { *p++ = '0'; }
		len--;
	}
	for (i=minlen-1,ii=1; i>=0; i--,ii++)
	{
		*(p+i) = (char)((n%10)+'0');
		if (ii%3 == 0 && i != 0 && bComma)
			*(p+--i) = ',';
		n/=10;
	}
	p += minlen;

	*dp = p;
}


/*------------------------------------------------
   make a string from date and time format
--------------------------------------------------*/
void MakeFormat(char* s, SYSTEMTIME* pt, int beat100, char* fmt)
{
	char *sp, *dp, *p;
	DWORD TickCount = 0;

	sp = fmt; dp = s;
	while(*sp)
	{
		if(*sp == '<' && *(sp + 1) == '%')
		{
			sp += 2;
			while(*sp)
			{
				if(*sp == '%' && *(sp + 1) == '>')
				{
					sp += 2;
					break;
				}
				if(*sp == '\"')
				{
					sp++;
					while(*sp != '\"' && *sp)
					{
						p = CharNext(sp);
						while(sp != p) *dp++ = *sp++;
					}
					if(*sp == '\"') sp++;
				}
				else if(*sp == '/')
				{
					p = SDate; while(*p) *dp++ = *p++;
					sp++;
				}
				else if(*sp == ':')
				{
					p = STime; while(*p) *dp++ = *p++;
					sp++;
				}

				else if(*sp == 'S' && *(sp + 1) == 'S' && *(sp + 2) == 'S')
				{
					SetNumFormat(&dp, (int)pt->wMilliseconds, 3, 0, FALSE);
					sp += 3;
				}

				else if(*sp == 'y' && *(sp + 1) == 'y')
				{
					int len;
					len = 2;
					if (*(sp + 2) == 'y' && *(sp + 3) == 'y') len = 4;

					SetNumFormat(&dp, (len==2)?(int)pt->wYear%100:(int)pt->wYear, len, 0, FALSE);
					sp += len;
				}
				else if(*sp == 'm')
				{
					if(*(sp + 1) == 'm' && *(sp + 2) == 'e')
					{
						*dp++ = MonthEng[pt->wMonth-1][0];
						*dp++ = MonthEng[pt->wMonth-1][1];
						*dp++ = MonthEng[pt->wMonth-1][2];
						sp += 3;
					}
					else if(*(sp + 1) == 'm' && *(sp + 2) == 'm')
					{
						if(*(sp + 3) == 'm')
						{
							p = MonthLong;
							while(*p) *dp++ = *p++;
							sp += 4;
						}
						else
						{
							p = MonthShort;
							while(*p) *dp++ = *p++;
							sp += 3;
						}
					}
					else
					{
						if(*(sp + 1) == 'm')
						{
							*dp++ = (char)((int)pt->wMonth / 10) + '0';
							sp += 2;
						}
						else
						{
							if(pt->wMonth > 9)
								*dp++ = (char)((int)pt->wMonth / 10) + '0';
							sp++;
						}
						*dp++ = (char)((int)pt->wMonth % 10) + '0';
					}
				}
				else if(*sp == 'a' && *(sp + 1) == 'a' && *(sp + 2) == 'a')
				{
					if(*(sp + 3) == 'a')
					{
						p = DayOfWeekLong;
						while(*p) *dp++ = *p++;
						sp += 4;
					}
					else
					{
						p = DayOfWeekShort;
						while(*p) *dp++ = *p++;
						sp += 3;
					}
				}
				else if(*sp == 'd')
				{
					if(*(sp + 1) == 'd' && *(sp + 2) == 'e')
					{
						p = DayOfWeekEng[pt->wDayOfWeek];
						while(*p) *dp++ = *p++;
						sp += 3;
					}
					else if(*(sp + 1) == 'd' && *(sp + 2) == 'd')
					{
						if(*(sp + 3) == 'd')
						{
							p = DayOfWeekLong;
							while(*p) *dp++ = *p++;
							sp += 4;
						}
						else
						{
							p = DayOfWeekShort;
							while(*p) *dp++ = *p++;
							sp += 3;
						}
					}
					else
					{
						if(*(sp + 1) == 'd')
						{
							*dp++ = (char)((int)pt->wDay / 10) + '0';
							sp += 2;
						}
						else
						{
							if(pt->wDay > 9)
								*dp++ = (char)((int)pt->wDay / 10) + '0';
							sp++;
						}
						*dp++ = (char)((int)pt->wDay % 10) + '0';
					}
				}
				else if(*sp == 'h')
				{
					int hour;
					hour = pt->wHour;
					if(bHour12)
					{
						if(hour > 12) hour -= 12;
						else if(hour == 0) hour = 12;
						if(hour == 12 && bHourZero) hour = 0;
					}
					if(*(sp + 1) == 'h')
					{
						*dp++ = (char)(hour / 10) + '0';
						sp += 2;
					}
					else
					{
						if(hour > 9) 
						{
							*dp++ = (char)(hour / 10) + '0';
						}
						sp++;
					}
					*dp++ = (char)(hour % 10) + '0';
				}
				else if (*sp == 'w' )
				{
					char xs_diff[3];
					int xdiff;
					int hour;

					xs_diff[0] = (char)(*(sp+2));
					xs_diff[1] = (char)(*(sp+3));
					xs_diff[2] = (char)'\x0';
					xdiff = atoi( xs_diff );
					if ( *(sp+1) == '-' ) xdiff = -xdiff;
					hour = ( pt->wHour + xdiff )%24;
					if ( hour < 0 ) hour += 24;
					if ( bHour12 ) 
					{
						if(hour > 12) hour -= 12;
						else if(hour == 0) hour = 12;
						if(hour == 12 && bHourZero) hour = 0;
					}
					*dp++ = (char)(hour / 10) + '0';
					*dp++ = (char)(hour % 10) + '0';
					sp += 4;
				}
				else if(*sp == 'n')
				{
					if(*(sp + 1) == 'n')
					{
						*dp++ = (char)((int)pt->wMinute / 10) + '0';
						sp += 2;
					}
					else
					{
						if(pt->wMinute > 9)
							*dp++ = (char)((int)pt->wMinute / 10) + '0';
						sp++;
					}
					*dp++ = (char)((int)pt->wMinute % 10) + '0';
				}
				else if(*sp == 's')
				{
					if(*(sp + 1) == 's')
					{
						*dp++ = (char)((int)pt->wSecond / 10) + '0';
						sp += 2;
					}
					else
					{
						if(pt->wSecond > 9)
							*dp++ = (char)((int)pt->wSecond / 10) + '0';
						sp++;
					}
					*dp++ = (char)((int)pt->wSecond % 10) + '0';
				}
				else if(*sp == 't' && *(sp + 1) == 't')
				{
					if(pt->wHour < 12) p = AM; else p = PM;
					while(*p) *dp++ = *p++;
					sp += 2;
				}
				else if(*sp == 'A' && *(sp + 1) == 'M')
				{
					if(*(sp + 2) == '/' &&
						*(sp + 3) == 'P' && *(sp + 4) == 'M')
					{
						if(pt->wHour < 12) *dp++ = 'A';
						else *dp++ = 'P';
						*dp++ = 'M'; sp += 5;
					}
					else if(*(sp + 2) == 'P' && *(sp + 3) == 'M')
					{
						if(pt->wHour < 12) p = AM; else p = PM;
						while(*p) *dp++ = *p++;
						sp += 4;
					}
					else *dp++ = *sp++;
				}
				else if(*sp == 'a' && *(sp + 1) == 'm' && *(sp + 2) == '/' &&
					*(sp + 3) == 'p' && *(sp + 4) == 'm')
				{
					if(pt->wHour < 12) *dp++ = 'a';
					else *dp++ = 'p';
					*dp++ = 'm'; sp += 5;
				}
				else if(*sp == '\\' && *(sp + 1) == 'n')
				{
					*dp++ = 0x0d; *dp++ = 0x0a;
					sp += 2;
				}
				// internet time
				else if (*sp == '@' && *(sp + 1) == '@' && *(sp + 2) == '@')
				{
					*dp++ = '@';
					*dp++ = (char)( beat100 / 10000 + '0' );
					*dp++ = (char)( (beat100 % 10000) / 1000 + '0' );
					*dp++ = (char)( (beat100 % 1000) / 100 + '0' );
					sp += 3;
					if(*sp == '.' && *(sp + 1) == '@')
					{
						*dp++ = '.';
						*dp++ = (char)(beat100 % 100) / 10 + '0';
						sp += 2;
					}
				}
				// alternate calendar
				else if(*sp == 'Y' && AltYear > -1)
				{
					int n = 1;
					while(*sp == 'Y') { n *= 10; sp++; }
					if(n < AltYear)
					{
						n = 1; while(n < AltYear) n *= 10;
					}
					for (;;)
					{
						*dp++ = (char)( (AltYear % n) / (n/10) + '0' );
						if(n == 10) break;
						n /= 10;
					}
				}
				else if(*sp == 'g')
				{
					char *p2;
					p = EraStr;
					while(*p && *sp == 'g')
					{
						p2 = CharNextExA((WORD)codepage, p, 0);
						while(p != p2) *dp++ = *p++;
						sp++;
					}
					while(*sp == 'g') sp++;
				}
				else if(*sp == 'R') // System Resources
				{
					int i, per, len, slen;
					BOOL bComma = FALSE;
					i = 3;
					if(*(sp + 1) == 'S') i = 0;
					else if(*(sp + 1) == 'G') i = 1;
					else if(*(sp + 1) == 'U') i = 2;
					if(i < 3 && bWin95)
					{
						sp += 2;
						per = iFreeRes[i];
						if(GetNumFormat(&sp, 'x', ',', &len, &slen, &bComma) == TRUE)
						{
							SetNumFormat(&dp, per, len, slen, bComma);
						}
						else
						{
							if(per > 99) *dp++ = (char)((per / 100) + '0');
							if(per > 9)  *dp++ = (char)((per % 100) / 10 + '0');
							*dp++ = (char)((per % 10) + '0');
						}
					}
					else *dp++ = *sp++;
				}

				// CPU Usage
				else if(*sp == 'C') 
				{
					if(iCPUUsage >= 0 && *(sp + 1) == 'U')
					{
						int len, slen;
						BOOL bComma = FALSE;
						int CPU = 0;

						if (isdigit(*(sp + 2)) && *(sp + 2) != '8' && *(sp + 2) != '9')
						{	// from perfmon
							int processorNum = *(sp + 2) - '0';
							CPU = CPUUsage[processorNum];
							sp += 3;
						}
						else
						{	// legacy(Total)
							CPU = iCPUUsage;
							sp += 2;
						}

						if(GetNumFormat(&sp, 'x', ',', &len, &slen, &bComma) == TRUE)
						{
							SetNumFormat(&dp, CPU, len, slen, bComma);
						}
						else
						{
							if(CPU > 99) *dp++ = (char)((CPU % 1000) / 100 + '0');
							*dp++ = (char)((CPU % 100) / 10 + '0');
							*dp++ = (char)((CPU % 10) + '0');
						}
					}

					// CPU Clock
					else if(*(sp + 1) == 'C')
					{
						int len, slen;
						BOOL bComma = FALSE;
						int clock = 0;

						if (isdigit(*(sp + 2)) && *(sp + 2) != '8' && *(sp + 2) != '9')
						{	// from perfmon
							int processorNum = *(sp + 2) - '0';
							clock = iCPUClock[processorNum];
							sp += 3;
						}
						else
						{	// legacy(#0)
							clock = iCPUClock[0];
							sp += 2;
						}

						if(GetNumFormat(&sp, 'x', ',', &len, &slen, &bComma) == TRUE)
						{
							SetNumFormat(&dp, clock, len, slen, bComma);
						}
						else
						{
							SetNumFormat(&dp, clock, 3, 0, FALSE);
						}
					}

					else *dp++ = *sp++;
				}

				// Battery mode
				else if(*sp == 'A' && *(sp + 1) == 'D' ) 
				{
					sp += 2;
					if(bWin95 || bWin2000)
					{
						if(pw_mode == 0)
						{
							*dp++ = 'D'; *dp++ = 'C';
						}
						else if(pw_mode == 1)
						{
							*dp++ = 'A'; *dp++ = 'C';
						}
						else
						{
							*dp++ = 'U'; *dp++ = 'N';
						}
					}
				}
				else if(*sp == 'a' && *(sp + 1) == 'd' ) // Battery mode
				{
					sp += 2;
					if(bWin95 || bWin2000)
					{
						if(pw_mode == 0)
						{
							*dp++ = 'D';
						}
						else if(pw_mode == 1)
						{
							*dp++ = 'A';
						}
						else
						{
							*dp++ = 'U';
						}
					}
				}
				else if(*sp == 'M') // Available Physical Memory
				{
					int len, slen;
					DWORDLONG ms, mst;
					BOOL bComma = FALSE;
					ms = mst = (DWORDLONG)-1;
					if(*(sp + 1) == 'K')
					{
						sp += 2;
						ms = msMemory.ullAvailPhys/1024;
					}
					else if(*(sp + 1) == 'M')
					{
						sp += 2;
						ms = msMemory.ullAvailPhys/(1024*1024);
					}
					else if(*(sp + 1) == 'T')
					{
						if(*(sp + 2) == 'P')      ms = msMemory.ullTotalPhys;
						else if(*(sp + 2) == 'F') ms = msMemory.ullTotalPageFile;
						else if(*(sp + 2) == 'V') ms = msMemory.ullTotalVirtual;
						if(ms != -1)
						{
							if(*(sp + 3) == 'K') { ms/=1024; sp+=4; }
							else if(*(sp + 3) == 'M') { ms/=1024*1024; sp+=4; }
							else ms = (DWORDLONG)-1;
						}
					}
					else if(*(sp + 1) == 'A')
					{
						if(*(sp + 2) == 'P')
						{
							ms  = msMemory.ullAvailPhys;
							mst = msMemory.ullTotalPhys;
						}
						else if(*(sp + 2) == 'F')
						{
							ms  = msMemory.ullAvailPageFile;
							mst = msMemory.ullTotalPageFile;
						}
						else if(*(sp + 2) == 'V')
						{
							ms  = msMemory.ullAvailVirtual;
							mst = msMemory.ullTotalVirtual;
						}
						if(ms != -1)
						{
							if(*(sp + 3) == 'K') { ms/=1024; sp+=4; }
							else if(*(sp + 3) == 'M') { ms/=1024*1024; sp+=4; }
							else if(*(sp + 3) == 'P') { mst/=100; if(!mst) ms=0; else ms=ms/mst; sp+=4; }
							else ms = (DWORDLONG)-1;
						}
					}
					else if(*(sp + 1) == 'U')
					{
						if(*(sp + 2) == 'P')
						{
							ms  = msMemory.ullTotalPhys - msMemory.ullAvailPhys;
							mst = msMemory.ullTotalPhys;
						}
						else if(*(sp + 2) == 'F')
						{
							ms  = msMemory.ullTotalPageFile - msMemory.ullAvailPageFile;
							mst = msMemory.ullTotalPageFile;
						}
						else if(*(sp + 2) == 'V')
						{
							ms  = msMemory.ullTotalVirtual - msMemory.ullAvailVirtual;
							mst = msMemory.ullTotalVirtual;
						}
						if(ms != -1)
						{
							if(*(sp + 3) == 'K') { ms/=1024; sp+=4; }
							else if(*(sp + 3) == 'M') { ms/=1024*1024; sp+=4; }
							else if(*(sp + 3) == 'P') { mst/=100; if(!mst) ms=0; else ms=ms/mst; sp+=4; }
							else ms = (DWORDLONG)-1;
						}
					}

					if(ms != -1)
					{
						if(GetNumFormat(&sp, 'x', ',', &len, &slen, &bComma) == FALSE) { len=1; slen=0; }
						_ASSERTE(ms <= INT_MAX);
						SetNumFormat(&dp, (int)ms, len, slen, bComma);
					}
					else *dp++ = *sp++;
				}
				else if(*sp == 'B' && *(sp + 1) == 'T') // Board Temperature
				{
					int len, slen, i, mbm;
					BOOL bComma = FALSE;
					sp += 2;
					i = 0;
					if(*sp >= '0' && *sp <= '7')
					{
						i = *sp - '0';
						sp++;
					}
					mbm = (int)temperatures[i];
					if (mbm==255)
						mbm = 0;
					if(GetNumFormat(&sp, 'x', ',', &len, &slen, &bComma) == TRUE)
					{
						SetNumFormat(&dp, mbm, len, slen, bComma);
					}
					else
					{
						SetNumFormat(&dp, mbm, 1, 0, FALSE);
					}
				}
				else if(*sp == 'B' && *(sp + 1) == 'V') // Board Voltage
				{
					int len, slen, i, mbm;
					double mbmd;
					BOOL bComma = FALSE;

					sp += 2;
					i = 0;
					if(*sp >= '0' && *sp <= '7')
					{
						i = *sp - '0';
						sp++;
					}
					mbmd = voltages[i];
					if(mbmd < 0)
					{
						*dp++ = (char)'-';
						mbmd = -mbmd;
					}
					mbm = (int)mbmd;
					if(GetNumFormat(&sp, 'x', ',', &len, &slen, &bComma) == TRUE)
					{
						SetNumFormat(&dp, mbm, len, slen, bComma);
					}
					else
					{
						SetNumFormat(&dp, mbm, 1, 0, FALSE);
					}
					mbmd = (mbmd-(int)mbmd);
					if(*sp == '.')
					{
						sp++;
						if(GetNumFormat(&sp, 'x', ',', &len, &slen, &bComma) == TRUE)
						{
							*dp++ = (char)'.';
							if(len > 3) len = 3;
							for(i=0; i<len; i++) mbmd *= 10;
							mbm = (int)mbmd;
							SetNumFormat(&dp, mbm, len, 0, FALSE);
						}
					}
					else
					{
						*dp++ = (char)'.';
						mbmd *= 1000;
						mbm = (int)mbmd;
						SetNumFormat(&dp, mbm, 3, 0, FALSE);
					}
				}
				else if(*sp == 'B' && *(sp + 1) == 'F') // Board Fan
				{
					int len, slen, i, mbm;
					BOOL bComma = FALSE;

					sp += 2;
					i = 0;
					if(*sp >= '0' && *sp <= '7')
					{
						i = *sp - '0';
						sp++;
					}
					mbm = (int)fans[i];
					if (mbm==255)
						mbm = 0;
					if(GetNumFormat(&sp, 'x', ',', &len, &slen, &bComma) == TRUE)
					{
						SetNumFormat(&dp, mbm, len, slen, bComma);
					}
					else
					{
						SetNumFormat(&dp, mbm, 1, 0, FALSE);
					}
				}
				else if(*sp == 'B' && *(sp + 1) == 'L') // Battery Life Percentage
				{
					sp += 2;
					if(bWin95 || bWin2000)
					{
						if(iBatteryLife <= 100)
						{
							int len, slen;
							BOOL bComma = FALSE;
							if(GetNumFormat(&sp, 'x', ',', &len, &slen, &bComma) == TRUE)
							{
								SetNumFormat(&dp, iBatteryLife, len, slen, bComma);
							}
							else
							{
								if(iBatteryLife > 99)
									*dp++ = (char)((iBatteryLife % 1000) / 100 + '0');
								*dp++ = (char)((iBatteryLife % 100) / 10 + '0');
								*dp++ = (char)((iBatteryLife % 10) + '0');
							}
						}
					}
				}
				else if(*sp == 'B' && (*(sp + 1) == 'h' || *(sp + 1) == 'n' || *(sp + 1) == 's' || *(sp + 1) == '_'))  // Battery Life Time
				{
					int len, slen;
					BOOL bComma = FALSE;
					sp++;
					if(GetNumFormat(&sp, 'h', ',', &len, &slen, &bComma) == TRUE)
					{
						SetNumFormat(&dp, blt_h, len, slen, bComma);
					}
					if(GetNumFormat(&sp, 'n', ',', &len, &slen, &bComma) == TRUE)
					{
						SetNumFormat(&dp, blt_m, len, slen, bComma);
					}
					if(GetNumFormat(&sp, 's', ',', &len, &slen, &bComma) == TRUE)
					{
						SetNumFormat(&dp, blt_s, len, slen, bComma);
					}
				}
/*				else if(*sp == 'P' && *(sp + 1) == 'M') // Performance Monitor
				{
					int len, slen, i, pm;
					BOOL bComma = FALSE;

					sp += 2;
					i = 0;
					if(*sp >= '0' && *sp <= '3')
					{
						i = *sp - '0';
						sp++;
					}
					pm = (int)permon[i];
					if(GetNumFormat(&sp, 'x', ',', &len, &slen, &bComma) == TRUE)
					{
						SetNumFormat(&dp, pm, len, slen, bComma);
					}
					else
					{
						SetNumFormat(&dp, pm, 1, 0, FALSE);
					}
				} */
				else if(*sp == 'N') // Network Interface
				{
					int len, slen, i, nt;
					double ntd;
					BOOL bComma = FALSE;

					i = -1;
					if(*(sp + 1) == 'R')
					{
						if(*(sp + 2) == 'A')
						{
							if(*(sp + 3) == 'B') i = 0;
							else if(*(sp + 3) == 'K') i = 4;
							else if(*(sp + 3) == 'M') i = 8;
						}
						else if(*(sp + 2) == 'S')
						{
							if(*(sp + 3) == 'B') i = 2;
							else if(*(sp + 3) == 'K') i = 6;
							else if(*(sp + 3) == 'M') i = 10;
						}
					}
					if(*(sp + 1) == 'S')
					{
						if(*(sp + 2) == 'A')
						{
							if(*(sp + 3) == 'B') i = 1;
							else if(*(sp + 3) == 'K') i = 5;
							else if(*(sp + 3) == 'M') i = 9;
						}
						else if(*(sp + 2) == 'S')
						{
							if(*(sp + 3) == 'B') i = 3;
							else if(*(sp + 3) == 'K') i = 7;
							else if(*(sp + 3) == 'M') i = 11;
						}
					}
					if(i != -1)
					{
						sp += 4;
						ntd = net[i];
						nt = (int)ntd;
						if(GetNumFormat(&sp, 'x', ',', &len, &slen, &bComma) == TRUE)
						{
							SetNumFormat(&dp, nt, len, slen, bComma);
						}
						else
						{
							SetNumFormat(&dp, nt, 1, 0, FALSE);
						}
						ntd = (ntd-(int)ntd);
						if(*sp == '.')
						{
							sp++;
							if(GetNumFormat(&sp, 'x', ',', &len, &slen, &bComma) == TRUE)
							{
								*dp++ = (char)'.';
								if(len > 3) len = 3;
								for(i=0; i<len; i++) ntd *= 10;
								nt = (int)ntd;
								SetNumFormat(&dp, nt, len, 0, FALSE);
							}
						}
					}
					else *dp++ = *sp++;
				}
				else if(*sp == 'L' && _strncmp(sp, "LDATE", 5) == 0)
				{
					char s[80], *p;
					GetDateFormatWA(MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT),
						DATE_LONGDATE, pt, NULL, s, 80);
					p = s;
					while(*p) *dp++ = *p++;
					sp += 5;
				}
				else if(*sp == 'D' && _strncmp(sp, "DATE", 4) == 0)
				{
					char s[80], *p;
					GetDateFormatWA(MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT),
						DATE_SHORTDATE, pt, NULL, s, 80);
					p = s;
					while(*p) *dp++ = *p++;
					sp += 4;
				}
				else if(*sp == 'T' && _strncmp(sp, "TIME", 4) == 0)
				{
					char s[80], *p;
					GetTimeFormatWA(MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT),
						TIME_FORCE24HOURFORMAT, pt, NULL, s, 80);
					p = s;
					while(*p) *dp++ = *p++;
					sp += 4;
				}
				else if(*sp == 'S')
				{
					int len, slen, st;
					BOOL bComma = FALSE;
					sp++;
					if(GetNumFormat(&sp, 'd', ',', &len, &slen, &bComma) == TRUE)
					{
						if (!TickCount) TickCount = GetTickCount();
						st = TickCount/86400000;		//day
						SetNumFormat(&dp, st, len, slen, bComma);
					}
					else if(GetNumFormat(&sp, 'a', ',', &len, &slen, &bComma) == TRUE)
					{
						if (!TickCount) TickCount = GetTickCount();
						st = TickCount/3600000;		//hour
						SetNumFormat(&dp, st, len, slen, bComma);
					}
					else if(GetNumFormat(&sp, 'h', ',', &len, &slen, &bComma) == TRUE)
					{
						if (!TickCount) TickCount = GetTickCount();
						st = (TickCount/3600000)%24;
						SetNumFormat(&dp, st, len, slen, FALSE);
					}
					else if(GetNumFormat(&sp, 'n', ',', &len, &slen, &bComma) == TRUE)
					{
						if (!TickCount) TickCount = GetTickCount();
						st = (TickCount/60000)%60;
						SetNumFormat(&dp, st, len, slen, FALSE);
					}
					else if(GetNumFormat(&sp, 's', ',', &len, &slen, &bComma) == TRUE)
					{
						if (!TickCount) TickCount = GetTickCount();
						st = (TickCount/1000)%60;
						SetNumFormat(&dp, st, len, slen, FALSE);
					}
					else if(*sp == 'T')
					{
						DWORD dw;
						int sth, stm, sts;
						if (!TickCount) TickCount = GetTickCount();
						dw = TickCount;
						dw /= 1000;
						sts = dw%60; dw /= 60;
						stm = dw%60; dw /= 60;
						sth = dw;

						SetNumFormat(&dp, sth, 1, 0, FALSE);
						*dp++ = ':';
						SetNumFormat(&dp, stm, 2, 0, FALSE);
						*dp++ = ':';
						SetNumFormat(&dp, sts, 2, 0, FALSE);

						sp++;
					}
					else
						*dp++ = 'S';
				}
				else if(*sp == 'H')
				{
					int dv, dski, len, slen, i;
					BOOL bComma = FALSE;
					double dsk = 0;
					dv = *(sp + 2) - 'A';
					if(*(sp + 1) == 'T')
					{
						if (*(sp + 3) == 'M')
							dsk = diskAll[dv];
						else if (*(sp + 3) == 'G')
							dsk = diskAll[dv+26];
					}
					if(*(sp + 1) == 'A')
					{
						if (*(sp + 3) == 'M')
							dsk = diskFree[dv];
						else if (*(sp + 3) == 'G')
							dsk = diskFree[dv+26];
						else if (*(sp + 3) == 'P')
							dsk = (diskFree[dv]/diskAll[dv])*100;
					}
					if(*(sp + 1) == 'U')
					{
						if (*(sp + 3) == 'M')
							dsk = diskAll[dv] - diskFree[dv];
						else if (*(sp + 3) == 'G')
							dsk = diskAll[dv+26] - diskFree[dv+26];
						else if (*(sp + 3) == 'P')
							dsk = ((diskAll[dv] - diskFree[dv])/diskAll[dv])*100;
					}
					sp += 4;
					dski = (int)dsk;
					if(GetNumFormat(&sp, 'x', ',', &len, &slen, &bComma) == TRUE)
					{
						SetNumFormat(&dp, dski, len, slen, bComma);
					}
					else
					{
						SetNumFormat(&dp, dski, 1, 0, FALSE);
					}
					dsk = (dsk-(int)dsk);
					if(*sp == '.')
					{
						sp++;
						if(GetNumFormat(&sp, 'x', ',', &len, &slen, &bComma) == TRUE)
						{
							*dp++ = (char)'.';
							if(len > 6) len = 6;
							for(i=0; i<len; i++) dsk *= 10;
							dski = (int)dsk;
							SetNumFormat(&dp, dski, len, 0, FALSE);
						}
					}
				}
				else if(*sp == 'V' && *(sp + 1) == 'L') // Volume
				{
					int len, slen;
					BOOL bComma = FALSE;
					sp += 2;
					if(GetNumFormat(&sp, 'x', ',', &len, &slen, &bComma) == TRUE)
					{
						SetNumFormat(&dp, iVolume, len, slen, FALSE);
					}
					else
					{
						SetNumFormat(&dp, iVolume, 3, 2, FALSE);
					}
				}
				else if(*sp == 'U' && *(sp + 1) == 'S' && *(sp + 2) == 'T' && *(sp + 3) == 'R')
				{
					char *ustr;
					int strNo = 0;

					strNo = *(sp + 4) - '0';
					sp += 5;
					if (usrstr[strNo].u_str != 0)
					{
						ustr = usrstr[strNo].u_str;
						while(*ustr) *dp++ = *ustr++;
					}
				}
				else
				{
					p = CharNext(sp);
					while(sp != p) *dp++ = *sp++;
				}
			}
		}
		else
		{
			p = CharNext(sp);
			while(sp != p) *dp++ = *sp++;
		}
	}
	*dp = 0;
}

/*------------------------------------------------
  check format
--------------------------------------------------*/
DWORD FindFormat(char* fmt)
{
	char *sp;
	DWORD ret = 0;

	sp = fmt;
	while(*sp)
	{
		if(*sp == '<' && *(sp + 1) == '%')
		{
			sp += 2;
			while(*sp)
			{
				if(*sp == '%' && *(sp + 1) == '>')
				{
					sp += 2;
					break;
				}
				if(*sp == '\"')
				{
					sp++;
					while(*sp != '\"' && *sp) sp++;
					if(*sp == '\"') sp++;
				}
				else if(*sp == 's')
				{
					sp++;
					ret |= FORMAT_SECOND;
				}
				else if (*sp == '@' && *(sp + 1) == '@' && *(sp + 2) == '@')
				{
					sp += 3;
					if(*sp == '.' && *(sp + 1) == '@')
					{
						ret |= FORMAT_BEAT2;
						sp += 2;
					}
					else
						ret |= FORMAT_BEAT1;
				}
				else if(*sp == 'R' &&
					(*(sp + 1) == 'S' || *(sp + 1) == 'G' || *(sp + 1) == 'U') )
				{
					sp += 2;
					ret |= FORMAT_SYSINFO;
				}
				else if(*sp == 'C' && *(sp + 1) == 'U' &&
					(isdigit(*(sp + 2)) && *(sp + 2) != '8' && *(sp + 2) != '9') )
				{
					sp += 3;
					ret |= FORMAT_PERMON;
				}
				else if(*sp == 'C' && *(sp + 1) == 'U')
				{
					sp += 2;
					ret |= FORMAT_SYSINFO;
					ret |= FORMAT_PERMON;
				}
				else if(*sp == 'C' && *(sp + 1) == 'C')
				{
					sp += 2;
					ret |= FORMAT_CPU;
				}
				else if(*sp == 'B' && *(sp + 1) == 'L')
				{
					sp += 2;
					ret |= FORMAT_BATTERY;
				}
				else if(*sp == 'B' && (*(sp + 1) == 'h' || *(sp + 1) == 'n' || *(sp + 1) == 's' || *(sp + 1) == '_'))
				{
					sp += 2;
					ret |= FORMAT_BATTERY;
				}
				else if(*sp == 'A' && *(sp + 1) == 'D' )
				{
					sp += 2;
					ret |= FORMAT_BATTERY;
				}
				else if(*sp == 'a' && *(sp + 1) == 'd' )
				{
					sp += 2;
					ret |= FORMAT_BATTERY;
				}
				else if(*sp == 'M' && (*(sp + 1) == 'K' || *(sp + 1) == 'M'))
				{
					sp += 2;
					ret |= FORMAT_MEMORY;
				}
				else if(*sp == 'M' &&
					(*(sp + 1) == 'T' || *(sp + 1) == 'A' || *(sp + 1) == 'U') &&
					(*(sp + 2) == 'P' || *(sp + 2) == 'F' || *(sp + 2) == 'V') &&
					(*(sp + 3) == 'K' || *(sp + 3) == 'M' || *(sp + 3) == 'P'))
				{
					sp += 4;
					ret |= FORMAT_MEMORY;
				}
				else if(*sp == 'B' && *(sp + 1) == 'T')
				{
					sp += 2;
					ret |= FORMAT_MOTHERBRD;
				}
				else if(*sp == 'B' && *(sp + 1) == 'V')
				{
					sp += 2;
					ret |= FORMAT_MOTHERBRD;
				}
				else if(*sp == 'B' && *(sp + 1) == 'F')
				{
					sp += 2;
					ret |= FORMAT_MOTHERBRD;
				}
/*				else if(*sp == 'P' && *(sp + 1) == 'M')
				{
					sp += 2;
					ret |= FORMAT_PERMON;
				} */
				else if(*sp == 'N' &&
					(*(sp + 1) == 'R' || *(sp + 1) == 'S') &&
					(*(sp + 2) == 'S' || *(sp + 2) == 'A') &&
					(*(sp + 3) == 'M' || *(sp + 3) == 'K' || *(sp + 3) == 'B'))
				{
					sp += 4;
					ret |= FORMAT_NET;
				}
				else if(*sp == 'H' && (*(sp + 1) == 'A' || *(sp + 1) == 'U' || *(sp + 1) == 'T') && (*(sp + 2) >= 'A' && *(sp + 2) <= 'Z') && (*(sp + 3) == 'M' || *(sp + 3) == 'G' || *(sp + 3) == 'P'))
				{
					int dv;
					dv = *(sp + 2) - 'A';
					actdvl[dv] = 1;
					sp += 4;
					ret |= FORMAT_HDD;
				}
				else if(*sp == 'V' && *(sp + 1) == 'L')
				{
					sp += 2;
					ret |= FORMAT_VOL;
				}
				else sp = CharNext(sp);
			}
		}
		else sp = CharNext(sp);
	}
	return ret;
}

