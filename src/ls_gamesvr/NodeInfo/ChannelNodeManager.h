#ifndef _ChannelNodeManager_h_
#define _ChannelNodeManager_h_

#include "ChannelCopyNode.h"
class ChannelNode;
typedef vector<ChannelNode*> vChannelNode;
typedef vChannelNode::iterator vChannelNode_iter;

class ChannelNodeManager
{
	static ChannelNodeManager *sg_Instance;
	
protected:
	// 원본
	vChannelNode            m_vChannelNode;	
	MemPooler< ChannelNode >	m_MemNode;
	
	// 복사본
	vChannelCopyNode		m_vChannelCopyNode;

	DWORD                   m_dwCurTime;
	
public:
	ChannelNode *CreateNewNode();
	void RemoveNode( DWORD dwChannelIndex );

	void AddCopyChannel( ChannelCopyNode *pChannel );
	void RemoveCopyChannel( DWORD dwIndex );
	
public:
	int RemainderNode(){ return m_MemNode.GetCount(); }
	
public:
	int GetNodeSize(){ return m_vChannelNode.size(); }
	int GetCopyChannelNodeSize(){ return m_vChannelCopyNode.size(); }
	
public:
	ChannelNode* GetChannelNode( DWORD dwIndex );  
	ChannelParent* GetGlobalChannelNode( DWORD dwIndex );
	
public:  // 서버 동기화
	void ConnectServerNodeSync( ServerNode *pServerNode );

public:
	void ChannelNode_AllTimeExit(void);
	
public:
	static ChannelNodeManager &GetInstance();
	static void ReleaseInstance();
		
public:
	void InitMemoryPool( const DWORD dwServerIndex );
	void ReleaseMemoryPool();

private:
	ChannelNodeManager();
	virtual ~ChannelNodeManager();	
};
#define g_ChannelNodeManager ChannelNodeManager::GetInstance()
#endif
