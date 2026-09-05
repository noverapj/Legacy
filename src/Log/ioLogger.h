#pragma once

#include "ioILogger.h"
#include "ioFile.h"
#include "ioDefine.h"

#ifdef USE_THREAD

class ioLogger : public ioILogger
{
public:
	ioLogger(void);
	virtual ~ioLogger(void);

private:
	void Init();
	void Destroy();

public:
	// 재정의
	virtual void ExcuteOpen(CLogData* logData);
	virtual void ExcuteWrite(CLogData* logData);
	virtual void ExcuteClose(CLogData* logData);
	virtual void ExcuteSetCategory(CLogData* logData);
	virtual	BOOL GetTcpState()	{ return m_tcpState; }
	virtual void ExcuteInitData();

private:
	void CreateFolder( char* file );
	BOOL CheckLevel(const int level);

public:
	void SetLogLevel( const int32 logLevel )	{ m_logLevel = logLevel; }
	const int GetLogLevel()						{ return m_logLevel; }
	// 기존함수
	void OpenLog(int logLevel, char* fileName, bool append = false);
	void CloseLog();
	void PrintNoEnterLog(int logLevel, char* text);
	void PrintLog(int logLevel, char* text);
	void PrintTimeAndLog(int logLevel, char* text);
	void DebugLog(int logLevel, char* text, char* fileLine);

public://get/set
	void SetCategoryName(char* categoryName, BOOL tcpState);
	const char* GetCategoryName() { return m_categoryName; }
	const char* GetOpenFileName() { return m_openFileName; }
	void SetTcpState(BOOL val) { m_tcpState = val; }
	void SetInitData();
	BOOL GetOpenState() const { return m_openState; }
	void SetOpenState(BOOL val) { m_openState = val; }
	int GetCategoryIndex() const { return m_categoryIndex; }
	void SetCategoryIndex(int val) { m_categoryIndex = val; }

private:
	char m_openFileName[MAX_FILENAME_SIZE];
	char m_categoryName[MAX_CATEGORY_NAME_SIZE];
	int m_logLevel;
	ioFileWriter m_fileWriter;
	BOOL m_tcpState;
	BOOL m_openState;
	int m_categoryIndex;
};


#else // USE_THREAD

class ioLogger;

class LogSync  
{
private:
	ioLogger *m_pLOG;
	
public:
	LogSync( ioLogger *pLog );
	virtual ~LogSync();
};

class ioLogger
{
public:
	ioLogger(void);
	virtual ~ioLogger(void);

private:
	void Init();
	void Destroy();

	void CreateFolder( char *file );
	BOOL CheckLevel(const int level);

public:
	void SetLogLevel( const int32 logLevel )	{ m_logLevel = logLevel; }
	const int GetLogLevel()						{ return m_logLevel; }

private:
	int m_logLevel;
	std::string m_fileName;

	FILE* m_pFile;
public:
	CRITICAL_SECTION	m_critical_section;

public:
	// 기존함수
	void OpenLog(int logLevel, char* fileName, bool append = false);
	void CloseLog();

	void PrintNoEnterLog(int logLevel, char* text, BOOL tcpMode = FALSE);
	void PrintLog(int logLevel, char* text, BOOL tcpMode = FALSE);
	void PrintTimeAndLog(int logLevel, char* text, BOOL tcpMode = FALSE);
	void DebugLog(int logLevel, char* text, char* fileLine, BOOL tcpMode = FALSE);
public://TCP 관련 함수 
	int CreateTCPConnect(int port);
};

#endif // USE_THREAD
