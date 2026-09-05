#pragma once

struct NodeEntry
{
	NodeEntry() : next(nullptr) {}
	NodeEntry* volatile  next; 
}; 

struct NodeData
{
	NodeData() {}
	NodeEntry m_nodeEntry;
};

class DummyMPSC
{
public:
	DummyMPSC() : data(0)
	{}

	int data;
};

class DummyWF : public NodeData
{
public:
	DummyWF() : data(0)
	{}

	int data;
};