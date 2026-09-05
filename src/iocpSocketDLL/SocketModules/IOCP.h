#pragma once

enum OV_AsyncFlags
{
	ASYNCFLAG_NONE = 0x00,
	ASYNCFLAG_SEND,
	ASYNCFLAG_RECEIVE
};

struct IOContext 
{
	WSAOVERLAPPED	m_wsaOverlapped;
	WSABUF			m_wsaBuffer;
};

struct IOBufferedContext
{
	IOContext	m_stContext;			
	DWORD		m_flags;			
	DWORD		m_dwBytesTransfer;
};

