#pragma once
#include <hash_map>

class ServerInfoManager
{
public:
	ServerInfoManager(void);
	virtual ~ServerInfoManager(void);

public:
	bool Init();
	int GetSize();
	void AddServerInfo(int serverid);
	bool DelServerInfo(int serverId);
	void Print();
	void PrintSendCount();
protected:
	std::vector<ServerInfo_*>  m_serverInfos;
  

};
