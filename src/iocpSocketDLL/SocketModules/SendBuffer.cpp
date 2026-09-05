#include "../iocpSocketDLL.h"
#include "SendBuffer.h"


CSendBuffer::CSendBuffer(void)
{
	Init();
}


CSendBuffer::~CSendBuffer(void)
{
}

void CSendBuffer::Init()
{
	m_totalSize			= 0;
	m_dwBytesTransfer	= 0;
	m_flags				= 0;
	ZeroMemory( m_sendBuffer, sizeof( m_sendBuffer ) );
	ZeroMemory( &m_stContext, sizeof( m_stContext ) );
}

void CSendBuffer::WriteBuffer( const char* buffer, const int size )
{
	// Copy : buffer
	CopyMemory( GetSendBuffer() + GetBufferSize(), buffer, size );

	// Size Update
	UpdateBufferSize( size );
	
	m_flags						= ASYNCFLAG_SEND;
	m_dwBytesTransfer			= 0;
	m_stContext.m_wsaBuffer.buf	= GetSendBuffer();
	m_stContext.m_wsaBuffer.len	= GetBufferSize();
	::ZeroMemory( &m_stContext.m_wsaOverlapped, sizeof( WSAOVERLAPPED ) );
}

void CSendBuffer::UpdateBufferSize( const int size )
{
	m_totalSize += size;
}

char* CSendBuffer::GetSendBuffer()
{
	return m_sendBuffer;
}

const int CSendBuffer::GetBufferSize() const
{
	return m_totalSize;
}
