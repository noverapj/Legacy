#include "stdafx.h"

//#include "../Window.h"
#include "../MainProcess.h"

#include "User.h"
#include "ShuffleRoomManager.h"
#include "LevelMatchManager.h"
#include "ServerNodeManager.h"

#include "../Util/IORandom.h"

ShuffleRoomManager* ShuffleRoomManager::sg_Instance = NULL;
ShuffleRoomManager::ShuffleRoomManager()
{
	m_dwCurTime				= 0;
	m_iMaxShuffleRoomSize	= 0;
	m_nModeMaxCount			= 0;

	LoadINI();
}

ShuffleRoomManager::~ShuffleRoomManager()
{
}

ShuffleRoomManager &ShuffleRoomManager::GetInstance()
{
	if( sg_Instance == NULL )
		sg_Instance = new ShuffleRoomManager;
	return *sg_Instance;
}

void ShuffleRoomManager::ReleaseInstance()
{
	SAFEDELETE( sg_Instance );
}

void ShuffleRoomManager::InitMemoryPool( const DWORD dwServerIndex )
{
	{
		ioINILoader kLoader( "ls_config_game.ini" );
		kLoader.SetTitle( "MemoryPool" );
		m_iMaxShuffleRoomSize = kLoader.LoadInt( "shuffleroom_pool", 3000 );
	}
	
	int iStartIndex = dwServerIndex * m_iMaxShuffleRoomSize;

	m_MemNode.CreatePool( 0, m_iMaxShuffleRoomSize, FALSE );
	for( int i=1; i<=m_iMaxShuffleRoomSize; ++i )
	{
		m_MemNode.Push( new ShuffleRoomNode( i + iStartIndex ) );
	}

	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[info][shuffle]Init memory pool : [%d ~ %d]", iStartIndex + 1, iStartIndex + m_iMaxShuffleRoomSize );
}

void ShuffleRoomManager::ReleaseMemoryPool()
{
	vShuffleRoomNode_iter iter, iEnd;
	iEnd = m_vShuffleRoomNode.end();
	for( iter=m_vShuffleRoomNode.begin(); iter!=iEnd; ++iter )
	{
		ShuffleRoomNode *pNode = *iter;
		pNode->OnDestroy();
		m_MemNode.Push( pNode );
	}
	m_vShuffleRoomNode.clear();
	m_vShuffleRoomCopyNode.clear();
	m_MemNode.DestroyPool();
}

ShuffleRoomNode *ShuffleRoomManager::CreateNewShuffleRoom()
{
	ShuffleRoomNode* pNewNode = ( ShuffleRoomNode* )m_MemNode.Remove();
	if( !pNewNode )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"ShuffleRoomManager::CreateNewShuffleRoom MemPool Zero!");
		return NULL;
	}

	m_vShuffleRoomNode.push_back( pNewNode );
	pNewNode->OnCreate();

	return pNewNode;
}

void ShuffleRoomManager::RemoveShuffleRoom( ShuffleRoomNode *pNode )
{	
	vShuffleRoomNode_iter iter, iEnd;
	iEnd = m_vShuffleRoomNode.end();
	for( iter=m_vShuffleRoomNode.begin(); iter!=iEnd; ++iter )
	{
		ShuffleRoomNode *pCursor = *iter;
		if( pCursor == pNode )
		{
			pCursor->OnDestroy();
			m_MemNode.Push( pCursor );
			m_vShuffleRoomNode.erase( iter );
			break;
		}
	}
}

ShuffleRoomNode *ShuffleRoomManager::GetShuffleRoomNode( DWORD dwIndex )
{
	vShuffleRoomNode_iter iter, iEnd;
	iEnd = m_vShuffleRoomNode.end();
	for( iter=m_vShuffleRoomNode.begin(); iter!=iEnd; ++iter )
	{
		ShuffleRoomNode *pCursor = *iter;
		if( pCursor->GetIndex() == dwIndex )
		{
			return pCursor;
		}
	}

	return NULL;
}

ShuffleRoomNode *ShuffleRoomManager::GetShuffleRoomNodeArray( int iArray )
{
	if( (int)m_vShuffleRoomNode.size() <= iArray )
		return NULL;

	return m_vShuffleRoomNode[iArray];
}

ShuffleRoomCopyNode *ShuffleRoomManager::GetShuffleRoomCopyNodeArray( int iArray )
{
	if( (int)m_vShuffleRoomCopyNode.size() <= iArray )
		return NULL;

	return m_vShuffleRoomCopyNode[iArray];
}

void ShuffleRoomManager::AddCopyShuffleRoom( ShuffleRoomCopyNode *pShuffleRoom )
{
	vShuffleRoomCopyNode_iter iter,iEnd;
	iEnd = m_vShuffleRoomCopyNode.end();
	for( iter=m_vShuffleRoomCopyNode.begin() ; iter!=iEnd ; ++iter )
	{
		ShuffleRoomCopyNode *pCursor = *iter;
		if( pCursor == pShuffleRoom )
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ShuffleRoomManager::AddCopyShuffleRoom 이미 리스트에 있는 복사본임(%d)", pCursor->GetIndex() );
			return;
		}
	}
	m_vShuffleRoomCopyNode.push_back( pShuffleRoom );
}

void ShuffleRoomManager::RemoveShuffleCopyRoom( ShuffleRoomCopyNode *pShuffleRoom )
{
	vShuffleRoomCopyNode_iter iter,iEnd;
	iEnd = m_vShuffleRoomCopyNode.end();
	for( iter=m_vShuffleRoomCopyNode.begin() ; iter!=iEnd ; ++iter )
	{
		ShuffleRoomCopyNode *pCursor = *iter;

		if( pCursor == pShuffleRoom )
		{
			m_vShuffleRoomCopyNode.erase( iter );
			return;
		}
	}
}

void ShuffleRoomManager::RemoveUserCopyNode( DWORD dwUserIndex, const ioHashString &rkName )
{
	vShuffleRoomNode_iter iter, iEnd;
	iEnd = m_vShuffleRoomNode.end();
	for( iter = m_vShuffleRoomNode.begin(); iter!=iEnd; ++iter )
	{
		ShuffleRoomNode *pCursor = *iter;
		if( pCursor->LeaveUser( dwUserIndex, rkName ) )
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ShuffleRoomManager::RemoveUserCopyNode : Leave User (%d) - %s(%d)", pCursor->GetIndex(), rkName.c_str(), dwUserIndex );
			return;
		}
	}
}

void ShuffleRoomManager::ConnectServerNodeSync( ServerNode *pServerNode )
{
	if( pServerNode == NULL ) return;

	static vShuffleRoomNode vShuffleRoom;
	vShuffleRoom.clear();

	LOOP_GUARD();
	vShuffleRoomNode_iter iter = m_vShuffleRoomNode.begin();
	while( iter != m_vShuffleRoomNode.end() )
	{
		ShuffleRoomNode *pNode = *iter++;
		vShuffleRoom.push_back( pNode );		
	}
	LOOP_GUARD_CLEAR();

	// 오리지날 전투룸 정보만 N개씩 끊어서 전송
	LOOP_GUARD();
	while( true )
	{
		int iMaxSize = min( SSTPK_CONNECT_SYNC_SHUFFLEROOM_MAX, (int)vShuffleRoom.size() );
		if( iMaxSize == 0 )
			break;

		SP2Packet kPacket( SSTPK_CONNECT_SYNC );
		kPacket << SSTPK_CONNECT_SYNC_SHUFFLEROOM << iMaxSize;
		for(int i = 0;i < iMaxSize;i++)
		{
			ShuffleRoomNode *pNode  = vShuffleRoom[0];
			pNode->FillSyncCreate( kPacket );
			vShuffleRoom.erase( vShuffleRoom.begin() );
		}
		pServerNode->SendMessage( kPacket );
	}
	LOOP_GUARD_CLEAR();
}

ShuffleRoomParent* ShuffleRoomManager::GetGlobalShuffleRoomNode( DWORD dwIndex )
{
	ShuffleRoomParent *pReturn = (ShuffleRoomParent*)GetShuffleRoomNode( dwIndex );
	if( pReturn )
		return pReturn;

	vShuffleRoomCopyNode_iter iter,iEnd;
	iEnd = m_vShuffleRoomCopyNode.end();
	for( iter=m_vShuffleRoomCopyNode.begin() ; iter!=iEnd ; ++iter )
	{
		ShuffleRoomCopyNode *pCursor = *iter;
		if( pCursor->GetIndex() == dwIndex )
		{
			return pCursor;
		}
	}
	return NULL;
}

void ShuffleRoomManager::LoadINI()
{
	ioINILoader kLoader( "config/sp2_shuffleroom.ini" );
	kLoader.SetTitle( "common" );
	m_nModeMaxCount = kLoader.LoadInt( "max_mode", 1 );

	kLoader.SetTitle( "ReserveUser" );
	m_iMaxPlayer = kLoader.LoadInt( "max_player", 2 );

	kLoader.SetTitle( "playbouns" );
	m_fWinBonus						= kLoader.LoadFloat( "win_bonus", 1.0f );
	m_fLoseBonus					= kLoader.LoadFloat( "lose_bonus", 0.5f );
	m_fWinningStreakBonus			= kLoader.LoadFloat( "winning_streak_bonus", 0.5f );
	m_fWinningStreakMax				= kLoader.LoadFloat( "winning_streak_max", 5.0f );
	m_fModeConsecutivelyBonus		= kLoader.LoadFloat( "consecutive_play_bonus", 0.05f );
	m_fModeConsecutivelyMaxBonus	= kLoader.LoadFloat( "consecutive_play_max", 0.50f );
		
	kLoader.SetTitle( "ModeList" );
	char szBuf[MAX_PATH] = "", szKey[MAX_PATH] = "";
	
	int iModeCount = kLoader.LoadInt( "mode_cnt", 1 );
	for( int i = 0; i < iModeCount; ++i )
	{
		ShuffleModeTypeInfo kInfo;

		wsprintf( szKey, "mode%d_type", i+1 );
		kInfo.m_iModeType = kLoader.LoadInt( szKey, -1 );

		wsprintf( szKey, "mode%d_title", i+1 );
		kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_ModeTitle = szBuf;

		wsprintf( szKey, "mode%d_sub_cnt", i+1 );
		int iSubCnt = kLoader.LoadInt( szKey, 0 );

		for( int j=0; j<iSubCnt; ++j )
		{
			ShuffleModeSubTypeInfo kSubInfo;
			wsprintf( szKey, "mode%d_sub%d_type", i+1, j+1 );
			kSubInfo.m_iSubType = kLoader.LoadInt( szKey, 0 );

			wsprintf( szKey, "mode%d_sub%d_title", i+1, j+1 );
			kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
			kSubInfo.m_SubTitle = szBuf;

			wsprintf( szKey, "mode%d_sub%d_map_cnt", i+1, j+1 );
			int iMapCnt = kLoader.LoadInt( szKey, 0 );
			for( int k=0; k<iMapCnt; ++k )
			{
				wsprintf( szKey, "mode%d_sub%d_map%d", i+1, j+1, k+1 );
				int iMapIndex = kLoader.LoadInt( szKey, 0 );
				kSubInfo.m_vMapList.push_back( iMapIndex );
			}

			kInfo.m_vSubTypeInfo.push_back( kSubInfo );
		}

		m_vShuffleModeList.push_back( kInfo );
	}

	m_ShuffleBonusMode.Init();
	m_ShuffleBonusMode.m_iModeType = kLoader.LoadInt( "bonus_mode_type", -1 );

	kLoader.LoadString( "bonus_mode_title", "", szBuf, MAX_PATH );
	m_ShuffleBonusMode.m_ModeTitle = szBuf;

	int iSubCnt = kLoader.LoadInt( "bonus_mode_sub_cnt", 0 );
	for( int i=0; i<iSubCnt; ++i )
	{
		ShuffleModeSubTypeInfo kSubInfo;
		wsprintf( szKey, "bonus_mode_sub%d_type", i+1 );
		kSubInfo.m_iSubType = kLoader.LoadInt( szKey, 0 );

		wsprintf( szKey, "bonus_mode_sub%d_title", i+1 );
		kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kSubInfo.m_SubTitle = szBuf;

		wsprintf( szKey, "bonus_mode_sub%d_map_cnt", i+1 );
		int iMapCnt = kLoader.LoadInt( szKey, 0 );
		for( int j=0; j<iMapCnt; ++j )
		{
			wsprintf( szKey, "bonus_mode_sub%d_map%d", i+1, j+1 );
			int iMapIndex = kLoader.LoadInt( szKey, 0 );
			kSubInfo.m_vMapList.push_back( iMapIndex );
		}

		m_ShuffleBonusMode.m_vSubTypeInfo.push_back( kSubInfo );
	}

	kLoader.SetTitle( "BonusPoint" );

	m_dwShuffleBonusPointTime = (DWORD)kLoader.LoadInt( "bonus_point_time", 10000 );

	m_vShufflePoint.clear();
	int iCnt = kLoader.LoadInt( "phase_cnt", 0 );
	for( int i = 0; i < iCnt; ++i )
	{
		wsprintf( szKey, "phase%d_point", i+1 );
		m_vShufflePoint.push_back( kLoader.LoadInt( szKey, 0 ) );
	}

	//
	kLoader.SetTitle( "correction_var" );
	int iSize = kLoader.LoadInt( "time_correct_count", 0 );
	for( int i=0; i<iSize; ++i )
	{
		TimeCorrectionVar kInfo;
		wsprintf( szKey, "time_correct%d_min", i+1 );
		kInfo.m_dwMinTime = (DWORD)kLoader.LoadInt( szKey, 0 );

		wsprintf( szKey, "time_correct%d_max", i+1 );
		kInfo.m_dwMaxTime = (DWORD)kLoader.LoadInt( szKey, 0 );

		wsprintf( szKey, "time_correct%d_star", i+1 );
		kInfo.m_iStarCnt = kLoader.LoadInt( szKey, 0 );

		wsprintf( szKey, "time_correct%d_pt", i+1 );
		kInfo.m_iPoint = kLoader.LoadInt( szKey, 0 );

		m_vTimeCorrection.push_back( kInfo );
	}

	//
	iSize = kLoader.LoadInt( "user_correct_count", 0 );
	for( int i=0; i<iSize; ++i )
	{
		UserCorrectionVar kInfo;
		wsprintf( szKey, "user_correct%d_cnt", i+1 );
		kInfo.m_iUserCnt = kLoader.LoadInt( szKey, 0 );

		wsprintf( szKey, "user_correct%d_star", i+1 );
		kInfo.m_iStarCnt = kLoader.LoadInt( szKey, 0 );

		wsprintf( szKey, "user_correct%d_pt", i+1 );
		kInfo.m_iPoint = kLoader.LoadInt( szKey, 0 );

		m_vUserCorrection.push_back( kInfo );
	}

	//
	kLoader.SetTitle( "correction_table" );
	iSize = kLoader.LoadInt( "correct_table_count", 0 );
	for( int i=0; i<iSize; ++i )
	{
		CorrectionTable kInfo;
		wsprintf( szKey, "correct_table%d_min", i+1 );
		kInfo.m_iMinCorrectionPt = kLoader.LoadInt( szKey, 0 );

		wsprintf( szKey, "correct_table%d_max", i+1 );
		kInfo.m_iMaxCorrectionPt = kLoader.LoadInt( szKey, 0 );

		wsprintf( szKey, "correct_table%d_phase", i+1 );
		kInfo.m_iStarPhase = kLoader.LoadInt( szKey, 0 );

		m_vCorrectionTable.push_back( kInfo );
	}

	kLoader.SetTitle( "matching_table" );
	iSize = kLoader.LoadInt( "match_correction_user_cnt", 0 );
	for( int i=0; i<iSize; ++i )
	{
		MatchCorrectionUser kInfo;
		wsprintf( szKey, "match_correction_user%d", i+1 );
		kInfo.m_iUserCount = kLoader.LoadInt( szKey, 0 );

		wsprintf( szKey, "match_correction_user%d_pt", i+1 );
		kInfo.m_iPoint = kLoader.LoadInt( szKey, 0 );

		m_vMatchCorrectionUser.push_back( kInfo );
	}

	iSize = kLoader.LoadInt( "match_correction_time_cnt", 0 );
	for( int i=0; i<iSize; ++i )
	{
		MatchCorrectionTime kInfo;
		wsprintf( szKey, "match_correction_time%d_min", i+1 );
		kInfo.m_dwMinTime = (DWORD)kLoader.LoadInt( szKey, 0 );

		wsprintf( szKey, "match_correction_time%d_max", i+1 );
		kInfo.m_dwMaxTime = (DWORD)kLoader.LoadInt( szKey, 0 );

		wsprintf( szKey, "match_correction_time%d_pt", i+1 );
		kInfo.m_iPoint = kLoader.LoadInt( szKey, 0 );

		m_vMatchCorrectionTime.push_back( kInfo );
	}

	for( int i = 0; i < GetMaxPhase(); ++i )
	{
		wsprintf( szKey, "match_correction_phase%d", i+1 );
		m_vMatchCorrectionPhase.push_back( kLoader.LoadInt( szKey, 0 ) );
	}

	m_iMatchCheckCount = kLoader.LoadInt( "match_minmax_level_cnt", 0 );
	for( int i=0; i<m_iMatchCheckCount; ++i )
	{
		MatchConditionLevel Level;
		wsprintf( szKey, "match%d_user_min_level", i+1 );		
		Level.m_iUserMinLevel = kLoader.LoadInt( szKey, 0 );

		wsprintf( szKey, "match%d_user_max_level", i+1 );
		Level.m_iUserMaxLevel = kLoader.LoadInt( szKey, 0 );

		wsprintf( szKey, "match%d_condition_count", i+1 );
		int iCount = kLoader.LoadInt( szKey, 0 );

		for( int j = 0; j < iCount; ++j )
		{
			MatchConditionValue MatchCondition;

			wsprintf( szKey, "match%d_condition%d_min", i+1, j+1 );
			MatchCondition.m_iMatchMinLevel = kLoader.LoadInt( szKey, 0 );

			wsprintf( szKey, "match%d_condition%d_max", i+1, j+1 );
			MatchCondition.m_iMatchMaxLevel = kLoader.LoadInt( szKey, 0 );

			Level.m_vMatchConditionValue.push_back( MatchCondition );
		}

		m_vMatchConditionLevel.push_back( Level );
	}

	kLoader.SetTitle( "kick_out_table" );
	int iKickCount = kLoader.LoadInt( "kickout_count", 0 );
	for( int i = 0; i < iKickCount; ++i )
	{
		KickOutCondition Kick;
		sprintf_s( szKey, "kickout%d_user_min_level", i+1 );
		Kick.m_iUserLevelMin = kLoader.LoadInt( szKey, 0 );

		sprintf_s( szKey, "kickout%d_user_max_level", i+1 );
		Kick.m_iUserLevelMax = kLoader.LoadInt( szKey, 0 );

		sprintf_s( szKey, "kickout%d_min_level", i+1 );
		Kick.m_iKickOutMinLevel = kLoader.LoadInt( szKey, 0 );

		sprintf_s( szKey, "kickout%d_max_level", i+1 );
		Kick.m_iKickOutMaxLevel = kLoader.LoadInt( szKey, 0 );

		m_vKickOutCondition.push_back( Kick );
	}
	m_dwKickVoteEnableTime = kLoader.LoadInt( "kick_out_enable_time", 0 );

	kLoader.SetTitle( "raward_item" );
	m_iRewardType = kLoader.LoadInt( "reward_item_type", 0 );
	m_iRewardPeriod = kLoader.LoadInt( "reward_item_period", 0 );
	m_iRewardIndex = kLoader.LoadInt( "reward_item_index", 0 );

	kLoader.SetTitle( "TodayMode");
	for( int i = 0; i < 7; ++i )
	{
		for( int j = 0; j < GetMaxModeCount(); ++j )
		{
			wsprintf( szKey, "todaymode%d_mode%d", i+1, j+1 );
			m_TodayModeInfo[i].push_back( kLoader.LoadInt( szKey, 1 ) );
		}
	}
}

int ShuffleRoomManager::GetMaxPhase()
{
	//보너스 모드
	ShuffleRoomInfo kInfo;
	GetShuffleBonusModeSubInfo( kInfo );

	if( kInfo.m_iModeIdx == -1 )
		return GetMaxModeCount();

	//보너스 모드 포함
	return GetMaxModeCount() + 1;
}

int ShuffleRoomManager::GetShuffleRoomUserCount()
{
	int iCount = 0;
	vShuffleRoomNode_iter iter, iEnd;
	iEnd = m_vShuffleRoomNode.end();
	for( iter=m_vShuffleRoomNode.begin() ;iter!=iEnd; ++iter )
	{
		ShuffleRoomNode *pNode = *iter;
		iCount += pNode->GetJoinUserCnt();
	}
	return iCount;
}

int ShuffleRoomManager::GetShuffleRoomPlayUserCount()
{
	int iCount = 0;
	vShuffleRoomNode_iter iter, iEnd;
	iEnd = m_vShuffleRoomNode.end();
	for( iter=m_vShuffleRoomNode.begin(); iter!=iEnd; ++iter )
	{
		ShuffleRoomNode *pNode = *iter;
		if( pNode->IsShuffleModePlaying() )
			iCount += pNode->GetJoinUserCnt();
	}
	return iCount;
}

int ShuffleRoomManager::GetSortShuffleRoomPoint( SortShuffleRoom &rkSortRoom, int iKillDeathLevel )
{
	if( rkSortRoom.m_pNode == NULL )
		return 0;

	int iReturnPoint = 0;

	// 인원 가중치
	iReturnPoint += GetMatchCorrectionUserCnt( rkSortRoom.m_pNode->GetPlayUserCnt() );
	
	// 페이즈 가중치
	iReturnPoint += GetMatchCorrectionPhase( rkSortRoom.m_pNode->GetPhase() );

	// 시간 가중치
	DWORD dwCheckTime = TIMEGETTIME() - rkSortRoom.m_pNode->GetCreateTime();
	iReturnPoint += GetMatchCorrectionTime( dwCheckTime );

	return iReturnPoint;
}

ShuffleRoomParent *ShuffleRoomManager::GetJoinShuffleRoomNode( int iAverageLevel, int iCheckCount /* = 0 */ )
{
	vShuffleRoomParent vShuffleRoomList;

	if( !IsBlocked() )
	{
		vShuffleRoomNode_iter iter, iEnd;
		iEnd = m_vShuffleRoomNode.end();
		for( iter=m_vShuffleRoomNode.begin() ; iter!=iEnd ; ++iter )
		{
			ShuffleRoomNode *pNode = *iter;
			if( !pNode )
				continue;

			if( !pNode->IsShuffleRoom() )
				continue;

			if( pNode->GetPlayModeType() == MT_SHUFFLE_BONUS )
				continue;

			vShuffleRoomList.push_back( (ShuffleRoomParent*)pNode );
		}
	}

	vShuffleRoomCopyNode_iter iterCopy, iEndCopy;
	iEndCopy = m_vShuffleRoomCopyNode.end();
	for( iterCopy =m_vShuffleRoomCopyNode.begin() ; iterCopy!=iEndCopy ; ++iterCopy )
	{
		ShuffleRoomCopyNode *pNode = *iterCopy;

		if( !pNode )
			continue;

		if( pNode->IsBlocked() )
			continue;

		if( pNode->GetPlayModeType() == MT_SHUFFLE_BONUS )
			continue;

		vShuffleRoomList.push_back( (ShuffleRoomParent*)pNode );
	}

	static vSortShuffleRoom vSortList;	
	vSortList.clear();

	vShuffleRoomParent_iter iterParent, iEndParent;
	iEndParent = vShuffleRoomList.end();
	for(iterParent = vShuffleRoomList.begin();iterParent != iEndParent;++iterParent)
	{
		ShuffleRoomParent *pNode = *iterParent;
		if( pNode == NULL )
			continue;

		if( pNode->IsEmptyShuffleRoom() )
			continue;

		if( pNode->IsFull() )
			continue;

		if( !CheckKillDeathLevel( iAverageLevel, pNode->GetAbilityMatchLevel(), iCheckCount ) )
			continue;

		SortShuffleRoom ssr;
		ssr.m_pNode   = pNode;
		ssr.m_iPoint  = GetSortShuffleRoomPoint( ssr, iAverageLevel );
		vSortList.push_back( ssr );
	}

	int iMaxList = vSortList.size();
	if( iMaxList == 0 ) return NULL;

	std::sort( vSortList.begin(), vSortList.end(), ShuffleRoomSort() );

	SortShuffleRoom &kNode = vSortList[0];
	ShuffleRoomParent *pReturn = kNode.m_pNode;
	vShuffleRoomList.clear();
	return pReturn;	
}

bool ShuffleRoomManager::CheckKillDeathLevel( int iRoomLevel, int iUserLevel, int iCheckCount /* = 0 */ )
{
	// 방이 적으면 수준차 적용하지 않음
	if( GetNodeSize() + GetCopyNodeSize() < g_LevelMatchMgr.GetPartyLevelCheckMinRoom() )
		return true;

	// 셋팅이 없으면 수준차 적용하지 않음
	if( m_vMatchConditionLevel.empty() )
		return true;

	for( vMatchConditionLevel::iterator iter = m_vMatchConditionLevel.begin(); iter != m_vMatchConditionLevel.end(); ++iter )
	{
		const MatchConditionLevel& rkLevel = *iter;
		if( COMPARE( iRoomLevel, rkLevel.m_iUserMinLevel, rkLevel.m_iUserMaxLevel + 1 ) )
		{
			iCheckCount = max( iCheckCount, 0 );
			iCheckCount = min( iCheckCount, (int)rkLevel.m_vMatchConditionValue.size() - 1 );

			const MatchConditionValue& rkValue = rkLevel.m_vMatchConditionValue[iCheckCount];

			int iMin = iRoomLevel - rkValue.m_iMatchMinLevel;
			iMin = max( iMin, 0 );

			int iMax = iRoomLevel + rkValue.m_iMatchMaxLevel;
			iMax = min( iMax, g_LevelMatchMgr.GetRoomEnterLevelMax() );

			return COMPARE( iUserLevel, iMin, iMax );
		}
	}

	return false;
}

int ShuffleRoomManager::GetMatchCheckMaxCount( int iUserLevel )
{ 
	for( vMatchConditionLevel::iterator iter = m_vMatchConditionLevel.begin(); iter != m_vMatchConditionLevel.end(); ++iter )
	{
		const MatchConditionLevel& rkLevel = *iter;
		if( COMPARE( iUserLevel, rkLevel.m_iUserMinLevel, rkLevel.m_iUserMaxLevel + 1 ) )
		{
			return static_cast<int>( rkLevel.m_vMatchConditionValue.size() );
		}
	}

	return 0;
}

bool ShuffleRoomManager::IsKickOutMaxLevel( int iRoomLevel, int iUserLevel )
{
	if( m_vKickOutCondition.empty() )
		return false;
			
	for( vKickOutCondition::iterator iter = m_vKickOutCondition.begin(); iter != m_vKickOutCondition.end(); ++iter )
	{
		const KickOutCondition& rkKick = *iter;

		if( COMPARE( iUserLevel, rkKick.m_iUserLevelMin, rkKick.m_iUserLevelMax + 1 ) )
		{
			if( rkKick.m_iKickOutMaxLevel == 0 )
			{
				return false;
			}
			else if( iRoomLevel +  rkKick.m_iKickOutMaxLevel <= iUserLevel )
			{
				return true;
			}
			else
			{
				return false;
			}
		}
	}

	return false;
}

bool ShuffleRoomManager::IsKickOutMinLevel( int iRoomLevel, int iUserLevel )
{
	if( m_vKickOutCondition.empty() )
	{
		return false;
	}

	for( vKickOutCondition::iterator iter = m_vKickOutCondition.begin(); iter != m_vKickOutCondition.end(); ++iter )
	{
		const KickOutCondition& rkKick = *iter;

		if( COMPARE( iUserLevel, rkKick.m_iUserLevelMin, rkKick.m_iUserLevelMax + 1 ) )
		{
			if( rkKick.m_iKickOutMinLevel == 0 )
			{
				return false;
			}
			else if( iRoomLevel - rkKick.m_iKickOutMinLevel >= iUserLevel )
			{
				return true;
			}
			else
			{
				return false;
			}
		}
	}

	return false;
}

void ShuffleRoomManager::UpdateProcess()
{
	if( TIMEGETTIME() - m_dwCurTime < 1000 ) return;
	m_dwCurTime = TIMEGETTIME();

	FUNCTION_TIME_CHECKER( 100000.0f, 0 );          // 0.1 초 이상 걸리면로그 남김

	static vShuffleRoomNode vDestroyNode;
	vDestroyNode.clear();

	vShuffleRoomNode_iter iter, iEnd;
	iEnd = m_vShuffleRoomNode.end();
	for( iter=m_vShuffleRoomNode.begin(); iter!=iEnd; ++iter )
	{
		ShuffleRoomNode *pNode = *iter;
		
		if( pNode->IsReserveTimeOver() )
			vDestroyNode.push_back( pNode );
		else
			pNode->Process();
	}

	if( !vDestroyNode.empty() )
	{
		iEnd = vDestroyNode.end();
		for( iter=vDestroyNode.begin(); iter!=iEnd; ++iter )
		{
			RemoveShuffleRoom( *iter );
		}
		vDestroyNode.clear();
	}
}

void ShuffleRoomManager::GetShuffleModeList( vShuffleRoomInfo &rkModeIndexList )
{
	CTime CurTime = CTime::GetCurrentTime();
	int iWeek = CurTime.GetDayOfWeek() - 1;
	iWeek = max( iWeek , 0 );
	iWeek = min( iWeek , 6 );

	rkModeIndexList.clear();
	for( int i=0; i < GetMaxModeCount(); ++i )
	{
		if( m_TodayModeInfo[iWeek].empty() )
		{
			LOG.PrintTimeAndLog(0, "%s - today mode setting error!!", __FUNCTION__ );
			return;
		}

		int iModeIdx = m_TodayModeInfo[iWeek][i];
		int iSubIdx, iMapIdx;
		GetShuffleModeSubInfo( iModeIdx, iSubIdx, iMapIdx );
		ShuffleRoomInfo kInfo( iModeIdx, iSubIdx, iMapIdx );

		rkModeIndexList.push_back( kInfo );
	}
}

void ShuffleRoomManager::GetShuffleModeSubInfo( IN int iModeType, OUT int &iSubIdx, OUT int &iMapIdx )
{
	IORandom Rand;
	Rand.SetRandomSeed( timeGetTime() );

	int iSize = m_vShuffleModeList.size();
	for( int i=0; i<iSize; ++i )
	{
		ShuffleModeTypeInfo kInfo = m_vShuffleModeList[i];
		if( kInfo.m_iModeType == iModeType )
		{
			if( !kInfo.IsSubTypeEmpty() )
			{
				int iSubTypeSize = kInfo.GetSubTypeSize();
				
				Rand.Randomize();
				int iSubNum = Rand.Random( iSubTypeSize );
				iSubIdx = kInfo.m_vSubTypeInfo[iSubNum].m_iSubType;

				if( !kInfo.m_vSubTypeInfo[iSubNum].IsMapEmpty() )
				{
					int iMapSize = kInfo.m_vSubTypeInfo[iSubNum].GetMapSize();

					Rand.Randomize();
					int iMapNum = Rand.Random( iMapSize );
					iMapIdx = kInfo.m_vSubTypeInfo[iSubNum].m_vMapList[iMapNum];
					return;
				}
				else
				{
					iMapIdx = 1;
				}
			}
			else
			{
				iSubIdx = 1;
				iMapIdx = 1;
			}
		}
		else
		{
			iSubIdx = 1;
			iMapIdx = 1;
		}
	}
}

void ShuffleRoomManager::GetShuffleBonusModeSubInfo( ShuffleRoomInfo &rkModeIndexList )
{
	int iModeIdx = -1, iSubIdx = -1, iMapIdx = -1;
	iModeIdx = m_ShuffleBonusMode.m_iModeType;

	int iSubTypeSize = m_ShuffleBonusMode.GetSubTypeSize();
	IORandom Rand;
	Rand.SetRandomSeed( timeGetTime() );

	if( !m_ShuffleBonusMode.IsSubTypeEmpty() )
	{
		Rand.Randomize();
		int iSubNum = Rand.Random( iSubTypeSize );
		iSubIdx = m_ShuffleBonusMode.m_vSubTypeInfo[iSubNum].m_iSubType;

		if( !m_ShuffleBonusMode.m_vSubTypeInfo[iSubNum].IsMapEmpty() )
		{
			int iMapSize = m_ShuffleBonusMode.m_vSubTypeInfo[iSubNum].GetMapSize();
			Rand.Randomize();
			int iMapNum = Rand.Random( iMapSize );
			iMapIdx = m_ShuffleBonusMode.m_vSubTypeInfo[iSubNum].m_vMapList[iMapNum];
		}
		else
		{
			iMapIdx = 1;
		}

		rkModeIndexList = ShuffleRoomInfo( iModeIdx, iSubIdx, iMapIdx );
	}
	else
	{
		iSubIdx = 1;
		iMapIdx = 1;
	}
}

int ShuffleRoomManager::GetShufflePoint( int iPhase )
{
	if( COMPARE( iPhase, 0, (int)m_vShufflePoint.size() ) )
		return max( 1, m_vShufflePoint[iPhase] );
	
	return 1;
}

int ShuffleRoomManager::GetTimeCorrectionStarCnt( DWORD dwTime )
{
	vTimeCorrectionVar_iter iter = m_vTimeCorrection.begin();
	vTimeCorrectionVar_iter iEnd = m_vTimeCorrection.end();
	for( ; iter !=iEnd; ++iter )
	{
		int iStarCnt = (*iter).GetStarCnt( dwTime );
		if( iStarCnt > 0 )
			return iStarCnt;
	}

	return 0;
}

int ShuffleRoomManager::GetTimeCorrectionValue( DWORD dwTime )
{
	vTimeCorrectionVar_iter iter = m_vTimeCorrection.begin();
	vTimeCorrectionVar_iter iEnd = m_vTimeCorrection.end();
	for( ; iter !=iEnd; ++iter )
	{
		int iPoint = (*iter).GetPoint( dwTime );
		if( iPoint > 0 )
			return iPoint;
	}

	return 0;
}

int ShuffleRoomManager::GetUserCorrectionStarCnt( int iUserCnt )
{
	vUserCorrectionVar_iter iter = m_vUserCorrection.begin();
	vUserCorrectionVar_iter iEnd = m_vUserCorrection.end();
	for( ; iter !=iEnd; ++iter )
	{
		int iStarCnt = (*iter).GetStarCnt( iUserCnt );
		if( iStarCnt > 0 )
			return iStarCnt;
	}

	return 0;
}

int ShuffleRoomManager::GetUserCorrectionValue( int iUserCnt )
{
	vUserCorrectionVar_iter iter = m_vUserCorrection.begin();
	vUserCorrectionVar_iter iEnd = m_vUserCorrection.end();
	for( ; iter !=iEnd; ++iter )
	{
		int iPoint = (*iter).GetPoint( iUserCnt );
		if( iPoint > 0 )
			return iPoint;
	}

	return 0;
}

int ShuffleRoomManager::GetStarPhaseByCorrection( int iCorrectionPt )
{
	vCorrectionTable_iter iter = m_vCorrectionTable.begin();
	vCorrectionTable_iter iEnd = m_vCorrectionTable.end();
	int iSize = m_vCorrectionTable.size();
	for( ; iter !=iEnd; ++iter )
	{
		int iPhase = (*iter).GetPhase( iCorrectionPt );
		if( iPhase > 0 )
			return iPhase;
	}

	return 0;
}

int ShuffleRoomManager::GetMatchCorrectionUserCnt( int iUserCnt )
{
	vMatchCorrectionUser_iter iter = m_vMatchCorrectionUser.begin();
	vMatchCorrectionUser_iter iEnd = m_vMatchCorrectionUser.end();
	for( ; iter !=iEnd; ++iter )
	{
		int iPoint = (*iter).GetPoint( iUserCnt );
		if( iPoint > 0 )
			return iPoint;
	}

	return 0;
}

int ShuffleRoomManager::GetMatchCorrectionTime( DWORD dwTime )
{
	vMatchCorrectionTime_iter iter = m_vMatchCorrectionTime.begin();
	vMatchCorrectionTime_iter iEnd = m_vMatchCorrectionTime.end();
	for( ; iter !=iEnd; ++iter )
	{
		int iPoint = (*iter).GetPoint( dwTime );
		if( iPoint > 0 )
			return iPoint;
	}

	return 0;
}

int ShuffleRoomManager::GetMatchCorrectionPhase( int iPhase )
{
	if( !COMPARE( iPhase, 0, GetMaxPhase() ) )
		return 0;

	if( !COMPARE( iPhase, 0, static_cast<int>( m_vMatchCorrectionPhase.size() ) ) )
		return 0;

	return m_vMatchCorrectionPhase[iPhase];
}

float ShuffleRoomManager::GetModeConsecutivelyBonus( User* pUser )
{
	float fCount = 0.0f;
	if( pUser )
		fCount = static_cast<float>( pUser->GetModeConsecutivelyCnt() );

	float fBonus = fCount * m_fModeConsecutivelyBonus;	
	return min( fBonus, m_fModeConsecutivelyMaxBonus );
}