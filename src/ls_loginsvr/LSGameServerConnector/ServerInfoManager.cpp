#include "stdafx.h"
#include "ServerInfoManager.h"


ServerInfoManager::ServerInfoManager(void)
{
	Init();
}


ServerInfoManager::~ServerInfoManager(void)
{
	Destroy();
}

void ServerInfoManager::Init()
{
 	m_serverInfos.reserve( g_Config()->NGameServerMax() );
}

void ServerInfoManager::Destroy()
{
	m_serverInfos.clear();
}

void ServerInfoManager::AddServerInfo(int serverId, int sockState, const TCHAR* ipAddr, int port, int serverIndex)
{
	ServerInfo_* serverInfo = Find( -1 );
	if(serverInfo)
	{
		serverInfo->serverIndex	= serverIndex;
		serverInfo->serverId	= serverId;
		serverInfo->socketState	= sockState;
		serverInfo->serverState	= ESVRSTATE::LS_RESPSERVERSTATE;
		serverInfo->port		= port;
		strcpy_s(serverInfo->ipAddr, ipAddr);
	}
	else
	{
		ServerInfo_ stdata;
		stdata.serverId		= serverId;
		stdata.socketState	= sockState;
		stdata.serverState	= ESVRSTATE::LS_RESPSERVERSTATE;
		stdata.serverIndex	= serverIndex;
		stdata.port			= port;
		strcpy_s(stdata.ipAddr,ipAddr);

		m_serverInfos.push_back(stdata);
	}
}

void ServerInfoManager::DelServerInfo(int serverId)
{
	ServerInfo_* serverInfo = Find(serverId);
	if(serverInfo)
	{
		serverInfo->Clear();
	}
}

void ServerInfoManager::UpdateServerInfo(int serverId, TCHAR* publicAddr, int gameIndex, int clientPort, int userCount, int serverState)
{
	ServerInfo_* serverInfo = Find(serverId);
	if(serverInfo)
	{
		serverInfo->gameIndex	= gameIndex;
		serverInfo->userCount	= userCount;
		serverInfo->serverState	= serverState;
		serverInfo->csport		= clientPort;
		strcpy_s(serverInfo->publicAddr, publicAddr);
	}
}

void ServerInfoManager::UpdateServerState(int serverId, int serverState)
{
	ServerInfo_* serverInfo = Find(serverId);
	if(serverInfo)
	{
		serverInfo->serverState = serverState;
	}
}

void ServerInfoManager::UpdateSockState( int serverId, int sockState )
{
	ServerInfo_* serverInfo = Find(serverId);
	if(serverInfo)
	{
		serverInfo->socketState = sockState;
	}
}

void ServerInfoManager::TranslateIndex(int serverIndex, int& gameIndex)
{
	for(int i=0; i<(int)m_serverInfos.size();++i)
	{
		if(m_serverInfos[i].serverIndex == serverIndex)
		{
			gameIndex = m_serverInfos[i].gameIndex;
			return;
		}
	}

	gameIndex = 0;
}

void ServerInfoManager::MakeServerInfo( SP2Packet& pk )
{
	for(int i=0 ; i<(int)m_serverInfos.size(); i++)
	{
		pk << m_serverInfos[i].serverIndex;
		pk << m_serverInfos[i].userCount;
		pk << m_serverInfos[i].sendCount;
		pk << m_serverInfos[i].serverState;
	}
}

int ServerInfoManager::GetUserCount()
{
	int userCount = 0;
	for(SERVERINFOS::iterator it = m_serverInfos.begin() ; it != m_serverInfos.end() ; ++it)
	{
		ServerInfo_* serverInfo = &(*it);
		if(serverInfo->socketState != ESOCKET::LS_DISCONNECTED)
		{
			userCount += serverInfo->userCount;
		}
	}
	return userCount;
}

typedef pair<int, int>					SERVERTOUSERCOUNT;
typedef std::vector<SERVERTOUSERCOUNT>	SERVERTOUSERCOUNTS;

bool CounterSort(const SERVERTOUSERCOUNT& lhs, const SERVERTOUSERCOUNT& rhs) 
{ 
	return lhs.second < rhs.second; 
} 

int SortByUserCount(SERVERTOUSERCOUNTS& Counters, const int disperseCount)
{
	if( Counters.size() > disperseCount)
	{
		// 정렬
		std::sort( Counters.begin(), Counters.end(), CounterSort );
	}

	// 분산개수에서 랜덤으로 선택
	int iIndex = rand() % disperseCount;
	SERVERTOUSERCOUNT ServerToUserCount = Counters[iIndex];

	int serverId = ServerToUserCount.first;
	return (serverId);
}

ServerInfo_* ServerInfoManager::GetIdleNode()
{
	if(GetSize() == 0)	return NULL;

	SERVERTOUSERCOUNTS Counters;
	Counters.reserve(GetSize());

	for(SERVERINFOS::iterator it = m_serverInfos.begin() ; it != m_serverInfos.end() ; ++it)
	{
		ServerInfo_& serverInfo = (*it);
		if(!IsActive(serverInfo))
			continue;

		Counters.push_back( make_pair(serverInfo.serverId, serverInfo.userCount) );
	}

	int serverId = SortByUserCount(Counters, FREE_SERVERCOUNT);
	return Find( serverId );
}

BOOL ServerInfoManager::IsActive( ServerInfo_ &info )
{
	if(info.userCount >= g_Config()->ServerFullCount())		return FALSE;
	if(info.socketState == ESOCKET::LS_DISCONNECTED)		return FALSE;
	if(info.serverState == ESVRSTATE::LS_ZOMBIESERVERSTATE) return FALSE;
	if(info.serverState == ESVRSTATE::LS_BLOCKED)			return FALSE;
	return TRUE;
}

void ServerInfoManager::Print()
{
	for(SERVERINFOS::iterator it = m_serverInfos.begin() ; it != m_serverInfos.end() ; ++it)
	{
		ServerInfo_* serverInfo = &(*it);
		if(serverInfo->socketState != ESOCKET::LS_DISCONNECTED)
		{
			Debug(_T("%s:%d::User:%d::SendCount:%d\n"),
				serverInfo->ipAddr, 
				serverInfo->port, 
				serverInfo->userCount, 
				serverInfo->sendCount);
		}
	}
}

ServerInfo_* ServerInfoManager::Find(const int serverId)
{
	for(SERVERINFOS::iterator it = m_serverInfos.begin() ; it != m_serverInfos.end() ; ++it)
	{
		ServerInfo_* serverInfo = &(*it);
		if(serverId == serverInfo->serverId )
		{
			return serverInfo;
		}
	}
	return NULL;
}