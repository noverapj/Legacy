#include "../iocpSocketDLL.h"
#include "Packet.h"
#include "ConnectNode.h"
#include "AcceptorNode.h"
#include "ServerSocket.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ServerSocket::ServerSocket() : m_socket(INVALID_SOCKET), m_packet(NULL), m_acceptor(NULL)
{
	Init();
}

ServerSocket::~ServerSocket()
{
	Destroy();
}

void ServerSocket::Init()
{
 
}

void ServerSocket::Destroy()
{
	CloseSocket();
	::WaitForSingleObject(m_hThread, 1000);
	if(m_packet)
	{
		delete m_packet;
		m_packet = NULL;
	}

	if(m_acceptor)
	{
		delete m_acceptor;
		m_acceptor = NULL;
	}
}

void ServerSocket::CloseSocket()
{
	if(m_socket != INVALID_SOCKET)
	{
		closesocket(m_socket);
		m_socket = INVALID_SOCKET;
	}
}

bool ServerSocket::Start(const char *iip, const int port, const int backlog)
{
	sockaddr_in sockAddr;
	bool reuse = true, keepAlive = true;

	::ZeroMemory(&sockAddr,sizeof(sockAddr));
	sockAddr.sin_family			= AF_INET;
	inet_pton(AF_INET, iip, &sockAddr.sin_addr);
	sockAddr.sin_port			= ::htons(port);

	m_socket = ::socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if(m_socket == INVALID_SOCKET)
	{
		PrintTimeAndLog(0,"ServerSocket FAILED:%d",WSAGetLastError());
		return false;
	}

	::setsockopt(m_socket,SOL_SOCKET,SO_REUSEADDR,(char*)&reuse,sizeof(reuse));
	::setsockopt(m_socket,SOL_SOCKET,SO_KEEPALIVE,(char*)&keepAlive,sizeof(keepAlive));

	if(::bind(m_socket,(sockaddr*)&sockAddr,sizeof(sockAddr)) == SOCKET_ERROR || ::listen(m_socket, backlog) == SOCKET_ERROR)
	{
		PrintTimeAndLog(0,"ServerSocket bind - listen FAILED:%d",WSAGetLastError());
		return false;
	}
	
	Begin();
	PrintTimeAndLog(0,"Server Start : %s : %d",iip,port);
	return true;
}

void ServerSocket::SetAcceptor(AcceptorNode* node, const uint32 packetId)
{ 
	if(NULL == m_acceptor)
	{
		m_acceptor = node; 
		m_packet = new CPacket(packetId);
	}
}

SOCKET ServerSocket::AcceptConnection(const uint32 time)
{
	return ::accept(m_socket,0,0);
}

void ServerSocket::AcceptSession(SOCKET socket)
{
	if(m_acceptor)
	{
		m_acceptor->ReceivePacket(*m_packet, socket);
	}
	else
	{
		// 2012-07-02 youngdie, acceptor가 없을 경우 클라이언트를 받아들이지 않는다
		closesocket(socket);
	}
}

void ServerSocket::Run()
{
	while( TRUE )
	{	
		SOCKET socket = AcceptConnection( INFINITE );			
		if( INVALID_SOCKET != socket )
		{
			AcceptSession( socket );
		}
	}
	PrintTimeAndLog(0, "ioClientBind Thread EXIT");
}