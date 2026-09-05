#include "stdafx.h"
#include "Network/DBServer.h"
#include "Network/ioPacketQueue.h"
#include "Database/cQueryManager.h"
#include "ReportUtil.h"
#include "ThreadPool/ioThreadPool.h"
#include "../ioINILoader/ioINILoader.h"
#include "NodeInfo/LogNodeManager.h"
#include "MainProcess.h"


BOOL tokenize(const std::string& str, const std::string& delimiters, std::vector<std::string>& tokens)
{
	tokens.clear();

	std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
	std::string::size_type pos = str.find_first_of(delimiters, lastPos);
	
	std::string token;
	while (std::string::npos != pos || std:: string::npos != lastPos)
	{
		token = str.substr(lastPos, pos - lastPos);
		tokens.push_back(token);

		lastPos = str.find_first_not_of(delimiters, pos);

		pos = str.find_first_of(delimiters, lastPos);
	}
	return (tokens.size() > 0) ? TRUE : FALSE;
}

ioMainProcess *ioMainProcess::sg_Instance = NULL;
ioMainProcess::ioMainProcess() 
{
	m_fQueryCheckTime		= 1000.0f;
	m_i64MainLoopCount		= 0;
	m_dwMainLoopCheckTime	= 0;
	m_dwLogCheckTime		= 0;
	m_dwPacketLogTime		= 0;
	m_iQueryThreadCnt		= 0;
	m_iThreadIndexSeqStyle	= 0;
	m_dwThreadIndexSeq		= 0;
	m_logServerState		= FALSE;
}

ioMainProcess::~ioMainProcess()
{
}

ioMainProcess &ioMainProcess::GetInstance()
{
	if( sg_Instance == NULL )
		sg_Instance = new ioMainProcess;
	return *sg_Instance;
}

void ioMainProcess::ReleaseInstance()
{
	SAFEDELETE( sg_Instance );
}

bool ioMainProcess::Initialize( const ioHashString &szTile )
{	
//	LOG.SetTCPMode(TRUE,5454,40001,"172.20.20.138");
	m_szTitle = szTile;
	SetConsoleTitle( m_szTitle.c_str() );
	
	return ReloadINI();
}

bool ioMainProcess::ReloadINI()
{
	char szCurrentPath[MAX_PATH] = "";
	GetCurrentDirectory(MAX_PATH, szCurrentPath);
	strcat_s(szCurrentPath, MAX_PATH, "\\ls_config_dba.ini");

	m_fQueryCheckTime = (float)GetPrivateProfileInt( "MainLoopCommon", "QueryCheckTime", 1000, szCurrentPath );	
	m_iQueryThreadCnt = GetPrivateProfileInt( "QueryThread", "ThreadCount", 16, szCurrentPath );	
	m_iThreadIndexSeqStyle = GetPrivateProfileInt( "QueryThread", "ThreadIndexSeqStyle", 0, szCurrentPath );
	return true;
}

bool ioMainProcess::LoadQuery()
{
	ioINILoader loader("ls_query.ini");
	loader.SetTitle( "QUERY" );

	int count = loader.LoadInt( "count", 0 );
	char query[4096], key[512];
	std::vector<std::string> tokens;
	std::string delimeter = " ";
	for(int i = 1 ; i <= count ; ++i)
	{
		sprintf_s( key, "%d", i);
		loader.LoadString( key, "", query, sizeof(query) );

		if(tokenize( query, delimeter, tokens ))
		{
			g_queryManager.RegisterQuery( i, tokens[0].c_str() );
			for(uint32 index = 1 ; index < tokens.size() ; index++)
			{
				std::string value = tokens[index];
				uint32 factor = 0;

				if( tokens[index] == "INT8" )
				{
					factor = FT_INT8;
				}
				else if( tokens[index] == "INT16" )
				{
					factor = FT_INT16;
				}
				else if( tokens[index] == "INT" )
				{
					factor = FT_INT32;
				}
				else if( tokens[index] == "INT32" )
				{
					factor = FT_INT32;
				}
				else if( tokens[index] == "INT64" )
				{
					factor = FT_INT64;
				}
				else if( tokens[index] == "FLOAT" )
				{
					factor = FT_FLOAT;
				}
				else if( tokens[index] == "DATETIME" )
				{
					factor = FT_DATETIME;
				}
				else if( tokens[index] == "STR" )
				{
					factor = FT_STRING;
				}
				else
				{
					return false;
				}
				g_queryManager.AddFactor( i, factor );
			}
		}
	}
	return true;
}

void ioMainProcess::ProcessMainLoopCheck( DWORD dwProcessTime )
{
	m_i64MainLoopCount++;
	if( dwProcessTime - m_dwMainLoopCheckTime < 60000 )
		return;

	m_dwMainLoopCheckTime = dwProcessTime;
	LOG.PrintTimeAndLog( 0, "MainLoop : 1Min - %I64d", m_i64MainLoopCount );
	m_i64MainLoopCount = 0;
}

void ioMainProcess::ProcessPacketLog( DWORD dwProcessTime )
{
	if( dwProcessTime > 0 && dwProcessTime - m_dwPacketLogTime < 600000 )
		return;

	m_dwPacketLogTime = dwProcessTime;

	g_ThreadPool.PrintLog();
}

void ioMainProcess::LogCurrentStates( DWORD dwProcessTime )
{
	if( dwProcessTime > 0 && dwProcessTime - m_dwLogCheckTime < 600000 )
		return;

	m_dwLogCheckTime = dwProcessTime;
	char szCurTime[1024] = "";
	static char szPrevTime[MAX_PATH] = "";
	SYSTEMTIME st;
	GetLocalTime(&st);
	wsprintf(szCurTime,"%02d%02d",st.wMonth,st.wDay);

	if(strcmp(szCurTime,szPrevTime) != 0)
	{
		strcpy_s(szPrevTime,szCurTime);

		char BugLogName[MAX_PATH] = "";
		char ReportLogName[MAX_PATH] = "";
		wsprintf(BugLogName,"%s%s%s%s", g_MainProc.GetLogFolder().c_str(), "\\BUG",szCurTime,".log");
		wsprintf(ReportLogName,"%s%s%s%s",g_MainProc.GetLogFolder().c_str(), "\\REPORT",szCurTime,".log");

		{
			LOG.PrintTimeAndLog(0, "<<< --------------------  End File -------------------- >>>\r\n");
			LOG.CloseLog();
			LOG.OpenLog(0,BugLogName,true);	
			LOG.PrintTimeAndLog(0, "<<< --------------------  Create File -------------------- >>>\r\n");
		}	

		{
			ReportLOG.PrintTimeAndLog(0, "<<< --------------------  End File -------------------- >>>\r\n");
			ReportLOG.CloseLog();
			ReportLOG.OpenLog(0,ReportLogName,true);
			ReportLOG.PrintTimeAndLog(0, "<<< --------------------  Create File -------------------- >>>\r\n");
		}		
	}
}

int ioMainProcess::GetThreadIndexSeq( DWORD dwQueryIndex )
{
	if( m_iThreadIndexSeqStyle == 0 )
		return dwQueryIndex % max( 1, GetQueryThreadCnt() );
	
	m_dwThreadIndexSeq++;
	if( m_dwThreadIndexSeq >= (DWORD)GetQueryThreadCnt() )
		m_dwThreadIndexSeq = 0;
	return m_dwThreadIndexSeq;
}

void ioMainProcess::Process(uint32& idleTime)
{
	if(g_RecvQueue.PacketParsing() == 0)
		idleTime = 1;
	else
		idleTime = 0;

	DWORD current = timeGetTime();
	LogCurrentStates( current );
	ProcessPacketLog( current );
	ProcessMainLoopCheck( current );
}

void ioMainProcess::PrintTimeAndLog(int debuglv, LPSTR fmt )
{
	LOG.PrintTimeAndLog( debuglv, fmt );
}

void ioMainProcess::DebugLog(int debuglv, LPSTR filename, int linenum, LPSTR fmt )
{
	LOG.DebugLog( debuglv, filename, linenum, fmt );
}

void ioMainProcess::Save()
{
	// 서버 종료시 처리 할것들.
	LOG.PrintTimeAndLog( 0, "- SAVE DATA" );

	// 로그 쓰레드
	LOG.PrintTimeAndLog(0, "<<< --------------------  End File -------------------- >>>\r\n");
	LOG.CloseLog();

	ReportLOG.PrintTimeAndLog(0, "<<< --------------------  End File -------------------- >>>\r\n");
	ReportLOG.CloseLog();

	g_LogNodeManager->CloseAllNodeFiles("- SAVE DATA");
	
	Sleep( 300 );
}
