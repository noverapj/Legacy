#pragma once


class Handler
{
public:
	Handler(MPSCRecvQueue* queue);//kyg º¯°æ
	virtual ~Handler(void);

public://funtion
	virtual void Init() = 0;
	Operation* FindOperation(unsigned int opid);

protected:
	std::vector<Operation*> m_operations;
	MPSCRecvQueue* m_queue;
};

