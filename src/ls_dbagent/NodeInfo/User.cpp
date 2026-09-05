// User.cpp: implementation of the User class.
//
//////////////////////////////////////////////////////////////////////

#include "../stdafx.h"
#include "../ThreadPool/ioThreadPool.h"
#include "../Network/DBServer.h"
#include "../Network/SP2Packet.h"
#include "../Network/ioPacketQueue.h"
#include "../Database/cQueryManager.h"
#include "UserNodeManager.h"
#include "User.h"



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

User::User( SOCKET s, DWORD dwSendBufSize, DWORD dwRecvBufSize ) : CConnectNode( s, dwSendBufSize, dwRecvBufSize )
{
	m_IP[0] = NULL;
	m_port	= 0;
}

User::~User()
{

}

void User::OnCreate()
{
	CConnectNode::OnCreate();
	//SOCKET으로 IP 얻는다.
	SOCKADDR_IN sockaddrClient;
	int sockaddrinSize=sizeof(sockaddrClient);
	::getpeername(m_socket,(LPSOCKADDR)&sockaddrClient,&sockaddrinSize);
	SetClientAddr(sockaddrClient);
}

void User::OnDestroy()
{
	CConnectNode::OnDestroy();
}

void User::GetPeerIP()
{
	CConnectNode::GetPeerIP( m_IP, sizeof(m_IP), m_port );
}

void User::SessionClose(BOOL safely)
{
	if(IsActive())
	{
		CPacket packet(ITPK_CLOSE_SESSION);
		ReceivePacket( packet );
	}
}

void User::ReceivePacket( CPacket &packet )
{
	// 쿼리는 패킷을 받으면 즉시 스레드로 넘김.
	switch( packet.GetPacketID() )
	{
	case DTPK_QUERY:
		if(IsActive())
		{
			g_ThreadPool.AttachQuery( this, (SP2Packet&)packet );
			//g_RecvQueue.InsertQueue((DWORD)this, packet, PK_QUEUE_SESSION);	
		}
		break;

	case DTPK_QUERY_PING :
		break;

	default:
		g_RecvQueue.InsertQueue((DWORD)this, packet, PK_QUEUE_SESSION);
		break;
	}
}

void User::PacketParsing( CPacket &packet )
{
	switch(packet.GetPacketID())
	{
	case DTPK_QUERY:
		OnQuery( (SP2Packet&)packet );
		break;

	case ITPK_CLOSE_SESSION:
		OnClose( (SP2Packet&)packet );
		break;

	default:
		{
			LOG.PrintTimeAndLog( 0, "Invalid packet(User) : 0x%x[%s:%d]", packet.GetPacketID(), GetIP(), GetPort() );
		}
		break;
	}
}

void User::OnQuery( SP2Packet &packet )
{
	//// 디버깅용 쿼리실행
	//QueryTable queryTable;
	//queryTable.SetUser( this );

	//if(!packet.Read( queryTable.m_QueryData )) 
	//{
	//	LOG.PrintTimeAndLog( 0, "Invalid query(packet) : 0x%x[%s:%d]", packet.GetPacketID(), GetIP(), GetPort() );
	//	return;
	//}

	//cExecuter* executer = g_queryManager.GetExecuter(GetCurrentThreadId());	
	//if(!executer) return;

	//cSerialize* storage = executer->GetStorage();
	//if(!storage) return;

	//CQueryResultData result;
	//executer->Execute( queryTable.GetUser(), &queryTable.m_QueryData, result );  

	//SP2Packet kPacket( DTPK_QUERY );
	//if(kPacket.Write(result))
	//{
	//	SendPacket( kPacket );
	//}
}

void User::OnClose( SP2Packet &packet )
{
	g_UserNodeManager.RemoveNode( this );
}

BOOL User::SendPacket( SP2Packet &packet )
{
	ThreadSync ts((SuperParent*)this);

	if( !SendMessage( packet, TRUE ) )
	{
		LOG.PrintTimeAndLog( 0, "Send failed to %s ", GetPublicIP() );
		return FALSE;
	}
	return TRUE;
}