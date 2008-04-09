#include <windows.h>
#include <tchar.h>
#include "maplay.h"
#include "reader.h"
#include "mahelper.h"

const int tabbitrate[3][3][16] = 
{
   {{0,32,64,96,128,160,192,224,256,288,320,352,384,416,448,},		//V1L1
	{0,32,48,56, 64, 80, 96,112,128,160,192,224,256,320,384,},		//V1L2
	{0,32,40,48, 56, 64, 80, 96,112,128,160,192,224,256,320,} },	//V1L3

   {{0,32,48,56,64,80,96,112,128,144,160,176,192,224,256,},			//V2L1
	{0,8,16,24,32,40,48,56,64,80,96,112,128,144,160,},				//V2L2
	{0,8,16,24,32,40,48,56,64,80,96,112,128,144,160,} },			//V2L3

   {{0,32,48,56,64,80,96,112,128,144,160,176,192,224,256,},			//V2.5L1
	{0,8,16,24,32,40,48,56,64,80,96,112,128,144,160,},				//V2.5L2
	{0,8,16,24,32,40,48,56,64,80,96,112,128,144,160,} }				//V2.5L3
};

const int tabsamplingrate[3][4] = 
{
	{44100, 48000, 32000, 0},
	{22050, 24000, 16000, 0},
	{11025, 12000, 8000, 0}
};

const LPTSTR genre_strings[] = {
	_T("Blues"), _T("Classic Rock"), _T("Country"), _T("Dance"), _T("Disco"),
	_T("Funk"), _T("Grunge"), _T("Hip-Hop"), _T("Jazz"), _T("Metal"),
	_T("New Age"), _T("Oldies"), _T("Other"), _T("Pop"), _T("R&B"),
	_T("Rap"), _T("Reggae"), _T("Rock"), _T("Techno"), _T("Industrial"),
	_T("Alternative"), _T("Ska"), _T("Death Metal"), _T("Pranks"), _T("Soundtrack"),
	_T("Euro-Techno"), _T("Ambient"), _T("Trip-Hop"), _T("Vocal"), _T("Jazz+Funk"),
	_T("Fusion"), _T("Trance"), _T("Classical"), _T("Instrumental"), _T("Acid"),
	_T("House"), _T("Game"), _T("Sound Clip"), _T("Gospel"), _T("Noise"),
	_T("Alternative Rock"), _T("Bass"), _T("Soul"), _T("Punk"), _T("Space"),
	_T("Meditative"), _T("Instrumental Pop"), _T("Instrumental Rock"), _T("Ethnic"), _T("Gothic"),
	_T("Darkwave"), _T("Techno-Industrial"), _T("Electronic"), _T("Pop-Folk"), _T("Eurodance"),
	_T("Dream"), _T("Southern Rock"), _T("Comedy"), _T("Cult"), _T("Gangsta"),
	_T("Top 40"), _T("Christian Rap"), _T("Pop/Funk"), _T("Jungle"), _T("Native American"),
	_T("Cabaret"), _T("New Wave"), _T("Psychadelic"), _T("Rave"), _T("Showtunes"),
	_T("Trailer"), _T("Lo-Fi"), _T("Tribal"), _T("Acid Punk"), _T("Acid Jazz"),
	_T("Polka"), _T("Retro"), _T("Musical"), _T("Rock & Roll"), _T("Hard Rock"),
	_T("Folk"), _T("Folk-Rock"), _T("National Folk"), _T("Swing"), _T("Fast Fusion"),
	_T("Bebob"), _T("Latin"), _T("Revival"), _T("Celtic"), _T("Bluegrass"),
	_T("Avantgarde"), _T("Gothic Rock"), _T("Progressive Rock"), _T("Psychedelic Rock"), _T("Symphonic Rock"),
	_T("Slow Rock"), _T("Big Band"), _T("Chorus"), _T("Easy Listening"), _T("Acoustic"),
	_T("Humour"), _T("Speech"), _T("Chanson"), _T("Opera"), _T("Chamber Music"),
	_T("Sonata"), _T("Symphony"), _T("Booty Bass"), _T("Primus"), _T("Porn Groove"),
	_T("Satire"), _T("Slow Jam"), _T("Club"), _T("Tango"), _T("Samba"),
	_T("Folklore"), _T("Ballad"), _T("Power Ballad"), _T("Rhythmic Soul"), _T("Freestyle"),
	_T("Duet"), _T("Punk Rock"), _T("Drum Solo"), _T("Acapella"), _T("Euro-House"),
	_T("Dance Hall"), _T("Goa"), _T("Drum & Bass"), _T("Club-House"), _T("Hardcore"),
	_T("Terror"), _T("Indie"), _T("BritPop"), _T("Negerpunk"), _T("Polsk Punk"),
	_T("Beat"), _T("Christian Gangsta Rap"), _T("Heavy Metal"), _T("Black Metal"), _T("Crossover"),
	_T("Contemporary Christian"), _T("Christian Rock"), _T("Merengue"), _T("Salsa"), _T("Thrash Metal"),
	_T("Anime"), _T("Jpop"), _T("Synthpop"), NULL
};

BOOL IsValidFile(LPCTSTR pszFile)
{
	BOOL fRet = FALSE;
	CReader* pReader = new CReader();
	if (!pReader->Open(pszFile))
		return FALSE;

	// scan first 4096 bytes
	DWORD dwRead;
	BYTE bBuff[4096];
	if (pReader->Read(bBuff, sizeof(bBuff), &dwRead) && dwRead >= 4) {
		DWORD dwBuff = MAKELONG(MAKEWORD(bBuff[3], bBuff[2]), MAKEWORD(bBuff[1], bBuff[0]));
		if ((dwBuff & 0xFFFFFF00) == 0x49443300)
			fRet = TRUE; // ID3v2 = MP3
		else if (dwBuff == 0x52494646) {
			// RIFF
			if (dwRead >= 12) {
				dwBuff = MAKELONG(MAKEWORD(bBuff[11], bBuff[10]), MAKEWORD(bBuff[9], bBuff[8]));
				if (dwBuff == 0x524D5033) // RMP3
					fRet = TRUE;
				else if (dwBuff == 0x57415645) { // RIFF Wave
					if (dwRead >= 16) {
						dwBuff = MAKELONG(MAKEWORD(bBuff[15], bBuff[14]), MAKEWORD(bBuff[13], bBuff[12]));
						if (dwBuff == 0x666D7420) { // fmt 
							if (dwRead >= 21) {
								dwBuff = MAKELONG(MAKEWORD(bBuff[20], bBuff[21]), 0);
								if (dwBuff == 0x55 ||
									dwBuff == 0x50) // format id
									fRet = TRUE;
							}
						}
					}
				}
			}
		}
		else {
			DWORD cb = 0;
			while (cb < dwRead - 4) {
				MPEG_AUDIO_INFO info1, info2;
				if (ParseHeader(bBuff + cb, &info1)) {
					if (cb + info1.nFrameSize + 4 < dwRead) {
						ParseHeader(bBuff + cb + info1.nFrameSize, &info2);
						if (info1.nVersion == info2.nVersion &&
							info1.nLayer == info2.nLayer &&
							info1.nSamplingRate == info2.nSamplingRate) {
								fRet = TRUE;
								break;
						}
					}
				}
				cb++;
			}
		}
	}
	pReader->Close();
	delete pReader;
	return fRet;
}

BOOL ParseHeader(DWORD dwHeader, MPEG_AUDIO_INFO* pinfo)
{
	if (!CheckAudioHeader(dwHeader))
		return FALSE;

	int nPadding;
	MPEG_AUDIO_INFO info;
	info.nVersion = (dwHeader & 0x00100000) ? 2 - ((dwHeader & 0x00080000) >> 19) : 3;
	info.nLayer = 4 - ((dwHeader & 0x00060000) >> 17);
	info.nBitRate = tabbitrate[info.nVersion - 1][info.nLayer - 1][(dwHeader & 0x0000F000) >> 12];
	info.nSamplingRate = tabsamplingrate[info.nVersion - 1][(dwHeader & 0x00000C00) >> 10];
	nPadding = (dwHeader & 0x00000200) >> 9;
	info.nChannels = (((dwHeader & 0x000000C0) >> 6) == 3) ? 1 : 2;
	if (!info.nSamplingRate)
		return FALSE;

	if (!info.nBitRate)
		return FALSE;	// freeformat

	if (info.nLayer == 1) {
		info.nFrameSize = (12 * info.nBitRate * 1000 / info.nSamplingRate + nPadding) * 4;
		info.nSamplesPerFrame = 384;
	}
	else {
		if (info.nLayer >= 2 && info.nVersion >= 2) {
			// LSF
			info.nFrameSize = 72 * info.nBitRate * 1000 / info.nSamplingRate + nPadding;
			info.nSamplesPerFrame = 576;
		}
		else {
			info.nFrameSize = 144 * info.nBitRate * 1000 / info.nSamplingRate + nPadding;
			info.nSamplesPerFrame = 1152;
		}
	}
	*pinfo = info;
	return TRUE;
}

BOOL ParseHeader(LPBYTE pbHeader, MPEG_AUDIO_INFO* pinfo)
{
	DWORD dwHeader = MAKELONG(MAKEWORD(pbHeader[3], pbHeader[2]), 
								MAKEWORD(pbHeader[1], pbHeader[0]));
	return ParseHeader(dwHeader, pinfo);
}

int GetFrameSize(DWORD dwHeader)
{
	if (CheckAudioHeader(dwHeader))
		return 0;
	
	MPEG_AUDIO_INFO info;
	if (!ParseHeader(dwHeader, &info))
		return 0;

	return info.nFrameSize;
}

int GetFrameSize(LPBYTE pbHeader)
{
	DWORD dwHeader = MAKELONG(MAKEWORD(pbHeader[3], pbHeader[2]), 
								MAKEWORD(pbHeader[1], pbHeader[0]));
	return GetFrameSize(dwHeader);
}

void ConvertFromTagStr(BYTE buff[30], LPTSTR pszBuff, int nLen)
{
	char szBuff[31];
	memset(szBuff, 0, sizeof(szBuff));
	memcpy(szBuff, buff, sizeof(BYTE) * nLen);
	for (int i = nLen - 1; szBuff[i] == ' '; i--)
		szBuff[i] = '\0';

#ifdef _UNICODE
	MultiByteToWideChar(CP_ACP, 0, (char*)szBuff, -1, pszBuff, 31);
#else
	strcpy(pszBuff, szBuff);
#endif
}

BOOL GetId3Tag(LPCTSTR pszFile, ID3TAGV1* pTag)
{
	memset(pTag, 0, sizeof(ID3_TAG_V1));
	BOOL fRet = FALSE;

	// this supports ID3TAG v1 only !!
	CReader* pReader = new CReader();
	if (!pReader->Open(pszFile))
		return FALSE;

	LONGLONG llSize = pReader->GetSize();
	llSize -= sizeof(ID3_TAG_V1);
	pReader->SetPointer(llSize, FILE_BEGIN);

	DWORD dwRead;
	ID3_TAG_V1 id3tag;
	if (pReader->Read((LPBYTE)&id3tag, sizeof(id3tag), &dwRead) &&
		dwRead == sizeof(id3tag)) {
		if (id3tag.tag[0] == 'T' && id3tag.tag[1] == 'A' && id3tag.tag[2] == 'G') {
			// it's ID3TAG v1 !!
			fRet = TRUE;
			pTag->nTrackNum = 0;

			char buff[5];
			memset(buff, 0, sizeof(buff));
			memcpy(buff, id3tag.year, sizeof(id3tag.year));
			pTag->nYear = atoi(buff);

			ConvertFromTagStr(id3tag.albumName, pTag->szAlbum);
			ConvertFromTagStr(id3tag.artistName, pTag->szArtist);
			ConvertFromTagStr(id3tag.trackName, pTag->szTrack);

			//ID3TAG v1.1
			if (id3tag.comment[28] == NULL) {
				pTag->nTrackNum = id3tag.comment[29];
				ConvertFromTagStr(id3tag.comment, pTag->szComment, 28);
			}
			else
				ConvertFromTagStr(id3tag.comment, pTag->szComment);

			if (id3tag.genre < 148)
				_tcscpy(pTag->szGenre, genre_strings[id3tag.genre]);
			else
				memset(pTag->szGenre, 0, sizeof(pTag->szGenre));
		}
	}
	pReader->Close();
	delete pReader;
	return fRet;
}

BOOL SetId3Tag(LPCTSTR pszFile, ID3TAGV1* pTag)
{
	return FALSE;
}