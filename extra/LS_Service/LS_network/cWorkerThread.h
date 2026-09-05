#pragma once


#include "cBaseThread.h"
#include "cBuffer.h"
#include "cPacket.h"

class cIocpContext;

enum EventID
{
	EVENT_CONNECT = 1,
	EVENT_RECEIVE,
	EVENT_CLOSE
};

//////////////////////////////////////////////////////////////////////
// IOCP Worker Thread
//////////////////////////////////////////////////////////////////////

class cWorkerThread : public cBaseThread
{
public:
	cWorkerThread();
	virtual ~cWorkerThread();

	void Init();
	void Destroy();

public:
	BOOL OnConnect( cIocpContext* iocpContext );
	BOOL OnDisconnect( cIocpContext* iocpContext );
	BOOL OnReceive( cIocpContext* iocpContext, DWORD length );

	BOOL CheckPacket( cIocpContext* iocpContext, uint8* buffer, uint32 length );
	BOOL PushCommand( EventID eventId, cIocpContext* iocpContext, uint8* buffer = NULL, uint32 length = 0);

protected:
	virtual void Process();

protected:
	cBuffer	m_storage;
	cPacket	*m_packet;
};