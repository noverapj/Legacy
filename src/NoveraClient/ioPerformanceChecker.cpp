

#include "stdafx.h"

#include "ioPerformanceChecker.h"

// #define ENABLE_PERF_CHECK

// 여기는 1/10000 초의 세상, 1000으로 오인안하기를..

#define TICK_GAP_TIME			10000
#define CHECK_WAIT_TIME			100000
#define CHECK_ERROR_SEND_GAP	3600000

static ioPerformanceChecker *s_pInstance = NULL;

ioPerformanceChecker& ioPerformanceChecker::GetInstance()
{
	if( !s_pInstance )
	{
		s_pInstance = new ioPerformanceChecker;
	}

	return *s_pInstance;
}

void ioPerformanceChecker::ReleaseInstance()
{
	SAFEDELETE( s_pInstance );
}

ioPerformanceChecker::ioPerformanceChecker()
{
	m_dwStartTime   = 0;

	m_dwPreCheckTime = 0;
	m_iLoopCount = 0;

	m_dwWaitTime = 0;
	m_bCheckNow  = false;

	m_iEvenCount = 0;
	m_dwEvenErrorSendTime = 0;

	m_bLogging = false;
	m_bSendLog = false;
}

ioPerformanceChecker::~ioPerformanceChecker()
{
}

void ioPerformanceChecker::Initialize( bool bLogging, bool bSendLog )
{
#ifdef ENABLE_PERF_CHECK

	m_dwStartTime = 0;

	m_dwPreCheckTime = 0;
	m_iLoopCount = 0;

	m_dwWaitTime = PERFCHECKTIME() + CHECK_WAIT_TIME;
	m_bCheckNow  = false;

	m_iEvenCount = 0;
	m_dwEvenErrorSendTime = PERFCHECKTIME() + CHECK_ERROR_SEND_GAP;
	m_CheckPointMap.clear();

	m_bLogging = bLogging;
	m_bSendLog = bSendLog;

#endif
}

void ioPerformanceChecker::FrameStart()
{
#ifdef ENABLE_PERF_CHECK

	DWORD dwCurTime = PERFCHECKTIME();
	if( m_dwWaitTime < dwCurTime )
	{
		m_bCheckNow = true;
	}
	else
	{
		m_bCheckNow = false;
		return;
	}

	if( m_dwStartTime > 0 )
	{
		m_iLoopCount++;
	}
	else	// 시작
	{
		m_dwStartTime = dwCurTime;
		m_iLoopCount  = 0;
	}

	m_dwPreCheckTime = dwCurTime;
#endif
}

void ioPerformanceChecker::CheckPoint( int iPoint )
{
#ifdef ENABLE_PERF_CHECK

	if( !m_bCheckNow )	return;

	CheckPointMap::iterator iter = m_CheckPointMap.find( iPoint );
	if( iter == m_CheckPointMap.end() )
	{
		m_CheckPointMap.insert( CheckPointMap::value_type( iPoint, PointValue() ) );
	}

	DWORD dwCurTime = PERFCHECKTIME();
	if( dwCurTime > m_dwPreCheckTime )
	{
		PointValue &rkValue = m_CheckPointMap[iPoint];

		DWORD dwCheckTime = dwCurTime - m_dwPreCheckTime;
		rkValue.dwSum += dwCheckTime;

		if( m_iEvenCount > 2 && rkValue.dwEven > 500 )
		{
			if( rkValue.dwEven * 3 < dwCheckTime )
			{
				SendLog( iPoint, rkValue.dwEven, dwCheckTime );
			}
		}
	}

	m_dwPreCheckTime = dwCurTime;

#endif
}

void ioPerformanceChecker::SendLog( int iPoint, DWORD dwEvenTime, DWORD dwCheckTime )
{
#ifdef ENABLE_PERF_CHECK

	if( m_dwEvenErrorSendTime > PERFCHECKTIME() )
		return;

	static char s_ErrorLog[2048];
	wsprintf( s_ErrorLog, "%s/%s\n%s\n%s\n%d:%d:%d", g_MyInfo.GetPrivateID().c_str(),
													 g_MyInfo.GetPublicID().c_str(),
													 Setting::GetFullSystemInfo(),
													 Setting::GetSettingString(),
													 iPoint, dwEvenTime, dwCheckTime );

	if( m_bSendLog && P2PNetwork::IsNetworkPlaying() )
	{
		SP2Packet kPacket( LUPK_LOG );
		kPacket << "CheckError";
		kPacket << s_ErrorLog;
		kPacket << 365;   // 오류번호 : 365
		kPacket << false; // write db
		P2PNetwork::SendLog( kPacket );
	}

	m_dwEvenErrorSendTime = PERFCHECKTIME() + CHECK_ERROR_SEND_GAP;
	LOG.PrintLog( 0, "%s", s_ErrorLog );

#endif
}

void ioPerformanceChecker::Analysis()
{
#ifdef ENABLE_PERF_CHECK

	if( !m_bCheckNow )	return;
	if( m_dwStartTime == 0 || m_iLoopCount == 0 )
		return;

	DWORD dwCurTime = PERFCHECKTIME();
	if( dwCurTime <= m_dwStartTime )	return;

	DWORD dwGap = dwCurTime - m_dwStartTime;
	if( TICK_GAP_TIME > dwGap )
		return;

	if( m_bLogging )
	{
		LOG.PrintTimeAndLog( 0, "---------------------" );

		float fFrame = (float)m_iLoopCount * 10000.0f / (float)dwGap;
		LOG.PrintLog( 0, "Frame : %.1f", fFrame );

		int iPointCount = m_CheckPointMap.size();
		CheckPointMap::iterator iter = m_CheckPointMap.begin();
		for( int i=0 ; i<iPointCount ; i++, ++iter )
		{
			PointValue &rkValue = iter->second;
			float fEvenTime = (float)rkValue.dwSum / (float)m_iLoopCount;

			LOG.PrintLog( 0, "%2d : Waste %d, Even : %.2f", iter->first, rkValue.dwSum, fEvenTime );
		}
	}

	m_dwStartTime = 0;

	m_dwPreCheckTime = 0;
	m_iLoopCount  = 0;

	// 평균값 누적
	CheckPointMap::iterator iter = m_CheckPointMap.begin();
	for( ; iter!=m_CheckPointMap.end(); ++iter )
	{
		PointValue &rkValue = iter->second;

		rkValue.dwEven = ( rkValue.dwEven * m_iEvenCount + rkValue.dwSum ) / ( m_iEvenCount + 1 );
		rkValue.dwSum  = 0;	// 초기화
	}

	m_iEvenCount++;

#endif
}



