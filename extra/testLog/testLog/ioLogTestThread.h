#pragma once

#include "ioThread.h"

class ioLogTestThread : public ioThread
{
public:
	ioLogTestThread(void);
	~ioLogTestThread(void);

public:
	void Run();

protected:
	void TEST_Open();
	void TEST_Close();
	void TEST_Write();
};

