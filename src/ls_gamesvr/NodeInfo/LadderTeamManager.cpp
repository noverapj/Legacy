#include "stdafx.h"

#include "../EtcHelpFunc.h"
//#include "../Window.h"
#include "../MainProcess.h"

#include "User.h"
#include "LadderTeamManager.h"
#include "ServerNodeManager.h"

LadderTeamManager* LadderTeamManager::sg_Instance = NULL;
LadderTeamManager::LadderTeamManager()
{
	m_dwCurTime = 0;
	m_iMaxLadderTeamSize = 0;
	m_iLadderBattleWinPoint  = 3;
	m_iLadderBattleDrawPoint = 1;
	m_iLadderBattleLosePoint = 1;
	m_dwSearchMatchFullTime  = 13000;
	m_dwLastMatchDelayTime   = 20000;
	m_iSearchMatchSendSec    = 3;
	m_fSeasonEndDecreaseRate = 1.0f;
	m_bCampBattlePlay        = false;
	m_iAverageHeroMatchPoint = 640;
	InitTotalModeList();
}

LadderTeamManager::~LadderTeamManager()
{
}

LadderTeamManager &LadderTeamManager::GetInstance()
{
	if( sg_Instance == NULL )
		sg_Instance = new LadderTeamManager;
	return *sg_Instance;
}

void LadderTeamManager::ReleaseInstance()
{
	SAFEDELETE( sg_Instance );
}

void LadderTeamManager::InitMemoryPool( const DWORD dwServerIndex )
{
	{
		ioINILoader kLoader( "ls_config_game.ini" );
		kLoader.SetTitle( "MemoryPool" );
		m_iMaxLadderTeamSize = kLoader.LoadInt( "ladderteam_pool", 3000 );
	}
	int iStartIndex      = dwServerIndex * m_iMaxLadderTeamSize;

	int i = 0;

	m_MemNode.CreatePool( 0, m_iMaxLadderTeamSize, FALSE );
	for(i = 1;i <= m_iMaxLadderTeamSize;i++)
	{
		m_MemNode.Push( new LadderTeamNode( i + iStartIndex ) );
	}
	
	ioINILoader kLoader1( "config/sp2_ladderteam.ini" );
	kLoader1.SetTitle( "info" );
	m_dwSearchMatchFullTime  = kLoader1.LoadInt( "Search_Full_Time", 13000 );
	m_iSearchMatchSendSec    = kLoader1.LoadInt( "Search_Send_Sec", 3 );
	m_dwLastMatchDelayTime   = kLoader1.LoadInt( "LastMatchDelayTime", 20000 );

	kLoader1.SetTitle( "team_rank" );
	m_iLadderBattleWinPoint  = kLoader1.LoadInt( "Win_Point", 3 );
	m_iLadderBattleDrawPoint = kLoader1.LoadInt( "Draw_Point", 1 );
	m_iLadderBattleLosePoint = kLoader1.LoadInt( "Lose_Point", 1 );

	kLoader1.SetTitle( "hero_match" );
	m_fSeasonEndDecreaseRate = kLoader1.LoadFloat( "SeasonEndDecreaseRate", 1.0f );

	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[info][ladder]Init memory pool : [%d ~ %d]", iStartIndex + 1, iStartIndex + m_iMaxLadderTeamSize );
}

void LadderTeamManager::ReleaseMemoryPool()
{
	/*vLadderTeamNode_iter iter, iEnd;
	iEnd = m_vLadderTeamNode.end();
	for(iter = m_vLadderTeamNode.begin();iter != iEnd;++iter)
	{
		LadderTeamNode *pNode = *iter;
		pNode->OnDestroy();
		m_MemNode.Push( pNode );
	}*/

	mLadderTeamNode_iter it  = m_mLadderTeamNode.begin();

	for( ; it != m_mLadderTeamNode.end(); it++ )
	{
		LadderTeamNode *pNode = it->second;
		if( !pNode ) continue;

		pNode->OnDestroy();
		m_MemNode.Push( pNode );
	}

	m_mLadderTeamNode.clear();
	m_mLadderTeamCopyNode.clear();
	m_MemNode.DestroyPool();
}

LadderTeamNode *LadderTeamManager::CreateNewLadderTeam()
{
	LadderTeamNode *pNewNode = (LadderTeamNode *)m_MemNode.Remove();
	if( !pNewNode )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"LadderTeamManager::CreateNewLadderTeam MemPool Zero!");
		return NULL;
	}

	//m_vLadderTeamNode.push_back( pNewNode );
	m_mLadderTeamNode.insert( make_pair(pNewNode->GetIndex(), pNewNode) );
	pNewNode->OnCreate();
	return pNewNode;
}

void LadderTeamManager::RemoveLadderTeam( LadderTeamNode *pNode )
{	
	/*vLadderTeamNode_iter iter, iEnd;
	iEnd = m_vLadderTeamNode.end();
	for(iter = m_vLadderTeamNode.begin();iter != iEnd;++iter)
	{
		LadderTeamNode *pCursor = *iter;
		if( pCursor == pNode )
		{
			pCursor->OnDestroy();
			m_MemNode.Push( pCursor );
			m_vLadderTeamNode.erase( iter );
			break;
		}
	}*/
	if( !pNode )
		return;

	mLadderTeamNode_iter findIter	= m_mLadderTeamNode.find(pNode->GetIndex());
	if( findIter != m_mLadderTeamNode.end() )
	{
		LadderTeamNode *pCursor = findIter->second;
		if( pCursor )
		{
			RemoveSearchingList(pCursor->GetIndex(),  pCursor->IsHeroMatchMode());
			pCursor->OnDestroy();
			m_MemNode.Push( pCursor );
			m_mLadderTeamNode.erase(findIter);
		}
	}
	else
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"[warning][ladderteam]None exist ladder Index so can not be deleted" );
	}
}

int LadderTeamManager::GetHeroNodeSize()
{
	int iReturnNodeSize = 0;
	mLadderTeamNode_iter iter = m_mLadderTeamNode.begin();
	for(;iter != m_mLadderTeamNode.end();++iter)
	{
		LadderTeamNode *pCursor = iter->second;
		if( pCursor == NULL ) continue;
		if( pCursor->IsHeroMatchMode() == false ) continue;

		iReturnNodeSize++;
	}
	return iReturnNodeSize;
}

LadderTeamNode *LadderTeamManager::GetLadderTeamNode( DWORD dwIndex )
{
	/*vLadderTeamNode_iter iter, iEnd;
	iEnd = m_vLadderTeamNode.end();
	for(iter = m_vLadderTeamNode.begin();iter != iEnd;++iter)
	{
		LadderTeamNode *pCursor = *iter;
		if( pCursor->GetIndex() == dwIndex )
		{
			return pCursor;
		}
	}*/

	mLadderTeamNode_iter findIter	= m_mLadderTeamNode.find(dwIndex);
	if( findIter != m_mLadderTeamNode.end() )
	{
		return findIter->second;
	}

	return NULL;
}

void LadderTeamManager::AddCopyLadderTeam( LadderTeamCopyNode *pLadderTeam )
{
	/*vLadderTeamCopyNode_iter iter,iEnd;
	iEnd = m_vLadderTeamCopyNode.end();
	for( iter=m_vLadderTeamCopyNode.begin() ; iter!=iEnd ; ++iter )
	{
		LadderTeamCopyNode *pCursor = *iter;

		if( pCursor == pLadderTeam )
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "LadderTeamManager::AddCopyLadderTeam 이미 리스트에 있는 복사본임(%d)", pCursor->GetIndex() );
			return;
		}
	}
	m_vLadderTeamCopyNode.push_back( pLadderTeam );*/

	if( !pLadderTeam )
		return;

	mLadderTeamCopyNode_iter it = m_mLadderTeamCopyNode.find(pLadderTeam->GetIndex());

	if( it != m_mLadderTeamCopyNode.end() )
	{
		LadderTeamCopyNode *pCursor = it->second;
		if( pCursor )
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "LadderTeamManager::AddCopyLadderTeam 이미 리스트에 있는 복사본임(%d)", pCursor->GetIndex() );
		
		return;
	}

	m_mLadderTeamCopyNode.insert(make_pair(pLadderTeam->GetIndex(), pLadderTeam));
}

void LadderTeamManager::RemoveLadderTeamCopy( LadderTeamCopyNode *pLadderTeam )
{
	/*vLadderTeamCopyNode_iter iter,iEnd;
	iEnd = m_vLadderTeamCopyNode.end();
	for( iter=m_vLadderTeamCopyNode.begin() ; iter!=iEnd ; ++iter )
	{
		LadderTeamCopyNode *pCursor = *iter;

		if( pCursor == pLadderTeam )
		{
			if( LadderTeamNode::TMS_SEARCH_PROCEED == pCursor->GetTeamState() )
			{
				RemoveCopyNodeSearchingList(pCursor->GetIndex(), pCursor->IsHeroMatchMode());
			}

			m_vLadderTeamCopyNode.erase( iter );
			return;
		}
	}*/
	if( !pLadderTeam )
		return;

	mLadderTeamCopyNode_iter it = m_mLadderTeamCopyNode.find(pLadderTeam->GetIndex());
	
	if( it == m_mLadderTeamCopyNode.end() )
		return;

	LadderTeamCopyNode *pCursor = it->second;
	if( pCursor )
	{
		if( LadderTeamNode::TMS_SEARCH_PROCEED == pCursor->GetTeamState() )
		{
			RemoveCopyNodeSearchingList(pCursor->GetIndex(), pCursor->IsHeroMatchMode());
		}
	}

	m_mLadderTeamCopyNode.erase(it);
}

void LadderTeamManager::RemoveUserCopyNode( DWORD dwUserIndex, const ioHashString &rkName )
{
	/*vLadderTeamNode_iter iter, iEnd;
	iEnd = m_vLadderTeamNode.end();
	for(iter = m_vLadderTeamNode.begin();iter != iEnd;++iter)
	{
		LadderTeamNode *pCursor = *iter;
		if( pCursor->LeaveUser( dwUserIndex, rkName ) )
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "LadderTeamManager::RemoveUserCopyNode : Leave User (%d) - %s(%d)", pCursor->GetIndex(), rkName.c_str(), dwUserIndex );
			return;
		}
	}*/

	mLadderTeamNode_iter it = m_mLadderTeamNode.begin();

	for( ; it != m_mLadderTeamNode.end(); it++ )
	{
		LadderTeamNode *pCursor = it->second;
		if( !pCursor ) continue;
		if( pCursor->LeaveUser( dwUserIndex, rkName ) )
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "LadderTeamManager::RemoveUserCopyNode : Leave User (%d) - %s(%d)", pCursor->GetIndex(), rkName.c_str(), dwUserIndex );
			return;
		}
	}
}

void LadderTeamManager::ConnectServerNodeSync( ServerNode *pServerNode )
{
	if( pServerNode == NULL ) return;

	static vLadderTeamNode vLadderTeam;
	vLadderTeam.clear();

	LOOP_GUARD();
	/*vLadderTeamNode_iter iter = m_vLadderTeamNode.begin();
	while( iter != m_vLadderTeamNode.end() )
	{
		LadderTeamNode *pNode = *iter++;
		vLadderTeam.push_back( pNode );		
	}*/
	mLadderTeamNode_iter it = m_mLadderTeamNode.begin();
	
	for( ; it != m_mLadderTeamNode.end(); it++ )
	{
		LadderTeamNode *pNode = it->second;
		if( pNode )
			vLadderTeam.push_back( pNode );		
	}

	LOOP_GUARD_CLEAR();

	LOOP_GUARD();
	// 오리지날 길드팀 정보만 N개씩 끊어서 전송
	while( true )
	{
		int iMaxSize = min( SSTPK_CONNECT_SYNC_LADDERTEAM_MAX, (int)vLadderTeam.size() );
		if( iMaxSize == 0 )
			break;

		SP2Packet kPacket( SSTPK_CONNECT_SYNC );
		kPacket << SSTPK_CONNECT_SYNC_LADDERTEAM << iMaxSize;
		for(int i = 0;i < iMaxSize;i++)
		{
			LadderTeamNode *pNode  = vLadderTeam[0];
			pNode->FillSyncCreate( kPacket );
			vLadderTeam.erase( vLadderTeam.begin() );
		}
		pServerNode->SendMessage( kPacket );
	}
	LOOP_GUARD_CLEAR();
}

LadderTeamParent* LadderTeamManager::GetGlobalLadderTeamNode( DWORD dwIndex )
{
	LadderTeamParent *pReturn = (LadderTeamParent*)GetLadderTeamNode( dwIndex );
	if( pReturn )
		return pReturn;

	/*vLadderTeamCopyNode_iter iter,iEnd;
	iEnd = m_vLadderTeamCopyNode.end();
	for( iter=m_vLadderTeamCopyNode.begin() ; iter!=iEnd ; ++iter )
	{
		LadderTeamCopyNode *pCursor = *iter;
		if( pCursor->GetIndex() == dwIndex )
		{
			return pCursor;
		}
	}*/

	mLadderTeamCopyNode_iter it = m_mLadderTeamCopyNode.find(dwIndex);
	if( it != m_mLadderTeamCopyNode.end() )
		return it->second;

	return NULL;
}

void LadderTeamManager::SetCampBattlePlay( bool bCampBattlePlay )
{
	m_bCampBattlePlay = bCampBattlePlay;
	
	if( !IsCampBattlePlay() )
	{
		// Original Ladder Team만 처리한다.
		/*vLadderTeamNode_iter iter, iEnd;
		iEnd = m_vLadderTeamNode.end();
		for(iter=m_vLadderTeamNode.begin();iter != iEnd;++iter)
		{
			LadderTeamNode *pNode = *iter;
			if( pNode == NULL ) continue;
			if( pNode->IsEmptyUser() ) continue;
			
			SP2Packet kPacket( STPK_LADDERTEAM_MACRO );
			kPacket << LADDERTEAM_MACRO_CAMP_BATTLE_END;
			pNode->SendPacketTcp( kPacket );
		}*/

		mLadderTeamNode_iter it = m_mLadderTeamNode.begin();
		for( ; it != m_mLadderTeamNode.end(); it++ )
		{
			LadderTeamNode *pNode = it->second;
			if( !pNode ) continue;
			if( pNode->IsEmptyUser() ) continue;
			
			SP2Packet kPacket( STPK_LADDERTEAM_MACRO );
			kPacket << LADDERTEAM_MACRO_CAMP_BATTLE_END;
			pNode->SendPacketTcp( kPacket );
		}
	}
}

void LadderTeamManager::CreateTeamMatchingTable()
{
	FUNCTION_TIME_CHECKER( 100000.0f, 0 );          // 0.1 초 이상 걸리면로그 남김
	g_LevelMatchMgr.InitLadderLevelMatch();

	static vSortLadderTeam vTeamList;
	vTeamList.clear();

	//Original Ladder
	/*vLadderTeamNode_iter iter, iEnd;
	iEnd = m_vLadderTeamNode.end();
	for(iter=m_vLadderTeamNode.begin();iter != iEnd;++iter)
	{
		LadderTeamNode *pTeam = *iter;
		if( pTeam->IsHeroMatchMode() ) continue;

		SortLadderTeam kTeam;
		kTeam.m_pNode  = (LadderTeamParent*)pTeam;
		kTeam.m_iPoint = pTeam->GetAbilityMatchLevel();
		vTeamList.push_back( kTeam );
	}*/

	mLadderTeamNode_iter it = m_mLadderTeamNode.begin();
	for( ; it != m_mLadderTeamNode.end(); it++ )
	{
		LadderTeamNode *pTeam = it->second;
		if( !pTeam ) continue;
		if( pTeam->IsHeroMatchMode() ) continue;

		SortLadderTeam kTeam;
		kTeam.m_pNode  = (LadderTeamParent*)pTeam;
		kTeam.m_iPoint = pTeam->GetAbilityMatchLevel();
		vTeamList.push_back( kTeam );
	}

	//Copy Ladder
	/*vLadderTeamCopyNode_iter iterCopy, iEndCopy;
	iEndCopy = m_vLadderTeamCopyNode.end();
	for(iterCopy = m_vLadderTeamCopyNode.begin();iterCopy != iEndCopy;++iterCopy)
	{
		LadderTeamCopyNode *pTeam = *iterCopy;
		if( pTeam->IsHeroMatchMode() ) continue;

		SortLadderTeam kTeam;
		kTeam.m_pNode  = (LadderTeamParent*)pTeam;
		kTeam.m_iPoint = pTeam->GetAbilityMatchLevel();
		vTeamList.push_back( kTeam );
	}*/
	mLadderTeamCopyNode_iter itCopy = m_mLadderTeamCopyNode.begin();
	for( ; itCopy != m_mLadderTeamCopyNode.end(); itCopy++ )
	{
		LadderTeamCopyNode *pTeam = itCopy->second;
		if( !pTeam ) continue;
		if( pTeam->IsHeroMatchMode() ) continue;

		SortLadderTeam kTeam;
		kTeam.m_pNode  = (LadderTeamParent*)pTeam;
		kTeam.m_iPoint = pTeam->GetAbilityMatchLevel();
		vTeamList.push_back( kTeam );
	}

	if( vTeamList.empty() )
		return;

	std::sort( vTeamList.begin(), vTeamList.end(), LadderTeamSort() );

	int iMaxList = vTeamList.size();
	int i = 0;
	int iLimitCount = max( 1, (float)iMaxList * g_LevelMatchMgr.GetLadderEnterLimitCount() );
	for(i = 0;i < g_LevelMatchMgr.GetRoomEnterLevelMax();i++)
	{		
		int rIndex = 0;
		for(;rIndex < iMaxList;rIndex++)
		{
			SortLadderTeam &kSortTeam = vTeamList[rIndex];
			if( kSortTeam.m_iPoint >= i )
				break;
		}

		int iMaxRemain = max( 0, ( rIndex + iLimitCount ) - iMaxList );
		int iMinRemain = max( 0, iLimitCount - rIndex );
		int iLowIndex  = max( rIndex - iLimitCount - iMaxRemain, 0 );
		int iHighIndex = min( iLimitCount + iMinRemain + rIndex, iMaxList );

		//자신있지만 벡터의 사이즈를 넘는 인덱스가 혹시 나오면 뻗을까봐....-_-;
		if( iLowIndex < 0 )
			iLowIndex = 0;
		else if( iLowIndex >= iMaxList )
			iLowIndex = iMaxList - 1;

		if( iHighIndex < 0 )
			iHighIndex = 0;
		else if( iHighIndex > iMaxList )
			iHighIndex = iMaxList;

		SortLadderTeam &kLowTeam = vTeamList[iLowIndex];
		SortLadderTeam &kHighTeam = vTeamList[iHighIndex - 1];
		g_LevelMatchMgr.InsertLadderLevelMatch( min( i, kLowTeam.m_iPoint ), max( i, kHighTeam.m_iPoint ) );
	}	
}

void LadderTeamManager::CreateHeroMatchingPoint()
{
	__int64 iTotalMatchPoint = 0;
	int iHeroCount = 0;
	//Original Ladder
	/*vLadderTeamNode_iter iter, iEnd;
	iEnd = m_vLadderTeamNode.end();
	for(iter=m_vLadderTeamNode.begin();iter != iEnd;++iter)
	{
		LadderTeamNode *pTeam = *iter;
		if( !pTeam->IsHeroMatchMode() ) continue;

		iHeroCount++;
		iTotalMatchPoint = (__int64)iTotalMatchPoint + (__int64)pTeam->GetHeroMatchPoint();
	}*/

	mLadderTeamNode_iter it = m_mLadderTeamNode.begin();
	for( ; it != m_mLadderTeamNode.end(); it++ )
	{
		LadderTeamNode *pTeam = it->second;
		if( !pTeam ) continue;
		if( !pTeam->IsHeroMatchMode() ) continue;

		iHeroCount++;
		iTotalMatchPoint = (__int64)iTotalMatchPoint + (__int64)pTeam->GetHeroMatchPoint();
	}

	//Copy Ladder
	/*vLadderTeamCopyNode_iter iterCopy, iEndCopy;
	iEndCopy = m_vLadderTeamCopyNode.end();
	for(iterCopy = m_vLadderTeamCopyNode.begin();iterCopy != iEndCopy;++iterCopy)
	{
		LadderTeamCopyNode *pTeam = *iterCopy;
		if( !pTeam->IsHeroMatchMode() ) continue;

		iHeroCount++;
		iTotalMatchPoint = (__int64)iTotalMatchPoint + (__int64)pTeam->GetHeroMatchPoint();
	}*/

	mLadderTeamCopyNode_iter itCopy = m_mLadderTeamCopyNode.begin();
	for( ; itCopy != m_mLadderTeamCopyNode.end(); itCopy++ )
	{
		LadderTeamCopyNode *pTeam = itCopy->second;
		if( !pTeam ) continue;
		if( !pTeam->IsHeroMatchMode() ) continue;

		iHeroCount++;
		iTotalMatchPoint = (__int64)iTotalMatchPoint + (__int64)pTeam->GetHeroMatchPoint();
	}

	if( iHeroCount == 0 )
		m_iAverageHeroMatchPoint = g_LevelMatchMgr.GetRoomEnterLevelMax() * 10;
	else
	{
		m_iAverageHeroMatchPoint = (float)iTotalMatchPoint / iHeroCount;
	}
}

void LadderTeamManager::CreateHeroMatchingTable()
{
	FUNCTION_TIME_CHECKER( 100000.0f, 0 );          // 0.1 초 이상 걸리면로그 남김
	g_LevelMatchMgr.InitLadderHeroLevelMatch();

	CreateHeroMatchingPoint();
	
	bool bMatchLog = g_LevelMatchMgr.IsMatchingLadderHeroLOG();

	//Original Ladder
	static vSortLadderTeam vTeamList;
	vTeamList.clear();

	/*vLadderTeamNode_iter iter, iEnd;
	iEnd = m_vLadderTeamNode.end();
	for(iter=m_vLadderTeamNode.begin();iter != iEnd;++iter)
	{
		LadderTeamNode *pTeam = *iter;
		if( !pTeam->IsHeroMatchMode() ) continue;

		SortLadderTeam kTeam;
		kTeam.m_pNode  = (LadderTeamParent*)pTeam;
		kTeam.m_iPoint = pTeam->GetAbilityMatchLevel();		
		vTeamList.push_back( kTeam );

		if( bMatchLog )
		{
			LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "%s : Lv - %d", pTeam->GetTeamName().c_str(), kTeam.m_iPoint );
		}
	}*/

	mLadderTeamNode_iter it = m_mLadderTeamNode.begin();
	for( ; it != m_mLadderTeamNode.end(); it++ )
	{
		LadderTeamNode *pTeam = it->second;
		if( !pTeam ) continue;
		if( !pTeam->IsHeroMatchMode() ) continue;

		SortLadderTeam kTeam;
		kTeam.m_pNode  = (LadderTeamParent*)pTeam;
		kTeam.m_iPoint = pTeam->GetAbilityMatchLevel();		
		vTeamList.push_back( kTeam );

		LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "CreateHeroMatchingTable point: %d %d", kTeam.m_pNode->GetIndex(), kTeam.m_iPoint );

		if( bMatchLog )
		{
			LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "%s : Lv - %d", pTeam->GetTeamName().c_str(), kTeam.m_iPoint );
		}
	}

	//Copy Ladder
	/*vLadderTeamCopyNode_iter iterCopy, iEndCopy;
	iEndCopy = m_vLadderTeamCopyNode.end();
	for(iterCopy = m_vLadderTeamCopyNode.begin();iterCopy != iEndCopy;++iterCopy)
	{
		LadderTeamCopyNode *pTeam = *iterCopy;
		if( !pTeam->IsHeroMatchMode() ) continue;

		SortLadderTeam kTeam;
		kTeam.m_pNode  = (LadderTeamParent*)pTeam;
		kTeam.m_iPoint = pTeam->GetAbilityMatchLevel();		
		vTeamList.push_back( kTeam );

		if( bMatchLog )
		{
			LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "%s : Lv - %d", pTeam->GetTeamName().c_str(), kTeam.m_iPoint );
		}
	}*/
	mLadderTeamCopyNode_iter itCopy = m_mLadderTeamCopyNode.begin();
	for( ; itCopy != m_mLadderTeamCopyNode.end(); itCopy++ )
	{
		LadderTeamCopyNode *pTeam = itCopy->second;
		if( !pTeam )	continue;
		if( !pTeam->IsHeroMatchMode() ) continue;

		SortLadderTeam kTeam;
		kTeam.m_pNode  = (LadderTeamParent*)pTeam;
		kTeam.m_iPoint = pTeam->GetAbilityMatchLevel();		
		vTeamList.push_back( kTeam );

		if( bMatchLog )
		{
			LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "%s : Lv - %d", pTeam->GetTeamName().c_str(), kTeam.m_iPoint );
		}
	}

	if( vTeamList.empty() )
		return;

	std::sort( vTeamList.begin(), vTeamList.end(), LadderTeamSort() );

	int iMaxList = vTeamList.size();
	int i = 0;
	int iLimitCount = max( 1, (float)iMaxList * g_LevelMatchMgr.GetLadderHeroEnterLimitCount() );
	for(i = 0;i < g_LevelMatchMgr.GetRoomEnterLevelMax();i++)
	{		
		int rIndex = 0;
		for(;rIndex < iMaxList;rIndex++)
		{
			SortLadderTeam &kSortTeam = vTeamList[rIndex];
			if( kSortTeam.m_iPoint >= i )
				break;
		}

		int iMaxRemain = max( 0, ( rIndex + iLimitCount ) - iMaxList );
		int iMinRemain = max( 0, iLimitCount - rIndex );
		int iLowIndex  = max( rIndex - iLimitCount - iMaxRemain, 0 );
		int iHighIndex = min( iLimitCount + iMinRemain + rIndex, iMaxList );

		//자신있지만 벡터의 사이즈를 넘는 인덱스가 혹시 나오면 뻗을까봐....-_-;
		if( iLowIndex < 0 )
			iLowIndex = 0;
		else if( iLowIndex >= iMaxList )
			iLowIndex = iMaxList - 1;

		if( iHighIndex < 0 )
			iHighIndex = 0;
		else if( iHighIndex > iMaxList )
			iHighIndex = iMaxList;

		SortLadderTeam &kLowTeam = vTeamList[iLowIndex];
		SortLadderTeam &kHighTeam = vTeamList[iHighIndex - 1];
		g_LevelMatchMgr.InsertLadderHeroLevelMatch( min( i, kLowTeam.m_iPoint ), max( i, kHighTeam.m_iPoint ) );

		LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "CreateHeroMatchingTable : i: %d iMaxList: %d iLimitCount: %d rIndex: %d iLowIndex: %d iHighIndex: %d iMaxRemain: %d iMinReamain: %d LowPoint: %d HighPoint: %d"
			, i, iMaxList, iLimitCount, rIndex, iLowIndex, iHighIndex, iMaxRemain, iMinRemain, kLowTeam.m_iPoint, kHighTeam.m_iPoint );

		if( bMatchLog )
		{
			LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "InsertLadderHeroLevelMatch : %d] Min(%d) - Max(%d)", i, min( i, kLowTeam.m_iPoint ), max( i, kHighTeam.m_iPoint ) );
		}
	}	
}

void LadderTeamManager::InitTotalModeList()
{
	m_vTotalRandomModeList.clear();

	ioINILoader kLoader( "config/sp2_mode_select.ini" );

	int i, j;
	char szKey[MAX_PATH] = "";
	char szBuf[MAX_PATH] = "";
	kLoader.SetTitle( "ModeTypeInfo" );
	int iModeCnt = kLoader.LoadInt( "mode_cnt", 0 );
	for( i = 0; i < iModeCnt; ++i )
	{
		wsprintf( szKey, "mode%d", i+1 );
		kLoader.SetTitle( szKey );

		ioHashString szModeTitle;
		kLoader.LoadString( "mode_title", "", szBuf, MAX_PATH );
		szModeTitle = szBuf;

		int iModeType = kLoader.LoadInt( "mode_type", 0 );

		LadderTeamRandomMode kRandomMode;
		kRandomMode.m_Title = szModeTitle;
		kRandomMode.m_ModeType = (ModeType)iModeType;

		int iSubCnt = kLoader.LoadInt( "sub_cnt", 0 );
		for( j = 0; j < iSubCnt; ++j )
		{
			int iSubType;
			ioHashString szSubTitle;
			bool bActive;

			sprintf_s( szKey, "sub%d_active", j+1 );
			bActive = kLoader.LoadBool( szKey, true );

			wsprintf( szKey, "sub%d_title", j+1 );
			kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
			szSubTitle = szBuf;

			wsprintf( szKey, "sub%d_type", j+1 );
			iSubType = kLoader.LoadInt( szKey, 0 );
			
			LadderTeamMapInfo kLadderTeamMapInfo;
			kLadderTeamMapInfo.m_bActive  = bActive;
			kLadderTeamMapInfo.m_Title    = szSubTitle;
			kLadderTeamMapInfo.m_iSubType = iSubType;

			kRandomMode.m_SubList.push_back( kLadderTeamMapInfo );
		}

		// inactive sub mode 처리, 기존 값으로 셋팅함 array를 유지하기 위해서
		int iSize = kRandomMode.m_SubList.size();
		IntVec vActiveArray;
		vActiveArray.reserve( iSize );
		for (int i = 0; i < iSize ; i++)
		{
			LadderTeamMapInfo &rCurInfo = kRandomMode.m_SubList[i];
			if( rCurInfo.m_bActive )
				vActiveArray.push_back( i );
		}
		int iActiveArraySize = vActiveArray.size();
		int iActiveCnt = 0;
		for (int i = 0; i < iSize ; i++)
		{
			LadderTeamMapInfo &rInActivekInfo = kRandomMode.m_SubList[i];
			if( rInActivekInfo.m_bActive )
				continue;
			if( iActiveArraySize <= 0 )
				break;
			int iArray = vActiveArray[iActiveCnt];
			if( !COMPARE( iArray, 0, iSize ) )
				break;
			LadderTeamMapInfo &rActivekInfo = kRandomMode.m_SubList[iArray];
			rInActivekInfo = rActivekInfo;
			rInActivekInfo.m_bActive = false;
			iActiveCnt++;
			if( iActiveCnt >= iActiveArraySize )
				iActiveCnt = 0;
		}
		vActiveArray.clear();
		//

		m_vTotalRandomModeList.push_back( kRandomMode );
	}
}

bool LadderTeamManager::CheckLadderTeamRandomMode( int iModeIndex, int iMapIndex, ModeType &eModeType, int &iSubType )
{
	// 모드
	int iModeCnt = m_vTotalRandomModeList.size();
	if( COMPARE( iModeIndex - 1, 0, iModeCnt ) )
	{
		eModeType = m_vTotalRandomModeList[iModeIndex - 1].m_ModeType;
	}
	else
		return false;
	
	// 맵
	int iSubCnt = m_vTotalRandomModeList[iModeIndex - 1].m_SubList.size();
	if( COMPARE( iMapIndex - 1, 0, iSubCnt ) )
	{
		iSubType = m_vTotalRandomModeList[iModeIndex - 1].m_SubList[iMapIndex - 1].m_iSubType;		
	}

	return true;
}

bool LadderTeamManager::SearchingLadderMode(LadderTeamNode *pRequestTeam)
{
	mLadderTeamNode_iter it;
	
	static vLadderTeamNodeSort vSearchTeamNode;
	vSearchTeamNode.clear();
	
	for( int i = 0; i < (int)m_vSearchingLadderList.size(); i++ )
	{
		it = m_mLadderTeamNode.find(m_vSearchingLadderList[i]);
		if( it == m_mLadderTeamNode.end() )
			continue;

		static int iGapLevel = 0;
		LadderTeamNode *pNode = it->second;
		if( !pNode ) continue;
		if( !DoMatching(pRequestTeam, pNode, iGapLevel) )
			continue;

		LadderTeamNodeSort sortNode;
		sortNode.m_pNode = pNode;
		sortNode.m_iGapLevel = iGapLevel;
		vSearchTeamNode.push_back( sortNode );
	}

	static vLadderTeamNodeSort vSearchTeamCopyNode;
	vSearchTeamCopyNode.clear();

	if( vSearchTeamNode.size() == 0 )
	{
		//카피노드에서 검색
		mLadderTeamCopyNode_iter it;
		for( int i = 0; i < (int)m_vCopyNodeSearchingLadderList.size(); i++ )
		{
			it = m_mLadderTeamCopyNode.find(m_vCopyNodeSearchingLadderList[i]);
			if( it == m_mLadderTeamCopyNode.end() )
				continue;

			static int iGapLevel = 0;
			LadderTeamCopyNode *pNode = it->second;
			if( !pNode ) continue;
			if( !DoMatching(pRequestTeam, pNode, iGapLevel) )
				continue;

			LadderTeamNodeSort sortNode;
			sortNode.m_pNode = pNode;
			sortNode.m_iGapLevel = iGapLevel;
			vSearchTeamCopyNode.push_back( sortNode );
		}
	}

	if( vSearchTeamNode.size() == 0 && vSearchTeamCopyNode.size() == 0 )
	{
		pRequestTeam->IncreaseSearchCount();
		return false;


	}

	LadderTeamParent* pSearchingNode			= NULL;
	DWORD dwSearchingIndex						= 0;

	if( vSearchTeamNode.size() == 0 )
	{
		std::sort( vSearchTeamCopyNode.begin(), vSearchTeamCopyNode.end(), SortLadderTeamNode() );

		LadderTeamNodeSort& sortNode = vSearchTeamCopyNode[0];
		pSearchingNode = sortNode.m_pNode;
		dwSearchingIndex =  pSearchingNode->GetIndex();
	}
	else
	{
		std::sort( vSearchTeamNode.begin(), vSearchTeamNode.end(), SortLadderTeamNode() );

		LadderTeamNodeSort& sortNode = vSearchTeamCopyNode[0];
		pSearchingNode = sortNode.m_pNode;
		dwSearchingIndex	= pSearchingNode->GetIndex();
	}

	pRequestTeam->SetMatchReserve( dwSearchingIndex );
	pRequestTeam->SetTeamState( LadderTeamParent::TMS_MATCH_RESERVE );
	pRequestTeam->InitSearchCount();

	pSearchingNode->MatchRoomRequest( pRequestTeam->GetIndex() );

	return true;
}

bool LadderTeamManager::SearchingTeamMode(LadderTeamNode *pRequestTeam)
{
	LadderTeamNode *pSearchingNode			= NULL;
	LadderTeamCopyNode *pSearchingCopyNode	= NULL;

	mLadderTeamNode_iter it;
	for( int i = 0; i < (int)m_vSearchingTeamList.size(); i++ )
	{
		it = m_mLadderTeamNode.find(m_vSearchingTeamList[i]);
		if( it == m_mLadderTeamNode.end() )
			continue;

		static int iGapLevel = 0;
		LadderTeamNode *pNode = it->second;
		
		if( !pNode ) continue;
		if( !DoMatching(pRequestTeam, pNode, iGapLevel) )
			continue;

		pSearchingNode	= pNode;
		break;
	}

	if( !pSearchingNode )
	{
		//카피노드에서 검색
		mLadderTeamCopyNode_iter it;
		for( int i = 0; i < (int)m_vCopyNodeSearchingTeamList.size(); i++ )
		{
			it = m_mLadderTeamCopyNode.find(m_vCopyNodeSearchingTeamList[i]);
			if( it == m_mLadderTeamCopyNode.end() )
				continue;

			static int iGapLevel = 0;
			LadderTeamCopyNode *pNode = it->second;

			if( !pNode ) continue;
			if( !DoMatching(pRequestTeam, pNode, iGapLevel) )
				continue;

			pSearchingCopyNode	= pNode;
			break;
		}
	}
	
	if( !pSearchingNode && !pSearchingCopyNode )
	{
		pRequestTeam->IncreaseSearchCount();
		return false;
	}

	DWORD dwSearchingIndex = 0;

	if( pSearchingNode )
		dwSearchingIndex	=  pSearchingNode->GetIndex();
	else
		dwSearchingIndex	= pSearchingCopyNode->GetIndex();

	pRequestTeam->SetMatchReserve( dwSearchingIndex );
	pRequestTeam->SetTeamState( LadderTeamParent::TMS_MATCH_RESERVE );
	pRequestTeam->InitSearchCount();

	if( pSearchingNode )
		pSearchingNode->MatchRoomRequest( pRequestTeam->GetIndex() );
	else
		pSearchingCopyNode->MatchRoomRequest( pRequestTeam->GetIndex() );

	return true;
}

bool LadderTeamManager::SearchBattleMatch( LadderTeamNode *pRequestTeam )
{
	FUNCTION_TIME_CHECKER( 100000.0f, 0 );          // 0.1 초 이상 걸리면로그 남김
	if( !IsCampBattlePlay() ) return false;

	if( !pRequestTeam )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[warning][ladderteam]Searching request team does not exist" );
		return false;
	}
	if( !pRequestTeam->IsOriginal() )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[warning][ladderteam]Request team is different server" );
		return false;
	}

	if( pRequestTeam->IsHeroMatchMode() )
	{
		return SearchingLadderMode(pRequestTeam);
	}
	else
	{
		return SearchingTeamMode(pRequestTeam);
	}

	return false;
}

/*
bool LadderTeamManager::SearchBattleMatch( LadderTeamNode *pRequestTeam )
{
	FUNCTION_TIME_CHECKER( 100000.0f, 0 );          // 0.1 초 이상 걸리면로그 남김
	if( !IsCampBattlePlay() ) return false;

	if( !pRequestTeam )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[warning][ladderteam]Searching request team does not exist" );
		return false;
	}
	if( !pRequestTeam->IsOriginal() )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[warning][ladderteam]Request team is different server" );
		return false;
	}

	if( !pRequestTeam->IsHeroMatchMode() )
	{
		// 진영전 1인 플레이 안됨
		if( pRequestTeam->GetJoinUserCnt() <= 1 )
			return false;
	}

	bool bSameCampSearch = false;
	if( pRequestTeam->GetSearchCount() >= 3 )
		bSameCampSearch = true;

	static vSortLadderTeam vLadderTeamList;
	vLadderTeamList.clear();

	vLadderTeamNode_iter iter, iEnd;
	iEnd = m_vLadderTeamNode.end();
	for(iter = m_vLadderTeamNode.begin();iter != iEnd;++iter)
	{
		LadderTeamNode *pNode = *iter;
		if( pNode == NULL ) continue;
		if( pRequestTeam->GetIndex() == pNode->GetIndex() ) continue;
		if( pNode->IsEmptyUser() ) continue;
		if( pRequestTeam->IsHeroMatchMode() != pNode->IsHeroMatchMode() ) continue;  //영웅전 유저 끼리만 대전 가능함.
		if( !bSameCampSearch )
		{
			if( pRequestTeam->GetCampType() == pNode->GetCampType() ) continue;			//같은 진영끼리 대전 X
		}
		if( !pNode->IsHeroMatchMode() && pNode->GetJoinUserCnt() <= 1 ) continue;    //진영전 1인 플레이 안됨
		if( pRequestTeam->IsBadPingKick() != pNode->IsBadPingKick() ) continue;      //네트워크 불량 옵션이 같아야함
		if( pRequestTeam->IsMatchReserve( pNode->GetIndex() ) ) continue;
		if( !pNode->IsSearching() ) continue;
		//if( pRequestTeam->IsReMatchLimit( pNode->GetIndex(), m_dwLastMatchDelayTime ) ) continue;

		// 요청자 옵션 체크
		// 수준매칭 검색 옵션 체크
		if( pRequestTeam->IsSearchLevelMatch() )
		{
			if( pRequestTeam->IsHeroMatchMode() )
			{
				if( !g_LevelMatchMgr.IsLadderHeroLevelJoin( pRequestTeam->GetAbilityMatchLevel(), pNode->GetAbilityMatchLevel(), JOIN_CHECK_MINMAX_LEVEL ) ) continue;
			}
			else
			{
				if( !g_LevelMatchMgr.IsLadderLevelJoin( pRequestTeam->GetAbilityMatchLevel(), pNode->GetAbilityMatchLevel(), JOIN_CHECK_MINMAX_LEVEL ) ) continue;
			}
		}

		// 동일 인원 옵션 체크
		if( pRequestTeam->IsSearchSameUser() )
		{
			if( pRequestTeam->GetJoinUserCnt() != pNode->GetJoinUserCnt() ) continue;
		}		
		
		// 상대팀 옵션 체크
		if( pNode->IsSearchLevelMatch() )
		{
			if( pNode->IsHeroMatchMode() )
			{
				if( !g_LevelMatchMgr.IsLadderHeroLevelJoin( pNode->GetAbilityMatchLevel(), pRequestTeam->GetAbilityMatchLevel(), JOIN_CHECK_MINMAX_LEVEL ) ) continue;
			}
			else
			{
				if( !g_LevelMatchMgr.IsLadderLevelJoin( pNode->GetAbilityMatchLevel(), pRequestTeam->GetAbilityMatchLevel(), JOIN_CHECK_MINMAX_LEVEL ) ) continue;
			}
		}
		// 동일 인원 옵션 체크
		if( pNode->IsSearchSameUser() )
		{
			if( pRequestTeam->GetJoinUserCnt() != pNode->GetJoinUserCnt() ) continue;
		}
		
		// 모드 체크
		if( pRequestTeam->GetSelectMode() != -1 && pNode->GetSelectMode() != -1 )
		{
			if( pNode->GetSelectMode() != pRequestTeam->GetSelectMode() ) continue;
		}
		// 맵 체크
		if( pRequestTeam->GetSelectMap() != -1 && pNode->GetSelectMap() != -1 )
		{
			if( pNode->GetSelectMap() != pRequestTeam->GetSelectMap() ) continue;
		}

		SortLadderTeam slt;
		slt.m_pNode  = (LadderTeamParent*)pNode;
		slt.m_iPoint = abs( pRequestTeam->GetAbilityMatchLevel() - pNode->GetAbilityMatchLevel() );
		vLadderTeamList.push_back( slt );
		break;
	}

	if( vLadderTeamList.empty() )
	{
		vLadderTeamCopyNode_iter iterCopy, iEndCopy;
		iEndCopy = m_vLadderTeamCopyNode.end();
		for( iterCopy=m_vLadderTeamCopyNode.begin() ; iterCopy!=iEndCopy ; ++iterCopy )
		{
			LadderTeamCopyNode *pNode = *iterCopy;
			if( pNode == NULL ) continue;
			if( pRequestTeam->GetIndex() == pNode->GetIndex() ) continue;
			if( pNode->IsEmptyUser() ) continue;
			if( pRequestTeam->IsHeroMatchMode() != pNode->IsHeroMatchMode() ) continue; //영웅전 유저 끼리만 대전 가능함.
			if( !bSameCampSearch )
			{
				if( pRequestTeam->GetCampType() == pNode->GetCampType() ) continue;			//같은 진영끼리 대전 X
			}
			if( !pNode->IsHeroMatchMode() && pNode->GetJoinUserCnt() <= 1 ) continue;    //진영전 1인 플레이 안됨
			if( pRequestTeam->IsBadPingKick() != pNode->IsBadPingKick() ) continue;      //네트워크 불량 옵션이 같아야함
			if( pRequestTeam->IsMatchReserve( pNode->GetIndex() ) ) continue;
			if( !pNode->IsSearching() ) continue;
			//if( pRequestTeam->IsReMatchLimit( pNode->GetIndex(), m_dwLastMatchDelayTime ) ) continue;

			// 요청자 옵션 체크
			// 수준매칭 검색 옵션 체크
			if( pRequestTeam->IsSearchLevelMatch() )
			{
				if( pRequestTeam->IsHeroMatchMode() )
				{
					if( !g_LevelMatchMgr.IsLadderHeroLevelJoin( pRequestTeam->GetAbilityMatchLevel(), pNode->GetAbilityMatchLevel(), JOIN_CHECK_MINMAX_LEVEL ) ) continue;
				}
				else
				{
					if( !g_LevelMatchMgr.IsLadderLevelJoin( pRequestTeam->GetAbilityMatchLevel(), pNode->GetAbilityMatchLevel(), JOIN_CHECK_MINMAX_LEVEL ) ) continue;
				}
			}
			// 동일 인원 옵션 체크
			if( pRequestTeam->IsSearchSameUser() )
			{
				if( pRequestTeam->GetJoinUserCnt() != pNode->GetJoinUserCnt() ) continue;
			}

			// 상대팀 옵션 체크
			if( pNode->IsSearchLevelMatch() )
			{
				if( pNode->IsHeroMatchMode() )
				{
					if( !g_LevelMatchMgr.IsLadderHeroLevelJoin( pNode->GetAbilityMatchLevel(), pRequestTeam->GetAbilityMatchLevel(), JOIN_CHECK_MINMAX_LEVEL ) ) continue;
				}
				else
				{
					if( !g_LevelMatchMgr.IsLadderLevelJoin( pNode->GetAbilityMatchLevel(), pRequestTeam->GetAbilityMatchLevel(), JOIN_CHECK_MINMAX_LEVEL ) ) continue;
				}
			}
			// 동일 인원 옵션 체크
			if( pNode->IsSearchSameUser() )
			{
				if( pRequestTeam->GetJoinUserCnt() != pNode->GetJoinUserCnt() ) continue;
			}

			// 모드 체크
			if( pRequestTeam->GetSelectMode() != -1 && pNode->GetSelectMode() != -1 )
			{
				if( pNode->GetSelectMode() != pRequestTeam->GetSelectMode() ) continue;
			}
			// 맵 체크
			if( pRequestTeam->GetSelectMap() != -1 && pNode->GetSelectMap() != -1 )
			{
				if( pNode->GetSelectMap() != pRequestTeam->GetSelectMap() ) continue;
			}

			SortLadderTeam slt;
			slt.m_pNode  = (LadderTeamParent*)pNode;
			slt.m_iPoint = abs( pRequestTeam->GetAbilityMatchLevel() - pNode->GetAbilityMatchLevel() );
			vLadderTeamList.push_back( slt );
			break;
		}
	}

	if( vLadderTeamList.empty() )
	{
		//LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "SearchBattleMatch : 매칭팀 없음" );
		pRequestTeam->IncreaseSearchCount();
		return false;
	}
	//std::sort( vLadderTeamList.begin(), vLadderTeamList.end(), LadderTeamSort() );
	
	// 최상단 래더팀에 대전 신청
	SortLadderTeam &kLadderTeam = vLadderTeamList[0];
	if( !kLadderTeam.m_pNode ) return false;
	
	pRequestTeam->SetMatchReserve( kLadderTeam.m_pNode->GetIndex() );
	pRequestTeam->SetTeamState( LadderTeamParent::TMS_MATCH_RESERVE );
	pRequestTeam->InitSearchCount();
	//
	kLadderTeam.m_pNode->MatchRoomRequest( pRequestTeam->GetIndex() );

	//vLadderTeamList.clear();	
	return true;
}*/

int LadderTeamManager::GetLadderTeamUserCount()
{
	/*int iCount = 0;
	vLadderTeamNode_iter iter, iEnd;
	iEnd = m_vLadderTeamNode.end();
	for(iter = m_vLadderTeamNode.begin();iter != iEnd;++iter)
	{
		LadderTeamNode *pNode = *iter;
		iCount += pNode->GetJoinUserCnt();
	}
	return iCount;*/

	int iCount = 0;
	mLadderTeamNode_iter it = m_mLadderTeamNode.begin();
	
	for( ; it != m_mLadderTeamNode.end(); it++ )
	{
		LadderTeamNode *pNode =it->second;
		if( !pNode ) continue;

		iCount += pNode->GetJoinUserCnt();
	}
	
	return iCount;
}

int LadderTeamManager::GetSortLadderTeamPoint( SortLadderTeam &rkLadderTeam, UserParent *pUserParent )
{
	if( !rkLadderTeam.m_pNode || !pUserParent )
		return SORT_LADDERTEAM_POINT;

	int iReturnPoint = 0;
	// 길드 팀이면 최상위 위치함.
	if( rkLadderTeam.m_pNode->IsGuildTeam() && rkLadderTeam.m_pNode->GetGuildIndex() == pUserParent->GetGuildIndex() )
		return iReturnPoint;

    // 입장 불가능한 방
	if( rkLadderTeam.m_iState != SortLadderTeam::LTS_ACTIVE )         
		iReturnPoint = SORT_LADDERTEAM_POINT;

	// 다른 진영 팀
	if( rkLadderTeam.m_iState == SortLadderTeam::LTS_CAMP_NOT_MATCH )
		iReturnPoint += 1000000;

	// 현재 인원이 적당한 방 우선 순위 ( 4인 기준 )
	if( rkLadderTeam.m_iState == SortLadderTeam::LTS_ACTIVE )
		iReturnPoint += ( abs( 3 - rkLadderTeam.m_pNode->GetJoinUserCnt() ) ) * 100000;
	else
		iReturnPoint += ( abs( 4 - rkLadderTeam.m_pNode->GetJoinUserCnt() ) ) * 100000;

	// 방 최대 인원이 적당한 방 우선 순위 ( 4인 기준 )
	iReturnPoint += ( abs( 4 - rkLadderTeam.m_pNode->GetMaxPlayer() ) ) * 1000;

	// 실력차이가 적은 방 우선 순위
	iReturnPoint += abs( rkLadderTeam.m_pNode->GetAbilityMatchLevel() - pUserParent->GetKillDeathLevel() );

	return iReturnPoint;
}

int LadderTeamManager::GetSortLadderTeamState( LadderTeamParent *pLadderTeamParent, UserParent *pUserParent )
{
	if( !pLadderTeamParent || !pUserParent )
		return SortLadderTeam::LTS_FULL_USER;

	int iReturnState = SortLadderTeam::LTS_ACTIVE;
	if( pLadderTeamParent->GetCampType() != pUserParent->GetUserCampPos() )
		iReturnState = SortLadderTeam::LTS_CAMP_NOT_MATCH;
	else if( pLadderTeamParent->IsMatchPlay() )
		iReturnState = SortLadderTeam::LTS_MATCH_PLAY;
	else if( pLadderTeamParent->IsFull() )
		iReturnState = SortLadderTeam::LTS_FULL_USER;
	else if( pLadderTeamParent->GetJoinGuildIndex() != 0 && pLadderTeamParent->GetJoinGuildIndex() != pUserParent->GetGuildIndex() )
		iReturnState = SortLadderTeam::LTS_GUILD_TEAM_JOIN;
/*	else     팀 입장시 수준매칭 제거 
	{
		 //같은 팀이면 레벨 매칭 하지 않는다.
		if( pLadderTeamParent->IsGuildTeam() && pLadderTeamParent->GetGuildIndex() == pUserParent->GetGuildIndex() ) 
			iReturnState = SortLadderTeam::LTS_ACTIVE;
		else if( !g_LevelMatchMgr.IsLadderLevelJoin( pLadderTeamParent->GetAbilityMatchLevel(), pUserParent->GetKillDeathLevel(), JOIN_CHECK_MIN_LEVEL ) )
			iReturnState = SortLadderTeam::LTS_NOT_MIN_LEVEL_MATCH;
		else if( !g_LevelMatchMgr.IsLadderLevelJoin( pLadderTeamParent->GetAbilityMatchLevel(), pUserParent->GetKillDeathLevel(), JOIN_CHECK_MAX_LEVEL ) )
			iReturnState = SortLadderTeam::LTS_NOT_MAX_LEVEL_MATCH;
	}
*/
	return iReturnState;
}

void LadderTeamManager::SendCurLadderTeamList( User *pUser, int iPage, int iMaxCount, bool bHeroMatch )
{
	FUNCTION_TIME_CHECKER( 100000.0f, 0 );          // 0.1 초 이상 걸리면로그 남김

	if( pUser == NULL ) return;

	static vSortLadderTeam vLadderTeamList;
	vLadderTeamList.clear();

	/*vLadderTeamNode_iter iter, iEnd;
	iEnd = m_vLadderTeamNode.end();
	for(iter = m_vLadderTeamNode.begin();iter != iEnd;++iter)
	{
		LadderTeamNode *pNode = *iter;
		if( pNode == NULL ) continue;
		if( pNode->IsEmptyUser() ) continue;
		if( pNode->IsHeroMatchMode() != bHeroMatch ) continue;

		SortLadderTeam slt;
		slt.m_pNode  = (LadderTeamParent*)pNode;
		slt.m_iState = GetSortLadderTeamState( pNode, pUser );
		slt.m_iPoint = GetSortLadderTeamPoint( slt, pUser );

		vLadderTeamList.push_back( slt );
	}*/

	mLadderTeamNode_iter it = m_mLadderTeamNode.begin();

	for( ; it != m_mLadderTeamNode.end(); it++ )
	{
		LadderTeamNode *pNode = it->second;
		if( pNode == NULL ) continue;
		if( pNode->IsEmptyUser() ) continue;
		if( pNode->IsHeroMatchMode() != bHeroMatch ) continue;

		SortLadderTeam slt;
		slt.m_pNode  = (LadderTeamParent*)pNode;
		slt.m_iState = GetSortLadderTeamState( pNode, pUser );
		slt.m_iPoint = GetSortLadderTeamPoint( slt, pUser );

		vLadderTeamList.push_back( slt );
	}

	/*vLadderTeamCopyNode_iter iterCopy, iEndCopy;
	iEndCopy = m_vLadderTeamCopyNode.end();
	for( iterCopy=m_vLadderTeamCopyNode.begin() ; iterCopy!=iEndCopy ; ++iterCopy )
	{
		LadderTeamCopyNode *pNode = *iterCopy;
		if( pNode == NULL ) continue;
		if( pNode->IsEmptyUser() ) continue;
		if( pNode->IsHeroMatchMode() != bHeroMatch ) continue;

		SortLadderTeam slt;
		slt.m_pNode  = (LadderTeamParent*)pNode;
		slt.m_iState = GetSortLadderTeamState( pNode, pUser );
		slt.m_iPoint = GetSortLadderTeamPoint( slt, pUser );

		vLadderTeamList.push_back( slt );
	}*/

	mLadderTeamCopyNode_iter itCopy = m_mLadderTeamCopyNode.begin();
	for( ; itCopy != m_mLadderTeamCopyNode.end(); itCopy++ )
	{
		LadderTeamCopyNode *pNode = itCopy->second;
		if( pNode == NULL ) continue;
		if( pNode->IsEmptyUser() ) continue;
		if( pNode->IsHeroMatchMode() != bHeroMatch ) continue;

		SortLadderTeam slt;
		slt.m_pNode  = (LadderTeamParent*)pNode;
		slt.m_iState = GetSortLadderTeamState( pNode, pUser );
		slt.m_iPoint = GetSortLadderTeamPoint( slt, pUser );

		vLadderTeamList.push_back( slt );
	}

	int iMaxList = vLadderTeamList.size();
	if( iMaxList == 0 )
	{
		SP2Packet kPacket( STPK_LADDER_TEAM_LIST );
		kPacket << 0 << 0;
		pUser->SendMessage( kPacket );
		return;
	}
	std::sort( vLadderTeamList.begin(), vLadderTeamList.end(), LadderTeamSort() );

	int iStartPos = iPage * iMaxCount;
	int iCurSize  = max( 0, min( iMaxList - iStartPos, iMaxCount ) );

	SP2Packet kPacket( STPK_LADDER_TEAM_LIST );
	kPacket << iMaxList << iCurSize;
	for(int i = iStartPos; i < iStartPos + iCurSize;i++)
	{
		SortLadderTeam &kSortNode	= vLadderTeamList[i];
		LadderTeamParent *pLadderTeamNode = kSortNode.m_pNode;
		if( pLadderTeamNode )
		{
			kPacket << pLadderTeamNode->GetIndex() << pLadderTeamNode->GetCampType() << pLadderTeamNode->GetGuildIndex() << pLadderTeamNode->GetGuildMark() << pLadderTeamNode->GetSelectMode() << kSortNode.m_iState << pLadderTeamNode->GetTeamName() << pLadderTeamNode->GetPW() << pLadderTeamNode->GetJoinUserCnt() << pLadderTeamNode->GetMaxPlayer() 
					<< pLadderTeamNode->GetTeamLevel() << pLadderTeamNode->GetWinRecord() << pLadderTeamNode->GetLoseRecord() << pLadderTeamNode->GetVictoriesRecord();
		}		
		else    //예외
		{
			kPacket << 0 << 0 << 0 << 0 << 0 << "" << "" << 0 << 0 << 0 << 0 << 0;
		}
	}	
	pUser->SendMessage( kPacket );
	vLadderTeamList.clear();	
}

void LadderTeamManager::SendLadderTeamJoinInfo( UserParent *pUserParent, DWORD dwIndex )
{
	if( !pUserParent ) return;

	LadderTeamParent *pLadderTeam = GetGlobalLadderTeamNode( dwIndex );
	if( pLadderTeam )
		pLadderTeam->OnLadderTeamInfo( pUserParent );
}

void LadderTeamManager::SortLadderTeamRank( bool bHeroMatch, User *pSendUser /* = NULL */, DWORD dwTeamIndex /* = 0 */, int iCurPage /* = 0 */, int iMaxCount /* = 0  */ )
{
	FUNCTION_TIME_CHECKER( 100000.0f, 0 );          // 0.1 초 이상 걸리면로그 남김

	static vSortLadderTeam vLadderTeamList;
	vLadderTeamList.clear();

	/*vLadderTeamNode_iter iter, iEnd;
	iEnd = m_vLadderTeamNode.end();
	for(iter = m_vLadderTeamNode.begin();iter != iEnd;++iter)
	{
		LadderTeamNode *pNode = *iter;
		if( pNode == NULL ) continue;
		if( pNode->IsEmptyUser() ) continue;
		if( pNode->IsHeroMatchMode() != bHeroMatch ) continue;

		SortLadderTeam slt;
		slt.m_pNode  = (LadderTeamParent*)pNode;
		slt.m_iPoint = pNode->GetRankingPoint();

		vLadderTeamList.push_back( slt );
	}*/
	mLadderTeamNode_iter it = m_mLadderTeamNode.begin();

	for( ; it != m_mLadderTeamNode.end(); it++ )
	{
		LadderTeamNode *pNode = it->second;
		if( pNode == NULL ) continue;
		if( pNode->IsEmptyUser() ) continue;
		if( pNode->IsHeroMatchMode() != bHeroMatch ) continue;

		SortLadderTeam slt;
		slt.m_pNode  = (LadderTeamParent*)pNode;
		slt.m_iPoint = pNode->GetRankingPoint();

		vLadderTeamList.push_back( slt );
	}

	/*vLadderTeamCopyNode_iter iterCopy, iEndCopy;
	iEndCopy = m_vLadderTeamCopyNode.end();
	for(iterCopy = m_vLadderTeamCopyNode.begin();iterCopy != iEndCopy;++iterCopy)
	{
		LadderTeamCopyNode *pNode = *iterCopy;
		if( pNode == NULL ) continue;
		if( pNode->IsEmptyUser() ) continue;
		if( pNode->IsHeroMatchMode() != bHeroMatch ) continue;

		SortLadderTeam slt;
		slt.m_pNode  = (LadderTeamParent*)pNode;
		slt.m_iPoint = pNode->GetRankingPoint();

		vLadderTeamList.push_back( slt );
	}*/
	mLadderTeamCopyNode_iter itCopy = m_mLadderTeamCopyNode.begin();
	for( ; itCopy != m_mLadderTeamCopyNode.end(); itCopy++ )
	{
		LadderTeamCopyNode *pNode = itCopy->second;
		if( pNode == NULL ) continue;
		if( pNode->IsEmptyUser() ) continue;
		if( pNode->IsHeroMatchMode() != bHeroMatch ) continue;

		SortLadderTeam slt;
		slt.m_pNode  = (LadderTeamParent*)pNode;
		slt.m_iPoint = pNode->GetRankingPoint();

		vLadderTeamList.push_back( slt );
	}
	
	if( vLadderTeamList.empty() ) 
	{
		// 유저에게 팀 없음 전송
		if( pSendUser )       
		{
			SP2Packet kPacket( STPK_LADDER_TEAM_RANK_LIST );
			kPacket << bHeroMatch << iCurPage << 0 << 0;
			pSendUser->SendMessage( kPacket );
		}
		return;
	}
	std::sort( vLadderTeamList.begin(), vLadderTeamList.end(), LadderTeamSort() );

	int iMaxList = vLadderTeamList.size();

	// 양쪽진영 모든팀에 대한 랭킹
	int iRank = 1;
	for(int i = 0;i < iMaxList;i++)
	{
		SortLadderTeam &kSlt = vLadderTeamList[i];
		if( kSlt.m_pNode == NULL ) continue;

		// 랭킹 적용 : 원본 & 복사본 전부 갱신하다. 랭킹은 서버별로 동기화에 시간차가 발생 할 수있다.
		kSlt.m_pNode->UpdateRanking( iRank++ );
	}

	// 유저에게 팀 랭킹 리스트 전송
	if( pSendUser )       
	{
		int iMyTeamRank = 0;
		static vSortLadderTeam vLadderRankList;
		vLadderRankList.clear();

		for(int i = 0;i < iMaxList;i++)
		{
			SortLadderTeam &kSlt = vLadderTeamList[i];
			if( kSlt.m_pNode == NULL ) continue;

			/*        진영별 팀 랭킹
			if( kSlt.m_pNode->GetCampType() == iCampType )
			{
				vLadderRankList.push_back( vLadderTeamList[i] )	;
			}		
			*/
			
			if( dwTeamIndex == kSlt.m_pNode->GetIndex() )
				iMyTeamRank = kSlt.m_pNode->GetTeamRanking();

			// 양쪽진영 모든팀에 대한 랭킹
			vLadderRankList.push_back( vLadderTeamList[i] )	;
		}	
		vLadderTeamList.clear();
		iMaxList = vLadderRankList.size();

		// 특정 진영팀의 페이지를 호출한다.
		if( iMyTeamRank != 0 && iMaxCount != 0 )
		{
			iCurPage = max( iMyTeamRank - 1, 0 )  / iMaxCount;
		}

		int iStartPos = iCurPage * iMaxCount;
		int iCurSize  = max( 0, min( iMaxList - iStartPos, iMaxCount ) );

		SP2Packet kPacket( STPK_LADDER_TEAM_RANK_LIST );
		kPacket << bHeroMatch << iCurPage << iMaxList << iCurSize;
		for(int i = iStartPos; i < iStartPos + iCurSize;i++)
		{
			SortLadderTeam &kSortNode	= vLadderRankList[i];
			LadderTeamParent *pLadderTeamNode = kSortNode.m_pNode;
			if( pLadderTeamNode )
			{
				kPacket << pLadderTeamNode->GetIndex() << pLadderTeamNode->GetCampType() << pLadderTeamNode->GetTeamRanking() << pLadderTeamNode->GetTeamLevel() << pLadderTeamNode->GetTeamName()
					    << pLadderTeamNode->GetGuildIndex() << pLadderTeamNode->GetGuildMark() << pLadderTeamNode->GetPW() << pLadderTeamNode->GetWinRecord() << pLadderTeamNode->GetLoseRecord() << pLadderTeamNode->GetVictoriesRecord()
						<< pLadderTeamNode->GetJoinUserCnt() << pLadderTeamNode->GetMaxPlayer();
			}		
			else    //예외
			{
				kPacket << 0 << 0 << 0 << "" << 0 << 0 << "" << 0 << 0 << 0 << 0 << 0;
			}
		}	
		pSendUser->SendMessage( kPacket );
		vLadderRankList.clear();
	}		
}

void LadderTeamManager::UpdateProcess()
{
	if( TIMEGETTIME() - m_dwCurTime < 1000 ) return;
	m_dwCurTime = TIMEGETTIME();


	FUNCTION_TIME_CHECKER( 100000.0f, 0 );          // 0.1 초 이상 걸리면로그 남김

	static vLadderTeamNode vDestroyNode;
	vDestroyNode.clear();

	/*vLadderTeamNode_iter iter, iEnd;
	iEnd = m_vLadderTeamNode.end();
	for(iter = m_vLadderTeamNode.begin();iter != iEnd;++iter)
	{
		LadderTeamNode *pNode = *iter;
		if( pNode->IsReserveTimeOver() )
			vDestroyNode.push_back( pNode );
		else
			pNode->Process();
	}*/
	mLadderTeamNode_iter it = m_mLadderTeamNode.begin();
	for( ; it != m_mLadderTeamNode.end(); it++ )
	{
		LadderTeamNode *pNode = it->second;
		if( !pNode ) continue;

		if( pNode->IsReserveTimeOver() )
			vDestroyNode.push_back( pNode );
		else
			pNode->Process();
	}

	vLadderTeamNode_iter iter, iEnd;
	if( !vDestroyNode.empty() )
	{
		iEnd = vDestroyNode.end();
		for(iter = vDestroyNode.begin();iter != iEnd;++iter)
		{
			RemoveLadderTeam( *iter );
		}
		vDestroyNode.clear();
	}

	// 서칭 중인 녀석들중에 존재 하지 않는 것들 삭제
	/*DWORDVec::iterator dwIter;

	dwIter = m_vSearchingTeamList.begin();
	while( dwIter != m_vSearchingTeamList.end() )
	{
		it =  m_mLadderTeamNode.find(*dwIter);
		if( it == m_mLadderTeamNode.end() )
		{
			dwIter = m_vSearchingTeamList.erase(dwIter);
		}
		else
			dwIter++;
	}

	dwIter = m_vSearchingLadderList.begin();
	while( dwIter != m_vSearchingLadderList.end() )
	{
		it =  m_mLadderTeamNode.find(*dwIter);
		if( it == m_mLadderTeamNode.end() )
		{
			dwIter = m_vSearchingLadderList.erase(dwIter);
		}
		else
			dwIter++;
	}*/
}

void LadderTeamManager::RemoveSearchingList(DWORD dwIndex,  bool bLadder)
{
	if( bLadder )
	{
		RemoveSearchingLadderList(dwIndex);
	}
	else
	{
		RemoveSearchingTeamList(dwIndex);
	}
}

void LadderTeamManager::AddSearchingList(DWORD dwIndex,  bool bLadder)
{
	if( bLadder )
	{
		AddSearchingLadderList(dwIndex);
	}
	else
	{
		AddSearchingTeamList(dwIndex);
	}
}

void LadderTeamManager::RemoveSearchingTeamList(DWORD dwIndex)
{
	DWORDVec::iterator it = m_vSearchingTeamList.begin();

	for( ; it != m_vSearchingTeamList.end(); it++ )
	{
		if( dwIndex == *it )
		{
			m_vSearchingTeamList.erase(it);
			return;
		}
	}

	//LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[warnig][ladderteam]This index is not searching state : [%d]", dwIndex );
}

void LadderTeamManager::RemoveSearchingLadderList(DWORD dwIndex)
{
	DWORDVec::iterator it = m_vSearchingLadderList.begin();

	for( ; it != m_vSearchingLadderList.end(); it++ )
	{
		if( dwIndex == *it )
		{
			m_vSearchingLadderList.erase(it);
			return;
		}
	}

	//LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[warnig][ladder]This index is not searching state : [%d]", dwIndex );
}

void LadderTeamManager::AddSearchingTeamList(DWORD dwIndex)
{
	DWORDVec::iterator it;
	for( it = m_vSearchingTeamList.begin(); it != m_vSearchingTeamList.end(); it++ )
	{
		if( dwIndex == *it )
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[warnig][ladderteam]This index is already exist : [%d]", dwIndex );
			return;
		}
	}

	m_vSearchingTeamList.push_back(dwIndex);
}

void LadderTeamManager::AddSearchingLadderList(DWORD dwIndex)
{
	DWORDVec::iterator it;
	for( it = m_vSearchingLadderList.begin(); it != m_vSearchingLadderList.end(); it++ )
	{
		if( dwIndex == *it )
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[warnig][ladder]This index is already exist : [%d]", dwIndex );
			return;
		}
	}

	m_vSearchingLadderList.push_back(dwIndex);
}

void LadderTeamManager::RemoveCopyNodeSearchingList(DWORD dwIndex,  bool bLadder)
{
	if( bLadder )
	{
		RemoveCopyNodeSearchingLadderList(dwIndex);
	}
	else
	{
		RemoveCopyNodeSearchingTeamList(dwIndex);
	}
}

void LadderTeamManager::AddCopyNodeSearchingList(DWORD dwIndex,  bool bLadder)
{
	if( bLadder )
	{
		AddCopyNodeSearchingLadderList(dwIndex);
	}
	else
	{
		AddCopyNodeSearchingTeamList(dwIndex);
	}
}

void LadderTeamManager::RemoveCopyNodeSearchingTeamList(DWORD dwIndex)
{
	DWORDVec::iterator it = m_vCopyNodeSearchingTeamList.begin();

	for( ; it != m_vCopyNodeSearchingTeamList.end(); it++ )
	{
		if( dwIndex == *it )
		{
			m_vCopyNodeSearchingTeamList.erase(it);
			return;
		}
	}

	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[warnig][ladderteam]This copy node index is not searching state : [%d]", dwIndex );
}

void LadderTeamManager::RemoveCopyNodeSearchingLadderList(DWORD dwIndex)
{
	DWORDVec::iterator it = m_vCopyNodeSearchingLadderList.begin();

	for( ; it != m_vCopyNodeSearchingLadderList.end(); it++ )
	{
		if( dwIndex == *it )
		{
			m_vCopyNodeSearchingLadderList.erase(it);
			return;
		}
	}

	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[warnig][ladder]This copy node index is not searching state : [%d]", dwIndex );
}

void LadderTeamManager::AddCopyNodeSearchingTeamList(DWORD dwIndex)
{
	DWORDVec::iterator it = m_vCopyNodeSearchingTeamList.begin();
	for( ; it != m_vCopyNodeSearchingTeamList.end(); it++ )
	{
		if( dwIndex == *it )
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[warnig][ladderteam]This copy node index is already exist : [%d]", dwIndex );
			return;
		}
	}

	m_vCopyNodeSearchingTeamList.push_back(dwIndex);
}

void LadderTeamManager::AddCopyNodeSearchingLadderList(DWORD dwIndex)
{
	DWORDVec::iterator it = m_vCopyNodeSearchingLadderList.begin();
	for( ; it != m_vCopyNodeSearchingLadderList.end(); it++ )
	{
		if( dwIndex == *it )
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[warnig][ladder]This copy node index is already exist : [%d]", dwIndex );
			return;
		}
	}

	m_vCopyNodeSearchingLadderList.push_back(dwIndex);
}