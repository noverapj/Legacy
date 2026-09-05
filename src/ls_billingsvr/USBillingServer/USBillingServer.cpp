#include "../stdafx.h"
#include "../MainProcess.h"
#include "../Network/GameServer.h"
#include "../Network/iocpHandler.h"
#include "../Network/ioPacketQueue.h"
#include "./USBillingServer.h"
#include "../Util/Ringbuffer.h"
#include "../channeling/iochannelingnodemanager.h"
#include "../channeling/iochannelingnodeWemadeBuy.h"
#include "../database/logdbclient.h"
#include "../Local/ioLocalManager.h"
#include "../Local/ioLocalParent.h"

extern CLog LOG;

USBillingServer *USBillingServer::sg_Instance = NULL;
USBillingServer::USBillingServer( SOCKET s, DWORD dwSendBufSize, DWORD dwRecvBufSize ) : CConnectNode( s, dwSendBufSize, dwRecvBufSize )
{
	InitData();
}

USBillingServer::~USBillingServer()
{	
}

USBillingServer &USBillingServer::GetInstance()
{
	if(sg_Instance == NULL)
	{
		ioINILoader kLoader( "ls_config_billingsvr.ini" );
		kLoader.SetTitle( "USBillingServer Buffer" );
		int iSendBufferSize = kLoader.LoadInt( "SendBufferSize", 16384 );
		int iRecvBufferSize = kLoader.LoadInt( "RecvBufferSize", 16384 );
		sg_Instance = new USBillingServer( INVALID_SOCKET, iSendBufferSize, iRecvBufferSize );
	}
	return *sg_Instance;
}

void USBillingServer::ReleaseInstance()
{		
	SAFEDELETE( sg_Instance );
}

bool USBillingServer::ConnectTo( bool bStart )
{
	ioINILoader kLoader( "ls_config_billingsvr.ini" );
	kLoader.SetTitle( "NETWORK" );

	char szServerIP[MAX_PATH]="";
	kLoader.LoadString( "USBillingServerIP", "", szServerIP, MAX_PATH );

	int iSSPort = kLoader.LoadInt( "USBillingServerPORT", 9000 );


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
	AfterCreate();
	LOG.PrintTimeAndLog( 0, "OnConnect (IP:%s PORT:%d RESULT:%d)", szServerIP, iSSPort, 0 );
	return true;
}

void USBillingServer::InitData()
{
	m_dwCurrentTime = 0;
	m_bSendAlive    = false;
	SetActive(false);
}

void USBillingServer::OnCreate()
{
	InitData();
	CConnectNode::OnCreate();
	

	m_dwCurrentTime = TIMEGETTIME();
}

void USBillingServer::OnDestroy()
{
	CConnectNode::OnDestroy();

	LOG.PrintTimeAndLog( 0, "Disconnect USBillingServer!" );
}

bool USBillingServer::SendMessage( CPacket &rkPacket )
{
	if( m_socket == INVALID_SOCKET ) return false;

	CPacket SendPacket( rkPacket.GetBuffer(), rkPacket.GetBufferSize() );
	return CConnectNode::SendMessage( SendPacket.GetBuffer() + SendPacket.GetCurPos(), SendPacket.GetBufferSize() - SendPacket.GetCurPos(), TRUE );
}

bool USBillingServer::CheckNS( CPacket &rkPacket )
{
	return true;             //네트웍 감시 필요없다.
}

int USBillingServer::GetConnectType()
{
	return CONNECT_TYPE_US_BILLING_SERVER;
}

void USBillingServer::ProcessTime()
{	
	if( g_App.IsWantExit() ) return;

	if( TIMEGETTIME() - m_dwCurrentTime > UPDATE_TIME )       
	{
		if( !IsActive() )
		{
			ConnectTo( false );
			LOG.PrintTimeAndLog( 0, "%s Connect", __FUNCTION__ );
		}
		else
		{	
			BILL_PACK_HEALTH_CHECK kInfo;
			SP2Packet kPacket( BTUBTPK_ALIVE_REQUEST );
			kInfo.Htonl();
			kPacket << kInfo;
			SendMessage( kPacket );
			//LOG.PrintTimeAndLog( 0, "%s Send ALIVE Check", __FUNCTION__ );
			if( m_bSendAlive )
			{
				LOG.PrintTimeAndLog( 0, "%s USBillingServer No Answer.", __FUNCTION__  );
				g_LogDBClient.OnInsertBillingServerError( CNT_WEMADEBUY, BILLING_ERROR_LOG_NO_ANSWER,"Billing server no answer." );
				OnDestroy();
				m_bSendAlive = false; // 초기화
			}
			else
			{
				m_bSendAlive = true;
			}
		}
		
		m_dwCurrentTime = TIMEGETTIME();
	}
}

void USBillingServer::SessionClose(BOOL safely)
{
	CPacket packet(BTUBTPK_CLOSE);
	ReceivePacket( packet );
}

void USBillingServer::DispatchReceive(CPacket& packet, DWORD bytesTransferred)
{
	m_recvIO.AddBytesTransferred( bytesTransferred );

	int loopCount = 0;

	while( m_recvIO.GetBytesTransferred() > 0 )
	{
		if( m_recvIO.GetBytesTransferred() < 4 ) // 2: Length, 2:Type
		{
			break;
		}

		WORD  wPacketSize  = 0;
		WORD  wType        = 0;
		DWORD dwPacketID   = 0;

		m_recvIO.GetBuffer( &wPacketSize, sizeof(WORD), 0); //kyg int형으로 오는거같은데 왜 기존코드에선 word로 받았을까 고민 
		m_recvIO.GetBuffer( &wType, sizeof(WORD), 2);

		wPacketSize = ntohs( wPacketSize );
		wType       = ntohs( wType );

		enum { ALIVE = 40, BALANCE = 10, BUY = 20, CANCEL = 22, };
		if( wType == ALIVE )
		{
			dwPacketID = BTUBTPK_ALIVE_RESULT; 
		}
		else if( wType == BALANCE )
		{
			dwPacketID = BTUBTPK_BALANCE_RESULT;
		}
		else if( wType == BUY )
		{
			dwPacketID = BTUBTPK_BUY_RESULT;
		}
		else if( wType == CANCEL )
		{
			dwPacketID = BTUBTPK_CANCEL_RESULT;
		}
		else
		{
			LOG.PrintTimeAndLog( 0, "%s Error Code : %d", __FUNCTION__, wType );
			break;
		}

		SP2Packet RecvPacket( dwPacketID );
		RecvPacket.SetDataAdd( (char*)m_recvIO.GetBuffer(), min( wPacketSize, m_recvIO.GetBytesTransferred() ), true );
		//

		if( (RecvPacket.IsValidPacket() == true) && (m_recvIO.GetBytesTransferred() >= wPacketSize) )
		{
			if( !CheckNS( RecvPacket ) ) return;

			ReceivePacket( RecvPacket );

			m_recvIO.AfterReceive( wPacketSize );
		}
		else 
			break;

		if(loopCount > 200)
		{
			LOG.PrintTimeAndLog(0,"Error %s LoopCountOver(%d:%d)",wPacketSize,bytesTransferred);
			m_recvIO.AfterReceive(bytesTransferred);
			break;
		}
	}

	WaitForPacketReceive();
}

void USBillingServer::OnClose( SP2Packet &rkPacket )
{
	OnDestroy();
}

void USBillingServer::ReceivePacket( CPacket &packet )
{
	g_RecvQueue.InsertQueue( (DWORD)this, packet, PK_QUEUE_SESSION );
}

void USBillingServer::PacketParsing( CPacket &packet )
{
	SP2Packet &kPacket = (SP2Packet&)packet;

	switch( kPacket.GetPacketID() )
	{
	case BTUBTPK_ALIVE_RESULT:
		OnAlive( kPacket );
		break;
	case BTUBTPK_BALANCE_RESULT:
		OnBalance( kPacket );
		break;
	case BTUBTPK_BUY_RESULT:
		OnBuy( kPacket );
		break;
	case BTUBTPK_CANCEL_RESULT:
		OnCancel( kPacket );
		break;
	case BTUBTPK_CLOSE:
		OnClose( kPacket );
		break;
	default:
		LOG.PrintTimeAndLog( 0, "USBillingServer::PacketParsing 알수없는 패킷 : 0x%x", kPacket.GetPacketID() );
		break;
	}
}

#if 1
void USBillingServer::OnAlive( SP2Packet &rkPacket )
{
	BILL_PACK_HEALTH_CHECK kResult;
	rkPacket >> kResult;
	kResult.Ntohl();
	m_bSendAlive = false; // toggle로 보낸 send 값을 정상 처리함.
	//LOG.PrintTimeAndLog( 0, "%s:%d:%d", __FUNCTION__, kResult.ReqType, kResult.RetCode );
}

void USBillingServer::OnBalance( SP2Packet &rkPacket )
{
	BILL_PACK_GETBALANCE kResult;
	rkPacket >> kResult;
	kResult.Ntohl();
	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	if( pLocal )
		pLocal->OnRecieveGetCash( kResult );
}

void USBillingServer::OnBuy( SP2Packet &rkPacket )
{
	BILL_PACK_PURCHASEITEM kResult;
	rkPacket >> kResult;
	kResult.Ntohl();
	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	if( pLocal )
		pLocal->OnRecieveOutputCash( kResult );
}

void USBillingServer::OnCancel( SP2Packet &rkPacket )
{
	BILL_PACK_CNLPURCHASE kResult;
	rkPacket >> kResult;
	kResult.Ntohl();
	//이미 유저가 없는 상태이므로 유저에게 알릴 필요 없음
	LOG.PrintTimeAndLog( 0, "%s:%d:%s[%s]%d:%s", __FUNCTION__, kResult.UserNo, kResult.UserID, kResult.ChargeNo, kResult.RetCode, kResult.RetMsg );
}
#endif