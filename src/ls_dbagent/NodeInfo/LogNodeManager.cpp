#include "../stdafx.h"
#include "LogNode.h"
#include "LogNodeManager.h"
#include "../Network/ioLogServer.h"


#define CLOG_MAX 500 // 추후 ini? 로 수정? 

LogNodeManager::LogNodeManager(void)
{
	m_iSendBufferSize = 0;
	m_iRecvBufferSize = 0;
}

LogNodeManager::~LogNodeManager(void)
{
}

void LogNodeManager::InitMemoryPool()
{
	char szCurrentPath[MAX_PATH] = "";
	GetCurrentDirectory(MAX_PATH, szCurrentPath);
	strcat_s(szCurrentPath, "\\ls_config_dba.ini");

	int iSessionCount = 100; //어떻게할지 정할것 
	m_iSendBufferSize = GetPrivateProfileInt( "Session Buffer", "SendBufferSize", 16384, szCurrentPath );	
	m_iRecvBufferSize = GetPrivateProfileInt( "Session Buffer", "RecvBufferSize", 16384, szCurrentPath );	

	m_MemNode.CreatePool( 0, iSessionCount, TRUE );
	for(int i = 0;i < iSessionCount;i++)
	{
		m_MemNode.Push( new LogNode( INVALID_SOCKET, m_iSendBufferSize, m_iRecvBufferSize ) );
	}

	m_MemCLog.CreatePool(100,CLOG_MAX+1,TRUE);
}

void LogNodeManager::ReleaseMemoryPool()
{
	POSITION pos = m_logNodes.GetHeadPosition();

	while(pos)
	{
		LogNode* node = m_logNodes.GetAt(pos);

		m_MemNode.Push(node);

		m_logNodes.RemoveAt(pos);
		m_logNodes.GetNext(pos);
	}

	m_MemNode.DestroyPool();
}

LogNode* LogNodeManager::CreateLogNode(SOCKET fd)
{
	LogNode *newNode = m_MemNode.Pop();

	if(newNode == NULL)
	{
		newNode = new LogNode( INVALID_SOCKET, m_iSendBufferSize, m_iRecvBufferSize );
		if(newNode == NULL )
		{
			LOG.PrintTimeAndLog(0,"UserNodeManager::CreateNewNode MemPool Zero!");
			return NULL;
		}
	}
	newNode->SetSocket(fd);
	newNode->OnCreate();

	return newNode;
}

void LogNodeManager::AddLogNode( LogNode* logNode )
{
	m_logNodes.AddTail(logNode);
}

void LogNodeManager::DelLogNode( LogNode* logNode )
{
	POSITION pos = m_logNodes.Find(logNode);

	if(pos)
	{
		LogNode* node = m_logNodes.GetAt(pos);
		m_MemNode.Push(node);
		m_logNodes.RemoveAt(pos);
	}
}

CLog* LogNodeManager::CreateCLog()
{
	CLog* logTmp = m_MemCLog.Pop();
	return logTmp;
}

void LogNodeManager::PushCLog( CLog* logTmp )
{
	logTmp->InitData();

	m_MemCLog.Push(logTmp);
}

void LogNodeManager::CloseAllNodeFiles( char* closeString )
{
	POSITION pos = m_logNodes.GetHeadPosition();

	while(pos)
	{
		LogNode* node = m_logNodes.GetAt(pos);
		node->CloseFiles(closeString);
		m_logNodes.GetNext(pos);
	}
}

