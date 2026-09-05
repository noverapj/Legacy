#include "stdafx.h"
#include "Ringbuffer.h"

CRingBuffer::CRingBuffer( const DWORD dwBufferSize, const DWORD dwPacketSize ) : m_dwBufferSize( dwBufferSize ), m_dwPlusBufferSize( m_dwBufferSize / 2 ),
																				 m_dwPacketSize( dwPacketSize ), m_bRotateBuffer( false ),
																				 m_aBuffer( NULL ), m_aInstantBuffer( NULL )
{
	m_aBuffer = new char[m_dwBufferSize + m_dwPacketSize];
	InitBuffer();	
}

CRingBuffer::~CRingBuffer()
{
	if( m_aBuffer )
	{
		delete[] m_aBuffer;
		m_aBuffer = NULL;
	}

	if( m_aInstantBuffer )
	{
		delete[] m_aInstantBuffer;
		m_aInstantBuffer = NULL;
	}
}

void CRingBuffer::InitBuffer()
{
	if( m_aInstantBuffer )
	{
		delete[] m_aInstantBuffer;
		m_aInstantBuffer = NULL;
	}

	::ZeroMemory( m_aBuffer, m_dwBufferSize + m_dwPacketSize );
	m_pStart  = m_aBuffer;       
	m_pEnd    = m_aBuffer;         
	m_pRealEnd= m_pEnd;    
}

bool CRingBuffer::IsNewMemory( int iAddBuffer )
{
	if( m_aInstantBuffer )         // 이미 한번 메모리 할당이 되었다
	{
		if( m_pRealEnd - m_aBuffer > (int)m_dwBufferSize )
			return true;
		return false;
	}

	if( m_pStart - m_pRealEnd <= iAddBuffer )
		return true;
	return false;
}

void CRingBuffer::Realloc()
{
	if( m_aInstantBuffer )   
	{
		// 기존의 버퍼가 처리되지 않았으면 계속 늘려만 간다.
		m_dwBufferSize   += m_dwPlusBufferSize;
		
		char *aTempBuffer = m_aBuffer;
		m_aBuffer = new char[m_dwBufferSize + m_dwPacketSize];

		int iUseSize = m_pRealEnd - aTempBuffer;
		::CopyMemory( m_aBuffer, aTempBuffer, iUseSize );
		m_pRealEnd = m_aBuffer + iUseSize;

		if( aTempBuffer )
		{
			delete[] aTempBuffer;
			aTempBuffer = NULL;
		}

		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Realloc Memory Type A: %d", m_dwBufferSize );
	}
	else
	{
		// 버퍼를 더 증가 시킨다.
		m_dwBufferSize += m_dwPlusBufferSize;
		m_aInstantBuffer = m_aBuffer;
		m_aBuffer = new char[m_dwBufferSize + m_dwPacketSize];

		int iUseSize = m_pRealEnd - m_aInstantBuffer;
		::CopyMemory( m_aBuffer, m_aInstantBuffer, iUseSize );
		m_pRealEnd = m_aBuffer + iUseSize;

		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Realloc Memory Type B: %d", m_dwBufferSize );
	}	
}

void CRingBuffer::WriteBuffer( char *pBuffer, int iBufferSize )
{
	if( !m_bRotateBuffer ) 
	{
		::CopyMemory( m_pEnd, pBuffer, iBufferSize );
		m_pEnd    += iBufferSize;		
		m_pRealEnd = m_pEnd;
		
		if( m_pEnd - m_aBuffer > (int)m_dwBufferSize )  // 다음 패킷 부터는 처음부터 넣는다.
		{
			m_pRealEnd      = m_aBuffer;                
			m_bRotateBuffer = true;         // 메모리 재할당이 발생하면 포인터로 판단이 어려워진다.
		}
	}
	else                         
	{
		if( IsNewMemory( iBufferSize ) )      // 버퍼를 늘려야하나?
			Realloc();
		::CopyMemory( m_pRealEnd, pBuffer, iBufferSize );
		m_pRealEnd += iBufferSize;		
	}
}

void CRingBuffer::SendComplete( int iBytesTransfer )
{
	m_pStart += iBytesTransfer;
	if( m_pStart >= m_pEnd )       // 다 보냈다.
	{
		// 일회용 버퍼 삭제
		if( m_aInstantBuffer )
		{
			delete[] m_aInstantBuffer;
			m_aInstantBuffer = NULL;
		}
		m_pStart = m_aBuffer;
		if( m_pEnd != m_pRealEnd )
			m_pEnd = m_pRealEnd;
		else
			m_pEnd = m_aBuffer;
		m_pRealEnd = m_pEnd;

		m_bRotateBuffer = false;
	}
}
