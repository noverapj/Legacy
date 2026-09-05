

#include "stdafx.h"
//#include "../Window.h"
#include "../MainProcess.h"

#include "Mode.h"
#include "Room.h"
#include "RoomNodeManager.h"
#include "LadderTeamManager.h"
#include "LevelMatchManager.h"
#include "ShuffleRoomManager.h"
#include "BattleRoomNode.h"
#include "ioEventUserNode.h"
#include "ioEtcItemManager.h"
#include "ioPresentHelper.h"
#include "ioItemInfoManager.h"
#include "ioExerciseCharIndexManager.h"
#include "ModeItemCrator.h"
#include "MissionManager.h"
#include "MatchManager.h"

#include "../EtcHelpFunc.h"
#include "ioMyLevelMgr.h"
#include "KickOutVoteHelp.h"
#include "TournamentManager.h"
#include "ioSpiritManager.h"
#include "../DataBase/LogDBClient.h"
#include "../MainServerNode/MainServerNode.h"
#include <strsafe.h>
#include "../Local/ioLocalParent.h"
#include "../DataBase/DBClient.h"

extern CLog HackLOG;

Mode::Mode( Room *pCreator )
{
	m_pCreator = pCreator;

	m_ModeState = MS_READY;
	m_bRoomInfoLog = false;
	m_iCurRound = 1;
	m_iMaxRound = 4;
	m_iNeedRound = 0;

	if( pCreator )
	{
		m_bTournamentRoom = pCreator->IsTournamentRoom();	
		m_dwTournamentIndex = pCreator->GetTournamentIndex();
	}

	m_iCurRoundType = 0;
	m_iCurRoundTimeType = 0;

	m_iBlueTeamWinCnt = 0;
	m_iRedTeamWinCnt  = 0;
	m_iScoreRate = 1;

	m_iReadyBlueUserCnt = 0;
	m_iReadyRedUserCnt  = 0;

	m_iReadyBlueGuildLevel = 0;
	m_iReadyRedGuildLevel  = 0; 

	m_iBlueKillCount	= 0;
	m_iBlueDeathCount	= 0;
	m_iRedKillCount		= 0;
	m_iRedDeathCount	= 0;

	m_iPushStructIdx = 0;
	m_iBallStructIdx = 0;
	m_iMachineStructIdx = 0;

	m_dwModeStartTime = 0;
	m_dwStateChangeTime = 0;
	m_dwRoundDuration	= 0;
	m_dwReadyStateTime  = 4000;
	m_dwSuddenDeathTime = 60000;
	m_dwResultStateTime = 7000;
	m_dwFinalResultWaitStateTime = 10000;
	m_dwFinalResultStateTime = 13000;
	m_dwTournamentRoomFinalResultAddTime = 10000;
	m_dwTimeClose = 10000;

	m_dwRoundTimeSendTime = 0;

	m_dwCurSuddenDeathDuration = 0;

	m_CurRoundWinTeam = WTT_NONE;
	
	m_iCurItemSupplyIdx = 0;
	m_iCurBallSupplyIdx = 0;

	m_iAbuseMinDamage = 1;
	m_iBluePosArray   = 0;
	m_iRedPosArray    = 0;
	m_iStatPoint = 1;

	m_bStopTime = true;


	m_bRequestDestroy = false;
	m_bRoundSetEnd = false;
	
	m_ModePointRound	= 0;
	m_ModeLadderPointRound = 0;
	m_fScoreCorrection  = 0.0f;
	m_fLadderScoreCorrection = 0.0f;
	m_fPlayModeBonus    = 0.0f;
	m_fPesoCorrection   = 8.0f;
	m_fExpCorrection    = 1.0f;
	m_dwModePointTime   = 0;
	m_dwModePointMaxTime= 0;
	m_dwModeRecordPointTime = 0;

	m_iMaxPlayer        = 0;
	m_iConstMaxPlayer   = MAX_PLAYER;
	m_bAwardStage       = false;
	m_dwAwardingTime	= 0;
	
	m_vRoundHistory.reserve( 10 );

	m_iKillDeathPoint   = 10;
	m_iWinPoint			= 30;
	m_iDrawPoint		= 20;
	m_iLosePoint		= 10;

	m_bCheckContribute = false;
	m_bCheckAwardChoose = false;
	m_bCheckAllRecvDamageList = false;
	m_bCheckSuddenDeathContribute = false;
	m_bZeroHP = false;
	m_dwCharLimitCheckTime = 0;

	m_fModeHeroExpert  = 0.0f;
	m_fModeLadderPoint = 0.0f;
	m_fLadderLevelCorrection = 0.0f;

	m_fBlueReserveLadderPoint = 0.0f;
	m_fRedReserveLadderPoint  = 0.0f;

	m_fLadderGuildTeamOne = 1.0f;
	m_fLadderGuildTeamTwo = 1.0f;
	
	m_fSameCampPenalty = 1.0f;
	
	m_dwAwardRandBonusSeed = 0;

	m_dwSpacialAwardType   = 0;
	m_dwSpacialAwardLimitTime = 0;
	m_iSpacialAwardLimitUserCnt = 0;
	m_iSpacialAwardMaxUserCnt   = 0;
	m_fSpacialAwardCorrection = 0.0f;
	m_dwSpacialAwardRandSeed = 0;
	m_dwSpecialAwardMaxTime = 0;
	m_iSendAwardCount        = 0;

	m_fSuddenDeathBlueCont     = 0.0f;
	m_fSuddenDeathRedCont      = 0.0f;

	m_dwEventCheckTime = 0;
	m_dwBonusAlarmTime = 0;

	m_fFriendBonus = 0.05f;
	m_fMaxFriendBonus = 0.20f;

	m_dwShuffleBonusPointTime = 0;

	// 2020-02-17 by bckim, 낚시 시스템 확장
	m_dwStatePrivateFishingTime = TIMEGETTIME();		
	// End. 2020-02-17 by bckim, 낚시 시스템 확장


	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "Mode::Mode : %d", pCreator->GetRoomIndex() );
}

Mode::~Mode()
{	
	if( m_pCreator )
		LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "Mode::~Mode : %d", m_pCreator->GetRoomIndex() );
}

void Mode::DestroyMode()
{	
	m_vPushStructList.clear();
	m_vRoundHistory.clear();
	m_AwardBonusTable.clear();
	m_AwardRandBonusTable.clear();
	m_HeroTitleBonusMap.clear();
	m_vBallStructList.clear();
	m_vBallSupplyList.clear();
	m_vBallPositionList.clear();
	m_vMachineSupplyList.clear();

	DestoryModeItem();
}

void Mode::InitMode()
{
	DestoryModeItem();

	m_dwCurModeItemIndex = 0;
	m_dwModeStartTime = TIMEGETTIME();
	m_dwShuffleBonusPointTime = TIMEGETTIME() + g_ShuffleRoomManager.GetShuffleBonusPointTime();

	LoadINIValue();

	SetModeState( MS_READY );
}

void Mode::SetRoundType( int iRoundType, int iRoundTimeType )
{
	m_iCurRoundType = iRoundType;
	m_iCurRoundTimeType = iRoundTimeType;
}

void Mode::LoadINIValue()
{
	ioINILoader &rkLoader = g_ModeINIMgr.GetINI( GetModeINIFileName() );

	LoadFriendBonus( rkLoader );
	LoadRoundCtrlValue( rkLoader );
	LoadPointValueList( rkLoader );
	LoadRevivalTimeList( rkLoader );
	LoadItemRandomPosList( rkLoader );
	LoadItemSupplyList( rkLoader );
	LoadBallList();
	LoadMachineList();
	LoadObjectGroupList( rkLoader );
	LoadStatPoint( rkLoader );
	LoadVoteInfo( rkLoader );
	LoadLadderValue( rkLoader );
	LoadAwardValue();
	LoadMapINIValue();
	LoadModeAwardValue( rkLoader );
	LoadHeroTitleValue( rkLoader );
	LoadEtcItemSyncList( rkLoader );
	LoadMonsterRewardRate( rkLoader );
	LoadShuffleInfo( rkLoader );
	LoadGaugeRecorveyInfo( rkLoader );
}

void Mode::LoadFriendBonus( ioINILoader &rkLoader )
{
	rkLoader.SetTitle( "friend_bonus" );

	m_fFriendBonus    = rkLoader.LoadFloat( "bonus", 1.0f );
	m_fMaxFriendBonus = rkLoader.LoadFloat( "max_bonus", 1.0f );
}

void Mode::LoadItemRandomPosList( ioINILoader &rkLoader )
{
	int iSubNum = GetModeSubNum();
	int iGroupNum = GetModeMapNum();

	char szBuf[MAX_PATH], szTitle[MAX_PATH];

	switch( GetModeType() )
	{
	case MT_BOSS:
		wsprintf( szTitle, "boss%d_mapitem_group%d", iSubNum, iGroupNum );
		break;
	case MT_UNDERWEAR:
		wsprintf( szTitle, "underwear%d_mapitem_group%d", iSubNum, iGroupNum );
		break;
	case MT_CBT:
		wsprintf( szTitle, "cbt%d_mapitem_group%d", iSubNum, iGroupNum );
		break;
	case MT_CATCH:
		wsprintf( szTitle, "catch%d_mapitem_group%d", iSubNum, iGroupNum );
		break;
	case MT_CATCH_RUNNINGMAN:
		wsprintf( szTitle, "catch%d_runningman_mapitem_group%d", iSubNum, iGroupNum );
		break;
	case MT_KING:
		wsprintf( szTitle, "hidden%d_mapitem_group%d", iSubNum, iGroupNum );
		break;
	case MT_MONSTER_SURVIVAL:
		wsprintf( szTitle, "monster_survival%d_mapitem_group%d", iSubNum, iGroupNum );
		break;
	case MT_SURVIVAL:
		wsprintf( szTitle, "survival%d_mapitem_group%d", iSubNum, iGroupNum );
		break;
	case MT_FIGHT_CLUB:
		wsprintf( szTitle, "fight%d_mapitem_group%d", iSubNum, iGroupNum );
		break;
	case MT_TOWER_DEFENSE:
		wsprintf( szTitle, "tower%d_mapitem_group%d", iSubNum, iGroupNum );
		break;
	case MT_DARK_XMAS:
		wsprintf( szTitle, "darkxmas%d_mapitem_group%d", iSubNum, iGroupNum );
		break;
	case MT_FIRE_TEMPLE:
		wsprintf( szTitle, "firetemple%d_mapitem_group%d", iSubNum, iGroupNum );
		break;
	case MT_SYMBOL:
		wsprintf( szTitle, "symbol%d_mapitem_group%d", iSubNum, iGroupNum );
		break;
	case MT_TEAM_SURVIVAL:
		wsprintf( szTitle, "team_survival%d_mapitem_group%d", iSubNum, iGroupNum );
		break;
	case MT_TEAM_SURVIVAL_AI:
		wsprintf( szTitle, "team_survivalAI%d_mapitem_group%d", iSubNum, iGroupNum );
		break;
	case MT_TRAINING:
		wsprintf( szTitle, "training%d_mapitem_group%d", iSubNum, iGroupNum );
		break;
	case MT_FOOTBALL:
		wsprintf( szTitle, "football%d_mapitem_group%d", iSubNum, iGroupNum );
		break;
	case MT_HEROMATCH:
		wsprintf( szTitle, "heromatch%d_mapitem_group%d", iSubNum, iGroupNum );
		break;
	case MT_GANGSI:
		wsprintf( szTitle, "gangsi%d_mapitem_group%d", iSubNum, iGroupNum );
		break;
	case MT_DUNGEON_A:
		wsprintf( szTitle, "dungeon_a%d_mapitem_group%d", iSubNum, iGroupNum );
		break;
	case MT_HEADQUARTERS:
		wsprintf( szTitle, "headquarters%d_mapitem_group%d", iSubNum, iGroupNum );
		break;
	case MT_DOBULE_CROWN:
		wsprintf( szTitle, "double_crown%d_mapitem_group%d", iSubNum, iGroupNum );
		break;
	case MT_SHUFFLE_BONUS:
		wsprintf( szTitle, "shuffle%d_mapitem_group%d", iSubNum, iGroupNum );
		break;
	case MT_FACTORY:
		wsprintf( szTitle, "factory%d_mapitem_group%d", iSubNum, iGroupNum );
		break;
	case MT_RAID:
		wsprintf( szTitle, "raid%d_mapitem_group%d", iSubNum, iGroupNum );
		break;
	case MT_SUCCESSION:
		wsprintf( szTitle, "succession%d_mapitem_group%d", iSubNum, iGroupNum );
		break;
#ifdef FARMING_MODE_BY_BCKIM		// 2019-07-03 by bckim, 파밍모드 추가
	case MT_FARMING:
		wsprintf( szTitle, "underwear%d_mapitem_group%d", iSubNum, iGroupNum );
		break;
#endif 	// FARMING_MODE_BY_BCKIM		// 2019-07-03 by bckim, 파밍모드 추가

	}

	rkLoader.SetTitle( szTitle );

	int iItemPosCnt = rkLoader.LoadInt( "pos_cnt", 0 );

	m_vItemCreatePosList.clear();
	m_vItemShufflePosList.clear();
	m_vItemCreatePosList.reserve( iItemPosCnt );

	for( int i=0 ; i<iItemPosCnt ; i++ )
	{
		Vector3 vPos;
		memset(szBuf, 0, MAX_PATH);
		wsprintf( szBuf, "pos%d_x", i+1 );
		vPos.x = rkLoader.LoadFloat( szBuf, 0.0f );

		vPos.y = 0.0f;

		wsprintf( szBuf, "pos%d_z", i+1 );
		vPos.z = rkLoader.LoadFloat( szBuf, 0.0f );
		m_vItemCreatePosList.push_back( vPos );
	}
}

void Mode::LoadItemSupplyList( ioINILoader &rkLoader )
{
	int iSubNum = GetModeSubNum();
	int iGroupNum = GetModeMapNum();
	char szBuf[MAX_PATH], szTitle[MAX_PATH], szTag[MAX_PATH];

	switch( GetModeType() )
	{
	case MT_BOSS:
		wsprintf( szTitle, "boss%d_mapitem_group%d", iSubNum, iGroupNum );
		break;
	case MT_UNDERWEAR:
		wsprintf( szTitle, "underwear%d_mapitem_group%d", iSubNum, iGroupNum );
		break;
	case MT_CBT:
		wsprintf( szTitle, "cbt%d_mapitem_group%d", iSubNum, iGroupNum );
		break;
	case MT_CATCH:
		wsprintf( szTitle, "catch%d_mapitem_group%d", iSubNum, iGroupNum );
		break;
	case MT_CATCH_RUNNINGMAN:
		wsprintf( szTitle, "catch%d_runningman_mapitem_group%d", iSubNum, iGroupNum );
		break;
	case MT_KING:
		wsprintf( szTitle, "hidden%d_mapitem_group%d", iSubNum, iGroupNum );
		break;
	case MT_MONSTER_SURVIVAL:
		wsprintf( szTitle, "monster_survival%d_mapitem_group%d", iSubNum, iGroupNum );
		break;
	case MT_SURVIVAL:
		wsprintf( szTitle, "survival%d_mapitem_group%d", iSubNum, iGroupNum );
		break;
	case MT_FIGHT_CLUB:
		wsprintf( szTitle, "fight%d_mapitem_group%d", iSubNum, iGroupNum );
		break;
	case MT_TOWER_DEFENSE:
		wsprintf( szTitle, "tower%d_mapitem_group%d", iSubNum, iGroupNum );
		break;
	case MT_DARK_XMAS:
		wsprintf( szTitle, "darkxmas%d_mapitem_group%d", iSubNum, iGroupNum );
		break;
	case MT_FIRE_TEMPLE:
		wsprintf( szTitle, "firetemple%d_mapitem_group%d", iSubNum, iGroupNum );
		break;
	case MT_SYMBOL:
		wsprintf( szTitle, "symbol%d_mapitem_group%d", iSubNum, iGroupNum );
		break;
	case MT_TEAM_SURVIVAL:
		wsprintf( szTitle, "team_survival%d_mapitem_group%d", iSubNum, iGroupNum );
		break;
	case MT_TEAM_SURVIVAL_AI:
		wsprintf( szTitle, "team_survivalAI%d_mapitem_group%d", iSubNum, iGroupNum );
		break;
	case MT_TRAINING:
		wsprintf( szTitle, "training%d_mapitem_group%d", iSubNum, iGroupNum );
		break;
	case MT_FOOTBALL:
		wsprintf( szTitle, "football%d_mapitem_group%d", iSubNum, iGroupNum );
		break;
	case MT_HEROMATCH:
		wsprintf( szTitle, "heromatch%d_mapitem_group%d", iSubNum, iGroupNum );
		break;
	case MT_GANGSI:
		wsprintf( szTitle, "gangsi%d_mapitem_group%d", iSubNum, iGroupNum );
		break;
	case MT_DUNGEON_A:
		wsprintf( szTitle, "dungeon_a%d_mapitem_group%d", iSubNum, iGroupNum );
		break;
	case MT_HEADQUARTERS:
		wsprintf( szTitle, "headquarters%d_mapitem_group%d", iSubNum, iGroupNum );
		break;
	case MT_DOBULE_CROWN:
		wsprintf( szTitle, "double_crown%d_mapitem_group%d", iSubNum, iGroupNum );
		break;
	case MT_SHUFFLE_BONUS:
		wsprintf( szTitle, "shuffle%d_mapitem_group%d", iSubNum, iGroupNum );
		break;
	case MT_FACTORY:
		wsprintf( szTitle, "factory%d_mapitem_group%d", iSubNum, iGroupNum );
		break;
	case MT_RAID:
		wsprintf( szTitle, "raid%d_mapitem_group%d", iSubNum, iGroupNum );
		break;
	case MT_SUCCESSION:
		wsprintf( szTitle, "succession%d_mapitem_group%d", iSubNum, iGroupNum );
		break;
#ifdef FARMING_MODE_BY_BCKIM		// 2019-07-03 by bckim, 파밍모드 추가
	case MT_FARMING:
		wsprintf( szTitle, "underwear%d_mapitem_group%d", iSubNum, iGroupNum );
		break;
#endif 	// FARMING_MODE_BY_BCKIM		// 2019-07-03 by bckim, 파밍모드 추가
	}

	rkLoader.SetTitle( szTitle );

	int iTimeCnt = rkLoader.LoadInt( "supply_time_cnt", 0 );

	m_vItemSupplyTimeList.clear();
	m_vItemSupplyTimeList.reserve( iTimeCnt );

	for( int i=0 ; i<iTimeCnt ; i++ )
	{
		SupplyItemTime kItemTime;

		wsprintf( szTag, "supply_time%d", i+1 );
		DWORD dwTime = (DWORD)rkLoader.LoadInt( szTag, 0 );
		kItemTime.m_dwSupplyTime = dwTime;

		wsprintf( szTag, "supply%d_item_cnt", i+1 );
		int iItemCnt = rkLoader.LoadInt( szTag, 0 );

		for( int j=0; j < iItemCnt; j++ )
		{
			wsprintf( szTag, "supply%d_item%d_name", i+1, j+1 );
			rkLoader.LoadString( szTag, "", szBuf, MAX_PATH );

			kItemTime.m_ItemList.push_back( szBuf );
		}

		wsprintf( szTag, "supply%d_pos_cnt", i+1 );
		int iPosCnt = rkLoader.LoadInt( szTag, 0 );

		for( int j=0; j < iPosCnt; j++ )
		{
			Vector3 vPos(0.0f, 0.0f, 0.0f);

			wsprintf( szTag, "supply%d_pos%d_x", i+1, j+1 );
			vPos.x = rkLoader.LoadFloat( szTag, 0.0f );

			wsprintf( szTag, "supply%d_pos%d_z", i+1, j+1 );
			vPos.z = rkLoader.LoadFloat( szTag, 0.0f );

			kItemTime.m_PosList.push_back( vPos );
		}

		m_vItemSupplyTimeList.push_back( kItemTime );
	}
}

void Mode::LoadBallList()
{
	ioINILoader &rkLoader = g_ModeINIMgr.GetINI( GetModeINIFileName() );

	int iSubNum = GetModeSubNum();
	int iGroupNum = GetModeMapNum();
	char szTitle[MAX_PATH];

	m_vBallStructList.clear();
	m_iBallStructIdx = 0;

	m_vBallSupplyList.clear();
	m_vBallPositionList.clear();

	switch( GetModeType() )
	{
	case MT_BOSS:
		wsprintf( szTitle, "boss%d_object_group%d", iSubNum, iGroupNum );
		break;
	case MT_UNDERWEAR:
		wsprintf( szTitle, "underwear%d_object_group%d", iSubNum, iGroupNum );
		break;
	case MT_CBT:
		wsprintf( szTitle, "cbt%d_object_group%d", iSubNum, iGroupNum );
		break;
	case MT_CATCH:
		wsprintf( szTitle, "catch%d_object_group%d", iSubNum, iGroupNum );
		break;
	case MT_CATCH_RUNNINGMAN:
		wsprintf( szTitle, "catch%d_runningman_object_group%d", iSubNum, iGroupNum );
		break;
	case MT_KING:
		wsprintf( szTitle, "hidden%d_object_group%d", iSubNum, iGroupNum );
		break;
	case MT_MONSTER_SURVIVAL:
		wsprintf( szTitle, "monster_survival%d_object_group%d", iSubNum, iGroupNum );
		break;
	case MT_SURVIVAL:
		wsprintf( szTitle, "survival%d_object_group%d", iSubNum, iGroupNum );
		break;
	case MT_FIGHT_CLUB:
		wsprintf( szTitle, "fight%d_object_group%d", iSubNum, iGroupNum );
		break;
	case MT_TOWER_DEFENSE:
		wsprintf( szTitle, "tower%d_object_group%d", iSubNum, iGroupNum );
		break;
	case MT_DARK_XMAS:
		wsprintf( szTitle, "darkxmas%d_object_group%d", iSubNum, iGroupNum );
		break;
	case MT_FIRE_TEMPLE:
		wsprintf( szTitle, "firetemple%d_object_group%d", iSubNum, iGroupNum );
		break;
	case MT_SYMBOL:
		wsprintf( szTitle, "symbol%d_object_group%d", iSubNum, iGroupNum );
		break;
	case MT_TEAM_SURVIVAL:
		wsprintf( szTitle, "team_survival%d_object_group%d", iSubNum, iGroupNum );
		break;
	case MT_TEAM_SURVIVAL_AI:
		wsprintf( szTitle, "team_survival%d_object_group%d", iSubNum, iGroupNum );
		break;
	case MT_TRAINING:
		wsprintf( szTitle, "training%d_object_group%d", iSubNum, iGroupNum );
		break;
	case MT_FOOTBALL:
		wsprintf( szTitle, "football%d_object_group%d", iSubNum, iGroupNum );
		break;
	case MT_HEROMATCH:
		wsprintf( szTitle, "heromatch%d_object_group%d", iSubNum, iGroupNum );
		break;
	case MT_GANGSI:
		wsprintf( szTitle, "gangsi%d_object_group%d", iSubNum, iGroupNum );
		break;
	case MT_DUNGEON_A:
		wsprintf( szTitle, "dungeon_a%d_object_group%d", iSubNum, iGroupNum );
		break;
	case MT_HEADQUARTERS:
		wsprintf( szTitle, "headquarters%d_object_group%d", iSubNum, iGroupNum );
		break;
	case MT_DOBULE_CROWN:
		wsprintf( szTitle, "double_crown%d_object_group%d", iSubNum, iGroupNum );
		break;
	case MT_SHUFFLE_BONUS:
		wsprintf( szTitle, "shuffle%d_object_group%d", iSubNum, iGroupNum );
		break;
	case MT_FACTORY:
		wsprintf( szTitle, "factory%d_object_group%d", iSubNum, iGroupNum );
		break;
	case MT_RAID:
		wsprintf( szTitle, "raid%d_object_group%d", iSubNum, iGroupNum );
		break;
	case MT_SUCCESSION:
		wsprintf( szTitle, "succession%d_object_group%d", iSubNum, iGroupNum );
		break;
#ifdef FARMING_MODE_BY_BCKIM		// 2019-07-03 by bckim, 파밍모드 추가
	case MT_FARMING:
		wsprintf( szTitle, "underwear%d_object_group%d", iSubNum, iGroupNum );
		break;
#endif 	// FARMING_MODE_BY_BCKIM		// 2019-07-03 by bckim, 파밍모드 추가
	}

	rkLoader.SetTitle( szTitle );

	int iBallStructCnt = rkLoader.LoadInt( "ball_struct_cnt", 0 );
	m_vBallSupplyList.reserve( iBallStructCnt );

	for( int i=0; i<iBallStructCnt; i++ )
	{
		BallStruct kBall;

		wsprintf( szTitle, "ball_struct%d_num", i+1 );
		kBall.m_iNum = rkLoader.LoadInt( szTitle, 0 );

		wsprintf( szTitle, "ball_struct%d_range", i+1 );
		kBall.m_fCreateRange = rkLoader.LoadFloat( szTitle, 0.0f );

		wsprintf( szTitle, "ball_struct%d_pos_x", i+1 );
		kBall.m_CreatePos.x = rkLoader.LoadFloat( szTitle, 0.0f );

		wsprintf( szTitle, "ball_struct%d_pos_y", i+1 );
		kBall.m_CreatePos.y = rkLoader.LoadFloat( szTitle, 0.0f );

		wsprintf( szTitle, "ball_struct%d_pos_z", i+1 );
		kBall.m_CreatePos.z = rkLoader.LoadFloat( szTitle, 0.0f );

		wsprintf( szTitle, "ball_struct%d_supplytime", i+1 );
		kBall.m_dwSupplyTime = rkLoader.LoadInt( szTitle, 0 );

		m_vBallSupplyList.push_back( kBall );
	}

	int iBallPositionCnt = rkLoader.LoadInt( "ball_start_position_cnt", 0 );
	m_vBallPositionList.reserve( iBallPositionCnt );
	for( int j=0; j < iBallPositionCnt; ++j )
	{
		Vector3 vPos;
		wsprintf( szTitle, "ball_start%d_pos_x", j+1 );
		vPos.x = rkLoader.LoadFloat( szTitle, 0.0f );

		wsprintf( szTitle, "ball_start%d_pos_y", j+1 );
		vPos.y = rkLoader.LoadFloat( szTitle, 0.0f );

		wsprintf( szTitle, "ball_start%d_pos_z", j+1 );
		vPos.z = rkLoader.LoadFloat( szTitle, 0.0f );

		m_vBallPositionList.push_back( vPos );
	}
}

void Mode::LoadMachineList()
{
	ioINILoader &rkLoader = g_ModeINIMgr.GetINI( GetModeINIFileName() );

	int iSubNum = GetModeSubNum();
	int iGroupNum = GetModeMapNum();
	char szTitle[MAX_PATH];

	m_vMachineStructList.clear();
	m_iMachineStructIdx = 0;

	m_vMachineSupplyList.clear();

	switch( GetModeType() )
	{
	case MT_BOSS:
		wsprintf( szTitle, "boss%d_object_group%d", iSubNum, iGroupNum );
		break;
	case MT_UNDERWEAR:
		wsprintf( szTitle, "underwear%d_object_group%d", iSubNum, iGroupNum );
		break;
	case MT_CBT:
		wsprintf( szTitle, "cbt%d_object_group%d", iSubNum, iGroupNum );
		break;
	case MT_CATCH:
		wsprintf( szTitle, "catch%d_object_group%d", iSubNum, iGroupNum );
		break;
	case MT_CATCH_RUNNINGMAN:
		wsprintf( szTitle, "catch%d_runningman_object_group%d", iSubNum, iGroupNum );
		break;
	case MT_KING:
		wsprintf( szTitle, "hidden%d_object_group%d", iSubNum, iGroupNum );
		break;
	case MT_MONSTER_SURVIVAL:
		wsprintf( szTitle, "monster_survival%d_object_group%d", iSubNum, iGroupNum );
		break;
	case MT_SURVIVAL:
		wsprintf( szTitle, "survival%d_object_group%d", iSubNum, iGroupNum );
		break;
	case MT_FIGHT_CLUB:
		wsprintf( szTitle, "fight%d_object_group%d", iSubNum, iGroupNum );
		break;
	case MT_TOWER_DEFENSE:
		wsprintf( szTitle, "tower%d_object_group%d", iSubNum, iGroupNum );
		break;
	case MT_DARK_XMAS:
		wsprintf( szTitle, "darkxmas%d_object_group%d", iSubNum, iGroupNum );
		break;
	case MT_FIRE_TEMPLE:
		wsprintf( szTitle, "firetemple%d_object_group%d", iSubNum, iGroupNum );
		break;
	case MT_SYMBOL:
		wsprintf( szTitle, "symbol%d_object_group%d", iSubNum, iGroupNum );
		break;
	case MT_TEAM_SURVIVAL:
		wsprintf( szTitle, "team_survival%d_object_group%d", iSubNum, iGroupNum );
		break;
	case MT_TEAM_SURVIVAL_AI:
		wsprintf( szTitle, "team_survival%d_object_group%d", iSubNum, iGroupNum );
		break;
	case MT_TRAINING:
		wsprintf( szTitle, "training%d_object_group%d", iSubNum, iGroupNum );
		break;
	case MT_FOOTBALL:
		wsprintf( szTitle, "football%d_object_group%d", iSubNum, iGroupNum );
		break;
	case MT_HEROMATCH:
		wsprintf( szTitle, "heromatch%d_object_group%d", iSubNum, iGroupNum );
		break;
	case MT_GANGSI:
		wsprintf( szTitle, "gangsi%d_object_group%d", iSubNum, iGroupNum );
		break;
	case MT_DUNGEON_A:
		wsprintf( szTitle, "dungeon_a%d_object_group%d", iSubNum, iGroupNum );
		break;
	case MT_HEADQUARTERS:
		wsprintf( szTitle, "headquarters%d_object_group%d", iSubNum, iGroupNum );
		break;
	case MT_DOBULE_CROWN:
		wsprintf( szTitle, "double_crown%d_object_group%d", iSubNum, iGroupNum );
		break;
	case MT_PRACTICE:
		wsprintf( szTitle, "practice%d_object_group%d", iSubNum, iGroupNum );
		break;
	case MT_SHUFFLE_BONUS:
		wsprintf( szTitle, "shuffle%d_object_group%d", iSubNum, iGroupNum );
		break;
	case MT_FACTORY:
		wsprintf( szTitle, "factory%d_object_group%d", iSubNum, iGroupNum );
		break;
	case MT_RAID:
		wsprintf( szTitle, "raid%d_object_group%d", iSubNum, iGroupNum );
		break;
	case MT_SUCCESSION:
		wsprintf( szTitle, "succession%d_object_group%d", iSubNum, iGroupNum );
		break;
#ifdef FARMING_MODE_BY_BCKIM		// 2019-07-03 by bckim, 파밍모드 추가
	case MT_FARMING:
		wsprintf( szTitle, "underwear%d_object_group%d", iSubNum, iGroupNum );
		break;
#endif 	// FARMING_MODE_BY_BCKIM		// 2019-07-03 by bckim, 파밍모드 추가

	}

	rkLoader.SetTitle( szTitle );

	int iMachineStructCnt = rkLoader.LoadInt( "machine_struct_cnt", 0 );
	m_vMachineSupplyList.reserve( iMachineStructCnt );

	for( int i=0; i<iMachineStructCnt; i++ )
	{
		MachineStruct kMachine;

		wsprintf( szTitle, "machine_struct%d_num", i+1 );
		kMachine.m_iNum = rkLoader.LoadInt( szTitle, 0 );

		wsprintf( szTitle, "machine_struct%d_pos_x", i+1 );
		kMachine.m_CreatePos.x = rkLoader.LoadFloat( szTitle, 0.0f );

		wsprintf( szTitle, "machine_struct%d_pos_y", i+1 );
		kMachine.m_CreatePos.y = rkLoader.LoadFloat( szTitle, 0.0f );

		wsprintf( szTitle, "machine_struct%d_pos_z", i+1 );
		kMachine.m_CreatePos.z = rkLoader.LoadFloat( szTitle, 0.0f );

		wsprintf( szTitle, "machine_struct%d_supplytime", i+1 );
		kMachine.m_dwSupplyTime = rkLoader.LoadInt( szTitle, 0 );

		m_vMachineSupplyList.push_back( kMachine );
	}
}

void Mode::LoadRoundCtrlValue( ioINILoader &rkLoader )
{
	rkLoader.SetTitle( "info" );
	m_iMaxPlayer = min( MAX_PLAYER, rkLoader.LoadInt( "max_player", MAX_PLAYER ) );
	m_iConstMaxPlayer = min( MAX_PLAYER, rkLoader.LoadInt( "const_max_player", MAX_PLAYER ) );

	rkLoader.SetTitle( "round" );

	m_iMaxRound  = rkLoader.LoadInt( "max_round", 4 );
	m_iNeedRound = rkLoader.LoadInt( "win_need_round", 1 );

	m_dwRoundDuration   = rkLoader.LoadInt( "round_time", 300000 );

	//
	switch( GetModeType() )
	{
	case MT_CATCH:
	case MT_CATCH_RUNNINGMAN:
	case MT_UNDERWEAR:
	case MT_CBT:
	case MT_SUCCESSION:
	case MT_FARMING:		// 2019-07-03 by bckim, 파밍모드 추가
		if( m_iCurRoundType != -1 && m_iCurRoundTimeType != -1 )
		{
			char szKey[MAX_PATH] = "";

			wsprintf( szKey, "max_round_type%d", m_iCurRoundType+1 );

			int iCurMaxRound = rkLoader.LoadInt( szKey, 0 );
			if( iCurMaxRound > 0 )
			{
				m_iMaxRound = iCurMaxRound;
				m_iNeedRound = m_iMaxRound / 2 + 1;
			}

			wsprintf( szKey, "round_time_type%d", m_iCurRoundTimeType+1 );

			int iCurRoundTime = rkLoader.LoadInt( szKey, 0 );
			if( iCurRoundTime > 0 )
			{
				m_dwRoundDuration = iCurRoundTime;
			}
		}
		break;
	}

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

	SetStartPosArray();

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

	rkLoader.SetTitle( "pc_room" );
	m_fPesoCorrection = rkLoader.LoadFloat( "pc_room", 8.0f );
}

void Mode::LoadPointValueList( ioINILoader &rkLoader )
{
	rkLoader.SetTitle( "point" );
	m_ModePointRound  = rkLoader.LoadInt( "mode_point_round", 5 );
	m_ModeLadderPointRound = rkLoader.LoadInt( "mode_ladder_point_round", 5 );
	m_dwModePointTime = rkLoader.LoadInt( "mode_point_time", 0xFFFFFFFF );
	m_dwModePointMaxTime = rkLoader.LoadInt( "mode_point_max_time", 900000 );
	m_dwModeRecordPointTime = rkLoader.LoadInt( "mode_record_point_time", 0xFFFFFFFF );
	m_fPlayModeBonus  = rkLoader.LoadFloat( "mode_plus_bonus", 0.0f );
	m_fPesoCorrection = rkLoader.LoadFloat( "PesoCorrection", 8.0f );
	m_fExpCorrection  = rkLoader.LoadFloat( "ExpCorrection", 1.0f );
	m_fScoreCorrection  = rkLoader.LoadFloat( "ScoreCorrection", 0.0f );
	m_fLadderScoreCorrection = rkLoader.LoadFloat( "LadderScoreCorrection", 0.0f );
	m_fPesoCorrection = rkLoader.LoadFloat( "PesoCorrection", 8.0f );
}

void Mode::LoadRevivalTimeList( ioINILoader &rkLoader )
{
	rkLoader.SetTitle( "revival_time" );

	m_dwDefaultRevivalTime  = rkLoader.LoadInt( "default_time", 10000 );
	m_dwIncreaseRevivalTime = rkLoader.LoadInt( "increase_time", 5000 );
	m_dwMaxRevivalTime      = rkLoader.LoadInt( "max_time", 30000 );

	m_dwSelectCharTime      = rkLoader.LoadInt( "select_char_time", 5000 );

	m_dwDropItemLiveTime = rkLoader.LoadInt( "drop_item_live_time", 20000 );
}

void Mode::LoadVoteInfo( ioINILoader &rkLoader )
{
	m_KickOutVote.LoadVoteInfo( this, rkLoader );
}

void Mode::LoadLadderValue( ioINILoader &rkLoader )
{
	rkLoader.SetTitle( "ladder_point" );
	m_fModeHeroExpert  = rkLoader.LoadFloat( "mode_hero_expert", 0.0f );
	m_fModeLadderPoint = rkLoader.LoadFloat( "mode_ladder_point", 0.0f );
	m_fLadderLevelCorrection = rkLoader.LoadFloat( "mode_ladder_level_correction", 0.0f );
	m_fLadderGuildTeamOne = rkLoader.LoadFloat( "mode_ladder_guild_team_one", 2.0f );
	m_fLadderGuildTeamTwo = rkLoader.LoadFloat( "mode_ladder_guild_team_two", 4.0f );
	m_fSameCampPenalty	 = rkLoader.LoadFloat( "mode_same_camp_penalty", 1.0f );

	// 세력비에 따른 진영 보너스
	m_InfluenceBonusList.clear();
	int iMaxInfluence = rkLoader.LoadInt( "max_influence_size", 0 );
	for(int i = 0;i < iMaxInfluence;i++)
	{
		InfluenceBonus kBonus;
		char szKey[MAX_PATH] = "";
		sprintf_s( szKey, "influence_gap_%d", i + 1 );
		kBonus.fGapInfluence = rkLoader.LoadFloat( szKey, 0.0f );
		sprintf_s( szKey, "influence_bonus_%d", i + 1 );
		kBonus.fInfluenceBonus = rkLoader.LoadFloat( szKey, 0.0f );
		m_InfluenceBonusList.push_back( kBonus );
	}
}

void Mode::LoadAwardValue()
{
	ioINILoader &rkLoader = g_ModeINIMgr.GetINI( "config/sp2_awarding.ini" );
	rkLoader.SetTitle( "info" );
	m_dwAwardingTime = rkLoader.LoadInt( "award_time", 10000 );
	m_iSendAwardCount = rkLoader.LoadInt( "SendAwardCount", 0 );         
	int i = 0;

	// 시상식 항목별 보너스
	int iMaxAward = rkLoader.LoadInt( "MaxAward", 0 );
	for(i = 0;i < iMaxAward;i++)
	{
		char szKey[MAX_PATH] = "";
		sprintf_s( szKey, "Award%d", i + 1 );
		rkLoader.SetTitle( szKey );

		int   iType  = rkLoader.LoadInt( "Type", 0 );
		float fBonus = rkLoader.LoadFloat( "Bonus", 0.0f );
		m_AwardBonusTable.insert( AwardBonusMap::value_type( iType, fBonus ) );
	}

	// 시상식 추가 보너스
	m_AwardRandBonusTable.clear();
	m_dwAwardRandBonusSeed = 0;
	rkLoader.SetTitle( "AwardRandBonus" );
	int iMaxBonus = rkLoader.LoadInt( "MaxBonus", 0 );
	for(i = 0;i < iMaxBonus;i++)
	{
		AwardRandBonusData kData;
		char szKey[MAX_PATH] = "";
		sprintf_s( szKey, "Bonus%d_Rand", i + 1 );
		kData.m_dwRand = rkLoader.LoadInt( szKey, 0 );
		sprintf_s( szKey, "Bonus%d_Multiply", i + 1 );
		kData.m_fBonus = rkLoader.LoadFloat( szKey, false );
		m_AwardRandBonusTable.push_back( kData );
		m_dwAwardRandBonusSeed += kData.m_dwRand;
	}
}

void Mode::LoadGaugeRecorveyInfo( ioINILoader &rkLoader )
{
	rkLoader.SetTitle( "recovery" );
	m_fLastAttackKillRecoveryRate = rkLoader.LoadFloat( "last_attack_kill_recovery_rate", 1.0f );
	m_fBestAttackKillRecoveryRate = rkLoader.LoadFloat( "best_attack_kill_recovery_rate", 1.0f );

	m_fLastDropDieKillRecoveryRate = rkLoader.LoadFloat( "last_drop_kill_recovery_rate", 1.0f );
	m_fBestDropDieKillRecoveryRate = rkLoader.LoadFloat( "best_drop_kill_recovery_rate", 1.0f );
}

void Mode::SetModeState( ModeState eState )
{
	m_ModeState = eState;
	m_dwStateChangeTime = TIMEGETTIME();
	
	switch( m_ModeState )
	{
	case MS_READY:
		{
			int iRecordCnt = GetRecordCnt();
			for( int i = 0 ; i < iRecordCnt ; i++ )
			{
				ModeRecord *pRecord = FindModeRecord( i );
				if( pRecord && pRecord->pUser )
				{
					// 이벤트가 종료되면 이벤트 용병 삭제
					pRecord->pUser->ChildrenDayEventEndProcess();
				}
			}		
		}
		break;
	case MS_PLAY:
		{
			if( 1 < m_iCurRound )
			{
				int iRecordCnt = GetRecordCnt();
				for( int i = 0 ; i < iRecordCnt ; i++ )
				{
					ModeRecord *pRecord = FindModeRecord( i );
					if( pRecord && pRecord->pUser )
					{
						pRecord->StartPlaying();        //( 관전X, 데스타임X )
						pRecord->pUser->StartCharLimitDate( GetCharLimitCheckTime(), __FILE__, __LINE__ );				
						pRecord->pUser->StartEtcItemTime( __FUNCTION__ );
					}
				}		
			}
			else if ( m_iCurRound == 1 )
			{
				m_pCreator->InitRoomPlayTime();
			}

		}
		break;
	case MS_RESULT_WAIT:
		{
		}
		break;
	}
}

void Mode::SetMaxPlayer( int iMaxPlayer )
{
	// 최대 유저가 현재 접속한 유저보다 작을 수 없다.
	m_iMaxPlayer = max( m_pCreator->GetPlayUserCnt(), min( m_iConstMaxPlayer, iMaxPlayer ) );
}

void Mode::AddNewRecordEtcItemSync( User *pUser )
{
	if( !pUser ) return;
	if( m_vEtcItemSyncList.empty() ) return;

	// 이미 접속해있는 유저 + 자신의 정보를 입장한 유저에게 전송
	{
		int i = 0;
		vEtcItemSyncList vEtcItemSyncList;
		int iRecordCnt = GetRecordCnt();
		for(i = 0;i < iRecordCnt;i++)
		{
			ModeRecord *pRecord = FindModeRecord( i );
			if( !pRecord ) continue;
			if( !pRecord->pUser ) continue;

			EtcItemSyncData kEtcItem;
			pRecord->pUser->SyncModeEtcItem( m_vEtcItemSyncList, kEtcItem.m_vEtcItemList );
			if( !kEtcItem.m_vEtcItemList.empty() )
			{
				kEtcItem.m_szName = pRecord->pUser->GetPublicID();
				vEtcItemSyncList.push_back( kEtcItem );
			}
		}		

		int iListSize = vEtcItemSyncList.size();
		if( iListSize > 0 )
		{
			SP2Packet kPacket( STPK_ROOM_USER_ETCITEM_SYNC );
			kPacket << iListSize;
			for(i = 0;i < iListSize;i++)
			{
				EtcItemSyncData kEtcItem = vEtcItemSyncList[i];
				kPacket << kEtcItem.m_szName;

				int iEtcItemSize = kEtcItem.m_vEtcItemList.size();
				kPacket << iEtcItemSize;
				for(int j = 0;j < iEtcItemSize;j++)
				{
					kPacket << kEtcItem.m_vEtcItemList[j];
				}
			}
			pUser->SendMessage( kPacket );
			vEtcItemSyncList.clear();
		}
	}

	// 지금 들어온 유저의 정보를 이미 접속해있는 유저에게 전송 
	{
		IntVec kEtcItemList;
		pUser->SyncModeEtcItem( m_vEtcItemSyncList, kEtcItemList );
		if( !kEtcItemList.empty() )
		{
			SP2Packet kPacket( STPK_ROOM_USER_ETCITEM_SYNC );
			kPacket << 1;			
			kPacket << pUser->GetPublicID();

			int iEtcItemSize = kEtcItemList.size();
			kPacket << iEtcItemSize;
			for(int i = 0;i < iEtcItemSize;i++)
			{
				kPacket << kEtcItemList[i];
			}
			m_pCreator->RoomSendPacketTcp( kPacket, pUser );
			kEtcItemList.clear();
		}
	}
}

void Mode::ProcessTime()
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
#ifdef 	FISHING_SYSTEM_EX_BY_BCKIM	// 2020-02-17 by bckim, 낚시 시스템 확장
		IsAvailablePrivateFishing();		// 낚싯터 설치 후 잔여시간 확인 후 제거 
#endif	// FISHING_SYSTEM_EX_BY_BCKIM	// End. 2020-02-17 by bckim, 낚시 시스템 확장
		break;
	case MS_RESULT_WAIT:
		ProcessResultWait();
		break;
	case MS_RESULT:
		ProcessResult();
		break;
	}

	// 강퇴 시간 체크
	m_KickOutVote.ProcessVote();
}

void Mode::ProcessPrepare()
{
}

void Mode::ProcessReady()
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

	// 라운드가 시작될 때 유저 수를 확인
	m_iReadyBlueUserCnt = max( m_iReadyBlueUserCnt, GetTeamUserCnt( TEAM_BLUE ) );
	m_iReadyRedUserCnt  = max( m_iReadyRedUserCnt, GetTeamUserCnt( TEAM_RED ) );

	// 길드전 라운드 시작될 때 레벨 확인
	if( m_pCreator->GetRoomStyle() == RSTYLE_LADDERBATTLE )
	{
		m_iReadyBlueGuildLevel = max( m_iReadyBlueGuildLevel, m_pCreator->GetLadderTeamLevel( TEAM_BLUE ) );
		m_iReadyRedGuildLevel  = max( m_iReadyRedGuildLevel, m_pCreator->GetLadderTeamLevel( TEAM_RED ) );
	}

	// 레디 상태에서 이탈한 유저에 대한 체크
	CheckUserLeaveEnd();
}

void Mode::ProcessPlayCharHireTimeStop()
{
	// 용병 고용 시간은 한 세트에 m_dwModePointMaxTime이상 흐르지 않는다.
	int i = 0;
	int iRecordCnt = GetRecordCnt();
	for(i = 0;i < iRecordCnt;i++)
	{
		ModeRecord *pRecord = FindModeRecord( i );
		if( !pRecord ) continue;
		if( pRecord->eState != RS_PLAY ) continue;
		if( !pRecord->bClassPlayHireTimeCheck ) continue;

		if( IsPlayCharHireTimeStop( pRecord ) )
		{
			if( pRecord->pUser )
			{				
				if( pRecord->pUser->UpdateCharLimitDate() )
				{
					pRecord->bClassPlayHireTimeCheck = false; // 다음 부터 체크하지 않는다.
					LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "모드별 용병 시간 감소 최대값이 넘어서 시간 정지 시킴: %s:%dSec", pRecord->pUser->GetPublicID().c_str(), m_dwModePointMaxTime / 1000 );
				}
			}
		}
	}
}


void Mode::ProcessResultWait()
{
	if( m_bRoundSetEnd )
	{
		if( m_bCheckContribute && m_bCheckAwardChoose && m_bCheckAllRecvDamageList )
		{
			if( m_dwStateChangeTime + m_dwFinalResultWaitStateTime < TIMEGETTIME() )
			{
				FinalRoundProcess();
				SetModeState( MS_RESULT );
			}
		}
		else
		{
			if( m_dwStateChangeTime + 5000 < TIMEGETTIME() )
			{
				FinalRoundProcess();
				SetModeState( MS_RESULT );
			}
		}
	}
	else
	{
		SetModeState( MS_RESULT );
	}
}

void Mode::ProcessResult()
{
	DWORD dwResultDuration = m_dwResultStateTime;
	if( m_bRoundSetEnd )
	{
		dwResultDuration = m_dwFinalResultStateTime;
		if( m_bAwardStage ) // 시상식 시간이 최종 결과 시간에 포함된다.
			dwResultDuration += m_dwAwardingTime;

		// 대회 경기는 시상식이 없으므로 다른 전용 시간을 적용
		if( m_bTournamentRoom )
			dwResultDuration += m_dwTournamentRoomFinalResultAddTime;
	}

	if( m_dwStateChangeTime + dwResultDuration > TIMEGETTIME() )
		return;

	if( m_bRoundSetEnd )
	{
		if( ExecuteReservedExit() )
			return;	// true가 리턴된 경우는 모든 유저가 나가버린 상태
	}

	if( !m_bRoundSetEnd )
	{
		m_iCurRound++;                 // 세트가 종료되지 않은 상태에서만 라운드를 증가시킨다.
		RestartMode();
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
		
	}
}

void Mode::CheckSuddenDeathEnd()
{
	if( m_pCreator == NULL ) return;
	if( !m_bTournamentRoom ) return;
	if( GetState() != Mode::MS_PLAY ) return;
	if( m_dwCurSuddenDeathDuration == 0 ) return;

	DWORD dwGapTime = TIMEGETTIME() - m_dwCurSuddenDeathDuration;
	if( dwGapTime + 1000 < m_dwSuddenDeathTime ) return;

	//
	m_dwCurSuddenDeathDuration = 0;
	m_fSuddenDeathBlueCont     = 0.0f;
	m_fSuddenDeathRedCont      = 0.0f;
	// 유저들에게 기여도 보내라고 전송 ------- : 
	SP2Packet kPacket( STPK_TOURNAMENT_SUDDEN_DEATH );
	kPacket << TOURNAMENT_SUDDEN_DEATH_TIME_END;

	int iRecordCnt = GetRecordCnt();
	for( int i=0 ; i<iRecordCnt ; i++ )
	{
		ModeRecord *pRecord = FindModeRecord( i );
		if( !pRecord ) continue;
		if( !pRecord->pUser ) continue;
		if( pRecord->eState != RS_PLAY ) continue;

		// ( 플레이중인 유저들에게만 전송 )
		pRecord->pUser->SendMessage( kPacket );
	}
}

int Mode::GetPlayingUserCnt()
{
	int iPlayUserCnt = 0;
	int iRecordCnt = GetRecordCnt();
	for( int i=0 ; i<iRecordCnt ; i++ )
	{
		if( IsPlayingUser( i ) )
		{
			iPlayUserCnt++;
		}
	}

	return iPlayUserCnt;
}

int Mode::GetTeamUserCnt( TeamType eTeam )
{
	int iSameTeamCnt = 0;
	int iRecordCnt = GetRecordCnt();
	for( int i=0 ; i<iRecordCnt ; i++ )
	{
		ModeRecord *pRecord = FindModeRecord( i );
		if( pRecord && pRecord->pUser->GetTeam() == eTeam )
		{
			iSameTeamCnt++;
		}
	}

	return iSameTeamCnt;
}

bool Mode::IsPlayingUser( int iIdx )
{
	ModeRecord *pRecord = FindModeRecord( iIdx );
	if( pRecord && pRecord->eState != RS_LOADING )
		return true;

	return false;
}

bool Mode::IsTimeClose()
{
	//20080910 입장 시간 제한 없어짐
	return false;

	//입장제한시간 체크
	DWORD dwGapTime = TIMEGETTIME() - m_dwModeStartTime;
	if( dwGapTime > m_dwTimeClose )
	{
		if( m_pCreator->GetRoomStyle() == RSTYLE_BATTLEROOM )
			return m_pCreator->IsRoomProcess();        //룸 프로세스가 돌고있는 중에만 입장 불가
		else 
			return true;
	}

	if( m_bRoundSetEnd ) return true;        //세트가 종료되었다.

	// 전투룸에서는 라운드 종료 체크를 하지 않는다.
	// 훈련에서도 라운드 종료체크를 하지 않기로 함.
	return false;

	if( m_pCreator->GetRoomStyle() == RSTYLE_BATTLEROOM )
		return false;

	bool bClose = false;
	switch( m_ModeState )
	{
	case MS_READY:
		break;
	case MS_PLAY:
		if( m_iCurRound == m_iMaxRound )
		{
			DWORD dwCurTime = TIMEGETTIME() - m_dwStateChangeTime;
			if( dwCurTime > m_dwRoundDuration -	m_dwTimeClose )
				bClose = true;
		}
		break;
	case MS_RESULT_WAIT:
	case MS_RESULT:
		bClose = true;
		break;
	}

	return bClose;
}

void Mode::RestartMode()
{
	m_dwCurRoundDuration = m_dwRoundDuration;
	m_bZeroHP = false;

	m_iCurItemSupplyIdx = 0;
	m_iCurBallSupplyIdx = 0;

	int iRecordCnt = GetRecordCnt();
	for( int i=0 ; i<iRecordCnt ; i++ )
	{
		ModeRecord *pRecord = FindModeRecord( i );	
		pRecord->pUser->EquipDBItemToAllChar();
	}
}

void Mode::CheckRoundTimePing()
{
	if( m_ModeState != MS_PLAY )	return;

	DWORD dwCurTime = TIMEGETTIME();
	if( m_dwRoundTimeSendTime + 5000 < dwCurTime )
	{
		DWORD dwGap = dwCurTime - m_dwStateChangeTime;
		SP2Packet kPacket( SUPK_MODE_PING );
		kPacket << dwGap;
		m_pCreator->RoomSendPacketUdp( kPacket );

		m_dwRoundTimeSendTime = dwCurTime;
	}
}

void Mode::CheckItemSupply( DWORD dwStartTime )
{
	if( m_ModeState != MS_PLAY )	return;

	ItemVector vItemList;
	int iSupplyTimeCnt = m_vItemSupplyTimeList.size();
	DWORD dwTimeGap = TIMEGETTIME() - dwStartTime;

	if( !COMPARE( m_iCurItemSupplyIdx, 0, iSupplyTimeCnt ) )
		return;

	LOOP_GUARD();
	while( m_vItemSupplyTimeList[m_iCurItemSupplyIdx].m_dwSupplyTime <= dwTimeGap )
	{
		int iItemCnt = m_vItemSupplyTimeList[m_iCurItemSupplyIdx].m_ItemList.size();

		ObjectItemList vObjectItemList;
		vObjectItemList.clear();
		vObjectItemList.reserve( iItemCnt );

		int i = 0;
		for(int i=0; i<iItemCnt; i++ )
		{
			ObjectItem kObjectItem;
			Vector3 vPos = m_vItemSupplyTimeList[m_iCurItemSupplyIdx].GetRandomItemPos();

			kObjectItem.m_ObjectItemName = m_vItemSupplyTimeList[m_iCurItemSupplyIdx].m_ItemList[i];
			kObjectItem.m_fPosX = vPos.x;
			kObjectItem.m_fPosZ = vPos.z;
			vObjectItemList.push_back( kObjectItem );
		}

		int iObjectCnt = vObjectItemList.size();

		for(int i=0; i<iObjectCnt; i++ )
		{
			const ObjectItem &rkObjItem = vObjectItemList[i];

			ioItem *pItem = m_pCreator->CreateItemByName( rkObjItem.m_ObjectItemName );
			if( pItem )
			{
				Vector3 vPos( rkObjItem.m_fPosX, 0.0f, rkObjItem.m_fPosZ );
				pItem->SetItemPos( vPos );
				pItem->SetEnableDelete(true);
				vItemList.push_back( pItem );
			}
		}

		m_iCurItemSupplyIdx++;

		if( !COMPARE( m_iCurItemSupplyIdx, 0, iSupplyTimeCnt ) )
			break;
	}
	LOOP_GUARD_CLEAR();

	if( vItemList.empty() )
		return;

	int iNewItemCnt = vItemList.size();
	SP2Packet kPacket( STPK_FIELD_ITEM_SUPPLY );
	kPacket << iNewItemCnt;
	for( int i=0; i<iNewItemCnt; i++ )
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

void Mode::CheckBallSupply( DWORD dwStartTime )
{
	if( m_ModeState != MS_PLAY )	return;

	int iSupplyTimeCnt = m_vBallSupplyList.size();
	DWORD dwTimeGap = TIMEGETTIME() - dwStartTime;

	if( !COMPARE( m_iCurBallSupplyIdx, 0, iSupplyTimeCnt ) )
		return;

	LOOP_GUARD();
	BallStructList vBallList;
	while( m_vBallSupplyList[m_iCurBallSupplyIdx].m_dwSupplyTime <= dwTimeGap )
	{
		m_iBallStructIdx++;

		BallStruct eBall;
		eBall.m_iIndex = m_iBallStructIdx;
		eBall.m_iNum = m_vBallSupplyList[m_iCurBallSupplyIdx].m_iNum;
		eBall.m_TargetRot = m_vBallSupplyList[m_iCurBallSupplyIdx].m_TargetRot;

		Vector3 vCenter = m_vBallSupplyList[m_iCurBallSupplyIdx].m_CreatePos;
		Vector3 vStartPos = GetBallStartPosition();
		vCenter.x = vStartPos.x;
		vCenter.z = vStartPos.z;

		eBall.m_CreatePos = vCenter;

		m_vBallStructList.push_back( eBall );
		vBallList.push_back( eBall );

		m_iCurBallSupplyIdx++;

		if( !COMPARE( m_iCurBallSupplyIdx, 0, iSupplyTimeCnt ) )
			break;
	}
	LOOP_GUARD_CLEAR();

	if( vBallList.empty() )
		return;

	SP2Packet kBallPacket( STPK_BALLSTRUCT_INFO );
	int iBallStructCnt = vBallList.size();
	kBallPacket << iBallStructCnt;

	for( int i=0; i<iBallStructCnt; i++ )
	{
		const BallStruct &rkBall = vBallList[i];

		kBallPacket << rkBall.m_iNum;
		kBallPacket << rkBall.m_iIndex;
		kBallPacket << rkBall.m_CreatePos;
		kBallPacket << rkBall.m_TargetRot;
	}
	SendRoomAllUser( kBallPacket );
}

bool Mode::ExecuteReservedExit()
{
	int iRecordCnt = GetRecordCnt();

	std::vector< User* > vExitUser;
	vExitUser.reserve( iRecordCnt );

	for( int i=0 ; i<iRecordCnt ; i++ )
	{
		ModeRecord *pRecord = FindModeRecord( i );
		if( !pRecord )	continue;
		
		// 나가기 예약한 유저
		if( pRecord->pUser->IsExitRoomReserved() )
		{
			vExitUser.push_back( pRecord->pUser );
		}
		else if( m_pCreator->IsSafetyLevelRoom() )
		{
			// 훈련병으로 들어와서 킬 잘하는 유저는 내보낸다.
			if( pRecord->pUser->IsLeaveSafeRoom() )
			{
				vExitUser.push_back( pRecord->pUser );
			}
		}
	}

	// log
	int iExitUserCnt = vExitUser.size();
	for(int i=0 ; i<iExitUserCnt ; i++ )
	{
		int iRecordCnt = GetRecordCnt();
		ModeRecord *pRecord = FindModeRecord(vExitUser[i]);
		SetModeEndDBLog( pRecord, iExitUserCnt, LogDBClient::PRT_EXIT_ROOM );
	}
	//
	//SetModeEndDBLog( m_pCreator, LogDBClient::PRT_EXIT_ROOM );


	for(int i=0 ; i<iExitUserCnt ; i++ )
	{
		vExitUser[i]->LeaveRoom();
		if( vExitUser[i]->IsBattleRoom() )		//파티중일 경우 파티 탈퇴도 실행.
			vExitUser[i]->LeaveBattleRoom();
		else if( vExitUser[i]->IsLadderTeam() )	//길드전일 경우 길드 탈퇴
			vExitUser[i]->LeaveLadderTeam();
		else if( vExitUser[i]->IsShuffleRoom() )
			vExitUser[i]->LeaveShuffleRoom();

		int iResult = EXIT_ROOM_OK;
		if( m_pCreator->IsSafetyLevelRoom() )
		{
			if( vExitUser[i]->IsLeaveSafeRoom() )
				iResult = EXIT_ROOM_SAFETY_KICK;
		}

		vExitUser[i]->ExitRoomToTraining( iResult, false );
	}

	if( iRecordCnt == iExitUserCnt )
		return true;

	return false;
}

bool Mode::ExecuteReservedExitViewUser()
{
	int iRecordCnt = GetRecordCnt();

	std::vector< User* > vExitUser;
	vExitUser.reserve( iRecordCnt );

	for( int i=0 ; i<iRecordCnt ; i++ )
	{
		ModeRecord *pRecord = FindModeRecord( i );
		if( !pRecord )	continue;
		if( pRecord->pUser->IsExitRoomReserved() &&
			(pRecord->eState == RS_VIEW || pRecord->eState == RS_OBSERVER) )
		{
			vExitUser.push_back( pRecord->pUser );
		}
	}

	int iExitUserCnt = vExitUser.size();
	for(int i=0 ; i<iExitUserCnt ; i++ )
	{
		vExitUser[i]->LeaveRoom();
		if( vExitUser[i]->IsBattleRoom() )       
			vExitUser[i]->LeaveBattleRoom();
		else if( vExitUser[i]->IsLadderTeam() )
			vExitUser[i]->LeaveLadderTeam();
		else if( vExitUser[i]->IsShuffleRoom() )
			vExitUser[i]->LeaveShuffleRoom();

		vExitUser[i]->ExitRoomToTraining( EXIT_ROOM_OK, false );
	}

	return false;
}

bool Mode::ExecuteReservedExitPlayUser()
{
	int iRecordCnt = GetRecordCnt();

	std::vector< User* > vExitUser;
	vExitUser.reserve( iRecordCnt );

	for( int i=0 ; i<iRecordCnt ; i++ )
	{
		ModeRecord *pRecord = FindModeRecord( i );
		if( !pRecord )	continue;
		if( pRecord->pUser->ExitRoomTimeOver() )
		{
			vExitUser.push_back( pRecord->pUser );
		}
	}

	int iExitUserCnt = vExitUser.size();
	for(int i=0 ; i<iExitUserCnt ; i++ )
	{
		vExitUser[i]->LeaveRoom();
		if( vExitUser[i]->IsBattleRoom() ) 
			vExitUser[i]->LeaveBattleRoom();
		else if( vExitUser[i]->IsLadderTeam() )
			vExitUser[i]->LeaveLadderTeam();
		else if( vExitUser[i]->IsShuffleRoom() )
			vExitUser[i]->LeaveShuffleRoom();
		
		vExitUser[i]->ExitRoomToTraining( EXIT_ROOM_OK, false );
	}

	return false;
}

void Mode::SetFirstRevivalTime( ModeRecord *pRecord )
{
	if( pRecord )
	{
		pRecord->iRevivalCnt  = 0;
		pRecord->dwCurDieTime = 0;
		pRecord->dwRevivalGap = (DWORD)GetRevivalGapTime( 0 );
		pRecord->iRevivalCnt  = 0;
	}
}

void Mode::NotifyChangeChar( User *pUser, int iSelectChar, int iPrevCharType  )
{
	if( pUser )
	{
		ModeRecord *pRecord = FindModeRecord( pUser );
		if( pRecord )
		{
			pRecord->AddClassPlayingTime( iPrevCharType );
			pRecord->dwClassPlayingStartTime = TIMEGETTIME();
		}
		pUser->SetCharJoined( iSelectChar, true );
	}
}

void Mode::UpdateDieState( User *pDier )
{
	ModeRecord *pDieRecord = FindModeRecord( pDier );
	if( pDieRecord )
	{
		pDieRecord->dwCurDieTime = 0;
		pDieRecord->bDieState = true;
		pDieRecord->bExperienceState = false;
	}
}

void Mode::UpdateUserDieTime( User *pDier )
{
	ModeRecord *pDieRecord = FindModeRecord( pDier );
	if( pDieRecord && pDieRecord->bDieState )
	{
		float fRedTeamCnt = GetCurTeamUserCnt( TEAM_RED );
		float fBlueTeamCnt = GetCurTeamUserCnt( TEAM_BLUE );
		float fRate = 1.0f;

		if( fRedTeamCnt > 0 && fBlueTeamCnt > 0 )
		{
			if( pDier->GetTeam() == TEAM_RED && fRedTeamCnt < fBlueTeamCnt )
				fRate = fRedTeamCnt / fBlueTeamCnt;
			else if( pDier->GetTeam() == TEAM_BLUE && fBlueTeamCnt < fRedTeamCnt )
				fRate = fBlueTeamCnt / fRedTeamCnt;
		}

		DWORD dwRevivalGap = (DWORD)GetRevivalGapTime( pDieRecord->iRevivalCnt );
		pDieRecord->dwCurDieTime = TIMEGETTIME();
		pDieRecord->dwRevivalGap = dwRevivalGap * fRate;
		pDieRecord->iRevivalCnt++;
	}
}

void Mode::UpdateDropDieRecord( User *pDier, const ioHashString &szAttacker, const ioHashString &szBestAttacker  )
{
	ModeRecord *pDieRecord = FindModeRecord( pDier );
	if( !pDieRecord )	return;

	// 마지막으로 타격한 유저와 죽은 유저
	if( !szAttacker.IsEmpty() && pDier->GetPublicID() != szAttacker )
	{
		ModeRecord *pAttRecord = FindModeRecord( szAttacker );
		if( !pAttRecord )	return;

		float fKillPoint = 0.5f + ( (float)GetKillPoint( pAttRecord->pUser->GetTeam() ) * 0.5f );
		if( pDieRecord->pUser->GetTeam() != pAttRecord->pUser->GetTeam() )
		{
			pAttRecord->AddKillCount( m_pCreator->GetRoomStyle(), GetModeType(), fKillPoint );
			pDieRecord->AddDeathCount( m_pCreator->GetRoomStyle(), GetModeType(), GetDeathPoint( pDieRecord->pUser->GetTeam() ) );
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
		}
	}
	else
	{
		pDieRecord->AddDeathCount( m_pCreator->GetRoomStyle(), GetModeType(), GetDeathPoint( pDieRecord->pUser->GetTeam() ) );
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

void Mode::UpdateWeaponDieRecord( User *pDier, const ioHashString &szAttacker, const ioHashString &szBestAttacker )
{
	ModeRecord *pDieRecord = FindModeRecord( pDier );
	if( !pDieRecord )	return;

	// 마지막으로 타격한 유저와 죽은 유저
	ModeRecord *pKillRecord = FindModeRecord( szAttacker );
	if( pKillRecord )
	{
		float fKillPoint = 0.5f + ( (float)GetKillPoint( pKillRecord->pUser->GetTeam() ) * 0.5f );
		if( pKillRecord->pUser->GetTeam() != pDier->GetTeam() )
		{
			pKillRecord->AddKillCount( m_pCreator->GetRoomStyle(), GetModeType(), fKillPoint );
			pDieRecord->AddDeathCount( m_pCreator->GetRoomStyle(), GetModeType(), GetDeathPoint( pDieRecord->pUser->GetTeam() ) );
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
			}
		}
	}
	else
	{
		pDieRecord->AddDeathCount( m_pCreator->GetRoomStyle(), GetModeType(), GetDeathPoint( pDieRecord->pUser->GetTeam() ) );
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

void Mode::UpdateUserDieNextProcess( User *pDier, const ioHashString &szAttacker, const ioHashString &szBestAttacker )
{
	if( GetState() != Mode::MS_PLAY ) return;

	CheckRoundEnd( true );
}

void Mode::UpdateRoundRecord()
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

void Mode::UpdateUserRank()
{
	int iRecordCnt = GetRecordCnt();

	RankList vRankList;
	vRankList.reserve( iRecordCnt );

	for( int i=0 ;i<iRecordCnt ; i++ )
	{
		ModeRecord *pRecord = FindModeRecord( i );
		if( !pRecord || pRecord->eState == RS_LOADING )
			continue;

		RankInfo kInfo;
		kInfo.szName	  = pRecord->pUser->GetPublicID(); 
		kInfo.iTotalKill  = pRecord->GetAllTotalKill();
		kInfo.iTotalDeath = pRecord->GetAllTotalDeath();

		if( m_bRoundSetEnd )	// Final Round
		{
			kInfo.bAbuse = IsAbuseUser( i );
		}

		vRankList.push_back( kInfo );
	}

	if( !m_bRoundSetEnd )
	{
		std::sort( vRankList.begin(), vRankList.end(), RankInfoSort() );
	}
	else
	{
		std::sort( vRankList.begin(), vRankList.end(), FinalRankInfoSort() );
	}

	int iRank = 1;
	RankList::iterator iter, iEnd;
	iEnd = vRankList.end();
	for( iter=vRankList.begin() ; iter!=iEnd ; ++iter, ++iRank )
	{
		ModeRecord *pRecord = FindModeRecord( iter->szName );
		if( pRecord )
		{
			if( pRecord->iPreRank == 0 )
			{
				pRecord->iPreRank = iRank;
			}
			else
			{
				pRecord->iPreRank = pRecord->iCurRank;
			}

			pRecord->iCurRank = iRank;
		}
	}
}

void Mode::GetUserRankByNextTeam( UserRankInfoList &rkUserRankList, bool bRandom )
{
	rkUserRankList.clear();

	int iRecordCnt = GetRecordCnt();

	NextTeamInfoList vInfoList;
	vInfoList.reserve( iRecordCnt );

	for( int i=0 ;i<iRecordCnt ; i++ )
	{
		ModeRecord *pRecord = FindModeRecord( i );
		if( !pRecord ) continue;
		if( pRecord->eState == RS_OBSERVER ) continue;

		NextTeamInfo kInfo;

		if( bRandom )
		{
			kInfo.fPoint         = rand() % 10000 + 100;
			kInfo.szName	     = pRecord->pUser->GetPublicID(); 
			kInfo.iTotalKill     = rand() % 1000 + 10;
			kInfo.iTotalDeath    = rand() % 1000 + 10;
		}
		else
		{
			float fContributePoint = pRecord->fContributePer * Help::GetContributePerRate();
			float fKillDeathPoint = (float)pRecord->pUser->GetKillDeathLevel() * Help::GetKillDeathLevelRate();

			kInfo.fPoint         = fContributePoint * fKillDeathPoint;
			kInfo.szName	     = pRecord->pUser->GetPublicID(); 
			kInfo.iTotalKill     = pRecord->GetAllTotalKill();
			kInfo.iTotalDeath    = pRecord->GetAllTotalDeath();
		}
		vInfoList.push_back( kInfo );
	}

	std::sort( vInfoList.begin(), vInfoList.end(), NextTeamInfoSort() );

	NextTeamInfoList::iterator iter, iEnd;
	iEnd = vInfoList.end();
	int iRank = 1;
	for( iter=vInfoList.begin() ; iter!=iEnd ; ++iter, ++iRank )
	{
		NextTeamInfo kNextTeamInfo = *iter;
		
		UserRankInfo kUserRankInfo;
		kUserRankInfo.szName = kNextTeamInfo.szName;
		kUserRankInfo.iRank = iRank;

		rkUserRankList.push_back( kUserRankInfo );
	}
}

void Mode::GetUserRankByKillDeath( UserRankInfoList &rkUserRankList )
{
	rkUserRankList.clear();

	int iRecordCnt = GetRecordCnt();

	KillDeathRankList vRankList;
	vRankList.reserve( iRecordCnt );

	for( int i=0 ;i<iRecordCnt ; i++ )
	{
		ModeRecord *pRecord = FindModeRecord( i );
		if( !pRecord ) continue;
		if( pRecord->eState == RS_OBSERVER ) continue;

		KillDeathRankInfo kRankInfo;
		kRankInfo.iKillDeathLevel = pRecord->pUser->GetKillDeathLevel();
		kRankInfo.szName = pRecord->pUser->GetPublicID();

		vRankList.push_back(kRankInfo);
	}

	std::sort( vRankList.begin(), vRankList.end(), KillDeathLevelSort() );

	KillDeathRankList::iterator iter, iEnd;
	iEnd = vRankList.end();
	int iRank = 1;
	for( iter=vRankList.begin() ; iter!=iEnd ; ++iter, ++iRank )
	{
		KillDeathRankInfo kRankInfo = *iter;
		
		UserRankInfo kUserRankInfo;
		kUserRankInfo.szName = kRankInfo.szName;
		kUserRankInfo.iRank = iRank;

		rkUserRankList.push_back( kUserRankInfo );
	}
}

int Mode::GetMeaningRankUserCnt()
{
	int iMeaningCnt = 0;

	int iRecordCnt = GetRecordCnt();
	for( int i=0 ; i<iRecordCnt ; i++ )
	{
		if( IsPlayingUser( i ) && !IsAbuseUser( i ) )
			iMeaningCnt++;
	}

	return iMeaningCnt;
}

void Mode::UpdateBattleRoomRecord()
{
	if( m_pCreator->GetRoomStyle() != RSTYLE_BATTLEROOM ) return;
	if( GetModeType() == MT_SURVIVAL || GetModeType() == MT_BOSS || GetModeType() == MT_GANGSI || GetModeType() == MT_FIGHT_CLUB ) return;

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

	if( pBattleRoom && pBattleRoom->IsOriginal() )
	{
		TeamType eWinTeam  = TEAM_NONE;
		if( m_iBlueTeamWinCnt > m_iRedTeamWinCnt )
			eWinTeam = TEAM_BLUE;
		else if( m_iRedTeamWinCnt > m_iBlueTeamWinCnt )
			eWinTeam = TEAM_RED;

		BattleRoomNode *pOriginalNode = (BattleRoomNode*)pBattleRoom;
		pOriginalNode->UpdateRecord( eWinTeam );
	}
	else
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Mode::UpdateBattleRoomRecord() Not Battle Room" );
	}
}

void Mode::UpdateShuffleRoomRecord()
{
	if( m_pCreator->GetRoomStyle() != RSTYLE_SHUFFLEROOM ) return;
	if( GetModeType() == MT_SURVIVAL || GetModeType() == MT_BOSS || GetModeType() == MT_GANGSI || GetModeType() == MT_FIGHT_CLUB || GetModeType() == MT_SHUFFLE_BONUS ) return;

	ShuffleRoomParent *pShuffleRoom = NULL;

	int iRecordCnt = GetRecordCnt();
	for(int i = 0;i < iRecordCnt;i++)
	{
		ModeRecord *pRecord = FindModeRecord( i );
		if( pRecord && pRecord->pUser )
		{
			pShuffleRoom = pRecord->pUser->GetMyShuffleRoom();
			if( pShuffleRoom && pShuffleRoom->IsOriginal() )
				break;
		}
	}

	if( pShuffleRoom && pShuffleRoom->IsOriginal() )
	{
		TeamType eWinTeam  = TEAM_NONE;
		if( m_iBlueTeamWinCnt > m_iRedTeamWinCnt )
			eWinTeam = TEAM_BLUE;
		else if( m_iRedTeamWinCnt > m_iBlueTeamWinCnt )
			eWinTeam = TEAM_RED;

		ShuffleRoomNode *pOriginalNode = dynamic_cast<ShuffleRoomNode*>( pShuffleRoom );
		if( pOriginalNode )
			pOriginalNode->UpdateRecord( eWinTeam );
	}
	else
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Mode::UpdateShuffleRoomRecord() Not ShuffleRoom" );
	}
}

void  Mode::UpdateTournamentRecord()
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
		
		TeamType eWinTeam  = TEAM_NONE;
		if( m_iBlueTeamWinCnt > m_iRedTeamWinCnt )
			eWinTeam = TEAM_BLUE;
		else if( m_iRedTeamWinCnt > m_iBlueTeamWinCnt )
			eWinTeam = TEAM_RED;

		// 한쪽팀이 모두 이탈하면 남아있는 팀이 이긴다.
		int iBlueUserCount = GetCurTeamUserCnt( TEAM_BLUE );
		int iRedUserCount  = GetCurTeamUserCnt( TEAM_RED );
		if( iBlueUserCount == 0 )
		{
			// 레드팀 승
			eWinTeam = TEAM_RED;
		}
		else if( iRedUserCount == 0 )
		{
			// 블루팀 승
			eWinTeam = TEAM_BLUE;
		}

		// 결과를 전송 - - -
		DWORD dwTourIndex	  = pBattleNode->GetTournamentIndex();
		DWORD dwBlueTeamIndex = pBattleNode->GetTournamentBlueIndex();
		DWORD dwRedTeamIndex  = pBattleNode->GetTournamentRedIndex();
		BYTE  BlueTourPos     = pBattleNode->GetTournamentBlueTourPos();
		BYTE  RedTourPos      = pBattleNode->GetTournamentRedTourPos();

		LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "Mode::UpdateTournamentRecord() %d - %d - %d - %d - %d - %d - %d - %d", dwTourIndex, dwBlueTeamIndex, dwRedTeamIndex, (int)BlueTourPos, (int)RedTourPos, (int)eWinTeam, m_iBlueTeamWinCnt, m_iRedTeamWinCnt );

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
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Mode::UpdateTournamentRecord() Win Team None : %d", dwTourIndex );
			break;
		}
	}
	else
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Mode::UpdateTournamentRecord() Not Battle Room" );
	}
}

float Mode::GetLadderGuildTeamBonus( TeamType eMyTeam )
{
	if( m_pCreator->GetRoomStyle() != RSTYLE_LADDERBATTLE ) return 1.0f;

	if( m_pCreator->IsLadderGuildTeam( TEAM_BLUE ) && m_pCreator->IsLadderGuildTeam( TEAM_RED ) )
		return m_fLadderGuildTeamTwo;
	else if( m_pCreator->IsLadderGuildTeam( eMyTeam ) )
		return m_fLadderGuildTeamOne;
	return 1.0f;
}

int Mode::DecreasePenaltyLadderPoint( User *pUser )
{
	// 2009.06.11 룸 이탈시 래더포인트 감소 없음
	return 0;

	if( m_pCreator->GetRoomStyle() != RSTYLE_LADDERBATTLE ) return 0;

	ModeRecord *pRecord = FindModeRecord( pUser );
	if( !pRecord || !pRecord->pUser ) return 0;
	if( m_dwModeRecordPointTime == 0 ) return 0;
	if( pRecord->pUser->IsObserver() ) return 0;
	if( pRecord->pUser->IsStealth() ) return 0;
    
	float fPlayTime = (float)GetRecordPlayTime( pRecord ) / m_dwModeRecordPointTime;
	float fModeLadderPoint = GetModeLadderPoint( pRecord->pUser, TEAM_NONE, fPlayTime );
    int iMinusLadderPoint = min( pRecord->pUser->GetLadderPoint(), (int)fModeLadderPoint );
	pRecord->pUser->AddLadderPoint( -iMinusLadderPoint );

	if( pRecord->pUser->GetTeam() == TEAM_BLUE )
		m_fBlueReserveLadderPoint += (float)iMinusLadderPoint;
	else if( pRecord->pUser->GetTeam() == TEAM_RED )
		m_fRedReserveLadderPoint += (float)iMinusLadderPoint;

	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "Mode::DecreasePenaltyLadderPoint : %s - (%d / %d = %.2f) - (%.2f - %d)",
							pUser->GetPublicID().c_str(), GetRecordPlayTime( pRecord ), m_dwModeRecordPointTime, fPlayTime,
							fModeLadderPoint, iMinusLadderPoint );

	return iMinusLadderPoint;
}

float Mode::GetModeLadderPoint( User *pUser, TeamType eWinTeam, float fPlayTime )
{
	if( !pUser ) return 0.0f;

	// 기본 포인트
	float fModeLadderPoint = m_fModeLadderPoint;
	// 양팀 실력 계급 수준차
	float fLevelGapPer = ( (float)abs( m_pCreator->GetLadderTeamLevel( TEAM_BLUE ) - m_pCreator->GetLadderTeamLevel( TEAM_RED ) ) / g_LevelMatchMgr.GetRoomEnterLevelMax() );
	float fAbilityLevelPoint = 2.0f - ( (float)min( fLevelGapPer, 1.0f ) * m_fLadderLevelCorrection );
	if( fAbilityLevelPoint <= 0.0f )
		fAbilityLevelPoint = 0.1f;
	// 플레이 시간 보정값
    float fPlayTimePer = fPlayTime;
	// 스코어 차이 보정값
	float fRoundPoint = (float)m_ModeLadderPointRound;
	float fScoreGap = GetResultScoreGapValue( true, eWinTeam );
	// 인원 보정값
	float fUserCorrection = GetUserCorrection( eWinTeam, (float)m_ModePointRound, GetResultScoreGapValue( false, eWinTeam ) );      //경험치 & 포인트 계산과 동일하게 간다.

	// 최종 스코어 보정값
	if( pUser->GetTeam() == eWinTeam )
		fScoreGap = fRoundPoint + fScoreGap;
	else
		fScoreGap = fRoundPoint - fScoreGap;

	// 길드 팀 보너스
// 	float fGuildTeamBonus = GetLadderGuildTeamBonus( pUser->GetTeam() );
// 
// 	// 세력비에따른 보너스
// 	float fCampInfluence = m_pCreator->GetCampPointBonus( pUser->GetTeam() );

	float fGuildTeamBonus = 1.0f, fCampInfluence = 1.0f;
	LadderTeamParent *pLadderTeam = pUser->GetMyLadderTeam();
	if( pLadderTeam )
	{
		fGuildTeamBonus = GetLadderGuildTeamBonus( (TeamType)pLadderTeam->GetCampType() );
		fCampInfluence = m_pCreator->GetCampPointBonus( (TeamType)pLadderTeam->GetCampType() );
	}
	else
	{
		// 길드 팀 보너스
		float fGuildTeamBonus = GetLadderGuildTeamBonus( pUser->GetTeam() );

		// 세력비에따른 보너스
		float fCampInfluence = m_pCreator->GetCampPointBonus( pUser->GetTeam() );
	}
	

	//차단 G
	float fBlockPoint = pUser->GetBlockPointPer();

	// event
	float fEventBonus = 1.0f; // 이벤트 %가 더해진후 실재 포인트에 곱해진다.
	if( pUser )
	{
		EventUserManager &rEventUserManager = pUser->GetEventUserMgr();
		LadderPointEventUserNode *pEventNode = static_cast<LadderPointEventUserNode*> ( rEventUserManager.GetEventUserNode( EVT_LADDER_POINT ) );
		if( pEventNode )
		{
			fEventBonus += pEventNode->GetEventPer( 0.0f, pUser );
		}
	}
	
	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "Mode::GetLadderPoint[%d][%d:%d] : %.2f x %.2f x %.2f x %.2f x %.2f x %.2f x %.2f x %.2f x %.2f", m_pCreator->GetRoomIndex(), (int)pUser->GetTeam(), (int)eWinTeam,
						 fModeLadderPoint, fAbilityLevelPoint, fPlayTimePer, fScoreGap, fUserCorrection, fGuildTeamBonus, fCampInfluence, fBlockPoint, fEventBonus );
	//     기본 포인트      x 실력 계급 수준차    x 플레이시간   x 스코어차이 보정값 x 인원 보정 x 길드팀 보너스 x 세력비에따른 보너스 x 명성 x 이벤트
	return fModeLadderPoint * fAbilityLevelPoint * fPlayTimePer * fScoreGap * fUserCorrection * fGuildTeamBonus * fCampInfluence * fBlockPoint * fEventBonus;
}

float Mode::GetHeroTitleBonus( User *pUser )
{
	if( !pUser ) return 0.0f;
	if( m_pCreator->GetRoomStyle() != RSTYLE_LADDERBATTLE ) return 0.0f;
	if( GetModeType() != MT_HEROMATCH ) return 0.0f;
	if( pUser->GetHeroTitle() == 0 ) return 0.0f;

	HeroTitleBonusMap::iterator iter = m_HeroTitleBonusMap.find( pUser->GetHeroTitle() );
	if( iter != m_HeroTitleBonusMap.end() )
		return iter->second;
	return 0.0f;
}

float Mode::GetCampInfluenceBonus( float fInfluenceGap )
{
	for(int i = 0;i < (int)m_InfluenceBonusList.size();i++)
	{
		InfluenceBonus &rkBonus = m_InfluenceBonusList[i];
		if( fInfluenceGap >= rkBonus.fGapInfluence )
			return rkBonus.fInfluenceBonus;
	}
	return 1.0f;
}

void Mode::UpdateLadderBattleRecord()
{
	if( m_pCreator->GetRoomStyle() != RSTYLE_LADDERBATTLE ) return;
	if( GetModeType() == MT_SURVIVAL || GetModeType() == MT_BOSS || GetModeType() == MT_GANGSI || GetModeType() == MT_FIGHT_CLUB ) return;
	if( m_dwModeRecordPointTime == 0 ) return;
	if( !g_LadderTeamManager.IsCampBattlePlay() )
	{
		LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "UpdateLadderBattleRecord[%d] 결과가 발생하였지만 이미 진영전이 종료되었다.", m_pCreator->GetRoomIndex() );
		return;
	}

	//
	TeamType eWinTeam  = TEAM_NONE;
	if( m_iBlueTeamWinCnt > m_iRedTeamWinCnt )
		eWinTeam = TEAM_BLUE;
	else if( m_iRedTeamWinCnt > m_iBlueTeamWinCnt )
		eWinTeam = TEAM_RED;

	if( GetModeType() == MT_HEROMATCH )
	{
		// 이긴 유저가 없으면 무승부
		if( GetTeamUserCnt( eWinTeam ) == 0 ) 
		{
			eWinTeam = TEAM_NONE;
		}
	}

	bool bHeroMatch = false;
	LadderTeamParent *pBlueLadderTeam, *pRedLadderTeam;
	pBlueLadderTeam = pRedLadderTeam = NULL;

	DWORD dwBluePlayTime, dwRedPlayTime;
	dwBluePlayTime = dwRedPlayTime = 0;
	int i = 0;
	int iRecordCnt = GetRecordCnt();
	for(i = 0;i < iRecordCnt;i++)
	{
		ModeRecord *pRecord = FindModeRecord( i );
		if( pRecord && pRecord->pUser )
		{
			if( pRecord->pUser->GetTeam() == TEAM_BLUE )
			{
				if( !pBlueLadderTeam )
					pBlueLadderTeam = pRecord->pUser->GetMyLadderTeam();
				if( dwBluePlayTime == 0 )
					dwBluePlayTime = GetRecordPlayTime( pRecord );
			}
			else if( pRecord->pUser->GetTeam() == TEAM_RED )
			{
				if( !pRedLadderTeam )
					pRedLadderTeam = pRecord->pUser->GetMyLadderTeam();
				if( dwRedPlayTime == 0 )
					dwRedPlayTime = GetRecordPlayTime( pRecord );
			}
		}
	}

	BOOL bSameCamp	= FALSE;

	if( pBlueLadderTeam && pRedLadderTeam )
	{
		if( pBlueLadderTeam->GetCampType() == pRedLadderTeam->GetCampType() )
			bSameCamp	= TRUE;
	}

	// 래더 포인트 계산
	UpdateLadderBattlePoint( eWinTeam, max( dwBluePlayTime, dwRedPlayTime ), bSameCamp );

	{
		// 유저들이 획득한 래더 포인트와 전적을 메인 서버에 업데이트한다.
		SP2Packet kMainPacket( MSTPK_LADDER_MODE_RESULT_UPDATE );

		DWORDVec vUpdateRecord;
		for(i = 0;i < iRecordCnt;i++)
		{
			ModeRecord *pRecord = FindModeRecord( i );
			if( !pRecord ) continue;
			if( !pRecord->pUser ) continue;
			if( pRecord->iTotalLadderPoint == 0 ) continue;

			vUpdateRecord.push_back( (DWORD)pRecord );
		}
		int iUpdateSize = vUpdateRecord.size();
		kMainPacket << iUpdateSize;
		// 유저인덱스 << 유저진영타입 << 길드인덱스 << 획드포인트
		for(i = 0;i < iUpdateSize;i++)
		{
			ModeRecord *pRecord = (ModeRecord *)vUpdateRecord[i];
			if( !pRecord || !pRecord->pUser )
			{
				kMainPacket << 0 << 0 << 0 << 0 << false;
			}
			else
			{
				if( pRecord->iTotalLadderPoint < 0 )
					pRecord->iTotalLadderPoint = 0;
				kMainPacket << pRecord->pUser->GetUserIndex() << pRecord->pUser->GetUserCampPos() << pRecord->pUser->GetGuildIndex() << pRecord->iTotalLadderPoint;

				// 정규 리그 팀 진영 포인트		
				DWORD dwTourIndex = 0;
				DWORD dwTeamIndex = 0;
				if( g_TournamentManager.IsRegularTournamentTeamEntryAgreeState() )
				{
					ioUserTournament *pUserTournament = pRecord->pUser->GetUserTournament();
					if( pUserTournament )
					{	
						dwTourIndex = g_TournamentManager.GetRegularTournamentIndex();
						ioUserTournament::TeamData &rkTeamData = pUserTournament->GetTournamentTeamData( dwTourIndex );
						if( rkTeamData.m_dwTourIndex == dwTourIndex )
						{
							dwTeamIndex = rkTeamData.m_dwTeamIndex;
						}
					}
				}
				if( dwTourIndex != 0 && dwTeamIndex != 0 )
				{
					kMainPacket << true << dwTourIndex << dwTeamIndex;
				}
				else
				{
					kMainPacket << false;
				}
			}
		}
		vUpdateRecord.clear();
		// 길드 전적을 메인서버에 업데이트한다.
		if( pBlueLadderTeam )
		{
			pBlueLadderTeam->UpdateRecord( TEAM_BLUE, eWinTeam );

			// 길드팀이면 길드 전적 업데이트
			if( m_pCreator->IsLadderGuildTeam( TEAM_BLUE ) )
			{
				int iWinLoseTiePoint = GetWinLoseTiePoint( TEAM_BLUE, eWinTeam, (float)dwBluePlayTime / m_dwModeRecordPointTime );		
				if( iWinLoseTiePoint != 0 || m_iBlueKillCount != 0 || m_iBlueDeathCount != 0 )
				{
					int iWinLoseTieType = LADDER_TEAM_RESULT_WIN;
					if( TEAM_RED == eWinTeam )
						iWinLoseTieType = LADDER_TEAM_RESULT_LOSE;
					else if( TEAM_NONE == eWinTeam )
						iWinLoseTieType = LADDER_TEAM_RESULT_TIE;						

					kMainPacket << pBlueLadderTeam->GetGuildIndex() << iWinLoseTieType << iWinLoseTiePoint << m_iBlueKillCount << m_iBlueDeathCount;
				}		
			}
			m_iBlueKillCount = m_iBlueDeathCount = 0;
			bHeroMatch = pBlueLadderTeam->IsHeroMatchMode();
		}

		if( pRedLadderTeam )
		{
			pRedLadderTeam->UpdateRecord( TEAM_RED, eWinTeam );

			// 길드팀이면 길드 전적 업데이트
			if( m_pCreator->IsLadderGuildTeam( TEAM_RED ) )
			{
				int iWinLoseTiePoint = GetWinLoseTiePoint( TEAM_RED, eWinTeam, (float)dwRedPlayTime / m_dwModeRecordPointTime );		
				if( iWinLoseTiePoint != 0 || m_iRedKillCount != 0 || m_iRedDeathCount != 0 )
				{
					int iWinLoseTieType = LADDER_TEAM_RESULT_WIN;
					if( TEAM_BLUE == eWinTeam )
						iWinLoseTieType = LADDER_TEAM_RESULT_LOSE;
					else if( TEAM_NONE == eWinTeam )
						iWinLoseTieType = LADDER_TEAM_RESULT_TIE;	

					kMainPacket << pRedLadderTeam->GetGuildIndex() << iWinLoseTieType << iWinLoseTiePoint << m_iRedKillCount << m_iRedDeathCount;
				}
			}
			m_iRedKillCount = m_iRedDeathCount = 0;
			bHeroMatch = pRedLadderTeam->IsHeroMatchMode();
		}
		// 메인서버에 결과 전송
		g_MainServer.SendMessage( kMainPacket );
	}
	// 랭킹 정렬.
	g_LadderTeamManager.SortLadderTeamRank( bHeroMatch );
	m_pCreator->SetLadderCurrentRank();
}

void Mode::UpdateLadderBattlePoint( TeamType eWinTeam, DWORD dwModePlayTime, BOOL bSameCampe )
{
	if( m_pCreator->GetRoomStyle() != RSTYLE_LADDERBATTLE ) return;
	if( GetModeType() == MT_SURVIVAL || GetModeType() == MT_BOSS || GetModeType() == MT_GANGSI || GetModeType() == MT_FIGHT_CLUB ) return;	
	if( m_dwModeRecordPointTime == 0 ) return;
	if( eWinTeam != TEAM_BLUE && eWinTeam != TEAM_RED ) return;
	if( dwModePlayTime == 0 ) return;

	// 래더 포인트 & 래더 경험치 분배
	int i = 0;
	int iRecordCnt = GetRecordCnt();
	for(i = 0;i < iRecordCnt;i++)
	{
		ModeRecord *pRecord = FindModeRecord( i );
		if( !pRecord ) continue;
		if( !pRecord->pUser ) continue;
		if( IsAbuseUser( i ) ) continue;
		if( pRecord->pUser->GetTeam() != TEAM_BLUE && pRecord->pUser->GetTeam() != TEAM_RED ) continue;

		float fModeLadderPoint = GetModeLadderPoint( pRecord->pUser, eWinTeam, (float)dwModePlayTime / m_dwModeRecordPointTime );

		if( bSameCampe )
			fModeLadderPoint = fModeLadderPoint * m_fSameCampPenalty;

		if( fModeLadderPoint < 0.0f )
		{
			fModeLadderPoint = 0.0f;
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Mode::UpdateLadderBattlePoint 진영포인트 마이너스 발생!!!! [%.2f]", fModeLadderPoint );
		}
		int iUserLadderPoint  = pRecord->pUser->GetLadderPoint();
		pRecord->iTotalLadderPoint = fModeLadderPoint;
		pRecord->pUser->AddLadderPoint( fModeLadderPoint );
		LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "Mode::UpdateLadderBattlePoint LoseTeam[%d][%d] : [%s] [%d] [%d] [%.2f]", (int)eWinTeam, (int)pRecord->pUser->GetTeam(), 
								pRecord->pUser->GetPublicID().c_str(), iUserLadderPoint, pRecord->pUser->GetLadderPoint(), fModeLadderPoint );

		// 래더 경험치 - 이긴팀만 지급
		if( eWinTeam == pRecord->pUser->GetTeam() )
			pRecord->pUser->AddHeroExpert( m_fModeHeroExpert );
	}
/*	
	// 기본 모드 래더 포인트
	float fModeLadderPoint = GetLadderPoint( (float)dwModePlayTime / m_dwModeRecordPointTime );
	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "Mode::UpdateLadderBattlePoint : [%d] [%.2f] [%.2f] [%.2f]", dwModePlayTime, fModeLadderPoint, m_fBlueReserveLadderPoint, m_fRedReserveLadderPoint );
	//////////////////////////////////////////////////////////////////////////

	// 진팀의 래더 포인트를 빼서 딜 한다.
	float fLoseTeamTotalPoint = 0.0f;
	int i = 0;
	int iRecordCnt = GetRecordCnt();
	for(i = 0;i < iRecordCnt;i++)
	{
		ModeRecord *pRecord = FindModeRecord( i );
		if( !pRecord ) continue;
		if( !pRecord->pUser ) continue;
		if( pRecord->pUser->GetTeam() != TEAM_BLUE && pRecord->pUser->GetTeam() != TEAM_RED ) continue;
		
        if( pRecord->pUser->GetTeam() != eWinTeam )
		{
			int iUserLadderPoint  = pRecord->pUser->GetLadderPoint();
			int iMinusLadderPoint = min( iUserLadderPoint, (int)fModeLadderPoint );
			fLoseTeamTotalPoint += iMinusLadderPoint;
			
			pRecord->iTotalLadderPoint = -iMinusLadderPoint;
			pRecord->pUser->AddLadderPoint( -iMinusLadderPoint );
			LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "Mode::UpdateLadderBattlePoint LoseTeam[%d][%d] : [%s] [%d] [%d]", (int)eWinTeam, (int)pRecord->pUser->GetTeam(), 
									pRecord->pUser->GetPublicID().c_str(), iUserLadderPoint, pRecord->pUser->GetLadderPoint() );
		}
	}
	
	// 예약된 래더 포인트 추가
	if( eWinTeam == TEAM_BLUE )
		fLoseTeamTotalPoint += m_fRedReserveLadderPoint;
	else
		fLoseTeamTotalPoint += m_fBlueReserveLadderPoint;

	// 래더포인트가 0이면 분배 필요 없음
	if( fLoseTeamTotalPoint == 0.0f ) return;

	// 이긴팀에게 분배
	int iWinTeamUserCount = GetTeamUserCnt( eWinTeam );
	float fWinTeamLadderPoint = fLoseTeamTotalPoint / max( 1, iWinTeamUserCount );
	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "Mode::UpdateLadderBattlePoint Total Ladder Point : %.2f / %d = %.2f", fLoseTeamTotalPoint, iWinTeamUserCount, fWinTeamLadderPoint );

	for(i = 0;i < iRecordCnt;i++)
	{
		ModeRecord *pRecord = FindModeRecord( i );
		if( !pRecord ) continue;
		if( !pRecord->pUser ) continue;
		if( IsAbuseUser( i ) ) continue;
		if( pRecord->pUser->GetTeam() != TEAM_BLUE && pRecord->pUser->GetTeam() != TEAM_RED ) continue;

		if( pRecord->pUser->GetTeam() == eWinTeam )
		{
			int iUserLadderPoint = pRecord->pUser->GetLadderPoint();
			pRecord->iTotalLadderPoint = fWinTeamLadderPoint;
			pRecord->pUser->AddLadderPoint( fWinTeamLadderPoint );
			LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "Mode::UpdateLadderBattlePoint WinTeam[%d][%d] : [%s] [%d] [%d]", (int)eWinTeam, (int)pRecord->pUser->GetTeam(), 
									pRecord->pUser->GetPublicID().c_str(), iUserLadderPoint, pRecord->pUser->GetLadderPoint() );
		}
	}
*/
}

bool Mode::IsAbuseUser( int iIdx )
{
	ModeRecord *pRecord = FindModeRecord( iIdx );
	if( !pRecord )	return true;

	if( pRecord->iTotalDamage <= m_iAbuseMinDamage )
		return true;

	return false;
}

bool Mode::IsAbuseUser( ModeRecord *pRecord )
{
	if( pRecord == NULL ) return true;

	if( pRecord->iTotalDamage <= m_iAbuseMinDamage )
		return true;
	return false;
}

float Mode::GetResultScoreGapValue( bool bLadderPoint, TeamType eWinTeam )
{
	// 포로 + 데스매치 + 보스모드 + 상징물 + 히든킹
	float fScoreGap = 0.0f;
	if( bLadderPoint )
	{
		fScoreGap = (float)abs( m_iBlueTeamWinCnt - m_iRedTeamWinCnt ) * m_fLadderScoreCorrection;
	}
	else
	{
		fScoreGap = (float)abs( m_iBlueTeamWinCnt - m_iRedTeamWinCnt ) * m_fScoreCorrection;
	}	
	return fScoreGap;
}

float Mode::GetUserCorrection( TeamType eWinTeam, float fRoundPoint, float fScoreGap )
{
	float fUserCorrection = 1.0f;
	if( GetModeType() != MT_SURVIVAL && GetModeType() != MT_BOSS && GetModeType() != MT_GANGSI && GetModeType() != MT_FIGHT_CLUB && GetModeType() != MT_SHUFFLE_BONUS )      //서바이벌은 제외
	{
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
	}
	return fUserCorrection;
}

void Mode::GetModeInfo( SP2Packet &rkPacket )
{
	rkPacket << GetModeType();

	rkPacket << m_iCurRound;
	rkPacket << m_iMaxRound;

	rkPacket << m_dwRoundDuration;

	rkPacket << m_iBlueTeamWinCnt;
	rkPacket << m_iRedTeamWinCnt;

	rkPacket << m_iBluePosArray;
	rkPacket << m_iRedPosArray;	
}

void Mode::GetExtraModeInfo( SP2Packet &rkPacket )
{
}

void Mode::GetModeHistory( SP2Packet &rkPacket )
{
	int i = 0;
	int HistorySize = m_vRoundHistory.size();
	if( m_iCurRound > HistorySize )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Mode::GetModeInfo() %d != %d", m_iCurRound, HistorySize );
		
		for( i = 0; i < m_iCurRound; i++ )	
		{
			RoundHistory rh;
			rkPacket << rh.iBluePoint << rh.iRedPoint;
		}
	}
	else
	{
		for( i = 0; i < m_iCurRound; i++ )	
		{
			RoundHistory rh = m_vRoundHistory[i];
			rkPacket << rh.iBluePoint << rh.iRedPoint;
		}
	}
}

void Mode::SetRoundEndInfo( WinTeamType eWinTeam )
{
	m_CurRoundWinTeam = eWinTeam;

	if( m_iCurRound == m_iMaxRound ||
		GetTeamUserCnt( TEAM_BLUE ) == 0 ||
		GetTeamUserCnt( TEAM_RED ) == 0 )
	{
		m_bRoundSetEnd = true;
	}

	m_bCheckContribute = false;
	m_bCheckAwardChoose = false;
	m_bCheckAllRecvDamageList = false;
	m_bCheckSuddenDeathContribute = false;
	SetModeState( MS_RESULT_WAIT );

	UpdateRoundRecord();

	m_vPushStructList.clear();
	m_vBallStructList.clear();
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
		}
		if( pRecord->pUser && pRecord->pUser->GetStartTimeLog() > 0 )
		{
			if( pRecord->eState != RS_VIEW && pRecord->eState != RS_OBSERVER )
				pRecord->AddDeathTime( TIMEGETTIME() - pRecord->pUser->GetStartTimeLog() );
			else
				g_LogDBClient.OnInsertTime( pRecord->pUser, LogDBClient::TT_VIEW );
			pRecord->pUser->SetStartTimeLog(0);
		}
	}

	ClearObjectItem();
}

void Mode::SendRoundResult( WinTeamType eWinTeam )
{
	SP2Packet kPacket( STPK_ROUND_END );
	kPacket << eWinTeam;
	kPacket << m_iRedTeamWinCnt;
	kPacket << m_iBlueTeamWinCnt;
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
				pRecord->pUser->IncreaseMyVictories( IsWinTeam( eWinTeam, pRecord->pUser->GetTeam() ) );

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

void Mode::FillResultSyncUser( SP2Packet &rkPacket )
{
	// 최종 기여도와 시상 내역을 서버로 전송해줄 유저를 선정한다.
	// 안전하게 2명의 유저를 선정하고 먼저 온 유저 정보를 사용.
	const int iMaxListUser = 2;
	ioHashStringVec vSyncUserList;
	int iRecordCnt = GetRecordCnt();
	int i = 0;
	for(i = 0;i < iRecordCnt;i++)
	{
		ModeRecord *pRecord = FindModeRecord( i );
		if( !pRecord || pRecord->eState == RS_LOADING ) continue;
		if( !pRecord->pUser ) continue;
        
		vSyncUserList.push_back( pRecord->pUser->GetPublicID() );
		
		if( vSyncUserList.size() >= iMaxListUser )
			break;
	}
	int iSyncList = vSyncUserList.size();
	rkPacket << iSyncList;
	for(i = 0;i < iSyncList;i++)
		rkPacket << vSyncUserList[i];
}

float Mode::GetTotalVictoriesRate()
{
	int iRecordCnt = GetRecordCnt();
	float fTotalUserCnt = 0.0f;
	float fTotalVictories = 0.0f;
	for(int i=0; i < iRecordCnt; ++i )
	{
		if( IsPlayingUser(i) )
		{
			ModeRecord *pRecord = FindModeRecord( i );
			if( !pRecord )	continue;

			User *pUser = pRecord->pUser;
			if( !pUser ) continue;

			if( pUser->IsObserver() ) continue;
			if( pUser->IsStealth() ) continue;

			if( m_dwModePointTime == 0 || m_dwModeRecordPointTime == 0 )
				continue;

			if( IsAbuseUser( i ) ) continue;

			fTotalUserCnt += 1.0f;

			float fCurVictories = 0.0f;
			int iCurVictoriesCnt = pRecord->iVictories;
			if( pUser->IsLadderTeam() )
				iCurVictoriesCnt = pUser->GetMyVictories();

			int iGapVictories = iCurVictoriesCnt - Help::GetFirstVictories();
			if( iGapVictories >= 0 )
			{
				fCurVictories = Help::GetFirstVictoriesRate() + Help::GetVictoriesRate() * iGapVictories;
				fCurVictories = min( Help::GetMaxVictoriesRate(), fCurVictories );
			}
			fTotalVictories += 1.0f + fCurVictories;
		}
	}

	float fTotalVictoriesRate = 1.0f;
	if( fTotalVictories > 0.0f )
		fTotalVictoriesRate = fTotalUserCnt / fTotalVictories;
	return fTotalVictoriesRate;
}

float Mode::GetTotalModeConsecutivelyRate()
{
	int iRecordCnt = GetRecordCnt();
	float fTotalUserCnt = 0.0f;
	float fTotalConsecutively = 0.0f;
	for(int i=0; i < iRecordCnt; ++i )
	{
		if( IsPlayingUser(i) )
		{
			ModeRecord *pRecord = FindModeRecord( i );
			if( !pRecord )	continue;

			User *pUser = pRecord->pUser;
			if( !pUser ) continue;

			if( pUser->IsObserver() ) continue;
			if( pUser->IsStealth() ) continue;

			if( m_dwModePointTime == 0 || m_dwModeRecordPointTime == 0 )
				continue;

			if( IsAbuseUser( i ) ) continue;

			fTotalUserCnt += 1.0f;
			fTotalConsecutively += 1.0f + pUser->GetModeConsecutivelyBonus();
		}
	}

	float fTotalConsecutivelyRate = 1.0f;
	if( fTotalConsecutively > 0.0f )
		fTotalConsecutivelyRate = fTotalUserCnt / fTotalConsecutively;
	return fTotalConsecutivelyRate;
}

void Mode::FinalRoundProcess()
{
	if( !m_bRoundSetEnd )	return;

	SetModeEndDBLog( m_pCreator, LogDBClient::PRT_END_SET );

	int iRecordCnt = GetRecordCnt();
	int i = 0;
	ModeType eModeType = GetModeType();
	if( eModeType == MT_CATCH ||
		eModeType == MT_HEROMATCH ||
		eModeType == MT_CATCH_RUNNINGMAN ||
		eModeType == MT_UNDERWEAR ||
		eModeType == MT_FARMING ||  		// 2019-07-03 by bckim, 파밍모드 추가
		eModeType == MT_CBT ||
		eModeType == MT_SUCCESSION )
	{
		m_iMaxRound = m_iCurRound;
	}

	//
    UpdateBattleRoomRecord();
	UpdateLadderBattleRecord();
	UpdateTournamentRecord();
	UpdateShuffleRoomRecord();

	// 결과 승무패 및 경험치, 페소 지급
	float fTotalVictoriesRate = GetTotalVictoriesRate();
	float fTotalConsecutivelyRate = GetTotalModeConsecutivelyRate();

	ShuffleRoomNode *pShuffleNode = NULL;
	
	for(int i = 0 ; i < iRecordCnt; i++ )
	{
		if( IsPlayingUser(i) )
		{
			ModeRecord *pRecord = FindModeRecord( i );
			if( !pRecord )
				continue;

			if( !pRecord->pUser )
				continue;

			bool bAbuseUser = IsAbuseUser( i );

			FinalRoundPoint( pRecord, bAbuseUser, fTotalVictoriesRate, fTotalConsecutivelyRate );
		}
	}
	
	//결과 전송전 미리 전송
	FinalRoundProcessByShuffle();

	// 결과 전송
	DWORD dwServerDate = Help::ConvertCTimeToDate( CTime::GetCurrentTime() );
	for(int i = 0; i < iRecordCnt ; i++ )
	{
		if( IsPlayingUser(i) )
		{
			ModeRecord *pRecord = FindModeRecord( i );
			if( !pRecord )
				continue;

			if( !pRecord->pUser )
				continue;

			FinalRoundResult( pRecord, dwServerDate );	
		}
	}
}

void Mode::FinalRoundProcessByShuffle()
{	
	for( int i = 0; i < GetRecordCnt(); ++i )
	{
		ModeRecord *pRecord = FindModeRecord( i );
		if( !pRecord )
			continue;

		if( !pRecord->pUser )
			continue;
			
		//오늘의 모드 결과 전송
		ShuffleRoomNode* pNode = dynamic_cast<ShuffleRoomNode*>( pRecord->pUser->GetMyShuffleRoom() );
		if( pNode )
			pNode->FinalRoundhufflePlayPoint( this, pRecord );		
	}
}

TeamType Mode::GetWinTeam()
{	
	if( m_iBlueTeamWinCnt > m_iRedTeamWinCnt )
		return TEAM_BLUE;
	else if( m_iRedTeamWinCnt > m_iBlueTeamWinCnt )
		return TEAM_RED;

	return TEAM_NONE;
}

void Mode::FinalRoundPoint( ModeRecord *pRecord, bool bAbuseUser, float fTotalVictoriesRate, float fTotalConsecutivelyRate )
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
	
	TeamType eWinTeam  = GetWinTeam();

	//승무패 기록
	int iWinLoseTiePoint = 0;
	if( !bAbuseUser )
	{
		if( GetModeType() == MT_SURVIVAL || GetModeType() == MT_BOSS || GetModeType() == MT_FIGHT_CLUB || GetModeType() == MT_SHUFFLE_BONUS )
		{
			iWinLoseTiePoint = GetWinLoseTiePoint( pRecord->fContributePer, (float)GetRecordPlayTime( pRecord ) / m_dwModeRecordPointTime );
			if( pRecord->fContributePer >= 1.0f )
				pUser->AddWinCount( m_pCreator->GetRoomStyle(), GetModeType(), iWinLoseTiePoint );
			else
				pUser->AddLoseCount( m_pCreator->GetRoomStyle(), GetModeType(), iWinLoseTiePoint );
		}
		else if( GetModeType() == MT_GANGSI )
		{
			iWinLoseTiePoint = GetWinLoseTiePoint( pUser->GetTeam(), eWinTeam, (float)GetRecordPlayTime( pRecord ) / m_dwModeRecordPointTime );
			// 승무패 기록 하지 않음
			// 스코어가 다른 모드랑 다름
			if( m_iRedTeamWinCnt > m_iBlueTeamWinCnt )
				eWinTeam = TEAM_RED;
			else
				eWinTeam = TEAM_BLUE;
		
		}
		else if( GetModeType() == MT_FOOTBALL )
		{
			iWinLoseTiePoint = GetWinLoseTiePoint( pUser->GetTeam(), eWinTeam, (float)GetRecordPlayTime( pRecord ) / m_dwModeRecordPointTime );
			// 승무패 기록 하지 않음
		}
		else
		{
			iWinLoseTiePoint = GetWinLoseTiePoint( pUser->GetTeam(), eWinTeam, (float)GetRecordPlayTime( pRecord ) / m_dwModeRecordPointTime );
			if( eWinTeam == pUser->GetTeam() )
			{
				pUser->AddWinCount(  m_pCreator->GetRoomStyle(), GetModeType(), iWinLoseTiePoint );				
			}
			else if( eWinTeam != TEAM_NONE )
			{
				// 패배
				if( GetModeType() == MT_HEROMATCH )
				{
					// 래더전에서 중간 종료되면 패배처리 안함.
					if( m_iBlueTeamWinCnt >= m_iNeedRound || m_iRedTeamWinCnt >= m_iNeedRound )
						pUser->AddLoseCount(  m_pCreator->GetRoomStyle(), GetModeType(), iWinLoseTiePoint );
				}
				else
				{
					pUser->AddLoseCount(  m_pCreator->GetRoomStyle(), GetModeType(), iWinLoseTiePoint );
				}
			}
		}
	}							
	else if( bAbuseUser )
	{
		if( GetModeType() == MT_HEROMATCH )
		{
			// 래더전은 어뷰즈 유저여도 패는 적용한다.			
			if( eWinTeam == pUser->GetTeam() )
			{	
				// 승은 어뷰즈 기록 안함
			}
			else if( eWinTeam != TEAM_NONE )
			{
				// 패배
				iWinLoseTiePoint = GetWinLoseTiePoint( pUser->GetTeam(), eWinTeam, (float)GetRecordPlayTime( pRecord ) / m_dwModeRecordPointTime );
				// 래더전에서 중간 종료되면 패배처리 안함.
				if( m_iBlueTeamWinCnt >= m_iNeedRound || m_iRedTeamWinCnt >= m_iNeedRound )
					pUser->AddLoseCount(  m_pCreator->GetRoomStyle(), GetModeType(), iWinLoseTiePoint );
			}
		}		
	}
	//플레이 중인 모드
	ModeCategory ePlayMode = GetPlayModeCategory();

	//총 라운드 수 A
	float fRoundPoint = m_ModePointRound;			
	//스코어 차이  B
	float fScoreGap   = GetResultScoreGapValue( false, eWinTeam );        
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
		if( pUser->IsDeveloper() )
		{
			LOG.PrintTimeAndLog(0 ,"%s - pcroom_bouns - result:  %f = bouns(%f) * user count(%f)", 
				__FUNCTION__,
				pRecord->fBonusArray[BA_FRIEND],
				GetPcRoomFriendBonus(),
				(float)GetSameFriendUserCnt( pUser )  );
		}
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

	// 연속 모드 보너스 T
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
	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "[%d] %s [%d]: %s EXP: ( %.2f %s %.2f ) x %.2f x %.4f x %.2f x %.2f x ( %.2f + %.2f + %.2f + %.2f + %.2f + %.2f + %.2f + %.2f + %.2f + %.2f ) x %.2f = %.2f",
		                    m_pCreator->GetRoomIndex(), pUser->GetPublicID().c_str(), (int)bAbuseUser, szLogArguWinLose, 
		                    fRoundPoint, szLogArguPlusMinus, fScoreGap, fUserCorrection, fPlayTimeCorrection, fExpCorrection, fBlockPoint, fContributePer, fGuildBonus, fSoldierCntBonus, fPCRoomBonusExp, fModeBonus, fFriendBonusPer, fEventBonus, fEtcItemBonus, fCampBattleBonus, fEtcItemExpBonus, fModeConsecutivelyBonus, fAcquireExp );

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

	//연승 보너스
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
	}*/

	//pRecord->fBonusArray[BA_VICTORIES_PESO] = fCurVictories;

	float fVictoriesBonus = (1.0f + fCurVictories) * fTotalVictoriesRate;
	fAcquirePeso = fAcquirePeso * fVictoriesBonus;
	fAcquirePeso = fAcquirePeso * fModeConsecutivelyBonus;

	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[%d] %s [%d]: %s PESO : (( %.2f %s %.2f ) x %.2f x %.4f x %.2f x %.2f x ( %.2f + %.2f + %.2f + %.2f + %.2f + %.2f + %.2f + %.2f + %.2f + %.2f + %.2f ) x %.2f x %.2f ) = %.2f",
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

#ifdef ARENA_MODE_BY_BCKIM		// 2018-07-25 by bckim, 아레나 모드 추가			_경험치
		if( pUser->IsClassTypeExerciseStyle(pRecord->iResultClassTypeList[i], EXERCISE_RENTAL) == false && pUser->GetModeType() != MT_ARENA)
		{
			pUser->AddClassExp( pRecord->iResultClassTypeList[i], pRecord->iResultClassPointList[i] + fSoldierExpBonus );
		}

		if( pUser->AddGradeExp( pRecord->iResultClassPointList[i] ) )
			pRecord->bResultLevelUP = true;

		pRecord->iResultClassPointList[i] += fSoldierExpBonus;

		if( pUser->IsClassTypeExerciseStyle( pRecord->iResultClassTypeList[i], EXERCISE_RENTAL) || pUser->GetModeType() == MT_ARENA)
		{	// 계급 경험치만 획득하고 용병 경험치는 획득 안됨
			pRecord->iResultClassPointList[i] = 0;
		}		
#else
		// 경험치 지급 및 레벨업 확인
		if( pUser->IsClassTypeExerciseStyle( pRecord->iResultClassTypeList[i], EXERCISE_RENTAL ) == false )
			pUser->AddClassExp( pRecord->iResultClassTypeList[i], pRecord->iResultClassPointList[i] + fSoldierExpBonus );
		if( pUser->AddGradeExp( pRecord->iResultClassPointList[i] ) )
			pRecord->bResultLevelUP = true;

		pRecord->iResultClassPointList[i] += fSoldierExpBonus;

		if( pUser->IsClassTypeExerciseStyle( pRecord->iResultClassTypeList[i], EXERCISE_RENTAL ) )
		{
			// 계급 경험치만 획득하고 용병 경험치는 획득 안됨
			pRecord->iResultClassPointList[i] = 0;
		}
#endif
	}

	// 용병단과 용병들의 레벨업 보상을 지급한다.
	pRecord->iTotalPeso += pUser->GradeNClassUPBonus();	

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
}

void Mode::FinalRoundResult( ModeRecord *pRecord, DWORD dwServerDate )
{	
	if( !pRecord || !pRecord->pUser )
		return;

	int i = 0;
	User *pUser = pRecord->pUser;

	SP2Packet kPacket( STPK_FINAL_ROUND_RESULT );
	// 최종 결과 시간( 결과 브리핑 + 시상식 + 최종 결과 )
	PACKET_GUARD_VOID( kPacket.Write(m_dwFinalResultStateTime+m_dwAwardingTime) );

	if( m_pCreator->GetRoomStyle() == RSTYLE_LADDERBATTLE )
	{
		//각 팀의 랭킹 전송
		m_pCreator->FillLadderTeamRank( kPacket );
		//각 팀의 길드 보너스 전송
		PACKET_GUARD_VOID( kPacket.Write(GetLadderGuildTeamBonus(TEAM_BLUE)) );
		PACKET_GUARD_VOID( kPacket.Write(GetLadderGuildTeamBonus(TEAM_RED)) );
		//래더전 경험치
		PACKET_GUARD_VOID( kPacket.Write(pUser->GetHeroExp()) );
	}

	pUser->FillFinalRoundResult( m_pCreator->GetRoomStyle(), GetModeType(), kPacket );
	
	// 용병 획득 경험치
	int iExpCharSize = pRecord->iResultClassTypeList.size();

	PACKET_GUARD_VOID( kPacket.Write(iExpCharSize) );
	for(i = 0;i < iExpCharSize;i++)
	{
		PACKET_GUARD_VOID( kPacket.Write(pRecord->iResultClassTypeList[i]) );
		PACKET_GUARD_VOID( kPacket.Write(pRecord->iResultClassPointList[i]) );
	}
	
	// 내 정보.
	PACKET_GUARD_VOID( kPacket.Write(pUser->GetMoney()) );
	for (i = 0; i < BA_MAX ; i++)
	{
		PACKET_GUARD_VOID( kPacket.Write(pRecord->fBonusArray[i]) );
	}
	
	// 플레이중인 유저 결과 정보 
	int iPlayUserCnt = 0;
	int iRecordCnt = GetRecordCnt();
	for(int i=0 ; i<iRecordCnt ; i++ )
	{
		ModeRecord *pResultRecord = FindModeRecord( i );
		if( !pResultRecord || pResultRecord->eState == RS_LOADING )
			continue;

		iPlayUserCnt++;
	}
	PACKET_GUARD_VOID( kPacket.Write(iPlayUserCnt) );
	for(int i=0 ; i<iRecordCnt ; i++ )
	{
		ModeRecord *pResultRecord = FindModeRecord( i );
		if( !pResultRecord || pResultRecord->eState == RS_LOADING )
			continue;		
		
		//( 아이디 - 경험치 - 페소 - 레벨업 - 레벨 - 현재 래더 포인트 - 획득 래더포인트 )
		PACKET_GUARD_VOID( kPacket.Write(pResultRecord->pUser->GetPublicID()) );
		PACKET_GUARD_VOID( kPacket.Write(pResultRecord->iTotalExp) );
		PACKET_GUARD_VOID( kPacket.Write(pResultRecord->iTotalPeso) );
		PACKET_GUARD_VOID( kPacket.Write(pResultRecord->bResultLevelUP) );
		PACKET_GUARD_VOID( kPacket.Write(pResultRecord->pUser->GetGradeLevel()) );
		PACKET_GUARD_VOID( kPacket.Write(pResultRecord->pUser->GetLadderPoint()) );
		PACKET_GUARD_VOID( kPacket.Write(pResultRecord->iTotalLadderPoint) );
	}

	PACKET_GUARD_VOID( kPacket.Write(false) );	// 다음 모드 모름

	if( pRecord->iTotalExp <= 0 )
	{
		PACKET_GUARD_VOID( kPacket.Write(-1) );
		PACKET_GUARD_VOID( kPacket.Write(0) );
	}
	else
	{
		int iRewardSpirit = g_SpiritManager.GetModeReward( GetModeType() );
		int iRewardSpiritQt = g_SpiritManager.GetModeRewardQuantity();
		PACKET_GUARD_VOID( kPacket.Write(iRewardSpirit) );
		PACKET_GUARD_VOID( kPacket.Write(iRewardSpiritQt) );
		pRecord->pUser->SetModeRewardSpirit( iRewardSpirit, iRewardSpiritQt, GetModeType() );
	}

	pUser->SendMessage( kPacket );
}

Vector3 Mode::GetRandomItemPos(ioItem *pItem)
{
	if( m_vItemCreatePosList.empty() )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Mode::GetRandomItemPos() - PosList is Empty!" );
		return Vector3( 0.0f, 0.0f, 0.0f );
	}

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

int Mode::GetRevivalGapTime( int iRevivalCnt )
{
	DWORD dwRevivalTime;

	if( m_pCreator->IsSafetyLevelRoom() )
	{
		dwRevivalTime = m_dwDefaultRevivalTime;
	}
	else
	{
		dwRevivalTime = m_dwDefaultRevivalTime + (m_dwIncreaseRevivalTime*iRevivalCnt);
		dwRevivalTime = min( m_dwMaxRevivalTime, dwRevivalTime );
	}

	return dwRevivalTime;
}

void Mode::LoadObjectGroupList( ioINILoader &rkLoader )
{
	InitObjectGroupList();
}

DWORD Mode::GetRecordPlayTime( ModeRecord *pRecord )
{
	if( !pRecord ) return 0;
	
	return min( pRecord->GetAllPlayingTime(), m_dwModePointMaxTime );
}

bool Mode::IsUserPlayState( User *pUser )
{
	ModeRecord *pRecord = FindModeRecord( pUser );
	if( !pRecord )	return false;

	if( pRecord->eState == RS_PLAY )
		return true;

	return false;
}

void Mode::SendRoomAllUser( SP2Packet &rkPacket, User *pSend )
{
	if( m_pCreator )
	{
		m_pCreator->RoomSendPacketTcp( rkPacket, pSend );
	}
}

void Mode::SendRoomPlayUser( SP2Packet &rkPacket )
{
	int iRecordCnt = GetRecordCnt();
	for( int i=0 ; i<iRecordCnt ; i++ )
	{
		ModeRecord *pRecord = FindModeRecord( i );
		if( pRecord && pRecord->eState != RS_LOADING )
		{
			pRecord->pUser->SendMessage( rkPacket );
		}
	}
}

void Mode::SendRoomPlayUser( SP2Packet &rkPacket, const ioHashString &rkPassName, bool bSendObserver )
{
	int iRecordCnt = GetRecordCnt();
	for( int i=0 ; i<iRecordCnt ; i++ )
	{
		ModeRecord *pRecord = FindModeRecord( i );
		if( !pRecord ) continue;
		if( !pRecord->pUser ) continue;
		if( pRecord->eState == RS_LOADING ) continue;
		if( pRecord->pUser->GetPublicID() == rkPassName ) continue;
		if( !bSendObserver && pRecord->pUser->IsObserver() ) continue;

		pRecord->pUser->SendMessage( rkPacket );
	}
}

bool Mode::ProcessTCPPacket( User *pSend, SP2Packet &rkPacket )
{
	switch( rkPacket.GetPacketID() )
	{
	case CTPK_EXIT_ROOM:
		OnExitRoom( pSend, rkPacket );
		return true;
	case CTPK_PUSHSTRUCT_CREATE:
		OnPushStructCreate( pSend, rkPacket );
		return true;
	case CTPK_PUSHSTRUCT_DIE:
		OnPushStructDie( pSend, rkPacket );
		return true;
	case CTPK_CREATE_OBJECTITEM:
		OnCreateObjectItem( pSend, rkPacket );
		return true;
	case CTPK_CREATE_FIELD_ITEM_BY_USE_SKILL:		// 2019-03-11 by bckim, 티메가테 
		OnCreateFieldItemByUseSkill( pSend, rkPacket );
		return true;								// End. 2019-03-11 by bckim, 티메가테 
	case CTPK_CURRENT_DAMAGELIST:
		OnCurrentDamageList( pSend, rkPacket );
		return true;
	case CTPK_EVENT_SCENE_END:
		OnEventSceneEnd( pSend, rkPacket );
		return true;
	case CTPK_REPOSITION_FIELDITEM:
		OnRepositionFieldItem( rkPacket );
		return true;
	case CTPK_CATCH_CHAR:
		OnCatchChar( pSend, rkPacket );
		return true;
	case CTPK_ESCAPE_CATCH_CHAR:
		OnEscapeCatchChar( pSend, rkPacket );
		return true;
	case CTPK_PLAYRECORD_INFO:
		OnPlayRecordInfo( pSend, rkPacket );
		return true;
	case CTPK_LAST_PLAYRECORD_INFO:
		OnLastPlayRecordInfo( pSend, rkPacket );
		return true;
	case CTPK_AWARDING_INFO:
		OnAwardingInfo( pSend, rkPacket );
		return true;
	case CTPK_AWARDING_RESULT:
		OnAwardingResult( pSend, rkPacket );
		return true;
	case CTPK_ABSORB_REQUEST:
		OnAbsorbInfo( pSend, rkPacket );
		return true;
	case CTPK_CHAT_MODE:
		OnChatModeState( pSend, rkPacket );
		return true;
	case CTPK_EXPERIENCE_MODE:
		OnExperienceState( pSend, rkPacket );
		return true;
	case CTPK_USER_KICK_VOTE:
		OnUserKickVote( pSend, rkPacket );
		return true;
	case CTPK_BALLSTRUCT_REPOSITION:
		OnRepositionBallStruct( rkPacket );
		return true;
	case CTPK_MACHINESTRUCT:
		OnMachineStruct( pSend, rkPacket );
		return true;
	case CTPK_PUSHSTRUCT_OWNER_CLEAR:
		OnPushStructDieByOwnerClear( pSend, rkPacket );
		return true;
	case CTPK_DROP_ITEM:
		OnDropItem( pSend, rkPacket );
		return true;
	case CTPK_ITEM_MOVE_DROP:
		OnDropMoveItem( pSend, rkPacket );
		return true;
	case CTPK_NPC_SPAWN_SKILL:
		OnNpcSpawn( pSend, rkPacket);
		return true;
	case CTPK_TOURNAMENT_SUDDEN_DEATH:
		OnSuddenDeathPlayRecordInfo( pSend, rkPacket );
		return true;
	case CTPK_ROUND_END:
		OnRoundEnd( pSend, rkPacket );
		return true;
	case CTPK_ROUND_END_CONTRIBUTE:
		OnRoundEndContribute( pSend, rkPacket );
		return true;
	case CTPK_CREATE_MODE_ITEM:
		OnCreateModeItem( rkPacket );
		return true;
	case CTPK_GET_MODE_ITEM:
		OnGetModeItem( rkPacket );
		return true;
	}

	return false;
}

void Mode::DestroyPushStructByLeave( const ioHashString &rkName )
{
	CRASH_GUARD();
	LOOP_GUARD();
	PushStructList::iterator iter = m_vPushStructList.begin();
	while( iter != m_vPushStructList.end() )
	{
		if( (*iter).m_OwnerName == rkName )
			iter = m_vPushStructList.erase( iter );
		else
			iter++;
	}
	LOOP_GUARD_CLEAR();

	SP2Packet kReturn( STPK_PUSHSTRUCT_OWNER_CLEAR );
	
	PACKET_GUARD_VOID( kReturn.Write(rkName) );

	SendRoomAllUser( kReturn );
}

void Mode::OnPushStructDie( User *pSend, SP2Packet &rkPacket )
{
	if( !IsEnableState( pSend ) )
		return;

	int iIndex = 0;
	bool bEffect = false;
	PACKET_GUARD_VOID( rkPacket.Read(iIndex) );
	PACKET_GUARD_VOID( rkPacket.Read(bEffect) );

	int iCnt = m_vPushStructList.size();
	for( int i=0; i < iCnt; i++ )
	{
		if( m_vPushStructList[i].m_iIndex != iIndex )
			continue;

		ioHashString szOwner = m_vPushStructList[i].m_OwnerName;

		m_vPushStructList.erase( m_vPushStructList.begin() + i );

		int iPushCnt = 0, iNum = 0;
		Vector3 vPos;
		Quaternion qtTargetRot;

		int iCreateType = 0;
		ioHashString szKiller;
		PACKET_GUARD_VOID( rkPacket.Read(iPushCnt) );
		MAX_GUARD(iPushCnt, 100);

		SP2Packet kReturn( STPK_PUSHSTRUCT_DIE );
		PACKET_GUARD_VOID( kReturn.Write(1) );
		PACKET_GUARD_VOID( kReturn.Write(iIndex) );
		PACKET_GUARD_VOID( kReturn.Write(bEffect) );
		PACKET_GUARD_VOID( kReturn.Write(iPushCnt) );

		for( int j=0; j < iPushCnt; ++j )
		{
			PACKET_GUARD_VOID( rkPacket.Read(iNum) );
			PACKET_GUARD_VOID( rkPacket.Read(vPos) );
			PACKET_GUARD_VOID( rkPacket.Read(qtTargetRot) );
			PACKET_GUARD_VOID( rkPacket.Read(iCreateType) );

			switch( iCreateType )
			{
			case SCT_NORMAL:
				break;
			case SCT_KILLER:
				{
					PACKET_GUARD_VOID( rkPacket.Read(szKiller) );
					szOwner = szKiller;
				}
				break;
			case SCT_NO_OWNER:
				szOwner.Clear();
				break;
			}

			CreatePushStructByPushStruct( kReturn, iNum, vPos, qtTargetRot, szOwner );
		}

		SendRoomAllUser( kReturn );

		CreateObjectItemByPushStruct( rkPacket );
		break;
	}
}

void Mode::CreatePushStructByPushStruct( SP2Packet &rkReturn, int iNum, Vector3 vPos, Quaternion qtTargetRot, const ioHashString &szOwner )
{
	PushStruct kPush;
	kPush.m_iNum = iNum;
	kPush.m_CreatePos = vPos;
	kPush.m_TargetRot = qtTargetRot;
	kPush.m_OwnerName = szOwner;

	m_iPushStructIdx++;
	kPush.m_iIndex = m_iPushStructIdx;
	kPush.m_dwCreateTime = TIMEGETTIME();

	m_vPushStructList.push_back( kPush );

	DWORD dwSeed = timeGetTime();

	rkReturn << iNum;
	rkReturn << m_iPushStructIdx;
	rkReturn << vPos;
	rkReturn << kPush.m_TargetRot;
	rkReturn << szOwner;
	rkReturn << 0;			// GapTime
	rkReturn << dwSeed;
}

void Mode::OnCreateObjectItem( User *pSend, SP2Packet &rkPacket )
{
	if( !IsEnableState( pSend ) )
		return;

	Vector3 vPosition;
	DWORD dwItemCode = 0;
	ioHashString szCreateItemName;
	ioHashString szOwnerName;
	bool bImmediately;
	int eObjectCreateType = 0;

	PACKET_GUARD_VOID( rkPacket.Read( vPosition ) );
	PACKET_GUARD_VOID( rkPacket.Read( dwItemCode ) );
	PACKET_GUARD_VOID( rkPacket.Read( szCreateItemName ) );
	PACKET_GUARD_VOID( rkPacket.Read( szOwnerName ) );
	PACKET_GUARD_VOID( rkPacket.Read( bImmediately ) );
	PACKET_GUARD_VOID( rkPacket.Read( eObjectCreateType ) );
	
	if( dwItemCode == 0 )
		return;

	ioItem *pItem = m_pCreator->CreateItemByCode( dwItemCode );
	if( pItem )
	{
		if( bImmediately )
		{
			pSend->ImmediatelyEquipItem( pItem, szCreateItemName, eObjectCreateType, rkPacket );
		}
		else
		{
			pItem->SetItemPos( vPosition );

			SP2Packet kPacket( STPK_FIELD_ITEM_SUPPLY );			
			PACKET_GUARD_VOID( kPacket.Write( 1 ) );

			m_pCreator->AddFieldItem( pItem );
			PACKET_GUARD_VOID( kPacket.Write( pItem->GetItemCode() ) );
			PACKET_GUARD_VOID( kPacket.Write( pItem->GetItemReinforce() ) );
			PACKET_GUARD_VOID( kPacket.Write( pItem->GetItemMaleCustom() ) );
			PACKET_GUARD_VOID( kPacket.Write( pItem->GetItemFemaleCustom() ) );
			PACKET_GUARD_VOID( kPacket.Write( pItem->GetGameIndex() ) );
			PACKET_GUARD_VOID( kPacket.Write( pItem->GetItemPos() ) );
			PACKET_GUARD_VOID( kPacket.Write( szOwnerName ) );
			PACKET_GUARD_VOID( kPacket.Write( szCreateItemName ) );

			SendRoomAllUser( kPacket );
		}
	}
}

// 2019-03-11 by bckim, 티메가테 
void Mode::OnCreateFieldItemByUseSkill(User *pSend, SP2Packet &rkPacket )
{
	if( !IsEnableState( pSend ) )
		return;

	Vector3 vPosition[3];
	DWORD dwItemCode = 0;
	ioHashString szOwnerName;

	DWORD dwPackageIndex = 0;
	DWORD dwEtcItemType[3] = { 1011991,1011992,1011993 };		// 갑옷 : 1011991  투구 : 1011992  	망토 : 1011993


	for( int i=0; i<3; i++)
	{
		PACKET_GUARD_VOID( rkPacket.Read( vPosition[i]) );
	}
	PACKET_GUARD_VOID( rkPacket.Read( dwItemCode ) );
	PACKET_GUARD_VOID( rkPacket.Read( szOwnerName ) );

	/*
	const ioItem *pPreItem_temp = pSend->GetItem( EQUIP_ARMOR );
	if(  pPreItem_temp )
	{
		int iArmorItemeCode = pPreItem_temp->GetItemCode();
	}
	*/

	for( int i=0; i<3 ; i++)
	{
		ioEtcItem* pEtcItem = g_EtcItemMgr.FindEtcItem( dwEtcItemType[i] );
		if( pEtcItem )
		{
			pSend->FindSuperGashaponPackageRandomPresent( dwEtcItemType[i],dwPackageIndex);
			ioItem *pItem = m_pCreator->CreateItemByCode( dwPackageIndex );

			if( pItem )
			{
				pItem->SetItemPos( vPosition[i] );
				m_pCreator->AddFieldItem( pItem );
				
				SP2Packet kPacket( STPK_FIELD_ITEM_SUPPLY );			
				PACKET_GUARD_VOID( kPacket.Write( 1 ) );
				PACKET_GUARD_VOID( kPacket.Write( pItem->GetItemCode() ) );
				PACKET_GUARD_VOID( kPacket.Write( pItem->GetItemReinforce() ) );
				PACKET_GUARD_VOID( kPacket.Write( pItem->GetItemMaleCustom() ) );
				PACKET_GUARD_VOID( kPacket.Write( pItem->GetItemFemaleCustom() ) );
				PACKET_GUARD_VOID( kPacket.Write( pItem->GetGameIndex() ) );
				PACKET_GUARD_VOID( kPacket.Write( pItem->GetItemPos() ) );
				PACKET_GUARD_VOID( kPacket.Write( szOwnerName ) );			
				SendRoomAllUser( kPacket );

				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[CREATE_FIELD_ITEM_BY_USE_SKILL] RECV_SERVER [%s][%u][%u]", pSend->GetPublicID().c_str(),dwEtcItemType[i],dwPackageIndex);
			}	
		}
	}
}
// End. 2019-03-11 by bckim, 티메가테 

void Mode::OnAbsorbInfo( User *pSend, SP2Packet &rkPacket )
{
	int i = 0;
	int iMineCnt = 0, iItemCnt = 0, iStructCnt = 0;
	static IntVec vMineList, vItemList, vStructList;
	vMineList.clear();
	vItemList.clear();
	vStructList.clear();

	// 순서 지킬 것 Mine -> Item -> Struct
	// Mine
	PACKET_GUARD_VOID( rkPacket.Read(iMineCnt) );
	MAX_GUARD(iMineCnt, 100);
	for(int i=0; i < iMineCnt; ++i )
	{
		int iMineIndex = 0;
		PACKET_GUARD_VOID( rkPacket.Read(iMineIndex) );
		vMineList.push_back( iMineIndex );
	}

	// Item
	PACKET_GUARD_VOID( rkPacket.Read(iItemCnt) );
	MAX_GUARD(iItemCnt, 100);
	for(int i=0; i < iItemCnt; ++i )
	{
		int iItemIndex = 0;
		PACKET_GUARD_VOID( rkPacket.Read(iItemIndex) );

		ioItem *pField = m_pCreator->FindFieldItem( iItemIndex );
		if( pField )
		{
			m_pCreator->RemoveFieldItem( pField );
			vItemList.push_back( iItemIndex );
		}
	}

	// Struct
	PACKET_GUARD_VOID( rkPacket.Read(iStructCnt) );
	MAX_GUARD(iStructCnt, 100);
	for(int i=0; i < iStructCnt; ++i )
	{
		int iStructIndex = 0;
		PACKET_GUARD_VOID( rkPacket.Read(iStructIndex) );

		int iCnt = m_vPushStructList.size();
		for( int j=0; j < iCnt; ++j )
		{
			if( m_vPushStructList[j].m_iIndex != iStructIndex )
				continue;

			m_vPushStructList.erase( m_vPushStructList.begin() + j );
			vStructList.push_back( iStructIndex );
			break;
		}
	}

	ioHashString szCharName, szSkillName;
	PACKET_GUARD_VOID( rkPacket.Read(szCharName) );
	PACKET_GUARD_VOID( rkPacket.Read(szSkillName) );

	// Send
	int iListCnt = 0;
	SP2Packet kPacket( STPK_ABSORB_APPLY );

	iListCnt = vMineList.size();
	PACKET_GUARD_VOID( kPacket.Write(iListCnt) );
	for(int i=0; i < iListCnt; ++i )
	{
		PACKET_GUARD_VOID( kPacket.Write(vMineList[i]) );
	}

	iListCnt = vItemList.size();
	PACKET_GUARD_VOID( kPacket.Write(iListCnt) );
	for(int i=0; i < iListCnt; ++i )
	{
		PACKET_GUARD_VOID( kPacket.Write(vItemList[i]) );
	}

	iListCnt = vStructList.size();
	PACKET_GUARD_VOID( kPacket.Write(iListCnt) );
	for(int i=0; i < iListCnt; ++i )
	{
		PACKET_GUARD_VOID( kPacket.Write(vStructList[i]) );
	}
	
	PACKET_GUARD_VOID( kPacket.Write(szCharName) );
	PACKET_GUARD_VOID( kPacket.Write(szSkillName) );

	SendRoomAllUser( kPacket );
}

void Mode::OnChatModeState( User *pSend, SP2Packet &rkPacket )
{
	if( !IsEnableState( pSend ) )
		return;

	if( m_pCreator->GetPlazaModeType() != PT_COMMUNITY )
		return;

	ioHashString szName;
	bool bChatMode;
	int iSubType;
	
	rkPacket >> szName >> iSubType >> bChatMode;

	ModeRecord *pRecord = FindModeRecord( szName );
	if( pRecord )
	{
		pRecord->bChatModeState = bChatMode;
	}

	SP2Packet kReturn( STPK_CHAT_MODE );
	kReturn << szName;
	kReturn << iSubType;
	kReturn << bChatMode;
	SendRoomAllUser( kReturn );
}

void Mode::OnExperienceState( User *pSend, SP2Packet &rkPacket )
{
	if( !IsEnableState( pSend ) )
		return;

	if( !m_pCreator->IsNoBattleModeType() ) return;

	int iSubType;
	bool bExperienceMode;
	int  iExperienceClassType = 0;
	ioHashString szExperienceID;
	rkPacket >> iSubType >> bExperienceMode;

	if( bExperienceMode )
	{
		rkPacket >> szExperienceID >> iExperienceClassType;

#ifdef HERO_POSSIBLE_CHECK
		if( !g_ItemPriceMgr.IsPossibleHero(iExperienceClassType))
		{
			LOG.PrintTimeAndLog( 0 , "[FAULT_PROC]%s - ID : %s, LimitHeroExperience: %d", __FUNCTION__, pSend->GetPublicID().c_str(), iExperienceClassType );
			g_LogDBClient.OnInsertGameLogInfo( 40001, pSend, 0, iExperienceClassType, LogDBClient::OVS_EXPERIENCE_CHAR, 0, 0, 0, 0, NULL );
			m_pCreator->RoomKickOut(pSend->GetPublicID().c_str());
			return;
		}
#endif
	}
	
	ModeRecord *pRecord = FindModeRecord( pSend->GetPublicID() );
	if( pRecord )
	{
		pRecord->bExperienceState = bExperienceMode;
		pRecord->szExperienceID   = szExperienceID;
		pRecord->iExperienceClassType = iExperienceClassType;
	}

	if( bExperienceMode )
	{	
		// 체험 치장 + 장비 세팅
		pSend->SetExperienceChar( rkPacket );
	}

	SP2Packet kPacket( STPK_EXPERIENCE_MODE );
	kPacket << pSend->GetPublicID() << iSubType;	
	kPacket << szExperienceID << iExperienceClassType;
	if( bExperienceMode )
	{
		// 용병 정보 전송
		pSend->FillExperienceChar( kPacket );
	}
	else
	{
		rkPacket << false;
	}
	SendRoomAllUser( kPacket );
}

void Mode::OnPushStructCreate( User *pSend, SP2Packet &rkPacket )
{
	if( !IsEnableState( pSend ) )
		return;

	int iDeleteIndex, iCreateNum;
	Vector3 vCreatePos;
	ioHashString szOwner, szItemName;
	Quaternion qtTargetRot;

	rkPacket >> iDeleteIndex >> iCreateNum;
	rkPacket >> vCreatePos >> qtTargetRot >> szOwner >> szItemName;

	DWORD dwEtcCode;
	rkPacket >> dwEtcCode;


#ifdef 	FISHING_SYSTEM_EX_BY_BCKIM	// 2020-02-17 by bckim, 낚시 시스템 확장

	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[PRIVATE_FISHING] OnPushStructCreate m_iPushStructIdx[%d] USER[%s]",m_iPushStructIdx,szOwner.c_str());

	if( COMPARE( dwEtcCode, ioEtcItem::EIT_ETC_PRIVATE_FISHING_PLACE_ITEM_1, ioEtcItem::EIT_ETC_PRIVATE_FISHING_PLACE_ITEM_100 +1 ))				
	{
		if( pSend )
		{
			ioUserEtcItem *pUserEtcItem = pSend->GetUserEtcItem();
			if( !pUserEtcItem )
				return;

			if( false == pUserEtcItem->HaveAThisItem(dwEtcCode) )			
			{	
				SP2Packet kFailReturn( STPK_PUSHSTRUCT_CREATE );				
				kFailReturn << false;
				kFailReturn << FISHING_PLACE_NON_HAVE_ITEM;		// error  // 해당 아이템이 없음 // 시간 경과 
				pSend->SendMessage( kFailReturn );

#ifdef 	FISHING_SYSTEM_EX_BY_BCKIM_DEBUG	// 2020-02-17 by bckim, 낚시 시스템 확장
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[STPK_PUSHSTRUCT_CREATE] FISHING_PLACE_NON_HAVE_ITEM USER[%s] CODE[%lu]",pSend->GetPublicID().c_str(),dwEtcCode);
#endif	// FISHING_SYSTEM_EX_BY_BCKIM_DEBUG	// End. 2020-02-17 by bckim, 낚시 시스템 		

				return;
			}

			int iCnt = m_vPushStructList.size();
			for( int i=0; i < iCnt; i++ )
			{
				if( m_vPushStructList[i].m_OwnerName == pSend->GetPublicID() && m_vPushStructList[i].m_dwCreateEtcCode == dwEtcCode)
				{	
					SP2Packet kFailReturn( STPK_PUSHSTRUCT_CREATE );					
					kFailReturn << false;
					kFailReturn << FISHING_PLACE_DUPLICATE_STRUCT;	//error				// 낚시터 중복 설치 						
					pSend->SendMessage( kFailReturn );

#ifdef 	FISHING_SYSTEM_EX_BY_BCKIM_DEBUG	// 2020-02-17 by bckim, 낚시 시스템 확장
					LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[STPK_PUSHSTRUCT_CREATE] FISHING_PLACE_DUPLICATE_STRUCT USER[%s] CODE[%lu]",pSend->GetPublicID().c_str(),dwEtcCode);
#endif	// FISHING_SYSTEM_EX_BY_BCKIM_DEBUG	// End. 2020-02-17 by bckim, 낚시 시스템 		

					return;
				}				
			}			
		}
	}
#endif	// FISHING_SYSTEM_EX_BY_BCKIM	// End. 2020-02-17 by bckim, 낚시 시스템 		

	// Delete
	int iCnt = m_vPushStructList.size();
	for( int i=0; i < iCnt; i++ )
	{
		if( m_vPushStructList[i].m_iIndex != iDeleteIndex )
			continue;

		m_vPushStructList.erase( m_vPushStructList.begin() + i );
		break;
	}

	// Create
	PushStruct kPush;
	kPush.m_iNum = iCreateNum;
	kPush.m_CreatePos = vCreatePos;
	kPush.m_TargetRot = qtTargetRot;
	kPush.m_OwnerName = szOwner;
	m_iPushStructIdx++;
	kPush.m_iIndex = m_iPushStructIdx;
	kPush.m_dwCreateTime = TIMEGETTIME();
	kPush.m_dwCreateEtcCode = dwEtcCode;

	m_vPushStructList.push_back( kPush );

	DWORD dwSeed = timeGetTime();

#ifdef 	FISHING_SYSTEM_EX_BY_BCKIM_DEBUG	// 2020-02-17 by bckim, 낚시 시스템 확장
	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[PRIVATE_FISHING] OnPushStructCreate m_iPushStructIdx[%d] USER[%s]",m_iPushStructIdx,szOwner.c_str());
#endif	// FISHING_SYSTEM_EX_BY_BCKIM_DEBUG	// End. 2020-02-17 by bckim, 낚시 시스템 		

	SP2Packet kStruct( STPK_PUSHSTRUCT_CREATE );
#ifdef 	FISHING_SYSTEM_EX_BY_BCKIM	// 2020-02-17 by bckim, 낚시 시스템 확장
	bool bCreate = true;
	kStruct << bCreate;
#endif	// FISHING_SYSTEM_EX_BY_BCKIM	// End. 2020-02-17 by bckim, 낚시 시스템 확장
	// Delete
	kStruct << iDeleteIndex;
	// Create
	kStruct << iCreateNum;
	kStruct << m_iPushStructIdx;
	kStruct << vCreatePos;
	kStruct << qtTargetRot;
	kStruct << szOwner;
	kStruct << szItemName;
	kStruct << 0;			// GapTime
	kStruct << dwSeed;
	kStruct << dwEtcCode;
	SendRoomAllUser( kStruct );
}

void Mode::OnCurrentDamageList( User *pSend, SP2Packet &rkPacket )
{
	int iDamageUserCnt = 0;
	PACKET_GUARD_VOID( rkPacket.Read(iDamageUserCnt) );
	MAX_GUARD(iDamageUserCnt, 500);

	// check abuse
	for( int i=0 ; i<iDamageUserCnt ; i++ )
	{
		ioHashString szName;
		int iTotalDamage = 0;

		PACKET_GUARD_VOID( rkPacket.Read(szName) );
		PACKET_GUARD_VOID( rkPacket.Read(iTotalDamage) );

		ModeRecord *pRecord = FindModeRecord( szName );
		if( pRecord )
		{
			pRecord->iTotalDamage += iTotalDamage;
		}
	}

	if( m_bRoundSetEnd )
	{
		ModeRecord *pSendRecord = FindModeRecord( pSend );
		if( pSendRecord )
			pSendRecord->bRecvDamageList = true;

		m_bCheckAllRecvDamageList = IsCheckAllDamageList();
	}
}

void Mode::OnEventSceneEnd( User *pSend, SP2Packet &rkPacket )
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
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Mode::OnEventSceneEnd - %s Not Exist Record",
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
	if( !m_pCreator->IsNoBattleModeType() && ( pRecord->pUser->IsObserver() || pRecord->pUser->IsStealth() ) )
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
	else if( m_bUseViewMode && m_ModeState == MS_PLAY && dwPastTime > m_dwViewCheckTime )
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
		pRecord->pUser->StartCharLimitDate( GetCharLimitCheckTime(), __FILE__, __LINE__ );
		pRecord->pUser->StartEtcItemTime( __FUNCTION__ );

		SP2Packet kPacket( STPK_ROUND_JOIN );
		kPacket << pRecord->pUser->GetPublicID();
		kPacket << iModeState;
		kPacket << dwPastTime;
		kPacket << GetSelectCharTime();
		kPacket << m_dwCurRoundDuration;
		SendRoomAllUser( kPacket );
	}
}

bool Mode::GetPushStructInfo( SP2Packet &rkPacket )
{
	if( m_vPushStructList.empty() )
		return false;

	int iPushStructCnt = m_vPushStructList.size();
	rkPacket << iPushStructCnt;

	for( int i=0; i<iPushStructCnt; i++ )
	{
		const PushStruct &rkPush = m_vPushStructList[i];

		DWORD dwSeed = timeGetTime();
		DWORD dwGapTime = 0;
		if( rkPush.m_dwCreateTime > 0 )
			dwGapTime = TIMEGETTIME() - rkPush.m_dwCreateTime;

		rkPacket << rkPush.m_iNum;
		rkPacket << rkPush.m_iIndex;
		rkPacket << rkPush.m_CreatePos;
		rkPacket << rkPush.m_TargetRot;
		rkPacket << rkPush.m_OwnerName;
		rkPacket << dwGapTime;
		rkPacket << dwSeed;
		rkPacket << rkPush.m_dwCreateEtcCode;
	}

	return true;
}

// 2020-02-17 by bckim, 낚시 시스템 확장
bool Mode::GetStructInfo( int iStructIndex , PushStruct &Structinfo )
{	
	if( m_vPushStructList.empty() )
		return false;

	int iPushStructCnt = m_vPushStructList.size();
	{
		for( int i=0; i<iPushStructCnt ; i++)
		{
			if( m_vPushStructList[i].m_iIndex == iStructIndex )
			{
				Structinfo =  m_vPushStructList[i];
				return true;
			}
		}		
	}
	return false;
}
void Mode::IsAvailablePrivateFishing()
{
	if( m_dwStatePrivateFishingTime + 30000 > TIMEGETTIME() )
		return;

	m_dwStatePrivateFishingTime = TIMEGETTIME();

	if( m_vPushStructList.empty() )
		return;

	int iStructIndex = 0;			// 파괴 대상 낚시터 // 체크 10초마다 
	int iPushStructCnt = m_vPushStructList.size();

	for( int i=0; i<iPushStructCnt ; i++)
	{		
		DWORD dwETCItemCode = m_vPushStructList[i].m_dwCreateEtcCode;

		if( false == COMPARE( dwETCItemCode, ioEtcItem::EIT_ETC_PRIVATE_FISHING_PLACE_ITEM_1, ioEtcItem::EIT_ETC_PRIVATE_FISHING_PLACE_ITEM_100 +1 ))				
			continue;

		User *pUser = m_pCreator->FindUserInRoom( m_vPushStructList[i].m_OwnerName );				
		if( pUser )
		{
			ioUserEtcItem *pUserEtcItem = pUser->GetUserEtcItem();
			if( !pUserEtcItem )
				return;

			if( false == pUserEtcItem->HaveAThisItem(dwETCItemCode) )
			{
				iStructIndex = m_vPushStructList[i].m_iIndex;
				break;
			}
		}
	}

	DestroyPrivateFishing(iStructIndex);
	return;
}
bool Mode::DestroyPrivateFishing( int iStructIndex )
{
	if( m_vPushStructList.empty() )
		return false;

	bool bEffect = true;
	int iCnt = m_vPushStructList.size();

#ifdef 	FISHING_SYSTEM_EX_BY_BCKIM_DEBUG	// 2020-02-17 by bckim, 낚시 시스템 확장
	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[PRIVATE_FISHING] STPK_PUSHSTRUCT_DIE m_vPushStructList BEFORE SIZE:[%d]",iCnt);
#endif	// FISHING_SYSTEM_EX_BY_BCKIM_DEBUG	// End. 2020-02-17 by bckim, 낚시 시스템 		

	for( int i=0; i < iCnt; i++ )
	{
		if( m_vPushStructList[i].m_iIndex != iStructIndex )
			continue;

		m_vPushStructList.erase( m_vPushStructList.begin() + i );

		SP2Packet kReturn( STPK_PUSHSTRUCT_DIE );
		PACKET_GUARD_BOOL( kReturn.Write(1) );
		PACKET_GUARD_BOOL( kReturn.Write(iStructIndex) );
		PACKET_GUARD_BOOL( kReturn.Write(bEffect) );
		PACKET_GUARD_BOOL( kReturn.Write(1) );

		SendRoomAllUser( kReturn );

#ifdef 	FISHING_SYSTEM_EX_BY_BCKIM_DEBUG	// 2020-02-17 by bckim, 낚시 시스템 확장
		int iCount = m_vPushStructList.size();
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[PRIVATE_FISHING] STPK_PUSHSTRUCT_DIE m_vPushStructList AFTER SIZE:[%d]",iCount);
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[PRIVATE_FISHING] STPK_PUSHSTRUCT_DIE DestroyPrivateFishing::USER[%s] STRUCT_NUM[%d]",m_vPushStructList[i].m_OwnerName,iStructIndex);
#endif	// FISHING_SYSTEM_EX_BY_BCKIM_DEBUG	// End. 2020-02-17 by bckim, 낚시 시스템 		

		return true;
	}

	return false;
}

// End. 2020-02-17 by bckim, 낚시 시스템 확장

bool Mode::GetBallStructInfo( SP2Packet &rkPacket )
{
	if( m_vBallStructList.empty() )
		return false;

	int iBallStructCnt = m_vBallStructList.size();
	rkPacket << iBallStructCnt;

	for( int i=0; i<iBallStructCnt; i++ )
	{
		const BallStruct &rkBall = m_vBallStructList[i];

		rkPacket << rkBall.m_iNum;
		rkPacket << rkBall.m_iIndex;
		rkPacket << rkBall.m_CreatePos;
		rkPacket << rkBall.m_TargetRot;
	}

	return true;
}

bool Mode::GetMachineStructInfo( SP2Packet &rkPacket )
{
	if( m_vMachineStructList.empty() )
		return false;

	rkPacket << MACHINE_INFO;
	int iMachineStructCnt = (int)m_vMachineStructList.size();
	rkPacket << iMachineStructCnt;

	for( int i=0; i<iMachineStructCnt; i++ )
	{
		rkPacket << m_vMachineStructList[i].m_iNum;
		rkPacket << m_vMachineStructList[i].m_iIndex;
		rkPacket << m_vMachineStructList[i].m_CreatePos;
		rkPacket << m_vMachineStructList[i].m_TargetRot;
	}

	return true;
}

void Mode::SetStartPosArray()
{
	IntVec vPosArray;
	vPosArray.reserve(2);
	for( int i=0; i<2; i++ )
	{
		vPosArray.push_back(i);
	}

	//io_random_shuffle( vPosArray.begin(), vPosArray.end() );

	m_iBluePosArray = vPosArray[0];
	m_iRedPosArray  = vPosArray[1];
}

void Mode::LoadStatPoint( ioINILoader &rkLoader )
{
	m_iStatPoint = rkLoader.LoadInt( "statpoint", "level_per_stat_point", 1 );
}

void Mode::CreateObjectItemByPushStruct( SP2Packet &rkPacket )
{
	Vector3 vPosition;
	ioHashString szName;
	
	rkPacket >> vPosition >> szName;

	if( szName.IsEmpty() ) return;

	ioItem *pItem = m_pCreator->CreateItemByName( szName );
	if( pItem )
	{
		pItem->SetItemPos( vPosition );

		SP2Packet kPacket( STPK_FIELD_ITEM_SUPPLY );
		kPacket << 1;

		m_pCreator->AddFieldItem( pItem );
		kPacket << pItem->GetItemCode();
		kPacket << pItem->GetItemReinforce();
		kPacket << pItem->GetItemMaleCustom();
		kPacket << pItem->GetItemFemaleCustom();
		kPacket << pItem->GetGameIndex();
		kPacket << pItem->GetItemPos();
		kPacket << pItem->GetOwnerName();
		kPacket << "";
		SendRoomAllUser( kPacket );
	}
}

void Mode::OnRepositionFieldItem( SP2Packet &rkPacket )
{
	if( !m_pCreator )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Mode::OnRepositionFieldItem - Not Room" );
		return;
	}

	int iGameIndex;
	rkPacket >> iGameIndex;

	ioItem *pFieldItem = m_pCreator->FindFieldItem( iGameIndex );
	if( !pFieldItem )	return;

	Vector3 vPos = GetRandomItemPos( pFieldItem );
	vPos.y = 0.0f;
	pFieldItem->SetItemPos( vPos );

	SP2Packet kPacket( STPK_REPOSITION_FIELDITEM );
	kPacket << pFieldItem->GetGameIndex();
	kPacket << pFieldItem->GetItemPos();
	SendRoomAllUser( kPacket );

#ifdef FLAG_MODE_BY_BCKIM_DEBUG		// 전투시작 임시 값 확인  OnBattleRoomCommand
	LOG.PrintTimeAndLog(0,"[FLAG_MODE_BY_BCKIM:OnRepositionFieldItem] INFO>>CTPK_REPOSITION_FIELDITEM [%s]",pFieldItem->GetItemName().c_str());	
#endif // FLAG_MODE_BY_BCKIM_DEBUG	

}

void Mode::SetExitRoomByCheckValue( User *pSend )
{
	bool bExitLobby = true;
	int iExitType = EXIT_ROOM_MACRO_OUT;
	int iPenaltyPeso = 0;

	pSend->SetExitPosition( bExitLobby );

	if( !IsEnableState( pSend ) && iExitType == EXIT_ROOM_LOBBY )
		return;

	bool bNoBattle = false;
	if( GetModeType() == MT_TRAINING || GetModeType() == MT_HEADQUARTERS || GetModeType() == MT_HOUSE )
	{
		bNoBattle = true;
	}

	int iDownPeso = 0;
	if( !bNoBattle )
	{
		ModeRecord *pRecord = FindModeRecord( pSend );
		if( pRecord )
		{
			pRecord->AddPlayingTime();
		}

		if( iPenaltyPeso < 0 )
		{
			HackLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s PenaltyPeso Hack :%d:%s:%s:%d->10000", __FUNCTION__, pSend->GetUserIndex(), pSend->GetPublicID().c_str(), pSend->GetPrivateID().c_str(), iPenaltyPeso );
			iPenaltyPeso = 10000; // 임의의 큰 수
		}
		iDownPeso = pSend->DecreasePenaltyPeso( iPenaltyPeso );
		int iMinusLadderPoint = DecreasePenaltyLadderPoint( pSend );

		SP2Packet kPenaltyPacket( STPK_EXIT_ROOM_PENALTY );
		kPenaltyPacket << pSend->GetMoney() << pSend->GetLadderPoint() << iPenaltyPeso;			
		// 래더전에서 이탈시에 패로 기록
		if( SetLeaveUserPenalty( pSend ) )
		{
			kPenaltyPacket << true << (int)m_pCreator->GetRoomStyle() << (int)GetModeType() << m_iLosePoint;      // 패배 포인트 전송
		}
		else
		{
			kPenaltyPacket << false;
		}
			
		pSend->SendMessage( kPenaltyPacket );

		// log			
		g_LogDBClient.OnInsertPeso( pSend, -iDownPeso, LogDBClient::PT_EXIT_ROOM );
	}

	pSend->LeaveRoom( true );
	if( pSend->IsBattleRoom() )      
		pSend->LeaveBattleRoom();
	else if( pSend->IsLadderTeam() )
		pSend->LeaveLadderTeam();
	else if( pSend->IsShuffleRoom() )
		pSend->LeaveShuffleRoom();

	pSend->ExitRoomToTraining( iExitType, !bNoBattle );
}

void Mode::ExitRoom( User *pSend, bool bExitLobby, int iExitType, int iPenaltyPeso )
{
	pSend->SetExitPosition( bExitLobby );

	if( !IsEnableState( pSend ) && iExitType == EXIT_ROOM_LOBBY )
		return;	

	if( iExitType == EXIT_ROOM_KEY_ABUSE ||
		iExitType == EXIT_ROOM_DAMAGE_ABUSE ||
		iExitType == EXIT_ROOM_SPEEDHACK ||
		iExitType == EXIT_ROOM_MACRO_OUT )
	{
		bool bNoBattle = false;
		if( GetModeType() == MT_TRAINING || GetModeType() == MT_HEADQUARTERS || GetModeType() == MT_HOUSE )
		{
			bNoBattle = true;
		}

		int iDownPeso = 0;
		if( !bNoBattle )
		{
			ModeRecord *pRecord = FindModeRecord( pSend );
			if( pRecord )
			{
				pRecord->AddPlayingTime();
			}

			if( iPenaltyPeso < 0 )
			{
				HackLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s PenaltyPeso Hack :%d:%s:%s:%d->10000", __FUNCTION__, pSend->GetUserIndex(), pSend->GetPublicID().c_str(), pSend->GetPrivateID().c_str(), iPenaltyPeso );
				iPenaltyPeso = 10000; // 임의의 큰 수
			}
			iDownPeso = pSend->DecreasePenaltyPeso( iPenaltyPeso );
			int iMinusLadderPoint = DecreasePenaltyLadderPoint( pSend );

			SP2Packet kPenaltyPacket( STPK_EXIT_ROOM_PENALTY );

			PACKET_GUARD_VOID( kPenaltyPacket.Write(pSend->GetMoney()) );
			PACKET_GUARD_VOID( kPenaltyPacket.Write(pSend->GetLadderPoint()) );
			PACKET_GUARD_VOID( kPenaltyPacket.Write(iPenaltyPeso) );

			// 래더전에서 이탈시에 패로 기록
			if( SetLeaveUserPenalty( pSend ) )
			{
				PACKET_GUARD_VOID( kPenaltyPacket.Write(true) ); 
				PACKET_GUARD_VOID( kPenaltyPacket.Write((int)m_pCreator->GetRoomStyle()) );
				PACKET_GUARD_VOID( kPenaltyPacket.Write((int)GetModeType()) );
				PACKET_GUARD_VOID( kPenaltyPacket.Write(m_iLosePoint) );      // 패배 포인트 전송
			}
			else
			{
				PACKET_GUARD_VOID( kPenaltyPacket.Write(false) );
			}
			
			pSend->SendMessage( kPenaltyPacket );

			// log			
			g_LogDBClient.OnInsertPeso( pSend, -iDownPeso, LogDBClient::PT_EXIT_ROOM );
		}

		pSend->LeaveRoom( RoomParent::RLT_KICK );

		if( pSend->IsBattleRoom() )      
			pSend->LeaveBattleRoom();
		else if( pSend->IsLadderTeam() )
			pSend->LeaveLadderTeam();
		else if( pSend->IsShuffleRoom() )
			pSend->LeaveShuffleRoom();

		if( iExitType == EXIT_ROOM_SPEEDHACK )
		{
			SP2Packet kPacket( STPK_EXIT_ROOM );

			PACKET_GUARD_VOID( kPacket.Write(EXIT_ROOM_SPEEDHACK) );
			PACKET_GUARD_VOID( kPacket.Write(-1) );
			PACKET_GUARD_VOID( kPacket.Write(!bNoBattle) );
			PACKET_GUARD_VOID( kPacket.Write(-1) );

			pSend->SendMessage( kPacket );
		}
		else
		{
			pSend->ExitRoomToTraining( iExitType, !bNoBattle );
		}
	}
	else if( iExitType == EXIT_ROOM_QUICK_OUT || 
			 iExitType == EXIT_ROOM_CHAR_LIMIT ||
			 iExitType == EXIT_ROOM_MONSTER_COIN_LACK ||
			 iExitType == EXIT_ROOM_RAID_COIN_LACK ||
			 iExitType == EXIT_ROOM_BAD_NETWORK )
	{
		if( iExitType == EXIT_ROOM_BAD_NETWORK )
		{
			SP2Packet kPenaltyPacket( STPK_EXIT_ROOM_PENALTY );

			PACKET_GUARD_VOID( kPenaltyPacket.Write(pSend->GetMoney()) );
			PACKET_GUARD_VOID( kPenaltyPacket.Write(pSend->GetLadderPoint()) );
			PACKET_GUARD_VOID( kPenaltyPacket.Write(0) );

			// 래더전에서 이탈시에 패로 기록
			if( SetLeaveUserPenalty( pSend ) )
			{
				PACKET_GUARD_VOID( kPenaltyPacket.Write(true) );
				PACKET_GUARD_VOID( kPenaltyPacket.Write((int)m_pCreator->GetRoomStyle()) );
				PACKET_GUARD_VOID( kPenaltyPacket.Write((int)GetModeType()) );
				PACKET_GUARD_VOID( kPenaltyPacket.Write(m_iLosePoint) );      // 패배 포인트 전송
			}
			else
			{
				PACKET_GUARD_VOID( kPenaltyPacket.Write(false) );
			}

			pSend->SendMessage( kPenaltyPacket );

			pSend->LeaveRoom( RoomParent::RLT_BADNETWORK );
		}
		else
			pSend->LeaveRoom();
	
		//파티중일 경우 파티 탈퇴도 실행.
		LeavePartyByStyle( pSend, m_pCreator->GetRoomStyle() );

		pSend->ExitRoomToTraining( iExitType, false );
	}
	else if( iExitType == EXIT_ROOM_RESERVED || iExitType == EXIT_ROOM_CANCELED ) //예약 처리
	{
		int iResult;
		if( pSend->ToggleExitRoomReserve() )
			iResult = EXIT_ROOM_RESERVED;
		else
			iResult = EXIT_ROOM_CANCELED;

		SP2Packet kPacket( STPK_EXIT_ROOM );
		kPacket << iResult;
		kPacket << -1;
		kPacket << false;
		pSend->SendMessage( kPacket );
	}
	else if( iExitType == EXIT_ROOM_EXCEPTION )
	{
		pSend->LeaveRoom();

		//파티중일 경우 파티 탈퇴도 실행.
		LeavePartyByStyle( pSend, m_pCreator->GetRoomStyle() );

		pSend->ExitRoomToTraining( iExitType, false );
	}
	else
	{
		bool bPenalty = false;
		if( !pSend->IsExitRoomDelay() && iExitType == EXIT_ROOM_PENALTY )
		{
			ModeRecord *pRecord = FindModeRecord(pSend);
			if( pRecord )
			{
				pRecord->AddPlayingTime();
			}

			if( iPenaltyPeso < 0 )
			{
				HackLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s PenaltyPeso Hack :%d:%s:%s:%d->10000", __FUNCTION__, pSend->GetUserIndex(), pSend->GetPublicID().c_str(), pSend->GetPrivateID().c_str(), iPenaltyPeso );
				iPenaltyPeso = 10000; // 임의의 큰 수
			}
			int iDownPeso = pSend->DecreasePenaltyPeso( iPenaltyPeso );
			int iMinusLadderPoint = DecreasePenaltyLadderPoint( pSend );

			SP2Packet kPenaltyPacket( STPK_EXIT_ROOM_PENALTY );

			PACKET_GUARD_VOID( kPenaltyPacket.Write(pSend->GetMoney()) );
			PACKET_GUARD_VOID( kPenaltyPacket.Write(pSend->GetLadderPoint()) );
			PACKET_GUARD_VOID( kPenaltyPacket.Write(iPenaltyPeso) );

			// 래더전에서 이탈시에 패로 기록
			if( SetLeaveUserPenalty( pSend ) )
			{
				PACKET_GUARD_VOID( kPenaltyPacket.Write(true) );
				PACKET_GUARD_VOID( kPenaltyPacket.Write((int)m_pCreator->GetRoomStyle()) );
				PACKET_GUARD_VOID( kPenaltyPacket.Write((int)GetModeType()) );
				PACKET_GUARD_VOID( kPenaltyPacket.Write(m_iLosePoint) );      // 패배 포인트 전송
			}
			else
			{
				kPenaltyPacket << false;
			}
			pSend->SendMessage( kPenaltyPacket );

			bPenalty = true;
			g_LogDBClient.OnInsertPeso( pSend, -iDownPeso, LogDBClient::PT_EXIT_ROOM );
		}

		pSend->ClearPreRoomNum();
		pSend->LeaveRoom();

		//파티중일 경우 파티 탈퇴도 실행.
		LeavePartyByStyle( pSend, m_pCreator->GetRoomStyle() );

		pSend->ExitRoomToTraining( iExitType, bPenalty );
	}
}

void Mode::OnExitRoom( User *pSend, SP2Packet &rkPacket )
{
	bool bExitLobby = false;
	int iExitType = 0;
	int iPenaltyPeso = 0;

	PACKET_GUARD_VOID( rkPacket.Read(iExitType) );
	PACKET_GUARD_VOID( rkPacket.Read(iPenaltyPeso) );
	PACKET_GUARD_VOID( rkPacket.Read(bExitLobby) );

	ExitRoom( pSend, bExitLobby, iExitType, iPenaltyPeso );
	/*pSend->SetExitPosition( bExitLobby );

	if( !IsEnableState( pSend ) && iExitType == EXIT_ROOM_LOBBY )
		return;	

	if( iExitType == EXIT_ROOM_KEY_ABUSE ||
		iExitType == EXIT_ROOM_DAMAGE_ABUSE ||
		iExitType == EXIT_ROOM_SPEEDHACK ||
		iExitType == EXIT_ROOM_MACRO_OUT )
	{
		bool bNoBattle = false;
		if( GetModeType() == MT_TRAINING || GetModeType() == MT_HEADQUARTERS )
		{
			bNoBattle = true;
		}

		int iDownPeso = 0;
		if( !bNoBattle )
		{
			ModeRecord *pRecord = FindModeRecord( pSend );
			if( pRecord )
			{
				pRecord->AddPlayingTime();
			}

			if( iPenaltyPeso < 0 )
			{
				HackLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s PenaltyPeso Hack :%d:%s:%s:%d->10000", __FUNCTION__, pSend->GetUserIndex(), pSend->GetPublicID().c_str(), pSend->GetPrivateID().c_str(), iPenaltyPeso );
				iPenaltyPeso = 10000; // 임의의 큰 수
			}
			iDownPeso = pSend->DecreasePenaltyPeso( iPenaltyPeso );
			int iMinusLadderPoint = DecreasePenaltyLadderPoint( pSend );

			SP2Packet kPenaltyPacket( STPK_EXIT_ROOM_PENALTY );

			PACKET_GUARD_VOID( kPenaltyPacket.Write(pSend->GetMoney()) );
			PACKET_GUARD_VOID( kPenaltyPacket.Write(pSend->GetLadderPoint()) );
			PACKET_GUARD_VOID( kPenaltyPacket.Write(iPenaltyPeso) );

			// 래더전에서 이탈시에 패로 기록
			if( SetLeaveUserPenalty( pSend ) )
			{
				PACKET_GUARD_VOID( kPenaltyPacket.Write(true) ); 
				PACKET_GUARD_VOID( kPenaltyPacket.Write((int)m_pCreator->GetRoomStyle()) );
				PACKET_GUARD_VOID( kPenaltyPacket.Write((int)GetModeType()) );
				PACKET_GUARD_VOID( kPenaltyPacket.Write(m_iLosePoint) );      // 패배 포인트 전송
			}
			else
			{
				PACKET_GUARD_VOID( kPenaltyPacket.Write(false) );
			}
			
			pSend->SendMessage( kPenaltyPacket );

			// log			
			g_LogDBClient.OnInsertPeso( pSend, -iDownPeso, LogDBClient::PT_EXIT_ROOM );
		}

		pSend->LeaveRoom( RoomParent::RLT_KICK );

		if( pSend->IsBattleRoom() )      
			pSend->LeaveBattleRoom();
		else if( pSend->IsLadderTeam() )
			pSend->LeaveLadderTeam();
		else if( pSend->IsShuffleRoom() )
			pSend->LeaveShuffleRoom();

		if( iExitType == EXIT_ROOM_SPEEDHACK )
		{
			SP2Packet kPacket( STPK_EXIT_ROOM );

			PACKET_GUARD_VOID( kPacket.Write(EXIT_ROOM_SPEEDHACK) );
			PACKET_GUARD_VOID( kPacket.Write(-1) );
			PACKET_GUARD_VOID( kPacket.Write(!bNoBattle) );
			PACKET_GUARD_VOID( kPacket.Write(-1) );

			pSend->SendMessage( kPacket );
		}
		else
		{
			pSend->ExitRoomToTraining( iExitType, !bNoBattle );
		}
	}
	else if( iExitType == EXIT_ROOM_QUICK_OUT || 
			 iExitType == EXIT_ROOM_CHAR_LIMIT ||
			 iExitType == EXIT_ROOM_MONSTER_COIN_LACK ||
			 iExitType == EXIT_ROOM_BAD_NETWORK )
	{
		if( iExitType == EXIT_ROOM_BAD_NETWORK )
		{
			SP2Packet kPenaltyPacket( STPK_EXIT_ROOM_PENALTY );

			PACKET_GUARD_VOID( kPenaltyPacket.Write(pSend->GetMoney()) );
			PACKET_GUARD_VOID( kPenaltyPacket.Write(pSend->GetLadderPoint()) );
			PACKET_GUARD_VOID( kPenaltyPacket.Write(0) );

			// 래더전에서 이탈시에 패로 기록
			if( SetLeaveUserPenalty( pSend ) )
			{
				PACKET_GUARD_VOID( kPenaltyPacket.Write(true) );
				PACKET_GUARD_VOID( kPenaltyPacket.Write((int)m_pCreator->GetRoomStyle()) );
				PACKET_GUARD_VOID( kPenaltyPacket.Write((int)GetModeType()) );
				PACKET_GUARD_VOID( kPenaltyPacket.Write(m_iLosePoint) );      // 패배 포인트 전송
			}
			else
			{
				PACKET_GUARD_VOID( kPenaltyPacket.Write(false) );
			}

			pSend->SendMessage( kPenaltyPacket );

			pSend->LeaveRoom( RoomParent::RLT_BADNETWORK );
		}
		else
			pSend->LeaveRoom();
	
		//파티중일 경우 파티 탈퇴도 실행.
		LeavePartyByStyle( pSend, m_pCreator->GetRoomStyle() );

		pSend->ExitRoomToTraining( iExitType, false );
	}
	else if( iExitType == EXIT_ROOM_RESERVED || iExitType == EXIT_ROOM_CANCELED ) //예약 처리
	{
		int iResult;
		if( pSend->ToggleExitRoomReserve() )
			iResult = EXIT_ROOM_RESERVED;
		else
			iResult = EXIT_ROOM_CANCELED;

		SP2Packet kPacket( STPK_EXIT_ROOM );
		kPacket << iResult;
		kPacket << -1;
		kPacket << false;
		pSend->SendMessage( kPacket );
	}
	else if( iExitType == EXIT_ROOM_EXCEPTION )
	{
		pSend->LeaveRoom();

		//파티중일 경우 파티 탈퇴도 실행.
		LeavePartyByStyle( pSend, m_pCreator->GetRoomStyle() );

		pSend->ExitRoomToTraining( iExitType, false );
	}
	else
	{
		bool bPenalty = false;
		if( !pSend->IsExitRoomDelay() && iExitType == EXIT_ROOM_PENALTY )
		{
			ModeRecord *pRecord = FindModeRecord(pSend);
			if( pRecord )
			{
				pRecord->AddPlayingTime();
			}

			if( iPenaltyPeso < 0 )
			{
				HackLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s PenaltyPeso Hack :%d:%s:%s:%d->10000", __FUNCTION__, pSend->GetUserIndex(), pSend->GetPublicID().c_str(), pSend->GetPrivateID().c_str(), iPenaltyPeso );
				iPenaltyPeso = 10000; // 임의의 큰 수
			}
			int iDownPeso = pSend->DecreasePenaltyPeso( iPenaltyPeso );
			int iMinusLadderPoint = DecreasePenaltyLadderPoint( pSend );

			SP2Packet kPenaltyPacket( STPK_EXIT_ROOM_PENALTY );

			PACKET_GUARD_VOID( kPenaltyPacket.Write(pSend->GetMoney()) );
			PACKET_GUARD_VOID( kPenaltyPacket.Write(pSend->GetLadderPoint()) );
			PACKET_GUARD_VOID( kPenaltyPacket.Write(iPenaltyPeso) );

			// 래더전에서 이탈시에 패로 기록
			if( SetLeaveUserPenalty( pSend ) )
			{
				PACKET_GUARD_VOID( kPenaltyPacket.Write(true) );
				PACKET_GUARD_VOID( kPenaltyPacket.Write((int)m_pCreator->GetRoomStyle()) );
				PACKET_GUARD_VOID( kPenaltyPacket.Write((int)GetModeType()) );
				PACKET_GUARD_VOID( kPenaltyPacket.Write(m_iLosePoint) );      // 패배 포인트 전송
			}
			else
			{
				kPenaltyPacket << false;
			}
			pSend->SendMessage( kPenaltyPacket );

			bPenalty = true;
			g_LogDBClient.OnInsertPeso( pSend, -iDownPeso, LogDBClient::PT_EXIT_ROOM );
		}

		pSend->ClearPreRoomNum();
		pSend->LeaveRoom();

		//파티중일 경우 파티 탈퇴도 실행.
		LeavePartyByStyle( pSend, m_pCreator->GetRoomStyle() );

		pSend->ExitRoomToTraining( iExitType, bPenalty );
	}*/
}

void Mode::CheckFieldItemLiveTime()
{
	int iCnt = m_pCreator->GetFieldItemCnt();
	ItemVector vItemList;

	for( int i=0; i < iCnt; i++ )
	{
		ioItem *pField = m_pCreator->GetFieldItem( i );
		if( !pField ) continue;
		if( pField->GetDropTime() == 0 ) continue;
		if( pField->IsNotDeleteItem() ) continue;

		DWORD dwGapTime = TIMEGETTIME() - pField->GetDropTime();
		if( dwGapTime > m_dwDropItemLiveTime )
		{
			vItemList.push_back( pField );
		}
	}
	if( vItemList.empty() ) return;

	int iItemCnt = vItemList.size();
	SP2Packet kPacket( STPK_FIELD_ITEM_DELETE );
	kPacket << iItemCnt;
	for(int i=0; i<iItemCnt; i++ )
	{
		ioItem *pItem = vItemList[i];
		kPacket << pItem->GetGameIndex();
		m_pCreator->RemoveFieldItem( pItem );
		delete pItem;
	}

	SendRoomAllUser( kPacket );
}

void Mode::ProcessRevival()
{
	DWORD dwCurTime = TIMEGETTIME();

	int iRecordCnt = GetRecordCnt();
	for( int i=0 ; i<iRecordCnt ; i++ )
	{
		ModeRecord *pRecord = FindModeRecord( i );
		if( !pRecord || pRecord->dwCurDieTime == 0 )
			continue;

		if( pRecord->dwCurDieTime + pRecord->dwRevivalGap < dwCurTime )
		{
			pRecord->dwCurDieTime = 0;
			pRecord->bDieState = false;

			SP2Packet kUserItemPk( STPK_CHAR_EQUIPDATA );
			pRecord->pUser->EquipDBItemToAllChar();
			pRecord->pUser->FillEquipItemData( kUserItemPk );
			pRecord->pUser->FillEquipMedalItem( kUserItemPk );
			pRecord->pUser->FillEquipCustomMedalItem( kUserItemPk );
			m_pCreator->RoomSendPacketTcp( kUserItemPk );

			SP2Packet kPacket( STPK_USER_REVIVAL );
			kPacket << pRecord->pUser->GetPublicID();
			kPacket << GetSelectCharTime();
			kPacket << timeGetTime();
			m_pCreator->RoomSendPacketTcp( kPacket );
		}
	}
}

void Mode::GetRevivalTime( SP2Packet &rkPacket, const ioHashString &szName )
{
	ModeRecord *pRecord = FindModeRecord( szName );
	if( pRecord )
		rkPacket << pRecord->dwRevivalGap;
	else
		rkPacket << 0;
}

void Mode::OnCatchChar( User *pSend, SP2Packet &rkPacket )
{
	ioHashString szName;
	rkPacket >> szName;

	/*
	DWORD dwDuration;
	rkPacket >> dwDuration;

	ioHashString szTieEffect, szEscapeEffect, szRope, szCreateChar;
	rkPacket >> szTieEffect >> szEscapeEffect >> szRope >> szCreateChar;
	*/

	ModeRecord *pRecord = FindModeRecord( szName );
	if( pRecord )         //유저 포박
	{
		if( pRecord->eState == RS_LOADING )
			return;

		if( !pRecord->bCatchState )
		{
			pRecord->bCatchState = true;

			SP2Packet kPacket( STPK_CATCH_CHAR );
			kPacket.SetDataAdd( (char*)rkPacket.GetData(), rkPacket.GetDataSize() );
			SendRoomAllUser( kPacket );

			/*
			SP2Packet kPacket( STPK_CATCH_CHAR );
			kPacket << szName;
			kPacket << dwDuration;
			kPacket << szTieEffect;
			kPacket << szEscapeEffect;
			kPacket << szRope;
			kPacket << szCreateChar;
			SendRoomAllUser( kPacket );
			*/
		}
	}
	else                  //NPC 포박
	{
		SP2Packet kPacket( STPK_CATCH_CHAR );
		kPacket.SetDataAdd( (char*)rkPacket.GetData(), rkPacket.GetDataSize() );
		SendRoomAllUser( kPacket );

		/*
		SP2Packet kPacket( STPK_CATCH_CHAR );
		kPacket << szName;
		kPacket << dwDuration;
		kPacket << szTieEffect;
		kPacket << szEscapeEffect;
		kPacket << szRope;
		kPacket << szCreateChar;
		SendRoomAllUser( kPacket );
		*/
	}
}

void Mode::OnEscapeCatchChar( User *pSend, SP2Packet &rkPacket )
{
	ioHashString szName;
	bool bEscapeFail;
	rkPacket >> szName;
	rkPacket >> bEscapeFail;

	ModeRecord *pRecord = FindModeRecord( szName );
	if( pRecord )         //유저 포박 해제
	{
		if( pRecord->eState == RS_LOADING )
			return;

		if( pRecord->bCatchState )
		{
			pRecord->bCatchState = false;

			SP2Packet kPacket( STPK_ESCAPE_CATCH_CHAR );
			kPacket << pRecord->pUser->GetPublicID();
			kPacket << bEscapeFail;
			SendRoomAllUser( kPacket );
		}	
	}
	else                  //NPC 포박 해제
	{
		SP2Packet kPacket( STPK_ESCAPE_CATCH_CHAR );
		kPacket << szName;
		kPacket << bEscapeFail;
		SendRoomAllUser( kPacket );
	}	
	
}

void Mode::OnPlayRecordInfo( User *pUser, SP2Packet &rkPacket )
{
	ioHashString szRecver;
	rkPacket >> szRecver;

	ModeRecord *pRecord = FindModeRecord( szRecver );
	if( !pRecord )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Mode::OnPlayRecordInfo() - %s is Not Exist.", szRecver.c_str() );
		return;
	}

	SP2Packet kReturnPacket( STPK_PLAYRECORD_INFO );
	kReturnPacket.SetDataAdd( (char*)rkPacket.GetData(), rkPacket.GetDataSize() );
	pRecord->pUser->SendMessage( kReturnPacket );
}

void Mode::OnLastPlayRecordInfo( User *pUser, SP2Packet &rkPacket )
{
	if( m_bCheckContribute ) return;

	m_bCheckContribute = true;

	int i = 0;
	int iCharCnt = 0;
	PACKET_GUARD_VOID( rkPacket.Read(iCharCnt) );
	MAX_GUARD(iCharCnt, 50);

	for(i = 0;i < iCharCnt;i++)
	{
		ioHashString szName;
		int iContribute = 0, iUniqueTotalKill = 0, iUniqueTotalDeath = 0, iVictories = 0;
		PACKET_GUARD_VOID( rkPacket.Read(szName) );
		PACKET_GUARD_VOID( rkPacket.Read(iContribute) );
		PACKET_GUARD_VOID( rkPacket.Read(iUniqueTotalKill) );
		PACKET_GUARD_VOID( rkPacket.Read(iUniqueTotalDeath) );
		PACKET_GUARD_VOID( rkPacket.Read(iVictories) );

		ModeRecord *pRecord = FindModeRecord( szName );
		if( pRecord )
		{
			pRecord->iContribute	   = iContribute;
			pRecord->iUniqueTotalKill  = iUniqueTotalKill;
			pRecord->iUniqueTotalDeath = iUniqueTotalDeath;
			pRecord->iVictories		   = iVictories;
			LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "Mode::OnLastContributeInfo Recv: %s - %d - %d - %d - %d", szName.c_str(), iContribute, iUniqueTotalKill, iUniqueTotalDeath, iVictories );

			// error check
			if( iContribute == 0 )
			{
				if( iUniqueTotalKill != 0 || iUniqueTotalDeath != 0 )
					LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "Mode::OnLastContributeInfo Error: %s - %d - %d - %d - %d", szName.c_str(), iContribute, iUniqueTotalKill, iUniqueTotalDeath, iVictories );
			}	
		}
	}
	
	int iRecordCnt		= GetRecordCnt();
	int iMaxContribute	= 0;
	for(i = 0;i < iRecordCnt;i++)
	{
		ModeRecord *pRecord = FindModeRecord( i );
		if( !pRecord ) continue;
		if( !pRecord->pUser ) continue;
		iMaxContribute += pRecord->iContribute;
	}

	// 옵저버 유저 제외
	int iOb = 0;
	for(i = 0;i < iRecordCnt;i++)
	{
		ModeRecord *pRecord = FindModeRecord( i );
		if( !pRecord ) continue;
		if( !pRecord->pUser ) continue;
		if( pRecord->pUser->IsObserver() || pRecord->pUser->IsStealth() )
			iOb++;
	}

	if( iMaxContribute > 0 )
	{
		for(i = 0;i < iRecordCnt;i++)
		{
			ModeRecord *pRecord = FindModeRecord( i );
			if( !pRecord ) continue;
			if( !pRecord->pUser ) continue;
			
			pRecord->fContributePer = (float)( iRecordCnt - iOb ) * ((float)pRecord->iContribute / iMaxContribute);
			LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "Mode::OnLastContributeInfo Per: %s - %.2f", pRecord->pUser->GetPublicID().c_str(), pRecord->fContributePer );
		}	
	}	

	AddModeContributeByShuffle();

	// 전체 유저의 기여도와 킬데스레벨을 전체 유저에게 전송한다.
	SP2Packet kPacket( STPK_FINAL_RESULT_USER_INFO );
	PACKET_GUARD_VOID( kPacket.Write(iRecordCnt) );
	for(i = 0;i < iRecordCnt;i++)
	{
		ModeRecord *pRecord = FindModeRecord( i );
		if( !pRecord ) continue;
		if( !pRecord->pUser ) continue;

		int iVictories = pRecord->iVictories;
		if( pRecord->pUser->IsLadderTeam() )
			pRecord->pUser->GetMyVictories();

		PACKET_GUARD_VOID( kPacket.Write(pRecord->pUser->GetPublicID()) );
		PACKET_GUARD_VOID( kPacket.Write(pRecord->iContribute) );
		PACKET_GUARD_VOID( kPacket.Write(pRecord->iUniqueTotalKill) );
		PACKET_GUARD_VOID( kPacket.Write(pRecord->iUniqueTotalDeath) );
		PACKET_GUARD_VOID( kPacket.Write(iVictories) );
		PACKET_GUARD_VOID( kPacket.Write(pRecord->pUser->GetKillDeathLevel()) );
		PACKET_GUARD_VOID( kPacket.Write(pRecord->pUser->IsSafetyLevel()) );

		g_UserNodeManager.UpdateUserSync( pRecord->pUser );      //유저 정보 갱신
	}
	m_pCreator->RoomSendPacketTcp( kPacket );
}

void Mode::OnSuddenDeathPlayRecordInfo( User *pUser, SP2Packet &rkPacket )
{
	if( pUser == NULL ) return;
	if( !m_bTournamentRoom ) return;
	if( GetState() != MS_PLAY ) return;
	if( m_bCheckSuddenDeathContribute ) return;

	int iCommand = 0;
	PACKET_GUARD_VOID( rkPacket.Read(iCommand) );

	if( iCommand != TOURNAMENT_SUDDEN_DEATH_CONTRIBUTE )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "OnSuddenDeathPlayRecordInfo Command Error : %s - %d", pUser->GetPublicID().c_str(), iCommand );
		return;
	}

	m_bCheckSuddenDeathContribute = true;

	int i = 0;
	int iCharCnt = 0;
	PACKET_GUARD_VOID( rkPacket.Read(iCharCnt) );
	MAX_GUARD(iCharCnt, 100);

	for(i = 0;i < iCharCnt;i++)
	{
		ioHashString szName;
		int iContribute = 0, iUniqueTotalKill = 0, iUniqueTotalDeath = 0, iVictories = 0;
		PACKET_GUARD_VOID( rkPacket.Read(szName) );
		PACKET_GUARD_VOID( rkPacket.Read(iContribute) );
		PACKET_GUARD_VOID( rkPacket.Read(iUniqueTotalKill) );
		PACKET_GUARD_VOID( rkPacket.Read(iUniqueTotalDeath) );
		PACKET_GUARD_VOID( rkPacket.Read(iVictories) );

		ModeRecord *pRecord = FindModeRecord( szName );
		if( pRecord )
		{
			pRecord->iContribute	   = iContribute;
			pRecord->iUniqueTotalKill  = iUniqueTotalKill;
			pRecord->iUniqueTotalDeath = iUniqueTotalDeath;
			pRecord->iVictories		   = iVictories;
			LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "Mode::OnSuddenDeathPlayRecordInfo Recv: %s - %d - %d - %d - %d", szName.c_str(), iContribute, iUniqueTotalKill, iUniqueTotalDeath, iVictories );

			// error check
			if( iContribute == 0 )
			{
				if( iUniqueTotalKill != 0 || iUniqueTotalDeath != 0 )
					LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "Mode::OnSuddenDeathPlayRecordInfo Error: %s - %d - %d - %d - %d", szName.c_str(), iContribute, iUniqueTotalKill, iUniqueTotalDeath, iVictories );
			}
		}
	}

	int iRecordCnt		= GetRecordCnt();
	int iMaxContribute	= 0;
	for(i = 0;i < iRecordCnt;i++)
	{
		ModeRecord *pRecord = FindModeRecord( i );
		if( !pRecord ) continue;
		if( !pRecord->pUser ) continue;
		iMaxContribute += pRecord->iContribute;
	}

	// 옵저버 유저 제외
	int iOb = 0;
	for(i = 0;i < iRecordCnt;i++)
	{
		ModeRecord *pRecord = FindModeRecord( i );
		if( !pRecord ) continue;
		if( !pRecord->pUser ) continue;
		if( pRecord->pUser->IsObserver() || pRecord->pUser->IsStealth() )
			iOb++;
	}

	if( iMaxContribute > 0 )
	{
		for(i = 0;i < iRecordCnt;i++)
		{
			ModeRecord *pRecord = FindModeRecord( i );
			if( !pRecord ) continue;
			if( !pRecord->pUser ) continue;

			pRecord->fContributePer = (float)( iRecordCnt - iOb ) * ((float)pRecord->iContribute / iMaxContribute);
			LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "Mode::OnSuddenDeathPlayRecordInfo Per: %s - %.2f - %d - %d - %d", pRecord->pUser->GetPublicID().c_str(), pRecord->fContributePer, iOb, pRecord->iContribute, iMaxContribute );
		}	
	}	

	// 기여도 합으로 승패 결정
	float fBlueContributePer = 0.0f;
	float fRedContributePer  = 0.0f;
	for(i = 0;i < iRecordCnt;i++)
	{
		ModeRecord *pRecord = FindModeRecord( i );
		if( !pRecord ) continue;
		if( !pRecord->pUser ) continue;

		if( pRecord->pUser->GetTeam() == TEAM_BLUE )
			fBlueContributePer += pRecord->fContributePer;
		else if( pRecord->pUser->GetTeam() == TEAM_RED )
			fRedContributePer += pRecord->fContributePer;
	}

	//
	if( fBlueContributePer == fRedContributePer )
	{
		// 기여도가 같으면 주사위
		if( rand() % 2 == 0 )
		{
			fBlueContributePer += 0.01f;
			fRedContributePer  -= 0.01f;
		}
		else
		{
			fRedContributePer  += 0.01f;
			fBlueContributePer -= 0.01f;
		}
	}

	m_fSuddenDeathBlueCont     = fBlueContributePer;
	m_fSuddenDeathRedCont      = fRedContributePer;

	//
	SP2Packet kPacket( STPK_TOURNAMENT_SUDDEN_DEATH );
	PACKET_GUARD_VOID( kPacket.Write(TOURNAMENT_SUDDEN_DEATH_CONTRIBUTE) );
	PACKET_GUARD_VOID( kPacket.Write(m_fSuddenDeathBlueCont) );
	PACKET_GUARD_VOID( kPacket.Write(m_fSuddenDeathRedCont) );
	m_pCreator->RoomSendPacketTcp( kPacket );	

	CheckRoundEnd( false );
}

void Mode::OnAwardingInfo( User *pUser, SP2Packet &rkPacket )
{
	ioHashString szRecver;
	rkPacket >> szRecver;

	ModeRecord *pRecord = FindModeRecord( szRecver );
	if( !pRecord )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Mode::OnAwardingInfo() - %s is Not Exist.", szRecver.c_str() );
		return;
	}

	SP2Packet kReturnPacket( STPK_AWARDING_INFO );
	kReturnPacket.SetDataAdd( (char*)rkPacket.GetData(), rkPacket.GetDataSize() );
	pRecord->pUser->SendMessage( kReturnPacket );
}

int Mode::GetAwardPoint( float fPlayTime )
{
	float fUserPer = (float)m_pCreator->GetPlayUserCnt() / 2.0f ;
	float fPoint = fUserPer * fPlayTime * 100.0f;
	return fPoint;
}

float Mode::GetAwardBonus( int iAwardType, float fPlayTime )
{
	float fAwardBonus = 0.0f;
	AwardBonusMap::const_iterator iter = m_AwardBonusTable.find( iAwardType );
	if( iter != m_AwardBonusTable.end() )
		fAwardBonus = iter->second;
	if( fAwardBonus == 0.0f )
		return fAwardBonus;

	fAwardBonus *= GetAwardRandomBonus();
	float fUserPer = (float)min( 10, m_pCreator->GetPlayUserCnt() ) / 2.0f ;
	
	if( fAwardBonus > 0.00f )
		fAwardBonus = max( 0.01f, fAwardBonus * fUserPer );
	else if( fAwardBonus < 0.00f )
		fAwardBonus = max( -0.01f, fAwardBonus * fUserPer );
	return fAwardBonus;
}

float Mode::GetAwardRandomBonus()
{
	if( m_dwAwardRandBonusSeed == 0 )
		return 1.0f;

	DWORD dwCurValue = 0;
	DWORD dwRand = rand()%m_dwAwardRandBonusSeed;
	vAwardRandBonusData::iterator iter = m_AwardRandBonusTable.begin();
	for(;iter < m_AwardRandBonusTable.end();iter++)
	{
		AwardRandBonusData &rkData = *iter;
		if( COMPARE( dwRand, dwCurValue, dwCurValue + rkData.m_dwRand ) )
		{			
			return rkData.m_fBonus;
		}
		dwCurValue += rkData.m_dwRand;
	}
	return 1.0f;
}

bool Mode::CheckSpacialAwardUser( ioHashString &rkLuckyUser )
{
	if( m_dwSpacialAwardType == 0 ||  m_dwSpacialAwardLimitTime == 0 || m_fSpacialAwardCorrection == 0.0f ) return false;
	if( m_dwSpacialAwardRandSeed == 0 || m_dwModePointMaxTime == 0 || m_iSpacialAwardMaxUserCnt == 0 ) return false;

	int i = 0;
	int iPlayUserCount = 0;
	DWORD dwPlayTime = 0;
	for(i = 0;i < GetRecordCnt();i++)
	{
		ModeRecord *pRecord = FindModeRecord( i );
		if( pRecord == NULL ) continue;
		if( pRecord->eState == RS_LOADING ) continue;
		if( pRecord->pUser == NULL ) continue;
		if( pRecord->pUser->IsObserver() ) continue;                     // 옵져버 유저
		if( pRecord->pUser->IsStealth() ) continue;
		if( pRecord->iTotalDamage <= m_iAbuseMinDamage ) continue;       // 어뷰즈 유저

		iPlayUserCount++;
		dwPlayTime = max( dwPlayTime ,GetRecordPlayTime( pRecord ) );
	}

	if( iPlayUserCount <= m_iSpacialAwardLimitUserCnt ) return false;
	if( dwPlayTime < m_dwSpacialAwardLimitTime ) return false;

	// 긴 시간 플레이로 1.0f을 넘는 경우 제한
	float fTimeRate = ((float)dwPlayTime / m_dwSpecialAwardMaxTime);
	fTimeRate = min( fTimeRate, 1.0f );

	DWORD dwCurValue = m_fSpacialAwardCorrection * ((float)iPlayUserCount / m_iSpacialAwardMaxUserCnt) * fTimeRate;
	if( rand()%m_dwSpacialAwardRandSeed <= dwCurValue )
	{
		// 팀 플레이 경우에는 이긴팀 중 한명에게 행운상 지급
		TeamType eWinTeam  = TEAM_NONE;
		if( m_iBlueTeamWinCnt > m_iRedTeamWinCnt )
			eWinTeam = TEAM_BLUE;
		else if( m_iRedTeamWinCnt > m_iBlueTeamWinCnt )
			eWinTeam = TEAM_RED;

		ioHashStringVec kLuckyNameList;
		for(i = 0;i < GetRecordCnt();i++)
		{
			ModeRecord *pRecord = FindModeRecord( i );
			if( pRecord == NULL ) continue;
			if( pRecord->eState == RS_LOADING ) continue;
			if( pRecord->pUser == NULL ) continue;
			if( pRecord->pUser->IsObserver() ) continue;                     // 옵져버 유저
			if( pRecord->pUser->IsStealth() ) continue;
			if( pRecord->iTotalDamage <= m_iAbuseMinDamage ) continue;       // 어뷰즈 유저


			/* 몬스터 모드는 팀과 승패를 체크하지 않음 */
			if( GetModeType() == MT_BOSS || GetModeType() == MT_SURVIVAL || GetModeType() == MT_GANGSI || GetModeType() == MT_FIGHT_CLUB || GetModeType() == MT_SHUFFLE_BONUS )   // 싱글 모드에서는 기여도 100이상이 이긴팀
			{
				if( pRecord->fContributePer < 100.0f ) continue;
			}
			else if( GetModeType() != MT_MONSTER_SURVIVAL && GetModeType() != MT_DUNGEON_A && 
				GetModeType() != MT_RAID &&
				!Help::IsMonsterDungeonMode(GetModeType()) ) // 팀플 모드에서는 승리팀만 지급
			{
				if( pRecord->pUser->GetTeam() != eWinTeam ) continue;         
			}

			if( pRecord->pUser->GetBlockType() != BKT_NORMAL ) continue;     // 명성 주의 or 제한
			if( GetRecordPlayTime( pRecord ) < m_dwSpacialAwardLimitTime ) continue; // 1분 이내 플레이 유저는 제외

			kLuckyNameList.push_back( pRecord->pUser->GetPublicID() );
		}

		if( !kLuckyNameList.empty() )
		{
			int r = rand()%kLuckyNameList.size();
			rkLuckyUser = kLuckyNameList[r];
			return true;
		}
	}
	return false;
}

void Mode::OnAwardingResult( User *pUser, SP2Packet &rkPacket )
{
	if( m_bCheckAwardChoose ) return;

	m_bCheckAwardChoose = true;

	int i = 0;
	int iAwardSize = 0;
	PACKET_GUARD_VOID( rkPacket.Read(iAwardSize) );
	MAX_GUARD(iAwardSize, 50);

	if( m_bTournamentRoom || iAwardSize == 0 || m_dwModePointTime == 0 )
	{
		m_bAwardStage = false;
		return;	
	}

	// 스페셜 수상에 당첨될 유저 검색 : 행운상
	int iSendAwardSize = iAwardSize;
	ioHashString kLuckyUser;
	if( CheckSpacialAwardUser( kLuckyUser ) )
	{
		//행운상이 있으면 시상식중 마지막 하나를 뺀다. 단, 시상 갯수가 3개 이하이면 그냥 4번째 시상이 행운상.
		if( iAwardSize >= m_iSendAwardCount )  
		{
			iAwardSize -= 1;
		}
		else
		{
			iSendAwardSize += 1;       
		}
	}

	SP2Packet kReturnPacket( STPK_AWARDING_RESULT );
	PACKET_GUARD_VOID( kReturnPacket.Write(m_dwAwardingTime) );
	PACKET_GUARD_VOID( kReturnPacket.Write(iSendAwardSize) );

	// 일반 수상
	for(i = 0;i < iAwardSize;i++)
	{
		ioHashString szName;
		int iType = 0, iValue = 0;
		PACKET_GUARD_VOID( rkPacket.Read(iType) );
		PACKET_GUARD_VOID( rkPacket.Read(szName) );
		PACKET_GUARD_VOID( rkPacket.Read(iValue) );

		ModeRecord *pRecord = FindModeRecord( szName );
		if( !pRecord || !pRecord->pUser ) 
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Mode::OnAwardingResult() - %s is Not Exist.", szName.c_str() );
			continue;
		}

		float fPlayTimePer = (float)GetRecordPlayTime( pRecord ) / m_dwModeRecordPointTime;
		float fAwardBonus  = GetAwardBonus( iType, fPlayTimePer );		
		int iPoint = max( 1, GetAwardPoint( fPlayTimePer ) );	
		LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "Mode::OnAwardingResult() - RoomIndex[%d] - UserCount:[%d] - RoomStyle:[%d] - %s - Award[%d] - Point[%d] - Bonus[%.2f] - PlayPer[%.2f]", m_pCreator->GetRoomIndex(),
								m_pCreator->GetPlayUserCnt(), m_pCreator->GetRoomStyle(), pRecord->pUser->GetPublicID().c_str(), iType, iPoint, fAwardBonus, fPlayTimePer );
		pRecord->pUser->AddAward( iType, iPoint );
		pRecord->fBonusArray[BA_AWARD_BONUS] += fAwardBonus;

		PACKET_GUARD_VOID( kReturnPacket.Write(iType) );
		PACKET_GUARD_VOID( kReturnPacket.Write(pRecord->pUser->GetGradeLevel()) );
		PACKET_GUARD_VOID( kReturnPacket.Write(pRecord->pUser->GetTeam()) );
		PACKET_GUARD_VOID( kReturnPacket.Write(szName) );
		PACKET_GUARD_VOID( kReturnPacket.Write(iValue) );
		PACKET_GUARD_VOID( kReturnPacket.Write(iPoint) );
		PACKET_GUARD_VOID( kReturnPacket.Write(fAwardBonus) );

		{	// 이벤트로 특별 아이템 지급
			int iEtcItemCount = ( iPoint + 100 ) / 100;
			if( iEtcItemCount <= Help::GetAwardEtcItemBonusAbusePoint() )
				iEtcItemCount = 0;
			PACKET_GUARD_VOID( kReturnPacket.Write(Help::GetAwardEtcItemBonus()) );
			PACKET_GUARD_VOID( kReturnPacket.Write(iEtcItemCount) );
			// 선물로 지급
			g_PresentHelper.SendAwardEtcItemBonus( pRecord->pUser, Help::GetAwardEtcItemBonus(), iEtcItemCount );
		}

		// 가장 긴 시간 플레이했던 용병의 정보(치장 포함)
		int iClassType = pRecord->GetHighPlayingClass();
		pRecord->pUser->FillClassData( iClassType, false, kReturnPacket );
		pRecord->pUser->FillEquipMedalItemByClassType( iClassType, kReturnPacket );
		pRecord->pUser->FillEquipCustomMedalItemByClassType( iClassType, kReturnPacket );
	}
	// 행운상
	if( !kLuckyUser.IsEmpty() )
	{
		ModeRecord *pRecord = FindModeRecord( kLuckyUser );
		if( pRecord && pRecord->pUser ) 
		{
			float fPlayTimePer = (float)GetRecordPlayTime( pRecord ) / m_dwModeRecordPointTime;
			float fAwardBonus  = GetAwardBonus( m_dwSpacialAwardType, fPlayTimePer );		
			int iPoint = max( 1, GetAwardPoint( fPlayTimePer ) );	
			LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "Mode::OnAwardingResult() - RoomIndex[%d] - UserCount:[%d] - RoomStyle:[%d] - %s - Award[%d] - Point[%d] - Bonus[%.2f] - PlayPer[%.2f]", m_pCreator->GetRoomIndex(),
				m_pCreator->GetPlayUserCnt(), m_pCreator->GetRoomStyle(), pRecord->pUser->GetPublicID().c_str(), m_dwSpacialAwardType, iPoint, fAwardBonus, fPlayTimePer );
			pRecord->pUser->AddAward( m_dwSpacialAwardType, iPoint );
			pRecord->fBonusArray[BA_AWARD_BONUS] += fAwardBonus;
			
			PACKET_GUARD_VOID( kReturnPacket.Write(m_dwSpacialAwardType) );
			PACKET_GUARD_VOID( kReturnPacket.Write(pRecord->pUser->GetGradeLevel()) );
			PACKET_GUARD_VOID( kReturnPacket.Write(pRecord->pUser->GetTeam()) );
			PACKET_GUARD_VOID( kReturnPacket.Write(kLuckyUser) );
			PACKET_GUARD_VOID( kReturnPacket.Write(0) );
			PACKET_GUARD_VOID( kReturnPacket.Write(iPoint) );
			PACKET_GUARD_VOID( kReturnPacket.Write(fAwardBonus) );

			{	// 이벤트로 특별 아이템 지급
				int iEtcItemCount = ( iPoint + 100 ) / 100;
				if( iEtcItemCount <= Help::GetAwardEtcItemBonusAbusePoint() )
					iEtcItemCount = 0;
				kReturnPacket << Help::GetAwardEtcItemBonus() << iEtcItemCount;
				// 선물로 지급
				g_PresentHelper.SendAwardEtcItemBonus( pRecord->pUser, Help::GetAwardEtcItemBonus(), iEtcItemCount );
			}

			// 가장 긴 시간 플레이했던 용병의 정보(치장 포함)
			int iClassType = pRecord->GetHighPlayingClass();
			pRecord->pUser->FillClassData( iClassType, false, kReturnPacket );	
			pRecord->pUser->FillEquipMedalItemByClassType( iClassType, kReturnPacket );

			int iPresentType = 0, iPresentValue1 = 0, iPresentValue2 = 0;
			bool bAlarm = false;
			// 선물 지급
			g_PresentHelper.SendSpacialAwardPresent( pRecord->pUser, kReturnPacket, iPresentType, iPresentValue1, iPresentValue2, bAlarm );

			// award lucky present에 관한 전서버 알림
			g_UserNodeManager.AlarmEventInAwardLuckyPresentToAllUsers(pRecord->pUser, bAlarm, iPresentType, iPresentValue1, iPresentValue2 );
		}		
		else
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Mode::OnAwardingResult() - %s is Not Exist.", kLuckyUser.c_str() );
	}
	SendRoomAllUser( kReturnPacket );

	m_bAwardStage = true;	
}

void Mode::OnRoundEnd( User *pUser, SP2Packet &rkPacket )
{
}

void Mode::OnRoundEndContribute( User *pUser, SP2Packet &rkPacket )
{
}

void Mode::LeavePartyByStyle( User *pUser, RoomStyle eStyle )
{
	if( !pUser )
		return;

	switch( eStyle )
	{
	case RSTYLE_BATTLEROOM:
		pUser->LeaveBattleRoom();
		break;
	case RSTYLE_LADDERBATTLE:
		pUser->LeaveLadderTeam();
		break;
	case RSTYLE_SHUFFLEROOM:
		pUser->LeaveShuffleRoom();
		break;
	}
}

void Mode::OnUserKickVote( User *pUser, SP2Packet &rkPacket )
{
	if( m_bTournamentRoom )
		return;     // 대회 경기방은 강퇴 안됨

	int iVoteType;
	rkPacket >> iVoteType;

	switch( iVoteType )
	{
	case USER_KICK_VOTE_PROPOSAL:            //제안
		{
			ioHashString szKickName, szKickReason;
			rkPacket >> szKickName >> szKickReason;

			// 예외 처리			
			if( pUser->GetPublicID() == szKickName ) // 자신의 아이디인지 확인
			{
				SP2Packet kPacket( STPK_USER_KICK_VOTE );
				kPacket << USER_KICK_VOTE_PROPOSAL_ERROR_1;
				pUser->SendMessage( kPacket );
				return;
			}

			if( g_UserNodeManager.IsDeveloper( szKickName.c_str() ) ) // 개발자 & 마스터 & 매니저
			{
				SP2Packet kPacket( STPK_USER_KICK_VOTE );
				kPacket << USER_KICK_VOTE_PROPOSAL_ERROR_2;
				pUser->SendMessage( kPacket );
				return;
			}

			if( pUser->GetBlockType() != BKT_NORMAL )
			{
				SP2Packet kPacket( STPK_USER_KICK_VOTE );
				kPacket << USER_KICK_VOTE_PROPOSAL_ERROR_3;
				pUser->SendMessage( kPacket );
				return;
			}

			if( pUser->IsObserver() || pUser->IsStealth() )      // 관전자는 투표 제안 X
			{
				SP2Packet kPacket( STPK_USER_KICK_VOTE );
				kPacket << USER_KICK_VOTE_PROPOSAL_ERROR_4;
				pUser->SendMessage( kPacket );
				return;
			}

			if( m_KickOutVote.IsVoting() ) // 현재 투표가 진행중인지 확인
			{
				SP2Packet kPacket( STPK_USER_KICK_VOTE );
				kPacket << USER_KICK_VOTE_PROPOSAL_ERROR_5;
				pUser->SendMessage( kPacket );
				return;
			}

			if( m_KickOutVote.IsVoteProposalUser( pUser->GetPublicID() ) ) // 1회 투표 제안을 했던 유저 처리
			{
				SP2Packet kPacket( STPK_USER_KICK_VOTE );
				kPacket << USER_KICK_VOTE_PROPOSAL_ERROR_6;
				pUser->SendMessage( kPacket );
				return;
			}

			ModeRecord *pKickUserRecord = FindModeRecord( szKickName );
			if( !pKickUserRecord ) // 대상가 동일한 룸에 있는지 확인
			{
				SP2Packet kPacket( STPK_USER_KICK_VOTE );
				kPacket << USER_KICK_VOTE_PROPOSAL_ERROR_7;
				pUser->SendMessage( kPacket );
				return;
			}

			if( pKickUserRecord->pUser->GetUserEventType() == USER_TYPE_BROADCAST_AFRICA )      //MC 유저인지 확인
			{
				SP2Packet kPacket( STPK_USER_KICK_VOTE );
				kPacket << USER_KICK_VOTE_PROPOSAL_ERROR_8;
				pUser->SendMessage( kPacket );
				return;
			}					

			// 모드에 따른 투표 불가 처리
			int iModeLimitType = GetUserKickVoteLimit( szKickName );
			if( iModeLimitType != 0 )
			{
				SP2Packet kPacket( STPK_USER_KICK_VOTE );
				kPacket << iModeLimitType << m_KickOutVote.GetKickVoteUserPool() << m_KickOutVote.GetKickVoteRoundWin() << m_KickOutVote.GetKickVoteRoundTime();
				pUser->SendMessage( kPacket );
				return;
			}

			if( m_pCreator && m_pCreator->GetRoomStyle() == RSTYLE_SHUFFLEROOM )
			{
				DWORD dwEnableTime = g_ShuffleRoomManager.GetKickOutVoteEnableTime();
				if( dwEnableTime != 0 && dwEnableTime < TIMEGETTIME() - GetModeStartTime() )
				{
					SP2Packet kPacket( STPK_USER_KICK_VOTE );
					kPacket << USER_KICK_VOTE_PROPOSAL_ERROR_14;
					pUser->SendMessage( kPacket );
					return;
				}
			}

			m_KickOutVote.StartVote( pUser->GetPublicID(), szKickName, szKickReason );

			// 이상 없으면 옵저버를 제외하고 강퇴 투표 진행
			SP2Packet kPacket( STPK_USER_KICK_VOTE );
			kPacket << USER_KICK_VOTE_PROPOSAL << pUser->GetPublicID() << szKickName << szKickReason << m_KickOutVote.GetVoteProcessTime();
			
			int iRecordCnt = GetRecordCnt();
			for(int i = 0;i < iRecordCnt;i++)
			{
				ModeRecord *pRecord = FindModeRecord( i );
				if( !pRecord || !pRecord->pUser ) continue;
				if( pRecord->pUser->IsObserver() ) continue;
				if( pRecord->pUser->IsStealth() ) continue;
				if( pRecord->pUser->GetPublicID() == szKickName ) continue;				

				// 투표 제안 전송
                pRecord->pUser->SendMessage( kPacket );
		
				// 투표 용지 지급
				m_KickOutVote.InsertVoteUserList( pRecord->pUser->GetPublicID(), pRecord->pUser->GetTeam() );
			}

			// 투표 제안한 유저는 일단 찬성 처리
			m_KickOutVote.SetKickVote( pUser->GetPublicID(), USER_KICK_VOTE_APPROVE );
		}
		break;
	case USER_KICK_VOTE_APPROVE:        // 찬성
	case USER_KICK_VOTE_OPPOSITION:     // 반대
	case USER_KICK_VOTE_ABSTENTION:     // 기권
		if( m_KickOutVote.IsVoting() )
		{
			ioHashString szKickName;
			rkPacket >> szKickName;

			if( szKickName == m_KickOutVote.GetVoteTargetName() )
			{
				m_KickOutVote.SetKickVote( pUser->GetPublicID(), iVoteType );
			}
		}
		break;
	}	
}

void Mode::SetStopTime( bool bStop )
{
	if( !bStop && m_bStopTime )
	{
		m_dwStateChangeTime = TIMEGETTIME();
		m_bStopTime = false;

		SP2Packet kPacket( STPK_ROUND_TIME_STOP );
		kPacket << false;
		SendRoomAllUser( kPacket );
	}
}

void Mode::ClearObjectItem()
{
	int iCharCnt = GetRecordCnt();
	for( int i = 0; i < iCharCnt; i++)
	{
		ModeRecord *pRecord = FindModeRecord( i );
		if(pRecord->pUser)
		{
			ioItem *pItem = pRecord->pUser->ReleaseItem(EQUIP_OBJECT);
			SAFEDELETE( pItem );
		}
	}
}

bool Mode::IsCanPickItemState()
{
	if( m_ModeState == MS_PLAY )
		return true;

	return false;
}

bool Mode::IsEnableState( User *pUser )
{
	ModeRecord *pRecord = FindModeRecord( pUser );
	if( pRecord )
	{
		if( GetModeType() == MT_HOUSE )
			return true;

		if( pRecord->eState != RS_LOADING )
			return true;
	}

	return false;
}

bool Mode::IsCheckAllDamageList()
{
	int iRecordCnt = GetRecordCnt();
	for(int i = 0;i < iRecordCnt;i++ )       
	{
		ModeRecord *pRecord = FindModeRecord( i );
		if( !pRecord || !pRecord->pUser ) continue;
		if( pRecord->eState != RS_PLAY ) continue;
		
		if( !pRecord->bRecvDamageList ) 
			return false;
	}
	return true;
}

void Mode::SendScoreGauge()
{
}

bool Mode::SetLeaveUserPenalty( User *pLeave )
{
	if( IsRoundSetEnd() )
	{
		// MS_RESULT_WAIT 와 MS_RESULT의 차이는 WAIT의 경우에는 아직 Record가 기록되지 않은 상태이다.
		if( GetState() == MS_RESULT ) 
			return false;
	}
	if( !pLeave ) return false;
	
	switch( GetModeType() )
	{
	case MT_HEROMATCH:
		{
			// 영웅전에서는 패배 처리
			pLeave->AddLoseCount( m_pCreator->GetRoomStyle(), GetModeType(), m_iLosePoint );
			return true;
		}
		break;
	}

	return false;
}

bool Mode::CheckRoundJoin( User *pSend )
{
	if( m_ModeState != MS_RESULT && m_ModeState != MS_RESULT_WAIT )
		return false;

	ModeRecord *pRecord = FindModeRecord( pSend );
	if( !pRecord )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Mode::CheckRoundJoin - %s Not Exist Record",
								 pSend->GetPublicID().c_str() );
		return false;
	}

	pRecord->pUser->EquipDBItemToAllChar();
	SetFirstRevivalTime( pRecord );

	int iModeState = MS_RESULT_WAIT;

	DWORD dwPastTime = TIMEGETTIME() - m_dwStateChangeTime;

	pRecord->eState = RS_PLAY;
	pRecord->StartPlaying();        //( 관전X, 데스타임X )
	pRecord->pUser->StartCharLimitDate( GetCharLimitCheckTime(), __FILE__, __LINE__ );
	pRecord->pUser->StartEtcItemTime( __FUNCTION__ );

	SP2Packet kPacket( STPK_ROUND_JOIN );
	kPacket << pRecord->pUser->GetPublicID();
	kPacket << iModeState;
	kPacket << dwPastTime;
	kPacket << GetSelectCharTime();
	SendRoomAllUser( kPacket );

	return true;
}

void Mode::FillExperienceMode( User *pUser, SP2Packet &rkPacket )
{
	ModeRecord *pRecord = FindModeRecord( pUser );
	if( pRecord )
	{
		if( pRecord->bExperienceState )
			rkPacket << pRecord->bExperienceState << pRecord->szExperienceID << pRecord->iExperienceClassType;
		else
			rkPacket << false;
	}
	else
	{
		rkPacket << false;
	}
}

bool Mode::IsPlayCharHireTimeStop( ModeRecord *pRecord )
{
	if( !pRecord ) return false;
	if( pRecord->GetCharHireCheckTime() > m_dwModePointMaxTime )
		return true;
	return false;
}

int Mode::GetKillPoint( TeamType eMyTeam )
{
	int iBlueCnt = GetTeamUserCnt( TEAM_BLUE );
	int iRedCnt  = GetTeamUserCnt( TEAM_RED );
	if( iBlueCnt == 0 || iRedCnt == 0 )     //블루 레드가 아니면 서바의 개인팀이다.	
		return m_iKillDeathPoint;

	float fKillPer = 1.0f;
	switch( eMyTeam )
	{
	case TEAM_BLUE:
		fKillPer = (float)iBlueCnt / iRedCnt;
		break;
	case TEAM_RED:
		fKillPer = (float)iRedCnt / iBlueCnt;
		break;	
	}	

	if( fKillPer > 1.0f )
		return m_iKillDeathPoint;

	float fKillPoint = m_iKillDeathPoint;
	return ( fKillPoint * fKillPer ) + 0.5f;
}

int Mode::GetDeathPoint( TeamType eMyTeam )
{
	int iBlueCnt = GetTeamUserCnt( TEAM_BLUE );
	int iRedCnt  = GetTeamUserCnt( TEAM_RED );
	if( iBlueCnt == 0 || iRedCnt == 0 )     //블루 레드가 아니면 서바의 개인팀이다.	
		return m_iKillDeathPoint;

	float fDeathPer = 1.0f;
	switch( eMyTeam )
	{
	case TEAM_BLUE:
		fDeathPer = (float)iRedCnt / iBlueCnt;
		break;
	case TEAM_RED:
		fDeathPer = (float)iBlueCnt / iRedCnt;
		break;	
	}	

	if( fDeathPer > 1.0f )
		return m_iKillDeathPoint;

	float fDeathPoint = m_iKillDeathPoint;
	return ( fDeathPoint * fDeathPer ) + 0.5f;
}

int Mode::GetWinLoseTiePoint( TeamType eMyTeam, TeamType eWinTeam, float fPlayTimePer )
{
	int iMyTeamCnt	  = 0;
	int iOtherTeamCnt = 0;
	switch( eMyTeam )
	{
	case TEAM_BLUE:
		{
			iMyTeamCnt    = GetTeamUserCnt( TEAM_BLUE );
			iOtherTeamCnt = GetTeamUserCnt( TEAM_RED );
		}
		break;
	case TEAM_RED:
		{
			iMyTeamCnt    = GetTeamUserCnt( TEAM_RED );
			iOtherTeamCnt = GetTeamUserCnt( TEAM_BLUE );
		}
		break;
	default:
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Mode::GetWinLoseTiePoint Error : %d - %d - %.2f", (int)eMyTeam, (int)eWinTeam, fPlayTimePer );
		return 0;
	}

	if( eWinTeam == TEAM_NONE )           //무
	{
		int iReturnPoint  = (float)m_iDrawPoint * fPlayTimePer;
		return min( m_iDrawPoint, iReturnPoint );
	}
	else if( eWinTeam == eMyTeam )         //승
	{
		int iHighValue    = ( iMyTeamCnt + iOtherTeamCnt ) * m_iDrawPoint;
		int iLowValue     = ( iMyTeamCnt * m_iWinPoint ) + ( iOtherTeamCnt * m_iLosePoint );
		int iReturnPoint  = (float)m_iWinPoint * ( (float)iHighValue / iLowValue ) * fPlayTimePer;
		return min( m_iWinPoint, iReturnPoint );
	}
	else                              //패
	{
		int iHighValue    = ( iMyTeamCnt + iOtherTeamCnt ) * m_iDrawPoint;
		int iLowValue     = ( iMyTeamCnt * m_iLosePoint ) + ( iOtherTeamCnt * m_iWinPoint );
		int iReturnPoint  = (float)m_iLosePoint * ( (float)iHighValue / iLowValue ) * fPlayTimePer;
		return min( m_iLosePoint, iReturnPoint );
	}
	return 0;
}

int Mode::GetWinLoseTiePoint( float fContributePer, float fPlayTimePer )
{
	if( fContributePer >= 1.0f )
		return min( (float)m_iWinPoint * fPlayTimePer, m_iWinPoint );
	return min( (float)m_iLosePoint * fPlayTimePer, m_iLosePoint );
}

void Mode::AddTeamKillPoint( TeamType eTeam, int iKillPoint )
{
	if( m_pCreator->GetRoomStyle() != RSTYLE_LADDERBATTLE ) return;

	if( eTeam == TEAM_BLUE )
		m_iBlueKillCount += iKillPoint;
	else if( eTeam == TEAM_RED )
		m_iRedKillCount += iKillPoint;

#ifdef FLAG_MODE_BY_BCKIM_DEBUG		// 전투시작 임시 값 확인  OnBattleRoomCommand
	LOG.PrintTimeAndLog(0,"[FLAG_MODE_BY_BCKIM:AddTeamKillPoint] m_iBlueKillCount[%d]m_iRedKillCount[%d]iKillPoint[%d]",m_iBlueKillCount,m_iRedKillCount,iKillPoint);
#endif // FLAG_MODE_BY_BCKIM_DEBUG


}

void Mode::AddTeamDeathPoint( TeamType eTeam, int iDeathPoint )
{
	if( m_pCreator->GetRoomStyle() != RSTYLE_LADDERBATTLE ) return;

	if( eTeam == TEAM_BLUE )
		m_iBlueDeathCount += iDeathPoint;
	else if( eTeam == TEAM_RED )
		m_iRedDeathCount += iDeathPoint;
}

int Mode::GetSameFriendUserCnt( User *pUser )
{
	if( !pUser ) return 0;
	
	CTimeSpan minusTime( Help::GetFriendBonusBeforeDay(), Help::GetFriendBonusBeforeHour(), Help::GetFriendBonusBeforeMin(), 0 );
	CTime cBeforeHour  = CTime::GetCurrentTime() - minusTime;
	DWORD dwBeforeHour = Help::ConvertCTimeToDate( cBeforeHour );

	if( !Help::IsRoomFriendCnt() )
		return pUser->GetFriendOnlineUserCount( dwBeforeHour );

	int iReturnCount = 0;
	int iRecordCnt = GetRecordCnt();
	for( int i=0 ; i<iRecordCnt ; i++ )
	{
		ModeRecord *pRecord = FindModeRecord( i );
		if( !pRecord || !pRecord->pUser ) continue;
		if( pRecord->pUser == pUser ) continue;
		if( pRecord->pUser->IsDeveloper() ) continue;
		if( pUser->IsFriendRegHourCheck( pRecord->pUser->GetPublicID(), dwBeforeHour ) )
			iReturnCount++;
	}
	return iReturnCount;
}

float Mode::GetMaxFriendBonus()
{
	return Help::GetMaxFriendBonus() * m_fMaxFriendBonus;
}

float Mode::GetFriendBonus()
{
	return Help::GetFriendBonus() * m_fFriendBonus;
}

float Mode::GetPcRoomMaxFriendBonus()
{
	return Help::GetPcRoomMaxFriendBonus() * m_fMaxFriendBonus;
}

float Mode::GetPcRoomFriendBonus()
{
	return Help::GetPcRoomFriendBonus() * m_fFriendBonus;
}

void Mode::SetModeEndDBLog( ModeRecord *pRecord, int iMaxRecord, int iLogType )
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
		{
			// 데스타임이 없는 모드는 세팅하지 않음.
			if( GetModeType() != MT_TRAINING && GetModeType() != MT_HEADQUARTERS )
			{
				pRecord->AddDeathTime( TIMEGETTIME() - pRecord->pUser->GetStartTimeLog() );
			}
		}
		g_LogDBClient.OnInsertPlayResult( pRecord, (LogDBClient::PlayResultType) iLogType );
	}
	else
	{
		if( pRecord->pUser->GetStartTimeLog() > 0 )
			g_LogDBClient.OnInsertTime( pRecord->pUser, LogDBClient::TT_VIEW );
	}
	pRecord->bWriteLog = true;
	pRecord->pUser->SetStartTimeLog(0);

	if( iLogType != LogDBClient::PRT_END_SET )
		return;

	if( pRecord->pUser->IsBattleRoom() || pRecord->pUser->IsLadderTeam() || pRecord->pUser->IsShuffleRoom() )
		pRecord->pUser->SetStartTimeLog(TIMEGETTIME());
}

void Mode::SetModeEndDBLog( Room* pRoom, int iLogType )
{
	g_LogDBClient.OnInsertPlayRoomResult( pRoom, (LogDBClient::PlayResultType) iLogType );
}

DWORD Mode::GetRemainPlayTime()
{
	switch( m_ModeState )
	{
	case MS_READY:
		return m_dwRoundDuration;
	case MS_PLAY:
		return m_dwRoundDuration - min( m_dwRoundDuration, ( TIMEGETTIME() - m_dwStateChangeTime ) );
	}
	return 0;
}

void Mode::SetChatModeState( const ioHashString &rkName, bool bChatMode )
{
	ModeRecord *pRecord = FindModeRecord( rkName );
	if( pRecord )
	{
		pRecord->bChatModeState = bChatMode;
	}
}

bool Mode::IsChatModeState( User *pUser )
{
	ModeRecord *pRecord = FindModeRecord( pUser );
	if( pRecord )
	{
		return pRecord->bChatModeState;
	}

	return false;
}

bool Mode::IsExperienceModeState( User *pUser )
{
	ModeRecord *pRecord = FindModeRecord( pUser );
	if( pRecord )
	{
		return pRecord->bExperienceState;
	}

	return false;
}

void Mode::SetFishingState( const ioHashString &rkName, bool bFishing )
{
	ModeRecord *pRecord = FindModeRecord( rkName );
	if( pRecord )
	{
		pRecord->bFishingState = bFishing;
	}
}

bool Mode::IsFishingState( const ioHashString &rkName )
{
	ModeRecord *pRecord = FindModeRecord( rkName );
	if( pRecord )
	{
		return pRecord->bFishingState;
	}

	return false;
}

void Mode::ProcessEvent()
{
	if( m_ModeState != MS_PLAY )	return;

	DWORD dwCurTime = TIMEGETTIME();

	if( m_dwEventCheckTime == 0 )
	{
		m_dwEventCheckTime = TIMEGETTIME();
		return;
	}

	if( m_dwEventCheckTime + MAX_EVENT_CHECK_MS < dwCurTime )
	{
		int iRecordCnt = GetRecordCnt();
		for( int i=0 ; i<iRecordCnt ; i++ )
		{
			ModeRecord *pRecord = FindModeRecord( i );
			if( !pRecord || !pRecord->pUser ) continue;
			if( pRecord->pUser->IsObserver() ) continue;
			if( pRecord->pUser->IsStealth() ) continue;

			pRecord->pUser->GetEventUserMgr().Process( pRecord->pUser );
		}
		m_dwEventCheckTime = dwCurTime;
	}
}

void Mode::ProcessBonusAlarm()
{
	if( m_ModeState != MS_PLAY ) return;

	if( m_dwBonusAlarmTime == 0 )
		m_dwBonusAlarmTime = TIMEGETTIME();

	if( TIMEGETTIME() - m_dwBonusAlarmTime < 2000 ) 
		return;

	//플레이 중인 모드
	ModeCategory ePlayMode = GetPlayModeCategory();

	m_dwBonusAlarmTime = TIMEGETTIME();
	int iRecordCnt = GetRecordCnt();
	for( int i=0 ; i<iRecordCnt ; i++ )
	{
		ModeRecord *pRecord = FindModeRecord( i );
		if( !pRecord || !pRecord->pUser ) continue;
		if( pRecord->bBonusAlarmSend ) continue;
		if( pRecord->pUser->IsObserver() ) continue;
		if( pRecord->pUser->IsStealth() ) continue;

		pRecord->bBonusAlarmSend = true;
		float fFriendBonus, fPCRoomBonus, fGuildBonus;
		fFriendBonus = fPCRoomBonus = fGuildBonus = 0.00f;

		//친구 보너스
		if( pRecord->pUser->IsPCRoomAuthority() )
		{		
			fFriendBonus = min( GetPcRoomMaxFriendBonus(), GetPcRoomFriendBonus() * (float)GetSameFriendUserCnt( pRecord->pUser ) );
		}
		else
		{
			fFriendBonus = min( GetMaxFriendBonus(), GetFriendBonus() * (float)GetSameFriendUserCnt( pRecord->pUser ) );
		}

		//길드보너스
		fGuildBonus = m_pCreator->GetGuildBonus( pRecord->pUser->GetTeam() );
		//PC방 보너스
		float fPCRoomBonusExp = 0.0f;
		float fPCRoomBonusPeso= 0.0f;
		if( pRecord->pUser->IsPCRoomAuthority() )
		{
			fPCRoomBonusExp  = Help::GetPCRoomBonusExp();
			fPCRoomBonusPeso = Help::GetPCRoomBonusPeso();

			if( g_EventMgr.IsAlive( EVT_PCROOM_BONUS, pRecord->pUser->GetChannelingType(), ePlayMode ) )
			{
				EventUserManager &rEventUserManager = pRecord->pUser->GetEventUserMgr();
				PCRoomEventUserNode* pPcroomEventNode = static_cast<PCRoomEventUserNode*> ( rEventUserManager.GetEventUserNode( EVT_PCROOM_BONUS, ePlayMode ) );
			
				if( pPcroomEventNode )
					pPcroomEventNode->SetPesoAndExpBonus( pRecord->pUser ,fPCRoomBonusPeso, fPCRoomBonusExp, ePlayMode ); 
			}
		}

		if( fFriendBonus > 0.00f || fPCRoomBonusExp > 0.00f || fPCRoomBonusPeso > 0.00f || fGuildBonus > 0.00f )
		{ 
			SP2Packet kPacket( SUPK_SERVER_ALARM_MENT );
			PACKET_GUARD_VOID( kPacket.Write( UDP_SERVER_ALARM_MODE_BONUS ) );
			PACKET_GUARD_VOID( kPacket.Write( fFriendBonus ) );
			PACKET_GUARD_VOID( kPacket.Write( fPCRoomBonusExp ) );
			PACKET_GUARD_VOID( kPacket.Write( fPCRoomBonusPeso ) );
			PACKET_GUARD_VOID( kPacket.Write( fGuildBonus ) );

			g_UDPNode.SendMessage( pRecord->pUser->GetPublicIP(), pRecord->pUser->GetUDP_port(), kPacket );
		}
	}
}

void Mode::OnDropDieUser( User *pDieUser, SP2Packet &rkPacket )
{
	if( pDieUser == NULL ) 
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Mode::OnDropDieUser None User!!" );
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

#ifdef FLAG_MODE_BY_BCKIM
	if ( pDieUser != NULL && pDieUser->GetModeType()== MT_FLAG ) //	&& szLastAttackerName.IsEmpty() == false )  // 2018-06-22 by bckim, 깃발모드 자살시에도 점수 줘라  
	{	// pDieUser->m_pMyRoom->GetModeInfo()
		FlagMode *pFlag = ToFlagMode( pDieUser->GetMyRoom()->GetModeInfo() );
		pFlag->FlagModeAddPoint(pDieUser->GetTeam(), pDieUser);

#ifdef FLAG_MODE_BY_BCKIM_DEBUG
		LOG.PrintTimeAndLog(0,"[FLAG_MODE_BY_BCKIM:DropDie]:DIE_ID(%s), attack(%s)",pDieUser->GetPublicID().c_str(), szLastAttackerName.c_str());
#endif // FLAG_MODE_BY_BCKIM_DEBUG
	}
#endif 

	UpdateUserDieNextProcess( pRecord->pUser, szLastAttackerName, szBestAttackerName );
	//relay
#ifdef ANTIHACK
	SendRelayGroupTeamScore( pRecord->pUser );
#endif
}

void Mode::OnDropDieNpc( const ioHashString &rkDieName, SP2Packet &rkPacket )
{
}

void Mode::OnWeaponDieUser( User *pDieUser, SP2Packet &rkPacket )
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

	//relay
#ifdef ANTIHACK
	SendRelayGroupTeamScore( pDieUser );
#endif
}

void Mode::OnWeaponDieNpc( const ioHashString &rkDieName, SP2Packet &rkPacket )
{
}

void Mode::LoadModeAwardValue( ioINILoader &rkLoader )
{
	// 스페셜 시상 : 행운상
	rkLoader.SetTitle( "SpacialAward" );
	m_dwSpacialAwardType = rkLoader.LoadInt( "AwardType", 0 );
	m_dwSpacialAwardLimitTime = rkLoader.LoadInt( "LimitTime", 0 );
	m_iSpacialAwardLimitUserCnt = rkLoader.LoadInt( "LimitUserCount", 0 );
	m_iSpacialAwardMaxUserCnt   = rkLoader.LoadInt( "MaxUserCount", 0 );
	m_fSpacialAwardCorrection = rkLoader.LoadFloat( "AwardCorrection", 0.0f );
	m_dwSpacialAwardRandSeed = rkLoader.LoadInt( "AwardRandSeed", 0 );
	m_dwSpecialAwardMaxTime = rkLoader.LoadInt( "AwardMaxTime", 0 );
}

void Mode::LoadHeroTitleValue( ioINILoader &rkLoader )
{
	//
	rkLoader.SetTitle( "HeroTitleBonus" );
	int iMaxBonus = rkLoader.LoadInt( "MaxBonus", 0 );
	for(int i = 0;i < iMaxBonus;i++)
	{
		char szKey[MAX_PATH] = "";
		
		sprintf_s( szKey, "Title%d_Type", i + 1 );
		DWORD dwTitleType = rkLoader.LoadInt( szKey, 0 );
	
		sprintf_s( szKey, "Title%d_Bonus", i + 1 );
		float fTitleBonus = rkLoader.LoadFloat( szKey, 0.0f );
        
		m_HeroTitleBonusMap.insert( HeroTitleBonusMap::value_type( dwTitleType, fTitleBonus ) );
	}
}

void Mode::LoadEtcItemSyncList( ioINILoader &rkLoader )
{
	//
	rkLoader.SetTitle( "EtcItemSync" );
	int iMaxEtcItem = rkLoader.LoadInt( "MaxEtcItem", 0 );
	for(int i = 0;i < iMaxEtcItem;i++)
	{
		char szKey[MAX_PATH] = "";
		sprintf_s( szKey, "EtcItem%d", i + 1 );
		m_vEtcItemSyncList.push_back( rkLoader.LoadInt( szKey, 0 ) );
	}
}

void Mode::LoadShuffleInfo( ioINILoader &rkLoader )
{
	if( m_pCreator->GetRoomStyle() != RSTYLE_SHUFFLEROOM )
		return;

	rkLoader.SetTitle( "shuffle_round" );

	int iMaxRound = rkLoader.LoadInt( "shuffle_max_round", 0 );
	if( iMaxRound > 0 )
		m_iMaxRound = iMaxRound;

	int iNeedRound = rkLoader.LoadInt( "shuffle_win_need_round", 0 );
	if( iNeedRound > 0 )
		m_iNeedRound = iNeedRound;

	DWORD dwRoundDuration = (DWORD)rkLoader.LoadInt( "shuffle_round_time", 0 );
	if( dwRoundDuration > 0 )
		m_dwRoundDuration = dwRoundDuration;
}

void Mode::OnRepositionBallStruct( SP2Packet &rkPacket )
{
	int iIndex;
	rkPacket >> iIndex;

	int iCnt = m_vBallStructList.size();
	for( int i=0; i < iCnt; i++ )
	{
		if( m_vBallStructList[i].m_iIndex != iIndex )
			continue;

		SP2Packet kPacket( STPK_BALLSTRUCT_REPOSITION );
		kPacket << iIndex;
		kPacket << m_vBallStructList[i].m_CreatePos;
		SendRoomAllUser( kPacket );
		return;
	}
}

Vector3 Mode::GetBallStartPosition()
{
	IORandom eRandom;
	eRandom.Randomize();

	int iCnt = m_vBallPositionList.size();
	int iIndex = eRandom.Random( iCnt );
	if( COMPARE( iIndex, 0, iCnt ) )
	{
		return m_vBallPositionList[iIndex];
	}

	return Vector3( 0.0f, 0.0f, 0.0f );
}

void Mode::CheckMachineSupply( DWORD dwStartTime )
{
	if( m_ModeState != MS_PLAY )	return;

	int iSupplyTimeCnt = m_vMachineSupplyList.size();
	DWORD dwTimeGap = TIMEGETTIME() - dwStartTime;

	if( !COMPARE( m_iCurMachineSupplyIdx, 0, iSupplyTimeCnt ) )
		return;

	LOOP_GUARD();
	MachineStructList vMachineList;
	while( m_vMachineSupplyList[m_iCurMachineSupplyIdx].m_dwSupplyTime <= dwTimeGap )
	{
		m_iMachineStructIdx++;

		MachineStruct eMachine;
		eMachine.m_iIndex = m_iMachineStructIdx;
		eMachine.m_iNum = m_vMachineSupplyList[m_iCurMachineSupplyIdx].m_iNum;
		eMachine.m_TargetRot = m_vMachineSupplyList[m_iCurMachineSupplyIdx].m_TargetRot;
		eMachine.m_CreatePos = m_vMachineSupplyList[m_iCurMachineSupplyIdx].m_CreatePos;

		m_vMachineStructList.push_back( eMachine );
		vMachineList.push_back( eMachine );

		m_iCurMachineSupplyIdx++;

		if( !COMPARE( m_iCurMachineSupplyIdx, 0, iSupplyTimeCnt ) )
			break;
	}
	LOOP_GUARD_CLEAR();

	if( vMachineList.empty() )
		return;

	SP2Packet kMachinePacket( STPK_MACHINESTRUCT );
	kMachinePacket << MACHINE_INFO;
	int iStructCnt = (int)vMachineList.size();
	kMachinePacket << iStructCnt;

	for( int i=0; i<iStructCnt; i++ )
	{
		kMachinePacket << vMachineList[i].m_iNum;
		kMachinePacket << vMachineList[i].m_iIndex;
		kMachinePacket << vMachineList[i].m_CreatePos;
		kMachinePacket << vMachineList[i].m_TargetRot;
	}
	SendRoomAllUser( kMachinePacket );
}

void Mode::OnMachineStruct( User *pSend, SP2Packet &rkPacket )
{
	int iSubType;
	rkPacket >> iSubType;

	switch( iSubType )
	{
	case MACHINE_TAKE:
		OnMachineStructTake( pSend, rkPacket );
		break;
	case MACHINE_RELEASE:
		OnMachineStructRelease( pSend, rkPacket );
		break;
	case MACHINE_DIE:
		OnMachineStructDie( pSend, rkPacket );
		break;
	default:
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Mode::OnMachineStruct - WrongType: %d", iSubType );
		break;
	}
}

void Mode::OnMachineStructDie( User *pSend, SP2Packet &rkPacket )
{
	if( !IsEnableState( pSend ) )
		return;

	int iIndex;
	bool bEffect;
	rkPacket >> iIndex >> bEffect;

	int iCnt = m_vMachineStructList.size();
	for( int i=0; i < iCnt; i++ )
	{
		if( m_vMachineStructList[i].m_iIndex != iIndex )
			continue;

		m_vMachineStructList.erase( m_vMachineStructList.begin() + i );

		SP2Packet kReturn( STPK_MACHINESTRUCT );
		kReturn << MACHINE_DIE;
		kReturn << iIndex;
		kReturn << bEffect;
		SendRoomAllUser( kReturn );
		break;
	}
}

void Mode::OnMachineStructTake( User *pSend, SP2Packet &rkPacket )
{
	if( !IsEnableState( pSend ) )
		return;

	int iIndex;
	ioHashString szName = pSend->GetPublicID();
	rkPacket >> iIndex;

	int iCnt = m_vMachineStructList.size();
	for( int i=0; i < iCnt; i++ )
	{
		if( m_vMachineStructList[i].m_iIndex != iIndex )
			continue;

		if( m_vMachineStructList[i].m_bTake )
		{
			// 예외발생 이미 누군가 사용중
			SP2Packet kMachinePacket( STPK_MACHINESTRUCT );
			kMachinePacket << MACHINE_TAKE_FAIL;
			kMachinePacket << szName;
			kMachinePacket << iIndex;
			SendRoomAllUser( kMachinePacket );
			return;
		}

		m_vMachineStructList[i].m_TakeCharName = szName;
		m_vMachineStructList[i].m_bTake = true;

		SP2Packet kMachinePacket( STPK_MACHINESTRUCT );
		kMachinePacket << MACHINE_TAKE_OK;
		kMachinePacket << szName;
		kMachinePacket << iIndex;
		SendRoomAllUser( kMachinePacket );
		return;
	}
}

void Mode::OnMachineStructRelease( User *pSend, SP2Packet &rkPacket )
{
	if( !IsEnableState( pSend ) )
		return;

	int iIndex;
	ioHashString szName = pSend->GetPublicID();
	rkPacket >> iIndex;

	Vector3 vPos;
	rkPacket >> vPos;

	int iCnt = m_vMachineStructList.size();
	for( int i=0; i < iCnt; i++ )
	{
		if( m_vMachineStructList[i].m_iIndex != iIndex )
			continue;

		if( !m_vMachineStructList[i].m_bTake )
		{
			// 예외발생 이미 해제된 상태
			SP2Packet kMachinePacket( STPK_MACHINESTRUCT );
			kMachinePacket << MACHINE_RELEASE_FAIL;
			kMachinePacket << szName;
			kMachinePacket << iIndex;
			SendRoomAllUser( kMachinePacket );
			return;
		}

		m_vMachineStructList[i].Init();

		SP2Packet kMachinePacket( STPK_MACHINESTRUCT );
		kMachinePacket << MACHINE_RELEASE_OK;
		kMachinePacket << szName;
		kMachinePacket << iIndex;
		kMachinePacket << vPos;
		SendRoomAllUser( kMachinePacket );
		return;
	}
}

void Mode::OnPushStructDieByOwnerClear( User *pSend, SP2Packet &rkPacket )
{
	if( !IsEnableState( pSend ) )
		return;

	ioHashString szOwner;
	rkPacket >> szOwner;

	DestroyPushStructByLeave( szOwner );
}

void Mode::OnDropItem( User *pSend, SP2Packet &rkPacket )
{
	if( !IsEnableState( pSend ) )
		return;

	ioHashString kOwnerName;
	rkPacket >> kOwnerName;
	if( pSend->GetPublicID() == kOwnerName )
		pSend->OnDropItem( rkPacket );
	else
	{
		// NPC가 드랍한 아이템이다.
		OnDropItemNpc( pSend, kOwnerName, rkPacket );
	}
}

void Mode::OnDropMoveItem( User *pSend, SP2Packet &rkPacket )
{
	if( !IsEnableState( pSend ) )
		return;

	ioHashString kOwnerName;
	rkPacket >> kOwnerName;
	if( pSend->GetPublicID() == kOwnerName )
		pSend->OnItemMoveDrop( rkPacket );
	else
	{
		// NPC가 드랍한 아이템이다.
		OnDropMoveItemNpc( pSend, kOwnerName, rkPacket );
	}
}

void Mode::OnNpcSpawn( User *pSend, SP2Packet &rkPacket )
{
	if( !IsEnableState( pSend ) )
		return;

	OnNpcSpawn(rkPacket);
}


void Mode::CheckNeedSendPushStruct()
{
	if( m_ModeState != MS_PLAY )	return;
	if( m_vPushStructList.empty() ) return;

	DWORD dwCurTime = TIMEGETTIME();

	enum { SEND_PUSH_STRUCT_CNT = 20, };

	PushStructList vSendList;
	vSendList.reserve( SEND_PUSH_STRUCT_CNT );

	int iRecordCnt = GetRecordCnt();
	for( int i=0 ; i<iRecordCnt ; i++ )
	{
		ModeRecord *pRecord = FindModeRecord( i );
		if( !pRecord ) continue;

		User *pUser = pRecord->pUser;
		if( !pUser ) continue;

		DWORD dwCheckTime = pUser->GetSendPushStructCheckTime();
		bool bClearInfo = false;
		if( dwCheckTime == 0 )		// enter first
			bClearInfo = true;

		if( pUser->IsNeedSendPushStruct() && dwCheckTime < dwCurTime )
		{
			vSendList.clear();
			
			int iCheckMaxIndex = 0;
			if( bClearInfo )
			{
				iCheckMaxIndex = m_iPushStructIdx;
				pUser->SetNeedSendPushStructIndex( iCheckMaxIndex );
			}
			else
			{
				iCheckMaxIndex = pUser->GetNeedSendPushStructIndex();
			}

			int iStructIndex = pUser->GetSendPushStructIndex();
			int iNewStructIndex = iStructIndex;

			int iCurSendCnt = 0;
			int iPushStructCnt = m_vPushStructList.size();
			for( int j=0; j<iPushStructCnt; j++ )
			{
				const PushStruct &rkPush = m_vPushStructList[j];
				// 들어왔을때 마지막 구조물 인덱스와 비교
				if( rkPush.m_iIndex > iCheckMaxIndex )
					continue;

				// 앞서 보낸것 이후 것인지 비교
				if( rkPush.m_iIndex > iStructIndex )
				{
					++iCurSendCnt;
					iNewStructIndex = rkPush.m_iIndex;
					vSendList.push_back( rkPush );
				}

				if( SEND_PUSH_STRUCT_CNT <= iCurSendCnt )
					break;
			}

			if( vSendList.empty() )
			{
				pUser->SetNeedSendPushStruct( false );
				pUser->SetNeedSendPushStructIndex( 0 );
				pUser->SetSendPushStructIndex( 0 );
				pUser->SetSendPushStructCheckTime( 0 );
				return;
			}
			else
			{
				pUser->SetSendPushStructIndex( iNewStructIndex );
				pUser->SetSendPushStructCheckTime( (dwCurTime + 1000) );
			}

			SP2Packet kPushStructInfo( STPK_PUSHSTRUCT_INFO_ENTER );

			kPushStructInfo << bClearInfo;

			int iSendCnt = (int)vSendList.size();
			kPushStructInfo << iSendCnt;

			for( int k=0; k < iSendCnt; ++k )
			{
				const PushStruct &rkPush = vSendList[k];

				DWORD dwSeed = timeGetTime();
				DWORD dwGapTime = 0;
				if( rkPush.m_dwCreateTime > 0 )
					dwGapTime = dwCurTime - rkPush.m_dwCreateTime;

				kPushStructInfo << rkPush.m_iNum;
				kPushStructInfo << rkPush.m_iIndex;
				kPushStructInfo << rkPush.m_CreatePos;
				kPushStructInfo << rkPush.m_TargetRot;
				kPushStructInfo << rkPush.m_OwnerName;
				kPushStructInfo << dwGapTime;
				kPushStructInfo << dwSeed;
				kPushStructInfo << rkPush.m_dwCreateEtcCode;
			}

			pUser->SendMessage( kPushStructInfo );
		}
	}
}

void Mode::AddModeContributeByShuffle()
{
	if( !m_pCreator )
		return;

	if( m_pCreator->GetRoomStyle() != RSTYLE_SHUFFLEROOM )
		return;

	DWORD dwPlayTime = TIMEGETTIME() - GetModeStartTime();

	int iRecordCnt = GetRecordCnt();
	for( int i = 0; i<iRecordCnt; ++i )
	{
		ModeRecord *pRecord = FindModeRecord( i );
		if( !pRecord )
			continue;

		User *pUser = pRecord->pUser;
		if( pUser )
		{
			ShuffleRoomNode *pShuffleRoom = dynamic_cast<ShuffleRoomNode*>( pUser->GetMyShuffleRoom() );
			if( pShuffleRoom )
				pShuffleRoom->AddModeContributeByShuffle( pUser->GetPublicID().c_str(), pUser->GetUserIndex(), pRecord->fContributePer, dwPlayTime );
		}
	}
}

ModeItem* Mode::CreateModeItem( int eType )
{
	ModeItem* pItem = ModeItemCreator::CreateModeItemTemplate( (ModeItemType)eType );
	if( pItem )
	{		
		m_dwCurModeItemIndex++;
		pItem->m_dwModeItemIdx = m_dwCurModeItemIndex;

		m_vModeItem.push_back( pItem );
	}

	return pItem;
}

ModeItem* Mode::FindModeItem( DWORD dwModeItemIndex )
{
	if( m_vModeItem.empty() )
		return NULL;

	for( ModeItemVec::iterator iter = m_vModeItem.begin(); iter != m_vModeItem.end(); ++iter )
	{
		ModeItem* pItem = *iter;
		if( pItem && pItem->m_dwModeItemIdx == dwModeItemIndex )
			return pItem;
	}
	
	return NULL;
}

void Mode::DeleteModeItem( DWORD dwModeItemIndex )
{
	for( ModeItemVec::iterator iter = m_vModeItem.begin(); iter != m_vModeItem.end(); ++iter )
	{
		ModeItem* pItem = *iter;
		if( pItem && pItem->m_dwModeItemIdx == dwModeItemIndex )			
		{
			m_vModeItem.erase( iter );
			return;
		}
	}
}

void Mode::DestoryModeItem()
{
	if( m_vModeItem.empty() )
		return;

	for( ModeItemVec::iterator iter = m_vModeItem.begin(); iter != m_vModeItem.end(); ++iter )
	{
		ModeItem* pItem = *iter;
		if( pItem )
			SAFEDELETE( pItem );
	}

	m_vModeItem.clear();
}

void Mode::OnCreateModeItem( SP2Packet &rkPacket )
{

}

void Mode::OnGetModeItem( SP2Packet &rkPacket )
{
	if( m_ModeState != MS_PLAY )
		return;

	DWORD dwUserIndex, dwModeItemIndex = 0;	
	PACKET_GUARD_VOID( rkPacket.Read( dwUserIndex ) );
	PACKET_GUARD_VOID( rkPacket.Read( dwModeItemIndex ) );

	const ModeItem* pItem = FindModeItem( dwModeItemIndex );
	if( pItem )
	{
		DeleteModeItem( dwModeItemIndex );
		SP2Packet kPacket( STPK_GET_MODE_ITEM );
		kPacket << dwUserIndex;
		kPacket << dwModeItemIndex;
		SendRoomAllUser( kPacket );
	}	
}

ModeCategory Mode::GetPlayModeCategory()
{
	if( !m_pCreator )
		return MC_BATTLE;

	int iRoomStyle = m_pCreator->GetRoomStyle(); //전투방, 셔플 구별

	//오늘의 모드
	if( iRoomStyle == RSTYLE_SHUFFLEROOM )
		return MC_SHUFFLE;

	//진영전, 레더
	if( iRoomStyle == RSTYLE_LADDERBATTLE )
		return MC_LADDER;

	//전투방일 경우 전투방 모드에 따라 구별
	int iBattleMode = GetModeType();

	//몬스터 던전 + 해골
	if( iBattleMode == MT_FIRE_TEMPLE || iBattleMode == MT_TOWER_DEFENSE || iBattleMode == MT_DARK_XMAS
		|| iBattleMode == MT_FACTORY || iBattleMode == MT_MONSTER_SURVIVAL || iBattleMode == MT_RAID )
		return MC_MONSTER;

	//전투방
	return MC_BATTLE;
}

#ifdef ANTIHACK
void Mode::SendRelayGroupTeamWinCnt()
{
	// 1라운드 종료시마다 udp 모듈쪽에 필요한 정보를 넘겨줌
	if( m_pCreator )
	{
		g_Relay.UpdateRelayGroupWin( m_pCreator->GetRoomIndex(), m_iRedTeamWinCnt, m_iBlueTeamWinCnt );
	}
	else
	{
		//CheatLOG.PrintTimeAndLog( 0, "[relay] TEST - UpdateRelayGroupWin Error" );
	}
}

void Mode::SendRelayGroupTeamScore( User* pUser )
{
	if( !pUser )
		return;

	if( m_pCreator )
	{
		g_Relay.UpdateRelayGroupScore( m_pCreator->GetRoomIndex(), (int)pUser->GetTeam() );
	}
	else
	{
		//CheatLOG.PrintTimeAndLog( 0, "[relay] TEST - SendRelayGroupTeamScore Error %u", pUser->GetUserIndex() );
	}
}
#endif

DWORD Mode::GetUniqueMonsterIDGenerate()
{
	//monster ID generate 유저랑 겹쳐도 상관 없게끔
	IORandom MonsterRandom;
	DWORD dwMonsterID = MonsterRandom.Random( 1000000 );
	while( 1 )
	{
		bool bFind = false;
		int nSize = m_vecMonsterID.size();
		for( int i = 0; i < nSize; ++i )
		{
			if( m_vecMonsterID[i] == dwMonsterID )
			{
				bFind = true;
				break;
			}
		}

 		if( bFind == false )
		{
			m_vecMonsterID.push_back( dwMonsterID );
			break;
		}	
		
		dwMonsterID = MonsterRandom.Random( 1000000 );
	}
	

	return dwMonsterID;
}

bool Mode::CheckTCPPacket( SP2Packet &rkPacket )
{
	FUNCTION_TIME_CHECKER( 100000.0f, rkPacket.GetPacketID() );

	switch( rkPacket.GetPacketID() )
	{
	case CTPK_DROP_ITEM:
		if( GetModeState() != MS_PLAY )
			return true;
		break;
	case CTPK_CURRENT_DAMAGELIST:
		if( GetModeState() != MS_RESULT_WAIT && GetModeState() != MS_PLAY )
			return true;
		break;
	case CTPK_LAST_PLAYRECORD_INFO:
		//case CTPK_AWARDING_RESULT:
		if( GetModeState() != MS_RESULT_WAIT )
			return true;
		break;
	}

	return false;
}
