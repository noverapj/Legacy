#include "../iocpSocketDLL.h"
#include "ReceiveIO.h"

CReceiveIO::CReceiveIO(void)
{
	m_maxBuffer = 0;
	m_context.m_flags = 0;
	m_context.m_aBuffer = NULL;
	m_context.m_dwBytesTransferred = 0;
}

CReceiveIO::~CReceiveIO(void)
{
	Destroy();
}

void CReceiveIO::Init(const int size)
{
	m_maxBuffer = size;

	m_context.m_aBuffer = new char[size];
}

void CReceiveIO::Destroy()
{
	if( m_context.m_aBuffer )
	{
		delete[] m_context.m_aBuffer;
		m_context.m_aBuffer = NULL;
	}
}

void CReceiveIO::InitRecvIO()
{
	::ZeroMemory( &m_context.m_stContext, sizeof( m_context.m_stContext ) );
	::ZeroMemory( m_context.m_aBuffer, m_maxBuffer );

	m_context.m_flags			  = 0;					
	m_context.m_dwBytesTransferred = 0;	
}


void CReceiveIO::AddBytesTransferred(const DWORD bytesTransferred)
{
	m_context.m_dwBytesTransferred += bytesTransferred;
}

void CReceiveIO::GetBuffer(void* buffer, const int size, const int offset)
{
	memcpy( buffer, m_context.m_aBuffer+offset, size );
}

bool CReceiveIO::Receivable()
{
	if(GetBytesTransferred() >= GetMaxBuffer())
	{
		PrintTimeAndLog(0,"ERROR : Receive Buffer (%lu, %lu)", GetBytesTransferred(), GetMaxBuffer());
		return false;
	}
	return true;
}

void CReceiveIO::AfterReceive(const int bufferSize)
{
	if (m_context.m_dwBytesTransferred < static_cast<DWORD>(bufferSize))
	{
		PrintTimeAndLog(0,"[ERROR] : Receive byte error - [m_maxBuffer size:%lu, m_dwBytesTransferred size:%lu, recev size:%d]", GetMaxBuffer(), m_context.m_dwBytesTransferred, bufferSize );
		InitRecvIO();
		return;
	}

	m_context.m_dwBytesTransferred -= bufferSize;
	if( GetBytesTransferred() > 0 )
	{
		memcpy( m_context.m_aBuffer, (m_context.m_aBuffer + bufferSize), GetBytesTransferred() );
	}
}

void CReceiveIO::ReadyToReceive()
{
	m_context.m_flags = ASYNCFLAG_RECEIVE;
	m_context.m_stContext.m_wsaBuffer.buf = &m_context.m_aBuffer[ GetBytesTransferred() ];
	m_context.m_stContext.m_wsaBuffer.len = m_maxBuffer - GetBytesTransferred();
	::ZeroMemory( &m_context.m_stContext.m_wsaOverlapped, sizeof(WSAOVERLAPPED) );
}
