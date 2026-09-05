#include "stdafx.h"
#include "ioLogger.h"
#include "ioLogDataPool.h"
#include "ioDefine.h"
#include "../include/Log.h"
#include "ioTCPConnectNode.h"


CLog::CLog(void)
{
	Init();
}

CLog::~CLog(void)
{
	Destroy();
}

void CLog::Init()
{
	m_logger = new ioLogger;
}

void CLog::SetTcpMode( char* daemonName, int tcpPort, int svrPort, char* ipAddr)
{
	g_TCPNode->PushOperation(FLAG_CONNECT,daemonName,tcpPort, svrPort, ipAddr,m_logger);
}

void CLog::SetCategory( char* categoryName, BOOL tcpState )
{
	m_logger->SetCategoryName(categoryName,tcpState);
}

void CLog::Destroy()
{
	if(m_logger)
	{
		delete m_logger;
		m_logger = NULL;
	}
}

void CLog::OpenLog(int logLevel, char* fileName, bool append)
{
	m_logger->OpenLog(logLevel, fileName, append);
}

void CLog::CloseLog()
{
	m_logger->CloseLog();
}

void CLog::PrintNoEnterLog(int logLevel, LPSTR fmt,...)
{
	char text[MAX_BUFFER_SIZE] = { 0, };

	va_list args;
	va_start( args, fmt );
	vsprintf_s( text, sizeof(text), fmt, args );
	va_end( args );

	m_logger->PrintNoEnterLog(logLevel, text);
}

void CLog::PrintLog(int logLevel, LPSTR fmt,...)
{
	char text[MAX_BUFFER_SIZE] = { 0, };

	va_list args;
	va_start( args, fmt );
	vsprintf_s( text, sizeof(text), fmt, args );
	va_end( args );

	m_logger->PrintLog(logLevel, text);
}

void CLog::PrintTimeAndLog(int logLevel, LPSTR fmt,...)
{
	char text[MAX_BUFFER_SIZE] = { 0, };

	va_list args;
	va_start( args, fmt );
	vsprintf_s( text, sizeof(text), fmt, args );
	va_end( args );

	m_logger->PrintTimeAndLog(logLevel, text);
}

void CLog::DebugLog(int logLevel, LPSTR filename, int linenum, LPSTR fmt,...)
{
	char fileLine[MAX_BUFFER_FILE_LINE] = { 0, };
	char text[MAX_BUFFER_SIZE] = { 0, };

	sprintf_s( fileLine, sizeof(fileLine), "[FILE : %s, LINE : %d] ", filename, linenum );

	// parameter
	va_list args;
	va_start( args, fmt );
	vsprintf_s( text, sizeof(text), fmt, args );
	va_end( args );

	m_logger->DebugLog(logLevel, text, fileLine);
}

void CLog::DebugLog(int logLevel, LPSTR fileLine, LPSTR fmt,...)
{
	char text[MAX_BUFFER_SIZE] = { 0, };
	va_list args;
	va_start( args, fmt );
	vsprintf_s( text, sizeof(text), fmt, args );
	va_end( args );
	m_logger->DebugLog(logLevel, text, fileLine);

}

void CLog::GetMemoryInfo(int& remainCount, int& usingCount, int& maxUsing, int& dropCount)
{
	remainCount		= g_logDataPool->GetRemainCount();
	usingCount		= g_logDataPool->GetToalCount() - usingCount;
	maxUsing		= g_logDataPool->GetMaxUsingCount();
	dropCount		= g_logDataPool->GetDropCount();
}

void CLog::InitData()
{
	m_logger->SetInitData();
}
