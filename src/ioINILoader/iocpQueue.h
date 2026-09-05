#pragma once

class iocpQueue
{
public:
	iocpQueue(void);
	~iocpQueue(void);

public:
	bool	Startup();
	void	Cleanup();

public:
	LPDWORD	Dequeue();
	LPDWORD	Dequeue( DWORD &bytes );

	LONG GetCount()	{ return m_count; }
	HANDLE GetHandle() { return m_completionPort; }
public:
	BOOL Enqueue( const DWORD completionKey, const DWORD bytes );
	long GetProcessCount()
	{
		long val = m_processCount;
		InterlockedExchange(&m_processCount,0);
		return val;
	}

public:
	volatile LONG m_count;

protected:
	int		m_timeout;
	HANDLE	m_completionPort;
	long	m_processCount;
};
