#pragma once

#include "Packet.h"

// Àü¹æ
class CSendBuffer;
class NetworkSecurity;

class IOCP_SOCKET_API CSendIO
{
public:
	CSendIO();
	virtual ~CSendIO();

	void Reset();

private:
	CSendBuffer*	 m_sendBuffer;
	NetworkSecurity* m_networkSecurity;

	BOOL IsWriteAble( CSendBuffer* sendBuffer, const int packetSize );

public:
	virtual	SOCKET	GetSocketHandle() = 0;
	virtual	void	CloseConnection() = 0;
	virtual	void	SetNS( NetworkSecurity* ns );
	virtual	bool	SendMessage( CPacket& rkPacket, const BOOL immediatelySend = FALSE );
	virtual	bool	SendMessage( const char* buffer, const int length, const BOOL immediatelySend, uint32& errorNum );

public:
	bool PacketSend( CSendBuffer* sendBuffer );
	void ReturnSendBufferMemoryPool( CSendBuffer* sendBuffer );

	void InitMemberPointer( CSendBuffer* sendBuffer );
	void SetMemberPointer( CSendBuffer* sendBuffer );
	void FlushSendBuffer();

	CSendBuffer* GetSendBufferMemberPointer	();

public:
	LONG GetSendCount()	{ return m_sendCount; }

protected:
	void IncreseSendCount();
	void DecreaseSendCount();

	volatile LONG m_sendCount;
};
