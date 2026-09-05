#pragma once

#include "ioTestCase.h"

class ioTestQuerySend1 : public ioTestCase
{
public:
	ioTestQuerySend1(void);
	virtual ~ioTestQuerySend1(void);

	void Init();
	void Destroy();

public:
	virtual void Run();

	//int m_iDBAgentThreadID;
	ValueType GetValueType(VariableType nType,int len);
};

