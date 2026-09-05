#include "stdafx.h"

#include "Scheduler.h"
#include "SchedulerNode.h"
#include "../ioMainProcess.h"
#include "../network/Protocol.h"
#include "../Network/ioPacketQueue.h"
#include "../NodeInfo/UserNodeManager.h"

SchedulerNode::SchedulerNode(void)
{
	Init();
}

SchedulerNode::~SchedulerNode(void)
{
	Destroy();
}

void SchedulerNode::Init()
{
}

void SchedulerNode::Destroy()
{
}

void SchedulerNode::ReceivePacket( CPacket &packet )
{
	g_RecvQueue.InsertQueue( (DWORD)this, packet, PK_QUEUE_INTERNAL );
}

void SchedulerNode::PacketParsing( CPacket &packet )
{
	SP2Packet &kPacket = (SP2Packet&)packet;

	switch( packet.GetPacketID() )
	{
	case ITPK_SENDBUFFER_FLUSH_PROCESS:
		OnSendBufferFlushProcess( kPacket );
		break;

	default:
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "0x%x Unknown CPacket",  kPacket.GetPacketID() );
		break;
	}
}

void SchedulerNode::Call(const int MSG)
{
	CPacket packet(MSG);
	ReceivePacket(packet);
}

void SchedulerNode::OnSendBufferFlushProcess( SP2Packet &packet )
{
	g_UserNodeManager.UserNode_SendBufferFlush();
}
