#pragma once

#include "cPacket.h"
#include "cBuffer.h"
#include "cClientSocket.h"


class cConnector : public cClientSocket 
{
public:
	cConnector();
	virtual ~cConnector();

	void Init();
	void Destroy();

public:
	void SetIdentity( const uint32 ID )	{ m_ID = ID;	}
	const uint32 GetIdentity()			{ return m_ID;	}	

	const uint32 GetSequence()						
	{	
		if(m_sequence >= 0xff) m_sequence = 0;
		return ++m_sequence;		
	}	

	BOOL SendPacket(	const uint32 command,
						const uint8* buffer,
						const uint32 length );

protected:
	virtual	void OnConnected();
	virtual	void OnDisconnected();
	virtual	void OnReceived( uint8* buffer, const uint32 length );

	void CheckPacket( uint8* buffer, uint32 length );
	void PushCommand( uint32 command, uint8* buffer, uint32 length );

private:
	uint32	m_ID;
	uint32	m_sequence;
	cPacket	*m_packet;
	cBuffer	m_storage;
	cBuffer	m_temporary;
};
