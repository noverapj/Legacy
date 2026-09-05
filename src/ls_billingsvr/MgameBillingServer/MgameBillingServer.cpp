#include "../stdafx.h"
#include "../MainProcess.h"
#include "../Network/GameServer.h"
#include "../Network/iocpHandler.h"
#include "../Network/ioPacketQueue.h"
#include "./mgamebillingserver.h"
#include "../Util/Ringbuffer.h"
#include "../channeling/iochannelingnodemanager.h"
#include "../channeling/iochannelingnodemgame.h"
#include "../database/logdbclient.h"
#include "../Local/ioLocalManager.h"
#include "../Network/ConnectAssist.h"

extern CLog LOG;

MgameBillingServer *MgameBillingServer::sg_Instance = NULL;
MgameBillingServer::MgameBillingServer( SOCKET s, DWORD dwSendBufSize, DWORD dwRecvBufSize ) : CConnectNode( s, dwSendBufSize, dwRecvBufSize )
{
	InitData();
	SetActive(false);
	ZeroMemory(m_szServerIP,MAX_PATH);
}

MgameBillingServer::~MgameBillingServer()
{	
}

MgameBillingServer &MgameBillingServer::GetInstance()
{
	if(sg_Instance == NULL)
	{
		ioINILoader kLoader( "ls_config_billingsvr.ini" );
		kLoader.SetTitle( "MgameServer Buffer" );
		int iSendBufferSize = kLoader.LoadInt( "SendBufferSize", 16384 );
		int iRecvBufferSize = kLoader.LoadInt( "RecvBufferSize", 16384 );
		sg_Instance = new MgameBillingServer( INVALID_SOCKET, iSendBufferSize, iRecvBufferSize );
	}
	return *sg_Instance;
}

void MgameBillingServer::ReleaseInstance()
{		
	SAFEDELETE( sg_Instance );
}

bool MgameBillingServer::ConnectTo( bool bStart )
{
	ioINILoader kLoader( "ls_config_billingsvr.ini" );
	kLoader.SetTitle( "NETWORK" );

	
	kLoader.LoadString( "MgameServerIP", "", m_szServerIP, MAX_PATH );

	m_iSSPort = kLoader.LoadInt( "MgameServerPORT", 9000 );


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
			LOG.PrintTimeAndLog( 0, "[warning][mgame]%s fail connect Errcode(%d)[%s:%d]", __FUNCTION__,dwError, m_szServerIP, m_iSSPort );
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

void MgameBillingServer::InitData()
{
	m_dwCurrentTime = 0;
	m_bSendAlive    = false;

	m_iSSPort = 0;
}

void MgameBillingServer::OnCreate()
{
	CConnectNode::OnCreate();
	

	m_dwCurrentTime = TIMEGETTIME();
}

bool MgameBillingServer::AfterCreate()
{
	g_iocp.AddHandleToIOCP( (HANDLE)GetSocket(), (DWORD)this );
	LOG.PrintTimeAndLog( 0, "[info][mgame]MgameBillingServerOnConnect (IP:%s PORT:%d RESULT:%d)", m_szServerIP, m_iSSPort, 0 );

	return CConnectNode::AfterCreate();
}
void MgameBillingServer::OnDestroy()
{
	CConnectNode::OnDestroy();

	LOG.PrintTimeAndLog( 0, "[warning][mgame]Disconnect Mgame Server!" );
}

bool MgameBillingServer::SendMessage( CPacket &rkPacket )
{
	if( m_socket == INVALID_SOCKET ) return false; //kyg 헤더 삭제하는 코드가 있었음 

	CPacket SendPacket( rkPacket.GetBuffer(), rkPacket.GetBufferSize() );
	return CConnectNode::SendMessage( SendPacket.GetBuffer() + SendPacket.GetCurPos(), SendPacket.GetBufferSize() - SendPacket.GetCurPos(), TRUE );
}

bool MgameBillingServer::CheckNS( CPacket &rkPacket )
{
	return true;             //네트웍 감시 필요없다.
}

int MgameBillingServer::GetConnectType()
{
	return CONNECT_TYPE_MGAME_BILLING_SERVER;
}

void MgameBillingServer::ProcessTime()
{	
	if( g_App.IsWantExit() ) return;

	if( TIMEGETTIME() - m_dwCurrentTime > UPDATE_TIME )       
	{
		static int iCnt = 0;
		if( !IsActive() )
		{
		//	ConnectTo( false );
		}
		else
		{	
			MgameCashRequest kInfo;
			kInfo.SetInfo( CASH_ACTION_REQUEST_CHECK, "", 0, "", 0, "", "", false );
			SP2Packet kPacket( BMTPK_CASH_ACTION_REQUEST );
			kPacket << kInfo;
			SendMessage( kPacket );
			
			if( m_bSendAlive )
			{
				LOG.PrintTimeAndLog( 0, "[info][mgame]%s Mgame Billing Server No Answer.", __FUNCTION__  );
				g_LogDBClient.OnInsertBillingServerError( CNT_MGAME, BILLING_ERROR_LOG_NO_ANSWER,"Billing server no answer." );
				//SessionClose();
			}
			m_bSendAlive = true;
		}
		
		m_dwCurrentTime = TIMEGETTIME();
	}
}

void MgameBillingServer::DispatchReceive( CPacket& packet, DWORD bytesTransferred )
{
	m_recvIO.AddBytesTransferred( bytesTransferred );

	while( m_recvIO.GetBytesTransferred() > 0 ) //kyg mgame 빌링서버에서 들어온 정보가 신뢰 할 수 있는 정보인지를 로직에서 확인함.. 좀더 위엣단에서 할수 있는 방법 강구.. 
	{
		// 헤더가 없는 패킷에 헤더를 더한다. CConnectNode::DispatchReceive() 이부분만 다름.
		DWORD dwPacketSize = sizeof( MgameCashResult );
		SP2Packet RecvPacket( BMTPK_CASH_ACTION_RESULT );
		RecvPacket.SetDataAdd( (char*)m_recvIO.GetBuffer(), m_recvIO.GetBytesTransferred(), true ); //kyg 위험코드 빌링이 바쁠때 TCP 데이터가 켭쳐서 들어온다면 제대로 처리가 안될수있음 
		// TEST
		//LOG.PrintTimeAndLog( 0, "%s [%d]", __FUNCTION__, m_RecvIO.m_dwBytesTransferred );
		//
		if( RecvPacket.IsValidPacket() == true && m_recvIO.GetBytesTransferred() >= dwPacketSize )
		{
			// TEST
			//char szLog[MAX_PATH]="";
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

void MgameBillingServer::ReceivePacket( CPacket &packet )
{
	g_RecvQueue.InsertQueue( (DWORD)this, packet, PK_QUEUE_SESSION );
}

void MgameBillingServer::PacketParsing( CPacket &packet )
{
	SP2Packet &kPacket = (SP2Packet&)packet;

	switch( kPacket.GetPacketID() )
	{
	case MNSTPK_CLOSE:
		{
			OnClose(kPacket);
		}
		break;
	case BMTPK_CASH_ACTION_RESULT:
		OnCashAction( kPacket );
		break;
	default:
		LOG.PrintTimeAndLog( 0, "MgameBillingServer::PacketParsing 알수없는 패킷 : 0x%x", kPacket.GetPacketID() );
		break;
	}
}

void MgameBillingServer::OnCashAction( SP2Packet &rkPacket )
{
	MgameCashResult kResult;
	rkPacket >> kResult;
	if( !kResult.IsValidPacket() )
	{
		LOG.PrintTimeAndLog( 0, "%s InValid Packet %x:%x:%d[%d:%s:%d:%d]%x:%x", __FUNCTION__, kResult.m_byStart[0], kResult.m_byStart[1], kResult.m_sSize,
			                                                                    kResult.m_iRet, kResult.m_szUserID, kResult.m_iMCash, kResult.m_iPCash,
																				kResult.m_byEnd[0], kResult.m_byEnd[1] );

		g_LogDBClient.OnInsertBillingServerError( CNT_MGAME, BILLING_ERROR_LOG_INVALID_HEADER,"Invalid packet header." );
		return;
	}

	if( kResult.m_iRet  == CASH_ACTION_RESULT_ALIVE_SUCCESS )
	{
		m_bSendAlive = false;
		return;
	}

	ioChannelingNodeMgame *pNode = static_cast<ioChannelingNodeMgame*> ( g_ChannelingMgr.GetNode( CNT_MGAME ) );
	if( pNode )
		pNode->OnRecieveCashAction( kResult );
}

void MgameBillingServer::SessionClose( BOOL safely/*=TRUE */ )
{
	CPacket packet(MNSTPK_CLOSE);
	ReceivePacket( packet );

}

void MgameBillingServer::OnClose( SP2Packet &rkPacket )
{
	if(IsActive())
	{
		OnDestroy();

		g_ConnectAssist.PushNode(this);
	}
}


