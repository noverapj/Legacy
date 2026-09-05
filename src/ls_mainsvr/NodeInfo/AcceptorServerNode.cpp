#include "../stdafx.h"
#include "ServerNode.h"
#include "ServerNodeManager.h"
#include "../Network/iocpHandler.h"
#include "../Network/ioPacketQueue.h"
#include "../Network/SP2Packet.h"
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
	ServerNode *serverNode = g_ServerNodeManager.CreateServerNode( socket );
	if( serverNode )
	{
		g_ServerNodeManager.AddServerNode( serverNode );
		g_iocp.AddHandleToIOCP((HANDLE)socket,(DWORD)serverNode);
		if(!serverNode->AfterCreate())
		{
			serverNode->SessionClose();
		}

		Information( "-Gameserver Accepted\n" );
	}
}