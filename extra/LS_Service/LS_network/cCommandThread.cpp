// cCommandThread.cpp: implementation of the cCommandThread class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "cIocpContext.h"
#include "cCommandQueue.h"
#include "cCommandQueueElem.h"
#include "cCommandThread.h"
#include "cSystem.h"
#include "../include/cSingleton.h"

#include <iostream>


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

cCommandThread::cCommandThread()
{
	Init();
}

cCommandThread::~cCommandThread()
{
	Destroy();
}

void cCommandThread::Init()
{
}

void cCommandThread::Destroy()
{
}

//////////////////////////////////////////////////////////////////////
// operation
//////////////////////////////////////////////////////////////////////

void cCommandThread::Process()
{
	cCommandQueue* commandQueue = cSingleton<cSystem>::GetInstance()->GetCommandQueue();
	if( !commandQueue )	return;

	Trace( _T("CommandThread :: %lu\r\n"), GetCurrentThreadId() );
	
	IProcessor* processor = cSingleton<cSystem>::GetInstance()->GetProcessor();
	if( !processor ) return;

	processor->SetCountPointer( commandQueue->m_count );

	cCommandQueueElem* command = NULL;
	while( !IsTerminated() )
	{
		command = reinterpret_cast<cCommandQueueElem*>(commandQueue->Dequeue());
		if( command )
		{
			// process command
			if( command->GetContext() )
			{
				processor->Process(	command->GetContext(),
									command->GetCommand(),
									command->GetData(),
									command->GetLength() );
			}
			else
			{
				processor->Process(	command->GetID(),
									command->GetCommand(),
									command->GetData(),
									command->GetLength() );
			}

			commandQueue->Free( command );
		}
	}
}


