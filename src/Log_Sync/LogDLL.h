#ifndef ___LOG_H__
#define ___LOG_H__

#ifdef EXPORT_LOG_SYNC
#define LOG_SYNC_API __declspec(dllexport)
#else
#define LOG_SYNC_API __declspec(dllimport)
#endif

#define WIN32_LEAN_AND_MEAN

#include <stdio.h>	
#include <windows.h>
#include <direct.h>
#include <assert.h>
#include <conio.h>
#include <share.h>

#ifndef LOG_SYNC_API
#define LOG_SYNC_API __declspec(dllexport)
#endif

/*
#ifndef __FLTUSED__
#define __FLTUSED__
extern "C" __declspec(selectany) int _fltused=1;
#endif
*/
#pragma warning(disable:4251)


#ifdef ASSERT
#undef ASSERT
#endif

#define ASSERT(f)			{if(!(f)) {LOG.DebugLog(0, __FILE__,__LINE__,"ASSERT Fail,return");		    assert(f);return;}}
#define ASSERT_minusone(f)	{if(!(f)) {LOG.DebugLog(0, __FILE__,__LINE__,"ASSERT Fail,return -1");		assert(f);return -1;}}
#define ASSERT_false(f)		{if(!(f)) {LOG.DebugLog(0, __FILE__,__LINE__,"ASSERT Fail,return false");	assert(f);return false;}}

class LOG_SYNC_API CLog
{
public:
	char				m_file_name[255];
	int					m_debug_level;
	CRITICAL_SECTION	m_critical_section;

	void OpenLog(int debuglv, char* fName, bool is_append = false);	
	void CloseLog();
	void PrintNoEnterLog(int debuglv, LPSTR fmt,...); 
	void PrintLog(int debuglv, LPSTR fmt,...);
	void PrintTimeAndLog(int debuglv, LPSTR fmt,...);
	void DebugLog(int debuglv, LPSTR filename, int linenum, LPSTR fmt,...);
	void WriteReadyFolderAndFiles(char *fName);
	void PrintConsoleLog(int debuglv,LPSTR fmt,...);
	void SaveLog();
	void CloseAndRelease();

	CLog();
	virtual ~CLog();

protected:
	FILE* m_pFile;
};

class LOG_SYNC_API LogSync  
{
private:
	CLog *m_pLOG;
	
public:
	LogSync( CLog *pLog );
	virtual ~LogSync();
};

extern LOG_SYNC_API CLog LOG;
LOG_SYNC_API void MBox(HWND thwnd, LPSTR title, LPSTR szStr,...);
LOG_SYNC_API void DebugMBox(LPSTR filename, int linenum, HWND thwnd, LPSTR szStr,...);

#endif 
