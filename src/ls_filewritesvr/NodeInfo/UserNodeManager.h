
#pragma once

#include "User.h"

typedef vector<User*> vUser;
typedef vUser::iterator vUser_iter;

class UserNodeManager : public SuperParent 
{
private:
	static UserNodeManager *sg_Instance;

	// 원본 유저 노드
	vUser	                m_vUserNode;
	//MemoryPool              m_MemNode;
	MemPooler<User>			m_MemNode;

	//
	DWORD                   m_current_timer;

	//
	int                     m_iMaxConnection;

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
	int GetMaxConnection(){ return m_iMaxConnection; }

public:
	int GetNodeSize(){ return m_vUserNode.size(); }

private:      //내부에서만 사용.
	User* GetDestroyNode( const ioHashString &szID );           //접속 종료 대기중 아이디

public:
	void UserNode_AllTimeExit(void);   
	void SendMessageAll( SP2Packet &rkPacket, User *pOwner = NULL );
	void UserNode_SendBufferFlush();

private:     	/* Singleton Class */
	UserNodeManager();
	virtual ~UserNodeManager();
};

#define g_UserNodeManager UserNodeManager::GetInstance()
