#include "stdafx.h"
#include "cSessionTable.h"


cSessionTable g_sessionTable;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

cSessionTable::cSessionTable(void)
{
	Init();
}

cSessionTable::~cSessionTable(void)
{
	Destroy();
}

void cSessionTable::Init()
{
}

void cSessionTable::Destroy()
{
	m_sessions.clear();
}

//////////////////////////////////////////////////////////////////////
// Operation
//////////////////////////////////////////////////////////////////////

BOOL cSessionTable::Insert(const uint32 entity, IIocpContext* context)
{
	cSession* session = Get(entity);
	if(!session)
	{
		session = new cSession;
		session->SetContext(context);
		session->OnConnect();

		m_sessions[entity] = session;
		return TRUE;
	}
	return FALSE;
}

void cSessionTable::Remove(const uint32 entity)
{
	cSession* session = Get(entity);
	if(!session) return;
	
	SESSION_TABLE::iterator it = m_sessions.find(entity);
	if(it != m_sessions.end())
	{
		m_sessions.erase(it);
	}

	switch(session->GetType())
	{
	case SESSION_LOGIN :
	case SESSION_PLAYER :
	case SESSION_SERVER :
	default :
		{
			delete session;
		}
		break;
	}
}

cSession* cSessionTable::Get(const uint32 entity)
{
	SESSION_TABLE::iterator it = m_sessions.find(entity);
	return (it != m_sessions.end()) ? it->second : NULL;
}

cSession* cSessionTable::GetFirst()
{
	m_iterator = m_sessions.begin();
	return (m_iterator != m_sessions.end()) ? m_iterator->second : NULL;
}

cSession* cSessionTable::GetNext()
{
	++m_iterator;
	return (m_iterator != m_sessions.end()) ? m_iterator->second : NULL;
}

