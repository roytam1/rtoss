#include <windows.h>
#include <tchar.h>
#ifndef _WIN32_WCE
#include <mmsystem.h>
#endif
#include "dmo.h"
#include "mediabuf.h"

const GUID IID_IMediaBuffer = {0x59eff8b9, 0x938c, 0x4a26, 0x82, 0xf2, 0x95, 0xcb, 0x84, 0xcd, 0xc8, 0x37};

// CMediaBuffer
CMediaBuffer::CMediaBuffer(DWORD dwMax) : 
	m_lRef(1), m_pbBuf(NULL), m_dwBuf(0), m_dwMax(dwMax)
{
	m_pbBuf = new BYTE[m_dwMax];
}

CMediaBuffer::~CMediaBuffer()
{
	if (m_pbBuf) {
		delete [] m_pbBuf;
	}
}

HRESULT STDMETHODCALLTYPE CMediaBuffer::QueryInterface(REFIID riid, void __RPC_FAR *__RPC_FAR *ppvObject)
{
	if (memcmp(&riid, &IID_IMediaBuffer, sizeof(GUID)) == 0 || memcmp(&riid, &IID_IUnknown, sizeof(GUID)) == 0) {
		InterlockedIncrement(&m_lRef);
		*ppvObject = this;
	}
	return E_FAIL;
}

ULONG STDMETHODCALLTYPE CMediaBuffer::AddRef(void)
{
	return InterlockedIncrement(&m_lRef);
}

ULONG STDMETHODCALLTYPE CMediaBuffer::Release(void)
{
	LONG lRef = InterlockedDecrement(&m_lRef);
	if (lRef <= 0) {
		delete this;
		return lRef;
	}
	return lRef;
}

HRESULT STDMETHODCALLTYPE CMediaBuffer::SetLength(DWORD cbLength)
{
	if (cbLength > m_dwMax)
		return E_INVALIDARG;

	m_dwBuf = cbLength;
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CMediaBuffer::GetMaxLength(DWORD __RPC_FAR *pcbMaxLength)
{
	if (pcbMaxLength == NULL)
		return E_POINTER;

	*pcbMaxLength = m_dwMax;
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CMediaBuffer::GetBufferAndLength(BYTE __RPC_FAR *__RPC_FAR *ppBuffer, DWORD __RPC_FAR *pcbLength)
{
	if (pcbLength == NULL)
		return E_POINTER;

	if (ppBuffer != NULL)
		*ppBuffer = m_pbBuf;
	*pcbLength = m_dwBuf;
	return S_OK;
}
