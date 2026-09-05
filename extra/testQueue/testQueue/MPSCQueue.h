#pragma once

#include "DATA.h"

class WFQueue
{
public:
	WFQueue(void);
	virtual ~WFQueue(void);

	bool Init();

public:
	bool Enqueue( NodeData *node );
	NodeEntry* Dequeue();

	int GetSize() { return m_count;}

protected:
	void PushNode(NodeEntry* element);

private:
	NodeEntry* volatile  m_head; 
	NodeEntry*           m_tail; 
	NodeEntry            m_stub; 
	long	m_count;
	__int64 m_offset;
};

