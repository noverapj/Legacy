#include "StdAfx.h"
#include "TestMPSCQueue.h"
#include <set>
#include <iostream>

extern INT G_TESTCOUNT;
extern LONG G_NUMBER;
extern BOOL G_TERMINATE;

TestMPSCQueue::TestMPSCQueue(int count) : m_count(count)
{
}


TestMPSCQueue::~TestMPSCQueue(void)
{
}

void TestMPSCQueue::Run()
{
	G_NUMBER = 0;
	G_TERMINATE = FALSE;

	m_queue.Init(G_TESTCOUNT, 1); 

	std::vector<boost::thread*> m_threads;
	for(int i=0; i< m_count; i++)
	{
		m_threads.push_back(new boost::thread(&TestMPSCQueue::Input,this));
	}
 
	//for(int i=0; i<m_threads.size(); ++i)
	//	m_threads[i]->join();


	boost::thread popthread(&TestMPSCQueue::Pop,this);
	popthread.join();
	m_queue.ReleaseMemoryPool();
	
}

double TestMPSCQueue::Input()
{
	int count = 0;
	while(!G_TERMINATE)
	{
		DummyMPSC* node = new DummyMPSC;
		if(!node)
		{
			int a;
			a = 0;
			return 0;
		}

		++count;

		LONG value = InterlockedIncrement(&G_NUMBER);
		node->data = value;

		if(m_queue.Enqueue((DWORD)node) == false)
		{
			//printf("Memory Pool Zero\n");
			break;
		}

		if(value >= G_TESTCOUNT)
		{
			break;
		}
	}
 	return 0;
}

void TestMPSCQueue::Pop()
{
	int popcount = 0;
	boost::timer el;

	DummyMPSC* popval = NULL;

	while(1)
	{
		popval = reinterpret_cast<DummyMPSC*>(m_queue.Dequeue());
		if(popval == NULL)
		{
			SwitchToThread();
			continue;
		}
		if(popcount == 0)
		{
			el.restart();
		}

		delete popval;

		popcount++;
		if(popcount >= G_TESTCOUNT)
		{
			G_TERMINATE = TRUE;
			break;
		}
	}

	printf("Finished popCount :[%d] time: [%0.4f], remain : %lu\n",popcount, el.elapsed(), m_queue.GetSize());

	while(TRUE)
	{
		popval = reinterpret_cast<DummyMPSC*>(m_queue.Dequeue());
		if(popval == NULL) break;

		delete popval;
	}

	if(m_queue.GetSize() != 0)
	{
		printf("remove not complete : %lu\n", m_queue.GetSize());
	}
}

void TestMPSCQueue::Start()
{
	boost::thread thread(&TestMPSCQueue::Run,this);
}
