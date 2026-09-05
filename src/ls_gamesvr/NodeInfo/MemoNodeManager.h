#ifndef _MemoNodeManager_h_
#define _MemoNodeManager_h_


class User;

#define MAX_MEMO     10000

class MemoNode
{
protected:
	ioHashString m_ToID;
	ioHashString m_FromID;
	ioHashString m_Memo;
	DWORD        m_dwMemoTime;

public:
	void SetMemo( ioHashString ToID, ioHashString FromID, ioHashString Memo );

public:
	const ioHashString &GetToID(){ return m_ToID; }
	const ioHashString &GetFromID(){ return m_FromID; }
	const ioHashString &GetMemo(){ return m_Memo; }
	DWORD GetTime(){ return m_dwMemoTime; }

public:
	MemoNode();
	virtual ~MemoNode();
};
typedef vector< MemoNode > vMemoNode;
typedef vMemoNode::iterator vMemoNode_iter;
typedef vMemoNode::reverse_iterator vMemoNode_riter;

class MemoNodeManager
{
	static MemoNodeManager *sg_Instance;
	
protected:
	vMemoNode            m_vMemoNode;	
		
public:
	void AddMemo( MemoNode &rkMemo );

public:
	int GetNodeSize(){ return m_vMemoNode.size(); }

public:
	void SendMemo( UserParent *pUser, int iSendCount );

	
public:
	static MemoNodeManager &GetInstance();
	static void ReleaseInstance();
		
private:
	MemoNodeManager();
	virtual ~MemoNodeManager();	
};
#define g_MemoNodeManager MemoNodeManager::GetInstance()
#endif
