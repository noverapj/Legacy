#include "StdAfx.h"
#include "TestIOCPQueue.h"
#include <set>
#include <iostream>

TestIOCPQueue::TestIOCPQueue(void)
{
	Startup(1);
}


TestIOCPQueue::~TestIOCPQueue(void)
{
	m_nodes.DestroyPool();
}

void TestIOCPQueue::Start()
{

}

void TestIOCPQueue::Run()
{
	int tmpval;

	//	flush(stdin);
	printf("\nQueCount : ");
	std::cin >> tmpval;
	m_nodes.CreatePool(tmpval/2,tmpval);
 
	int quecount = tmpval;

	printf("Thread Count : ");
	scanf("%d",&tmpval);
	std::vector<boost::thread*> m_threads;
	for(int i=0; i<tmpval; ++i)
	{
		m_threads.push_back(new boost::thread(&TestIOCPQueue::Input,this,(quecount/tmpval)*i,quecount/tmpval));
	}

	//for(int i=0; i<m_threads.size(); ++i)
	//	m_threads[i]->join();

	boost::thread popthread(&TestIOCPQueue::Pop,this,quecount);
	popthread.join();
	Cleanup();

}


double TestIOCPQueue::Input( int i, int maxcount )
{
	DWORD tmp =0;
	boost::timer el;
	int index = 0;
	int tmpval = i;
	for(index; index<maxcount; ++index)
	{
		i++;
		NodeData* node = reinterpret_cast<NodeData*>(m_nodes.Pop());
		node->data = reinterpret_cast<void*>(i);
		
		Enqueue((DWORD)node,tmp);

	}
	printf("InputThread(%d) %0.4f\n",tmpval,el.elapsed());
	return 0;

	
}

void TestIOCPQueue::Pop(int quecount)
{
	std::set<int> testvalues;
	
	NodeData* popval = NULL;
	int popcount = 0;
	boost::timer el;
	while(1)
	{

		popval = reinterpret_cast<NodeData*>(Dequeue());
		if(popval == NULL)
			break;
		//	printf("%d\n",popval);
		if(unique_check(testvalues.insert((int)popval->data)) == -1)
		{
			printf("중복데이터 \n");
			break;
		}
		if(testvalues.size() > 5000000)
		{
		//	printf("D:%d",popcount);
			testvalues.clear();
		}
		popcount++;
		m_nodes.Push((NodeData*)popval);
		if(popcount >= quecount)
		{
			printf("완료\n");
			break;
		}
	}


	printf("Finished popCount :[%d] time: [%0.4f]\n",popcount,el.elapsed());
}
