#ifndef _MgrToolNodeManager_h_
#define _MgrToolNodeManager_h_

#include "MgrToolNode.h"

using namespace std;

typedef vector< MgrToolNode * > vMgrToolNode;
typedef vMgrToolNode::iterator vMgrToolNode_iter;

class MgrToolNodeManager
{
protected:
	static MgrToolNodeManager *sg_Instance;
	vMgrToolNode	           m_vMgrToolNode;
	MemPooler<MgrToolNode>	   m_MemNode;

	DWORD					  m_dwCurrentTime;

public:
	static MgrToolNodeManager &GetInstance();
	static void ReleaseInstance();

public:
	void InitMemoryPool();
	void ReleaseMemoryPool();

public:
	MgrToolNode *CreateMgrToolNode( SOCKET s );

public:
	void AddNode( MgrToolNode *pNewNode );
	void RemoveNode( MgrToolNode *pNode );
	int RemainderNode()	{ return m_MemNode.GetCount(); }
	int GetNodeSize()	{ return m_vMgrToolNode.size(); }
	MgrToolNode* GetNode( const ioHashString &szGUID );
	
public:
	void ProcessMgrToolNode();
	void MgrToolNode_SendBufferFlush();

public:
	void SendMessageAllNode( SP2Packet &rkPacket );
	void SendMessageGUIDNode( ioHashString& szGUID, SP2Packet& rkPacket );

private:     	/* Singleton Class */
	MgrToolNodeManager();
	virtual ~MgrToolNodeManager();
};
#define g_MgrTool MgrToolNodeManager::GetInstance()
#endif