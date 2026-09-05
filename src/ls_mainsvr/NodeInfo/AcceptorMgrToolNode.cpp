#include "../stdafx.h"
#include "MgrToolNode.h"
#include "MgrToolNodeManager.h"
#include "../Network/iocpHandler.h"
#include "../Network/ioPacketQueue.h"
#include "../Network/SP2Packet.h"
#include "AcceptorMgrToolNode.h"

AcceptorMgrToolNode::AcceptorMgrToolNode(void)
{
	Init();
}

AcceptorMgrToolNode::~AcceptorMgrToolNode(void)
{
	Destroy();
}

void AcceptorMgrToolNode::Init()
{
}

void AcceptorMgrToolNode::Destroy()
{
}

void AcceptorMgrToolNode::ReceivePacket( CPacket &packet, SOCKET socket )
{
	if(!g_RecvQueue.InsertQueue( (DWORD)this, packet, socket ))
	{
		// accept큐가 없으므로 접속을 받지 않는다
		closesocket(socket);
	}
}

void AcceptorMgrToolNode::PacketParsing( CPacket &packet, SOCKET socket )
{
	SP2Packet &kPacket = (SP2Packet&)packet;

	switch( packet.GetPacketID() )
	{
	case ITPK_ACCEPT_SESSION:
		OnAccept( kPacket, socket );
		break;
	}
}

void AcceptorMgrToolNode::OnAccept( SP2Packet &packet, SOCKET socket )
{
	MgrToolNode *newNode = g_MgrTool.CreateMgrToolNode( socket );
	if( newNode )
	{
		g_iocp.AddHandleToIOCP((HANDLE)socket, (DWORD)newNode);
		g_MgrTool.AddNode( newNode );
		if(!newNode->AfterCreate())
		{
			newNode->SessionClose();
		}
	}
}