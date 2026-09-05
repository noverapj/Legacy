#include "stdafx.h"

#include "Scheduler.h"
#include "SchedulerNode.h"

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
	g_Queue()->InsertQueue( (DWORD)this, packet, PK_QUEUE_INTERNAL );
}

void SchedulerNode::PacketParsing( CPacket &packet )
{
	FUNCTION_TIME_CHECKER( 500000.0f, packet.GetPacketID() );          // 0.5 초 이상 걸리면로그 남김
	SP2Packet &kPacket = (SP2Packet&)packet;

	switch( packet.GetPacketID() )
	{
	case ITPK_SENDBUFFER_FLUSH_PROCESS:
		{
			OnSendBufferFlushProcess(kPacket);
		}
		break;
	default:
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "0x%x Scedulernode Unknown CPacket",  kPacket.GetPacketID() );
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
	g_ServerConnectMgr()->Flush();
	g_ClientMgr()->Flush();
 
}

 