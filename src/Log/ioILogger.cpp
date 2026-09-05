#include "stdafx.h"
#include "LogData.h"
#include "ioLogDataPool.h"
#include "ioLogQueue.h"
#include "ioLogThread.h"
#include "ioILogger.h"


ioILogger::ioILogger(void)
{
	Init();
}

ioILogger::~ioILogger(void)
{
	Destroy();
}

void ioILogger::Init()
{
	Register();
}

void ioILogger::Destroy()
{
}

void ioILogger::Register()
{
	// 서버 내려갈때 file close를 위해 등록.
	g_logThread->Register( this );
}

CLogData* ioILogger::PopData()
{
	return g_logDataPool->Pop();
}

void ioILogger::PushData( CLogData* logData )
{
	if( logData == NULL )
		return;

	// init
	logData->Init();

	// push
	g_logDataPool->Push( logData );
}

void ioILogger::Enqueue(CLogData* logData)
{
	g_logQueue->Enqueue(reinterpret_cast<DWORD>(logData), sizeof(CLogData));
}