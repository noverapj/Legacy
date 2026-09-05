#pragma once

#include "ioDefine.h"

class ioILogger;

class CLogData
{
public:
	CLogData()	{ Init(); }
	~CLogData()	{ Destroy(); }

	void Init()
	{
		m_messageType	= LOG_MESSAGE_TYPE_NONE;
		m_logLevel		= 0;
		m_recordType	= 0;
		m_instance		= NULL;
		ZeroMemory(&m_st, sizeof(m_st));
		ZeroMemory(m_fileLine, sizeof(m_fileLine));
		ZeroMemory(m_buffer, sizeof(m_buffer));
		ZeroMemory(m_categoryName, sizeof(m_categoryName));
		m_tcpState = FALSE;
		m_categoryNum = 0;
	}

	void Destroy()
	{
		Init();
	}

public:
	// open
	void Set(ioILogger* instance, LogMessageTypes msgType, int32 logLevel, const char* file)
	{
		m_instance		= instance;
		m_messageType	= msgType;
		m_logLevel		= logLevel;
		GetLocalTime(&m_st);
		CopyMemory(m_fileLine, file, sizeof(m_fileLine));
	}

	void Set(ioILogger* instance, LogMessageTypes msgType, const char* categoryName, BOOL tcpState)
	{
		m_instance = instance;
		m_messageType = msgType;
		strcpy_s(m_categoryName,categoryName);
		m_tcpState = tcpState;
	}
	// close
	void Set(ioILogger* instance, LogMessageTypes msgType)
	{
		m_instance		= instance;
		m_messageType	= msgType;
	}

	// write
	void Set(ioILogger* instance, LogMessageTypes msgType, const int32 logLevel, const int32 recordType, const char* text, const char* fileLine=NULL)
	{
		m_instance		= instance;
		m_messageType	= msgType;
		m_logLevel		= logLevel;
		m_recordType	= recordType;
		GetLocalTime( &m_st );
		if(text)
			CopyMemory(m_buffer, text, sizeof(m_buffer));
		if(fileLine)
			CopyMemory(m_fileLine, fileLine, sizeof(m_fileLine));
	}

	LogMessageTypes GetType()	{ return m_messageType; }
	int GetLogLevel()			{ return m_logLevel; }
	int GetRecordType()			{ return m_recordType; }
	char* GetFileLine()			{ return m_fileLine; }
	char* GetBuffer()			{ return m_buffer; }
	char* GetCategoryName()		{ return m_categoryName; }
	BOOL  GetTcpState()			{ return m_tcpState; }
	ioILogger* GetInstance()	{ return m_instance; }
	WORD GetHour()				{ return m_st.wHour; }
	WORD GetMinute()			{ return m_st.wMinute; }
	WORD GetSecond()			{ return m_st.wSecond; }
	SYSTEMTIME* GetSt()			{ return &m_st; }
	int GetCategoryNum() const { return m_categoryNum; }
	void SetCategoryNum(int val) { m_categoryNum = val; }
	
protected:
	LogMessageTypes m_messageType;
	int32 m_logLevel;
	int32 m_recordType;		
	char m_fileLine[ MAX_BUFFER_FILE_LINE ];
	char m_buffer[ MAX_BUFFER_SIZE ];
	char m_categoryName[MAX_CATEGORY_NAME_SIZE];
	int m_categoryNum;

	BOOL m_tcpState;
	SYSTEMTIME m_st;
	ioILogger* m_instance;
};
