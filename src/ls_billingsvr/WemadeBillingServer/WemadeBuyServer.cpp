#include "../stdafx.h"
#include "../MainProcess.h"
#include "../Network/GameServer.h"
#include "../Network/iocpHandler.h"
#include "../Network/ioPacketQueue.h"
#include "./WemadeBuyServer.h"
#include "../Util/Ringbuffer.h"
#include "../channeling/iochannelingnodemanager.h"
#include "../channeling/iochannelingnodeWemadeBuy.h"
#include "../database/logdbclient.h"
#include "../Local/ioLocalManager.h"
#include "../Network/ConnectAssist.h"


extern CLog LOG;
WemadeBuyServer *WemadeBuyServer::sg_Instance = NULL;
WemadeBuyServer::WemadeBuyServer( SOCKET s, DWORD dwSendBufSize, DWORD dwRecvBufSize ) : CConnectNode( s, dwSendBufSize, dwRecvBufSize )
{
	InitData();
	ZeroMemory(m_szServerIP,MAX_PATH);
	m_iSSPort = 0;
}

WemadeBuyServer::~WemadeBuyServer()
{	
}

WemadeBuyServer &WemadeBuyServer::GetInstance()
{
	if(sg_Instance == NULL)
	{
		ioINILoader kLoader( "ls_config_billingsvr.ini" );
		kLoader.SetTitle( "WemadeBuyServer Buffer" );
		int iSendBufferSize = kLoader.LoadInt( "SendBufferSize", 16384 );
		int iRecvBufferSize = kLoader.LoadInt( "RecvBufferSize", 16384 );
		sg_Instance = new WemadeBuyServer( INVALID_SOCKET, iSendBufferSize, iRecvBufferSize );
	}
	return *sg_Instance;
}

void WemadeBuyServer::ReleaseInstance()
{		
	SAFEDELETE( sg_Instance );
}

bool WemadeBuyServer::ConnectTo( bool bStart )
{
	ioINILoader kLoader( "ls_config_billingsvr.ini" );
	kLoader.SetTitle( "NETWORK" );

	
	kLoader.LoadString( "WemadeBuyServerIP", "", m_szServerIP, MAX_PATH );

	m_iSSPort = kLoader.LoadInt( "WemadeBuyServerPORT", 9000 );


	SOCKET socket = ::socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
	if( socket == INVALID_SOCKET )
	{
		LOG.PrintTimeAndLog( 0, "%s fail socket %d[%s:%d]", __FUNCTION__, GetLastError(), m_szServerIP, m_iSSPort );
		return false;
	}
	sockaddr_in serv_addr;
	serv_addr.sin_family		= AF_INET;
	inet_pton(AF_INET, m_szServerIP, &serv_addr.sin_addr);
	serv_addr.sin_port			= htons( m_iSSPort );
 
 
	int retval = ::connect( socket, (sockaddr*)&serv_addr, sizeof(serv_addr) );
	if( retval != 0 ) 
	{
		DWORD dwError = GetLastError();
		if( dwError != WSAEWOULDBLOCK )
		{
		//	LOG.PrintTimeAndLog( 0, "%s fail connect Errcode(%d)[%s:%d]", __FUNCTION__,dwError, m_szServerIP, m_iSSPort );
			bool reuse = true;
			::setsockopt(GetSocket(),SOL_SOCKET,SO_REUSEADDR,(TCHAR*)&reuse,sizeof(reuse));
			closesocket(socket);
			return false;
		}
	}

	// block
	CConnectNode::SetSocket( socket );

	//OnCreate();	
	//AfterCreate();
	LOG.PrintTimeAndLog( 0, "[info][wemade]OnConnect (IP:%s PORT:%d RESULT:%d)", m_szServerIP, m_iSSPort, 0 );
	return true;
}

void WemadeBuyServer::InitData()
{
	m_dwCurrentTime = 0;
	m_bSendAlive    = false;
	SetActive(false);

}

void WemadeBuyServer::OnCreate()
{
	InitData();
	CConnectNode::OnCreate();

	m_dwCurrentTime = TIMEGETTIME();
}

bool WemadeBuyServer::AfterCreate()
{
	g_iocp.AddHandleToIOCP( (HANDLE)GetSocket(), (DWORD)this );
	LOG.PrintTimeAndLog( 0, "[info][wemade]WemadeBuyServerConnect (IP:%s PORT:%d RESULT:%d)", m_szServerIP, m_iSSPort, 0 );

	return CConnectNode::AfterCreate();
}
void WemadeBuyServer::OnDestroy()
{
	CConnectNode::OnDestroy();

	LOG.PrintTimeAndLog( 0, "[warning][wemade]Disconnect WemadeBuyServer!" );
}

bool WemadeBuyServer::SendMessage( CPacket &rkPacket )
{
	if( m_socket == INVALID_SOCKET ) return false;

	CPacket SendPacket( rkPacket.GetBuffer(), rkPacket.GetBufferSize() );
	return CConnectNode::SendMessage( SendPacket.GetBuffer() + SendPacket.GetCurPos(), SendPacket.GetBufferSize() - SendPacket.GetCurPos(), TRUE );
}

bool WemadeBuyServer::CheckNS( CPacket &rkPacket )
{
	return true;             //네트웍 감시 필요없다.
}

int WemadeBuyServer::GetConnectType()
{
	return CONNECT_TYPE_WEMADE_BUY_SERVER;
}

void WemadeBuyServer::ProcessTime()
{	
	if( g_App.IsWantExit() ) return;

	if( TIMEGETTIME() - m_dwCurrentTime > UPDATE_TIME )       
	{
		if( !IsActive() )
		{
		//	ConnectTo( false );
		//	LOG.PrintTimeAndLog( 0, "%s Connect", __FUNCTION__ );
		}
		else
		{	
			PHEADER kInfo;
			SP2Packet kPacket( BTWBTPK_ALIVE_REQUEST );
			kPacket << kInfo;
			SendMessage( kPacket ); //kyg 데이터가 이상하게 오는것같아 일단 핑안보냄 
			//LOG.PrintTimeAndLog( 0, "%s Send ALIVE Check", __FUNCTION__ );
			if( m_bSendAlive )
			{
				LOG.PrintTimeAndLog( 0, "%s WemadeBuyServer No Answer.", __FUNCTION__  );
				g_LogDBClient.OnInsertBillingServerError( CNT_WEMADEBUY, BILLING_ERROR_LOG_NO_ANSWER,"Billing server no answer." );
				SessionClose();
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
#if 0 
bool WemadeBuyServer::Dispatch( DWORD bytesTransferred, OVERLAPPED *ov, int &iReturnCode )
{

	iReturnCode = 1001;
	LPIOBufferedContext lpIOBufferedContext = (LPIOBufferedContext)ov;
	iReturnCode = 1002;
	if( lpIOBufferedContext->m_flags == ASYNCFLAG_RECEIVE )
	{
		iReturnCode = 1003;
		DispatchReceiveForWemade( bytesTransferred ); // CConnectNode::Dispatch() 메소드와 이부분만 다름
		iReturnCode = 1004;
		return true;
	}
	else if( lpIOBufferedContext->m_flags == ASYNCFLAG_SEND )
	{
		iReturnCode = 1005;
		if( bytesTransferred == 0 )
		{
			iReturnCode = 1006;
			LOG.PrintTimeAndLog( 0, "Dispatch Send FAILED : Send Size : %d", bytesTransferred );
		}
		iReturnCode = 1007;
		WaitForPacketSend( bytesTransferred );
		iReturnCode = 1008;
		return true;
	}

	return false;
}
#endif
void WemadeBuyServer::DispatchReceive( CPacket& packet, DWORD bytesTransferred )
{
	m_recvIO.AddBytesTransferred( bytesTransferred );

	int loopCount = 0;

	while( m_recvIO.GetBytesTransferred() > 0 )
	{
		if( m_recvIO.GetBytesTransferred() < 6 ) // 2: Code, 2:Version, 2:Length
		{
			break;
		}

		DWORD dwPacketID   = 0;
		WORD  wCode        = 0;
		WORD  wPacketSize  = 0;
		m_recvIO.GetBuffer( &wCode, sizeof(WORD), 0); //kyg int형으로 오는거같은데 왜 기존코드에선 word로 받았을까 고민 
		m_recvIO.GetBuffer( &wPacketSize, sizeof(WORD), 4);
// 		memcpy( &wCode, m_RecvIO.m_aBuffer, 2, 2 ); 
// 		memcpy( &wPacketSize, &m_RecvIO.m_aBuffer[4], 2 ); 

		switch(wCode)
		{
		case ALIVE:
			dwPacketID = BTWBTPK_ALIVE_RESULT; 
			break;
		case BALANCE:
			dwPacketID = BTWBTPK_BALANCE_RESULT; 
			break;
		case BUY:
			dwPacketID = BTWBTPK_BUY_RESULT; 
			break;
		case PRESENT:
			dwPacketID = BTWBTPK_PRESENT_RESULT; 
			break;
		case CANCEL:
			dwPacketID = BTWBTPK_CANCEL_RESULT; 
			break;
		case SUBSCRIPTION_RETRACT:
			dwPacketID = BTWBTPK_SUBSCRIPTION_RETRACT;
			break;
		case SUBSCRIPTION_RETRACT_CHECK:
			dwPacketID = BTWBTPK_SUBSCRIPTION_RETRACT_CHECK; 
			break;
		case SUBSCRIPTION_RETRACT_CANCEL:
			dwPacketID = BTWBTPK_SUBSCRIPTION_RETRACT_CANCEL; 
			break;
		default:
			LOG.PrintTimeAndLog( 0, "%s Error Code : %d", __FUNCTION__, wCode );
		}


		SP2Packet RecvPacket( dwPacketID );
		RecvPacket.SetDataAdd( (char*)m_recvIO.GetBuffer(), min( wPacketSize, m_recvIO.GetBytesTransferred() ), true );
		//

		if( (RecvPacket.IsValidPacket() == true) && (m_recvIO.GetBytesTransferred() >= wPacketSize) )
		{
			if( !CheckNS( RecvPacket ) ) return;

			ReceivePacket( RecvPacket );

			m_recvIO.AfterReceive( wPacketSize );
//          쓰레드 전환을 감소시키기 위해서 제거     2010.12.13
//			Sleep(0);
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

void WemadeBuyServer::ReceivePacket( CPacket &packet )
{
	g_RecvQueue.InsertQueue( (DWORD)this, packet, PK_QUEUE_SESSION );
}

void WemadeBuyServer::PacketParsing( CPacket &packet )
{
	SP2Packet &kPacket = (SP2Packet&)packet;

	switch( kPacket.GetPacketID() )
	{
	case ITPK_CLOSE_SESSION:
		{
			OnClose(kPacket);
		}
		break;

	case BTWBTPK_ALIVE_RESULT:
		OnAlive( kPacket );
		break;
	case BTWBTPK_BALANCE_RESULT:
		OnBalance( kPacket );
		break;
	case BTWBTPK_BUY_RESULT:
		OnBuy( kPacket );
		break;
	case BTWBTPK_PRESENT_RESULT:
		OnPresent( kPacket );
		break;
	case BTWBTPK_CANCEL_RESULT:
		OnCancel( kPacket );
		break;
	case BTWBTPK_SUBSCRIPTION_RETRACT_CHECK:
		{
			OnRetractCheck(kPacket);
		}
		break;
	case BTWBTPK_SUBSCRIPTION_RETRACT:
		{
			OnRetract(kPacket);
		}
		break;;
	case BTWBTPK_SUBSCRIPTION_RETRACT_CANCEL:
		{
			OnRetractCancel(kPacket);
		}
		break;

	default:
		LOG.PrintTimeAndLog( 0, "WemadeBuyServer::PacketParsing 알수없는 패킷 : 0x%x", kPacket.GetPacketID() );
		break;
	}
}

void WemadeBuyServer::OnAlive( SP2Packet &rkPacket )
{
	SC_ALIVE kResult;
	rkPacket >> kResult;

	LOG.PrintTimeAndLog(0,"%s",__FUNCTION__);
	m_bSendAlive = false; // toggle로 보낸 send 값을 정상 처리함.
	//LOG.PrintTimeAndLog( 0, "%s:%d:%d:%s", __FUNCTION__, kResult.Code, kResult.ResultCode, kResult.ResultMessage );
}

void WemadeBuyServer::OnBalance( SP2Packet &rkPacket )
{
	SC_BALANCE kResult;
	rkPacket >> kResult;

	ioChannelingNodeWemadeBuy *pNode = static_cast<ioChannelingNodeWemadeBuy*> ( g_ChannelingMgr.GetNode( CNT_WEMADEBUY ) );
	if( pNode )
		pNode->OnRecieveGetCash( kResult );
}

void WemadeBuyServer::OnBuy( SP2Packet &rkPacket )
{
	SC_PURCHASEITEM kResult;
	rkPacket >> kResult;

	ioChannelingNodeWemadeBuy *pNode = static_cast<ioChannelingNodeWemadeBuy*> ( g_ChannelingMgr.GetNode( CNT_WEMADEBUY ) );
	if( pNode )
		pNode->OnRecieveOutputCash( kResult );
}

void WemadeBuyServer::OnPresent( SP2Packet &rkPacket )
{
	SC_GIFTITEM kResult;
	rkPacket >> kResult;

	ioChannelingNodeWemadeBuy *pNode = static_cast<ioChannelingNodeWemadeBuy*> ( g_ChannelingMgr.GetNode( CNT_WEMADEBUY ) );
	if( pNode )
		pNode->OnRecievePresentCash( kResult );
}

void WemadeBuyServer::OnCancel( SP2Packet &rkPacket )
{
	SC_CNLPURCHASE kResult;
	rkPacket >> kResult;

	//이미 유저가 없는 상태이므로 유저에게 알릴 필요 없음
	LOG.PrintTimeAndLog( 0, "%s:%d:%s[%s]%d:%s", __FUNCTION__, kResult.UserNo, kResult.UserID, kResult.ChargeNo, kResult.Result, kResult.ResultMessage );
}

void WemadeBuyServer::SessionClose( BOOL safely/*=TRUE */ )
{
	CPacket packet(ITPK_CLOSE_SESSION);
	ReceivePacket( packet );
}

void WemadeBuyServer::OnClose( SP2Packet &rkPacket )
{ 
	if(IsActive())
	{
		OnDestroy();
		LOG.PrintTimeAndLog(0,"%s OnClose",__FUNCTION__);
		g_ConnectAssist.PushNode(this);
	}
}

void WemadeBuyServer::OnRetractCheck( SP2Packet& rkPacket )
{
	SC_RETRACT_PAYBACK kResult;
	rkPacket >> kResult;

	ioChannelingNodeWemadeBuy *pNode = static_cast<ioChannelingNodeWemadeBuy*> ( g_ChannelingMgr.GetNode( CNT_WEMADEBUY ) );
	if( pNode )
		pNode->OnReceiveRetractCheckCash( kResult );
}

void WemadeBuyServer::OnRetract( SP2Packet& rkPacket )
{
	SC_RETRACT kResult;
	rkPacket >> kResult;

	ioChannelingNodeWemadeBuy *pNode = static_cast<ioChannelingNodeWemadeBuy*> ( g_ChannelingMgr.GetNode( CNT_WEMADEBUY ) );
	if( pNode )
		pNode->OnReceiveRetractCash( kResult );
}


void WemadeBuyServer::OnRetractCancel( SP2Packet& rkPacket )
{ //청약 철회 취소에 관한것도 작업 해야함 
	SC_RETRACT_CANCEL result;
	rkPacket >> result;
	int a = 0;
}