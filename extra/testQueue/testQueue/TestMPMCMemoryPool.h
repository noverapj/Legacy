#pragma once

#include "TestCase.h"
#include "mpmc_seh_mempool.h"
#include <set>
#include "MemPooler.h"

struct test_
{
	char str[256];
	int i;
};

class TestMPMCMemoryPool : public TestCase
{
public:
	TestMPMCMemoryPool(void);
	virtual ~TestMPMCMemoryPool(void);
 
	virtual void Run();
	virtual void TestFunc(int i, int maxcount);
	virtual void oriTestFunc(int i, int maxcount);
	virtual void PopFunc(int i);
	virtual void oriPopFunc(int i);

	MPMC_MemPooler<test_> m_memPooler;
	MemPooler<test_> m_orimemPooler;
};

