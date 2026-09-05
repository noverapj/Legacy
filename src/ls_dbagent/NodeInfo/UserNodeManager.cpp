// UserNodeManager.cpp: implementation of the UserNodeManager class.
//
//////////////////////////////////////////////////////////////////////

#include "../stdafx.h"
#include "User.h"
#include "UserNodeManager.h"
#include "../Network/DBServer.h"
UserNodeManager *UserNodeManager::sg_Instance = NULL;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

UserNodeManager::UserNodeManager()
{
	m_vUserNode.reserve(1000);
	m_iSendBufferSize = 0;
	m_iRecvBufferSize = 0;
}

UserNodeManager::~UserNodeManager()
{
	m_vUserNode.clear();
}

UserNodeManager &UserNodeManager::GetInstance()
{
	if(sg_Instance == NULL)
		sg_Instance = new UserNodeManager;
	return *sg_Instance;
}

void UserNodeManager::ReleaseInstance()
{
	SAFEDELETE(sg_Instance);
}

void UserNodeManager::InitMemoryPool()
{
	char szCurrentPath[MAX_PATH] = "";
	GetCurrentDirectory(MAX_PATH, szCurrentPath);
	strcat_s(szCurrentPath, "\\ls_config_dba.ini");

	int iSessionCount = GetPrivateProfileInt( "Session Buffer", "SessionCount", 10, szCurrentPath );	
	m_iSendBufferSize = GetPrivateProfileInt( "Session Buffer", "SendBufferSize", 16384, szCurrentPath );	
	m_iRecvBufferSize = GetPrivateProfileInt( "Session Buffer", "RecvBufferSize", 16384, szCurrentPath );	
	
	m_MemNode.CreatePool( 0, iSessionCount, TRUE );
	for(int i = 0;i < iSessionCount;i++)
	{
		m_MemNode.Push( new User( INVALID_SOCKET, m_iSendBufferSize, m_iRecvBufferSize ) );
	}
}

void UserNodeManager::ReleaseMemoryPool()
{
	vUser_iter iter = m_vUserNode.begin();
	vUser_iter iter_Prev;
	
	while(iter != m_vUserNode.end())
	{
		iter_Prev = iter;
		User *item = *iter_Prev;
		item->OnDestroy();
		m_MemNode.Push(item);
		m_vUserNode.erase(iter_Prev);
	}
	m_MemNode.DestroyPool();
}

User* UserNodeManager::CreateNewNode(SOCKET s)
{
	User *newNode = m_MemNode.Remove();
	if(newNode == NULL)
	{
		newNode = new User( INVALID_SOCKET, m_iSendBufferSize, m_iRecvBufferSize );
		if(newNode == NULL )
		{
			LOG.PrintTimeAndLog(0,"UserNodeManager::CreateNewNode MemPool Zero!");
			return NULL;
		}
	}

	newNode->SetSocket(s);
	newNode->OnCreate();
	return newNode;
}

void UserNodeManager::AddUserNode(User *usernode)
{
	m_vUserNode.push_back(usernode);
}

void UserNodeManager::RemoveNode(User *usernode)
{
	vUser_iter iter = m_vUserNode.begin();
	vUser_iter iter_Prev;
	
	while(iter != m_vUserNode.end())
	{
		iter_Prev = iter++;
		User *item = *iter_Prev;
		if(item == usernode)
		{
			LOG.PrintTimeAndLog( 0, "Disconnected - IP(%s), IP(%s), Port(%d)", item->GetPublicIP(), item->GetPrivateIP(), item->GetUDP_port() );
			item->OnDestroy();
			m_MemNode.Push(item);
			m_vUserNode.erase(iter_Prev);
			break;
		}
	}
}