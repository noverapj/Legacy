#pragma once
#include "BaseClass/operation.h"

class LSLogicOperations : public Operation
{
public:
	LSLogicOperations(MPSCRecvQueue* queue) : Operation(queue) { };
	virtual ~LSLogicOperations(void) { };
};

class TestOperation : public LSLogicOperations
{
public:
	TestOperation(MPSCRecvQueue* queue) : LSLogicOperations(queue) { };

public:
	virtual int Run(void * arg);
};

class OnAccept : public LSLogicOperations
{
public:
	OnAccept(MPSCRecvQueue* queue) : LSLogicOperations(queue) {};

public:
	virtual int Run(void* arg);
};

class ReConnect : public LSLogicOperations
{
public:
	ReConnect(MPSCRecvQueue* queue) : LSLogicOperations(queue) {};

public:
	virtual int Run(void* arg);
};

class ReceiveOperation : public LSLogicOperations
{
public:
	ReceiveOperation(MPSCRecvQueue* queue) : LSLogicOperations(queue) {};

public:
	virtual int Run(void * arg);
};

class SchedulerOperation : public LSLogicOperations
{
public:
	SchedulerOperation(MPSCRecvQueue* queue) : LSLogicOperations(queue) {};

public:
	virtual int Run(void * arg);
};

class ChangeTickTime : public LSLogicOperations
{
public:
	ChangeTickTime(MPSCRecvQueue* queue) : LSLogicOperations(queue) {};

public:
	virtual int Run(void * arg);
};

class TrySendServerInfo : public LSLogicOperations
{
public:
	TrySendServerInfo(MPSCRecvQueue* queue) : LSLogicOperations(queue) { m_firstState = false; };

public:
	virtual int Run(void* arg);
	bool    m_firstState;
};

