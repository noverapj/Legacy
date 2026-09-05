#include "StdAfx.h"
#include "ClientNode.h"


ClientNode::ClientNode( SOCKET s, DWORD dwSendBufSize, DWORD dwRecvBufSize ) : CConnectNode(s, dwSendBufSize,dwRecvBufSize)
{
	if(g_Config()->UseSecurity() == 1) //kyg 다시 살릴것 
 		SetNS(new ioServerSecurity);
}
 
ClientNode::ClientNode() : CConnectNode(INVALID_SOCKET,DEFAULT_BUFFER,DEFAULT_BUFFER*2+1) 
{
	if(g_Config()->UseSecurity() == 1)
		SetNS(new ioServerSecurity);
}

ClientNode::~ClientNode(void)
{
}

void ClientNode::OnCreate()
{
	CConnectNode::OnCreate();

	m_nodeType = 0;
	m_currentTime = std::clock();
	IsGhost(ECONNECTSTATE::LS_GHOST);
	m_waitNumber = 0;

	if( g_Config()->UseSecurity() )
	{		
		ioServerSecurity *pSS = (ioServerSecurity *)m_pNS;
		if( pSS ) 
		{
			pSS->InitDoSAttack( 30 );
			pSS->InitState( m_socket );		
		}
	}
}

void ClientNode::OnDestroy()
{ 
	CConnectNode::OnDestroy();
}

bool ClientNode::CheckNS( CPacket &rkPacket )
{
	if( m_pNS == NULL ) return true;

	ioServerSecurity *pSS = reinterpret_cast<ioServerSecurity*>(m_pNS);
	if( !pSS->IsCheckSum( rkPacket ) )
	{
 		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "User::CheckNS Check Sum Fail!! [%s : 0x%x]",
  			"EE", rkPacket.GetPacketID() );

		ExceptionClose( 0 );

		return false;
	}

	if( !pSS->CheckState( rkPacket ) )
	{
  		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "User::CheckNS State Not Same Client:%d, Server:%d [%s : 0x%x]", 
  			rkPacket.GetState(), pSS->GetRcvState(), "EE", rkPacket.GetPacketID() );

		ExceptionClose( 0 );

		return false;
	}

	if( !pSS->UpdateReceiveCount() )
 	{
  		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "User::CheckNS ONE SEC MANY PACKET(%d)!! [%s : 0x%x]", 
  			pSS->GetRcvCount(), "EE", rkPacket.GetPacketID() );

		ExceptionClose( 0 );

		return false;
	}

	return true;
}

void ClientNode::SessionClose( BOOL safely )
{
	if(IsActive())
	{
		CPacket packet( EPROTOCOL::ITPK_CLOSE_SESSION );
		ReceivePacket( packet );
	}

}

bool ClientNode::SendMessage( CPacket &rkPacket )
{
	return CConnectNode::SendMessage(rkPacket);
}

void ClientNode::ReceivePacket( CPacket &packet )
{
	if(g_Queue()->InsertQueue( (DWORD)this, packet, (PacketQueueTypes)PK_QUEUE_SESSION ) == false)
	{
		g_State()->PrintLowTime();
	}
}

void ClientNode::PacketParsing( CPacket &packet )
{
 	SP2Packet &kPacket = (SP2Packet&)packet;
	FUNCTION_TIME_CHECKER( 100000.0f, kPacket.GetPacketID() );  

	switch( kPacket.GetPacketID() )
	{
	case EPROTOCOL::ITPK_CLOSE_SESSION:
		{
			OnClose();
		}
		break;

	case EPROTOCOL::LSTPK_CONNECT_CLIENT:
		{
			OnClient(kPacket);
		}
		break;

	case EPROTOCOL::LSPTK_TICKET_REQUEST:
		{
			OnResponseTicketInfo();
		}
		break;
		
	case EPROTOCOL::LSPTK_CONNECT_MONITOR:
		{
			OnMonitor(kPacket);
		}
		break;

	case EPROTOCOL::LSPTK_CONTROL_SERVER:
		{
			OnControlServer(kPacket);
		}
		break;

	case EPROTOCOL::LSPTK_WHITELIST_REQUEST:
		{
			OnWhiteList(kPacket);
		}
		break;

#if 0 
	case EPROTOCOL::ITPK_OPERATIONTYPE+1:
		{
			//printf("%d ",g_State()->TestCount());
			IsGhost(ECONNECTSTATE::LS_USERCONNECTED);
			SetConnectType(ENODETYPE::MONITOR);
			SP2Packet ack( EPROTOCOL::ITPK_OPERATIONTYPE+1);
			//CConnectNode::SendMessage(ack,TRUE);
			g_State()->IncrementTestCount();
			
			if(g_State()->TestCount() >= 100000)
			{
				g_State()->PrintTime();
			}
		}
		break;
#endif
	}
}
 
void ClientNode::OnClient( SP2Packet & kPacket )
{
	if(RestrictIP())
	{
		Debug(_T("- Restricted IP\r\n"));
		SendErrorCode(EERRORCODE::LS_NOSERVER);
		return;
	}

	AVGPacket(); //kyg 

	OnClient_ stdata;

	kPacket >> stdata;

	IsGhost(ECONNECTSTATE::LS_USERCONNECTED);
	SetConnectType(ENODETYPE::USER);

	if(!g_Config()->Allblock())
	{
		if(g_UserInfoMgr()->GetSize() < g_Config()->UserInfoMax())
		{
			g_UserInfoMgr()->AddUser(GetSocket(),stdata.userid);

			TrySendServerInfo_ streq;

			streq.opid = EOPID::TRYSENDSERVERINFO;

			PUTQFUNNC(streq,g_Queue());
		}
		else
		{
			SendErrorCode(EERRORCODE::LS_WAITLISTMAX);

			IsGhost(ECONNECTSTATE::LS_GHOST);
		}
	}
	else
	{
		IsGhost(ECONNECTSTATE::LS_ALLBLOCK);

		SendErrorCode(EERRORCODE::LS_SERVERALLBLOCK);
	}
}

void ClientNode::OnClose()
{
	g_UserInfoMgr()->DelUser(GetSocket());

	g_ClientMgr()->DelClient(this);

	OnDestroy();
}

void ClientNode::OnMonitor( SP2Packet& kPacket )
{
	IsGhost(ECONNECTSTATE::LS_USERCONNECTED);
	SetConnectType(ENODETYPE::MONITOR);
}

void ClientNode::OnControlServer( SP2Packet& kPacket )
{
	if(GetConnectType() == ENODETYPE::MONITOR)
	{
		int type;
		kPacket >> type;
		switch(type)
		{

		case EMCONTROLTYPE::LS_GETSERVERINFO:
			{
				OnGetServerInfo();
			}
			break;
		case EMCONTROLTYPE::LS_FILLSERVERINFO:
			{
				OnFillServerInfo();
			}
			break;
		case EMCONTROLTYPE::LS_SETALLSERVERBLOCK:
			{
				OnSetAllServerBlock(kPacket, type);
			}
			break;
		case EMCONTROLTYPE::LS_SETSERVERBLOCK:
			{
				OnSetServerBlock(kPacket, type);
			}
			break;
		case EMCONTROLTYPE::LS_FILLINFODRAW:
			{
				OnFillDraw(type);
			}
			break;
		case EMCONTROLTYPE::LS_QUICKEXIT:
			{
				LOG.PrintTimeAndLog(0,"Monitor::QuickExit Request");
				S_IO::instance()->stop();
			}
			break;
		default:
			LOG.PrintTimeAndLog(0,"[SVRCONTROL]Error Unknown controltype:(%d)",type);
			SendErrorCode(EERRORCODE::LS_SERVERALLBLOCK);
		}
	}
}

void ClientNode::OnResponseTicketInfo()
{
	if(g_ServerInfoMgr()->GetSize() != 0)
	{
		SP2Packet pk(EPROTOCOL::LSPTK_TICKET_RESPONSE);
		int ticket = g_UserInfoMgr()->GetTiketInfo(GetSocket());
		pk << ticket;
		SendMessage(pk);
		return;
	}

	//kyg 이때 서버 산거없다고 에러코드 전송 
	SendErrorCode(EERRORCODE::LS_NOSERVER);
}

void ClientNode::OnFillDraw( int type )
{
	int now = std::clock();
	int ntime = now - g_State()->Now();
	ntime = ntime / 1000;

	SP2Packet pk(EPROTOCOL::LSPTK_SERVER_RESPONSE);
	pk << type;

	LoginServerInfo_ stdata;
	stdata.acceptCount				= g_State()->AcceptCount();
	stdata.acceptCountpersec		= stdata.acceptCount / ntime;
	stdata.clientPoolCount			= g_ClientMgr()->GetMemPoolSize();
	stdata.closeCount				= g_State()->CloseCount();
	stdata.opPoolCount				= g_OPMemPool()->GetSize();
	stdata.packetQueueCount			= g_Queue()->GetSize();
	stdata.port						= g_Config()->NPort();
	stdata.serverConnectorPoolCount = g_ConnectAssist()->GetMemPoolSize();
	stdata.serverConnectCount		= g_ServerInfoMgr()->GetSize();
	stdata.userinfoCount			= g_UserInfoMgr()->GetSize();
	strcpy_s(stdata.ipAddr,g_Config()->GetIP());

	pk << stdata;
	pk << g_ServerInfoMgr()->GetSize();

	g_ServerInfoMgr()->MakeServerInfo(pk);

	SendMessage(pk);
}

void ClientNode::OnSetServerBlock( SP2Packet& kPacket, int type )
{
	int serverIndex = 0, gameIndex = 0;
	int blockState = 0;

	kPacket >> serverIndex;
	kPacket >> blockState;

	if(-1 == serverIndex)
	{
		// 게임서버들에게 게임서버가 블록 됨을 알림
		SP2Packet req(EPROTOCOL::LSPTK_BLOCK_REQUEST);

		req << 0;
		req << blockState;

		g_ServerConnectMgr()->Broadcast(req);
	}
	else
	{
		// 로그인서버의 게임서버인덱스를 실제 게임서버로 매치
		g_ServerInfoMgr()->TranslateIndex(serverIndex, gameIndex);

		// 게임서버들에게 게임서버가 블록 됨을 알림
		SP2Packet req(EPROTOCOL::LSPTK_BLOCK_REQUEST);

		req << gameIndex;
		req << blockState;

		g_ServerConnectMgr()->Broadcast(req);
	}

	// 모니터링 툴에 응답
	SP2Packet pk(EPROTOCOL::LSPTK_SERVER_RESPONSE);

	pk << type;
	pk << serverIndex;
	pk << blockState;

	SendMessage(pk);

	LOG.PrintTimeAndLog(0, "[SVRCONTROL]serverId:%d BlockeState(%d)", serverIndex, blockState);
}

void ClientNode::OnSetAllServerBlock( SP2Packet& kPacket, int type )
{
	SP2Packet pk(EPROTOCOL::LSPTK_SERVER_RESPONSE); 
	int blockstate;

	pk << type;
	
	kPacket >> blockstate;

	if(blockstate >= 2)
	{
		LOG.PrintTimeAndLog(0,"[SVRCONTROL]Error SetServerblock value(%d)",blockstate);
		return;
	}

	LOG.PrintTimeAndLog(0,"[SVRCONTROL]All Server Blockstate :%d",blockstate);

	g_Config()->Allblock(blockstate);

	pk << g_Config()->GetIP();
	pk << blockstate;

	SendMessage(pk);
}

void ClientNode::OnGetServerInfo()
{
	SP2Packet pk(EPROTOCOL::LSPTK_SERVER_RESPONSE);
	int type = EMCONTROLTYPE::LS_GETSERVERINFO;

	pk << type;
	int maxuser = g_Config()->ServerFullCount();
	int maxserver = g_Config()->NGameServerMax();
	int waituser = g_UserInfoMgr()->GetSize();
	int maxwait = g_Config()->UserInfoMax();
	int allblock = g_Config()->Allblock();

	pk << maxuser;
	pk << maxserver;
	pk << waituser;
	pk << maxwait;
	pk << allblock;

	pk << g_Config()->GetIP();

	VIPADDR& serverAddrs = g_Config()->ServerAddr();

	for(uint32 i=0; i<serverAddrs.size();++i)
	{
		SVRCONNECTINFO_& svrAddr = serverAddrs[i];

		pk << svrAddr.serverIndex;
		pk << svrAddr.serverName;
		pk << 0; // not use
		pk << svrAddr.ipAddr;
		pk << svrAddr.port;
	}

	SendMessage(pk);
}

void ClientNode::OnFillServerInfo()
{
	SP2Packet pk(EPROTOCOL::LSPTK_SERVER_RESPONSE);
	int type		= EMCONTROLTYPE::LS_FILLSERVERINFO;
	int waituser	= g_UserInfoMgr()->GetSize();
	int maxuser		= g_Config()->ServerFullCount();
	int maxserver	= g_ServerInfoMgr()->GetSize();
	int maxwait		= g_Config()->UserInfoMax();

	pk << type;
	pk << maxuser;
	pk << maxserver;
	pk << waituser;
	pk << maxwait;
	pk << g_Config()->GetIP();

	g_ServerInfoMgr()->MakeServerInfo(pk);

	SendMessage(pk);
}

void ClientNode::OnWhiteList(SP2Packet& kPacket)
{
	BOOL bWhiteListOn = FALSE;
	kPacket >> bWhiteListOn;

	g_Config()->SetWhiteList(bWhiteListOn);
}

void ClientNode::SendErrorCode( int errcode )
{
	if(IsActive())
	{
		SP2Packet sp(EPROTOCOL::LSPTK_ERROR);

		sp << errcode;

		SendMessage(sp);
	}
}

BOOL ClientNode::RestrictIP()
{
	int port;
	char IP[64];
	GetPeerIP(IP, sizeof(IP), port);

	Debug(_T("OnClient - %s:%d\r\n"), IP, port);

	if(!g_Config()->CheckWhiteList(IP)) return TRUE;

	if(g_Config()->CheckBlackList(IP)) return TRUE;

	Debug(_T("OnClient RestrictIP- %s:%d\r\n"), IP, port);

	return FALSE;
}

void ClientNode::AVGPacket()
{
	if(g_State()->Now() == 0)
	{
		g_State()->Now(std::clock());
		g_State()->OnClientCount(0);
	}

	g_State()->OnClientCount(g_State()->OnClientCount() + 1);
}