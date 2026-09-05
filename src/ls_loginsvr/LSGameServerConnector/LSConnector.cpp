#include "StdAfx.h"
#include "LSConnector.h"


LSConnector::LSConnector( SOCKET s, DWORD dwSendBufSize, DWORD dwRecvBufSize ) : CConnectNode(s,dwSendBufSize,dwRecvBufSize)
{
	Init();

}

LSConnector::~LSConnector(void)
{
	Destroy();
}
 
LSConnector::LSConnector() : CConnectNode(INVALID_SOCKET,DEFAULT_BUFFER,DEFAULT_BUFFER*2+1)
{
	Init();
};

void LSConnector::Init()
{
	m_referenceCount = 0;
}

void LSConnector::Destroy()
{
}

bool LSConnector::ConnectTo(std::string serverIP,int& serverPort)
{
	m_ipAddr	= serverIP;
	m_port		= serverPort;
	SOCKET fd = ::socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	//소켓옵션 리유즈로 할것 

	if(fd == INVALID_SOCKET)
	{
		LOG.PrintTimeAndLog(0,"LsConnector::Connect to socket %d[%s:%d]",GetLastError(), serverIP.c_str(), serverPort);
		return false;
	}

	sockaddr_in serv_addr;
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(serverIP.c_str());
	serv_addr.sin_port = htons(serverPort);
	if(::connect(fd,(sockaddr*)&serv_addr,sizeof(serv_addr)) != 0)
	{
		bool reuse = true;
		::setsockopt(GetSocket(),SOL_SOCKET,SO_REUSEADDR,(TCHAR*)&reuse,sizeof(reuse));
		closesocket(fd);

		LOG.PrintTimeAndLog(0,"LsConnector::Connect to socket %d[%s:%d]",GetLastError(), serverIP.c_str(), serverPort);
		return false;
	}

	g_Iocp()->AddHandleToIOCP((HANDLE)fd,(DWORD)this);
	CConnectNode::SetSocket(fd);
	OnCreate();
	AfterCreate();
	LOG.PrintTimeAndLog(0,"OnConnect (IP : %s port %d)", serverIP.c_str(), serverPort);
	return true;
}

void LSConnector::OnCreate()
{
	CConnectNode::OnCreate();
	UpdateSockState(ESOCKET::LS_CONNECTED);
}

void LSConnector::OnDestroy()
{
	CConnectNode::OnDestroy();
	LOG.PrintTimeAndLog(0,"LSConnector : Disconnect");
}

bool LSConnector::SendMessage(CPacket& rkPacket)
{
	return CConnectNode::SendMessage(rkPacket);
}

bool LSConnector::CheckNS( CPacket &rkPacket )
{
	return true;
}

int LSConnector::GetConnectType()
{
	return ENODETYPE::CONNECTOR;
}

void LSConnector::ReceivePacket( CPacket &packet )
{
	g_Queue()->InsertQueue( (DWORD)this, packet, PK_QUEUE_SESSION );
}

void LSConnector::OnClose( SP2Packet &packet )
{	
	UpdateSockState( ESOCKET::LS_DISCONNECTED );
	g_ServerConnectMgr()->DelConnector(this);

	OnDestroy();
}

void LSConnector::PacketParsing( CPacket &packet )
{
	FUNCTION_TIME_CHECKER( 100000.0f, packet.GetPacketID() );  
	switch(packet.GetPacketID())
	{
	case EPROTOCOL::ITPK_CLOSE_SESSION:
		OnClose((SP2Packet&)packet);
		break;

	case EPROTOCOL::ITPK_OPERATIONTYPE:
		break;

	case EPROTOCOL::LSTPK_STATUS_RESPONSE:
		OnResponse(packet);
		break;

	default:
		//LOG.PrintTimeAndLog( 0, "ServerNode::PacketParsing 알수없는 패킷 : 0x%x", packet.GetPacketID() ); 있게할지 없게 할지 고민 
		break;
	}
}

void LSConnector::SessionClose( BOOL safely/* =TRUE */ )
{
	if(IsActive())
	{
		CPacket packet( EPROTOCOL::ITPK_CLOSE_SESSION );
		ReceivePacket( packet );
	}
}

void LSConnector::OnResponse( CPacket & packet )
{
	int serverIndex = 0, blockState = 0, userCount = 0, clientPort = 0;
	int serverState = ESVRSTATE::LS_RESPSERVERSTATE;
	TCHAR serverIP[STR_IP_MAX];

	packet >> serverIndex;
	packet >> blockState;
	packet >> userCount;
	packet >> serverIP;
	packet >> clientPort;

	// 블록여부
	if(1 == blockState)
	{
		serverState = ESVRSTATE::LS_BLOCKED;
	}
	
	g_ServerInfoMgr()->UpdateServerInfo(
		GetSocket(),
		serverIP,
		serverIndex,
		clientPort,
		userCount,
		serverState);
 
	ResetReferenceCount(0);
}

void LSConnector::UpdateState( int value )
{
	g_ServerInfoMgr()->UpdateServerState(GetSocket(), value);
}

void LSConnector::UpdateSockState( int value )
{
	g_ServerInfoMgr()->UpdateSockState(GetSocket(), value);
}
