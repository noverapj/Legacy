#pragma once


#include <hash_map>

class ServerConnectorMgr
{
public:
	ServerConnectorMgr(void);
	virtual ~ServerConnectorMgr(void);

public:
	void Init();

public:
	void InitMemoryPool();
	void ReleaseMemoryPool();

public: //fuction
	void PushClient(GameServerNode* node);
	void AddClient(GameServerNode* node);
	bool DelClient(GameServerNode* node);
	void CreateConnectoClients(ConnectIPAddrs& IpAddrs);
	void ConnectClient(SVRCONNECTINFO_& connectAddr);

public:
	void SendMessageAllNode( SP2Packet &sPacket );
	void SendBufferFlush();

public:
	GameServerNode* GetServerNodeByID(int serverID);
	int GetNodeSize() {return m_serverNodes.size();}
	int GetRoomCount();
	
public:
	std::vector<GameServerNode*> m_serverNodes;
	ConnectIPAddrs  m_ipAddrs;
	bool m_timerState;

};

