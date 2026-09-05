

#include "stdafx.h"
//#include "../Window.h"
#include "../MainProcess.h"

#include "BattleMode.h"

#include "Room.h"
#include "BattleModeHelp.h"
#include "ioEventUserNode.h"
#include "ioEtcItemManager.h"
#include "LadderTeamManager.h"
#include "ioItemInfoManager.h"
#include "TournamentManager.h"
#include "ioExerciseCharIndexManager.h"
#include "MissionManager.h"
#include "../EtcHelpFunc.h"

#include "RoomNodeManager.h"

#include "../DataBase/LogDBClient.h"
#include <strsafe.h>

BattleMode::BattleMode( Room *pCreator ) : Mode( pCreator )
{
	m_iRedKillPoint = 0;
	m_iBlueKillPoint = 0;

	m_fRedKillPointRate = 0.0f;
	m_fBlueKillPointRate = 0.0f;

	// 2019-02-14 by bckim, 배틀 모드 추가
	m_dwDurationTime	= 15000;
	
	m_dwTagAcceptHoldTime	= 10000;
	m_iEntryActionPermitCount = 1;
	
	m_iBlueUserTagCount = 0;
	m_iRedUserTagCount  = 0;

	m_iBlueDiePlayerCnt = 0;
	m_iRedDiePlayerCnt = 0;
	// End. 2019-02-14 by bckim, 배틀 모드 추가

}

BattleMode::~BattleMode()
{
}

void BattleMode::DestroyMode()
{
	Mode::DestroyMode();

	m_vRecordList.clear();
}

void BattleMode::LoadINIValue()
{
	Mode::LoadINIValue();

	m_dwCurRoundDuration = m_dwRoundDuration;
	m_vRoundHistory.clear();

	LoadAdditionalInfo();

}

void BattleMode::LoadRoundCtrlValue( ioINILoader &rkLoader )
{
	Mode::LoadRoundCtrlValue( rkLoader );

	rkLoader.SetTitle( "round" );
	m_fWinScoreConstant = rkLoader.LoadFloat( "win_score_constant", 1.0f );

	m_fScoreGapConst = rkLoader.LoadFloat( "score_gap_const", 50.0f );
	m_fScoreGapRateConst = rkLoader.LoadFloat( "score_gap_rate_const", 1.5f );
	m_fLadderScoreGapConst = rkLoader.LoadFloat( "ladder_score_gap_const", 50.0f );
	m_fLadderScoreGapRateConst = rkLoader.LoadFloat( "ladder_score_gap_rate_const", 1.5f );
}

void BattleMode::LoadAdditionalInfo()
{
	ioINILoader &rkLoader = g_ModeINIMgr.GetINI( GetModeINIFileName() );
	rkLoader.SetTitle( "Additional_options" );

	m_dwDurationTime = rkLoader.LoadInt( "Entry_Duration_Time", 15000 );
	m_dwTagAcceptHoldTime  = rkLoader.LoadInt( "Tag_Accept_Hold_Time", 10000 );
	m_iEntryActionPermitCount =  rkLoader.LoadInt( "Entry_Action_Permit_Count", 1 );

	SP2Packet kPacket( STPK_ROUND_START );
	kPacket << m_iCurRound;
	SendRoomPlayUser( kPacket );
	SetModeState( MS_PLAY );

	return;
}
/*
void BattleMode::InitObjectGroupList()
{
	ioINILoader &rkLoader = g_ModeINIMgr.GetINI( GetModeINIFileName() );

	int iSubNum = GetModeSubNum();
	int iGroupNum = GetModeMapNum();
	char szBuf[MAX_PATH], szTitle[MAX_PATH];

	m_vPushStructList.clear();
	m_iPushStructIdx = 0;

	wsprintf( szTitle, "team_survival%d_object_group%d", iSubNum, iGroupNum );
	rkLoader.SetTitle( szTitle );

	int iPushStructCnt = rkLoader.LoadInt( "push_struct_cnt", 0 );
	m_vPushStructList.reserve( iPushStructCnt );

	for( int i=0; i<iPushStructCnt; i++ )
	{
		PushStruct kPush;
		kPush.m_iIndex = i + 1;

		wsprintf( szTitle, "push_struct%d_num", i+1 );
		kPush.m_iNum = rkLoader.LoadInt( szTitle, 0 );
		
		wsprintf( szTitle, "push_struct%d_pos_x", i+1 );
		kPush.m_CreatePos.x = rkLoader.LoadFloat( szTitle, 0.0f );

		wsprintf( szTitle, "push_struct%d_pos_y", i+1 );
		kPush.m_CreatePos.y = rkLoader.LoadFloat( szTitle, 0.0f );
		
		wsprintf( szTitle, "push_struct%d_pos_z", i+1 );
		kPush.m_CreatePos.z = rkLoader.LoadFloat( szTitle, 0.0f );

		m_iPushStructIdx = kPush.m_iIndex;
		m_vPushStructList.push_back( kPush );
	}


	int iObjectItemCnt = rkLoader.LoadInt( "object_item_cnt", 0 );
	ObjectItemList vObjectItemList;
	vObjectItemList.clear();
	vObjectItemList.reserve( iObjectItemCnt );

	for(int i=0; i<iObjectItemCnt; i++ )
	{
		ObjectItem kObjectItem;
		wsprintf( szTitle, "object_item%d_name", i+1 );
		rkLoader.LoadString( szTitle, "", szBuf, MAX_PATH );
		kObjectItem.m_ObjectItemName = szBuf;
		wsprintf( szTitle, "object_item%d_pos_x", i+1 );
		kObjectItem.m_fPosX = rkLoader.LoadFloat( szTitle, 0.0f );
		wsprintf( szTitle, "object_item%d_pos_z", i+1 );
		kObjectItem.m_fPosZ = rkLoader.LoadFloat( szTitle, 0.0f );

		vObjectItemList.push_back( kObjectItem );
	}

	//Push Struct
	SP2Packet kPushPacket( STPK_PUSHSTRUCT_INFO );
	if( GetPushStructInfo( kPushPacket ) )
	{
		SendRoomAllUser( kPushPacket );
	}

	//Object Item
	ItemVector vItemList;
	int iObjectCnt = vObjectItemList.size();

	for(int i=0; i<iObjectCnt; i++ )
	{
		const ObjectItem &rkObjItem = vObjectItemList[i];

		ioItem *pItem = m_pCreator->CreateItemByName( rkObjItem.m_ObjectItemName );
		if( pItem )
		{
			Vector3 vPos( rkObjItem.m_fPosX, 0.0f, rkObjItem.m_fPosZ );
			pItem->SetItemPos( vPos );
			vItemList.push_back( pItem );
		}
	}

	if( vItemList.empty() )
		return;

	SP2Packet kPacket( STPK_FIELD_ITEM_SUPPLY );
	int iNewItemCnt = vItemList.size();
	kPacket << iNewItemCnt;
	for(int i=0; i<iNewItemCnt; i++ )
	{
		ioItem *pItem = vItemList[i];
		m_pCreator->AddFieldItem( pItem );
		kPacket << pItem->GetItemCode();
		kPacket << pItem->GetItemReinforce();
		kPacket << pItem->GetItemMaleCustom();
		kPacket << pItem->GetItemFemaleCustom();
		kPacket << pItem->GetGameIndex();
		kPacket << pItem->GetItemPos();
		kPacket << pItem->GetOwnerName();
		kPacket << "";
	}
	SendRoomAllUser( kPacket );
}
*/
// 2019-02-14 by bckim, 배틀 모드 추가
void BattleMode::SetOrder( DWORD dwUserIDX, int iOrder)
{
	int iCharCnt = m_vRecordList.size();
	for( int i=0 ; i<iCharCnt ; i++ )
	{
		BattleModeRecord* pUserRecord = &m_vRecordList[i];
		if( pUserRecord->pUser->GetUserIndex() ==  dwUserIDX )
		{ 
			pUserRecord->m_iBattle_Order = iOrder;
			LOG.PrintTimeAndLog(LOG_DEBUG_LEVEL,"[SetOrder::RESULT>>Clent[%s]IDX[%u] ORDER[%d]",pUserRecord->pUser->GetPublicID().c_str(),pUserRecord->pUser->GetUserIndex(),pUserRecord->m_iBattle_Order );
		}
	}
}
// End. 2019-02-14 by bckim, 배틀 모드 추가


void BattleMode::AddNewRecord( User *pUser )
{
	BattleModeRecord kRecord;
	kRecord.pUser = pUser;

	User* pOriginUser = g_UserNodeManager.GetUserNode( pUser->GetUserIndex() );
	if(pOriginUser)
	{
		kRecord.m_iBattle_Order = pOriginUser->m_iBattleMode_Order;
	}

	m_vRecordList.push_back( kRecord );
	
	// 임시 
	LOG.PrintTimeAndLog(LOG_DEBUG_LEVEL,"[<<<<< +++++++++++++++ADD USER CHECK!!! >>>>>  UserRecord[%s]>> State[%d] ",pUser->GetPublicID().c_str(), kRecord.m_iBattle_Order);

	UpdateUserRank();
}

void BattleMode::RemoveRecord( User *pUser, bool bRoomDestroy )
{
	int iPreCnt = GetCurTeamUserCnt( pUser->GetTeam() );

	int iCharCnt = m_vRecordList.size();
	for( int i=0 ; i<iCharCnt ; i++ )
	{
		if( m_vRecordList[i].pUser == pUser                )
		{
			SetModeEndDBLog( &m_vRecordList[i], iCharCnt, LogDBClient::PRT_EXIT_ROOM );

			BattleModeRecord *pSyncUserRecord =(BattleModeRecord*)FindBattleModeRecord(pUser );

			LOG.PrintTimeAndLog(LOG_DEBUG_LEVEL,"[<<<<< -------------REMOVE USER CHECK!!! >>>>>  UserRecord[%s]>> State[%d] ",pUser->GetPublicID().c_str(),GetUserState(pSyncUserRecord));

			if( GetUserState(pSyncUserRecord) == USER_STATE_IN_BATTLE )			// 전투중인 넘이 나가면, 다음 주자 뽑뽑
				SendNextRunnerInfo(pUser, false);
			
			if( GetUserState(pSyncUserRecord) == USER_STATE_DEAD )		// 죽은넘이 나가면, 죽은넘 카운트 차감. 
			{
				if( m_vRecordList[i].pUser->GetTeam() == TEAM_RED )
					m_iRedDiePlayerCnt--;
				else if( m_vRecordList[i].pUser->GetTeam() == TEAM_BLUE )
					m_iBlueDiePlayerCnt--;
			}
			m_vRecordList.erase( m_vRecordList.begin() + i );
			UpdateCurKillPoint( pUser->GetTeam(), iPreCnt );
			break;
		}
	}

	UpdateUserRank();
	
	if( pUser )
	{
		m_KickOutVote.RemoveVoteUserList( pUser->GetPublicID() );
	}


	if( !m_bRoundSetEnd && !bRoomDestroy && GetState() == MS_PLAY )
	{
		// 유저가 전부 나가면 1점 !!
		WinTeamType eWinTeam = WTT_DRAW;	
		int iBlueUser = GetCurTeamUserCnt( TEAM_BLUE );
		int iRedUser  = GetCurTeamUserCnt( TEAM_RED );
		if( iBlueUser == 0 )
			eWinTeam = WTT_RED_TEAM;
		else if( iRedUser == 0 )
			eWinTeam = WTT_BLUE_TEAM;
		else 
			return;

		SetRoundEndInfo( eWinTeam );
		if( m_bRoundSetEnd )       //세트가 종료되면 세트의 결과를 전송
		{
			if( m_iRedTeamWinCnt > m_iBlueTeamWinCnt )
				eWinTeam = WTT_RED_TEAM;
			else if( m_iRedTeamWinCnt < m_iBlueTeamWinCnt )
				eWinTeam = WTT_BLUE_TEAM;
			else
				eWinTeam = WTT_DRAW;

			if( m_bTournamentRoom )
			{
				// 서든 데스 모드에서는 나간팀이 진다.
				if( iBlueUser == 0 )
					eWinTeam = WTT_RED_TEAM;
				else
					eWinTeam = WTT_BLUE_TEAM;
			}
		}
		SendRoundResult( eWinTeam );
	}

}

void BattleMode::ProcessPlay()
{
	//ProcessRevival();		// 부활

	CheckRoundTimePing();
	CheckFieldItemLiveTime();
	CheckItemSupply( m_dwStateChangeTime );
	CheckBallSupply( m_dwStateChangeTime );
	CheckMachineSupply( m_dwStateChangeTime );
	CheckNeedSendPushStruct();
	CheckRoundEnd( true );
	ProcessEvent();
	ProcessBonusAlarm();
}

void BattleMode::RestartMode()
{
	Mode::RestartMode();

	// 결과중에 유저들이 전부 나가면 최종 결과 실행.
	if( GetTeamUserCnt( TEAM_BLUE ) == 0 || 
		GetTeamUserCnt( TEAM_RED ) == 0 )
	{
		m_bRoundSetEnd = true;
		m_bCheckContribute = false;
		m_bCheckAwardChoose = false;
		SetModeState( MS_RESULT_WAIT );
		WinTeamType eWinTeam = WTT_DRAW;
		if( m_iRedTeamWinCnt > m_iBlueTeamWinCnt )
			eWinTeam = WTT_RED_TEAM;
		else if( m_iRedTeamWinCnt < m_iBlueTeamWinCnt )
			eWinTeam = WTT_BLUE_TEAM;
		else
			eWinTeam = WTT_DRAW;
		SendRoundResult( eWinTeam );
		return;
	}

	int iRecordCnt = m_vRecordList.size();
	for( int i=0 ; i<iRecordCnt ; i++ )
	{
		BattleModeRecord &rkRecord = m_vRecordList[i];
		rkRecord.dwPlayingStartTime= 0;
		rkRecord.dwCurDieTime = 0;
		rkRecord.iRevivalCnt = 0;
		rkRecord.dwRevivalGap = (DWORD)GetRevivalGapTime( 0 );
		rkRecord.bCatchState = false;
		rkRecord.bDieState = false;

		if( rkRecord.eState == RS_VIEW )
			rkRecord.eState = RS_PLAY;

		// 추가 
		rkRecord.m_iUserState  = USER_STATE_WAITING;
		rkRecord.m_iHP  = 100;
		rkRecord.m_bTagWaiting  = false;
		rkRecord.m_iEntryActivation  = ENTRY_STATE_BEFORE;
		rkRecord.m_iEntryDurationTime  = 0;
		rkRecord.m_iBattle_Order  = BATTLE_ORDER_RANDOM;
		rkRecord.m_bEntryState	= false;
		rkRecord.m_iEntryStateCheckCount = 0;
		//
	}

	m_CurRoundWinTeam = WTT_NONE;
	
	m_iRedKillPoint = 0;
	m_iBlueKillPoint = 0;

	// 2019-02-14 by bckim, 배틀 모드 추가
	m_dwDurationTime	= 15000;			// 설정 파일..
	m_dwTagAcceptHoldTime	= 10000;			// 설정 파일..
	m_iEntryActionPermitCount = 1;

	m_iBlueUserTagCount = 0;
	m_iRedUserTagCount  = 0;

	m_iBlueDiePlayerCnt = 0;
	m_iRedDiePlayerCnt = 0;
	// End. 2019-02-14 by bckim, 배틀 모드 추가


	m_pCreator->DestroyAllFieldItems();

	SetModeState( MS_READY );

	SP2Packet kPacket( STPK_ROUND_READY );
	kPacket << m_iCurRound;
	kPacket << m_iBluePosArray;
	kPacket << m_iRedPosArray;
	kPacket << m_dwCurRoundDuration;
	SendRoomAllUser( kPacket );

	//InitObjectGroupList();
}

int BattleMode::GetUserKickVoteLimit( const ioHashString &szKickUserName )
{
	ModeRecord *pKickRecord = FindModeRecord( szKickUserName );
	if( !pKickRecord || !pKickRecord->pUser )
		return USER_KICK_VOTE_PROPOSAL_ERROR_7;

	// 인원 체크 
	if( !pKickRecord->pUser->IsObserver() )
	{
		int iAlreadyTeam = GetTeamUserCnt( pKickRecord->pUser->GetTeam() );
		if( iAlreadyTeam < m_KickOutVote.GetKickVoteUserPool() )
			return USER_KICK_VOTE_PROPOSAL_ERROR_12;
	}

	// 라운드 or 시간 체크
	if( IsRoundSetEnd() )
		return USER_KICK_VOTE_PROPOSAL_ERROR_9;

	// 시간 체크
	DWORD dwGapTime = TIMEGETTIME() - m_dwModeStartTime;
	if( dwGapTime > m_KickOutVote.GetKickVoteRoundTime() )
		return USER_KICK_VOTE_PROPOSAL_ERROR_9;
	return 0;
}

ModeRecord* BattleMode::FindModeRecord( const ioHashString &rkName )
{
	if( rkName.IsEmpty() )	return NULL;

	CRASH_GUARD();
	int iRecordCnt = m_vRecordList.size();
	for( int i=0 ; i<iRecordCnt ; i++ )
	{
		if( m_vRecordList[i].pUser->GetPublicID() == rkName )
			return &m_vRecordList[i];
	}

	return NULL;
}

ModeRecord* BattleMode::FindModeRecord( User *pUser )
{
	if( !pUser )	return NULL;

	int iRecordCnt = m_vRecordList.size();
	for( int i=0 ; i<iRecordCnt ; i++ )
	{
		if( m_vRecordList[i].pUser == pUser )
			return &m_vRecordList[i];
	}

	return NULL;
}

ModeRecord* BattleMode::FindModeRecord( int iIdx )
{
	if( COMPARE( iIdx, 0, GetRecordCnt() ) )
		return &m_vRecordList[iIdx];

	return NULL;
}

BattleModeRecord* BattleMode::FindBattleModeRecord( const ioHashString &rkName )
{
	return (BattleModeRecord*)FindModeRecord( rkName );
}

BattleModeRecord* BattleMode::FindBattleModeRecord( User *pUser )
{
	return (BattleModeRecord*)FindModeRecord( pUser );
}

ModeType BattleMode::GetModeType() const
{
	return MT_BATTLE;
}

void BattleMode::GetModeInfo( SP2Packet &rkPacket )
{
	Mode::GetModeInfo( rkPacket );

	rkPacket << m_fWinScoreConstant;

	rkPacket << m_iRedKillPoint;
	rkPacket << m_iBlueKillPoint;

	GetModeHistory( rkPacket );
}

void BattleMode::GetExtraModeInfo( SP2Packet &rkPacket )
{
	rkPacket << m_fWinScoreConstant;

	rkPacket << m_iRedKillPoint;
	rkPacket << m_iBlueKillPoint;
}

void BattleMode::GetModeHistory( SP2Packet &rkPacket )
{
	int i = 0;
	int HistorySize = m_vRoundHistory.size();

	if( HistorySize == 0 || m_iCurRound-1 > HistorySize )
	{
		for( i = 0; i < m_iCurRound; i++ )	
		{
			RoundHistory rh;
			rkPacket << rh.iBluePoint << rh.iRedPoint;
		}
	}
	else
	{
		for( i = 0; i < m_iCurRound-1; i++ )	
		{
			RoundHistory rh = m_vRoundHistory[i];
			rkPacket << rh.iBluePoint << rh.iRedPoint;
		}

		if( HistorySize == m_iCurRound )
		{
			RoundHistory rh = m_vRoundHistory[m_iCurRound-1];
			rkPacket << rh.iBluePoint << rh.iRedPoint;
		}
		else
		{
			rkPacket << 0 << 0;
		}
	}
}

void BattleMode::GetCharModeInfo( SP2Packet &rkPacket, const ioHashString &rkName, bool bDieCheck )
{
	BattleModeRecord *pRecord = FindBattleModeRecord( rkName );
	if( pRecord )
	{
		// 레코드 정보 유무
		rkPacket << true;

		int iKillSize = pRecord->iKillInfoMap.size();
		rkPacket << iKillSize;

		LOOP_GUARD();
		KillDeathInfoMap::iterator iter_k = pRecord->iKillInfoMap.begin();
		while( iter_k != pRecord->iKillInfoMap.end() )
		{
			rkPacket << iter_k->first;
			rkPacket << iter_k->second;

			++iter_k;
		}
		LOOP_GUARD_CLEAR();

		int iDeathSize = pRecord->iDeathInfoMap.size();
		rkPacket << iDeathSize;

		LOOP_GUARD();
		KillDeathInfoMap::iterator iter_d = pRecord->iDeathInfoMap.begin();
		while( iter_d != pRecord->iDeathInfoMap.end() )
		{
			rkPacket << iter_d->first;
			rkPacket << iter_d->second;

			++iter_d;
		}
		LOOP_GUARD_CLEAR();

		if( bDieCheck )
		{
			rkPacket << pRecord->bDieState;
		}
		rkPacket << pRecord->bCatchState;
	}
	else
	{
		// 레코드 정보 유무
		rkPacket << false;
	}
}

int BattleMode::GetRecordCnt() const
{
	return m_vRecordList.size();
}

const char* BattleMode::GetModeINIFileName() const
{
	return "config/BattleMode.ini";
}

TeamType BattleMode::GetNextTeamType()
{
	int iRedCnt, iBlueCnt;
	iRedCnt = iBlueCnt = 0;

	int i = 0;
	int iCharCnt = m_vRecordList.size();
	for(int i=0 ; i<iCharCnt ; i++ )
	{
		if( m_vRecordList[i].pUser->GetTeam() == TEAM_RED )
			iRedCnt++;
		else if( m_vRecordList[i].pUser->GetTeam() == TEAM_BLUE )
			iBlueCnt++;
	}

	if( iRedCnt > iBlueCnt )
		return TEAM_BLUE;
	else if( iBlueCnt > iRedCnt )
		return TEAM_RED;

	// Red == Blue
	if( m_iRedTeamWinCnt > m_iBlueTeamWinCnt )
		return TEAM_BLUE;
	else if( m_iBlueTeamWinCnt > m_iRedTeamWinCnt )
		return TEAM_RED;

	// 평균 레벨이 낮은 팀
	int iRedLevel = 0;
	int iBlueLevel= 0;
	for(int i=0 ; i<iCharCnt ; i++ )
	{
		if( m_vRecordList[i].pUser->GetTeam() == TEAM_RED )
			iRedLevel += m_vRecordList[i].pUser->GetGradeLevel();
		else if( m_vRecordList[i].pUser->GetTeam() == TEAM_BLUE )
			iBlueLevel += m_vRecordList[i].pUser->GetGradeLevel();
	}
	if( iRedCnt != 0 )
		iRedLevel /= iRedCnt;
	if( iBlueCnt != 0 )
		iBlueLevel /= iBlueCnt;

	if( iRedLevel < iBlueLevel )
		return TEAM_RED;
	else if( iRedLevel > iBlueLevel )
		return TEAM_BLUE;

	int iRandomTeam = rand() % 2;
	if( iRandomTeam > 0 )
		return TEAM_RED;

	return TEAM_BLUE;
}

float BattleMode::GetResultScoreGapValue( bool bLadderPoint, TeamType eWinTeam )
{
	float fScoreGap = 0.0f;
	if( bLadderPoint )
	{
		if( eWinTeam == TEAM_BLUE )
			fScoreGap = (abs(m_fRedKillPointRate - m_fBlueKillPointRate) + m_fLadderScoreGapConst) / (m_fBlueKillPointRate + m_fLadderScoreGapConst);
		else
			fScoreGap = (abs(m_fRedKillPointRate - m_fBlueKillPointRate) + m_fLadderScoreGapConst) / (m_fRedKillPointRate + m_fLadderScoreGapConst);
		fScoreGap *= m_fLadderScoreGapRateConst;
	}
	else
	{         
		if( eWinTeam == TEAM_BLUE )
			fScoreGap = (abs(m_fRedKillPointRate - m_fBlueKillPointRate) + m_fScoreGapConst) / (m_fBlueKillPointRate + m_fScoreGapConst);
		else
			fScoreGap = (abs(m_fRedKillPointRate - m_fBlueKillPointRate) + m_fScoreGapConst) / (m_fRedKillPointRate + m_fScoreGapConst);
		fScoreGap *= m_fScoreGapRateConst;
	}
	return fScoreGap;
}

float BattleMode::GetUserCorrection( TeamType eWinTeam, float fRoundPoint, float fScoreGap )
{
	float fUserCorrection = 1.0f;

	int iBlueUserCnt = max( m_iReadyBlueUserCnt, GetTeamUserCnt( TEAM_BLUE ) );
	int iRedUserCnt  = max( m_iReadyRedUserCnt, GetTeamUserCnt( TEAM_RED ) );

	float fA = fRoundPoint * (iBlueUserCnt + iRedUserCnt);
	float fB = fRoundPoint + fScoreGap;
	float fC = fRoundPoint - fScoreGap;
	if( eWinTeam == TEAM_BLUE )
		fUserCorrection = fA / max( 1.0f, ( ( fB * iBlueUserCnt ) + ( fC * iRedUserCnt ) ) );
	else
		fUserCorrection = fA / max( 1.0f, ( ( fB * iRedUserCnt ) + ( fC * iBlueUserCnt ) ) );
	if( fUserCorrection > 1.0f )
		fUserCorrection = 1.0f;
	return fUserCorrection;
}

void BattleMode::CheckRoundEnd( bool bProcessCall )
{
	DWORD dwGapTime = TIMEGETTIME() - m_dwStateChangeTime;

	WinTeamType eWinTeam = WTT_DRAW;
	if( m_iRedDiePlayerCnt == GetTeamUserCnt( TEAM_RED ) )
		eWinTeam = WTT_BLUE_TEAM;
	else if( m_iBlueDiePlayerCnt == GetTeamUserCnt( TEAM_BLUE ) )
		eWinTeam = WTT_RED_TEAM;

	// LOG.PrintTimeAndLog(LOG_DEBUG_LEVEL,"[<<<<< CheckRoundEnd >>>>> [TEAM_BLUE_COUNT][%d][Die:%d]     [TEAM_RED_COUNT][%d][Die:%d]",GetTeamUserCnt(TEAM_BLUE),m_iBlueDiePlayerCnt,GetTeamUserCnt(TEAM_RED),m_iRedDiePlayerCnt);
	
	if( m_dwCurRoundDuration < dwGapTime+1000 )
	{
		if( !m_bZeroHP )
		{
			m_bZeroHP = true;
			m_dwCurRoundDuration = 0;
			m_dwCurSuddenDeathDuration = TIMEGETTIME();
			m_fSuddenDeathBlueCont	   = 0.0f;
			m_fSuddenDeathRedCont	   = 0.0f;

			SP2Packet kPacket( STPK_ZERO_HP );
			kPacket << m_dwSuddenDeathTime;
			SendRoomAllUser( kPacket );

			// 0초가되면 시간이 멈춘다.
			int iRecordCnt = GetRecordCnt();
			for( int i=0 ; i<iRecordCnt ; i++ )
			{
				ModeRecord *pRecord = FindModeRecord( i );
				if( !pRecord ) continue;

				if( pRecord->pUser )
				{
					if( pRecord->eState != RS_VIEW && pRecord->eState != RS_OBSERVER )
						pRecord->pUser->SetStartTimeLog( TIMEGETTIME() );
				}
			}
		}
	}


	if( bProcessCall && eWinTeam == WTT_DRAW )
	{
		// LOG.PrintTimeAndLog(LOG_DEBUG_LEVEL,"[<<<<< CheckRoundEnd >>bProcessCall && eWinTeam >>> bProcessCall[%d]  WTT_DRAW",bProcessCall);
		return;
	}

	int iBlueUser = GetTeamUserCnt( TEAM_BLUE );
	int iRedUser  = GetTeamUserCnt( TEAM_RED );
	if( iBlueUser == 0 || iRedUser == 0 )
	{
		eWinTeam = WTT_DRAW;
	}

	if( m_dwCurRoundDuration < dwGapTime )
		m_dwCurRoundDuration = 0;
	else
		m_dwCurRoundDuration -= dwGapTime;

	SetRoundEndInfo( eWinTeam );

	if( m_bRoundSetEnd )       //세트가 종료되면 세트의 결과를 전송
	{
		LOG.PrintTimeAndLog(LOG_DEBUG_LEVEL,"[<<<<< CheckRoundEnd >>>>> [m_bRoundSetEnd][%d]",m_bRoundSetEnd);

		if( m_iRedTeamWinCnt > m_iBlueTeamWinCnt )
			eWinTeam = WTT_RED_TEAM;
		else if( m_iRedTeamWinCnt < m_iBlueTeamWinCnt )
			eWinTeam = WTT_BLUE_TEAM;
		else
			eWinTeam = WTT_DRAW;

		if( m_bTournamentRoom )
		{
			// 서든 데스 모드에서는 나간팀이 진다.
			if( iBlueUser == 0 )
			{
				LOG.PrintTimeAndLog(LOG_DEBUG_LEVEL,"[<<<<< CheckRoundEnd >>>>> [m_bTournamentRoom][%d] >>WTT_RED_TEAM",m_bTournamentRoom);
				eWinTeam = WTT_RED_TEAM;
			}
			else
			{
				LOG.PrintTimeAndLog(LOG_DEBUG_LEVEL,"[<<<<< CheckRoundEnd >>>>> [m_bTournamentRoom][%d] >>WTT_BLUE_TEAM",m_bTournamentRoom);
				eWinTeam = WTT_BLUE_TEAM;
			}
		}
	}
	SendRoundResult( eWinTeam );
}

void BattleMode::SetRoundEndInfo( WinTeamType eWinTeam )
{
	m_CurRoundWinTeam = eWinTeam;

	if(	GetTeamUserCnt( TEAM_BLUE ) == 0 ||
		GetTeamUserCnt( TEAM_RED ) == 0 )
	{
		m_bRoundSetEnd = true;
	}

	m_bCheckContribute = false;
	m_bCheckAwardChoose = false;
	SetModeState( MS_RESULT_WAIT );

	UpdateRoundRecord();

	if( m_iNeedRound > 0 )
	{
		if( m_iRedTeamWinCnt == m_iNeedRound || m_iBlueTeamWinCnt == m_iNeedRound )
			m_bRoundSetEnd = true;
	}
	
	m_vPushStructList.clear();
	m_vBallStructList.clear();
	m_vMachineStructList.clear();
	m_pCreator->DestroyAllFieldItems();

	// PlayingTime Update
	int iRecordCnt = GetRecordCnt();
	for( int i=0 ; i<iRecordCnt ; i++ )
	{
		ModeRecord *pRecord = FindModeRecord( i );
		if( pRecord )
		{
			pRecord->AddPlayingTime();
			pRecord->AddClassPlayingTime();

			if( pRecord->pUser && pRecord->pUser->GetStartTimeLog() > 0 )
			{
				if( pRecord->eState != RS_VIEW && pRecord->eState != RS_OBSERVER )
					pRecord->AddDeathTime( TIMEGETTIME() - pRecord->pUser->GetStartTimeLog() );
				else
					g_LogDBClient.OnInsertTime( pRecord->pUser, LogDBClient::TT_VIEW );
				pRecord->pUser->SetStartTimeLog(0);
			}
		}
	}
	
	int HistorySize = m_vRoundHistory.size();
	if( m_iCurRound-1 > HistorySize )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "TeamSurvivalMode::SetRoundEndInfo() m_iCurRound != m_vRoundHistory.size()" );
	}
	else
	{
		RoundHistory rh;
		if( eWinTeam == TEAM_RED )
		{
			rh.iBluePoint = 0;
			rh.iRedPoint = 1;
		}
		else if( eWinTeam == TEAM_BLUE )
		{
			rh.iBluePoint = 1;
			rh.iRedPoint = 0;
		}
		m_vRoundHistory.push_back( rh );
	}
}

void BattleMode::UpdateRoundRecord()
{
	int iRecordCnt = GetRecordCnt();
	for( int i=0 ; i<iRecordCnt ; i++ )
	{
		ModeRecord *pRecord = FindModeRecord( i );
		if( pRecord )
		{
			if( pRecord->pUser )
			{
				pRecord->pUser->UpdateCharLimitDate();
				pRecord->pUser->UpdateEtcItemTime( __FUNCTION__ );
				pRecord->pUser->DeleteEtcItemPassedDate();
				pRecord->pUser->DeleteExtraItemPassedDate(true);
				pRecord->pUser->DeleteMedalItemPassedDate(true);
				pRecord->pUser->DeleteExMedalSlotPassedDate();
				pRecord->pUser->DeleteCharAwakePassedDate( );
				pRecord->pUser->DeleteCostumePassedDate();
				pRecord->pUser->ReleaseAccessoryPassedDate();
				// 임시 : 시간육성
				pRecord->pUser->CheckTimeGrowth();
				pRecord->pUser->DeleteExpiredBonusCash();
			}
		}
	}

	UpdateUserRank();

	if( IsRedWin( m_CurRoundWinTeam ) )
		m_iRedTeamWinCnt++;
	else if( IsBlueWin( m_CurRoundWinTeam ) )
		m_iBlueTeamWinCnt++;
}

void BattleMode::OnEventSceneEnd( User *pSend, SP2Packet &rkPacket )
{
	bool bRoundChange;
	int iRoomIndex;
	rkPacket >> bRoundChange >> iRoomIndex;
	if( iRoomIndex != m_pCreator->GetRoomIndex() )
		return;

	if( !bRoundChange )
	{
		if( !pSend->IsObserver() && !pSend->IsStealth() )
		{
			SP2Packet kPacket( STPK_START_SELECT_CHAR );
			kPacket << GetSelectCharTime();
			pSend->SendMessage( kPacket );
		}

		return;
	}

	ModeRecord *pRecord = FindModeRecord( pSend );
	if( !pRecord )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "TeamSurvivalMode::OnEventSceneEnd - %s Not Exist Record",
								pSend->GetPublicID().c_str() );
		return;
	}

	//로딩 시간을 로그로 남김
	pRecord->CheckLoadingTime();
	pRecord->pUser->EquipDBItemToAllChar();
	SetFirstRevivalTime( pRecord );

	int iModeState;
	switch( m_ModeState )
	{
	case MS_READY:
	case MS_PLAY:
		iModeState = m_ModeState;
		break;
	case MS_RESULT_WAIT:
	case MS_RESULT:
		iModeState = MS_RESULT_WAIT;
		break;
	}

	DWORD dwPastTime = TIMEGETTIME() - m_dwStateChangeTime;
	if( pRecord->pUser->IsObserver() || pRecord->pUser->IsStealth() )
	{
		pRecord->eState = RS_OBSERVER;
		pRecord->pUser->SetStartTimeLog( TIMEGETTIME() );

		SP2Packet kPacket( STPK_ROUND_JOIN_OBSERVER );
		kPacket << pRecord->pUser->GetPublicID();
		kPacket << iModeState;
		kPacket << dwPastTime;
		kPacket << m_dwCurRoundDuration;
		kPacket << m_fWinScoreConstant;
		kPacket << m_iRedKillPoint;
		kPacket << m_iBlueKillPoint;
		SendRoomAllUser( kPacket );
	}
	else if( m_bUseViewMode && m_ModeState == MS_PLAY && dwPastTime > m_dwViewCheckTime )
	{
		pRecord->eState = RS_VIEW;
		pRecord->pUser->SetStartTimeLog( TIMEGETTIME() );

		SP2Packet kPacket( STPK_ROUND_JOIN_VIEW );
		kPacket << pRecord->pUser->GetPublicID();
		kPacket << iModeState;
		kPacket << dwPastTime;
		kPacket << m_dwCurRoundDuration;
		kPacket << m_fWinScoreConstant;
		kPacket << m_iRedKillPoint;
		kPacket << m_iBlueKillPoint;
		SendRoomAllUser( kPacket );
	}
	else
	{
		int iPreCnt = GetCurTeamUserCnt( pRecord->pUser->GetTeam() );

		pRecord->eState = RS_PLAY;
		pRecord->StartPlaying();        //( 관전X, 데스타임X )
		pRecord->pUser->StartCharLimitDate( Mode::GetCharLimitCheckTime(), __FILE__, __LINE__ );
		pRecord->pUser->StartEtcItemTime( __FUNCTION__ );

		UpdateCurKillPoint( pRecord->pUser->GetTeam(), iPreCnt );

		SP2Packet kPacket( STPK_ROUND_JOIN );
		kPacket << pRecord->pUser->GetPublicID();
		kPacket << iModeState;
		kPacket << dwPastTime;
		kPacket << GetSelectCharTime();
		kPacket << m_dwCurRoundDuration;
		kPacket << m_fWinScoreConstant;
		kPacket << m_iRedKillPoint;
		kPacket << m_iBlueKillPoint;

		kPacket << m_dwDurationTime;
		kPacket << m_dwTagAcceptHoldTime;
		kPacket << m_iEntryActionPermitCount;

		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"[BATTLE_MODE]+[RoomSendPacketTcp]+Clent[%s]", pRecord->pUser->GetPublicID().c_str());

		SendRoomAllUser( kPacket );
	}
}

int BattleMode::GetCurTeamUserCnt( TeamType eTeam )
{
	int iUserCnt = 0;
	int iRecordCnt = m_vRecordList.size();
	for( int i=0 ; i<iRecordCnt ; i++ )
	{
		ModeRecord *pRecord = FindModeRecord( i );
		if( pRecord )
		{
			if( pRecord->eState == RS_VIEW ||
				pRecord->eState == RS_OBSERVER ||
				pRecord->eState == RS_LOADING )
				continue;

			User *pUser = pRecord->pUser;

			if( pUser && pUser->GetTeam() == eTeam )
				iUserCnt++;
		}
		else
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "TeamSurvivalMode::GetCurTeamUserCnt() - User's ModeRecord is Not Exist(%d).", i );
		}
	}

	return iUserCnt;
}

bool BattleMode::CheckRoundJoin( User *pSend )
{
	if( m_ModeState != MS_RESULT && m_ModeState != MS_RESULT_WAIT )
		return false;

	ModeRecord *pRecord = FindModeRecord( pSend );
	if( !pRecord )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "TeamSurvivalMode::CheckRoundJoin - %s Not Exist Record",
								 pSend->GetPublicID().c_str() );
		return false;
	}

	pRecord->pUser->EquipDBItemToAllChar();
	SetFirstRevivalTime( pRecord );

	int iModeState = MS_RESULT_WAIT;

	DWORD dwPastTime = TIMEGETTIME() - m_dwStateChangeTime;

	int iPreCnt = GetCurTeamUserCnt( pRecord->pUser->GetTeam() );

	pRecord->eState = RS_PLAY;
	pRecord->StartPlaying();        //( 관전X, 데스타임X )	
	pRecord->pUser->StartCharLimitDate( Mode::GetCharLimitCheckTime(), __FILE__, __LINE__ );
	pRecord->pUser->StartEtcItemTime( __FUNCTION__ );

	UpdateCurKillPoint( pRecord->pUser->GetTeam(), iPreCnt );

	SP2Packet kPacket( STPK_ROUND_JOIN );
	kPacket << pRecord->pUser->GetPublicID();
	kPacket << iModeState;
	kPacket << dwPastTime;
	kPacket << GetSelectCharTime();
	kPacket << m_dwCurRoundDuration;
	SendRoomAllUser( kPacket );

	SP2Packet kModeInfoPk( STPK_MODE_INFO );
	GetModeInfo( kModeInfoPk );
	SendRoomPlayUser( kModeInfoPk );
	
	return true;
}

void BattleMode::CheckUserLeaveEnd()
{
	if( m_bRoundSetEnd ) return;

	LOG.PrintTimeAndLog(LOG_DEBUG_LEVEL,"[<<<<< CheckUserLeaveEnd >>>>>  m_bRoundSetEnd[%d]",m_bRoundSetEnd);


	if( GetTeamUserCnt( TEAM_BLUE ) == 0 ||	GetTeamUserCnt( TEAM_RED ) == 0 )
	{
		CheckRoundEnd( false );
		LOG.PrintTimeAndLog(LOG_DEBUG_LEVEL,"[<<<<< CheckUserLeaveEnd >>> CheckRoundEnd >>>>> [TEAM_BLUE_COUNT][%d] [TEAM_RED_COUNT][%d]",GetTeamUserCnt( TEAM_BLUE ),GetTeamUserCnt( TEAM_RED ));

	}
}

void BattleMode::UpdateDropDieRecord( User *pDier, const ioHashString &szAttacker, const ioHashString &szBestAttacker )
{
	ModeRecord *pDieRecord = FindModeRecord( pDier );
	if( !pDieRecord )	return;

	// 마지막으로 타격한 유저와 죽은 유저
	TeamType eDieTeam = pDieRecord->pUser->GetTeam();
	if( !szAttacker.IsEmpty() && pDier->GetPublicID() != szAttacker )
	{
		ModeRecord *pAttRecord = FindModeRecord( szAttacker );
		if( !pAttRecord )	return;

		float fKillPoint = 0.5f + ( (float)GetKillPoint( pAttRecord->pUser->GetTeam() ) * 0.5f );
		if( pDieRecord->pUser->GetTeam() != pAttRecord->pUser->GetTeam() )
		{
			pAttRecord->AddKillCount( m_pCreator->GetRoomStyle(), GetModeType(), fKillPoint );
			pDieRecord->AddDeathCount( m_pCreator->GetRoomStyle(), GetModeType(), GetDeathPoint( pDieRecord->pUser->GetTeam() ) );

			if( eDieTeam == TEAM_RED )
				SetScore( TEAM_BLUE );
			else if( eDieTeam == TEAM_BLUE )
				SetScore( TEAM_RED );
		}
		else
		{
			if( pAttRecord->pUser != pDieRecord->pUser )	// team kill
			{
				pAttRecord->AddKillCount( m_pCreator->GetRoomStyle(), GetModeType(), -fKillPoint );
				pDieRecord->AddDeathCount( m_pCreator->GetRoomStyle(), GetModeType(), GetDeathPoint( pDieRecord->pUser->GetTeam() ) );
			}
			else
			{
				pDieRecord->AddDeathCount( m_pCreator->GetRoomStyle(), GetModeType(), GetDeathPoint( pDieRecord->pUser->GetTeam() ) );
			}

			if( eDieTeam == TEAM_RED )
				SetScore( TEAM_BLUE );
			else if( eDieTeam == TEAM_BLUE )
				SetScore( TEAM_RED );
		}
	}
	else
	{
		pDieRecord->AddDeathCount( m_pCreator->GetRoomStyle(), GetModeType(), GetDeathPoint( pDieRecord->pUser->GetTeam() ) );

		if( eDieTeam == TEAM_RED )
			SetScore( TEAM_BLUE );
		else if( eDieTeam == TEAM_BLUE )
			SetScore( TEAM_RED );
	}

	// 가장 많은 데미지를 입힌 유저
	if( !szBestAttacker.IsEmpty() && pDier->GetPublicID() != szBestAttacker )
	{
		ModeRecord *pAttRecord = FindModeRecord( szBestAttacker );
		if( !pAttRecord )	return;

		float fKillPoint = 0.5f + ( (float)GetKillPoint( pAttRecord->pUser->GetTeam() ) * 0.5f );
		if( pDieRecord->pUser->GetTeam() != pAttRecord->pUser->GetTeam() )
			pAttRecord->AddKillCount( m_pCreator->GetRoomStyle(), GetModeType(), fKillPoint );
	}
}

void BattleMode::UpdateWeaponDieRecord( User *pDier, const ioHashString &szAttacker, const ioHashString &szBestAttacker )
{
	ModeRecord *pDieRecord = FindModeRecord( pDier );
	if( !pDieRecord )	return;

	// 마지막으로 타격한 유저와 죽은 유저
	TeamType eDieTeam = pDieRecord->pUser->GetTeam();
	ModeRecord *pKillRecord = FindModeRecord( szAttacker );
	if( pKillRecord )
	{
		float fKillPoint = 0.5f + ( (float)GetKillPoint( pKillRecord->pUser->GetTeam() ) * 0.5f );
		if( pKillRecord->pUser->GetTeam() != pDier->GetTeam() )
		{
			pKillRecord->AddKillCount( m_pCreator->GetRoomStyle(), GetModeType(), fKillPoint );
			pDieRecord->AddDeathCount( m_pCreator->GetRoomStyle(), GetModeType(), GetDeathPoint( pDieRecord->pUser->GetTeam() ) );

			if( eDieTeam == TEAM_RED )
				SetScore( TEAM_BLUE );
			else if( eDieTeam == TEAM_BLUE )
				SetScore( TEAM_RED );
		}
		else
		{
			if( pKillRecord->pUser != pDier )	// team kill
			{
				pKillRecord->AddKillCount( m_pCreator->GetRoomStyle(), GetModeType(), -fKillPoint );
				pDieRecord->AddDeathCount( m_pCreator->GetRoomStyle(), GetModeType(), GetDeathPoint( pDieRecord->pUser->GetTeam() ) );
			}
			else
			{
				pDieRecord->AddDeathCount( m_pCreator->GetRoomStyle(), GetModeType(), GetDeathPoint( pDieRecord->pUser->GetTeam() ) );

				if( eDieTeam == TEAM_RED )
					SetScore( TEAM_BLUE );
				else if( eDieTeam == TEAM_BLUE )
					SetScore( TEAM_RED );
			}
		}
	}
	else
	{
		pDieRecord->AddDeathCount( m_pCreator->GetRoomStyle(), GetModeType(), GetDeathPoint( pDieRecord->pUser->GetTeam() ) );

		if( eDieTeam == TEAM_RED )
			SetScore( TEAM_BLUE );
		else if( eDieTeam == TEAM_BLUE )
			SetScore( TEAM_RED );
	}

	// 가장 많은 데미지를 입힌 유저
	ModeRecord *pBestAttackerRecord = FindModeRecord( szBestAttacker );
	if( pBestAttackerRecord )
	{
		float fKillPoint = 0.5f + ( (float)GetKillPoint( pBestAttackerRecord->pUser->GetTeam() ) * 0.5f );
		if( pBestAttackerRecord->pUser->GetTeam() != pDier->GetTeam() )
			pBestAttackerRecord->AddKillCount( m_pCreator->GetRoomStyle(), GetModeType(), fKillPoint );
	}
}

void BattleMode::UpdateCurKillPoint( TeamType eTeam, int iPreCnt )
{
	int iCurCnt = GetCurTeamUserCnt( eTeam );
	if( iCurCnt <= 0 || iPreCnt <= 0 )
		return;

	float fChangeRate = (float)iCurCnt / iPreCnt;

	if( eTeam == TEAM_RED && m_iBlueKillPoint > 0 )
		m_iBlueKillPoint = m_iBlueKillPoint * fChangeRate;
	else if( eTeam == TEAM_BLUE && m_iRedKillPoint > 0 )
		m_iRedKillPoint = m_iRedKillPoint * fChangeRate;

	UpdateCurKillPointRate();
}

void BattleMode::UpdateCurKillPointRate()
{
	float fRedCnt = GetCurTeamUserCnt( TEAM_BLUE ) * m_fWinScoreConstant * 100;
	float fBlueCnt = GetCurTeamUserCnt( TEAM_RED ) * m_fWinScoreConstant * 100;

	if( fRedCnt <= 0 || fBlueCnt <= 0 )
		return;

	if( m_iBlueKillPoint > 0 )
	{
		m_fBlueKillPointRate = m_iBlueKillPoint / fBlueCnt;
		m_fBlueKillPointRate = min( 1.0f, m_fBlueKillPointRate );
	}

	if( m_iRedKillPoint > 0 )
	{
		m_fRedKillPointRate = m_iRedKillPoint / fRedCnt;
		m_fRedKillPointRate = min( 1.0f, m_fRedKillPointRate );
	}
}

void BattleMode::SetScore( TeamType eTeam )
{
	if(eTeam == TEAM_BLUE)
		m_iBlueKillPoint += 100;
	else if(eTeam == TEAM_RED)
		m_iRedKillPoint += 100;

	UpdateCurKillPointRate();
}

void BattleMode::FinalRoundPoint( ModeRecord *pRecord, bool bAbuseUser, float fTotalVictoriesRate, float fTotalConsecutivelyRate )
{
	fTotalVictoriesRate = 1.0f;
	fTotalConsecutivelyRate = 1.0f;

	User *pUser = pRecord->pUser;
	if( !pUser )
		return;

	// 옵저버는 이 함수를 실행할 필요가 없다.
	if( pUser->IsObserver() || pUser->IsStealth() )
	{
		pUser->SetModeConsecutively( MT_NONE );       // 옵저버는 연속 게임 초기화
		return;
	}

	int iCurMaxSlot = pUser->GetCurMaxCharSlot();

	if( m_dwModePointTime == 0 || m_dwModeRecordPointTime == 0 )
		return;

	TeamType eWinTeam  = TEAM_NONE;
	if( m_iBlueTeamWinCnt > m_iRedTeamWinCnt )
		eWinTeam = TEAM_BLUE;
	else if( m_iRedTeamWinCnt > m_iBlueTeamWinCnt )
		eWinTeam = TEAM_RED;


	//승패 기록
	int iWinLoseTiePoint = 0;
	if( !bAbuseUser )
	{
		iWinLoseTiePoint = GetWinLoseTiePoint( pUser->GetTeam(), eWinTeam, (float)GetRecordPlayTime( pRecord ) / m_dwModeRecordPointTime );
		if( eWinTeam == pUser->GetTeam() )
			pUser->AddWinCount(  m_pCreator->GetRoomStyle(), GetModeType(), iWinLoseTiePoint );				
		else if( eWinTeam != TEAM_NONE )
			pUser->AddLoseCount(  m_pCreator->GetRoomStyle(), GetModeType(), iWinLoseTiePoint );
	}							

	//플레이 중인 모드
	ModeCategory ePlayMode = GetPlayModeCategory();

	//총 라운드 수 A
	float fRoundPoint = m_ModePointRound;			
	//스코어 차이  B
	float fScoreGap = GetResultScoreGapValue( false, eWinTeam );
	if( eWinTeam == TEAM_NONE )
		fScoreGap = 0.0f;
	//인원 보정 C
	float fUserCorrection = GetUserCorrection( eWinTeam, fRoundPoint, fScoreGap );
	//플레이 시간 보정값 D
	float fPlayTimeCorrection = (float)GetRecordPlayTime( pRecord ) / m_dwModePointTime;
	//페소보정값 E
	float fPesoCorrection = m_fPesoCorrection;
	//경험치 보정값 F
	float fExpCorrection  = m_fExpCorrection;
	//차단 G
	float fBlockPoint = pUser->GetBlockPointPer();
	//기여도 H
	float fContributePer = pRecord->fContributePer;
	//길드보너스 I
	pRecord->fBonusArray[BA_GUILD] = m_pCreator->GetGuildBonus( pRecord->pUser->GetTeam() );
	float fGuildBonus = pRecord->fBonusArray[BA_GUILD];
	//용병 보너스 J
	pRecord->fBonusArray[BA_SOLDIER_CNT] = Help::GetSoldierPossessionBonus( pUser->GetActiveCharCount() );
	float fSoldierCntBonus = pRecord->fBonusArray[BA_SOLDIER_CNT];
	//PC방 보너스 K
	if( pUser->IsPCRoomAuthority() )
	{
		pRecord->fBonusArray[BA_PCROOM_EXP] = Help::GetPCRoomBonusExp();
		pRecord->fBonusArray[BA_PCROOM_PESO]= Help::GetPCRoomBonusPeso();

		if( g_EventMgr.IsAlive( EVT_PCROOM_BONUS, pUser->GetChannelingType(), ePlayMode ) )
		{
			EventUserManager &rEventUserManager = pUser->GetEventUserMgr();
			PCRoomEventUserNode* pPcroomEventNode = static_cast<PCRoomEventUserNode*> ( rEventUserManager.GetEventUserNode( EVT_PCROOM_BONUS, ePlayMode ) );
			
			if( pPcroomEventNode )
				pPcroomEventNode->SetPesoAndExpBonus( pUser ,pRecord->fBonusArray[BA_PCROOM_PESO], pRecord->fBonusArray[BA_PCROOM_EXP], ePlayMode ); 
		}
	}
	float fPCRoomBonusExp  = pRecord->fBonusArray[BA_PCROOM_EXP];
	float fPCRoomBonusPeso = pRecord->fBonusArray[BA_PCROOM_PESO];
	//모드 보너스 L
	if( pUser )
	{
		// EVT_MODE_BONUS (1)
		//셔플 모드 일경우 모드 체크 건너뛰기 위해. 셔플 모드가 아닐경우 모드 체크 
		ModeCategory eModeBonus = ePlayMode;

		if( eModeBonus != MC_SHUFFLE )
			eModeBonus = MC_DEFAULT;

		EventUserManager &rEventUserManager = pUser->GetEventUserMgr();
		ModeBonusEventUserNode* pEvent1 = static_cast< ModeBonusEventUserNode* >( rEventUserManager.GetEventUserNode( EVT_MODE_BONUS, eModeBonus ) );
		if( pEvent1 )
		{
			if( pEvent1->IsEventMode( GetModeType(), eModeBonus ) )
				pRecord->fBonusArray[ BA_PLAYMODE ] = pEvent1->GetEventPer( fPCRoomBonusExp, pUser, eModeBonus );
		}

		// EVT_MODE_BONUS2 (2)
		ModeBonusEventUserNode* pEvent2 = static_cast< ModeBonusEventUserNode* >( rEventUserManager.GetEventUserNode( EVT_MODE_BONUS2, eModeBonus ) );
		if( pEvent2 )
		{
			if( pEvent2->IsEventMode( GetModeType(), eModeBonus ) )
				pRecord->fBonusArray[ BA_PLAYMODE ] += pEvent2->GetEventPer( fPCRoomBonusExp, pUser, eModeBonus );
		}
		
		if( pRecord->fBonusArray[BA_PLAYMODE] == 0.0f )
		{
			pRecord->fBonusArray[BA_PLAYMODE] = m_fPlayModeBonus;
		}
	}
	float fModeBonus = pRecord->fBonusArray[BA_PLAYMODE];

	//친구 보너스 M	
	if( pUser->IsPCRoomAuthority() )
	{		
		pRecord->fBonusArray[BA_FRIEND] = min( GetPcRoomMaxFriendBonus(), GetPcRoomFriendBonus() * (float)GetSameFriendUserCnt( pUser ) );
	}
	else
	{
		pRecord->fBonusArray[BA_FRIEND] = min( GetMaxFriendBonus(), GetFriendBonus() * (float)GetSameFriendUserCnt( pUser ) );
	}

	float fFriendBonusPer = pRecord->fBonusArray[BA_FRIEND];
	// 이벤트 경험치 보너스 N
	float fEventBonus = 0.0f;
	if( pUser )
	{
		EventUserManager &rEventUserManager = pUser->GetEventUserMgr();
		ExpEventUserNode *pEventNode = static_cast<ExpEventUserNode*> ( rEventUserManager.GetEventUserNode( EVT_EXP, ePlayMode ) );
		if( pEventNode )
		{
			pRecord->fBonusArray[BA_EVENT] = pEventNode->GetEventPer( fPCRoomBonusExp, pUser, ePlayMode );
			fEventBonus = pRecord->fBonusArray[BA_EVENT];
		}

		// second event : evt_exp2
		ExpEventUserNode* pExp2 = static_cast< ExpEventUserNode* >( rEventUserManager.GetEventUserNode( EVT_EXP2, ePlayMode ) );
		if( pExp2 )
		{
			pRecord->fBonusArray[ BA_EVENT ] += pExp2->GetEventPer( fPCRoomBonusExp, pUser, ePlayMode );
			fEventBonus = pRecord->fBonusArray[ BA_EVENT ];
		}
	}
	// 이벤트 페소 보너스 O
	float fPesoEventBonus = 0.0f;
	if( pUser )
	{
		EventUserManager &rEventUserManager = pUser->GetEventUserMgr();
		PesoEventUserNode *pEventNode = static_cast<PesoEventUserNode*> ( rEventUserManager.GetEventUserNode( EVT_PESO, ePlayMode ) );
		if( pEventNode )
		{
			pRecord->fBonusArray[BA_EVENT_PESO] = pEventNode->GetPesoPer( fPCRoomBonusPeso, pUser, ePlayMode );
			fPesoEventBonus = pRecord->fBonusArray[BA_EVENT_PESO];
		}

		// second event : evt_peso2
		PesoEventUserNode* pPeso2 = static_cast< PesoEventUserNode* >( rEventUserManager.GetEventUserNode( EVT_PES02, ePlayMode ) );
		if( pPeso2 )
		{
			pRecord->fBonusArray[ BA_EVENT_PESO ] += pPeso2->GetPesoPer( fPCRoomBonusPeso, pUser, ePlayMode );
			fPesoEventBonus = pRecord->fBonusArray[ BA_EVENT_PESO ];
		}
	}
	// 권한 아이템 보너스 P
	float fEtcItemBonus = 0.0f;
	float fEtcItemPesoBonus = 0.0f;
	float fEtcItemExpBonus  = 0.0f;
	if( pUser )
	{
		ioUserEtcItem *pUserEtcItem = pUser->GetUserEtcItem();
		if( pUserEtcItem )
		{
			ioEtcItem *pItemItem = g_EtcItemMgr.FindEtcItem( ioEtcItem::EIT_ETC_PESO_EXP_BONUS );
			ioUserEtcItem::ETCITEMSLOT kSlot;
			if( pUserEtcItem->GetEtcItem( ioEtcItem::EIT_ETC_PESO_EXP_BONUS, kSlot) && pItemItem )
			{
				pRecord->fBonusArray[BA_ETC_ITEM] = ( (float) pItemItem->GetUseValue() / 100.0f ); // 20 -> 0.20
				fEtcItemBonus = pRecord->fBonusArray[BA_ETC_ITEM];
			}

			pItemItem = g_EtcItemMgr.FindEtcItem( ioEtcItem::EIT_ETC_PESO_BONUS );
			if( pUserEtcItem->GetEtcItem( ioEtcItem::EIT_ETC_PESO_BONUS, kSlot ) && pItemItem )
			{
				pRecord->fBonusArray[BA_ETC_ITEM_PESO] = ( (float) pItemItem->GetUseValue() / 100.0f ); // 20 -> 0.20
				fEtcItemPesoBonus = pRecord->fBonusArray[BA_ETC_ITEM_PESO];
			}

			pItemItem = g_EtcItemMgr.FindEtcItem( ioEtcItem::EIT_ETC_EXP_BONUS );
			if( pUserEtcItem->GetEtcItem( ioEtcItem::EIT_ETC_EXP_BONUS, kSlot ) && pItemItem )
			{
				pRecord->fBonusArray[BA_ETC_ITEM_EXP] = ( (float) pItemItem->GetUseValue() / 100.0f ); // 20 -> 0.20
				fEtcItemExpBonus = pRecord->fBonusArray[BA_ETC_ITEM_EXP];
			}
		}
	}
	// 진영전 보너스 Q
	if( m_pCreator->GetRoomStyle() == RSTYLE_LADDERBATTLE )
		pRecord->fBonusArray[BA_CAMP_BONUS] = Help::GetLadderBonus();
	float fCampBattleBonus = pRecord->fBonusArray[BA_CAMP_BONUS];

	// 시상식 보너스 R
	float fAwardBonus = pRecord->fBonusArray[BA_AWARD_BONUS];

	// 영웅전 타이틀 보너스 S
	pRecord->fBonusArray[BA_HERO_TITLE_PESO] = GetHeroTitleBonus( pUser );
	float fHeroTitlePesoBonus = pRecord->fBonusArray[BA_HERO_TITLE_PESO];

	// 연속 모드 보너스 T 14.04.09부로 사용하지 않음
	//pRecord->fBonusArray[BA_MODE_CONSECUTIVELY] = pUser->GetModeConsecutivelyBonus();
	//float fModeConsecutivelyBonus = (1.0f + pRecord->fBonusArray[BA_MODE_CONSECUTIVELY]) * fTotalConsecutivelyRate;
	float fModeConsecutivelyBonus = 1.0f;

	//획득 경험치
	float fAcquireExp       = 0.0f;
	float fExpPlusValue     = max( 0.0f, fContributePer + fGuildBonus + fSoldierCntBonus + fPCRoomBonusExp + fModeBonus + fFriendBonusPer + fEventBonus + fEtcItemBonus + fCampBattleBonus + fEtcItemExpBonus );
	float fExpTotalMultiply = fUserCorrection * fPlayTimeCorrection * fExpCorrection * fBlockPoint * fExpPlusValue;
	char szLogArguWinLose[MAX_PATH]="";
	char szLogArguPlusMinus[MAX_PATH]="";
	if( pUser->GetTeam() == eWinTeam )
	{
		fAcquireExp = ( fRoundPoint + fScoreGap ) * fExpTotalMultiply;
		StringCbCopy( szLogArguWinLose, sizeof(szLogArguWinLose), "WIN" );
		StringCbCopy( szLogArguPlusMinus, sizeof(szLogArguPlusMinus), "+" );
	}
	else
	{
		fAcquireExp = ( fRoundPoint - fScoreGap ) * fExpTotalMultiply;
		StringCbCopy( szLogArguWinLose, sizeof(szLogArguWinLose), "LOSE" );
		StringCbCopy( szLogArguPlusMinus, sizeof(szLogArguPlusMinus), "-" );
	}
	fAcquireExp = fAcquireExp * fModeConsecutivelyBonus;
	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "[%d] %s [%d]: %s EXP : ( %.2f %s %.2f ) x %.2f x %.4f x %.2f x %.2f x ( %.2f + %.2f + %.2f + %.2f + %.2f + %.2f + %.2f + %.2f + %.2f + %.2f ) x %.2f = %.2f",
		                    m_pCreator->GetRoomIndex(), pUser->GetPublicID().c_str(), (int)bAbuseUser, szLogArguWinLose, 
		                    fRoundPoint, szLogArguPlusMinus, fScoreGap, fUserCorrection, fPlayTimeCorrection, fExpCorrection, fBlockPoint, fContributePer, fGuildBonus, 
							fSoldierCntBonus, fPCRoomBonusExp, fModeBonus, fFriendBonusPer, fEventBonus, fEtcItemBonus, fCampBattleBonus, fEtcItemExpBonus, fModeConsecutivelyBonus, fAcquireExp );

	if( m_bTournamentRoom )
	{
		// 대회 경기방은 경험치 없음. 
		fAcquireExp = 0.0f;           
	}

	//획득 페소
	float fAcquirePeso       = 0.0f;
	float fPesoPlusValue     = max( 0.0f, fContributePer + fGuildBonus + fPCRoomBonusPeso + fModeBonus + fFriendBonusPer + fPesoEventBonus + fEtcItemBonus + fCampBattleBonus + fAwardBonus + fEtcItemPesoBonus + fHeroTitlePesoBonus );
	float fPesoTotalMultiply = fUserCorrection * fPlayTimeCorrection * fPesoCorrection * fBlockPoint * fPesoPlusValue;
	if( pUser->GetTeam() == eWinTeam )
	{
		fAcquirePeso = ( fRoundPoint + fScoreGap ) * fPesoTotalMultiply;
	}
	else
	{
		fAcquirePeso = ( fRoundPoint - fScoreGap ) * fPesoTotalMultiply;
	}

	float fCurVictories = 0.0f;
	/*float fCurVictories = 0.0f;
	int iCurVictoriesCnt = pRecord->iVictories;
	if( pUser->IsLadderTeam() )
		iCurVictoriesCnt = pUser->GetMyVictories();

	int iGapVictories = iCurVictoriesCnt - Help::GetFirstVictories();
	if( iGapVictories >= 0 )
	{
		fCurVictories = Help::GetFirstVictoriesRate() + Help::GetVictoriesRate() * iGapVictories;
		fCurVictories = min( Help::GetMaxVictoriesRate(), fCurVictories );
	}

	pRecord->fBonusArray[BA_VICTORIES_PESO] = fCurVictories;*/

	float fVictoriesBonus = (1.0f + fCurVictories) * fTotalVictoriesRate;
	fAcquirePeso = fAcquirePeso * fVictoriesBonus;
	fAcquirePeso = fAcquirePeso * fModeConsecutivelyBonus;

	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "[%d] %s [%d]: %s PESO : (( %.2f %s %.2f ) x %.2f x %.4f x %.2f x %.2f x ( %.2f + %.2f + %.2f + %.2f + %.2f + %.2f + %.2f + %.2f + %.2f + %.2f + %.2f ) x %.2f x %.2f ) = %.2f",
		                    m_pCreator->GetRoomIndex(), pUser->GetPublicID().c_str(), (int)bAbuseUser, szLogArguWinLose, 
		                    fRoundPoint, szLogArguPlusMinus, fScoreGap, fUserCorrection, fPlayTimeCorrection, fPesoCorrection, fBlockPoint, fContributePer, fGuildBonus, fPCRoomBonusPeso, fModeBonus, fFriendBonusPer, fPesoEventBonus, fEtcItemBonus, fCampBattleBonus, fAwardBonus, fEtcItemPesoBonus, fHeroTitlePesoBonus, fModeConsecutivelyBonus, fVictoriesBonus, fAcquirePeso );

	fAcquirePeso += 0.5f;     //반올림

	if( m_bTournamentRoom )
	{
		// 대회 경기방은 페소 고정. 
		fAcquirePeso = g_TournamentManager.GetRegularTournamentBattleRewardPeso( m_dwTournamentIndex );             
	}

	//어뷰즈 판정
	if( bAbuseUser )
	{
		fAcquireExp = 0.0f;
		fAcquirePeso= 0.0f;
	}
	else
	{
		// 플레이 시간 이벤트
		EventUserManager &rEventUserManager = pUser->GetEventUserMgr();
		ChanceMortmainCharEventUserNode *pEventNode = static_cast<ChanceMortmainCharEventUserNode*> ( rEventUserManager.GetEventUserNode( EVT_CHANCE_MORTMAIN_CHAR ) );
		if( pEventNode )
		{
			pEventNode->UpdatePlayTime( pUser, GetRecordPlayTime( pRecord ) );
		}
		PlayTimePresentEventUserNode *pPlayTimePresentEventNode = static_cast<PlayTimePresentEventUserNode*> ( rEventUserManager.GetEventUserNode( EVT_PLAYTIME_PRESENT ) );
		if( pPlayTimePresentEventNode )
			pPlayTimePresentEventNode->UpdatePlayTime( pUser, GetRecordPlayTime( pRecord ) );
	}

	//진영전이 종료되면 포인트와 페소를 지급하지 않는다.
	if( m_pCreator->GetRoomStyle() == RSTYLE_LADDERBATTLE )
	{
		if( !g_LadderTeamManager.IsCampBattlePlay() )
		{
			fAcquireExp = 0.0f;
			fAcquirePeso= 0.0f;
		}
	}
	pRecord->iTotalExp  = 0;
	pRecord->iTotalPeso = 0;
	pRecord->iTotalAddPeso = 0;

	// 페소 지급.
	pUser->AddMoney( (int)fAcquirePeso );
	g_LogDBClient.OnInsertGameLogInfo(LogDBClient::GLT_PESO_GAIN, pUser, 0, 0, LogDBClient::LET_MODE, LogDBClient::PT_BATTLE, GetModeType(), 0, (int)fAcquirePeso, NULL);

	pRecord->iTotalPeso = (int)fAcquirePeso;
	pRecord->iTotalAddPeso = (int)fAcquirePeso;

	int i = 0;
	enum { MAX_GET_POINT_CHAR = 5, };

	DWORDVec dwPlayTimeList;
	dwPlayTimeList.clear();

	pRecord->iResultClassTypeList.clear();
	pRecord->iResultClassPointList.clear();
	pRecord->bResultLevelUP = false;		

	// 연속 모드 
	if( !bAbuseUser )
	{
		pUser->SetModeConsecutively( GetModeType() );
	}

	if( !bAbuseUser )
	{
		// 다음주 용병 가격을 위해 플레이 시간을 따로 저장한다.
		g_ItemPriceMgr.SetGradePlayTimeCollected( pRecord->pUser->GetGradeLevel(), GetRecordPlayTime( pRecord ) / 1000 );
	}

	// 출전 시간에 따라 상위 클래스를 구한다.
	DWORD dwTotalTime = pRecord->GetHighPlayingTime( MAX_GET_POINT_CHAR, pRecord->iResultClassTypeList, dwPlayTimeList );
	if( dwTotalTime == 0 ) 
	{
		LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "Return Playing Time Zeor!!!" );
		return;
	}

	int iListSize = pRecord->iResultClassTypeList.size();
	for(i = 0; i < MAX_GET_POINT_CHAR; i++)
	{
		if( i >= iCurMaxSlot ) break;
		if( !COMPARE( i, 0, iListSize ) ) break;

		if( pRecord->iResultClassTypeList[i] == 0 ) continue;

		float fSoldierPer = (float)dwPlayTimeList[i] / dwTotalTime;
		int iCurPoint = ( fAcquireExp * fSoldierPer ) + 0.5f;     //반올림
		pRecord->iResultClassPointList.push_back( iCurPoint );
		pRecord->iTotalExp += pRecord->iResultClassPointList[i];

		// 용병 경험치 특별 아이템 - 보너스
		float fClassPoint = (float)pRecord->iResultClassPointList[i];
		float fClassBonus = pUser->GetSoldierExpBonus( pRecord->iResultClassTypeList[i] );
		float fAddEventExp = pUser->GetExpBonusEvent();
		float fSoldierExpBonus = ( fClassPoint * ( fClassBonus + fAddEventExp ) );	
		LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "ClassType[%d] PlayTime[%d - %d - %d] - WinPoint[%d] - Bonus[%.2f]", pRecord->iResultClassTypeList[i],
																					   dwPlayTimeList[i],
																					   dwTotalTime,
																					   GetRecordPlayTime( pRecord ),
																					   pRecord->iResultClassPointList[i], fSoldierExpBonus );

		// 경험치 지급 및 레벨업 확인
		if( pUser->IsClassTypeExerciseStyle( pRecord->iResultClassTypeList[i], EXERCISE_RENTAL ) == false )
			pUser->AddClassExp( pRecord->iResultClassTypeList[i], pRecord->iResultClassPointList[i] + fSoldierExpBonus );
		if( pUser->AddGradeExp( pRecord->iResultClassPointList[i] ) )
			pRecord->bResultLevelUP = true;

		// 
		pRecord->iResultClassPointList[i] += fSoldierExpBonus;

		if( pUser->IsClassTypeExerciseStyle( pRecord->iResultClassTypeList[i], EXERCISE_RENTAL ) )
		{
			// 계급 경험치만 획득하고 용병 경험치는 획득 안됨
			pRecord->iResultClassPointList[i] = 0;
		}
	}
	// 용병단과 용병들의 레벨업 보상을 지급한다.
	pRecord->iTotalPeso += pUser->GradeNClassUPBonus();	

	//미션
	static DWORDVec vValues;
	vValues.clear();
	if( pUser )
	{
		if( pRecord )
		{
			vValues.push_back(pRecord->GetAllPlayingTime());
			vValues.push_back(m_pCreator->GetRoomStyle());
			vValues.push_back(GetModeType());
			
			g_MissionMgr.DoTrigger(MISSION_CLASS_MODEPLAY, pUser, vValues);
		}
	}
}

void BattleMode::SendRoundResult( WinTeamType eWinTeam )
{

	if( eWinTeam == WTT_BLUE_TEAM)
		LOG.PrintTimeAndLog(LOG_DEBUG_LEVEL,"[<<<<< SendRoundResult >>>>> [WTT_BLUE_TEAM]");
	if( eWinTeam == WTT_RED_TEAM)
		LOG.PrintTimeAndLog(LOG_DEBUG_LEVEL,"[<<<<< SendRoundResult >>>>> [WTT_RED_TEAM ]");



	SP2Packet kPacket( STPK_ROUND_END );
	kPacket << eWinTeam;
	kPacket << m_iRedTeamWinCnt;
	kPacket << m_iBlueTeamWinCnt;

	kPacket << m_fRedKillPointRate;
	kPacket << m_fBlueKillPointRate;
	
	kPacket << GetPlayingUserCnt();

	int iRecordCnt = GetRecordCnt();
	for( int i=0 ; i<iRecordCnt ; i++ )
	{
		ModeRecord *pRecord = FindModeRecord( i );
		if( !pRecord || pRecord->eState == RS_LOADING )
			continue;

		kPacket << pRecord->pUser->GetPublicID();

		//
		int iMyVictories = 0;
		if( pRecord->pUser )
		{
			if( m_bRoundSetEnd && eWinTeam != WTT_DRAW && eWinTeam != WTT_NONE )
				pRecord->pUser->IncreaseMyVictories( IsWinTeam(eWinTeam, pRecord->pUser->GetTeam()) );

			iMyVictories = pRecord->pUser->GetMyVictories();
		}

		kPacket << iMyVictories;
		//

		//
		int iKillSize = pRecord->iKillInfoMap.size();
		kPacket << iKillSize;

		LOOP_GUARD();
		KillDeathInfoMap::iterator iter_k = pRecord->iKillInfoMap.begin();
		while( iter_k != pRecord->iKillInfoMap.end() )
		{
			kPacket << iter_k->first;
			kPacket << iter_k->second;

			++iter_k;
		}
		LOOP_GUARD_CLEAR();

		int iDeathSize = pRecord->iDeathInfoMap.size();
		kPacket << iDeathSize;

		LOOP_GUARD();
		KillDeathInfoMap::iterator iter_d = pRecord->iDeathInfoMap.begin();
		while( iter_d != pRecord->iDeathInfoMap.end() )
		{
			kPacket << iter_d->first;
			kPacket << iter_d->second;

			++iter_d;
		}
		LOOP_GUARD_CLEAR();
		//

		kPacket << pRecord->iCurRank;
		kPacket << pRecord->iPreRank;
	}

	kPacket << m_bRoundSetEnd;

	FillResultSyncUser( kPacket );

	SendRoomAllUser( kPacket );

	// 클라이언트가 위의 패킷을 받으면 서버로 캐릭터 살리는 패킷을 보내는데 이해가 되지 않는다. 
	// 그냥 아래처럼하면 패킷 보낼 필요 없지 않을까?  LJH..... 20081002
	for(int i = 0;i < iRecordCnt;i++)
	{
		ModeRecord *pRecord = FindModeRecord( i );
		if( !pRecord || !pRecord->pUser )
			continue;
		pRecord->pUser->SetCharDie( false );
	}

#ifdef ANTIHACK
	SendRelayGroupTeamWinCnt();  
#endif
}

void BattleMode::BattleModeWarTimeStart( User *pSend, SP2Packet &rkPacket)
{
	if( !pSend ) return;
	
	int iWarTimeStartType = 0;
	ioHashString szWarTimeStartName;	
	ioHashString szWarTimeEndName;
	int  iUserHP_Value = 100;		// 유저 HP 정보 갱신 

	PACKET_GUARD_VOID( rkPacket.Read(iWarTimeStartType) ); 
	PACKET_GUARD_VOID( rkPacket.Read(szWarTimeStartName) );
	PACKET_GUARD_VOID( rkPacket.Read(iUserHP_Value) );
	PACKET_GUARD_VOID( rkPacket.Read(szWarTimeEndName) );
	

#ifdef BATTLE_MODE_BY_BCKIM_DEBUG	// 2019-02-14 by bckim, 배틀 모드 추가
	int iRecordCnt_temp = m_vRecordList.size();
	for( int i=0; i<iRecordCnt_temp;i++)
	{	
		BattleModeRecord* pTempUser = &m_vRecordList[i];            

		if ( pTempUser->pUser->GetTeam() == TEAM_BLUE )
			LOG.PrintTimeAndLog(LOG_DEBUG_LEVEL,"[<<<<< HP&&ORDER CHECK!!! >>>>> [ TEAM_BLUE ]UserRecord[%s]>>m_iHP[%d] order[%d] ",pTempUser->pUser->GetPublicID().c_str(),pTempUser->m_iHP, pTempUser->m_iBattle_Order);
		else if ( pTempUser->pUser->GetTeam() == TEAM_RED )
			LOG.PrintTimeAndLog(LOG_DEBUG_LEVEL,"[<<<<< HP&&ORDER CHECK!!! >>>>> [ TEAM_RED  ]UserRecord[%s]>>m_iHP[%d] order[%d] ",pTempUser->pUser->GetPublicID().c_str(),pTempUser->m_iHP, pTempUser->m_iBattle_Order);
		else
			LOG.PrintTimeAndLog(LOG_DEBUG_LEVEL,"[<<<<< HP&&ORDER CHECK!!! >>>>> [ TEAM_NONE ]UserRecord[%s]>>m_iHP[%d] order[%d] ",pTempUser->pUser->GetPublicID().c_str(),pTempUser->m_iHP, pTempUser->m_iBattle_Order);
	}
#endif // BATTLE_MODE_BY_BCKIM_DEBUG	// 2019-02-14 by bckim, 배틀 모드 추가	


	BattleModeRecord *pSyncUserRecord =(BattleModeRecord*)FindBattleModeRecord(pSend);
	if( NULL == pSyncUserRecord )
		return;

	if( pSend->IsObserver() || USER_STATE_DEAD == GetUserState(pSyncUserRecord) )
	{
		LOG.PrintTimeAndLog(LOG_DEBUG_LEVEL,"[<<BATTLE_MODE::Observer or USER_STATE_DEAD >> User[%s]",pSend->GetPublicID().c_str());
		return;
	}


	switch(iWarTimeStartType)
	{
	case WARTIME_PLAY:			// 첫출전 
		{
			if( szWarTimeEndName.IsEmpty() && GetTeamTagCount(pSend->GetTeam()) > 0)			// 참전 대상이 맞는지 확인 
			{ 
				SP2Packet kReturn( STPK_BATTLE_MODE_WARTIME_START );
				PACKET_GUARD_VOID( kReturn.Write(WARTIME_START_ERROR) );
				PACKET_GUARD_VOID( kReturn.Write(szWarTimeStartName) );
				pSend->SendMessage( kReturn );
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[WarTimeStart>>>WARTIME_PLAY] ERROR pUser[%s]",pSend->GetPublicID().c_str());
				return;
			}
		}
		break;
	case WARTIME_TAG:
		{
			BattleModeRecord *pSyncUserEndRecord =(BattleModeRecord*)FindBattleModeRecord(szWarTimeEndName);	
			if( pSyncUserRecord && GetBattleModeTagFlag(pSyncUserRecord) && GetEntryActivation(pSyncUserEndRecord) != ENTRY_STATE_PROCEEDING )
			{
				InitBattleModeTagFalg(pSyncUserRecord);				
				SetUserState(pSyncUserEndRecord,USER_STATE_WAITING );
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[WarTimeStart>>>WARTIME_TAG] IN[%s] OUT[%s]",pSend->GetPublicID().c_str(),szWarTimeEndName);
			}
			else
			{
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[WarTimeStart>>>WARTIME_TAG] ERROR IN[%s] OUT[%s]",pSend->GetPublicID().c_str(),szWarTimeEndName);
				return;
			}			
		}
		break;
	case ENTRY_ACTION:			// 난입	// 난입 가능 유저인지 ?
		{
			// if( szWarTimeEndName.IsEmpty() && GetEntryActivation(pSyncUserRecord) == ENTRY_STATE_BEFORE)
			if( GetRecordOnBoardEntryActTeam(pSend->GetTeam()) == NULL && GetEntryActivation(pSyncUserRecord) == ENTRY_STATE_BEFORE) 
			{
				SetEntryActivation( pSyncUserRecord,ENTRY_STATE_PROCEEDING);		// 여기 함수 에서 시간 설정 함 
				SetUserState(pSyncUserRecord,USER_STATE_IN_BATTLE );
				pSyncUserRecord->m_bEntryState = false;
				
				if( GetBattleModeTagFlag(pSyncUserRecord) )						// 현재 테그 요청상태중이면 안되요~
					SetBattleModeTagFalg(pSyncUserRecord, false);				// 테그 상태 false 변경하고 난입 처리 

				SP2Packet kReturn( STPK_BATTLE_MODE_WARTIME_START );
				PACKET_GUARD_VOID( kReturn.Write(WARTIME_START_ERROR) );
				PACKET_GUARD_VOID( kReturn.Write(szWarTimeStartName) );
				pSend->SendMessage( kReturn );
				
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[WarTimeStart>>>ENTRY_ACTION] [%s]",pSend->GetPublicID().c_str());
			}
			else
			{
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[WarTimeStart>>>ENTRY_ACTION] ERROR [%s] ENTRY_STATE[%d]",pSend->GetPublicID().c_str(),(int)GetEntryActivation(pSyncUserRecord) );
				return;
			}
		}
		break;		
	case WARTIME_DIE:			// 사망 후 다음 주자인데.. 난입중인 유저는 아닌데 ? 
		{			
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[WarTimeStart>>>WARTIME_DIE] [%s]",pSend->GetPublicID().c_str());
		}
		break;		
	default:
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[WarTimeStart>>> DEFAULT] ERROR [%s]",pSend->GetPublicID().c_str());
		}
		return; 
	}
		
	SetBattleModeUserHp( pSyncUserRecord, iUserHP_Value);				// 유저 HP 정보 
	SetUserState(pSyncUserRecord,USER_STATE_IN_BATTLE );				// 들어오는넘 상태 변경 


	SP2Packet kReturn( STPK_BATTLE_MODE_WARTIME_START );
	PACKET_GUARD_VOID( kReturn.Write(WARTIME_START_OK) );
	PACKET_GUARD_VOID( kReturn.Write(iWarTimeStartType) );
	PACKET_GUARD_VOID( kReturn.Write(pSend->GetPublicID()) );
	PACKET_GUARD_VOID( kReturn.Write(szWarTimeStartName) );
	PACKET_GUARD_VOID( kReturn.Write(szWarTimeEndName) );
	m_pCreator->RoomSendPacketTcp( kReturn );

	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[WarTimeStart>>>SUCCESS] pSend_IN[%s] OUT_USER[%s]",pSend->GetPublicID().c_str(),szWarTimeEndName);
	return;
}

void BattleMode::BattleModeTagWaitForRequest( User *pSend, SP2Packet &rkPacket)  // xprm dycjd 
{
	if( !pSend ) return;
	
	int iUserHP_Value = 100;		// 유저 HP 정보 갱신 
	bool bTagFlag = false;
	ioHashString szWarTimeStartName;

	PACKET_GUARD_VOID( rkPacket.Read(szWarTimeStartName) );
	PACKET_GUARD_VOID( rkPacket.Read(iUserHP_Value) );	
	PACKET_GUARD_VOID( rkPacket.Read(bTagFlag) );
	
	if( pSend->IsObserver() )
	{
		LOG.PrintTimeAndLog(LOG_DEBUG_LEVEL,"[<<BATTLE_MODE::Observer>> User[%s]",pSend->GetPublicID().c_str());
		return;
	}

	BattleModeRecord *pSyncUserRecord =(BattleModeRecord*)FindBattleModeRecord(pSend);
	if( pSyncUserRecord)
	{
		SetBattleModeUserHp( pSyncUserRecord, iUserHP_Value);		// 유저 HP 정보 
		if ( SetBattleModeTagFalg(pSyncUserRecord,bTagFlag) == false || GetUserState(pSyncUserRecord) == USER_STATE_DEAD )		// 변경 사항 없음 sync X  || 죽은넘은 테그 요청 못하지 
		{
			SP2Packet kReturn( STPK_BATTLE_MODE_TAG_WAIT_FOR_REQUEST );
			PACKET_GUARD_VOID( kReturn.Write( TAG_WAIT_FOR_REQUEST_EXCEPTION ));
			PACKET_GUARD_VOID( kReturn.Write(pSend->GetPublicID()) );
			PACKET_GUARD_VOID( kReturn.Write( GetBattleModeTagFlag(pSyncUserRecord) ));
			LOG.PrintTimeAndLog(LOG_DEBUG_LEVEL,"[TAG_WAIT_FOR_REQUEST:TAG_WAIT_FOR_REQUEST_EXCEPTION]>>Clent[%s][%d]",pSend->GetPublicID().c_str(),GetBattleModeTagFlag(pSyncUserRecord) );
			pSend->SendMessage( kReturn );
			return;
		}
		
		SP2Packet kReturn( STPK_BATTLE_MODE_TAG_WAIT_FOR_REQUEST );
		PACKET_GUARD_VOID( kReturn.Write(TAG_WAIT_FOR_REQUEST_OK) );
		PACKET_GUARD_VOID( kReturn.Write(pSend->GetPublicID()) );
		PACKET_GUARD_VOID( kReturn.Write( GetBattleModeTagFlag(pSyncUserRecord) ));
		LOG.PrintTimeAndLog(LOG_DEBUG_LEVEL,"[TAG_WAIT_FOR_REQUEST:TAG_WAIT_FOR_REQUEST_OK]>>Clent[%s][%d]",pSend->GetPublicID().c_str(),GetBattleModeTagFlag(pSyncUserRecord) );
		m_pCreator->RoomSendPacketTcp( kReturn );
		return;
	}

	LOG.PrintTimeAndLog(LOG_DEBUG_LEVEL,"[TAG_WAIT_FOR_REQUEST:pSyncUserRecord=NULL]>>Clent[%s][%d]",pSend->GetPublicID().c_str(),GetBattleModeTagFlag(pSyncUserRecord) );
	return;
}

void BattleMode::BattleModeTagAccept( User *pSend, SP2Packet &rkPacket)		// 테그 수락
{
	if( !pSend ) return;

	if( pSend->IsObserver() )
	{
		LOG.PrintTimeAndLog(LOG_DEBUG_LEVEL,"[<<BATTLE_MODE::Observer>> User[%s]",pSend->GetPublicID().c_str());
		return;
	}

	ioHashString szWarTimeUserName;
	int iUserHP_WarTimeUser = 100;		// 유저 HP 정보 갱신 
	ioHashString szTagToWarTimeUserName;
	int iUserHP_TagUser = 100;			// 유저 HP 정보 갱신 

	PACKET_GUARD_VOID( rkPacket.Read(szWarTimeUserName) );
	PACKET_GUARD_VOID( rkPacket.Read(iUserHP_WarTimeUser) );	
	PACKET_GUARD_VOID( rkPacket.Read(szTagToWarTimeUserName) );
	PACKET_GUARD_VOID( rkPacket.Read(iUserHP_TagUser) );
	
	BattleModeRecord *pSyncCurUserRecord =  (BattleModeRecord*)FindModeRecord( pSend );
	BattleModeRecord *pSyncNextUserRecord =  (BattleModeRecord*)FindModeRecord(szTagToWarTimeUserName);

	if(  NULL == pSyncCurUserRecord || NULL == pSyncNextUserRecord)
		return;
		
	if( CheckTagAcceptTime(pSyncNextUserRecord) == false )
	{
		SP2Packet kReturn( STPK_BATTLE_MODE_TAG_ACCEPT );
		PACKET_GUARD_VOID( kReturn.Write(TAG_ACCEPT_NOT_TIME) );
		PACKET_GUARD_VOID( kReturn.Write(pSend->GetPublicID()) );
		PACKET_GUARD_VOID( kReturn.Write(GetTagAcceptTime(pSyncNextUserRecord)) );
		pSend->SendMessage(kReturn);

		LOG.PrintTimeAndLog(LOG_DEBUG_LEVEL,"[CTPK_BATTLE_MODE_TAG_ACCEPT::TAG_ACCEPT_NOT_TIME>>Clent[%s]>>[%u]",pSend->GetPublicID().c_str(),GetTagAcceptTime(pSyncNextUserRecord));
		return;
	}

	if( GetBattleModeTagFlag(pSyncNextUserRecord)												// 테그 요청한넘 맞음 ?
		&& GetEntryActivation(pSyncNextUserRecord) != ENTRY_STATE_PROCEEDING					// 현재 난입 플레이중이 아닌거임?
		&& pSyncCurUserRecord->pUser->GetTeam() == pSyncNextUserRecord->pUser->GetTeam())		// 같은 팀인감 ?		
	{
		IncreaseTeamTagCount(pSend->GetTeam());				// 테그 성공 count up!
		SetBattleModeUserHp(pSyncCurUserRecord, iUserHP_WarTimeUser);	// 현 참전중인 유저( 대기상태로 변경될 유져) 
		SetBattleModeUserHp(pSyncNextUserRecord, iUserHP_TagUser);		// 참전할 유저 (출전할 유저)
		InitTagAcceptTime(pSyncCurUserRecord);							// 들어가는넘.. 시간 넣어라.. 

		SP2Packet kReturn( STPK_BATTLE_MODE_TAG_ACCEPT );
		PACKET_GUARD_VOID( kReturn.Write(TAG_ACCEPT_OK) );
		PACKET_GUARD_VOID( kReturn.Write(pSend->GetPublicID()) );
		PACKET_GUARD_VOID( kReturn.Write(szTagToWarTimeUserName));
		m_pCreator->RoomSendPacketTcp(kReturn);	

		LOG.PrintTimeAndLog(LOG_DEBUG_LEVEL,"[CTPK_BATTLE_MODE_TAG_ACCEPT::TAG_ACCEPT_OK>>Clent[%s]>>[%s]",pSend->GetPublicID().c_str(),szTagToWarTimeUserName.c_str());
		return;
	}
	else
	{	
		SP2Packet kReturn( STPK_BATTLE_MODE_TAG_ACCEPT );
		PACKET_GUARD_VOID( kReturn.Write(BATTLE_MODE_EXCEPTION) );
		PACKET_GUARD_VOID( kReturn.Write(pSend->GetPublicID()) );
		pSend->SendMessage(kReturn);
		
		LOG.PrintTimeAndLog(LOG_DEBUG_LEVEL,"[CTPK_BATTLE_MODE_TAG_ACCEPT::BATTLE_MODE_EXCEPTION>>Clent[%s]>>[%s]",pSend->GetPublicID().c_str(),szTagToWarTimeUserName.c_str());
		return;
	}
}
void BattleMode::BattleModeEntryStart( User *pSend, SP2Packet &rkPacket)		// 난입 
{
	if( !pSend ) return;

	if( pSend->IsObserver() )
	{
		LOG.PrintTimeAndLog(LOG_DEBUG_LEVEL,"[<<BATTLE_MODE::Observer>> User[%s]",pSend->GetPublicID().c_str());
		return;
	}

	BattleModeRecord *pSyncUserRecord =(BattleModeRecord*)FindBattleModeRecord(pSend);
	if( NULL == pSyncUserRecord )
		return;	

	int iUserHP_Value = 0;
	bool bTagFlag = false;
	ioHashString szWarTimeStartName;	

	PACKET_GUARD_VOID( rkPacket.Read(szWarTimeStartName) );
	PACKET_GUARD_VOID( rkPacket.Read(iUserHP_Value) );	
	PACKET_GUARD_VOID( rkPacket.Read(bTagFlag) );

	if(	ENTRY_STATE_BEFORE != GetEntryActivation(pSyncUserRecord) )			// 가능한 기회가 있는지 확인해보보
	{
		SP2Packet kReturn( STPK_BATTLE_MODE_ENTRY_START );
		PACKET_GUARD_VOID( kReturn.Write( ENTRY_START_CHANCE_ONCE_EXCEPTION ));
		PACKET_GUARD_VOID( kReturn.Write(pSend->GetPublicID()) );
		PACKET_GUARD_VOID( kReturn.Write(GetEntryActionCount(pSyncUserRecord)));
		pSend->SendMessage( kReturn );
		LOG.PrintTimeAndLog(LOG_DEBUG_LEVEL,"[ENTRY_START_CHANCE_ONCE_EXCEPTION>>Clent[%s]EntryAct[%d]",pSend->GetPublicID().c_str(),GetEntryActivation(pSyncUserRecord));
		return;
	}

	if(	GetRecordOnBoardEntryActTeam(pSend->GetTeam()) != NULL	
		|| GetUserState(pSyncUserRecord) == USER_STATE_IN_BATTLE	// 전투중인데 혹시나 난입패킷이 들어오면 안됨. 
		|| GetUserState(pSyncUserRecord) == USER_STATE_DEAD)		// 현팀 난입중인 유저 있으면 안되요~// 죽은 상태인데 난입요청? 안되죠			
	{
		SP2Packet kReturn( STPK_BATTLE_MODE_ENTRY_START );
		PACKET_GUARD_VOID( kReturn.Write( ENTRY_START_ON_BOARD_EXCEPTION ));
		PACKET_GUARD_VOID( kReturn.Write(pSend->GetPublicID()) );
		PACKET_GUARD_VOID( kReturn.Write(GetEntryActionCount(pSyncUserRecord)));
		pSend->SendMessage( kReturn );
		LOG.PrintTimeAndLog(LOG_DEBUG_LEVEL,"[ENTRY_START_ON_BOARD_EXCEPTION>>Clent[%s]EntryAct[%d]",pSend->GetPublicID().c_str(),GetEntryActivation(pSyncUserRecord));
		return;
	}

	SetBattleModeUserHp( pSyncUserRecord, iUserHP_Value);		// 유저 HP 정보 

	// wartime start쪽으로 옮김 
	//if( GetBattleModeTagFlag(pSyncUserRecord) )						// 현재 테그 요청상태중이면 안되요~
	//	SetBattleModeTagFalg(pSyncUserRecord, false);				// 테그 상태 false 변경하고 난입 처리 
	
	SP2Packet kReturn( STPK_BATTLE_MODE_ENTRY_START );
	PACKET_GUARD_VOID( kReturn.Write(ENTRY_START_OK) );
	PACKET_GUARD_VOID( kReturn.Write(pSend->GetPublicID()) );
	PACKET_GUARD_VOID( kReturn.Write(m_dwDurationTime));
	PACKET_GUARD_VOID( kReturn.Write(GetEntryActionPermitCount() - GetEntryActionCount(pSyncUserRecord) - 1));
	pSend->SendMessage( kReturn );

	LOG.PrintTimeAndLog(LOG_DEBUG_LEVEL,"[ENTRY_START_OK>>Clent[%s] EntryAct[%d] RemainCNT[%d]",pSend->GetPublicID().c_str(),GetEntryActivation(pSyncUserRecord),GetEntryActionPermitCount() - GetEntryActionCount(pSyncUserRecord) -1 );

	return;
}

void BattleMode::BattleModeSetEntryStateFlag( User *pSend, SP2Packet &rkPacket)
{
	if( !pSend ) return;

	if( pSend->IsObserver() )
	{
		LOG.PrintTimeAndLog(LOG_DEBUG_LEVEL,"[<<BATTLE_MODE::Observer>> User[%s]",pSend->GetPublicID().c_str());
		return;
	}

	BattleModeRecord *pSyncUserRecord =(BattleModeRecord*)FindBattleModeRecord(pSend);
	if( NULL == pSyncUserRecord )
		return;	

	int iUserHP_Value = 0;
	bool bEntryState = false;
	ioHashString szWarTimeStartName;	
	PACKET_GUARD_VOID( rkPacket.Read(szWarTimeStartName) );
	PACKET_GUARD_VOID( rkPacket.Read(iUserHP_Value) );	
	PACKET_GUARD_VOID( rkPacket.Read(bEntryState) );	

	SetBattleModeUserHp( pSyncUserRecord, iUserHP_Value);		// 유저 HP 정보 

	LOG.PrintTimeAndLog(LOG_DEBUG_LEVEL,"[<<BATTLE_MODE::BattleModeSetEntryStateFlag>> User[%s]m_bEntryState[%d]",pSend->GetPublicID().c_str(),pSyncUserRecord->m_bEntryState);
	pSyncUserRecord->m_bEntryState = bEntryState;
	LOG.PrintTimeAndLog(LOG_DEBUG_LEVEL,"[<<BATTLE_MODE::BattleModeSetEntryStateFlag>> User[%s]m_bEntryState[%d]",pSend->GetPublicID().c_str(),pSyncUserRecord->m_bEntryState);

	return;
}


bool BattleMode::SetBattleModeUserHp( BattleModeRecord *pSyncUserRecord, int iSyncHP)
{
	if(pSyncUserRecord )   // iSyncHP 범위 체크 할것. 
	{
		pSyncUserRecord->m_iHP = iSyncHP;
		return true;
	}
	return false;
}
bool BattleMode::SetBattleModeTagFalg( BattleModeRecord *pSyncUserRecord, bool bFlag)
{
	if(pSyncUserRecord )
	{
		if( pSyncUserRecord->m_bTagWaiting != bFlag )   
		{
			pSyncUserRecord->m_bTagWaiting = bFlag;
			return true;
		}
	}
	return false;
}

BattleModeRecord* BattleMode::GetRecordOnBoardEntryActTeam( TeamType team_type )
{
	int iRecordCnt = m_vRecordList.size();
	for( int i=0; i<iRecordCnt;i++)	
	{
		if( m_vRecordList[i].pUser->GetTeam() == team_type && m_vRecordList[i].m_iEntryActivation == ENTRY_STATE_PROCEEDING )
		{	
			return (BattleModeRecord*)(&m_vRecordList[i]);
		}		
	}
	return NULL;
}

bool BattleMode::SetEntryActivation( BattleModeRecord *pSyncUserRecord, int iFlag)
{

	// #define	ENTRY_STATE_BEFORE		1		초기값 
	// #define	ENTRY_STATE_PROCEEDING	2		난입해서 싸우고 있는중 
	// #define	ENTRY_STATE_COMPLETE	3		난입 한번 했음. 더이상 못함 

	if(pSyncUserRecord )			
	{
		if( GetEntryActivation(pSyncUserRecord) != iFlag )   
		{
			if( iFlag == ENTRY_STATE_PROCEEDING)		// 난입해서 싸우기 시작함. START
				pSyncUserRecord->m_iEntryDurationTime = TIMEGETTIME();

			if( iFlag == ENTRY_STATE_COMPLETE )			// 난입해서 싸우고 들어감. END
			{
				pSyncUserRecord->m_iEntryActionCount++;
				if( pSyncUserRecord->m_iEntryActionCount < m_iEntryActionPermitCount)		// 횟수가 남았으면.. 완료상태 만들지 말고, 처음으로 
				{
					iFlag = ENTRY_STATE_BEFORE;	
				}
				pSyncUserRecord->m_iEntryDurationTime = 0;
			}

			pSyncUserRecord->m_iEntryActivation = iFlag;
			LOG.PrintTimeAndLog(LOG_DEBUG_LEVEL,"[SetEntryActivation>>Clent[%s]] EntryActivation[%d]",pSyncUserRecord->pUser->GetPublicID().c_str(),iFlag);

			return true;
		}
	}
	return false;
}

bool BattleMode::ProcessTCPPacket( User *pSend, SP2Packet &rkPacket )
{
	if( Mode::ProcessTCPPacket( pSend, rkPacket ) )
		return true;

	switch( rkPacket.GetPacketID() )
	{
	case CTPK_BATTLE_MODE_WARTIME_START:			// 참전
		{
			LOG.PrintTimeAndLog(LOG_DEBUG_LEVEL,"[CTPK_BATTLE_MODE_WARTIME_START]>>Clent[%s]",pSend->GetPublicID().c_str());
			BattleModeWarTimeStart( pSend, rkPacket );			
		}
		return true;				
	case CTPK_BATTLE_MODE_TAG_WAIT_FOR_REQUEST:		// 테그 요청 
		{
			LOG.PrintTimeAndLog(LOG_DEBUG_LEVEL,"[CTPK_BATTLE_MODE_TAG_WAIT_FOR_REQUEST]>>Clent[%s]",pSend->GetPublicID().c_str());
			BattleModeTagWaitForRequest( pSend, rkPacket );					
		}
		return true;
	case CTPK_BATTLE_MODE_TAG_ACCEPT:				// 테그 수락  
		{
			LOG.PrintTimeAndLog(LOG_DEBUG_LEVEL,"[CTPK_BATTLE_MODE_TAG_ACCEPT]>>Clent[%s]",pSend->GetPublicID().c_str());
			BattleModeTagAccept( pSend, rkPacket );			
		}
		return true;
	case CTPK_BATTLE_MODE_ENTRY_START:				// 난입 
		{
			LOG.PrintTimeAndLog(LOG_DEBUG_LEVEL,"[CTPK_BATTLE_MODE_ENTRY_START]>>Clent[%s]",pSend->GetPublicID().c_str());
			BattleModeEntryStart( pSend, rkPacket );
		}			
		return true;
	case CTPK_BATTLE_MODE_ENTRY_STATE_OK:				// HP skill info 
		{
			LOG.PrintTimeAndLog(LOG_DEBUG_LEVEL,"[CTPK_BATTLE_MODE_ENTRY_STATE_OK]>>Clent[%s]",pSend->GetPublicID().c_str());
			BattleModeSetEntryStateFlag( pSend, rkPacket );
		}
		return true;
	}
	return false;
}


void BattleMode::OnDropDieUser( User *pDieUser, SP2Packet &rkPacket )
{
	if( pDieUser == NULL ) 
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "BattleMode::OnDropDieUser None User!!" );
		return;
	}

	float fDiePosX = 0.0f, fDiePosZ = 0.0f;
	PACKET_GUARD_VOID( rkPacket.Read(fDiePosX) );
	PACKET_GUARD_VOID( rkPacket.Read(fDiePosZ) );

	int iLastAttackerTeam = 0;
	ioHashString szLastAttackerName;
	ioHashString szLastAttackerSkillName;
	DWORD dwLastAttackerWeaponItemCode = 0;
	PACKET_GUARD_VOID( rkPacket.Read(szLastAttackerName) );
	PACKET_GUARD_VOID( rkPacket.Read(szLastAttackerSkillName) );
	PACKET_GUARD_VOID( rkPacket.Read(dwLastAttackerWeaponItemCode) );
	PACKET_GUARD_VOID( rkPacket.Read(iLastAttackerTeam) );

	ModeRecord *pRecord = FindModeRecord( pDieUser );
	if( !pRecord ) return;

	if( pRecord->eState == RS_LOADING ) return;

	pRecord->SetDieLastAttackerInfo( szLastAttackerName, (TeamType)iLastAttackerTeam, dwLastAttackerWeaponItemCode );

	UpdateDieState( pRecord->pUser );

	int iDamageCnt = 0;
	ioHashString szBestAttackerName;
	PACKET_GUARD_VOID( rkPacket.Read(iDamageCnt) );
	MAX_GUARD(iDamageCnt, 100);

	int iTotalDamage = 0;
	int iLastDamage = 0;
	int iBestDamage = 0;

	if( iDamageCnt > 0 )
	{
		DamageTableList vDamageList;
		vDamageList.reserve( iDamageCnt );

		for( int i=0; i < iDamageCnt; ++i )
		{
			DamageTable kDamageTable;
			PACKET_GUARD_VOID( rkPacket.Read(kDamageTable.szName) );
			PACKET_GUARD_VOID( rkPacket.Read(kDamageTable.iDamage) );

			if( kDamageTable.szName == szLastAttackerName )
				iLastDamage = kDamageTable.iDamage;

			vDamageList.push_back( kDamageTable );

			if( kDamageTable.iDamage > 0 )
			{
				iTotalDamage += kDamageTable.iDamage;

				ModeRecord *pRecord = FindModeRecord( kDamageTable.szName );
				if( pRecord )
				{
					pRecord->iTotalDamage += kDamageTable.iDamage;
				}
			}
		}

		std::sort( vDamageList.begin(), vDamageList.end(), DamageTableSort() );

		szBestAttackerName = vDamageList[0].szName;
		iBestDamage = vDamageList[0].iDamage;
	}

	if( GetState() == Mode::MS_PLAY )
	{
		UpdateDropDieRecord( pRecord->pUser, szLastAttackerName, szBestAttackerName );
	}	

	float fLastRate = 0.0f;
	float fBestRate = 0.0f;

	if( iTotalDamage > 0 )
	{
		fLastRate = ((float)iLastDamage / iTotalDamage) * m_fLastDropDieKillRecoveryRate;
		fBestRate = ((float)iBestDamage / iTotalDamage) * m_fBestDropDieKillRecoveryRate;
	}


	// 2019-02-14 by bckim, 배틀 모드 추가
	SendNextRunnerInfo(pDieUser ,true);// 다음 나올주자 선정 
	// End. 2019-02-14 by bckim, 배틀 모드 추가

	SP2Packet kReturn( STPK_DROP_DIE );
	PACKET_GUARD_VOID( kReturn.Write(pRecord->pUser->GetPublicID()) );
	PACKET_GUARD_VOID( kReturn.Write(szLastAttackerName) );
	PACKET_GUARD_VOID( kReturn.Write(szLastAttackerSkillName) );
	PACKET_GUARD_VOID( kReturn.Write(dwLastAttackerWeaponItemCode) );
	PACKET_GUARD_VOID( kReturn.Write(iLastAttackerTeam) );
	PACKET_GUARD_VOID( kReturn.Write(szBestAttackerName) );
	PACKET_GUARD_VOID( kReturn.Write(fLastRate) );
	PACKET_GUARD_VOID( kReturn.Write(fBestRate) );
	GetCharModeInfo( kReturn, pRecord->pUser->GetPublicID() );
	GetCharModeInfo( kReturn, szLastAttackerName );
	m_pCreator->RoomSendPacketTcp( kReturn );

	UpdateUserDieNextProcess( pRecord->pUser, szLastAttackerName, szBestAttackerName );	
	return;

}


void BattleMode::OnWeaponDieUser( User *pDieUser, SP2Packet &rkPacket )
{
	if( pDieUser == NULL ) 
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Mode::OnWeaponDieUser None User!!" );
		return;
	}

	// 유저가 죽은 위치.
	float fDiePosX = 0.0f, fDiePosZ = 0.0f;
	PACKET_GUARD_VOID( rkPacket.Read(fDiePosX) );
	PACKET_GUARD_VOID( rkPacket.Read(fDiePosZ) );

	int iLastAttackerTeam = 0;
	ioHashString szLastAttackerName;
	ioHashString szLastAttackerSkillName;
	DWORD dwLastAttackerWeaponItemCode = 0;
	PACKET_GUARD_VOID( rkPacket.Read(szLastAttackerName) );
	PACKET_GUARD_VOID( rkPacket.Read(szLastAttackerSkillName) );
	PACKET_GUARD_VOID( rkPacket.Read(dwLastAttackerWeaponItemCode) );
	PACKET_GUARD_VOID( rkPacket.Read(iLastAttackerTeam) );

	ModeRecord *pRecord = FindModeRecord( pDieUser );
	if( !pRecord ) return;

	if( pRecord->eState == RS_LOADING ) return;

	if( pRecord->pUser->IsEquipedItem() ) return;

	pRecord->SetDieLastAttackerInfo( szLastAttackerName, (TeamType)iLastAttackerTeam, dwLastAttackerWeaponItemCode );

	UpdateDieState( pRecord->pUser );

	int iDamageCnt = 0;
	ioHashString szBestAttackerName;
	PACKET_GUARD_VOID( rkPacket.Read(iDamageCnt) );
	MAX_GUARD(iDamageCnt, 300);

	int iTotalDamage = 0;
	int iLastDamage = 0;
	int iBestDamage = 0;

	if( iDamageCnt > 0 )
	{
		DamageTableList vDamageList;
		vDamageList.reserve( iDamageCnt );

		for( int i=0; i < iDamageCnt; ++i )
		{
			DamageTable kDamageTable;
			PACKET_GUARD_VOID( rkPacket.Read(kDamageTable.szName) );
			PACKET_GUARD_VOID( rkPacket.Read(kDamageTable.iDamage) );

			if( kDamageTable.szName == szLastAttackerName )
				iLastDamage = kDamageTable.iDamage;

			vDamageList.push_back( kDamageTable );

			if( kDamageTable.iDamage > 0 )
			{
				iTotalDamage += kDamageTable.iDamage;

				ModeRecord *pRecord = FindModeRecord( kDamageTable.szName );
				if( pRecord )
				{
					pRecord->iTotalDamage += kDamageTable.iDamage;
				}
			}
		}

		std::sort( vDamageList.begin(), vDamageList.end(), DamageTableSort() );

		szBestAttackerName = vDamageList[0].szName;
		iBestDamage = vDamageList[0].iDamage;
	}

	if( GetState() == Mode::MS_PLAY )
	{
		UpdateWeaponDieRecord( pRecord->pUser, szLastAttackerName, szBestAttackerName );
	} 

	float fLastRate = 0.0f;
	float fBestRate = 0.0f;

	if( iTotalDamage > 0 )
	{
		fLastRate = ((float)iLastDamage / iTotalDamage) * m_fLastAttackKillRecoveryRate;
		fBestRate = ((float)iBestDamage / iTotalDamage) * m_fBestAttackKillRecoveryRate;
	}

	SendNextRunnerInfo(pDieUser, true);// 다음 나올주자 선정 

	SP2Packet kReturn( STPK_WEAPON_DIE );
	PACKET_GUARD_VOID( kReturn.Write(pRecord->pUser->GetPublicID()) );
	PACKET_GUARD_VOID( kReturn.Write(szLastAttackerName) );
	PACKET_GUARD_VOID( kReturn.Write(szLastAttackerSkillName) );
	PACKET_GUARD_VOID( kReturn.Write(dwLastAttackerWeaponItemCode) );
	PACKET_GUARD_VOID( kReturn.Write(iLastAttackerTeam) );
	PACKET_GUARD_VOID( kReturn.Write(szBestAttackerName) );
	PACKET_GUARD_VOID( kReturn.Write(fLastRate) );
	PACKET_GUARD_VOID( kReturn.Write(fBestRate) );
	GetCharModeInfo( kReturn, pRecord->pUser->GetPublicID() );
	GetCharModeInfo( kReturn, szLastAttackerName );
	m_pCreator->RoomSendPacketTcp( kReturn );

	UpdateUserDieNextProcess( pRecord->pUser, szLastAttackerName, szBestAttackerName );

	return;
}
Vector3 BattleMode::GetRandomItemPos(ioItem *pItem)
{
	if(pItem)
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"[BattleMode::GetRandomItemPos] Clent[%s]",pItem->GetItemName().c_str());
		
		Vector3 vPos(0.0f,0.0f,0.0f);
		pItem->SetItemPos(vPos);
		return vPos;
	}
	return Mode::GetRandomItemPos( pItem );
}

void BattleMode::IncreaseTeamTagCount(TeamType teamtype)
{
	if(teamtype == TEAM_BLUE )
		m_iBlueUserTagCount++;

	if(teamtype == TEAM_RED )
		m_iRedUserTagCount++;
}
int BattleMode::GetTeamTagCount(TeamType teamtype)
{
	if(teamtype == TEAM_BLUE )
		return m_iBlueUserTagCount;

	if(teamtype == TEAM_RED )
		return m_iRedUserTagCount;

	return 0;
}


void BattleMode::SendNextRunnerInfo(User *pDieUser, bool bDieUser )
{
	if( m_bZeroHP )
	{
		LOG.PrintTimeAndLog(LOG_DEBUG_LEVEL,"[SendNextRunnerInfo>>>>>>>>>>>>>>>>> ZERO HP [%d] !! ",m_bZeroHP);
		return;
	}

	BattleModeRecord* pDieUserRecord = FindBattleModeRecord(pDieUser);
	if( pDieUserRecord )
	{
		if(bDieUser)
			SetUserState(pDieUserRecord,USER_STATE_DEAD );
	}
	else
		return;

	if( ENTRY_STATE_PROCEEDING == GetEntryActivation(pDieUserRecord) )		//죽은 넘이 난입 유저이다
	{
		SetEntryActivation(pDieUserRecord,ENTRY_STATE_COMPLETE);			// 
		//pDieUserRecord->m_bEntryState = true;
		pDieUserRecord->m_iEntryStateCheckCount = 0;
		return;
	}

	int iMinHP = 1000;
	BattleModeRecord* pNextUserRecord;			// hp가 제일 낮은 넘. 
	BattleModeRecord* pUserRecord;
	bool bExistNextUser = false;

#ifdef BATTLE_MODE_BY_BCKIM_DEBUG	// 2019-02-14 by bckim, 배틀 모드 추가
	int iRecordCnt_temp = m_vRecordList.size();
	for( int i=0; i<iRecordCnt_temp;i++)
	{	
		//BattleModeRecord tempUser = m_vRecordList[i];
		BattleModeRecord* pTempUser = &m_vRecordList[i];            

		if ( pTempUser->pUser->GetTeam() == TEAM_BLUE )
			LOG.PrintTimeAndLog(LOG_DEBUG_LEVEL,"[<<<<< HP CHECK!!! >>>>> [ TEAM_BLUE ]UserRecord[%s]>>m_iHP[%d] ",pTempUser->pUser->GetPublicID().c_str(),pTempUser->m_iHP);
		else if ( pTempUser->pUser->GetTeam() == TEAM_RED )
			LOG.PrintTimeAndLog(LOG_DEBUG_LEVEL,"[<<<<< HP CHECK!!! >>>>> [ TEAM_RED  ]UserRecord[%s]>>m_iHP[%d] ",pTempUser->pUser->GetPublicID().c_str(),pTempUser->m_iHP);
		else
			LOG.PrintTimeAndLog(LOG_DEBUG_LEVEL,"[<<<<< HP CHECK!!! >>>>> [ TEAM_NONE ]UserRecord[%s]>>m_iHP[%d] ",pTempUser->pUser->GetPublicID().c_str(),pTempUser->m_iHP);
	}
#endif // BATTLE_MODE_BY_BCKIM_DEBUG	// 2019-02-14 by bckim, 배틀 모드 추가

	int iRecordCnt = m_vRecordList.size();
	for( int i=0; i<iRecordCnt;i++)
	{
		pUserRecord = &m_vRecordList[i];
		if(pUserRecord->pUser->GetTeam() == pDieUser->GetTeam() 
			&& pUserRecord->pUser->GetPublicID() != pDieUser->GetPublicID() 
			&& GetUserState(pUserRecord) == USER_STATE_WAITING )
		{
			LOG.PrintTimeAndLog(LOG_DEBUG_LEVEL,"[SendNextRunnerInfo  CHECK!!! >> UserRecord[%s]>>m_iHP[%d] NextUserRecord iMinHP[%d]",pUserRecord->pUser->GetPublicID().c_str(),pUserRecord->m_iHP,iMinHP);

			if( pUserRecord->m_iHP < iMinHP  )	// 대기 상태인 유저 중 // 죽은넘은 제외됨 
			{
				bExistNextUser = true;
				iMinHP = pUserRecord->m_iHP;
				pNextUserRecord = pUserRecord;
				LOG.PrintTimeAndLog(LOG_DEBUG_LEVEL,"[SendNextRunnerInfo 1111 >>>>> pUserRecord.m_iHP < iMinHP");
				continue;
			}

			if ( bExistNextUser && pUserRecord->m_iHP == iMinHP && GetBattleModeTagFlag(pNextUserRecord) == GetBattleModeTagFlag(pUserRecord) )	// 테그 요청상태가 같음? (on / off) 
			{					// 대기 상태인 유저 중 // 죽은넘은 제외됨
				if( GetBattleModeOrder(pNextUserRecord) >  GetBattleModeOrder(pUserRecord) )		// 주자 선택 순위 확인 
				{
					pNextUserRecord = pUserRecord;
					LOG.PrintTimeAndLog(LOG_DEBUG_LEVEL,"[WARTIME_DIE_WAIT_USER_START_OK>>DIE_Clent[%s]NEXTUSER{%s]",pDieUser->GetPublicID().c_str(),pNextUserRecord->pUser->GetPublicID().c_str());
				}
			}
			else if(  bExistNextUser && pUserRecord->m_iHP == iMinHP )
			{
				if( GetBattleModeTagFlag(pNextUserRecord) == false )
				{
					pNextUserRecord = pUserRecord;
					LOG.PrintTimeAndLog(LOG_DEBUG_LEVEL,"[SendNextRunnerInfo 2222 >>>>> pUserRecord->m_iHP < iMinHP");
				}
			}				
		}
	}

	if(bExistNextUser)
	{
		SP2Packet kbattleReturn( STPK_BATTLE_MODE_WARTIME_DIE_START );	
		PACKET_GUARD_VOID( kbattleReturn.Write(WARTIME_DIE_WAIT_USER_START_OK) );
		PACKET_GUARD_VOID( kbattleReturn.Write(pNextUserRecord->pUser->GetPublicID()) );
		m_pCreator->RoomSendPacketTcp( kbattleReturn );
		LOG.PrintTimeAndLog(LOG_DEBUG_LEVEL,"[WARTIME_DIE_WAIT_USER_START_OK>>DIE_Clent[%s]NEXTUSER{%s]",pDieUser->GetPublicID().c_str(),pNextUserRecord->pUser->GetPublicID());
		return;
	}

	BattleModeRecord* pEntryUserRecord = GetRecordOnBoardEntryActTeam(pDieUserRecord->pUser->GetTeam());
	if( pEntryUserRecord )
	{
		SP2Packet kbattleReturn( STPK_BATTLE_MODE_WARTIME_DIE_START );	
		PACKET_GUARD_VOID( kbattleReturn.Write(WARTIME_DIE_ENTRY_USER_START_OK) );
		PACKET_GUARD_VOID( kbattleReturn.Write(pEntryUserRecord->pUser->GetPublicID()) );
		m_pCreator->RoomSendPacketTcp( kbattleReturn );
		LOG.PrintTimeAndLog(LOG_DEBUG_LEVEL,"[WARTIME_DIE_ENTRY_USER_START_OK>>DIE_Clent[%s]NEXTUSER{%s]",pDieUser->GetPublicID().c_str(),pEntryUserRecord->pUser->GetPublicID());
		SetEntryActivation(pEntryUserRecord,ENTRY_STATE_COMPLETE);
		return;
	}

	LOG.PrintTimeAndLog(LOG_DEBUG_LEVEL,"[STPK_BATTLE_MODE_WARTIME_DIE_START>>NEXT USER NONE!!!!>>DIE_Clent[%s]",pDieUser->GetPublicID().c_str());
	return;
}


void BattleMode::ProcessTime()
{
	Mode::ProcessTime();
	EntryDurationTimeOut();
	CheckEntryDurationTime();

	CheckTickTagAcceptTime();
}

void BattleMode::CheckTickTagAcceptTime()
{
	int iRecordCnt = m_vRecordList.size();
	for( int i=0; i<iRecordCnt;i++)
	{
		BattleModeRecord* pUserRecord = &m_vRecordList[i];
		DWORD dwTickTime = TIMEGETTIME() - pUserRecord->m_dwTagAcceptTime;
		if(  dwTickTime > m_dwTagAcceptHoldTime && pUserRecord->m_dwTagAcceptTime !=0 && GetUserState(pUserRecord) == USER_STATE_WAITING )
		{
			pUserRecord->m_dwTagAcceptTime = 0;
			SP2Packet kReturn( STPK_BATTLE_MODE_TAG_ACCEPT_TIME_OK );					
			PACKET_GUARD_VOID( kReturn.Write(pUserRecord->pUser->GetPublicID()) );					
			m_pCreator->RoomSendPacketTcp( kReturn );

			LOG.PrintTimeAndLog(LOG_DEBUG_LEVEL,"[STPK_BATTLE_MODE_TAG_ACCEPT_TIME_OK>>Clent[%s][%u]>[%u]",pUserRecord->pUser->GetPublicID().c_str(),dwTickTime,m_dwTagAcceptHoldTime);
		}
	}
	return;	
}

void BattleMode::CheckEntryDurationTime()
{
	int iRecordCnt = m_vRecordList.size();
	for( int i=0; i<iRecordCnt;i++)
	{
		BattleModeRecord* pUserRecord = &m_vRecordList[i];
		if(  pUserRecord->m_iEntryActivation == ENTRY_STATE_PROCEEDING )
		{
			DWORD TempDurationTime = pUserRecord->m_iEntryDurationTime;
			if( pUserRecord->pUser && TempDurationTime )
			{	
				if(  TIMEGETTIME() - TempDurationTime > m_dwDurationTime ) 
				{ 	
					if( pUserRecord->m_iEntryStateCheckCount > 10 )
					{
						LOG.PrintTimeAndLog(LOG_DEBUG_LEVEL,"[STPK_BATTLE_MODE_ENTRY_TIMEOUT_CHECK >>> CLOSE Clent[%s] CHECK_COUNT[%d]",pUserRecord->pUser->GetPublicID().c_str(),pUserRecord->m_iEntryStateCheckCount);
						pUserRecord->m_iEntryStateCheckCount = 0;
						pUserRecord->pUser->CloseConnection();
						return;
					}
					else
					{
						SP2Packet kReturn( STPK_BATTLE_MODE_ENTRY_TIMEOUT_CHECK );					
						PACKET_GUARD_VOID( kReturn.Write(pUserRecord->pUser->GetPublicID()) );					
						pUserRecord->pUser->SendMessage( kReturn );

						pUserRecord->m_iEntryStateCheckCount++;

						LOG.PrintTimeAndLog(LOG_DEBUG_LEVEL,"[STPK_BATTLE_MODE_ENTRY_TIMEOUT_CHECK Clent[%s] CHECK_COUNT[%d]",pUserRecord->pUser->GetPublicID().c_str(),pUserRecord->m_iEntryStateCheckCount);
					}
				}
			}
		}
	}
	return;			           
}

void BattleMode::EntryDurationTimeOut()
{
	int iRecordCnt = m_vRecordList.size();
	for( int i=0; i<iRecordCnt;i++)
	{
		BattleModeRecord* pUserRecord = &m_vRecordList[i];
		if(  pUserRecord->m_iEntryActivation == ENTRY_STATE_PROCEEDING && pUserRecord->m_bEntryState)
		{
			DWORD TempDurationTime = pUserRecord->m_iEntryDurationTime;
			if( pUserRecord->pUser && TempDurationTime )
			{
				if(  TIMEGETTIME() - TempDurationTime > m_dwDurationTime ) 
				{ 
					pUserRecord->m_bEntryState = false;
					pUserRecord->m_iEntryStateCheckCount = 0;

					if( m_bZeroHP == false )		// 데스타임이 아닐때만 아래 패킷을 보낸다 
					{
						SP2Packet kReturn( STPK_BATTLE_MODE_ENTRY_TIMEOUT );					
						PACKET_GUARD_VOID( kReturn.Write(pUserRecord->pUser->GetPublicID()) );					
						m_pCreator->RoomSendPacketTcp( kReturn );
					}

					LOG.PrintTimeAndLog(LOG_DEBUG_LEVEL,"[STPK_BATTLE_MODE_ENTRY_END_TIMEOUT>>Clent[%s][%u]>[%u]",pUserRecord->pUser->GetPublicID().c_str(),TIMEGETTIME()-TempDurationTime,m_dwDurationTime);
					SetEntryActivation( pUserRecord,ENTRY_STATE_COMPLETE);
					SetUserState(pUserRecord, USER_STATE_WAITING );	
				}
			}
		}
	}
	return;			           
}

bool BattleMode::CheckTagAcceptTime(BattleModeRecord *pUserRecord)
{
	if( pUserRecord )
	{
		DWORD RemainsTick = TIMEGETTIME() - (pUserRecord->m_dwTagAcceptTime);
		if(  RemainsTick <  m_dwTagAcceptHoldTime)		// 해당시간 안지남.
		{
			LOG.PrintTimeAndLog(LOG_DEBUG_LEVEL,"[CheckTagAcceptTime[FALSE]>>RemainsTick[%u]-[%u]=[%u]:[%u]",TIMEGETTIME(),pUserRecord->m_dwTagAcceptTime,RemainsTick,m_dwTagAcceptHoldTime);
			return false;
		}
		LOG.PrintTimeAndLog(LOG_DEBUG_LEVEL,"[CheckTagAcceptTime[TRUE_]>>RemainsTick[%u]-[%u]=[%u]:[%u]",TIMEGETTIME(),pUserRecord->m_dwTagAcceptTime,RemainsTick,m_dwTagAcceptHoldTime);
		return true;
	}
	return false;
}

DWORD BattleMode::GetTagAcceptTime(BattleModeRecord *pUserRecord)
{
	if( pUserRecord )
		return TIMEGETTIME() - (pUserRecord->m_dwTagAcceptTime);
	return 0;
}

void BattleMode::InitTagAcceptTime(BattleModeRecord *pUserRecord)
{
	if( pUserRecord )
		pUserRecord->m_dwTagAcceptTime = TIMEGETTIME();
}

bool BattleMode::SetUserState(BattleModeRecord *pSyncUserRecord, int state)
{
	if( pSyncUserRecord )
	{
		if( pSyncUserRecord->m_iUserState != state )
		{
			pSyncUserRecord->m_iUserState = state;
			LOG.PrintTimeAndLog(LOG_DEBUG_LEVEL,"[SetUserState>>Clent[%s]>> USER_STATE[%d]",pSyncUserRecord->pUser->GetPublicID().c_str(),state);

			if( state == USER_STATE_DEAD)
				SetBattleModeUserHp(pSyncUserRecord, 0 );			// 죽은 상태면 HP 0으로 
			return true;
		}
	}	
	return false;
}

int BattleMode::GetUserState(BattleModeRecord *pSyncUserRecord)
{
	if( pSyncUserRecord )
		return pSyncUserRecord->m_iUserState;
	return 0;
}

void BattleMode::UpdateDieState( User *pDier )
{
	BattleModeRecord *pDieRecord = FindBattleModeRecord( pDier );
	if( !pDieRecord ) return;
	if( pDieRecord->bDieState ) return;

	pDieRecord->bDieState = true;
	pDieRecord->dwCurDieTime = 0;
	pDieRecord->bExperienceState = false;

	if( GetUserState(pDieRecord) != USER_STATE_DEAD )
	{
		if( pDier->GetTeam() == TEAM_RED )
			m_iRedDiePlayerCnt++;
		else if( pDier->GetTeam() == TEAM_BLUE )
			m_iBlueDiePlayerCnt++;
	}
}
