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
	void OnRoomProcess( SP2Packet &packet );
	void OnTimerProcess( SP2Packet &packet );
	void OnPingProcess( SP2Packet &packet );
	void OnUserGhostProcess( SP2Packet &packet );
	void OnUserUpdateProcess( SP2Packet &packet );
	void OnMonsterCoinProcess( SP2Packet &packet );
	void OnSendBufferFlushProcess( SP2Packet &packet );
	void OnShuffleRoomProcess( SP2Packet &packet );
	void OnLadderteamMatchingProcess(SP2Packet &packet);
	void OnEnerPrisonerRoomLogProcess(SP2Packet &packet);
#ifdef ANTIHACK
	void OnUDPAntiHackCheck( SP2Packet &packet );
	void OnUDPAntiHackPenalty( SP2Packet &packet );
	void OnAntiHackReload( SP2Packet &packet );
#endif	
};
