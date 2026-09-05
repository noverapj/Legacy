#include "../iocpSocketDLL.h"
#include "../Processor/Processor.h"
#include "LogicThread.h"

extern CProcessor* g_processor;

LogicThread::LogicThread(void) //: m_processor(NULL)
{
	Init();
}

LogicThread::~LogicThread(void)
{
	Destroy();
}

void LogicThread::Init()
{
}

void LogicThread::Destroy()
{
}

void LogicThread::Run()
{
	uint32 idleTime;

	while(TRUE)
	{
		g_processor->Process(idleTime);
		if(idleTime > 0)
		{
			::SleepEx(idleTime, FALSE);
		}
	}
}