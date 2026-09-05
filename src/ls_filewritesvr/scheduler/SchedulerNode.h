#pragma once

#include "Scheduler.h"

class CCommandNode;

class SchedulerNode : public CCommandNode, public Scheduler
{
public:
	SchedulerNode(void);
	~SchedulerNode(void);

	void Init();
	void Destroy();

public:
	virtual void ReceivePacket( CPacket &packet );
	virtual void PacketParsing( CPacket &packet );

	void Call(const int MSG);

protected:
	void OnSendBufferFlushProcess( SP2Packet &packet );
};
