#include "../stdafx.h"
#include "../MainProcess.h"
#include "../Network/GameServer.h"
#include "../Network/iocpHandler.h"
#include "../Network/ioPacketQueue.h"
#include "./ToonilandBillingServer.h"
#include "../Util/Ringbuffer.h"
#include "../channeling/iochannelingnodemanager.h"
#include "../channeling/iochannelingnodeTooniland.h"
#include "../database/logdbclient.h"
#include "../Local/ioLocalManager.h"
#include "../Local/ioLocalParent.h"

#include "../Network/ConnectAssist.h"


extern CLog LOG;

ToonilandBillingServer *ToonilandBillingServer::sg_Instance = NULL;
ToonilandBillingServer::ToonilandBillingServer( SOCKET s, DWORD dwSendBufSize, DWORD dwRecvBufSize ) : CConnectNode( s, dwSendBufSize, dwRecvBufSize )
{
	InitData();
}

ToonilandBillingServer::~ToonilandBillingServer()
{	
}

ToonilandBillingServer &ToonilandBillingServer::GetInstance()
{
	if(sg_Instance == NULL)
	{
		ioINILoader kLoader( "ls_config_billingsvr.ini" );
		kLoader.SetTitle( "ToonilandServer Buffer" );
		int iSendBufferSize = kLoader.LoadInt( "SendBufferSize", 16384 );
		int iRecvBufferSize = kLoader.LoadInt( "RecvBufferSize", 16384 );
		sg_Instance = new ToonilandBillingServer( INVALID_SOCKET, iSendBufferSize, iRecvBufferSize );
	}
	return *sg_Instance;
}

void ToonilandBillingServer::ReleaseInstance()
{		
	SAFEDELETE( sg_Instance );
}

bool ToonilandBillingServer::ConnectTo( bool bStart )
{
	ioINILoader kLoader( "ls_config_billingsvr.ini" );
	kLoader.SetTitle( "NETWORK" );

	kLoader.LoadString( "ToonilandServerIP", "", m_szServerIP, MAX_PATH );

	m_iSSPort = kLoader.LoadInt( "ToonilandServerPORT", 9000 );



	SOCKET socket = ::socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
	if( socket == INVALID_SOCKET )
	{
		LOG.PrintTimeAndLog( 0, "%s fail socket %d[%s:%d]", __FUNCTION__, GetLastError(), m_szServerIP, m_iSSPort );
		return false;
	}
	sockaddr_in serv_addr;
	serv_addr.sin_family		= AF_INET;
	serv_addr.sin_addr.s_addr	= inet_addr( m_szServerIP );
	serv_addr.sin_port			= htons( m_iSSPort );
 
 
	int retval = ::connect( socket, (sockaddr*)&serv_addr, sizeof(serv_addr) );
	if( retval != 0 ) 
	{
		DWORD dwError = GetLastError();
		if( dwError != WSAEWOULDBLOCK )
		{
			LOG.PrintTimeAndLog( 0, "[warning][tooniland]%s fail connect Errcode(%d)[%s:%d]", __FUNCTION__,dwError, m_szServerIP, m_iSSPort );
			bool reuse = true;
			::setsockopt(GetSocket(),SOL_SOCKET,SO_REUSEADDR,(TCHAR*)&reuse,sizeof(reuse));
			closesocket(socket);
			return false;
		}
	}
	// block
	CConnectNode::SetSocket( socket );

	return true;
}

void ToonilandBillingServer::InitData()
{
	m_dwCurrentTime = 0;				
	m_bSendAlive    = false;
	SetActive(false);
	ZeroMemory(m_szServerIP,MAX_PATH);
	m_iSSPort = 0;
}

void ToonilandBillingServer::OnCreate()
{
	CConnectNode::OnCreate();

	m_dwCurrentTime = TIMEGETTIME();
}

bool ToonilandBillingServer::AfterCreate()
{
	g_iocp.AddHandleToIOCP( (HANDLE)GetSocket(), (DWORD)this );
	LOG.PrintTimeAndLog( 0, "[info][tooniland]ToonilandBillingServerOnConnect (IP:%s PORT:%d RESULT:%d)", m_szServerIP, m_iSSPort, 0 );

	return CConnectNode::AfterCreate();
}

void ToonilandBillingServer::OnDestroy()
{
	CConnectNode::OnDestroy();

	LOG.PrintTimeAndLog( 0, "[warning][tooniland]Disconnect Tooniland Server!" );
}

bool ToonilandBillingServer::SendMessage( CPacket &rkPacket )
{
	if( m_socket == INVALID_SOCKET ) return false;

	CPacket SendPacket( rkPacket.GetBuffer(), rkPacket.GetBufferSize() );
	return CConnectNode::SendMessage( SendPacket.GetBuffer() + SendPacket.GetCurPos(), SendPacket.GetBufferSize() - SendPacket.GetCurPos(), TRUE );
}

bool ToonilandBillingServer::CheckNS( CPacket &rkPacket )
{
	return true;             //네트웍 감시 필요없다.
}

int ToonilandBillingServer::GetConnectType()
{
	return CONNECT_TYPE_TOONILAND_BILLING_SERVER;
}

void ToonilandBillingServer::ProcessTime()
{	
	if( g_App.IsWantExit() ) return;

	if( TIMEGETTIME() - m_dwCurrentTime > UPDATE_TIME )       
	{
		static int iCnt = 0;
		if( !IsActive() )
		{
			//ConnectTo( false );
		}
		else
		{	
			char szPacket[MAX_PATH]="";
			char szAgencyNo[ioLocalParent::MAX_AGENCY_NO_PLUS_ONE]="";

			ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
			if( pLocal )
			{
				pLocal->GetAgencyNo( szAgencyNo, sizeof( szAgencyNo ), true );
			}
			StringCbPrintf( szPacket, sizeof( szPacket ), "toon|onmedia|auth|%s|finalhyuk|WEMADE|LOSTSAGA|\n", szAgencyNo );
			m_sAliveCheckCPID = szAgencyNo;
			
			SP2Packet kPacket( BTNTPK_FIND_CASH_REQUEST );
			kPacket << szPacket;
			kPacket.SetBufferSizeMinusOne(); // 스트링 NULL 제외
			SendMessage( kPacket );
			
			if( m_bSendAlive )
			{
				LOG.PrintTimeAndLog( 0, "%s Tooniland Billing Server No Answer.", __FUNCTION__  );
				g_LogDBClient.OnInsertBillingServerError( CNT_TOONILAND, BILLING_ERROR_LOG_NO_ANSWER,"Billing server no answer." );
				//SessionClose();
			}
			m_bSendAlive = true;
		}
		
		m_dwCurrentTime = TIMEGETTIME();
	}
}

void ToonilandBillingServer::DispatchReceive(CPacket& packet, DWORD bytesTransferred)
{
	m_recvIO.AddBytesTransferred( bytesTransferred );

	while( m_recvIO.GetBytesTransferred() > 0 )
	{
		// 헤더가 없는 패킷에 헤더를 더한다. CConnectNode::DispatchReceive() 이부분만 다름.
		// bill|123456789|0-2611594-35600\n
		// auth|123456789|0-1000\n
		DWORD dwPacketSize = 0; 
		const char* buffer = m_recvIO.GetBuffer();
		for (int i = 0; i < (int)m_recvIO.GetBytesTransferred() ; i++)
		{
			if( buffer[i] == '\n' )
			{
				dwPacketSize = i+1;
				break;
			}
		}

		if( dwPacketSize == 0 )
			break;

		DWORD dwPacketID = 0;
		if( strncmp( m_recvIO.GetBuffer(), "auth", 4 ) == 0 )
		{
			dwPacketID = BTNTPK_FIND_CASH_RESULT;
		}
		else if( strncmp( m_recvIO.GetBuffer(), "bill", 4 ) == 0 )
		{
			dwPacketID = BTNTPK_BUY_CASH_RESULT;
		}
		else if( strncmp( m_recvIO.GetBuffer(), "cancel", 6 ) == 0 )
		{
			dwPacketID = BTNTPK_SUBSCRIPTION_RETRACT_CASH_RESULT;
		}

		SP2Packet RecvPacket( dwPacketID );
		RecvPacket.SetDataAdd( (char*)m_recvIO.GetBuffer(), min( dwPacketSize, m_recvIO.GetBytesTransferred() ), true );
		if( RecvPacket.IsValidPacket() == true && m_recvIO.GetBytesTransferred() >= dwPacketSize )
		{
			// TEST
			//char szLog[2000]="";
			//for (int i = 0; i < (int)m_RecvIO.m_dwBytesTransferred; i++)
			//{
			//	char szOneLog[MAX_PATH]="";
			//	StringCbPrintf( szOneLog, sizeof( szOneLog ), "%d|", m_RecvIO.m_aBuffer[i] );
			//	StringCbCat( szLog, sizeof( szLog ), szOneLog );
			//}
			//LOG.PrintTimeAndLog( 0, "%s [%d][%s]", __FUNCTION__, m_RecvIO.m_dwBytesTransferred, szLog );
			//
			if( !CheckNS( RecvPacket ) ) return;

			ReceivePacket( RecvPacket );

			m_recvIO.AfterReceive( dwPacketSize );
		}
		else 
			break;
	}

	WaitForPacketReceive();
}

void ToonilandBillingServer::ReceivePacket( CPacket &packet )
{
	g_RecvQueue.InsertQueue( (DWORD)this, packet, PK_QUEUE_SESSION );
}

void ToonilandBillingServer::PacketParsing( CPacket &packet )
{
	SP2Packet &kPacket = (SP2Packet&)packet;

	switch( kPacket.GetPacketID() )
	{
	case ITPK_CLOSE_SESSION:
		{
			OnClose(kPacket);
		}
		break;

	case BTNTPK_FIND_CASH_RESULT:
		OnFindCash( kPacket );
		break;
	case BTNTPK_BUY_CASH_RESULT:
		OnBuyCash( kPacket );
		break;
	case BTNTPK_SUBSCRIPTION_RETRACT_CASH_RESULT:
		OnSubscriptionRetractCash( kPacket );
		break;
	default:
		LOG.PrintTimeAndLog( 0, "ToonilandBillingServer::PacketParsing 알수없는 패킷 : 0x%x", kPacket.GetPacketID() );
		break;
	}
}

void ToonilandBillingServer::OnFindCash( SP2Packet &rkPacket )
{
	ioHashString sResult;
	rkPacket >> sResult;

	ioChannelingNodeTooniland *pNode = static_cast<ioChannelingNodeTooniland*> ( g_ChannelingMgr.GetNode( CNT_TOONILAND ) );
	if( pNode )
		pNode->OnRecieveGetCash( sResult );
}

void ToonilandBillingServer::OnBuyCash( SP2Packet &rkPacket )
{
	ioHashString sResult;
	rkPacket >> sResult;

	ioChannelingNodeTooniland *pNode = static_cast<ioChannelingNodeTooniland*> ( g_ChannelingMgr.GetNode( CNT_TOONILAND ) );
	if( pNode )
		pNode->OnRecieveOutputCash( sResult );
}

void ToonilandBillingServer::OnSubscriptionRetractCash( SP2Packet &rkPacket )
{
	ioHashString sResult;
	rkPacket >> sResult;

	ioChannelingNodeTooniland *pNode = static_cast<ioChannelingNodeTooniland*> ( g_ChannelingMgr.GetNode( CNT_TOONILAND ) );
	if( pNode )
		pNode->OnReceiveRetractCash( sResult );

	LOG.PrintTimeAndLog( 0, "%s Result %s", __FUNCTION__, sResult.c_str() );
}

void ToonilandBillingServer::SessionClose( BOOL safely/*=TRUE */ )
{
	CPacket packet(ITPK_CLOSE_SESSION);
	ReceivePacket( packet );
}

void ToonilandBillingServer::OnClose( SP2Packet &rkPacket )
{ 
	if(IsActive())
	{
		OnDestroy();
		g_ConnectAssist.PushNode(this);
	}
}