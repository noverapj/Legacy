#include "../stdafx.h"
#include "../MainProcess.h"
#include "../Network/GameServer.h"
#include "../Network/iocpHandler.h"
#include "../Network/ioPacketQueue.h"
#include "NexonEUSessionServer.h"
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


NexonEUSessionServer *NexonEUSessionServer::sg_Instance = NULL;
NexonEUSessionServer::NexonEUSessionServer( SOCKET s, DWORD dwSendBufSize, DWORD dwRecvBufSize ) : CConnectNode( s, dwSendBufSize, dwRecvBufSize )
{
	InitData();
	ZeroMemory(m_szServerIP,MAX_PATH);
	m_bFirst = true;	
}


NexonEUSessionServer::~NexonEUSessionServer(void)
{
}


void hexdumpEU(const void * buf, size_t size)
{
	const uchar * cbuf = (const uchar *) buf;
	const ulong BYTES_PER_LINE = 16;
	ulong offset, minioffset;

	for (offset = 0; offset < size; offset += BYTES_PER_LINE)
	{
		// OFFSETXX  xx xx xx xx xx xx xx xx  xx xx . . .
		//     . . . xx xx xx xx xx xx   abcdefghijklmnop
		printf("NexonEUSSO %08x  ", cbuf + offset);
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


void NexonEUSessionServer::InitData()
{
	m_dwCurrentTime = 0;
	m_bSendAlive    = false;
	SetActive(false);
	m_serverNo = 0;
	m_iEUTestMode = 0;
	ZeroMemory(m_iEUTestPublicIP, MAX_PATH);
}

NexonEUSessionServer &NexonEUSessionServer::GetInstance()
{
	if(sg_Instance == NULL)
	{
		ioINILoader kLoader( "ls_config_billingsvr.ini" );
		kLoader.SetTitle( "NexonEUServer Buffer" );
		int iSendBufferSize = kLoader.LoadInt( "SendBufferSize", 16384 );
		int iRecvBufferSize = kLoader.LoadInt( "RecvBufferSize", 16384 );
		
		sg_Instance = new NexonEUSessionServer( INVALID_SOCKET, iSendBufferSize, iRecvBufferSize );
	}
	return *sg_Instance;
}

void NexonEUSessionServer::ReleaseInstance()
{
	SAFEDELETE( sg_Instance );
}

bool NexonEUSessionServer::ConnectTo( bool bStart )
{
	ioINILoader kLoader( "ls_config_billingsvr.ini" );
	kLoader.SetTitle( "NETWORK" );

	kLoader.LoadString( "NexonEUSessionServerIP", "", m_szServerIP, MAX_PATH );

	m_iPort = kLoader.LoadInt( "NexonEUSessionServerPORT", -1 );

	if(m_iPort == -1)
	{
		LOG.PrintTimeAndLog(0,"Error %s Port Is NULL ",__FUNCTION__);
		return false;
	}

	
	

	
	

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

void NexonEUSessionServer::OnCreate()
{	
	InitData();	

	CConnectNode::OnCreate();

	m_dwCurrentTime = TIMEGETTIME();
}

bool NexonEUSessionServer::AfterCreate()
{
	bool state = false;

	g_iocp.AddHandleToIOCP( (HANDLE)GetSocket(), (DWORD)this );
	LOG.PrintTimeAndLog( 0, "[info][nexon]NexonEUSessionServerOnConnect (IP:%s PORT:%d RESULT:%d)", m_szServerIP, m_iPort, 0 );

	state = CConnectNode::AfterCreate();
	 
	//패킷전송 
	SendInitPacket(m_bFirst);
	
	m_bFirst = false;

	return state;
	
}

bool NexonEUSessionServer::SendInitPacket( bool bFirst )
{
	EU_SESSION_INITIALIZE initPacket;
	initPacket.SetInfo( 50360359, 99 );
	//initPacket.Ntohl();

	SP2Packet packet;
	packet.SetPosBegin();
	packet.Write(initPacket);
	SendMessage( packet );

	bFirst = false;
	
	return true;
}

void NexonEUSessionServer::OnDestroy()
{
	CConnectNode::OnDestroy();

	LOG.PrintTimeAndLog( 0, "[warning][nexon]Disconnect NexonEU Server!" );
}

void NexonEUSessionServer::SessionClose( BOOL safely )
{
	CPacket packet(ITPK_CLOSE_SESSION);
	ReceivePacket( packet );
}

int NexonEUSessionServer::GetConnectType()
{
	return CONNECT_TYPE_NEXON_EU_SESSION_SERVER;
}

bool NexonEUSessionServer::CheckNS( CPacket &rkPacket )
{
	return true;
}

void NexonEUSessionServer::ProcessTime()
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
			EU_SESSION_CHECK alive;
			alive.SetInfo( TIMEGETTIME() );
			
			SP2Packet packet;
			packet.SetPosBegin();
			packet.Write( alive );
			SendMessage( packet ); 

			if( m_bSendAlive )
			{
				LOG.PrintTimeAndLog( 0, "%s NexonEUSessionServer No Answer.", __FUNCTION__  );
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


void NexonEUSessionServer::DispatchReceive(CPacket& packet, DWORD bytesTransferred)
{
	m_recvIO.AddBytesTransferred( bytesTransferred );

	//hexdumpEU( m_recvIO.GetBuffer(), bytesTransferred );

	int loopCount = 0;
	while( m_recvIO.GetBytesTransferred() > 0 )
	{
		m_recvIO.GetBuffer(&m_packetParse, sizeof(m_packetParse), 0);	//헤더만 복사함
		
		SP2Packet recvPacket( m_packetParse.code );

		int sizeofPacketParse = m_packetParse.size; 
		
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
			LOG.PrintTimeAndLog(0,"DispatchReceive Error Init RecvIO Size :%d PacketID : %x",bytesTransferred, m_packetParse.code );
			break;
		}
		
	}

	WaitForPacketReceive();
}


bool NexonEUSessionServer::SendMessage( CPacket &rkPacket )
{
	if( m_socket == INVALID_SOCKET ) return false; 
	ThreadSync ts(this);
	CPacket SendPacket( rkPacket.GetBuffer(), rkPacket.GetBufferSize() );
	
	//hexdumpEU(SendPacket.GetBuffer() + SendPacket.GetCurPos(),SendPacket.GetBufferSize() - SendPacket.GetCurPos());
	return CConnectNode::SendMessage( SendPacket.GetBuffer() + SendPacket.GetCurPos(), SendPacket.GetBufferSize() - SendPacket.GetCurPos(), TRUE );
}

void NexonEUSessionServer::ReceivePacket( CPacket &packet )
{
	g_RecvQueue.InsertQueue( (DWORD)this, packet, PK_QUEUE_SESSION );
}


void NexonEUSessionServer::OnClose( SP2Packet &rkPacket )
{
	if(IsActive())
	{
		OnDestroy();

		g_UserInfoManager->SetOnSyncState(false);

		g_ConnectAssist.PushNode(this);
	}
	LOG.PrintTimeAndLog(0,"NexonEUSessionServer OnClose ");
}

void NexonEUSessionServer::PacketParsing( CPacket &packet )
{
	SP2Packet &kPacket = (SP2Packet&)packet;

	__try
	{
		switch( kPacket.GetPacketID() )
		{
		case ITPK_CLOSE_SESSION:
			OnClose( kPacket );
		break;
		case NEXON_EU_AUTH_INITIALIZE_REPLY :
			OnInitialize( kPacket );
			break;
		case NEXON_EU_AUTH_HELLO :
			OnAlive( kPacket );
			break;
		case NEXON_EU_AUTH_SESSION4_REPLY :
			OnSessionResponse( kPacket );
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
void NexonEUSessionServer::OnInitialize( SP2Packet &rkPacket )
{
	EU_SESSION_RESPONSE initializeResponse;
	rkPacket.Read( initializeResponse );

	if (initializeResponse.initializeResult == 1 )
	{
		printf("size : %d\n", initializeResponse.size );
		LOG.PrintTimeAndLog(0,"%s Initialize Fail ret: %d", "NexonEUSessionServer::OnInitialize", initializeResponse.initializeResult);		
		SessionClose();
		OnDestroy();
	}
	else if(initializeResponse.initializeResult == 0 )
	{
		LOG.PrintTimeAndLog(0,"%s Initialize Success ret:%d", "NexonEUSessionServer::OnInitialize", initializeResponse.initializeResult );		
	}
	
	
}

void NexonEUSessionServer::OnAlive( SP2Packet &rkPacket )
{
	EU_SESSION_CHECK alive;
	rkPacket.Read( alive );

	m_bSendAlive = false;
}


void NexonEUSessionServer::OnSessionResponse( SP2Packet &rkPacket )
{
	EU_SESSION4_REPLY reply;
	reply.Init();
	rkPacket.Read( reply );

	//바로 채널링으로 보내줌
	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	if( pLocal )
		pLocal->OnRecieveLoginData( reply );
}