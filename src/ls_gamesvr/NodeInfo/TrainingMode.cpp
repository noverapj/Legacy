#include "stdafx.h"
#include "../MainProcess.h"

#include "User.h"
#include "Room.h"
#include "RoomNodeManager.h"
#include "ModeHelp.h"
#include "ioEventUserNode.h"
#include "ioEtcItemManager.h"
#include "ioSetItemInfoManager.h"
#include "ioItemInfoManager.h"
#include "ioSetItemInfo.h"
#include "MonsterSurvivalModeHelp.h"
#include "ioMonsterMapLoadMgr.h"
#include "ioExerciseCharIndexManager.h"
#include "TrainingMode.h"

#include "../EtcHelpFunc.h"
#include "../DataBase/LogDBClient.h"
#include "../DataBase/DBClient.h"
#include <strsafe.h>
#include "../MainServerNode/MainServerNode.h"

#define MAX_RANK		32

TrainingMode::TrainingMode( Room *pCreator ) : Mode( pCreator )
{
	m_bHasCrown = false;

	m_dwEtcItemCheckTime = 0;
	m_dwEtcItemCurTime   = 0;

	m_dwMaxRunningManDeco = 0;

	m_bNpcMode = false;
	m_nNpcCount = 0;
	m_dwDiceRewardTime = 5000;

	m_dwAliveTime = 0;

	m_vecMonsterRec.clear();
	m_PlazaTeamList.clear();
	m_PlazaTeamPosArray.clear();

	m_eNpc = NORMAL_NPC;
}

TrainingMode::~TrainingMode()
{
}

void TrainingMode::LoadMonsterSpawnFactor( ioINILoader &rkLoader )
{
	rkLoader.SetTitle( "monster_factor");

	m_dwStartSpawnMin = rkLoader.LoadInt( "start_spawn_time_min", 30000 );
	m_dwStartSpawnMax = rkLoader.LoadInt( "start_spawn_time_max", 120000 );

	m_nSpawnRnd = rkLoader.LoadInt( "npc_spawn_rate", 0 );
	m_nAwakeningRnd = rkLoader.LoadInt( "awakening_npc_spawn_rate", 500 );

	m_dwSpawnMin = rkLoader.LoadInt( "spawn_time_min", 30000 );
	m_dwSpawnMax = rkLoader.LoadInt( "spawn_time_max", 60000 );

	m_dwDiceRewardTime = rkLoader.LoadInt( "DiceRewardTime", 5000 );

	m_IORandom.Randomize();
	m_dwSpawnTime = m_IORandom.Random( m_dwStartSpawnMin, m_dwStartSpawnMax );
}

void TrainingMode::LoadRoundCtrlValue( ioINILoader &rkLoader )
{
	rkLoader.SetTitle( "info" );
	m_iMaxPlayer = min( MAX_PLAZA_PLAYER, rkLoader.LoadInt( "max_player", MAX_PLAZA_PLAYER ) );
	m_iConstMaxPlayer = min( MAX_PLAZA_PLAYER, rkLoader.LoadInt( "const_max_player", MAX_PLAZA_PLAYER ) );

	rkLoader.SetTitle( "round" );

	m_iMaxRound  = rkLoader.LoadInt( "max_round", 4 );

	m_dwRoundDuration   = rkLoader.LoadInt( "round_time", 300000 );
	m_dwReadyStateTime  = rkLoader.LoadInt( "ready_state_time", 4000 );
	m_dwSuddenDeathTime = rkLoader.LoadInt( "sudden_death_time", 60000 );
	m_dwResultStateTime = rkLoader.LoadInt( "result_state_time", 7000 );
	m_dwFinalResultWaitStateTime = rkLoader.LoadInt( "final_result_wait_state_time", 10000 );
	m_dwFinalResultStateTime = rkLoader.LoadInt( "final_result_state_time", 13000 );
	m_dwTournamentRoomFinalResultAddTime = rkLoader.LoadInt( "tournament_room_final_result_time", 10000 );

	if( m_pCreator->GetRoomStyle() == RSTYLE_BATTLEROOM )
		m_dwTimeClose       = rkLoader.LoadInt("sham_battle_close_time", 10000 );
	else
		m_dwTimeClose       = rkLoader.LoadInt("room_close_time", 10000 );

	m_bUseViewMode = rkLoader.LoadBool( "use_view_mode", false );
	m_dwViewCheckTime = rkLoader.LoadInt( "view_check_time", 30000 );

	m_iAbuseMinDamage = rkLoader.LoadInt( "abuse_min_damage", 1 );

	m_iScoreRate = rkLoader.LoadInt( "win_team_score_rate", 1 );

	m_dwRoomExitTime = rkLoader.LoadInt( "exit_room_time", 5000 );

	m_dwCharLimitCheckTime = rkLoader.LoadInt( "char_limit_check_min", 60000 );

	m_dwEtcItemCheckTime = rkLoader.LoadInt( "etcitem_check_time", 60000 );

	// 라운드만큼 버퍼 생성.
	m_vRoundHistory.clear();
	for(int i = 0; i < m_iMaxRound;i++)
	{
		RoundHistory rh;
		m_vRoundHistory.push_back( rh );
	}

	rkLoader.SetTitle( "record" );
	m_iKillDeathPoint = rkLoader.LoadInt( "kill_death_point", 10 );
	m_iWinPoint		  = rkLoader.LoadInt( "win_point", 30 );
	m_iDrawPoint	  = rkLoader.LoadInt( "draw_point", 20 );
	m_iLosePoint	  = rkLoader.LoadInt( "lose_point", 10 );

	rkLoader.SetTitle( "running_man" );
	m_dwMaxRunningManDeco = rkLoader.LoadInt( "max_runningman_deco", 0 );

	m_PlazaTeamList.clear();
	for( int i=TEAM_PRIVATE_1;i < TEAM_PRIVATE_32+1;i++)
	{
		m_PlazaTeamList.push_back( i );
	}
	SetStartPosArray();

	LoadMonsterSpawnFactor( rkLoader );

	//--------------------------------
	// 룰렛
	//--------------------------------

	// 초기화.
	m_EventRoulette.Init();

	// ini load
	rkLoader.SetTitle( "ROULETTE_EVENT" );
	bool bActive = rkLoader.LoadBool( "ACTIVE", false );
	if( bActive == false )
		return;

	m_EventRoulette.SetCoinCount( rkLoader.LoadInt( "USE_COIN_COUNT", 0 ) );
	m_EventRoulette.SetBoardingTime( rkLoader.LoadInt( "ROULETTE_BOARDING_TIME", 0 ) );
	m_EventRoulette.SetSpinTime( rkLoader.LoadInt( "ROULETTE_SPIN_TIME", 0 ) );

	int max_group = rkLoader.LoadInt( "USER_GROUP_MAX_COUNT", 0 );

	// Declare)
	GROUPRANGE	stRange;
	ANGLEDATA	stAngleData;
	std::vector< ANGLEDATA > vecAngle;

	for( int i = 0 ; i < max_group ; ++i )
	{
		stRange.Init();
		
		char szKeyName[MAX_PATH]="";
		StringCbPrintf( szKeyName, sizeof( szKeyName ), "GROUP%d_ANGLE_COUNT", i+1 );
		int angle_count = rkLoader.LoadInt( szKeyName, 0 );

		char szKeyName1[MAX_PATH]="";
		StringCbPrintf( szKeyName1, sizeof( szKeyName1 ), "USER_GROUP%d_RANGE_MIN", i+1 );
		stRange.m_min = rkLoader.LoadInt( szKeyName1, 0 );

		char szKeyName2[MAX_PATH]="";
		StringCbPrintf( szKeyName2, sizeof( szKeyName2 ), "USER_GROUP%d_RANGE_MAX", i+1 );
		stRange.m_max = rkLoader.LoadInt( szKeyName2, 0 );

		// GroupRange : push_back
		m_EventRoulette.InsertGroupRange( stRange );

		vecAngle.clear();

		int min = 0;
		float realpercent = 0;
		int showpercent = 0;

		for( int j = 0 ; j < angle_count ; ++j )
		{
			stAngleData.Init();

			// 실제 당첨 확률 % 입력
			char szKeyNamePercent[MAX_PATH]="";
			StringCbPrintf( szKeyNamePercent, sizeof( szKeyNamePercent ), "GROUP%d_ANGLE%d_REAL_PERCENT", i+1, j+1 );
			float percent = rkLoader.LoadFloat( szKeyNamePercent, 0.0f );
			realpercent += percent * 100;

			stAngleData.m_maxRealPercent = realpercent;

			// 보여지는 앵글 %
			char szKeyNameShow[MAX_PATH]="";
			StringCbPrintf( szKeyNameShow, sizeof( szKeyNameShow ), "GROUP%d_ANGLE%d_SHOW_PERCENT", i+1, j+1 );
			int value = rkLoader.LoadInt( szKeyNameShow, 0 );
			showpercent += value;

			stAngleData.m_angle_min = min;
			stAngleData.m_angle_max = static_cast< int >( ( 360.0f *  showpercent ) / 100 );
			min = stAngleData.m_angle_max + 1;
			
			// 마지막 일때 max 체크
			if( j + 1 == angle_count )
			{
				if( stAngleData.m_maxRealPercent != 10000 )
				{
					LOG.PrintTimeAndLog( 0, "\n ERROR: [ROULETTE_EVENT] - ini percent : %d \n", stAngleData.m_maxRealPercent );
					stAngleData.m_maxRealPercent = 10000;
				}

				if( stAngleData.m_angle_max != 360 )
				{
					LOG.PrintTimeAndLog( 0, "\n ERROR: [ROULETTE_EVENT] - ini angle error... group : %d, per : %d, angle_max : %d \n", i, realpercent, stAngleData.m_angle_max );
					stAngleData.m_angle_max = 360;
				}
			}
			
			char szPresentPeriod[MAX_PATH]="";
			StringCbPrintf( szPresentPeriod, sizeof( szPresentPeriod ), "GROUP%d_ANGLE%d_PRESENT_PERIOD", i+1, j+1 );
			stAngleData.m_angle_PresentPeriod = rkLoader.LoadInt( szPresentPeriod, 0 );

			char szPresentType[MAX_PATH]="";
			StringCbPrintf( szPresentType, sizeof( szPresentType ), "GROUP%d_ANGLE%d_PRESENT_TYPE", i+1, j+1 );
			stAngleData.m_angle_PresentType = rkLoader.LoadInt( szPresentType, 0 );

			char szPresentMent[MAX_PATH]="";
			StringCbPrintf( szPresentMent, sizeof( szPresentMent ), "GROUP%d_ANGLE%d_PRESENT_MENT", i+1, j+1 );
			stAngleData.m_angle_PresentMent = rkLoader.LoadInt( szPresentMent, 0 );

			char szPresentState[MAX_PATH]="";
			StringCbPrintf( szPresentState, sizeof( szPresentState ), "GROUP%d_ANGLE%d_PRESENT_STATE", i+1, j+1 );
			stAngleData.m_angle_PresentState = rkLoader.LoadInt( szPresentState, 0 );

			char szPresentValue1[MAX_PATH]="";
			StringCbPrintf( szPresentValue1, sizeof( szPresentValue1 ), "GROUP%d_ANGLE%d_PRESENT_VALUE1", i+1, j+1 );
			stAngleData.m_angle_PresentValue1 = rkLoader.LoadInt( szPresentValue1, 0 );

			char szPresentValue2[MAX_PATH]="";
			StringCbPrintf( szPresentValue2, sizeof( szPresentValue2 ), "GROUP%d_ANGLE%d_PRESENT_VALUE2", i+1, j+1 );
			stAngleData.m_angle_PresentValue2 = rkLoader.LoadInt( szPresentValue2, 0 );

			vecAngle.push_back( stAngleData );
		}

		// GroupAngle : push_back
		if( ! vecAngle.empty() )
			m_EventRoulette.InsertAngleData( vecAngle );
	}
}



void TrainingMode::SetStartPosArray()
{
	int iTeamPosCnt = m_PlazaTeamList.size();
	if(iTeamPosCnt <= 1) // 최소한 2개 필요
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Error 2 - TrainingMode::GetStartPosArray");
		return;
	}

	m_PlazaTeamPosArray.reserve(iTeamPosCnt);
	for( int i=0; i<iTeamPosCnt; i++ )
	{
		m_PlazaTeamPosArray.push_back(i);
	}

	io_random_shuffle( m_PlazaTeamPosArray.begin(), m_PlazaTeamPosArray.end() );

	m_iBluePosArray = m_PlazaTeamPosArray[0];
	m_iRedPosArray  = m_PlazaTeamPosArray[1];
}

void TrainingMode::DestroyMode()
{
	Mode::DestroyMode();

	m_vRecordList.clear();
}

void TrainingMode::AddNewRecord( User *pUser )
{
	TrainingRecord kRecord;
	kRecord.pUser = pUser;
	m_vRecordList.push_back( kRecord );

	RemoveTeamType( pUser->GetTeam() );
	UpdateUserRank();

	PlayMonsterSync( &kRecord );
}

void TrainingMode::RemoveRecord( User *pUser, bool bRoomDestroy )
{
	if(!pUser) return;

	CheckCreateCrown( pUser );
	CheckResetSymbol( pUser );

	// 방에서 나갈때 룰렛체크.
	if( m_EventRoulette.RemoveUser( pUser ) == TRUE )
	{
		// 유저가 나가면서 룰렛 참여 인원이 없다면 룰렛 종료.
		SP2Packet kPacket( STPK_ROULETTE_END );
		kPacket << true;	// 강제 종료.
		SendRoomAllUser( kPacket );
	}
	
	int iCharCnt = m_vRecordList.size();
	for( int i=0 ; i<iCharCnt ; i++ )
	{
		if( m_vRecordList[i].pUser == pUser )
		{
			SetModeEndDBLog( &m_vRecordList[i], iCharCnt, LogDBClient::PRT_EXIT_ROOM );
			m_vRecordList.erase( m_vRecordList.begin() + i );

			//if( m_pCreator->GetPlazaModeType() == PT_GUILD )
			//{
			//	if( !IsGuildUser( pUser->GetGuildIndex() ) )     //길드 유저가 없을 때만 팀 타입을 반환한다. 
			//		AddTeamType( pUser->GetTeam() );
			//}
			//else
			AddTeamType( pUser->GetTeam() );			
			break;
		}
	}

	UpdateUserRank();
	
	if( pUser )
	{
		m_KickOutVote.RemoveVoteUserList( pUser->GetPublicID() );
		RemoveRecordChangeMonsterSync( pUser->GetPublicID() );
	}
}

void TrainingMode::AddTeamType( TeamType eTeam )
{
	if( COMPARE( eTeam, TEAM_PRIVATE_1, TEAM_PRIVATE_32+1 ) )
		m_PlazaTeamList.push_back( eTeam ); 
	else
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "TrainingMode::AddTeamType %d - %d", (int)m_pCreator->GetPlazaModeType(), eTeam );
}

void TrainingMode::RemoveTeamType( TeamType eTeam )
{
	int iTeamSize = m_PlazaTeamList.size();
	for(int i = 0;i < iTeamSize;i++)
	{
		if( m_PlazaTeamList[i] == eTeam )
		{
			m_PlazaTeamList.erase( m_PlazaTeamList.begin() + i );
			break;
		}
	}
}

bool TrainingMode::IsGuildUser( DWORD dwGuildIndex )
{
	int iCharCnt = m_vRecordList.size();
	for( int i=0 ; i<iCharCnt ; i++ )
	{
		if( !m_vRecordList[i].pUser ) continue;
		if( m_vRecordList[i].pUser->GetGuildIndex() == dwGuildIndex )
			return true;
	}
	return false;
}

void TrainingMode::ProcessReady()
{
	DWORD dwCurTime = TIMEGETTIME();
	if( m_dwStateChangeTime + m_dwReadyStateTime >= dwCurTime )
		return;
	// 유저 입장 대기중이면 플레이 상태로 전환하지 않는다.
	if( m_pCreator->IsRoomEnterUserDelay() )
		return;

	SP2Packet kPacket( STPK_ROUND_START );
	kPacket << m_iCurRound;
	SendRoomPlayUser( kPacket );
	SetModeState( MS_PLAY );

	if( m_pCreator->GetPlazaModeType() != PT_BATTLE )
		m_bNpcMode = false;

	if( m_bNpcMode )
		m_dwSpawnTime += dwCurTime;

	m_iReadyBlueUserCnt = max( m_iReadyBlueUserCnt, GetTeamUserCnt( TEAM_BLUE ) );
	m_iReadyRedUserCnt  = max( m_iReadyRedUserCnt, GetTeamUserCnt( TEAM_RED ) );

	CheckUserLeaveEnd();
}


void TrainingMode::ProcessPlay()
{
	ProcessRevival();
	CheckFieldItemLiveTime();
	CheckItemSupply( m_dwStateChangeTime );	
	CheckBallSupply( m_dwStateChangeTime );
	CheckMachineSupply( m_dwStateChangeTime );
	CheckNeedSendPushStruct();
	CheckCrownExist();
	ProcessPlayEtcItemTime();
	ProcessDiceReward();
	ProcessSpawnNpc();
}

void TrainingMode::ProcessDiceReward()
{
	if( !m_bNpcMode ) return;

	if( m_DiceRewardList.empty() ) return;

	m_vMonsterDiceResult.clear();

	for(int i = 0;i < (int)m_DiceRewardList.size();i++)
	{
		MonsterDieDiceReward &rkDiceReward = m_DiceRewardList[i];
		if( TIMEGETTIME() - rkDiceReward.m_dwRewardTime < m_dwDiceRewardTime ) continue;

		ModeRecord *pPrizeRecord = NULL;        
		DWORD        dwPrizeValue = 0;
		for(int k = 0;k < (int)rkDiceReward.m_vRankUser.size();k++)
		{
			ModeRecord *pRecord = FindModeRecord( rkDiceReward.m_vRankUser[k] );
			if( !pRecord ) continue;
			if( !pRecord->pUser ) continue;

			pPrizeRecord = pRecord;
		}

		if( pPrizeRecord )
		{
			CTimeSpan cPresentGapTime( rkDiceReward.m_iPresentPeriod, 0, 0, 0 );
			CTime kPresentTime = CTime::GetCurrentTime() + cPresentGapTime;

			pPrizeRecord->pUser->AddPresentMemory( g_MainServer.GetSendID(), rkDiceReward.m_iPresentType, 
				rkDiceReward.m_iPresentValue1, rkDiceReward.m_iPresentValue2, 0, 0,
				rkDiceReward.m_iPresentMent, kPresentTime, rkDiceReward.m_iPresentState );
			
			g_LogDBClient.OnInsertPresent( 0, g_MainServer.GetSendID(), g_App.GetPublicIP().c_str(), pPrizeRecord->pUser->GetUserIndex(), rkDiceReward.m_iPresentType,
				rkDiceReward.m_iPresentValue1, rkDiceReward.m_iPresentValue2, 0, 0, LogDBClient::PST_RECIEVE, "DiceReward" );
			pPrizeRecord->pUser->SendPresentMemory();

			MonsterDiceResult stMonsterDiceResult;
			stMonsterDiceResult.szName = pPrizeRecord->pUser->GetPublicID();
			stMonsterDiceResult.nPresentType = rkDiceReward.m_iPresentType;
			stMonsterDiceResult.nPresentValue1 = rkDiceReward.m_iPresentValue1;
			stMonsterDiceResult.nPresentValue2 = rkDiceReward.m_iPresentValue2;

			m_vMonsterDiceResult.push_back( stMonsterDiceResult );
		}
		else
		{
			MonsterDiceResult stMonsterDiceResult;
			stMonsterDiceResult.szName = "";
			stMonsterDiceResult.nPresentType = rkDiceReward.m_iPresentType;
			stMonsterDiceResult.nPresentValue1 = rkDiceReward.m_iPresentValue1;
			stMonsterDiceResult.nPresentValue2 = rkDiceReward.m_iPresentValue2;

			m_vMonsterDiceResult.push_back( stMonsterDiceResult );
		}
	}

	if( !m_vMonsterDiceResult.empty() )
	{
		m_DiceRewardList.clear();

		SP2Packet kPacket( STPK_MONSTER_DICE_RESULT );
		int nSize = (int)m_vMonsterDiceResult.size();
		kPacket << nSize;

		for(int i = 0; i < nSize; i++ )
		{
			kPacket << m_vMonsterDiceResult[i].szName << m_vMonsterDiceResult[i].nPresentType << 
						m_vMonsterDiceResult[i].nPresentValue1 << m_vMonsterDiceResult[i].nPresentValue2;
		}

		SendRoomAllUser( kPacket );
	}
}


void TrainingMode::UpdateRoundRecord()
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
				pRecord->pUser->DeleteExpiredBonusCash();
			}
		}
	}

	UpdateUserRank();	
}

ModeType TrainingMode::GetModeType() const
{
	return MT_TRAINING;
}

void TrainingMode::GetCharModeInfo( SP2Packet &rkPacket, const ioHashString &szName, bool bDieCheck )
{
	TrainingRecord *pRecord = FindTrainingRecord( szName );
	if( pRecord )
	{
		rkPacket << true;

		rkPacket << pRecord->dwRunningManDeco << pRecord->szRunningManName;

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

bool TrainingMode::OnModeChangeChar( User *pSend, int iCharArray, bool bWait, int iSelectCharArray, DWORD dwCharChangeIndex )
{
	if( m_dwMaxRunningManDeco == 0 ) return false;

	TrainingRecord *pRecord = FindTrainingRecord( pSend );
	if( pRecord == NULL )
		return false;

	// 런닝맨 치장 적용
	if( pRecord->dwRunningManDeco == 0 )
	{
		pRecord->dwRunningManDeco = rand() % m_dwMaxRunningManDeco + 1;
		SP2Packet kPacket( STPK_RUNNINGMAN_DECO_SYNC );
		kPacket << pSend->GetPublicID() << pRecord->dwRunningManDeco;
		SendRoomAllUser( kPacket );
	}
	return false;            // 용병 교체는 pSend가 처리하고 여기서는 런닝맨 치장 인덱스만 전송
}

bool TrainingMode::ProcessTCPPacket( User *pSend, SP2Packet &rkPacket )
{
	if( Mode::ProcessTCPPacket( pSend, rkPacket ) )
		return true;

	switch( rkPacket.GetPacketID() )
	{
	case CTPK_SYMBOL_DIE:
		OnSymbolDie( pSend, rkPacket );
		return true;
	case CTPK_RUNNINGMAN_NAME_SYNC:
		OnRunningManNameSync( pSend, rkPacket );
		return true;
	case CTPK_ROULETTE_START:
		OnRouletteStart( pSend );
		return true;
	case CTPK_ROULETTE_JOIN_END:
		OnRouletteJoinEnd( pSend );
		return true;
	case CTPK_ROULETTE_END:
		OnRouletteEnd( pSend );
		return true;
	}

	return false;
}

void TrainingMode::OnRunningManNameSync( User *pUser, SP2Packet &rkPacket )
{
	if( m_dwMaxRunningManDeco == 0 ) return;

	TrainingRecord *pRecord = FindTrainingRecord( pUser );
	if( pRecord == NULL )
		return;

	rkPacket >> pRecord->szRunningManName;

	SP2Packet kPacket( STPK_RUNNINGMAN_NAME_SYNC );
	kPacket << pUser->GetPublicID() << pRecord->szRunningManName;
	SendRoomAllUser( kPacket );
}

//! 룰렛 시작 / 탑승 요청.
void TrainingMode::OnRouletteStart( User* pSend )
{
	// Check : 룰렛 상태
	if( m_EventRoulette.GetState() == TRUE )
	{
		// 탑승가능 ?
		if( m_EventRoulette.SetRouletteJoin( pSend ) == FALSE )
			return;
		
		// Declare)
		std::vector< User* > vecData;
		m_EventRoulette.GetBoardingMember( vecData );

		// BroadCast
		SendBoardingMember( vecData );
	}
	else
	{
		// Check : 코인이 있는지만 체크하자. ( 두번째 파라미터 TRUE )
		if( pSend->UseRouletteCoin( m_EventRoulette.GetUseCoinCount(), TRUE ) )
		{
			//-------------------
			// 룰렛 시작.
			//-------------------
			m_EventRoulette.SetRouletteStart( pSend );

			// BroadCast
			SP2Packet kPacket( STPK_ROULETTE_START );
			kPacket << static_cast< int >( CEventRoulette::ROULETTE_START_SUCCESS );
			kPacket << pSend->GetPublicID();
			SendRoomAllUser( kPacket );
		}
		else
		{
			// 코인이 부족하여 시작 못함.
			SP2Packet kPacket( STPK_ROULETTE_START );
			kPacket << static_cast< int >( CEventRoulette::ROULETTE_START_NOT_ENOUGH_COIN );
			pSend->SendMessage( kPacket );
		}
	}
}

void TrainingMode::OnRouletteJoinEnd( User* pSend )
{
	// Declare) ioHashString
	ioHashString master;

	// Get : Master
	if( m_EventRoulette.GetRouletteMaster( master ) == FALSE )
		return;

	// Check : Master
	if( master != pSend->GetPublicID() )
		return;

	// Get : 돌림판 참여 인원수.
	int iJoinUserCount = m_EventRoulette.GetBoardingMemberCount();

	// 인원수로 angle 선택이 틀려짐.
	int Pos = m_EventRoulette.GetGroupRangePosition( iJoinUserCount );

	// Get : Angle값 구함. 선물코드와 갯수 저장.
	int iAngle = m_EventRoulette.GetNewAngle( Pos );

	// BroadCast
	SP2Packet kPacket( STPK_ROULETTE_JOIN_END );
	kPacket << iAngle;
	SendRoomAllUser( kPacket );
}

void TrainingMode::OnRouletteEnd( User* pSend )
{
	// Check : state
	if( m_EventRoulette.GetState() == FALSE )
		return;

	// Declare) ioHashString
	ioHashString master;
	if( m_EventRoulette.GetRouletteMaster( master ) == FALSE )
		return;

	// Check : Master
	if( master != pSend->GetPublicID() )
		return;

	// 룰렛 종료.
	m_EventRoulette.SetRouletteEnd( pSend );

	// BroadCast
	SP2Packet kPacket( STPK_ROULETTE_END );
	kPacket << false;	// 일반종료
	SendRoomAllUser( kPacket );
}

void TrainingMode::SendBoardingMember( std::vector< User* >& rMember )
{
	// Declare)
	SP2Packet kPacket( STPK_ROULETTE_START );
	kPacket << static_cast< int >( CEventRoulette::ROULETTE_JOIN_SUCCESS );
	kPacket << m_EventRoulette.GetBoardingMemberCount();

	std::vector< User* >::iterator	iter	= rMember.begin();
	std::vector< User* >::iterator	iterEnd	= rMember.end();

	while( iter != iterEnd )
	{
		kPacket << (*iter)->GetPublicID();

		++iter;
	}

	SendRoomAllUser( kPacket );
}

int TrainingMode::GetRecordCnt() const
{
	return m_vRecordList.size();
}

const char* TrainingMode::GetModeINIFileName() const
{
	return "config/trainingmode.ini";
}

void TrainingMode::GetModeInfo( SP2Packet &rkPacket )
{
	rkPacket << GetModeType();

	int iSymbolCnt = m_vSymbolStructList.size();
	rkPacket << iSymbolCnt;
	for( int i=0 ; i<iSymbolCnt ; i++ )
	{
		rkPacket << m_vSymbolStructList[i].m_bActive;
		rkPacket << m_vSymbolStructList[i].m_Team;
		rkPacket << m_vSymbolStructList[i].m_iRevivalCnt;
	}

	int iPosCnt = m_PlazaTeamPosArray.size();
	rkPacket << iPosCnt;

	for( int i=0; i < iPosCnt; ++i )
		rkPacket << m_PlazaTeamPosArray[i];

	// 유저 접속시 보내는 룰렛 데이터.
	m_EventRoulette.ModeInfo( rkPacket );
}

TeamType TrainingMode::GetNextTeamType()
{
	if( m_PlazaTeamList.empty() )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "TrainingMode::GetNextTeamType Not Team : %d", (int)m_vRecordList.size() );
		return TEAM_NONE;
	}

	return (TeamType)m_PlazaTeamList[0];
}

int TrainingMode::GetUserKickVoteLimit( const ioHashString &szKickUserName )
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
	return 0;
}

ModeRecord* TrainingMode::FindModeRecord( const ioHashString &rkName )
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

ModeRecord* TrainingMode::FindModeRecord( User *pUser )
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

ModeRecord* TrainingMode::FindModeRecord( int iIdx )
{
	if( COMPARE( iIdx, 0, GetRecordCnt() ) )
		return &m_vRecordList[iIdx];

	return NULL;
}


TrainingRecord* TrainingMode::FindTrainingRecord( const ioHashString &rkName )
{
	return (TrainingRecord*)FindModeRecord( rkName );
}

TrainingRecord* TrainingMode::FindTrainingRecord( User *pUser )
{
	return (TrainingRecord*)FindModeRecord( pUser );
}

TrainingRecord* TrainingMode::FindTrainingRecordByUserID( const ioHashString &rkUserID )
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

User* TrainingMode::ExistTrainingUser()
{
	User* pUser = NULL;

	TrainingRecordList::iterator	iter	= m_vRecordList.begin();
	TrainingRecordList::iterator	iterEnd	= m_vRecordList.end();

	for( ; iter != iterEnd ; iter++ )
	{
		TrainingRecord& rkRecord = *iter;

		if( rkRecord.pUser == NULL )
			continue;
		if( rkRecord.eState != RS_PLAY )
			continue;

		pUser = rkRecord.pUser;
		break;
	}

	return pUser;
}

bool TrainingMode::IsPlazaMonsterEventAvailable()
{
	bool bResult = false;

	User* pExistUser = ExistTrainingUser();
	if( pExistUser == NULL )
		return bResult;

	EventUserManager& rEventUserManager = pExistUser->GetEventUserMgr();
	PlazaMonsterEventUserNode* pEvent = static_cast< PlazaMonsterEventUserNode* >( rEventUserManager.GetEventUserNode( EVT_PLAZA_MONSTER ) );
	if( pEvent )
	{	
		if( pEvent->IsEventTime( pExistUser ) )
			bResult = true;
	}

	return bResult;
}

int TrainingMode::GetCurTeamUserCnt( TeamType eTeam )
{
	int iCurCount = 0;
	int iRecordCnt = m_vRecordList.size();
	for( int i=0 ; i<iRecordCnt ; i++ )
	{
		if( m_vRecordList[i].pUser->GetTeam() == eTeam )
		{
			iCurCount++;
		}
	}
	return iCurCount;
}

void TrainingMode::UpdateUserDieTime( User *pDier )
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

void TrainingMode::InitObjectGroupList()
{
	LoadNpc();
	LoadSymbolStruct();

	ioINILoader &rkLoader = g_ModeINIMgr.GetINI( GetModeINIFileName() );

	int iSubNum = GetModeSubNum();
	int iGroupNum = GetModeMapNum();
	char szBuf[MAX_PATH], szTitle[MAX_PATH];

	m_vPushStructList.clear();
	m_iPushStructIdx = 0;

	wsprintf( szTitle, "training%d_object_group%d", iSubNum, iGroupNum );
	rkLoader.SetTitle( szTitle );

	int iPushStructCnt = rkLoader.LoadInt( "push_struct_cnt", 0 );
	m_vPushStructList.reserve( iPushStructCnt );

	//Push Struct
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
	vObjectItemList.reserve( iObjectItemCnt );

	//Object Item
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

	LoadWearItem(vItemList);

	if( vItemList.empty() )
		return;

	int iNewItemCnt = vItemList.size();
	SP2Packet kPacket( STPK_FIELD_ITEM_SUPPLY );
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

void TrainingMode::LoadWearItem( ItemVector &rvItemList)
{
	m_bHasCrown = false;
	// ini load
	ioINILoader &rkLoader = g_ModeINIMgr.GetINI( GetModeINIFileName() );

	int iSubNum = GetModeSubNum();
	int iGroupNum = GetModeMapNum();

	char szBuf[MAX_PATH]="", szTitle[MAX_PATH]="";
	wsprintf( szTitle, "training%d_object_group%d", iSubNum, iGroupNum );
	rkLoader.SetTitle( szTitle );
	int iWearItemCnt = rkLoader.LoadInt( "wear_item_cnt", 0 );

	LoadWearPosList(rkLoader);

	for(int i=0; i<iWearItemCnt; i++ )
	{
		memset(szTitle, 0, MAX_PATH);
		memset(szBuf, 0, MAX_PATH);

		ObjectItem kWearItem;
		wsprintf( szTitle, "wear_item%d_name", i+1 );
		rkLoader.LoadString( szTitle, "", szBuf, MAX_PATH );
		
		ioItem *pItem = m_pCreator->CreateItemByName( szBuf );
		if( pItem )
		{
			pItem->SetItemPos( GetRandomWearPos( true ) );
			rvItemList.push_back( pItem );

			if( !m_bHasCrown )
				m_bHasCrown = true;
		}
	}
}

void TrainingMode::LoadWearPosList(ioINILoader &rkLoader )
{
	char szTitle[MAX_PATH] = "";

	int iSubNum = GetModeSubNum();
	int iMapIndex = GetModeMapNum();
	wsprintf( szTitle, "training%d_wear_generate%d", iSubNum, iMapIndex );

	int iItemPosCnt = rkLoader.LoadInt( szTitle, "pos_cnt", 0 );

	m_vWearPosList.clear();
	m_vWearPosList.reserve( iItemPosCnt );

	char szBuf[MAX_PATH]="";
	for( int i=0 ; i<iItemPosCnt ; i++ )
	{
		Vector3 vPos;
		memset(szBuf, 0, MAX_PATH);
		wsprintf( szBuf, "pos%d_x", i+1 );
		vPos.x = rkLoader.LoadFloat( szTitle, szBuf, 0.0f );

		vPos.y = 0.0f;
		
		memset(szBuf, 0, MAX_PATH);
		wsprintf( szBuf, "pos%d_z", i+1 );
		vPos.z = rkLoader.LoadFloat( szTitle, szBuf, 0.0f );
		m_vWearPosList.push_back( vPos );
	}
}

void TrainingMode::LoadSymbolStruct()
{
	ioINILoader &rkLoader = g_ModeINIMgr.GetINI( GetModeINIFileName() );

	int iSubNum = GetModeSubNum();
	int iGroupNum = GetModeMapNum();

	char szBuf[MAX_PATH]="", szTitle[MAX_PATH]="", szTeam[MAX_PATH];
	wsprintf( szTitle, "training%d_symbol_group%d", iSubNum, iGroupNum );
	rkLoader.SetTitle( szTitle );

	int iSymbolCnt = rkLoader.LoadInt( "symbol_cnt", 0 );

	for( int i=0 ; i<iSymbolCnt ; i++ )
	{
		wsprintf( szBuf, "symbol%d_team", i+1 );
		rkLoader.LoadString( szBuf, "NONE", szTeam, MAX_PATH );

		SymbolStruct kSymbol;
		kSymbol.m_OrgTeam = ConvertStringToTeamType( szTeam );
		m_vSymbolStructList.push_back( kSymbol );
	}

	RestoreSymbolList();
	SetReSymbolAcitvity();
}

void TrainingMode::LoadNpc()
{
	ioINILoader &rkLoader = g_ModeINIMgr.GetINI( GetModeINIFileName() );

	int iSubNum = GetModeSubNum();
	int iGroupNum = GetModeMapNum();

	char szBuf[MAX_PATH]="", szTitle[MAX_PATH]="";
	wsprintf( szTitle, "training%d_npc_group%d", iSubNum, iGroupNum );
	rkLoader.SetTitle( szTitle );

	int iNpcCnt = rkLoader.LoadInt( "npc_cnt", 0 );

	m_bNpcMode = false;
	m_nNpcCount = 0;

	if( iNpcCnt > MAX_NPC )
		iNpcCnt = MAX_NPC;

	for( int i = 0; i < iNpcCnt; i++ )
	{
		char szKey[MAX_PATH] = "";

		sprintf_s( szKey, "spawn%d_rand_table", i+1 );
		m_stSpawnBoss[i].nRandTable = rkLoader.LoadInt( szKey, 1 );

		sprintf_s( szKey, "spawn%d_team", i+1 );
		m_stSpawnBoss[i].nTeam = rkLoader.LoadInt( szKey, 1 );

		sprintf_s( szKey, "spawn%d_start_time", i+1 );
		m_stSpawnBoss[i].dwStartTime = rkLoader.LoadInt( szKey, 10000 );

		sprintf_s( szKey, "spawn%d_alive_time", i+1 );
		m_stSpawnBoss[i].dwAliveTime = rkLoader.LoadInt( szKey, 7200000 );  // 디폴트 2시간

		sprintf_s( szKey, "spawn%d_x_pos", i+1 );
		m_stSpawnBoss[i].fXPos = rkLoader.LoadFloat( szKey, 0.f );

		sprintf_s( szKey, "spawn%d_z_pos", i+1 );
		m_stSpawnBoss[i].fZPos = rkLoader.LoadFloat( szKey, 0.f );

		m_stSpawnBoss[i].bSpawn = false;

		if( m_nSpawnRnd != 0 )
			m_bNpcMode = true;
	}

// 	m_stSpawnBoss[NORMAL_NPC].dwAliveTime = 120000;
// 	m_stSpawnBoss[AWAKENING_NPC].dwAliveTime = 120000;

}


Vector3 TrainingMode::GetRandomWearPos( bool bStartRound )
{
	int iMaxWearPos = m_vWearPosList.size();
	if(iMaxWearPos <= 0)
	{
		return Vector3( 0.0f, 0.0f, 0.0f );
	}
	int iTempArray = rand() % iMaxWearPos;

	if( bStartRound )
		return m_vWearPosList[0];

	return m_vWearPosList[iTempArray];
}

Vector3 TrainingMode::GetRandomItemPos(ioItem *pItem)
{
	if(pItem)
	{
		int iEquipSlot = Help::GetEquipSlot( pItem->GetItemCode() );
		if(iEquipSlot == EQUIP_WEAR)
			return GetRandomWearPos( false );
	}

	if( m_vItemCreatePosList.empty() )
		return Vector3( 0.0f, 0.0f, 0.0f );

	Vector3 vPos;
	if( !m_vItemShufflePosList.empty() )
	{
		vPos = m_vItemShufflePosList.front();
		m_vItemShufflePosList.pop_front();
		return vPos;
	}

	m_vItemShufflePosList.clear();
	m_vItemShufflePosList.insert( m_vItemShufflePosList.begin(),
								  m_vItemCreatePosList.begin(),
								  m_vItemCreatePosList.end() );

	io_random_shuffle( m_vItemShufflePosList.begin(),
						 m_vItemShufflePosList.end() );

	vPos = m_vItemShufflePosList.front();
	m_vItemShufflePosList.pop_front();
	return vPos;
}

void TrainingMode::SetReSymbolAcitvity()
{
	// init symbol act
	int iSymbolCnt = m_vSymbolStructList.size();
	for( int i=0 ; i<iSymbolCnt ; i++ )
	{	
		SymbolStruct &rkSymbol = m_vSymbolStructList[i];
		rkSymbol.SetActive( true );
	}

	// send
	SP2Packet kPacket( STPK_SYMBOL_ACTIVITY );
	kPacket << iSymbolCnt;
	for(int i=0 ; i<iSymbolCnt ; i++ )
	{
		kPacket << m_vSymbolStructList[i].m_bActive;
	}

	SendRoomAllUser( kPacket );
}

void TrainingMode::RestoreSymbolList()
{
	int iSymbolCnt = m_vSymbolStructList.size();
	for( int i=0 ; i<iSymbolCnt ; i++ )
	{
		m_vSymbolStructList[i].RestoreTeam();
	}
}

void TrainingMode::CheckCreateCrown(User *pUser)
{
	if(!pUser) return;

	ioItem *pDropItem = pUser->ReleaseItem(EQUIP_WEAR);
	if(!pDropItem) return;
	if( pDropItem->GetCrownItemType() == ioItem::MCT_NONE ) return;
	if(!m_pCreator) return;

	pDropItem->SetItemPos( GetRandomWearPos( true ) );

	SP2Packet kPacket( STPK_FIELD_ITEM_SUPPLY );
	kPacket << 1;

	m_pCreator->AddFieldItem( pDropItem );
	kPacket << pDropItem->GetItemCode();
	kPacket << pDropItem->GetItemReinforce();
	kPacket << pDropItem->GetItemMaleCustom();
	kPacket << pDropItem->GetItemFemaleCustom();
	kPacket << pDropItem->GetGameIndex();
	kPacket << pDropItem->GetItemPos();
	kPacket << pDropItem->GetOwnerName();
	kPacket << "";
	
	SendRoomAllUser( kPacket, pUser );
}

void TrainingMode::OnSymbolDie( User *pUser, SP2Packet &rkPacket )
{
	int iArrayIdx, iRevivalCnt;
	rkPacket >> iArrayIdx >> iRevivalCnt;

	float fRevivalHP;
	rkPacket >> fRevivalHP;

	int iSymbolCnt = m_vSymbolStructList.size();
	if( !COMPARE( iArrayIdx, 0, iSymbolCnt ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "TrainingMode::OnSymbolDie - %s Kill Symbol Overflow(%d/%d)",
								pUser->GetPublicID().c_str(), iArrayIdx, iRevivalCnt );
		return;
	}

	SymbolStruct &rkSymbol = m_vSymbolStructList[iArrayIdx];
	if( rkSymbol.m_iRevivalCnt > iRevivalCnt )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "TrainingMode::OnSymbolDie - %s Kill Symbol Not Equal RevivalCnt",
								pUser->GetPublicID().c_str() );
		return;
	}

	TeamType eKillTeam = pUser->GetTeam();
	TeamType ePreTeam = rkSymbol.m_Team;

	rkSymbol.m_Team = eKillTeam;
	rkSymbol.m_iRevivalCnt++;

	SP2Packet kReturn( STPK_SYMBOL_DIE );
	kReturn << pUser->GetPublicID();
	kReturn << iArrayIdx;
	kReturn << fRevivalHP;
	kReturn << rkSymbol.m_iRevivalCnt;
	kReturn << eKillTeam;
	SendRoomAllUser( kReturn );
}

void TrainingMode::CheckResetSymbol( User *pUser )
{
	if( !pUser ) return;

	int iSize = m_vSymbolStructList.size();
	if( iSize <= 0 ) return;

	IntVec vResetList;
	for( int i=0; i < iSize; ++i )
	{
		if( pUser->GetTeam() == m_vSymbolStructList[i].m_Team )
		{
			if( GetCurTeamUserCnt( pUser->GetTeam() ) - 1 <= 0 )       //팀원이 모두 나가면 상징물 중립 전환
				vResetList.push_back(i);
		}
	}

	int iCheckCnt = vResetList.size();
	if( iCheckCnt <= 0 ) return;

	SP2Packet kReturn( STPK_SYMBOL_DIE_LIST );
	kReturn << iCheckCnt;

	for( int j=0; j < iCheckCnt; ++j )
	{
		int iArrayIndex = vResetList[j];
		TeamType eKillTeam = TEAM_NONE;
		TeamType ePreTeam = m_vSymbolStructList[iArrayIndex].m_Team;

		m_vSymbolStructList[iArrayIndex].m_Team = eKillTeam;
		m_vSymbolStructList[iArrayIndex].m_iRevivalCnt++;

		kReturn << iArrayIndex;
		kReturn << m_vSymbolStructList[iArrayIndex].m_iRevivalCnt;
		kReturn << eKillTeam;
	}

	SendRoomAllUser( kReturn, pUser );
}

void TrainingMode::CheckCrownExist()
{
	if( m_bHasCrown )
	{
		bool bCrown = false;

		// 왕관 쓴 사람있는지 체크
		int iCharCnt = m_vRecordList.size();
		for( int i=0 ; i<iCharCnt ; i++ )
		{
			if( m_vRecordList[i].pUser )
			{
				const ioItem *pItem = m_vRecordList[i].pUser->GetItem( EQUIP_WEAR );
				if( pItem && pItem->GetCrownItemTypeConst() != ioItem::MCT_NONE )
				{
					bCrown = true;
					break;
				}
			}
		}

		if( bCrown ) return;

		// 왕관이 필드에 있는지 체크
		int iCnt = m_pCreator->GetFieldItemCnt();
		for( int i=0; i < iCnt; i++ )
		{
			ioItem *pField = m_pCreator->GetFieldItem( i );
			if( !pField ) continue;

			if( pField->GetCrownItemType() != ioItem::MCT_NONE )
			{
				bCrown = true;
				break;
			}
		}

		if( bCrown ) return;

		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "TrainingMode::CheckCrownExist() - Crown is Not Exist: %d", m_pCreator->GetRoomNumber() );
		m_bHasCrown = false;
	}
}

void TrainingMode::ProcessPlayEtcItemTime()
{
	if( TIMEGETTIME() - m_dwEtcItemCurTime < m_dwEtcItemCheckTime )
		return;
	m_dwEtcItemCurTime = TIMEGETTIME();

	int iRecordCnt = GetRecordCnt();
	for(int i = 0;i < iRecordCnt;i++)
	{
		ModeRecord *pRecord = FindModeRecord( i );
		if( !pRecord ) continue;
		if( pRecord->eState != RS_PLAY ) continue;
		if( !pRecord->pUser ) continue;

		pRecord->pUser->UpdateEtcItemTime( __FUNCTION__ );
		pRecord->pUser->StartEtcItemTime( __FUNCTION__ );
	}
}


void TrainingMode::InstantSpawnNpc()
{
	m_dwSpawnTime = TIMEGETTIME() + 1000;
}

void TrainingMode::InstantKillNpc()
{
	m_dwAliveTime = TIMEGETTIME() + 1000;
}

void TrainingMode::ProcessSpawnNpc()
{
	if( !Help::IsSpawnNpc() )
		return;

	if( !m_bNpcMode )
		return;

	// 이벤트 체크.
	if( m_stSpawnBoss[m_eNpc].bSpawn )	{
		if( m_dwAliveTime < TIMEGETTIME() )
		{
			SP2Packet kPacket( STPK_MONSTER_FORCE_DIE );
			kPacket << 1;
			kPacket << m_stSpawnBoss[m_eNpc].strName;
			SendRoomAllUser( kPacket );
			ResetForceDie();
		}
	}
	else if( IsPlazaMonsterEventAvailable() == true )
	{
		if( m_dwSpawnTime < TIMEGETTIME() )
		{
			m_stSpawnBoss[m_eNpc].bSpawn = true;

			m_vecMonsterRec.clear();

			if(g_MonsterMapLoadMgr.GetRandMonster( m_stSpawnBoss[m_eNpc].nRandTable, m_vecMonsterRec, m_nNpcCount, 1, false, false))
			{
				m_vecMonsterRec[m_vecMonsterRec.size()-1].eTeam = (TeamType)m_stSpawnBoss[m_eNpc].nTeam;
				m_vecMonsterRec[m_vecMonsterRec.size()-1].nGrowthLvl = 100;
			}

			if( !m_vecMonsterRec.empty())
			{
				m_dwAliveTime = m_dwSpawnTime + m_stSpawnBoss[m_eNpc].dwAliveTime;

				SP2Packet kPacket( STPK_SPAWN_MONSTER );
				kPacket << (int)m_vecMonsterRec.size();

				for(int i = 0; i < (int)m_vecMonsterRec.size(); i++)
				{
					MonsterRecord &rkMonster = m_vecMonsterRec[i];

					rkMonster.eState         = RS_PLAY;
					rkMonster.szSyncUser     = SearchMonsterSyncUser();

					int iRandIndex = 0;
					m_IORandom.Randomize();
					int rndX = m_IORandom.Random(0, 128);
					int rndZ = m_IORandom.Random(0, 128);

					if( rndX % 2 == 0)
						rkMonster.fStartXPos     = m_stSpawnBoss[m_eNpc].fXPos + rndX;
					else
						rkMonster.fStartXPos     = m_stSpawnBoss[m_eNpc].fXPos - rndX;

					if( rndZ % 2 == 0)
						rkMonster.fStartZPos     = m_stSpawnBoss[m_eNpc].fZPos + rndZ;
					else
						rkMonster.fStartZPos     = m_stSpawnBoss[m_eNpc].fZPos - rndZ;

					m_stSpawnBoss[m_eNpc].strName = rkMonster.szName;   

					bool bAwakening = (m_eNpc == NORMAL_NPC) ? false : true;

#ifdef ANTIHACK
					kPacket << rkMonster.dwNPCIndex << rkMonster.dwCode << rkMonster.szName << rkMonster.szSyncUser << rkMonster.eTeam << 
#else
					kPacket << rkMonster.dwCode << rkMonster.szName << rkMonster.szSyncUser << rkMonster.eTeam << 
#endif

							rkMonster.nGrowthLvl << rkMonster.bGroupBoss << rkMonster.nGroupIdx << 
							rkMonster.dwStartTime << rkMonster.fStartXPos << rkMonster.fStartZPos << (m_dwAliveTime - m_dwSpawnTime) << bAwakening;
				}

				SendRoomAllUser(kPacket);

				m_pCreator->SetSubState(true);
			}
		}
	}
}


const ioHashString &TrainingMode::SearchMonsterSyncUser()
{
	static ioHashString szError = "동기화유저없음";
	if( m_vRecordList.empty() ) 
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "TrainingMode::SearchMonsterSyncUser(%d) : None User Record", m_pCreator->GetRoomIndex() );
		return szError;
	}

	TrainingRecord *pReturnRecord = NULL;
	TrainingRecordList::iterator iter = m_vRecordList.begin();
	for(;iter != m_vRecordList.end();iter++)
	{
		TrainingRecord &rkRecord = *iter;
		if( rkRecord.pUser == NULL ) continue;
		if( !pReturnRecord )
		{
			pReturnRecord = &rkRecord;
			continue;
		}		
		if( rkRecord.eState != RS_PLAY ) continue;
//		if( rkRecord.pUser->GetTeam() != TEAM_BLUE ) continue;        // 관전 유저는 동기화 주체에서 제외시킨다.

		int iPrevPoint = pReturnRecord->pUser->GetPingStep() + pReturnRecord->iMonsterSyncCount;
		int iNextPoint = rkRecord.pUser->GetPingStep() + rkRecord.iMonsterSyncCount;
		if( iPrevPoint > iNextPoint )
			pReturnRecord = &rkRecord;
	}

	if( pReturnRecord == NULL || pReturnRecord->pUser == NULL )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "TrainingMode::SearchMonsterSyncUser(%d) : None Return Record : %d", m_pCreator->GetRoomIndex(), m_vRecordList.size() );
		return szError;
	}

	// 몬스터 한마리 동기화 추가~
	pReturnRecord->iMonsterSyncCount++;
	return pReturnRecord->pUser->GetPublicID();
}



void TrainingMode::PlayMonsterSync( TrainingRecord *pSendRecord )
{
	if( m_bRoundSetEnd ) return;
	if( GetState() != MS_PLAY ) return;        // 플레이중 입장한 유저들에게만 동기화 시킨다.
	if( pSendRecord == NULL || pSendRecord->pUser == NULL ) return;

	if( !m_bNpcMode ) return;
	if( !m_stSpawnBoss[m_eNpc].bSpawn ) return;

	SP2Packet kPacket( STPK_MONSTER_INFO_SYNC );

	int iSyncSize = m_vecMonsterRec.size();	
	kPacket << iSyncSize;
	for(int i = 0;i < iSyncSize;i++)
	{
		MonsterRecord &rkMonster = m_vecMonsterRec[i];

		bool bAwakening = (m_eNpc == NORMAL_NPC) ? false : true;


#ifdef ANTIHACK
		kPacket << rkMonster.dwNPCIndex << rkMonster.dwCode << rkMonster.szName << rkMonster.szSyncUser << rkMonster.eTeam
#else
		kPacket << rkMonster.dwCode << rkMonster.szName << rkMonster.szSyncUser << rkMonster.eTeam
#endif
			<< rkMonster.bGroupBoss << rkMonster.nGroupIdx << rkMonster.dwStartTime << 
			rkMonster.fStartXPos << rkMonster.fStartZPos << max(1, (m_dwAliveTime - TIMEGETTIME() + 1000)) << bAwakening;
	}

	pSendRecord->pUser->SendMessage( kPacket ); 
}


void TrainingMode::RemoveRecordChangeMonsterSync( const ioHashString &rkRemoveName )
{
	if( m_bRoundSetEnd ) return;
	if( m_vRecordList.empty() ) return;
	if( rkRemoveName.IsEmpty() ) return;

	if( !m_bNpcMode ) return;
	if( !m_stSpawnBoss[m_eNpc].bSpawn ) return;

	MonsterRecordList vSyncRecord;

	for(int i = 0;i < (int)m_vecMonsterRec.size();i++)
	{
		MonsterRecord &rkMonster = m_vecMonsterRec[i];
		if( rkMonster.eState != RS_PLAY ) continue;
		if( rkMonster.szSyncUser != rkRemoveName ) continue;

		rkMonster.szSyncUser = SearchMonsterSyncUser();
		vSyncRecord.push_back( rkMonster );
	}

	if( vSyncRecord.empty() ) return;

	int iSyncSize = vSyncRecord.size();
	SP2Packet kPacket( STPK_MONSTER_SYNC_CHANGE );
	kPacket << iSyncSize;
	for(int i = 0;i < iSyncSize;i++)
	{
		MonsterRecord &rkMonster = m_vecMonsterRec[i];
		kPacket << rkMonster.szName << rkMonster.szSyncUser;
	}
	SendRoomAllUser( kPacket );
	vSyncRecord.clear();
}


void TrainingMode::OnWeaponDieNpc( const ioHashString &rkDieName, SP2Packet &rkPacket )
{
	if( m_vecMonsterRec.empty() )
		return;

	MonsterRecord *pDieMonster = &m_vecMonsterRec[0];

	if( pDieMonster->eState != RS_PLAY )
		return;

	// 몬스터가 죽은 위치.
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
		pDieMonster->eState = RS_DIE;
		pDieMonster->dwCurDieTime = TIMEGETTIME();

		TrainingRecord *pSyncUserRecord = FindTrainingRecord( pDieMonster->szSyncUser );

		if( pSyncUserRecord )
			pSyncUserRecord->iMonsterSyncCount = max( 0, pSyncUserRecord->iMonsterSyncCount - 1 );
	}

	// 데미지 리스트 처리
	int iDamageCnt;
	ioHashString szBestAttackerName;
	rkPacket >> iDamageCnt;

	int iTotalDamage = 0;
	int iLastDamage = 0;
	int iBestDamage = 0;

	DamageTableList vDamageList;
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
	kReturn << pDieMonster->szName;
	kReturn << szLastAttackerName;
	kReturn << szLastAttackerSkillName;
	kReturn << dwLastAttackerWeaponItemCode;
	kReturn << iLastAttackerTeam;
	kReturn << szBestAttackerName;
	kReturn << fLastRate;
	kReturn << fBestRate;
	GetCharModeInfo( kReturn, pDieMonster->szName );
	GetCharModeInfo( kReturn, szLastAttackerName );
	SendRoomAllUser( kReturn );

 	OnMonsterDieToReward( pDieMonster->szName, vDamageList, szLastAttackerName, pDieMonster->dwDiceTable, pDieMonster->iExpReward, pDieMonster->iPesoReward );

	ResetSpawnNPC();
}

bool TrainingMode::SetAwakeningNPC()
{
	m_IORandom.Randomize();
	int nRnd = m_IORandom.Random(1000);

	if( nRnd > m_nAwakeningRnd )
	{
		m_eNpc = AWAKENING_NPC;
		m_dwSpawnTime = TIMEGETTIME() + 5000;
		return true;
	}
	
	return false;
}



void TrainingMode::OnMonsterDieToReward( const ioHashString &rkMonsterName, DamageTableList &rkDamageTable, const ioHashString &szLastAttacker, DWORD dwDiceTable, int iExpReward, int iPesoReward )
{
	if( rkDamageTable.empty() ) return;

	bool bSendPresent = false;

	int nPresent = 1;
	for(int i = rkDamageTable.size() - 1; i >= 0; i--)
	{
		DamageTable &rkTable = rkDamageTable[i];
		if( rkTable.iDamage == 0 ) continue;

		MonsterDieDiceReward kDiceReward;

		g_MonsterMapLoadMgr.GetMonsterDicePresent( dwDiceTable, MAX_RANK - nPresent, g_MainServer.GetSendID(), kDiceReward.m_iPresentType, kDiceReward.m_iPresentState,
			kDiceReward.m_iPresentMent, kDiceReward.m_iPresentPeriod, kDiceReward.m_iPresentValue1, kDiceReward.m_iPresentValue2 );

		if( kDiceReward.m_iPresentType != 0 )
		{
			kDiceReward.m_vRankUser.push_back( rkTable.szName );

			kDiceReward.m_dwRewardTime = TIMEGETTIME();
			m_DiceRewardList.push_back( kDiceReward );
		}

		if( i < MAX_RANK )
			nPresent++;
	}

	if( !szLastAttacker.IsEmpty() )
	{
		MonsterDieDiceReward kDiceReward;

		g_MonsterMapLoadMgr.GetMonsterDicePresent( dwDiceTable, 0, g_MainServer.GetSendID(), kDiceReward.m_iPresentType, kDiceReward.m_iPresentState,
			kDiceReward.m_iPresentMent, kDiceReward.m_iPresentPeriod, kDiceReward.m_iPresentValue1, kDiceReward.m_iPresentValue2 );

		if( kDiceReward.m_iPresentType != 0 )
		{
			kDiceReward.m_vRankUser.push_back( szLastAttacker );

			kDiceReward.m_dwRewardTime = TIMEGETTIME();
			m_DiceRewardList.push_back( kDiceReward );
		}
	}

	// 경험치 & 페소
	vMonsterDieReward kRewardList;

	_OnMonsterDieToExpPeso( rkDamageTable, kRewardList, iExpReward, iPesoReward );

	if( kRewardList.empty() ) //&& m_DiceRewardList.empty() ) //  kDiceReward.m_iPresentType == 0 )
		return;       // 지급할게 아무것도 없다.

	for(int i = 0; i < GetRecordCnt(); i++)
	{
		ModeRecord *pRecord = FindModeRecord(i);

		if( !pRecord )	continue;
		if( !pRecord->pUser ) continue;

		SP2Packet kPacket( STPK_MONSTER_DIE_REWARD );

		kPacket << false;

		// 주사위 선물 전송
// 		if( kDiceReward.m_iPresentType == 0 )
// 		{
// 			kPacket << false;
// 		}
// 		else
// 		{
// 			kPacket << true << rkMonsterName;
// 			kPacket << kDiceReward.m_iPresentType << kDiceReward.m_iPresentValue1 << kDiceReward.m_iPresentValue2;
// 
// 			int iUserSize = kDiceReward.m_vRankUser.size();
// 			kPacket << iUserSize;
// 			for(int j = 0;j < iUserSize;j++)
// 			{
// 				kPacket << kDiceReward.m_vRankUser[j];
// 			}
// 		}

		// 페소 & 경험치 전송
		int iRewardSize = kRewardList.size();
		kPacket << iRewardSize;
		for(int k = 0;k < iRewardSize;k++)
		{
			MonsterDieReward &rkRewardUser = kRewardList[k];
			if( rkRewardUser.pUser )
			{
				kPacket << rkRewardUser.pUser->GetPublicID() << rkRewardUser.iRewardExp << rkRewardUser.iRewardPeso;
				// 자신의 정보는 디테일하게 전송
				if( rkRewardUser.pUser == pRecord->pUser )
				{
					// 
					kPacket << pRecord->pUser->GetMoney() << pRecord->pUser->GetGradeLevel() << pRecord->pUser->GetGradeExpert();

					// 용병들
					int iMaxClassExp = min( (int)rkRewardUser.vRewardClassType.size(), (int)rkRewardUser.vRewardClassPoint.size() );
					kPacket << iMaxClassExp;
					for(int c = 0;c < iMaxClassExp;c++)
					{
						kPacket << rkRewardUser.vRewardClassType[c] << rkRewardUser.vRewardClassPoint[c];
					}
				}
			}
			else
			{
				kPacket << "" << 0 << 0;
			}
		}

		pRecord->pUser->SendMessage( kPacket );
	}	
}



void TrainingMode::OnDropDieNpc( const ioHashString &rkDieName, SP2Packet &rkPacket )
{
	if( m_vecMonsterRec.empty() )
		return;

	MonsterRecord *pDieMonster = &m_vecMonsterRec[0];

	if( pDieMonster->eState != RS_PLAY )
		return;

	// 몬스터가 죽은 위치.
	float fDiePosX, fDiePosZ;
	rkPacket >> fDiePosX >> fDiePosZ;

	int iLastAttackerTeam;
	ioHashString szLastAttackerName;
	ioHashString szLastAttackerSkillName;
	DWORD dwLastAttackerWeaponItemCode;
	rkPacket >> szLastAttackerName >> szLastAttackerSkillName >> dwLastAttackerWeaponItemCode >> iLastAttackerTeam;

	// 몬스터 죽음 처리
	{
		pDieMonster->eState = RS_DIE;
		pDieMonster->dwCurDieTime = TIMEGETTIME();

		TrainingRecord *pSyncUserRecord = FindTrainingRecord( pDieMonster->szSyncUser );

		if( pSyncUserRecord )
			pSyncUserRecord->iMonsterSyncCount = max( 0, pSyncUserRecord->iMonsterSyncCount - 1 );
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
		LOG.PrintTimeAndLog(LOG_DEBUG_LEVEL, "CTPK_DROP_DIE(Champs Mode)Error - DamageCnt:%d  LastAttacker:%s", iDamageCnt, szLastAttackerName.c_str());
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

// 	if( GetState() == Mode::MS_PLAY )
// 	{
// 		UpdateMonsterDieRecord( szLastAttackerName, szBestAttackerName );
// 	}	

	float fLastRate = 0.0f;
	float fBestRate = 0.0f;

	if( iTotalDamage > 0 )
	{
		fLastRate = (float)iLastDamage / iTotalDamage;
		fBestRate = (float)iBestDamage / iTotalDamage;
	}

	SP2Packet kReturn( STPK_DROP_DIE );
	kReturn << pDieMonster->szName;
	kReturn << szLastAttackerName;
	kReturn << szLastAttackerSkillName;
	kReturn << dwLastAttackerWeaponItemCode;
	kReturn << iLastAttackerTeam;
	kReturn << szBestAttackerName;
	kReturn << fLastRate;
	kReturn << fBestRate;
	GetCharModeInfo( kReturn, pDieMonster->szName );
	GetCharModeInfo( kReturn, szLastAttackerName );
	SendRoomAllUser( kReturn );

	// 아이템 드랍
// 	OnMonsterDieToItemDrop( fDiePosX, fDiePosZ, pDieMonster );	
// 
// 	// 보상 아이템 드랍
// 	OnMonsterDieToRewardItemDrop( fDiePosX, fDiePosZ, vDamageList, pDieMonster );
// 
// 	// 선물 지급
// 	OnMonsterDieToPresent( pDieMonster->dwPresentCode );
// 
// 	// 경험치 & 페소 & 주사위 선물 지급
 	OnMonsterDieToReward( pDieMonster->szName, vDamageList, szLastAttackerName, pDieMonster->dwDiceTable, pDieMonster->iExpReward, pDieMonster->iPesoReward );
// 
// 	// 몬스터 죽음 타입 처리
// 	OnMonsterDieToTypeProcess( pDieMonster->dwDieType );
// 
// 	m_szLastKillerName = szLastAttackerName;

	ResetSpawnNPC();
}

void TrainingMode::ResetSpawnNPC()
{
	m_stSpawnBoss[m_eNpc].bSpawn = false;
	m_stSpawnBoss[m_eNpc].strName = "";

	m_pCreator->SetSubState(false);

	if( m_eNpc == NORMAL_NPC )	
	{
		if( !SetAwakeningNPC() )
		{
			m_IORandom.Randomize();
			m_dwSpawnTime = m_IORandom.Random( m_dwSpawnMin, m_dwSpawnMax ) + TIMEGETTIME();
		}
	}
	else if( m_eNpc == AWAKENING_NPC )
	{
		m_eNpc = NORMAL_NPC;

		m_IORandom.Randomize();
		m_dwSpawnTime = m_IORandom.Random( m_dwSpawnMin, m_dwSpawnMax ) + TIMEGETTIME();
	}
}

void TrainingMode::ResetForceDie()
{
	m_stSpawnBoss[m_eNpc].bSpawn = false;
	m_stSpawnBoss[m_eNpc].strName = "";
	m_eNpc = NORMAL_NPC;
	m_IORandom.Randomize();
	m_dwSpawnTime = m_IORandom.Random( m_dwSpawnMin, m_dwSpawnMax ) + TIMEGETTIME();
	m_pCreator->SetSubState(false);
}


void TrainingMode::_OnMonsterDieToExpPeso( DamageTableList &rkDamageTable, vMonsterDieReward &rkDieRewardList, int iExpReward, int iPesoReward )
{
	if( iExpReward == 0 && iPesoReward == 0 )
		return;

	float fTotalConsecutivelyRate = GetTotalModeConsecutivelyRate();
	enum { MAX_GET_POINT_CHAR = 5, };
	for(int i = 0;i < (int)rkDamageTable.size();i++)
	{
		DamageTable &rkTable = rkDamageTable[i];

		ModeRecord *pRecord = FindModeRecord( rkTable.szName );
		if( !pRecord )	continue;
		if( !pRecord->pUser ) continue;
		if( rkTable.iDamage == 0 ) continue;

		float fExpPoint = (float)iExpReward; 
		float fPesoPoint= (float)iPesoReward;

		MonsterDieReward kRewardUser;
		kRewardUser.pUser = pRecord->pUser;

		//인원 보정 C
		float fUserCorrection = 1.0f;
		//플레이 시간 보정값 D
		float fPlayTimeCorrection = 1.0f;
		//페소보정값 E
		float fPesoCorrection = m_fPesoCorrection;
		//경험치 보정값 F
		float fExpCorrection  = m_fExpCorrection;
		//차단 G
		float fBlockPoint = pRecord->pUser->GetBlockPointPer();
		//기여도 H
		float fContributePer = 1.0f;
		//길드보너스 I
		float fGuildBonus = 0.0f;
		//용병 보너스 J
		float fSoldierCntBonus = Help::GetSoldierPossessionBonus( pRecord->pUser->GetActiveCharCount() );
		//PC방 보너스 K
		float fPCRoomBonusExp = 0.0f;
		float fPCRoomBonusPeso= 0.0f;
		if( pRecord->pUser->IsPCRoomAuthority() )
		{
			if( g_EventMgr.IsAlive( EVT_PCROOM_BONUS, pRecord->pUser->GetChannelingType() ) )
			{
				fPCRoomBonusExp = ( (float)g_EventMgr.GetValue( EVT_PCROOM_BONUS, EA_PCROOM_BONUS ) / 100.0f );  // 40 - > 0.40f
				fPCRoomBonusPeso= ( (float)g_EventMgr.GetValue( EVT_PCROOM_BONUS, EA_PCROOM_BONUS ) / 100.0f );  // 40 - > 0.40f
			}
			else
			{
				fPCRoomBonusExp  = Help::GetPCRoomBonusExp();
				fPCRoomBonusPeso = Help::GetPCRoomBonusPeso();
			}
		}
		//모드 보너스 L
		
		float tempModeBonus = 0.0f;
		{
			// EVT_MODE_BONUS (1)
			EventUserManager &rEventUserManager = pRecord->pUser->GetEventUserMgr();
			ModeBonusEventUserNode* pEvent1 = static_cast< ModeBonusEventUserNode* >( rEventUserManager.GetEventUserNode( EVT_MODE_BONUS ) );
			if( pEvent1 )
			{
				if( pEvent1->IsEventMode( GetModeType() ) )
					tempModeBonus = pEvent1->GetEventPer( fPCRoomBonusExp, pRecord->pUser );
			}

			// EVT_MODE_BONUS2 (2)
			ModeBonusEventUserNode* pEvent2 = static_cast< ModeBonusEventUserNode* >( rEventUserManager.GetEventUserNode( EVT_MODE_BONUS2 ) );
			if( pEvent2 )
			{
				if( pEvent2->IsEventMode( GetModeType() ) )
					tempModeBonus += pEvent2->GetEventPer( fPCRoomBonusExp, pRecord->pUser );
			}
		
			if( tempModeBonus == 0.0f )
			{
				tempModeBonus = m_fPlayModeBonus;
			}
		}

		float fModeBonus = tempModeBonus;

		//친구 보너스 M
		float fFriendBonusPer = 0.0f;		
		if( pRecord->pUser->IsPCRoomAuthority() )
		{		
			fFriendBonusPer = min( GetPcRoomMaxFriendBonus(), GetPcRoomFriendBonus() * (float)GetSameFriendUserCnt( pRecord->pUser ) );
		}
		else
		{
			fFriendBonusPer = min( GetMaxFriendBonus(), GetFriendBonus() * (float)GetSameFriendUserCnt( pRecord->pUser ) );
		}

		// 이벤트 경험치 보너스 N
		float fEventBonus = 0.0f;
		EventUserManager &rEventUserManager = pRecord->pUser->GetEventUserMgr();
		ExpEventUserNode *pExpEventNode = static_cast<ExpEventUserNode*> ( rEventUserManager.GetEventUserNode( EVT_EXP ) );
		if( pExpEventNode )
		{
			fEventBonus = pExpEventNode->GetEventPer( fPCRoomBonusExp, pRecord->pUser );
		}

		// second event : evt_exp2
		ExpEventUserNode* pExp2 = static_cast< ExpEventUserNode* >( rEventUserManager.GetEventUserNode( EVT_EXP2 ) );
		if( pExp2 )
		{
			fEventBonus += pExp2->GetEventPer( fPCRoomBonusExp, pRecord->pUser );
		}

		// 이벤트 페소 보너스 O
		float fPesoEventBonus = 0.0f;
		PesoEventUserNode *pPesoEventNode = static_cast<PesoEventUserNode*> ( rEventUserManager.GetEventUserNode( EVT_PESO ) );
		if( pPesoEventNode )
		{
			fPesoEventBonus = pPesoEventNode->GetPesoPer( fPCRoomBonusPeso, pRecord->pUser );
		}

		// second event : evt_peso2
		PesoEventUserNode* pPeso2 = static_cast< PesoEventUserNode* >( rEventUserManager.GetEventUserNode( EVT_PES02 ) );
		if( pPeso2 )
		{
			fPesoEventBonus += pPeso2->GetPesoPer( fPCRoomBonusPeso, pRecord->pUser );
		}

		// 권한 아이템 보너스 P
		float fEtcItemBonus = 0.0f;
		float fEtcItemPesoBonus = 0.0f;
		float fEtcItemExpBonus  = 0.0f;
		ioUserEtcItem *pUserEtcItem = pRecord->pUser->GetUserEtcItem();
		if( pUserEtcItem )
		{
			ioEtcItem *pItemItem = g_EtcItemMgr.FindEtcItem( ioEtcItem::EIT_ETC_PESO_EXP_BONUS );
			ioUserEtcItem::ETCITEMSLOT kSlot;
			if( pUserEtcItem->GetEtcItem( ioEtcItem::EIT_ETC_PESO_EXP_BONUS, kSlot) && pItemItem )
			{
				fEtcItemBonus = ( (float) pItemItem->GetUseValue() / 100.0f ); // 20 -> 0.20
			}

			pItemItem = g_EtcItemMgr.FindEtcItem( ioEtcItem::EIT_ETC_PESO_BONUS );
			if( pUserEtcItem->GetEtcItem( ioEtcItem::EIT_ETC_PESO_BONUS, kSlot ) && pItemItem )
			{
				fEtcItemPesoBonus = ( (float) pItemItem->GetUseValue() / 100.0f ); // 20 -> 0.20
			}

			pItemItem = g_EtcItemMgr.FindEtcItem( ioEtcItem::EIT_ETC_EXP_BONUS );
			if( pUserEtcItem->GetEtcItem( ioEtcItem::EIT_ETC_EXP_BONUS, kSlot ) && pItemItem )
			{
				fEtcItemExpBonus = ( (float) pItemItem->GetUseValue() / 100.0f ); // 20 -> 0.20
			}
		}
		// 연속 모드 보너스 T
		float fModeConsecutivelyBonus = (1.0f + pRecord->pUser->GetModeConsecutivelyBonus()) * fTotalConsecutivelyRate;

		//획득 경험치
		float fExpPlusValue     = max( 0.0f, fContributePer + fGuildBonus + fSoldierCntBonus + fPCRoomBonusExp + fModeBonus + fFriendBonusPer + fEventBonus + fEtcItemBonus + fEtcItemExpBonus );
		float fExpTotalMultiply = fUserCorrection * fPlayTimeCorrection * fBlockPoint * fExpPlusValue;
		fExpPoint = fExpPoint * fExpCorrection * fExpTotalMultiply * fModeConsecutivelyBonus;

		//획득 페소
		float fAcquirePeso       = 0.0f;
		float fPesoPlusValue     = max( 0.0f, fContributePer + fGuildBonus + fPCRoomBonusPeso + fModeBonus + fFriendBonusPer + fPesoEventBonus + fEtcItemBonus + fEtcItemPesoBonus );
		float fPesoTotalMultiply = fUserCorrection * fPlayTimeCorrection * fBlockPoint * fPesoPlusValue;
		fPesoPoint = fPesoPoint * fExpCorrection * fPesoTotalMultiply * fModeConsecutivelyBonus;

		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "_OnMonsterDieToExpPeso %s : %d - %d == %.2f - %.2f", pRecord->pUser->GetPublicID().c_str(), iExpReward, iPesoReward, fExpPoint, fPesoPoint );

		// 페소 
		kRewardUser.iRewardPeso = fPesoPoint;
		pRecord->pUser->AddMoney( kRewardUser.iRewardPeso );
		g_LogDBClient.OnInsertGameLogInfo(LogDBClient::GLT_PESO_GAIN, pRecord->pUser, 0, 0, LogDBClient::LET_MODE, LogDBClient::PT_BATTLE, GetModeType(), 0, kRewardUser.iRewardPeso, NULL);

		// 경험치
		DWORDVec dwPlayTimeList;
		DWORD dwTotalTime = pRecord->GetCurrentHighPlayingTime( pRecord->pUser->GetSelectClassType(), MAX_GET_POINT_CHAR, kRewardUser.vRewardClassType, dwPlayTimeList );
		if( dwTotalTime > 0 ) 
		{
			int iListSize = kRewardUser.vRewardClassType.size();
			for(int k = 0; k < MAX_GET_POINT_CHAR; k++)
			{
				if( !COMPARE( k, 0, iListSize ) ) break;

				if( kRewardUser.vRewardClassType[k] == 0 ) continue;

				float fSoldierPer = (float)dwPlayTimeList[k] / dwTotalTime;
				int iCurPoint = ( fExpPoint * fSoldierPer ) + 0.5f;     //반올림

				kRewardUser.vRewardClassPoint.push_back( iCurPoint );
				kRewardUser.iRewardExp += iCurPoint;
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ClassType[%d] PlayTime[%d - %d] - WinPoint[%d]", kRewardUser.vRewardClassType[k], dwPlayTimeList[k], dwTotalTime,
					kRewardUser.vRewardClassPoint[k] );

				// 경험치 지급 및 레벨업 확인
				if( pRecord->pUser->IsClassTypeExerciseStyle( kRewardUser.vRewardClassType[k], EXERCISE_RENTAL ) == false )
					pRecord->pUser->AddClassExp( kRewardUser.vRewardClassType[k], kRewardUser.vRewardClassPoint[k] );
				if( pRecord->pUser->AddGradeExp( kRewardUser.vRewardClassPoint[k] ) )
					kRewardUser.bGradeUP = true;

				if( pRecord->pUser->IsClassTypeExerciseStyle( kRewardUser.vRewardClassType[k], EXERCISE_RENTAL ) )
				{
					// 계급 경험치만 획득하고 용병 경험치는 획득 안됨
					kRewardUser.vRewardClassPoint[k] = 0;
				}
			}	
			// 레벨업 보상
			kRewardUser.iRewardPeso += pRecord->pUser->GradeNClassUPBonus();
		}
		rkDieRewardList.push_back( kRewardUser );
	}
}