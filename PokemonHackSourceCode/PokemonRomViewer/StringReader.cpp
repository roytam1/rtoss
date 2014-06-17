#include "stdafx.h"
#include "StringReader.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
CStringReader::CStringReader()
{
	m_pbBuffer = NULL;
	m_dwLength = 0;
	m_dwPosition = 0;
}

CStringReader::~CStringReader()
{
	Close();
}

BOOL CStringReader::Open(LPCTSTR szFileName)
{
	BOOL	bResult;
	CString	szLine;

	try
	{
		Close();

		bResult = m_file.Open(szFileName, CFile::modeRead | CFile::shareDenyNone);
		if(!bResult)
			throw 0;

		m_dwLength = (DWORD)(m_file.GetLength());
		m_pbBuffer = new BYTE[m_dwLength];
		if(!m_pbBuffer)
			throw 0;

		if(m_file.Read(m_pbBuffer, m_dwLength) != m_dwLength)
			throw 0;

		bResult = ReadLine(szLine);
		if(!bResult)
			throw 0;

		bResult = TRUE;
	}
	catch(int)
	{
		bResult = FALSE;
	}
	catch(CFileException *e)
	{
		e->Delete();
		bResult = FALSE;
	}

	return bResult;
}

VOID CStringReader::Close()
{
	m_dwLength = 0;
	m_dwPosition = 0;

	if(m_pbBuffer)
	{
		delete []m_pbBuffer;
		m_pbBuffer = NULL;
	}

	if(m_file.m_hFile != CFile::hFileNull)
	{
		m_file.Close();
	}
}

BOOL CStringReader::ReadLine(CString &szLine)
{
	BOOL	bResult = TRUE;
	WCHAR	c;

	try
	{
		szLine = _T("");

		if(!m_pbBuffer)
			throw 0;

		do
		{
			if(m_dwPosition < m_dwLength)
			{
				c = *(reinterpret_cast<WCHAR *>(m_pbBuffer + m_dwPosition));
				m_dwPosition += sizeof(WCHAR);

				if(c != 0xFEFF && c != L'\r' && c != L'\n')
				{
					szLine += c;
				}
			}
			else
			{
				break;
			}
		}
		while(c != L'\n');
	}
	catch(int)
	{
		bResult = FALSE;
	}
	//catch(CFileException *e)
	//{
	//	e->Delete();
	//	bResult = FALSE;
	//}

	return bResult;
}

BOOL CStringReader::GetNextLine(StringEntry &se)
{
	BOOL	bResult;
	CString	szLine;
	DWORD	m, n, l, k;
	BOOL	bSkip;

	try
	{
		if(!m_pbBuffer)
			throw 0;

		bResult = ReadLine(szLine);
		if(!bResult)
			throw 0;

		l = szLine.GetLength();
		bSkip = TRUE;
		k = 0;

		for(m = n = 0; n < l && m < 1 + LANG_COUNT; ++n)
		{
			if(bSkip)
			{
				if(szLine.GetAt(n) == L'{')
				{
					bSkip = FALSE;
					k = n + 1;
				}
			}
			else
			{
				if(szLine.GetAt(n) == L'}')
				{
					if(m == 0)
					{
						se.dwId = _tcstoul(szLine.Mid(k, n - k), 0, 0);
					}
					else
					{
						se.rgszText[m - 1] = szLine.Mid(k, n - k);
					}
					++m;
					bSkip = TRUE;
				}
			}
		}

		if(m < LANG_COUNT - 1)
			throw 0;

		bResult = TRUE;
	}
	catch(int)
	{
		bResult = FALSE;
	}
	//catch(CFileException *e)
	//{
	//	e->Delete();
	//	bResult = FALSE;
	//}

	if(!bResult)
	{
		for(m = 0; m < LANG_COUNT; ++m)
		{
			se.rgszText[m].Empty();
		}
	}

	return bResult;
}

BOOL CStringReader::BatchReadString(LPCTSTR szFileName, StringEntry *pSE, DWORD dwCount)
{
	BOOL	bResult;
	DWORD	dwIndex;

	try
	{
		if(!Open(szFileName))
			throw 0;

		for(dwIndex = 0; dwIndex < dwCount; ++dwIndex)
		{
			bResult = GetNextLine(pSE[dwIndex]);
			if(!bResult)
				throw 0;
		}
	}
	catch(int)
	{
		bResult = FALSE;
	}

	Close();

	return bResult;
}
