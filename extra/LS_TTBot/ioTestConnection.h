#pragma once

#include "ioTestCase.h"

class ioTestConnection : public ioTestCase
{
public:
	ioTestConnection(void);
	~ioTestConnection(void);

	void Init();
	void Destroy();

public:
	virtual void Run();
};
