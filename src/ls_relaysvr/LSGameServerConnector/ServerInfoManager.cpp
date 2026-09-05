#include "stdafx.h"
#include "ServerInfoManager.h"


ServerInfoManager::ServerInfoManager(void)
{
	Init();
}


ServerInfoManager::~ServerInfoManager(void)
{
	m_serverInfos.clear();
}

bool ServerInfoManager::Init()
{
 	int max = g_Config.NGameServerMax();
 	m_serverInfos.reserve(max);
	return true;
}
 
int ServerInfoManager::GetSize()
{
	return m_serverInfos.size();
}

void ServerInfoManager::Print()
{
	for(int i=0; i<(int)m_serverInfos.size(); ++i)
	{
		if(m_serverInfos[i].serverState != ESOCKET::LS_DISCONNECTED)
			Debug(_T("%s:%d::User:%d\n"),m_serverInfos[i].ipAddr,m_serverInfos[i].port,m_serverInfos[i].userCount);
	}
}

void ServerInfoManager::AddServerInfo( int serverid )
{

}

bool ServerInfoManager::DelServerInfo(int serverId)
{
	for(int i=0; i<(int)m_serverInfos.size(); ++i)
	{
		if(m_serverInfos[i].serverId == serverId )
		{
			ServerInfo_& stdata = m_serverInfos[i];
			stdata.userCount = 0;
			stdata.roomCount = 0;
			stdata.serverState = ESOCKET::LS_DISCONNECTED;
			stdata.serverId = -1;
			break;
		}
	}
	return true;
}

void ServerInfoManager::PrintSendCount()
{
	for(int i=0; i<(int)m_serverInfos.size(); ++i)
	{
		if(m_serverInfos[i].serverState != ESOCKET::LS_DISCONNECTED)
			Debug(_T("%s:%d::SendCount:%d\n"), m_serverInfos[i].ipAddr,m_serverInfos[i].port,m_serverInfos[i].sendCount);
	}
}

