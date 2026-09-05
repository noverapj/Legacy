#include "StdAfx.h"
#include "TestMPMCMemoryPool.h"
#include <boost/timer.hpp>
#define MAXCOUNT 1000000

TestMPMCMemoryPool::TestMPMCMemoryPool(void)
{
}


TestMPMCMemoryPool::~TestMPMCMemoryPool(void)
{
}

void TestMPMCMemoryPool::Run()
{
	m_memPooler.CreatePool(MAXCOUNT/2,MAXCOUNT);
	test_* tt = new test_;
	m_memPooler.CreatePush(tt);
	m_orimemPooler.CreatePool(MAXCOUNT/2,MAXCOUNT);
	boost::thread newmemt(&TestMPMCMemoryPool::TestFunc,this,0,0);
	boost::thread orimemt(&TestMPMCMemoryPool::oriTestFunc,this,0,0);
	newmemt.join();
	orimemt.join();


}

void TestMPMCMemoryPool::TestFunc( int i, int maxcount )
{
	boost::timer el;
	std::vector<test_*> tests;
	for(int i=0; i<MAXCOUNT; ++i)
	{
		test_* ptest = m_memPooler.Pop();
		ptest->i = i;
		tests.push_back(ptest);
	}
	for(int i=0; i<tests.size(); ++i)
	{
		m_memPooler.Push(tests[i]);
	}
	printf("New Mem Finsith[%d]:%0.3f\n",tests.size(),el.elapsed());
	std::vector<boost::thread*> threads;
	for(int i=0; i<8; ++i)
	{
		threads.push_back(new boost::thread(&TestMPMCMemoryPool::PopFunc,this,i));
	}
	for(int i=0; i<threads.size(); ++i)
	{
		threads[i]->join();
	}


}

void TestMPMCMemoryPool::oriTestFunc( int i, int maxcount )
{

	boost::timer el;
	std::vector<test_*> tests;
	for(int i=0; i<MAXCOUNT; ++i)
	{
		test_* ptest = m_orimemPooler.Pop();
		ptest->i = i;
		tests.push_back(ptest);
	}
	for(int i=0; i<tests.size(); ++i)
	{
		m_orimemPooler.Push(tests[i]);
	}
	printf("\nOriMem Finsith[%d]:%0.3f\n",tests.size(),el.elapsed());
	std::vector<boost::thread*> threads;
	for(int i=0; i<8; ++i)
	{
		threads.push_back(new boost::thread(&TestMPMCMemoryPool::oriPopFunc,this,i));
	}
	for(int i=0; i<threads.size(); ++i)
	{
		threads[i]->join();
	}


}
#define POPCOUNT 500000

void TestMPMCMemoryPool::PopFunc( int i )
{ 
	boost::timer el;
	for(int j = 0; j<POPCOUNT;++j)
	{
		test_* ptest = m_memPooler.Pop();
		if(j% 300000 == 1)
			printf("n:%d ",ptest->i);
		m_memPooler.Push(ptest);
	}
	printf("\n[%d]Popfunc Finsh:%0.3f (free:%d)\n",i,el.elapsed(),m_memPooler.GetCount());
}

void TestMPMCMemoryPool::oriPopFunc( int i )
{
	boost::timer el;
	for(int j = 0; j<POPCOUNT;++j)
	{
		test_* ptest = m_orimemPooler.Pop();
	//	printf("O:%d ",ptest->i);
		if(j% 300000 == 1)
			printf("o:%d ",ptest->i);
		m_orimemPooler.Push(ptest);
	}
	printf("\n[%d]OriPopfunc Finsh:%0.3f (free:%d)\n",i,el.elapsed(),m_orimemPooler.GetCount());
}
