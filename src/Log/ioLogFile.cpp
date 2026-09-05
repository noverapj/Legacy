
#include "stdafx.h"
#include "ioLogFile.h"


CLog::CLog(void)
{
	Init();
}


CLog::~CLog(void)
{
	Destroy();
}

void CLog::Init()
{
	m_debugLevel = 0;
	ZeroMemory( m_fileName, sizeof( m_fileName ) );

	// 서버 내려갈때 file close를 위해 등록.
	g_LogManager->Register( this );

#ifdef DIRECT_WRITE
	InitializeCriticalSection(&m_critical_section);
#endif
}

void CLog::Destroy()
{
#ifdef DIRECT_WRITE
	DeleteCriticalSection(&m_critical_section);
#endif
}

void CLog::WriteReadyFolderAndFiles( char *fName )
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

void CLog::OpenLog(int debuglv, char* fileName, bool is_append )
{
#ifdef DIRECT_WRITE
	m_debugLevel = debuglv;
	strcpy_s (m_fileName, fileName);
	WriteReadyFolderAndFiles(m_fileName);
	if (is_append == false)
	{
		if ((m_pFile = _fsopen(m_fileName, "w",_SH_DENYNO)) == NULL) 
		{
			MessageBox(NULL, "LOG OpenLOG Error", "Error - LogDLL", MB_OK);
			return;
		}
	}
	else
	{
		if ((m_pFile = _fsopen(m_fileName, "a",_SH_DENYNO)) == NULL) 
		{
			MessageBox(NULL, "LOG OpenLOG Error", "Error - LogDLL", MB_OK);
			return;
		}
	}
	
	PrintTimeAndLog(0,"");
	PrintTimeAndLog(0,"<<< --------------------  Create File -------------------- >>>");
	PrintTimeAndLog(0,"");
#else
	// Pop
	LogData* pData = g_LogBufferManager->Pop();
	if( pData == NULL )
		return;

	pData->debugid = debuglv;
	strcpy_s( pData->fileline, sizeof( pData->fileline ), fileName );
	pData->messageType = LOG_MESSAGE_TYPE_OPEN;

	
	// Queue에 던짐.
	pData->pointerLog = this;
	g_MessageQueue.Enqueue( (DWORD)pData, sizeof( LogData ) );
#endif
}

void CLog::ExcutePorcess( LogData* messageLogData )
{
	if( messageLogData == NULL )
		return;
	if( messageLogData->debugid < 0 )
	{
		PushData(messageLogData);
		return;
	}
	
	char temp[ MAX_BUFFER_SIZE ] = {0,};
	
	switch( messageLogData->writeType )
	{
	case LOGFILE_WRITETYPE_PRINTTIMEANDLOG:
		{
			sprintf_s( temp, "%02d:%02d.%02d lv%d %s \r\n", messageLogData->st.wHour, messageLogData->st.wMinute, messageLogData->st.wSecond, messageLogData->debugid, messageLogData->buffer );
		}
		break;

	case LOGFILE_WRITETYPE_DEBUGLOG:
		{
			sprintf_s( temp, "%02d:%02d.%02d lv%d %s %s \r\n", messageLogData->st.wHour, messageLogData->st.wMinute, messageLogData->st.wSecond, messageLogData->debugid, messageLogData->fileline, messageLogData->buffer );
		}
		break;

	case LOGFILE_WRITETYPE_PRINTLOG:
		{
			sprintf_s( temp, "lv%d %s \r\n", messageLogData->debugid, messageLogData->buffer );
		}
		break;

	case LOGFILE_WRITETYPE_PRINTNOENTERLOG:
		{
			sprintf_s( temp, "%s", messageLogData->buffer );
		}
		break;
	}
#ifdef DIRECT_WRITE
#else
	// write
	m_fileWriter.WriteFormat( _T("%s"), temp );
	
	// return
	PushData( messageLogData );
#endif
}

void CLog::ExcuteClose()
{
#ifdef DIRECT_WRITE
#else
	// file close
	m_fileWriter.Close();
#endif
}

void CLog::ExcuteOpen( LogData* messageLogData )
{
#ifdef DIRECT_WRITE
#else
	// DebugLevel 기억.
	SetDebugLevel( messageLogData->debugid );

	// FileName
	strcpy_s( m_fileName, sizeof( m_fileName ), messageLogData->fileline );

	// Folder
	WriteReadyFolderAndFiles( m_fileName );

	// file pointer
	if( ! m_fileWriter.Open( m_fileName, OPEN_ALWAYS ) )
	{
		return;
	}

	// offset을 뒤로..
	m_fileWriter.Move( FILE_END, 0 );

	PrintTimeAndLog(0,"");
	PrintTimeAndLog(0,"<<< --------------------  Create File -------------------- >>>");
	PrintTimeAndLog(0,"");

	// return
	PushData( messageLogData );
#endif
}

void CLog::SaveLog()
{
	PrintTimeAndLog(0, "");
	PrintTimeAndLog(0, "<<< --------------------  End File -------------------- >>>");
	PrintTimeAndLog(0, "");
}

void CLog::CloseLog()
{
#ifdef DIRECT_WRITE
	if (m_pFile!=NULL)
	{
		PrintTimeAndLog(0, "");
		PrintTimeAndLog(0, "<<< --------------------  End File -------------------- >>>");
		PrintTimeAndLog(0, "");
		fclose(m_pFile);
	}
	m_pFile = NULL;
#else
	SaveLog();

	// Pop
	LogData* pData = g_LogBufferManager->Pop();
	if( pData == NULL )
		return;

	pData->messageType = LOG_MESSAGE_TYPE_CLOSE;
	pData->pointerLog = this;
	g_MessageQueue.Enqueue( (DWORD)pData, sizeof( LogData ) );
#endif
}

void CLog::TerminateLog( LogData* message )
{
	ExcuteClose();

	// return
	PushData( message );
}

void CLog::PrintNoEnterLog(int debuglv, LPSTR fmt,...)
{
#ifdef DIRECT_WRITE
	if( debuglv < m_debugLevel)
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
#else
	if( debuglv < m_debugLevel)
		return;
	
	// Pop
	LogData* pData = g_LogBufferManager->Pop();
	if( pData == NULL )
		return;

	// declare
	SYSTEMTIME st;
	char temp[ MAX_BUFFER_SIZE ] = { 0, };
	char file[ MAX_BUFFER_FILE_LINE ] = { 0, };

	// parameter
	va_list args;
	va_start( args, fmt );
	vsprintf_s( temp, fmt, args );
	va_end( args );

	// check
	if( strlen( temp ) >= MAX_BUFFER_SIZE )
	{
		PushData( pData );
		return;
	}

	// set
	pData->SetData( LOG_MESSAGE_TYPE_PROCESS, debuglv, LOGFILE_WRITETYPE_PRINTNOENTERLOG, st, file, temp );


	// enqueue
	pData->pointerLog = this;
	g_MessageQueue.Enqueue( (DWORD)pData, sizeof( LogData ) );
#endif
}

void CLog::PrintLog(int debuglv, LPSTR fmt,...)
{
#ifdef DIRECT_WRITE
	if( debuglv < m_debugLevel)
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
#else

	if( debuglv < m_debugLevel )
		return;

	// Pop
	LogData* pData = g_LogBufferManager->Pop();
	if( pData == NULL )
		return;
	
	// declare
	SYSTEMTIME st;
	char temp[ MAX_BUFFER_SIZE ] = { 0, };
	char file[ MAX_BUFFER_FILE_LINE ] = { 0, };

	// parameter
	va_list args;
	va_start( args, fmt );
	vsprintf_s( temp, fmt, args );
	va_end( args );

	// check
	if( strlen( temp ) >= MAX_BUFFER_SIZE )
	{
		PushData( pData );
		return;
	}

	// set
	pData->SetData( LOG_MESSAGE_TYPE_PROCESS, debuglv, LOGFILE_WRITETYPE_PRINTLOG, st, file, temp );
	

	// enqueue
	pData->pointerLog = this;
	g_MessageQueue.Enqueue( (DWORD)pData, sizeof( LogData ) );
#endif
}

void CLog::PrintTimeAndLog(int debuglv, LPSTR fmt,...)
{
#ifdef DIRECT_WRITE
	if( debuglv < m_debugLevel)
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
#else
	if( debuglv < m_debugLevel)
		return;
		
	// Pop
	LogData* pData = g_LogBufferManager->Pop();
	if( pData == NULL )
		return;

	// declare
	SYSTEMTIME st;
	GetLocalTime( &st );

	char temp[ MAX_BUFFER_SIZE ] = { 0, };
	char file[ MAX_BUFFER_FILE_LINE ] = { 0, };

	// parameter
	va_list args;
	va_start( args, fmt );
	vsprintf_s( temp, fmt, args );
	va_end( args );

	// check
	if( strlen( temp ) >= MAX_BUFFER_SIZE )
	{
		PushData( pData );
		return;
	}

	// set
	pData->SetData( LOG_MESSAGE_TYPE_PROCESS, debuglv, LOGFILE_WRITETYPE_PRINTTIMEANDLOG, st, file, temp );
	

	// enqueue
	pData->pointerLog = this;
	g_MessageQueue.Enqueue( (DWORD)pData, sizeof( LogData ) );
#endif
}

void CLog::DebugLog(int debuglv, LPSTR filename, int linenum, LPSTR fmt,...)
{
#ifdef DIRECT_WRITE
	if( debuglv < m_debugLevel)
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
#else
	if( debuglv < m_debugLevel )
		return;
	
	// Pop
	LogData* pData = g_LogBufferManager->Pop();
	if( pData == NULL )
		return;

	// declare
	SYSTEMTIME st;
	GetLocalTime( &st );

	char file[ MAX_BUFFER_FILE_LINE ] = { 0, };
	sprintf_s( pData->fileline, "[FILE : %s, LINE : %d] ", filename, linenum );

	char temp[ MAX_BUFFER_SIZE ] = { 0, };

	// parameter
	va_list args;
	va_start( args, fmt );
	vsprintf_s( temp, fmt, args );
	va_end( args );

	// check
	if( ( strlen( file ) >= MAX_BUFFER_FILE_LINE ) || ( strlen( temp ) >= MAX_BUFFER_SIZE ) )
	{
		PushData( pData );
		return;
	}

	// set
	pData->SetData( LOG_MESSAGE_TYPE_PROCESS, debuglv, LOGFILE_WRITETYPE_DEBUGLOG, st, file, temp );
	

	// enqueue
	pData->pointerLog = this;
	g_MessageQueue.Enqueue( (DWORD)pData, sizeof( LogData ) );
#endif
}

#ifdef DIRECT_WRITE

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

#endif
