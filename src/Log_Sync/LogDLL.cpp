// Log.cpp: implementation of the CLog class.
//
//////////////////////////////////////////////////////////////////////

#include "LogDLL.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}

LOG_SYNC_API CLog LOG;


LOG_SYNC_API void MBox(HWND thwnd, LPSTR title, LPSTR szStr,...)
{
	ShowCursor(TRUE);
    static char szBuf[10240];
	va_list args;
    va_start(args,szStr);
    vsprintf_s(szBuf,szStr,args);
    va_end(args);
	
	if (strlen(szBuf) >= 10000)
	{
		MessageBox(thwnd, "string over", "Error - LogDLL",MB_OK);
		return;
	}
	
	MessageBox(thwnd, szBuf, title,MB_OK);
	ShowCursor(FALSE);
}

LOG_SYNC_API void DebugMBox(LPSTR filename, int linenum, HWND thwnd, LPSTR szStr,...)
{
	ShowCursor(TRUE);
    static char szBuf[10240];
	va_list args;
    va_start(args,szStr);
    vsprintf_s(szBuf,szStr,args);
    va_end(args);

	if (strlen(szBuf) >= 10000)
   {
		MessageBox(thwnd, "string over", "Error - LogDLL",MB_OK);
		return;
   }

	static char title[10240];
	sprintf_s(title, "DEBUG [FILE : %s, LINE : %d]",filename, linenum );

	MessageBox(thwnd, szBuf, title,MB_OK);
	ShowCursor(FALSE);
}


LogSync::LogSync( CLog *pLog ) : m_pLOG(pLog)
{
	if(m_pLOG == NULL) return;
	::EnterCriticalSection(&m_pLOG->m_critical_section);
}

LogSync::~LogSync()
{
	if(m_pLOG == NULL) return;
	::LeaveCriticalSection(&m_pLOG->m_critical_section);
}

CLog::CLog()
{
	m_pFile = NULL;
	memset(m_file_name, 0, sizeof (m_file_name));
	m_debug_level = 0;
	InitializeCriticalSection(&m_critical_section);
}

CLog::~CLog()
{
	DeleteCriticalSection(&m_critical_section);
}

void CLog::CloseAndRelease()
{
	CloseLog();	
}

void CLog::OpenLog(int debuglv, char *fName, bool is_append)
{
	m_debug_level = debuglv;
	strcpy_s (m_file_name, fName);
	WriteReadyFolderAndFiles(m_file_name);
	if (is_append == false)
	{
		if ((m_pFile = _fsopen(m_file_name, "w",_SH_DENYNO)) == NULL) 
		{
			MessageBox(NULL, "LOG OpenLOG Error", "Error - LogDLL", MB_OK);
			return;
		}
	}
	else
	{
		if ((m_pFile = _fsopen(m_file_name, "a",_SH_DENYNO)) == NULL) 
		{
			MessageBox(NULL, "LOG OpenLOG Error", "Error - LogDLL", MB_OK);
			return;
		}
	}
	
	PrintTimeAndLog(0,"");
	PrintTimeAndLog(0,"<<< --------------------  Create File -------------------- >>>");
	PrintTimeAndLog(0,"");
}

void CLog::WriteReadyFolderAndFiles(char *fName)
{
	char   szFileName[255] ;
	WIN32_FIND_DATA fd ;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	HANDLE hFind2 = INVALID_HANDLE_VALUE;
	char pTemp[255]="";
	char pDataNum[255]="";
	char szTmp[255] ;
	char  *pdest ; 
	char  *ptemp;
    int   result;
	char   szAppDir[255] ;
	GetCurrentDirectory(255, (LPSTR)szAppDir) ;

	sprintf_s(szFileName, "%s\\%s",szAppDir, fName) ;
	hFind = FindFirstFile(szFileName, &fd);
	if (hFind == INVALID_HANDLE_VALUE ) 
	{
		pdest = (char *)fName ;
		do
		{
			ptemp = strchr(pdest, '\\') ;
			if ( ptemp  != NULL )
			{
				memset(szFileName, 0, sizeof(szFileName) ) ;
				result = ptemp - fName  ;
				strncpy_s(szFileName, fName , result) ;
				sprintf_s(szTmp, "%s\\%s", szAppDir, szFileName) ;
				hFind2 = FindFirstFile(szTmp, &fd);
				if (hFind2 == INVALID_HANDLE_VALUE ) 
				{   
					_mkdir( szTmp ) ;
				}
				else
					FindClose (hFind2);
				pdest = ptemp+1 ;
			}
		} while ( ptemp != NULL ) ;
	}
	else 
		FindClose (hFind);
	sprintf_s(szFileName, "%s\\%s", szAppDir, fName) ;

	SetFileAttributes( szFileName, FILE_ATTRIBUTE_ARCHIVE ) ;
}

void CLog::SaveLog()
{
	if( m_pFile == NULL ) return;

	LogSync ls(this);
	
	fclose(m_pFile);
	m_pFile = NULL;
	if ((m_pFile  = _fsopen(m_file_name, "a",_SH_DENYNO))== NULL)
	{
		MessageBox(NULL, "PrintLog Error fopen", "Error - LogDLL", MB_OK);
	}	
}

void CLog::PrintNoEnterLog(int debuglv, LPSTR fmt,...)
{
	if( debuglv < m_debug_level)
		return;

	LogSync ls(this);
	if(fseek(m_pFile, 0, SEEK_END))
		return;
	
	static char buffer[2048];
	strcpy_s (buffer, "");
	
	va_list args;
    
	va_start(args,fmt);
	vsprintf_s(buffer,fmt,args);
	va_end(args);
	fwrite(buffer, strlen(buffer), 1, m_pFile);
	if (fseek(m_pFile, 0, SEEK_END))
		return;

//	fclose(m_pFile);
//	m_pFile = NULL;
//	if ((m_pFile  = fopen(m_file_name, "a"))== NULL)
//	{
//		MessageBox(NULL, "PrintLog Error fopen", "Error - LogDLL", MB_OK);
//		return;
//	}	
}

void CLog::PrintLog(int debuglv, LPSTR fmt,...)
{
	if( debuglv < m_debug_level)
		return;

	LogSync ls(this);
	if (fseek(m_pFile, 0, SEEK_END))
		return;
	
	static char buffer[2048];
	strcpy_s (buffer, "");

	sprintf_s(buffer, "lv%d ",debuglv);

	va_list args;
	va_start(args,fmt);
	vsprintf_s(buffer,fmt,args);
	va_end(args);
	fwrite(buffer, strlen(buffer), 1, m_pFile);
	if (fseek(m_pFile, 0, SEEK_END))
		return;
	
	strcpy_s(buffer,"");
	lstrcat(buffer,"\r\n");
	fwrite(buffer, strlen(buffer), 1, m_pFile);

//	fclose(m_pFile);
//	m_pFile = NULL;
//	if ((m_pFile  = fopen(m_file_name, "a"))== NULL)
//	{
//		MessageBox(NULL, "PrintLog Error fopen", "Error - LogDLL", MB_OK);
//		return;
//	}	
}


void CLog::PrintTimeAndLog(int debuglv, LPSTR fmt,...)
{
	if( debuglv < m_debug_level)
		return;

	LogSync ls(this);
	if (fseek(m_pFile, 0, SEEK_END))
		return;
	
	static char buffer[2048];
	strcpy_s (buffer, "");

	SYSTEMTIME st;
	GetLocalTime(&st);
	sprintf_s(buffer, "%02d:%02d.%02d lv%d ", st.wHour, st.wMinute, st.wSecond, debuglv);
	fwrite(buffer, strlen(buffer), 1, m_pFile);
	if (fseek(m_pFile, 0, SEEK_END))
		return;
	
	strcpy_s (buffer, "");
	
	va_list args;
    
	va_start(args,fmt);
	vsprintf_s(buffer,fmt,args);
	va_end(args);
	if (strlen(buffer) >= 2000)	
	{
		MessageBox(NULL, "PrintLog Error overflow string", "Error - LogDLL", MB_OK);
		return;
	}
	fwrite(buffer, strlen(buffer), 1, m_pFile);
	if(fseek(m_pFile, 0, SEEK_END))
		return;
	
	strcpy_s(buffer,"");
	lstrcat(buffer,"\r\n");
	fwrite(buffer, strlen(buffer), 1, m_pFile);

//	fclose(m_pFile);
//	m_pFile = NULL;
//	if ((m_pFile  = fopen(m_file_name, "a"))== NULL)
//	{
//		MessageBox(NULL, "PrintLog Error fopen", "Error - LogDLL", MB_OK);
//		return;
//	}	
}

void CLog::PrintConsoleLog(int debuglv,LPSTR fmt,...)
{
	if( debuglv < m_debug_level)
		return;

	LogSync ls(this);	
	static char buffer[2048];
	strcpy_s (buffer, "");
	
	va_list args;
    
	va_start(args,fmt);
	vsprintf_s(buffer,fmt,args);
	va_end(args);

	printf("%s\n",buffer);	
}

void CLog::DebugLog(int debuglv, LPSTR filename, int linenum, LPSTR fmt,...)
{
	if( debuglv < m_debug_level)
		return;

	LogSync ls(this);
	if (fseek(m_pFile, 0, SEEK_END))
		return;
	
	static char buffer[2048];
	strcpy_s (buffer, "");
	
	SYSTEMTIME st;
	GetLocalTime(&st);
	sprintf_s(buffer, "%02d:%02d.%02d lv%d ", st.wHour, st.wMinute, st.wSecond, debuglv);
	fwrite(buffer, strlen(buffer), 1, m_pFile);
	if (fseek(m_pFile, 0, SEEK_END))
		return;
	
	strcpy_s (buffer, "");

	// write debug info.
	
	sprintf_s(buffer, "[FILE : %s, LINE : %d] ", filename, linenum);
	fwrite(buffer, strlen(buffer), 1, m_pFile);

	if (fseek(m_pFile, 0, SEEK_END))
		return;
	strcpy_s (buffer, "");
	
	va_list args;
    
	va_start(args,fmt);
	vsprintf_s(buffer,fmt,args);
	va_end(args);
	fwrite(buffer, strlen(buffer), 1, m_pFile);
	if(fseek(m_pFile, 0, SEEK_END))
		return;
	
	strcpy_s(buffer,"");
	lstrcat(buffer,"\r\n");
	fwrite(buffer, strlen(buffer), 1, m_pFile);

//	fclose(m_pFile);
//	m_pFile = NULL;
//	if ((m_pFile  = fopen(m_file_name, "a"))== NULL)
//	{
//		MessageBox(NULL, "PrintLog Error fopen", "Error - LogDLL", MB_OK);
//		return;
//	}	
}


void CLog::CloseLog()
{
	if (m_pFile!=NULL)
	{
		PrintTimeAndLog(0, "");
		PrintTimeAndLog(0, "<<< --------------------  End File -------------------- >>>");
		PrintTimeAndLog(0, "");
		fclose(m_pFile);
	}
	m_pFile = NULL;
	
}

