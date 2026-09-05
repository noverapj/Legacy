#pragma once

#include "TestCase.h"

struct NodeData
{
	void* data;
	NodeData* volatile  next; 
};
class TestIOCPQueue : public TestCase, cIocpQueue
{
public:
	TestIOCPQueue(void);
	virtual ~TestIOCPQueue(void);
	virtual void Start();
	virtual void Run();

public:
	double Input(int i, int maxcount);
	void Pop(int quecount);
private:
	MemPooler<NodeData> m_nodes;

};

