#include "StdAfx.h"
#include "AcceptorMgr.h"


AcceptorMgr::AcceptorMgr(void)
{
}


AcceptorMgr::~AcceptorMgr(void)
{
}

void AcceptorMgr::Init()
{
}

void AcceptorMgr::Destory()
{ 
}

void AcceptorMgr::ReceivePacket( CPacket &packet, SOCKET socket )
{
	if(!g_Queue()->InsertQueue( (DWORD)this, packet, socket))
	{
		closesocket(socket);
	}
}

void AcceptorMgr::PacketParsing( CPacket &packet, SOCKET socket )
{
	SP2Packet &kPacket = (SP2Packet&)packet;
	switch( packet.GetPacketID() )
	{
	case EPROTOCOL::ITPK_ACCEPT_SESSION:
		OnAccept( kPacket, socket );
		break;
	}
}

void AcceptorMgr::OnAccept( SP2Packet &packet, SOCKET socket )
{
	g_ClientMgr()->CreateClientNode(socket);
	//Information(_T("OnAccept\r\n"));
}