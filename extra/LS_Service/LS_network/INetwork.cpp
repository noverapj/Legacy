#include "stdafx.h"
#include "cPacket.h"
#include "cIocpContext.h"
#include "cConnectorQueue.h"
#include "cSystem.h"
#include "../include/cSingleton.h"
#include "../include/IIocpContext.h"
#include "../include/INetwork.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

INetwork::INetwork(void)
{
}

INetwork::~INetwork(void)
{
}
//

//////////////////////////////////////////////////////////////////////
// Operation
//////////////////////////////////////////////////////////////////////

BOOL INetwork::Flush(IIocpContext *context)
{
	cIocpContext* iocpContext = static_cast<cIocpContext*>(context);
	return iocpContext->Flush();
}

BOOL INetwork::Send(	IIocpContext *context,
						const uint16 command, 
						const uint8* buffer, 
						const uint16 length,
						const BOOL encoding)
{
	cIocpContext* iocpContext = static_cast<cIocpContext*>(context);

	static cPacket *packet = NULL;
	if(NULL == packet)
	{
		packet = new cPacket(cSingleton<cSystem>::GetInstance()->GetEncoding());
	}

	packet->Make(	command, 
					buffer, 
					length,
					encoding ? 1 : 0 );

	return iocpContext->SendRequest(packet->GetBuffer(), packet->GetLength());
}

void INetwork::Send(	const uint32 ID,
						const uint16 command, 
						const uint8* buffer, 
						const uint16 length)
{
	cConnectorQueue* connectorQueue = cSingleton<cSystem>::GetInstance()->GetConnectorQueue();
	if(connectorQueue)
	{
		connectorQueue->PushCommand(	ID,
										command,
										buffer,
										length);
	}
}
