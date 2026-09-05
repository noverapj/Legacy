#pragma once

#include <vector>
#include "cServerSocket.h"

class cServerSocketManager
{
public:
	cServerSocketManager(void);
	~cServerSocketManager(void);

	void Init();
	void Destroy();

public:
	BOOL AddSocket(const uint16 port, const int32 backlog, uint32& ID);
	cServerSocket* GetSocket(const uint32 ID);

	const uint32 GetCount()	{ return m_sockets.size(); }

protected:
	typedef std::vector<cServerSocket*> SERVERSOCKETS;
	SERVERSOCKETS m_sockets;
};
