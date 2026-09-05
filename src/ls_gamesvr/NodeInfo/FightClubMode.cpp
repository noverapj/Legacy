#include "stdafx.h"
//#include "../Window.h"
#include "../MainProcess.h"

#include "User.h"
#include "Room.h"
#include "FightClubModeHelp.h"
#include "FightClubMode.h"
#include "ioSetItemInfoManager.h"
#include "ioItemInfoManager.h"
#include "ioSetItemInfo.h"
#include "RoomNodeManager.h"
#include "ioEventUserNode.h"
#include "ioEtcItemManager.h"
#include "TournamentManager.h"
#include "ioExerciseCharIndexManager.h"
#include "BattleRoomNode.h"
#include "../MainServerNode/MainServerNode.h"
#include "../EtcHelpFunc.h"
#include "../DataBase/LogDBClient.h"
#include "MissionManager.h"

#include <strsafe.h>
FightClubMode::FightClubMode( Room *pCreator ) : Mode( pCreator ), m_iFightSeqArray( 0 )
{	
	m_bFighterInfoPacket = false;
	m_SingleTeamList.clear();
	m_SingleTeamPosArray.clear();

	m_fChampionPointRate	= 0.0f;
	m_iChampionVictory		= 0;
	m_fChallengerPointRate	= 0.0f;
	m_iChallengerVictory	= 0;
	m_fScoreGapConst		= 0.0f;
	m_fScoreGapRateConst	= 0.0f;

	m_fFightVictoryBonus	= 0.0f;
	m_iMaxFightVictoryBonusCount = 0;

	m_dwAbuseTime			= 0;
	m_dwCheerAbuseTime		= 0;
	m_fightCheerPesoConst	= 0;
	m_fightCheerMaxConst	= 0;
	m_fightCheerAwaiterConst = 0;
	m_fightCheerChampionConst = 0;

	m_bTimeAbuse  = false;
	m_bCheerAbuse = false;

	m_bFirstNPC = true;
	m_bUseFightNPC = false;
	m_bReserveNewChallenger = false;
	m_ReserveNewChallengerName.Clear();

	m_iCurFightNPCStage = 0;
}

FightClubMode::~FightClubMode()
{
}

void FightClubMode::LoadINIValue()
{
	Mode::LoadINIValue();

	ioINILoader &rkLoader = g_ModeINIMgr.GetINI( GetModeINIFileName() );
	LoadFightNPCInfo( rkLoader );

	m_dwCurRoundDuration = m_dwRoundDuration;
}

void FightClubMode::LoadRoundCtrlValue( ioINILoader &rkLoader )
{
	rkLoader.SetTitle( "info" );
	m_iMaxPlayer = min( MAX_PLAYER, rkLoader.LoadInt( "max_player", MAX_PLAYER ) );
	m_iConstMaxPlayer = min( MAX_PLAYER, rkLoader.LoadInt( "const_max_player", MAX_PLAYER ) );

	rkLoader.SetTitle( "round" );
	m_iMaxRound = 1;

	m_dwRoundDuration   = rkLoader.LoadInt( "round_time", 300000 );

	m_dwReadyStateTime  = rkLoader.LoadInt( "ready_state_time", 4000 );
	m_dwSuddenDeathTime = rkLoader.LoadInt( "sudden_death_time", 60000 );
	m_dwResultStateTime = rkLoader.LoadInt( "result_state_time", 7000 );
	m_dwFinalResultWaitStateTime = rkLoader.LoadInt( "final_result_wait_state_time", 10000 );
	m_dwFinalResultStateTime = rkLoader.LoadInt( "final_result_state_time", 13000 );
	m_dwTournamentRoomFinalResultAddTime = rkLoader.LoadInt( "tournament_room_final_result_time", 10000 );
	m_dwTimeClose       = rkLoader.LoadInt("room_close_time", 10000 );

	m_bUseViewMode = rkLoader.LoadBool( "use_view_mode", false );
	m_dwViewCheckTime = rkLoader.LoadInt( "view_check_time", 30000 );

	m_iAbuseMinDamage = rkLoader.LoadInt( "abuse_min_damage", 1 );

	m_iScoreRate = rkLoader.LoadInt( "win_team_score_rate", 1 );

	m_dwRoomExitTime = rkLoader.LoadInt( "exit_room_time", 5000 );

	m_dwCharLimitCheckTime = rkLoader.LoadInt( "char_limit_check_min", 60000 );
	
	// 팀데스매치 기반 보상
	m_fScoreGapConst = rkLoader.LoadFloat( "score_gap_const", 50.0f );
	m_fScoreGapRateConst = rkLoader.LoadFloat( "score_gap_rate_const", 1.5f );

	m_fFightVictoryBonus	= rkLoader.LoadFloat( "fight_victory_bonus_rate", 0.20f );
	m_iMaxFightVictoryBonusCount = rkLoader.LoadInt( "max_fight_victory_bonus_count", 20 );

	m_dwAbuseTime = rkLoader.LoadInt( "fight_abuse_time", 30000 );
	m_dwCheerAbuseTime = rkLoader.LoadInt( "fight_abuse_cheer_time", 10000 );

	m_fightCheerPesoConst = rkLoader.LoadInt( "fight_cheer_peso_const", 42 );
	m_fightCheerMaxConst = rkLoader.LoadInt( "fight_cheer_max_const", 30 );
	m_fightCheerAwaiterConst = rkLoader.LoadInt( "fight_cheer_awaiter_const", 24 );
	m_fightCheerChampionConst = rkLoader.LoadInt( "fight_cheer_champion_const", 2 );
	
	m_SingleTeamList.clear();
	for( int i=TEAM_PRIVATE_1;i < TEAM_PRIVATE_16+1;i++)
	{
		m_SingleTeamList.push_back( i );
	}

	SetStartPosArray();
}

void FightClubMode::LoadFightNPCInfo( ioINILoader &rkLoader )
{
	m_iCurFightNPCStage = 0;
	m_FightNPCList.clear();

	rkLoader.SetTitle( "fight_npc" );

	int iStageCnt = rkLoader.LoadInt( "fight_npc_stage_cnt", 0 );
	int iMaxNPCCnt = rkLoader.LoadInt( "fight_npc_cnt", 0 );

	if( iStageCnt > 0 )
	{
		IntVec vIndex;
		for( int i=0; i < iMaxNPCCnt; ++i )
		{
			vIndex.push_back( i );
		}

		io_random_shuffle( vIndex.begin(), vIndex.end() );


		char szTitle[MAX_PATH];
		m_FightNPCList.reserve( iStageCnt );

		for( int i=0; i < iStageCnt; ++i )
		{
			int iIndex = vIndex[i];

			wsprintf( szTitle, "fight_npc%d_stage%d_code", iIndex+1, i+1 );

			int iNPCCode = rkLoader.LoadInt( szTitle, 0 );
			m_FightNPCList.push_back( iNPCCode );
		}
	}

	m_dwNewChallengerDuration = (DWORD)rkLoader.LoadInt( "new_challenger_duration", 0 );
	m_dwNPCContinueDuration = (DWORD)rkLoader.LoadInt( "npc_continue_duration", 0 );
}

void FightClubMode::SetStartPosArray()
{
	int iSinglePosCnt = m_SingleTeamList.size();
	if(iSinglePosCnt <= 1) // 최소한 2개 필요
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Error 2 - FightClubMode::GetStartPosArray");
		return;
	}

	m_SingleTeamPosArray.reserve(iSinglePosCnt);
	for( int i=0; i<iSinglePosCnt; i++ )
	{
		m_SingleTeamPosArray.push_back(i);
	}

	io_random_shuffle( m_SingleTeamPosArray.begin(), m_SingleTeamPosArray.end() );

	m_iBluePosArray = m_SingleTeamPosArray[0];
	m_iRedPosArray  = m_SingleTeamPosArray[1];
}

void FightClubMode::DestroyMode()
{
	Mode::DestroyMode();
	m_vRecordList.clear();
}

void FightClubMode::InitObjectGroupList()
{
	ioINILoader &rkLoader = g_ModeINIMgr.GetINI( GetModeINIFileName() );

	int iSubNum = GetModeSubNum();
	int iGroupNum = GetModeMapNum();
	char szBuf[MAX_PATH], szTitle[MAX_PATH];

	m_vPushStructList.clear();
	m_iPushStructIdx = 0;

	wsprintf( szTitle, "fight%d_object_group%d", iSubNum, iGroupNum );
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

int  FightClubMode::AddRecordFightClubState( bool bObsever )
{
	if( bObsever )
		return FIGHTCLUB_OBSERVER;

	if( m_bRoundSetEnd )           // 라운드 종료시 들어왔다.
		return FIGHTCLUB_WAITING;

	FightClubRecord *pChampion = FindFightClubChampion();
	if( pChampion == NULL )
		return FIGHTCLUB_CHAMPION;

	if( m_bUseFightNPC )
		return FIGHTCLUB_WAITING;

	FightClubRecord *pChallenger = FindFightClubChallenger();
	if( pChallenger == NULL )
	{
		return FIGHTCLUB_CHALLENGER;
	}

	return FIGHTCLUB_WAITING;
}

void FightClubMode::AddNewRecord( User *pUser )
{
	if( !pUser )
		return;

	FightClubRecord kRecord;
	kRecord.pUser		= pUser;
	kRecord.bFightWin	= false;
	kRecord.iFightSeq	= GetFightSeqArray();
	kRecord.iFightState = AddRecordFightClubState( ( pUser->IsObserver() || pUser->IsStealth() ) );

	m_vRecordList.push_back( kRecord );

	RemoveTeamType( pUser->GetTeam() );
	UpdateUserRank();

	int iPlayUserCnt = m_pCreator->GetPlayUserCnt();
	if( m_bUseFightNPC && iPlayUserCnt > 1 && (!pUser->IsObserver() && !pUser->IsStealth()) )
	{
		SetUseFightNPC( false );
		SendFightNPCInfo( pUser );
		m_bFirstNPC = true;

		if( m_ModeState == MS_READY || m_ModeState == MS_PLAY )
		{
			SetNewChallengerState( pUser->GetPublicID() );

			FightClubRecord *pChampRecord = FindFightClubChampion();
			if( pChampRecord )
			{
				pChampRecord->bFightWin = true;
			}
		}
		else if( m_ModeState == MS_RESULT_WAIT || m_ModeState == MS_RESULT )
		{
			m_bReserveNewChallenger = true;
			m_ReserveNewChallengerName = pUser->GetPublicID();
		}
	}
	else
	{
		SendFightNPCInfo( pUser );
	}
}

void FightClubMode::RemoveRecord( User *pUser, bool bRoomDestroy )
{
	if(!pUser) return;
	
	int iCharCnt = m_vRecordList.size();
	for( int i=0 ; i<iCharCnt ; i++ )
	{
		if( m_vRecordList[i].pUser == pUser )
		{
			SetModeEndDBLog( &m_vRecordList[i], iCharCnt, LogDBClient::PRT_EXIT_ROOM );
			AddTeamType( pUser->GetTeam() );
			m_vRecordList.erase( m_vRecordList.begin() + i );
			break;
		}
	}

	UpdateUserRank();
	
	if( pUser )
	{
		m_KickOutVote.RemoveVoteUserList( pUser->GetPublicID() );
	}
}

void FightClubMode::SetModeEndDBLog( ModeRecord *pRecord, int iMaxRecord, int iLogType )
{
	if( !pRecord ) 
		return;
	if( !pRecord->pUser ) 
		return;

	pRecord->AddPlayingTime(); // 중간 종료시 시간 기록 위해
	pRecord->AddClassPlayingTime();
	if( GetRecordPlayTime( pRecord ) > 0 )
	{
		if( pRecord->pUser->GetStartTimeLog() > 0 )
			pRecord->AddDeathTime( 0 );
		g_LogDBClient.OnInsertPlayResult( pRecord, (LogDBClient::PlayResultType) iLogType );
	}
	else
	{
		if( pRecord->pUser->GetStartTimeLog() > 0 )
			g_LogDBClient.OnInsertTime( pRecord->pUser, LogDBClient::TT_FIGHTMODE_VIEW );
	}
	pRecord->bWriteLog = true;
	pRecord->pUser->SetStartTimeLog(0);

	if( iLogType != LogDBClient::PRT_END_SET )
		return;

	if( pRecord->pUser->IsBattleRoom() || pRecord->pUser->IsLadderTeam() )
		pRecord->pUser->SetStartTimeLog(TIMEGETTIME());
}

void FightClubMode::AddTeamType( TeamType eTeam )
{
	if( COMPARE( eTeam, TEAM_PRIVATE_1, TEAM_PRIVATE_16+1 ) )
		m_SingleTeamList.push_back( eTeam ); 
	else
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "FightClubMode::AddTeamType %d", eTeam );
}

void FightClubMode::RemoveTeamType( TeamType eTeam )
{
	int iTeamSize = m_SingleTeamList.size();
	for(int i = 0;i < iTeamSize;i++)
	{
		if( m_SingleTeamList[i] == eTeam )
		{
			m_SingleTeamList.erase( m_SingleTeamList.begin() + i );
			break;
		}
	}
}

void FightClubMode::SetNewChallengerState( const ioHashString &szName )
{
	m_dwNewChallengerTime = TIMEGETTIME();

	SetModeState( MS_NEW_CHALLENGER );

	SP2Packet kPacket( STPK_FIGHT_NPC );
	kPacket << FIGHT_NPC_NEWCHALLENGER;
	kPacket << szName;
	SendRoomAllUser(kPacket);

	m_bReserveNewChallenger = false;
	m_ReserveNewChallengerName.Clear();
}

void FightClubMode::SetNPCContinueState()
{
	SetModeState( MS_NPC_CONTINUE );

	m_dwNPCContinueTime = TIMEGETTIME();

	SP2Packet kPacket( STPK_FIGHT_NPC );
	kPacket << FIGHT_NPC_CONTINUE;
	SendRoomAllUser(kPacket);
}

void FightClubMode::ProcessTime()
{
	switch( m_ModeState )
	{
	case MS_READY:
		ProcessReady();
		break;
	case MS_PLAY:
		ProcessPlayCharHireTimeStop();
		ProcessPlay();
		ExecuteReservedExitViewUser();
		ExecuteReservedExitPlayUser();
		break;
	case MS_RESULT_WAIT:
		ProcessResultWait();
		break;
	case MS_RESULT:
		ProcessResult();
		break;
	case MS_NEW_CHALLENGER:
		ProcessNewChallenger();
		break;
	case MS_NPC_CONTINUE:
		ProcessNPCFightContinue();
		break;
	}

	// 강퇴 시간 체크
	m_KickOutVote.ProcessVote();
}

void FightClubMode::ProcessPlay()
{
	ProcessRevival();
	CheckFieldItemLiveTime();
	CheckItemSupply( m_dwStateChangeTime );
	CheckBallSupply( m_dwStateChangeTime );
	CheckMachineSupply( m_dwStateChangeTime );
	CheckNeedSendPushStruct();
	CheckRoundEnd( true );
	ProcessEvent();
	ProcessBonusAlarm();
}

void FightClubMode::ProcessNewChallenger()
{
	DWORD dwCurTime = TIMEGETTIME();

	if( m_dwNewChallengerTime + m_dwNewChallengerDuration < dwCurTime )
	{
		RestartMode();
	}
}

void FightClubMode::ProcessNPCFightContinue()
{
	if( m_bRequestDestroy )
		return;

	DWORD dwCurTime = TIMEGETTIME();

	if( m_dwNPCContinueTime + m_dwNPCContinueDuration < dwCurTime )
	{
		m_ModeState = MS_RESULT;
		m_bRequestDestroy = true;

		// log
		int iRecordCnt = GetRecordCnt();
		for(int i = 0;i < iRecordCnt;i++ )
		{
			ModeRecord *pRecord = FindModeRecord( i );
			SetModeEndDBLog( pRecord, iRecordCnt, LogDBClient::PRT_END_SET );
		}
	}
}

void FightClubMode::ProcessResultWait()
{
	// HP 전송 받을 때까지 대기
	if( m_dwStateChangeTime + m_dwFinalResultWaitStateTime < TIMEGETTIME() )
	{
		if( m_bFighterInfoPacket )
		{
			//SetFightEndResult();
			SetFightEndResult_Renew();
			SetModeState( MS_RESULT );
		}
	}
	else
	{
		// 대기 시간동안 안오면 무승부 처리
		//SetFightEndResult();
		SetFightEndResult_Renew();
		SetModeState( MS_RESULT );
	}
}

void FightClubMode::ProcessResult()
{
	DWORD dwResultDuration = m_dwResultStateTime;
	if( m_bRoundSetEnd )
	{
		dwResultDuration = m_dwFinalResultStateTime;
	}

	if( m_dwStateChangeTime + dwResultDuration > TIMEGETTIME() )
		return;

	// npc와 싸워서 졌을경우 continue 상태로 넘기기 위한 값
	bool bContinueState = false;

	int iPlayUserCnt = 0;
	int iModeMinUserCnt = 0;
	if( m_pCreator )
	{
		iPlayUserCnt = m_pCreator->GetPlayUserCnt();
		iModeMinUserCnt = m_pCreator->GetModeMinUserCnt();
	}

	FightClubRecord *pChamp = FindFightClubChampion();

	if( m_bUseFightNPC )
	{
		if( m_bRoundSetEnd )
		{
			if( ExecuteReservedExit() )
				return;	// true가 리턴된 경우는 모든 유저가 나가버린 상태
		}

		if( iPlayUserCnt >= 1 )
		{
			if( pChamp )
			{
				if( !pChamp->bFightWin )
				{
					if( pChamp->pUser->GetPublicID() == m_RoundEndChampName )
					{
						bContinueState = true;
						m_bRoundSetEnd = false;
					}
					else
					{
						m_bRoundSetEnd = false;
						m_iCurFightNPCStage = 0;
					}
				}
			}
			else
			{
				m_bRoundSetEnd = true;
				m_iCurFightNPCStage = 0;
			}
		}
		else
		{
			m_bRoundSetEnd = true;
			m_iCurFightNPCStage = 0;
		}
	}
	else if( m_bTournamentRoom )
	{
		m_bRoundSetEnd = true;       // 대회 경기는 1회용
	}
	else
	{
		if( m_bRoundSetEnd )
		{
			if( ExecuteReservedExit() )
				return;	// true가 리턴된 경우는 모든 유저가 나가버린 상태
		}
		else
		{
			if( iPlayUserCnt == 0 )
			{
				m_bRoundSetEnd = true;
			}
			else if( iPlayUserCnt == 1 )
			{
				if( iModeMinUserCnt == 1 )
					m_bRoundSetEnd = false;
				else
					m_bRoundSetEnd = true;
			}
			else
			{
				m_bRoundSetEnd = false;
			}
		}

		if( !pChamp || !pChamp->bFightWin )
		{
			m_iCurFightNPCStage = 0;
		}
	}

	if( bContinueState )
	{
		SetNPCContinueState();
	}
	else if( !m_bRoundSetEnd )
	{
		if( m_bReserveNewChallenger )
			SetNewChallengerState( m_ReserveNewChallengerName );
		else
			RestartMode();        // 다음 라운드 시작
	}
	else
	{
		m_bRequestDestroy = true;

		// log
		int iRecordCnt = GetRecordCnt();
		for(int i = 0;i < iRecordCnt;i++ )
		{
			ModeRecord *pRecord = FindModeRecord( i );
			SetModeEndDBLog( pRecord, iRecordCnt, LogDBClient::PRT_END_SET );
		}
		//
		Mode::SetModeEndDBLog( m_pCreator, LogDBClient::PRT_END_SET );
	}
}

void FightClubMode::ReSetChampion()
{
	FightClubRecord *pChampRecord = FindFightClubChampion();
	FightClubRecord *pChallengerRecord = FindFightClubChallenger();

	if( m_bUseFightNPC )
	{
		// 챔피언 승패 처리
		// 도전자 승패 처리
		if( !pChampRecord && pChallengerRecord )
		{
			m_iCurFightNPCStage = 0;
			pChallengerRecord->iFightState = FIGHTCLUB_CHAMPION;
		}
		else if( !pChampRecord && !pChallengerRecord )
		{
			FightClubRecord *pRecord = static_cast< FightClubRecord * >( FindModeRecord( 0 ) );
			if( pRecord )
			{
				m_iCurFightNPCStage = 0;
				pRecord->iFightState = FIGHTCLUB_CHAMPION;
			}
		}
	}
	else
	{
		// 챔피언 승패 처리
		if( pChampRecord )
		{
			if( pChampRecord->bFightWin == false )
			{
				pChampRecord->iFightSeq   = GetFightSeqArray();
				pChampRecord->iFightState = FIGHTCLUB_WAITING;
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[info][figthclub]Champ lose : [%lu]", pChampRecord->pUser->GetUserIndex() );
			}
		}

		// 도전자 승패 처리
		if( pChallengerRecord )
		{
			if( pChallengerRecord->bFightWin == false )
			{
				pChallengerRecord->iFightState = FIGHTCLUB_WAITING;
				pChallengerRecord->iFightSeq   = GetFightSeqArray();
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Challenger Lose : %s", pChallengerRecord->pUser->GetPublicID().c_str() );
			}
			else
			{
				pChallengerRecord->iFightState     = FIGHTCLUB_CHAMPION;
			}
		}
	}
}

void FightClubMode::RestartMode()
{
	m_bZeroHP = false;
	m_bTimeAbuse = false;
	m_bCheerAbuse = false;
	m_bFighterInfoPacket = false;

	m_RoundEndChampName.Clear();

	m_bReserveNewChallenger = false;
	m_ReserveNewChallengerName.Clear();

	m_dwCurRoundDuration = m_dwRoundDuration;

	m_iCurItemSupplyIdx = 0;
	m_iCurBallSupplyIdx = 0;

	int i = 0;
	int iRecordCnt = m_vRecordList.size();

	int iPlayUserCnt = 0;
	int iModeMinUserCnt = 0;
	if( m_pCreator )
	{
		iPlayUserCnt = m_pCreator->GetPlayUserCnt();
	}

	if( !m_bUseFightNPC && iPlayUserCnt == 1 )
	{
		SetUseFightNPC( true );
	}

	ReSetChampion();

	// 다음 도전자
	if( !m_bUseFightNPC )
	{
		for(i = 0;i < iRecordCnt;i++)
		{
			FightClubRecord &rkRecord = m_vRecordList[i];

			if( rkRecord.pUser == NULL ) continue;
			//if( rkRecord.eState == RS_LOADING ) continue;

			rkRecord.bFightWin = false;
			if( IsNextFighter( rkRecord.pUser->GetPublicID() ) )
			{
				if( FindFightClubChampion() )      // 챔피언이 있으면 도전자
				{
					rkRecord.iFightState = FIGHTCLUB_CHALLENGER;
					LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[info][fightclub]New challenger : %d", rkRecord.pUser->GetUserIndex() );
				}
				else                               // 챔피언이 없으면 챔피언
				{
					rkRecord.iFightState = FIGHTCLUB_CHAMPION;
					LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[info][fightclub]New Champion : %d", rkRecord.pUser->GetUserIndex() );
				}
			}
		}
	}

	m_CurRoundWinTeam = WTT_NONE;
	m_pCreator->DestroyAllFieldItems();

	SetModeState( MS_READY );

	for(i = 0;i < iRecordCnt;i++)
	{
		FightClubRecord &rkRecord = m_vRecordList[i];
		if( rkRecord.pUser == NULL ) continue;

		rkRecord.pUser->EquipDBItemToLiveChar();
		rkRecord.dwCurDieTime = 0;
		rkRecord.iRevivalCnt  = 0;
		rkRecord.dwRevivalGap = (DWORD)GetRevivalGapTime( 0 );
		rkRecord.bCatchState  = false;
		rkRecord.iTotalDamage = 0;

		rkRecord.dwPlayingStartTime= 0;
		rkRecord.bDieState	= false;
		rkRecord.bFightWin	= false;
		rkRecord.iFightCheer= 0;
		if( rkRecord.eState == RS_OBSERVER ) continue;

		if( IsFightDelay( rkRecord.pUser->GetPublicID() ) )
		{
			rkRecord.eState = RS_VIEW;
			rkRecord.pUser->SetStartTimeLog( TIMEGETTIME() );
		}
		else
		{
			rkRecord.eState = RS_PLAY;
			rkRecord.StartPlaying();        //( 관전X, 데스타임X )
			rkRecord.pUser->StartCharLimitDate( Mode::GetCharLimitCheckTime(), __FILE__, __LINE__ );
			rkRecord.pUser->StartEtcItemTime( __FUNCTION__ );
		}
	}

	SP2Packet kPacket( STPK_ROUND_READY );
	kPacket << m_bUseFightNPC;		// fight club만 사용
	kPacket << m_iCurRound;
	kPacket << m_iBluePosArray;
	kPacket << m_iRedPosArray;
	kPacket << m_dwCurRoundDuration;

	// 다음 라운드 유저 순서 정보
	kPacket << iRecordCnt;
	for(i = 0;i < iRecordCnt;i++)
	{
		FightClubRecord &rkRecord = m_vRecordList[i];
		if( rkRecord.pUser == NULL )
		{
			kPacket << "" << rkRecord.iFightState << rkRecord.iFightSeq << rkRecord.iFightVictories << rkRecord.iFightCheer;
		}
		else
		{
			kPacket << rkRecord.pUser->GetPublicID() << rkRecord.iFightState << rkRecord.iFightSeq << rkRecord.iFightVictories << rkRecord.iFightCheer;
		}
	}
	SendRoomPlayUser( kPacket );

	InitObjectGroupList();
}

void FightClubMode::NextBattleRoomComplete()
{
	int i = 0;
	int iRecordCnt = m_vRecordList.size();
	SP2Packet kPacket( STPK_FIGHTCLUB_SEQUENCE );
	kPacket << iRecordCnt;
	for(i = 0;i < iRecordCnt;i++)
	{
		FightClubRecord &rkRecord = m_vRecordList[i];
		if( rkRecord.pUser == NULL )
		{
			kPacket << "" << rkRecord.iFightState << rkRecord.iFightSeq << rkRecord.iFightVictories << rkRecord.iFightCheer;
		}
		else
		{
			kPacket << rkRecord.pUser->GetPublicID() << rkRecord.iFightState << rkRecord.iFightSeq << rkRecord.iFightVictories << rkRecord.iFightCheer;
		}
	}
	SendRoomAllUser( kPacket );
}

bool FightClubMode::IsPlayCheckFighter()
{
	FightClubRecord *pChampRecord = FindFightClubChampion();
	if( pChampRecord == NULL )
	{
		return false;
	}

	if( pChampRecord->bDieState )
	{
		return false;
	}

	if( m_bUseFightNPC )
	{
		if( m_CurFightNPCRecord.eState == RS_DIE )
		{
			return false;
		}
	}
	else
	{
		FightClubRecord *pChallengerRecord = FindFightClubChallenger();
		if( pChallengerRecord )
		{
			if( pChallengerRecord->bDieState )
				return false;
		}
		else
		{
			return false;
		}
	}

	return true;
}

void FightClubMode::CheckRoundEnd( bool bProcessCall )
{
	if( !bProcessCall )
	{
		if( !IsPlayCheckFighter() )
		{
			WinTeamType eWinTeam = WTT_DRAW;
			SetRoundEndInfo( eWinTeam );
			SendRoundResult( eWinTeam );
		}
		return;
	}
	
	DWORD dwGapTime = TIMEGETTIME() - m_dwStateChangeTime;
	if( m_dwCurRoundDuration < dwGapTime+1000 )
	{
		// 시간 만료일경우 HP 비교로 판정
		m_dwCurRoundDuration = 0;

		WinTeamType eWinTeam = WTT_DRAW;
		SetRoundEndInfo( eWinTeam );
		SendRoundResult( eWinTeam );
	}
}

void FightClubMode::SetRoundEndInfo( WinTeamType eWinTeam )
{
	// 시간 어뷰즈 체크
	DWORD dwElapse = TIMEGETTIME() - m_dwStateChangeTime;
	if( dwElapse < m_dwAbuseTime )
	{
		m_bTimeAbuse = true;
	}
	if( dwElapse < (m_dwAbuseTime + m_dwCheerAbuseTime) )
	{
		m_bCheerAbuse = true;
	}

	// 다음 상대가 없으면 종료 : 최소 2명 필요
	int iRecordCnt = m_vRecordList.size();

	int iPlayUserCnt = 0;
	int iModeMinUserCnt = 0;
	if( m_pCreator )
	{
		iPlayUserCnt = m_pCreator->GetPlayUserCnt();
		iModeMinUserCnt = m_pCreator->GetModeMinUserCnt();
	}

	FightClubRecord *pChampRecord = FindFightClubChampion();
	if( pChampRecord && pChampRecord->pUser )
	{
		m_RoundEndChampName = pChampRecord->pUser->GetPublicID();
	}

	if( m_bUseFightNPC )
	{
		if( iPlayUserCnt >= 1 )
		{
			m_iCurFightNPCStage++;

			if( pChampRecord && pChampRecord->bDieState )
				m_bRoundSetEnd = false;
			else if( !COMPARE( m_iCurFightNPCStage, 0, (int)m_FightNPCList.size() ) )
				m_bRoundSetEnd = true;
			else
				m_bRoundSetEnd = false;
		}
		else
		{
			m_bRoundSetEnd = false;
		}
	}
	else
	{
		if( iPlayUserCnt == 0 )
		{
			m_bRoundSetEnd = true;
		}
		else if( iPlayUserCnt == 1 )
		{
			if( iModeMinUserCnt == 1 )
				m_bRoundSetEnd = false;
			else
				m_bRoundSetEnd = true;
		}
		else
		{
			m_bRoundSetEnd = false;
		}
	}

	m_CurRoundWinTeam   = eWinTeam;
	m_bCheckContribute  = false;
	m_bCheckAwardChoose = false;
	m_bCheckSuddenDeathContribute = false;
	SetModeState( MS_RESULT_WAIT );

	UpdateRoundRecord();

	m_vPushStructList.clear();
	m_vBallStructList.clear();
	m_vMachineStructList.clear();
	m_pCreator->DestroyAllFieldItems();


	for( int i=0; i < iRecordCnt; i++ )
	{
		ModeRecord *pRecord = FindModeRecord( i );
		if( !pRecord ) continue;

		pRecord->AddPlayingTime();
		pRecord->AddClassPlayingTime();
		if( GetRecordPlayTime( pRecord ) > 0 )
		{
			g_LogDBClient.OnInsertPlayResult( pRecord, LogDBClient::PRT_END_SET );
		}

		if( pRecord->pUser && pRecord->pUser->GetStartTimeLog() > 0 )
		{
			if( pRecord->eState != RS_VIEW && pRecord->eState != RS_OBSERVER )
				pRecord->AddDeathTime( 0 );
			else if( pRecord->eState == RS_OBSERVER )
				g_LogDBClient.OnInsertTime( pRecord->pUser, LogDBClient::TT_VIEW );
			else if( pRecord->eState == RS_VIEW )
				g_LogDBClient.OnInsertTime( pRecord->pUser, LogDBClient::TT_FIGHTMODE_VIEW );
			pRecord->pUser->SetStartTimeLog(0);
		}
	}
	ClearObjectItem();
}

void FightClubMode::SendRoundResult( WinTeamType eWinTeam )
{
	// 해당 패킷을 받으면 클라이언트에서 HP정보를 보내준다.
	SP2Packet kPacket( STPK_ROUND_END );
	kPacket << m_bRoundSetEnd;
	SendRoomAllUser( kPacket );

#ifdef ANTIHACK
	SendRelayGroupTeamWinCnt();
#endif //ANTIHACK
}

bool FightClubMode::IsTimeAbuse()
{
	return m_bTimeAbuse;
}

bool FightClubMode::IsCheerAbuse()
{
	return m_bCheerAbuse;
}

void FightClubMode::SetFightEndResult()
{
	//int i = 0;
	//// 보상 지급
	//SP2Packet kPacket( STPK_FIGHTCLUB_RESULT );
	//kPacket << m_dwFinalResultStateTime;

	//// 챔피언 결과
	//FightClubRecord *pChampion = FindFightClubChampion();
	//if( pChampion && pChampion->pUser )
	//{
	//	//
	//	if( pChampion->bFightWin )
	//	{
	//		pChampion->iFightVictories++;
	//		m_iChampionVictory = pChampion->iFightVictories;
	//	}
	//	else
	//	{
	//		pChampion->iFightVictories = 0;
	//	}

	//	// 경험치 페소 처리
	//	FightEndRewardProcess( pChampion, pChampion->bFightWin );
	//	
	//	kPacket << pChampion->pUser->GetPublicID() << pChampion->iFightVictories << pChampion->bFightWin << pChampion->iTotalExp << pChampion->iTotalPeso << pChampion->pUser->GetGradeLevel() << pChampion->pUser->GetMoney();
	//	// 용병 획득 경험치
	//	int iExpCharSize = pChampion->iResultClassTypeList.size();
	//	kPacket << iExpCharSize;
	//	for(i = 0;i < iExpCharSize;i++)
	//		kPacket << pChampion->iResultClassTypeList[i] << pChampion->iResultClassPointList[i];

	//	// 보너스 항목
	//	for (i = 0; i < BA_MAX ; i++)
	//		kPacket << pChampion->fBonusArray[i];
	//}
	//else
	//{
	//	kPacket << "" << 0 << false << 0 << 0 << 0 << 0 << (__int64)0;

	//	// 보너스 항목
	//	for (i = 0; i < BA_MAX ; i++)
	//		kPacket << 0.0f;
	//}

	//// 도전자 결과
	//FightClubRecord *pChallenger = FindFightClubChallenger();
	//if( pChallenger && pChallenger->pUser )
	//{
	//	//
	//	if( pChallenger->bFightWin )
	//	{
	//		pChallenger->iFightVictories++;
	//		m_iChallengerVictory = pChallenger->iFightVictories;
	//	}
	//	else
	//	{
	//		pChallenger->iFightVictories = 0;
	//	}

	//	// 경험치 페소 처리
	//	FightEndRewardProcess( pChallenger, pChallenger->bFightWin );

	//	kPacket << pChallenger->pUser->GetPublicID() << pChallenger->iFightVictories << pChallenger->bFightWin << pChallenger->iTotalExp << pChallenger->iTotalPeso << pChallenger->pUser->GetGradeLevel() << pChallenger->pUser->GetMoney();

	//	// 용병 획득 경험치
	//	int iExpCharSize = pChallenger->iResultClassTypeList.size();
	//	kPacket << iExpCharSize;
	//	for(i = 0;i < iExpCharSize;i++)
	//		kPacket << pChallenger->iResultClassTypeList[i] << pChallenger->iResultClassPointList[i];

	//	// 보너스 항목
	//	for (i = 0; i < BA_MAX ; i++)
	//		kPacket << pChallenger->fBonusArray[i];
	//}
	//else
	//{
	//	kPacket << "" << 0 << false << 0 << 0 << 0 << 0 << (__int64)0;

	//	// 보너스 항목
	//	for (i = 0; i < BA_MAX ; i++)
	//		kPacket << 0.0f;
	//}
	//SendRoomAllUser( kPacket );
	//
}

void FightClubMode::SetFightEndResult_Renew()
{
	UpdateTournamentRecord();

	// [패킷정의]
	// 시간 승리타입(1:챔피온, 2:도전자) 챔피온 도전자 챔피온등급 도전자등급 연승횟수 타겟타입 [가변]
	// 결과타입    1 : 챔피온에게 전송 (경험치 페소 레벨 머니 보너스정보)
	//            2 : 도전자에게 전송 (경험치 페소 레벨 머니 보너스정보)
	//			  3 : 대기자에게 전송 (응원자아이디 보상페소)
	//   

	int32 winnerIndex = 0, victoryCount = 0, straightVictory = 0;
	int32 champion = 0, challenger = 0, championGrade = 0, challengerGrade = 0;
	FightClubResults resultType = FIGHTCLUB_RESULT_NONE;
	ioHashString kChampionName = "", kChallengerName = "";

	// 챔피언 결과
	FightClubRecord *pChampion = FindFightClubChampion();
	if( pChampion && pChampion->pUser )
	{
		champion		= pChampion->pUser->GetUserIndex();
		championGrade	= pChampion->pUser->GetGradeLevel();
		kChampionName	= pChampion->pUser->GetPublicID();

		if( pChampion->bFightWin )
		{
			pChampion->iFightVictories++;
			m_iChampionVictory	= pChampion->iFightVictories;
			victoryCount		= pChampion->iFightVictories;

			winnerIndex			= 1;
			resultType			= FIGHTCLUB_RESULT_CHANPIONWIN;
		}
		else
		{
			straightVictory = pChampion->iFightVictories;
			
			pChampion->iFightVictories = 0;
			resultType	= FIGHTCLUB_RESULT_CHALLENGERWIN;

			if( m_bUseFightNPC )
			{
				m_iChampionVictory = 1;
				victoryCount = 1;
			}
		}

		// 경험치 페소 처리
		FightEndRewardProcess( pChampion, pChampion->bFightWin );
	}

	// 도전자 결과
	FightClubRecord *pChallenger = FindFightClubChallenger();
	if( pChallenger && pChallenger->pUser )
	{
		challenger		= pChallenger->pUser->GetUserIndex();
		challengerGrade	= pChallenger->pUser->GetGradeLevel();
		kChallengerName = pChallenger->pUser->GetPublicID();

		if( pChallenger->bFightWin )
		{
			pChallenger->iFightVictories++;
			m_iChallengerVictory	= pChallenger->iFightVictories;
			victoryCount			= pChallenger->iFightVictories;

			winnerIndex				= 2;
			resultType				= FIGHTCLUB_RESULT_CHALLENGERWIN;
		}
		else
		{
			pChallenger->iFightVictories = 0;
			resultType	= FIGHTCLUB_RESULT_CHANPIONWIN;
		}

		// 경험치 페소 처리
		FightEndRewardProcess( pChallenger, pChallenger->bFightWin );
	}
	
	// 패킷생성
	int count = m_vRecordList.size();
	for(int i = 0 ; i < count ; i++)
	{
		SP2Packet kPacket( STPK_FIGHTCLUB_RESULT );
		kPacket << m_dwFinalResultStateTime << (int32)resultType << kChampionName << kChallengerName << championGrade << challengerGrade << victoryCount;

		FightClubRecord &rkRecord = m_vRecordList[i];
		if(rkRecord.pUser->GetUserIndex() == champion)
		{
			kPacket << (int)1 << pChampion->iTotalExp << pChampion->iTotalPeso << pChampion->pUser->GetMoney();

			// 용병 획득 경험치
			int iExpCharSize = pChampion->iResultClassTypeList.size();
			kPacket << iExpCharSize;
			for(int charIndex = 0 ; charIndex < iExpCharSize ; charIndex++)
				kPacket << pChampion->iResultClassTypeList[charIndex] << pChampion->iResultClassPointList[charIndex];

			// 보너스 항목
			for (int bonusIndex = 0; bonusIndex < BA_MAX ; bonusIndex++)
				kPacket << pChampion->fBonusArray[bonusIndex];
		}
		else if(rkRecord.pUser->GetUserIndex() == challenger)
		{
			kPacket << (int)2 << pChallenger->iTotalExp << pChallenger->iTotalPeso << pChallenger->pUser->GetMoney();

			// 용병 획득 경험치
			int iExpCharSize = pChallenger->iResultClassTypeList.size();
			kPacket << iExpCharSize;
			for(int charIndex = 0 ; charIndex < iExpCharSize ; charIndex++)
				kPacket << pChallenger->iResultClassTypeList[charIndex] << pChallenger->iResultClassPointList[charIndex];

			// 보너스 항목
			for (int bonusIndex = 0; bonusIndex < BA_MAX ; bonusIndex++)
				kPacket << pChallenger->fBonusArray[bonusIndex];
		}
		else
		{
			int cheerPeso = GetCheerPeso(winnerIndex, straightVictory, rkRecord);
			kPacket << (int)3 << rkRecord.iFightCheer << cheerPeso;
			if( rkRecord.pUser )
			{
				rkRecord.pUser->AddMoney(cheerPeso);
				g_LogDBClient.OnInsertGameLogInfo(LogDBClient::GLT_PESO_GAIN, rkRecord.pUser, 0, 0, LogDBClient::LET_MODE, LogDBClient::PT_BATTLE, GetModeType(), 0, cheerPeso, NULL);
			}
		}

		rkRecord.pUser->SendMessage( kPacket );
	}
}

float FightClubMode::GetScoreGapValue( int iFightState )
{
	float fScoreGap = 0.0f;
	if( iFightState == FIGHTCLUB_CHAMPION )
		fScoreGap = (abs(m_fChallengerPointRate - m_fChampionPointRate) + m_fScoreGapConst) / (m_fChampionPointRate + m_fScoreGapConst);
	else
		fScoreGap = (abs(m_fChallengerPointRate - m_fChampionPointRate) + m_fScoreGapConst) / (m_fChallengerPointRate + m_fScoreGapConst);
	fScoreGap *= m_fScoreGapRateConst;

	if( fScoreGap <= 0 || fScoreGap > m_ModePointRound )
	{
		fScoreGap = 0.0f;
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[warning][fightclub] fScoreGap value is error : [m_fChallengerPointRate:%f m_fChampionPointRate:%f]", m_fChallengerPointRate, m_fChampionPointRate);
	}

	return fScoreGap;
}

//void Mode::FinalRoundPoint( int iRecordIdx, float fTotalVictoriesRate, float fTotalConsecutivelyRate ) 결과를 기존과 다른 방식으로 적용한다.
void FightClubMode::FightEndRewardProcess( FightClubRecord *pRecord, bool bWinRecord )
{
	if( !pRecord )	return;

	User *pUser = pRecord->pUser;
	if( !pUser ) return;

	// 팀 데스매치와 동일한 방식으로 진행됨

	// 옵저버는 이 함수를 실행할 필요가 없다.
	if( pUser->IsObserver() || pUser->IsStealth() ) 
	{
		pUser->SetModeConsecutively( MT_NONE );       // 옵저버는 연속 게임 초기화
		return;
	}

	int iCurMaxSlot = pUser->GetCurMaxCharSlot();

	if( m_dwModePointTime == 0 || m_dwModeRecordPointTime == 0 )
		return;

	bool bAbuseUser = IsAbuseUser( pRecord );
	if( !bAbuseUser )
		bAbuseUser = IsTimeAbuse();    // 시간에 따른 어뷰즈

	//승패 기록
	int iWinLoseTiePoint = 0;
	if( !bAbuseUser )
	{
		iWinLoseTiePoint = GetWinLoseTiePoint( 1.0f, (float)GetRecordPlayTime( pRecord ) / m_dwModeRecordPointTime );
		if( bWinRecord )
			pUser->AddWinCount(  m_pCreator->GetRoomStyle(), GetModeType(), iWinLoseTiePoint );				
		else
			pUser->AddLoseCount(  m_pCreator->GetRoomStyle(), GetModeType(), iWinLoseTiePoint );
	}							

	//플레이 중인 모드
	ModeCategory ePlayMode = GetPlayModeCategory();

	//총 라운드 수 A
	float fRoundPoint = m_ModePointRound;			
	//스코어 차이  B
	float fScoreGap = GetScoreGapValue( pRecord->iFightState );
	//인원 보정 C
	float fUserCorrection = 1.0f;
	//플레이 시간 보정값 D
	float fPlayTimeCorrection = (float)GetRecordPlayTime( pRecord ) / m_dwModePointTime;
	//페소보정값 E
	float fPesoCorrection = m_fPesoCorrection;
	//경험치 보정값 F
	float fExpCorrection  = m_fExpCorrection;
	//차단 G
	float fBlockPoint = pUser->GetBlockPointPer();
	//기여도 H
	float fContributePer = 1.0f;
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
	float tempModeBonus = 0.0f;

	if( pUser )
	{
		ModeCategory eModeBonus = ePlayMode;
		if( eModeBonus != MC_SHUFFLE )
			eModeBonus = MC_DEFAULT;

		// EVT_MODE_BONUS (1)
		EventUserManager &rEventUserManager = pUser->GetEventUserMgr();
		ModeBonusEventUserNode* pEvent1 = static_cast< ModeBonusEventUserNode* >( rEventUserManager.GetEventUserNode( EVT_MODE_BONUS, eModeBonus ) );
		if( pEvent1 )
		{
			if( pEvent1->IsEventMode( GetModeType(), eModeBonus ) )
				tempModeBonus = pEvent1->GetEventPer( fPCRoomBonusExp, pUser, eModeBonus );
		}

		// EVT_MODE_BONUS2 (2)
		ModeBonusEventUserNode* pEvent2 = static_cast< ModeBonusEventUserNode* >( rEventUserManager.GetEventUserNode( EVT_MODE_BONUS2, eModeBonus ) );
		if( pEvent2 )
		{
			if( pEvent2->IsEventMode( GetModeType(), eModeBonus ) )
				tempModeBonus += pEvent2->GetEventPer( fPCRoomBonusExp, pUser, eModeBonus );
		}
		
		if( tempModeBonus == 0.0f )
		{
			tempModeBonus = m_fPlayModeBonus;
		}
	}
	float fModeBonus = tempModeBonus;
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
		if( pRecord->fBonusArray[BA_EVENT] == fEventBonus )
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
		else
		{
			fEventBonus = pRecord->fBonusArray[BA_EVENT];
		}
	}
	// 이벤트 페소 보너스 O
	float fPesoEventBonus = 0.0f;
	if( pUser )
	{
		if( pRecord->fBonusArray[ BA_EVENT_PESO ] == fPesoEventBonus )
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
		else
		{
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
	float fCampBattleBonus = pRecord->fBonusArray[BA_CAMP_BONUS];

	// 시상식 보너스 R
	float fAwardBonus = pRecord->fBonusArray[BA_AWARD_BONUS];

	// 영웅전 타이틀 보너스 S
	float fHeroTitlePesoBonus = pRecord->fBonusArray[BA_HERO_TITLE_PESO];

	// 연속 모드 보너스 T
	float fModeConsecutivelyBonus = 1.0f;

	// 챔피언 모드 연승 보너스 Exp & Peso
	float fFightVictoriesBonus = 0.0f;
	if( bWinRecord )
	{
		if( pRecord->iFightVictories >= 2 )
		{
			int iFightVictoriesCount = min( m_iMaxFightVictoryBonusCount, pRecord->iFightVictories );
			fFightVictoriesBonus = (float)iFightVictoriesCount * m_fFightVictoryBonus;
			pRecord->fBonusArray[BA_VICTORIES_PESO] = fFightVictoriesBonus;
		}
		else if( pRecord->iFightState == FIGHTCLUB_CHALLENGER )
		{
			if( m_iChampionVictory >= 2 )
			{
				int iFightVictoriesCount = min( m_iMaxFightVictoryBonusCount, m_iChampionVictory );
				fFightVictoriesBonus = (float)iFightVictoriesCount * m_fFightVictoryBonus;
				pRecord->fBonusArray[BA_VICTORIES_PESO] = fFightVictoriesBonus;

				// 2번주지 않기 위해서 예외 초기화 - 유저가 이탈하면 초기화된다.
				m_iChampionVictory = 0;
			}
		}
	}
	else
	{
		pRecord->fBonusArray[BA_VICTORIES_PESO] = fFightVictoriesBonus;
	}

	//획득 경험치
	float fAcquireExp       = 0.0f;
	float fExpPlusValue     = max( 0.0f, fContributePer + fGuildBonus + fSoldierCntBonus + fPCRoomBonusExp + fModeBonus + fFriendBonusPer + fEventBonus + fEtcItemBonus + fCampBattleBonus + fEtcItemExpBonus + fFightVictoriesBonus );
	float fExpTotalMultiply = fUserCorrection * fPlayTimeCorrection * fExpCorrection * fBlockPoint * fExpPlusValue;
	char szLogArguWinLose[MAX_PATH]="";
	char szLogArguPlusMinus[MAX_PATH]="";
	if( bWinRecord )
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
	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "[%d] %s [%d]: %s EXP : ( %.2f %s %.2f ) x %.2f x %.4f x %.2f x %.2f x ( %.2f + %.2f + %.2f + %.2f + %.2f + %.2f + %.2f + %.2f + %.2f + %.2f + %.2f ) x %.2f = %.2f",
		m_pCreator->GetRoomIndex(), pUser->GetPublicID().c_str(), (int)bAbuseUser, szLogArguWinLose, 
		fRoundPoint, szLogArguPlusMinus, fScoreGap, fUserCorrection, fPlayTimeCorrection, fExpCorrection, fBlockPoint, fContributePer, fGuildBonus, 
		fSoldierCntBonus, fPCRoomBonusExp, fModeBonus, fFriendBonusPer, fEventBonus, fEtcItemBonus, fCampBattleBonus, fEtcItemExpBonus, fFightVictoriesBonus, fModeConsecutivelyBonus, fAcquireExp );

	if( m_bTournamentRoom )
	{
		// 대회 경기방은 경험치 없음. 
		fAcquireExp = 0.0f;           
	}

	//획득 페소
	float fAcquirePeso       = 0.0f;
	float fPesoPlusValue     = max( 0.0f, fContributePer + fGuildBonus + fPCRoomBonusPeso + fModeBonus + fFriendBonusPer + fPesoEventBonus + fEtcItemBonus + fCampBattleBonus + fAwardBonus + fEtcItemPesoBonus + fHeroTitlePesoBonus + fFightVictoriesBonus );
	float fPesoTotalMultiply = fUserCorrection * fPlayTimeCorrection * fPesoCorrection * fBlockPoint * fPesoPlusValue;
	if( bWinRecord )
	{
		fAcquirePeso = ( fRoundPoint + fScoreGap ) * fPesoTotalMultiply;
	}
	else
	{
		fAcquirePeso = ( fRoundPoint - fScoreGap ) * fPesoTotalMultiply;
	}
	
	float fVictoriesBonus = 1.0f;
	fAcquirePeso = fAcquirePeso * fVictoriesBonus;
	fAcquirePeso = fAcquirePeso * fModeConsecutivelyBonus;

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

	pRecord->iTotalExp  = 0;
	pRecord->iTotalPeso = 0;
	pRecord->iTotalAddPeso = 0;

	// 페소 지급.
// 	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[info][fightclub] result : %s : %s PESO : (( %.2f %s %.2f ) x %.2f x %.4f x %.2f x %.2f x ( %.2f + %.2f + %.2f + %.2f + %.2f + %.2f + %.2f + %.2f + %.2f + %.2f + %.2f + %.2f ) x %.2f x %.2f ) + 0.5 = %.2f",
// 		pUser->GetPublicID().c_str(), szLogArguWinLose,
// 		fRoundPoint, szLogArguPlusMinus, fScoreGap, fUserCorrection, fPlayTimeCorrection, fPesoCorrection, 	fBlockPoint, fContributePer, fGuildBonus, fPCRoomBonusPeso, fModeBonus, fFriendBonusPer, fPesoEventBonus, fEtcItemBonus, fCampBattleBonus, fAwardBonus, fEtcItemPesoBonus, fHeroTitlePesoBonus, fFightVictoriesBonus, fModeConsecutivelyBonus, fVictoriesBonus, fAcquirePeso );

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

	if( !bAbuseUser )
	{
		// 다음주 용병 가격을 위해 플레이 시간을 따로 저장한다.
		g_ItemPriceMgr.SetGradePlayTimeCollected( pRecord->pUser->GetGradeLevel(), GetRecordPlayTime( pRecord ) / 1000 );
	}

	// 출전 시간에 따라 상위 클래스를 구한다.
	DWORD dwTotalTime = pRecord->GetHighPlayingTime( MAX_GET_POINT_CHAR, pRecord->iResultClassTypeList, dwPlayTimeList );
	if( dwTotalTime == 0 ) 
	{
		// 보상 받은 용병 플레이 시간 초기화
		pRecord->PlayTimeInit();

		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Return Playing Time Zero!!!" );
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
	int iGradeNClassUPBonus = pUser->GradeNClassUPBonus();	
	pRecord->iTotalPeso += iGradeNClassUPBonus;
	
	//LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[info][fightclub] grade and class level up bonus %s %d", pUser->GetPublicID().c_str(),iGradeNClassUPBonus );

	//미션
	static DWORDVec vValues;
	vValues.clear();
	if( pUser )
	{
		if( pRecord && m_pCreator )
		{
			vValues.push_back(pRecord->GetAllPlayingTime());
			vValues.push_back(m_pCreator->GetRoomStyle());
			vValues.push_back(GetModeType());

			g_MissionMgr.DoTrigger(MISSION_CLASS_MODEPLAY, pUser, vValues);
		}
	}

	// 보상 받은 용병 플레이 시간 초기화

	pRecord->PlayTimeInit();
}

int FightClubMode::GetCheerPeso( int winnerIndex, int straightVictory, FightClubRecord &rkRecord )
{
	if( IsCheerAbuse() )
	{
		return 0;
	}

	int totalPeso = 0; // 응원 보상 페소
	if( winnerIndex > 0 && rkRecord.iFightCheer == winnerIndex) 
	{
		// 예측이 맞았을 경우의 보상
		double dividendRate = 0; // 배당률
		double winnerWeight = 0; // 승자 가중치

		dividendRate = (double)GetAllCheerCount() / ((double)(GetWinnerCheerCount(winnerIndex) * m_fightCheerAwaiterConst));

		if(straightVictory <= 0)
		{
			winnerWeight = (1 / ((double)m_fightCheerChampionConst + 1));
		}
		else
		{
			winnerWeight = (double)straightVictory / ((double)(straightVictory * m_fightCheerChampionConst) + 1);
		}

		totalPeso = (int)(m_fightCheerPesoConst * (dividendRate + winnerWeight) + 0.5);
	
		if(totalPeso > GetMaxCheerPeso())
			totalPeso = GetMaxCheerPeso();
	}
	return totalPeso;
}

int FightClubMode::GetAllCheerCount()
{
	int count = 0;

	int iRecordCnt = m_vRecordList.size();
	for( int i=0 ; i< iRecordCnt ; i++ )
	{
		FightClubRecord &rkRecord = m_vRecordList[i];
		if(0 != rkRecord.iFightCheer)
		{
			++count;
		}
	}
	return count;
}

int FightClubMode::GetWinnerCheerCount(const int winnerIndex)
{
	int count = 0;

	int iRecordCnt = m_vRecordList.size();
	for( int i=0 ; i< iRecordCnt ; i++ )
	{
		FightClubRecord &rkRecord = m_vRecordList[i];
		if(winnerIndex == rkRecord.iFightCheer)
		{
			++count;
		}
	}
	return count;
}

int FightClubMode::GetMaxCheerPeso()
{
	return m_fightCheerMaxConst;
}

void FightClubMode::GetModeHistory( SP2Packet &rkPacket )
{
	// 사용하지 않음 
}

void FightClubMode::OnEventSceneEnd( User *pSend, SP2Packet &rkPacket )
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
			FightClubRecord *pRecord = FindFightClubRecord( pSend );
			if( pRecord == NULL )
			{
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "FightClubMode::OnEventSceneEnd - %s Not Exist Record",
					pSend->GetPublicID().c_str() );
				return;
			}

			if( pRecord->iFightState != FIGHTCLUB_WAITING )
			{
				SP2Packet kPacket( STPK_START_SELECT_CHAR );
				kPacket << GetSelectCharTime();
				pSend->SendMessage( kPacket );
			}
		}

		if( m_bUseFightNPC )
		{
			CheckFightNPC( pSend, false );
		}
		return;
	}

	ModeRecord *pRecord = FindModeRecord( pSend );
	if( !pRecord )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "FightClubMode::OnEventSceneEnd - %s Not Exist Record",
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
	case MS_NEW_CHALLENGER:
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
		SendRoomAllUser( kPacket );
	}
	else if( IsFightDelay( pRecord->pUser->GetPublicID() ) || ( m_bUseViewMode && m_ModeState == MS_PLAY && dwPastTime > m_dwViewCheckTime ) )
	{
		pRecord->eState = RS_VIEW;
		pRecord->pUser->SetStartTimeLog( TIMEGETTIME() );
		SP2Packet kPacket( STPK_ROUND_JOIN_VIEW );

		kPacket << pRecord->pUser->GetPublicID();
		kPacket << iModeState;
		kPacket << dwPastTime;
		kPacket << m_dwCurRoundDuration;
		SendRoomAllUser( kPacket );
	}
	else
	{
		pRecord->eState = RS_PLAY;
		pRecord->StartPlaying();        //( 관전X, 데스타임X )
		pRecord->pUser->StartCharLimitDate( Mode::GetCharLimitCheckTime(), __FILE__, __LINE__ );
		pRecord->pUser->StartEtcItemTime( __FUNCTION__ );

		SP2Packet kPacket( STPK_ROUND_JOIN );
		kPacket << pRecord->pUser->GetPublicID();
		kPacket << iModeState;
		kPacket << dwPastTime;
		kPacket << GetSelectCharTime();
		kPacket << m_dwCurRoundDuration;
		SendRoomAllUser( kPacket );
	}

	if( m_bUseFightNPC )
	{
		CheckFightNPC( pSend, true );
	}
}

bool FightClubMode::IsFightDelay( const ioHashString &rkName )
{
	FightClubRecord *pRecord = FindFightClubRecord( rkName );
	if( pRecord == NULL )
		return true;

	FightClubRecord *pChamp = FindFightClubChampion();
	if( pChamp == pRecord )
		return false;
	if( pChamp == NULL )
		return false;

	if( m_bUseFightNPC )
		return true;

	FightClubRecord *pChallenger = FindFightClubChallenger();
	if( pChallenger == pRecord )
		return false;
	if( pChallenger == NULL )
		return false;

	return true;
}

bool FightClubMode::IsNextFighter( const ioHashString &rkName )
{
	FightClubRecord *pRecord = FindFightClubRecord( rkName );
	if( pRecord == NULL )
		return false;
	if( pRecord->eState == RS_OBSERVER )  
		return false;     // 관전자
	if( pRecord->iFightState != FIGHTCLUB_WAITING )
		return false;     // 이미 파이터다!!

	// 이미 파이터 2명이 선정 되었다.
	if( FindFightClubChampion() && FindFightClubChallenger() )
		return false;
	

	int iLowSequence = MAX_INT_VALUE;	
	int iRecordCnt = GetRecordCnt();
	for( int i=0 ; i<iRecordCnt ; i++ )
	{
		FightClubRecord *pFightRecord = static_cast< FightClubRecord * >( FindModeRecord( i ) );
		if( pFightRecord )
		{
			if( pFightRecord->eState == RS_OBSERVER ) continue;
			if( pFightRecord->iFightState != FIGHTCLUB_WAITING ) continue;

			if( pFightRecord->iFightSeq < iLowSequence )
			{
				iLowSequence = pFightRecord->iFightSeq;
			}
		}
	}

	if( pRecord->iFightSeq == iLowSequence )
		return true;
	return false;
}

void FightClubMode::UpdateRoundRecord()
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
}

TeamType FightClubMode::GetTournamentWinTeam( DWORD dwBlueTeamIndex )
{
	TeamType eWinTeam = TEAM_NONE;
	FightClubRecord *pChampion = FindFightClubChampion();
	if( pChampion && pChampion->pUser )
	{
		if( pChampion->bFightWin )			
		{
			// 챔피언이 이겼으면 챔피언의 팀 인덱스에 맞게 팀 승리
			ioUserTournament *pUserTournament = pChampion->pUser->GetUserTournament();
			if( pUserTournament )
			{
				if( pUserTournament->IsAlreadyTeam( dwBlueTeamIndex ) )
					eWinTeam = TEAM_BLUE;
				else 
					eWinTeam = TEAM_RED;
			}
		}
		else
		{
			// 도전자가 이겼으면 도전자의 팀 인덱스에 맞게 팀 승리
			ioUserTournament *pUserTournament = pChampion->pUser->GetUserTournament();
			if( pUserTournament )
			{
				if( pUserTournament->IsAlreadyTeam( dwBlueTeamIndex ) )
					eWinTeam = TEAM_RED;
				else 
					eWinTeam = TEAM_BLUE;
			}
		}
	}

	if( eWinTeam == TEAM_NONE )
	{
		// 챔피언이 나갔다!!!!!
		FightClubRecord *pChallenger = FindFightClubChallenger();
		if( pChallenger && pChallenger->pUser )
		{
			// 무조건 도전자 승리
			ioUserTournament *pUserTournament = pChallenger->pUser->GetUserTournament();
			if( pUserTournament )
			{
				if( pUserTournament->IsAlreadyTeam( dwBlueTeamIndex ) )
					eWinTeam = TEAM_BLUE;
				else 
					eWinTeam = TEAM_RED;
			}
		}
	}

	return eWinTeam;
}

void  FightClubMode::UpdateTournamentRecord()
{
	if( !m_bTournamentRoom ) return;

	BattleRoomParent *pBattleRoom = NULL;

	int iRecordCnt = GetRecordCnt();
	for(int i = 0;i < iRecordCnt;i++)
	{
		ModeRecord *pRecord = FindModeRecord( i );
		if( pRecord && pRecord->pUser )
		{
			pBattleRoom = pRecord->pUser->GetMyBattleRoom();
			if( pBattleRoom && pBattleRoom->IsOriginal() )
				break;
		}
	}

	if( pBattleRoom == NULL ) return;

	BattleRoomNode *pBattleNode = static_cast< BattleRoomNode * >( pBattleRoom );

	if( pBattleNode->IsOriginal() )
	{
		if( pBattleNode->GetBattleEventType() != BET_TOURNAMENT_BATTLE )
			return;

		DWORD dwTourIndex	  = pBattleNode->GetTournamentIndex();
		DWORD dwBlueTeamIndex = pBattleNode->GetTournamentBlueIndex();
		DWORD dwRedTeamIndex  = pBattleNode->GetTournamentRedIndex();
		BYTE  BlueTourPos     = pBattleNode->GetTournamentBlueTourPos();
		BYTE  RedTourPos      = pBattleNode->GetTournamentRedTourPos();
		TeamType eWinTeam     = GetTournamentWinTeam( dwBlueTeamIndex );

		LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "FightClubMode::UpdateTournamentRecord() %d - %d - %d - %d - %d - %d - %d - %d", dwTourIndex, dwBlueTeamIndex, dwRedTeamIndex, (int)BlueTourPos, (int)RedTourPos, (int)eWinTeam, m_iBlueTeamWinCnt, m_iRedTeamWinCnt );

		// 결과를 전송 - - -
		switch( eWinTeam )
		{
		case TEAM_BLUE:
			{
				SP2Packet kPacket( MSTPK_TOURNAMENT_BATTLE_RESULT );
				kPacket << dwTourIndex << dwBlueTeamIndex << dwRedTeamIndex << BlueTourPos + 1;
				g_MainServer.SendMessage( kPacket );

				LOG.PrintTimeAndLog(0, "%s - win : %d - lose : %d, win-pos : %d", __FUNCTION__, dwBlueTeamIndex, dwRedTeamIndex, BlueTourPos + 1 );
			}
			break;
		case TEAM_RED:
			{
				SP2Packet kPacket( MSTPK_TOURNAMENT_BATTLE_RESULT );
				kPacket << dwTourIndex << dwRedTeamIndex << dwBlueTeamIndex << RedTourPos + 1;
				g_MainServer.SendMessage( kPacket );

				LOG.PrintTimeAndLog(0, "%s - win : %d - lose : %d, win-pos : %d", __FUNCTION__, dwRedTeamIndex , dwBlueTeamIndex, RedTourPos + 1 );
			}
			break;
		default:
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "FightClubMode::UpdateTournamentRecord() Win Team None : %d", dwTourIndex );
			break;
		}
	}
	else
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "FightClubMode::UpdateTournamentRecord() Not Battle Room" );
	}
}

ModeType FightClubMode::GetModeType() const
{
	return MT_FIGHT_CLUB;
}

void FightClubMode::GetCharModeInfo( SP2Packet &rkPacket, const ioHashString &szName, bool bDieCheck )
{
	FightClubRecord *pRecord = FindFightClubRecord( szName );
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
		rkPacket << pRecord->iFightState << pRecord->iFightSeq << pRecord->iFightVictories << pRecord->iFightCheer;
	}
	else
	{
		// 레코드 정보 유무
		rkPacket << false;
	}
}

void FightClubMode::GetNewUserModeInfo( SP2Packet &rkPacket, User *pUser )
{
	if( pUser == NULL )
		rkPacket << 0 << 0;
	else
	{
		FightClubRecord *pRecord = FindFightClubRecord( pUser );
		if( pRecord == NULL )
			rkPacket << 0 << 0 << 0 << 0;
		else
			rkPacket << pRecord->iFightState << pRecord->iFightSeq << pRecord->iFightVictories << pRecord->iFightCheer;
	}
}

int FightClubMode::GetRecordCnt() const
{
	return m_vRecordList.size();
}

const char* FightClubMode::GetModeINIFileName() const
{
	return "config/fightmode.ini";
}

void FightClubMode::GetModeInfo( SP2Packet &rkPacket )
{
	rkPacket << GetModeType();
	rkPacket << m_iCurRound;
	rkPacket << m_iMaxRound;

	rkPacket << m_dwRoundDuration;

	int iPosCnt = m_SingleTeamPosArray.size();
	rkPacket << iPosCnt;

	for( int i=0; i < iPosCnt; ++i )
		rkPacket << m_SingleTeamPosArray[i];
}

TeamType FightClubMode::GetNextTeamType()
{
	if( m_SingleTeamList.empty() )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "FightClubMode::GetNextTeamType Not Team : %d", (int)m_vRecordList.size() );
		return TEAM_NONE;
	}

	return (TeamType)m_SingleTeamList[0];
}

int FightClubMode::GetUserKickVoteLimit( const ioHashString &szKickUserName )
{
	ModeRecord *pKickRecord = FindModeRecord( szKickUserName );
	if( !pKickRecord || !pKickRecord->pUser )
		return USER_KICK_VOTE_PROPOSAL_ERROR_7;

	// 인원 체크 
	if( !pKickRecord->pUser->IsObserver() )
	{
		int iPlayUserCount = 0;
		int iRecordCnt = GetRecordCnt();
		for(int i = 0;i < iRecordCnt;i++)
		{
			ModeRecord *pRecord = FindModeRecord( i );
			if( !pRecord || !pRecord->pUser ) continue;
			if( pRecord->pUser->IsObserver() ) continue;
			if( pRecord->pUser->IsStealth() ) continue;

			iPlayUserCount++;
		}
		if( iPlayUserCount <= m_KickOutVote.GetKickVoteUserPool() )
			return USER_KICK_VOTE_PROPOSAL_ERROR_11;
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

ModeRecord* FightClubMode::FindModeRecord( const ioHashString &rkName )
{
	if( rkName.IsEmpty() )	return NULL;

	CRASH_GUARD();
	int iRecordCnt = m_vRecordList.size();
	for( int i=0 ; i<iRecordCnt ; i++ )
	{
		if( m_vRecordList[i].pUser->GetPublicID() == rkName )
		{
			return &m_vRecordList[i];
		}
	}

	return NULL;
}

ModeRecord* FightClubMode::FindModeRecord( User *pUser )
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

ModeRecord* FightClubMode::FindModeRecord( int iIdx )
{
	if( COMPARE( iIdx, 0, GetRecordCnt() ) )
		return &m_vRecordList[iIdx];

	return NULL;
}


FightClubRecord* FightClubMode::FindFightClubRecord( const ioHashString &rkName )
{
	return (FightClubRecord*)FindModeRecord( rkName );
}

FightClubRecord* FightClubMode::FindFightClubRecord( User *pUser )
{
	return (FightClubRecord*)FindModeRecord( pUser );
}

FightClubRecord* FightClubMode::FindFightClubRecordByUserID( const ioHashString &rkUserID )
{
	if( rkUserID.IsEmpty() )	return NULL;

	CRASH_GUARD();
	int iRecordCnt = m_vRecordList.size();
	for( int i=0 ; i<iRecordCnt ; i++ )
	{
		if( m_vRecordList[i].pUser->GetPublicID() == rkUserID )
		{
			return &m_vRecordList[i];
		}
	}

	return NULL;
}

FightClubRecord* FightClubMode::FindFightClubChampion()
{
	int iRecordCnt = GetRecordCnt();
	for( int i=0 ; i<iRecordCnt ; i++ )
	{
		FightClubRecord *pRecord = static_cast< FightClubRecord * >( FindModeRecord( i ) );
		if( pRecord )
		{
			if( pRecord->iFightState == FIGHTCLUB_CHAMPION )
				return pRecord;
		}
	}
	return NULL;
}

FightClubRecord* FightClubMode::FindFightClubChallenger()
{
	int iRecordCnt = GetRecordCnt();
	for( int i=0 ; i<iRecordCnt ; i++ )
	{
		FightClubRecord *pRecord = static_cast< FightClubRecord * >( FindModeRecord( i ) );
		if( pRecord )
		{
			if( pRecord->iFightState == FIGHTCLUB_CHALLENGER )
				return pRecord;
		}
	}
	return NULL;
}

int FightClubMode::GetCurTeamUserCnt( TeamType eTeam )
{
	return 1;
}

void FightClubMode::UpdateUserDieTime( User *pDier )
{
	ModeRecord *pDieRecord = FindModeRecord( pDier );
	if( pDieRecord && pDieRecord->bDieState )
	{
		DWORD dwRevivalGap = (DWORD)GetRevivalGapTime( pDieRecord->iRevivalCnt );
		pDieRecord->dwCurDieTime = TIMEGETTIME();
		pDieRecord->dwRevivalGap = dwRevivalGap;
		pDieRecord->iRevivalCnt++;
	}
}

void FightClubMode::CheckUserLeaveEnd()
{
	if( m_bRoundSetEnd )
		return;

	if( m_bUseFightNPC )
	{
		if( FindFightClubChampion() )
			return;
	}
	else
	{
		if( FindFightClubChampion() && FindFightClubChallenger() ) 
			return;
	}
	
	if( GetState() == MS_RESULT_WAIT || GetState() == MS_RESULT )
		return;      // 결과중에는 받지 않는다.

	// 챔피언과 도전자가 있으면 라운드 종료 체크할 필요없다.
	CheckRoundEnd( false );
}

bool FightClubMode::ProcessTCPPacket( User *pSend, SP2Packet &rkPacket )
{
	if( Mode::ProcessTCPPacket( pSend, rkPacket ) )
		return true;

	switch( rkPacket.GetPacketID() )
	{
	case CTPK_FIGHTCLUB_CHEER:
		OnFightClubCheer( pSend, rkPacket );
		return true;
	case CTPK_FIGHTCLUB_RESULT_INFO:
		OnFightClubResultInfo( pSend, rkPacket );
		return true;
	case CTPK_FIGHT_NPC:
		OnFightClubFightNPC( pSend, rkPacket );
		return true;
	}

	return false;
}

void FightClubMode::OnFightClubCheer( User *pSend, SP2Packet &rkPacket )
{
	if( pSend == NULL ) return;
	if( GetState() == MS_RESULT_WAIT || GetState() == MS_RESULT ) return;
	
	DWORD dwGapTime = TIMEGETTIME() - m_dwStateChangeTime;
	if( dwGapTime > 80000 )
	{
		// 80초 이상 걸리면 패스 본인에게만 전송 - 보상이 들어가면 더 타이트하게 잡자!!
		FightClubRecord *pRecord = static_cast< FightClubRecord * >( FindModeRecord( pSend ) );
		if( pRecord )
		{
			// 실패
			SP2Packet kPacket( STPK_FIGHTCLUB_CHEER );
			kPacket << false << pRecord->iFightCheer;
			pSend->SendMessage( kPacket );
		}
	}
	else
	{
		// 변경 사항 적용
		FightClubRecord *pRecord = static_cast< FightClubRecord * >( FindModeRecord( pSend ) );
		if( pRecord )
		{
			if( pRecord->iFightState == FIGHTCLUB_WAITING )
			{
				rkPacket >> pRecord->iFightCheer;

				// 성공
				SP2Packet kPacket( STPK_FIGHTCLUB_CHEER );
				kPacket << true << pSend->GetPublicID() << pRecord->iFightCheer;
				m_pCreator->RoomSendPacketTcp( kPacket );			
			}
			else
			{
				// 실패
				SP2Packet kPacket( STPK_FIGHTCLUB_CHEER );
				kPacket << false << pRecord->iFightCheer;
				pSend->SendMessage( kPacket );
			}
		}
	}
}

void FightClubMode::OnFightClubResultInfo( User *pSend, SP2Packet &rkPacket )
{
	if( pSend == NULL ) return;
	if( GetState() != MS_RESULT_WAIT ) return;
	if( m_bFighterInfoPacket ) return;

	//
	rkPacket >> m_fChampionPointRate >> m_fChallengerPointRate;

	if( m_fChampionPointRate > 1.0f )
	{
		m_fChampionPointRate	= 0.0f;
	}
	if( m_fChallengerPointRate > 1.0f )
	{
		m_fChallengerPointRate	= 0.0f;
	}

	// 승리 유저와 패배 유저.
	if( m_bUseFightNPC )
	{
		FightClubRecord *pChamp = FindFightClubChampion();
		if( pChamp )
		{
			if( pChamp->bDieState )
			{
				pChamp->bFightWin = false;
			}
			else if( m_CurFightNPCRecord.eState == RS_DIE )
			{
				pChamp->bFightWin = true;
			}
			else
			{
				// 두명 모두 존재할 경우 HP 승패
				if( m_fChampionPointRate >= m_fChallengerPointRate )
				{
					pChamp->bFightWin = true;
				}
				else
				{
					pChamp->bFightWin = false;
				}
			}
		}
	}
	else
	{
		FightClubRecord *pChamp = FindFightClubChampion();
		FightClubRecord *pChallenger = FindFightClubChallenger();
		if( !pChamp || !pChallenger )
		{
			// 두명중에 한명이 이탈한 상황이면 남아있는 유저 승리
			if( pChamp )
			{
				if( pChallenger == NULL )
				{
					pChamp->bFightWin = true;
					LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "Champion Win(%s) 1: %.2f - %.2f", pChamp->pUser->GetPublicID().c_str(), m_fChampionPointRate, m_fChallengerPointRate );
				}
			}

			if( pChallenger )
			{
				if( pChamp == NULL )
				{
					pChallenger->bFightWin = true;
					LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "Challenger Win(%s) 2: %.2f - %.2f", pChallenger->pUser->GetPublicID().c_str(), m_fChampionPointRate, m_fChallengerPointRate );
				}
			}
		}
		else
		{
			if( pChamp->bDieState )
			{
				pChallenger->bFightWin = true;
				LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "Challenger Win(%s) 3: %.2f - %.2f", pChallenger->pUser->GetPublicID().c_str(), m_fChampionPointRate, m_fChallengerPointRate );
			}
			else if( pChallenger->bDieState )
			{
				pChamp->bFightWin = true;
				LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "Champion Win(%s) 4: %.2f - %.2f", pChamp->pUser->GetPublicID().c_str(), m_fChampionPointRate, m_fChallengerPointRate );
			}
			else
			{
				// 두명 모두 존재할 경우 HP 승패
				if( m_fChampionPointRate > m_fChallengerPointRate )
				{
					pChamp->bFightWin = true;
					LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "Champion Win(%s) 5: %.2f - %.2f", pChamp->pUser->GetPublicID().c_str(), m_fChampionPointRate, m_fChallengerPointRate );
				}
				else if( m_fChallengerPointRate > m_fChampionPointRate )
				{
					pChallenger->bFightWin = true;
					LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "Champion Win(%s) 6: %.2f - %.2f", pChallenger->pUser->GetPublicID().c_str(), m_fChampionPointRate, m_fChallengerPointRate );
				}
				else
				{
					// 착용한 아이템 갯수에 따른 승패
					if( pChamp->pUser && pChallenger->pUser )
					{
						if( pChamp->pUser->GetEquipItemCount() < pChallenger->pUser->GetEquipItemCount() )
						{
							pChallenger->bFightWin = true;
						}
						else
						{
							pChamp->bFightWin = true;
						}
					}
					else
					{
						pChamp->bFightWin = true;
					}
					LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "Champion Win(%s) 7: %.2f - %.2f", pChamp->pUser->GetPublicID().c_str(), m_fChampionPointRate, m_fChallengerPointRate );
				}
			}
		}
	}


	// 이탈한 유저는 -1000.0으로 도착하므로 0.0으로 만들어줘야함.
	if( m_fChampionPointRate <= 0.0f )
	{
		m_fChampionPointRate = 0.0f;
	}

	if( m_fChallengerPointRate <= 0.0f )
	{
		m_fChallengerPointRate = 0.0f;
	}
	// 최소 0.0을 넣지 않는다.
	m_fChampionPointRate += 0.0001f;
	m_fChallengerPointRate += 0.0001f;
	m_bFighterInfoPacket = true;
}

void FightClubMode::OnDropDieUser( User *pDieUser, SP2Packet &rkPacket )
{
	if( pDieUser == NULL ) 
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "FightClubMode::OnDropDieUser None User!!" );
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

	FightClubRecord *pRecord = FindFightClubRecord( pDieUser );
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

				ModeRecord *pDamageRecord = FindModeRecord( kDamageTable.szName );
				if( pDamageRecord )
				{
					pDamageRecord->iTotalDamage += kDamageTable.iDamage;
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
}

void FightClubMode::OnWeaponDieUser( User *pDieUser, SP2Packet &rkPacket )
{
	if( pDieUser == NULL ) 
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "FightClubMode::OnWeaponDieUser None User!!" );
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

	FightClubRecord *pRecord = FindFightClubRecord( pDieUser );
	if( !pRecord ) return;

	if( pRecord->eState == RS_LOADING ) return;

	if( pRecord->pUser->IsEquipedItem() ) return;

	pRecord->SetDieLastAttackerInfo( szLastAttackerName, (TeamType)iLastAttackerTeam, dwLastAttackerWeaponItemCode );

	UpdateDieState( pRecord->pUser );

	int iDamageCnt = 0;
	ioHashString szBestAttackerName;
	PACKET_GUARD_VOID( rkPacket.Read(iDamageCnt) );
	MAX_GUARD(iDamageCnt, 50);

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
}

void FightClubMode::UpdateDropDieRecord( User *pDier, const ioHashString &szAttacker, const ioHashString &szBestAttacker  )
{
	if( m_bUseFightNPC )	// npc와 대전중이면 무시
		return;

	Mode::UpdateDropDieRecord( pDier, szAttacker, szBestAttacker );
}

void FightClubMode::UpdateWeaponDieRecord( User *pDier, const ioHashString &szAttacker, const ioHashString &szBestAttacker )
{
	if( m_bUseFightNPC )	// npc와 대전중이면 무시
		return;

	Mode::UpdateWeaponDieRecord( pDier, szAttacker, szBestAttacker );
}

void FightClubMode::UpdateUserDieNextProcess( User *pDier, const ioHashString &szAttacker, const ioHashString &szBestAttacker )
{
	if( GetState() != Mode::MS_PLAY ) return;

	CheckRoundEnd( false );
}

void FightClubMode::SetUseFightNPC( bool bSetNPC )
{
	if( m_FightNPCList.empty() )
	{
		m_bUseFightNPC = false;
		return;
	}

	m_bUseFightNPC = bSetNPC;
}

void FightClubMode::SendFightNPCInfo( User *pUser )
{
	if( !pUser )
		return;

	if( m_FightNPCList.empty() )
		return;
	
	int iNPCSize = m_FightNPCList.size();
	int iNPCCode = 0;

	int iTeamType = TEAM_PRIVATE_50;
	m_FightNPCRecordList.reserve( iNPCSize );

	SP2Packet kPacket( STPK_FIGHT_NPC );
	kPacket << FIGHT_NPC_INFO;
	kPacket << m_bUseFightNPC;
	kPacket << iNPCSize;

	for( int i=0; i < iNPCSize; ++i )
	{
		FightNPCRecord kRecord;

		char szNpcName[MAX_PATH] = "";

		iTeamType++;
		iNPCCode = m_FightNPCList[i];

		sprintf_s( szNpcName, " -N%d- ", iNPCCode );

		kRecord.dwCode = iNPCCode;
		kRecord.szName = szNpcName;
		kRecord.eState = RS_PLAY;
		kRecord.eTeam = (TeamType)iTeamType;

		m_FightNPCRecordList.push_back( kRecord );

#ifdef ANTIHACK
		kPacket << kRecord.dwNPCIndex << kRecord.dwCode << kRecord.szName << kRecord.eTeam;
#else
		kPacket << kRecord.dwCode << kRecord.szName << kRecord.eTeam;
#endif

	}

	pUser->SendMessage( kPacket );
}

void FightClubMode::CheckFightNPC( User *pUser, bool bFirst )
{
	FightClubRecord *pChampion = FindFightClubChampion();
	if( !pChampion )
		return;

	if( m_FightNPCList.empty() )
		return;

	bool bAllUser = false;
	if( pChampion->pUser->GetPublicID() == pUser->GetPublicID() )
	{
		bAllUser = true;
	}
	
	int iNPCSize = m_FightNPCList.size();
	int iNPCCode = 0;

	/*
	if( m_bFirstNPC || bFirst )
	{
		m_bFirstNPC = false;
		SendFightNPCInfo( pUser );
	}
	*/

	if( !COMPARE( m_iCurFightNPCStage, 0, iNPCSize ) )
		m_iCurFightNPCStage = 0;

	if( m_FightNPCRecordList[m_iCurFightNPCStage].dwCode > 0 )
	{
		m_CurFightNPCRecord = m_FightNPCRecordList[m_iCurFightNPCStage];
		m_CurFightNPCRecord.eState = RS_PLAY;

		SP2Packet kPacket( STPK_FIGHT_NPC );
		kPacket << FIGHT_NPC_CREATE;
		kPacket << m_iCurFightNPCStage;
		kPacket << pChampion->pUser->GetPublicID();

		if( bAllUser )
			SendRoomAllUser( kPacket );
		else
			pUser->SendMessage( kPacket );
	}
}

void FightClubMode::OnDropDieNpc( const ioHashString &rkDieName, SP2Packet &rkPacket )
{
	if( m_CurFightNPCRecord.szName != rkDieName )
		return;

	if( m_CurFightNPCRecord.eState != RS_PLAY )
		return;

	float fDiePosX, fDiePosZ;
	rkPacket >> fDiePosX >> fDiePosZ;

	int iLastAttackerTeam;
	ioHashString szLastAttackerName;
	ioHashString szLastAttackerSkillName;
	DWORD dwLastAttackerWeaponItemCode;
	rkPacket >> szLastAttackerName >> szLastAttackerSkillName >> dwLastAttackerWeaponItemCode >> iLastAttackerTeam;

	// npc 죽음 처리
	{
		m_CurFightNPCRecord.eState = RS_DIE;
		m_CurFightNPCRecord.dwCurDieTime = TIMEGETTIME();
	}

	// 데미지 리스트 처리
	int iDamageCnt;
	ioHashString szBestAttackerName;
	rkPacket >> iDamageCnt;

	int iTotalDamage = 0;
	int iLastDamage = 0;
	int iBestDamage = 0;

	DamageTableList vDamageList;

	if(iDamageCnt > 100)
	{
		LOG.PrintTimeAndLog(0, "CTPK_DROP_DIE(Champs Mode)Error - DamageCnt:%d  LastAttacker:%s", iDamageCnt, szLastAttackerName.c_str());
		iDamageCnt = 0;
	}

	if( iDamageCnt > 0 )
	{
		vDamageList.reserve( iDamageCnt );

		for( int i=0; i < iDamageCnt; ++i )
		{
			DamageTable kDamageTable;
			rkPacket >> kDamageTable.szName;
			rkPacket >> kDamageTable.iDamage;

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

	float fLastRate = 0.0f;
	float fBestRate = 0.0f;

	if( iTotalDamage > 0 )
	{
		fLastRate = (float)iLastDamage / iTotalDamage;
		fBestRate = (float)iBestDamage / iTotalDamage;
	}

	SP2Packet kReturn( STPK_DROP_DIE );
	kReturn << m_CurFightNPCRecord.szName;
	kReturn << szLastAttackerName;
	kReturn << szLastAttackerSkillName;
	kReturn << dwLastAttackerWeaponItemCode;
	kReturn << iLastAttackerTeam;
	kReturn << szBestAttackerName;
	kReturn << fLastRate;
	kReturn << fBestRate;
	GetCharModeInfo( kReturn, m_CurFightNPCRecord.szName );
	GetCharModeInfo( kReturn, szLastAttackerName );
	SendRoomAllUser( kReturn );

	if( GetState() != Mode::MS_PLAY )
		return;

	CheckRoundEnd( false );
}

void FightClubMode::OnWeaponDieNpc( const ioHashString &rkDieName, SP2Packet &rkPacket )
{
	if( m_CurFightNPCRecord.szName != rkDieName )
		return;

	if( m_CurFightNPCRecord.eState != RS_PLAY )
		return;

	float fDiePosX, fDiePosZ;
	rkPacket >> fDiePosX >> fDiePosZ;

	// Killer 유저 정보.
	int iLastAttackerTeam;
	ioHashString szLastAttackerName;
	ioHashString szLastAttackerSkillName;
	DWORD dwLastAttackerWeaponItemCode;
	rkPacket >> szLastAttackerName >> szLastAttackerSkillName >> dwLastAttackerWeaponItemCode >> iLastAttackerTeam;

	// 몬스터 죽음 처리
	{
		m_CurFightNPCRecord.eState = RS_DIE;
		m_CurFightNPCRecord.dwCurDieTime = TIMEGETTIME();
	}

	// 데미지 리스트 처리
	int iDamageCnt;
	ioHashString szBestAttackerName;
	rkPacket >> iDamageCnt;

	int iTotalDamage = 0;
	int iLastDamage = 0;
	int iBestDamage = 0;

	DamageTableList vDamageList;

	if(iDamageCnt > 100)
	{
		LOG.PrintTimeAndLog(0, "CTPK_DROP_DIE(Champs Mode)Error - DamageCnt:%d  LastAttacker:%s", iDamageCnt, szLastAttackerName.c_str());
		iDamageCnt = 0;
	}

	if( iDamageCnt > 0 )
	{
		vDamageList.reserve( iDamageCnt );

		for( int i=0; i < iDamageCnt; ++i )
		{
			DamageTable kDamageTable;
			rkPacket >> kDamageTable.szName;
			rkPacket >> kDamageTable.iDamage;

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

	float fLastRate = 0.0f;
	float fBestRate = 0.0f;

	if( iTotalDamage > 0 )
	{
		fLastRate = (float)iLastDamage / iTotalDamage;
		fBestRate = (float)iBestDamage / iTotalDamage;
	}

	SP2Packet kReturn( STPK_WEAPON_DIE );
	kReturn << m_CurFightNPCRecord.szName;
	kReturn << szLastAttackerName;
	kReturn << szLastAttackerSkillName;
	kReturn << dwLastAttackerWeaponItemCode;
	kReturn << iLastAttackerTeam;
	kReturn << szBestAttackerName;
	kReturn << fLastRate;
	kReturn << fBestRate;
	GetCharModeInfo( kReturn, m_CurFightNPCRecord.szName );
	GetCharModeInfo( kReturn, szLastAttackerName );
	SendRoomAllUser( kReturn );

	if( GetState() != Mode::MS_PLAY )
		return;

	CheckRoundEnd( false );
}

void FightClubMode::OnFightClubFightNPC( User *pSend, SP2Packet &rkPacket )
{
	if( pSend == NULL ) return;

	int iType;
	rkPacket >> iType;

	switch( iType )
	{
	case FIGHT_NPC_RETRY:
		{
			if( m_bRequestDestroy || (GetState() != MS_NPC_CONTINUE) )
				return;

			m_iCurFightNPCStage--;
			if( m_iCurFightNPCStage <= 0 )
				m_iCurFightNPCStage = 0;

			RestartMode();
		}
		break;
	case FIGHT_NPC_END:
		{
			if( m_bRequestDestroy || (GetState() != MS_NPC_CONTINUE) )
				return;

			m_bRequestDestroy = true;

			// log
			int iRecordCnt = GetRecordCnt();
			for(int i = 0;i < iRecordCnt;i++ )
			{
				ModeRecord *pRecord = FindModeRecord( i );
				SetModeEndDBLog( pRecord, iRecordCnt, LogDBClient::PRT_END_SET );
			}
		}
		break;
	}
}


