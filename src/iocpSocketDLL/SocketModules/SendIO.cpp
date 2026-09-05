
#include "../iocpSocketDLL.h"
#include "SendIO.h"

CSendIO::CSendIO() : m_sendBuffer( NULL ), m_sendCount(0), m_networkSecurity( NULL )
{
}

CSendIO::~CSendIO()
{
	m_sendBuffer		= NULL;
	m_networkSecurity	= NULL;
}

void CSendIO::Reset()
{
	m_sendBuffer	= NULL;
	m_sendCount		= 0;
}

bool CSendIO::SendMessage( CPacket& rkPacket, const BOOL immediatelySend )
{
	// Declare)
	CPacket SendPacket( rkPacket.GetBuffer(), rkPacket.GetBufferSize() );

	if( m_networkSecurity )
		m_networkSecurity->PrepareMsg( SendPacket );

	// Declare)
	uint32 errorNum = 0;

	// Send
	if( SendMessage( SendPacket.GetBuffer(), SendPacket.GetBufferSize(), immediatelySend, errorNum ) != true )
	{
		PrintTimeAndLog( 0, "Error) SendMessage Fail.. SendBuffer empty code : %u", errorNum );
	}

	if( m_networkSecurity )
		m_networkSecurity->CompletionMsg( SendPacket );

	return true;
}

bool CSendIO::SendMessage( const char* buffer, const int length, const BOOL immediatelySend, uint32& errorNum )
{
	// Declare) ptr
	CSendBuffer* tempBufferPtr = GetSendBufferMemberPointer();

	// Check) 멤버가 Null인지..
	if( tempBufferPtr == NULL )
	{
		// Null이면 MemPool에서 빼옴.
		tempBufferPtr = g_SendBufferManager->Pop();
		if(!tempBufferPtr) 
		{
			errorNum = 1;
			return false;
		}
	}
	else
	{
		SetMemberPointer( NULL );
	}

	// Check Size..
	if( IsWriteAble( tempBufferPtr, length ) == TRUE )
	{
		// 바로 Write
		tempBufferPtr->WriteBuffer( buffer, length );
	}
	else
	{
		// Check : 여유 공간
		int remainSize = MAX_SENDBUFFER - tempBufferPtr->GetBufferSize();
		if( remainSize != 0 )
		{
			// 여유공간만큼 copy
			tempBufferPtr->WriteBuffer( buffer, remainSize );
		}

		// 꽉차서 Send
		PacketSend( tempBufferPtr );

		// Check : 전체 남은 size
		int afterSendRemainSize = length - remainSize;

		// LoopCount
		int loopCount = afterSendRemainSize / MAX_SENDBUFFER;

		// 버퍼가 작아지거나 packet size가 커지지 않을경우에는 0
		if( loopCount == 0 )
		{
			// Mempool Pop
			tempBufferPtr = g_SendBufferManager->Pop();
			if(!tempBufferPtr) 
			{
				errorNum = 2;
				return false;
			}


			// Write
			tempBufferPtr->WriteBuffer( buffer + remainSize, length - remainSize );
		}
		else
		{
			int Pos = remainSize;

			for( int i = 0 ; i < loopCount ; ++i )
			{
				// Mempool Pop
				tempBufferPtr = g_SendBufferManager->Pop();
				if(!tempBufferPtr) 
				{
					errorNum = 3;
					return false;
				}

				// Write
				tempBufferPtr->WriteBuffer( buffer + Pos, MAX_SENDBUFFER );

				// Pos 갱신 : write size만큼;
				Pos += MAX_SENDBUFFER;

				// Send
				PacketSend( tempBufferPtr );
			}

			// Check : 나머지 있는지
			if( length - Pos > 0 )
			{
				// Mempool Pop
				tempBufferPtr = g_SendBufferManager->Pop();
				if(!tempBufferPtr) 
				{
					errorNum = 4;
					return false;
				}

				// Write
				tempBufferPtr->WriteBuffer( buffer + Pos, length - Pos );

				// 마지막은 담아놓기만..
			}
			else
			{
				tempBufferPtr = NULL;
			}
		}// loopCount
	}// IsWriteable?


	// Check Option
	if( immediatelySend == FALSE )
	{
		// 멤버로 기억.
		SetMemberPointer( tempBufferPtr );
	}
	else
	{
		// 즉시 Send
		if(tempBufferPtr != NULL)
		{
			PacketSend( tempBufferPtr );
		}
	}

	return true;
}

bool CSendIO::PacketSend( CSendBuffer* sendBuffer )
{
	// log..
	//PrintTimeAndLog( 0, "%s - MemIndex : [ %d ], size : %d", __FUNCTION__, pSendBuffer->GetIndex(), pSendBuffer->GetBufferSize() );

	// Declare)
	DWORD dwFlags = 0, dwCount = 1;

	// WSASend
	int nSendResult = ::WSASend( 
		GetSocketHandle(), 
		&sendBuffer->m_stContext.m_wsaBuffer,
		dwCount,
		&sendBuffer->m_dwBytesTransfer,
		dwFlags,
		(WSAOVERLAPPED*)&sendBuffer->m_stContext.m_wsaOverlapped,
		NULL );

	// Send후 Null 초기화.
	InitMemberPointer( sendBuffer );

	// Check
	if( nSendResult == SOCKET_ERROR ) 
	{
		DWORD dwError = ::WSAGetLastError();
		if( dwError != WSA_IO_PENDING ) 
		{
			PrintTimeAndLog( 0, "Session SendMessage WriteFile FAILED:%d : BufferSize:%d", dwError, sendBuffer->GetBufferSize() );
			CloseConnection();
			//ExceptionClose( 0 );

			// 만약 WSASend() 에러시 SendBuffer를 메모리풀에 반환한다.
			ReturnSendBufferMemoryPool( sendBuffer );
			return false;
		}
	}

	// ++
	IncreseSendCount();

	return true;
}

void CSendIO::SetNS( NetworkSecurity* ns )
{
	m_networkSecurity = ns;
}

void CSendIO::ReturnSendBufferMemoryPool( CSendBuffer* sendBuffer )
{
	// Init
	sendBuffer->Init();

	// MemPool Push
	g_SendBufferManager->Push( sendBuffer );
}

void CSendIO::InitMemberPointer( CSendBuffer* sendBuffer )
{
	if( m_sendBuffer == sendBuffer )
	{
		m_sendBuffer = NULL;
	}
}

void CSendIO::SetMemberPointer( CSendBuffer* sendBuffer )
{
	m_sendBuffer = sendBuffer;
}

CSendBuffer* CSendIO::GetSendBufferMemberPointer()
{
	if( m_sendBuffer )
		return m_sendBuffer;

	return NULL;
}

void CSendIO::FlushSendBuffer()
{
	CSendBuffer* sendBuffer = GetSendBufferMemberPointer();
	if( sendBuffer == NULL )
		return;

	if( sendBuffer->GetBufferSize() > 0 )
	{
		SetMemberPointer( NULL );
		PacketSend( sendBuffer );
	}
}

BOOL CSendIO::IsWriteAble( CSendBuffer* sendBuffer, const int packetSize )
{
	if( sendBuffer->GetBufferSize() + packetSize > MAX_SENDBUFFER )
		return FALSE;

	return TRUE;
}

void CSendIO::IncreseSendCount()
{
	InterlockedIncrement( &m_sendCount );
}

void CSendIO::DecreaseSendCount()
{
	InterlockedDecrement( &m_sendCount );
}
