#include "stdafx.h"
#include "MissionManager.h"
#include "MissionData.h"
#include "User.h"
#include "../EtcHelpFunc.h"
#include "ioMission.h"
#include "Room.h"
#include "ioQuestManager.h"
#include "../DataBase/LogDBClient.h"

template<> MissionManager *Singleton< MissionManager >::ms_Singleton = 0;

MissionManager::MissionManager()
{
	Init();
}

MissionManager::~MissionManager()
{
	Destroy();
}

MissionManager& MissionManager::GetSingleton()
{
	return Singleton< MissionManager >::GetSingleton();
}

void MissionManager::Init()
{
	m_dwTimerCheckTime	= 0;

	m_vActiveDate.clear();
	m_vNextActiveDate.clear();
	m_vActiveIndex.clear();
	m_vMaxIndex.clear();

	for( int i = 0; i <= MT_MONTHLY; i++ )
	{
		m_vActiveDate.push_back(0);
		m_vNextActiveDate.push_back(0);
		m_vActiveIndex.push_back(0);
		m_vMaxIndex.push_back(0);
	}
}

void MissionManager::DeleteDailyMissionTable()
{
	mCurMissionTable::iterator it = m_mDailyMissionTable.begin();

	for(	; it != m_mDailyMissionTable.end(); it++ )
	{
		Mission* pMission = it->second;
		if( pMission )
			delete pMission;
	}

	m_mDailyMissionTable.clear();
}

void MissionManager::DeleteWeeklyMissionTable()
{
	mCurMissionTable::iterator it = m_mWeeklyMissionTable.begin();

	for(	; it != m_mWeeklyMissionTable.end(); it++ )
	{
		Mission* pMission = it->second;
		if( pMission )
			delete pMission;
	}

	m_mWeeklyMissionTable.clear();
}

void MissionManager::DeleteMonthlyMissionTable()
{
	mCurMissionTable::iterator it = m_mMonthlyMissionTable.begin();

	for(	; it != m_mMonthlyMissionTable.end(); it++ )
	{
		Mission* pMission = it->second;
		if( pMission )
			delete pMission;
	}

	m_mMonthlyMissionTable.clear();
}

void MissionManager::Destroy()
{
	DeleteDailyMissionTable();
	DeleteWeeklyMissionTable();
	DeleteMonthlyMissionTable();
}

void MissionManager::ProcessMission()
{
	if( TIMEGETTIME() - m_dwTimerCheckTime < (60 * 1000) ) 
		return;

	m_dwTimerCheckTime = TIMEGETTIME();

	FUNCTION_TIME_CHECKER( 100000.0f, 0 );          // 0.1 초 이상 걸리면로그 남김

	CTime kCurTime  = CTime::GetCurrentTime();
	DWORD dwCurTime = (DWORD)kCurTime.GetTime();
	DWORD dwNextDate = 0;
	static IntVec vResetTypeVec;
	vResetTypeVec.clear();

	for( int i = 0; i < MT_MONTHLY + 1; i++ )
	{
		dwNextDate = GetNextMissionDate(i);
		if( dwNextDate != 0 )
		{
			if( dwCurTime >= dwNextDate )
			{
				//초기화
				ChangeActiveMissionDate(i);
				ChangeActiveMissionData(i);
				vResetTypeVec.push_back(i);
			}
		}
	}

	if( !vResetTypeVec.empty() )
		g_UserNodeManager.UserNode_MissionTargetTypeReset(vResetTypeVec);
}

void MissionManager::LoadMissionTypeIniFile(ioINILoader& kLoader, int iType)
{
	switch( iType )
	{
	case MT_DAILY:
		kLoader.LoadFile( "config/sp2_daily_mission.ini" );
		break;
	case MT_WEEKLY:
		kLoader.LoadFile( "config/sp2_weekly_mission.ini" );
		break;
	case MT_MONTHLY:
		kLoader.LoadFile( "config/sp2_monthly_mission.ini" );
		break;
	}
}

void MissionManager::ReloadMissionTypeIniFile(ioINILoader& kLoader, int iType)
{
	switch( iType )
	{
	case MT_DAILY:
		kLoader.ReloadFile( "config/sp2_daily_mission.ini" );
		break;
	case MT_WEEKLY:
		kLoader.ReloadFile( "config/sp2_weekly_mission.ini" );
		break;
	case MT_MONTHLY:
		kLoader.ReloadFile( "config/sp2_monthly_mission.ini" );
		break;
	}
}

DWORD MissionManager::GetStartDateFromINI(ioINILoader& kLoader, const int iIndex)
{
	char szKey[MAX_PATH]="";

	kLoader.SetTitle("common");
	int iResetHour = kLoader.LoadInt("reset_hour", 0);

	StringCbPrintf( szKey, sizeof( szKey ), "date%d", iIndex );
	kLoader.SetTitle( szKey );
	int iStartDate		= kLoader.LoadInt( "start_date", 0 );
	if( 0 == iStartDate )
		return 0;

	int iYear			= (iStartDate / 10000) + 2000;
	int iMonth			= (iStartDate % 10000 ) / 100;
	int iDay			= (iStartDate % 10000 ) % 100;

	CTime cDate = CTime( Help::GetSafeValueForCTimeConstructor( iYear, iMonth, iDay, iResetHour, 0, 0 ) );
	return cDate.GetTime();
}

BOOL MissionManager::IsPrevMissionData(int iType, int iIndex)
{
	//
	ioINILoader kLoader;
	LoadMissionTypeIniFile(kLoader, iType);
	DWORD dwStartDate = GetStartDateFromINI(kLoader, iIndex);

	CTime cCurTime = CTime::GetCurrentTime();
	CTime cStartTime(dwStartDate);

	switch(iType)
	{
	case MT_DAILY:
		{
			CTimeSpan cSpan = cCurTime - cStartTime;
			if( cSpan.GetDays() > 0 )
				return TRUE;

			return FALSE;
		}
	case MT_WEEKLY:
		{
			char szKey[MAX_PATH] = "";
			kLoader.SetTitle("common");
			int iResetHour = kLoader.LoadInt("reset_hour", 0);
			BOOL bTest = kLoader.LoadBool("test", 0);

			StringCbPrintf( szKey, sizeof( szKey ), "date%d", iIndex );
			kLoader.SetTitle( szKey );
			int iEndDate		= kLoader.LoadInt( "end_date", 0 );

			int iYear			= (iEndDate / 10000) + 2000;
			int iMonth			= (iEndDate % 10000 ) / 100;
			int iDay			= (iEndDate % 10000 ) % 100;
			int iMinute			= 0;

			if( bTest )
			{
				iResetHour = kLoader.LoadInt("end_hour", iResetHour);
				iMinute	   = kLoader.LoadInt("end_minute", 0);
			}

			CTime cEndDate = CTime( Help::GetSafeValueForCTimeConstructor( iYear, iMonth, iDay, iResetHour, iMinute, 0 ) );
			
			if( cEndDate < cCurTime)
				return TRUE;

			return FALSE;
		}
	case MT_MONTHLY:
		{
			if(  cCurTime.GetYear() != cStartTime.GetYear() )
				return TRUE;

			if( cCurTime.GetYear() == cStartTime.GetYear() && cCurTime.GetMonth() > cStartTime.GetMonth() )
				return TRUE;

			return FALSE;

		}
	}
	return TRUE;
}

BOOL MissionManager::LoadINI(BOOL bReload)
{
	Init();

	char szKey[MAX_PATH]="";

	//daily Mission Read
	int iType	= 0;
	for(	; iType <= MT_MONTHLY; iType++ )
	{
		BOOL bFlag = FALSE;

		ioINILoader kLoader;
		if( bReload )
		{
			ReloadMissionTypeIniFile(kLoader, iType );
			switch( iType )
			{
			case MT_DAILY:
				DeleteDailyMissionTable();
				break;
			case MT_WEEKLY:
				DeleteWeeklyMissionTable();
				break;
			case MT_MONTHLY:
				DeleteMonthlyMissionTable();
				break;
			}
		}
		else
			LoadMissionTypeIniFile(kLoader, iType);

		kLoader.SetTitle( "common" );
		int iResetHour		= kLoader.LoadInt( "reset_hour", 0 );	
		int iMaxMissionCount = kLoader.LoadInt( "max_count", 0 );
		if( iMaxMissionCount == 0 )
			iMaxMissionCount = 1000;

		BOOL bClose			= kLoader.LoadBool( "close", 0 );

		if( bClose )
		{
			DeleteDailyMissionTable();
			DeleteWeeklyMissionTable();
			DeleteMonthlyMissionTable();

			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[warning][missionmgr] mission is closed");
			return TRUE;
		}

		int iDate = 1;
		for(	; iDate <= iMaxMissionCount; iDate++ )
		{
			DWORD dwStartDate = GetStartDateFromINI(kLoader, iDate);
			if( 0 == dwStartDate )
				break;

			CTime cCurDate = CTime::GetCurrentTime();
			DWORD dwCurDate = cCurDate.GetTime();

			if( dwStartDate < dwCurDate )
				continue;

			if( dwCurDate < dwStartDate && FALSE == bFlag )
			{
				m_vNextActiveDate[iType] = dwStartDate;
				m_vActiveIndex[iType] = iDate - 1;
				bFlag = TRUE;
			}
		}

		m_vMaxIndex[iType] = iDate - 1;
		if( 0 == m_vActiveIndex[iType] )
		{
			//첫 인덱스, 마지막 인덱스 시간을 비교.
			if( GetMaxIndex(iType) != 1 && !IsPrevMissionData(iType, 1) )
				continue;
		
			if( IsPrevMissionData(iType, m_vMaxIndex[iType]) )
				continue;

			int iIndex = m_vMaxIndex[iType];
			m_vActiveIndex[iType] = iIndex;

			ioINILoader kLoader;
			LoadMissionTypeIniFile(kLoader, iType);
			DWORD dwStartDate = GetStartDateFromINI(kLoader, iIndex);
			m_vActiveDate[iType] = dwStartDate;
		}

		//현재 동작중인 미션으로 셋팅.
		DWORD dwStartDate = GetStartDateFromINI(kLoader, m_vActiveIndex[iType]);
		CTime cCurdate = CTime::GetCurrentTime();
		DWORD dwCurDate = cCurdate.GetTime();
		DWORD dwEndDate = 0;

		m_vActiveDate[iType] = dwStartDate;

		for( int i = 0; i < 1000; i++ )
		{
			char szValues[MAX_PATH] = "";

			StringCbPrintf( szKey, sizeof( szKey ), "mission%d_code", i+1 );
			int iMissionCode	= kLoader.LoadInt(szKey, 0);
			if( 0 == iMissionCode )
				break;

			StringCbPrintf( szKey, sizeof( szKey ), "mission%d_type", i+1 );
			int iMissionClass	= kLoader.LoadInt(szKey, 0);

			StringCbPrintf( szKey, sizeof( szKey ), "mission%d_values", i+1 );
			kLoader.LoadString(szKey, "", szValues, MAX_PATH );

			//value 파싱
			IntVec vValues;
			Help::TokenizeToINT(szValues, ".", vValues);

			StringCbPrintf( szKey, sizeof( szKey ), "mission%d_present", i+1 );
			int iPresent		= kLoader.LoadInt(szKey, 0);

			Mission* pMission = new Mission;
			if( !pMission )
				continue;

			if( !pMission->Create(iMissionCode, (MissionClasses)iMissionClass, (MissionTypes)iType, vValues, iPresent) )
				return FALSE;

			switch( iType )
			{
			case MT_DAILY:
				m_mDailyMissionTable.insert(make_pair(pMission->GetMissionCode(), pMission));
				break;
			case MT_WEEKLY:
				m_mWeeklyMissionTable.insert(make_pair(pMission->GetMissionCode(), pMission));
				break;
			case MT_MONTHLY:
				m_mMonthlyMissionTable.insert(make_pair(pMission->GetMissionCode(), pMission));
				break;
			}
		}
	}
	
	//Active mission code
	mCurMissionTable::iterator it;
	for( it = m_mDailyMissionTable.begin() ; it != m_mDailyMissionTable.end(); it++ )
	{
		Mission *pMission = it->second;
		if( pMission )
			LOG.PrintTimeAndLog(LOG_DEBUG_LEVEL, "[info][missionmgr] daily active mission code : %d", pMission->GetMissionCode());
	}

	for( it = m_mWeeklyMissionTable.begin(); it != m_mWeeklyMissionTable.end(); it++ )
	{
		Mission *pMission = it->second;
		if( pMission )
			LOG.PrintTimeAndLog(LOG_DEBUG_LEVEL, "[info][missionmgr] weekly active mission code : %d", pMission->GetMissionCode());
	}

	for( it = m_mMonthlyMissionTable.begin(); it != m_mMonthlyMissionTable.end(); it++ )
	{
		Mission *pMission = it->second;
		if( pMission )
			LOG.PrintTimeAndLog(LOG_DEBUG_LEVEL, "[info][missionmgr] monthly active mission code : %d", pMission->GetMissionCode());
	}

	return TRUE;
}

DWORD MissionManager::GetNextMissionDate(const int iMissionType)
{
	if( !COMPARE(iMissionType, MT_DAILY, MT_MONTHLY+1) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[warning][missionmgr] non exist missiontype : [type:%d]", iMissionType);
		return 0;
	}

	return m_vNextActiveDate[iMissionType];
}

DWORD MissionManager::GetActiveMissionDate(const int iMissionType)
{
	if( !COMPARE(iMissionType, MT_DAILY, MT_MONTHLY+1) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[warning][missionmgr] non exist missiontype : [type:%d]", iMissionType);
		return -1;
	}

	return m_vActiveDate[iMissionType];
}

int MissionManager::GetActiveIndex(const int iMissionType)
{
	if( !COMPARE(iMissionType, MT_DAILY, MT_MONTHLY+1) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[warning][missionmgr] non exist missiontype : [type:%d]", iMissionType);
		return -1;
	}

	return m_vActiveIndex[iMissionType];
}

void MissionManager::SetActiveMissionDate(const int iMissionType, DWORD dwDate)
{
	if( !COMPARE(iMissionType, MT_DAILY, MT_MONTHLY+1) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[warning][missionmgr] non exist missiontype : [type:%d]", iMissionType);
		return;
	}

	m_vActiveDate[iMissionType] = dwDate;
}

void MissionManager::SetNextMissionDate(const int iMissionType, DWORD dwDate)
{
	if( !COMPARE(iMissionType, MT_DAILY, MT_MONTHLY+1) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[warning][missionmgr] non exist missiontype : [type:%d]", iMissionType);
		return;
	}

	m_vNextActiveDate[iMissionType] = dwDate;
}

void MissionManager::SetActiveIndex(const int iMissionType, int iIndex)
{
	if( !COMPARE(iMissionType, MT_DAILY, MT_MONTHLY+1) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[warning][missionmgr] non exist missiontype : [type:%d]", iMissionType);
		return;
	}

	m_vActiveIndex[iMissionType] = iIndex;
}

int MissionManager::GetMaxIndex(const int iMissionType)
{
	if( !COMPARE(iMissionType, MT_DAILY, MT_MONTHLY+1) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[warning][missionmgr] non exist missiontype : [type:%d]", iMissionType);
		return 0;
	}

	return m_vMaxIndex[iMissionType];
}

void MissionManager::ChangeActiveMissionDate(const int iType)
{
	if( !COMPARE(iType, MT_DAILY, MT_MONTHLY+1) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[warning][missionmgr] non exist missiontype : [type:%d]", iType);
		return;
	}

	DWORD dwNextDate = GetNextMissionDate(iType);
	if( 0 == dwNextDate )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[warning][missionmgr] next date is not exist : [type:%d activeIndex:%d]", iType, m_vActiveIndex[iType]);

		//ini에서 로드 실시.
		int iActiveIndex = GetActiveIndex(iType);
		if( iActiveIndex < 0 )
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[warining][missionmgr] current index is not exist : [type:%d ]", iType);
			return;
		}

		ioINILoader kLoader;
		LoadMissionTypeIniFile(kLoader, iType);
		dwNextDate = GetStartDateFromINI(kLoader, iActiveIndex + 1);
		if( 0 == dwNextDate )
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[error][missionmgr] next date is not exist : [type:%d activeIndex:%d]", iType, m_vActiveIndex[iType]);
			return;
		}
	}
	
	int iNextIndex = GetActiveIndex(iType) + 1;
	if( iNextIndex > GetMaxIndex(iType) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[warning][missionmgr] next index is overflow : [type:%d nextIndex:%d]", iType, iNextIndex);
		return;
	}

	ioINILoader kLoader;
	LoadMissionTypeIniFile(kLoader, iType);
	SetActiveIndex(iType, iNextIndex);
	SetActiveMissionDate(iType, dwNextDate);

	dwNextDate = GetStartDateFromINI(kLoader, iNextIndex + 1);

	if( 0 == dwNextDate )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[warning][missionmgr] next date is not exist : [type:%d activeIndex:%d]", iType, m_vActiveIndex[iType]);
		SetNextMissionDate(iType, 0);
		return;
	}

	SetNextMissionDate(iType, dwNextDate);
}

BOOL MissionManager::IsAlive(const DWORD dwCode)
{
	if( GetActiveMission(dwCode) )
		return TRUE;

	return FALSE;
}

void MissionManager::DoTrigger(const MissionClasses eMissionClass, User* pUser, DWORDVec& vValues, BOOL bMacro)
{
	if( !pUser )
		return;

	ioMission* pUserMission = pUser->GetUserMission();
	if( !pUserMission )
		return;

	pUserMission->TriggerMission(eMissionClass, vValues, bMacro);

	//일일,주간 미션과 연관된 미션들 체크.
	for( int i = 0; i <= MT_MONTHLY; i++ )
	{	
		int iCompletedCnt = pUserMission->GetCompletedTypeCount(i);
		if( iCompletedCnt <= 0 )
			continue;

		static DWORDVec vCompleteVec;
		vCompleteVec.clear();
		vCompleteVec.push_back(iCompletedCnt);

		if( MT_DAILY == i )
		{
			pUserMission->TriggerMission(MISSION_CLASS_DAILY_COMPLETE, vCompleteVec);
			pUserMission->TriggerMission(MISSION_CLASS_DAILY_ALL_CLEAR, vCompleteVec);
		}
		else if( MT_WEEKLY == i )
		{
			pUserMission->TriggerMission(MISSION_CLASS_WEEKLY_COMPLETE, vCompleteVec);
			pUserMission->TriggerMission(MISSION_CLASS_WEEKLY_ALL_CLEAR, vCompleteVec);
		}
		else
			pUserMission->TriggerMission(MISSION_CLASS_MONTHLY_ALL_CLEAR, vCompleteVec);	

		pUserMission->InitCompletedTypeCount(i);
	}
}

BOOL MissionManager::IsComplete(MissionData* pMissionData)
{
	if( !pMissionData )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[warining][missionmgr] missiondata is null");
		return FALSE;
	}

	Mission* pMission = GetActiveMission(pMissionData->GetCode());
	if( pMission )
	{
		return pMission->IsComplete(pMissionData->GetValue());
	}

	return FALSE;
}

Mission* MissionManager::GetActiveMission(const DWORD dwCode)
{
	mCurMissionTable::iterator it = m_mDailyMissionTable.find(dwCode);
	if( it != m_mDailyMissionTable.end() )
		return it->second;

	it = m_mWeeklyMissionTable.find(dwCode);
	if( it != m_mWeeklyMissionTable.end() )
		return it->second;

	it = m_mMonthlyMissionTable.find(dwCode);
	if( it != m_mMonthlyMissionTable.end() )
		return it->second;

	return NULL;
}

DWORD MissionManager::GetPresentID(DWORD dwCode)
{
	Mission* pMission = GetActiveMission(dwCode);
	if( pMission )
		return pMission->GetMissionPresent();

	return 0;
}

void MissionManager::SendReward(User* pUser, const DWORD dwMissionCode)
{
	if( !pUser )
		return;

	Mission* pMission = GetActiveMission(dwMissionCode);
	if( pMission )
	{
		bool bDirect = g_QuestMgr.SendRewardPresent(pUser, pMission->GetMissionPresent() );
		if( !bDirect )
			pUser->SendPresentMemory();

		//LogDB insert
		int iParam1 = 0, iParam2 = 0, iParam3 = 0, iParam4 = 0;
		g_QuestMgr.GetRewardPresent(pMission->GetMissionPresent(), iParam1, iParam2, iParam3, iParam4);

		g_LogDBClient.OnInsertGameLogInfo(LogDBClient::GLT_COMPLETE_COMPENSATION, pUser, 0, pMission->GetMissionCode(), (int)(pMission->GetMissionType()) + 1, iParam1, iParam2, iParam3, iParam4, NULL );
	}
	else
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[warning][missionmgr] mission is not exist, present is not recved : [missioncode:%d]", dwMissionCode );
	}
	
}

void MissionManager::GetResetMissionType(const DWORD dwDate, IntVec& vMissionType)
{
	bool bResetFlag[MT_MONTHLY+1] = {false, false, false};
	DWORD dwActiveDate = 0;

	for( int i = 0; i <= MT_MONTHLY; i++ )
	{
		dwActiveDate = GetActiveMissionDate(i);

		if( 0 == dwActiveDate || dwDate < m_vActiveDate[i] )
			bResetFlag[i] = true;
	}

	vMissionType.push_back(bResetFlag[MT_DAILY]);
	vMissionType.push_back(bResetFlag[MT_WEEKLY]);
	vMissionType.push_back(bResetFlag[MT_MONTHLY]);
}

void MissionManager::TurnCurDataIntoNextData(ioMission* pUserMission, int iResetType)
{
	if( !pUserMission )
		return;

	if( !COMPARE(iResetType, MT_DAILY, MT_MONTHLY+1) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[warning][missionmgr] non exist missiontype : [type:%d]", iResetType);
		return;
	}

	//pUserMission->InitMissionType(iResetType);
	pUserMission->DeleteMissionData(iResetType);
	FillActiveMissionData( pUserMission, iResetType);
}

void MissionManager::FillActiveMissionData(ioMission* pUserMission, const int iMissionType)
{
	if( !pUserMission )
		return;

	if( !COMPARE(iMissionType, MT_DAILY, MT_MONTHLY+1) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[warning][missionmgr] non exist missiontype : [type:%d]", iMissionType);
		return;
	}

	mCurMissionTable::iterator it;
	switch( iMissionType )
	{
	case MT_DAILY:
		{
			it = m_mDailyMissionTable.begin();
			for( ; it != m_mDailyMissionTable.end(); it++ )
			{
				Mission* pMission = it->second;
				if( pMission )
					pUserMission->InsertMission(pMission->GetMissionCode(), MS_PROGRESS, MT_DAILY, 0 );
			}
			break;
		}
	case MT_WEEKLY:
		{
			it = m_mWeeklyMissionTable.begin();
			for( ; it != m_mWeeklyMissionTable.end(); it++ )
			{
				Mission* pMission = it->second;
				if( pMission )
					pUserMission->InsertMission(pMission->GetMissionCode(), MS_PROGRESS, MT_WEEKLY, 0 );
			}
			break;
		}
	case  MT_MONTHLY:
		{
			it = m_mMonthlyMissionTable.begin();
			for( ; it != m_mMonthlyMissionTable.end(); it++ )
			{
				Mission* pMission = it->second;
				if( pMission )
					pUserMission->InsertMission(pMission->GetMissionCode(), MS_PROGRESS, MT_MONTHLY, 0 );
			}
			break;
		}
	}
}

void MissionManager::FillAllActiveMissionData(ioMission* pUserMission)
{
	if( !pUserMission )
		return;

	FillActiveMissionData(pUserMission, MT_DAILY);
	FillActiveMissionData(pUserMission, MT_WEEKLY);
	FillActiveMissionData(pUserMission, MT_MONTHLY);
}

void MissionManager::ChangeActiveMissionData(const int iType)
{
	DWORD dwActiveDate = 0;

	switch(iType)
	{
	case MT_DAILY:
		{
			DeleteDailyMissionTable();
			break;
		}
	case MT_WEEKLY:
		{
			DeleteWeeklyMissionTable();
			break;
		}
	case MT_MONTHLY:
		{
			DeleteMonthlyMissionTable();
			break;
		}
	}

	CreateActiveMissionTable(iType);
}

void MissionManager::CreateActiveMissionTable(const int iType)
{
	ioINILoader kLoader;
	LoadMissionTypeIniFile(kLoader, iType);
	DWORD dwActiveDate = GetActiveMissionDate(iType);
	int iIndex = GetActiveIndex(iType);
	if( iIndex <= 0 || iIndex > GetMaxIndex(iType))
		return;

	if( dwActiveDate != 0 )
	{
		char szKey[MAX_PATH];
		char szValues[MAX_PATH] = "";
		StringCbPrintf( szKey, sizeof( szKey ), "date%d", iIndex );
		kLoader.SetTitle(szKey);
		for( int i = 0; i < 1000; i++ )
		{

			StringCbPrintf( szKey, sizeof( szKey ), "mission%d_code", i+1 );
			int iMissionCode	= kLoader.LoadInt(szKey, 0);
			if( 0 == iMissionCode )
				break;

			StringCbPrintf( szKey, sizeof( szKey ), "mission%d_type", i+1 );
			int iMissionClass	= kLoader.LoadInt(szKey, 0);

			StringCbPrintf( szKey, sizeof( szKey ), "mission%d_values", i+1 );
			kLoader.LoadString(szKey, "", szValues, MAX_PATH );

			//value 파싱
			IntVec vValues;
			Help::TokenizeToINT(szValues, ".", vValues);

			StringCbPrintf( szKey, sizeof( szKey ), "mission%d_present", i+1 );
			int iPresent		= kLoader.LoadInt(szKey, 0);

			Mission* pMission = new Mission;
			if( !pMission )
				break;

			if( !pMission->Create(iMissionCode, (MissionClasses)iMissionClass, (MissionTypes)iType, vValues, iPresent) )
			{
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[warning][missionmgr] mission value is invalid : [code:%d]", iMissionCode);
				continue;
			}

			switch( iType )
			{
			case MT_DAILY:
				m_mDailyMissionTable.insert(make_pair(pMission->GetMissionCode(), pMission));
				break;
			case MT_WEEKLY:
				m_mWeeklyMissionTable.insert(make_pair(pMission->GetMissionCode(), pMission));
				break;
			case MT_MONTHLY:
				m_mMonthlyMissionTable.insert(make_pair(pMission->GetMissionCode(), pMission));
			}
		}

		//동작 중인 미션 로그!
		mCurMissionTable::iterator it;
		for( it = m_mDailyMissionTable.begin() ; it != m_mDailyMissionTable.end(); it++ )
		{
			Mission *pMission = it->second;
			if( pMission )
				LOG.PrintTimeAndLog(LOG_DEBUG_LEVEL, "[info][missionmgr] daily active mission code : %d", pMission->GetMissionCode());
		}

		for( it = m_mWeeklyMissionTable.begin(); it != m_mWeeklyMissionTable.end(); it++ )
		{
			Mission *pMission = it->second;
			if( pMission )
				LOG.PrintTimeAndLog(LOG_DEBUG_LEVEL, "[info][missionmgr] weekly active mission code : %d", pMission->GetMissionCode());
		}

		for( it = m_mMonthlyMissionTable.begin(); it != m_mMonthlyMissionTable.end(); it++ )
		{
			Mission *pMission = it->second;
			if( pMission )
				LOG.PrintTimeAndLog(LOG_DEBUG_LEVEL, "[info][missionmgr] monthly active mission code : %d", pMission->GetMissionCode());
		}
	}
}

BOOL MissionManager::IsTimeMission(const DWORD dwCode)
{
	Mission* pMission = GetActiveMission(dwCode);
	if( pMission )
	{
		int iMissionClass = pMission->GetMissionClass();

		switch( iMissionClass )
		{
		case MISSION_CLASS_LOGINTIME_CHECK:
			return TRUE;
		}
	}

	return FALSE;
}

int MissionManager::GetMissionClass(const DWORD dwCode)
{
	Mission* pMission = GetActiveMission(dwCode);
	if( pMission )
		return pMission->GetMissionClass();

	return MISSION_CLASS_NONE;
}

void MissionManager::TestChangeNextDate( SP2Packet& kPacket )
{
	int iMacroType = 0;
	int iType = 0;
	int iMinutes = 0;
	int iNextTab = 0;

	PACKET_GUARD_VOID( kPacket.Read(iMacroType) );
	PACKET_GUARD_VOID( kPacket.Read(iType) );

	if( !COMPARE(iType, 0, MT_MONTHLY + 1) )
		return;

	if( 1 == iMacroType )
	{
		PACKET_GUARD_VOID( kPacket.Read(iMinutes) );
		
		DWORD dwActiveDate = GetActiveMissionDate(iType);
		DWORD dwNextDate  = GetNextMissionDate(iType);

		if( dwActiveDate != 0 && dwNextDate != 0 )
		{
	
			CTime cNextDate = CTime::GetCurrentTime();
			CTimeSpan cSpan(0,0,iMinutes,0);
			cNextDate += cSpan;

			DWORD dwNextDate = cNextDate.GetTime();
			SetNextMissionDate(iType, dwNextDate);
		}
	}
	else if( 2 == iMacroType )
	{
		PACKET_GUARD_VOID( kPacket.Read(iNextTab) );
		PACKET_GUARD_VOID( kPacket.Read(iMinutes) );

		int iActiveIndex = GetActiveIndex(iType);
		if( iActiveIndex != 0 )
		{
			if( 0 == iNextTab )
				return;

			iActiveIndex += iNextTab;
			if( GetMaxIndex(iType) < iActiveIndex )
				return;

			SetActiveIndex(iType, iActiveIndex-1);

			CTime cNextDate = CTime::GetCurrentTime();
			CTimeSpan cSpan(0,0,iMinutes,0);
			cNextDate += cSpan;

			DWORD dwNextDate = cNextDate.GetTime();
			SetNextMissionDate(iType, dwNextDate);
		}

	}
}

DWORD MissionManager::GetMostRapidNextActiveDate()
{
	DWORD dwNextActiveDate = GetNextMissionDate(MT_DAILY);
	if( 0 == dwNextActiveDate )
	{
		DWORD dwNextWeekly = GetNextMissionDate(MT_WEEKLY);
		DWORD dwNextMonthly = GetNextMissionDate(MT_MONTHLY);
		
		if( dwNextWeekly != 0 && 0 == dwNextMonthly )
			dwNextActiveDate = dwNextWeekly;
		else if( dwNextMonthly != 0 && 0 == dwNextWeekly )
			dwNextActiveDate = dwNextMonthly;
		else if( dwNextMonthly != 0 && dwNextWeekly != 0 )
		{
			dwNextActiveDate = min(dwNextWeekly, dwNextMonthly);
		}
	}

	return dwNextActiveDate;
}