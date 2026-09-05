#pragma once


#include "BaseClass/operation.h"

class GameServerNode;

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
	int MakePortIndex(const GameServerNode* node);
};

class ReConnect : public LSLogicOperations
{
public:
	ReConnect(MPSCRecvQueue* queue) : LSLogicOperations(queue)
	{
	};
public:
	virtual int Run(void* arg);
};

class ReceiveOperation : public LSLogicOperations
{
public:
	ReceiveOperation(MPSCRecvQueue* queue) : LSLogicOperations(queue)
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

class ChangeTickTime : public LSLogicOperations
{
public:
	ChangeTickTime(MPSCRecvQueue* queue) : LSLogicOperations(queue)
	{
	};
public:
	virtual int Run(void * arg);
};

