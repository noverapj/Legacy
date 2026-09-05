#ifndef _ServerNodeManager_h_
#define _ServerNodeManager_h_

#include "ServerNode.h"

typedef std::vector< ServerNode * > vServerNode;
typedef vServerNode::iterator vServerNode_iter;
class ServerNodeManager : public SuperParent
{
protected:
	static ServerNodeManager *sg_Instance;
	vServerNode	              m_vServerNode;
	MemPooler<ServerNode>	  m_MemNode;
	int						  m_iDestroyCount;
	DWORD					  m_dwCurrentTime;


public:
	static ServerNodeManager &GetInstance();
	static void ReleaseInstance();

public:
	void InitMemoryPool();
	void ReleaseMemoryPool();

public:
	ServerNode *CreateServerNode( SOCKET s );
	void AddServerNode( ServerNode *pNewNode );
	void RemoveNode( ServerNode *pServerNode );
	int RemainderNode()		 { return m_MemNode.GetCount(); }
	int GetNodeSize()		 { return m_vServerNode.size(); }
	int GetDestroyNodeSize() { return m_iDestroyCount; }
	void ServerNode_SendBufferFlush();

public:
	void ProcessServerNode();

public:
	bool SendMessageIP( ioHashString &rszIP, int iClientPort, SP2Packet &rkPacket );
	void SendLoginInfo();

private:     	/* Singleton Class */
	ServerNodeManager();
	virtual ~ServerNodeManager();
};
#define g_ServerNodeManager ServerNodeManager::GetInstance()
#endif