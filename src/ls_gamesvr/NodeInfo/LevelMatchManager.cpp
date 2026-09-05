// RoomReserveManager.cpp: implementation of the RoomReserveManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "RoomNodeManager.h"
#include "BattleRoomManager.h"
#include "LadderTeamManager.h"
#include "LevelMatchManager.h"

LevelMatchManager *LevelMatchManager::sg_Instance = NULL;
LevelMatchManager::LevelMatchManager()
{
	m_iRoomEnterSafetyLevel = 2;
	m_iAddGradeLevel        = 1;
	m_iLeaveSafetyRoomLevel = 15;
	m_iLeaveSafetyRoomKo    = 10;	
	m_iRoomEnterLevelMax    = 64;
	m_iPartyLevelCheckMinRoom = 0;

	m_fPartyEnterLimitCount  = 0.0f;
	m_dwPartySortUpdateTime  = 10000;
	m_dwPartyUpdateTime      = 0;
	m_iMatchingSafetyLowLevel= 5;
	m_iMatchingSafetyLowLevelHighLimit = 10;
	m_iMatchingSafetyHighLevel= 30;
	m_iMatchingSafetyHighLevelLowLimit = 15;

	m_dwPlazaSortUpdateTime  = 10000;
	m_dwPlazaUpdateTime      = 0;
	m_fPlazaEnterLimitCount  = 0.0f;
	m_iMatchingPlazaLowLevel   = 5;
	m_iMatchingPlazaLowLevelHighLimit = 10;
	m_iMatchingPlazaHighLevel  = 30;
	m_iMatchingPlazaHighLevelLowLimit = 15;

	m_fLadderEnterLimitCount  = 0.0f;
	m_dwLadderSortUpdateTime  = 10000;
	m_dwLadderUpdateTime      = 0;
	m_iMatchingLadderLowLevel = 5;
	m_iMatchingLadderLowLevelHighLimit = 10;
	m_iMatchingLadderHighLevel= 30;
	m_iMatchingLadderHighLevelLowLimit = 15;

	m_fLadderHeroEnterLimitCount  = 0.0f;
	m_dwLadderHeroSortUpdateTime  = 10000;
	m_dwLadderHeroUpdateTime      = 0;
	m_iMatchingLadderHeroLowLevel = 5;
	m_iMatchingLadderHeroLowLevelHighLimit = 10;
	m_iMatchingLadderHeroHighLevel= 30;
	m_iMatchingLadderHeroHighLevelLowLimit = 15;
	m_bMatchingLadderHeroLOG = false;

	m_iKillDeathMinLevelGap	= 0;
}

LevelMatchManager::~LevelMatchManager()
{
	m_vPartyEnterLevelMin.clear();
	m_vPartyEnterLevelMax.clear();

	m_vPlazaEnterLevelMin.clear();
	m_vPlazaEnterLevelMax.clear();

	m_vLadderEnterLevelMin.clear();
	m_vLadderEnterLevelMax.clear();

	m_vLadderHeroEnterLevelMin.clear();
	m_vLadderHeroEnterLevelMax.clear();
}

LevelMatchManager &LevelMatchManager::GetInstance()
{
	if( sg_Instance == NULL )
		sg_Instance = new LevelMatchManager;
	return *sg_Instance;
}

void LevelMatchManager::ReleaseInstance()
{
	SAFEDELETE( sg_Instance );
}

void LevelMatchManager::CheckNeedReload()
{
	ioINILoader kLoader;
	kLoader.SetFileName( "config/sp2_level_match.ini" );
	if( kLoader.ReadBool( "info", "Change", false ) )
	{
		LoadLevelMatchInfo();
		MatchingTableLOG();
	}
}

void LevelMatchManager::LoadLevelMatchInfo()
{
	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";
	int i = 0;

	ioINILoader kLoader;
	kLoader.ReloadFile( "config/sp2_level_match.ini" );

	kLoader.SetTitle( "info" );
	m_iPartyLevelCheckMinRoom = kLoader.LoadInt( "PartyLevelCheckMinRoom", 0 );

	// 공통 정보
	kLoader.SetTitle( "level_info" );
	m_iRoomEnterSafetyLevel	= kLoader.LoadInt( "safety_level", 2 );
	m_iAddGradeLevel        = kLoader.LoadInt( "add_level", 1 );
	m_iLeaveSafetyRoomLevel = kLoader.LoadInt( "leave_safety_room_level", 15 );
	m_iLeaveSafetyRoomKo    = kLoader.LoadInt( "leave_safety_room_ko", 10 );
	m_iRoomEnterLevelMax    = kLoader.LoadInt( "max_level", 64 );
	m_iKillDeathMinLevelGap	= kLoader.LoadInt( "kill_death_min_level_gap", 5 );

	// 파티 레벨별 매칭
	kLoader.SetTitle( "party_level_info" );	
	m_dwPartySortUpdateTime = kLoader.LoadInt( "update_time", 10000 );
	m_fPartyEnterLimitCount = kLoader.LoadFloat( "limit_count", 0.33f );
	m_iMatchingSafetyLowLevel			= kLoader.LoadInt( "match_safety_low_level", 5 );
	m_iMatchingSafetyLowLevelHighLimit	= kLoader.LoadInt( "match_safety_low_level_high_limit", 10 );
	m_iMatchingSafetyHighLevel			= kLoader.LoadInt( "match_safety_high_level", 30 );
	m_iMatchingSafetyHighLevelLowLimit	= kLoader.LoadInt( "match_safety_high_level_low_limit", 15 );

	// 광장 레벨별 매칭
	kLoader.SetTitle( "plaza_level_info" );
	m_dwPlazaSortUpdateTime = kLoader.LoadInt( "update_time", 10000 );
	m_fPlazaEnterLimitCount = kLoader.LoadFloat( "limit_count", 0.33f );
	m_iMatchingPlazaLowLevel			= kLoader.LoadInt( "match_plaza_low_level", 5 );
	m_iMatchingPlazaLowLevelHighLimit	= kLoader.LoadInt( "match_plaza_low_level_high_limit", 10 );
	m_iMatchingPlazaHighLevel			= kLoader.LoadInt( "match_plaza_high_level", 30 );
	m_iMatchingPlazaHighLevelLowLimit	= kLoader.LoadInt( "match_plaza_high_level_low_limit", 15 );

	// 래더 레벨별 매칭
	kLoader.SetTitle( "ladder_level_info" );	
	m_dwLadderSortUpdateTime = kLoader.LoadInt( "update_time", 10000 );
	m_fLadderEnterLimitCount = kLoader.LoadFloat( "limit_count", 0.33f );
	m_iMatchingLadderLowLevel			= kLoader.LoadInt( "match_ladder_low_level", 5 );
	m_iMatchingLadderLowLevelHighLimit	= kLoader.LoadInt( "match_ladder_low_level_high_limit", 10 );
	m_iMatchingLadderHighLevel			= kLoader.LoadInt( "match_ladder_high_level", 30 );
	m_iMatchingLadderHighLevelLowLimit	= kLoader.LoadInt( "match_ladder_high_level_low_limit", 15 );

	// 래더 - 영웅전 레벨별 매칭
	kLoader.SetTitle( "ladder_hero_level_info" );	
	m_dwLadderHeroSortUpdateTime = kLoader.LoadInt( "update_time", 10000 );
	m_fLadderHeroEnterLimitCount = kLoader.LoadFloat( "limit_count", 0.33f );
	m_iMatchingLadderHeroLowLevel			= kLoader.LoadInt( "match_ladder_hero_low_level", 5 );
	m_iMatchingLadderHeroLowLevelHighLimit	= kLoader.LoadInt( "match_ladder_hero_low_level_high_limit", 10 );
	m_iMatchingLadderHeroHighLevel			= kLoader.LoadInt( "match_ladder_hero_high_level", 30 );
	m_iMatchingLadderHeroHighLevelLowLimit	= kLoader.LoadInt( "match_ladder_hero_high_level_low_limit", 15 );
	m_bMatchingLadderHeroLOG                = kLoader.LoadBool( "match_ladder_hero_log", false );

	// 로드 완료
	//kLoader.SetTitle( "info" );
	//kLoader.SaveBool( "Change", false );
}

void LevelMatchManager::ProcessLevelMatch()
{
	FUNCTION_TIME_CHECKER( 100000.0f, 0 );          // 0.1 초 이상 걸리면로그 남김

	if( TIMEGETTIME() - m_dwPartyUpdateTime > GetPartySortUpdateTime() )
	{
		m_dwPartyUpdateTime = TIMEGETTIME();
		g_BattleRoomManager.CreateMatchingTable();
	}

	if( TIMEGETTIME() - m_dwPlazaUpdateTime > GetPlazaSortUpdateTime() )
	{
		m_dwPlazaUpdateTime = TIMEGETTIME();
		g_RoomNodeManager.CreateMatchingTable();
	}

	if( TIMEGETTIME() - m_dwLadderUpdateTime > GetLadderSortUpdateTime() )
	{
		m_dwLadderUpdateTime = TIMEGETTIME();
		g_LadderTeamManager.CreateTeamMatchingTable();
	}

	if( TIMEGETTIME() - m_dwLadderHeroUpdateTime > GetLadderHeroSortUpdateTime() )
	{
		m_dwLadderHeroUpdateTime = TIMEGETTIME();
		g_LadderTeamManager.CreateHeroMatchingTable();
	}
}

void LevelMatchManager::MatchingTableLOG()
{
	int i = 0;
	int iPartySize = min( (int)m_vPartyEnterLevelMin.size(), (int)m_vPartyEnterLevelMax.size() );
	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "[Party Matching Table]" );
	for(i = 0;i < iPartySize;i++)
	{
		LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "UserLevel:%d : Min[%d] - Max[%d]", i, m_vPartyEnterLevelMin[i], m_vPartyEnterLevelMax[i] );
	}

	int iPlazaSize = min( (int)m_vPlazaEnterLevelMin.size(), (int)m_vPlazaEnterLevelMax.size() );
	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "[Plaza Matching Table]" );
	for(i = 0;i < iPlazaSize;i++)
	{
		LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "UserLevel:%d : Min[%d] - Max[%d]", i, m_vPlazaEnterLevelMin[i], m_vPlazaEnterLevelMax[i] );
	}

	int iLadderSize = min( (int)m_vLadderEnterLevelMin.size(), (int)m_vLadderEnterLevelMax.size() );
	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "[Ladder Matching Table]" );
	for(i = 0;i < iLadderSize;i++)
	{
		LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "UserLevel:%d : Min[%d] - Max[%d]", i, m_vLadderEnterLevelMin[i], m_vLadderEnterLevelMax[i] );
	}

	int iLadderHeroSize = min( (int)m_vLadderHeroEnterLevelMin.size(), (int)m_vLadderHeroEnterLevelMax.size() );
	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "[Ladder Hero Matching Table]" );
	for(i = 0;i < iLadderHeroSize;i++)
	{
		LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "UserLevel:%d : Min[%d] - Max[%d]", i, m_vLadderHeroEnterLevelMin[i], m_vLadderHeroEnterLevelMax[i] );
	}
}

void LevelMatchManager::InitPartyLevelMatch()
{
	m_vPartyEnterLevelMin.clear();
	m_vPartyEnterLevelMax.clear();
}

void LevelMatchManager::InsertPartyLevelMatch( int iMinLevel, int iMaxLevel )
{
	int iPartyLevel = (int)m_vPartyEnterLevelMin.size();
	if( iPartyLevel < m_iMatchingSafetyLowLevel )
		iMaxLevel = min( iPartyLevel + m_iMatchingSafetyLowLevelHighLimit, iMaxLevel );
	else if( iPartyLevel >= m_iMatchingSafetyHighLevel )
		iMinLevel = max( m_iMatchingSafetyHighLevelLowLimit, iMinLevel );

	m_vPartyEnterLevelMin.push_back( iMinLevel );
	m_vPartyEnterLevelMax.push_back( iMaxLevel );
}

int LevelMatchManager::GetPartyLevelLowLimit( int iLevel )
{
	// 최대 레벨을 넘어서면 레벨을 최대값에 고정
	if( iLevel >= (int)m_vPartyEnterLevelMin.size() )
		iLevel = max( (int)m_vPartyEnterLevelMin.size() - 1, 0 );	

	int iMinLevel = min( GetRoomEnterSafetyLevel(), iLevel );
	if( COMPARE( iLevel, 0, (int)m_vPartyEnterLevelMin.size() ) )
		iMinLevel = m_vPartyEnterLevelMin[iLevel];
	if( iLevel < GetRoomEnterSafetyLevel() )
		iMinLevel = 0;
	return iMinLevel;
}

int LevelMatchManager::GetPartyLevelHighLimit( int iLevel )
{
	// 최대 레벨을 넘어서면 레벨을 최대값에 고정
	if( iLevel >= (int)m_vPartyEnterLevelMax.size() )
		iLevel = max( (int)m_vPartyEnterLevelMax.size() - 1, 0 );	

	int iMaxLevel = max( GetRoomEnterLevelMax(), iLevel );
	if( COMPARE( iLevel, 0, (int)m_vPartyEnterLevelMax.size() ) )
		iMaxLevel = m_vPartyEnterLevelMax[iLevel];
	if( iLevel < GetRoomEnterSafetyLevel() )
		iMaxLevel = GetRoomEnterSafetyLevel() - 1;
	return iMaxLevel;
}

bool LevelMatchManager::IsPartyLevelJoin( int iParty, int iUser, int iCheckLevelType )
{
	int iMinLevel = GetPartyLevelLowLimit( iUser );
	int iMaxLevel = GetPartyLevelHighLimit( iUser );

	int iPartyLevel = min( GetRoomEnterLevelMax(), iParty );
	if( iCheckLevelType == JOIN_CHECK_MIN_LEVEL )
		return ( iPartyLevel >= iMinLevel );
	else if( iCheckLevelType == JOIN_CHECK_MAX_LEVEL )
		return ( iPartyLevel <= iMaxLevel );
	return COMPARE( iPartyLevel, iMinLevel, iMaxLevel + 1 );
}

void LevelMatchManager::InitPlazaLevelMatch()
{
	m_vPlazaEnterLevelMin.clear();
	m_vPlazaEnterLevelMax.clear();
}

void LevelMatchManager::InsertPlazaLevelMatch( int iMinLevel, int iMaxLevel )
{
	int iPlazaLevel = (int)m_vPlazaEnterLevelMin.size();
	if( iPlazaLevel < m_iMatchingPlazaLowLevel )
		iMaxLevel = min( iPlazaLevel + m_iMatchingPlazaLowLevelHighLimit, iMaxLevel );
	else if( iPlazaLevel >= m_iMatchingPlazaHighLevel )
		iMinLevel = max( m_iMatchingPlazaHighLevelLowLimit, iMinLevel );

	m_vPlazaEnterLevelMin.push_back( iMinLevel );
	m_vPlazaEnterLevelMax.push_back( iMaxLevel );
}

int LevelMatchManager::GetPlazaLevelLowLimit( int iLevel )
{
	// 최대 레벨을 넘어서면 레벨을 최대값에 고정
	if( iLevel >= (int)m_vPlazaEnterLevelMin.size() )
		iLevel = max( (int)m_vPlazaEnterLevelMin.size() - 1, 0 );	

	int iMinLevel = min( GetRoomEnterSafetyLevel(), iLevel );
	if( COMPARE( iLevel, 0, (int)m_vPlazaEnterLevelMin.size() ) )
		iMinLevel = m_vPlazaEnterLevelMin[iLevel];
	if( iLevel < GetRoomEnterSafetyLevel() )
		iMinLevel = 0;
	return iMinLevel;
}

int LevelMatchManager::GetPlazaLevelHighLimit( int iLevel )
{
	// 최대 레벨을 넘어서면 레벨을 최대값에 고정
	if( iLevel >= (int)m_vPlazaEnterLevelMax.size() )
		iLevel = max( (int)m_vPlazaEnterLevelMax.size() - 1, 0 );	

	int iMaxLevel = max( GetRoomEnterLevelMax(), iLevel );
	if( COMPARE( iLevel, 0, (int)m_vPlazaEnterLevelMax.size() ) )
		iMaxLevel = m_vPlazaEnterLevelMax[iLevel];
	if( iLevel < GetRoomEnterSafetyLevel() )
		iMaxLevel = GetRoomEnterSafetyLevel() - 1;
	return iMaxLevel;
}

bool LevelMatchManager::IsPlazaLevelJoin( int iPlaza, int iUser, int iCheckLevelType )
{
	int iMinLevel = GetPlazaLevelLowLimit( iUser );
	int iMaxLevel = GetPlazaLevelHighLimit( iUser );

	int iPlazaLevel = min( GetRoomEnterLevelMax(), iPlaza );
	if( iCheckLevelType == JOIN_CHECK_MIN_LEVEL )
		return ( iPlazaLevel >= iMinLevel );
	else if( iCheckLevelType == JOIN_CHECK_MAX_LEVEL )
		return ( iPlazaLevel <= iMaxLevel );
	return COMPARE( iPlazaLevel, iMinLevel, iMaxLevel + 1 );
}

void LevelMatchManager::InitLadderLevelMatch()
{
	m_vLadderEnterLevelMin.clear();
	m_vLadderEnterLevelMax.clear();
}

void LevelMatchManager::InsertLadderLevelMatch( int iMinLevel, int iMaxLevel )
{
	int iLadderLevel = (int)m_vLadderEnterLevelMin.size();
	if( iLadderLevel < m_iMatchingLadderLowLevel )
		iMaxLevel = min( iLadderLevel + m_iMatchingLadderLowLevelHighLimit, iMaxLevel );
	else if( iLadderLevel >= m_iMatchingLadderHighLevel )
		iMinLevel = max( m_iMatchingLadderHighLevelLowLimit, iMinLevel );

	m_vLadderEnterLevelMin.push_back( iMinLevel );
	m_vLadderEnterLevelMax.push_back( iMaxLevel );
}

int LevelMatchManager::GetLadderLevelLowLimit( int iLevel )
{
	// 최대 레벨을 넘어서면 레벨을 최대값에 고정
	if( iLevel >= (int)m_vLadderEnterLevelMin.size() )
		iLevel = max( (int)m_vLadderEnterLevelMin.size() - 1, 0 );	

	int iMinLevel = min( GetRoomEnterSafetyLevel(), iLevel );
	if( COMPARE( iLevel, 0, (int)m_vLadderEnterLevelMin.size() ) )
		iMinLevel = m_vLadderEnterLevelMin[iLevel];
	if( iLevel < GetRoomEnterSafetyLevel() )
		iMinLevel = 0;
	return iMinLevel;
}

int LevelMatchManager::GetLadderLevelHighLimit( int iLevel )
{
	// 최대 레벨을 넘어서면 레벨을 최대값에 고정
	if( iLevel >= (int)m_vLadderEnterLevelMax.size() )
		iLevel = max( (int)m_vLadderEnterLevelMax.size() - 1, 0 );	

	int iMaxLevel = max( GetRoomEnterLevelMax(), iLevel );
	if( COMPARE( iLevel, 0, (int)m_vLadderEnterLevelMax.size() ) )
		iMaxLevel = m_vLadderEnterLevelMax[iLevel];
	if( iLevel < GetRoomEnterSafetyLevel() )
		iMaxLevel = GetRoomEnterSafetyLevel() - 1;
	return iMaxLevel;
}

bool LevelMatchManager::IsLadderLevelJoin( int iLadder, int iUser, int iCheckLevelType )
{
	int iMinLevel = GetLadderLevelLowLimit( iUser );
	int iMaxLevel = GetLadderLevelHighLimit( iUser );

	int iLadderLevel = min( GetRoomEnterLevelMax(), iLadder );
	if( iCheckLevelType == JOIN_CHECK_MIN_LEVEL )
		return ( iLadderLevel >= iMinLevel );
	else if( iCheckLevelType == JOIN_CHECK_MAX_LEVEL )
		return ( iLadderLevel <= iMaxLevel );
	return COMPARE( iLadderLevel, iMinLevel, iMaxLevel + 1 );
}

void LevelMatchManager::InitLadderHeroLevelMatch()
{
	m_vLadderHeroEnterLevelMin.clear();
	m_vLadderHeroEnterLevelMax.clear();
}

void LevelMatchManager::InsertLadderHeroLevelMatch( int iMinLevel, int iMaxLevel )
{
	int iLadderHeroLevel = (int)m_vLadderHeroEnterLevelMin.size();
	if( iLadderHeroLevel < m_iMatchingLadderHeroLowLevel )
		iMaxLevel = min( iLadderHeroLevel + m_iMatchingLadderHeroLowLevelHighLimit, iMaxLevel );
	else if( iLadderHeroLevel >= m_iMatchingLadderHeroHighLevel )
		iMinLevel = max( m_iMatchingLadderHeroHighLevelLowLimit, iMinLevel );

	m_vLadderHeroEnterLevelMin.push_back( iMinLevel );
	m_vLadderHeroEnterLevelMax.push_back( iMaxLevel );

	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "InsertLadderHeroLevelMatch : minLevel: %d maxLevel: %d"
			, iMinLevel, iMaxLevel );

	if( IsMatchingLadderHeroLOG() )
	{
		LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "IsMatchingLadderHeroLOG : %d][%d][%d]", iLadderHeroLevel, iMinLevel, iMaxLevel );
	}
}

int LevelMatchManager::GetLadderHeroLevelLowLimit( int iLevel )
{
	// 최대 레벨을 넘어서면 레벨을 최대값에 고정
	if( iLevel >= (int)m_vLadderHeroEnterLevelMin.size() )
		iLevel = max( (int)m_vLadderHeroEnterLevelMin.size() - 1, 0 );	

	int iMinLevel = min( GetRoomEnterSafetyLevel(), iLevel );
	if( COMPARE( iLevel, 0, (int)m_vLadderHeroEnterLevelMin.size() ) )
		iMinLevel = m_vLadderHeroEnterLevelMin[iLevel];
	if( iLevel < GetRoomEnterSafetyLevel() )
		iMinLevel = 0;
	return iMinLevel;
}

int LevelMatchManager::GetLadderHeroLevelHighLimit( int iLevel )
{
	// 최대 레벨을 넘어서면 레벨을 최대값에 고정
	if( iLevel >= (int)m_vLadderHeroEnterLevelMax.size() )
		iLevel = max( (int)m_vLadderHeroEnterLevelMax.size() - 1, 0 );	

	int iMaxLevel = max( GetRoomEnterLevelMax(), iLevel );
	if( COMPARE( iLevel, 0, (int)m_vLadderHeroEnterLevelMax.size() ) )
		iMaxLevel = m_vLadderHeroEnterLevelMax[iLevel];
	if( iLevel < GetRoomEnterSafetyLevel() )
		iMaxLevel = GetRoomEnterSafetyLevel() - 1;
	return iMaxLevel;
}

bool LevelMatchManager::IsLadderHeroLevelJoin( int iLadderSearchCnt, int iLadder, int iUser, int& iGapLevel )
{
	// 무조건 1차 범위부터 찾는다
	int iMinLevel = iLadder - LADDER_LEVEL_MATCH;
	int iMaxLevel = iLadder + LADDER_LEVEL_MATCH;

	if( iMinLevel < 0 )
		iMinLevel = 0;

	if( iMaxLevel > GetRoomEnterLevelMax() )
		iMaxLevel = GetRoomEnterLevelMax();

	if( COMPARE(iUser, iMinLevel, iMaxLevel + 1) )
	{
		iGapLevel = abs(iLadder - iUser);
		return true;
	}

	// 2차 범위에서 찾기
	if( iLadderSearchCnt > 2 )
	{
		iMinLevel = iMinLevel - LADDER_LEVEL_MATCH;
		iMaxLevel = iMaxLevel + LADDER_LEVEL_MATCH;

		if( iMinLevel < 0 )
			iMinLevel = 0;

		if( iMaxLevel > GetRoomEnterLevelMax() )
			iMaxLevel = GetRoomEnterLevelMax();

		if( COMPARE(iUser, iMinLevel, iMaxLevel + 1) )
		{
			iGapLevel = abs(iLadder - iUser);
			return true;
		}
	}

	// 3차 범위에서 찾기
	if( iLadderSearchCnt > 10 )
	{
		iMinLevel = iMinLevel - LADDER_LEVEL_MATCH;
		iMaxLevel = iMaxLevel + LADDER_LEVEL_MATCH;

		if( iMinLevel < 0 )
			iMinLevel = 0;

		if( iMaxLevel > GetRoomEnterLevelMax() )
			iMaxLevel = GetRoomEnterLevelMax();

		if( COMPARE(iUser, iMinLevel, iMaxLevel + 1) )
		{
			iGapLevel = abs(iLadder - iUser);
			return true;
		}
	}

	return false;

	//int iMinLevel = GetLadderHeroLevelLowLimit( iUser );
	//int iMaxLevel = GetLadderHeroLevelHighLimit( iUser );

	//int iLadderHeroLevel = min( GetRoomEnterLevelMax(), iLadder );

	//LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "IsLadderHeroLevelJoin : iLadder: %d iUser: %d, iCheckLevelType: %d iMinLevel: %d iMaxLevel: %d iLadderHeroLevel: %d"
	//		, iLadder, iUser, iCheckLevelType, iMinLevel, iMaxLevel, iLadderHeroLevel );

	//if( iCheckLevelType == JOIN_CHECK_MIN_LEVEL )
	//	return ( iLadderHeroLevel >= iMinLevel );
	//else if( iCheckLevelType == JOIN_CHECK_MAX_LEVEL )
	//	return ( iLadderHeroLevel <= iMaxLevel );
	//return COMPARE( iLadderHeroLevel, iMinLevel, iMaxLevel + 1 );
}
