#ifndef __ThailandOTPNodeManager_h__
#define __ThailandOTPNodeManager_h__

#include "ThailandOTPNode.h"
#include "../../include/MemPooler.h"
class SP2Packet;

typedef std::vector< ThailandOTPNode* > vThailandOTPNode;
typedef vThailandOTPNode::iterator vThailandOTPNode_iter;

class ThailandOTPNodeManager : public SuperParent
{
protected:
	static ThailandOTPNodeManager *sg_Instance;
	vThailandOTPNode               m_vOTPNode;
	MemPooler<ThailandOTPNode>     m_MemNode;

public:
	static ThailandOTPNodeManager &GetInstance();
	static void ReleaseInstance();

public:
	void InitMemoryPool();
	void ReleaseMemoryPool();
	ThailandOTPNode * ConnectTo();
	void Process();
	void RemoveNode( ThailandOTPNode *pLoginNode );
	void Node_Destroy();

	int GetNodeSize(){ return m_vOTPNode.size(); }
	int RemainderNode(){ return m_MemNode.GetCount(); }
	
protected:
	ThailandOTPNode *CreateNode( SOCKET s );
	
private:     	/* Singleton Class */
	ThailandOTPNodeManager(void);
	virtual ~ThailandOTPNodeManager(void);
};
#define g_ThailandOTPNodeManager ThailandOTPNodeManager::GetInstance()

#endif // __ThailandOTPNodeManager_h__