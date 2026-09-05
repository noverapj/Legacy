#pragma once

class ioMainProcess : public CProcessor
{
	static ioMainProcess *sg_Instance;

public:
	static ioMainProcess &GetInstance();
	static void ReleaseInstance();

protected:
	ioHashString m_szTitle;
	ioHashString m_szINI;
	ioHashString m_szLogFolder;
	BOOL m_logServerState;

protected:
	float m_fQueryCheckTime;
	DWORD m_dwLogCheckTime;
	DWORD m_dwPacketLogTime;
	int   m_iQueryThreadCnt;
	int   m_iThreadIndexSeqStyle;
	DWORD m_dwThreadIndexSeq;

protected:
	DWORD m_dwMainLoopCheckTime;
	__int64 m_i64MainLoopCount;

protected:
	bool ReloadINI();

public:
	bool LoadQuery();

protected:
	void ProcessPacketLog( DWORD dwProcessTime );
	void ProcessMainLoopCheck( DWORD dwProcessTime );	

public:	
	void SetINI( const char* szINI )			{ m_szINI = szINI; }
	void SetLogFolder( const char* szFolder )	{ m_szLogFolder = szFolder; }

	bool Initialize( const ioHashString &szTile );
	void LogCurrentStates( DWORD dwProcessTime );

public:
	virtual void Process(uint32& idleTime);

public:
	const ioHashString& GetINI() const				{ return m_szINI; }
	const ioHashString& GetLogFolder() const		{ return m_szLogFolder; }

	inline const float GetQueryCheckTime() const	{ return m_fQueryCheckTime; }
	inline const int GetQueryThreadCnt() const		{ return m_iQueryThreadCnt; }
	int GetThreadIndexSeq( DWORD dwQueryIndex );

	BOOL GetLoggerState() const { return m_logServerState; }
	void SetLoggerState(BOOL val) { m_logServerState = val; }

	// 재정의
	void PrintTimeAndLog(int debuglv, LPSTR fmt );
	void DebugLog(int debuglv, LPSTR filename, int linenum, LPSTR fmt );

	// 종료
	void Save();

private: /* Singleton */
	ioMainProcess();
	virtual ~ioMainProcess();
};

#define g_MainProc  ioMainProcess::GetInstance()