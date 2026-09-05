#include "stdafx.h"
#include ".\ioShutDownManager.h"
#include <strsafe.h>
#include "UserNodeManager.h"
#include "..\EtcHelpFunc.h"

template<> ioShutDownManager *Singleton< ioShutDownManager >::ms_Singleton = 0;

ioShutDownManager::ioShutDownManager(void)
{
	m_dwCurrentTime       = 0;
	m_dwStartTotalMinutes = 0;
	m_dwEndTotalMinutes   = 0;
	m_dwBaseYears         = 0;
	m_bActive             = false;
}

ioShutDownManager::~ioShutDownManager(void)
{

}

void ioShutDownManager::LoadINI()
{
	ioINILoader kLoader( "config/sp2_shut_down.ini" );
	kLoader.SetTitle( "Info" );
	int iStartHour    = kLoader.LoadInt( "StartHour", 0 ); // 0~23
	int iStartMinutes = kLoader.LoadInt( "StartMinutes", 0 ); 
	int iEndHour      = kLoader.LoadInt( "EndHour", 0 ); // 0~23
	int iEndMinutes   = kLoader.LoadInt( "EndMinutes", 0 ); 
	m_dwBaseYears     = kLoader.LoadInt( "BaseYears", 0 ); // 몇살미만이면 shutdown 인지.
	m_bTemporaryUserShutDown = kLoader.LoadBool( "TemporaryUserShutDwon", true );

	m_dwStartTotalMinutes = ( iStartHour  * TOTAL_MIN_PER_TIME ) + iStartMinutes;
	m_dwEndTotalMinutes = ( iEndHour  * TOTAL_MIN_PER_TIME ) + iEndMinutes;
}

void ioShutDownManager::Process()
{
	if( TIMEGETTIME() - m_dwCurrentTime < 60000 )  // 1분
		return;

	m_dwCurrentTime = TIMEGETTIME();

	FUNCTION_TIME_CHECKER( 100000.0f, 0 );          // 0.1 초 이상 걸리면로그 남김

	SYSTEMTIME st;
	GetLocalTime(&st);

	DWORD dwCurrentTotalMinute =  ( st.wHour * TOTAL_MIN_PER_TIME ) + st.wMinute;

	bool bOk = false;
	if( m_dwStartTotalMinutes > m_dwEndTotalMinutes )
	{
		if( COMPARE( dwCurrentTotalMinute, m_dwStartTotalMinutes, TOTAL_MIN_PER_DAY ) || 
			COMPARE( dwCurrentTotalMinute, 0, m_dwEndTotalMinutes + 1 ) )
		{
			bOk = true;
		}
	}
	else
	{
		if( COMPARE( dwCurrentTotalMinute, m_dwStartTotalMinutes, m_dwEndTotalMinutes + 1) )
		{
			bOk = true;
		}
	}

	if( bOk )
	{
		if( !m_bActive )
			g_UserNodeManager.SetNodeShutDownCheckTime( 0 );

		m_bActive = true;
	}
	else
		m_bActive = false;
}

ioShutDownManager & ioShutDownManager::GetSingleton()
{
	return Singleton< ioShutDownManager >::GetSingleton();
}

bool ioShutDownManager::CheckShutDownUser( const char *szBirthDate, short iYearType, bool bFormalityUser )
{
	if( !bFormalityUser )
	{
		if( m_bTemporaryUserShutDown )
			return true;
		else
			return false;
	}

	if( !szBirthDate )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s szBirthDate == NULL." , __FUNCTION__ );
		return true; // error면 셧다운 유저
	}

	int iYear = 0;
	if( iYearType == 1 || iYearType == 2  || iYearType == 5 || iYearType == 6 )
		iYear = 1900;
	else if( iYearType == 3 || iYearType == 4 || iYearType == 7 || iYearType == 8 )
		iYear = 2000;
	else
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[info][shutdown]Year type : [%d]" ,iYearType );
		return true; // error면 셧다운 유저
	}

	enum { MAX_TEMP = 3, MAX_COPY = 2, }; 
	char szTemp[MAX_TEMP]="";
	int iCopyCnt = 0;
	ZeroMemory( szTemp, sizeof( szTemp ) );
	StringCbCopyN( szTemp, sizeof( szTemp ), szBirthDate, MAX_COPY );
	iCopyCnt += MAX_COPY;

	int iAddYear = atoi( szTemp );
	iYear += iAddYear;
	
	ZeroMemory( szTemp, sizeof( szTemp ) );
	StringCbCopyN( szTemp, sizeof( szTemp ), &szBirthDate[iCopyCnt], MAX_COPY );
	iCopyCnt += MAX_COPY;
	int iMonth   = atoi( szTemp );

	ZeroMemory( szTemp, sizeof( szTemp ) );
	StringCbCopyN( szTemp, sizeof( szTemp ), &szBirthDate[iCopyCnt], MAX_COPY );
	int iDay   = atoi( szTemp );

	DWORD dwBirthDate   = ( iYear*10000 ) + ( iMonth*100 ) + iDay;

	CTimeSpan kGapTime( 1, 0, 0, 0 );
	CTime kCurrentDate = CTime::GetCurrentTime() + kGapTime;
	DWORD dwCurrentDate = ( (kCurrentDate.GetYear()-m_dwBaseYears) * 10000) + (kCurrentDate.GetMonth() * 100) + kCurrentDate.GetDay();

	if( dwBirthDate >  dwCurrentDate )
		return true;

	return false;
}