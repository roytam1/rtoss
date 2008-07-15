#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tchar.h>
#include <stdlib.h>
#include <stdio.h>

#ifndef _WIN32_WCE
#include <mmsystem.h>
#endif

#include "mapplugin.h"

#pragma pack(1)
typedef struct tID3Tagv1 {
	BYTE	tag[3];
	BYTE	trackName[30];
	BYTE	artistName[30];
	BYTE	albumName[30];
	BYTE	year[4];
	BYTE	comment[30];
	BYTE	genre;
}ID3_TAG_V1;
#pragma pack()

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

BOOL GetId3TagV1(FILE* fp, MAP_PLUGIN_FILETAG* pTag)
{
	char buff[5];
	ID3_TAG_V1 id3tag;
	BOOL bRet = FALSE;

	long curoffset = ftell(fp);
	fseek(fp, -sizeof(ID3_TAG_V1), SEEK_END);
	
	if (!fread(&id3tag, sizeof(ID3_TAG_V1), 1, fp))
		goto done;

	if (id3tag.tag[0] != 'T' || id3tag.tag[1] != 'A' || id3tag.tag[2] != 'G')
		goto done;

	bRet = TRUE;
	memset(buff, 0, sizeof(buff));
	memcpy(buff, id3tag.year, sizeof(id3tag.year));
	pTag->nYear = atoi(buff);

	ConvertFromTagStr(id3tag.albumName, pTag->szAlbum, 30);
	ConvertFromTagStr(id3tag.artistName, pTag->szArtist, 30);
	ConvertFromTagStr(id3tag.trackName, pTag->szTrack, 30);

	//ID3TAG v1.1
	if (id3tag.comment[28] == NULL) {
		pTag->nTrackNum = id3tag.comment[29];
		ConvertFromTagStr(id3tag.comment, pTag->szComment, 28);
	}
	else
		ConvertFromTagStr(id3tag.comment, pTag->szComment, 30);

	if (id3tag.genre < 148)
		_tcscpy(pTag->szGenre, genre_strings[id3tag.genre]);
	else
		memset(pTag->szGenre, 0, sizeof(pTag->szGenre));
	
done:
	fseek(fp, curoffset, SEEK_SET);
	return bRet;
}
