#pragma once

#include "ioTestCase.h"

class ioTestEcho : public ioTestCase
{
public:
	ioTestEcho(void);
	~ioTestEcho(void);

	void Init();
	void Destroy();

public:
	virtual void Run();
};
