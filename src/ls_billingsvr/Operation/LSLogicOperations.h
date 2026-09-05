#pragma once


#include "../BaseClass/operation.h"

class ConnectNode;

class LSLogicOperations :
	public Operation
{
public:
	LSLogicOperations(MPSCRecvQueue* queue) : Operation(queue)
	{
	};
	virtual ~LSLogicOperations(void)
	{
	};
};

class TestOperation : public LSLogicOperations
{
public:
	TestOperation(MPSCRecvQueue* queue) : LSLogicOperations(queue)
	{
	};
public:
	virtual int Run(void * arg);
};

class OnAccept : public LSLogicOperations
{
public:
	OnAccept(MPSCRecvQueue* queue) : LSLogicOperations(queue)
	{
	};
public:
	virtual int Run(void* arg);

protected:
	int MakePortIndex(const ConnectNode* node);
};

class OnConnect : public LSLogicOperations
{
public:
	OnConnect(MPSCRecvQueue* queue) : LSLogicOperations(queue)
	{
	};
public:
	virtual int Run(void* arg);
};

class ReNexonInit : public LSLogicOperations
{
public:
	ReNexonInit(MPSCRecvQueue* queue) : LSLogicOperations(queue)
	{
	};
public:
	virtual int Run(void * arg);
};

class SchedulerOperation : public LSLogicOperations
{
public:
	SchedulerOperation(MPSCRecvQueue* queue) : LSLogicOperations(queue)
	{
	};
public:
	virtual int Run(void * arg);

	void OnGhostCheck();

	void SendServerInfo();

};

class WriteProcessLog : public LSLogicOperations
{
public:
	WriteProcessLog(MPSCRecvQueue* queue) : LSLogicOperations(queue)
	{
	};
public:
	virtual int Run(void * arg);
};

class TestCloseOperation : public LSLogicOperations
{
public:
	TestCloseOperation(MPSCRecvQueue* queue) : LSLogicOperations(queue)
	{
	};
public:
	virtual int Run(void * arg);
};