#include "StdAfx.h"
#include "ioSelectShutDownAnnouncer.h"


ioSelectShutDownAnnouncer::ioSelectShutDownAnnouncer(void)
{
	m_iShutDownYear  = 0;
	m_iShutDownMonth = 0;
	m_iShutDownDay   = 0;
	m_iShutDownHour  = 0;
	m_iShutDownMinute= 0;
	m_bAnnounce       = false;
	m_dwRemainMinutes = 0;
	m_dwCheckTime     = 0;
	m_dwAnnounceRemainMinutes = 0;
	m_bProcess        = false;
}


ioSelectShutDownAnnouncer::~ioSelectShutDownAnnouncer(void)
{
	m_mAnnounceInfo.clear();
}

void ioSelectShutDownAnnouncer::LoadINI()
{
	ioINILoader_e kLoader( "config/sp2_select_shut_down.ini");
	kLoader.SetTitle_e( "Info" );
	int iMax = kLoader.LoadInt( "Max", 0);

	m_mAnnounceInfo.clear();

	char szKey[MAX_PATH]="";
	char szBuf[MAX_PATH]="";
	DWORD dwRemainMinutes = 0;
	ioHashString sMent;

	for (int i = 0; i < iMax ; i++)
	{
		sprintf_s_e( szKey, "Minutes%d", i+1 );
		dwRemainMinutes = kLoader.LoadInt( szKey, 0 );
		sprintf_s_e( szKey, "Ment%d", i+1 );
		kLoader.LoadString( szKey, "", szBuf, sizeof( szBuf ) );
		sMent = szBuf;
		m_mAnnounceInfo.insert( mAnnounceInfo::value_type( dwRemainMinutes, sMent ) );		
	}
}

void ioSelectShutDownAnnouncer::SetShutDownTime( int iYear, int iMonth, int iDay, int iHour, int iMinute )
{
	m_bProcess = true;

	m_iShutDownYear  = iYear;
	m_iShutDownMonth = iMonth;
	m_iShutDownDay   = iDay;
	m_iShutDownHour  = iHour;
	m_iShutDownMinute= iMinute;

#ifdef _DEBUG
#ifndef TEST_SUITE_ON
	LOG.PrintTimeAndLog( 0, "%s [%d-%d-%d %d:%d]", __FUNCTION__, iYear, iMonth, iDay, iHour, iMinute );
#endif
#endif

}

void ioSelectShutDownAnnouncer::Process( time_t kServerTime, DWORD dwRealGetTime )
{
	if( !m_bProcess )
		return;

	if( dwRealGetTime - m_dwCheckTime < CHECK_TIME )
	{
		m_bAnnounce = false;
		return;
	}
	m_dwCheckTime = dwRealGetTime;
	kServerTime += ( dwRealGetTime/1000 );// 현재까지의 클라이언트초를 넣는다.
	
	time_t kShutDownTime = ConvertSecondTime( m_iShutDownYear, m_iShutDownMonth, m_iShutDownDay, m_iShutDownHour, m_iShutDownMinute, 0 );
	
	int iGapSec = kShutDownTime - kServerTime;
	if( iGapSec < 0 || iGapSec >= 604800 ) // 604800초는 일주일
	{
		m_bProcess = false;
		return;
	}
	int iGapMinutes = iGapSec/60;

	ioHashString sAnnounce;
	GetAnnounce( iGapMinutes , sAnnounce );
	if( sAnnounce.IsEmpty() )
		return;
	m_bAnnounce = true;
	m_dwRemainMinutes = iGapMinutes;
}

bool ioSelectShutDownAnnouncer::IsAnnounce()
{
	return m_bAnnounce;
}

DWORD ioSelectShutDownAnnouncer::GetRemainMinutes()
{
	return m_dwRemainMinutes;
}

void ioSelectShutDownAnnouncer::GetAnnounce( IN DWORD dwRemainMinutes, OUT ioHashString &rsAnnounce )
{
	rsAnnounce = "";
	mAnnounceInfo::iterator iter = m_mAnnounceInfo.find( dwRemainMinutes );
	if( iter != m_mAnnounceInfo.end() )
	{
		rsAnnounce = iter->second;
		return;
	}
}

time_t ioSelectShutDownAnnouncer::ConvertSecondTime( int year, int month, int day, int hour, int minute, int second )
{
	struct tm base_time;
	base_time.tm_year = year - 1900;
	base_time.tm_mon  = month - 1;
	base_time.tm_mday = day;	
	base_time.tm_hour = hour;
	base_time.tm_min  = minute;		
	base_time.tm_sec  = second;
	base_time.tm_isdst= 0;

	return mktime( &base_time );
}

void ioSelectShutDownAnnouncer::WriteFile( const char *szDate )
{
	char szEncrypt[MAX_PATH]="";
	strcpy( szEncrypt, szDate );

	int iLength = strlen( szDate );
	for (int i = 0; i < iLength ; i++)
	{
		szEncrypt[i] = szDate[i] ^ 0xF1 ^ 0x35;
	}

	FILE *pFile = fopen( "info/cds", "wb" );
	if( pFile )
	{
		fwrite( szEncrypt, iLength, 1, pFile );
		fflush( pFile );
		fclose( pFile );
	}
}

void ioSelectShutDownAnnouncer::ReadFile()
{
	char szPath[MAX_PATH*2]="";
	::GetCurrentDirectory(MAX_PATH*2,szPath);
	StringCbCat( szPath, sizeof( szPath ), "/info/cds" );

	int iFileSize = 0;
	char szEncrypt[MAX_PATH]="";
	FILE *pFile = fopen( szPath, "rb" ); // channeling data shutdown
	if( !pFile )
		return;
	
	fread( szEncrypt, sizeof( szEncrypt ), 1, pFile );
	fseek( pFile, 0, SEEK_END );
	iFileSize = ftell( pFile );
	fclose( pFile );	
	

	char szDate[MAX_PATH]="";
	for (int i = 0; i < iFileSize ; i++)
	{
		szDate[i] = szEncrypt[i] ^ 0xF1 ^ 0x35;
	}

	int iType = 0;
	char szTemp[MAX_PATH]="";
	int iCnt = 0;
	int iLength = strlen( szDate );
	for (int i = 0; i < iLength+1 ; i++)
	{
		if( szDate[i] == '-' || szDate[i] == ' ' || i == iLength )	
		{
			int iValue = atoi( szTemp );
			if( iType == 0 )
			{
				m_iShutDownYear  = iValue;
			}
			else if( iType == 1 )
			{
				m_iShutDownMonth = iValue;
			}
			else if( iType == 2 )
			{
				m_iShutDownDay = iValue;
			}
			else if( iType == 3 )
			{
				m_iShutDownHour = iValue;
			}

			iType++;
			iCnt = 0;
			ZeroMemory( szTemp, sizeof( szTemp ) );
		}
		else
		{
			if( COMPARE( iCnt , 0 , MAX_PATH ) )
			{
				szTemp[iCnt] = szDate[i];
				iCnt++;
			}
		}
	}

	m_bProcess = true;
	m_iShutDownMinute = 0; 

#ifdef _DEBUG
#ifndef TEST_SUITE_ON
	LOG.PrintTimeAndLog( 0, "%s [%d-%d-%d %d:%d]", __FUNCTION__, m_iShutDownYear, m_iShutDownMonth, m_iShutDownDay, m_iShutDownHour, 0 );
#endif
#endif
}

int ioSelectShutDownAnnouncer::GetYear()
{
	return m_iShutDownYear;
}

int ioSelectShutDownAnnouncer::GetMonth()
{
	return m_iShutDownMonth;
}

int ioSelectShutDownAnnouncer::GetDay()
{
	return m_iShutDownDay;
}

int ioSelectShutDownAnnouncer::GetHour()
{
	return m_iShutDownHour;
}