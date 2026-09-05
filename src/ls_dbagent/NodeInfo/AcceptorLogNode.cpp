#include "../stdafx.h"
#include "LogNode.h"
#include "LogNodeManager.h"
#include "../Network/iocpHandler.h"
#include "../Network/ioPacketQueue.h"
#include "../Network/SP2Packet.h"
#include "AcceptorLogNode.h"


AcceptorLogNode::AcceptorLogNode(void)
{
	Init();
}


AcceptorLogNode::~AcceptorLogNode(void)
{
	Destroy();
}

void AcceptorLogNode::Init()
{

}

void AcceptorLogNode::Destroy()
{

}

void AcceptorLogNode::ReceivePacket( CPacket &packet, SOCKET socket )
{
	if(!g_RecvQueue.InsertQueue( (DWORD)this, packet, socket ))
	{
		// accept큐가 없으므로 접속을 받지 않는다
		closesocket(socket);
	}
}

void AcceptorLogNode::PacketParsing( CPacket &packet, SOCKET socket )
{
	SP2Packet &kPacket = (SP2Packet&)packet;

	switch( packet.GetPacketID() )
	{
	case ITPK_ACCEPT_SESSION:
		OnAccept( kPacket, socket );
		break;

	}
}

void AcceptorLogNode::OnAccept( SP2Packet &packet, SOCKET socket )
{
	LogNode* logNode = g_LogNodeManager->CreateLogNode( socket );

	if( logNode )
	{
		g_iocp.AddHandleToIOCP((HANDLE)socket, (DWORD)logNode);		
		g_LogNodeManager->AddLogNode( logNode );

		if(!logNode->AfterCreate())
			logNode->SessionClose();

		logNode->GetPeerIP();
	}
	else
	{
		// 유저 노드가 없다. 소켓 종료
		SetSocketOpation(socket);
		closesocket( socket );
	}
}

void AcceptorLogNode::SetSocketOpation( SOCKET& socket )
{
	LINGER	solinger;
	solinger.l_linger = 0;
	solinger.l_onoff  = 1;
	setsockopt( socket, SOL_SOCKET, SO_LINGER, (char*)&solinger, sizeof(solinger) );
	::shutdown( socket, SD_BOTH );
}
