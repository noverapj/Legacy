#pragma once

#include "ioTestCase.h"

class ioTestQuerySend2 : public ioTestCase
{
public:
	ioTestQuerySend2(void);
	virtual ~ioTestQuerySend2(void);

	void Init();
	void Destroy();

public:
	virtual void Run();

	//int m_iDBAgentThreadID;
	ValueType GetValueType(VariableType nType,int len);
};

