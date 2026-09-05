#include "../stdafx.h"

#include "MatchNodeManager.h"
#include "MatchNode.h"
#include "ServerNodeManager.h"
#include <strsafe.h>

MatchNodeManager* MatchNodeManager::sg_Instance = NULL;
MatchNodeManager::MatchNodeManager()
{
	m_iTopRankYear = 0;
	m_iTopRankMonth = 0;
	m_iTopRankDay = 0;
	m_iWeekStartHour = 0;
	m_iWeekEndHour = 0;
	m_iWeekendStartHour = 0;
	m_iWeekendEndHour = 0;
	m_iVacationStartMonth = 0;
	m_iVacationEndMonth = 0;
	m_iVacationStartDay = 0;
	m_iVacationEndDay = 0;
	m_iVacationStartHour = 0;
	m_iVacationEndHour = 0;
	m_bActive = false;
	m_mMatchUser.clear();
	m_vTierPoint.clear();
	m_vWinPoint.clear();
	m_vLosePoint.clear();
	m_iScale = 0;
	m_iExpansionTime = 0;
}

MatchNodeManager::~MatchNodeManager()
{
}

MatchNodeManager &MatchNodeManager::GetInstance()
{
	if( sg_Instance == NULL )
		sg_Instance = new MatchNodeManager;
	return *sg_Instance;
}

void MatchNodeManager::ReleaseInstance()
{
	SAFEDELETE( sg_Instance );
}

void MatchNodeManager::LoadINI()
{
	m_vTierPoint.clear();
	m_vWinPoint.clear();
	m_vLosePoint.clear();
	m_vPlayTier.clear();

	//ioINILoader kLoader( "config/sp2_ranking.ini" );
	ioINILoader kLoader;
	kLoader.ReloadFile( "config/sp2_ranking.ini" );

	LOG.PrintTimeAndLog( 0, "[info][match] TopRank Update : %d/%d/%d", m_iTopRankYear, m_iTopRankMonth, m_iTopRankDay );

	kLoader.SetTitle( "RankSeason" );
	m_iWeekStartHour	        = kLoader.LoadInt( "week_starthour", 0);
	m_iWeekEndHour			    = kLoader.LoadInt( "week_endhour", 0 );	

	m_iWeekendStartHour	        = kLoader.LoadInt( "weekend_starthour", 0);
	m_iWeekendEndHour		    = kLoader.LoadInt( "weekend_endhour", 0 );	

	m_iVacationStartMonth       = kLoader.LoadInt( "vacation_startmonth", 0);
	m_iVacationEndMonth		    = kLoader.LoadInt( "vacation_endmonth", 0 );	
	
	m_iVacationStartDay         = kLoader.LoadInt( "vacation_startday", 0);
	m_iVacationEndDay		    = kLoader.LoadInt( "vacation_endday", 0 );	

	m_iVacationStartHour        = kLoader.LoadInt( "vacation_starthour", 0);
	m_iVacationEndHour		    = kLoader.LoadInt( "vacation_endhour", 0 );	

	kLoader.SetTitle( "RankTier" );
	m_iScale = kLoader.LoadInt( "scale", 0 );
	m_iExpansionTime = kLoader.LoadInt( "expansion_time", 0 );
	m_iWinStreakScale	= kLoader.LoadInt( "winstreak_scale", 0 );
	m_iLoseStreakScale	= kLoader.LoadInt( "losestreak_scale", 0 );
	m_iMatchTimeout	= kLoader.LoadInt( "matching_timeout", 120 );
	 
	m_iMatchStep = kLoader.LoadInt( "count", 0 );
	for( int i=0 ;i < m_iMatchStep; i++ )
	{
		int ipoint = 0;
		char szKey[MAX_PATH];
		sprintf_s( szKey, "Tier%d", i + 1 );
		ipoint = kLoader.LoadInt( szKey, 0 );
		m_vTierPoint.push_back( ipoint );
	}

	kLoader.SetTitle( "RankPoint" );
	int pcount = kLoader.LoadInt( "pcount", 0 );
	for( int i=0 ;i < pcount ; i++ )
	{
		int iwinpoint = 0, ilosepoint = 0, itier = 0;
		char szKey1[MAX_PATH];
		char szKey2[MAX_PATH];
		char szKey3[MAX_PATH];
		sprintf_s( szKey1, "winpoint%d", i + 1 );
		sprintf_s( szKey2, "losepoint%d", i + 1 );
		sprintf_s( szKey3, "tier%d", i + 1 );
		iwinpoint = kLoader.LoadInt( szKey1, 0 );
		ilosepoint = kLoader.LoadInt( szKey2, 0 );
		itier = kLoader.LoadInt( szKey3, 0 );
		m_vWinPoint.push_back( iwinpoint );
		m_vLosePoint.push_back( ilosepoint );
		m_vPlayTier.push_back( itier );
	}

	LOG.PrintTimeAndLog( 0, "[info][match] ini load : tiercount[%d] stepcount[%d] scale[%d] winstreakScale[%d] losestreakScale[%d] expansiontime[%d]", 
		m_iMatchStep, pcount, m_iScale, m_iWinStreakScale, m_iLoseStreakScale, m_iExpansionTime );
}

void MatchNodeManager::InitMemoryPool()
{
	int iNodeSize = 0, iUserSize = 0;
	{
		ioINILoader kLoader( "ls_config_main.ini" );

		kLoader.SetTitle( "MemoryPool" );
		iNodeSize = kLoader.LoadInt( "matchnode_pool", 5000 );
		iUserSize = kLoader.LoadInt( "matchuser_pool", 10000 );
	}

	m_MatchMemNode.CreatePool( 0, iNodeSize, FALSE );
	for(int i = 0 ; i < iNodeSize ; i++)
	{
		m_MatchMemNode.Push( new MatchNode( i ) );
	}
	
	LOG.PrintTimeAndLog( 0, "[info][match]Init memory node pool size : [%d]", iNodeSize );

	m_MatchMemNode.CreatePool( 0, iUserSize, FALSE );
	for(int j = 0 ; j < iUserSize ; j++)
	{
		m_UserMemNode.Push( new MatchUser() );
	}
	
	LOG.PrintTimeAndLog( 0, "[info][match]Init memory user pool size : [%d]", iUserSize );

	LoadINI();
}

void MatchNodeManager::ReleaseMemoryPool()
{
	mMatchUser_iter iter  = m_mMatchUser.begin();

	for( ; iter != m_mMatchUser.end(); iter++ )
	{
		MatchUser *pNode = iter->second;
		if( !pNode ) continue;

		pNode->Init();
		m_UserMemNode.Push( pNode );
	}

	m_mMatchUser.clear();
	m_UserMemNode.DestroyPool();
}

MatchNode* MatchNodeManager::CreateMatchNode()
{
	MatchNode* pNewNode = (MatchNode *)m_MatchMemNode.Remove();
	if( !pNewNode )
	{
		LOG.PrintTimeAndLog( 0,"MatchNodeManager::CreateMatchNode MemPool Zero!");
		return NULL;
	}

	LOG.PrintTimeAndLog( 0, "[info][match] CreateMatchNode index[%d]", pNewNode->GetIndex() );
	return pNewNode;
}

void MatchNodeManager::RemoveMatchNode( MatchNode *pNode )
{	

}


MatchUser* MatchNodeManager::CreateMatchUser( DWORD dwUserIndex, int iMatchPoint, int iWinStreakCount, int iLoseStreakCount )
{
	MatchUser* pMatchUser = (MatchUser *)m_UserMemNode.Remove();
	if( !pMatchUser )
	{
		LOG.PrintTimeAndLog( 0,"MatchNodeManager::CreateMatchUser MemPool Zero!");
		return NULL;
	}

	int istart = 0, iend = 0, iTierPoint = 0;
	//유저의 MMR 구간 구하기
	GetTierPoint( iMatchPoint, istart, iend, iTierPoint );		

	m_mMatchUser.insert( make_pair(dwUserIndex, pMatchUser) );
	pMatchUser->SetMatchUser( dwUserIndex, iMatchPoint, iWinStreakCount, iTierPoint, iLoseStreakCount );
	
	pMatchUser->SetStartTime();	

	LOG.PrintTimeAndLog( 0,"[info][match] create match user [%d] point[%d] MMR[%d] startTier: %d, EndTier : %d WINSTREAK[%d], LOSESTREAKCOUNT[%d]",		
		dwUserIndex, iMatchPoint, iTierPoint, istart, iend, iWinStreakCount, iLoseStreakCount );

	return pMatchUser;
}


void MatchNodeManager::RemoveMatchUser( MatchUser* pUser )
{	
	if( !pUser )
		return;

	// 대기 유저 삭제 상태로 변경
	mMatchUser_iter findIter	= m_mMatchUser.find(pUser->GetUserIndex());
	if( findIter != m_mMatchUser.end() )
	{
		MatchUser* pCursor = findIter->second;
		if( pCursor )
		{
			pCursor->SetMatchState( MatchUser::MatchDelete );
		}
	}
	else
	{
		LOG.PrintTimeAndLog( 0,"[warning][match]None exist reserve matchuser Index so can not be deleted" );
	}
}

void MatchNodeManager::MatchProcess()
{
	
	// 유저 수 조회
	char szBuf[MAX_PATH]="";
	char szLog[MAX_PATH];
	memset( szLog, 0, sizeof( szLog ) );

	int iMemberCount  = 0;

	for( int i = 0; i< m_iMatchStep; i++ )
	{
		iMemberCount = m_mMachingMap.count( i );
		sprintf_s( szBuf, MAX_PATH,  " Tier[%d]:[%d] ", i, iMemberCount );
		strcat_s( szLog, szBuf );
	}

	LOG.PrintTimeAndLog( 0,"[info][match] Waiting User Count %s", szLog );

	// 매칭 맵 & 매칭 대기 유저 맵 동시 삭제
	for( mUserMatchingMap::iterator mapIter = m_mMachingMap.begin() ; mapIter != m_mMachingMap.end() ; )
	{
		MatchUser* pUserMap = mapIter->second;
		if( pUserMap && pUserMap->GetMatchState() == MatchUser::MatchDelete )
		{
			//// 대기 유저 맵에서 삭제 
			mMatchUser_iter Iter	= m_mMatchUser.find( pUserMap->GetUserIndex() );

			if( Iter != m_mMatchUser.end() )
			{
				MatchUser* pUserList = Iter->second;
				
				if( pUserList )
				{
					//m_UserMemNode.Push( pUserList );	//회수
					m_mMatchUser.erase(Iter++);			//삭제
					printf("대기유저맵 %d 삭제\n", pUserList->GetUserIndex() );
					
					LOG.PrintTimeAndLog( 0,"[info][match] Remove match user in userList [%d] point[%d] tier[%d] win[%d]", pUserList->GetUserIndex(), 
						pUserList->GetMatchPoint(), pUserList->GetUserTierPoint(), pUserList->GetWinStreakCount());
				}
			}

			LOG.PrintTimeAndLog( 0,"[info][match] Remove match user in MatchingMap [%d] point[%d] tier[%d] win[%d]", pUserMap->GetUserIndex(), 
				pUserMap->GetMatchPoint(), pUserMap->GetUserTierPoint(), pUserMap->GetWinStreakCount());

			printf("검색맵 %d 삭제함\n", pUserMap->GetUserIndex() );
			m_UserMemNode.Push( pUserMap );		//회수
			pUserMap->Init();
			m_mMachingMap.erase(mapIter++);			//삭제

			

		}
		else
		{
			mapIter++;
		}
	}

	// 매칭 맵에서 조회 되지 않아서 유저 맵에 남아있는 경우
	for( mMatchUser_iter iter = m_mMatchUser.begin() ; iter != m_mMatchUser.end() ; )
	{
		MatchUser* pUser = iter->second;
		if( pUser && pUser->GetMatchState() == MatchUser::MatchDelete )
		{
			//printf("%d 삭제함\n", pUser->GetUserIndex() );
			//pUser->Init();
			m_UserMemNode.Push( pUser );		//회수
			m_mMatchUser.erase(iter++);			//삭제
			
			LOG.PrintTimeAndLog( 0,"[info][match] Remove match user in userList [%d] point[%d] tier[%d] win[%d]", pUser->GetUserIndex(), 
				pUser->GetMatchPoint(), pUser->GetUserTierPoint(), pUser->GetWinStreakCount());
		}
		else
		{
			iter++;
		}
	}

	

	for(mMatchUser_iter iter = m_mMatchUser.begin() ; iter != m_mMatchUser.end() ; ++iter)
	{
		MatchUser* pUser = iter->second;
		if( !pUser )
			continue;

		if( pUser->IsTimeOver() ) // 노드에 대기 중인 유저도 타임아웃 검사. 180 초 지난 유저
		{
			SendMatchErrorResult( pUser, MATCH_TIMEOVER );
			continue;
		}

		if( pUser->GetMatchState() == MatchUser::MatchEnter ) // 노드에 추가된 유저는 패스
			continue;

		if( pUser->GetMatchState() == MatchUser::MatchDelete ) // 매칭 성공 후 삭제 대기 유저 패스
			continue;

		DoMatchUser( pUser );
	}

	
}

void MatchNodeManager::DoMatchUser( MatchUser* pUser )
{
	int nMaching = 0, iscale = 0, iDelayTime = 0;
	
	int iINIScale = GetScale();

	CTime ctCurTime = CTime::GetCurrentTime();
	iDelayTime = g_MatchNodeManager.GetExpansionTime();			//ini 세팅 값
	CTimeSpan ctGapTime = ctCurTime - pUser->GetStartctTime();
	
	if( iINIScale > 0 && iDelayTime > 0)
		iscale = ctGapTime.GetTotalSeconds() / iDelayTime;
	else
		iscale = 0;

	// 매칭 진행 안했던 유저
	if( pUser->GetUserScale() == 0 )
	{
		pUser->SetUserScale( 1 );
		m_mMachingMap.insert( make_pair(pUser->GetUserTierPoint(), pUser ) );
	}

	// MMR 범위 존재 유무 검색
	//nMaching = m_mMachingMap.count( pUser->GetUserTierPoint() );
	
	MatchUser* pCursor = NULL;
	mUserMatchingMap::iterator iter = m_mMachingMap.end();

	// case1. 나와 동일한 MMR ?
	pCursor = GetMatchingUser( pUser, pUser->GetUserTierPoint(), iter );
	if( pCursor )
	{
		SendMatchSuccessResult( pUser, pCursor );
		//m_mMachingMap.erase( iter );
		
		printf("%d:%d, :%d:%d, %d:%d 동일 티어 찾음\n", pUser->GetUserIndex(), pCursor->GetUserIndex(),
			pUser->GetMatchPoint(), pCursor->GetMatchPoint(),
			pUser->GetWinStreakCount(), pCursor->GetWinStreakCount()
			);
		return;
	}

	// case2. 확장 검색 시간이 되었는가 ?
	/*
	iniScale : 4 
	iDelayTime : 10sec
	-> 유저 매칭 대기 시간 : 54초 지난 경우
	-> iscale : 5   
	*/

	if( iINIScale < iscale )
		iscale = iINIScale;
	
	// 확장 시간 안됨 패스
	if( iscale == 0 )
	{
		return;
	}
		
	pCursor = GetMatchingUser( pUser, pUser->GetUserTierPoint() + iscale, iter );
	if( pCursor )
	{
		SendMatchSuccessResult( pUser, pCursor );
		//m_mMachingMap.erase( iter );
		printf("%d+%d 티어 찾음\n", pUser->GetUserIndex(), pUser->GetUserTierPoint() + iscale );
		return;
	}

	pCursor = GetMatchingUser( pUser, pUser->GetUserTierPoint() - iscale, iter );
	if( pCursor )
	{
		SendMatchSuccessResult( pUser, pCursor );
		//m_mMachingMap.erase( iter );
		printf("%d-%d 티어 찾음\n", pUser->GetUserIndex(), pUser->GetUserTierPoint() - iscale );
		return;
	}
		
	return ;
}

bool MatchNodeManager::IsExistMatchUser( DWORD dwUserIndex )
{
	mMatchUser_iter findIter	= m_mMatchUser.find(dwUserIndex);
	if( findIter == m_mMatchUser.end() )
		return false;

	return true;
}

MatchUser* MatchNodeManager::FindMatchUser( DWORD dwUserIndex )
{
	mMatchUser_iter findIter	= m_mMatchUser.find(dwUserIndex);
	if( findIter == m_mMatchUser.end() )
		return NULL;

	return findIter->second;
}

void MatchNodeManager::SendMatchErrorResult( MatchUser* pUser, int iResult )
{
	int iStartTier = 0, iEndTier = 0, iTemp = 0;
	GetTierPoint( pUser->GetMatchPoint(), iStartTier, iEndTier, iTemp );

	SP2Packet kPacket( MSTPK_SUCCESSION_MATCHING_REQUEST );
	PACKET_GUARD_VOID( kPacket.Write( iResult ) );
	PACKET_GUARD_VOID( kPacket.Write( pUser->GetUserIndex() ) );
	PACKET_GUARD_VOID( kPacket.Write( iStartTier ) );
	PACKET_GUARD_VOID( kPacket.Write( iEndTier ) );

	g_ServerNodeManager.SendMessageAllNode( kPacket );

	LOG.PrintTimeAndLog( 0,"[info][match] failed result[%d] user[%d] point[%d] Tier[%d] Win[%d]", iResult, pUser->GetUserIndex(), pUser->GetMatchPoint(), 
		pUser->GetUserTierPoint(), pUser->GetWinStreakCount() );
	
	RemoveMatchUser( pUser );
}


void MatchNodeManager::SendMatchSuccessResult( MatchUser* pNode, MatchUser* pCompetitionNode )
{

	mMatchUser_iter Iter	= m_mMatchUser.find( pNode->GetUserIndex() );
	mMatchUser_iter competitionIter	= m_mMatchUser.find( pCompetitionNode->GetUserIndex() );

	if( Iter != m_mMatchUser.end() && competitionIter != m_mMatchUser.end() )
	{
		MatchUser* pUser1 = Iter->second;
		MatchUser* pUser2 = competitionIter->second;

		
		if( pUser1 && pUser2 )
		{
			pUser1->SetMatchState( MatchUser::MatchDelete );
			pUser2->SetMatchState( MatchUser::MatchDelete );

			int iStartTier = 0, iEndTier = 0, iTemp = 0;
			GetTierPoint( pUser1->GetMatchPoint(), iStartTier, iEndTier, iTemp );

			///매칭 맵에서 삭제해야함

			SP2Packet kPacket( MSTPK_SUCCESSION_MATCHING_REQUEST );
			PACKET_GUARD_VOID( kPacket.Write( MATCH_SUCCESS ) );
			PACKET_GUARD_VOID( kPacket.Write( pUser1->GetUserIndex() ) );
			PACKET_GUARD_VOID( kPacket.Write( pUser2->GetUserIndex() ) );
			PACKET_GUARD_VOID( kPacket.Write( iStartTier ) );
			PACKET_GUARD_VOID( kPacket.Write( iEndTier ) );

			g_ServerNodeManager.SendMessage( kPacket );


			printf("[info][match] success index[%d]:MMR[%d]:WINSTREK[%d], competitor index[%d]:MMR[%d]:WINSTREK[%d]\n",
				pUser1->GetUserIndex(), pUser1->GetUserTierPoint(), pUser1->GetWinStreakCount(), 
				pUser2->GetUserIndex(), pUser2->GetUserTierPoint(), pUser2->GetWinStreakCount() );


			LOG.PrintTimeAndLog( 0,"[info][match] success index[%d]:MMR[%d]:WINSTREK[%d]:LOSESTREAK[%d], competitor index[%d]:MMR[%d]:WINSTREK[%d]:LOSESTREAK[%d]",
				pUser1->GetUserIndex(), pUser1->GetUserTierPoint(), pUser1->GetWinStreakCount(), pUser1->GetLoseStreakCount(),
				pUser2->GetUserIndex(), pUser2->GetUserTierPoint(), pUser2->GetWinStreakCount(), pUser2->GetLoseStreakCount());
		}

	}

}

void MatchNodeManager::RankSeasonProcess()
{
	CTime ct = CTime::GetCurrentTime();

	// TopRank
	if( ct.GetYear() != m_iTopRankYear || ct.GetMonth() != m_iTopRankMonth || ct.GetDay() != m_iTopRankDay )
	{
		// file write
		ioINILoader kLoader( "config/sp2_ranking.ini" );
		kLoader.SaveInt( "TopRank", "year", (int)ct.GetYear() );
		kLoader.SaveInt( "TopRank", "month", (int)ct.GetMonth() );
		kLoader.SaveInt( "TopRank", "day", (int)ct.GetDay() );

		m_iTopRankYear = ct.GetYear();
		m_iTopRankMonth = ct.GetMonth();
		m_iTopRankDay = ct.GetDay();
	}

	// Season
	bool bSeason = false;
	DWORD dwVacationStart = m_iVacationStartMonth * 100 + m_iVacationStartDay;
	DWORD dwVacationEnd = m_iVacationEndMonth * 100 + m_iVacationEndDay;
	DWORD dwCurrent = ct.GetMonth() * 100 + ct.GetDay();

	if( dwVacationStart <= dwCurrent && dwCurrent <= dwVacationEnd )
	{
		if( m_iVacationStartHour <= ct.GetHour() && ct.GetHour() <= m_iVacationEndHour )
			bSeason = true;
	}
	else
	{
		if( ct.GetDayOfWeek() == 1 || ct.GetDayOfWeek() == 7 )
		{
			if( m_iWeekStartHour <= ct.GetHour() && ct.GetHour() <= m_iWeekEndHour )
				bSeason = true;	
			
		}
		else
		{
			if( m_iWeekendStartHour <= ct.GetHour() && ct.GetHour() <= m_iWeekendEndHour )
				bSeason = true;	
		}
	}
	/*
	SP2Packet kPacket( MSTPK_MATCH_RANKSEASON_SYNC );
	PACKET_GUARD_VOID( kPacket.Write( m_iTopRankYear ) );
	PACKET_GUARD_VOID( kPacket.Write( m_iTopRankMonth ) );
	PACKET_GUARD_VOID( kPacket.Write( m_iTopRankDay ) );
	PACKET_GUARD_VOID( kPacket.Write( bSeason ) );
	int count = (int)m_vPlayTier.size();
	PACKET_GUARD_VOID( kPacket.Write( count ) );
	for( int i=0 ; i<count ; ++i )
	{
		PACKET_GUARD_VOID( kPacket.Write( m_vWinPoint[i] ) );
		PACKET_GUARD_VOID( kPacket.Write( m_vLosePoint[i] ) );
		PACKET_GUARD_VOID( kPacket.Write( m_vPlayTier[i] ) );
	}

	g_ServerNodeManager.SendMessageAllNode( kPacket );
	*/
}

void MatchNodeManager::GetTierPoint( int iMatchPoint, int& iStartTier, int& iEndTier, int& iTierPoint )
{
	int icount = (int)m_vTierPoint.size();

	for( int i = 0; i< icount; i++ )
	{
		if( iMatchPoint < 0 )
		{
			iStartTier = -1;
			iEndTier = 0; 
			iTierPoint = 0;
			return;
		}
		// MMR 포인트가 Tier1 보다 작은 경우
		if( m_vTierPoint[i] > iMatchPoint )
		{
			iStartTier = 0;
			iEndTier = m_vTierPoint[i];
			iTierPoint = i+1;
			return;
		}

		// 세팅된 Tier 포인트보다 유저MMR 이 높은경우
		if( i == icount-1 )
		{
			iStartTier = m_vTierPoint[i];
			iEndTier = m_vTierPoint[i];
			iTierPoint = i+1;
			return;
		}

		if( m_vTierPoint[i] <= iMatchPoint && m_vTierPoint[i+1] > iMatchPoint )
		{
			iStartTier = m_vTierPoint[i];
			iEndTier = m_vTierPoint[i+1];
			iTierPoint = i+1;
			return;
		}

		
	}

	/*
	for( int i=0 ; i<icount ; ++i )
	{
		if( i == 0 || icount < 2 )
		{
			if( iMatchPoint <= m_vTierPoint[i])
			{
				iStartTier = 0;
				iEndTier = m_vTierPoint[i];
				iTierPoint = i;
				return;
			}
			else
			{
				if( icount > 1 )
				{
					if( iMatchPoint > m_vTierPoint[i] && iMatchPoint <= m_vTierPoint[i+1] )
					{
						iStartTier = m_vTierPoint[i];
						iEndTier = m_vTierPoint[i+1];
						iTierPoint = i;
						return;
					}
				}
			}
		}
		else if( i == icount-1 )
		{
			if( iMatchPoint > m_vTierPoint[i])
			{
				iStartTier = m_vTierPoint[i];
				iEndTier = 0;
				iTierPoint = i;
				return;
			}
		}
		else
		{
			if( iMatchPoint > m_vTierPoint[i] && iMatchPoint <= m_vTierPoint[i+1] )
			{
				iStartTier = m_vTierPoint[i];
				iEndTier = m_vTierPoint[i+1];
				iTierPoint = i;
				return;
			}
		}
	}
	*/
}

// 연승찾기
MatchUser* MatchNodeManager::GetMatchingUser( MatchUser* pUser, int iTierPoint, mUserMatchingMap::iterator& cursorIter )
{
	std::vector< MatchUser* > vMatchNodeSort;
	mUserMatchingMap::iterator sortIter;

	cursorIter = m_mMachingMap.end();		
	sortIter = m_mMachingMap.end();		//같은 구간 내 유저 MMR 포인트로 정렬

	// 나와 같은 Tier 구간 유저 추출
	m_machingRangeIter = m_mMachingMap.equal_range( iTierPoint );

	MatchUser* pCursor = NULL;


	//HRYOON 2017.02.08 적용 예정
	for( sortIter  = m_machingRangeIter.first;  sortIter != m_machingRangeIter.second;  sortIter++ )
	{
		MatchUser* pTemp = NULL;
		pTemp = sortIter->second;
		if( pTemp->GetMatchState() != MatchUser::MatchDelete )
			vMatchNodeSort.push_back( pTemp );
	}
	
	std::sort( vMatchNodeSort.begin(), vMatchNodeSort.end(), MatchSort() );
	


	for( cursorIter  = m_machingRangeIter.first;  cursorIter != m_machingRangeIter.second;  cursorIter++ )
	{
		pCursor = cursorIter->second;
		printf("총 : %d, 찾은갯수 : %d, %d\n", m_mMachingMap.size(), m_mMachingMap.count(iTierPoint), pCursor->GetUserIndex());
		if( !pCursor )
			continue;
	
		if( pCursor->GetUserIndex() == pUser->GetUserIndex() )
		{
			pCursor = NULL;
			continue;
		}
	
		if( pCursor->GetUserIndex() == 0 )
		{
			pCursor = NULL;
			continue;
		}
	
		if( pCursor->GetMatchState() == MatchUser::MatchDelete )
		{
			pCursor = NULL;
			continue;
		}

		// 연승 확장 스케일
		int iWinINIScale = GetWinStreakScale();
		int iLoseINIScale = GetLoseStreakScale();
		int nCount = 0;
		
		
		int iWinStreakCount = pUser->GetWinStreakCount();
		// 현재 연승이 0인 경우 연패에서 찾음
		if( iWinStreakCount == 0 )
		{
			for( nCount= 1; nCount <= iLoseINIScale; nCount++ )
			{
				// 같은 연패 찾음
				if ( pCursor->GetLoseStreakCount() == pUser->GetLoseStreakCount() )
				{
					printf("같은 연패 찾음%d, %d\n", pUser->GetUserIndex(), pCursor->GetUserIndex() );
					return pCursor;
				}

				// 같은 연승 없을 경우
				else
				{
					if( pCursor->GetLoseStreakCount() == ( pUser->GetLoseStreakCount() + nCount) )
					{
						printf("연페수 -%d, %d, %d\n", nCount, pUser->GetUserIndex(), pCursor->GetUserIndex() );
						return pCursor;
					}
					else if (pCursor->GetLoseStreakCount() == ( pUser->GetLoseStreakCount() - nCount ) )
					{
						printf("연패수+%d, %d, %d\n", nCount, pUser->GetUserIndex(), pCursor->GetUserIndex() );
						return pCursor;
					}

				}

			}
		}
		else
		{
			// 연승 수가 1 이상인 경우
			for( nCount= 1; nCount <= iWinINIScale; nCount++ )
			{
				// 같은 연승수 찾음
				if ( pCursor->GetWinStreakCount() == pUser->GetWinStreakCount() )
				{
					printf("같은 연승수 찾음%d, %d\n", pUser->GetUserIndex(), pCursor->GetUserIndex() );
					return pCursor;
				}

				// 같은 연승 없을 경우
				else
				{
					if( pCursor->GetWinStreakCount() == ( pUser->GetWinStreakCount() + nCount) )
					{
						printf("연승수 -%d, %d, %d\n", nCount, pUser->GetUserIndex(), pCursor->GetUserIndex() );
						return pCursor;
					}
					else if (pCursor->GetWinStreakCount() == ( pUser->GetWinStreakCount() - nCount ) )
					{
						printf("연승수+%d, %d, %d\n", nCount, pUser->GetUserIndex(), pCursor->GetUserIndex() );
						return pCursor;
					}

				}

			}
		}
	}

	// 나와 맞는 연승수를 찾지 못한 경우 나와 비슷한 MMR Point 값 찾음
	if ( vMatchNodeSort.size() >= 2 )
	{
		for( int i = 0; i< vMatchNodeSort.size(); i++ )
		{
			MatchUser* pMatch = NULL;
			
			pMatch = vMatchNodeSort.at(i);

			if( !pMatch )
				return NULL;

			MatchUser* pComp = NULL;
			if(pMatch->GetUserIndex() == pUser->GetUserIndex() )
			{
				if( i == ( vMatchNodeSort.size() -1 ) )
				{
					try 
					{
						pComp = vMatchNodeSort.at(i-1);
						return pComp;
					}
					catch(out_of_range)
					{
						LOG.PrintTimeAndLog( 0,"[info][match] vector error [%d]:MMR[%d]:WINSTREK[%d]:LOSESTREAK[%d]"
						,pUser->GetUserIndex(), pUser->GetUserTierPoint(), pUser->GetWinStreakCount(), pUser->GetLoseStreakCount() );
						return NULL;
						
					}
					
				}
				else
				{
					try
					{
						pComp = vMatchNodeSort.at(i+1);
						return pComp;
					}
					catch (out_of_range)
					{
						LOG.PrintTimeAndLog( 0,"[info][match] vector error [%d]:MMR[%d]:WINSTREK[%d]:LOSESTREAK[%d]"
							,pUser->GetUserIndex(), pUser->GetUserTierPoint(), pUser->GetWinStreakCount(), pUser->GetLoseStreakCount() );
						return NULL;
					}
				}
			}
		}
	}
	return NULL;
	
	//return pCursor;	
}
