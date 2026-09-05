#pragma once

#include "../iocp/UDPCompletionHandler.h"

class UDPNode;

class IOCP_SOCKET_API ioUDPModule : public UDPCompletionHandler
{
public:
	ioUDPModule(void);
	virtual ~ioUDPModule(void);

public:
	bool SetUDPModule(std::vector<int>& recvports,
					  std::string& ipAddr,
					  int sendPort,
					  UDPNode *node,
					  int workerCount);
	bool InitIOCP(int workerCount);
	bool InitSocket( std::vector<int>&recvPorts, 
					 std::string &ipAddr, 
					 UDPNode * node, 
					 std::vector<SOCKINFOPAIR> &socketPairs,
					 int sendPort);
	SOCKET SetSocket(SOCKADDR_IN& addr, int addrSize, UDPNode * node );
};

