#pragma once

class cIocpQueue  
{
public:
	cIocpQueue();
	virtual ~cIocpQueue();

	BOOL	Startup( int32 timeout = INFINITE );
	void	Cleanup();

public:
	LPDWORD	Dequeue();
	LPDWORD	Dequeue( DWORD &bytes );

	LONG GetCount()	{ return m_count; }

protected:
	BOOL Enqueue( const DWORD completionKey, const DWORD bytes );

public:
	volatile LONG m_count;

protected:
	int32	m_timeout;
	HANDLE	m_completionPort;
};