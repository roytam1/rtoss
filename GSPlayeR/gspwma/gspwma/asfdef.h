#ifndef __ASF_DEF_H__
#define __ASF_DEF_H__

typedef unsigned char	BYTE;
typedef wchar_t			WCHAR;
typedef unsigned short	WORD;
typedef unsigned long	DWORD;
typedef ULONGLONG		QWORD;

// Top-level ASF object GUIDS
const GUID ASF_Header_Object = {0x75B22630, 0x668E, 0x11CF, 0xA6, 0xD9, 0x00, 0xAA, 0x00, 0x62, 0xCE, 0x6C}; // 75B22630-668E-11CF-A6D9-00AA0062CE6C
const GUID ASF_Data_Object = {0x75B22636, 0x668E, 0x11CF, 0xA6, 0xD9, 0x00, 0xAA, 0x00, 0x62, 0xCE, 0x6C}; // 75B22636-668E-11CF-A6D9-00AA0062CE6C
const GUID ASF_Simple_Index_Object = {0x33000890, 0xE5B1, 0x11CF, 0x89, 0xF4, 0x00, 0xA0, 0xC9, 0x03, 0x49, 0xCB}; // 33000890-E5B1-11CF-89F4-00A0C90349CB
const GUID ASF_Index_Object = {0xD6E229D3, 0x35DA, 0x11D1, 0x90, 0x34, 0x00, 0xA0, 0xC9, 0x03, 0x49, 0xBE}; // D6E229D3-35DA-11D1-9034-00A0C90349BE
const GUID ASF_Media_Object_Index_Object = {0xFEB103F8, 0x12AD, 0x4C64, 0x84, 0x0F, 0x2A, 0x1D, 0x2F, 0x7A, 0xD4, 0x8C}; // FEB103F8-12AD-4C64-840F-2A1D2F7AD48C
const GUID ASF_Timecode_Index_Object = {0x3CB73FD0, 0x0C4A, 0x4803, 0x95, 0x3D, 0xED, 0xF7, 0xB6, 0x22, 0x8F, 0x0C}; // 3CB73FD0-0C4A-4803-953D-EDF7B6228F0C

// Header Object GUIDs
const GUID ASF_File_Properties_Object = {0x8CABDCA1, 0xA947, 0x11CF, 0x8E, 0xE4, 0x00, 0xC0, 0x0C, 0x20, 0x53, 0x65}; // 8CABDCA1-A947-11CF-8EE4-00C00C205365
const GUID ASF_Stream_Properties_Object = {0xB7DC0791, 0xA9B7, 0x11CF, 0x8E, 0xE6, 0x00, 0xC0, 0x0C, 0x20, 0x53, 0x65}; // B7DC0791-A9B7-11CF-8EE6-00C00C205365
const GUID ASF_Header_Extension_Object = {0x5FBF03B5, 0xA92E, 0x11CF, 0x8E, 0xE3, 0x00, 0xC0, 0x0C, 0x20, 0x53, 0x65}; // 5FBF03B5-A92E-11CF-8EE3-00C00C205365
const GUID ASF_Codec_List_Object = {0x86D15240, 0x311D, 0x11D0, 0xA3, 0xA4, 0x00, 0xA0, 0xC9, 0x03, 0x48, 0xF6}; // 86D15240-311D-11D0-A3A4-00A0C90348F6
const GUID ASF_Script_Command_Object = {0x1EFB1A30, 0x0B62, 0x11D0, 0xA3, 0x9B, 0x00, 0xA0, 0xC9, 0x03, 0x48, 0xF6}; // 1EFB1A30-0B62-11D0-A39B-00A0C90348F6
const GUID ASF_Marker_Object = {0xF487CD01, 0xA951, 0x11CF, 0x8E, 0xE6, 0x00, 0xC0, 0x0C, 0x20, 0x53, 0x65}; // F487CD01-A951-11CF-8EE6-00C00C205365
const GUID ASF_Bitrate_Mutual_Exclusion_Object = {0xD6E229DC, 0x35DA, 0x11D1, 0x90, 0x34, 0x00, 0xA0, 0xC9, 0x03, 0x49, 0xBE}; // D6E229DC-35DA-11D1-9034-00A0C90349BE
const GUID ASF_Error_Correction_Object = {0x75B22635, 0x668E, 0x11CF, 0xA6, 0xD9, 0x00, 0xAA, 0x00, 0x62, 0xCE, 0x6C}; // 75B22635-668E-11CF-A6D9-00AA0062CE6C
const GUID ASF_Content_Description_Object = {0x75B22633, 0x668E, 0x11CF, 0xA6, 0xD9, 0x00, 0xAA, 0x00, 0x62, 0xCE, 0x6C};	// 75B22633-668E-11CF-A6D9-00AA0062CE6C
const GUID ASF_Extended_Content_Description_Object = {0xD2D0A440, 0xE307, 0x11D2, 0x97, 0xF0, 0x00, 0xA0, 0xC9, 0x5E, 0xA8, 0x50}; // D2D0A440-E307-11D2-97F0-00A0C95EA850
const GUID ASF_Content_Branding_Object = {0x2211B3FA, 0xBD23, 0x11D2, 0xB4, 0xB7, 0x00, 0xA0, 0xC9, 0x55, 0xFC, 0x6E}; // 2211B3FA-BD23-11D2-B4B7-00A0C955FC6E
const GUID ASF_Stream_Bitrate_Properties_Object = {0x7BF875CE, 0x468D, 0x11D1, 0x8D, 0x82, 0x00, 0x60, 0x97, 0xC9, 0xA2, 0xB2}; // 7BF875CE-468D-11D1-8D82-006097C9A2B2
const GUID ASF_Content_Encryption_Object = {0x2211B3FB, 0xBD23, 0x11D2, 0xB4, 0xB7, 0x00, 0xA0, 0xC9, 0x55, 0xFC, 0x6E}; // 2211B3FB-BD23-11D2-B4B7-00A0C955FC6E
const GUID ASF_Extended_Content_Encryption_Object = {0x298AE614, 0x2622, 0x4C17, 0xB9, 0x35, 0xDA, 0xE0, 0x7E, 0xE9, 0x28, 0x9C}; // 298AE614-2622-4C17-B935-DAE07EE9289C
const GUID ASF_Digital_Signature_Object = {0x2211B3FC, 0xBD23, 0x11D2, 0xB4, 0xB7, 0x00, 0xA0, 0xC9, 0x55, 0xFC, 0x6E}; // 2211B3FC-BD23-11D2-B4B7-00A0C955FC6E
const GUID ASF_Padding_Object = {0x1806D474, 0xCADF, 0x4509, 0xA4, 0xBA, 0x9A, 0xAB, 0xCB, 0x96, 0xAA, 0xE8}; // 1806D474-CADF-4509-A4BA-9AABCB96AAE8

// Stream Properties Object Stream Type GUIDs
const GUID ASF_Audio_Media = {0xF8699E40, 0x5B4D, 0x11CF, 0xA8, 0xFD, 0x00, 0x80, 0x5F, 0x5C, 0x44, 0x2B}; // F8699E40-5B4D-11CF-A8FD-00805F5C442B
const GUID ASF_Video_Media = {0xBC19EFC0, 0x5B4D, 0x11CF, 0xA8, 0xFD, 0x00, 0x80, 0x5F, 0x5C, 0x44, 0x2B}; // BC19EFC0-5B4D-11CF-A8FD-00805F5C442B
const GUID ASF_Command_Media = {0x59DACFC0, 0x59E6, 0x11D0, 0xA3, 0xAC, 0x00, 0xA0, 0xC9, 0x03, 0x48, 0xF6}; // 59DACFC0-59E6-11D0-A3AC-00A0C90348F6
const GUID ASF_JFIF_Media = {0xB61BE100, 0x5B4E, 0x11CF, 0xA8, 0xFD, 0x00, 0x80, 0x5F, 0x5C, 0x44, 0x2B}; // B61BE100-5B4E-11CF-A8FD-00805F5C442B
const GUID ASF_Degradable_JPEG_Media = {0x35907DE0, 0xE415, 0x11CF, 0xA9, 0x17, 0x00, 0x80, 0x5F, 0x5C, 0x44, 0x2B}; // 35907DE0-E415-11CF-A917-00805F5C442B
const GUID ASF_File_Transfer_Media = {0x91BD222C, 0xF21C, 0x497A, 0x8B, 0x6D, 0x5A, 0xA8, 0x6B, 0xFC, 0x01, 0x85}; // 91BD222C-F21C-497A-8B6D-5AA86BFC0185
const GUID ASF_Binary_Media = {0x3AFB65E2, 0x47EF, 0x40F2, 0xAC, 0x2C, 0x70, 0xA9, 0x0D, 0x71, 0xD3, 0x43}; // 3AFB65E2-47EF-40F2-AC2C-70A90D71D343


// Stream Properties Object Error Correction Type GUIDs 
const GUID ASF_No_Error_Correction = {0x20FB5700, 0x5B55, 0x11CF, 0xA8, 0xFD, 0x00, 0x80, 0x5F, 0x5C, 0x44, 0x2B}; // 20FB5700-5B55-11CF-A8FD-00805F5C442B
const GUID ASF_Audio_Spread	= {0xBFC3CD50, 0x618F, 0x11CF, 0x8B, 0xB2, 0x00, 0xAA, 0x00, 0xB4, 0xE2, 0x20}; // BFC3CD50-618F-11CF-8BB2-00AA00B4E220

// Web stream Type-Specific Data GUIDs
const GUID ASF_Web_Stream_Media_Subtype = {0x776257D4, 0xC627, 0x41CB, 0x8F, 0x81, 0x7A, 0xC7, 0xFF, 0x1C, 0x40, 0xCC}; // 776257D4-C627-41CB-8F81-7AC7FF1C40CC
const GUID ASF_Web_Stream_Format = {0xDA1E6B13, 0x8359, 0x4050, 0xB3, 0x98, 0x38, 0x8E, 0x96, 0x5B, 0xF0, 0x0C}; // DA1E6B13-8359-4050-B398-388E965BF00C


#pragma pack(1)

typedef struct _ASF_OBJ {
	GUID	ObjId;
	QWORD	ObjSize;
} ASF_OBJ, *PASF_OBJ;

// Header Object
typedef struct _ASF_HEADER_OBJ {
	GUID	ObjId;
	QWORD	ObjSize;
	DWORD	HeaderCount;
	BYTE	Reserved1;
	BYTE	Reserved2;
} ASF_HEADER_OBJ, *PASF_HEADER_OBJ;

// File Properties Object 
typedef struct _ASF_FILE_PROP_OBJ {
	GUID	ObjId;
	QWORD	ObjSize;
	GUID	FileId;
	QWORD	FileSize;
	QWORD	CreationDate;
	QWORD	DataPacketsCount;
	QWORD	PlayDuration;
	QWORD	SendDuration;
	QWORD	Preroll;
	DWORD	Flags;
	DWORD	MinDataPacketSize;
	DWORD	MaxDataPacketSize;
	DWORD	MaxBitrate;
} ASF_FILE_PROP_OBJ, *PASF_FILE_PROP_OBJ;

// Stream Properties Object
typedef struct _ASF_STREAM_PROP_OBJ {
	GUID	ObjId;
	QWORD	ObjSize;
	GUID	StreamTypeGUID;
	GUID	ErrorCorrectionType;
	QWORD	TimeOffset;
	DWORD	TypeSpecificDataLength;
	DWORD	ErrorCorrectionDataLength;
	WORD	Flags;
	DWORD	Reserved;
	// Type-Specific Data	BYTE	varies
	// Error Correction Data	BYTE	varies
} ASF_STREAM_PROP_OBJ, *PASF_STREAM_PROP_OBJ;

// Header Extension Object
typedef struct _ASF_HDR_EXT_OBJ {
	GUID	ObjId;
	QWORD	ObjSize;
	GUID	Reserved1;
	WORD	Reserved2;
	DWORD	HdrExtDataSize;
	// Header Extension Data	BYTE	varies
} ASD_HDR_EXT_OBJ, *PASD_HDR_EXT_OBJ;

// Codec List Object 
typedef struct _ASF_CODEC_LIST_OBJ {
	GUID	ObjId;
	QWORD	ObjSize;
	GUID	Reserved;
	DWORD	CodecEntriesCount;
	// Codec Entries	See below	Varies
} ASF_CODEC_LIST_OBJ, *PASF_CODEC_LIST_OBJ;

// Content Description Object
typedef struct _ASF_CONTENT_DESC_OBJ {
	GUID	ObjId;
	QWORD	ObjSize;
	WORD	TitleLength;
	WORD	AuthorLength;
	WORD	CopyrightLength;
	WORD	DescriptionLength;
	WORD	RatingLength;
	//Title	WCHAR	Varies
	//Author	WCHAR	Varies
	//Copyright	WCHAR	Varies
	//Description	WCHAR	Varies
	//Rating	WCHAR	Varies
} ASF_CONTENT_DESC_OBJ, *PASF_CONTENT_DESC_OBJ;

// Extended Content Description Object
typedef struct _ASF_EXT_CONTENT_DESC_OBJ {
	GUID	ObjId;
	QWORD	ObjSize;
	WORD	ContentDescriptorsCount;
} ASF_EXT_CONTENT_DESC_OBJ, *PASF_EXT_CONTENT_DESC_OBJ;

typedef struct _ASF_TYPE_SPEC_DATA {
	WORD  wFormatTag;
	WORD  nChannels; 
	DWORD nSamplesPerSec;
	DWORD nAvgBytesPerSec;
	WORD  nBlockAlign;
	WORD  wBitsPerSample;
	WORD  cbSize; 
	// Codec Specific Data	BYTE	varies
} ASF_TYPE_SPEC_DATA, *PASF_TYPE_SPEC_DATA;

// ASF Data Object
typedef struct _ASF_DATA_OBJ {
	GUID	ObjId;
	QWORD	ObjSize;
	GUID	FileId;
	QWORD	TotalDataPackets;
	WORD	Reserved;
	// Data Packets	See section 5.2 varies
} ASF_DATA_OBJ, *PASF_DATA_OBJ;


#pragma pack()

#endif // __ASF_DEF_H__