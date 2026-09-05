

#include "../stdafx.h"

#include "../include/CLog.h"

#define ERROR_TITLE "Error - LSLog"

float fDummy;	// for R6002 Run-time Error

LOG_API void MBox(HWND hWnd, LPSTR title, LPSTR fmt,...)
{
	fDummy = 2.0f;

	ShowCursor(TRUE);

	static char szBuf[10240];

	va_list args;
    va_start(args,fmt);
    vsprintf(szBuf,fmt,args);
    va_end(args);
	
	if(strlen(szBuf) >= 10000 )
	{
		MessageBox( hWnd, "MBox string over", ERROR_TITLE, MB_OK );
		return;
	}

	MessageBox(hWnd, szBuf, title,MB_OK);
	ShowCursor(FALSE);
}

LOG_API void DebugMBox(LPSTR filename, int linenum, HWND hWnd, LPSTR szStr,...)
{
	ShowCursor(TRUE);
    
	static char szBuf[10240];
	
	va_list args;
    va_start(args,szStr);
    vsprintf(szBuf,szStr,args);
    va_end(args);

	if (strlen(szBuf) >= 10000)
   {
		MessageBox( hWnd, "DebugMBox string over", ERROR_TITLE, MB_OK );
		return;
   }

	char szTitle[MAX_PATH];
	sprintf(szTitle, "DEBUG [FILE : %s, LINE : %d]",filename, linenum );

	MessageBox(hWnd, szBuf, szTitle,MB_OK);
	ShowCursor(FALSE);
}

CLog::CLog()
{
	memset( m_szFileName, 0, sizeof(m_szFileName) );
	m_iDebugLevel = 0;

	m_pFile = NULL;
	m_pConsol = NULL;
}

CLog::~CLog()
{
}

void CLog::CloseAndRelease()
{
	CloseLog();
}

void CLog::OpenLog(int debuglv, const char *szFileName, bool is_append)
{
	m_iDebugLevel = debuglv;

	WriteReadyFolderAndFiles( szFileName );

	if( is_append == false )
	{
		m_pFile = fopen(m_szFileName, "w");
		if( !m_pFile )
		{
			MessageBox(NULL, "CLog::OpenLog Error", ERROR_TITLE, MB_OK);
			return;
		}
	}
	else
	{
		m_pFile = fopen(m_szFileName, "a");
		if( !m_pFile )
		{
			MessageBox(NULL, "CLog::OpenLog Error", ERROR_TITLE, MB_OK);
			return;
		}
	}

	if( m_iDebugLevel != 0 )
	{
		AllocConsole(); 
		m_pConsol = freopen( "CONOUT$", "a", stdout );	
	}

	PrintTimeAndLog(0, "" );
	PrintTimeAndLog(0,"<<< --------------------  Create File -------------------- >>>");
	PrintTimeAndLog(0, "" );
}

void CLog::OpenConsole( int debuglv )
{
	m_iDebugLevel = debuglv;

	if( m_iDebugLevel != 0 )
	{
		AllocConsole(); 
		m_pConsol = freopen( "CONOUT$", "a", stdout );	
	}
}

void CLog::WriteReadyFolderAndFiles( const char *szFileName )
{
	char szAppDir[MAX_PATH];
	GetCurrentDirectory( MAX_PATH, szAppDir );

	sprintf( m_szFileName, "%s/%s", szAppDir, szFileName );

	char szDrive[MAX_PATH], szDir[MAX_PATH];
	_splitpath( m_szFileName, szDrive, szDir, NULL, NULL );

	char szWritePath[MAX_PATH];
	sprintf( szWritePath, "%s/%s", szDrive, szDir );

	const char *pDir = szWritePath;
	char createDir[MAX_PATH] = "";
	int len = 0;
	while (*pDir != '\0') 
	{
		if( *pDir=='/' || 
			*pDir=='\\' )
		{
			CreateDirectory(createDir,NULL);			
		}
		if(len >= MAX_PATH)
			break;
		createDir[len++] = *pDir;
		pDir++;
	}
	CreateDirectory(createDir,NULL);
}

void CLog::PrintNoEnterLog(int debuglv, LPSTR fmt,...)
{
	if( !m_pFile )	return;
	if( debuglv != 0 && debuglv != m_iDebugLevel )
		return;

	va_list args;
	va_start(args,fmt);
	vsprintf(m_szBuf,fmt,args);
	va_end(args);

	fwrite( m_szBuf, strlen(m_szBuf), 1, m_pFile );
	fflush( m_pFile );

	if( m_pConsol )
	{
		printf( "%s", m_szBuf );
	}
}

void CLog::PrintLog(int debuglv, LPSTR fmt,...)
{
	if( !m_pFile )	return;
	if( debuglv != 0 && debuglv != m_iDebugLevel )
		return;

	va_list args;
	va_start(args,fmt);
	vsprintf(m_szBuf,fmt,args);
	va_end(args);

	lstrcat(m_szBuf,"\r\n");
	fwrite(m_szBuf, strlen(m_szBuf), 1, m_pFile);
	fflush( m_pFile );

	if( m_pConsol )
	{
		printf( "%s", m_szBuf );
	}
}

void CLog::PrintTimeAndLog(int debuglv, LPSTR fmt,...)
{
	if( !m_pFile )	return;
	if( debuglv != 0 && debuglv != m_iDebugLevel )
		return;

	SYSTEMTIME st;
	GetLocalTime(&st);
	sprintf(m_szBuf, "%02d:%02d.%02d lv%d ", st.wHour, st.wMinute, st.wSecond, debuglv);
	fwrite(m_szBuf, strlen(m_szBuf), 1, m_pFile);
	fflush( m_pFile );

	va_list args;
	va_start(args,fmt);
	vsprintf(m_szBuf,fmt,args);
	va_end(args);

	lstrcat(m_szBuf,"\r\n");
	fwrite(m_szBuf, strlen(m_szBuf), 1, m_pFile);
	fflush( m_pFile );

	if( m_pConsol )
	{
		printf( "%s", m_szBuf );
	}
}

void CLog::PrintConsoleLog(int debuglv,LPSTR fmt,...)
{
	if( !m_pFile )	return;
	if( debuglv != 0 && debuglv != m_iDebugLevel )
		return;
	
	va_list args;
	va_start(args,fmt);
	vsprintf(m_szBuf,fmt,args);
	va_end(args);

	printf( "%s\n", m_szBuf );
}

void CLog::DebugLog(int debuglv, LPSTR filename, int linenum, LPSTR fmt,...)
{
	if( !m_pFile )	return;
	if( debuglv != 0 && debuglv != m_iDebugLevel )
		return;

	SYSTEMTIME st;
	GetLocalTime(&st);
	sprintf(m_szBuf, "%02d:%02d.%02d lv%d ", st.wHour, st.wMinute, st.wSecond, debuglv);
	fwrite(m_szBuf, strlen(m_szBuf), 1, m_pFile);
	fflush( m_pFile );

	sprintf(m_szBuf, "[FILE : %s, LINE : %d] ", filename, linenum);
	fwrite(m_szBuf, strlen(m_szBuf), 1, m_pFile);
	fflush( m_pFile );

	va_list args;
	va_start(args,fmt);
	vsprintf(m_szBuf,fmt,args);
	va_end(args);

	lstrcat( m_szBuf, "\r\n" );
	fwrite(m_szBuf, strlen(m_szBuf), 1, m_pFile);
	fflush( m_pFile );

	if( m_pConsol )
	{
		printf( "%s", m_szBuf );
	}
}

void CLog::CloseLog()
{
	if( m_pFile )
	{
		PrintTimeAndLog(0, "" );
		PrintTimeAndLog(0, "<<< --------------------  End File -------------------- >>>");
		PrintTimeAndLog(0, "" );
		fclose(m_pFile);
		m_pFile = NULL;
	}

	if(m_pConsol)
	{
		getch();
		
		fclose(m_pConsol);
		m_pConsol = NULL;

		FreeConsole();
	}
}

LOG_API CLog LOG;