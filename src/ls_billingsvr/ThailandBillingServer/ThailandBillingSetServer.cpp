#include "../stdafx.h"
#include "../MainProcess.h"
#include "../Network/GameServer.h"
#include "../Network/iocpHandler.h"
#include "../Network/ioPacketQueue.h"
#include "./ThailandBillingSetServer.h"
#include "../Util/Ringbuffer.h"
#include "../channeling/iochannelingnodemanager.h"
#include "../database/logdbclient.h"
#include "../Local/ioLocalManager.h"
#include "../Local/ioLocalThailand.h"


extern CLog LOG;

ThailandBillingSetServer *ThailandBillingSetServer::sg_Instance = NULL;
ThailandBillingSetServer::ThailandBillingSetServer( SOCKET s, DWORD dwSendBufSize, DWORD dwRecvBufSize ) : CConnectNode( s, dwSendBufSize, dwRecvBufSize )
{
	InitData();
}

ThailandBillingSetServer::~ThailandBillingSetServer()
{	
}

ThailandBillingSetServer &ThailandBillingSetServer::GetInstance()
{
	if(sg_Instance == NULL)
	{
		ioINILoader kLoader( "BillingRelayServerInfo.ini" );
		kLoader.SetTitle( "Thailand Server Session" );
		int iSendBufferSize = kLoader.LoadInt( "SendBufferSize", 16384 );
		int iRecvBufferSize = kLoader.LoadInt( "RecvBufferSize", 16384 );
		sg_Instance = new ThailandBillingSetServer( INVALID_SOCKET, iSendBufferSize, iRecvBufferSize );
	}
	return *sg_Instance;
}

void ThailandBillingSetServer::ReleaseInstance()
{		
	SAFEDELETE( sg_Instance );
}

bool ThailandBillingSetServer::ConnectTo( bool bStart )
{
	ioINILoader kLoader( "ls_config_billingsvr.ini" );
	kLoader.SetTitle( "NETWORK" );

	char szServerIP[MAX_PATH];
	kLoader.LoadString( "ThailandBillingSetServerIP", "", szServerIP, MAX_PATH );

	int iSSPort = kLoader.LoadInt( "ThailandBillingSetServerPORT", 9000 );


	SOCKET socket = ::socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
	if( socket == INVALID_SOCKET )
	{
		LOG.PrintTimeAndLog( 0, "%s fail socket %d[%s:%d]", __FUNCTION__, GetLastError(), szServerIP, iSSPort );
		return false;
	}
	sockaddr_in serv_addr;
	serv_addr.sin_family		= AF_INET;
	serv_addr.sin_addr.s_addr	= inet_addr( szServerIP );
	serv_addr.sin_port			= htons( iSSPort );

	// non-block
	unsigned long arg = 1;
	ioctlsocket( socket, FIONBIO, &arg );

	int retval = ::connect( socket, (sockaddr*)&serv_addr, sizeof(serv_addr) );
	if( retval != 0 ) 
	{
		DWORD dwError = GetLastError();
		if( dwError != WSAEWOULDBLOCK )
		{
			LOG.PrintTimeAndLog( 0, "%s fail connect %x:%d[%s:%d]", __FUNCTION__, this, dwError, szServerIP, iSSPort );
			return false;
		}
	}

	// block
	arg = 0;
	ioctlsocket( socket, FIONBIO, &arg );

	if( retval != 0 )
	{
		// timeout
		struct timeval tv;
		if( bStart )
			tv.tv_sec  = CONNECT_WAIT_SECONDS*2;
		else
			tv.tv_sec  = CONNECT_WAIT_SECONDS;
		tv.tv_usec = 0;

		fd_set writedfds;    
		FD_ZERO(&writedfds);
		FD_SET(socket, &writedfds);

		retval = select(socket+1, NULL, &writedfds, NULL, &tv);

		if(retval == 0)
		{
			LOG.PrintTimeAndLog( 0, "%s Error 1 %d",  __FUNCTION__, GetLastError() );
			return false;
		}
		else if(retval == SOCKET_ERROR)
		{
			LOG.PrintTimeAndLog( 0, "%s Error 2 %d",  __FUNCTION__, GetLastError() );
			return false;
		}

		if(!FD_ISSET(socket, &writedfds))
		{
			LOG.PrintTimeAndLog( 0, "%s Error 3 %d",  __FUNCTION__, GetLastError() );
			return false;
		}
	}

	g_iocp.AddHandleToIOCP( (HANDLE)socket, (DWORD)this );
	CConnectNode::SetSocket( socket );
	
	OnCreate();	
	
	if( !AfterCreate() )
	{
		LOG.PrintTimeAndLog(0,"Error %s::OnCreate",__FUNCTION__);
	}
	
	LOG.PrintTimeAndLog( 0, "%s OnConnect (IP:%s PORT:%d RESULT:%d)", __FUNCTION__, szServerIP, iSSPort, 0 );
	return true;
}

void ThailandBillingSetServer::InitData()
{
	m_dwCurrentTime = 0;
	SetActive(false);
}

void ThailandBillingSetServer::OnCreate()
{	
	InitData();

	CConnectNode::OnCreate();

	m_dwCurrentTime = TIMEGETTIME();
}

void ThailandBillingSetServer::OnDestroy()
{
	CConnectNode::OnDestroy();

	LOG.PrintTimeAndLog( 0, "%s", __FUNCTION__ );
}

bool ThailandBillingSetServer::SendMessage( CPacket &rkPacket )
{
	if( m_socket == INVALID_SOCKET ) return false;

	CPacket SendPacket( rkPacket.GetBuffer(), rkPacket.GetBufferSize() );
	return CConnectNode::SendMessage( SendPacket.GetBuffer() + SendPacket.GetCurPos(), SendPacket.GetBufferSize() - SendPacket.GetCurPos(), TRUE );
}

bool ThailandBillingSetServer::CheckNS( CPacket &rkPacket )
{
	return true;             //네트웍 감시 필요없다.
}

int ThailandBillingSetServer::GetConnectType()
{
	return CONNECT_TYPE_THAILAND_BILLING_SET_SERVER;
}

void ThailandBillingSetServer::ProcessTime()
{	
	if( g_App.IsWantExit() ) return;

	if( TIMEGETTIME() - m_dwCurrentTime > UPDATE_TIME )       
	{
		static int iCnt = 0;
		if( !IsActive() )
		{
			ConnectTo( false );
		}
		else
		{	
			ioLocalThailand *pLocal = static_cast<ioLocalThailand*> ( g_LocalMgr.GetLocal( ioLocalManager::LCT_THAILAND ) );
			if( pLocal )
			{
				ioHashString sPID;
				pLocal->SendOutputCash( "THAS.lsasb025", "127.0.0.1", "THAS.lsasb025", 0, 0, true , sPID );
			}
		}
		
		m_dwCurrentTime = TIMEGETTIME();
	}
}

void ThailandBillingSetServer::DispatchReceive(CPacket& packet, DWORD bytesTransferred)
{
	m_recvIO.AddBytesTransferred( bytesTransferred );

	while( m_recvIO.GetBytesTransferred() > 0 )
	{
		// 헤더가 없는 패킷에 헤더를 더한다. CConnectNode::DispatchReceive() 이부분만 다름.
		DWORD dwPacketSize = 0;
		const char* buffer = m_recvIO.GetBuffer();
		for (int i = 0; i < (int)m_recvIO.GetBytesTransferred() ; i++)
		{
			if( buffer[i] == ';' )
			{
				dwPacketSize = i+1;
				break;
			}
		}

		if( dwPacketSize == 0 )
			break;

		SP2Packet RecvPacket( BTTPK_BILLING_SET_RESULT );
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

			m_recvIO.AfterReceive( dwPacketSize);
		}
		else 
			break;
	}

	WaitForPacketReceive();
}

void ThailandBillingSetServer::ReceivePacket( CPacket &packet )
{
	g_RecvQueue.InsertQueue( (DWORD)this, packet, PK_QUEUE_SESSION );
}

void ThailandBillingSetServer::PacketParsing( CPacket &packet )
{
	SP2Packet &kPacket = (SP2Packet&)packet;

	switch( kPacket.GetPacketID() )
	{
	case ITPK_CLOSE_SESSION:
		{
			OnClose(kPacket);
		}
		break;

	case BTTPK_BILLING_SET_RESULT:
		OnBillingSet( kPacket );
		break;
	default:
		LOG.PrintTimeAndLog( 0, "%s 알수없는 패킷 : 0x%x", __FUNCTION__, kPacket.GetPacketID() );
		break;
	}
}

void ThailandBillingSetServer::OnBillingSet( SP2Packet &rkPacket )
{
	ioHashString sResult;
	rkPacket >> sResult;
	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	if( pLocal )
		pLocal->OnRecieveOutputCash( sResult );
}

void ThailandBillingSetServer::SessionClose( BOOL safely/*=TRUE */ )
{
	CPacket packet(ITPK_CLOSE_SESSION);
	ReceivePacket( packet );
}

void ThailandBillingSetServer::OnClose( SP2Packet &rkPacket )
{ 
	if(IsActive())
		OnDestroy();
}