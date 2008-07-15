#include <windows.h> 
#include <tchar.h>
#ifndef _WIN32_WCE
#include <mmsystem.h>
#endif
#include "dmo.h"
#include "mapplugin.h"
#include "asfdef.h"
#include "mediabuf.h"
#include "asfparser.h"
#include "wmadmo.h"

BOOL ParseContextDescriptionObject(LPBYTE pbBuf, DWORD dwBuf, MAP_PLUGIN_FILETAG* pTag)
{
	WCHAR szTemp[MAX_PLUGIN_TAG_STR];
	ASF_CONTENT_DESC_OBJ* pContentDescObj = (ASF_CONTENT_DESC_OBJ*)pbBuf;
	if (pContentDescObj->ObjSize < dwBuf)
		return FALSE;

	if (pContentDescObj->TitleLength) {
		memcpy(szTemp, pbBuf + sizeof(ASF_CONTENT_DESC_OBJ), min(sizeof(szTemp), pContentDescObj->TitleLength));
		szTemp[MAX_PLUGIN_TAG_STR - 1] = NULL;
#ifdef _UNICODE
		_tcscpy(pTag->szTrack, szTemp);
#else
		WideCharToMultiByte(CP_ACP, 0, szTemp, -1, pTag->szTrack, MAX_PLUGIN_TAG_STR, NULL, NULL);
#endif
	}
	if (pContentDescObj->AuthorLength) {
		memcpy(szTemp, pbBuf + sizeof(ASF_CONTENT_DESC_OBJ) + pContentDescObj->TitleLength, min(sizeof(szTemp), pContentDescObj->AuthorLength));
		szTemp[MAX_PLUGIN_TAG_STR - 1] = NULL;
#ifdef _UNICODE
		_tcscpy(pTag->szArtist, szTemp);
#else
		WideCharToMultiByte(CP_ACP, 0, szTemp, -1, pTag->szArtist, MAX_PLUGIN_TAG_STR, NULL, NULL);
#endif
	}

	return TRUE;
}

BOOL ParseExtendedContextDescriptionObject(LPBYTE pbBuf, DWORD dwBuf, MAP_PLUGIN_FILETAG* pTag)
{
	int i;
	LPWSTR pszDescName = NULL;
	LPWSTR pszDescValue = NULL;
	QWORD nDescValue;
	WORD wValue;
	DWORD dwValue;
	WORD wDescNameLen, wDescDataType, wDescDataLen;
	ASF_EXT_CONTENT_DESC_OBJ* pExtContDescObj = (ASF_EXT_CONTENT_DESC_OBJ*)pbBuf;
	if (pExtContDescObj->ObjSize < dwBuf)
		return FALSE;

	pbBuf += sizeof(ASF_EXT_CONTENT_DESC_OBJ);
	dwBuf -= sizeof(ASF_EXT_CONTENT_DESC_OBJ);
	for (i = 0; i < pExtContDescObj->ContentDescriptorsCount; i++) {
		memcpy(&wDescNameLen, pbBuf, sizeof(WORD));
		pszDescName = new WCHAR[wDescNameLen];
		if (!pszDescName)
			return FALSE;

		memcpy(pszDescName, pbBuf + sizeof(WORD), wDescNameLen);
		memcpy(&wDescDataType, pbBuf + sizeof(WORD) + wDescNameLen, sizeof(WORD));
		memcpy(&wDescDataLen, pbBuf + sizeof(WORD) + wDescNameLen + sizeof(WORD), sizeof(WORD));

		switch (wDescDataType) {
		case 0x0000:	// Unicode
			pszDescValue = new WCHAR[wDescDataLen];
			if (!pszDescValue) {
				delete [] pszDescName;
				return FALSE;
			}
			memcpy(pszDescValue, pbBuf + sizeof(WORD) + wDescNameLen + sizeof(WORD) + sizeof(WORD), wDescDataLen);
			break;
		case 0x0002:	// BOOL
		case 0x0003:	// DWORD
			memcpy(&dwValue, pbBuf + sizeof(WORD) + wDescNameLen + sizeof(WORD) + sizeof(WORD), sizeof(DWORD)); // for 32bits CPU only
			nDescValue = dwValue;
			break;
		case 0x0004:	// QWORD
			memcpy(&nDescValue, pbBuf + sizeof(WORD) + wDescNameLen + sizeof(WORD) + sizeof(WORD), sizeof(QWORD));
			break;
		case 0x0005:	// WORD
			memcpy(&wValue, pbBuf + sizeof(WORD) + wDescNameLen + sizeof(WORD) + sizeof(WORD), sizeof(WORD));
			nDescValue = wValue;
			break;
		case 0x0001:	// Byte array
			break;
		}

		if (wcscmp(pszDescName, L"WM/TrackNumber") == 0) {
			pTag->nTrackNum = nDescValue;
		}
		else if (wcscmp(pszDescName, L"WM/Year") == 0) {
			if (wDescDataType == 0x0000)
				pTag->nYear = _tcstol(pszDescValue, NULL, 10);
			else if (wDescDataType != 0x0001)
				pTag->nYear = nDescValue;
		}
		else if (wcscmp(pszDescName, L"WM/AlbumTitle") == 0) {
			if (wDescDataType == 0x0000) {
#ifdef _UNICODE
				_tcsncpy(pTag->szAlbum, pszDescValue, min(MAX_PLUGIN_TAG_STR, _tcslen(pszDescValue)));
				pTag->szAlbum[MAX_PLUGIN_TAG_STR - 1] = NULL;
#else
				WideCharToMultiByte(CP_ACP, 0, pszDescValue, -1, pTag->szAlbum, MAX_PLUGIN_TAG_STR, NULL, NULL);
#endif
			}
		}
		else if (wcscmp(pszDescName, L"WM/Genre") == 0) {
			if (wDescDataType == 0x0000) {
#ifdef _UNICODE
				_tcsncpy(pTag->szGenre, pszDescValue, min(MAX_PLUGIN_TAG_STR, _tcslen(pszDescValue)));
				pTag->szGenre[MAX_PLUGIN_TAG_STR - 1] = NULL;
#else
				WideCharToMultiByte(CP_ACP, 0, pszDescValue, -1, pTag->szGenre, MAX_PLUGIN_TAG_STR, NULL, NULL);
#endif
			}
		}

		if (pszDescName) {
			delete [] pszDescName;
			pszDescName = NULL;
		}
		if (pszDescValue) {
			delete [] pszDescValue;
			pszDescValue = NULL;
		}
		pbBuf += (sizeof(WORD) + wDescNameLen + sizeof(WORD) + sizeof(WORD) + wDescDataLen);
		dwBuf -= (sizeof(WORD) + wDescNameLen + sizeof(WORD) + sizeof(WORD) + wDescDataLen);
	}

	return TRUE;
}

BOOL ParseAsfHeader(LPBYTE pbBuf, DWORD dwBuf, ASF_FILE_INFO* pInfo)
{
	int i;
	BOOL bRet = FALSE;
	ASF_OBJ* pObj = {0};
	ASF_HEADER_OBJ* pHdrObj = NULL;
	ASF_FILE_PROP_OBJ* pFilePropObj = NULL;
	ASF_STREAM_PROP_OBJ* pStreamPropObj = NULL;

	if (dwBuf < sizeof(ASF_HEADER_OBJ))
		return FALSE;
	
	// ASF_Header_Object
	pHdrObj = (ASF_HEADER_OBJ*)pbBuf;
	if (pHdrObj->ObjId != ASF_Header_Object)
		return FALSE;

	pInfo->qwHeaderObjSize = pHdrObj->ObjSize;
	pbBuf += sizeof(ASF_HEADER_OBJ);
	dwBuf -= sizeof(ASF_HEADER_OBJ);
	for (i = 0; i < pHdrObj->HeaderCount; i++) {
		pObj = (ASF_OBJ*)pbBuf;
		if (dwBuf < pObj->ObjSize)
			break;

		if (pObj->ObjId == ASF_File_Properties_Object) {
			pFilePropObj = (ASF_FILE_PROP_OBJ*)pObj;
			pInfo->bBroadcast = pFilePropObj->Flags & 0x1 ? TRUE : FALSE;
			pInfo->bSeekable = pFilePropObj->Flags & 0x2 ? TRUE : FALSE;
			pInfo->FileInfo.nAvgBitrate = pFilePropObj->MaxBitrate / 1000;
			pInfo->FileSize = pFilePropObj->FileSize;
			pInfo->PacketsCount = pFilePropObj->DataPacketsCount;
			pInfo->MaxPacketSize = pFilePropObj->MaxDataPacketSize;
			pInfo->MinPacketSize = pFilePropObj->MinDataPacketSize;
			if (!pInfo->bBroadcast)
				pInfo->FileInfo.nDuration = pFilePropObj->PlayDuration / (10000000 / 1000);  
		}
		else if (pObj->ObjId == ASF_Stream_Properties_Object) {
			pStreamPropObj = (ASF_STREAM_PROP_OBJ*)pObj;
			if (pStreamPropObj->StreamTypeGUID == ASF_Audio_Media && !pInfo->pbFormat) {
				pInfo->nCurrentStreamNumber = pStreamPropObj->Flags & 0x7F;
				if (pStreamPropObj->TypeSpecificDataLength) {
					pInfo->pbFormat = new BYTE[pStreamPropObj->TypeSpecificDataLength];
					if (!pInfo->pbFormat)
						return FALSE;

					memcpy(pInfo->pbFormat, pbBuf + sizeof(ASF_STREAM_PROP_OBJ), pStreamPropObj->TypeSpecificDataLength);
					pInfo->dwFormat = pStreamPropObj->TypeSpecificDataLength;					
				}
			}
		}
		else if (pObj->ObjId == ASF_Content_Description_Object) { // optional
			ParseContextDescriptionObject((LPBYTE)pObj, pObj->ObjSize, &pInfo->FileTag);
		}
		else if (pObj->ObjId == ASF_Extended_Content_Description_Object) { // optional
			ParseExtendedContextDescriptionObject((LPBYTE)pObj, pObj->ObjSize, &pInfo->FileTag);
		}
		
		pbBuf += pObj->ObjSize;
		dwBuf -= pObj->ObjSize;
	}

	return TRUE;
}

BOOL FindDataObject(ASF_FILE_INFO* pInfo)
{
	ASF_OBJ Obj;
	DWORD dwRead;
	LARGE_INTEGER offset = {0};

	while (TRUE) {
		if (!ReadFile(pInfo->hFile, &Obj, sizeof(Obj), &dwRead, NULL) || dwRead != sizeof(Obj))
			return FALSE;

		if (Obj.ObjId == ASF_Data_Object) {
			offset.LowPart = SetFilePointer(pInfo->hFile, offset.LowPart, &offset.HighPart, FILE_CURRENT);
			pInfo->qwDataObjOffset = offset.QuadPart - sizeof(Obj);
			return TRUE;
		}

		offset.QuadPart = Obj.ObjSize - sizeof(Obj);
		SetFilePointer(pInfo->hFile, offset.LowPart, &offset.HighPart, FILE_CURRENT); 
	}
	return FALSE;
}

BOOL IsAsfFile(LPCTSTR pszFile)
{
	HANDLE hFile;
	BOOL bRet = FALSE;
	ASF_HEADER_OBJ HdrObj = {0};
	DWORD dwRead = 0;

	hFile = CreateFile(pszFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return FALSE;

	if (ReadFile(hFile, &HdrObj, sizeof(ASF_HEADER_OBJ), &dwRead, NULL) && dwRead == sizeof(ASF_HEADER_OBJ)) {
		if (HdrObj.ObjId == ASF_Header_Object)
			bRet = TRUE;
	}

	CloseHandle(hFile);
	return bRet;
}

BOOL OpenAsfFile(LPCTSTR pszFile, ASF_FILE_INFO* pInfo)
{
	ASF_OBJ Obj = {0};
	DWORD dwRead = 0;
	LPBYTE pbHdrObj = NULL;
	WAVEFORMATEX* pwf; 

	memset(pInfo, 0, sizeof(ASF_FILE_INFO));
	pInfo->hFile = CreateFile(pszFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (pInfo->hFile == INVALID_HANDLE_VALUE) {
		pInfo->hFile = NULL;
		goto error;
	}

	if (!ReadFile(pInfo->hFile, &Obj, sizeof(Obj), &dwRead, NULL) || dwRead != sizeof(Obj))
		goto error;

	if (Obj.ObjId != ASF_Header_Object)
		goto error;

	pbHdrObj = new BYTE[Obj.ObjSize];
	if (!pbHdrObj)
		goto error;

	memcpy(pbHdrObj, &Obj, sizeof(Obj));
	if (!ReadFile(pInfo->hFile, pbHdrObj + sizeof(Obj), Obj.ObjSize - sizeof(Obj), &dwRead, NULL) || dwRead != Obj.ObjSize - sizeof(Obj))
		goto error;

	if (!ParseAsfHeader(pbHdrObj, Obj.ObjSize, pInfo))
		goto error;

	if (!pInfo->qwHeaderObjSize || !pInfo->FileInfo.nDuration || !pInfo->pbFormat || !pInfo->MaxPacketSize)
		goto error;

	if (!FindDataObject(pInfo))
		goto error;

	if (!DMOLoad(pInfo))
		goto error;

	if (!DMOSetMediaType(pInfo))
		goto error;

	if (pbHdrObj) delete [] pbHdrObj;

	pwf = (WAVEFORMATEX*)pInfo->pbFormat;
	pInfo->FileInfo.nAvgBitrate = pwf->nAvgBytesPerSec * 8 / 1000;
	pInfo->FileInfo.nBitsPerSample = pwf->wBitsPerSample;
	pInfo->FileInfo.nChannels = pwf->nChannels;
	pInfo->FileInfo.nSampleRate = pwf->nSamplesPerSec;

	if (pInfo->MaxPacketSize != pInfo->MinPacketSize)
		goto error; // sorry...

	pInfo->pbReadBuf = new BYTE[pInfo->MaxPacketSize];
	if (!pInfo->pbReadBuf)
		goto error;

	SeekAsfFile(pInfo, 0);
	return TRUE;

error:
	CloseAsfFile(pInfo);
	if (pbHdrObj) delete [] pbHdrObj;
	return FALSE;
}

void CloseAsfFile(ASF_FILE_INFO* pInfo)
{
	DMOFree(pInfo);
	if (pInfo->hFile) {
		CloseHandle(pInfo->hFile);
	}
	if (pInfo->pbFormat) {
		delete [] pInfo->pbFormat;
	}
	if (pInfo->pbReadBuf) {
		delete [] pInfo->pbReadBuf;
	}
	memset(pInfo, 0, sizeof(ASF_FILE_INFO));
}

BOOL GetAsfFileTag(LPCTSTR pszFile, MAP_PLUGIN_FILETAG* pTag)
{
	BOOL bRet = FALSE;
	ASF_OBJ Obj = {0};
	DWORD dwRead = 0;
	LPBYTE pbHdrObj = NULL;
	ASF_FILE_INFO Info = {0};

	Info.hFile = CreateFile(pszFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (Info.hFile == INVALID_HANDLE_VALUE) {
		Info.hFile = NULL;
		goto done;
	}

	if (!ReadFile(Info.hFile, &Obj, sizeof(Obj), &dwRead, NULL) || dwRead != sizeof(Obj))
		goto done;

	if (Obj.ObjId != ASF_Header_Object)
		goto done;

	pbHdrObj = new BYTE[Obj.ObjSize];
	if (!pbHdrObj)
		goto done;

	memcpy(pbHdrObj, &Obj, sizeof(Obj));
	if (!ReadFile(Info.hFile, pbHdrObj + sizeof(Obj), Obj.ObjSize - sizeof(Obj), &dwRead, NULL) || dwRead != Obj.ObjSize - sizeof(Obj))
		goto done;

	if (!ParseAsfHeader(pbHdrObj, Obj.ObjSize, &Info))
		goto done;

	if (!Info.qwHeaderObjSize || !Info.FileInfo.nDuration || !Info.pbFormat)
		goto done;

	bRet = TRUE;
	*pTag = Info.FileTag;

done:
	CloseAsfFile(&Info);
	if (pbHdrObj) delete [] pbHdrObj;
	return bRet;
}

int ReadTypedValue(LPBYTE pbBuf, DWORD dwBuf, DWORD* pdwValue, int nType, DWORD dwDefault)
{
	WORD wTemp;
	DWORD dwTemp;

	switch (nType) {
	case 0:
		*pdwValue = dwDefault;
		return 0;
	case 1:
		if (dwBuf < 1)
			return -1;
		*pdwValue = *pbBuf;
		return 1;
	case 2:
		if (dwBuf < 2)
			return -1;
		memcpy(&wTemp, pbBuf, sizeof(WORD));
		*pdwValue = wTemp;
		return 2;
	case 3:
		if (dwBuf < 4)
			return -1;
		memcpy(&dwTemp, pbBuf, sizeof(DWORD));
		*pdwValue = dwTemp;
		return 4;
	}

	return -1;
}

BOOL ParsePacketHeader(LPBYTE pbBuf, DWORD dwBuf, ASF_FILE_INFO* pInfo)
{
	int nRet;
	LPBYTE pbBufHead = pbBuf;
	ASF_PACKET_INFO* pPacketInfo = &pInfo->PacketInfo;

	if (dwBuf < 1)
		return FALSE;

	// error correction data
	if (pbBuf[0] & 0x80) {
		if (pbBuf[0] != 0x82 && pbBuf[0] != 0x90)
			return FALSE;

		pbBuf += (pbBuf[0] & 0x0F);
	}
	pbBuf++;
	
	if (pbBuf - pbBufHead + 2 > dwBuf) 
		return FALSE;

	// Payload parsing information
	pPacketInfo->MultiplePayloadsPresent = pbBuf[0] & 0x01;
	pPacketInfo->SequenceType = (pbBuf[0] & 0x06) >> 1;
	pPacketInfo->PaddingLengthType = (pbBuf[0] & 0x18) >> 3;
	pPacketInfo->PacketLengthType = (pbBuf[0] & 0x60) >> 5;
	pPacketInfo->ErrorCorrectionPresent = (pbBuf[0] & 0x80) >> 7;

	pPacketInfo->ReplicatedDataLengthType = (pbBuf[1] & 0x03);
	pPacketInfo->OffsetIntoMediaObjectLengthType = (pbBuf[1] & 0x0C) >> 2;;
	pPacketInfo->MediaObjectNumberLengthType = (pbBuf[1] & 0x30) >> 4;
	pPacketInfo->StreamNumberLengthType = (pbBuf[1] & 0xC0) >> 6;
	pbBuf += 2;

	// PacketLength
	nRet = ReadTypedValue(pbBuf, dwBuf - (pbBuf - pbBufHead), 
			&pPacketInfo->PacketLength, pPacketInfo->PacketLengthType, pInfo->MaxPacketSize);
	if (nRet == -1)
		return FALSE;
	pbBuf += nRet;

	// Sequence
	nRet = ReadTypedValue(pbBuf, dwBuf - (pbBuf - pbBufHead), &pPacketInfo->Sequence, pPacketInfo->SequenceType, 0);
	if (nRet == -1)
		return FALSE;
	pbBuf += nRet;

	// PaddingLength
	nRet = ReadTypedValue(pbBuf, dwBuf - (pbBuf - pbBufHead), &pPacketInfo->PaddingLength, pPacketInfo->PaddingLengthType, 0);
	if (nRet == -1)
		return FALSE;
	pbBuf += nRet;

	// SendTime
	nRet = ReadTypedValue(pbBuf, dwBuf - (pbBuf - pbBufHead), &pPacketInfo->SendTime, 3, 0);
	if (nRet == -1)
		return FALSE;
	pbBuf += nRet;

	// Duration
	nRet = ReadTypedValue(pbBuf, dwBuf - (pbBuf - pbBufHead), &pPacketInfo->Duration, 2, 0);
	if (nRet == -1)
		return FALSE;
	pbBuf += nRet;

	if (pPacketInfo->PacketLength > pInfo->MaxPacketSize || pPacketInfo->PacketLength < 8)
		return FALSE;

	pPacketInfo->PacketHeaderSize = pbBuf - pbBufHead;
	return TRUE;
}

int ParsePayloadHeader(LPBYTE pbBuf, DWORD dwBuf, ASF_FILE_INFO* pInfo)
{
	int nRet;
	LPBYTE pbBufHead = pbBuf;
	ASF_PACKET_INFO* pPacketInfo = &pInfo->PacketInfo;
	ASF_PAYLOAD_INFO* pPayloadInfo = &pInfo->PayloadInfo;
	
	if (dwBuf < 1)
		return FALSE;

	pPayloadInfo->StreamNumber = pbBuf[0] & 0x7F;
	pPayloadInfo->Key = (pbBuf[0] & 0x80) >> 7;
	pbBuf++;

	// Media Object Number
	nRet = ReadTypedValue(pbBuf, dwBuf - (pbBuf - pbBufHead), &pPayloadInfo->MediaObjectNumber, pPacketInfo->MediaObjectNumberLengthType, 0);
	if (nRet == -1)
		return FALSE;
	pbBuf += nRet;

	// Offset Into Media Object
	nRet = ReadTypedValue(pbBuf, dwBuf - (pbBuf - pbBufHead), &pPayloadInfo->OffsetIntoMediaObject, pPacketInfo->OffsetIntoMediaObjectLengthType, 0);
	if (nRet == -1)
		return FALSE;
	pbBuf += nRet;

	// Replicated Data Length
	nRet = ReadTypedValue(pbBuf, dwBuf - (pbBuf - pbBufHead), &pPayloadInfo->ReplicatedDataLength, pPacketInfo->ReplicatedDataLengthType, 0);
	if (nRet == -1)
		return FALSE;
	pbBuf += nRet;

	// Replicated Data
	pbBuf += pPayloadInfo->ReplicatedDataLength;

	// Payload Length
	if (pPacketInfo->MultiplePayloadsPresent) {
		nRet = ReadTypedValue(pbBuf, dwBuf - (pbBuf - pbBufHead), &pPayloadInfo->PayloadDataLength, pPayloadInfo->PayloadDataLengthType, 0);
		if (nRet == 0 || nRet == -1)
			return FALSE;
		pbBuf += nRet;
	}
	
	// Found PayloadData!
	pPayloadInfo->PayloadHeaderSize = pbBuf - pbBufHead;

	if (!pPacketInfo->MultiplePayloadsPresent) {
		pPayloadInfo->PayloadDataLength = pPacketInfo->PacketLength - pPacketInfo->PacketHeaderSize - pPayloadInfo->PayloadHeaderSize - pPacketInfo->PaddingLength;
	}

	return TRUE;
}

int GetPayloadHeaderSize(LPBYTE pbBuf, DWORD dwBuf, ASF_FILE_INFO* pInfo)
{
	int nSize, nRet = 1;
	DWORD dwReplicatedDataLength;
	
	ASF_PACKET_INFO* pPacketInfo = &pInfo->PacketInfo;
	ASF_PAYLOAD_INFO* pPayloadInfo = &pInfo->PayloadInfo;

	if (dwBuf < MIN_PAYLOAD_HEADER)
		return -1;

	switch (pPacketInfo->MediaObjectNumberLengthType) {
	case 1: nRet++; break;
	case 2: nRet+=2; break;
	case 3: nRet+=4; break;
	}
	switch (pPacketInfo->OffsetIntoMediaObjectLengthType) {
	case 1: nRet++; break;
	case 2: nRet+=2; break;
	case 3: nRet+=4; break;
	}

	nSize = ReadTypedValue(pbBuf + nRet, dwBuf - nRet, &dwReplicatedDataLength, pPacketInfo->ReplicatedDataLengthType, 0);
	if (nSize == -1)
		return -1;
	
	nRet += nSize;
	nRet += dwReplicatedDataLength;
	if (pPacketInfo->MultiplePayloadsPresent) {
		switch (pPayloadInfo->PayloadDataLengthType) {
		case 0: return FALSE;
		case 1: nRet++; break;
		case 2: nRet+=2; break;
		case 3: nRet+=4; break;
		}
	}

	return nRet;
}

BOOL FillBuffer(HANDLE hFile, LPBYTE pbBuf, DWORD dwBuf)
{
	DWORD dwRead;
	if (!ReadFile(hFile, pbBuf, dwBuf, &dwRead, NULL) || dwRead != dwBuf)
		return FALSE;
	return TRUE;
}

int DecodeAsfFile(ASF_FILE_INFO* pInfo, WAVEHDR* pHdr)
{
	int nPayloadHeader;
	DWORD dwRead, dwBuf = 0;

	if (!pInfo->hFile)
		return PLUGIN_RET_ERROR;

	if (DMOProcessOutput(pHdr, pInfo))
		return PLUGIN_RET_SUCCESS;

	while (TRUE) {
		if (pInfo->nCurrentPacketNumber >= pInfo->PacketsCount) {
			if (!pInfo->bDiscontinuity)
				DMODiscontinuity(pInfo);
			
			if (DMOProcessOutput(pHdr, pInfo))
				return PLUGIN_RET_SUCCESS;

			return PLUGIN_RET_EOF;
		}

		if (!pInfo->PacketInfo.PacketHeaderSize) {
			pInfo->nPutPayloads = 0;
			if (!FillBuffer(pInfo->hFile, pInfo->pbReadBuf, MAX_PACKET_HEADER))
				break;

			dwBuf = MAX_PACKET_HEADER;
			if (!ParsePacketHeader(pInfo->pbReadBuf, dwBuf, pInfo))
				break;

			memmove(pInfo->pbReadBuf, pInfo->pbReadBuf + pInfo->PacketInfo.PacketHeaderSize, dwBuf - pInfo->PacketInfo.PacketHeaderSize);
			dwBuf -= pInfo->PacketInfo.PacketHeaderSize;
			if (pInfo->PacketInfo.MultiplePayloadsPresent) {
				if (dwBuf < 1) {
					if (!FillBuffer(pInfo->hFile, pInfo->pbReadBuf + dwBuf, 1))
						break;
					dwBuf += 1;
				}

				pInfo->PayloadInfo.NumberOfPayloads = pInfo->pbReadBuf[0] & 0x3F;
				pInfo->PayloadInfo.PayloadDataLengthType = (pInfo->pbReadBuf[0] & 0xC0) >> 6;
				
				memmove(pInfo->pbReadBuf, pInfo->pbReadBuf + 1, dwBuf - 1);
				dwBuf -= 1;
			}
			else {
				pInfo->PayloadInfo.NumberOfPayloads = 1;
				pInfo->PayloadInfo.PayloadDataLengthType = 0;
			}
		}

		while (pInfo->nPutPayloads < pInfo->PayloadInfo.NumberOfPayloads) {
			if (dwBuf < MIN_PAYLOAD_HEADER) {
				if (!FillBuffer(pInfo->hFile, pInfo->pbReadBuf + dwBuf, MIN_PAYLOAD_HEADER))
					return PLUGIN_RET_ERROR;
				dwBuf += MIN_PAYLOAD_HEADER;
			}

			nPayloadHeader = GetPayloadHeaderSize(pInfo->pbReadBuf, dwBuf, pInfo);
			if (nPayloadHeader == -1) 
				return PLUGIN_RET_ERROR;

			if (dwBuf < nPayloadHeader) {
				if (!FillBuffer(pInfo->hFile, pInfo->pbReadBuf + dwBuf, nPayloadHeader))
					break;
				dwBuf += nPayloadHeader;
			}

			if (!ParsePayloadHeader(pInfo->pbReadBuf, dwBuf, pInfo))
				return PLUGIN_RET_ERROR;

			if (pInfo->PayloadInfo.PayloadDataLength > pInfo->PacketInfo.PacketLength)
				return PLUGIN_RET_ERROR;

			dwBuf -= pInfo->PayloadInfo.PayloadHeaderSize;
			SetFilePointer(pInfo->hFile, (LONG)-dwBuf, NULL, FILE_CURRENT);
			
			if (pInfo->PayloadInfo.StreamNumber == pInfo->nCurrentStreamNumber) {
				if (!DMOProcessInputFile(pInfo->PayloadInfo.PayloadDataLength, pInfo))
					return PLUGIN_RET_ERROR;
			}
			else {
				SetFilePointer(pInfo->hFile, pInfo->PayloadInfo.PayloadDataLength, NULL, FILE_CURRENT);
			}

			dwBuf = 0;
			pInfo->nPutPayloads++;
			if (DMOProcessOutput(pHdr, pInfo)) {
				return PLUGIN_RET_SUCCESS;
			}
		}
		SetFilePointer(pInfo->hFile, pInfo->PacketInfo.PaddingLength, NULL, FILE_CURRENT);
		pInfo->nPutPayloads = 0;
		pInfo->nCurrentPacketNumber++;
		memset(&pInfo->PacketInfo, 0, sizeof(pInfo->PacketInfo));
	}

	return PLUGIN_RET_ERROR;
}

int SeekAsfFile(ASF_FILE_INFO* pInfo, long lTime)
{
	QWORD nPacket;
	LARGE_INTEGER offset;

	if (!pInfo->hFile || !pInfo->bSeekable)
		return -1;

	nPacket = (double)lTime / pInfo->FileInfo.nDuration * pInfo->PacketsCount;
	offset.QuadPart = pInfo->qwDataObjOffset + sizeof(ASF_DATA_OBJ);
	offset.QuadPart += nPacket * pInfo->MaxPacketSize;
	SetFilePointer(pInfo->hFile, offset.LowPart, &offset.HighPart, FILE_BEGIN);
	pInfo->nCurrentPacketNumber = nPacket;

	pInfo->nPutPayloads = 0;
	memset(&pInfo->PacketInfo, 0, sizeof(pInfo->PacketInfo));
	memset(&pInfo->PayloadInfo, 0, sizeof(pInfo->PayloadInfo));

	DMOFlush(pInfo);
	return (double)pInfo->FileInfo.nDuration / pInfo->PacketsCount * nPacket;
}

BOOL WINAPI StartDecodeAsfFile(ASF_FILE_INFO* pInfo)
{
	pInfo->nPutPayloads = 0;
	memset(&pInfo->PacketInfo, 0, sizeof(pInfo->PacketInfo));
	memset(&pInfo->PayloadInfo, 0, sizeof(pInfo->PayloadInfo));

	return TRUE;
}

void WINAPI StopDecodeAsfFile(ASF_FILE_INFO* pInfo)
{
	DMOFlush(pInfo);
}
