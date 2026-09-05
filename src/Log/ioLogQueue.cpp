#include "stdafx.h"
#include "ioLogger.h"
#include "LogData.h"
#include "ioLogThread.h"
#include "ioLogQueue.h"
#include "ioTCPConnectNode.h"


void ioLogQueue::Init()
{
}

void ioLogQueue::Destroy()
{
}

void ioLogQueue::Startup()
{
	ioQueue::Startup( INFINITE );
}

void ioLogQueue::Parse()
{
	DWORD bytes = 0;
	LPOVERLAPPED ov = NULL;
	int retVal;
	LPDWORD compKey = ioQueue::Dequeue(bytes,ov,retVal);
	
	if(ov == NULL)
	{
		CLogData* logData =  reinterpret_cast<CLogData*>(compKey);
		ParseLogData(logData,bytes);
	}
	else
	{
		if(compKey != NULL)
		{
			ioTCPConnectNode* tcpNode = reinterpret_cast<ioTCPConnectNode*>(compKey);
			ParseTcpLogData(ov, bytes, retVal, tcpNode);
		}
		else
		{//Log??
		}
	}
}

void ioLogQueue::ParseLogData( CLogData* logData,DWORD bytes )
{
	if( NULL == logData ) return;
	ioILogger* logger = logData->GetInstance();

	if(g_TCPNode->SendMessage(logData) != -1)
	{//전송 성공했을때는 리턴 
		return;
	}
		
	switch( logData->GetType() )
	{
	case LOG_MESSAGE_TYPE_CLOSE:
		{
			if(!logger) return;
			//여기 
			logger->ExcuteClose(logData);
		}
		break;

	case LOG_MESSAGE_TYPE_OPEN:
		{
			if( !logger ) return;
			//여기
			logger->ExcuteOpen(logData);
		}
		break;

	case LOG_MESSAGE_TYPE_TERMINATE:
		{
			while( TRUE )
			{ 
				if( logger == NULL )
					break;
				// 종료 : file close
				logger->ExcuteClose(NULL);
			}
		}
		break;

	case LOG_MESSAGE_TYPE_PROCESS:
		{
			if( !logger ) return;
			logger->ExcuteWrite(logData);
		}
		break;

	case LOG_MESSAGE_TYPE_SETCATEGORY:
		{
			if( !logger ) return;
			logger->ExcuteSetCategory(logData);
		}
		break;

	case LOG_MESSAGE_TYPE_INITDATA:
		{
			if( !logger ) return;
			logger->ExcuteInitData();
		}
	}
}

void ioLogQueue::ParseTcpLogData( LPOVERLAPPED overlapped, DWORD bytes, int& retVal, ioTCPConnectNode* tcpNode )
{
	LOGBufferedContext* logContext = reinterpret_cast<LOGBufferedContext*>(overlapped);
	switch(logContext->m_flags)
	{
	case FLAG_RECEIVE:
		{
			if(retVal == FALSE || bytes == 0)
			{
				tcpNode->SessionClose();
				return;
			}
			tcpNode->DispatchReceive(bytes);
		}
		break;
	
	case FLAG_CONNECT:
	case FLAG_CLOSE:
	default:
		{
			tcpNode->PacketParsing(logContext);
		}
		break;
	}
}
