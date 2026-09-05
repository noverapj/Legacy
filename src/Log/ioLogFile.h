#pragma once

#ifdef ASSERT
#undef ASSERT
#endif

#define ASSERT(f)			{if(!(f)) {LOG.DebugLog(0, __FILE__,__LINE__,"ASSERT Fail,return");		    assert(f);return;}}
#define ASSERT_minusone(f)	{if(!(f)) {LOG.DebugLog(0, __FILE__,__LINE__,"ASSERT Fail,return -1");		assert(f);return -1;}}
#define ASSERT_false(f)		{if(!(f)) {LOG.DebugLog(0, __FILE__,__LINE__,"ASSERT Fail,return false");	assert(f);return false;}}

//-----------------------------------------------------
// ioLogFile.cpp / .h
//  : 기존 사용하던 네이밍을 사용하기위해 CLog로 사용.
//-----------------------------------------------------

class CLog : public ILog
{
public:
	CLog(void);
	virtual ~CLog(void);

private:
	void Init();
	void Destroy();

	void SetDebugLevel( const int32 debugID ){ m_debugLevel = debugID; }
	void WriteReadyFolderAndFiles( char *fName );

private:
	int m_debugLevel;
	char m_fileName[ 255 ];

#ifdef DIRECT_WRITE
	FILE* m_pFile;
public:
	CRITICAL_SECTION	m_critical_section;
#else
	// file *
	ioFileWriter m_fileWriter;
#endif

public:
	// 재정의
	virtual void ExcutePorcess( LogData* messageLogData );
	virtual void ExcuteTerminate();

public:
	// 기존함수
	void OpenLog(int debuglv, char* fileName, bool is_append = false);
	void SaveLog();
	void CloseLog();
	void TerminateLog( LogData* message );
	void ExcuteOpen( LogData* messageLogData );

	void PrintNoEnterLog(int debuglv, LPSTR fmt,...);
	void PrintLog(int debuglv, LPSTR fmt,...);
	void PrintTimeAndLog(int debuglv, LPSTR fmt,...);
	void DebugLog(int debuglv, LPSTR filename, int linenum, LPSTR fmt,...);
};

#ifdef DIRECT_WRITE

class LogSync  
{
private:
	CLog *m_pLOG;
	
public:
	LogSync( CLog *pLog );
	virtual ~LogSync();
};

#endif
