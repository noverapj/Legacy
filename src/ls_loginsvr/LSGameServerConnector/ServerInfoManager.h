#pragma once

class ServerInfoManager
{
public:
	ServerInfoManager(void);
	virtual ~ServerInfoManager(void);

	void Init();
	void Destroy();

public:
	int GetSize()		{ return m_serverInfos.size(); }

	void AddServerInfo(int serverId,int sockState, const TCHAR* ipAddr, int port, int serverIndex);
	void DelServerInfo(int serverId);

	void UpdateServerInfo(int serverId, TCHAR* publicAddr, int gameIndex, int clientPort, int userCount, int serverState);
	void UpdateServerState(int serverId, int serverState);
	void UpdateSockState(int serverId, int sockState);
	
	void TranslateIndex(int serverIndex, int& gameIndex);
	void SortServerInfo();
	void MakeServerInfo(SP2Packet& pk);

	int GetUserCount();

public:
	ServerInfo_* GetIdleNode();
	BOOL IsActive( ServerInfo_ & info );

	void Print();

protected:
	ServerInfo_* Find(const int serverId);

protected:
	typedef std::vector<ServerInfo_> SERVERINFOS;
	SERVERINFOS m_serverInfos;
};
