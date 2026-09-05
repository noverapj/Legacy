#pragma once

#include <map>
#include <vector>
#include "cSession.h"


// 세션을 관리하는 테이블
class cSessionTable
{
public:
	cSessionTable(void);
	~cSessionTable(void);

	void Init();
	void Destroy();

public:
	BOOL Insert(const uint32 entity, IIocpContext* context);
	void Remove(const uint32 entity);

	uint32 GetCount()	{ return m_sessions.size(); }

	cSession* Get(const uint32 entity);
	cSession* GetFirst();
	cSession* GetNext();

private:
	typedef std::map<uint32,cSession*> SESSION_TABLE;

	SESSION_TABLE::iterator m_iterator;
	SESSION_TABLE m_sessions;
};

extern cSessionTable g_sessionTable;