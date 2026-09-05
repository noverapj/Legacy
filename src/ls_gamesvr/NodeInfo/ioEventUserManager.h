#ifndef __ioEventUserManager_h__
#define __ioEventUserManager_h__

#include "ioEventManager.h"

class EventUserNode;

class EventUserManager
{
protected:
	typedef std::vector< EventUserNode* > EventUserNodeVec;
	EventUserNodeVec m_EventUserNodeVec;

	EventUserNode *CreatEventUserNode( EventType eEventType, ModeCategory eModeCategory );

public:
	void Init();
	void Clear();
	int GetSize() const;
	int GetNodeSize( EventType eEventType ) const;
	EventType GetType( int iArray );

	void SetValue( EventType eEventType, int iArray, int iValue );
	int  GetValue( EventType eEventType, int iArray );
	void SetIndex( EventType eEventType, DWORD dwIndex );

	void Save( DWORD dwAgentID, const DWORD dwThreadID );
	void Backup();
	void UpdateFirst( User *pUser );

	void FillMoveData( SP2Packet &rkPacket );
	void ApplyMoveData( SP2Packet &rkPacket );
	void Process( User *pUser );     // 게임플레이 체크
	void ProcessTime( User *pUser ); // 접속시간 체크
	void SendData( User *pUser );

	void InsertDB( DWORD dwAgentID, const DWORD dwThreadID, const char *szUserGUID, DWORD dwUserIndex );

	EventUserNode *GetEventUserNode( EventType eEventType, ModeCategory eModeCategory = MC_DEFAULT );

	void GetSameClassEventTypeVec( IN EventType eParentEventType, OUT IntVec &rvEventTypeVec ); 

public:
	EventUserManager();
	virtual ~EventUserManager();
};

#endif // __ioEventUserManager_h__