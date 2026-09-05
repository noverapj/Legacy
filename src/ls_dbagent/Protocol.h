#ifndef ___PROTOCOL_H__
#define ___PROTOCOL_H__

//DB TCP PACKET
#define DTPK_QUERY              0x0001
#define DTPK_QUERY_PING			0x0002

// INTERNAL PACKET
#define ITPK_ACCEPT_SESSION		0x5555
#define ITPK_CLOSE_SESSION		0x5556

//LOG SERVER UDP PACKET   0x9000 ~ 0x9099        
#define LUPK_LOG				0x9000

//LOG Library Packet
#define LTPK_LOG				0x9C01

const int MAX_BUFFER_FILE_LINE = 256;
const int MAX_BUFFER_SIZE = 2048;
const int MAX_DAMONE_NAME_SIZE = 32;
const int MAX_CATEGORY_NAME_SIZE = 32;
const int MAX_FILENAME_SIZE = 256;

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
 
	int categoryIndex;
	char m_logMessage[MAX_BUFFER_SIZE];
	char m_fileLine[MAX_FILENAME_SIZE];
	SYSTEMTIME m_st;
	LOGMessage()
	{
		Init();
	};
	void Init()
	{
		categoryIndex = 0;
		ZeroMemory(m_logMessage,sizeof(m_logMessage));
		ZeroMemory(m_fileLine,sizeof(m_fileLine));
		ZeroMemory(&m_st,sizeof(m_st));
	}
};

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
	LOG_MESSAGE_TYPE_SET_CATEGORY,
	LOG_MESSAGE_TYPE_ON_CONNECT,
	LOG_MESSAGE_TYPE_END
};


#endif