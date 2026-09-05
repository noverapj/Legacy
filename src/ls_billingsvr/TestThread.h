#pragma once

class Thread;

class TestThread : public Thread
{
public:
	TestThread(void);
	~TestThread(void);

public:
	virtual void Run();
};

