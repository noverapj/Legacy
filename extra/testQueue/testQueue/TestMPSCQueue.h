#pragma once

#include "TestCase.h"
#include "TMPSCQueue.h"

class TestMPSCQueue : public TestCase
{
public:
	TestMPSCQueue(int count);
	virtual ~TestMPSCQueue(void);
	virtual void Start();
	virtual void Run();

public:
	double Input();
	void Pop();

protected:
	TMPSCQueue m_queue;
	int m_count;
};

