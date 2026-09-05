#include "../stdafx.h"

#include "../EtcHelpFunc.h"
#include "../QueryData/QueryResultData.h"
#include "../DataBase/DBClient.h"

#include "TournamentManager.h"
#include "ServerNodeManager.h"

#include <limits.h>

template<> TournamentManager* Singleton< TournamentManager >::ms_Singleton = 0;

TournamentManager::TournamentManager()
{
	m_dwProcessTime = 0;
}

TournamentManager::~TournamentManager()
{
	TournamentVec::iterator iter = m_TournamentList.begin();
	for(;iter != m_TournamentList.end();++iter)
	{
		SAFEDELETE( *iter );
	}
	m_TournamentList.clear();
}

TournamentManager& TournamentManager::GetSingleton()
{
	return Singleton< TournamentManager >::GetSingleton();
}

void TournamentManager::Initialize()
{

}

bool TournamentManager::IsAlreadyTournament( DWORD dwTourIndex )
{
	TournamentVec::iterator iter = m_TournamentList.begin();
	for(;iter != m_TournamentList.end();++iter)
	{
		TournamentNode *pTournament = *iter;
		if( pTournament == NULL ) continue;
		if( pTournament->GetIndex() == dwTourIndex )
			return true;		
	}
	return false;
}

void TournamentManager::DeleteTournament( DWORD dwTourIndex )
{
	TournamentVec::iterator iter = m_TournamentList.begin();
	for(;iter != m_TournamentList.end();++iter)
	{
		TournamentNode *pTournament = *iter;
		if( pTournament == NULL ) continue;
		if( pTournament->GetIndex() == dwTourIndex )
		{
			g_DBClient.OnInsertTournamentCustomReward( pTournament->GetIndex() );
			g_DBClient.OnInsertTournamentCustomBackup( pTournament->GetIndex() );
			g_DBClient.OnDeleteTournamentCustomData( pTournament->GetIndex() );
			LOG.PrintTimeAndLog( 0, "Delete Tournament : %d - %s", pTournament->GetIndex(), pTournament->GetTitle().c_str() );

			SAFEDELETE( pTournament );
			m_TournamentList.erase( iter );
			break;
		}
	}
}

void TournamentManager::ServerDownAllSave()
{
	TournamentVec::iterator iter = m_TournamentList.begin();
	for(;iter != m_TournamentList.end();++iter)
	{
		TournamentNode *pTournament = *iter;
		if( pTournament == NULL ) continue;

		pTournament->SaveDB();
		pTournament->SaveInfoDB();
		pTournament->SaveRoundDB();
		pTournament->SaveTeamProcess( false );   
	}
}

void TournamentManager::Process()
{
	// 10초에 1회만
	if( TIMEGETTIME() - m_dwProcessTime < 10000 ) 
		return;

	// 프로세스
	TournamentVec::iterator iter = m_TournamentList.begin();
	for(;iter != m_TournamentList.end();++iter)
	{
		TournamentNode *pTournament = *iter;
		if( pTournament == NULL ) continue;

		pTournament->Process();
	}
	DeleteProcess();	

	m_dwProcessTime = TIMEGETTIME();
}

void TournamentManager::DeleteProcess()
{
	// 종료된 유저 대회 처리
	static DWORDVec vDeleteList;
	vDeleteList.clear();

	TournamentVec::iterator iter = m_TournamentList.begin();
	for(;iter != m_TournamentList.end();++iter)
	{
		TournamentNode *pTournament = *iter;
		if( pTournament == NULL ) continue;
		if( !pTournament->IsCustomTournamentEnd() ) continue;

		vDeleteList.push_back( pTournament->GetIndex() );
	}

	for(int i = 0;i < (int)vDeleteList.size();i++)
		DeleteTournament( vDeleteList[i] );
	vDeleteList.clear();
}

TournamentNode *TournamentManager::CreateTournamentNode( CQueryResultData *pQueryData )
{
	BYTE Type = 0, State = 0;
	DWORD dwIndex = 0, dwOwnerIndex = 0, dwStartDate = 0, dwEndDate = 0;

	if(!pQueryData->GetValue( dwIndex ))			return NULL;	// 토너먼트 인덱스
	if(!pQueryData->GetValue( dwOwnerIndex ))	return NULL;	// 생성 유저 인덱스
	if(!pQueryData->GetValue( dwStartDate ))		return NULL;	// 시작일
	if(!pQueryData->GetValue( dwEndDate ))		return NULL;	// 종료일
	if(!pQueryData->GetValue( Type ))			return NULL;	// 타입
	if(!pQueryData->GetValue( State ))			return NULL;	// 상태

	if( IsAlreadyTournament( dwIndex ) )
	{
		LOG.PrintTimeAndLog( 0, "CreateTournamentNode : Already Tournament : %d - %d - %d - %d - %d - %d", dwIndex, dwOwnerIndex, dwStartDate, dwEndDate, (int)Type, (int)State );
		return NULL;
	}

	TournamentNode *pNode = new TournamentNode( dwIndex, dwOwnerIndex, dwStartDate, dwEndDate, Type, State );
	m_TournamentList.push_back( pNode );

	pNode->SendServerSync( NULL );
	return pNode;
}

void TournamentManager::CreateCompleteSort()
{
	if( m_TournamentList.size() <= 1 ) return;

	std::sort( m_TournamentList.begin(), m_TournamentList.end(), TournamentSort() );
}

void TournamentManager::ApplyTournamentInfoDB( CQueryResultData *pQueryData )
{
	DWORD dwTourIndex = 0;
	if(!pQueryData->GetValue( dwTourIndex ))	return;

	TournamentNode *pTournament = GetTournament( dwTourIndex );
	if( pTournament == NULL )
	{
		LOG.PrintTimeAndLog( 0, "ApplyTournamentInfoDB : %d None Pointer!!!!", dwTourIndex );
	}
	else
	{
		pTournament->ApplyTournamentInfoDB( pQueryData );
	}
}

void TournamentManager::ApplyTournamentRoundDB( CQueryResultData *pQueryData )
{
	DWORD dwTourIndex = 0;
	if(!pQueryData->GetValue( dwTourIndex ))	return;

	TournamentNode *pTournament = GetTournament( dwTourIndex );
	if( pTournament == NULL )
	{
		LOG.PrintTimeAndLog( 0, "ApplyTournamentInfoDB : %d None Pointer!!!!", dwTourIndex );
	}
	else
	{
		pTournament->ApplyTournamentRoundDB( pQueryData );
	}
}

void TournamentManager::ApplyTournamentConfirmUserListDB( CQueryResultData *pQueryData )
{
	DWORD dwTourIndex = 0;
	if(!pQueryData->GetValue( dwTourIndex ))	return;

	TournamentNode *pTournament = GetTournament( dwTourIndex );
	if( pTournament == NULL )
	{
		LOG.PrintTimeAndLog( 0, "ApplyTournamentConfirmUserListDB : %d None Pointer!!!!", dwTourIndex );
	}
	else
	{
		pTournament->ApplyTournamentConfirmUserListDB( pQueryData );
	}
}

void TournamentManager::ApplyTournamentPrevChampInfoDB( CQueryResultData *pQueryData )
{
	DWORD dwTourIndex = 0;
	if(!pQueryData->GetValue( dwTourIndex ))	return;

	TournamentNode *pTournament = GetTournament( dwTourIndex );
	if( pTournament == NULL )
	{
		LOG.PrintTimeAndLog( 0, "ApplyTournamentPrevChampInfoDB : %d None Pointer!!!!", dwTourIndex );
	}
	else
	{
		pTournament->ApplyTournamentPrevChampInfoDB( pQueryData );
	}
}

TournamentTeamNode *TournamentManager::CreateTournamentTeamNode( CQueryResultData *pQueryData )
{
	// - 팀인덱스, 리그인덱스, 팀이름, 팀장인덱스, 리그포지션, 팀맥스카운트, 응원포인트, 토너먼트위치, 진영포인트, 진영타입

	SHORT Position=0, StartPosition=0;
	char szTeamName[TOURNAMENT_TEAM_NAME_NUM_PLUS_ONE] = "";
	int iCheerPoint=0, iLaderPoint=0;
	BYTE MaxPlayer=0, CampPos=0, TourPos=0;
	DWORD dwTeamIndex=0, dwTournamentIndex=0, dwOwnerIndex=0;

	if(!pQueryData->GetValue( dwTeamIndex ))					return NULL;
	if(!pQueryData->GetValue( dwTournamentIndex ))				return NULL;
	if(!pQueryData->GetValue( szTeamName, TOURNAMENT_TEAM_NAME_NUM_PLUS_ONE ))	return NULL;
	if(!pQueryData->GetValue( dwOwnerIndex ))					return NULL;
	if(!pQueryData->GetValue( Position ))						return NULL;
	if(!pQueryData->GetValue( MaxPlayer ))						return NULL;
	if(!pQueryData->GetValue( iCheerPoint ))					return NULL;
	if(!pQueryData->GetValue( TourPos ))						return NULL;
	if(!pQueryData->GetValue( iLaderPoint ))					return NULL;
	if(!pQueryData->GetValue( CampPos ))						return NULL;
	if(!pQueryData->GetValue( StartPosition ))					return NULL;

	TournamentNode *pTournament = GetTournament( dwTournamentIndex );
	if( pTournament == NULL )
	{
		LOG.PrintTimeAndLog( 0, "TournamentManager::CreateTournamentTeamNode None : %d", dwTournamentIndex );
		return NULL;
	}

	return pTournament->CreateTeamNode( dwTeamIndex, szTeamName, dwOwnerIndex, iLaderPoint, 
										CampPos, MaxPlayer, iCheerPoint, Position, StartPosition, TourPos );	
}

void TournamentManager::CreateTeamCompleteRound()
{
	TournamentVec::iterator iter = m_TournamentList.begin();
	for(;iter != m_TournamentList.end(); ++iter )
	{
		TournamentNode *pTournament = *iter;
		if( pTournament == NULL ) continue;

		pTournament->CreateTournamentRoundHistory();
		pTournament->CreateNewRegularCheerTeamList();
	}
}

TournamentNode *TournamentManager::GetRegularTournament()
{
	TournamentVec::iterator iter = m_TournamentList.begin();
	for(;iter != m_TournamentList.end();++iter)
	{
		TournamentNode *pTournament = *iter;
		if( pTournament == NULL ) continue;
		if( pTournament->GetType() != TournamentNode::TYPE_REGULAR ) continue;

		return pTournament;		
	}
	
	return NULL;
}

TournamentNode *TournamentManager::GetTournament( DWORD dwIndex )
{
	TournamentVec::iterator iter = m_TournamentList.begin();
	for(;iter != m_TournamentList.end();++iter)
	{
		TournamentNode *pTournament = *iter;
		if( pTournament == NULL ) continue;

		if( pTournament->GetIndex() == dwIndex )
			return pTournament;
	}

	return NULL;
}

void TournamentManager::SendTournamentListServerSync( ServerNode *pSender )
{
	TournamentVec::iterator iter = m_TournamentList.begin();
	for(;iter != m_TournamentList.end();++iter)
	{
		TournamentNode *pTournament = *iter;
		if( pTournament == NULL ) continue;

		pTournament->SendServerSync( pSender );
	}
}

int TournamentManager::GetTournamentSeq( DWORD dwIndex )
{
	int iCurrentSeq = 0;
	TournamentVec::iterator iter = m_TournamentList.begin();
	for(;iter != m_TournamentList.end();iter++,iCurrentSeq++)
	{
		TournamentNode *pTournament = *iter;
		if( pTournament == NULL ) continue;
		if( pTournament->GetIndex() == dwIndex )
			return iCurrentSeq + 1;
 	}
	return 0;	
}

void TournamentManager::SendTournamentList( ServerNode *pSender, SP2Packet &rkPacket )
{
	if( pSender == NULL ) return;

	DWORD dwUserIndex, dwTourIndex;
	rkPacket >> dwUserIndex;

	int iCurrentPage, iStartCount, iRequestCount;
	rkPacket >> iCurrentPage >> iStartCount >> iRequestCount >> dwTourIndex;

	if( iStartCount < 0 ) return;      // 잘못된 시작점.
	
	if( dwTourIndex != 0 )
	{
		// 특정 리그의 페이지를 요청했음.
		int iCurrentSeq = GetTournamentSeq( dwTourIndex );
		if( iCurrentSeq <= MAX_FIRST_PAGE_LIST )          // 1페이지 -
		{
			iCurrentPage = 0;
			iStartCount  = 0;
			iRequestCount= MAX_FIRST_PAGE_LIST;
		}
		else											  // 1페이지 이상
		{
			iCurrentPage = 1 + ( (iCurrentSeq - MAX_FIRST_PAGE_LIST) - 1 ) / MAX_NEXT_PAGE_LIST;
			iStartCount  = MAX_FIRST_PAGE_LIST + (( iCurrentPage - 1 ) * MAX_NEXT_PAGE_LIST);
			iRequestCount= MAX_NEXT_PAGE_LIST;
		}
	}

	int iListSize = (int)m_TournamentList.size();
	if( iStartCount >= iListSize )
	{
		SP2Packet kPacket( MSTPK_TOURNAMENT_LIST_REQUEST );
		kPacket << dwUserIndex << dwTourIndex << iCurrentPage << 0;
		pSender->SendMessage( kPacket );
		return;         // 더이상 없음
	}

	int iSendSize = min( iListSize - iStartCount, iRequestCount );
	int i = iStartCount;
	SP2Packet kPacket( MSTPK_TOURNAMENT_LIST_REQUEST );
	kPacket << dwUserIndex << dwTourIndex << iCurrentPage << iSendSize;
	for(;i < iStartCount + iSendSize;i++)
	{
		TournamentNode *pTournament = m_TournamentList[i];
		if( pTournament == NULL ) continue;

		kPacket << pTournament->GetIndex() << pTournament->GetType();

		if( pTournament->GetType() == TournamentNode::TYPE_REGULAR )
		{
			kPacket << pTournament->GetRegularResourceType();
		}
		else 
		{
			kPacket << pTournament->GetOwnerIndex() << pTournament->GetBannerBig() << pTournament->GetBannerSmall();
		}
	}

	if( i >= iListSize )
		kPacket << true;	// 마지막 페이지
	else
		kPacket << false;  // 다음 페이지가 존재한다.
	pSender->SendMessage( kPacket );
}

void TournamentManager::SendTournamentTeamInfo( ServerNode *pSender, SP2Packet &rkPacket )
{
	if( pSender == NULL ) return;

	DWORD dwUserIndex, dwTourIndex, dwTeamIndex;
	rkPacket >> dwUserIndex >> dwTourIndex >> dwTeamIndex;

	TournamentNode *pTournament = GetTournament( dwTourIndex );
	if( pTournament == NULL ) return; // 종료된 리그
	
	TournamentTeamNode *pTeam = pTournament->GetTeamNode( dwTeamIndex );
	if( pTeam == NULL ) return;  // 해체된 팀

	SP2Packet kPacket( MSTPK_TOURNAMENT_TEAM_INFO );
	kPacket << dwUserIndex;
	pTeam->FillTeamInfo( kPacket );
	pSender->SendMessage( kPacket );
}

void TournamentManager::SendTournamentScheduleInfo( ServerNode *pSender, SP2Packet &rkPacket )
{
	if( pSender == NULL ) return;

	DWORD dwUserIndex, dwTourIndex;
	rkPacket >> dwUserIndex >> dwTourIndex;


	TournamentNode *pTournament = GetTournament( dwTourIndex );
	if( pTournament == NULL ) return; // 종료된 리그

	SP2Packet kPacket( MSTPK_TOURNAMENT_SCHEDULE_INFO );
	kPacket << dwUserIndex;
	pTournament->FillScheduleInfo( kPacket );
	pSender->SendMessage( kPacket );
}

void TournamentManager::SendTournamentRoundTeamData( ServerNode *pSender, SP2Packet &rkPacket )
{
	if( pSender == NULL ) return;

	DWORD dwUserIndex, dwTourIndex;
	rkPacket >> dwUserIndex >> dwTourIndex;

	TournamentNode *pTournament = GetTournament( dwTourIndex );
	if( pTournament == NULL ) return; // 종료된 리그

	int iStartRound, iTotalRoundCount;
	int iRoundTeamCount, iStartRountTeamArray;
	rkPacket >> iStartRound >> iTotalRoundCount >> iRoundTeamCount >> iStartRountTeamArray;

	SP2Packet kPacket( MSTPK_TOURNAMENT_ROUND_TEAM_DATA );
	kPacket << dwUserIndex << dwTourIndex << iStartRound << iTotalRoundCount << iRoundTeamCount << iStartRountTeamArray;
	pTournament->SendRoundTeamData( iStartRound, iTotalRoundCount, iRoundTeamCount, iStartRountTeamArray, kPacket );
	pSender->SendMessage( kPacket );
}

void TournamentManager::SendTournamentRoundCreateBattleRoom( ServerNode *pSender, SP2Packet &rkPacket )
{
	if( pSender == NULL ) return;

	DWORD dwTourIndex;
	rkPacket >> dwTourIndex;

	TournamentNode *pTournament = GetTournament( dwTourIndex );
	if( pTournament == NULL ) return; // 종료된 리그

	//
	int iMaxBattleRoom;
	rkPacket >> iMaxBattleRoom;
	pTournament->TournamentRoundCreateBattleRoom( pSender->GetServerIndex(), iMaxBattleRoom, rkPacket );
}

void TournamentManager::SendTournamentRoundTeamChange( SP2Packet &rkPacket )
{
	DWORD dwTourIndex;
	rkPacket >> dwTourIndex;

	TournamentNode *pTournament = GetTournament( dwTourIndex );
	if( pTournament == NULL ) return; // 종료된 리그

	pTournament->TournamentRoundChangeBattleTeam( rkPacket );
}

void TournamentManager::SendTournamentTeamAllocateList( ServerNode *pSender, SP2Packet &rkPacket )
{
	DWORD dwUserIndex, dwTourIndex;
	rkPacket >> dwUserIndex >> dwTourIndex;

	TournamentNode *pTournament = GetTournament( dwTourIndex );
	if( pTournament == NULL ) return; // 종료된 리그

	pTournament->TournamentTeamAllocateList( pSender, dwUserIndex, rkPacket );
}

void TournamentManager::ApplyTournamentTeamAllocateData( ServerNode *pSender, SP2Packet &rkPacket )
{
	if( pSender == NULL ) return;

	DWORD dwUserIndex, dwTourIndex;
	rkPacket >> dwUserIndex >> dwTourIndex;

	TournamentNode *pTournament = GetTournament( dwTourIndex );
	if( pTournament == NULL )
	{
		// 리그 종료 알림
		SP2Packet kPacket( MSTPK_TOURNAMENT_TEAM_ALLOCATE_DATA );
		kPacket << dwUserIndex << TOURNAMENT_TEAM_ALLOCATE_END << dwTourIndex;
		pSender->SendMessage( kPacket );
	}
	else
	{
		pTournament->TournamentTeamAllocateData( pSender, dwUserIndex, rkPacket );
	}
}

void TournamentManager::ApplyTournamentBattleResult( ServerNode *pSender, SP2Packet &rkPacket )
{
	if( pSender == NULL ) return;

	DWORD dwTourIndex;
	rkPacket >> dwTourIndex;

	TournamentNode *pTournament = GetTournament( dwTourIndex );
	if( pTournament == NULL ) return; // 종료된 리그

	pTournament->ApplyTournamentBattleResult( pSender, rkPacket );
}

void TournamentManager::SendTournamentJoinConfirmCheck( ServerNode *pSender, SP2Packet &rkPacket )
{
	if( pSender == NULL ) return;

	DWORD dwUserIndex, dwTourIndex;
	rkPacket >> dwUserIndex >> dwTourIndex;

	TournamentNode *pTournament = GetTournament( dwTourIndex );
	if( pTournament == NULL )
	{
		// 리그 종료 알림
		SP2Packet kPacket( MSTPK_TOURNAMENT_JOIN_CONFIRM_CHECK );
		kPacket << dwUserIndex << TOURNAMENT_JOIN_CONFIRM_CLOSE << dwTourIndex;
		pSender->SendMessage( kPacket );
	}
	else
	{
		if( pTournament->IsTournamentJoinConfirm( dwUserIndex ) )
		{
			SP2Packet kPacket( MSTPK_TOURNAMENT_JOIN_CONFIRM_CHECK );
			kPacket << dwUserIndex << TOURNAMENT_JOIN_CONFIRM_OK << dwTourIndex;
			pSender->SendMessage( kPacket );
		}
		else
		{
			SP2Packet kPacket( MSTPK_TOURNAMENT_JOIN_CONFIRM_CHECK );
			kPacket << dwUserIndex << TOURNAMENT_JOIN_CONFIRM_FAILED << dwTourIndex;
			pSender->SendMessage( kPacket );
		}
	}
}

void TournamentManager::ApplyTournamentJoinConfirmReg( SP2Packet &rkPacket )
{
	DWORD dwTourIndex;
	rkPacket >> dwTourIndex;

	TournamentNode *pTournament = GetTournament( dwTourIndex );
	if( pTournament == NULL )
		return;

	pTournament->TournamentJoinConfirmReg( rkPacket );
}

void TournamentManager::ApplyTournamentAnnounceChange( ServerNode *pSender, SP2Packet &rkPacket )
{
	if( pSender == NULL ) return;

	DWORD dwUserIndex, dwTourIndex;
	rkPacket >> dwUserIndex >> dwTourIndex;

	ioHashString szAnnounce;
	rkPacket >> szAnnounce;

	TournamentNode *pTournament = GetTournament( dwTourIndex );
	if( pTournament == NULL )
	{
		// 리그 종료 알림
		SP2Packet kPacket( MSTPK_TOURNAMENT_ANNOUNCE_CHANGE );
		kPacket << dwUserIndex << TOURNAMENT_ANNOUNCE_CHANGE_CLOSE << dwTourIndex;
		pSender->SendMessage( kPacket );
	}
	else
	{
		if( pTournament->ChangeAnnounce( dwUserIndex, szAnnounce ) )
		{
			SP2Packet kPacket( MSTPK_TOURNAMENT_ANNOUNCE_CHANGE );
			kPacket << dwUserIndex << TOURNAMENT_ANNOUNCE_CHANGE_OK << dwTourIndex << szAnnounce;
			pSender->SendMessage( kPacket );
		}
		else
		{
			SP2Packet kPacket( MSTPK_TOURNAMENT_ANNOUNCE_CHANGE );
			kPacket << dwUserIndex << TOURNAMENT_ANNOUNCE_CHANGE_FAILED << dwTourIndex;
			pSender->SendMessage( kPacket );
		}
	}
}

void TournamentManager::SendTournamentTotalTeamList( ServerNode *pSender, SP2Packet &rkPacket )
{
	if( pSender == NULL ) return;

	DWORD dwUserIndex, dwTourIndex;
	rkPacket >> dwUserIndex >> dwTourIndex;

	TournamentNode *pTournament = GetTournament( dwTourIndex );
	if( pTournament == NULL )
		return;

	pTournament->SendTournamentTotalTeamList( pSender, dwUserIndex, rkPacket );
}

void TournamentManager::ApplyTournamentCustomStateStart( ServerNode *pSender, SP2Packet &rkPacket )
{
	if( pSender == NULL ) return;

	DWORD dwUserIndex, dwTourIndex;
	rkPacket >> dwUserIndex >> dwTourIndex;

	TournamentNode *pTournament = GetTournament( dwTourIndex );
	if( pTournament == NULL )
	{
		// 리그 종료 알림
		SP2Packet kPacket( MSTPK_TOURNAMENT_CUSTOM_STATE_START );
		kPacket << dwUserIndex << TOURNAMENT_CUSTOM_STATE_START_CLOSE << dwTourIndex;
		pSender->SendMessage( kPacket );
	}
	else
	{
		pTournament->TournamentCustomStateStart( pSender, dwUserIndex, rkPacket );
	}
}

void TournamentManager::SendTournamentCustomRewardList( ServerNode *pSender, SP2Packet &rkPacket )
{
	if( pSender == NULL ) return;

	DWORD dwUserIndex, dwTourIndex;
	rkPacket >> dwUserIndex >> dwTourIndex;

	TournamentNode *pTournament = GetTournament( dwTourIndex );
	if( pTournament == NULL )
	{
		return;
	}
	else
	{
		pTournament->SendTournamentCustomRewardList( pSender, dwUserIndex );
	}
}

void TournamentManager::ApplyTournamentCustomRewardRegCheck( ServerNode *pSender, SP2Packet &rkPacket )
{
	if( pSender == NULL ) return;

	DWORD dwUserIndex, dwTourIndex;
	rkPacket >> dwUserIndex >> dwTourIndex;

	TournamentNode *pTournament = GetTournament( dwTourIndex );
	if( pTournament == NULL )
	{
		// 리그 종료 알림
		SP2Packet kPacket( MSTPK_TOURNAMENT_CUSTOM_REWARD_REG_CHECK );
		kPacket << dwUserIndex << TOURNAMENT_CUSTOM_REWARD_REG_CLOSE << dwTourIndex;
		pSender->SendMessage( kPacket );		
	}
	else
	{
		pTournament->ApplyTournamentCustomRewardRegCheck( pSender, dwUserIndex, rkPacket );
	}
}

void TournamentManager::ApplyTournamentCustomRewardRegUpdate( SP2Packet &rkPacket )
{
	DWORD dwTourIndex;
	rkPacket >> dwTourIndex;
	TournamentNode *pTournament = GetTournament( dwTourIndex );
	if( pTournament == NULL )
		return;

	pTournament->ApplyTournamentCustomRewardRegUpdate( rkPacket );
}

void TournamentManager::CreateTournamentTeam( ServerNode *pSender, SP2Packet &rkPacket )
{
	if( pSender == NULL ) return;

	BYTE CampPos;
	int iLadderPoint;
	ioHashString kTeamName;
	DWORD dwUserIndex, dwTourIndex, dwTeamIndex, dwOwnerIndex;
	rkPacket >> dwUserIndex >> dwTourIndex >> dwTeamIndex >> kTeamName >> dwOwnerIndex >> iLadderPoint >> CampPos;

	TournamentNode *pTournament = GetTournament( dwTourIndex );
	if( pTournament == NULL )
	{
		// 종료된 토너먼트
		SP2Packet kPacket( MSTPK_TOURNAMENT_TEAM_CREATE );
		kPacket << MS_TOURNAMENT_TEAM_CREATE_APP_END << dwUserIndex;
		pSender->SendMessage( kPacket );

		//팀 삭제 쿼리
		g_DBClient.OnDeleteTournamentTeam( dwTeamIndex );
	}
	else if( pTournament->GetState() != TournamentNode::STATE_TEAM_APP )
	{
		// 팀 모집 종료
		SP2Packet kPacket( MSTPK_TOURNAMENT_TEAM_CREATE );
		kPacket << MS_TOURNAMENT_TEAM_CREATE_APP_END << dwUserIndex;
		pSender->SendMessage( kPacket );

		//팀 삭제 쿼리
		g_DBClient.OnDeleteTournamentTeam( dwTeamIndex );
	}
	else 
	{
		// 생성
		TournamentTeamNode *pTeam = pTournament->CreateTeamNode( dwTeamIndex, kTeamName, dwOwnerIndex, iLadderPoint, CampPos );
		if( pTeam )
		{
			SP2Packet kPacket( MSTPK_TOURNAMENT_TEAM_CREATE );
			kPacket << MS_TOURNAMENT_TEAM_CREATE_OK << dwUserIndex << pTeam->GetTourIndex() << pTeam->GetTeamIndex() << pTeam->GetTeamName() << pTeam->GetOwnerIndex();
			kPacket << pTeam->GetPosition() << pTeam->GetMaxPlayer() << pTeam->GetCheerPoint() << pTeam->GetTourPos() << pTeam->GetLadderPoint() << pTeam->GetCampPos();
			pSender->SendMessage( kPacket );
		}
		else
		{
			SP2Packet kPacket( MSTPK_TOURNAMENT_TEAM_CREATE );
			kPacket << MS_TOURNAMENT_TEAM_CREATE_ALREADY_TEAM << dwUserIndex;
			pSender->SendMessage( kPacket );
		}
	}
}

void TournamentManager::DeleteTournamentTeam( SP2Packet &rkPacket )
{
	DWORD dwTourIndex, dwTeamIndex;
	rkPacket >> dwTourIndex >> dwTeamIndex;

	TournamentNode *pTournament = GetTournament( dwTourIndex );
	if( pTournament )
	{
		pTournament->DeleteTeamNode( dwTeamIndex );
	}

	g_DBClient.OnDeleteTournamentTeam( dwTeamIndex );
}

bool TournamentManager::IsTournamentTeamLeaveState( DWORD dwTourIndex )
{
	TournamentNode *pTournament = GetTournament( dwTourIndex );
	if( pTournament == NULL ) 
		return true;

	if( pTournament->GetState() == TournamentNode::STATE_TEAM_APP )
		return true;
	return false;
}

void TournamentManager::SetTournamentTeamLadderPointAdd( DWORD dwTourIndex, DWORD dwTeamIndex, int iLadderPoint )
{
	TournamentNode *pTournament = GetTournament( dwTourIndex );
	if( pTournament )
	{
		if( pTournament->GetType() != TournamentNode::TYPE_REGULAR )
		{
			LOG.PrintTimeAndLog( 0, "TournamentManager::SetTournamentTeamLadderPointAdd Tounament Not Regular Type %d - %d - %d", dwTourIndex, dwTeamIndex, iLadderPoint );
			return;       // 정규 리그만 수집
		}

		TournamentTeamNode *pTournamentTeam = pTournament->GetTeamNode( dwTeamIndex );
		if( pTournamentTeam )
		{
			pTournamentTeam->SetLadderPointAdd( iLadderPoint );
		}
		else
		{
			LOG.PrintTimeAndLog( 0, "TournamentManager::SetTournamentTeamLadderPointAdd Tounament Team NULL %d - %d - %d", dwTourIndex, dwTeamIndex, iLadderPoint );
		}
	}
	else
	{
		LOG.PrintTimeAndLog( 0, "TournamentManager::SetTournamentTeamLadderPointAdd Tounament NULL %d - %d - %d", dwTourIndex, dwTeamIndex, iLadderPoint );
	}
}

void TournamentManager::InsertRegularTournamentReward()
{
	TournamentNode *pTournament = GetRegularTournament();
	if( pTournament == NULL )
	{
		LOG.PrintTimeAndLog( 0, "InsertRegularTournamentReward None Regular Data" );
	}
	else
	{
		g_DBClient.OnInsertTournamentWinnerHistory( pTournament->GetTitle(), pTournament->GetStartDate(), pTournament->GetEndDate(), 0, "", "", 0 );
		g_DBClient.OnUpdateTournamentBackUP( pTournament->GetIndex() );		// 백업

		short TeamCount = (short)min( pTournament->GetStartTeamCount(), SHRT_MAX );
		short TeamPeso	= (short)min( pTournament->GetAdjustCheerTeamPeso(), SHRT_MAX );
		short UserPeso	= (short)min( pTournament->GetAdjustCheerUserPeso(), SHRT_MAX );
		g_DBClient.OnInsertTournamentRewardAdd( pTournament->GetStartDate(), pTournament->GetIndex(), TeamCount, TeamPeso, UserPeso );
	}
}

void TournamentManager::ApplyTournamentCheerDecision( ServerNode *pSender, SP2Packet &rkPacket )
{
	DWORD dwUserIndex, dwTourIndex;
	rkPacket >> dwUserIndex >> dwTourIndex;

	TournamentNode *pTournament = GetTournament( dwTourIndex );
	if( pTournament )
	{
		pTournament->ApplyTournamentCheerDecision( pSender, dwUserIndex, rkPacket );
	}
	else
	{
		SP2Packet kPacket( MSTPK_TOURNAMENT_CHEER_DECISION );
		kPacket << TOURNAMENT_CHEER_DECISION_CLOSE;
		kPacket << dwUserIndex;
		g_ServerNodeManager.SendMessageAllNode( kPacket );
	}
}