
#pragma once

#include "Scheduler.h"
#include "../Network/SP2Packet.h"

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
	void OnFlushProcess( SP2Packet &packet );
	void OnTradeProcess( SP2Packet &packet );
	void OnQuery( SP2Packet &packet );
	void OnSpecialShopCheck( SP2Packet &packet );
	void OnMatchProcess( SP2Packet &packet );
	void OnMatchRankSeason( SP2Packet &packet );
};
