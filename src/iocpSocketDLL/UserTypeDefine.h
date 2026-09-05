#pragma once
#include <utility>
#include <WinSock2.h>
#include "SocketModules/ReceiveIO.h"


class NetworkSecurity;

#define RECVCOUNT_MAX 2000

struct UDPIoInfo
{
	CReceiveIO recvio;
	SOCKET     fd;
	SOCKADDR_IN addr;
	NetworkSecurity *m_pns;
};

struct BUF64_
{	
	char data[64];
};

struct BUF256_
{	
	char data[256];
};

struct BUF128_
{
	char data[128];
};
struct BUF1024_
{	
	char data[1024];
};
struct BUF2048_
{	
	char data[2048];
};

typedef std::pair<SOCKET,SOCKADDR_IN> SOCKINFOPAIR;