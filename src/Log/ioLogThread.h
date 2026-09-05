#pragma once

#include "ioThread.h"
#include "ioFile.h"
#include "../include/cSingleton.h"

class ioILogger;

class ioLogThread : public ioThread
{
public:
	ioLogThread(void);
	~ioLogThread(void);

private:
	void Init();
	void Destroy();

private:
	void Run();

public:
	void Register(ioILogger* pLog);
	ioILogger* Unregister();

private:
	ioFileWriter errorLog;

	std::list<ioILogger*> m_instances;
};

#define g_logThread cSingleton<ioLogThread>::GetInstance()
