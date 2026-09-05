// UserNodeManager.h: interface for the UserNodeManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_USERNODEMANAGER_H__38D8F28B_C7F4_4DA8_8278_70870304D390__INCLUDED_)
#define AFX_USERNODEMANAGER_H__38D8F28B_C7F4_4DA8_8278_70870304D390__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
using namespace std;

class User;
typedef vector<User*> vUser;
typedef vUser::iterator vUser_iter;

class UserNodeManager : public SuperParent 
{
protected:
	static UserNodeManager *sg_Instance;
	vUser	                m_vUserNode;	
	MemPooler<User>			m_MemNode;
	int                     m_iSendBufferSize;
	int						m_iRecvBufferSize;

public:
	static UserNodeManager &GetInstance();
	static void ReleaseInstance();

public:
	void InitMemoryPool();
	void ReleaseMemoryPool();

public:
	User* CreateNewNode(SOCKET s);
	void AddUserNode(User *usernode);
	void RemoveNode(User *usernode);
	
public:
	int RemainderNode(){ return m_MemNode.GetCount(); }

public:
	int GetNodeSize(){ return m_vUserNode.size(); }
	
private:     	/* Singleton Class */
	UserNodeManager();
	virtual ~UserNodeManager();

};
#define g_UserNodeManager UserNodeManager::GetInstance()
#endif // !defined(AFX_USERNODEMANAGER_H__38D8F28B_C7F4_4DA8_8278_70870304D390__INCLUDED_)
