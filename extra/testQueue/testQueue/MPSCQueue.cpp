#include "stdafx.h"
#include "MPSCQueue.h"



WFQueue::WFQueue(void)
{	
	Init();
}

WFQueue::~WFQueue(void)
{
}

bool WFQueue::Init()
{
	m_head = m_tail = &m_stub;
	m_stub.next = NULL;
	m_count = 0;

	m_offset = reinterpret_cast<__int64>( &((reinterpret_cast<NodeData*>(0))->m_nodeEntry) ) ;
	return true;
}

bool WFQueue::Enqueue( NodeData* node )
{
	NodeEntry* prev = reinterpret_cast<NodeEntry*>(InterlockedExchangePointer((void*)(&m_head), (void*)(&node->m_nodeEntry)));
	prev->next = &(node->m_nodeEntry);
	InterlockedIncrement(&m_count);
	return true;
}

void WFQueue::PushNode( NodeEntry* element )
{
	element->next = NULL;
	NodeEntry* prev = reinterpret_cast<NodeEntry*>(InterlockedExchangePointer((void*)(&m_head), element));
	prev->next = element;
}

NodeEntry* WFQueue::Dequeue()
{  
	NodeEntry* tail = m_tail; 
	NodeEntry* next = tail->next; 
	if (tail == &m_stub) 
	{ 
		if (NULL == next) 
			return NULL; 
		m_tail = next; 
		tail = next; 
		next = next->next; 
	} 
	if (next) 
	{ 
		m_tail = next; 
		InterlockedDecrement(&m_count);
		NodeEntry* node = reinterpret_cast<NodeEntry*>(reinterpret_cast<__int64>(tail) - m_offset);
		return (node); 
	} 

	NodeEntry* head = m_head;
	if (tail != head) 
		return NULL; 

	PushNode(&m_stub); 

	next = tail->next; 
	if (next) 
	{ 
		m_tail = next; 
		InterlockedDecrement(&m_count);
		NodeEntry* node = reinterpret_cast<NodeEntry*>(reinterpret_cast<__int64>(tail) - m_offset);
		return (node); 
	} 
	return NULL;
}
