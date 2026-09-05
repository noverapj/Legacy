#pragma once

#include "TestCase.h"
#include "MPMCQueue.h"
#include "mpmc_queue.h"
#include "mpmc_seh_mempool.h"
#include <set>

class TestMPMCQueue  : public TestCase
{
public:
	TestMPMCQueue(void);
	virtual ~TestMPMCQueue(void);
	virtual void Start();
	virtual void Run();

public:
	double Input(int i, int maxcount);
	void Pop(int i,int maxcount,int quecount, std::vector<int>* sets);

protected:
	//MPMCQueue<int> m_queue;
	//ORIMPMCQueue<int> m_queue;
	mpmc_queue m_queue;
	//MPMC_MemPooler<int> m_pool;
	MemPooler<int> m_pool;
	
	
};

