// cIocpOv.cpp: implementation of the cIocpOv class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "cSystem.h"
#include "cIocpOv.h"
#include "../include/cSingleton.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

cIocpOv::cIocpOv(E_IO_OPERATION iocpOP) : m_buffer(NULL), m_dataLength(0), m_bufferLength(0)
{
	Init(0, iocpOP);
}

void cIocpOv::Init(const uint32 bufferLength, E_IO_OPERATION iocpOP)
{
	ZeroMemory(&m_wsaBuf, sizeof(m_wsaBuf));
	ZeroMemory(&m_overlapped, sizeof(WSAOVERLAPPED));

	SetOperation( iocpOP );
}

void cIocpOv::Destroy()
{
	if(m_buffer)
	{
		delete []m_buffer;
		m_buffer = NULL;
	}
	m_dataLength = 0;
	m_bufferLength = 0;
	ZeroMemory(&m_wsaBuf, sizeof(m_wsaBuf));
	ZeroMemory(&m_overlapped, sizeof(WSAOVERLAPPED));
}

void cIocpOv::Clean()
{
	if( m_buffer )
	{
		m_dataLength = 0;
		ZeroMemory(m_buffer, m_bufferLength);
		ZeroMemory(&m_overlapped, sizeof(WSAOVERLAPPED));
	}
}

//////////////////////////////////////////////////////////////////////
// iocpOP
//////////////////////////////////////////////////////////////////////

void cIocpOv::Alloc(const uint32 bufferLength)
{
	uint8* tempBuffer = new uint8[ bufferLength ];
	if( tempBuffer )
		ZeroMemory(tempBuffer, bufferLength);

	if( m_buffer )
	{
		CopyMemory( tempBuffer, m_buffer, m_dataLength );
		delete []m_buffer;
	}

	m_buffer		= tempBuffer;
	m_bufferLength	= bufferLength;

	m_wsaBuf.buf	= reinterpret_cast<CHAR*>(m_buffer);
	m_wsaBuf.len	= m_bufferLength;
}

void cIocpOv::CopyBuffer(const uint8* buffer, const uint32 bufferLength)
{
	if( bufferLength >= m_bufferLength )
	{
		Alloc( bufferLength );
	}

	CopyMemory(m_wsaBuf.buf, buffer, bufferLength);
	m_dataLength	= bufferLength;
	m_wsaBuf.len	= m_dataLength;
}

void cIocpOv::AppendBuffer(const uint8* buffer, const uint32 bufferLength)
{
	if( (m_dataLength+bufferLength) >= m_bufferLength )
	{
		Alloc( (m_dataLength+bufferLength) );
	}

	CopyMemory(m_wsaBuf.buf+m_dataLength, buffer, bufferLength);
	m_dataLength	+= bufferLength;
	m_wsaBuf.len	= m_dataLength;
}