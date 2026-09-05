#pragma once

#include "MemPooler.h"

struct TQueueElement_
{
	DWORD data;
	TQueueElement_* volatile  next; 
}; 

class TMPSCQueue
{
public:
	TMPSCQueue(void);
	virtual ~TMPSCQueue(void);
	bool Init(int seed,int timeout = 0);
	void ReleaseMemoryPool();
public:
	bool Enqueue(DWORD inputData);
	LPDWORD Dequeue();
	LPDWORD WaitDequeue();
	int GetSize() { return m_count;}

protected:
	void PushNode(TQueueElement_* element);

private:
	TQueueElement_* volatile  m_head; 
	TQueueElement_*           m_tail; 
	TQueueElement_            m_stub; 
	MemPooler<TQueueElement_> m_pool;
	long m_count;
	int m_timeout;

};

