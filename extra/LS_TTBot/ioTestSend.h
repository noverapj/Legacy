#pragma once
#include "ioTestCase.h"




class ioTestSend : public ioTestCase
{
public:
	ioTestSend(void);
	virtual ~ioTestSend(void);

	void Init();
	void Destroy();

public:
	virtual void Run();

	ValueType GetValueType(VariableType nType,int len);
};
