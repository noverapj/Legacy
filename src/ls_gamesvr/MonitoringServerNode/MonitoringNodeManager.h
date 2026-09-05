#ifndef __MonitoringNodeManager_h__
#define __MonitoringNodeManager_h__

#include "MonitoringNode.h"

using namespace std;

typedef vector< MonitoringNode * > vMonitoringNode;
typedef vMonitoringNode::iterator vMonitoringNode_iter;

class MonitoringNodeManager
{
protected:
	static MonitoringNodeManager *sg_Instance;
	vMonitoringNode	              m_vNode;
	MemPooler< MonitoringNode >	m_MemNode;

public:
	static MonitoringNodeManager &GetInstance();
	static void ReleaseInstance();

public:
	void InitMemoryPool();
	void ReleaseMemoryPool();

public:
	MonitoringNode *CreateNode( SOCKET s );
	void AddNode( MonitoringNode *pNewNode );
	void RemoveNode( MonitoringNode *pServerNode );

public:
	int RemainderNode(){ return m_MemNode.GetCount(); }
	int GetNodeSize(){ return m_vNode.size(); }

public:
	void ProcessFlush();

public:
	void SendMessageAllNode( SP2Packet &rkPacket );

public:
	MonitoringNodeManager(void);
	virtual ~MonitoringNodeManager(void);
};
#define g_MonitoringNodeManager MonitoringNodeManager::GetInstance()
#endif // __MonitoringNodeManager_h__