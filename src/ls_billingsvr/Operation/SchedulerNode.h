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
	void OnTestProcess( SP2Packet& packet );
	void OnRoomProcess( SP2Packet &packet );
	void OnTimerProcess( SP2Packet &packet );
	void OnPingProcess( SP2Packet &packet );
	void OnUserGhostProcess( SP2Packet &packet );
	void OnUserUpdateProcess( SP2Packet &packet );
	void OnMonsterCoinProcess( SP2Packet &packet );
	void OnSendBufferFlushProcess( SP2Packet &packet );
	void OnNexonAlive( SP2Packet &packet );
	void OnNexonSessionCheck( SP2Packet &packet );
	void OnWriteProcessLog( SP2Packet &packet );
	void OnDeleteMaxAliveTimeBillInfo( SP2Packet &packet );
	
public:
	int	m_BillMgrTimeout;	//BillMgr 타임아웃 지정
};
