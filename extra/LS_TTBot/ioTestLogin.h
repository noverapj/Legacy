#pragma once

#include "ioTestCase.h"

class ioTestLogin : public ioTestCase
{
public:
	ioTestLogin(void);
	~ioTestLogin(void);

	void Init();
	void Destroy();

public:
	virtual void Run();
};
