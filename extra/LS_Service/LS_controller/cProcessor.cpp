// cProcessor.cpp: implementation of the cProcessor class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "cCommand.h"
#include "cCommandHandler.h"
#include "SessionProc.h"
#include "cProcessor.h"
#include "../include/cSingleton.h"
#include "../include/IIocpContext.h"
#include "../include/INetwork.h"
#include "../common/Internal.h"

//#define _USE_TRYEXCEPT	1


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

cProcessor::cProcessor()
{
	Init();
}

cProcessor::~cProcessor()
{
	Destroy();
}

void	cProcessor::Init()
{
	m_commandHandler = new cCommandHandler;
}

void	cProcessor::Destroy()
{
	if(m_commandHandler)
	{
		delete m_commandHandler;
		m_commandHandler = NULL;
	}
}

//////////////////////////////////////////////////////////////////////
// operation
//////////////////////////////////////////////////////////////////////

void	cProcessor::Timeout()
{
	cSession* session = SessionProc::GetFirstSession();
	while(session)
	{
		Flush(session->GetContext());
		session = SessionProc::GetNextSession();
	}
}

void	cProcessor::Process(IIocpContext* context, const uint32 commandId, const uint8* data, const uint32 length)
{
#ifdef _USE_TRYEXCEPT
	__try
	{
#endif
		if(context && !context->IsActive())	
		{
			return;
		}

		cCommand* pCommand = m_commandHandler->Get(commandId);
		if(pCommand)
		{
			pCommand->Run(context, data, length);
			//Flush(context);
		}
		else
		{
			pCommand = m_commandHandler->Get(I_CLOSE);
			if(pCommand)
			{
				pCommand->Run(context, data, length);
			}
		}
#ifdef _USE_TRYEXCEPT
	}
	__except(1)
	{
		cout << _T("오류발생 클라이언트 Command :: ") << commandId << endl << flush;
	}
#endif
}

void	cProcessor::Process(const uint32 serverId, const uint32 commandId, const uint8* data, const uint32 length)
{
#ifdef _USE_TRYEXCEPT
	__try
	{
#endif
		cCommand* pCommand = m_commandHandler->Get(commandId);
		if(pCommand)
		{
			pCommand->Run(serverId, data, length);
		}
		else
		{
			Debug(_T("-----> 내부 프로토콜 검색 실패 : ServerID(%lu), Command(%lu)!!"), serverId, commandId);
		}
#ifdef _USE_TRYEXCEPT
	}
	__except(1)
	{
		cout << _T("오류발생 내부 Command :: ") << commandId << endl << flush;
	}
#endif
}

void cProcessor::Flush(IIocpContext* context)
{
	cSingleton<INetwork>::GetInstance()->Flush(context);
}
