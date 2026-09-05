#include "../iocpSocketDLL.h"
#include "MPSCQueue.h"

MPSCQueue::MPSCQueue(void)
{	
	Init();	 
}

MPSCQueue::~MPSCQueue(void)
{
}

BOOL MPSCQueue::Init()
{
	m_head = m_tail = &m_stub;
	m_stub.next = NULL;
	m_count = 0;
	m_offset = reinterpret_cast<__int64>( &((reinterpret_cast<NodeData*>(0))->m_nodeEntry) ) ;
	m_processCount = 0;
	return TRUE;
}

BOOL MPSCQueue::Enqueue( NodeData* node )
{
	NodeEntry* prev = reinterpret_cast<NodeEntry*>(
		InterlockedExchangePointer(reinterpret_cast<void* volatile*>(&m_head),
			&node->m_nodeEntry));
	prev->next = &(node->m_nodeEntry);
	InterlockedIncrement(&m_count);
	return TRUE;
}

void MPSCQueue::PushNode( NodeEntry* element )
{
	element->next = NULL;
	NodeEntry* prev = reinterpret_cast<NodeEntry*>(
		InterlockedExchangePointer(reinterpret_cast<void* volatile*>(&m_head),
			element));
	prev->next = element;
}

NodeEntry* MPSCQueue::Dequeue()
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
		node->next = 0;
		InterlockedIncrement(&m_processCount);
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
		node->next = 0;
		InterlockedIncrement(&m_processCount);
		return (node); 
	} 
	return NULL;
}

 