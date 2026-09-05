#include "StdAfx.h"
#include "TestMPMCQueue.h"
#include <iostream>
#include <set>

TestMPMCQueue::TestMPMCQueue(void)
{
}


TestMPMCQueue::~TestMPMCQueue(void)
{
}

void TestMPMCQueue::Start()
{

}
#define MAXXX 3
void TestMPMCQueue::Run()
{
#if 0 
	m_pool.CreatePool(MAXXX/2,MAXXX);
	std::vector<int*> ints;
	for(int i=0; i<MAXXX; ++i)
	{
		ints.push_back(m_pool.Pop());
	}
	for(int i=0; i<ints.size(); ++i)
	{
		*ints[i] = i;
	}
	for(int i=0; i<ints.size(); ++i)
	{
		m_pool.Push(reinterpret_cast<int*>(ints[i]));
	}
	ints.clear();
	for(int i=0; i<MAXXX; ++i)
	{
		int* a = m_pool.Pop();
		printf("P:%d\n",*a);
		ints.push_back(a);
	}
	for(int i=0; i<ints.size(); ++i)
	{
		m_pool.Push(reinterpret_cast<int*>(ints[i]));
	}
	ints.clear();
	for(int i=0; i<MAXXX; ++i)
	{
		int* a = m_pool.Pop();
		printf("P:%d\n",*a);
		ints.push_back(a);
	}
	for(int i=0; i<ints.size(); ++i)
	{
		m_pool.Push(reinterpret_cast<int*>(ints[i]));
	}
#endif
#if 1
	int tmpval;
	printf("\nQueCount : ");
	std::cin >> tmpval;
	int quecount = tmpval;

	printf("Thread Count : ");
	scanf_s("%d",&tmpval);
	std::vector<boost::thread*> m_threads;
	for(int i=0; i<tmpval; ++i)
	{
		m_threads.push_back(new boost::thread(&TestMPMCQueue::Input,this,(quecount/tmpval)*i,quecount/tmpval));
	}

	for(int i=0; i<m_threads.size(); ++i)
		m_threads[i]->join();
	printf("\n");
	int popcount = 2;
	std::vector<boost::thread*> m_popthreads;
	std::vector<std::vector<int>* > values;
	for(int i=0; i<popcount; ++i)
	{
		std::vector<int>* tmpset = new std::vector<int>;
		values.push_back(tmpset);
	}
	for(int i =0; i<popcount; ++i)
	{
		m_popthreads.push_back(new boost::thread(&TestMPMCQueue::Pop,this,i,(quecount/popcount),quecount,values[i]));
	}
	for(int i=0; i<popcount; ++i)
		m_popthreads[i]->join();
	printf("\n");
	std::set<int> mainset;
	for(int i=0; i< values.size(); ++i)
	{
		for(int j=0; j<values[i]->size(); ++j)
		{
			if(unique_check(mainset.insert(values[i]->at(j))) == -1)
				printf("중복데이터\n");
		}
	}
	printf("Final size = %d\n",mainset.size());
	for(int i=0; i<values.size(); ++i)
	{
		values[i]->clear();
		
	}
#endif
	//m_popthreads.join();
	//m_queue.ReleaseMemoryPool();

}

double TestMPMCQueue::Input( int i, int maxcount )
{
	
	boost::timer el;
	int index = 0;
	for(index; index<maxcount; ++index)
	{
		mpmc_queue::node_t* tt = new mpmc_queue::node_t;
//		 tt->data = new int(i+index);
		m_queue.enqueue(tt);
	}
	printf("i:%d max:%d time:%0.3f\n",i,maxcount,el.elapsed());
	return 0;
}

void TestMPMCQueue::Pop( int i,int maxcount,int quecount, std::vector<int>* setval)
{

	int index =0;
	//while(1)
	boost::timer el;
	for(index; index<maxcount; ++index)
	{
		mpmc_queue::node_t* valt = m_queue.dequeue();
//		int* val = (int*)valt->data;
//		if(val == NULL)
		{
		//	printf("NULL Size : %d\n",m_queue.GetSize());
		//	printf("빠다:%d\n",val);

			break;
		}
//		setval->push_back(*val);
		
		
		
		//printf("P[%d]:%d ",i,*val);
		//delete val;

	}
	printf("Elast::%0.3f\n",el.elapsed());

}
