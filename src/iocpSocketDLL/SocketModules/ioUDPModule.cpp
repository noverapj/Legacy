#include "../iocpSocketDLL.h"
#include "ioUDPModule.h"


ioUDPModule::ioUDPModule(void)
{
}
ioUDPModule::~ioUDPModule(void)
{
}
bool ioUDPModule::InitIOCP( int workerCount )
{
	if(!CreateIOCP())
		return false;
	CreateWorkers(workerCount);
	return true;
}

bool ioUDPModule::SetUDPModule( std::vector<int>& recvPorts,
								std::string& ipAddr,
								int sendPort,
								UDPNode *node,
								int workerCount )
{
	std::vector<SOCKINFOPAIR> socketPairs;
	if(InitIOCP(workerCount) == false)
	{
		DebugLog(0,__FILE__,__LINE__,"InitIOCP Fail");
		return false;
	}	
	if(InitSocket(recvPorts, ipAddr, node, socketPairs,sendPort) == false)
	{
		DebugLog(0,__FILE__,__LINE__,"InitSocket Fail");
		return false;
	}

	socketPairs.clear();
	ipAddr.clear();
	return true;
}

bool ioUDPModule::InitSocket( std::vector<int>& recvPorts, 
							  std::string &ipAddr, 
							  UDPNode * node, 
							  std::vector<SOCKINFOPAIR> &socketPairs,
							  int sendPort )
{
	SOCKADDR_IN addr;
	int addrSize = sizeof(addr);
	SOCKET recvSocket;
	SOCKET sendSocket = INVALID_SOCKET;
	for(unsigned int i=0; i<recvPorts.size(); ++i)
	{
		
		SOCKINFOPAIR pairData;
		int &nport = recvPorts[i];
		ZeroMemory(&addr,addrSize);

		inet_pton(AF_INET, ipAddr.c_str(), &addr.sin_addr);
		addr.sin_family = AF_INET;
		addr.sin_port = htons(nport);
		recvSocket = SetSocket(addr, addrSize, node);
		if(i == 0)
			sendSocket = recvSocket;
		if(recvSocket== INVALID_SOCKET)
			return false;

		pairData.first = recvSocket;
		memcpy(&pairData.second,&addr,addrSize);
		socketPairs.push_back(pairData);
	}
 
	ZeroMemory(&addr,addrSize);
	inet_pton(AF_INET, ipAddr.c_str(), &addr.sin_addr);
	addr.sin_family = AF_INET;
	addr.sin_port = htons(sendPort);
	
	//	SetSocket(addr,addrSize,node);
	if(sendSocket == INVALID_SOCKET)
	{
		return false;
	}
	node->SetSockInfo(socketPairs,sendSocket);
	node->SetAddr(ipAddr,recvPorts);
	return true;
}

SOCKET ioUDPModule::SetSocket(SOCKADDR_IN& addr, int addrSize, UDPNode * node )
{
	SOCKET sock = WSASocketW(AF_INET, SOCK_DGRAM, IPPROTO_UDP, NULL, NULL, WSA_FLAG_OVERLAPPED);
	if(sock == INVALID_SOCKET)
	{
		DebugLog(0,__FILE__,__LINE__,"WSASocket Create Fail");
		return INVALID_SOCKET;
	}
	if(bind(sock,reinterpret_cast<sockaddr*>(&addr),addrSize) == SOCKET_ERROR)
	{
		closesocket(sock);
		DebugLog(0,__FILE__,__LINE__,"WSASocket Bind Fail");
		//WSACleanup();
		return INVALID_SOCKET;
	}
	AddHandleToIOCP(reinterpret_cast<HANDLE>(sock),reinterpret_cast<DWORD>(node));
	return sock;
}

