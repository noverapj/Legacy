#pragma once


class ioQueue
{
public:
	ioQueue(void);
	virtual ~ioQueue(void);
	BOOL	Startup( int32 timeout = INFINITE );
	void	Cleanup();

public:
	LPDWORD	Dequeue();
	LPDWORD	Dequeue( DWORD &bytes, LPOVERLAPPED &ov, int &retVal);
	LONG GetCount()	{ return m_count; }

public:
	BOOL Enqueue( const DWORD completionKey, const DWORD bytes );
	BOOL Enqueue( const DWORD completionKey, const DWORD bytes, LPOVERLAPPED ov); //for socketOperation
	HANDLE CompletionPort() const { return m_completionPort; }
	void CompletionPort(HANDLE val) { m_completionPort = val; }
public:
	volatile LONG m_count;

protected:
	int32	m_timeout;
	HANDLE	m_completionPort;

};
