#include "stdafx.h"
#include "GameSvrUDPNode.h"
#include "../NodeInfo/UserNodeManager.h"
#include "../MainProcess.h"
#include "../EtcHelpFunc.h"
#include "SP2Packet.h"
#include "../Network/ioPacketQueue.h"
#include "../Network/GameServer.h"

GameSvrUDPNode::GameSvrUDPNode(void)
{
	SetActive( true );
	SetNS(new ioUDPSecurity);
	m_packet = new SP2Packet;
	m_inputCount = 0;
	m_processCount = 0;
	m_sendPacketCount = 0;
	m_maxSize = 0;
}


GameSvrUDPNode::~GameSvrUDPNode(void)
{
}

int GameSvrUDPNode::GetConnectType()
{
	return CONNECT_TYPE_UDP;
}

void GameSvrUDPNode::SessionClose( int index )
{
	if(IsActive())
	{
 
	}
}

void GameSvrUDPNode::PacketParsing( CPacket &packet )
{
	SP2Packet &kPacket = (SP2Packet&)packet;
	FUNCTION_TIME_CHECKER( 100000.0f, kPacket.GetPacketID() );  
	sockaddr_in addr;
	IncrementProcessCount();
	kPacket.GetSockAddress(addr);
	g_App.ProcessUDPPacket(&addr,kPacket ); // 기존 코드를 최대한 수정 안하기위해 
	 
}
 
void GameSvrUDPNode::ReceivePacket( CPacket &packet, int index )
{
	//char rcv_ip[16] = "";
	//int port = 0;

	SetMaxSize(packet.GetBufferSize());

	//port = MakeIpAddres(rcv_ip, m_recvInfos[index], port);

 	g_ProcessChecker.BroadcastThreadCheckTimeStart(); 
 	g_ProcessChecker.CallUDPThread();		 
	g_ProcessChecker.UDPRecvMessage( packet.GetBufferSize() ); 

//	g_ProcessChecker.ServerPacketProcess(packet.GetPacketID());

	SendPacket(m_recvInfos[index], packet);
}

int GameSvrUDPNode::MakeIpAddres( char* rcv_ip, UDPIoInfo* recvInfo, int& port )
{
	sprintf_s(rcv_ip, 16,"%d.%d.%d.%d",
		recvInfo->addr.sin_addr.s_net,
		recvInfo->addr.sin_addr.s_host, 
		recvInfo->addr.sin_addr.s_lh,
		recvInfo->addr.sin_addr.s_impno );
	port = ntohs(recvInfo->addr.sin_port );	return port;
}

void GameSvrUDPNode::SendPacket( UDPIoInfo* recvinfo, CPacket &packet )
{
	SP2Packet& rPacket = static_cast<SP2Packet&>(packet);

	rPacket.SetPosBegin();

	rPacket << recvinfo->addr;

	if( !Help::IsOnlyServerRelay() ) //릴레이 서버 옵션이 켜있음 PacketParsing 쪽으로 들어감 
	{
		g_RecvQueue.InsertQueue( (DWORD)this, rPacket, PK_QUEUE_UDP );
		return;
	}
	else if( !COMPARE( packet.GetPacketID(), CUPK_CONNECT, 0x5000 ) ) //컨넥션 부터 ~ 5000까지 패킷이 아니면 일단 PacketParsing 쪽으로 넘ㄱ미 
	{
		g_RecvQueue.InsertQueue( (DWORD)this, rPacket, PK_QUEUE_UDP );
	}
	else
	{
		switch( packet.GetPacketID() ) // Ping 이나 UDP동기화 타임, ReserveRoom등일때 다시 PacketParsing 으로 넘김 
		{
		case CUPK_CONNECT:                   // 서버가 알아야 할 내용.
		case CUPK_SYNCTIME:
		case CUPK_RESERVE_ROOM_JOIN:
		case CUPK_CHECK_KING_PING:
#ifdef FLAG_MODE_BY_BCKIM
		case CUPK_CHECK_FLAG_PING: 	// 2018-03-15 by bckim, 깃발모드 추가
#endif // FLAG_MODE_BY_BCKIM		
			{ 
				g_RecvQueue.InsertQueue( (DWORD)this, rPacket, PK_QUEUE_UDP );
			}
			break;
		default:							  
			{
				sockaddr_in addr;  
				rPacket.GetSockAddress(addr);
				
				if(!g_Relay.PushRelayPacket(recvinfo->addr, rPacket ))  
				{
				//	IncremnetContextpoolCount();
					//g_RecvQueue.InsertQueue( (DWORD)this, rPacket, PK_QUEUE_UDP );
				}
			}
			break;
		}
	}
}

void GameSvrUDPNode::SendLog( CPacket& packet )
{	
	if(!g_App.GetLogServerIP().IsEmpty())
	{
		SendMessage(g_App.GetLogServerIP().c_str(),g_App.GetLogServerPort(),packet );
	}
}

void GameSvrUDPNode::MakeLogicPacket( SP2Packet& packet, sockaddr_in& addr)
{
	packet << addr;
}

BOOL GameSvrUDPNode::SetNetworkSecurity(int i)
{
	SetNS(new ioUDPSecurity,i);
	return TRUE;
}
