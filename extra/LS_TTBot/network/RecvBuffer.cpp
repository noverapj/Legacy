

#include "../stdafx.h"
#include "RecvBuffer.h"

RecvBuffer::RecvBuffer()
{
	InitBuffer();
}

RecvBuffer::~RecvBuffer()
{
}

void RecvBuffer::InitBuffer()
{
	m_iCurRcvSize = 0;
}

void RecvBuffer::UpdateRecvSize( int iCurRcvSize )
{
	m_iCurRcvSize += iCurRcvSize;
}

void RecvBuffer::RewindBuffer( int iRewindSize )
{
	m_iCurRcvSize -= iRewindSize;

	memcpy( m_pRewindBuf, ( m_pBuf + iRewindSize ), m_iCurRcvSize );
	memcpy( m_pBuf, m_pRewindBuf, m_iCurRcvSize );
}

char* RecvBuffer::GetStartPtr()
{
	return m_pBuf;
}

char* RecvBuffer::GetFreeSpacePtr()
{
	return m_pBuf + m_iCurRcvSize;
}

int RecvBuffer::GetCurRcvSize() const
{
	return m_iCurRcvSize;
}

int RecvBuffer::GetFreeSpaceSize() const
{
	return MAX_BUFFER - m_iCurRcvSize;
}
