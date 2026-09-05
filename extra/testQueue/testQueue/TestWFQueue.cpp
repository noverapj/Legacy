#include "StdAfx.h"
#include "TestWFQueue.h"
#include <set>
#include <iostream>

extern INT G_TESTCOUNT;
extern LONG G_NUMBER;
extern BOOL G_TERMINATE;

TestWFQueue::TestWFQueue(int count) : m_count(count)
{
}


TestWFQueue::~TestWFQueue(void)
{
}

void TestWFQueue::Run()
{
	G_NUMBER = 0;
	G_TERMINATE = FALSE;

	std::vector<boost::thread*> m_threads;
	for(int i=0; i< m_count; i++)
	{
		m_threads.push_back(new boost::thread(&TestWFQueue::Input,this));
	}

	//for(int i=0; i<m_threads.size(); ++i)
	//	m_threads[i]->join();

	boost::thread popthread(&TestWFQueue::Pop,this);
	popthread.join();
}

double TestWFQueue::Input()
{
	int count = 0;
	while(!G_TERMINATE)
	{
		DummyWF* node = new DummyWF;
		if(!node)
		{
			int a;
			a = 0;
			return 0;
		}

		++count;

		LONG value = InterlockedIncrement(&G_NUMBER);
		node->data = value;

		if(m_queue.Enqueue(node) == false)
		{
			printf("Memory Pool Zero\n");
			break;
		}

		if(value >= G_TESTCOUNT)
		{
			break;
		}
	}
	return 0;
}

void TestWFQueue::Pop()
{
	std::set<int> values;

	int popcount = 0;
	boost::timer el;

	DummyWF* popval = NULL;
	while(TRUE)
	{
		popval = reinterpret_cast<DummyWF*>(m_queue.Dequeue());
		if(popval == NULL)
		{
			SwitchToThread();
			continue;
		}
		if(popcount == 0)
		{
			el.restart();
		}
 
		//if(unique_check(values.insert(popval->data)) == -1)
		//{
		//	printf("중복데이터 \n");
		//	break;
		//}

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
		popval = reinterpret_cast<DummyWF*>(m_queue.Dequeue());
		if(popval == NULL) break;

		delete popval;
	}

	if(m_queue.GetSize() != 0)
	{
		printf("remove not complete : %lu\n", m_queue.GetSize());
	}
}

void TestWFQueue::Start()
{
	boost::thread thread(&TestWFQueue::Run,this);
}
