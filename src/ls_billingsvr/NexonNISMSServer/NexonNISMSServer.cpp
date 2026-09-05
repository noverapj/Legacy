#include "../stdafx.h"
#include "../MainProcess.h"
#include "../Network/GameServer.h"
#include "../Network/iocpHandler.h"
#include "../Network/ioPacketQueue.h"
#include "NexonNISMSServer.h"
#include "../Util/Ringbuffer.h"
#include "../channeling/iochannelingnodemanager.h"
#include "../database/logdbclient.h"
#include "../Local/ioLocalManager.h"
#include "../Local/ioLocalEU.h"
#include "../Local/ioLocalParent.h"
#include "../Network/ConnectAssist.h"
#include "../NodeInfo/UserInfoManager.h"
#include "../NodeInfo/ServerNodeManager.h"

#include <atltime.h>

extern CLog LOG;


NexonNISMSServer *NexonNISMSServer::sg_Instance = NULL;
NexonNISMSServer::NexonNISMSServer( SOCKET s, DWORD dwSendBufSize, DWORD dwRecvBufSize ) : CConnectNode( s, dwSendBufSize, dwRecvBufSize )
{
	InitData();
	m_bFirst = true;		
	ZeroMemory(m_szServerIP,MAX_PATH);
}



NexonNISMSServer::~NexonNISMSServer(void)
{
}

void NexonNISMSServer::InitData()
{
	m_dwCurrentTime = 0;
	m_bSendAlive    = false;
	SetActive(false);
	//m_serverNo = 0;
}

NexonNISMSServer &NexonNISMSServer::GetInstance()
{
	if(sg_Instance == NULL)
	{
		ioINILoader kLoader( "ls_config_billingsvr.ini" );
		kLoader.SetTitle( "NexonNISMSServer Buffer" );
		int iSendBufferSize = kLoader.LoadInt( "SendBufferSize", 16384 );
		int iRecvBufferSize = kLoader.LoadInt( "RecvBufferSize", 16384 );
		
		sg_Instance = new NexonNISMSServer( INVALID_SOCKET, iSendBufferSize, iRecvBufferSize );
	}
	return *sg_Instance;
}

void NexonNISMSServer::ReleaseInstance()
{
	SAFEDELETE( sg_Instance );
}

void hexdump1(const void * buf, size_t size)
{
	const uchar * cbuf = (const uchar *) buf;
	const ulong BYTES_PER_LINE = 16;
	ulong offset, minioffset;

	for (offset = 0; offset < size; offset += BYTES_PER_LINE)
	{
		// OFFSETXX  xx xx xx xx xx xx xx xx  xx xx . . .
		//     . . . xx xx xx xx xx xx   abcdefghijklmnop
		printf("%08x  ", cbuf + offset);
		for (minioffset = offset;
			minioffset < offset + BYTES_PER_LINE;
			minioffset++)
		{
			if (minioffset - offset == (BYTES_PER_LINE / 2)) {
				printf(" ");
			}

			if (minioffset < size) {
				printf("%02x ", cbuf[minioffset]);
			} else {
				printf("   ");
			}
		}
		printf("  ");

		for (minioffset = offset;
			minioffset < offset + BYTES_PER_LINE;
			minioffset++)
		{
			if (minioffset >= size)
				break;

			if (cbuf[minioffset] < 0x20 ||
				cbuf[minioffset] > 0x7e)
			{
				printf(".");
			} else {
				printf("%c", cbuf[minioffset]);
			}
		}
		printf("\n");
	}
}


bool NexonNISMSServer::ConnectTo( bool bStart )
{
	ioINILoader kLoader( "ls_config_billingsvr.ini" );
	kLoader.SetTitle( "NETWORK" );

	kLoader.LoadString( "NexonNISMSServerIP", "", m_szServerIP, MAX_PATH );

	m_iPort = kLoader.LoadInt( "NexonNISMSServerPORT", -1 );

	if(m_iPort == -1)
	{
		LOG.PrintTimeAndLog(0,"Error %s Port Is NULL ",__FUNCTION__);
		return false;
	}
	
	m_serverNo = kLoader.LoadInt( "NexonNISMSServerNo", 99 );

	SOCKET socket = ::socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
	if( socket == INVALID_SOCKET )
	{
		LOG.PrintTimeAndLog( 0, "%s fail socket %d[%s:%d]", __FUNCTION__, GetLastError(), m_szServerIP, m_iPort );
		return false;
	}
	sockaddr_in serv_addr;
	serv_addr.sin_family		= AF_INET;
	serv_addr.sin_addr.s_addr	= inet_addr( m_szServerIP );
	serv_addr.sin_port			= htons( m_iPort );
 
	int retval = ::connect( socket, (sockaddr*)&serv_addr, sizeof(serv_addr) );
	if( retval != 0 ) 
	{
		DWORD dwError = GetLastError();
		if( dwError != WSAEWOULDBLOCK )
		{
			LOG.PrintTimeAndLog( 0, "[warning][nexon]%s fail connect Errcode(%d)[%s:%d]", __FUNCTION__,dwError, m_szServerIP, m_iPort );
			bool reuse = true;
			::setsockopt(GetSocket(),SOL_SOCKET,SO_REUSEADDR,(TCHAR*)&reuse,sizeof(reuse));
			closesocket(socket);
			return false;
		}
	}
	// block
	CConnectNode::SetSocket( socket );
	
	//OnCreate();
	
	return true;
}

void NexonNISMSServer::OnCreate()
{
	InitData();	

	CConnectNode::OnCreate();

	m_dwCurrentTime = TIMEGETTIME();
}

bool NexonNISMSServer::AfterCreate()
{
	bool state = false;

	g_iocp.AddHandleToIOCP( (HANDLE)GetSocket(), (DWORD)this );
	LOG.PrintTimeAndLog( 0, "[warning][nexon]NexonNISMSServerOnConnect (IP:%s PORT:%d RESULT:%d)", m_szServerIP, m_iPort, 0 );

	state = CConnectNode::AfterCreate();
	 
	//패킷전송 
	SendInitPacket(m_bFirst);
	
	m_bFirst = false;

	return state;
}

bool NexonNISMSServer::SendInitPacket( bool bFirst )
{
	EU_INITIALIZE initPacket;
	initPacket.SetInfo( "LSAGA" );	//코드 확인해야함
	m_serverNo = 99;
	initPacket.serverNo = m_serverNo; //포트별 변경 필요 
	initPacket.Htonl();

	SP2Packet packet;
	packet.SetPosBegin();
	packet.Write(initPacket);
	SendMessage( packet );

	bFirst = false;
	
	return true;
}

void NexonNISMSServer::OnDestroy()
{
	CConnectNode::OnDestroy();

	LOG.PrintTimeAndLog( 0, "[warning][nexon]Disconnect NexonNISMS Server!" );
}

void NexonNISMSServer::SessionClose( BOOL safely )
{
	CPacket packet(ITPK_CLOSE_SESSION);
	ReceivePacket( packet );
}

int NexonNISMSServer::GetConnectType()
{
	return CONNECT_TYPE_NEXON_EU_SERVER;
}

bool NexonNISMSServer::CheckNS( CPacket &rkPacket )
{
	return true;
}

void NexonNISMSServer::ProcessTime()
{	
	if( g_App.IsWantExit() ) return;

	//2분마다 Alive Packet 보냄
	if( TIMEGETTIME() - m_dwCurrentTime > UPDATE_TIME )       
	{
		if( !IsActive() )
		{
			
		}
		else
		{	
			EU_ALIVE alive;
			alive.SetInfo( TIMEGETTIME(), 0 );
			alive.Htonl();

			SP2Packet packet;
			packet.SetPosBegin();
			packet.Write( alive );
			SendMessage( packet ); 

			if( m_bSendAlive )
			{
				LOG.PrintTimeAndLog( 0, "%s NexonNISMSServer No Answer.", __FUNCTION__  );
				//OnDestroy();
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


void NexonNISMSServer::DispatchReceive(CPacket& packet, DWORD bytesTransferred)
{
	m_recvIO.AddBytesTransferred( bytesTransferred );

	//hexdump1( m_recvIO.GetBuffer(), bytesTransferred );

	int loopCount = 0;
	while( m_recvIO.GetBytesTransferred() > 0 )
	{
		m_recvIO.GetBuffer(&m_packetParse, sizeof(m_packetParse), 0);	//헤더만 복사함
		m_packetParse.Ntohl();

		SP2Packet recvPacket( m_packetParse.packetType );

		int sizeofPacketParse = m_packetParse.size + sizeof(NexonEUPacketHeader) - sizeof( m_packetParse.packetID ) - sizeof( m_packetParse.size ); 
		
		recvPacket.SetDataAdd( (char*)m_recvIO.GetBuffer(), min(sizeofPacketParse, (int)m_recvIO.GetBytesTransferred() ), true );

		if( recvPacket.IsValidPacket() == true && m_recvIO.GetBytesTransferred() >= m_packetParse.size )
		{

			if( !CheckNS( recvPacket ) ) return;

			ReceivePacket( recvPacket );

			m_recvIO.AfterReceive( sizeofPacketParse );

		}
		else 
		{
			//m_recvIO.InitRecvIO();
			LOG.PrintTimeAndLog(0,"DispatchReceive Error Init RecvIO Size :%d PacketID : %x",bytesTransferred, m_packetParse.packetType );
			break;
		}
		
	}

	WaitForPacketReceive();
}


bool NexonNISMSServer::SendMessage( CPacket &rkPacket )
{
	if( m_socket == INVALID_SOCKET ) return false; //kyg 헤더 삭제하는 코드가 있었음 
	ThreadSync ts(this);
	CPacket SendPacket( rkPacket.GetBuffer(), rkPacket.GetBufferSize() );
	
	//hexdump1(SendPacket.GetBuffer() + SendPacket.GetCurPos(),SendPacket.GetBufferSize() - SendPacket.GetCurPos());
	return CConnectNode::SendMessage( SendPacket.GetBuffer() + SendPacket.GetCurPos(), SendPacket.GetBufferSize() - SendPacket.GetCurPos(), TRUE );

	/*if( m_socket == INVALID_SOCKET ) return false;

	ThreadSync ts(this);
	CPacket SendPacket( rkPacket.GetBuffer(), rkPacket.GetBufferSize() );
	return CConnectNode::SendMessage( SendPacket.GetBuffer() + SendPacket.GetCurPos(), SendPacket.GetBufferSize() - SendPacket.GetCurPos(), TRUE );*/

//{
//	
//}
}

void NexonNISMSServer::ReceivePacket( CPacket &packet )
{
	g_RecvQueue.InsertQueue( (DWORD)this, packet, PK_QUEUE_SESSION );
}


void NexonNISMSServer::OnClose( SP2Packet &rkPacket )
{
	if(IsActive())
	{
		OnDestroy();

		g_UserInfoManager->SetOnSyncState(false);

		g_ConnectAssist.PushNode(this);
	}
	LOG.PrintTimeAndLog(0,"NexonNISMS Server OnClose ");
}


void NexonNISMSServer::PacketParsing( CPacket &packet )
{
	SP2Packet &kPacket = (SP2Packet&)packet;

	__try
	{
		switch( kPacket.GetPacketID() )
		{
		case ITPK_CLOSE_SESSION:
			OnClose( kPacket );
		break;
			//초기화 패킷 보내서 응답으로 들어온 패킷
		case NEXON_EU_INITIALIZE :
			OnInitialize( kPacket );
			break;

		case NEXON_EU_ALIVE	:
			OnAlive( kPacket );
			break;

			//GetCash
		case NEXON_EU_BALANCE : 
			OnCheckBalance( kPacket );
			break;

		case NEXON_EU_OUTPUT_BALANCE : 
			OnPurchaseAmount( kPacket );
			break;
			//OutputCash
		case NEXON_EU_PURCHASE_ITEM : 
			OnPurchaseItem( kPacket );
			break;
			//선물
		case NEXON_EU_PURCHASE_GIFT : 
			OnPurchaseGift( kPacket );
			break;
		default:
			printf("%s Error Code : %x", __FUNCTION__,  kPacket.GetPacketID() );
			LOG.PrintTimeAndLog( 0, "%s Error Code : %d", __FUNCTION__,  kPacket.GetPacketID() );
		}

	}
	__except(1)
	{
		LOG.PrintTimeAndLog(0,"%s Error PacketID : 0x%x",__FUNCTION__,kPacket.GetPacketID());		
	}

}

//초기화 패킷 보내서 응답으로 들어온 패킷
void NexonNISMSServer::OnInitialize( SP2Packet &rkPacket )
{
	EU_INITIALIZE_RESPONSE initializeResponse;
	
	rkPacket.Read( initializeResponse );
	initializeResponse.Ntohl();
	
	if (initializeResponse.result != 1 )
	{
		printf("size : %d\n", initializeResponse.size );
		LOG.PrintTimeAndLog(0,"%s Initialize Fail : ret %d", "NexonNISMSServer::OnInitialize", initializeResponse.result);		
		SessionClose();
		OnDestroy();
	}
	else
	{
		LOG.PrintTimeAndLog(0,"%s Initialize Success : ret %d", "NexonNISMSServer::OnInitialize", initializeResponse.result );		
	}
}

void NexonNISMSServer::OnAlive( SP2Packet &rkPacket )
{
	EU_ALIVE_RESPONSE aliveResponse;
	rkPacket.Read( aliveResponse );
	aliveResponse.Ntohl();

	//패킷을 10초 이내로 못받은 경우 연결 오류
	if( TIMEGETTIME() - aliveResponse.packetNo > ALIVE_TIME )       
	{
		LOG.PrintTimeAndLog(0,"[warning][nexon]%s Error Connection : %d",__FUNCTION__, aliveResponse.result);		
	}
	
	m_bSendAlive = false;
}

void NexonNISMSServer::OnCheckBalance( SP2Packet &rkPacket )
{
	EU_GETCASH_RESPONSE getCash;
	rkPacket.Read( getCash );
	getCash.Ntohl();

	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	if( pLocal )
		pLocal->OnReceiveGetCash( getCash );
}

void NexonNISMSServer::OnPurchaseAmount( SP2Packet &rkPacket )
{
	/*
	EU_AMOUNT_RESPONSE purchaseAmount;
	rkPacket.Read( purchaseAmount );
	purchaseAmount.Ntohl();

	*/
	EU_GETCASH_AMOUNT_RESPONSE getAmountCash;
	rkPacket.Read( getAmountCash );
	getAmountCash.Ntohl();

	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	if( pLocal )
		pLocal->OnRecieveOutputCashAmount( getAmountCash );
}

void NexonNISMSServer::OnPurchaseItem( SP2Packet &rkPacket )
{
	EU_PURCAHSEITEM_RESPONSE outputCash;
	rkPacket.Read( outputCash );
	outputCash.Ntohl();

	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	if( pLocal )
		pLocal->OnRecieveOutputCash( outputCash );
}

void NexonNISMSServer::OnPurchaseGift( SP2Packet &rkPacket )
{
	/*ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	if( pLocal )
		pLocal->OnRecieveGetCash( sResult );*/
}