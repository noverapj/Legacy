

#ifndef _CLog_h_
#define _CLog_h_

class LOG_API CLog
{
protected:
	char	m_szFileName[MAX_PATH];
	int		m_iDebugLevel;

	FILE	*m_pFile;
	FILE	*m_pConsol;
	char	m_szBuf[2048];

public:
	// debug lv : 0 Release version, 0302(3¿ù2ÀÏ) private debug version 
	void OpenLog( int debuglv, const char *szFileName, bool is_append = false );
	void CloseLog();

public:
	void OpenConsole( int debuglv );

public:
	void PrintNoEnterLog(int debuglv, LPSTR fmt,...);
	void PrintLog(int debuglv, LPSTR fmt,...);
	void PrintTimeAndLog(int debuglv, LPSTR fmt, ...);
	void DebugLog(int debuglv, LPSTR filename, int linenum, LPSTR fmt,...);
	void PrintConsoleLog(int debuglv,LPSTR fmt,...);
	void CloseAndRelease();

protected:
	void WriteReadyFolderAndFiles( const char *szFileName );

public:
	CLog();
	virtual ~CLog();
};

extern LOG_API CLog LOG;

#define ioASSERT(f)			{if(!(f)) {LOG.DebugLog(0, __FILE__,__LINE__,"ASSERT Fail,return");		    assert(f);return;}}
#define ioASSERT_minusone(f){if(!(f)) {LOG.DebugLog(0, __FILE__,__LINE__,"ASSERT Fail,return -1");		assert(f);return -1;}}
#define ioASSERT_false(f)	{if(!(f)) {LOG.DebugLog(0, __FILE__,__LINE__,"ASSERT Fail,return false");	assert(f);return false;}}

LOG_API void MBox( HWND hWnd, LPSTR title, LPSTR fmt,...);
LOG_API void DebugMBox( LPSTR filename, int linenum, HWND hWnd, LPSTR fmt,...);

#endif	// _CLog_h_
