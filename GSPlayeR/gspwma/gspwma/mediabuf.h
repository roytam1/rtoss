#ifndef __MEDIA_BUF_H__
#define __MEDIA_BUF_H__

class CMediaBuffer : public IMediaBuffer
{
public:
	CMediaBuffer(DWORD dwMax);

	// IUnknown
	virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void __RPC_FAR *__RPC_FAR *ppvObject);
	virtual ULONG STDMETHODCALLTYPE AddRef(void);
	virtual ULONG STDMETHODCALLTYPE Release(void);

	// IMediaBuffer
	virtual HRESULT STDMETHODCALLTYPE SetLength(DWORD cbLength);
	virtual HRESULT STDMETHODCALLTYPE GetMaxLength(DWORD __RPC_FAR *pcbMaxLength);
	virtual HRESULT STDMETHODCALLTYPE GetBufferAndLength(BYTE __RPC_FAR *__RPC_FAR *ppBuffer, DWORD __RPC_FAR *pcbLength);

protected:
	LONG	m_lRef;
	LPBYTE	m_pbBuf;
	DWORD	m_dwBuf;
	DWORD	m_dwMax;

private:
	~CMediaBuffer();
};


#endif // __MEDIA_BUF_H__