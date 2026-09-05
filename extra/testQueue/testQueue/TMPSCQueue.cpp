#include "stdafx.h"
#include "TMPSCQueue.h"



TMPSCQueue::TMPSCQueue(void)
{	
	m_timeout = 0;
}

TMPSCQueue::~TMPSCQueue(void)
{
}

bool TMPSCQueue::Init( int seed, int timeout )
{
	m_head = m_tail = &m_stub;
	m_stub.next = NULL;
	m_pool.CreatePool(seed/2,seed);
	m_count = 0;
	m_timeout = timeout;
	return true;

}

bool TMPSCQueue::Enqueue( DWORD inputData )
{
	TQueueElement_* element= m_pool.Pop();
	if(element == NULL)
		return false;
	element->next = NULL;
	element->data = inputData;
	TQueueElement_* prev =
		reinterpret_cast<TQueueElement_*>(InterlockedExchangePointer((void**)(&m_head),element));
	prev->next = element;
	InterlockedIncrement(&m_count);
	return true;
}

void TMPSCQueue::PushNode( TQueueElement_* element )
{
	element->next = NULL;
	TQueueElement_* prev = 
		reinterpret_cast<TQueueElement_*>(InterlockedExchangePointer((void**)(&m_head),element));
	prev->next =  element;
}

LPDWORD TMPSCQueue::Dequeue()
{  
	TQueueElement_* tmptail = m_tail; 
	TQueueElement_* next = tmptail->next; 
	if (tmptail == &m_stub) 
	{ 
		if (NULL == next) 
			return NULL; 
		m_tail = next; 
		tmptail = next; 
		next = next->next; 
	} 
	if (next) 
	{ 
		m_tail = next; 
		InterlockedDecrement(&m_count);
		DWORD rtval = tmptail->data;
		m_pool.Push(tmptail);
		return reinterpret_cast<LPDWORD>(rtval); 
	} 
	TQueueElement_* tmphead = m_head;
	if (tmptail != tmphead) 
		return NULL; 
	PushNode(&m_stub); 
	next = tmptail->next; 
	if (next) 
	{ 
		m_tail = next; 
		InterlockedDecrement(&m_count);
		LPDWORD rtval = reinterpret_cast<LPDWORD>(tmptail->data);
		m_pool.Push(tmptail);
		return reinterpret_cast<LPDWORD>(rtval); 
	} 
	return NULL;
}

LPDWORD TMPSCQueue::WaitDequeue()
{
	LPDWORD data; 
	while ((data = Dequeue()) == NULL) 
		Sleep (m_timeout);   // wait for nonbusy result 
	return data; 
}

void TMPSCQueue::ReleaseMemoryPool()
{
	m_pool.DestroyPool();
}

