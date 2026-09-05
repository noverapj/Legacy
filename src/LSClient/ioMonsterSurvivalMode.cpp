#include "stdafx.h"

#include "ioMonsterSurvivalMode.h"

#include "ioPlayStage.h"
#include "ioNpcChar.h"
#include "ioMapEventSceneMgr.h"
#include "ioModeGuideArrow.h"

#include "GUI/MiniMap.h"
#include "GUI/BossMonsterGaugeWnd.h"
#include "GUI/BriefingWnd.h"
#include "GUI/MonsterCoinWnd.h"
#include "GUI/CenterHelpWnd.h"
#include "GUI/RoundStateMainWnd.h"
#include "GUI/TimeScoreWnd.h"
#include "GUI/MiniSoldierSelectWnd.h"
#include "GUI/ioObserverWnd.h"
#include "GUI/LadderTeamWnd.h"
#include "GUI/TotalResultMainWnd.h"

bool ioMonsterSurvivalMode::m_bTurnDeathTime = false;
ioMonsterSurvivalMode::ioMonsterSurvivalMode( ioPlayStage *pCreator )
: ioPlayMode( pCreator )
{
	m_ModeType = MT_MONSTER_SURVIVAL;
	m_dwAllCurTurn = m_dwAllMaxTurn = 0;
	m_dwCurrentHighTurn = m_dwCurrentLowTurn = m_dwCurrentTurnHelpID = 0;
	m_dwReduceNpcCreateTime = 0;
	m_fRevivalRecoveryHP = FLOAT05;
	m_dwStartDungeonFloor = 0;
	m_dwStartCoinTime= m_dwCurStartCoinTime= m_dwCurStartCoinSec = 0;
	m_iAveragePlayer = 0;
	m_bUsingMonsterCoin = false;
	m_iUseMonsterCoinCnt = 0;
	m_bBossTurn = false;
	m_fBalanceMaxRate = 0.0f;
	m_CurrentMagicZone.Init();
	m_dwCurRevivalCoinTime = 0;
	m_dwStartTurnTime = 0;

	m_TreasureCardRandomIndex = 0;

	m_vCharRecordList.reserve( 100 );
}

ioMonsterSurvivalMode::~ioMonsterSurvivalMode()
{
	m_MagicZoneList.clear();
	m_bZeroHP = false;
	m_bTurnDeathTime = false;
	ClearMode();
}

void ioMonsterSurvivalMode::ClearMode()
{
	ioPlayMode::ClearMode();
}

void ioMonsterSurvivalMode::InitMode( int iMapIndex )
{
	m_iModeMapIndex = iMapIndex;

	LoadMap();
	LoadPushStructs();
	LoadMachineStruct();
	LoadObjectItems();
	LoadFuntionalObjects();
	LoadCharacters();
	LoadTurnHelp();
	LoadHighTurn();
	LoadMagicZone();
	LoadDisableSkillList();
	LoadStartBuffList();
	LoadModeGrowthRate();

	ioINILoader kLoader( GetModeINIFileName( m_ModeType ) );

	char szBuf[MAX_PATH];
	kLoader.SetTitle_e( "common_info" );
	m_dwRoundDuration = kLoader.LoadInt_e( "round_time", 300000 );
	m_dwCurRoundDuration = m_dwRoundDuration;

	m_fScoreWarningRate = kLoader.LoadFloat_e( "score_warning_rate", FLOAT05 );
	m_fRevivalRecoveryHP = kLoader.LoadFloat_e( "revival_recovery_hp_rate", FLOAT05 );

	m_dwStartCoinTime = kLoader.LoadInt_e( "start_coin_use_time", 0 );
	kLoader.LoadString_e( "coin_revival_buff", "", szBuf, MAX_PATH );
	m_szCoinRevivalBuff = szBuf;

	m_dwKeepLiveTime = kLoader.LoadInt_e( "npc_death_time", 35000 );

	LoadRevivalTime( kLoader );
	LoadModeResource( kLoader );
	LoadModeCamera( kLoader );

	kLoader.SetTitle_e( "ko_recover" );
	m_fKORecoverRate = kLoader.LoadFloat_e( "ko_recover_rate", 0.0f );
	kLoader.LoadString_e( "ko_recover_effect", "", szBuf, MAX_PATH );
	m_KORecoverEffect = szBuf;
	kLoader.LoadString_e( "ko_perfect_effect", "", szBuf, MAX_PATH );
	m_PerfectRecoverEffect = szBuf;
	kLoader.LoadString_e( "ko_buff", "", szBuf, MAX_PATH );
	m_KOBuff = szBuf;

	kLoader.SetTitle_e( "end_scene_camera" );
	m_fEndHeight = kLoader.LoadFloat_e( "end_camera_height", 0.0f );
	m_fEndDistance = kLoader.LoadFloat_e( "end_camera_distance", 0.0f );
	m_fEndFov = kLoader.LoadFloat_e( "end_camera_fov", 0.0f );
	kLoader.LoadString_e( "round_end_focus_effect", "", szBuf, MAX_PATH );
	m_RoundEndEffect = szBuf;

	LoadGUIValues();
	LoadPushCursor();
	LoadContribute();

	kLoader.SetTitle_e( "ui_info" );
	kLoader.LoadString_e( "1point_up_emoticon", "", szBuf, MAX_PATH );
	m_PointUpEmoticon = szBuf;

	kLoader.LoadString_e( "kill_point_emoticon_red", "", szBuf, MAX_PATH );
	m_KillPointEmoticonRed = szBuf;
	kLoader.LoadString_e( "kill_point_emoticon_blue", "", szBuf, MAX_PATH );
	m_KillPointEmoticonBlue = szBuf;
	kLoader.LoadString_e( "kill_point_num", "", szBuf, MAX_PATH );
	m_KillPointNum = szBuf;
	kLoader.LoadString_e( "kill_point_per", "", szBuf, MAX_PATH );
	m_KillPointPer = szBuf;
	kLoader.LoadString_e( "kill_point_sound", "", szBuf, MAX_PATH );
	m_KillPointSound = szBuf;

	if( !P2PNetwork::IsNetworkPlaying() )
	{
		SetModeState( MS_READY );
		LoadObjectItems();
	}

	g_MyInfo.ClearCharDiedInfo();
	g_MyInfo.ClearCharDiedPenaltyInfo();
	g_MyInfo.ClearCharJoinedInfo();

	g_GUIMgr.CheckShopSound( true );

	g_MyInfo.InitPraise();

	ClearGrowthLevel();

	// LoadMap() 이후 사용해야함
	SetMapAreaWeapon();

	SetMapDummy();

	if( g_BattleRoomMgr.IsBattleRoom() )
		g_BattleRoomMgr.ModeStartPlayerCount();
}

void ioMonsterSurvivalMode::LoadMap()
{
	ioPlayMode::LoadMap();

	int i = 0;
	ioINILoader kLoader( GetModeMapINIFileName( m_ModeType, m_iModeSubNum, m_iModeMapIndex ) );
	kLoader.SetTitle_e( "team_revival" );
	if( P2PNetwork::IsNetworkPlaying() )
	{
		int iMaxRevivalPos = kLoader.LoadFloat_e( "max_revival_pos", 0 );
		m_vRevivalPos.clear();
		m_vRevivalPos.reserve( iMaxRevivalPos );
		for( i=0 ; i<iMaxRevivalPos; i++ )
		{
			char szRevivalX[MAX_PATH], szRevivalZ[MAX_PATH];
			wsprintf_e(szRevivalX, "revival%d_x", i+1);
			wsprintf_e(szRevivalZ, "revival%d_z", i+1);

			D3DXVECTOR3 vRevivalPos;
			vRevivalPos.x  = kLoader.LoadFloat( szRevivalX, 0.0f );
			vRevivalPos.y  = 0.0f;
			vRevivalPos.z  = kLoader.LoadFloat( szRevivalZ, 0.0f );
			m_vRevivalPos.push_back(vRevivalPos);
		}

		//
		m_StartPosRandom.SetRandomSeed( g_BattleRoomMgr.GetIndex() );
		m_RevivalPosRandom.SetRandomSeed( g_BattleRoomMgr.GetIndex() );
	}
	else
	{
		m_vRevivalPos.clear();
		m_vRevivalPos.reserve( 1 );

		D3DXVECTOR3 vRevivalPos;
		vRevivalPos.x  = kLoader.LoadFloat_e( "single_x", 0.0f );
		vRevivalPos.z  = kLoader.LoadFloat_e( "single_z", 0.0f );
		m_vRevivalPos.push_back(vRevivalPos);
	}

	kLoader.SetTitle_e( "Common" );
	m_iAveragePlayer = kLoader.LoadInt_e( "average_player", 2 );
	m_fBalanceMaxRate= kLoader.LoadFloat_e( "balance_max_value", 4.0f );
	m_iUseMonsterCoinCnt = kLoader.LoadInt_e( "use_monster_coin_cnt", 0 );

	kLoader.SetTitle_e( "TreasureCard" );
	m_TreasureCardRandomIndex = kLoader.LoadInt_e( "TreasureRandomIndex", 0 );
}

void ioMonsterSurvivalMode::LoadModeResource( ioINILoader &rkLoader )
{
	MonsterSurvivalHelWnd *pHelpWnd = dynamic_cast<MonsterSurvivalHelWnd *>( g_GUIMgr.AddWnd( "XML/monstersurvivalhelpwnd.xml" ) );
	if( pHelpWnd )
		pHelpWnd->SetPlayMode( this );

	TimeScoreWnd *pTimeScoreWnd = dynamic_cast<TimeScoreWnd *>( g_GUIMgr.AddWnd( "XML/TimeScoreWnd.xml") );
	if(pTimeScoreWnd)
		pTimeScoreWnd->SetModeType( GetModeType() );

	g_ModeUIController.SetKOMsgWnd( g_GUIMgr.AddWnd( "XML/kochatmsgwnd.xml") );
	g_ModeUIController.SetCenterInfoWnd( g_GUIMgr.AddWnd( "XML/CenterInfoWnd.xml" ) );
	g_ModeUIController.SetCenterKillInfoWnd( NULL );

	g_GUIMgr.AddWnd( "XML/BossMonsterGaugeWnd.xml");

	MiniSoldierSelectWnd *pMiniSoldierWnd = dynamic_cast<MiniSoldierSelectWnd*>(g_GUIMgr.AddWnd( "XML/MiniSoldierSelectWnd.xml" ));
	if(pMiniSoldierWnd)
		pMiniSoldierWnd->SetPlayStage( m_pCreator );

	// CenterInfoWnd.xml 보다 나중에 추가 해야함
	g_GUIMgr.AddWnd( "XML/SkillListWnd.xml" );
	g_GUIMgr.AddWnd( "XML/AccessorySkillListWnd.xml" );

	// TimeScoreWnd 이후에 추가해야한다
	ioObserverBlueWnd *pBlueWnd = dynamic_cast<ioObserverBlueWnd *>( g_GUIMgr.AddWnd("XML/observerblueinfownd.xml") );
	if( pBlueWnd )
		pBlueWnd->InitObserverWnd( m_pCreator );

	ioObserverRedWnd *pRedWnd = dynamic_cast<ioObserverRedWnd *>( g_GUIMgr.AddWnd("XML/observerredinfownd.xml") );
	if( pRedWnd )
		pRedWnd->InitObserverWnd( m_pCreator );

	g_GUIMgr.AddWnd("XML/ObserverInfoWnd.xml");
	ioPlayMode::LoadModeResource( rkLoader );
	m_vRoundHistory.clear();
}

void ioMonsterSurvivalMode::LoadTurnHelp()
{
	const char *pMapINIFile = GetModeMapINIFileName( m_ModeType, m_iModeSubNum, m_iModeMapIndex );
	ioINILoader kLoader( pMapINIFile );

	m_TurnHelpList.clear();

	int i = 0;
	char szKey[MAX_PATH], szBuf[MAX_PATH];
	kLoader.SetTitle_e( "TurnHelp" );
	m_dwStartDungeonFloor = kLoader.LoadInt_e( "StartFloor", 0 );
	int iMaxTurn = kLoader.LoadInt_e( "MaxTurn", 0 );
	for(i = 0;i < iMaxTurn;i++)
	{
		TurnHelpList kHelpList;		
		sprintf_e( szKey, "turn%d_zone_index", i + 1 );
		kHelpList.m_dwZoneIndex = kLoader.LoadInt( szKey, 0 ); 
		sprintf_e( szKey, "turn%d_event_scene_index", i + 1 );
		kHelpList.m_dwEventSceneIndex = kLoader.LoadInt( szKey, 0 ); 
		sprintf_e( szKey, "turn%d_event_scene_effect", i + 1 );
		kHelpList.m_dwEventSceneEffect = kLoader.LoadInt( szKey, 0 ); 
		sprintf_e( szKey, "turn%d_monster_scene_index", i + 1 );
		kHelpList.m_dwMonsterSceneIndex = kLoader.LoadInt( szKey, 0 );
		m_TurnHelpList.push_back( kHelpList );
	}

	// 데스타임 헬프
	m_TurnDeathHelp.m_HelpData.clear();
	kLoader.SetTitle_e( "TurnDeathHelp" );
	int ilineCount = kLoader.LoadInt_e( "turn_line_count", 0 );
	m_TurnDeathHelp.m_dwShowTime = kLoader.LoadInt_e( "turn_show_time", 0 ); 
	kLoader.LoadString_e( "turn_play_sound", "", szBuf, MAX_PATH );
	m_TurnDeathHelp.m_szPlaySound = szBuf;
	for(i = 0;i < ilineCount;i++)
	{
		TurnHelp kData;
		//
		sprintf_e( szKey, "turn_line%d_scale", i + 1 );
		kData.m_fFontScale = kLoader.LoadFloat( szKey, 0.83f );
		//
		sprintf_e( szKey, "turn_line%d_color", i + 1 );
		kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		char *pTemp = NULL;
		kData.m_dwColor = strtoul( szBuf, &pTemp, 16 );

		//
		sprintf_e( szKey, "turn_line%d_text", i + 1 );
		kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kData.m_szText = szBuf;

		m_TurnDeathHelp.m_HelpData.push_back( kData );
	}
}

void ioMonsterSurvivalMode::LoadHighTurn()
{
	const char *pMapINIFile = GetModeMapINIFileName( m_ModeType, m_iModeSubNum, m_iModeMapIndex );
	ioINILoader kLoader( pMapINIFile );

	m_HighTurnList.clear();
	kLoader.SetTitle_e( "HighTurn" );
	int iMaxHighTurn = kLoader.LoadInt_e( "MaxHighTurn", 0 );
	for(int i = 0;i < iMaxHighTurn;i++)
	{
		char szKey[MAX_PATH], szBuf[MAX_PATH];
		sprintf_e( szKey, "HighTurn%d", i + 1 );
		kLoader.SetTitle( szKey );
		
		HighTurnData kHighTurn;
		kHighTurn.m_iStuckActiveRand = kLoader.LoadInt_e( "StuckActiveRand", 0 );
		kHighTurn.m_iStuckActiveRandAdd = kLoader.LoadInt_e( "StuckActiveRandAdd", 0 );
		kHighTurn.m_iStuckActiveRandMax = kLoader.LoadInt_e( "StuckActiveRandMax", 0 );
		kHighTurn.m_iStuckActiveSeed = kLoader.LoadInt_e( "StuckActiveSeed", 0 );
		kHighTurn.m_dwStuckActiveTime= kLoader.LoadInt_e( "StuckActiveTime", 0 );

		int j = 0;
		int iMaxStuckBuff = kLoader.LoadInt_e( "MaxStuckBuff", 0 );
		for(j = 0;j < iMaxStuckBuff;j++)
		{
			sprintf_e( szKey, "StuckBuff%d", j + 1 );
			kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
			kHighTurn.m_StuckBuffList.push_back( szBuf );
		}

		// 추가 스턱 정보
		int iMaxStuckExCnt = kLoader.LoadInt_e( "MaxStuckExCnt", 0 );
		for(j = 0;j < iMaxStuckExCnt;j++)
		{
			StuckExData kStuckExData;
			sprintf_e( szKey, "StuckExWeaponType%d", j + 1 );
			kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
			kStuckExData.m_szWeaponType = szBuf;

			sprintf_e( szKey, "StuckExActiveRand%d", j + 1 );
			kStuckExData.m_iStuckActiveRand = kLoader.LoadInt( szKey, 0 );

			sprintf_e( szKey, "StuckExActiveRandAdd%d", j + 1 );
			kStuckExData.m_iStuckActiveRandAdd = kLoader.LoadInt( szKey, 0 );

			sprintf_e( szKey, "StuckExActiveRandMax%d", j + 1 );
			kStuckExData.m_iStuckActiveRandMax = kLoader.LoadInt( szKey, 0 );

			kHighTurn.m_StuckExDataList.push_back( kStuckExData );
		}

		// 일반 턴 추가 ArmorClass / SpeedClass.
		kHighTurn.m_NormalBalance.m_fAddDefaultArmorClass = kLoader.LoadFloat_e( "AddDefaultArmorClass", 0.0f );
		kHighTurn.m_NormalBalance.m_fAddDefaultSpeedClass = kLoader.LoadFloat_e( "AddDefaultSpeedClass", 0.0f );
		kHighTurn.m_NormalBalance.m_fAddArmorClass = kLoader.LoadFloat_e( "AddArmorClass", 0.0f );
		kHighTurn.m_NormalBalance.m_fMaxArmorClass = kLoader.LoadFloat_e( "MaxArmorClass", 0.0f );
		kHighTurn.m_NormalBalance.m_fAddSpeedClass = kLoader.LoadFloat_e( "AddSpeedClass", 0.0f );
		kHighTurn.m_NormalBalance.m_fMaxSpeedClass = kLoader.LoadFloat_e( "MaxSpeedClass", 0.0f );

		// 보스 턴 추가 ArmorClass / SpeedClass.
		kHighTurn.m_BossBalance.m_fAddDefaultArmorClass = kLoader.LoadFloat_e( "BossAddDefaultArmorClass", kHighTurn.m_NormalBalance.m_fAddDefaultArmorClass );
		kHighTurn.m_BossBalance.m_fAddDefaultSpeedClass = kLoader.LoadFloat_e( "BossAddDefaultSpeedClass", kHighTurn.m_NormalBalance.m_fAddDefaultSpeedClass );
		kHighTurn.m_BossBalance.m_fAddArmorClass = kLoader.LoadFloat_e( "BossAddArmorClass", kHighTurn.m_NormalBalance.m_fAddArmorClass );
		kHighTurn.m_BossBalance.m_fMaxArmorClass = kLoader.LoadFloat_e( "BossMaxArmorClass", kHighTurn.m_NormalBalance.m_fMaxArmorClass );
		kHighTurn.m_BossBalance.m_fAddSpeedClass = kLoader.LoadFloat_e( "BossAddSpeedClass", kHighTurn.m_NormalBalance.m_fAddSpeedClass );
		kHighTurn.m_BossBalance.m_fMaxSpeedClass = kLoader.LoadFloat_e( "BossMaxSpeedClass", kHighTurn.m_NormalBalance.m_fMaxSpeedClass );

		// 턴 변경 사운드
		kLoader.LoadString_e( "HighTurnStartSnd", "", szBuf, MAX_PATH );
		kHighTurn.m_szHighTurnStartSnd = szBuf;

		kLoader.LoadString_e( "LowTurnStartSnd", "", szBuf, MAX_PATH );
		kHighTurn.m_szLowTurnStartSnd  = szBuf;

		// 턴정보
		int iMaxTurn = kLoader.LoadInt_e( "max_turn", 0 );
		for(j = 0;j < iMaxTurn;j++)
		{
			TurnData kTurnData;

			sprintf_e( szKey, "turn%d_end_object", j + 1 );
			kTurnData.m_iTurnEndHideObject = kLoader.LoadInt( szKey, -1 );

			sprintf_e( szKey, "turn%d_end_effect", j + 1 );
			kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
			kTurnData.m_szTurnEndEffect = szBuf;

			sprintf_e( szKey, "turn%d_end_loop_effect", j + 1 );
			kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
			kTurnData.m_szTurnEndLoopEffect = szBuf;

			sprintf_e( szKey, "turn%d_end_loop_effect_x", j + 1 );
			kTurnData.m_fTurnEndLoopEffectX = kLoader.LoadFloat( szKey, 0.0f );
			
			sprintf_e( szKey, "turn%d_end_loop_effect_y", j + 1 );
			kTurnData.m_fTurnEndLoopEffectY = kLoader.LoadFloat( szKey, 0.0f );

			sprintf_e( szKey, "turn%d_end_loop_effect_z", j + 1 );
			kTurnData.m_fTurnEndLoopEffectZ = kLoader.LoadFloat( szKey, 0.0f );

			kHighTurn.m_TurnData.push_back( kTurnData );
		}
		m_HighTurnList.push_back( kHighTurn );
	}
}

void ioMonsterSurvivalMode::LoadMagicZone()
{
	const char *pMapINIFile = GetModeMapINIFileName( m_ModeType, m_iModeSubNum, m_iModeMapIndex );
	ioINILoader kLoader( pMapINIFile );

	// 유저 탈출
	m_MagicZoneList.clear();
	kLoader.SetTitle_e( "MagicZone" );
	int i = 0;
	int iMaxZone = kLoader.LoadInt_e( "MaxZone", 0 );
	for(i = 0;i < iMaxZone;i++)
	{
		MagicZone kZone;
		char szKey[MAX_PATH], szBuf[MAX_PATH];
		
		sprintf_e( szKey, "zone%d_pos_x", i + 1 );
		kZone.m_vZonePos.x = kLoader.LoadFloat( szKey, 0.0f );
		sprintf_e( szKey, "zone%d_pos_y", i + 1 );
		kZone.m_vZonePos.y = kLoader.LoadFloat( szKey, 0.0f );
		sprintf_e( szKey, "zone%d_pos_z", i + 1 );
		kZone.m_vZonePos.z = kLoader.LoadFloat( szKey, 0.0f );
		
		sprintf_e( szKey, "zone%d_collision_range", i + 1 );
		kZone.m_fCollisionRange = kLoader.LoadFloat( szKey, 0.0f );
		sprintf_e( szKey, "zone%d_collision_time", i + 1 );
		kZone.m_dwCollisionTime = kLoader.LoadInt( szKey, 0 );

		sprintf_e( szKey, "zone%d_collision_range_ex", i + 1 );
		kZone.m_fCollisionRangeEx = kLoader.LoadFloat( szKey, kZone.m_fCollisionRange );
		sprintf_e( szKey, "zone%d_collision_time_ex", i + 1 );
		kZone.m_dwCollisionTimeEx = kLoader.LoadInt( szKey, 0 );
		
		sprintf_e( szKey, "zone%d_collision_buff", i + 1 );
		kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kZone.m_szCollisionBuff = szBuf;		
		
		sprintf_e( szKey, "zone%d_start_sound", i + 1 );
		kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kZone.m_szStartSound = szBuf;

		m_MagicZoneList.push_back( kZone );
	}
}

void ioMonsterSurvivalMode::ProcessMode()
{
	ProcessSelectCharacter();
	ProcessGUI();

	switch( m_ModeState )
	{
	case MS_READY:
		ProcessReadyState();
		CheckModeMapSound();
		break;
	case MS_PLAY:
		ProcessMapAreaWeapon();
		ProcessPlayState();
		CheckModeMapSound();
		ProcessNpcErase();
		g_EventMgr.Process();
		break;
	case MS_RESULT:
		ProcessResultState();
		break;
	}
}

void ioMonsterSurvivalMode::ProcessGUI()
{
	UpdateMiniMapSpotPoint();
	UpdateTimeScoreWnd();
	UpdateObserverWnd();
	UpdateRoundStateWnd();
	UpdateEndScoreWndTime();
	UpdateSoldierWndTime();

	ProcessModeGuide();
	ProcessPushEnableCursor();
	ProcessBlindMode();
	ProcessDeathTimeHelp();
	ProcessAutoStartAlarm();
}

void ioMonsterSurvivalMode::UpdateMiniMapSpotPoint()
{
	MiniMap *pMiniMap = dynamic_cast<MiniMap*>(g_GUIMgr.FindWnd( MINIMAP_WND ));
	if( !pMiniMap )	return;

	ioPlayMode::UpdateMiniMapSpotPoint();

	pMiniMap->ClearPointList();

	D3DXVECTOR3 vPos;
	DWORD dwGapTime = 0;

	// Other Char...
	RecordList::iterator iter;
	for( iter=m_vCharRecordList.begin() ; iter!=m_vCharRecordList.end() ; ++iter )
	{
		ioBaseChar *pChar = iter->pChar;
		if( pChar->IsOwnerChar() || !pChar->IsCanMiniMapVisible() )
			continue;

		dwGapTime = pChar->GetEventGapTime();
		vPos = pChar->GetWorldPosition();

		ioNpcChar *pNpcChar = ToNpcChar( pChar );
		if( pNpcChar )
		{
			if( pNpcChar->IsMonsterHalfBoss() || pNpcChar->IsMonsterLastBoss() ) continue;
		
			if( pNpcChar->IsDropZoneDownState() )
				pMiniMap->AddPoint( vPos.x, vPos.z, MiniMap::SPOT_DROP_USER, pNpcChar->GetTeam(), 0 );
			else if( pNpcChar->IsDeathMonster() )
				pMiniMap->AddPoint( vPos.x, vPos.z, MiniMap::SPOT_DEATH_MON, pNpcChar->GetTeam(), dwGapTime );			
			else
				pMiniMap->AddPoint( vPos.x, vPos.z, MiniMap::SPOT_USER, pNpcChar->GetTeam(), dwGapTime );
		}
		else
		{
			if( pChar->IsPrisonerMode() )
			{
				pMiniMap->AddPoint( vPos.x, vPos.z, MiniMap::SPOT_PRISON_USER, pChar->GetTeam(), dwGapTime );
			}
			else
			{
				if( pChar->IsDropZoneDownState() )
					pMiniMap->AddPoint( vPos.x, vPos.z, MiniMap::SPOT_DROP_USER, pChar->GetTeam(), 0 );
				else
					pMiniMap->AddPoint( vPos.x, vPos.z, MiniMap::SPOT_USER, pChar->GetTeam(), dwGapTime );
			}
		}
	}

	// Boss Mon
	for( iter=m_vCharRecordList.begin() ; iter!=m_vCharRecordList.end() ; ++iter )
	{
		ioBaseChar *pChar = iter->pChar;
		if( pChar->IsOwnerChar() || !pChar->IsCanMiniMapVisible() )
			continue;

		dwGapTime = pChar->GetEventGapTime();
		vPos = pChar->GetWorldPosition();

		ioNpcChar *pNpcChar = ToNpcChar( pChar );
		if( pNpcChar )
		{
			if( pNpcChar->IsMonsterHalfBoss() || pNpcChar->IsMonsterLastBoss() ) 
			{
				if( pNpcChar->IsDropZoneDownState() )
					pMiniMap->AddPoint( vPos.x, vPos.z, MiniMap::SPOT_DROP_BOSS_MON, pNpcChar->GetTeam(), dwGapTime );
				else
					pMiniMap->AddPoint( vPos.x, vPos.z, MiniMap::SPOT_BOSS_MON, pNpcChar->GetTeam(), dwGapTime );
			}
		}
	}

	//MyChar..
	ioBaseChar *pOwner = GetOwnerChar();
	if( pOwner && pOwner->IsCanMiniMapVisible() )
	{
		dwGapTime = pOwner->GetEventGapTime();
		vPos = pOwner->GetWorldPosition();

		if( pOwner->IsPrisonerMode() )
		{
			pMiniMap->AddPoint( vPos.x, vPos.z, MiniMap::SPOT_PRISON_ME, pOwner->GetTeam(), dwGapTime );
		}
		else
		{
			if( pOwner->IsDropZoneDownState() )
				pMiniMap->AddPoint( vPos.x, vPos.z, MiniMap::SPOT_DROP_ME, pOwner->GetTeam(), 0 );
			else
				pMiniMap->AddPoint( vPos.x, vPos.z, MiniMap::SPOT_ME, pOwner->GetTeam(), dwGapTime );
		}
	}

	// Ball & Dummy
	const GameEntityList &rkObjectList = m_pCreator->GetGameEntityList();
	GameEntityList::const_iterator iter_b;
	for( iter_b = rkObjectList.begin() ; iter_b!=rkObjectList.end() ; ++iter_b )
	{
		ioBall *pBall = ToBallStruct( *iter_b );
		if( pBall )
		{
			dwGapTime = pBall->GetEventGapTime();
			vPos = pBall->GetWorldPosition();
			pMiniMap->AddPoint( vPos.x, vPos.z, MiniMap::SPOT_BALL, TEAM_NONE, dwGapTime );
		}

		ioDummyChar *pDummy = ToDummyChar( *iter_b );
		if( pDummy && pDummy->IsVisibleMiniMap() )
		{
			vPos = pDummy->GetWorldPosition();
			pMiniMap->AddPoint( vPos.x, vPos.z, MiniMap::SPOT_USER, pDummy->GetTeam(), 0 );
		}
	}
	//
	
}

void ioMonsterSurvivalMode::UpdateTimeScoreWnd()
{
	m_bTurnDeathTime = false;

	TimeScoreWnd *pTimeScoreWnd = dynamic_cast<TimeScoreWnd *>( g_GUIMgr.FindWnd( TIME_SCORE_WND ) );
	if(!pTimeScoreWnd) return;

	BossMonsterGaugeWnd *pBossMonsterGaugeWnd = dynamic_cast< BossMonsterGaugeWnd * >( g_GUIMgr.FindWnd( MONSTER_BOSS_GAUGE_WND ) );
	if( !pBossMonsterGaugeWnd ) return;

	// 보스 게이지
	if( pBossMonsterGaugeWnd->IsShow() )
	{
		MonsterSurvivalRecord *pRecord = FindRecordCharName( pBossMonsterGaugeWnd->GetMonsterName() );
		if( pRecord && ToNpcChar( pRecord->pChar ) )
		{	
			if( pRecord->pChar->GetState() == CS_DIE )
				pBossMonsterGaugeWnd->HideWnd();
			else
			{
				StatusValue &rkStatus = pRecord->pChar->GetHP();
				pBossMonsterGaugeWnd->UpdateBossMonster( rkStatus.m_fCurValue, rkStatus.m_fMaxValue );
			}			
		}
	}
		
	// 턴 제한 시간
	pTimeScoreWnd->UpdateTurn( GetDungeonFloor(), m_iModeSubNum );
	DWORD dwTimeGap = 0;
	switch( m_ModeState )
	{
	case MS_READY:
		pTimeScoreWnd->UpdateTime( m_dwCurRoundDuration, m_dwRoundDuration );
		break;
	case MS_PLAY:
		dwTimeGap = FRAMEGETTIME() - m_dwPlayStartTime;
		if( m_dwCurRoundDuration > dwTimeGap )
		{
			pTimeScoreWnd->UpdateTime( m_dwCurRoundDuration - dwTimeGap, m_dwRoundDuration );

			ProcessWarningSound( m_dwCurRoundDuration );
		}
		else
		{
			m_bTurnDeathTime = true;
			pTimeScoreWnd->UpdateTime( 0, m_dwRoundDuration );
		}	
		break;
	case MS_RESULT:
		{
			m_bTurnDeathTime = true;
			pTimeScoreWnd->UpdateTime( 0, m_dwRoundDuration );
		}
		break;
	}

	if( m_ModeState != MS_PLAY )
	{
		ProcessScoreWarningSound( false );
	}
}

void ioMonsterSurvivalMode::UpdateRoundStateWnd()
{
	if( g_MyInfo.IsSortContribute() )
	{
		DWORD dwTimeGap = FRAMEGETTIME() - m_dwPlayStartTime;
		if( m_dwCurRoundDuration > dwTimeGap )
			ContributeRoundAllPercent( m_dwRoundDuration - (m_dwCurRoundDuration - dwTimeGap), m_dwRoundDuration );
		else
			ContributeRoundAllPercent( m_dwRoundDuration, m_dwRoundDuration );

		MonsterSurvivalRecord *pRecord = FindRecordCharName( g_MyInfo.GetPublicID() );
		if( pRecord )
			g_MyInfo.SetMyContribute( pRecord->Contribute.m_iPrevContributePer );
	}

	if( m_ModeState != MS_PLAY )
	{
		if( g_GUIMgr.IsShow( ROUND_STATE_MAIN_WND ) )
		{
			g_GUIMgr.HideWnd( ROUND_STATE_MAIN_WND );

			BossMonsterGaugeWnd *pBossMonsterGaugeWnd = dynamic_cast<BossMonsterGaugeWnd *>( g_GUIMgr.FindWnd( MONSTER_BOSS_GAUGE_WND ));
			if(pBossMonsterGaugeWnd)
				pBossMonsterGaugeWnd->ChangeShowAniState( BossMonsterGaugeWnd::SAT_DOWN );

			SetViewRoundState( false );
		}
		return;
	}

	if( g_Input.IsKeyDown( KEY_TAB ) )
	{
		RoundStateMainWnd *pRoundStateWnd = dynamic_cast<RoundStateMainWnd*>(g_GUIMgr.FindWnd( ROUND_STATE_MAIN_WND ));
		if( pRoundStateWnd && !pRoundStateWnd->IsShow() && !pRoundStateWnd->IsSearchShow() )	// 2019-07-03
		{
			DWORD dwTimeGap = FRAMEGETTIME() - m_dwPlayStartTime;
			if( m_dwCurRoundDuration > dwTimeGap )
				ContributeRoundAllPercent( m_dwRoundDuration - (m_dwCurRoundDuration - dwTimeGap), m_dwRoundDuration );
			else
				ContributeRoundAllPercent( m_dwRoundDuration, m_dwRoundDuration );

			pRoundStateWnd->ClearRoundInfo();
			for( int i=0 ; i<GetRecordCharCnt(); i++ )
			{
				RoundInfo kInfo;
				if( GetRoundInfo( i, kInfo ) )
					pRoundStateWnd->AddRoundInfo( kInfo );
			}
			pRoundStateWnd->SetDungeonsScore( GetDungeonFloor() );
			pRoundStateWnd->ShowWnd();
		}
	}
	else
	{
		if( g_GUIMgr.IsShow( ROUND_STATE_MAIN_WND ) )
		{
			g_GUIMgr.HideWnd( ROUND_STATE_MAIN_WND );
		}
	}
}

void ioMonsterSurvivalMode::ProcessReadyState()
{
	if( P2PNetwork::IsNetworkPlaying() )	return;
	if( m_dwReadyStartTime + 4000 > FRAMEGETTIME() )
		return;

	SetModeState( MS_PLAY );
}

void ioMonsterSurvivalMode::ProcessPlayState()
{
	UpdateSoldierRevivalPenalty();
	//UpdateTimeGrowthTime();

	m_pCreator->NotifySpringDampUseSet( true );

	if( !P2PNetwork::IsNetworkPlaying() )
	{
		CheckRoundEnd();
	}
	else
	{
		// 부활 코인 사용		
		ioBaseChar *pOwner = GetOwnerChar();
		if( pOwner )
		{
			if( pOwner->IsPrisonerMode() || pOwner->GetState() == CS_DIE )
			{
				if( m_dwCurRevivalCoinTime == 0 )
				{
					m_dwCurRevivalCoinTime = FRAMEGETTIME();
					UIKOTime( m_dwCurRevivalCoinTime );
				}

				if( pOwner->CheckUseMonsterCoin() )
				{					
					// 1초 딜레이 및 마법진이 나오면 자동으로 부활하므로 골드 코인 사용을 막는다.
					if( FRAMEGETTIME() - m_dwCurRevivalCoinTime > 2000 && m_CurrentMagicZone.m_dwCollisionTime == 0 )
					{
						MonsterCoinUseWnd *pCoinUseWnd = dynamic_cast< MonsterCoinUseWnd *>( g_GUIMgr.FindWnd( MONSTER_COIN_USE_WND ) );
						if( pCoinUseWnd && !pCoinUseWnd->IsShow() )
							pCoinUseWnd->UpdateUseRevivalCoin( pOwner );
					}
				}
			}
			else
			{
				m_dwCurRevivalCoinTime = 0;
			}
		}

		// 시작 코인 사용
		CheckStartCoinUse();
	}
}

void ioMonsterSurvivalMode::ProcessZoneCollision()
{
	ProcessMagicZoneCollision();
	ProcessEscapeZoneCollision();
}

void ioMonsterSurvivalMode::ProcessMagicZoneCollision()
{
	if( m_bRoundSetEnd ) return;
	if( m_CurrentMagicZone.m_dwCollisionTime == 0 ) return;

	if( FRAMEGETTIME() - m_CurrentMagicZone.m_dwCurCollisionTime < m_CurrentMagicZone.m_dwCollisionTime )
		return;

	ioHashStringVec kCharReocrdNameList;
	FindBaseCharRecordNameList( kCharReocrdNameList );
	int iCharCnt = kCharReocrdNameList.size();
	for(int i = 0;i < iCharCnt;i++)
	{
		MonsterSurvivalRecord *pRecord = FindRecordCharName( kCharReocrdNameList[i] );
		if( pRecord == NULL ) continue;
		if( pRecord->pChar == NULL ) continue;
		if( pRecord->pChar->GetTeam() != TEAM_BLUE ) continue;
		if( pRecord->pChar->GetState() == CS_VIEW  ||
			pRecord->pChar->GetState() == CS_DIE   ||
			pRecord->pChar->GetState() == CS_OBSERVER ||
			pRecord->pChar->GetState() == CS_LOADING )
		{
			continue;
		}

		D3DXVECTOR3 vDiff = m_CurrentMagicZone.m_vZonePos - pRecord->pChar->GetWorldPosition();
		vDiff.y = 0.0f;
		float fDistSq = D3DXVec3LengthSq( &vDiff );		
		float fCollision = m_CurrentMagicZone.m_fCollisionRange * m_CurrentMagicZone.m_fCollisionRange;

		// 마법진 영역 확장
		if( FRAMEGETTIME() - m_CurrentMagicZone.m_dwCurCollisionTimeEx > m_CurrentMagicZone.m_dwCollisionTimeEx )
			fCollision = m_CurrentMagicZone.m_fCollisionRangeEx * m_CurrentMagicZone.m_fCollisionRangeEx;

		if( fDistSq < fCollision || pRecord->pChar->IsPrisonerMode() ) // 포로는 어디에 있든 즉시 승천
		{
			if( pRecord->pChar->IsPrisonerMode() )
				pRecord->pChar->EscapePrisonerMode( TEAM_NONE, true );
			pRecord->pChar->AddNewBuff( m_CurrentMagicZone.m_szCollisionBuff, pRecord->pChar->GetCharName(), "", NULL );
			
			if( pRecord->pChar->IsOwnerChar() )
			{
				m_pCreator->NotifyOwnerCameraLoseTime( FRAMEGETTIME() - 500 );
				g_GUIMgr.HideWnd( MONSTER_COIN_USE_WND );
			}
		}
	}
	kCharReocrdNameList.clear();	
}

void ioMonsterSurvivalMode::ProcessEscapeZoneCollision()
{
}

void ioMonsterSurvivalMode::ProcessNpcErase()
{
	int iSize = m_NpcEraseList.size();
	for(int i = 0;i < iSize;i++)
	{
		NpcEraseData &rkNpcData = m_NpcEraseList[i];

		// NPC 죽은 후 35초뒤에 삭제.. 한번에 삭제하면 렉이 있을 수 있으니 프레임마다 한개씩 삭제
		if( FRAMEGETTIME() - rkNpcData.m_dwEraseTimer > m_dwKeepLiveTime )         
		{
			m_pCreator->RemoveUser( rkNpcData.m_szEraseNpc, true );
			RemoveEraseNpc( rkNpcData.m_szEraseNpc );
			return;
		}
	}
}

void ioMonsterSurvivalMode::CheckRoundEnd()
{
}

void ioMonsterSurvivalMode::CheckStartCoinUse()
{
	if( m_bRoundSetEnd ) return;
	if( m_iUseMonsterCoinCnt == 0 ) return;
	if( m_dwCurStartCoinTime == 0 ) return;
	if( g_BattleRoomMgr.IsObserverUser( g_MyInfo.GetPublicID() ) || m_pCreator->IsStealthMode() ) return;

	DWORD dwGapTime = FRAMEGETTIME() - m_dwCurStartCoinTime;
	DWORD dwNextSec = dwGapTime / 1000;
	if( m_dwCurStartCoinSec != dwNextSec )
	{
		m_dwCurStartCoinSec = dwNextSec;
		DWORD dwMaxSec = ( m_dwStartCoinTime / 1000 );
		if( !m_bUsingMonsterCoin )
		{
			int iMonsterCoinCount = g_MyInfo.GetEtcMonsterCoin() + g_MyInfo.GetEtcGoldMonsterCoin();
			if( iMonsterCoinCount < m_iUseMonsterCoinCnt )
			{
				MonsterCoinUseWnd *pCoinUseWnd = dynamic_cast< MonsterCoinUseWnd *>( g_GUIMgr.FindWnd( MONSTER_COIN_USE_WND ) );
				if( pCoinUseWnd )
				{
					char szMent[MAX_PATH] = "";
					sprintf( szMent, "%s - %s", g_ModeSelectInfo.GetBattleModeTitle( g_BattleRoomMgr.GetSelectModeIndex() ).c_str(), 
									 g_ModeSelectInfo.GetBattleMapTitle( g_BattleRoomMgr.GetSelectModeIndex(), g_BattleRoomMgr.GetSelectMapIndex() ).c_str() );
					m_bUsingMonsterCoin = pCoinUseWnd->UpdateUseStartCoinLack( szMent, m_iUseMonsterCoinCnt, m_dwCurStartCoinSec, dwMaxSec );
				}
				else
				{
					//UJ 140120, 보안강화: 몬스터 코인 없이 계속 플레이할 수 있는 치팅을 막는다
					SP2Packet kPacket( CTPK_EXIT_ROOM );
					kPacket << EXIT_ROOM_MONSTER_COIN_LACK << 0 << true;
					TCPNetwork::SendToServer( kPacket );
					g_BattleRoomMgr.LeaveRoom();
				}
			}
			else if( m_dwCurStartCoinSec > dwMaxSec )
			{				
				// 시작 코인 사용
				SP2Packet kPacket( CTPK_USE_MONSTER_COIN );
				kPacket << USE_MONSTER_COIN_START << m_iUseMonsterCoinCnt;
				TCPNetwork::SendToServer( kPacket );
				g_GUIMgr.HideWnd( MONSTER_COIN_USE_WND );
				m_bUsingMonsterCoin = true;
			}
		}
	}
}

int ioMonsterSurvivalMode::GetTeamPlayerCount( TeamType eTeam )
{
	int iUserCnt = 0;

	int iSize = GetRecordCharCnt();
	for(int i = 0;i < iSize;i++)
	{
		ioBaseChar *pChar = GetRecordChar( i );
		if( !pChar ) continue;
		if( pChar->GetState() == CS_VIEW ) continue;
		if( pChar->GetState() == CS_LOADING ) continue;
		if( pChar->GetState() == CS_OBSERVER ) continue;
		
		if( pChar->GetTeam() == eTeam )
			iUserCnt++;
	}

	return iUserCnt;
}

void ioMonsterSurvivalMode::ProcessResultState()
{
	ShowEndScoreWnd( true );

	int iCharCnt = m_vCharRecordList.size();
	for( int i=0; i<iCharCnt; i++ )
	{
		ioBaseChar *pChar = GetRecordChar( i );
		if( pChar && pChar->IsCatchMode() )
		{
			pChar->ApplyEscapeCatchState( false );
		}
	}

	DWORD dwTimeGap = FRAMEGETTIME() - m_dwResultStartTime;
	if( !m_bResultCameraChange && m_dwResultCameraDuration > 0 && dwTimeGap > m_dwResultCameraDuration )
	{
		m_bResultCameraChange = true;

		// 옵저버는 다르게 처리
		if( g_BattleRoomMgr.IsObserverUser( g_MyInfo.GetPublicID() ) || m_pCreator->IsStealthMode() )
		{
			ioBaseChar *pTarget = m_pCreator->GetCameraTargetChar();
			if( pTarget )
			{
				m_vEndLookAt		= pTarget->GetWorldPosition();
				m_szEndLookCharName = pTarget->GetCharName();
			}
			else
			{
				m_vEndLookAt = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
				m_szEndLookCharName.Clear();
			}
		}
		else
		{
			ioBaseChar *pOwner = m_pCreator->GetOwnerChar();
			if( pOwner )
			{
				m_vEndLookAt		= pOwner->GetWorldPosition();
				m_szEndLookCharName = pOwner->GetCharName();
			}
			else
			{
				m_vEndLookAt = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
				m_szEndLookCharName.Clear();
			}
		}
	}

	if( !P2PNetwork::IsNetworkPlaying() )
	{
		if( dwTimeGap > m_dwCurResultShowDuration )
		{
			ShowEndScoreWnd( false );

			ClearModeData();
			UpdateRoundCount();

			LoadPushStructs();
			LoadMachineStruct();

			ReStartMode();

			if( m_iCurRoundCount == 1 )
			{
				UpdateUserRank();
			}
		}
	}
}

void ioMonsterSurvivalMode::RenderPreGUI()
{
	if( m_ModeState == MS_PLAY )
	{
		RenderPushEnableCursor();
	}
}

void ioMonsterSurvivalMode::RenderAfterGUI()
{
}

void ioMonsterSurvivalMode::AddNewRecord( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	MonsterSurvivalRecord *pMonsterSurvivalRecord = FindRecordCharName(pChar->GetCharName());
	if( !pMonsterSurvivalRecord )
	{
		MonsterSurvivalRecord kRecord;
		kRecord.pChar  = pChar;
		kRecord.szName = pChar->GetCharName();
		m_vCharRecordList.push_back( kRecord );		

		UpdateUserRank();

		if( !ToNpcChar( pChar ) && !g_BattleRoomMgr.IsObserverUser(pChar->GetCharName()) )
			g_AwardMgr.AddNewAwardUser( pChar->GetCharName() );
	}
	else
	{
		pMonsterSurvivalRecord->pChar = pChar;
	}
}


void ioMonsterSurvivalMode::RemoveRecord( const ioHashString &rkName )
{
	int iCharCnt = m_vCharRecordList.size();
	for( int i=0 ; i<iCharCnt ; i++ )
	{
		if( m_vCharRecordList[i].szName == rkName )
		{
			m_vCharRecordList.erase( m_vCharRecordList.begin() + i );
			break;
		}
	}

	UpdateUserRank();
}


void ioMonsterSurvivalMode::RemoveRecord( int nIndex )
{
	int iCharCnt = m_vCharRecordList.size();
	if( iCharCnt > nIndex )
	{
		m_vCharRecordList.erase( m_vCharRecordList.begin() + nIndex );
		UpdateUserRank();
	}
}


ioBaseChar* ioMonsterSurvivalMode::GetRecordChar( int i )
{
	if( COMPARE( i, 0, GetRecordCharCnt() ) )
		return m_vCharRecordList[i].pChar;

	return NULL;
}

ioBaseChar* ioMonsterSurvivalMode::GetRecordChar( const ioHashString &rkName )
{
	MonsterSurvivalRecord *pRecord = FindRecordCharName( rkName );
	if( pRecord )
		return pRecord->pChar;

	return NULL;
}

int ioMonsterSurvivalMode::GetRecordCharCnt() const
{
	return m_vCharRecordList.size();
}

ioBaseChar* ioMonsterSurvivalMode::GetRecordCharTeamArray( TeamType eTeam, int iArray )
{
	ioHashStringVec kCharReocrdNameList;
	FindBaseCharRecordNameList( kCharReocrdNameList );

	int iCurCnt = 0;
	int iRecordCnt = kCharReocrdNameList.size();
	for( int i=0 ; i<iRecordCnt ; i++ )
	{
		MonsterSurvivalRecord *pRecord = FindRecordCharName( kCharReocrdNameList[i] );
		if( !pRecord ) continue;
		if( !pRecord->pChar ) continue;
		if( pRecord->pChar->GetState() == CS_OBSERVER ) continue;
		if( g_BattleRoomMgr.IsObserverUser( pRecord->pChar->GetCharName() ) ) continue;

		if( iCurCnt == iArray )
			return pRecord->pChar;
		iCurCnt++;
	}
	return NULL;
}

ModeRecord* ioMonsterSurvivalMode::FindModeRecord( int iIdx )
{
	if( COMPARE( iIdx, 0, GetRecordCharCnt() ) )
		return &m_vCharRecordList[iIdx];

	return NULL;
}

ModeRecord* ioMonsterSurvivalMode::FindModeRecordCharName( const ioHashString &rkName )
{
	return (ModeRecord*)FindRecordCharName( rkName );
}

void ioMonsterSurvivalMode::FindBaseCharRecordNameList( ioHashStringVec &rkRecordNameList )
{
	rkRecordNameList.clear();
	int iCharCnt = m_vCharRecordList.size();
	for( int i=0 ; i<iCharCnt ; i++ )
	{
		MonsterSurvivalRecord &rkRecord = m_vCharRecordList[i];
		if( rkRecord.pChar == NULL ) continue;
		if( ToNpcChar( rkRecord.pChar ) ) continue;
        
		rkRecordNameList.push_back( rkRecord.szName );
	}
}

MonsterSurvivalRecord* ioMonsterSurvivalMode::FindRecordCharName( const ioHashString &rkName )
{
	int iCharCnt = m_vCharRecordList.size();
	for( int i=0 ; i<iCharCnt ; i++ )
	{
		if( m_vCharRecordList[i].szName == rkName )
			return &m_vCharRecordList[i];
	}

	return NULL;
}

void ioMonsterSurvivalMode::SetRoundEndInfo( WinTeamType eTeam, bool bRoundSetEnd )
{
	ioPlayMode::SetRoundEndInfo( eTeam, bRoundSetEnd );	
	if( bRoundSetEnd )
	{
		g_GUIMgr.HideWnd( MONSTER_COIN_USE_WND );
	}
}

void ioMonsterSurvivalMode::SetRoundEndVictories()
{
	// 연승 기록 안함
}

void ioMonsterSurvivalMode::SetRoundEndMotion()
{	
	int iCharCnt = GetRecordCharCnt();	
	for( int i=0; i<iCharCnt; i++ )
	{
		ioBaseChar *pChar = GetRecordChar( i );
		if( !pChar ) continue;
		if( pChar->GetState() == CS_OBSERVER ) continue;
		if( g_BattleRoomMgr.IsObserverUser(pChar->GetCharName()) ) continue;

		RoundEndMotion eEndMotion = RM_WIN;
		if( m_WinTeamType == WTT_BLUE_TEAM )
		{
			if( ToNpcChar( pChar ) )
				eEndMotion = RM_LOSE;
		}
		else
		{
			if( !ToNpcChar( pChar ) )
				eEndMotion = RM_LOSE;
		}
		pChar->SetRoundEndMotion( eEndMotion );
		pChar->StartNoInputDelayTime( 10000 );
	}	
}

void ioMonsterSurvivalMode::ShowEndScoreWnd( bool bShow )
{
	if( g_GUIMgr.IsShow( BRIEFING_WND ) ) return;
	if( g_BattleRoomMgr.IsBattleRoom() )
	{
		if( g_GUIMgr.IsShow( PARTY_SHAM_BATTLE_WND ) )
			return;
	}
	if( m_bRoundSetEnd ) return;
	if( g_GUIMgr.IsShow( MIDDLE_JOINDELAY_WND ) ) return;

	if( bShow )
	{
		if( !g_GUIMgr.IsShow( ROUND_RESULT_MAIN_WND ) )
		{
			g_GUIMgr.ShowWnd( ROUND_RESULT_MAIN_WND );
		}
	}
	else
	{
		g_GUIMgr.HideWnd( ROUND_RESULT_MAIN_WND );
	}
}

void ioMonsterSurvivalMode::AddEraseNpc( const ioHashString &rkNpcName )
{
	NpcEraseData kEraseData;
	kEraseData.m_szEraseNpc = rkNpcName;
	kEraseData.m_dwEraseTimer = FRAMEGETTIME();
	m_NpcEraseList.push_back( kEraseData );
}

void ioMonsterSurvivalMode::RemoveEraseNpc( const ioHashString &rkNpcName )
{
	int iSize = m_NpcEraseList.size();
	for(int i = 0;i < iSize;i++)
	{
		NpcEraseData &rkNpcData = m_NpcEraseList[i];
		if( rkNpcData.m_szEraseNpc == rkNpcName )
		{
			m_NpcEraseList.erase( m_NpcEraseList.begin() + i );
			return;
		}
	}
}

bool ioMonsterSurvivalMode::GetRoundInfo( int iIndex, RoundInfo &rkInfo )
{
	if( ioPlayMode::GetRoundInfo( iIndex, rkInfo ) )
	{
		MonsterSurvivalRecord *pRecord = (MonsterSurvivalRecord*)FindModeRecord( iIndex );
		if( !pRecord )	return false;
		if( !pRecord->pChar ) return false;

		rkInfo.m_iContribute	= pRecord->Contribute.m_iContribute;
		rkInfo.m_iContributePer = pRecord->Contribute.m_iPrevContributePer;		
		return true;
	}
	return false;
}

bool ioMonsterSurvivalMode::GetResultInfo( int iIndex, ResultInfo &rkInfo )
{
	if( ioPlayMode::GetResultInfo( iIndex, rkInfo ) )
	{
		MonsterSurvivalRecord *pRecord = (MonsterSurvivalRecord*)FindModeRecord( iIndex );
		if( !pRecord )	return false;
		if( !pRecord->pChar ) return false;

		rkInfo.m_iContribute = pRecord->Contribute.m_iContribute;
		rkInfo.m_iContributePer = pRecord->Contribute.m_iPrevContributePer;
		return true;
	}
	return false;
}

bool ioMonsterSurvivalMode::GetFinalInfo( int iIndex, FinalInfo &rkInfo )
{
	if( ioPlayMode::GetFinalInfo( iIndex, rkInfo ) )
	{
		MonsterSurvivalRecord *pRecord = (MonsterSurvivalRecord*)FindModeRecord( iIndex );
		if( !pRecord )	return false;
		if( !pRecord->pChar ) return false;

		rkInfo.m_iContribute = pRecord->Contribute.m_iContribute;
		rkInfo.m_iContributePer = pRecord->Contribute.m_iPrevContributePer;
		return true;
	}
	return false;
}

void ioMonsterSurvivalMode::UpdateDropDieRecord( ioBaseChar *pDieChar, ioBaseChar *pKillChar, ioBaseChar *pBestAttacker )
{
	ioHashString szDieName, szAttName;
	TeamType eDieTeam = TEAM_NONE;
	TeamType eAttTeam = TEAM_NONE;
	bool bKOChat = true;
	//
	ioNpcChar *pKillNpc = ToNpcChar( pKillChar );
	if( pKillNpc )
	{
		szAttName = pKillNpc->GetViewName();
		eAttTeam  = pKillNpc->GetTeam();		
	}
	else if( pKillChar )
	{
		szAttName = pKillChar->GetCharName();
		eAttTeam  = pKillChar->GetTeam();
	}

	//
	ioNpcChar *pDieNpc = ToNpcChar( pDieChar );
	if( pDieNpc )
	{
		szDieName = pDieNpc->GetViewName();
		eDieTeam  = pDieNpc->GetTeam();
		AddEraseNpc( pDieNpc->GetCharName() );

		if( !pDieNpc->IsMonsterHalfBoss() && !pDieNpc->IsMonsterLastBoss() )
			bKOChat = false;
	}
	else if( pDieChar )
	{
		szDieName = pDieChar->GetCharName();
		eDieTeam  = pDieChar->GetTeam();
	}	

	if( pKillChar )
	{
		ModeRecord *pAttRecord = FindModeRecordCharName( pKillChar->GetCharName() );
		ModeRecord *pDieRecord = FindModeRecordCharName( pDieChar->GetCharName() );		

		if( pAttRecord && pDieRecord )
		{
			float fKillPoint = FLOAT05 + ( (float)GetKillPoint( pAttRecord->pChar->GetTeam() ) * FLOAT05 );
			if( pAttRecord->pChar->GetTeam() != pDieRecord->pChar->GetTeam() )
			{
				pAttRecord->AddKill( GetModeType(), fKillPoint, true, false );
				pDieRecord->AddDeath( GetModeType(), GetDeathPoint( pDieRecord->pChar->GetTeam() ) );		
				
				if( pKillChar->IsOwnerChar() && ToNpcChar( pDieChar ) )
				{
					const CHARACTER &rkKillerCharData = pKillChar->GetCharacterInfo();
					ioNpcChar *pNpcChar = ToNpcChar( pDieChar );
					g_QuestMgr.QuestCompleteTerm( QC_BATTLE_PVE_KO, GetDungeonFloor(), rkKillerCharData.m_class_type, pNpcChar->GetNpcCode() );
				}

				if( pKillChar->GetTeam() == GetOwnerTeam() && ToNpcChar( pDieChar ) )
				{
					const CHARACTER &rkKillerCharData = pKillChar->GetCharacterInfo();
					ioNpcChar *pNpcChar = ToNpcChar( pDieChar );
					g_QuestMgr.QuestCompleteTerm( QC_BATTLE_PVE_TEAM_KO, GetDungeonFloor(), rkKillerCharData.m_class_type, pNpcChar->GetNpcCode() );
				}
			}
			else
			{
				szAttName.Clear();
				eAttTeam = TEAM_NONE;
				if( pAttRecord->pChar == pDieRecord->pChar )	// 자기자신의 공격
				{
					pDieRecord->AddDeath( GetModeType(), GetDeathPoint( pDieRecord->pChar->GetTeam() ) );
				}
				else
				{
					pAttRecord->AddKill( GetModeType(), -fKillPoint, true, false );
					pDieRecord->AddDeath( GetModeType(), GetDeathPoint( pDieRecord->pChar->GetTeam() ) );
				}
			}

			pAttRecord->pChar->SetKillEmoticon( pAttRecord->GetUniqueTotalKill() );
			pDieRecord->pChar->SetDeathEmoticon();

			if( bKOChat )
				g_ModeUIController.SetKOMsg( eAttTeam, szAttName, pDieRecord->pChar->GetLastAttackerSkillName(), pDieRecord->pChar->GetLastAttackerItemCode(), eDieTeam, szDieName );
		}
	}
	else
	{
		ModeRecord *pDieRecord = FindModeRecordCharName( pDieChar->GetCharName() );
		if( pDieRecord )
		{
			pDieRecord->AddDeath( GetModeType(), GetDeathPoint( pDieRecord->pChar->GetTeam() ) );
			pDieRecord->pChar->SetDeathEmoticon();

			if( bKOChat )
				g_ModeUIController.SetKOMsg( eAttTeam, szAttName, pDieRecord->pChar->GetLastAttackerSkillName(), pDieRecord->pChar->GetLastAttackerItemCode(), eDieTeam, szDieName );
		}
	}

	// 멘티스 : 0001021 수정 jch, 드랍 시 고정 블라인드 처리 불필요. (void ioBaseChar::DropDieProcess - 블라인드 이미 처리 됨)
	/*
	if( pDieChar && pDieChar->IsOwnerChar() )
	{
		m_pCreator->StartScreenBlind( SBT_FIXED_BLIND, NULL, 0 );
	}
	*/

	// Best Attacker 처리 안함
}

void ioMonsterSurvivalMode::UpdateKillDeathRecord( ioPlayEntity *pDieEntity, ioPlayEntity *pKiller, ioPlayEntity *pBestAttacker )
{
	ioBaseChar *pKillerChar = ToBaseChar( pKiller );
	ioBaseChar *pDieChar = ToBaseChar( pDieEntity );

	ioHashString szDieName, szAttName;
	TeamType eDieTeam = TEAM_NONE;
	TeamType eAttTeam = TEAM_NONE;
	bool bKOChat = true;
	//
	ioNpcChar *pKillNpc = ToNpcChar( pKillerChar );
	if( pKillNpc )
	{
		szAttName = pKillNpc->GetViewName();
		eAttTeam  = pKillNpc->GetTeam();
	}
	else if( pKillerChar )
	{
		szAttName = pKillerChar->GetCharName();
		eAttTeam  = pKillerChar->GetTeam();
	}

	//
	ioNpcChar *pDieNpc = ToNpcChar( pDieChar );
	if( pDieNpc )
	{
		szDieName = pDieNpc->GetViewName();
		eDieTeam  = pDieNpc->GetTeam();
		AddEraseNpc( pDieNpc->GetCharName() );

		if( !pDieNpc->IsMonsterHalfBoss() && !pDieNpc->IsMonsterLastBoss() )
			bKOChat = false;
	}
	else if( pDieChar )
	{
		szDieName = pDieChar->GetCharName();
		eDieTeam  = pDieChar->GetTeam();
	}	

	if( pKillerChar && pDieChar )
	{
		ModeRecord *pAttRecord = FindModeRecordCharName( pKillerChar->GetCharName() );
		ModeRecord *pDieRecord = FindModeRecordCharName( pDieChar->GetCharName() );

		if( pAttRecord && pDieRecord )
		{
			if( pAttRecord->pChar->GetTeam() != pDieRecord->pChar->GetTeam() )
			{
				pAttRecord->AddKill( GetModeType(), GetKillPoint( pAttRecord->pChar->GetTeam() ), false, false );
				pDieRecord->AddDeath( GetModeType(), GetDeathPoint( pDieRecord->pChar->GetTeam() ) );

				if( pKillerChar->IsOwnerChar() && ToNpcChar( pDieChar ) )
				{
					const CHARACTER &rkKillerCharData = pKillerChar->GetCharacterInfo();
					ioNpcChar *pNpcChar = ToNpcChar( pDieChar );
					g_QuestMgr.QuestCompleteTerm( QC_BATTLE_PVE_KO, GetDungeonFloor(), rkKillerCharData.m_class_type, pNpcChar->GetNpcCode() );
				}

				if( pKillerChar->GetTeam() == GetOwnerTeam() && ToNpcChar( pDieChar ) )
				{
					const CHARACTER &rkKillerCharData = pKillerChar->GetCharacterInfo();
					ioNpcChar *pNpcChar = ToNpcChar( pDieChar );
					g_QuestMgr.QuestCompleteTerm( QC_BATTLE_PVE_TEAM_KO, GetDungeonFloor(), rkKillerCharData.m_class_type, pNpcChar->GetNpcCode() );
				}
			}
			else
			{
				szAttName.Clear();
				eAttTeam = TEAM_NONE;
				if( pAttRecord->pChar == pDieRecord->pChar )	// 자기자신의 공격
				{
					pDieRecord->AddDeath( GetModeType(), GetDeathPoint( pDieRecord->pChar->GetTeam() ) );
				}
				else
				{
					pAttRecord->AddKill( GetModeType(), -GetKillPoint( pAttRecord->pChar->GetTeam() ), false, false );
					pDieRecord->AddDeath( GetModeType(), GetDeathPoint( pDieRecord->pChar->GetTeam() ) );
				}
			}

			pAttRecord->pChar->SetKillEmoticon( pAttRecord->GetUniqueTotalKill() );
			pDieRecord->pChar->SetDeathEmoticon();

			if( bKOChat )
				g_ModeUIController.SetKOMsg( eAttTeam, szAttName, pDieRecord->pChar->GetLastAttackerSkillName(), pDieRecord->pChar->GetLastAttackerItemCode(), eDieTeam, szDieName );
		}
	}
	else
	{
		ModeRecord *pDieRecord = FindModeRecordCharName( pDieChar->GetCharName() );
		if( pDieRecord )
		{
			pDieRecord->AddDeath( GetModeType(), GetDeathPoint( pDieRecord->pChar->GetTeam() ) );
			pDieRecord->pChar->SetDeathEmoticon();

			if( bKOChat )
				g_ModeUIController.SetKOMsg( eAttTeam, szAttName, pDieRecord->pChar->GetLastAttackerSkillName(), pDieRecord->pChar->GetLastAttackerItemCode(), eDieTeam, szDieName );
		}
	}

	// Best Attacker 처리 안함
}

void ioMonsterSurvivalMode::ApplyModeInfo( SP2Packet &rkPacket )
{
	rkPacket >> m_iCurRoundCount;
	rkPacket >> m_iMaxRoundCount;

	rkPacket >> m_dwRoundDuration;

	rkPacket >> m_iBlueRoundWinCnt;
	rkPacket >> m_iRedRoundWinCnt;

	rkPacket >> m_iBluePosArray;
	rkPacket >> m_iRedPosArray;

	m_vRoundHistory.erase( m_vRoundHistory.begin(), m_vRoundHistory.end() );
	m_vRoundHistory.reserve( m_iCurRoundCount );

	for( int i=0; i < m_iCurRoundCount; i++ )
	{
		RoundHistory rh;
		rkPacket >> rh.iBluePoint;
		rkPacket >> rh.iRedPoint;
		m_vRoundHistory.push_back( rh );
	}
}

void ioMonsterSurvivalMode::ApplyExtraModeInfo( SP2Packet &rkPacket )
{
}

void ioMonsterSurvivalMode::ApplyCharModeInfo( const ioHashString &rkName, SP2Packet &rkPacket, bool bDieCheck )
{
	bool bRecord;
	rkPacket >> bRecord;

	if( !bRecord ) return;

	MonsterSurvivalRecord *pRecord = FindRecordCharName( rkName );
	if( pRecord )
	{
		// HARDCODE: 아래쪽에 유저가 없는 경우도 반드시 동일하게 해줄것
		int iKillCharCnt, iDeathCharCnt;
		pRecord->iKillInfoMap.clear();
		pRecord->iDeathInfoMap.clear();

		rkPacket >> iKillCharCnt;
		for( int i=0; i < iKillCharCnt; ++i )
		{
			int iClassType, iKillCnt;
			rkPacket >> iClassType >> iKillCnt;

			pRecord->iKillInfoMap.insert( KillDeathInfoMap::value_type(iClassType, iKillCnt) );
		}

		rkPacket >> iDeathCharCnt;
		for( int j=0; j < iDeathCharCnt; ++j )
		{
			int iClassType, iDeathCnt;
			rkPacket >> iClassType >> iDeathCnt;

			pRecord->iDeathInfoMap.insert( KillDeathInfoMap::value_type(iClassType, iDeathCnt) );
		}

		if( bDieCheck )
		{
			bool bDie;
			rkPacket >> bDie;

			if( bDie )
			{
				pRecord->pChar->SetDieState();
			}
		}

		bool bPrisoner, bCatch;
		rkPacket >> bPrisoner >> bCatch;
	}
	else
	{
		int iKillCharCnt, iDeathCharCnt;

		rkPacket >> iKillCharCnt;
		for( int i=0; i < iKillCharCnt; ++i )
		{
			int iClassType, iKillCnt;
			rkPacket >> iClassType >> iKillCnt;
		}

		rkPacket >> iDeathCharCnt;
		for( int j=0; j < iDeathCharCnt; ++j )
		{
			int iClassType, iDeathCnt;
			rkPacket >> iClassType >> iDeathCnt;
		}

		if( bDieCheck )
		{
			bool bDie;
			rkPacket >> bDie;
		}

		bool bPrisoner, bCatch;
		rkPacket >> bPrisoner >> bCatch;
	}
}

bool ioMonsterSurvivalMode::ProcessUDPPacket( SP2Packet &rkPacket )
{
	if( ioPlayMode::ProcessUDPPacket( rkPacket ) )
		return true;

	return false;
}

void ioMonsterSurvivalMode::NotifyEntityDie( ioPlayEntity *pEntity, ioPlayEntity *pKiller, ioBaseChar *pBestAttacker,
											float fLastRate, float fBestRate )
{	
	ioBaseChar *pDieChar = ToBaseChar( pEntity );
	ioBaseChar *pKillChar = ToBaseChar( pKiller );

	// 순서 주의 이모티콘 출력때문
	UpdateKillDeathRecord( pEntity, pKiller, pBestAttacker );
	OnNotifyCharDie( pDieChar, pKillChar, pBestAttacker, fLastRate, fBestRate );
}

void ioMonsterSurvivalMode::NotifyCharDropDie( ioBaseChar *pDieChar, ioBaseChar *pKillChar, ioBaseChar *pBestAttacker,
											  float fLastRate, float fBestRate )
{
	// 순서 주의 이모티콘 출력때문
	UpdateDropDieRecord( pDieChar, pKillChar, pBestAttacker );
	OnNotifyCharDie( pDieChar, pKillChar, pBestAttacker, fLastRate, fBestRate );
}

void ioMonsterSurvivalMode::NotifyCharDrop( ioPlayEntity *pEntity, float fDropDamage )
{
	ioBaseChar *pDropChar = ToBaseChar( pEntity );

	// NPC는 그냥 죽음
	if( ToNpcChar( pDropChar ) )
	{
		ioPlayMode::NotifyCharDrop( pEntity, fDropDamage );
		return;
	}

	if( !pDropChar ) return;
	if( !pDropChar->IsNeedProcess() ) return;

	if( pDropChar->IsPrisonerMode() )
	{
		if( P2PNetwork::IsNetworkPlaying() )
		{
			pDropChar->CheckLastAttackerTime( false );

			SP2Packet kServer( CTPK_PRISONER_DROP );
			kServer << pDropChar->GetLastAttackerName();
			FillOwnerDamagedListByOtherTeam( pDropChar->GetCharName(), pDropChar->GetTeam(), kServer, true );
			TCPNetwork::SendToServer( kServer );

			pDropChar->SetDieState();
		}
		else
		{
			pDropChar->EscapePrisonerMode( TEAM_NONE, false );
			pDropChar->SetDieState();
		}
	}
	else
	{
		ioHashString szAttackerName = pDropChar->GetLastAttackerName();
		if( pDropChar->IsForceDropping() && !szAttackerName.IsEmpty() )
		{
			pDropChar->SetLastAttackerName( "", "" );

			ioBaseChar *pAttacker = m_pCreator->GetBaseChar( szAttackerName );
			if( pAttacker && pDropChar->GetTeam() != pAttacker->GetTeam() )
			{
				NotifyOwnerDamaged( pDropChar->GetCharName(), pAttacker->GetCharName(), fDropDamage );
			}
		}		
	}
}

void ioMonsterSurvivalMode::OnNotifyCharDie( ioBaseChar *pDieChar, ioBaseChar *pKillChar, ioBaseChar *pBestAttacker,
											float fLastRate, float fBestRate )
{
	if( !pDieChar )
	{
		LOG.PrintTimeAndLog( 0, "ioMonsterSurvivalMode::OnNotifyCharDie - Entity is Not Char" );
		return;
	}

	MonsterSurvivalRecord *pDieRecord = FindRecordCharName( pDieChar->GetCharName() );
	if( !pDieRecord )
	{
		LOG.PrintTimeAndLog( 0, "ioMonsterSurvivalMode::OnNotifyCharDie - %s Not Exist Char",
								pDieChar->GetCharName().c_str() );
		return;
	}

	// 다음 NPC들 등장 시간 감소
	if( ToNpcChar( pDieChar ) )
	{
		ReduceNpcCreateTime();
	}

	if( P2PNetwork::IsNetworkPlaying() )
	{
		if( g_MyInfo.GetPublicID() == pDieChar->GetCharName() )
		{
			g_MyInfo.SetCharDied( pDieChar->GetSelectCharArray(), true );
		}
	}

	//For EndScene
	m_LastDieChar = pDieChar->GetCharName();

	if( pKillChar )
	{
		MonsterSurvivalRecord *pKillRecord = FindRecordCharName( pKillChar->GetCharName() );
		if( !pKillRecord )
		{
			LOG.PrintTimeAndLog( 0, "ioMonsterSurvivalMode::OnNotifyCharDie - %s Not Exist Kill Char",
									pKillChar->GetCharName().c_str() );

			m_LastKillChar.Clear();
		}
		else
		{
			if( pKillChar->GetTeam() == pDieChar->GetTeam() )
				m_LastKillChar.Clear();
			else
				m_LastKillChar = pKillChar->GetCharName();
		}
	}
	else
	{
		m_LastKillChar.Clear();
	}
}

void ioMonsterSurvivalMode::MonsterEventMent( const ioHashString &rkNextTarget )
{
	MonsterSurvivalRecord *pMonsterRecord = FindRecordCharName( rkNextTarget );
	if( !pMonsterRecord ) return;
	if( !pMonsterRecord->pChar ) return;

	ioNpcChar *pNpcChar = ToNpcChar( pMonsterRecord->pChar );
	if( !pNpcChar ) return;
	if( pNpcChar->GetMonsterMent().IsEmpty() ) return;

	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_OUTLINE_FULL_2X );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( pNpcChar->GetMonsterMentColor() );	
	kPrinter.AddTextPiece( pNpcChar->GetMonsterMentScale(), pNpcChar->GetMonsterMent().c_str() );
    SetCenterHelp( kPrinter, 0, pNpcChar->GetMonsterMentTime() );
}

void ioMonsterSurvivalMode::ReStartMode()
{
	D3DXVECTOR3 vStartPos;

	int iCharCnt = m_vCharRecordList.size();
	for( int i=0 ; i<iCharCnt ; i++ )
	{
		MonsterSurvivalRecord &rkRecord = m_vCharRecordList[i];
		if( !rkRecord.pChar ) continue;
		if( ToNpcChar( rkRecord.pChar ) ) continue;

		vStartPos = GetStartPos( rkRecord.pChar->GetTeam() );
		rkRecord.pChar->SetWorldPosition( vStartPos );

		rkRecord.dwRevivalGap = 0;
		rkRecord.dwCurDieTime = 0;
		rkRecord.iRevivalCnt = 0;

		if( m_iCurRoundCount == 1 )	// Final Round End
		{
			rkRecord.InitValues();

			m_iBlueRoundWinCnt = 0;
			m_iRedRoundWinCnt  = 0;
		}

		rkRecord.pChar->SetProtectionMode( PM_NONE, 0 );
		rkRecord.pChar->SetRoundEndMotion( RM_NONE );
		rkRecord.pChar->SetProtectionMode( PM_START, 2000 );
		rkRecord.pChar->Revival();

		if( rkRecord.pChar->GetState() != CS_OBSERVER )
		{
			if( !g_BattleRoomMgr.IsObserverUser(rkRecord.pChar->GetCharName()) )
				rkRecord.pChar->SetReadyState();
			else
				rkRecord.pChar->SetState( CS_OBSERVER );
		}
	}

	m_dwCurRevivalTime = 0;
	m_dwCheckWarningTime = 0;
	m_bScoreWarning = false;
	m_bFirstSelect = true;

	// 라운드제
	m_bZeroHP = false;
	TimeScoreWnd *pTimeScoreWnd = dynamic_cast<TimeScoreWnd *>( g_GUIMgr.FindWnd( TIME_SCORE_WND ) );
	if(pTimeScoreWnd)
		pTimeScoreWnd->InitAniState();

	BossMonsterGaugeWnd *pBossMonsterGaugeWnd = dynamic_cast<BossMonsterGaugeWnd *>( g_GUIMgr.FindWnd( MONSTER_BOSS_GAUGE_WND ) );
	if(pBossMonsterGaugeWnd)
		pBossMonsterGaugeWnd->InitAniState();

	int iHistorySize = m_vRoundHistory.size();
	if( (m_iCurRoundCount-1) == iHistorySize )
	{
		RoundHistory rh;
		m_vRoundHistory.push_back( rh );
	}

	if( m_bCharDieClearByRound )
	{
		g_MyInfo.ClearCharDiedInfo();
	}

	if( P2PNetwork::IsNetworkPlaying() )
	{
		m_pCreator->HidingAllFieldItem();
		m_pCreator->DestroyWeaponList();
	}

	g_GUIMgr.CheckShopSound( true );
	SetModeState( MS_READY );

	m_pCreator->ReCalculateDamageRate( true );

	// 라운드 시작시 브리핑
	m_pCreator->SetModeEndSceneCamera( false );

	if( !P2PNetwork::IsNetworkPlaying() )
		m_pCreator->StartModeStartEventCamera( true );
	else
		m_pCreator->StartModeStartEventCamera( false );

	SetModeMapSound();
	m_pCreator->EnableWorldEntityFunctions( true );
	m_pCreator->SetMapCameraFixedObject( false );

	// MapAreaWeapon Set
	SetMapAreaWeapon();
}

D3DXVECTOR3 ioMonsterSurvivalMode::GetRevivalPos( ioBaseChar *pChar )
{
	if( m_vRevivalPos.empty() )
		return m_vMapCenter;

	if( ToNpcChar( pChar ) )
	{
		ioNpcChar *pNpcChar = ToNpcChar( pChar );
		D3DXVECTOR3 vCenter( pNpcChar->GetStartXPos(), 0.0f, pNpcChar->GetStartZPos() );
		vCenter.y = GetMapHeightByRevial( vCenter.x, vCenter.z );
		return vCenter;
	}
	else
	{
		int iPosArray = m_RevivalPosRandom.Random( m_vRevivalPos.size() );
		if( !COMPARE( iPosArray, 0, (int)m_vRevivalPos.size() ) )
			iPosArray = 0;

		D3DXVECTOR3 vCenter = m_vRevivalPos[iPosArray];
		D3DXQUATERNION qtRot;
		D3DXQuaternionRotationYawPitchRoll( &qtRot, ioMath::RangeRandom( 0.0f, D3DX_PI * 2.0f ), 0.0f, 0.0f );

		D3DXVECTOR3 vMoveDir = qtRot * ioMath::UNIT_Z;
		vCenter += vMoveDir * ioMath::RangeRandom( FLOAT10, m_fRandomRange );
		vCenter.y = GetMapHeightByRevial( vCenter.x, vCenter.z );

		return vCenter;
	}
	return m_vMapCenter;
}

D3DXVECTOR3 ioMonsterSurvivalMode::GetStartPos( TeamType eTeamType, bool bRandom )
{
	if( m_vStartPos.empty() )
		return m_vMapCenter;

	int iPosArray = m_StartPosRandom.Random( m_vStartPos.size() );
	if( !COMPARE( iPosArray, 0, (int)m_vStartPos.size() ) )
		iPosArray = 0;

	D3DXVECTOR3 vCenter = m_vStartPos[iPosArray];
	D3DXQUATERNION qtRot;
	D3DXQuaternionRotationYawPitchRoll( &qtRot, ioMath::RangeRandom( 0.0f, D3DX_PI * 2.0f ), 0.0f, 0.0f );

	D3DXVECTOR3 vMoveDir = qtRot * ioMath::UNIT_Z;
	vCenter += vMoveDir * ioMath::RangeRandom( FLOAT10, m_fRandomRange );
	vCenter.y = GetMapHeightByRevial( vCenter.x, vCenter.z );

	return vCenter;
}

void ioMonsterSurvivalMode::ContributeLOG()
{
	char szLog[MAX_PATH];

	int iCharCnt = m_vCharRecordList.size();
	for( int i=0 ;i<iCharCnt; i++ )
	{
		MonsterSurvivalRecord *pRecord = &m_vCharRecordList[i];
		if( pRecord->pChar && !ToNpcChar( pRecord->pChar ) )
		{
			sprintf( szLog, "%s[%d] : K%d(%d) D%d(%d) A%d(%d) D%d(%d)", pRecord->szName.c_str(), (int)pRecord->pChar->GetTeam(),
																        pRecord->Contribute.m_iKillRank + 1, pRecord->Contribute.m_iKill,
																        pRecord->Contribute.m_iDamageRank + 1, pRecord->Contribute.m_iDamage,
																        pRecord->Contribute.m_iAttackCntRank + 1, pRecord->Contribute.m_iAttackCnt,
																		pRecord->Contribute.m_iDeathRank + 1, pRecord->Contribute.m_iDeath );
			g_ChatMgr.SetSystemMsg( szLog );
		}
	}
}

void ioMonsterSurvivalMode::ContributeConvertTeam( TeamType eTeam )
{
	if( m_vContributeTable.size() < MAX_PLAYER )
		return;

	ioHashStringVec kCharReocrdNameList;
	FindBaseCharRecordNameList( kCharReocrdNameList );

	int i = 0;
	int iCharCnt = kCharReocrdNameList.size();
	int iInfoCnt = 0;
	ContributeInfoList vInfo;

	//KO
	for(i = 0;i < iCharCnt;i++)
	{
		MonsterSurvivalRecord *pRecord = FindRecordCharName( kCharReocrdNameList[i] );
		if( !pRecord ) continue;
		if( !pRecord->pChar ) continue;
		if( pRecord->pChar->GetState() == CS_OBSERVER ) continue;
		if( g_BattleRoomMgr.IsObserverUser(pRecord->pChar->GetCharName()) ) continue;

		if( eTeam != TEAM_NONE )
		{
			if( pRecord->pChar && pRecord->pChar->GetTeam() != eTeam ) continue;
		}

		ContributeInfo Contribute;
		Contribute.m_iRecordArray     = i;
		Contribute.m_iContributePoint = pRecord->Contribute.m_iKill;
		vInfo.push_back( Contribute );
	}
	iInfoCnt = vInfo.size();

	std::sort( vInfo.begin(), vInfo.end(), ContributeSort() );
	ContributeLowerSort( vInfo );

	for(i = 0;i < iInfoCnt;i++)
	{
		int iArray = vInfo[i].m_iRecordArray;
		if( COMPARE( iArray, 0, iCharCnt ) )
		{
			MonsterSurvivalRecord *pRecord = FindRecordCharName( kCharReocrdNameList[iArray] );
			if( !pRecord->Contribute.m_bLastResultSync )
				pRecord->Contribute.m_iContribute = m_vContributeTable[vInfo[i].m_iRank];
			pRecord->Contribute.m_iKillRank = vInfo[i].m_iRank;
		}
	}
	vInfo.clear();

	//데미지 & 힐
	for(i = 0;i < iCharCnt;i++)
	{
		MonsterSurvivalRecord *pRecord = FindRecordCharName( kCharReocrdNameList[i] );
		if( !pRecord ) continue;
		if( !pRecord->pChar ) continue;
		if( pRecord->pChar->GetState() == CS_OBSERVER ) continue;
		if( g_BattleRoomMgr.IsObserverUser(pRecord->pChar->GetCharName()) ) continue;

		if( eTeam != TEAM_NONE )
		{
			if( pRecord->pChar && pRecord->pChar->GetTeam() != eTeam ) continue;
		}

		ContributeInfo Contribute;
		Contribute.m_iRecordArray = i;
		Contribute.m_iContributePoint = pRecord->Contribute.m_iDamage;
		vInfo.push_back( Contribute );
	}
	iInfoCnt = vInfo.size();

	std::sort( vInfo.begin(), vInfo.end(), ContributeSort() );
	ContributeLowerSort( vInfo );

	for(i = 0;i < iInfoCnt;i++)
	{
		int iArray = vInfo[i].m_iRecordArray;
		if( COMPARE( iArray, 0, iCharCnt ) )
		{
			MonsterSurvivalRecord *pRecord = FindRecordCharName( kCharReocrdNameList[iArray] );
			if( !pRecord->Contribute.m_bLastResultSync )
				pRecord->Contribute.m_iContribute += m_vContributeTable[vInfo[i].m_iRank] * 6;
			pRecord->Contribute.m_iDamageRank = vInfo[i].m_iRank;
		}
	}
	vInfo.clear();

	//유효 타격 횟수
	for(i = 0;i < iCharCnt;i++)
	{
		MonsterSurvivalRecord *pRecord = FindRecordCharName( kCharReocrdNameList[i] );
		if( !pRecord ) continue;
		if( !pRecord->pChar ) continue;
		if( pRecord->pChar->GetState() == CS_OBSERVER ) continue;
		if( g_BattleRoomMgr.IsObserverUser(pRecord->pChar->GetCharName()) ) continue;

		if( eTeam != TEAM_NONE )
		{
			if( pRecord->pChar && pRecord->pChar->GetTeam() != eTeam ) continue;
		}

		ContributeInfo Contribute;
		Contribute.m_iRecordArray = i;
		Contribute.m_iContributePoint = pRecord->Contribute.m_iAttackCnt;
		vInfo.push_back( Contribute );
	}
	iInfoCnt = vInfo.size();

	std::sort( vInfo.begin(), vInfo.end(), ContributeSort() );
	ContributeLowerSort( vInfo );

	for(i = 0;i < iInfoCnt;i++)
	{
		int iArray = vInfo[i].m_iRecordArray;
		if( COMPARE( iArray, 0, iCharCnt ) )
		{
			MonsterSurvivalRecord *pRecord = FindRecordCharName( kCharReocrdNameList[iArray] );
			if( !pRecord->Contribute.m_bLastResultSync )
				pRecord->Contribute.m_iContribute += m_vContributeTable[vInfo[i].m_iRank] * 2;
			pRecord->Contribute.m_iAttackCntRank = vInfo[i].m_iRank;
		}
	}
	vInfo.clear();

	//데쓰처리
	for(i = 0;i < iCharCnt;i++)
	{
		MonsterSurvivalRecord *pRecord = FindRecordCharName( kCharReocrdNameList[i] );
		if( !pRecord ) continue;
		if( !pRecord->pChar ) continue;
		if( pRecord->pChar->GetState() == CS_OBSERVER ) continue;
		if( g_BattleRoomMgr.IsObserverUser(pRecord->pChar->GetCharName()) ) continue;

		if( eTeam != TEAM_NONE )
		{
			if( pRecord->pChar && pRecord->pChar->GetTeam() != eTeam ) continue;
		}

		ContributeInfo Contribute;
		Contribute.m_iRecordArray = i;
		Contribute.m_iContributePoint = 100000 - pRecord->Contribute.m_iDeath;          
		vInfo.push_back( Contribute );
	}
	iInfoCnt = vInfo.size();

	std::sort( vInfo.begin(), vInfo.end(), ContributeSort() );
	ContributeLowerSort( vInfo );

	for(i = 0;i < iInfoCnt;i++)
	{
		int iArray = vInfo[i].m_iRecordArray;
		if( COMPARE( iArray, 0, iCharCnt ) )
		{
			MonsterSurvivalRecord *pRecord = FindRecordCharName( kCharReocrdNameList[iArray] );
			if( !pRecord->Contribute.m_bLastResultSync )
				pRecord->Contribute.m_iContribute += m_vContributeTable[vInfo[i].m_iRank];
			pRecord->Contribute.m_iDeathRank = vInfo[i].m_iRank;
		}
	}
	vInfo.clear();
	kCharReocrdNameList.clear();
}

void ioMonsterSurvivalMode::ContributeConvert()
{
	ContributeConvertTeam( TEAM_NONE );        //TEAM_NONE는 전체 유저에 대한 기여도 평가	
}

int ioMonsterSurvivalMode::ContributeMyRank( int &rRankValue1, int &rRankValue2, int &rRankValue3, int &rRankValue4 )
{
	rRankValue1 = rRankValue2 = rRankValue3 = rRankValue4 = -1;

	bool bObserver = false;
	ioBaseChar *pOwner = GetOwnerChar();
	if( pOwner && ( g_BattleRoomMgr.IsObserverUser(pOwner->GetCharName()) || m_pCreator->IsStealthMode() ) )
		bObserver = true;

	ioHashStringVec kCharReocrdNameList;
	FindBaseCharRecordNameList( kCharReocrdNameList );

	int i = 0;
	int iCharCnt = kCharReocrdNameList.size();
	int iInfoCnt = 0;
	ContributeInfoList vInfo;

	//KO
	for(i = 0;i < iCharCnt;i++)
	{
		MonsterSurvivalRecord *pRecord = FindRecordCharName( kCharReocrdNameList[i] );
		if( !pRecord ) continue;
		if( !pRecord->pChar ) continue;
		if( pRecord->pChar->GetState() == CS_OBSERVER ) continue;
		if( g_BattleRoomMgr.IsObserverUser(pRecord->pChar->GetCharName()) ) continue;

		ContributeInfo Contribute;
		Contribute.m_iRecordArray     = i;
		Contribute.m_iContributePoint = pRecord->Contribute.m_iKill;
		Contribute.m_iGradeLevel      = pRecord->pChar->GetLevel();
		Contribute.m_szName           = pRecord->szName;
		vInfo.push_back( Contribute );
	}
	std::sort( vInfo.begin(), vInfo.end(), ContributeRankSort() );
	iInfoCnt = vInfo.size();

	if( !bObserver )
	{
		for(rRankValue1 = 0;rRankValue1 < iInfoCnt;rRankValue1++)
		{
			ContributeInfo &kContribute = vInfo[rRankValue1];
			if( kContribute.m_szName == g_MyInfo.GetPublicID() )
				break;
		}
	}

	vInfo.clear();	

	//유효 타격 횟수
	for(i = 0;i < iCharCnt;i++)
	{
		MonsterSurvivalRecord *pRecord = FindRecordCharName( kCharReocrdNameList[i] );
		if( !pRecord ) continue;
		if( !pRecord->pChar ) continue;
		if( pRecord->pChar->GetState() == CS_OBSERVER ) continue;
		if( g_BattleRoomMgr.IsObserverUser(pRecord->pChar->GetCharName()) ) continue;

		ContributeInfo Contribute;
		Contribute.m_iRecordArray = i;
		Contribute.m_iContributePoint = pRecord->Contribute.m_iAttackCnt;
		Contribute.m_iGradeLevel      = pRecord->pChar->GetLevel();
		Contribute.m_szName           = pRecord->szName;
		vInfo.push_back( Contribute );
	}
	std::sort( vInfo.begin(), vInfo.end(), ContributeRankSort() );
	iInfoCnt = vInfo.size();

	if( !bObserver )
	{
		for(rRankValue2 = 0;rRankValue2 < iInfoCnt;rRankValue2++)
		{
			ContributeInfo &kContribute = vInfo[rRankValue2];
			if( kContribute.m_szName == g_MyInfo.GetPublicID() )
				break;
		}
	}

	vInfo.clear();	

	//데미지 & 힐
	for(i = 0;i < iCharCnt;i++)
	{
		MonsterSurvivalRecord *pRecord = FindRecordCharName( kCharReocrdNameList[i] );
		if( !pRecord ) continue;
		if( !pRecord->pChar ) continue;
		if( pRecord->pChar->GetState() == CS_OBSERVER ) continue;
		if( g_BattleRoomMgr.IsObserverUser(pRecord->pChar->GetCharName()) ) continue;

		ContributeInfo Contribute;
		Contribute.m_iRecordArray = i;
		Contribute.m_iContributePoint = pRecord->Contribute.m_iDamage;
		Contribute.m_iGradeLevel      = pRecord->pChar->GetLevel();
		Contribute.m_szName           = pRecord->szName;
		vInfo.push_back( Contribute );
	}
	std::sort( vInfo.begin(), vInfo.end(), ContributeRankSort() );
	iInfoCnt = vInfo.size();

	if( !bObserver )
	{
		for(rRankValue3 = 0;rRankValue3 < iInfoCnt;rRankValue3++)
		{
			ContributeInfo &kContribute = vInfo[rRankValue3];
			if( kContribute.m_szName == g_MyInfo.GetPublicID() )
				break;
		}
	}

	vInfo.clear();	
	kCharReocrdNameList.clear();
	return iInfoCnt;
}

void ioMonsterSurvivalMode::ContributeRoundAllPercent( DWORD dwCurTime, DWORD dwTotalTime )
{
	ioHashStringVec kCharReocrdNameList;
	FindBaseCharRecordNameList( kCharReocrdNameList );

	//기여도의 100분율
	int iMaxContribute	= 0;
	int iCharCnt		= kCharReocrdNameList.size();
	int iPlayCharCnt	= 0;

	int i = 0;
	for(i = 0;i < iCharCnt;i++)
	{
		MonsterSurvivalRecord *pRecord = FindRecordCharName( kCharReocrdNameList[i] );
		if( !pRecord ) continue;
		if( !pRecord->pChar ) continue;
		if( pRecord->pChar->GetState() == CS_OBSERVER ) continue;
		if( g_BattleRoomMgr.IsObserverUser(pRecord->pChar->GetCharName()) ) continue;
		
		iMaxContribute += pRecord->Contribute.m_iContribute;
		iPlayCharCnt++;
	}

	int iOwnerContribute = -1;
	if( iMaxContribute > 0 )
	{
		for(i = 0;i < iCharCnt;i++)
		{
			MonsterSurvivalRecord *pRecord = FindRecordCharName( kCharReocrdNameList[i] );
			if( !pRecord ) continue;
			if( !pRecord->pChar ) continue;
			if( pRecord->pChar->GetState() == CS_OBSERVER ) continue;
			if( g_BattleRoomMgr.IsObserverUser(pRecord->pChar->GetCharName()) ) continue;
			
			float fContributePer = (float)iPlayCharCnt * ((float)pRecord->Contribute.m_iContribute / iMaxContribute );
			pRecord->Contribute.m_iPrevContributePer = ( fContributePer + 0.001f ) * FLOAT100;

			if( pRecord->pChar->IsOwnerChar() )
				iOwnerContribute = pRecord->Contribute.m_iPrevContributePer;

			g_AwardMgr.AddContribute( pRecord->szName, pRecord->Contribute.m_iPrevContributePer );
		}
	}

	if( m_ModeState == MS_PLAY && iOwnerContribute >= 0 )
	{
		int iPraise = g_MyInfo.CheckContributePraise( iOwnerContribute );
		if( iPraise > 0 )
		{
			/*
			ioBaseChar *pOwner = GetOwnerChar();
			if( pOwner )
				pOwner->SetPraiseEmoticon( ioModeHelpManager::HT_CONTRIBUTE_PRAISE, iPraise );
			*/
			g_ChatMgr.SetSystemMsg( STR(1), iPraise );
			g_ModeHelpMgr.ShowContributePraise( iPraise );
		}
	}

	kCharReocrdNameList.clear();
}

void ioMonsterSurvivalMode::DamageContribute( const ioHashString szAttacker, const ioHashString szTarget, float fDamage, DWORD dwItemCode, int iWeaponType )
{
	if( szAttacker.IsEmpty() ) return;

	MonsterSurvivalRecord *pRecord = FindRecordCharName( szAttacker );
	if( !pRecord || !pRecord->pChar ) return;
	
	ioBaseChar *pAttacker = m_pCreator->GetBaseChar( szAttacker );
	if( !pAttacker ) return;

	ioBaseChar *pTarget   = m_pCreator->GetBaseChar( szTarget );
	if( !pTarget ) return;

	// 자신에 의한 타격 데미지 제외
	if( pAttacker->GetCharName() == pTarget->GetCharName() ) return;

	float fBonusDamage = 0;
	switch( iWeaponType )
	{
	case ioWeapon::WT_BLIZZARD:
		fBonusDamage = fDamage;
		break;
	}

	float fAddDamagePoint = FLOAT1;
//	if( IsContributeAddPointItemCode( dwItemCode ) )
//		fAddDamagePoint = 1.1f;
    
	float fDamagePoint = (float)((fDamage + fBonusDamage) * 100);
	pRecord->Contribute.m_iDamage += (float)fDamagePoint * fAddDamagePoint; 	
	
	g_AwardMgr.AddDamage( pRecord->szName, fDamagePoint, fDamage );

	if( fDamagePoint > 0.0f && !pAttacker->IsOwnerChar() )
	{
		CheckNonePlayTime( pAttacker );
	}

	ContributeConvert();
	ContributeRoundAllPercent();
	UpdateRoundInfoList();
	CheckTotalChamp();
}

void ioMonsterSurvivalMode::KillContribute( const ioHashString szKiller, const ioHashString szKilled, DWORD dwItemCode )
{
	if( szKilled.IsEmpty() ) return;

	int iKillPoint = 100;

	float fAddKillPoint = FLOAT1;
	if( IsContributeAddPointItemCode( dwItemCode ) )
		fAddKillPoint = 1.1f;

	if( szKiller.IsEmpty() || szKiller == szKilled )     //자살
	{
		MonsterSurvivalRecord *pRecord = FindRecordCharName( szKilled );
		if( !pRecord || !pRecord->pChar ) return;
		
		pRecord->Contribute.m_iKill -= iKillPoint;
	}
	else
	{
		MonsterSurvivalRecord *pKillerRecord = FindRecordCharName( szKiller );
		MonsterSurvivalRecord *pKilledRecord = FindRecordCharName( szKilled );
		if( pKillerRecord && pKilledRecord )
		{
			if( pKillerRecord->pChar && pKilledRecord->pChar )
			{
				if( pKillerRecord->pChar->GetTeam() == pKilledRecord->pChar->GetTeam() )    //팀킬
				{
					pKillerRecord->Contribute.m_iKill -= (float)iKillPoint * fAddKillPoint;
					g_AwardMgr.AddTeamKill( pKillerRecord->szName );
				}
				else
					pKillerRecord->Contribute.m_iKill += (float)iKillPoint * fAddKillPoint;
			}
		}
	}

	ContributeConvert();
	ContributeRoundAllPercent();
	UpdateRoundInfoList();
	CheckTotalChamp();
}

void ioMonsterSurvivalMode::AttackCntContribute( const ioHashString szAttacker, const ioHashString szTarget, DWORD dwItemCode, int iAttackValue/* = 100 */ )
{
	if( szAttacker.IsEmpty() || szTarget.IsEmpty() ) return;

	MonsterSurvivalRecord *pAttackerRecord = FindRecordCharName( szAttacker );
	MonsterSurvivalRecord *pTargetRecord = FindRecordCharName( szTarget );

	ioBaseChar *pAttack = m_pCreator->GetBaseChar( szAttacker );
	ioBaseChar *pTarget = m_pCreator->GetBaseChar( szTarget );
	if( !pAttack || !pTarget )
		return;

	if( pAttack != pTarget )
		CheckNonePlayTime( pAttack );

	//팀 타격 제외 해서 팀버프가 안되었음-_-;
	//임시 수정
	if( iAttackValue != 50 )
	{
		if( pAttack->GetTeam() == pTarget->GetTeam() )
			return;
	}	

	float fAddAttackPoint = FLOAT1;
	if( IsContributeAddPointItemCode( dwItemCode ) )
		fAddAttackPoint = 1.1f;

	if( pAttackerRecord && pTargetRecord )
	{
		if( pAttackerRecord->pChar && pTargetRecord->pChar )
		{
			pAttackerRecord->Contribute.m_iAttackCnt += (float)iAttackValue * fAddAttackPoint;
		}
	}

	ContributeConvert();
	ContributeRoundAllPercent();
	UpdateRoundInfoList();
	CheckTotalChamp();
}

void ioMonsterSurvivalMode::FillLastPlayRecordInfo( SP2Packet &rkPacket )
{
	ioHashStringVec kCharReocrdNameList;
	FindBaseCharRecordNameList( kCharReocrdNameList );
	int iCharCnt = kCharReocrdNameList.size();
	rkPacket << iCharCnt;
	for(int i = 0;i < iCharCnt;i++)
	{
		MonsterSurvivalRecord *pRecord = FindRecordCharName( kCharReocrdNameList[i] );
		rkPacket << pRecord->pChar->GetCharName();
		rkPacket << pRecord->Contribute.m_iContribute;		
		rkPacket << pRecord->GetUniqueTotalKill();
		rkPacket << pRecord->GetUniqueTotalDeath();
		rkPacket << g_VictoriesMgr.GetVictories( pRecord->pChar->GetCharName() );

#if defined( USE_GA )
		if( g_MyInfo.GetPublicID() == pRecord->pChar->GetCharName() )
		{
			g_HttpMng.SetKill( pRecord->GetUniqueTotalKill() );
			g_HttpMng.SetDeath( pRecord->GetUniqueTotalDeath() );
		}
#endif
	}
	kCharReocrdNameList.clear();
}

void ioMonsterSurvivalMode::FillPlayRecordInfo( SP2Packet &rkPacket )
{
	ioHashStringVec kCharReocrdNameList;
	FindBaseCharRecordNameList( kCharReocrdNameList );
	int iCharCnt = kCharReocrdNameList.size();
	rkPacket << iCharCnt;
	for(int i = 0;i < iCharCnt;i++)
	{
		MonsterSurvivalRecord *pRecord = FindRecordCharName( kCharReocrdNameList[i] );
		rkPacket << pRecord->pChar->GetCharName();
		rkPacket << pRecord->Contribute.m_iDamage;
		rkPacket << pRecord->Contribute.m_iAttackCnt;
		rkPacket << pRecord->Contribute.m_iKill;
		rkPacket << pRecord->Contribute.m_iDeath;
		rkPacket << pRecord->Contribute.m_iPrevContributePer;
		rkPacket << pRecord->GetUniqueTotalKill();
		rkPacket << pRecord->GetUniqueTotalDeath();
		rkPacket << g_VictoriesMgr.GetVictories( pRecord->pChar->GetCharName() );
	}
	kCharReocrdNameList.clear();
}

void ioMonsterSurvivalMode::ApplyPlayRecordInfo( SP2Packet &rkPacket )
{
	int iCharCnt;
	rkPacket >> iCharCnt;
	for(int i = 0;i < iCharCnt;i++)
	{
		ioHashString szName;
		int iDamage, iAttackCnt, iKill, iDeath, iPrevContributePer, iUniqueTotalKill, iUniqueTotalDeath, iVictories;
		rkPacket >> szName >> iDamage >> iAttackCnt >> iKill >> iDeath >> iPrevContributePer >> iUniqueTotalKill >> iUniqueTotalDeath >> iVictories;

		MonsterSurvivalRecord *pRecord = FindRecordCharName( szName );
		if( pRecord )
		{
			pRecord->Contribute.m_iDamage			= iDamage;
			pRecord->Contribute.m_iAttackCnt		= iAttackCnt;
			pRecord->Contribute.m_iKill             = iKill;
			pRecord->Contribute.m_iDeath            = iDeath;
			pRecord->Contribute.m_iPrevContributePer= iPrevContributePer;
			pRecord->iUniqueTotalKill = iUniqueTotalKill;
			pRecord->iUniqueTotalDeath= iUniqueTotalDeath;

			if( !g_LadderTeamMgr.IsLadderTeam() )
				g_VictoriesMgr.SetVictories( szName, iVictories );
		}
	}
	
	ContributeConvert();
	ContributeRoundAllPercent();
	UpdateRoundInfoList();
	CheckTotalChamp();
}

void ioMonsterSurvivalMode::ContributeResultSync( const ioHashString &szName, int iContribute )
{
	MonsterSurvivalRecord *pRecord = FindRecordCharName( szName );
	if( !pRecord ) return;

	pRecord->Contribute.m_bLastResultSync = true;
	pRecord->Contribute.m_iContribute     = iContribute;
}

void ioMonsterSurvivalMode::SetTurnSceneEventUI( bool bEventStart )
{
	if( bEventStart )
	{
		// 영화 모드 막기
		m_pCreator->SetCameraModeFixedBlock( true );

		m_pCreator->SetScreenBlindDownState();		

		SetBlindMode( true );
	}
	else
	{
		// 영화 모드 막기 해제
		m_pCreator->SetCameraModeFixedBlock( false );

		SetBlindMode( false );
	}
}

DWORD ioMonsterSurvivalMode::StartTurnCameraEventTime()
{
	if( m_dwAllCurTurn == 0 ) return 0;     // 첫턴은 이벤트 시간 추가하지 않음 

	if( COMPARE( m_dwCurrentTurnHelpID, 0, (DWORD)m_TurnHelpList.size() ) )
	{
		DWORD dwEventScene = m_TurnHelpList[m_dwCurrentTurnHelpID].m_dwEventSceneIndex;
		if( dwEventScene != 0 )
		{
			DWORD dwEventSceneTime = g_MapEventSceneMgr.GetMapEventSceneTime( dwEventScene );
			if( dwEventSceneTime == 0 )
				dwEventSceneTime = m_dwLoadEventDuration;
			return dwEventSceneTime;
		}
	}
	return 0;
}

void ioMonsterSurvivalMode::StartMonsterData( SP2Packet &rkPacket, DWORD dwCameraEventTime, ioHashString &rFirstMonsterName )
{	
	DWORD dwFirstStartTime = 0xFFFFFFFF;
	int iMaxNpc;
	rkPacket >> iMaxNpc;
	for(int i = 0;i < iMaxNpc;i++)
	{
		DWORD dwNpcCode, dwStartTime;
		float fStartXPos, fStartZPos;
		ioHashString kNpcName, kSyncName;
		rkPacket >> dwNpcCode >> kNpcName >> kSyncName >> dwStartTime >> fStartXPos >> fStartZPos;

		ioNpcChar *pNpcChar = g_NpcMgr.CreateNpcChar( NPC_NOT_INDEX, dwNpcCode, m_pCreator, this );
		if( !pNpcChar ) continue;		
		
		dwStartTime += dwCameraEventTime;
		pNpcChar->FillMaxHP();
		pNpcChar->SetCharName( kNpcName );
		pNpcChar->SetSyncUser( kSyncName );		
		pNpcChar->SetTeam( TEAM_RED );
		pNpcChar->StartAI( dwStartTime , fStartXPos, fStartZPos );
		pNpcChar->SetGradeUpRandomSeed( g_BattleRoomMgr.GetIndex() + kNpcName.GetHashCode() );
		pNpcChar->ReSetGradeLevel( 0 );
		AddNewRecord( pNpcChar );	

		if( dwStartTime < dwFirstStartTime )
		{
			dwFirstStartTime  = dwStartTime;
			rFirstMonsterName = kNpcName;
		}

		// 보스 게이지 세팅
		if( pNpcChar->IsMonsterHalfBoss() )
		{
			BossMonsterGaugeWnd *pBossMonsterGaugeWnd = dynamic_cast< BossMonsterGaugeWnd * >( g_GUIMgr.FindWnd( MONSTER_BOSS_GAUGE_WND ) );
			if( pBossMonsterGaugeWnd )
			{
				pBossMonsterGaugeWnd->ShowWnd();
				pBossMonsterGaugeWnd->SetBossMonster( BossMonsterGaugeWnd::GAUGE_HALF_BOSS, pNpcChar->GetCharName(), 
													  pNpcChar->GetMonsterIcon(), pNpcChar->GetMonsterMent() );
			}
		}
		else if( pNpcChar->IsMonsterLastBoss() )
		{
			BossMonsterGaugeWnd *pBossMonsterGaugeWnd = dynamic_cast< BossMonsterGaugeWnd * >( g_GUIMgr.FindWnd( MONSTER_BOSS_GAUGE_WND ) );
			if( pBossMonsterGaugeWnd )
			{
				pBossMonsterGaugeWnd->ShowWnd();
				pBossMonsterGaugeWnd->SetBossMonster( BossMonsterGaugeWnd::GAUGE_LAST_BOSS, pNpcChar->GetCharName(), 
													  pNpcChar->GetMonsterIcon(), pNpcChar->GetMonsterMent() );
			}
		}

		// 턴 밸런스
		if( COMPARE( m_dwCurrentHighTurn, 0, (DWORD)m_HighTurnList.size() ) )
		{
			// 스턱 정보 적용
			HighTurnData &rkTurnData = m_HighTurnList[m_dwCurrentHighTurn];
			pNpcChar->SetStuckInfo( min( rkTurnData.m_iStuckActiveRandMax, rkTurnData.m_iStuckActiveRand + ( (int)m_dwCurrentLowTurn * rkTurnData.m_iStuckActiveRandAdd ) ),
							        rkTurnData.m_iStuckActiveSeed, rkTurnData.m_dwStuckActiveTime, rkTurnData.m_StuckBuffList );
			
			for(int j = 0;j < (int)rkTurnData.m_StuckExDataList.size();j++)
			{
				StuckExData &rkExData = rkTurnData.m_StuckExDataList[j];
				pNpcChar->SetStuckExList( rkExData.m_szWeaponType, 
									      min( rkExData.m_iStuckActiveRandMax, rkExData.m_iStuckActiveRand + ( (int)m_dwCurrentLowTurn * rkExData.m_iStuckActiveRandAdd ) ) );
			}

			// ArmorClass And SpeedClass.
			TurnBalance kBalance = rkTurnData.m_NormalBalance;
			if( IsMonsterBossTurn() )
				kBalance = rkTurnData.m_BossBalance;
			pNpcChar->AddItemBalance( kBalance.m_fAddDefaultArmorClass, kBalance.m_fAddDefaultSpeedClass );
			pNpcChar->SetAddCharArmorClass( kBalance.m_fAddArmorClass * (int)m_dwCurrentLowTurn, kBalance.m_fMaxArmorClass );
			pNpcChar->SetAddCharSpeedClass( -( kBalance.m_fAddSpeedClass * (int)m_dwCurrentLowTurn ), kBalance.m_fMaxSpeedClass );
		}
	}
}

void ioMonsterSurvivalMode::StartTurnViewCharStart()
{
	// 유저 등장
	ioHashStringVec kCharReocrdNameList;
	FindBaseCharRecordNameList( kCharReocrdNameList );
	int iCharCnt = kCharReocrdNameList.size();
	for(int i = 0;i < iCharCnt;i++)
	{
		MonsterSurvivalRecord *pRecord = FindRecordCharName( kCharReocrdNameList[i] );
		if( pRecord == NULL ) continue;
		if( pRecord->pChar == NULL ) continue;
		if( pRecord->pChar->GetTeam() != TEAM_BLUE ) continue;
		if( pRecord->pChar->GetState() != CS_VIEW ) continue;

		D3DXVECTOR3 vStartPos = GetRevivalPos( pRecord->pChar );
		pRecord->pChar->SetWorldPosition( vStartPos );
		pRecord->pChar->SetCreateMotionState();
		pRecord->pChar->SetProtectionMode( PM_REVIVAL, 2000 );

		// HP 회복
		StatusValue &rkStatus = pRecord->pChar->GetHP();
		rkStatus.m_fCurValue = max( 0.0f, min( rkStatus.m_fCurValue + (rkStatus.m_fMaxValue * m_fRevivalRecoveryHP), rkStatus.m_fMaxValue ) );

		// 턴 밸런스
		if( COMPARE( m_dwCurrentHighTurn, 0, (DWORD)m_HighTurnList.size() ) )
		{
			// ArmorClass And SpeedClass.
			HighTurnData &rkTurnData = m_HighTurnList[m_dwCurrentHighTurn];

			TurnBalance kBalance = rkTurnData.m_NormalBalance;
			pRecord->pChar->SetAddCharArmorClass( -( kBalance.m_fAddArmorClass * (int)m_dwCurrentLowTurn ), kBalance.m_fMaxArmorClass );
		}

		if( pRecord->pChar->IsOwnerChar() )
			m_pCreator->NotifyOwnerCameraLoseTime( 0 );
	}
	kCharReocrdNameList.clear();
	SetTurnSceneEventUI( false );
}

void ioMonsterSurvivalMode::StartTurnViewCharPos()
{
	ioHashStringVec kCharReocrdNameList;
	FindBaseCharRecordNameList( kCharReocrdNameList );
	int iCharCnt = kCharReocrdNameList.size();
	for(int i = 0;i < iCharCnt;i++)
	{
		MonsterSurvivalRecord *pRecord = FindRecordCharName( kCharReocrdNameList[i] );
		if( pRecord == NULL ) continue;
		if( pRecord->pChar == NULL ) continue;
		if( pRecord->pChar->GetTeam() != TEAM_BLUE ) continue;
		if( pRecord->pChar->GetState() != CS_VIEW ) continue;

		D3DXVECTOR3 vStartPos = GetRevivalPos( pRecord->pChar );
		pRecord->pChar->SetWorldPosition( vStartPos );
	}
	kCharReocrdNameList.clear();
}

void ioMonsterSurvivalMode::StartTurnCharHPRecovery()
{
	ioHashStringVec kCharReocrdNameList;
	FindBaseCharRecordNameList( kCharReocrdNameList );
	int iCharCnt = kCharReocrdNameList.size();
	for(int i = 0;i < iCharCnt;i++)
	{
		MonsterSurvivalRecord *pRecord = FindRecordCharName( kCharReocrdNameList[i] );
		if( pRecord == NULL ) continue;
		if( pRecord->pChar == NULL ) continue;
		if( pRecord->pChar->GetTeam() != TEAM_BLUE ) continue;
		if( pRecord->pChar->GetState() == CS_OBSERVER ) continue;
		
		pRecord->pChar->FillMaxHP();
	}
	kCharReocrdNameList.clear();
}

void ioMonsterSurvivalMode::ReduceNpcCreateTime()
{
	int iCharCnt = m_vCharRecordList.size();
	for(int i = 0;i < iCharCnt;i++)
	{
		MonsterSurvivalRecord *pRecord = dynamic_cast<MonsterSurvivalRecord *>( FindModeRecord( i ) );
		if( pRecord == NULL ) continue;

		ioNpcChar *pNpcChar = ToNpcChar( pRecord->pChar );
		if( pNpcChar == NULL ) continue;

		pNpcChar->StartReduceTime( m_dwReduceNpcCreateTime );
	}
}

bool ioMonsterSurvivalMode::IsAllUserDie()
{
	ioHashStringVec kCharReocrdNameList;
	FindBaseCharRecordNameList( kCharReocrdNameList );
	int iCharCnt = kCharReocrdNameList.size();
	for(int i = 0;i < iCharCnt;i++)
	{
		MonsterSurvivalRecord *pRecord = FindRecordCharName( kCharReocrdNameList[i] );
		if( pRecord == NULL ) continue;
		if( pRecord->pChar == NULL ) continue;
		if( pRecord->pChar->GetTeam() != TEAM_BLUE ) continue;
		if( pRecord->pChar->GetState() == CS_OBSERVER ) continue;
		
		if( pRecord->pChar->GetState() != CS_DIE && !pRecord->pChar->IsPrisonerMode() )
			return false;
	}
	kCharReocrdNameList.clear();
	return true;
}

void ioMonsterSurvivalMode::OnZeroHP( SP2Packet &rkPacket )
{
	rkPacket >> m_dwSuddenDeathTime;
}

void ioMonsterSurvivalMode::OnTurnStart( SP2Packet &rkPacket )
{
	g_GUIMgr.HideWnd( MONSTER_BOSS_GAUGE_WND );

	DWORD dwPrevHighTurn;
	dwPrevHighTurn = m_dwCurrentHighTurn;

	if( m_bBossTurn )
	{
		// 보스 턴 다음 턴은 HP 풀로 회복
		StartTurnCharHPRecovery();
	}
	
	rkPacket >> m_bBossTurn >> m_dwAllCurTurn >> m_dwAllMaxTurn 
			 >> m_dwCurrentTurnHelpID >> m_dwCurRoundDuration 
			 >> m_dwCurrentHighTurn >> m_dwCurrentLowTurn >> m_dwReduceNpcCreateTime;
	
	m_StartPosRandom.SetRandomSeed( g_BattleRoomMgr.GetIndex() + m_dwAllCurTurn );
	m_RevivalPosRandom.SetRandomSeed( g_BattleRoomMgr.GetIndex() + m_dwAllCurTurn );

	m_dwPlayStartTime = FRAMEGETTIME();
	m_dwRoundDuration = m_dwCurRoundDuration;
	m_dwCheckWarningTime = 0;
	m_CurrentMagicZone.Init();
	UIZoneCollisionTime( 0 );

	// 데스몬 죽음!!
	int i = 0;
	int iMaxNPC;
	rkPacket >> iMaxNPC;
	for(i = 0;i < iMaxNPC;i++)
	{
		ioHashString kNpcName;
		rkPacket >> kNpcName;
		MonsterSurvivalRecord *pRecord = FindRecordCharName( kNpcName );
		if( pRecord == NULL ) continue;

		ioNpcChar *pNpcChar = ToNpcChar( pRecord->pChar );
		if( pNpcChar )
		{
			pNpcChar->StopNPC();
			AddEraseNpc( pNpcChar->GetCharName() );
		}
	}

	// 턴 몬스터
	ioHashString szFirstMonsterName;
	StartMonsterData( rkPacket, StartTurnCameraEventTime(), szFirstMonsterName );	

	TimeScoreWnd *pTimeScoreWnd = dynamic_cast<TimeScoreWnd *>( g_GUIMgr.FindWnd( TIME_SCORE_WND ) );
	if( pTimeScoreWnd )
	{
		pTimeScoreWnd->InitAniState();
		pTimeScoreWnd->UpdateTime( m_dwCurRoundDuration, m_dwRoundDuration );

		// 사운드
		if( COMPARE( m_dwCurrentHighTurn, 0, (int)m_HighTurnList.size() ) )
		{
			HighTurnData &rkTurnData = m_HighTurnList[m_dwCurrentHighTurn];
			if( dwPrevHighTurn != m_dwCurrentHighTurn )
			{				
				if( !rkTurnData.m_szHighTurnStartSnd.IsEmpty() )
					g_SoundMgr.PlaySound( rkTurnData.m_szHighTurnStartSnd );
			}
			else if( !rkTurnData.m_szLowTurnStartSnd.IsEmpty() )
			{
				g_SoundMgr.PlaySound( rkTurnData.m_szLowTurnStartSnd );
			}
		}
    }

	// 첫 턴
	if( m_dwAllCurTurn == 0 )
	{
		if( m_iUseMonsterCoinCnt > 0 )        // 코인 사용하는 난이도
		{
			m_dwCurStartCoinTime = FRAMEGETTIME();
			m_dwCurStartCoinSec  = 0xFFFFFFFF;
		}
		MonsterEventMent( szFirstMonsterName );

		// 첫턴 시작
		m_dwStartTurnTime = FRAMEGETTIME();

	}	// 두번째 턴부터는 이벤트 작동
	else if( COMPARE( m_dwCurrentTurnHelpID, 0, (DWORD)m_TurnHelpList.size() ) )
	{
		TurnHelpList &rkTurnHelp = m_TurnHelpList[m_dwCurrentTurnHelpID];
		SetTurnSceneEventUI( true );
		if( rkTurnHelp.m_dwEventSceneIndex != 0 )
		{		
			m_pCreator->StartTurnStartEventCamera( rkTurnHelp.m_dwEventSceneIndex, rkTurnHelp.m_dwMonsterSceneIndex, szFirstMonsterName );
		}

		if( rkTurnHelp.m_dwEventSceneEffect != 0 )
		{
			m_pCreator->SetMapCustomEffect( true, rkTurnHelp.m_dwEventSceneEffect );
		}

		// 몬스터 카메라 이벤트가 없으면 즉시 나옴
		if( rkTurnHelp.m_dwMonsterSceneIndex == 0 )
		{
			StartTurnViewCharStart();
		}
	}	

	// 커스텀 오브젝트 삭제
	m_pCreator->SetMapCustomObject( false, -1 );

	// 필드 소환물 삭제
	g_PushStructListMgr.DestroyStructList();

	// 필드 아이템 삭제
	m_pCreator->HidingAllFieldItem();
	m_pCreator->DestroyWeaponList();

	// 모든 몬스터 제거
	for(i = 0;i < (int)m_NpcEraseList.size();i++)
	{
		m_pCreator->RemoveUser( m_NpcEraseList[i].m_szEraseNpc, true );
	}
	m_NpcEraseList.clear();
}

void ioMonsterSurvivalMode::OnTurnEnd( SP2Packet &rkPacket )
{
#ifndef SHIPPING
	LOG.PrintTimeAndLog(0, "OnTurnEnd");
#endif

	bool bTurnMonsterAllDie;
	rkPacket >> bTurnMonsterAllDie;

	int i = 0;
	// 턴 종료 처리
	if( bTurnMonsterAllDie )
	{		
		// 마법진 생성
		m_pCreator->SetMapCustomObject( true, -1 );

		// 화면 블라인드
		m_pCreator->StartScreenBlind( SBT_FIXED_BLIND, NULL, 0 );

		// 골드 코인 사용 불가
		if( IsRenderGoldMonsterCoinHelp() )
		{
			g_GUIMgr.HideWnd( MONSTER_COIN_USE_WND );
		}

		// 마법진 위치 
		m_CurrentMagicZone.Init();
		UIZoneCollisionTime( 0 );
		if( COMPARE( m_dwCurrentTurnHelpID, 0, (DWORD)m_TurnHelpList.size() ) )
		{
			TurnHelpList &rkTurnHelp = m_TurnHelpList[m_dwCurrentTurnHelpID];
			if( COMPARE( rkTurnHelp.m_dwZoneIndex, 0, (DWORD)m_MagicZoneList.size() ) )
			{
				m_CurrentMagicZone = m_MagicZoneList[rkTurnHelp.m_dwZoneIndex];
				m_CurrentMagicZone.m_dwCurCollisionTime = FRAMEGETTIME();
				m_CurrentMagicZone.m_dwCurCollisionTimeEx = FRAMEGETTIME();
				if( !m_MagicZoneList[rkTurnHelp.m_dwZoneIndex].m_szStartSound.IsEmpty() )
					g_SoundMgr.PlaySound( m_MagicZoneList[rkTurnHelp.m_dwZoneIndex].m_szStartSound );
				UIZoneCollisionTime( m_CurrentMagicZone.m_dwCollisionTime );
			}

			if( rkTurnHelp.m_dwEventSceneEffect != 0 )
			{
				m_pCreator->SetMapCustomEffect( false, rkTurnHelp.m_dwEventSceneEffect );
			}
		}

		// 예외처리 : 마법진이 없다?
		if( m_CurrentMagicZone.m_dwCollisionTime == 0 )
		{
			ioHashStringVec kCharReocrdNameList;
			FindBaseCharRecordNameList( kCharReocrdNameList );
			int iCharCnt = kCharReocrdNameList.size();
			for(i = 0;i < iCharCnt;i++)
			{
				MonsterSurvivalRecord *pRecord = FindRecordCharName( kCharReocrdNameList[i] );
				if( pRecord == NULL ) continue;
				if( pRecord->pChar == NULL ) continue;
				if( pRecord->pChar->GetTeam() != TEAM_BLUE ) continue;
				if( pRecord->pChar->GetState() == CS_VIEW  ||
					pRecord->pChar->GetState() == CS_DIE   ||
					pRecord->pChar->GetState() == CS_OBSERVER ||
					pRecord->pChar->GetState() == CS_LOADING )
				{
					continue;
				}
				pRecord->pChar->AddNewBuff( "000_Ascension_Event", pRecord->pChar->GetCharName(), "", NULL );
			}
			kCharReocrdNameList.clear();
		}		
		
		// 플레이중인 유저만 체크
		if( !g_BattleRoomMgr.IsObserverUser( g_MyInfo.GetPublicID() ) && !m_pCreator->IsStealthMode() )
		{
			g_QuestMgr.QuestCompleteTerm( QC_PVE_ROUND_CLEAR, GetDungeonFloor(), g_BattleRoomMgr.GetModeStartPlayerCount(), (float)(FRAMEGETTIME() - m_dwStartTurnTime)/1000 );
			g_QuestMgr.QuestOccurTerm( QO_PVE_ROUND_CLEAR, GetDungeonFloor() );
		}
	}
	else
	{
		// 데스 타임이 되었는데 안나온 몬스터가 있다!!!
		RecordList::iterator iter;
		for( iter=m_vCharRecordList.begin() ; iter!=m_vCharRecordList.end() ; ++iter )
		{
			ioNpcChar *pNpcChar = ToNpcChar( iter->pChar );
			if( pNpcChar )
			{
				pNpcChar->ExceptionStartAI();	
			}
		}
	}

	// 데스몬 출격!!
	int iMaxNPC;
	rkPacket >> iMaxNPC;
	for(i = 0;i < iMaxNPC;i++)
	{
		DWORD dwNpcCode, dwStartTime;
		ioHashString kNpcName, kSyncName;
		float fStartXPos, fStartZPos;
		rkPacket >> dwNpcCode >> kNpcName >> kSyncName >> dwStartTime >> fStartXPos >> fStartZPos;

		ioNpcChar *pNpcChar = NULL;
		MonsterSurvivalRecord *pRecord = FindRecordCharName( kNpcName );
		if( pRecord && ToNpcChar( pRecord->pChar ) )
		{
			pNpcChar = ToNpcChar( pRecord->pChar );
			if( !pNpcChar ) continue;

			pNpcChar->Revival();
			pNpcChar->SetState( CS_LOADING );
			pNpcChar->FillMaxHP();
			pNpcChar->SetCharName( kNpcName );
			pNpcChar->SetSyncUser( kSyncName );		
			pNpcChar->SetTeam( TEAM_RED );
			pNpcChar->StartAI( dwStartTime, fStartXPos, fStartZPos );
//			pNpcChar->FixedOutLine();
			pNpcChar->SetDeathMonster( true );
		}
		else
		{
			pNpcChar = g_NpcMgr.CreateNpcChar( NPC_NOT_INDEX, dwNpcCode, m_pCreator, this );
			if( !pNpcChar ) continue;	

			pNpcChar->FillMaxHP();
			pNpcChar->SetCharName( kNpcName );
			pNpcChar->SetSyncUser( kSyncName );		
			pNpcChar->SetTeam( TEAM_RED );
			pNpcChar->StartAI( dwStartTime, fStartXPos, fStartZPos );
//			pNpcChar->FixedOutLine();
			pNpcChar->SetDeathMonster( true );
			AddNewRecord( pNpcChar );	
		}		
	}

	// 데스몬 출격 헬프~
	if( iMaxNPC > 0 ) 
	{
		if( !m_TurnDeathHelp.m_HelpData.empty() )
		{
			ioComplexStringPrinter kPrinter;
			kPrinter.SetTextStyle( TS_OUTLINE_FULL_2X );
			kPrinter.SetBkColor( 0, 0, 0 );
			for(i = 0;i < (int)m_TurnDeathHelp.m_HelpData.size();i++)
			{
				TurnHelp &rkData = m_TurnDeathHelp.m_HelpData[i];
				kPrinter.SetTextColor( rkData.m_dwColor );	
				kPrinter.AddTextPiece( rkData.m_fFontScale, rkData.m_szText.c_str() );
			}
			SetCenterHelp( kPrinter, 0, m_TurnDeathHelp.m_dwShowTime );		
		}

		if( !m_TurnDeathHelp.m_szPlaySound.IsEmpty() )
			g_SoundMgr.PlaySound( m_TurnDeathHelp.m_szPlaySound );

		// 몬스터가 살아있는데 사신이 등장한다면 시간이 종료된 상황이므로 헬프 표시
		if( !bTurnMonsterAllDie )
		{
			UIDeathTime( FRAMEGETTIME() );
			if( !IsAllUserDie() && m_CurrentMagicZone.m_dwCollisionTime == 0 )
				m_pCreator->StartScreenBlind( SBT_KILL, NULL, true );
		}
	}	

	// 모든 몬스터가 죽었으면 부활 유저 처리
	if( bTurnMonsterAllDie )
	{
		int iRevivalUser;
		rkPacket >> iRevivalUser;
		for(i = 0;i < iRevivalUser;i++)
		{
			ioHashString szRevivalName;
			rkPacket >> szRevivalName;

			MonsterSurvivalRecord *pRecord = FindRecordCharName( szRevivalName );
			if( !pRecord ) return;

			ioBaseChar *pChar = pRecord->pChar;
			if( !pChar ) continue;

			pRecord->dwCurDieTime = 0;

			D3DXVECTOR3 vRevivalPos = GetRevivalPos( pChar );
			pChar->SetWorldPosition( vRevivalPos );
			pChar->DestroyAllItem();
			pChar->Revival();
			pChar->CheckDefaultReinforce();
			pChar->SetPrisonerMode();
			pChar->SetState( CS_DELAY );
			pChar->SetCurHP( 0.0f );	

			if( pChar->IsOwnerChar() )
			{
				g_ModeHelpMgr.HideHelp( ioModeHelpManager::HT_VIEW );
				SetBlindMode( false );
			}
		}
	}
}

void ioMonsterSurvivalMode::OnMonsterSyncChange( SP2Packet &rkPacket )
{
	int iMaxNpc;
	rkPacket >> iMaxNpc;
	for(int i = 0;i < iMaxNpc;i++)
	{
		ioHashString szNpcName, szSyncName;
		rkPacket >> szNpcName >> szSyncName;

		MonsterSurvivalRecord *pRecord = FindRecordCharName( szNpcName );
		if( !pRecord ) continue;

		ioNpcChar *pNpc = ToNpcChar( pRecord->pChar );
		if( pNpc == NULL ) continue;

		pNpc->SetSyncUser( szSyncName );
	}
}

void ioMonsterSurvivalMode::OnMonsterInfoSync( SP2Packet &rkPacket )
{
	// 플레이 정보
	rkPacket >> m_bBossTurn >> m_dwAllCurTurn >> m_dwAllMaxTurn >> m_dwCurrentTurnHelpID >> m_dwCurrentHighTurn >> m_dwCurrentLowTurn >> m_dwReduceNpcCreateTime;

	int i, iMaxNpc;
	// 일반 NPC
	rkPacket >> iMaxNpc;

	if( iMaxNpc == 0 )
	{
		// 중간 참여를 했는데 살아있는 몬스터가 없으면 마법진 연다.
		if( m_CurrentMagicZone.m_dwCollisionTime == 0 )
		{
			// 마법진 생성
			m_pCreator->SetMapCustomObject( true, -1 );

			// 화면 블라인드
			m_pCreator->StartScreenBlind( SBT_FIXED_BLIND, NULL, 0 );

			// 마법진 위치 
			m_CurrentMagicZone.Init();
			UIZoneCollisionTime( 0 );
			if( COMPARE( m_dwCurrentTurnHelpID, 0, (DWORD)m_TurnHelpList.size() ) )
			{
				TurnHelpList &rkTurnHelp = m_TurnHelpList[m_dwCurrentTurnHelpID];
				if( COMPARE( rkTurnHelp.m_dwZoneIndex, 0, (DWORD)m_MagicZoneList.size() ) )
				{
					m_CurrentMagicZone = m_MagicZoneList[rkTurnHelp.m_dwZoneIndex];
					m_CurrentMagicZone.m_dwCurCollisionTime = FRAMEGETTIME() - m_CurrentMagicZone.m_dwCollisionTime;
					m_CurrentMagicZone.m_dwCurCollisionTimeEx = FRAMEGETTIME();
					UIZoneCollisionTime( m_CurrentMagicZone.m_dwCollisionTime );
				}

				if( rkTurnHelp.m_dwEventSceneEffect != 0 )
				{
					m_pCreator->SetMapCustomEffect( false, rkTurnHelp.m_dwEventSceneEffect );
				}
			}
		}
	}
	else
	{
		for(i = 0;i < iMaxNpc;i++)
		{
			DWORD dwNpcCode;
			ioHashString kNpcName, kSyncName;
			int iHighTurnIndex, iLowTurnIndex;
			rkPacket >> dwNpcCode >> kNpcName >> kSyncName >> iHighTurnIndex >> iLowTurnIndex;

			ioNpcChar *pNpcChar = g_NpcMgr.CreateNpcChar( NPC_NOT_INDEX, dwNpcCode, m_pCreator, this );
			if( !pNpcChar ) continue;

			pNpcChar->SetTeam( TEAM_RED );
			pNpcChar->SetCharName( kNpcName );
			pNpcChar->SetSyncUser( kSyncName );
			pNpcChar->SetGradeUpRandomSeed( g_BattleRoomMgr.GetIndex() + kNpcName.GetHashCode() );
			AddNewRecord( pNpcChar );	

			// 보스 게이지 세팅
			if( pNpcChar->IsMonsterHalfBoss() )
			{
				BossMonsterGaugeWnd *pBossMonsterGaugeWnd = dynamic_cast< BossMonsterGaugeWnd * >( g_GUIMgr.FindWnd( MONSTER_BOSS_GAUGE_WND ) );
				if( pBossMonsterGaugeWnd )
				{
					pBossMonsterGaugeWnd->ShowWnd();
					pBossMonsterGaugeWnd->SetBossMonster( BossMonsterGaugeWnd::GAUGE_HALF_BOSS, pNpcChar->GetCharName(), 
														  pNpcChar->GetMonsterIcon(), pNpcChar->GetMonsterMent() );
				}
			}
			else if( pNpcChar->IsMonsterLastBoss() )
			{
				BossMonsterGaugeWnd *pBossMonsterGaugeWnd = dynamic_cast< BossMonsterGaugeWnd * >( g_GUIMgr.FindWnd( MONSTER_BOSS_GAUGE_WND ) );
				if( pBossMonsterGaugeWnd )
				{
					pBossMonsterGaugeWnd->ShowWnd();
					pBossMonsterGaugeWnd->SetBossMonster( BossMonsterGaugeWnd::GAUGE_LAST_BOSS, pNpcChar->GetCharName(), 
														  pNpcChar->GetMonsterIcon(), pNpcChar->GetMonsterMent() );
				}
			}

			// 턴 밸런스
			if( COMPARE( iHighTurnIndex, 0, (int)m_HighTurnList.size() ) )
			{
				HighTurnData &rkTurnData = m_HighTurnList[iHighTurnIndex];
				pNpcChar->SetStuckInfo( min( rkTurnData.m_iStuckActiveRandMax, rkTurnData.m_iStuckActiveRand + ( iLowTurnIndex * rkTurnData.m_iStuckActiveRandAdd ) ),
					rkTurnData.m_iStuckActiveSeed, rkTurnData.m_dwStuckActiveTime, rkTurnData.m_StuckBuffList );

				for(int j = 0;j < (int)rkTurnData.m_StuckExDataList.size();j++)
				{
					StuckExData &rkExData = rkTurnData.m_StuckExDataList[j];
					pNpcChar->SetStuckExList( rkExData.m_szWeaponType, 
						min( rkExData.m_iStuckActiveRandMax, rkExData.m_iStuckActiveRand + ( iLowTurnIndex * rkExData.m_iStuckActiveRandAdd ) ) );
				}

				// ArmorClass And SpeedClass.
				TurnBalance kBalance = rkTurnData.m_NormalBalance;
				if( IsMonsterBossTurn() )
					kBalance = rkTurnData.m_BossBalance;
				pNpcChar->AddItemBalance( kBalance.m_fAddDefaultArmorClass, kBalance.m_fAddDefaultSpeedClass );
				pNpcChar->SetAddCharArmorClass( kBalance.m_fAddArmorClass * iLowTurnIndex, kBalance.m_fMaxArmorClass );
				pNpcChar->SetAddCharSpeedClass( -( kBalance.m_fAddSpeedClass * iLowTurnIndex ), kBalance.m_fMaxSpeedClass );
			}
		}
	}

	// Death NPC
	rkPacket >> iMaxNpc;
	for(i = 0;i < iMaxNpc;i++)
	{
		DWORD dwNpcCode;
		ioHashString kNpcName, kSyncName;
		int iHighTurnIndex;
		rkPacket >> dwNpcCode >> kNpcName >> kSyncName >> iHighTurnIndex;

		ioNpcChar *pNpcChar = g_NpcMgr.CreateNpcChar( NPC_NOT_INDEX, dwNpcCode, m_pCreator, this );
		if( !pNpcChar ) continue;

		pNpcChar->SetTeam( TEAM_RED );
		pNpcChar->SetCharName( kNpcName );
		pNpcChar->SetSyncUser( kSyncName );		
		AddNewRecord( pNpcChar );	
		if( COMPARE( iHighTurnIndex, 0, (int)m_HighTurnList.size() ) )
		{
			HighTurnData &rkTurnData = m_HighTurnList[iHighTurnIndex];
			pNpcChar->SetStuckInfo( 100, 100, 1000, rkTurnData.m_StuckBuffList );
		}
	}
}

void ioMonsterSurvivalMode::OnRoundContinue( SP2Packet &rkPacket )
{
	if( GetModeState() != MS_PLAY )
	{
		LOG.PrintTimeAndLog( 0, "ioMonsterSurvivalMode::OnRoundContinue Fail - %d", (int)GetModeState() );
		return;
	}

	DWORD dwContinueTime;
	rkPacket >> dwContinueTime;
	UIContinueTime( dwContinueTime );	
	if( dwContinueTime != 0 )
	{
		m_pCreator->StartScreenBlind( SBT_FIXED_BLIND, NULL, 0 );
	}
}

void ioMonsterSurvivalMode::OnRoundReady( SP2Packet &rkPacket )
{
	ShowEndScoreWnd( false );
	SetBlindMode( false );

	rkPacket >> m_iCurRoundCount;
	rkPacket >> m_iBluePosArray;
	rkPacket >> m_iRedPosArray;
	rkPacket >> m_dwCurRoundDuration;

	if( m_iCurRoundCount == 1 )
	{
		g_MyInfo.ClearCharDiedInfo();
		g_MyInfo.ClearCharDiedPenaltyInfo();
		g_MyInfo.ClearCharJoinedInfo();
	}

	ClearModeData();
	ReStartMode();

	g_GUIMgr.HideWnd( MIDDLE_JOINDELAY_WND );
}

void ioMonsterSurvivalMode::OnRoundJoin( SP2Packet &rkPacket )
{
	ioHashString szJoinChar;
	int iModeState;

	rkPacket >> szJoinChar;
	rkPacket >> iModeState;

	if( szJoinChar.IsEmpty() ) return;

	ioBaseChar *pJoinChar = m_pCreator->GetBaseChar( szJoinChar );
	if( !pJoinChar ) return;

	DWORD dwStateGap;
	DWORD dwSelectTime;

	if( pJoinChar->IsOwnerChar() )
	{
		SetModeState( (ModeState)iModeState );

		rkPacket >> dwStateGap;

		DWORD dwStartTime = FRAMEGETTIME() - dwStateGap;
		switch( m_ModeState )
		{
		case MS_READY:
			m_dwReadyStartTime = dwStartTime;
			break;
		case MS_PLAY:
			m_dwPlayStartTime = dwStartTime;
			break;
		case MS_RESULT:
			m_dwResultStartTime = dwStartTime;
			break;
		}

		rkPacket >> dwSelectTime;

		if( m_ModeState != MS_RESULT )
		{
			int iSelectChar = 0;
			
			iSelectChar = pJoinChar->GetSelectCharArray();
			pJoinChar->SetReadyState();

			StartChangeCharacter( iSelectChar, dwSelectTime );
		}		
		else
		{
			SetResultJoinState();
		}

		g_ModeHelpMgr.InitModeHelp();

		rkPacket >> m_dwCurRoundDuration;
	}
	else 
	{
		pJoinChar->SetReadyState();

		if( m_ModeState == MS_PLAY )
		{
			ioBaseChar *pOwner = GetOwnerChar();
			DWORD dwCurTime = FRAMEGETTIME();
			if( pOwner )
			{
				if( m_iCurRoundCount != 1 )
					g_ChatMgr.SetJoinUserChatMsg( pJoinChar->GetCharName().c_str(), pJoinChar->GetTeam(), pJoinChar->GetLevel() );
				else if( dwCurTime > m_dwPlayStartTime && dwCurTime - m_dwPlayStartTime >= 10000 )
					g_ChatMgr.SetJoinUserChatMsg( pJoinChar->GetCharName().c_str(), pJoinChar->GetTeam(), pJoinChar->GetLevel() );
			}
		}
	}

	m_pCreator->IncreaseTeamCntForBalance( pJoinChar->GetTeam() );
	m_pCreator->ReCalculateDamageRate( false );
}

void ioMonsterSurvivalMode::OnRoundJoinView( SP2Packet &rkPacket )
{
	ioHashString szJoinChar;
	int iModeState;

	rkPacket >> szJoinChar;
	rkPacket >> iModeState;

	if( szJoinChar.IsEmpty() ) return;

	ioBaseChar *pJoinChar = m_pCreator->GetBaseChar( szJoinChar );
	if( !pJoinChar ) return;

	DWORD dwStateGap;

	if( pJoinChar->IsOwnerChar() )
	{
		SetModeState( (ModeState)iModeState );

		rkPacket >> dwStateGap;

		DWORD dwStartTime = FRAMEGETTIME() - dwStateGap;
		switch( m_ModeState )
		{
		case MS_READY:
			m_dwReadyStartTime = dwStartTime;
			break;
		case MS_PLAY:
			m_dwPlayStartTime = dwStartTime;
			break;
		case MS_RESULT:
			m_dwResultStartTime = dwStartTime;
			break;
		}

		int iSelectChar = 0;

		pJoinChar->SetState( CS_VIEW );

		g_ModeHelpMgr.InitModeHelp();
		g_ModeHelpMgr.ShowHelp( ioModeHelpManager::HT_VIEW );

		SetBlindMode( true );
		g_GUIMgr.SetSkipUIByBlind( false );		

		g_ChatMgr.SetInfomationMsg( STR(1) );
		g_ChatMgr.SetInfomationMsg( STR(2) );

		rkPacket >> m_dwCurRoundDuration;
	}
	else if( m_ModeState == MS_PLAY )
	{
		ioBaseChar *pOwner = GetOwnerChar();
		DWORD dwCurTime = FRAMEGETTIME();
		if( pOwner )
		{
			if( m_iCurRoundCount != 1 )
				g_ChatMgr.SetJoinUserChatMsg( pJoinChar->GetCharName().c_str(), pJoinChar->GetTeam(), pJoinChar->GetLevel() );
			else if( dwCurTime > m_dwPlayStartTime && dwCurTime - m_dwPlayStartTime >= 10000 )
				g_ChatMgr.SetJoinUserChatMsg( pJoinChar->GetCharName().c_str(), pJoinChar->GetTeam(), pJoinChar->GetLevel() );
		}

		pJoinChar->SetState( CS_VIEW );
	}

	m_pCreator->ReCalculateDamageRate( false );
}

void ioMonsterSurvivalMode::OnRoundJoinObserver( SP2Packet &rkPacket )
{
	ioHashString szJoinChar;
	int iModeState;

	rkPacket >> szJoinChar;
	rkPacket >> iModeState;

	if( szJoinChar.IsEmpty() ) return;

	ioBaseChar *pJoinChar = m_pCreator->GetBaseChar( szJoinChar );
	if( !pJoinChar ) return;

	DWORD dwStateGap;

	if( pJoinChar->IsOwnerChar() )
	{
		SetModeState( (ModeState)iModeState );

		rkPacket >> dwStateGap;

		DWORD dwStartTime = FRAMEGETTIME() - dwStateGap;
		switch( m_ModeState )
		{
		case MS_READY:
			m_dwReadyStartTime = dwStartTime;
			break;
		case MS_PLAY:
			m_dwPlayStartTime = dwStartTime;
			break;
		case MS_RESULT:
			m_dwResultStartTime = dwStartTime;
			break;
		}

		int iSelectChar = 0;

		pJoinChar->SetState( CS_OBSERVER );

		g_ModeHelpMgr.InitModeHelp();

		g_ChatMgr.SetInfomationMsg( STR(1) );
		g_ChatMgr.SetInfomationMsg( STR(2) );

		rkPacket >> m_dwCurRoundDuration;

		if( g_BattleRoomMgr.IsBattleRoom() )
		{
			g_QuestMgr.QuestCompleteTerm( QC_ENTER_BATTLE_OBSERVER, GetModeType() );
		}
	}
	else if( m_ModeState == MS_PLAY )
	{
		pJoinChar->SetState( CS_OBSERVER );
	}

	m_pCreator->ReCalculateDamageRate( false );
}

void ioMonsterSurvivalMode::SetResultScoreWnd()
{
	
}

void ioMonsterSurvivalMode::SetTotalScoreWnd( IntVec &rvClassTypeList,
										      IntVec &rvClassPointList,
										      float fBlueGuildTeamBonus,
										      float fRedGuildTeamBonus,
										      float fBonusArray[BA_MAX] )
{
	TotalResultMainWnd *pTotalResult = dynamic_cast<TotalResultMainWnd *>(g_GUIMgr.FindWnd( TOTAL_RESULT_MAIN_WND ));
	if( !pTotalResult ) return;

	if( g_GUIMgr.IsShow( ROUND_RESULT_MAIN_WND ) )
		g_GUIMgr.HideWnd( ROUND_RESULT_MAIN_WND );

	ContributeRoundAllPercent( m_dwRoundDuration, m_dwRoundDuration );

	int i = 0;
	int iRecordCnt = GetRecordCharCnt();
	for( i=0 ; i<iRecordCnt ; i++ )
	{
		FinalInfo kInfo;
		if( GetFinalInfo( i, kInfo ) )
		{
			pTotalResult->AddFinalInfo( kInfo );
			if( g_BattleRoomMgr.IsBattleRoom() )
				g_BattleRoomMgr.UpdateUserLevel( kInfo.m_szName, kInfo.m_iLevel );
			else if( g_LadderTeamMgr.IsLadderTeam() )
				g_LadderTeamMgr.UpdateUserData( kInfo.m_szName, kInfo.m_iLevel, kInfo.m_iResultLadderPoint );
		}
	}

	pTotalResult->SetGuildTeamBonus( fBlueGuildTeamBonus, fRedGuildTeamBonus );
	pTotalResult->SetFinalRoundPoint( rvClassTypeList, rvClassPointList, fBonusArray );
	pTotalResult->SetDungeonsScore( GetDungeonFloor() , ( m_dwStartDungeonFloor - 1 ) + m_dwAllMaxTurn );
	m_dwCurResultShowDuration = m_dwFinalRoundResultDuration;

	SetTotalResultBriefingWnd();
}

void ioMonsterSurvivalMode::SetTotalResultBriefingWnd()
{
	// 팀 모드 최종 결과 브리핑
	ioBaseChar *pOwner = GetOwnerChar();
	if( pOwner )
	{
		BriefingWnd *pBriefingWnd = dynamic_cast<BriefingWnd*>(g_GUIMgr.FindWnd( BRIEFING_WND ));
		if( pBriefingWnd && !pBriefingWnd->IsShow() )
		{

			D3DXVECTOR3 vNewPos = ioMath::VEC3_ZERO;
			if( g_BattleRoomMgr.IsObserverUser( g_MyInfo.GetPublicID() ) || m_pCreator->IsStealthMode() )
			{
				ioBaseChar *pTargetChar = m_pCreator->GetBaseChar(m_szEndLookCharName);
				if( pTargetChar )
				{
					if( IsNeedUpdateMapEffect( pTargetChar ) )
						vNewPos = pTargetChar->GetWorldPosition();
				}
			}
			else
			{
				if( IsNeedUpdateMapEffect( pOwner ) )
					vNewPos = pOwner->GetWorldPosition();
			}
		
			if( m_WinTeamType == WTT_BLUE_TEAM )
				pBriefingWnd->SetResultInfo( m_pCreator, BriefingWnd::BRIEFING_FINAL_RESULT, BriefingWnd::RESULT_WIN, TEAM_BLUE, GetDungeonFloor() );
			else
				pBriefingWnd->SetResultInfo( m_pCreator, BriefingWnd::BRIEFING_FINAL_RESULT, BriefingWnd::RESULT_LOSE, TEAM_BLUE, GetDungeonFloor() );

			ioHashString szMapEffect = pBriefingWnd->GetFinalResultMapEffect( TEAM_BLUE );
			// 맵 이펙트 작렬!
			if( !szMapEffect.IsEmpty() )
			{
				if( vNewPos == ioMath::VEC3_ZERO )
					CreateMapEffect( szMapEffect, GetEndSceneLookAt(), ioMath::UNIT_ALL );
				else
					CreateMapEffect( szMapEffect, vNewPos, ioMath::UNIT_ALL );
			}
		}				
	}
	SetBlindMode( true, false );
}

void ioMonsterSurvivalMode::ProcessModeGuide()
{
	if( !m_pGuideArrow )	return;

	if( !g_GUIMgr.IsNoneUIMode() && !m_bRoundSetEnd )
	{
		ioBaseChar *pOwner = GetOwnerChar();
		if( pOwner )
		{
			if( m_CurrentMagicZone.m_dwCollisionTime > 0 )
			{	
				m_pGuideArrow->Process( pOwner, m_CurrentMagicZone.m_vZonePos, TEAM_BLUE );
				return;
			}
			else if( IsMonsterBossTurn() )
			{
				BossMonsterGaugeWnd *pBossMonsterGaugeWnd = dynamic_cast< BossMonsterGaugeWnd * >( g_GUIMgr.FindWnd( MONSTER_BOSS_GAUGE_WND ) );
				if( pBossMonsterGaugeWnd && pBossMonsterGaugeWnd->IsShow() )
				{
					MonsterSurvivalRecord *pRecord = FindRecordCharName( pBossMonsterGaugeWnd->GetMonsterName() );
					if( pRecord && ToNpcChar( pRecord->pChar ) && pRecord->pChar->GetState() != CS_DIE && pRecord->pChar->GetState() != CS_LOADING )
					{	
						m_pGuideArrow->Process( pOwner, pRecord->pChar->GetWorldPosition(), TEAM_RED );					
						return;
					}
				}
			}
		}
	}
	m_pGuideArrow->SetVisible( false );					
}

void ioMonsterSurvivalMode::CheckEndSceneLookAt()
{
	ioBaseChar *pOwner = m_pCreator->GetOwnerChar();

	if( !m_LastKillChar.IsEmpty() )
	{
		MonsterSurvivalRecord *pKillRecord = FindRecordCharName( m_LastKillChar );
		if( !pKillRecord )
		{
			if( pOwner )
			{
				m_vEndLookAt = pOwner->GetWorldPosition();
				m_szEndLookCharName = pOwner->GetCharName();
				pOwner->SetEndFocusEffect( GetRoundEndEffect() );
			}
			else
			{
				m_vEndLookAt = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );				
				m_szEndLookCharName.Clear();
			}
		}
		else
		{
			m_vEndLookAt = pKillRecord->pChar->GetWorldPosition();	
			m_szEndLookCharName = pKillRecord->pChar->GetCharName();
			pKillRecord->pChar->SetEndFocusEffect( GetRoundEndEffect() );
		}
	}
	else
	{
		if( !m_LastDieChar.IsEmpty() )
		{
			MonsterSurvivalRecord *pDieRecord = FindRecordCharName( m_LastDieChar );
			if( !pDieRecord )
			{
				if( pOwner )
				{
					m_vEndLookAt = pOwner->GetWorldPosition();
					m_szEndLookCharName = pOwner->GetCharName();
					pOwner->SetEndFocusEffect( GetRoundEndEffect() );
				}
				else
				{
					m_vEndLookAt = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );				
					m_szEndLookCharName.Clear();
				}
			}
			else
			{
				ioBaseChar *pTarget = CheckMostNearChar( pDieRecord->pChar );
				if( pTarget )
				{
					m_vEndLookAt = pTarget->GetWorldPosition();		
					m_szEndLookCharName = pTarget->GetCharName();
					pTarget->SetEndFocusEffect( GetRoundEndEffect() );
				}
				else
				{
					m_vEndLookAt = pDieRecord->pChar->GetWorldPosition();	
					m_szEndLookCharName = pDieRecord->pChar->GetCharName();
					pDieRecord->pChar->SetEndFocusEffect( GetRoundEndEffect() );
				}
			}
		}
		else
		{
			if( pOwner )
			{
				m_vEndLookAt = pOwner->GetWorldPosition();				
				m_szEndLookCharName = pOwner->GetCharName();
				pOwner->SetEndFocusEffect( GetRoundEndEffect() );
			}
			else
			{
				m_vEndLookAt = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );				
				m_szEndLookCharName.Clear();
			}
		}
	}
}

void ioMonsterSurvivalMode::ClearModeData()
{
	m_LastDieChar.Clear();
	m_LastKillChar.Clear();
	m_TurnHelpList.clear();

	for(int i = 0;i < (int)m_HighTurnList.size();i++)
	{
		HighTurnData &rkHighTurn = m_HighTurnList[i];
		rkHighTurn.m_TurnData.clear();
	}
	m_HighTurnList.clear();

	g_SoundMgr.AllSoundStop();
	g_SoundMgr.StopStream();
	m_pCreator->HidingAllFieldItem();
}

bool ioMonsterSurvivalMode::IsNeedRevival( ioBaseChar *pChar )
{
	return true;
	/*
	if( !pChar ) return false;

	if( !m_LastKillChar.IsEmpty() && m_LastKillChar == pChar->GetCharName() )
		return true;

	return false;
	*/
}

ioBaseChar* ioMonsterSurvivalMode::CheckMostNearChar( ioBaseChar *pChar )
{
	ioBaseChar *pNearChar = NULL;
	float fCharDistSq = std::numeric_limits<float>::infinity();
	int iCharCnt = m_vCharRecordList.size();
	for( int i=0 ; i<iCharCnt ; i++ )
	{
		ioBaseChar *pTarget = m_vCharRecordList[i].pChar;
		if( !pChar ) continue;
		if( pChar->GetTeam() == pTarget->GetTeam() ) continue;
		if( pTarget->GetState() == CS_DIE ||
			pTarget->GetState() == CS_VIEW ||
			pTarget->GetState() == CS_OBSERVER ||
			pTarget->GetState() == CS_READY ||
			pTarget->GetState() == CS_LOADING )
			continue;
		
		D3DXVECTOR3 vDiff = pChar->GetWorldPosition() - pTarget->GetWorldPosition();
		float fDiffSq = D3DXVec3LengthSq( &vDiff );
		if( fDiffSq < fCharDistSq )
		{
			pNearChar = pTarget;
			fCharDistSq = fDiffSq;
		}
	}

	return pNearChar;
}

int ioMonsterSurvivalMode::GetCurRank( const ioHashString& szCharName )
{
	ModeRecord *pTargetRecord = FindModeRecordCharName( szCharName );
	if( !pTargetRecord )	return -1;
	if( !pTargetRecord->pChar ) return -1;

	TeamType eTeam = pTargetRecord->pChar->GetTeam();

	int iCurRank = 1;
	int iSize = m_RoundInfoList.size();
	for( int i=0; i < iSize; i++ )
	{
		const RoundInfo &kRoundInfo = m_RoundInfoList[i];

		if( kRoundInfo.m_szName == szCharName )
			return iCurRank;

		if( kRoundInfo.m_Team == eTeam )
			iCurRank++;
	}

	return -1;
}

int ioMonsterSurvivalMode::GetContributePerChar( const ioHashString& szName )
{
	int iCharCnt = m_vCharRecordList.size();

	for(int i = 0;i < iCharCnt;i++)
	{
		MonsterSurvivalRecord *pRecord = &m_vCharRecordList[i];
		if( !pRecord )	continue;
		if( !pRecord->pChar ) continue;

		if( pRecord->pChar->GetCharName() == szName )
		{
			return pRecord->Contribute.m_iPrevContributePer;
		}
	}

	return 0;
}

bool ioMonsterSurvivalMode::IsRenderGoldMonsterCoinHelp()
{
	if( m_bRoundSetEnd ) return false;
	ioBaseChar *pOwnerChar = GetOwnerChar();
	if( pOwnerChar == NULL ) return false;
	if( pOwnerChar->IsPrisonerMode() || pOwnerChar->GetState() == CS_DIE )
	{
		if( FRAMEGETTIME() - m_dwCurRevivalCoinTime < 2000 ) return false;    // 1초 딜레이
		if( m_CurrentMagicZone.m_dwCollisionTime != 0 ) return false;         // 마법진이 나오면 자동으로 부활하므로 골드 코인 사용을 막는다.

		return true;
	}
	return false;
}

void ioMonsterSurvivalMode::UIContinueTime( DWORD dwContinueTime )
{
	MonsterSurvivalHelWnd *pHelpWnd = dynamic_cast<MonsterSurvivalHelWnd*>( g_GUIMgr.FindWnd( MONSTER_SURVIVAL_HELP_WND ) );
	if( pHelpWnd )
	{
		pHelpWnd->SetContinueTime( dwContinueTime );
	}
}

void ioMonsterSurvivalMode::UIZoneCollisionTime( DWORD dwZoneCollisionTime )
{
	MonsterSurvivalHelWnd *pHelpWnd = dynamic_cast<MonsterSurvivalHelWnd*>( g_GUIMgr.FindWnd( MONSTER_SURVIVAL_HELP_WND ) );
	if( pHelpWnd )
	{
		pHelpWnd->SetZoneCollisionTime( dwZoneCollisionTime );
	}
}

void ioMonsterSurvivalMode::UIKOTime( DWORD dwCurTime )
{
	MonsterSurvivalHelWnd *pHelpWnd = dynamic_cast<MonsterSurvivalHelWnd*>( g_GUIMgr.FindWnd( MONSTER_SURVIVAL_HELP_WND ) );
	if( pHelpWnd )
	{
		pHelpWnd->SetKOTime( dwCurTime );
	}
}

void ioMonsterSurvivalMode::UIDeathTime( DWORD dwCurTime )
{
	MonsterSurvivalHelWnd *pHelpWnd = dynamic_cast<MonsterSurvivalHelWnd*>( g_GUIMgr.FindWnd( MONSTER_SURVIVAL_HELP_WND ) );
	if( pHelpWnd )
	{
		pHelpWnd->SetDeathTime( dwCurTime );
	}
}

void ioMonsterSurvivalMode::OnRoundEnd( SP2Packet &rkPacket )
{
	int iWinTeam;
	rkPacket >> iWinTeam;
	rkPacket >> m_iRedRoundWinCnt;
	rkPacket >> m_iBlueRoundWinCnt;

#if defined( USE_GA )
	bool		bWin		= false;
	ioBaseChar *pGAOwner	= GetOwnerChar();
	if( pGAOwner )
	{
		if( IsWinTeam( (WinTeamType)iWinTeam, pGAOwner->GetTeam() ) )
			bWin = true;
		else
			bWin = false;
	}

	g_HttpMng.SetWin( bWin );
#endif

	int i = 0;
	int iRecordCnt = GetRecordCharCnt();
	for( i=0 ; i<iRecordCnt ; i++ )
	{
		ModeRecord *pRecord = FindModeRecord( i );
		if( pRecord )
		{
			pRecord->bResultShow = false;
		}
	}

	int iInfoCnt = 0;
	rkPacket >> iInfoCnt;
	for( i=0 ; i<iInfoCnt ; i++ )
	{
		ioHashString szName;
		rkPacket >> szName;

		ModeRecord *pRecord = FindModeRecordCharName( szName );
		if( pRecord )
		{
			int iMyVictories;
			rkPacket >> iMyVictories;

			if( g_LadderTeamMgr.IsLadderTeam() )
				g_VictoriesMgr.SetVictories( szName, iMyVictories );

			int iKillCharCnt, iDeathCharCnt;
			pRecord->iKillInfoMap.clear();
			pRecord->iDeathInfoMap.clear();

			rkPacket >> iKillCharCnt;
			for( int k=0; k < iKillCharCnt; ++k )
			{
				int iClassType, iKillCnt;
				rkPacket >> iClassType >> iKillCnt;

				pRecord->iKillInfoMap.insert( KillDeathInfoMap::value_type(iClassType, iKillCnt) );
			}

			rkPacket >> iDeathCharCnt;
			for( int j=0; j < iDeathCharCnt; ++j )
			{
				int iClassType, iDeathCnt;
				rkPacket >> iClassType >> iDeathCnt;

				pRecord->iDeathInfoMap.insert( KillDeathInfoMap::value_type(iClassType, iDeathCnt) );
			}

			rkPacket >> pRecord->iCurRank;
			rkPacket >> pRecord->iPreRank;

			pRecord->bResultShow = true;
		}
		else
		{
			int iMyVictories;
			rkPacket >> iMyVictories;

			int iKillCharCnt, iDeathCharCnt;

			rkPacket >> iKillCharCnt;
			for( int k=0;k < iKillCharCnt; ++k )
			{
				int iClassType, iKillCnt;
				rkPacket >> iClassType >> iKillCnt;
			}

			rkPacket >> iDeathCharCnt;
			for( int j=0; j < iDeathCharCnt; ++j )
			{
				int iClassType, iDeathCnt;
				rkPacket >> iClassType >> iDeathCnt;
			}

			int iCurRank, iPreRank;
			rkPacket >> iCurRank >> iPreRank;
		}
	}

	bool bRoundSetEnd;
	rkPacket >> bRoundSetEnd;

	// 기여도와 시상식을 동기화 시킬 유저
	bool bResultSync = false;
	int iResultSyncUser;
	rkPacket >> iResultSyncUser;
	for(i = 0;i < iResultSyncUser;i++)
	{
		ioHashString szUserName;
		rkPacket >> szUserName;
		
		if( szUserName == g_MyInfo.GetPublicID() )
			bResultSync = true;
	}

	SendOwnerDamageList( bRoundSetEnd );

	EndChangeCharacter();

	ioBaseChar *pOwner = GetOwnerChar();
	if( pOwner )
	{
		g_MyInfo.SetCharDiedPenalty( pOwner->GetSelectCharArray(), false );
	}

	SetRoundEndInfo( (WinTeamType)iWinTeam, bRoundSetEnd );

	if( m_bRoundSetEnd )
	{
		if( !m_CurChampName.IsEmpty() )
		{
			ioBaseChar *pChamp = GetRecordChar( m_CurChampName );
			if( pChamp )
			{
				pChamp->SetChampEmoticon( true, false );
			}
		}

		if( bResultSync )
		{
			// 최종 기여도를 서버로 전송한다.
			ContributeConvert();      //기여도 환산
			ContributeRoundAllPercent();

			SP2Packet kPacket( CTPK_LAST_PLAYRECORD_INFO );
			FillLastPlayRecordInfo( kPacket );
			TCPNetwork::SendToServer( kPacket );

			// 시상식 주역 4인을 서버로 전송한다.
			g_AwardMgr.SetFinalResult( (WinTeamType)iWinTeam );		
			int iRecordCnt = GetRecordCharCnt();
			for( i=0 ; i<iRecordCnt ; i++ )
			{
				FinalInfo kInfo;
				if( GetFinalInfo( i, kInfo ) )
				{
					g_AwardMgr.AddFinalInfo( kInfo );
				}
			}			
			SP2Packet kPacket1( CTPK_AWARDING_RESULT );
			g_AwardMgr.FillAwardResult( kPacket1 );
			TCPNetwork::SendToServer( kPacket1 );
		}
	}
}

bool ioMonsterSurvivalMode::ProcessTCPPacket( SP2Packet &rkPacket )
{
	ErrorReport::SetPosition( 1031, 1 );
	if( ioPlayMode::ProcessTCPPacket( rkPacket ) )
		return true;

	ErrorReport::SetPosition( 1031, 2 );

	switch( rkPacket.GetPacketID() )
	{
	case STPK_PRISONERMODE:
		ErrorReport::SetPosition( 1031, 3 );
		OnPrisonerMode( rkPacket );
		ErrorReport::SetPosition( 1031, 4 );
		return true;
	case STPK_PRISONER_ESCAPE:
		ErrorReport::SetPosition( 1031, 5 );
		OnEscapePrisonerMode( rkPacket );
		ErrorReport::SetPosition( 1031, 6 );
		return true;
	case STPK_PRISONER_DROP:
		ErrorReport::SetPosition( 1031, 7 );
		OnPrisonerDrop( rkPacket );
		ErrorReport::SetPosition( 1031, 8 );
		return true;
	case STPK_PRISONER_REVIVAL:
		ErrorReport::SetPosition( 1031, 11 );
		OnPrisonerRevival( rkPacket );
		ErrorReport::SetPosition( 1031, 12 );
		return true;
	case STPK_USE_MONSTER_COIN:
		ErrorReport::SetPosition( 1031, 13 );
		OnParentUseMonsterCoin( rkPacket );
		ErrorReport::SetPosition( 1031, 14 );
		return true;
	case STPK_MONSTER_FORCE_DIE:
		ErrorReport::SetPosition( 1031, 15 );
		OnMonsterForceDie( rkPacket );
		ErrorReport::SetPosition( 1031, 16 );
		return true;
	case STPK_MONSTER_REVIVAL:
		ErrorReport::SetPosition( 1031, 17 );
		OnMonsterRevival( rkPacket );
		ErrorReport::SetPosition( 1031, 18 );
		return true;
	case STPK_MONSTER_DIE_REWARD:
		ErrorReport::SetPosition( 1031, 19 );
		OnMonsterDieReward( rkPacket );
		ErrorReport::SetPosition( 1031, 20 );
		return true;
	case STPK_MONSTER_DICE_RESULT:
		ErrorReport::SetPosition( 1031, 21 );
		OnMonsterDiceResult( rkPacket );
		ErrorReport::SetPosition( 1031, 22 );
		return true;
	}

	return false;
}

void ioMonsterSurvivalMode::OnPrisonerMode( SP2Packet &rkPacket )
{	
	ioHashString szPrisonerName;
	rkPacket >> szPrisonerName;

	MonsterSurvivalRecord *pRecord = FindRecordCharName( szPrisonerName );
	if( !pRecord ) return;

	ioBaseChar *pChar = pRecord->pChar;
	if( !pChar )
	{
		LOG.PrintTimeAndLog( 0, "ioMonsterSurvivalMode::PrisonerUser - %s Not Exist User", szPrisonerName.c_str() );
		return;
	}

	ioHashString szLastAttacker, szLastAttackerSkill;
	D3DXVECTOR3 vCurPos;

	rkPacket >> szLastAttacker >> szLastAttackerSkill;
	rkPacket >> vCurPos;
	pChar->SetWorldPosition( vCurPos );
	pChar->SetPrisonerMode();

	ApplyCharModeInfo( szPrisonerName, rkPacket );
	ApplyCharModeInfo( szLastAttacker, rkPacket );

	KillContribute( szLastAttacker, szPrisonerName, 0 );

	pRecord->dwCurDieTime = 0;

	// For EndScene
	ioHashString szAttName = szLastAttacker;
	m_LastDieChar = pChar->GetCharName();
	TeamType eKillTeam = TEAM_NONE;
	MonsterSurvivalRecord *pKillRecord = FindRecordCharName( szLastAttacker );
	if( !pKillRecord )
	{
		m_LastKillChar.Clear();
	}
	else
	{
		//팀 제외 
		if( pKillRecord->pChar->GetTeam() == pChar->GetTeam()  )
			m_LastKillChar.Clear();
		else
			m_LastKillChar = pKillRecord->pChar->GetCharName();
		eKillTeam = pKillRecord->pChar->GetTeam();

		ioNpcChar *pNpcChar = ToNpcChar( pKillRecord->pChar );
		if( pNpcChar )
			szAttName = pNpcChar->GetViewName();
	}

	// 포로메세지
	g_ModeUIController.SetPrisonerMsg( eKillTeam, szAttName, szLastAttackerSkill, pChar->GetLastAttackerItemCode(), pChar->GetTeam(), szPrisonerName );

	if( pChar->IsOwnerChar() )
	{
		m_pCreator->StartScreenBlind( SBT_FIXED_BLIND, NULL, 0 );
	}
	else
	{
		if( pKillRecord )
		{
			ioBaseChar *pAttacker = pKillRecord->pChar;
			if( pAttacker->IsOwnerChar() )
			{
				if( pAttacker->GetState() == CS_USING_SKILL )
					m_pCreator->StartScreenBlind( SBT_CATCH_KILL, NULL, true );
				else
					m_pCreator->StartScreenBlind( SBT_CATCH_KILL, NULL, true );
			}
		}
	}


	// KO 처리
	if( pKillRecord && pRecord )
	{
		if( pKillRecord->pChar->GetTeam() != pRecord->pChar->GetTeam() )
		{
			pKillRecord->AddKill( GetModeType(), GetKillPoint( pKillRecord->pChar->GetTeam() ), false, false );
			pRecord->AddDeath( GetModeType(), GetDeathPoint( pRecord->pChar->GetTeam() ) );				
		}
		else
		{
			if( pKillRecord->pChar == pRecord->pChar )	// 자기자신의 공격
			{
				pRecord->AddDeath( GetModeType(), GetDeathPoint( pRecord->pChar->GetTeam() ) );
			}
			else
			{
				pKillRecord->AddKill( GetModeType(), -GetKillPoint( pKillRecord->pChar->GetTeam() ), false, false );
				pRecord->AddDeath( GetModeType(), GetDeathPoint( pRecord->pChar->GetTeam() ) );
			}
		}
		pKillRecord->pChar->SetKillEmoticon( pKillRecord->GetUniqueTotalKill() );
	}
	else if( pRecord )
	{
		pRecord->AddDeath( GetModeType(), GetDeathPoint( pRecord->pChar->GetTeam() ) );
	}
}

void ioMonsterSurvivalMode::OnEscapePrisonerMode( SP2Packet &rkPacket )
{
	ioHashString szName, szLastAttacker, szLastAttackerSkill;
	rkPacket >> szName >> szLastAttacker >> szLastAttackerSkill;

	ioBaseChar *pChar = GetRecordChar( szName );
	if( !pChar )
	{
		LOG.PrintTimeAndLog( 0, "ioMonsterSurvivalMode::OnEscapePrisonerMode - %s Not Exist User", szName.c_str() );
		return;
	}

	pChar->EscapePrisonerMode( TEAM_NONE, true );

	if( pChar->IsOwnerChar() )
	{
		// 화면 블라인드해제
		if( m_CurrentMagicZone.m_dwCollisionTime == 0 )
			m_pCreator->SetScreenBlindDownState();
	}

	// HP 회복
	StatusValue &rkStatus = pChar->GetHP();
	pChar->SetCurHP( rkStatus.m_fMaxValue * m_fRevivalRecoveryHP );

	// 포로메세지
	ioBaseChar *pKiller = GetRecordChar( szLastAttacker );
	if( pKiller )
	{
		ioHashString szAttName = szLastAttacker;
		ioNpcChar *pNpcChar = ToNpcChar( pKiller );
		if( pNpcChar )
			szAttName = pNpcChar->GetViewName();

		g_ModeUIController.SetEscapeMsg( pKiller->GetTeam(), szAttName, szLastAttackerSkill, pChar->GetLastAttackerItemCode(), pChar->GetTeam(), szName );
	}
	UIContinueTime( 0 );
}

void ioMonsterSurvivalMode::OnPrisonerDrop( SP2Packet &rkPacket )
{
	ioHashString szDieChar, szAttacker;
	rkPacket >> szDieChar >> szAttacker;

	float fLastRate;
	rkPacket >> fLastRate;

	ioBaseChar *pDieChar = m_pCreator->GetBaseChar( szDieChar );
	if( !pDieChar )	return;

	ioBaseChar *pKillChar = m_pCreator->GetBaseChar( szAttacker );

	UpdateDropDieRecord( pDieChar, pKillChar, NULL );

	ApplyCharModeInfo( szDieChar, rkPacket );
	ApplyCharModeInfo( szAttacker, rkPacket );

	pDieChar->SetDieState();

	pDieChar->EscapePrisonerMode( TEAM_NONE, false );

	KillContribute( szAttacker, szDieChar, 0 );

	if( pDieChar != GetOwnerChar() )
	{
		// 죽은 유저의 데미지 리스트는 All에 넣고 부활 후 부터 다시 체크한다.
		AttachOwnerDamageList( szDieChar );
	}
}

void ioMonsterSurvivalMode::OnPrisonerRevival( SP2Packet &rkPacket )
{
	ioHashString szPrisonerName;
	rkPacket >> szPrisonerName;

	MonsterSurvivalRecord *pRecord = FindRecordCharName( szPrisonerName );
	if( !pRecord ) return;

	ioBaseChar *pChar = pRecord->pChar;
	if( !pChar )
	{
		LOG.PrintTimeAndLog( 0, "ioMonsterSurvivalMode::OnPrisonerRevival - %s Not Exist User", szPrisonerName.c_str() );
		return;
	}
	pRecord->dwCurDieTime = 0;

	D3DXVECTOR3 vRevivalPos = GetRevivalPos( pChar );
	pChar->SetWorldPosition( vRevivalPos );
	pChar->DestroyAllItem();
	pChar->Revival();
	pChar->CheckDefaultReinforce();
	pChar->SetPrisonerMode();
	pChar->SetState( CS_DELAY );
	pChar->SetCurHP( 0.0f );	
	
	if( pChar->IsOwnerChar() )
	{
		g_ModeHelpMgr.HideHelp( ioModeHelpManager::HT_VIEW );
		SetBlindMode( false );
	}
	UIContinueTime( 0 );
}

void ioMonsterSurvivalMode::OnParentUseMonsterCoin( SP2Packet &rkPacket )
{
	OnUseMonsterCoin( rkPacket );
	if( m_pCreator )
		m_pCreator->OnCharEquipMedalItemData( rkPacket );
}

void ioMonsterSurvivalMode::OnUseMonsterCoin( SP2Packet &rkPacket )
{
	int iResult;
	rkPacket >> iResult;

	switch( iResult )
	{
	case USE_MONSTER_COIN_START_OK:
		{
			int nUseCoinCount		= 0;
			int nUseGoldCoinCount	= 0;
			m_bUsingMonsterCoin		= false;
			m_dwCurStartCoinTime	= 0;
			g_MyInfo.UseModeStartMonsterCoin( m_iUseMonsterCoinCnt, true, nUseCoinCount, nUseGoldCoinCount );			

			if( 0 < nUseCoinCount && 0 == nUseGoldCoinCount )
				g_ChatMgr.SetSystemMsg( STR(3), nUseCoinCount );
			else if( 0 == nUseCoinCount && 0 < nUseGoldCoinCount )
				g_ChatMgr.SetSystemMsg( "해골영웅 플레이를 위해 골드 몬스터코인 %d개 소모", nUseGoldCoinCount );
			else if( 0 < nUseCoinCount && 0 < nUseGoldCoinCount )
				g_ChatMgr.SetSystemMsg( "해골영웅 플레이를 위해 몬스터코인 %d개, 골드 몬스터코인 %d개 소모", nUseCoinCount, nUseGoldCoinCount );
		}
		break;
	case USE_MONSTER_COIN_REVIVAL_OK:
		{
			ioHashString szUseUser;
			rkPacket >> szUseUser;

			if( szUseUser == g_MyInfo.GetPublicID() )
			{
				// 코인 1개 삭제
				g_MyInfo.UseGoldMonsterCoin();

				// 타 용병 총알 + 레이저 충전
				g_MyInfo.InitCharWeaponItem();

				// 화면 블라인드해제
				if( m_CurrentMagicZone.m_dwCollisionTime == 0 )
					m_pCreator->SetScreenBlindDownState();
			}

			// HP 풀 회복 + 아이템 풀 착용
			MonsterSurvivalRecord *pRecord = FindRecordCharName( szUseUser );
			if( pRecord && pRecord->pChar )
			{
				pRecord->pChar->EscapePrisonerMode( TEAM_NONE, true );     // 포로 탈출
				pRecord->pChar->DestroyAllItem();

				// 드랍존에 있으면 부활 위치로 이동
				if( pRecord->pChar->IsCharDropZonePos() )
				{
					pRecord->pChar->SetWorldPosition( GetRevivalPos( pRecord->pChar ) );
				}

				if( pRecord->pChar->GetState() == CS_DIE )
				{	
					pRecord->pChar->Revival();
					if( pRecord->pChar->IsOwnerChar() )
					{
						g_ModeHelpMgr.HideHelp( ioModeHelpManager::HT_VIEW );
						SetBlindMode( false );
					}
				}	

				DWORD dwItemCode;
				int iItemReinforce, iItemIndex;
				DWORD dwItemMaleCustom, dwItemFemaleCustom;
				ioHashString szOwner;
				for( int i=0 ; i<MAX_EQUIP_SLOT ; i++ )
				{
					rkPacket >> dwItemCode >> iItemReinforce >> dwItemMaleCustom >> dwItemFemaleCustom  >> iItemIndex >> szOwner;

					if( dwItemCode > 0 )
					{
						ioItem *pItem = m_pCreator->CreateItem( dwItemCode );
						if( pItem )
						{
							pItem->ReLoadProperty();
							pItem->SetOwnerName( szOwner );
							pItem->SetItemCreateIndex( iItemIndex );
							pItem->SetItemReinforce( iItemReinforce );
							pItem->SetItemCustom( dwItemMaleCustom, dwItemFemaleCustom );

							// 코스튬 관련 (코스튬 동기화 : 서버에서 받을수도 있음)
							int nSubType = pItem->GetType()-1;
							if ( COMPARE( nSubType, 0, MAX_INVENTORY ) )
							{
								pItem->SetCostumeCustom( pRecord->pChar->GetCharacterInfo().m_EquipCostume[nSubType].m_Costume_Male_Custom, pRecord->pChar->GetCharacterInfo().m_EquipCostume[nSubType].m_Costume_Female_Custom );
								pItem->SetCostume( pRecord->pChar->GetCharacterInfo().m_EquipCostume[nSubType].m_CostumeCode, pRecord->pChar->IsOwnerChar() );
							}

							// Gauge Full
							if( pItem->HasSkillGauge() )
								pItem->SetCurSkillGauge( FLOAT100 );

							ioItem *pPreItem = pRecord->pChar->EquipItem( pItem, true );
							SAFEDELETE( pPreItem );
						}
					}
				}

				m_pCreator->OnCharEquipAccessoryData( rkPacket );
				m_pCreator->OnExpandMedalSlotData(rkPacket);

				pRecord->pChar->CheckDefaultReinforce();
				pRecord->pChar->FillMaxHP();
				pRecord->pChar->SetCoinRebirthState();

				if( !m_szCoinRevivalBuff.IsEmpty() )
					pRecord->pChar->AddNewBuff( m_szCoinRevivalBuff, pRecord->pChar->GetCharName(), "", NULL );

				UIContinueTime( 0 );			

				// 채팅 알림
				ioComplexStringPrinter kPrinter;
				g_ChatMgr.PresetChatComplexString( MSG_INFO_CHAT, kPrinter );

				kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(),STR(5), pRecord->pChar->GetCharName().c_str() );
				kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(),STR(6) );
				g_ChatMgr.SetChatComplexString( STR(2), kPrinter );
			}
			else
			{
				LOG.PrintTimeAndLog( 0, "ioPlayStage::OnUseMonsterCoin - %s Not Exist", szUseUser.c_str() );
			}
		}	
		break;
	case USE_MONSTER_COIN_FAIL_CNT:
		{
			g_ChatMgr.SetSystemMsg( STR(1) );
			m_bUsingMonsterCoin = false;
			int iMonsterCoin, iGoldMonsterCoin;
			rkPacket >> iMonsterCoin >> iGoldMonsterCoin;
			// 코인 리플레쉬
			g_MyInfo.SetEtcMonsterCoin( iMonsterCoin );
			g_MyInfo.SetEtcGoldMonsterCoin( iGoldMonsterCoin );

			// 유저 이탈
			SP2Packet kPacket( CTPK_EXIT_ROOM );
			kPacket << EXIT_ROOM_MONSTER_COIN_LACK << 0 << true;
			TCPNetwork::SendToServer( kPacket );
			g_BattleRoomMgr.LeaveRoom();
		}		
		break;
	case USE_MONSTER_COIN_LIVE_USER:
		break;
	case USE_MONSTER_COIN_AT_PCROOM:
		{
			m_bUsingMonsterCoin		= false;
			m_dwCurStartCoinTime	= 0;
		}
		break;
	}
}

void ioMonsterSurvivalMode::OnMonsterForceDie( SP2Packet &rkPacket )
{
	int iMaxNPC;
	rkPacket >> iMaxNPC;
	for(int i = 0;i < iMaxNPC;i++)
	{
		ioHashString kNpcName;
		rkPacket >> kNpcName;
		// 소환물 삭제
		m_pCreator->ClearUserCreateEntities( kNpcName );

		MonsterSurvivalRecord *pRecord = FindRecordCharName( kNpcName );
		if( pRecord && pRecord->pChar )
		{
			pRecord->pChar->SetDieState();
			AddEraseNpc( pRecord->pChar->GetCharName() );
		}
	}
}

void ioMonsterSurvivalMode::OnMonsterRevival( SP2Packet &rkPacket )
{
	DWORD dwNpcCode;
	ioHashString kNpcName, kSyncName;
	float fStartXPos, fStartZPos;
	rkPacket >> dwNpcCode >> kNpcName >> kSyncName >> fStartXPos >> fStartZPos;

	RemoveEraseNpc( kNpcName );
	ioNpcChar *pNpcChar = NULL;
	MonsterSurvivalRecord *pRecord = FindRecordCharName( kNpcName );
	if( pRecord && ToNpcChar( pRecord->pChar ) )
	{
		pNpcChar = ToNpcChar( pRecord->pChar );
		if( pNpcChar )
		{
			pNpcChar->Revival();
			pNpcChar->SetState( CS_LOADING );
			pNpcChar->FillMaxHP();
			pNpcChar->SetCharName( kNpcName );
			pNpcChar->SetSyncUser( kSyncName );		
			pNpcChar->SetTeam( TEAM_RED );
			pNpcChar->StartAI( 0, fStartXPos, fStartZPos );
			pNpcChar->ReSetGradeLevel( 0 );
			pNpcChar->ReEquipItem();

			// 턴 밸런스
			if( COMPARE( m_dwCurrentHighTurn, 0, (DWORD)m_HighTurnList.size() ) )
			{
				// ArmorClass And SpeedClass.
				HighTurnData &rkTurnData = m_HighTurnList[m_dwCurrentHighTurn];
				TurnBalance kBalance = rkTurnData.m_NormalBalance;
				if( IsMonsterBossTurn() )
					kBalance = rkTurnData.m_BossBalance;
				pNpcChar->AddItemBalance( kBalance.m_fAddDefaultArmorClass, kBalance.m_fAddDefaultSpeedClass );
				pNpcChar->SetAddCharArmorClass( kBalance.m_fAddArmorClass * (int)m_dwCurrentLowTurn, kBalance.m_fMaxArmorClass );
				pNpcChar->SetAddCharSpeedClass( -( kBalance.m_fAddSpeedClass * (int)m_dwCurrentLowTurn ), kBalance.m_fMaxSpeedClass );
			}
		}
	}
	else
	{
		pNpcChar = g_NpcMgr.CreateNpcChar( NPC_NOT_INDEX, dwNpcCode, m_pCreator, this );
		if( pNpcChar )
		{
			pNpcChar->FillMaxHP();
			pNpcChar->SetCharName( kNpcName );
			pNpcChar->SetSyncUser( kSyncName );		
			pNpcChar->SetTeam( TEAM_RED );
			pNpcChar->StartAI( 0, fStartXPos, fStartZPos );
			pNpcChar->ReSetGradeLevel( 0 );
			AddNewRecord( pNpcChar );

			if( pNpcChar )
			{
				// 턴 밸런스
				if( COMPARE( m_dwCurrentHighTurn, 0, (DWORD)m_HighTurnList.size() ) )
				{
					// 스턱 정보 적용
					HighTurnData &rkTurnData = m_HighTurnList[m_dwCurrentHighTurn];
					pNpcChar->SetStuckInfo( min( rkTurnData.m_iStuckActiveRandMax, rkTurnData.m_iStuckActiveRand + ( (int)m_dwCurrentLowTurn * rkTurnData.m_iStuckActiveRandAdd ) ),
						rkTurnData.m_iStuckActiveSeed, rkTurnData.m_dwStuckActiveTime, rkTurnData.m_StuckBuffList );

					for(int j = 0;j < (int)rkTurnData.m_StuckExDataList.size();j++)
					{
						StuckExData &rkExData = rkTurnData.m_StuckExDataList[j];
						pNpcChar->SetStuckExList( rkExData.m_szWeaponType, 
							min( rkExData.m_iStuckActiveRandMax, rkExData.m_iStuckActiveRand + ( (int)m_dwCurrentLowTurn * rkExData.m_iStuckActiveRandAdd ) ) );
					}

					// ArmorClass And SpeedClass.
					TurnBalance kBalance = rkTurnData.m_NormalBalance;
					if( IsMonsterBossTurn() )
						kBalance = rkTurnData.m_BossBalance;
					pNpcChar->AddItemBalance( kBalance.m_fAddDefaultArmorClass, kBalance.m_fAddDefaultSpeedClass );
					pNpcChar->SetAddCharArmorClass( kBalance.m_fAddArmorClass * (int)m_dwCurrentLowTurn, kBalance.m_fMaxArmorClass );
					pNpcChar->SetAddCharSpeedClass( -( kBalance.m_fAddSpeedClass * (int)m_dwCurrentLowTurn ), kBalance.m_fMaxSpeedClass );
				}
			}
		}
	}		
}

void ioMonsterSurvivalMode::OnMonsterDieReward( SP2Packet &rkPacket )
{
	int i = 0;
	// 주사위 선물 보상
	bool bDiceStart;
	rkPacket >> bDiceStart;
	if( bDiceStart )
	{
		ioHashString kMonsterName;
		rkPacket >> kMonsterName;

		short iPresentType;
		int iPresentValue1, iPresentValue2;
		rkPacket >> iPresentType >> iPresentValue1 >> iPresentValue2;

		ioHashStringVec kRankUserList;
		int iDamageRankSize;
		rkPacket >> iDamageRankSize;
		for(i = 0;i < iDamageRankSize;i++)
		{
			ioHashString kRankUser;
			rkPacket >> kRankUser;
			kRankUserList.push_back( kRankUser );
		}		
	}

	// 페소 & 경험치 보상
	int iRewardSize;
	rkPacket >> iRewardSize;
	for(i = 0;i < iRewardSize;i++)
	{
		ioHashString szRewardUser;
		rkPacket >> szRewardUser;

		int iRewardExp, iRewardPeso;
		rkPacket >> iRewardExp >> iRewardPeso;		

		ioBaseChar *pChar = m_pCreator->GetBaseChar( szRewardUser );
		if( szRewardUser == g_MyInfo.GetPublicID() )
		{
			__int64 iMoney;
			int iGradeLevel, iGradeExpert;
			rkPacket >> iMoney >> iGradeLevel >> iGradeExpert;

			int iPrevGradeLevel = g_MyInfo.GetGradeLevel();
			g_MyInfo.SetMoney( iMoney );
			g_MyInfo.SetGrade( iGradeLevel, iGradeExpert );
			if( iPrevGradeLevel != g_MyInfo.GetGradeLevel() )
			{				
				if( pChar )
					pChar->SetLevel( iGradeLevel );
			}
			
			int iMaxClassReward;
			rkPacket >> iMaxClassReward;
			for(int k = 0;k < iMaxClassReward;k++)
			{
				int iClassType, iClassPoint;
				rkPacket >> iClassType >> iClassPoint;
				g_MyInfo.AddClassExp( iClassType, iClassPoint );
			}
			
			if( iRewardExp > 0 )
			{
				g_ChatMgr.SetGradeExpObtainMsg( iRewardExp );
			}

			if( iRewardPeso > 0 )
			{
				g_ChatMgr.SetPesoObtainMsg( iRewardPeso );
			}
		}

		if( pChar )
		{
			// 획득 애니메이션
			pChar->SetMonsterReward( iRewardExp, iRewardPeso );

		}
	}	
}

void ioMonsterSurvivalMode::OnMonsterDiceResult( SP2Packet &rkPacket )
{
	// 주사위 결과
	ioHashString kPrizeWinner;
	rkPacket >> kPrizeWinner;

	short iPresentType;
	int iPresentValue1, iPresentValue2;
	rkPacket >> iPresentType >> iPresentValue1 >> iPresentValue2;

	g_ModeUIController.SetMonsterDiceResult( kPrizeWinner, iPresentType, iPresentValue1, iPresentValue2 );
}