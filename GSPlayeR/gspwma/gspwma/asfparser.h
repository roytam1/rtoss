/*
 *  GSPlayer Windows Media Audio Plug-in
 *  Copyright (C) 2007  Y.Nagamidori
 */

#ifndef __ASF_PARSER_H__
#define __ASF_PARSER_H__

typedef struct _ASF_PACKET_INFO {
	int MultiplePayloadsPresent;
	int SequenceType;
	int PaddingLengthType;
	int PacketLengthType;
	int ErrorCorrectionPresent;

	int ReplicatedDataLengthType;
	int OffsetIntoMediaObjectLengthType;
	int MediaObjectNumberLengthType;
	int StreamNumberLengthType;

	DWORD PacketLength;
	DWORD Sequence;
	DWORD PaddingLength;
	DWORD SendTime;
	DWORD Duration;

	DWORD PacketHeaderSize;
} ASF_PACKET_INFO, *PASF_PACKET_INFO;

typedef struct _ASF_PAYLOAD_INFO {
	int StreamNumber;
	int Key;
	DWORD MediaObjectNumber;
	DWORD OffsetIntoMediaObject;
	DWORD ReplicatedDataLength;

	int NumberOfPayloads;
	int PayloadDataLengthType;
	DWORD PayloadDataLength;

	DWORD PayloadHeaderSize;
} ASF_PAYLOAD_INFO, *PASF_PAYLOAD_INFO;

typedef struct _ASF_FILE_INFO {
	HANDLE					hFile;
	HMODULE					hCodecDll;
	IMediaObject*			pMediaObject;
	QWORD					qwHeaderObjSize;
	QWORD					qwDataObjOffset;
	QWORD					FileSize;
	QWORD					PacketsCount;
	DWORD					MaxPacketSize;
	DWORD					MinPacketSize;
	LPBYTE					pbFormat;
	DWORD					dwFormat;
	BOOL					bSeekable;
	BOOL					bBroadcast;
	int						nCurrentStreamNumber;
	MAP_PLUGIN_FILE_INFO	FileInfo;
	MAP_PLUGIN_FILETAG		FileTag;

	LPBYTE					pbReadBuf;
	CMediaBuffer*			pmbOut;

	int						nPutPayloads;
	BOOL					bDiscontinuity;
	QWORD					nCurrentPacketNumber;
	ASF_PACKET_INFO			PacketInfo;
	ASF_PAYLOAD_INFO		PayloadInfo;
} ASF_FILE_INFO, *PASF_FILE_INFO;

#define MAX_PACKET_HEADER		38
#define MIN_PAYLOAD_HEADER		13

extern "C" {

BOOL IsAsfFile(LPCTSTR pszFile);
BOOL OpenAsfFile(LPCTSTR pszFile, ASF_FILE_INFO* pInfo);
void CloseAsfFile(ASF_FILE_INFO* pInfo);
BOOL GetAsfFileTag(LPCTSTR pszFile, MAP_PLUGIN_FILETAG* pTag);
int DecodeAsfFile(ASF_FILE_INFO* pInfo, WAVEHDR* pHdr);
int SeekAsfFile(ASF_FILE_INFO* pInfo, long lTime);
BOOL WINAPI StartDecodeAsfFile(ASF_FILE_INFO* pInfo);
void WINAPI StopDecodeAsfFile(ASF_FILE_INFO* pInfo);

}

#endif // __ASF_PARSER_H__