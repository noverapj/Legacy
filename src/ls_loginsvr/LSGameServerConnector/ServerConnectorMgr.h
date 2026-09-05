#pragma once

#include <vector>

class ServerConnectorMgr
{
public:
	ServerConnectorMgr(void);
	~ServerConnectorMgr(void);

protected:
	void Init();
	void Destroy();

public:
	void Connect(VIPADDR& vIpAddr);

	void AddConnector(LSConnector* node);
	void DelConnector(LSConnector* node);
	
	int GetNodeSize()		{return m_connectors.size();}

public:
	void RequestStatus();
	void RequestPartition();

	void Broadcast( SP2Packet &rkPacket );

public:
	void Flush();
	void Print();

private:
	BOOL RearrangePartition(int& partitionCount);

private:
	int m_partitions;

public:
	typedef std::vector<LSConnector*> LSCONNECTORS;
	LSCONNECTORS m_connectors;
};

