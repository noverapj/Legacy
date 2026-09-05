#pragma once

#include "PacketQueue.h"
#include <set>
class IOCP_SOCKET_API MPSCQueue // : public SuperParent
{
public:
	MPSCQueue(void);
	virtual ~MPSCQueue(void);

	BOOL Init();

public:
	BOOL Enqueue( NodeData *node );
	NodeEntry* Dequeue();

	void TestFunc( NodeEntry* node );

	int GetSize() { return m_count;}
	long GetProcessCount()
	{
		long val = m_processCount;
		InterlockedExchange(&m_processCount,0);
		return val;
	}
protected:
	void PushNode(NodeEntry* element);

private:
	NodeEntry* volatile  m_head; 
	NodeEntry*           m_tail; 
	NodeEntry            m_stub; 
	long	m_count;
	__int64 m_offset;
	long m_processCount;
};

