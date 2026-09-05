#include "stdafx.h"
#include "ioLogQueue.h"
#include "ioLogThread.h"


ioLogThread::ioLogThread(void)
{
	Init();
}

ioLogThread::~ioLogThread(void)
{
	Destroy();
}

void ioLogThread::Init()
{
	SetStart( TRUE );

	// 큐
	g_logQueue->Startup();

	// 쓰레드 시작
	Begin();
}

void ioLogThread::Destroy()
{
	SetStart( FALSE );
}

void ioLogThread::Run()
{
	while( IsStart() )
	{
		__try
		{
			g_logQueue->Parse();
		}
		__except( 1 )
		{
			// file pointer
			if( ! errorLog.Open( "LogLib_Except.log", OPEN_ALWAYS ) )
			{
				break;
			}

			// offset을 뒤로..
			errorLog.Move( FILE_END, 0 );

			// write
			errorLog.WriteFormat( _T("%s"), "error.\r\n" );

			// close
			errorLog.Close();
		}
	}
}

void ioLogThread::Register( ioILogger* pLog )
{
	m_instances.push_back( pLog );
}

ioILogger* ioLogThread::Unregister()
{
	if( m_instances.empty() )
		return NULL;

	ioILogger* logData = m_instances.front();
	m_instances.pop_front();
	return logData;
}
