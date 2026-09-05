#pragma once


#include <WinSock2.h>

class CLogData;
class ioILogger;
class ioLogger;
class CLog;

//------------------------------------------//
// Enum
//------------------------------------------//
enum LogRecordTypes
{
	LOGRECORD_NONE = 0,
	LOGRECORD_PRINTTIMEANDLOG,	// PrintTimeAndLog
	LOGRECORD_DEBUGLOG,			// DebugLog
	LOGRECORD_PRINTLOG,			// PrintLog
	LOGRECORD_PRINTNOENTERLOG,	// PrintNoEnterLog
	LOGRECORD_END
};

enum LogMessageTypes
{
	LOG_MESSAGE_TYPE_NONE = 0,
	LOG_MESSAGE_TYPE_CLOSE,
	LOG_MESSAGE_TYPE_OPEN,
	LOG_MESSAGE_TYPE_TERMINATE,
	LOG_MESSAGE_TYPE_PROCESS,
	LOG_MESSAGE_TYPE_SETCATEGORY,
	LOG_MESSAGE_TYPE_ONCONNECT,
	LOG_MESSAGE_TYPE_INITDATA,
	LOG_MESSAGE_TYPE_END
};

enum TcpOperationTypes
{
	FLAG_RECEIVE = 13,
	FLAG_CONNECT,
	FLAG_CLOSE,
	FLAG_SENDLOG,
};

enum TcpSocketStates
{
	CONNECTED,
	DISCONNECTED,
};

const int MAX_BUFFER_FILE_LINE = 256;
const int MAX_BUFFER_SIZE = 2048;
const int MAX_DAMONE_NAME_SIZE = 32;
const int MAX_CATEGORY_NAME_SIZE = 32;
const int MAX_FILENAME_SIZE = 256;

struct LOGContext 
{
	WSAOVERLAPPED	m_wsaOverlapped;
	WSABUF			m_wsaBuffer;
};

struct LOGBufferedContext
{
	LOGContext	m_stContext;			
	DWORD		m_flags;			
	DWORD		m_dwBytesTransfer;
	int         m_port; 
	char        m_ipAddress[16];
	int         m_svrPort;
	CLogData*   m_logData;
	char        m_damoneName[MAX_DAMONE_NAME_SIZE];
	char        m_categoryName[MAX_CATEGORY_NAME_SIZE];
	ioLogger*	m_logInstance;

	LOGBufferedContext()
	{
		Init();
	};

	void Init()
	{
		m_flags = 0 ;
		m_logData = NULL;
		ZeroMemory(m_damoneName,sizeof(m_damoneName));
		ZeroMemory(&m_stContext,sizeof(m_stContext));
		ZeroMemory(m_categoryName,sizeof(m_categoryName));
		ZeroMemory(m_ipAddress,sizeof(m_ipAddress));
		ZeroMemory(&m_stContext.m_wsaOverlapped,sizeof(WSAOVERLAPPED));
		m_flags = 0;
		m_dwBytesTransfer = 0;
		m_port = 0;
		m_logInstance = 0;
	}

	WSAOVERLAPPED* GetOveralapped()		{ return &m_stContext.m_wsaOverlapped; }
	WSABUF* GetWsaBuf()					{ return &m_stContext.m_wsaBuffer; }
	LOGContext* GetContext()			{ return &m_stContext; }
	ioLogger* GetLogInstance()			{ return m_logInstance; }
};

struct LOGMessageHeader
{
	int m_logMessageType;
	int m_logRecordType;
	int m_logLevel;
	
	LOGMessageHeader()
	{
		Init();
	}
	
	void Init()
	{
		m_logMessageType = 0;
		m_logRecordType = 0;
		m_logLevel = 0;
	}
};

struct LOGMessage 
{
	int m_categoryIndex;
	char m_logMessage[MAX_BUFFER_SIZE];
	char m_fileLine[MAX_FILENAME_SIZE];
	SYSTEMTIME m_st;
	
	LOGMessage()
	{
		Init();
	};
	
	void Init()
	{
		m_categoryIndex = 0;
		ZeroMemory(m_logMessage,sizeof(m_logMessage));
		ZeroMemory(m_fileLine,sizeof(m_fileLine));
		ZeroMemory(&m_st,sizeof(m_st));
	}
};

struct ReserveLogData
{
	ioILogger* m_instance; 
	char m_openFileName[MAX_BUFFER_FILE_LINE];
	int m_logMessageType;
	int m_logRecordType;
	SYSTEMTIME m_st;

	ReserveLogData()
	{
		m_logRecordType = 0;
		m_logMessageType = 0;
		ZeroMemory(m_openFileName,sizeof(m_openFileName));
		m_instance = NULL;
		ZeroMemory(&m_st,sizeof(m_st));
	}
};

//LOG Library Packet
#define LTPK_LOG				0x9C01