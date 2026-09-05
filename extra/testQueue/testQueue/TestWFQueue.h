#pragma once

#include "TestCase.h"
#include "MPSCQueue.h"

class TestWFQueue : public TestCase
{
public:
	TestWFQueue(int count);
	virtual ~TestWFQueue(void);
	virtual void Start();
	virtual void Run();

public:
	double Input();
	void Pop();

protected:
	WFQueue m_queue;
	int m_count;

};

