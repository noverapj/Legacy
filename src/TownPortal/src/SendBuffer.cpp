

#include "../stdafx.h"
#include "SendBuffer.h"

SendBuffer::SendBuffer()
{
	InitBuffer();
}

SendBuffer::~SendBuffer()
{
	ClearBuffer();
}

void SendBuffer::InitBuffer()
{
	memset(m_pBuffer, 0, sizeof(m_pBuffer) );
	m_pStart = m_pEnd = m_pBuffer;
	m_iRestCnt = 0;
	m_bBlock = false;
}

void SendBuffer::ClearBuffer()
{
	memset(m_pBuffer, 0, sizeof(m_pBuffer) );
	m_pStart = m_pEnd = m_pBuffer;
	m_iRestCnt = 0;
	m_bBlock = false;
}

bool SendBuffer::EnqueueData( const char *pData, int iPacketSize)
{
	if( m_iRestCnt + iPacketSize > MAX_BUFFER )
		return false;
	
	memcpy(m_pEnd, pData, iPacketSize);

	int iExtra = 0;
	iExtra = m_pEnd + iPacketSize - m_pBuffer - MAX_BUFFER;
	if( iExtra >= 0)		// MAX_TEMP_BUFF 까지 복사가 되었는지
	{
		memcpy(m_pBuffer, m_pBuffer + MAX_BUFFER, iExtra);
		m_pEnd = m_pBuffer + iExtra;
	}
	else
	{
		m_pEnd += iPacketSize;
	}
	
	m_iRestCnt += iPacketSize;
	
	return true;
}

int SendBuffer::DequeueData(int iTransfered)
{
	int iExtra = m_pStart + iTransfered - m_pBuffer - MAX_BUFFER;
	int iRestSize = 0;

	if( iExtra >= 0)	
		m_pStart = m_pBuffer + iExtra;	
	else	
		m_pStart += iTransfered;
	
	m_iRestCnt -= iTransfered;
	iRestSize = m_iRestCnt;
	
	if( iRestSize > 0)
	{
		if( iRestSize > MAX_TEMP_BUFF ) iRestSize = MAX_TEMP_BUFF;

		iExtra = m_pBuffer + MAX_BUFFER - m_pStart;

		if( iExtra < iRestSize )	// 앞쪽에 있는 데이터를 뒤로 복사해옴
		{
			memcpy( m_pBuffer + MAX_BUFFER, m_pBuffer, iRestSize - iExtra );
		}
	}

	return iRestSize;
}

int SendBuffer::GetRestCnt()
{
	int iRestSize = m_iRestCnt;
	int iExtra = 0;

	if( iRestSize > 0)
	{
		if( iRestSize > MAX_TEMP_BUFF )	iRestSize = MAX_TEMP_BUFF;

		iExtra = m_pBuffer + MAX_BUFFER - m_pStart;

		if( iExtra < iRestSize )
		{
			memcpy( m_pBuffer + MAX_BUFFER, m_pBuffer, iRestSize - iExtra);
		}
	}

	return iRestSize;
}

char* SendBuffer::GetQueuePtr() const
{
	return m_pStart;
}

void SendBuffer::SetBlock( bool bBlock )
{
	m_bBlock = bBlock;
}

bool SendBuffer::IsBlocked() const
{
	return m_bBlock;
}