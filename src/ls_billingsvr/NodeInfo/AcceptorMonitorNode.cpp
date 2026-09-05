#include "../StdAfx.h"
#include "../MainProcess.h"
#include "../MonitoringServerNode/MonitoringNode.h"
#include "../MonitoringServerNode/MonitoringNodeManager.h"
#include "../Network/ioPacketQueue.h"
#include "AcceptorMonitorNode.h"

AcceptorMonitorNode::AcceptorMonitorNode(void)
{
	Init();
}

AcceptorMonitorNode::~AcceptorMonitorNode(void)
{
	Destroy();
}

void AcceptorMonitorNode::Init()
{
}

void AcceptorMonitorNode::Destroy()
{
}

void AcceptorMonitorNode::ReceivePacket( CPacket &packet, SOCKET socket )
{
	if(!g_RecvQueue.InsertQueue( (DWORD)this, packet, socket ))
	{
		// accept큐가 없으므로 접속을 받지 않는다
		closesocket(socket);
	}
}

void AcceptorMonitorNode::PacketParsing( CPacket &packet, SOCKET socket )
{
	SP2Packet &kPacket = (SP2Packet&)packet;

	switch( packet.GetPacketID() )
	{
	case ITPK_ACCEPT_SESSION:
		OnAccept( kPacket, socket );
		break;
	}
}

void AcceptorMonitorNode::OnAccept( SP2Packet &packet, SOCKET socket )
{
	MonitoringNode *monitorNode = g_MonitoringNodeManager.CreateNode( socket );
	if( monitorNode )
	{
		g_MonitoringNodeManager.AddNode(monitorNode);
		if(!monitorNode->AfterCreate())
		{
			monitorNode->SessionClose();
		}
	}
}
