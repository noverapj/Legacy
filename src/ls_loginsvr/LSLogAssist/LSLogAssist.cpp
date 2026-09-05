#include "StdAfx.h"
#include "LSLogAssist.h"
 

LSLogAssist::LSLogAssist(void)
{
	//Init(0);
	g_Logic()->SetNewdata();
	Begin();
}

LSLogAssist::~LSLogAssist(void)
{
}

bool LSLogAssist::Init(DWORD dwProcessTime )
{
	 
	return true;
}
void LSLogAssist::PutMessage(int debugid,LPSTR fmt,...)
{
	LOG_* stdata = (LOG_*)g_OPMemPool()->Pop(sizeof(LOG_));
	stdata->debugid = debugid;
	va_list args;
	va_start(args,fmt);
	vsprintf_s(stdata->message,fmt,args);
	va_end(args);
	PutQueue(stdata);
}

void LSLogAssist::PutQueue(LOG_* data)
{
	m_queue.push(data);
}
LOG_* LSLogAssist::GetQueue()
{
	return m_queue.pop(true);
}
void LSLogAssist::Run()
{
	while(1)
	{ 
		LOG_* stdata = GetQueue();
		if(stdata)
		{
			LOG.PrintTimeAndLog(stdata->debugid,stdata->message);
			g_OPMemPool()->Push(stdata);
		}

	}
}