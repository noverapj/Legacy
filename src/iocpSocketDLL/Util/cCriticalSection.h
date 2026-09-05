#pragma once

class cCriticalSection  
{
public:
	cCriticalSection()	
	{
		InitializeCriticalSection( &m_criticalSection );
	}

	~cCriticalSection()	{ DeleteCriticalSection( &m_criticalSection );	}

public:
	void Lock()		{ EnterCriticalSection( &m_criticalSection );	}
	void Unlock()	{ LeaveCriticalSection( &m_criticalSection );	}

private:
	CRITICAL_SECTION m_criticalSection;
};

