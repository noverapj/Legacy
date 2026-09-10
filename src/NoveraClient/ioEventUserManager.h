#pragma once


#include "ioEventManager.h"

class EventUserNode;
class EventUserManager
{
protected:
	typedef std::vector< EventUserNode* > EventUserNodeVec;
	EventUserNodeVec m_EventUserNodeVec;

	EventUserNode *CreatEventUserNode( EventType eEventType );

public:
	void Init();
	void Clear();
	int GetSize() const;
	int GetNodeSize( EventType eEventType ) const;
	EventType GetType( int iArray );

	void SetValue( EventType eEventType, int iArray, int iValue );
	int  GetValue( EventType eEventType, int iArray );
	void SetIndex( EventType eEventType, DWORD dwIndex );
	EventUserNode *GetEventUserNode( EventType eEventType );

	void GetSameClassEventTypeVec( IN EventType eParentEventType, OUT IntVec &rvEventTypeVec ); 

public:
	EventUserManager();
	virtual ~EventUserManager();
};

