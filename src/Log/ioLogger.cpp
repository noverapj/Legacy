#include "stdafx.h"
#include <vector>
#include <string>
#include <algorithm>
#include "LogData.h"
#include "ioLogThread.h"
#include "ioLogQueue.h"
#include "ioLogger.h"
#include "ioTCPConnectNode.h"


namespace IOLOGGER
{
	int Tokenizer(const std::string& str,const std::string& delimiters, std::vector<std::string>& tokens)
	{
		std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
		std::string::size_type pos = str.find_first_of(delimiters, lastPos);
		while (std::string::npos != pos || std::string::npos != lastPos)
		{
			tokens.push_back(str.substr(lastPos, pos - lastPos));
			lastPos = str.find_first_not_of(delimiters, pos);
			pos = str.find_first_of(delimiters, lastPos);
		}
		return tokens.size();
	}
	void LTrim(std::string& s,const std::string drop)
	{
		s.erase(0,s.find_first_not_of(drop));
	}

}

using namespace IOLOGGER;

#ifdef USE_THREAD

////////////////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
////////////////////////////////////////////////////////////////////////////////////
ioLogger::ioLogger(void) : m_logLevel(0)
{
	Init();
}

ioLogger::~ioLogger(void)
{
	Destroy();
}

void ioLogger::Init()
{
	ZeroMemory(m_categoryName,sizeof(m_categoryName));
	ZeroMemory(m_openFileName,sizeof(m_openFileName));
	m_tcpState = FALSE;
	m_openState = FALSE;
}

void ioLogger::Destroy()
{
}

////////////////////////////////////////////////////////////////////////////////////
// Operations
////////////////////////////////////////////////////////////////////////////////////

void ioLogger::CreateFolder( char* file )
{
	std::vector<std::string> tokens;
	int count = Tokenizer(file, "\\", tokens);
	if(count < 2) return;
	TCHAR temp[512], folder[512];
	GetCurrentDirectory(_countof(temp), temp);
	sprintf_s(folder, _countof(folder), _T("%s\\%s"), temp, tokens[0].c_str()); 
	CreateDirectory(folder, NULL);
	for(UINT i=1; i< tokens.size() -1; ++i)
	{
		sprintf_s(folder, _countof(folder), _T("%s\\%s"), folder, tokens[i].c_str()); 
		CreateDirectory(folder, NULL);
	}
}

void ioLogger::ExcuteOpen(CLogData* logData)
{
	// 로그 레벨
	SetLogLevel( logData->GetLogLevel() );

	// 폴더 생성
	//여기서 openfilename 저장
	strcpy_s(m_openFileName,logData->GetFileLine());
	CreateFolder( logData->GetFileLine() );
	std::string tmpfileName = logData->GetFileLine();
	LTrim(tmpfileName,"\\");
	
	// 파일생성
	if( !m_fileWriter.Open( tmpfileName.c_str(), OPEN_ALWAYS ) )
	{
		return;
	}

	// 파일 offset이동
	m_fileWriter.Move( FILE_END, 0 );

	// return
	PushData( logData );

	SetOpenState(TRUE);
}

void ioLogger::ExcuteClose(CLogData* logData)
{
	m_fileWriter.Close();

	// return
	PushData( logData );

	SetOpenState(FALSE);
}

void ioLogger::ExcuteWrite(CLogData* logData)
{
	if( (logData == NULL) || (logData->GetLogLevel() < 0) )
		return;
	//이부분에서 send
	
	switch( logData->GetRecordType() )
	{
	case LOGRECORD_PRINTTIMEANDLOG:
		{
			m_fileWriter.WriteFormat( "%02d:%02d.%02d lv%d %s \r\n", 
				logData->GetHour(), 
				logData->GetMinute(), 
				logData->GetSecond(), 
				logData->GetLogLevel(), 
				logData->GetBuffer() );
		}
		break;

	case LOGRECORD_DEBUGLOG:
		{
			m_fileWriter.WriteFormat( "%02d:%02d.%02d lv%d %s %s \r\n", 
				logData->GetHour(), 
				logData->GetMinute(), 
				logData->GetSecond(), 
				logData->GetLogLevel(), 
				logData->GetFileLine(), 
				logData->GetBuffer() );
		}
		break;

	case LOGRECORD_PRINTLOG:
		{
			m_fileWriter.WriteFormat( "lv%d %s \r\n", logData->GetLogLevel(), logData->GetBuffer() );
		}
		break;

	case LOGRECORD_PRINTNOENTERLOG:
		{
			m_fileWriter.WriteFormat( "%s", logData->GetBuffer() );
		}
		break;
	}
	// return
	PushData( logData );
}

void ioLogger::ExcuteSetCategory( CLogData* logData )
{
	strcpy_s(m_categoryName,logData->GetCategoryName());
	m_tcpState = logData->GetTcpState();
	SetCategoryIndex(g_TCPNode->GetCategoryIndex());
}

void ioLogger::ExcuteInitData()
{
	ZeroMemory(m_categoryName,sizeof(m_categoryName));
	m_tcpState = FALSE;
}

BOOL ioLogger::CheckLevel(const int level)
{
	return (level < GetLogLevel()) ? FALSE : TRUE;
}

void ioLogger::OpenLog(int logLevel, char* fileName, bool append )
{
	// Pop
	CLogData* logData = PopData();
	if( logData == NULL ) return;

	logData->Set(this, LOG_MESSAGE_TYPE_OPEN, logLevel, fileName);
	Enqueue(logData);
}

void ioLogger::CloseLog()
{
	CLogData* logData = PopData();
	if( logData == NULL ) return;

	logData->Set(this, LOG_MESSAGE_TYPE_CLOSE);

	Enqueue(logData);
}

void ioLogger::PrintNoEnterLog(int logLevel, char* text)
{
	if( !CheckLevel(logLevel) )
		return;
	
	// Pop
	CLogData* logData = PopData();
	if( logData == NULL )	return;

	// set
	logData->Set(this, LOG_MESSAGE_TYPE_PROCESS, logLevel, LOGRECORD_PRINTNOENTERLOG, text);

	Enqueue(logData);
}

void ioLogger::PrintLog(int logLevel, char* text)
{
	if( !CheckLevel(logLevel) )
		return;

	// Pop
	CLogData* logData = PopData();
	if( logData == NULL )	return;

	// set
	logData->Set(this, LOG_MESSAGE_TYPE_PROCESS, logLevel, LOGRECORD_PRINTLOG, text);

	Enqueue(logData);
}

void ioLogger::PrintTimeAndLog(int logLevel, char* text)
{
	if( !CheckLevel(logLevel) )
		return;
		
	// Pop
	CLogData* logData = PopData();
	if( logData == NULL ) return;

	// set
	logData->Set(this, LOG_MESSAGE_TYPE_PROCESS, logLevel, LOGRECORD_PRINTTIMEANDLOG, text);
	
	Enqueue(logData);
}

void ioLogger::DebugLog(int logLevel, char* text, char* fileLine)
{
	if( !CheckLevel(logLevel) )
		return;
	
	// Pop
	CLogData* logData = PopData();
	if( logData == NULL ) return;

	// set
	logData->Set(this, LOG_MESSAGE_TYPE_PROCESS, logLevel, LOGRECORD_DEBUGLOG, text, fileLine);
	
	Enqueue(logData);

}

void ioLogger::SetCategoryName(char* categoryName, BOOL tcpState)
{
	CLogData* logData = PopData();
	if( logData == NULL ) return;

	// set
	logData->Set(this, LOG_MESSAGE_TYPE_SETCATEGORY,categoryName,tcpState);

	Enqueue(logData);
}

void ioLogger::SetInitData()
{
	CLogData* logData = PopData();
	if( logData == NULL ) return;

	logData->Set(this, LOG_MESSAGE_TYPE_INITDATA);

	Enqueue(logData);
}
#else // USE_THREAD

LogSync::LogSync( ioLogger *pLog ) : m_pLOG(pLog)
{
	if(m_pLOG == NULL) return;
	::EnterCriticalSection(&m_pLOG->m_critical_section);
}

LogSync::~LogSync()
{
	if(m_pLOG == NULL) return;
	::LeaveCriticalSection(&m_pLOG->m_critical_section);
}

ioLogger::ioLogger(void) : m_pFile(NULL), m_logLevel(0)
{
	Init();
}

ioLogger::~ioLogger(void)
{
	Destroy();
}

void ioLogger::Init()
{
	ZeroMemory(m_fileName, sizeof(m_fileName));
	InitializeCriticalSection(&m_critical_section);
}

void ioLogger::Destroy()
{
	CloseLog();

	m_logLevel = 0;
	ZeroMemory(m_fileName, sizeof(m_fileName));

	DeleteCriticalSection(&m_critical_section);
}

void ioLogger::CreateFolder( char *file )
{
	std::vector<std::string> tokens;
	int count = Tokenizer(file, "\\", tokens);
	if(count < 2) return;

	TCHAR temp[512], folder[512];
	GetCurrentDirectory(_countof(temp), temp);
	sprintf_s(folder, _countof(folder), _T("%s\\%s"), temp, tokens[0].c_str()); 
	CreateDirectory(folder, NULL);
}

BOOL ioLogger::CheckLevel(const int level)
{
	return (level < GetLogLevel()) ? FALSE : TRUE;
}

void ioLogger::OpenLog(int logLevel, char* fileName, bool append, BOOL tcpMode)
{
	LogSync ls(this);

	CreateFolder(fileName);
	SetLogLevel(logLevel);

	if(m_pFile != NULL)
	{
		fclose(m_pFile);
		m_pFile = NULL;
	}
	if (append == false)
	{
		m_pFile = _fsopen(fileName, "w",_SH_DENYNO);
	}
	else
	{
		m_pFile = _fsopen(fileName, "a",_SH_DENYNO);
	}
}

void ioLogger::CloseLog()
{
	LogSync ls(this);
	if(m_pFile!=NULL)
	{
		fclose(m_pFile);
		m_pFile = NULL;
	}
}

void ioLogger::PrintNoEnterLog(int logLevel, char* text, BOOL tcpMode)
{
	if( !CheckLevel(logLevel) )
		return;

	LogSync ls(this);

	if(!m_pFile) return;

	if(fseek(m_pFile, 0, SEEK_END))
		return;
	
	fwrite(text, strlen(text), 1, m_pFile);
	if (fseek(m_pFile, 0, SEEK_END))
		return;
}

void ioLogger::PrintLog(int logLevel, char* text, BOOL tcpMode)
{
	if( !CheckLevel(logLevel) )
		return;

	LogSync ls(this);

	if(!m_pFile) return;

	if (fseek(m_pFile, 0, SEEK_END))
		return;
	
	static char buffer[2048];
	sprintf_s(buffer, sizeof(buffer), "lv%d %s\r\n",logLevel, text);

	fwrite(buffer, strlen(buffer), 1, m_pFile);
	if (fseek(m_pFile, 0, SEEK_END))
		return;
}

void ioLogger::PrintTimeAndLog(int logLevel, char* text, BOOL tcpMode)
{
	if( !CheckLevel(logLevel) )
		return;

	LogSync ls(this);

	if(!m_pFile) return;

	if (fseek(m_pFile, 0, SEEK_END))
		return;
	
	static char buffer[2048];
	strcpy_s (buffer, "");

	SYSTEMTIME st;
	GetLocalTime(&st);

	sprintf_s(buffer, "%02d:%02d.%02d lv%d %s\r\n", st.wHour, st.wMinute, st.wSecond, logLevel, text);
	fwrite(buffer, strlen(buffer), 1, m_pFile);
	if (fseek(m_pFile, 0, SEEK_END))
		return;
}

void ioLogger::DebugLog(int logLevel, char* text, char* fileLine, BOOL tcpMode)
{
	if( !CheckLevel(logLevel) )
		return;

	LogSync ls(this);

	if(!m_pFile) return;

	if (fseek(m_pFile, 0, SEEK_END))
		return;
	
	static char buffer[2048];
	strcpy_s (buffer, "");
	
	SYSTEMTIME st;
	GetLocalTime(&st);

	sprintf_s(buffer, "%02d:%02d.%02d lv%d %s\r\n", st.wHour, st.wMinute, st.wSecond, logLevel, text);
	fwrite(buffer, strlen(buffer), 1, m_pFile);
	if (fseek(m_pFile, 0, SEEK_END))
		return;
	
	// write debug info.
	fwrite(fileLine, strlen(fileLine), 1, m_pFile);
	if (fseek(m_pFile, 0, SEEK_END))
		return;
}

int ioLogger::CreateTCPConnect( int port )
{

}




#endif // USE_THREAD
