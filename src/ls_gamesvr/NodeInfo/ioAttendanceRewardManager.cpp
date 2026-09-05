#include "stdafx.h"

#include "../MainProcess.h"
#include "../DataBase/DBClient.h"
#include "../DataBase/LogDBClient.h"

#include "../MainServerNode/MainServerNode.h"
#include "../NodeInfo/ioEtcItemManager.h"
#include "../EtcHelpFunc.h"

#include "ioAttendanceRewardManager.h"

template<> ioAttendanceRewardManager* Singleton< ioAttendanceRewardManager >::ms_Singleton = 0;

ioAttendanceRewardManager::ioAttendanceRewardManager()
{
}

ioAttendanceRewardManager::~ioAttendanceRewardManager()
{
}

ioAttendanceRewardManager& ioAttendanceRewardManager::GetSingleton()
{
	return Singleton< ioAttendanceRewardManager >::GetSingleton();
}

void ioAttendanceRewardManager::Destroy()
{
	m_TodayAttendanceMap.clear();
	m_AccrueAttendanceMap.clear();
}

void ioAttendanceRewardManager::LoadINI()
{
	Destroy();
	char szKey[MAX_PATH] = "";
	char szBuf[MAX_PATH] = "";

	ioINILoader kLoader( "config/sp2_attendance_reward.ini" );
	kLoader.SetTitle( "common" );

	int iMaxLoad = kLoader.LoadInt( "max_load", 0 );
		
	for( int i = 0; i < iMaxLoad; i++ )
	{
		kLoader.SetTitle( "common" );
		sprintf_s( szKey, "load_date_%d", i + 1 );
		kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		DWORD dwTtitle = kLoader.LoadInt( szKey, 0 );

#ifdef _DEBUG		
		sprintf_s( szKey, "load title -  [%d]", dwTtitle );
		m_szLogVec.push_back( szKey );
#endif

		LoadTodayAttance( kLoader, dwTtitle );
		LoadAccrueAttance( kLoader, dwTtitle );
	}

#ifdef _DEBUG
	LOG.PrintTimeAndLog(0, "[%s] - Load Reward --------------------------", __FUNCTION__ );
	for( ioHashStringVec::iterator iter = m_szLogVec.begin(); iter != m_szLogVec.end(); ++iter )
	{
		LOG.PrintTimeAndLog(0, "%s", iter->c_str() );
	}
	LOG.PrintTimeAndLog(0, "[%s] - End Reward --------------------------", __FUNCTION__ );
#endif
}

void ioAttendanceRewardManager::LoadTodayAttance( ioINILoader& rkLoader, DWORD dwTtitle )
{	
	char szKey[MAX_PATH] = "";
	char szBuf[MAX_PATH] = "";

	sprintf_s( szKey, "%d", dwTtitle );
	rkLoader.SetTitle( szKey );
	
	for( int i = 0; i < MAX_DAY; i++ )
	{
		TodayAttendanceReward Reward;

		sprintf_s( szKey, "today_reward%d_type", i + 1 );
		Reward.m_iPresentType = rkLoader.LoadInt( szKey, 0 );

		sprintf_s( szKey, "today_reward%d_value1", i + 1 );
		Reward.m_iValue1 = rkLoader.LoadInt( szKey, 0 );

		sprintf_s( szKey, "today_reward%d_value2", i + 1 );
		Reward.m_iValue2 = rkLoader.LoadInt( szKey, 0 );

		sprintf_s( szKey, "today_reward%d_period", i + 1 );
		Reward.m_iPeriod = rkLoader.LoadInt( szKey, 7 );

		sprintf_s( szKey, "today_reward%d_ment", i + 1 );
		Reward.m_iMent = rkLoader.LoadInt( szKey, 0 );

		DWORD dwKey =  dwTtitle * 100 + ( i + 1 );
		if( Reward.m_iPresentType == 0 && Reward.m_iValue1 == 0 && Reward.m_iValue2 == 0 )
			continue;

		m_TodayAttendanceMap.insert( TodayAttendanceMap::value_type( dwKey, Reward ) );

#ifdef _DEBUG
		sprintf_s( szKey, "Today %d: %d, %d, %d", dwKey, Reward.m_iPresentType, Reward.m_iValue1, Reward.m_iValue2 );
		m_szLogVec.push_back( szKey );
#endif
	}
}

void ioAttendanceRewardManager::LoadAccrueAttance( ioINILoader& rkLoader, DWORD dwTtitle )
{
	char szKey[MAX_PATH] = "";
	char szBuf[MAX_PATH] = "";

	sprintf_s( szKey, "%d", dwTtitle );
	rkLoader.SetTitle( szKey );
	
	for( int i = 0; i < MAX_WEEKS; i++ )
	{
		AccrueAttendanceReward Reward;

		sprintf_s( szKey, "accrue_reward%d_term", i + 1 );
		int iTerm = rkLoader.LoadInt( szKey, 0 );
		if( iTerm == 0 )
			continue;

		Reward.m_iAccureTerm = iTerm;

		sprintf_s( szKey, "accrue_reward%d_type", i + 1 );
		Reward.m_iPresentType = rkLoader.LoadInt( szKey, 0 );

		sprintf_s( szKey, "accrue_reward%d_value1", i + 1 );
		Reward.m_iValue1 = rkLoader.LoadInt( szKey, 0 );

		sprintf_s( szKey, "accrue_reward%d_value2", i + 1 );
		Reward.m_iValue2 = rkLoader.LoadInt( szKey, 0 );

		sprintf_s( szKey, "accrue_reward%d_period", i + 1 );
		Reward.m_iPeriod = rkLoader.LoadInt( szKey, 0 );

		sprintf_s( szKey, "accrue_reward%d_ment", i + 1 );
		Reward.m_iMent = rkLoader.LoadInt( szKey, 0 );
		
		m_AccrueAttendanceMap.insert( AccrueAttendanceMap::value_type( dwTtitle * 100 + iTerm, Reward ) );

#ifdef _DEBUG
		sprintf_s( szKey, "Accrue %d: %d, %d, %d", iTerm, Reward.m_iPresentType, Reward.m_iValue1, Reward.m_iValue2 );
		m_szLogVec.push_back( szKey );
#endif
	}
}

void ioAttendanceRewardManager::SendTodayAttanceReward( User* pUser, DWORD dwTodayDate, OUT int& iPresentType, OUT int& iValue1, OUT int& iValue2 )
{
	if( !pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s User NULL!!", __FUNCTION__ ); 
		return;
	}

	TodayAttendanceMap::iterator iter = m_TodayAttendanceMap.find( dwTodayDate );
	if( iter == m_TodayAttendanceMap.end() )
		return;

	const TodayAttendanceReward& rkReward = iter->second;

	iPresentType = rkReward.m_iPresentType;
	iValue1 = rkReward.m_iValue1;
	iValue2 = rkReward.m_iValue2;

	CTimeSpan cPresentGapTime( rkReward.m_iPeriod, 0, 0, 0 );
	CTime kPresentTime = CTime::GetCurrentTime() + cPresentGapTime;
	pUser->AddPresentMemory( g_MainServer.GetSendID(), rkReward.m_iPresentType, rkReward.m_iValue1, rkReward.m_iValue2, 0, 0, rkReward.m_iMent, kPresentTime, ioUserPresent::PRESENT_STATE_NORMAL );
	pUser->SendPresentMemory();

	char szNote[MAX_PATH]="";
	StringCbPrintf( szNote, sizeof( szNote ) , "attend today reward date : %d", dwTodayDate );
	g_LogDBClient.OnInsertPresent( 0, g_MainServer.GetSendID(), g_App.GetPublicIP().c_str(), pUser->GetUserIndex(), rkReward.m_iPresentType, rkReward.m_iValue1, rkReward.m_iValue2, 0, 0, LogDBClient::PST_RECIEVE, szNote );
}

void ioAttendanceRewardManager::SendAccurePeriodReward( User* pUser, int iYear, int iMonth, int iAccurePeriod, OUT int& iPresentType, OUT int& iValue1, OUT int& iValue2 )
{
	if( !pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s User NULL!!", __FUNCTION__ ); 
		return;
	}

	AccrueAttendanceMap::iterator iter = m_AccrueAttendanceMap.find( iYear * 10000 + iMonth* 100 + iAccurePeriod );
	if( iter == m_AccrueAttendanceMap.end() )
		return;

	const AccrueAttendanceReward& rkReward = iter->second;

	iPresentType = rkReward.m_iPresentType;
	iValue1 = rkReward.m_iValue1;
	iValue2 = rkReward.m_iValue2;

	CTimeSpan cPresentGapTime( rkReward.m_iPeriod, 0, 0, 0 );
	CTime kPresentTime = CTime::GetCurrentTime() + cPresentGapTime;
	pUser->AddPresentMemory( g_MainServer.GetSendID(), rkReward.m_iPresentType, rkReward.m_iValue1, rkReward.m_iValue2, 0, 0, rkReward.m_iMent, kPresentTime, ioUserPresent::PRESENT_STATE_NORMAL );
	pUser->SendPresentMemory();

	char szNote[MAX_PATH]="";
	StringCbPrintf( szNote, sizeof( szNote ) , "attend accrue reward period : %d", iAccurePeriod );
	g_LogDBClient.OnInsertPresent( 0, g_MainServer.GetSendID(), g_App.GetPublicIP().c_str(), pUser->GetUserIndex(), rkReward.m_iPresentType, rkReward.m_iValue1, rkReward.m_iValue2, 0, 0, LogDBClient::PST_RECIEVE, szNote );
}