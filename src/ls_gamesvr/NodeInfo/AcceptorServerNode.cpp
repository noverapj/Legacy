#include "stdafx.h"
//#include "../Window.h"
#include "../MainProcess.h"
#include "ServerNode.h"
#include "ServerNodeManager.h"
#include "../Network/ioPacketQueue.h"
#include "AcceptorServerNode.h"

AcceptorServerNode::AcceptorServerNode(void)
{
	Init();
}

AcceptorServerNode::~AcceptorServerNode(void)
{
	Destroy();
}

void AcceptorServerNode::Init()
{
}

void AcceptorServerNode::Destroy()
{
}

void AcceptorServerNode::ReceivePacket( CPacket &packet, SOCKET socket )
{
	if(!g_RecvQueue.InsertQueue( (DWORD)this, packet, socket ))
	{
		// accept큐가 없으므로 접속을 받지 않는다
		closesocket(socket);
	}
}

void AcceptorServerNode::PacketParsing( CPacket &packet, SOCKET socket )
{
	SP2Packet &kPacket = (SP2Packet&)packet;

	switch( packet.GetPacketID() )
	{
	case ITPK_ACCEPT_SESSION:
		OnAccept( kPacket, socket );
		break;
	}
}

void AcceptorServerNode::OnAccept( SP2Packet &packet, SOCKET socket )
{
	FUNCTION_TIME_CHECKER( 100000.0f, 0 );          // 0.1 초 이상 걸리면로그 남김

	ServerNode *serverNode = g_ServerNodeManager.CreateServerNode( socket );
	if( serverNode )
	{
		g_ServerNodeManager.AddServerNode( serverNode );
		if(!serverNode->AfterCreate())
		{
			serverNode->SessionClose();
		}
	}
	else 
	{
		closesocket(socket);
	}
}