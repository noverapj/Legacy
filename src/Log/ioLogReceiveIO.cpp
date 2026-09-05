#include "stdafx.h"
#include "ioLogReceiveIO.h"

ioLogReceiveIO::ioLogReceiveIO(void)
{
}

ioLogReceiveIO::~ioLogReceiveIO(void)
{
	Destroy();
}

void ioLogReceiveIO::Init(const int size)
{
	m_maxBuffer = size;

	m_context.m_aBuffer = new char[size];
}

void ioLogReceiveIO::Destroy()
{
	if( m_context.m_aBuffer )
	{
		delete[] m_context.m_aBuffer;
		m_context.m_aBuffer = NULL;
	}
}

void ioLogReceiveIO::InitRecvIO()
{
	::ZeroMemory( &m_context.m_stContext, sizeof( m_context.m_stContext ) );
	::ZeroMemory( m_context.m_aBuffer, m_maxBuffer );

	m_context.m_flags			  = 0;					
	m_context.m_dwBytesTransferred = 0;	
}


void ioLogReceiveIO::AddBytesTransferred(const DWORD bytesTransferred)
{
	m_context.m_dwBytesTransferred += bytesTransferred;
}

void ioLogReceiveIO::GetBuffer(void* buffer, const int size, const int offset)
{
	memcpy( buffer, m_context.m_aBuffer+offset, size );
}

bool ioLogReceiveIO::Receivable()
{
	if(GetBytesTransferred() >= GetMaxBuffer())
	{
		//PrintTimeAndLog(0,"ERROR : Receive Buffer (%lu, %lu)", GetBytesTransferred(), GetMaxBuffer());
		return false;
	}
	return true;
}

void ioLogReceiveIO::AfterReceive(const int bufferSize)
{
	m_context.m_dwBytesTransferred -= bufferSize;

	if( GetBytesTransferred() > 0 )
	{
		memcpy( m_context.m_aBuffer, (m_context.m_aBuffer + bufferSize), GetBytesTransferred() );
	}	
}

void ioLogReceiveIO::ReadyToReceive()
{
	m_context.m_stContext.m_flags = FLAG_RECEIVE;
	m_context.m_stContext.GetWsaBuf()->buf = &m_context.m_aBuffer[ GetBytesTransferred() ];
	m_context.m_stContext.GetWsaBuf()->len = m_maxBuffer - GetBytesTransferred();
	::ZeroMemory( m_context.m_stContext.GetOveralapped(), sizeof(WSAOVERLAPPED) );
}
