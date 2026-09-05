#pragma once

#include "IOCP.h"

struct RecvIO
{
	IOContext	m_stContext;
	DWORD		m_flags;					
	char        *m_aBuffer;
	DWORD       m_dwBytesTransferred;			// read size 
};

class IOCP_SOCKET_API CReceiveIO
{
public:
	CReceiveIO(void);
	~CReceiveIO(void);

	void Init(const int size);
	void Destroy();

public:
	void InitRecvIO();

	void AddBytesTransferred(const DWORD bytesTransferred);
	const DWORD GetMaxBuffer()			{ return m_maxBuffer; }
	const DWORD GetBytesTransferred()	{ return m_context.m_dwBytesTransferred; }
	const char* GetBuffer()				{ return m_context.m_aBuffer; }
	void GetBuffer(void* buffer, const int size, const int offset);

	bool Receivable();

	void AfterReceive(const int bufferSize);
	void ReadyToReceive();

	WSABUF* GetWsaBuf()					{ return &m_context.m_stContext.m_wsaBuffer; }
	WSAOVERLAPPED* GetOveralapped()		{ return &m_context.m_stContext.m_wsaOverlapped; }

protected:
	DWORD m_maxBuffer;
	RecvIO m_context;
};
