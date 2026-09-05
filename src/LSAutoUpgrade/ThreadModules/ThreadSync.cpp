#include "../stdafx.h"
#include "ThreadSync.h"
#include "SuperParent.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ThreadSync::ThreadSync(SuperParent *pParent) : m_pSuperParent(pParent)
{
	if(m_pSuperParent == NULL) return;

	EnterCriticalSection(&m_pSuperParent->m_critical_section);
}

ThreadSync::~ThreadSync()
{
	if(m_pSuperParent == NULL) return;

	LeaveCriticalSection(&m_pSuperParent->m_critical_section);
}
