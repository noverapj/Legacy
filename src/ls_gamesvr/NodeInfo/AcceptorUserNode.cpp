#include "stdafx.h"
//#include "../Window.h"
#include "../MainProcess.h"
#include "User.h"
#include "UserNodeManager.h"
#include "../Network/iocpHandler.h"
#include "../Network/ioPacketQueue.h"
#include "AcceptorUserNode.h"

AcceptorUserNode::AcceptorUserNode(void)
{
	Init();
}

AcceptorUserNode::~AcceptorUserNode(void)
{
	Destroy();
}

void AcceptorUserNode::Init()
{
}

void AcceptorUserNode::Destroy()
{
}

void AcceptorUserNode::ReceivePacket( CPacket &packet, SOCKET socket )
{
	if(!g_RecvQueue.InsertQueue( (DWORD)this, packet, socket ))
	{
		// accept큐가 없으므로 접속을 받지 않는다
		closesocket(socket);
	}
}

void AcceptorUserNode::PacketParsing( CPacket &packet, SOCKET socket )
{
	SP2Packet &kPacket = (SP2Packet&)packet;

	switch( packet.GetPacketID() )
	{
	case ITPK_ACCEPT_SESSION:
		OnAccept( kPacket, socket );
		break;
	}
}

void AcceptorUserNode::OnAccept( SP2Packet &packet, SOCKET socket )
{
	FUNCTION_TIME_CHECKER( 100000.0f, 0 ); // 0.1 초 이상 걸리면로그 남김

	User* userNode = g_UserNodeManager.CreateNewNode( socket );
	if( userNode )
	{
		g_iocp.AddHandleToIOCP((HANDLE)socket, (DWORD)userNode);		
		g_UserNodeManager.AddUserNode( userNode );
		if(!userNode->AfterCreate())
		{
			userNode->SessionClose();
		}
	}
	else
	{
		// 유저 노드가 없다. 소켓 종료
		LINGER	solinger;
		solinger.l_linger = 0;
		solinger.l_onoff  = 1;
		setsockopt( socket, SOL_SOCKET, SO_LINGER, (char*)&solinger, sizeof(solinger) );
		::shutdown( socket, SD_BOTH );
		closesocket( socket );
	}
}