#pragma once


class MPSCRecvQueue;

class Operation
{
public:
	Operation(MPSCRecvQueue* queue);
	virtual ~Operation(void);

public://fuction
	virtual int Run(void * arg) = 0;

private:
	MPSCRecvQueue* m_queue;
};

