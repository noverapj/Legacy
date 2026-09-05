// LogTable.cpp: implementation of the CLogTable class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LogTable.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLogTable *CLogTable::sg_Instance = NULL;

CLogTable::CLogTable()
{
	ClearAllLogName();	
}

CLogTable::~CLogTable()
{
	ClearAllLogName();	
}

CLogTable &CLogTable::GetInstance()
{
	if( !sg_Instance )
		sg_Instance = new CLogTable;

	return *sg_Instance;
}

void CLogTable::ReleaseInstance()
{
	SAFEDELETE(sg_Instance);
}


void CLogTable::WriteLogContents( const ioHashString &rkLogName, const ioHashString &rkLogContents, int iErrorNumber )
{
	if(rkLogName.IsEmpty() || rkLogContents.IsEmpty()) return;

	LogTableMap::iterator iter = m_LogTableMap.find( rkLogName );

	if(iter != m_LogTableMap.end() )
	{
		CLog* pLog = iter->second;
		if(pLog)
			pLog->PrintTimeAndLog(0, "[%d]%s", iErrorNumber, rkLogContents.c_str());
	}
	else
	{	
		AddLogName(rkLogName, rkLogContents, iErrorNumber );
	}
}

void CLogTable::AddLogName( const ioHashString &rkLogName, const ioHashString &rkLogContents, int iErrorNumber )
{
	int iCurSize = (int) m_LogTableMap.size();
	if(iCurSize > MAX_LOG_TABLE)
	{
		LOG.PrintTimeAndLog(0, "CLogTable::AddLog - Error");
		return;
	}
	
	CLog *pLog = new CLog;
	m_LogTableMap.insert( LogTableMap::value_type( rkLogName , pLog));
	LogTableMap::iterator iter = m_LogTableMap.find( rkLogName );
	if(iter != m_LogTableMap.end() )
	{
		char szLogPath[MAX_PATH]="";
		wsprintf(szLogPath, "MLOG\\%s\\%s", rkLogName.c_str(), rkLogName.c_str() );
		SYSTEMTIME st;
		GetLocalTime(&st);
		char szCurTime[MAX_PATH]="";
		wsprintf(szCurTime, "%04d%02d%02d.log", st.wYear, st.wMonth, st.wDay);
		strcat(szLogPath, szCurTime);

		CLog* pLog = iter->second;
		if(pLog)
		{
			pLog->OpenLog(0, szLogPath, true);
			pLog->PrintTimeAndLog(0, "[%d]%s", iErrorNumber, rkLogContents.c_str());
		}
	}
}

void CLogTable::ClearAllLogName()
{
	LogTableMap::iterator iter, iEnd;
	iEnd = m_LogTableMap.end();
	for(iter = m_LogTableMap.begin(); iter != iEnd ; ++iter)
	{
		CLog* pLog = iter->second;
		if(pLog)
		{
			pLog->CloseAndRelease();
			delete pLog;
		}
	}
	m_LogTableMap.clear();
}