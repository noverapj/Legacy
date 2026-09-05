#pragma once


#include <atlcoll.h>
#include "../../include/cSingleton.h"
#include "../../include/MemPooler.h"

class LogNode;

class LogNodeManager
{
public:
	LogNodeManager(void);
	virtual ~LogNodeManager(void);

public:
	void InitMemoryPool();
	void ReleaseMemoryPool();

public:
	LogNode* CreateLogNode(SOCKET fd);
	void AddLogNode(LogNode* logNode);
	void DelLogNode(LogNode* logNode);
	CLog* CreateCLog();
	void PushCLog(CLog* logTmp);
	void CloseAllNodeFiles(char* closeString);

public:
	int GetNodeSize() { return m_logNodes.GetCount(); }

public:
	typedef ATL::CAtlList<LogNode*> LogNodes;
protected:
	LogNodes	 m_logNodes;
	MemPooler<LogNode>	m_MemNode;
	MemPooler<CLog>     m_MemCLog; //작업할것
	int m_iSendBufferSize;
	int	m_iRecvBufferSize;
	
};

#define g_LogNodeManager cSingleton<LogNodeManager>::GetInstance()