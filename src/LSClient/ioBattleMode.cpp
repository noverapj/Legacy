

#include "stdafx.h"

#include "ioBattleMode.h"
#include "ioLadderTeamMgr.h"
#include "ioShuffleRoomMgr.h"
#include "ioRankBattleManager.h"


#include "GUI/MiniSoldierSelectWnd.h"
#include "GUI/CenterInfoWnd.h"
//#include "GUI/TimeScoreWnd.h"
#include "GUI/ioObserverWnd.h"
#include "GUI/TotalResultMainWnd.h"
#include "GUI/RoundStateMainWnd.h"
#include "GUI/CenterKillInfoWnd.h"
#include "GUI/BattleModeWnd.h"
#include "GUI/BriefingWnd.h"

ioBattleMode::ioBattleMode( ioPlayStage *pCreator )
: ioPlayMode( pCreator )
{
	m_iBlueKillPoint = 0;
	m_iRedKillPoint = 0;

	m_ModeType = MT_BATTLE;

	m_vCharRecordList.reserve( 100 );
	m_vWaitingPos.clear();

	m_dwTagBtnDisableTime = 0;
	m_dwStartTagBtnDisableTime = 0;
	m_bWarTimeStartPacketSend = false;
	m_bFirstBlockCameraView = false;
	m_bEntryTimeOutEnableCheck = false;

	m_iMaxEntryCount = 0;
	m_iCurEntryCount = 0;
	m_dwEntryEnableTime = 0;
	m_dwTagDisableTime = 0;
	m_bUserSetting = false;
}

ioBattleMode::~ioBattleMode()
{
	m_bZeroHP = false;
	ClearMode();
}

void ioBattleMode::InitMode( int iMapIndex )
{
	m_iModeMapIndex = iMapIndex;

	LoadMap();
	LoadPushStructs();
	LoadMachineStruct();
	LoadObjectItems();
	LoadFuntionalObjects();
	LoadCharacters();
	LoadDisableSkillList();
	LoadStartBuffList();
	LoadModeGrowthRate();
	LoadTagZone();

	ioINILoader kLoader( GetModeINIFileName( m_ModeType ) );

	kLoader.SetTitle_e( "common_info" );
	m_dwRoundDuration = kLoader.LoadInt_e( "round_time", 300000 );
	m_dwCurRoundDuration = m_dwRoundDuration;

	m_fScoreWarningRate = kLoader.LoadFloat_e( "score_warning_rate", FLOAT05 );

	LoadRevivalTime( kLoader );
	LoadModeResource( kLoader );
	LoadModeCamera( kLoader );

	char szBuf[MAX_PATH];
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
	kLoader.LoadString_e( "entry_timeout_effect", "", szBuf, MAX_PATH );
	m_szEntryTimeOutEffect = szBuf;

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

	m_dwTagBtnDisableTime = 0;
	m_dwStartTagBtnDisableTime = 0;

	m_iBlueTeamScore = 0;
	m_iRedTeamScore = 0;

	m_bWarTimeStartPacketSend = false;

	m_bUserSetting = false;

	g_GUIMgr.HideWnd( MINIMAP_WND );

	m_pCreator->EnableMapDefaultEffect(true);


	for(int iCnt = 0; iCnt < g_BattleRoomMgr.GetTotalUserCount(); iCnt++)
	{
		m_mapFirstCheck.insert( std::pair<ioHashString, bool>(g_BattleRoomMgr.GetUserNameByVecIdx( iCnt ), false) );
	}
}

void ioBattleMode::LoadModeResource( ioINILoader &rkLoader )
{
	//TimeScoreWnd *pTimeScoreWnd = dynamic_cast<TimeScoreWnd *>( g_GUIMgr.AddWnd( "XML/TimeScoreWnd.xml") );
	//if(pTimeScoreWnd)
	//	pTimeScoreWnd->SetModeType( GetModeType() );

	g_ModeUIController.SetKOMsgWnd( g_GUIMgr.AddWnd( "XML/kochatmsgwnd.xml") );

	CenterInfoWnd *pCenterInfoWnd = dynamic_cast<CenterInfoWnd *>( g_GUIMgr.AddWnd("XML/CenterInfoWnd.xml") );
	if(pCenterInfoWnd)
	{
		g_ModeUIController.SetCenterInfoWnd( pCenterInfoWnd );
	}

	CenterKillInfoWnd *pCenterKillInfoWnd = dynamic_cast<CenterKillInfoWnd*>( g_GUIMgr.AddWnd( "XML/centerkillinfownd.xml" ) );
	if( pCenterKillInfoWnd )
	{
		g_ModeUIController.SetCenterKillInfoWnd( pCenterKillInfoWnd );
		g_ModeUIController.SetCenterKillInfoWndOffset( 0, KILL_INFO_WND_OFFSET_Y );
	}

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

	// 양 사이드에 블루팀, 레드팀 정보 UI 표시
	g_GUIMgr.AddWnd( "XML/battlemodebluesideuserinfownd.xml" );
	g_GUIMgr.AddWnd( "XML/battlemoderedsideuserinfownd.xml" );

	BattleModeTimeGaugeWnd *pBattleModeTimeGauge = dynamic_cast< BattleModeTimeGaugeWnd * >( g_GUIMgr.AddWnd( "XML/BattleModetimegaugewnd.xml" ) );
	if( pBattleModeTimeGauge )
		pBattleModeTimeGauge->SetPlayStage( m_pCreator );
	BattleModeFunctionWnd *pBattleModeFunction = dynamic_cast< BattleModeFunctionWnd * >( g_GUIMgr.AddWnd( "XML/battlemodefunctionwnd.xml" ) );
	if( pBattleModeFunction )
		pBattleModeFunction->SetPlayStage( m_pCreator );
	BattleModeBlueSideUserInfoWnd *pBattleModeBlueSide = dynamic_cast< BattleModeBlueSideUserInfoWnd * >( g_GUIMgr.FindWnd( BATTLE_MODE_BLUE_USER_INFO_WND ) );
	if( pBattleModeBlueSide )
		pBattleModeBlueSide->SetPlayStage( m_pCreator );
	BattleModeRedSideUserInfoWnd *pBattleModeRedSide = dynamic_cast< BattleModeRedSideUserInfoWnd * >( g_GUIMgr.FindWnd( BATTLE_MODE_RED_USER_INFO_WND ) );
	if( pBattleModeRedSide )
		pBattleModeRedSide->SetPlayStage( m_pCreator );
}

void ioBattleMode::LoadMap()
{
	LoadModeINIPATH();

	const char *pMapINIFile = GetModeMapINIFileName( m_ModeType,
		m_iModeSubNum,
		m_iModeMapIndex );

	ioINILoader kLoader( pMapINIFile );

	m_pCreator->LoadMap( kLoader );
	m_vMapCenter = m_pCreator->m_vMapCenter;

	kLoader.SetTitle_e( "drop_damage" );
	m_fDropDamageMapRate = kLoader.LoadFloat_e( "drop_damage_rate", FLOAT1 );

	kLoader.SetTitle_e( "team_start" );

	if( P2PNetwork::IsNetworkPlaying() )
	{
		int iMaxStartPos = kLoader.LoadFloat_e( "max_start_pos", 0 );

		char szStartX[MAX_PATH];
		char szStartZ[MAX_PATH];

		m_vStartPos.clear();
		m_vStartPos.reserve( iMaxStartPos );
		for( int i=0 ; i<iMaxStartPos; i++ )
		{
			wsprintf_e(szStartX, "start%d_x", i+1);
			wsprintf_e(szStartZ, "start%d_z", i+1);

			D3DXVECTOR3 vStartPos;
			vStartPos.x  = kLoader.LoadFloat( szStartX, 0.0f );
			vStartPos.z  = kLoader.LoadFloat( szStartZ, 0.0f );
			m_vStartPos.push_back(vStartPos);
		}
	}
	else
	{
		m_vStartPos.clear();
		m_vStartPos.reserve( 2 );

		D3DXVECTOR3 vStartPos;
		vStartPos.x  = kLoader.LoadFloat_e( "single_x", 0.0f );
		vStartPos.z  = kLoader.LoadFloat_e( "single_z", 0.0f );
		m_vStartPos.push_back(vStartPos);
		m_iRedPosArray = 0;

		vStartPos.x = kLoader.LoadFloat_e( "single_x", 0.0f );
		vStartPos.z = kLoader.LoadFloat_e( "single_z", 0.0f );
		m_vStartPos.push_back(vStartPos);
		m_iBluePosArray = 1;
	}

	// PassiveBuff List
	char szBuf[MAX_PATH], szKey[MAX_PATH];
	kLoader.SetTitle_e( "map_passive_buff" );

	m_vMapPassiveBuffList.clear();

	int iBuffCnt = kLoader.LoadInt_e( "passive_buff_cnt", 0 );
	if( iBuffCnt > 0 )
	{
		//m_mmMapPassiveBuffList.reserve( iBuffCnt );
		bool bFirstBuff = false;
		for( int i=0; i < iBuffCnt; ++i )
		{
			wsprintf_e( szKey, "passive_buff%d", i+1 );

			kLoader.LoadString( szKey, "", szBuf, MAX_PATH );

			wsprintf_e( szKey, "passive_buff%d_first_apply", i+1 );

			bFirstBuff = kLoader.LoadBool( szKey, false );

			MapBuffInfo info;
			info.m_szBuffName = szBuf;
			info.m_bFirstApply = bFirstBuff;
			m_vMapPassiveBuffList.push_back( info );
		}
	}

	kLoader.SetTitle_e( "event_scene" );
	m_dwMapCameraSceneID = kLoader.LoadInt_e( "camera_event_id", 0 );

	LoadMapAreaWeapon( kLoader );
	LoadMapDummyObject( kLoader );

	kLoader.SetTitle_e( "collision" );
	m_bMonsterVsMonsterCollision = kLoader.LoadBool_e( "monster_vs_monster", true );

	kLoader.SetTitle_e( "proto_type" );
	m_bProtoTypeMode = kLoader.LoadBool_e( "proto_mode", false );
	m_bTeamAttackMode= kLoader.LoadBool_e( "team_attack", false );
	m_bTeamDamageMode= kLoader.LoadBool_e( "team_damage", false );
	if( m_bProtoTypeMode )
	{
		LoadNPC();
	}

	// 대기자 용병 위치 
	kLoader.SetTitle_e( "team_waiting" );

	if( P2PNetwork::IsNetworkPlaying() )
	{
		int iMaxWaitingPos = kLoader.LoadFloat_e( "max_pos", 0 );

		char szStartX[MAX_PATH];
		char szStartZ[MAX_PATH];

		m_vWaitingPos.clear();
		m_vWaitingPos.reserve( iMaxWaitingPos );
		for( int i=0 ; i<iMaxWaitingPos; i++ )
		{
			wsprintf_e(szStartX, "start%d_x", i+1);
			wsprintf_e(szStartZ, "start%d_z", i+1);

			D3DXVECTOR3 vWaitingPos;
			vWaitingPos.x  = kLoader.LoadFloat( szStartX, 0.0f );
			vWaitingPos.z  = kLoader.LoadFloat( szStartZ, 0.0f );
			m_vWaitingPos.push_back(vWaitingPos);
		}
	}
}

void ioBattleMode::LoadTagZone()
{
	const char *pMapINIFile = GetModeMapINIFileName( m_ModeType, m_iModeSubNum, m_iModeMapIndex );
	ioINILoader kLoader( pMapINIFile );

	// 유저 탈출
	m_TagZoneList.clear();
	kLoader.SetTitle_e( "TagZone" );
	int i = 0;
	int iMaxZone = kLoader.LoadInt_e( "MaxZone", 0 );
	for(i = 0;i < iMaxZone;i++)
	{
		TagZone kZone;
		char szKey[MAX_PATH], szBuf[MAX_PATH];

		sprintf_e( szKey, "zone%d_pos_x", i + 1 );
		kZone.m_vZonePos.x = kLoader.LoadFloat( szKey, 0.0f );
		sprintf_e( szKey, "zone%d_pos_y", i + 1 );
		kZone.m_vZonePos.y = kLoader.LoadFloat( szKey, 0.0f );
		sprintf_e( szKey, "zone%d_pos_z", i + 1 );
		kZone.m_vZonePos.z = kLoader.LoadFloat( szKey, 0.0f );

		//kZone.m_vZonePos.y = m_pCreator->GetMapHeightByRevial( kZone.m_vZonePos.x, kZone.m_vZonePos.z );

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

		m_TagZoneList.push_back( kZone );
	}

	// 태그 존 생성
	//m_pCreator->SetMapCustomObject( true, -1 );

	// 마법진 위치 
	m_CurrentTagZone.Init();
	//UIZoneCollisionTime( 0 );
	//if( COMPARE( m_dwCurrentTurnHelpID, 0, (DWORD)m_TurnHelpList.size() ) )
	//{
	//	TurnHelpList &rkTurnHelp = m_TurnHelpList[m_dwCurrentTurnHelpID];
	//	if( COMPARE( rkTurnHelp.m_dwZoneIndex, 0, (DWORD)m_MagicZoneList.size() ) )
	//	{
	//		m_CurrentMagicZone = m_MagicZoneList[rkTurnHelp.m_dwZoneIndex];
	//		m_CurrentMagicZone.m_dwCurCollisionTime = FRAMEGETTIME();
	//		m_CurrentMagicZone.m_dwCurCollisionTimeEx = FRAMEGETTIME();
	//		if( !m_MagicZoneList[rkTurnHelp.m_dwZoneIndex].m_szStartSound.IsEmpty() )
	//			g_SoundMgr.PlaySound( m_MagicZoneList[rkTurnHelp.m_dwZoneIndex].m_szStartSound );
	//		UIZoneCollisionTime( m_CurrentMagicZone.m_dwCollisionTime );
	//	}

	//	if( rkTurnHelp.m_dwEventSceneEffect != 0 )
	//	{
	//		m_pCreator->SetMapCustomEffect( false, rkTurnHelp.m_dwEventSceneEffect );
	//	}
	//}
	int iIndex = m_TagZoneList.size() - 1;
	if(iIndex >= 0)
	{
		m_CurrentTagZone = m_TagZoneList[iIndex];
		m_CurrentTagZone.m_dwCurCollisionTime = FRAMEGETTIME();
		m_CurrentTagZone.m_dwCurCollisionTimeEx = FRAMEGETTIME();
		if( !m_TagZoneList[iIndex].m_szStartSound.IsEmpty() )
			g_SoundMgr.PlaySound( m_TagZoneList[iIndex].m_szStartSound );
	}

	// 예외처리 : 마법진이 없다?
	if( m_CurrentTagZone.m_dwCollisionTime == 0 )
	{
		ioHashStringVec kCharReocrdNameList;
		//FindBaseCharRecordNameList( kCharReocrdNameList );
		int iCharCnt = kCharReocrdNameList.size();
		for(int i = 0;i < iCharCnt;i++)
		{
			BattleModeRecord *pRecord = FindRecordCharName( kCharReocrdNameList[i] );
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
}

void ioBattleMode::ProcessMode()
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
		g_EventMgr.Process();
		ProcessTagZoneCollision();
		break;
	case MS_RESULT:
		ProcessResultState();
		break;
	}
}

void ioBattleMode::ProcessGUI()
{
	//UpdateMiniMapSpotPoint();
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

void ioBattleMode::ProcessTagZoneCollision()
{
	if( m_bRoundSetEnd ) return;

	ioHashStringVec kCharReocrdNameList;
	FindBaseCharRecordNameList( kCharReocrdNameList );
	int iCharCnt = kCharReocrdNameList.size();
	for(int i = 0;i < iCharCnt;i++)
	{
		BattleModeRecord *pRecord = FindRecordCharName( kCharReocrdNameList[i] );
		if( pRecord == NULL ) continue;
		if( pRecord->pChar == NULL ) continue;
		CharState eCharState = pRecord->pChar->GetState();

		if( pRecord->pChar->GetState() == CS_VIEW  ||
			pRecord->pChar->GetState() == CS_DIE   ||
			pRecord->pChar->GetState() == CS_OBSERVER ||
			pRecord->pChar->GetState() == CS_LOADING )
		{
			continue;
		}
		for(int j = 0; j < (int)m_TagZoneList.size(); j++)
		{

			D3DXVECTOR3 vDiff = m_TagZoneList[j].m_vZonePos - pRecord->pChar->GetWorldPosition();
			vDiff.y = 0.0f;
			float fDistSq = D3DXVec3LengthSq( &vDiff );		
			float fCollision = m_TagZoneList[j].m_fCollisionRange * m_TagZoneList[j].m_fCollisionRange;

			if( fDistSq < fCollision ) // 
			{
				// 태그하려고 가는 자기 자신만 아래 패킷을 보내야함 
				if( pRecord->pChar->GetCharName() == g_MyInfo.GetPublicID() 
					&& g_BattleRoomMgr.CheckUserBattleFightFlag(pRecord->pChar->GetCharName()) )
				{
					ioHashString szWaitUser = g_BattleRoomMgr.GetUserNameByWaitIndex( j + 1 );
					ioBaseChar* pWaitChar = m_pCreator->GetBaseChar(szWaitUser);
					//LOG.PrintTimeAndLog( 0, "%s - Enter Tag Zone[ %d ], Waitter TagState : %d", __FUNCTION__, j, g_BattleRoomMgr.GetBattleModeTag(szWaitUser) );
					if(pWaitChar && g_BattleRoomMgr.GetBattleModeTag(szWaitUser) == 1
						&& g_BattleRoomMgr.CheckSameTeam(szWaitUser, g_MyInfo.GetPublicID())	// 대기자 태그 상태 값이 1(태그 신청)이면 아래 패킷 보냄 (조건 추가 : 같은 팀인지 확인)
						&& !g_BattleRoomMgr.CheckUserEntryState( g_MyInfo.GetPublicID() ))		// 태그 수락자가 난입 상태가 아니어야함
					{
						// 태그 승낙 패킷 전송
						ioHashString szEmpty;
						int iMyCharHPRate = (int)((pRecord->pChar->GetHP().m_fCurValue / pRecord->pChar->GetHP().m_fMaxValue) * 100.0f);
						int iWaitUserHPRate = (int)((pWaitChar->GetHP().m_fCurValue / pWaitChar->GetHP().m_fMaxValue) * 100.0f);
						SP2Packet kPacket( CTPK_BATTLE_MODE_TAG_ACCEPT );
						kPacket << g_MyInfo.GetPublicID() << iMyCharHPRate;
						kPacket << szWaitUser << iWaitUserHPRate;
						TCPNetwork::SendToServer( kPacket );
						//if( g_BattleRoomMgr.GetBattleModeTag(szWaitUser) ==  )
						g_BattleRoomMgr.SwitchBattleModeTag(szWaitUser);		// 상태값 이제 2가 됨
						/////////////////////////
						TagStateInitInfo info;
						info.m_dwEndTime = FRAMEGETTIME() + 1500;
						info.m_szWaitUser = szWaitUser;
						m_TagStateList.push_back( info );
						LOG.PrintTimeAndLog( 0, "%s - List Insert - TagWaiter UserName : %s", __FUNCTION__, szWaitUser.c_str() );
						////////////////////////////////
						//LOG.PrintTimeAndLog( 0, "%s - Send CTPK_BATTLE_MODE_TAG_ACCEPT - MyCharName : %s, szWaitUser : %s", 
						//	__FUNCTION__, g_MyInfo.GetPublicID().c_str(), szWaitUser.c_str() );
						//g_BattleRoomMgr.SetUserBattleFightFlag(pRecord->pChar->GetCharName(), false);
						// 이 시점부터 태그 교체되기 전 (BATTLE_MODE_TAG_ACCEPT_OK 패킷 받기 전) HP가 닮아서 죽었을때 Die패킷 보내게되면 서버에서 문제가 생겨
						// 먼저 용병 상태값 변경
						pWaitChar->SetState( CS_BATTLE_WAITING );
					}
				}
			}

		}
	}
	kCharReocrdNameList.clear();	
}

void ioBattleMode::FindBaseCharRecordNameList( ioHashStringVec &rkRecordNameList )
{
	rkRecordNameList.clear();
	int iCharCnt = m_vCharRecordList.size();
	for( int i=0 ; i<iCharCnt ; i++ )
	{
		BattleModeRecord &rkRecord = m_vCharRecordList[i];
		if( rkRecord.pChar == NULL ) continue;

		rkRecordNameList.push_back( rkRecord.szName );
	}
}

//
void ioBattleMode::UpdateTimeScoreWnd()
{
	BattleModeTimeGaugeWnd *pTimeGauge = dynamic_cast< BattleModeTimeGaugeWnd * >( g_GUIMgr.FindWnd( BATTLE_MODE_TIME_GAUGE_WND ) );
	if( pTimeGauge == NULL ) return;

	bool bRedWarning = false;
	if( GetRedKillRate() >= m_fScoreWarningRate )
		bRedWarning = true;

	bool bBlueWarning = false;
	if( GetBlueKillRate() >= m_fScoreWarningRate )
		bBlueWarning = true;

	//float 값의 0.7이 0.699999로 나오는 현상 해결.(0.7 또는 0.9)
	//UI상에서 제대로 보이게 timer , teamscore, centerinfo 창에 하드코딩
	float fBlueTempRate = GetBlueKillRate() + 0.0000001f;
	float fRedTempRate = GetRedKillRate() + 0.0000001f;

	int iBluePoint = (int)( fBlueTempRate * FLOAT100);
	int iRedPoint = (int)( fRedTempRate * FLOAT100 );

	DWORD dwTimeGap = 0;
	switch( m_ModeState )
	{
	case MS_READY:
		{
			pTimeGauge->UpdateTime( m_dwRoundDuration, m_dwRoundDuration );
		}
		break;
	case MS_PLAY:
		{
			dwTimeGap = FRAMEGETTIME() - m_dwPlayStartTime;
			if( m_dwCurRoundDuration > dwTimeGap )
			{
				pTimeGauge->UpdateTime( m_dwCurRoundDuration - dwTimeGap, m_dwRoundDuration );

				ProcessWarningSound( m_dwCurRoundDuration );

				if( bRedWarning || bBlueWarning )
				{
					ProcessScoreWarningSound( true );
					g_ModeUIController.SetTeamDeathWarning( GetBlueKillRate(), GetRedKillRate() );
				}
				else
					ProcessScoreWarningSound( false );
			}
			else
			{			
				if( g_BattleRoomMgr.IsTournamentModeType() )
				{
					dwTimeGap = FRAMEGETTIME() - m_dwSuddenDaathStartTime;
					pTimeGauge->UpdateSuddenDeathTime( m_dwSuddenDeathTime - dwTimeGap, m_dwSuddenDeathTime );
				}
				pTimeGauge->UpdateTime( 0, m_dwRoundDuration );
			}

			// 태그 1.5초 체크
			if(m_dwTagBtnDisableTime > 0)
			{
				if( dwTimeGap >= m_dwTagBtnDisableTime + m_dwStartTagBtnDisableTime )
				{		
					m_dwTagBtnDisableTime = 0;
					BattleModeFunctionWnd *pFunctionWnd = dynamic_cast<BattleModeFunctionWnd*>(g_GUIMgr.FindWnd( BATTLE_MODE_FUNCTION_WND ));
					if( pFunctionWnd )
						pFunctionWnd->ActiveTagBtn();
				}
			}

			DWORD dwCurTime = FRAMEGETTIME();
			TagUserInitInfoList::iterator userTagStateIter = m_TagStateList.begin();
			while(userTagStateIter != m_TagStateList.end())
			{
				if( dwCurTime > userTagStateIter->m_dwEndTime )// 삭제
				{
					LOG.PrintTimeAndLog( 0, "%s - (TimeOver) List Delete  - TagWaiter UserName : %s", __FUNCTION__, userTagStateIter->m_szWaitUser.c_str() );
					g_BattleRoomMgr.SwitchBattleModeTag(userTagStateIter->m_szWaitUser, TAG_POSSIBLE_REQ);		
					m_TagStateList.erase( userTagStateIter++ );
				}
				else		
					userTagStateIter++;
			}
		}
		break;
	case MS_RESULT:
		{
			pTimeGauge->UpdateTime( 0, m_dwRoundDuration );
		}
		break;
	}

	if( m_ModeState != MS_PLAY )
	{
		ProcessScoreWarningSound( false );
	}
}

void ioBattleMode::UpdateRoundStateWnd()
{
	if( g_MyInfo.IsSortContribute() )
	{
		DWORD dwTimeGap = FRAMEGETTIME() - m_dwPlayStartTime;
		if( m_dwCurRoundDuration > dwTimeGap )
			ContributeRoundAllPercent( m_dwRoundDuration - (m_dwCurRoundDuration - dwTimeGap), m_dwRoundDuration );
		else
			ContributeRoundAllPercent( m_dwRoundDuration, m_dwRoundDuration );

		BattleModeRecord *pRecord = FindRecordCharName( g_MyInfo.GetPublicID() );
		if( pRecord )
			g_MyInfo.SetMyContribute( pRecord->Contribute.m_iPrevContributePer );
	}

	if( m_ModeState != MS_PLAY )
	{
		if( g_GUIMgr.IsShow( ROUND_STATE_MAIN_WND ) )
		{
			g_GUIMgr.HideWnd( ROUND_STATE_MAIN_WND );		

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
			for( int i = 0 ; i < GetRecordCharCnt(); i++ )
			{
				RoundInfo kInfo;
				if( GetRoundInfo( i, kInfo ) )
					pRoundStateWnd->AddRoundInfo( kInfo );
			}

			//float 값의 0.7이 0.699999로 나오는 현상 해결.(0.7 또는 0.9)
			//UI상에서 제대로 보이게 timer , teamscore, centerinfo 창에 하드코딩
			float fBlueTempRate = GetBlueKillRate() + 0.0000001f;
			float fRedTempRate = GetRedKillRate() + 0.0000001f;

			int iBluePoint = (int)( fBlueTempRate * FLOAT100);
			int iRedPoint = (int)( fRedTempRate * FLOAT100 );
			pRoundStateWnd->SetTeamScore( iBluePoint, iRedPoint );
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

void ioBattleMode::ProcessReadyState()
{
	// 관전자 위치 값 셋팅
	int iCharCnt = m_vCharRecordList.size();
	for( int i=0 ; i<iCharCnt ; i++ )
	{
		if( m_vCharRecordList[i].pChar )
		{
			// 관전자 위치 값 수정
			if( g_BattleRoomMgr.IsObserverUser( g_MyInfo.GetPublicID() ) )
			{
				ioBaseChar* pMyObserverChar = GetOwnerChar();
				int iBattleOrder = g_BattleRoomMgr.GetUserBattleOrder( m_vCharRecordList[i].pChar->GetCharName() );
				if( pMyObserverChar && iBattleOrder == BATTLE_ORDER_FIRST && m_vCharRecordList[i].pChar->GetTeam() == TEAM_BLUE )
				{
					pMyObserverChar->SetWorldPosition( m_vCharRecordList[i].pChar->GetWorldPosition() );
					break;
				}
			}
		}
	}

	if( P2PNetwork::IsNetworkPlaying() )	return;
	if( m_dwReadyStartTime + 4000 > FRAMEGETTIME() )
		return;

	ChangeCurFighterView();

	SetModeState( MS_PLAY );
}

void ioBattleMode::ProcessPlayState()
{
	UpdateSoldierRevivalPenalty();

	m_pCreator->NotifySpringDampUseSet( true );

	if( !P2PNetwork::IsNetworkPlaying() )
	{
		CheckRoundEnd();
	}
}

void ioBattleMode::CheckRoundEnd()
{
}

int ioBattleMode::GetTeamPlayerCount( TeamType eTeam )
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

float ioBattleMode::GetBlueKillRate()
{
	float fScoreKillPoint = GetScoreKillPoint( TEAM_RED );

	if( fScoreKillPoint <= 0) 
		return 0.0f;

	if( m_iBlueKillPoint > 0 )
	{
		float fRate = (float)m_iBlueKillPoint / fScoreKillPoint;
		return min( fRate, FLOAT1 );
	}

	return 0.0f;
}

float ioBattleMode::GetRedKillRate()
{
	float fScoreKillPoint = GetScoreKillPoint( TEAM_BLUE );

	if( fScoreKillPoint <= 0) 
		return 0.0f;

	if( m_iRedKillPoint > 0 )
	{
		float fRate = (float)m_iRedKillPoint / fScoreKillPoint;
		return min( fRate, FLOAT1 );
	}

	return 0.0f;
}

float ioBattleMode::GetScoreKillPoint( TeamType eTeam )
{
	float fScoreKillPoint = GetTeamPlayerCount( eTeam ) * m_fWinScoreConstant * 100;

	return fScoreKillPoint;
}

void ioBattleMode::ProcessResultState()
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

	g_BattleRoomMgr.ClearBattleModeInfoAtEndOfBattleMode();

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

void ioBattleMode::RenderPreGUI()
{
	if( m_ModeState == MS_PLAY )
	{
		RenderPushEnableCursor();
	}
}

void ioBattleMode::RenderAfterGUI()
{
	
}

void ioBattleMode::AddNewRecord( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	BattleModeRecord *pBattleRecord = FindRecordCharName(pChar->GetCharName());
	if( !pBattleRecord )
	{
		BattleModeRecord kRecord;
		kRecord.pChar  = pChar;
		kRecord.szName = pChar->GetCharName();
		m_vCharRecordList.push_back( kRecord );		

		UpdateUserRank();

		if( pChar )
		{
			if( !g_BattleRoomMgr.IsObserverUser(pChar->GetCharName()) )
				g_AwardMgr.AddNewAwardUser( pChar->GetCharName() );
			else if( g_ShuffleRoomMgr.IsShuffleRoom() )
				g_AwardMgr.AddNewAwardUser( pChar->GetCharName() );
			else if( g_RankBattleMgr.IsRankBattlePlaying() )
				g_AwardMgr.AddNewAwardUser( pChar->GetCharName() );
		}
	}
	else
	{
		pBattleRecord->pChar = pChar;
		ioHashString szCharName = pChar->GetCharName();
		int iBattleOrder = g_BattleRoomMgr.GetUserBattleOrder( szCharName );
		if(iBattleOrder != BATTLE_ORDER_FIRST)
		{
			// 대기할 용병 맵 중앙을 보게 회전시킴
			RotateSoldierToCenter( pChar );
		}
	}

	bool bAllCheck = true;
	mapFirstCheck::iterator iter = m_mapFirstCheck.begin();
	for(iter = m_mapFirstCheck.begin(); iter != m_mapFirstCheck.end(); iter++ )
	{
		if( (*iter).second == false )
			bAllCheck = false;
	}
	if( bAllCheck )
		return;

	// Second Check
	for(iter = m_mapFirstCheck.begin(); iter != m_mapFirstCheck.end(); iter++ )
	{
		if( (*iter).first == pChar->GetCharName() )
		{
			(*iter).second = true;
		}
	}


	int iTotalUserCnt = g_BattleRoomMgr.GetTotalUserCount();
	ioHashString szUserName = pChar->GetCharName();
	// 팀 섞기 옵션이 True일 경우에 대비해 팀 상태값 갱신
	g_BattleRoomMgr.SetBattleTeam( szUserName, pChar->GetTeam() );
	if( !szUserName.IsEmpty() )
	{
		//ioBaseChar* pChar = m_pCreator->GetBaseChar( szUserName );
		if( pChar )
		{
			int iBattleOrder = g_BattleRoomMgr.GetUserBattleOrder( szUserName );
			bool bFirstOrder = true;
			if(iBattleOrder != BATTLE_ORDER_FIRST)
			{
				m_bFirstBlockCameraView = true;

				bFirstOrder = false;
				// 대기 자리 인덱스 셋팅
				TeamType eTeam = (TeamType)g_BattleRoomMgr.GetUserTeam( szUserName );
				int iIndex = 0;

				if( eTeam == TEAM_BLUE )
				{
					if(iBattleOrder == BATTLE_ORDER_SECOND)
						g_BattleRoomMgr.SetWaitIndex( szUserName, BATTLE_BLUE_TOP );
					else if(iBattleOrder == BATTLE_ORDER_THIRD)
						g_BattleRoomMgr.SetWaitIndex( szUserName, BATTLE_BLUE_BOTTOM );
				}
				else if( eTeam == TEAM_RED )
				{
					if(iBattleOrder == BATTLE_ORDER_SECOND)
						g_BattleRoomMgr.SetWaitIndex( szUserName, BATTLE_RED_TOP );
					else if(iBattleOrder == BATTLE_ORDER_THIRD)
						g_BattleRoomMgr.SetWaitIndex( szUserName, BATTLE_RED_BOTTOM );
				}

				BattleModeFunctionWnd *pFunctionButtonWnd = dynamic_cast< BattleModeFunctionWnd * >( g_GUIMgr.FindWnd( BATTLE_MODE_FUNCTION_WND ) );
				// 자신이 대기자라면 대기자 버튼UI를 띄움
				if( g_MyInfo.GetPublicID() == szUserName )
				{
					if( pFunctionButtonWnd )
					{
						pFunctionButtonWnd->ShowFunctionButton( true );
						pFunctionButtonWnd->SetPlayStage(m_pCreator);
					}
				}
				// 출전 중이 아니라는 표시
				g_BattleRoomMgr.SetUserBattleFightFlag( szUserName, false );
				// 관전자가 아니면 상태값 변경, 관전자라면 대기 위치 인덱스 0 (TEAM_NONE일때 관전자라고 판단..)
				pChar->SetState( CS_BATTLE_WAITING );
				if( g_BattleRoomMgr.IsObserverUser( pChar->GetCharName() ) )
				{
					if( pFunctionButtonWnd && pChar->GetCharName() == g_MyInfo.GetPublicID() )
						pFunctionButtonWnd->ShowFunctionButton( false );

					pChar->SetObserverState();
					g_BattleRoomMgr.SetWaitIndex( szUserName, 0 );
				}
			}
			else
			{
				TeamType eTeam = (TeamType)g_BattleRoomMgr.GetUserTeam( szUserName );
				//LOG.PrintTimeAndLog( 0, "%s -  FirstUser - Name : %s, eTeam : %d, iBattleOrder : %d", 
				//	__FUNCTION__, szUserName.c_str(), (int)eTeam, iBattleOrder );
				// 대기 자리 인덱스 초기화 (출전했을때 상태 0으로 해놓고 아래서 변경) - SYH
				g_BattleRoomMgr.SetWaitIndex( szUserName, 0 );
				g_BattleRoomMgr.SetUserBattleFightFlag( szUserName, true );

				AddBattleModeUserInfoToUI( pChar );

				// 관전자 위치 값 수정
				if( g_BattleRoomMgr.IsObserverUser( g_MyInfo.GetPublicID() ) )
				{
					ioBaseChar* pMyObserverChar = GetOwnerChar();
					if( pMyObserverChar )
						pMyObserverChar->SetWorldPosition( pChar->GetWorldPosition() );
				}
			}

			/////////////////////////////////////

			if(bFirstOrder)
				m_pCreator->IncreaseTeamCntForBalance( pChar->GetTeam() );

			// 화면 양 사이드에 나타낼 유저 상태
			BattleModeTimeGaugeWnd *pTimeGauge = dynamic_cast< BattleModeTimeGaugeWnd * >( g_GUIMgr.FindWnd( BATTLE_MODE_TIME_GAUGE_WND ) );
			if( pTimeGauge )
			{
				TeamType eTeam = (TeamType)g_BattleRoomMgr.GetUserTeam( szUserName );
				if( eTeam == TEAM_BLUE)
				{
					BattleModeBlueSideUserInfoWnd *pBattleModeBlueSide = dynamic_cast< BattleModeBlueSideUserInfoWnd * >( g_GUIMgr.FindWnd( BATTLE_MODE_BLUE_USER_INFO_WND ) );
					if( pBattleModeBlueSide )
						pBattleModeBlueSide->SetBattleModeUser( pChar, g_BattleRoomMgr.GetUserBattleWaitPosIdx( szUserName ) );
				}
				else if( eTeam == TEAM_RED)
				{
					BattleModeRedSideUserInfoWnd *pBattleModeRedSide = dynamic_cast< BattleModeRedSideUserInfoWnd * >( g_GUIMgr.FindWnd( BATTLE_MODE_RED_USER_INFO_WND ) );
					if( pBattleModeRedSide )
						pBattleModeRedSide->SetBattleModeUser( pChar, g_BattleRoomMgr.GetUserBattleWaitPosIdx( szUserName ) );
				}
			}
		}
	}

	if( AllFirstUserCheck() )
	{
		BriefingWnd *pBriefingWnd = static_cast<BriefingWnd*>(g_GUIMgr.FindWnd( BRIEFING_WND ));
		if( pBriefingWnd )
			pBriefingWnd->SetStartBriefingForBattleMode();
	}

	m_bUserSetting = true;
}

void ioBattleMode::RemoveRecord( const ioHashString &rkName )
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

	BattleModeTimeGaugeWnd *pTimeGauge = dynamic_cast< BattleModeTimeGaugeWnd * >( g_GUIMgr.FindWnd( BATTLE_MODE_TIME_GAUGE_WND ) );
	if( pTimeGauge )
		pTimeGauge->RemoveCharSeqInfo( rkName );

	UpdateUserRank();
}


void ioBattleMode::RemoveRecord( int nIndex )
{
	int iCharCnt = m_vCharRecordList.size();
	if( iCharCnt > nIndex )
	{
		m_vCharRecordList.erase( m_vCharRecordList.begin() + nIndex );
		UpdateUserRank();
	}
}


ioBaseChar* ioBattleMode::GetRecordChar( int i )
{
	if( COMPARE( i, 0, GetRecordCharCnt() ) )
		return m_vCharRecordList[i].pChar;

	return NULL;
}

ioBaseChar* ioBattleMode::GetRecordChar( const ioHashString &rkName )
{
	BattleModeRecord *pRecord = FindRecordCharName( rkName );
	if( pRecord )
		return pRecord->pChar;

	return NULL;
}

int ioBattleMode::GetRecordCharCnt() const
{
	return m_vCharRecordList.size();
}

ModeRecord* ioBattleMode::FindModeRecord( int iIdx )
{
	if( COMPARE( iIdx, 0, GetRecordCharCnt() ) )
		return &m_vCharRecordList[iIdx];

	return NULL;
}

ModeRecord* ioBattleMode::FindModeRecordCharName( const ioHashString &rkName )
{
	return (ModeRecord*)FindRecordCharName( rkName );
}

BattleModeRecord* ioBattleMode::FindRecordCharName( const ioHashString &rkName )
{
	int iCharCnt = m_vCharRecordList.size();
	for( int i=0 ; i<iCharCnt ; i++ )
	{
		if( m_vCharRecordList[i].szName == rkName )
			return &m_vCharRecordList[i];
	}

	return NULL;
}

void ioBattleMode::SetRoundEndInfo( WinTeamType eTeam, bool bRoundSetEnd )
{
	ioPlayMode::SetRoundEndInfo( eTeam, bRoundSetEnd );

	// 
	if( bRoundSetEnd && m_pCreator )
	{
		ioBaseChar *pOwner = m_pCreator->GetOwnerChar();
		if( pOwner )
		{
			int iBluePoint = (int)( m_fFinalBluePoint * FLOAT100 );
			int iRedPoint = (int)( m_fFinalRedPoint * FLOAT100 );

			WinTeamType eWinTeamType = m_WinTeamType;
			if( m_iBlueRoundWinCnt > m_iRedRoundWinCnt )
				eWinTeamType = WTT_BLUE_TEAM;
			else if( m_iBlueRoundWinCnt < m_iRedRoundWinCnt )
				eWinTeamType = WTT_RED_TEAM;
			else 
			{
				iBluePoint = max( iBluePoint, iRedPoint );
				iRedPoint  = max( iBluePoint, iRedPoint );
				eWinTeamType = WTT_DRAW;
			}

			ioComplexStringPrinter kPrinter;
			g_ChatMgr.PresetChatComplexString( MSG_INFO_CHAT, kPrinter );

			kPrinter.SetTextColor( TCT_DEFAULT_BLUE );
			kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(1), iBluePoint );
			kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
			kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), " vs" );
			kPrinter.SetTextColor( TCT_DEFAULT_RED );
			kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(3), iRedPoint );
			kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
			kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(4));

			TeamType eViewTeam = pOwner->GetTeam();            
			if( eWinTeamType == WTT_DRAW )
				eViewTeam = TEAM_NONE;
			else if( g_BattleRoomMgr.IsObserverUser( pOwner->GetCharName() ) || m_pCreator->IsStealthMode() )
			{
				if( eWinTeamType == WTT_BLUE_TEAM )
					eViewTeam = TEAM_BLUE;				
				else
					eViewTeam = TEAM_RED;				
			}		

			// 팀 표시
			if( eViewTeam == TEAM_BLUE )
			{
				kPrinter.SetTextColor( TCT_DEFAULT_BLUE );
				kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(5) );
			}
			else if( eViewTeam == TEAM_RED )
			{
				kPrinter.SetTextColor( TCT_DEFAULT_RED );
				kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(6) );
			}

			kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
			if( IsWinTeam( eWinTeamType, eViewTeam ) )
				kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(7));
			else if( eWinTeamType == WTT_DRAW )
				kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(8));
			else
				kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(9));
			g_ChatMgr.SetChatComplexString( STR(2), kPrinter );
		}
	}
}

void ioBattleMode::ShowEndScoreWnd( bool bShow )
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

bool ioBattleMode::GetRoundInfo( int iIndex, RoundInfo &rkInfo )
{
	if( ioPlayMode::GetRoundInfo( iIndex, rkInfo ) )
	{
		BattleModeRecord *pRecord = (BattleModeRecord*)FindModeRecord( iIndex );
		if( !pRecord )	return false;
		if( !pRecord->pChar ) return false;

		rkInfo.m_iContribute	= pRecord->Contribute.m_iContribute;
		rkInfo.m_iContributePer = pRecord->Contribute.m_iPrevContributePer;		
		return true;
	}
	return false;
}

bool ioBattleMode::GetResultInfo( int iIndex, ResultInfo &rkInfo )
{
	if( ioPlayMode::GetResultInfo( iIndex, rkInfo ) )
	{
		BattleModeRecord *pRecord = (BattleModeRecord*)FindModeRecord( iIndex );
		if( !pRecord )	return false;
		if( !pRecord->pChar ) return false;

		rkInfo.m_iContribute = pRecord->Contribute.m_iContribute;
		rkInfo.m_iContributePer = pRecord->Contribute.m_iPrevContributePer;
		return true;
	}
	return false;
}

bool ioBattleMode::GetFinalInfo( int iIndex, FinalInfo &rkInfo )
{
	if( ioPlayMode::GetFinalInfo( iIndex, rkInfo ) )
	{
		BattleModeRecord *pRecord = (BattleModeRecord*)FindModeRecord( iIndex );
		if( !pRecord )	return false;
		if( !pRecord->pChar ) return false;

		rkInfo.m_iContribute = pRecord->Contribute.m_iContribute;
		rkInfo.m_iContributePer = pRecord->Contribute.m_iPrevContributePer;
		return true;
	}
	return false;
}

void ioBattleMode::ApplyModeInfo( SP2Packet &rkPacket )
{
	rkPacket >> m_iCurRoundCount;
	rkPacket >> m_iMaxRoundCount;

	rkPacket >> m_dwRoundDuration;

	rkPacket >> m_iBlueRoundWinCnt;
	rkPacket >> m_iRedRoundWinCnt;

	rkPacket >> m_iBluePosArray;
	rkPacket >> m_iRedPosArray;

	rkPacket >> m_fWinScoreConstant;

	rkPacket >> m_iRedKillPoint;
	rkPacket >> m_iBlueKillPoint;

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

void ioBattleMode::ApplyExtraModeInfo( SP2Packet &rkPacket )
{
	rkPacket >> m_fWinScoreConstant;

	rkPacket >> m_iRedKillPoint;
	rkPacket >> m_iBlueKillPoint;
}

void ioBattleMode::ApplyCharModeInfo( const ioHashString &rkName, SP2Packet &rkPacket, bool bDieCheck )
{
	bool bRecord;
	rkPacket >> bRecord;

	if( !bRecord ) return;

	BattleModeRecord *pRecord = FindRecordCharName( rkName );
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

		bool bCatch;
		rkPacket >> bCatch;

		//BattleModeTimeGaugeWnd *pTimeGauge = dynamic_cast< BattleModeTimeGaugeWnd * >( g_GUIMgr.FindWnd( BATTLE_MODE_TIME_GAUGE_WND ) );
		//if( pTimeGauge )
		//{
		//	int iBattleOrder = g_BattleRoomMgr.GetUserBattleOrder(pRecord->pChar->GetCharName());
		//	if(iBattleOrder == BATTLE_ORDER_FIRST)
		//	{
		//		TeamType eTeam = (TeamType)g_BattleRoomMgr.GetUserTeam(pRecord->pChar->GetCharName());
		//		pTimeGauge->SetBattleModeUser( pRecord->szName, eTeam );
		//	}
		//}
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

		bool bCatch;
		rkPacket >> bCatch;
	}
}

void ioBattleMode::NotifyEntityDie( ioPlayEntity *pEntity, ioPlayEntity *pKiller, ioBaseChar *pBestAttacker,
										 float fLastRate, float fBestRate )
{	
	ioBaseChar *pDieChar = ToBaseChar( pEntity );
	ioBaseChar *pKillChar = ToBaseChar( pKiller );

	// 순서 주의 이모티콘 출력때문
	UpdateKillDeathRecord( pEntity, pKiller, pBestAttacker );
	OnNotifyCharDie( pDieChar, pKillChar, pBestAttacker, fLastRate, fBestRate );
}

void ioBattleMode::NotifyCharDropDie( ioBaseChar *pDieChar, ioBaseChar *pKillChar, ioBaseChar *pBestAttacker,
										   float fLastRate, float fBestRate )
{
	// 순서 주의 이모티콘 출력때문
	UpdateDropDieRecord( pDieChar, pKillChar, pBestAttacker );
	OnNotifyCharDie( pDieChar, pKillChar, pBestAttacker, fLastRate, fBestRate );
}

void ioBattleMode::OnNotifyCharDie( ioBaseChar *pDieChar, ioBaseChar *pKillChar, ioBaseChar *pBestAttacker,
										 float fLastRate, float fBestRate )
{
	if( !pDieChar )
	{
		LOG.PrintTimeAndLog( 0, "ioBattleMode::OnNotifyCharDie - Entity is Not Char" );
		return;
	}

	BattleModeRecord *pDieRecord = FindRecordCharName( pDieChar->GetCharName() );
	if( !pDieRecord )
	{
		LOG.PrintTimeAndLog( 0, "ioBattleMode::OnNotifyCharDie - %s Not Exist Char",
								pDieChar->GetCharName().c_str() );
		return;
	}

	ioHashString szKillCharName;
	if( pKillChar && pKillChar->GetTeam() != pDieChar->GetTeam() )
	{
		szKillCharName = pKillChar->GetCharName();
		if( pBestAttacker && pBestAttacker->GetCharName() == pKillChar->GetCharName() )
		{
			pKillChar->SetKOHPRecover( m_fKORecoverRate * 2.0f, m_PerfectRecoverEffect );
			
			if( m_pCreator && m_pCreator->CheckRoomKOEffect() )
			{
				SetRecoveryGauge( pKillChar, fLastRate );
				//pKillChar->AddNewBuff( m_KOBuff, "", "", NULL );
			}
		}
		else
		{
			pKillChar->SetKOHPRecover( m_fKORecoverRate, m_KORecoverEffect );
			
			if( m_pCreator && m_pCreator->CheckRoomKOEffect() )
			{
				SetRecoveryGauge( pKillChar, fLastRate );
				//pKillChar->AddNewBuff( m_KOBuff, "", "", NULL );
			}

			if( pBestAttacker )
			{
				pBestAttacker->SetKOHPRecover( m_fKORecoverRate, m_KORecoverEffect );
				
				if( m_pCreator && m_pCreator->CheckRoomKOEffect() )
				{
					SetRecoveryGauge( pBestAttacker, fBestRate );
					//pBestAttacker->AddNewBuff( m_KOBuff, "", "", NULL );
				}

				if( pBestAttacker->IsOwnerChar() )
				{
					m_pCreator->StartScreenBlind( SBT_KILL, NULL, true );
				}
			}
		}

		if( m_ModeState != MS_RESULT )
		{
			if( pKillChar->GetTeam() == TEAM_RED )
			{
				m_iRedKillPoint += 100;
				SetKillPointEmoticon( TEAM_RED, szKillCharName );
			}
			else if( pKillChar->GetTeam() == TEAM_BLUE )
			{
				m_iBlueKillPoint += 100;
				SetKillPointEmoticon( TEAM_BLUE, szKillCharName );
			}
		}
	}
	else if( pKillChar && pKillChar->GetTeam() == pDieChar->GetTeam() )
	{
		// 자살 & 팀킬
		if( pKillChar->GetTeam() == TEAM_RED )
		{
			m_iBlueKillPoint += 100;
			SetKillPointEmoticon( TEAM_BLUE, szKillCharName );
		}
		else if( pKillChar->GetTeam() == TEAM_BLUE )
		{
			m_iRedKillPoint += 100;
			SetKillPointEmoticon( TEAM_RED, szKillCharName );
		}
	}
	else
	{
		// 자살
		if( pDieChar->GetTeam() == TEAM_RED )
		{
			m_iBlueKillPoint += 100;
			SetKillPointEmoticon( TEAM_BLUE, szKillCharName );
		}
		else if( pDieChar->GetTeam() == TEAM_BLUE )
		{
			m_iRedKillPoint += 100;
			SetKillPointEmoticon( TEAM_RED, szKillCharName );
		}
	}

	if( P2PNetwork::IsNetworkPlaying() )
	{
		if( g_MyInfo.GetPublicID() == pDieChar->GetCharName() )
		{
			g_MyInfo.SetCharDied( pDieChar->GetSelectCharArray(), true );
		}
	}

	if( pKillChar && pDieChar )
	{
		g_ModeUIController.SetTeamDeathKO( pKillChar->GetTeam(), pKillChar->GetCharName(), pDieChar->GetTeam(), pDieChar->GetCharName(), 
										   GetBlueKillRate(), GetRedKillRate() );

		if( !pKillChar->IsOwnerChar() && !pDieChar->IsOwnerChar() )
		{
			m_pCreator->StartScreenBlind( SBT_CATCH_KILL, NULL, true );
		}
	}
	else if( pDieChar )
	{
		g_ModeUIController.SetTeamDeathKO( TEAM_NONE, "", pDieChar->GetTeam(), pDieChar->GetCharName(), 
										   GetBlueKillRate(), GetRedKillRate() );
		if( !pDieChar->IsOwnerChar() )
		{
			m_pCreator->StartScreenBlind( SBT_CATCH_KILL, NULL, true );
		}
	}

	//For EndScene
	m_LastDieChar = pDieChar->GetCharName();

	if( pKillChar )
	{
		BattleModeRecord *pKillRecord = FindRecordCharName( pKillChar->GetCharName() );
		if( !pKillRecord )
		{
			LOG.PrintTimeAndLog( 0, "ioBattleMode::OnNotifyCharDie - %s Not Exist Kill Char",
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

void ioBattleMode::ReStartMode()
{
	BattleModeTimeGaugeWnd *pBattleModeTimeGauge = dynamic_cast< BattleModeTimeGaugeWnd * >( g_GUIMgr.FindWnd( BATTLE_MODE_TIME_GAUGE_WND ) );
	m_pCreator->SetScreenBlindDownState();

	D3DXVECTOR3 vStartPos;

	int iCharCnt = m_vCharRecordList.size();
	for( int i=0 ; i<iCharCnt ; i++ )
	{
		BattleModeRecord &rkRecord = m_vCharRecordList[i];
		if( !rkRecord.pChar ) continue;
		vStartPos = GetStartPos( rkRecord.pChar->GetTeam() );
		// 첫번째 출전이 아닐 경우 용병 생성 위치 수정
		if(g_BattleRoomMgr.GetUserBattleOrder(rkRecord.pChar->GetCharName()) != 1)
			vStartPos = GetWaitingPos(rkRecord.pChar, rkRecord.pChar->GetTeam());

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
			else if( g_ShuffleRoomMgr.IsShuffleRoom() )
				rkRecord.pChar->SetReadyState();
			else if( g_RankBattleMgr.IsRankBattlePlaying() )
				rkRecord.pChar->SetReadyState();
			else
				rkRecord.pChar->SetState( CS_OBSERVER );
		}
	}

	m_iBlueKillPoint = 0;
	m_iRedKillPoint = 0;

	m_dwCurRevivalTime = 0;
	m_dwCheckWarningTime = 0;
	m_bScoreWarning = false;
	m_bFirstSelect = true;

	// 라운드제
	m_bZeroHP = false;
	//TimeScoreWnd *pTimeScoreWnd = dynamic_cast<TimeScoreWnd *>( g_GUIMgr.FindWnd( TIME_SCORE_WND ) );
	//if(pTimeScoreWnd)
	//	pTimeScoreWnd->InitAniState();

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

D3DXVECTOR3 ioBattleMode::GetRevivalPos( ioBaseChar *pChar )
{
	D3DXVECTOR3 vPos = GetStartPos( pChar->GetTeam() );
	vPos.x += (float)( rand() % 100 ) / 20.0f;
	vPos.z += (float)( rand() % 100 ) / 20.0f;
	vPos.y = m_pCreator->GetMapHeightByRevial( vPos.x, vPos.z );

	return vPos;
}

D3DXVECTOR3 ioBattleMode::GetWaitingPos( ioBaseChar *pChar, TeamType eTeamType )
{
	int iBattleOrder = g_BattleRoomMgr.GetUserBattleOrder(pChar->GetCharName());

	D3DXVECTOR3 vWaitingPos;

	if(eTeamType == TEAM_BLUE)	
	{
		iBattleOrder -= BATTLE_ORDER_SECOND;
		if(iBattleOrder >= 0 && iBattleOrder < (int)m_vWaitingPos.size())
			vWaitingPos = m_vWaitingPos[iBattleOrder];
	}
	else if(eTeamType == TEAM_RED)
	{
		//iBattleOrder += 1;
		if(iBattleOrder >= 0 && iBattleOrder < (int)m_vWaitingPos.size())
			vWaitingPos = m_vWaitingPos[iBattleOrder];
	}
	vWaitingPos.y = m_pCreator->GetMapHeightByRevial( vWaitingPos.x, vWaitingPos.z );
	return vWaitingPos;
}

D3DXVECTOR3 ioBattleMode::GetWaitingPosByPosIdx( ioBaseChar *pChar )
{
	int iBattleWaitPosIdx = g_BattleRoomMgr.GetUserBattleWaitPosIdx(pChar->GetCharName());

	D3DXVECTOR3 vWaitingPos;

	//if(eTeamType == TEAM_BLUE)	
	//{
	//	if(iBattleWaitPosIdx >= 0 && iBattleWaitPosIdx < (int)m_vWaitingPos.size())
	//		vWaitingPos = m_vWaitingPos[iBattleWaitPosIdx];
	//}
	//else if(eTeamType == TEAM_RED)
	{
		//iBattleOrder += 1;
		iBattleWaitPosIdx -= 1;
		if(iBattleWaitPosIdx >= 0 && iBattleWaitPosIdx < (int)m_vWaitingPos.size())
			vWaitingPos = m_vWaitingPos[iBattleWaitPosIdx];
	}
	vWaitingPos.y = m_pCreator->GetMapHeightByRevial( vWaitingPos.x, vWaitingPos.z );
	return vWaitingPos;
}

void ioBattleMode::ContributeLOG()
{
	char szLog[MAX_PATH];

	int iCharCnt = m_vCharRecordList.size();
	for( int i=0 ;i<iCharCnt; i++ )
	{
		BattleModeRecord *pRecord = &m_vCharRecordList[i];
		if( pRecord->pChar )
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

void ioBattleMode::ContributeConvertTeam( TeamType eTeam )
{
	if( m_vContributeTable.size() < MAX_PLAYER )
		return;

	int i = 0;
	int iCharCnt = m_vCharRecordList.size();
	int iInfoCnt = 0;
	ContributeInfoList vInfo;

	//KO
	for(i = 0;i < iCharCnt;i++)
	{
		BattleModeRecord *pRecord = &m_vCharRecordList[i];
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
			BattleModeRecord *pRecord = &m_vCharRecordList[iArray];
			if( !pRecord->Contribute.m_bLastResultSync )
				pRecord->Contribute.m_iContribute = m_vContributeTable[vInfo[i].m_iRank];
			pRecord->Contribute.m_iKillRank = vInfo[i].m_iRank;
		}
	}
	vInfo.clear();

	//데미지 & 힐
	for(i = 0;i < iCharCnt;i++)
	{
		BattleModeRecord *pRecord = &m_vCharRecordList[i];
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
			BattleModeRecord *pRecord = &m_vCharRecordList[iArray];
			if( !pRecord->Contribute.m_bLastResultSync )
				pRecord->Contribute.m_iContribute += m_vContributeTable[vInfo[i].m_iRank] * 6;
			pRecord->Contribute.m_iDamageRank = vInfo[i].m_iRank;
		}
	}
	vInfo.clear();

	//유효 타격 횟수
	for(i = 0;i < iCharCnt;i++)
	{
		BattleModeRecord *pRecord = &m_vCharRecordList[i];
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
			BattleModeRecord *pRecord = &m_vCharRecordList[iArray];
			if( !pRecord->Contribute.m_bLastResultSync )
				pRecord->Contribute.m_iContribute += m_vContributeTable[vInfo[i].m_iRank] * 2;
			pRecord->Contribute.m_iAttackCntRank = vInfo[i].m_iRank;
		}
	}
	vInfo.clear();

	//데쓰처리
	for(i = 0;i < iCharCnt;i++)
	{
		BattleModeRecord *pRecord = &m_vCharRecordList[i];
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
			BattleModeRecord *pRecord = &m_vCharRecordList[iArray];
			if( !pRecord->Contribute.m_bLastResultSync )
				pRecord->Contribute.m_iContribute += m_vContributeTable[vInfo[i].m_iRank];
			pRecord->Contribute.m_iDeathRank = vInfo[i].m_iRank;
		}
	}
	vInfo.clear();
}

void ioBattleMode::ContributeConvert()
{
	ContributeConvertTeam( TEAM_NONE );        //TEAM_NONE는 전체 유저에 대한 기여도 평가	
}

int ioBattleMode::ContributeMyRank( int &rRankValue1, int &rRankValue2, int &rRankValue3, int &rRankValue4 )
{
	rRankValue1 = rRankValue2 = rRankValue3 = rRankValue4 = -1;

	bool bObserver = false;
	ioBaseChar *pOwner = GetOwnerChar();
	if( pOwner && ( g_BattleRoomMgr.IsObserverUser(pOwner->GetCharName()) || m_pCreator->IsStealthMode() ) )
		bObserver = true;

	int i = 0;
	int iCharCnt = m_vCharRecordList.size();
	int iInfoCnt = 0;
	ContributeInfoList vInfo;

	//KO
	for(i = 0;i < iCharCnt;i++)
	{
		BattleModeRecord *pRecord = &m_vCharRecordList[i];
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
		BattleModeRecord *pRecord = &m_vCharRecordList[i];
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
		BattleModeRecord *pRecord = &m_vCharRecordList[i];
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
	return iInfoCnt;
}

void ioBattleMode::ContributeRoundAllPercent( DWORD dwCurTime, DWORD dwTotalTime )
{
	//기여도의 100분율
	int iMaxContribute	= 0;
	int iCharCnt		= m_vCharRecordList.size();
	int iPlayCharCnt	= 0;

	int i = 0;
	for(i = 0;i < iCharCnt;i++)
	{
		BattleModeRecord *pRecord = &m_vCharRecordList[i];
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
			BattleModeRecord *pRecord = &m_vCharRecordList[i];
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
}

void ioBattleMode::DamageContribute( const ioHashString szAttacker, const ioHashString szTarget, float fDamage, DWORD dwItemCode, int iWeaponType )
{
	if( szAttacker.IsEmpty() ) return;

	BattleModeRecord *pRecord = FindRecordCharName( szAttacker );
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
    
	float fDamagePoint = (float)((fDamage + fBonusDamage) * 100) * pow( ContributeTeamPerA( pRecord->pChar->GetTeam(), 8.0f, FLOAT1 ), 3 );
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

void ioBattleMode::KillContribute( const ioHashString szKiller, const ioHashString szKilled, DWORD dwItemCode )
{
	if( szKilled.IsEmpty() ) return;

	int iKillPoint = 100;

	float fAddKillPoint = FLOAT1;
	if( IsContributeAddPointItemCode( dwItemCode ) )
		fAddKillPoint = 1.1f;

	// 죽은 사람 전장에 있지 않은 상태로 변경
	g_BattleRoomMgr.SetUserBattleFightFlag(szKilled, false);

	if( szKiller.IsEmpty() || szKiller == szKilled )     //자살
	{
		BattleModeRecord *pRecord = FindRecordCharName( szKilled );
		if( !pRecord || !pRecord->pChar ) return;
		
		pRecord->Contribute.m_iKill -= iKillPoint;
	}
	else
	{
		BattleModeRecord *pKillerRecord = FindRecordCharName( szKiller );
		BattleModeRecord *pKilledRecord = FindRecordCharName( szKilled );
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
					pKillerRecord->Contribute.m_iKill += (float)((float)iKillPoint * fAddKillPoint) * ContributeTeamPerA( pKillerRecord->pChar->GetTeam(), 8.0f, FLOAT1 );
			}
		}
	}

	ContributeConvert();
	ContributeRoundAllPercent();
	UpdateRoundInfoList();
	CheckTotalChamp();
}

void ioBattleMode::AttackCntContribute( const ioHashString szAttacker, const ioHashString szTarget, DWORD dwItemCode, int iAttackValue/* = 100 */ )
{
	if( szAttacker.IsEmpty() || szTarget.IsEmpty() ) return;

	BattleModeRecord *pAttackerRecord = FindRecordCharName( szAttacker );
	BattleModeRecord *pTargetRecord = FindRecordCharName( szTarget );

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

void ioBattleMode::FillLastPlayRecordInfo( SP2Packet &rkPacket )
{
	int iCharCnt = m_vCharRecordList.size();
	rkPacket << iCharCnt;
	for(int i = 0;i < iCharCnt;i++)
	{
		BattleModeRecord *pRecord = &m_vCharRecordList[i];
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
}

void ioBattleMode::FillPlayRecordInfo( SP2Packet &rkPacket )
{
	int iCharCnt = m_vCharRecordList.size();
	rkPacket << iCharCnt;
	for(int i = 0;i < iCharCnt;i++)
	{
		BattleModeRecord *pRecord = &m_vCharRecordList[i];
		rkPacket << pRecord->pChar->GetCharName();
		rkPacket << pRecord->Contribute.m_iDamage;
		rkPacket << pRecord->Contribute.m_iAttackCnt;
		rkPacket << pRecord->Contribute.m_iKill;
		rkPacket << pRecord->Contribute.m_iDeath;
		rkPacket << pRecord->Contribute.m_iPrevContributePer;
		rkPacket << pRecord->GetUniqueTotalKill();
		rkPacket << pRecord->GetUniqueTotalDeath();
		rkPacket << g_VictoriesMgr.GetVictories( pRecord->pChar->GetCharName() );
		rkPacket << pRecord->GetNoDeathKillCount();
	}
}

void ioBattleMode::ApplyPlayRecordInfo( SP2Packet &rkPacket )
{
	int iCharCnt;
	rkPacket >> iCharCnt;
	for(int i = 0;i < iCharCnt;i++)
	{
		ioHashString szName;
		int iDamage, iAttackCnt, iKill, iDeath, iPrevContributePer, iUniqueTotalKill, iUniqueTotalDeath, iVictories, iNoDeathKillCount;
		rkPacket >> szName >> iDamage >> iAttackCnt >> iKill >> iDeath >> iPrevContributePer >> iUniqueTotalKill >> iUniqueTotalDeath >> iVictories >> iNoDeathKillCount;
		//if( !szName.IsEmpty() )
		//	LOG.PrintTimeAndLog( 0, "%s - Check - 0 - CharName : %s", __FUNCTION__, szName.c_str() );
		BattleModeRecord *pRecord = FindRecordCharName( szName );
		if( pRecord )
		{
			pRecord->Contribute.m_iDamage			= iDamage;
			pRecord->Contribute.m_iAttackCnt		= iAttackCnt;
			pRecord->Contribute.m_iKill             = iKill;
			pRecord->Contribute.m_iDeath            = iDeath;
			pRecord->Contribute.m_iPrevContributePer= iPrevContributePer;
			pRecord->iUniqueTotalKill = iUniqueTotalKill;
			pRecord->iUniqueTotalDeath= iUniqueTotalDeath;
			pRecord->iNoDeathKillCount = iNoDeathKillCount;

			if( !g_LadderTeamMgr.IsLadderTeam() )
				g_VictoriesMgr.SetVictories( szName, iVictories );
		}
	}
	
	ContributeConvert();
	ContributeRoundAllPercent();
	UpdateRoundInfoList();
	CheckTotalChamp();
}

void ioBattleMode::ContributeResultSync( const ioHashString &szName, int iContribute )
{
	BattleModeRecord *pRecord = FindRecordCharName( szName );
	if( !pRecord ) return;

	pRecord->Contribute.m_bLastResultSync = true;
	pRecord->Contribute.m_iContribute     = iContribute;
}

void ioBattleMode::OnZeroHP( SP2Packet &rkPacket )
{
	rkPacket >> m_dwSuddenDeathTime;
	m_dwSuddenDaathStartTime = FRAMEGETTIME();

	int iCharCnt = m_vCharRecordList.size();
	ioBaseChar* pMyChar = NULL;
	for( int i=0 ; i<iCharCnt ; i++ )
	{
		m_vCharRecordList[i].pChar->SetCurHP( 0.0f );
		m_vCharRecordList[i].pChar->SetCurHPRate( 0.0f );
		if( m_vCharRecordList[i].pChar->GetCharName() == g_MyInfo.GetPublicID() && m_vCharRecordList[i].pChar->GetState() != CS_DIE )
			pMyChar = m_vCharRecordList[i].pChar;
	}

	ioBaseChar* pMyRealChar = GetOwnerChar();
	// 난입 후 스킬 게이지 수치 0으로 수정
	for( int iSlot=0; iSlot < 4; ++iSlot )
	{
		if( pMyRealChar )
		{
			ioItem *pItem = pMyRealChar->GetEquipedItem( iSlot );
			if( pItem )
			{
				// 게이지를 사용하는 패시브 스킬도 초기화
				ioSkill *pSkill = pMyRealChar->GetEquipedSkill( iSlot );
				ioPassiveSkill *pPassiveSkill = ToPassiveSkill( pSkill );
				if( pPassiveSkill && pPassiveSkill->UsePassiveGauge() )
					pPassiveSkill->SetCurPassiveGauge( 0.0f );
				else
					pItem->SetCurSkillGauge( 0.0f );
			}
		}
	}

	//LOG.PrintTimeAndLog( 0, "%s -  Recived ZeroHP Packet", __FUNCTION__ );

	m_bZeroHP = true;
	g_SoundMgr.PlaySound( m_TimeOutSound, DSBVOLUME_MAX, PSM_LOOP | PSM_THREAD );
	g_ModeUIController.SetDeathTime();

	// 모든 대기 유저 출전
	bool bFight = g_BattleRoomMgr.CheckUserBattleFightFlag( g_MyInfo.GetPublicID() );
	// 출전하지 않았다면
	if( !bFight && pMyRealChar ) 
	{
		int iNextUserHPRate = (int)((pMyRealChar->GetHP().m_fCurValue / pMyRealChar->GetHP().m_fMaxValue) * 100.0f);
		// 배틀 모드 입장 완료 패킷 전달
		int iStartType = WARTIME_DIE;//WARTIME_TIME_OUT;	// 전투 중이었던 다른 유저 용병이 죽었을 경우
		ioHashString szEmpty;
		szEmpty.Clear();
		SP2Packet kPacket( CTPK_BATTLE_MODE_WARTIME_START );
		kPacket << iStartType << g_MyInfo.GetPublicID() << iNextUserHPRate << szEmpty;
		TCPNetwork::SendToServer( kPacket );
		//LOG.PrintTimeAndLog( 0, "%s -  Send CTPK_BATTLE_MODE_WARTIME_START", __FUNCTION__ );
	}

	// 모든 유저 난입인지 체크해서 난입이면 난입 UI 해제시키자...
}

void ioBattleMode::OnRoundReady( SP2Packet &rkPacket )
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

	// 새로운 시작
	BattleModeTimeGaugeWnd *pTimeGauge = dynamic_cast< BattleModeTimeGaugeWnd * >( g_GUIMgr.FindWnd( BATTLE_MODE_TIME_GAUGE_WND ) );
	if( pTimeGauge )
	{
		pTimeGauge->ClearCharSeqInfo();	
	}

	BattleModeFunctionWnd *pFunctionButtonWnd = dynamic_cast< BattleModeFunctionWnd * >( g_GUIMgr.FindWnd( BATTLE_MODE_FUNCTION_WND ) );
	if( pFunctionButtonWnd )
		pFunctionButtonWnd->ShowFunctionButton( false );

	g_GUIMgr.HideWnd( MIDDLE_JOINDELAY_WND );
}

void ioBattleMode::OnRoundJoin( SP2Packet &rkPacket )
{
	ioHashString szJoinChar;
	int iModeState;

	rkPacket >> szJoinChar;
	rkPacket >> iModeState;

	if( szJoinChar.IsEmpty() ) 
		return;

	ioBaseChar *pJoinChar = m_pCreator->GetBaseChar( szJoinChar );
	if( !pJoinChar ) return;

	DWORD dwStateGap;
	DWORD dwSelectTime;

	//bool bFirstOrder = true;
	//int iBattleOrder = g_BattleRoomMgr.GetUserBattleOrder(pJoinChar->GetCharName());
	//if(iBattleOrder != BATTLE_ORDER_FIRST)
	//{
	//	m_bFirstBlockCameraView = true;

	//	bFirstOrder = false;
	//	// 대기 자리 인덱스 셋팅
	//	TeamType eTeam = (TeamType)g_BattleRoomMgr.GetUserTeam(pJoinChar->GetCharName());
	//	int iIndex = 0;

	//	if( eTeam == TEAM_BLUE )
	//	{
	//		if(iBattleOrder == BATTLE_ORDER_SECOND)
	//			g_BattleRoomMgr.SetWaitIndex(pJoinChar->GetCharName(), 1);
	//		else if(iBattleOrder == BATTLE_ORDER_THIRD)
	//			g_BattleRoomMgr.SetWaitIndex(pJoinChar->GetCharName(), 2);
	//	}
	//	else if( eTeam == TEAM_RED )
	//	{
	//		if(iBattleOrder == BATTLE_ORDER_SECOND)
	//			g_BattleRoomMgr.SetWaitIndex(pJoinChar->GetCharName(), 3);
	//		else if(iBattleOrder == BATTLE_ORDER_THIRD)
	//			g_BattleRoomMgr.SetWaitIndex(pJoinChar->GetCharName(), 4);
	//	}
	//	
	//	// 자신이 대기자라면 대기자 버튼UI를 띄움
	//	if(g_MyInfo.GetPublicID() == pJoinChar->GetCharName())
	//	{
	//		BattleModeFunctionWnd *pFunctionButtonWnd = dynamic_cast< BattleModeFunctionWnd * >( g_GUIMgr.FindWnd( BATTLE_MODE_FUNCTION_WND ) );
	//		if( pFunctionButtonWnd )
	//		{
	//			pFunctionButtonWnd->ShowFunctionButton( true );
	//			pFunctionButtonWnd->SetPlayStage(m_pCreator);
	//		}
	//	}
	//	// 출전 중이 아니라는 표시
	//	g_BattleRoomMgr.SetUserBattleFightFlag(pJoinChar->GetCharName(), false);
	//}
	//else
	//{
	//	TeamType eTeam = (TeamType)g_BattleRoomMgr.GetUserTeam(pJoinChar->GetCharName());
	//	LOG.PrintTimeAndLog( 0, "%s -  FirstUser - Name : %s, eTeam : %d, iBattleOrder : %d", 
	//		__FUNCTION__, pJoinChar->GetCharName().c_str(), (int)eTeam, iBattleOrder );
	//	// 대기 자리 인덱스 초기화 (출전했을때 상태 0으로 해놓고 아래서 변경) - SYH
	//	g_BattleRoomMgr.SetWaitIndex(pJoinChar->GetCharName(), 0);
	//	g_BattleRoomMgr.SetUserBattleFightFlag(pJoinChar->GetCharName(), true);
	//}
	int iBattleOrder = g_BattleRoomMgr.GetUserBattleOrder( szJoinChar );

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
		
		if(iBattleOrder == BATTLE_ORDER_FIRST)
		{
			rkPacket >> dwSelectTime;

			if( m_ModeState != MS_RESULT )
			{
				int iSelectChar = 0;

				iSelectChar = pJoinChar->GetSelectCharArray();
				//pJoinChar->SetReadyState();

				StartChangeCharacter( iSelectChar, dwSelectTime );

				if( g_BattleRoomMgr.IsBattleRoom() )
					g_QuestMgr.QuestCompleteTerm( QC_ENTER_BATTLE_PVP, GetModeType() );
				else if( g_LadderTeamMgr.IsLadderTeam() )
					g_QuestMgr.QuestCompleteTerm( QC_ENTER_CAMP_BATTLE, GetModeType() );
				else if( g_ShuffleRoomMgr.IsShuffleRoom() )
					g_QuestMgr.QuestCompleteTerm( QC_ENTER_BATTLE_PVP, GetModeType() );
				else if( g_RankBattleMgr.IsRankBattlePlaying() )
					g_QuestMgr.QuestCompleteTerm( QC_ENTER_BATTLE_PVP, GetModeType() );
			}		
			else
			{
				SetResultJoinState();
			}
			g_ModeHelpMgr.InitModeHelp();

			rkPacket >> m_dwCurRoundDuration;

			rkPacket >> m_fWinScoreConstant;
			rkPacket >> m_iRedKillPoint;
			rkPacket >> m_iBlueKillPoint;
		}
		else if(pJoinChar->GetState() != CS_OBSERVER)
		{
			rkPacket >> dwSelectTime;

			int iSelectChar = 0;
			
			iSelectChar = pJoinChar->GetSelectCharArray();
			pJoinChar->SetReadyState();

			m_iSelectCharacter = g_MyInfo.GetAutoSelectChar( iSelectChar, iSelectChar );

			SP2Packet kPacket( CTPK_CHANGE_CHAR );
			kPacket << m_iSelectCharacter;
			kPacket << false;
			kPacket << pJoinChar->GetCharChangeUniqueIndex() + 1;
			TCPNetwork::SendToServer( kPacket );

			g_ModeHelpMgr.InitModeHelp();

			g_ModeHelpMgr.ShowHelp( ioModeHelpManager::HT_VIEW );

			SetBlindMode( true );
			g_GUIMgr.SetSkipUIByBlind( false );		

			rkPacket >> m_dwCurRoundDuration;

			rkPacket >> m_fWinScoreConstant;
			rkPacket >> m_iRedKillPoint;
			rkPacket >> m_iBlueKillPoint;

			pJoinChar->SetState( CS_VIEW );
		}
	}
	else 
	{
		//if(iBattleOrder == BATTLE_ORDER_FIRST)
		//	pJoinChar->SetReadyState();

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

			//if( iBattleOrder != BATTLE_ORDER_FIRST )
			//	pJoinChar->SetState( CS_VIEW );
		}
		//if(bFirstOrder)
		{
			// 더미
			rkPacket >> dwStateGap;
			rkPacket >> dwSelectTime;

			rkPacket >> m_dwCurRoundDuration;

			rkPacket >> m_fWinScoreConstant;
			rkPacket >> m_iRedKillPoint;
			rkPacket >> m_iBlueKillPoint;
		}
	}

	rkPacket >> m_dwEntryEnableTime;
	rkPacket >> m_dwTagDisableTime;
	rkPacket >> m_iCurEntryCount;
	m_iMaxEntryCount = m_iCurEntryCount;

	// 난입 버튼 타이틀 변경 (카운트 횟수 줄이게...)
	BattleModeFunctionWnd *pFunctionButtonWnd = dynamic_cast< BattleModeFunctionWnd * >( g_GUIMgr.FindWnd( BATTLE_MODE_FUNCTION_WND ) );
	if( pFunctionButtonWnd )
		pFunctionButtonWnd->ResetEntryButtonTitle( m_iCurEntryCount );

	// 내 용병 진입 시에만 이 패킷 보내도록함 
	if( pJoinChar->GetCharName() == g_MyInfo.GetPublicID() && !m_bWarTimeStartPacketSend && iBattleOrder == BATTLE_ORDER_FIRST )
	{
		int iJoinCharHp = (int)((pJoinChar->GetHP().m_fCurValue / pJoinChar->GetHP().m_fMaxValue) * 100.0f);
		// 배틀 모드 입장 완료 패킷 전달
		ioHashString szEmpty;
		szEmpty.Clear();
		int iStartType = WARTIME_PLAY;
		SP2Packet kPacket( CTPK_BATTLE_MODE_WARTIME_START );
		kPacket << iStartType << g_MyInfo.GetPublicID() << iJoinCharHp << szEmpty;
		TCPNetwork::SendToServer( kPacket );
		m_bWarTimeStartPacketSend = true;
		//LOG.PrintTimeAndLog( 0, "%s -  Send CTPK_BATTLE_MODE_WARTIME_START", __FUNCTION__ );
	}
}

void ioBattleMode::OnRoundJoinView( SP2Packet &rkPacket )
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

		//g_ChatMgr.SetInfomationMsg( STR(1) );
		//g_ChatMgr.SetInfomationMsg( STR(2) );

		rkPacket >> m_dwCurRoundDuration;

		rkPacket >> m_fWinScoreConstant;
		rkPacket >> m_iRedKillPoint;
		rkPacket >> m_iBlueKillPoint;
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

		// 더미
		rkPacket >> dwStateGap;

		rkPacket >> m_dwCurRoundDuration;

		rkPacket >> m_fWinScoreConstant;
		rkPacket >> m_iRedKillPoint;
		rkPacket >> m_iBlueKillPoint;
	}
}

void ioBattleMode::OnRoundJoinObserver( SP2Packet &rkPacket )
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

		//g_ChatMgr.SetInfomationMsg( STR(1) );
		//g_ChatMgr.SetInfomationMsg( STR(2) );

		rkPacket >> m_dwCurRoundDuration;

		rkPacket >> m_fWinScoreConstant;
		rkPacket >> m_iRedKillPoint;
		rkPacket >> m_iBlueKillPoint;

		if( g_BattleRoomMgr.IsBattleRoom() )
		{
			g_QuestMgr.QuestCompleteTerm( QC_ENTER_BATTLE_OBSERVER, GetModeType() );
		}
	}
	else if( m_ModeState == MS_PLAY )
	{
		pJoinChar->SetState( CS_OBSERVER );

		// 더미
		rkPacket >> dwStateGap;

		rkPacket >> m_dwCurRoundDuration;

		rkPacket >> m_fWinScoreConstant;
		rkPacket >> m_iRedKillPoint;
		rkPacket >> m_iBlueKillPoint;
	}

	// 자신이 옵저버라면 대기자 기능 버튼 보이지 않음
	BattleModeFunctionWnd *pFunctionButtonWnd = dynamic_cast< BattleModeFunctionWnd * >( g_GUIMgr.FindWnd( BATTLE_MODE_FUNCTION_WND ) );
	if( pFunctionButtonWnd && szJoinChar == g_MyInfo.GetPublicID() )
	{
		pFunctionButtonWnd->ShowFunctionButton( false );
	}
}

void ioBattleMode::OnTagWaitForRequest(SP2Packet &rkPacket)
{
	int iTagReq = 0;
	ioHashString szRequesterName;
	bool bTagState = false;
	rkPacket >> iTagReq >> szRequesterName;

	ioBaseChar* pRequester = m_pCreator->GetBaseChar( szRequesterName );
	// 죽음 체크
	if(pRequester->GetState() == CS_DIE)
		return;

	if( iTagReq == TAG_WAIT_FOR_REQUEST_OK)
	{
		rkPacket >> bTagState;

		// 태그 / 태그 취소 요청 성공'
		if(!szRequesterName.IsEmpty())
		{
			if(bTagState)
			{
				if( g_BattleRoomMgr.GetBattleModeTag(szRequesterName) == TAG_POSSIBLE )
				{
					// 이 함수 지나면 유저의 태그 상태값은 0에서 1로, 그 다음 태그 수락 패킷 받으면 2
					g_BattleRoomMgr.SwitchBattleModeTag(szRequesterName, TAG_POSSIBLE_REQ);		
					SwitchTagZone(bTagState, szRequesterName);
				}
				else if( g_BattleRoomMgr.GetBattleModeTag(szRequesterName) == TAG_IMPOSSIBLE )
				{
					g_BattleRoomMgr.SwitchBattleModeTag(szRequesterName, TAG_IMPOSSIBLE_REQ);
					SwitchNonTaggedDisplay(bTagState, szRequesterName);
				}
			}
			else if(!bTagState)
			{
				if( g_BattleRoomMgr.GetBattleModeTag(szRequesterName) == TAG_POSSIBLE_REQ )
				{
					// 이 유저의 태그 상태값은 0이 됨, 태그 신청도 안한 상태
					g_BattleRoomMgr.SwitchBattleModeTag(szRequesterName, TAG_POSSIBLE);		
					SwitchTagZone(bTagState, szRequesterName);
					SwitchNonTaggedDisplay(bTagState, szRequesterName);
				}
				else if( g_BattleRoomMgr.GetBattleModeTag(szRequesterName) == TAG_IMPOSSIBLE_REQ )
				{
					// 이 유저의 태그 상태값은 -1이 됨, 태그 불가하고 신청 안한 상태
					g_BattleRoomMgr.SwitchBattleModeTag(szRequesterName, TAG_IMPOSSIBLE);
					SwitchNonTaggedDisplay(bTagState, szRequesterName);
				}
			}
			//LOG.PrintTimeAndLog( 0, "TagReq Name : %s, TagFlag : %d, TagState : %d", szRequesterName.c_str(), bTagState, g_BattleRoomMgr.GetBattleModeTag(szRequesterName) );
		}
		//LOG.PrintTimeAndLog( 0, "%s -  Received TAG_WAIT_FOR_REQUEST_OK", __FUNCTION__ );
	}
	else if( iTagReq == TAG_WAIT_FOR_REQUEST_EXCEPTION)
	{
		rkPacket >> bTagState;
		//LOG.PrintTimeAndLog( 0, "%s -  Received TAG_WAIT_FOR_REQUEST_EXCEPTION", __FUNCTION__ );
	}

	
}

void ioBattleMode::OnTagAccept(SP2Packet &rkPacket)
{
	int iTagReq = 0;
	ioHashString szStartUserName;
	ioHashString szEndUserName;
	bool bTagStage = false;
	rkPacket >> iTagReq;
	if( iTagReq == TAG_ACCEPT_OK)		// 태그 수락 성공
	{
		rkPacket >> szEndUserName;		// 들어가는 놈
		rkPacket >> szStartUserName;		// 나갈 놈

		ioBaseChar* pStartChar = m_pCreator->GetBaseChar(szStartUserName);		// 전장에 들어갈 용병
		ioBaseChar* pEndChar = m_pCreator->GetBaseChar(szEndUserName);				// 전장에 이탈할 용병
		if(pStartChar && pEndChar && g_MyInfo.GetPublicID() == szStartUserName)	// 전장 이탈 WARTIME 패킷 전달
		{
			SwitchTagZone(false, szEndUserName);				// 태그 완료되었으니 이펙트 제거
			//LOG.PrintTimeAndLog( 0, "%s -  StartUserName : %s, EnterUser : %s", __FUNCTION__, szStartUserName.c_str(), szEndUserName.c_str() );
			// 배틀 모드 입장 완료 패킷 전달
			ioHashString szEmpty;
			szEmpty.Clear();
			int iStartType = WARTIME_TAG;		// 태그
			int iStartCharHPRate = (int)((pStartChar->GetHP().m_fCurValue / pStartChar->GetHP().m_fMaxValue) * 100.0f);
			SP2Packet kPacket( CTPK_BATTLE_MODE_WARTIME_START );
			kPacket << iStartType << szStartUserName << iStartCharHPRate << szEndUserName;
			TCPNetwork::SendToServer( kPacket );
			m_bWarTimeStartPacketSend = true;
			//LOG.PrintTimeAndLog( 0, "%s -  Send CTPK_BATTLE_MODE_WARTIME_START", __FUNCTION__ );
		}
	}
	else if( iTagReq == TAG_ACCEPT_NOT_TIME )
	{
		DWORD dwTime = 0;
		rkPacket >> szEndUserName;		// 들어가려는 놈
		rkPacket >> dwTime;				// 태그 불가 시간 10초에 대한 남은 시간

		int iSec = (int)(dwTime * 0.001f);

		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1), iSec);

		//LOG.PrintTimeAndLog( 0, "%s - Received TAG_ACCEPT_NOT_TIME", __FUNCTION__ );

		ioBaseChar* pEndUser = m_pCreator->GetBaseChar(szEndUserName);
		if( pEndUser && pEndUser->GetState() == CS_BATTLE_WAITING )
			pEndUser->SetState( CS_DELAY );
	}
	//else if( iTagReq == TAG_WAIT_FOR_REQUEST_EXCEPTION)
	//	rkPacket >> bTagStage;

}

void ioBattleMode::OnBattleModeStartCheck(SP2Packet &rkPacket)
{
	int iStartType = 0;
	int iState = 0;							// 태그 or 난입 or 전투 중 유저가 죽었을 경우에 대한 값을 보내줌
	ioHashString szWaitUserName;			// 태그 당해서 지금은 전장에 들어간 상태인 녀석
	ioHashString szWarTimeStartName;		// 태그 당해서 지금은 전장에 들어간 상태인 녀석
	ioHashString szEndUserName;				// 태그 해서 지금은 대기 상태인 녀석

	rkPacket >> iStartType >> iState;		
	if( iStartType == WARTIME_START_OK)
	{
		rkPacket >> szWaitUserName >> szWarTimeStartName >> szEndUserName;

		ioBaseChar* pEndChar = m_pCreator->GetBaseChar(szEndUserName);
		ioBaseChar* pStartChar = m_pCreator->GetBaseChar(szWarTimeStartName);

		if(!szWarTimeStartName.IsEmpty() && !szEndUserName.IsEmpty() && pStartChar && pEndChar)	// 두명의 닉네임이 와서 태그되는거면
		{
			// 죽음 체크
			if(pStartChar->GetState() == CS_DIE || pEndChar->GetState() == CS_DIE)
				return;

			SwitchTagZone(false, szWarTimeStartName);
			//SetMapPassiveBuffDuringBattle(pEndChar, IMMOTAL_UNLIMITE_BUFF);	// 무적 버프 적용
			pEndChar->RemoveAllBuff();				// 모든 버프 삭제...(특정 장비같은 경우 삭제하지 말아야할 버프가 있으면 이 함수로 모두 삭제하지 말아야함..)
			pEndChar->SetState( CS_BATTLE_WAITING );

			//RemoveMapPassiveBuffDuringBattle(pStartChar, IMMOTAL_UNLIMITE_BUFF);	// 무적 버프 해제
			pStartChar->SetState( CS_DELAY, true, true );
			SetMapPassiveBuffDuringBattle(pStartChar, IMMOTAL_3SEC_BUFF);	// 3초 무적 버프 적용

			D3DXVECTOR3 vTagPos1 = pEndChar->GetWorldPosition();	// 대기할 용병 좌표
			//D3DXVECTOR3 vTagPos2 = pStartChar->GetWorldPosition();	// 전장 투입할 용병 좌표
			D3DXVECTOR3 vTagPos3 = GetWaitingPosByPosIdx( pStartChar );		// 나갈 용병의 대기했던 위치 구하기
			//pEndChar->SetWorldPosition(vTagPos2);					// (임시) 좌표 스왑
			pEndChar->SetWorldPosition( vTagPos3 );
			pEndChar->SetCurMoveSpeed( 0.0f );						// 가속도 0으로 만듦
			pEndChar->InitForcePower();
			pEndChar->ClearDirKey();
			pEndChar->ClearReserveKeyInput();
			//pEndChar->Set( 0.0f );
			// 대기할 용병 맵 중앙을 보게 회전시킴
			RotateSoldierToCenter( pEndChar );
			
			pStartChar->SetWorldPosition( vTagPos1 );
			g_BattleRoomMgr.SetUserBattleFightFlag( szEndUserName, false );
			g_BattleRoomMgr.SetUserBattleFightFlag( szWarTimeStartName, true );
			
			g_BattleRoomMgr.SwitchBattleModeTag(szEndUserName, TAG_IMPOSSIBLE);		// 대기자가 된 유저 태그 상태값 이제 -1(태그 불가)이 됨 10초 지나면 0으로됨
			g_BattleRoomMgr.SwitchBattleModeTag(szWarTimeStartName, TAG_POSSIBLE);
			g_BattleRoomMgr.SwitchWaitIndex(szWarTimeStartName, szEndUserName);
			SwapUserWaitPosIdxInUI( pStartChar, pEndChar );				// 유저 리스트 UI 쪽 재정렬
			
			// 태그가 완료되었으니 태그 상태 되돌려는 리스트에서 유저 이름으로 삭제
			TagUserInitInfoList::iterator userTagStateIter = m_TagStateList.begin();
			while(userTagStateIter != m_TagStateList.end())
			{
				if( szWarTimeStartName == userTagStateIter->m_szWaitUser )// 삭제
				{
					LOG.PrintTimeAndLog( 0, "%s - List Delete - TagComplete UserName : %s", __FUNCTION__, userTagStateIter->m_szWaitUser.c_str() );
					m_TagStateList.erase( userTagStateIter++ );
					break;
				}
				else		
					userTagStateIter++;
			}

			ShareSkillGaugeForSameTeam( szEndUserName );


			// 일단 자신 움직임 봉쇄, 태그 당한 쪽 움직이게 함
			g_BattleRoomMgr.SetUserBattleFightFlag(szEndUserName, false);
			g_BattleRoomMgr.SetUserBattleFightFlag(szWarTimeStartName, true);
			pStartChar->SetState( CS_DELAY, true, true );
			//pEndChar->SetState(CS_DELAY);

			// 태그 알림
			ShowTagAlarm( szWarTimeStartName );
		}
		else if (!szWarTimeStartName.IsEmpty() && pStartChar && szEndUserName.IsEmpty())		// 난입 또는 이전 주자가 죽는 상황으로 시작되면
		{
			//ioBaseChar* pStartChar = m_pCreator->GetBaseChar(szWarTimeStartName);
			// 죽음 체크
			if(pStartChar->GetState() == CS_DIE)
				return;
			if(pStartChar->GetState() != CS_LOADING)
				pStartChar->SetState( CS_DELAY, true, true );
			//RemoveMapPassiveBuffDuringBattle(pStartChar, IMMOTAL_UNLIMITE_BUFF);	// 무적 버프 해제

			if(iState != ENTRY_ACTION)		// 난입이 아닐때만 
				SetMapPassiveBuffDuringBattle(pStartChar, IMMOTAL_3SEC_BUFF);	// 3초 무적 버프 적용
			else		// 난입이면 이동기 버프 적용
			{
				// 난입 상태 지정
				g_BattleRoomMgr.SetUserEntryState( szWarTimeStartName, true );

				SetMapPassiveBuffDuringBattle(pStartChar, PIERROT_JUMP_BUFF);	// 삐에로 모자 스킬 중 이동기 버프
				// 용병에 붙는 팀별 난입 이펙트
				if( pStartChar->GetTeam() == TEAM_BLUE )
					SetMapPassiveBuffDuringBattle(pStartChar, ENTRY_BLUE_EFFECT_BUFF);	// 난입 표시 이펙트
				else if( pStartChar->GetTeam() == TEAM_RED )
					SetMapPassiveBuffDuringBattle(pStartChar, ENTRY_RED_EFFECT_BUFF);	// 난입 표시 이펙트

				// 자신이 난입하면 스킬 게이지 0으로 초기화
				if(szWarTimeStartName == g_MyInfo.GetPublicID())
				{
					for( int iSlot=0; iSlot < 4; ++iSlot )
					{
						ioItem *pItem = pStartChar->GetEquipedItem( iSlot );
						if( pItem )
						{
							// 게이지를 사용하는 패시브 스킬도 초기화
							ioSkill *pSkill = pStartChar->GetEquipedSkill( iSlot );
							ioPassiveSkill *pPassiveSkill = ToPassiveSkill( pSkill );
							if( pPassiveSkill && pPassiveSkill->UsePassiveGauge() )
								pPassiveSkill->SetCurPassiveGauge( 0.0f );
							else
								pItem->SetCurSkillGauge( 0.0f );
						}

						// 스킬의 액티브 카운트
						ioSkill *pEquipSkill = pStartChar->GetEquipedSkill( iSlot );
						if( pEquipSkill )
						{
							if( pEquipSkill->CheckUseBuff( pStartChar ) || pEquipSkill->CheckUseActiveCount() )
							{
								ioBuff *pBuff = pStartChar->GetUseActiveCountInBuff( pEquipSkill->GetName() );
								if( pBuff && pBuff->UseActiveCount() )
								{
									pEquipSkill->ResetCurActiveCount();
									pBuff->ResetCurActiveCount();
								}
							}
						}
					}
				}
				// 난입 알림
				ShowEntryAlarm( szWarTimeStartName );
			}

			// 태그 요청 상태일때 난입 OK 패킷 받으면 이펙트 OFF, 태그 상태 값 TAG_POSSIBLE로 변경
			TAG_STATE eTagState = (TAG_STATE)g_BattleRoomMgr.GetBattleModeTag(szWarTimeStartName);
			if( eTagState == TAG_POSSIBLE_REQ || eTagState == TAG_IMPOSSIBLE_REQ )
			{
				// 이 유저의 태그 상태값은 0이 됨, 태그 신청도 안한 상태
				g_BattleRoomMgr.SwitchBattleModeTag(szWarTimeStartName, TAG_POSSIBLE);		
				SwitchTagZone(false, szWarTimeStartName);
				SwitchNonTaggedDisplay(false, szWarTimeStartName);
			}

			g_BattleRoomMgr.SetUserBattleFightFlag( szWarTimeStartName, true );
		}

		// 대기자 버튼 UI On / Off
		BattleModeFunctionWnd *pFunctionButtonWnd = dynamic_cast< BattleModeFunctionWnd * >( g_GUIMgr.FindWnd( BATTLE_MODE_FUNCTION_WND ) );
		if(szWarTimeStartName == g_MyInfo.GetPublicID())
		{
			// 죽음 체크
			if(pStartChar->GetState() != CS_DIE)
			{
				pFunctionButtonWnd->ShowFunctionButton(false);
				SetBlindMode( false );		// 뷰모드 블라인드 모드 해제
			}
		}
		else if(szEndUserName == g_MyInfo.GetPublicID())
		{
			// 죽음 체크
			if(pEndChar->GetState() != CS_DIE)
			{
				m_bFirstBlockCameraView = true;
				pFunctionButtonWnd->ShowFunctionButton(true);
				SetBlindMode( true );		// 뷰모드 블라인드 모드 적용
			}
		}

		// 난입만 아니면 출전하는 주자를 상단 UI에 추가함
		if(iState != ENTRY_ACTION)
		{
			if( pStartChar->GetState() != CS_DIE )		// 죽지 않았을때 
			{
				g_BattleRoomMgr.SetWaitIndex(szWarTimeStartName, 0);		// 돌아갈 위치 인덱스 값을 0(난입이 아닌 출전일때 이 값으로 바꿔줘야 함)으로 바꿈
				BattleModeTimeGaugeWnd *pTimeGauge = dynamic_cast< BattleModeTimeGaugeWnd * >( g_GUIMgr.FindWnd( BATTLE_MODE_TIME_GAUGE_WND ) );
				if( pTimeGauge )
					pTimeGauge->SetBattleModeUser(szWarTimeStartName, pStartChar->GetTeam() );
			}

			// 내가 출전하는 유저이거나 대기자로 전환될 유저가 아니라면 시점 변경
			if( ( !pStartChar || szWarTimeStartName != g_MyInfo.GetPublicID() ) && ( !pEndChar || szEndUserName != g_MyInfo.GetPublicID() ) )
				ChangeCurFighterView();
		}
	}
	else if( iStartType == WARTIME_START_ERROR)
		rkPacket >> szWarTimeStartName;

	//LOG.PrintTimeAndLog( 0, "%s - Packet - StartUserName : %s, WarTimeStartName : %s", __FUNCTION__, szWaitUserName.c_str(), szWarTimeStartName.c_str() );
}

void ioBattleMode::OnBattleModeEntryStart(SP2Packet &rkPacket)
{
	int iEntryReq = 0;
	ioHashString szEntryUserName;
	bool bTagStage = false;
	DWORD dwEntryTime = 0;
	int iEntryCount = 0;		// 남은 난입 횟수

	rkPacket >> iEntryReq;
	if( iEntryReq == ENTRY_START_OK )
	{
		rkPacket >> szEntryUserName >> dwEntryTime;
		rkPacket >> m_iCurEntryCount;

		if(szEntryUserName.IsEmpty())
			return;
		ioBaseChar *pEntryChar = m_pCreator->GetBaseChar( szEntryUserName );
		if(!pEntryChar)
			return;
		// 죽음 체크
		if(pEntryChar->GetState() == CS_DIE)
			return;

		// 난입하는 유저가 자신이면 warTime패킷 보냄
		if(pEntryChar->GetCharName() == g_MyInfo.GetPublicID())
		{
			// 난입 유저 현재 대기 위치 저장
			SaveEntrySoldierPos( pEntryChar );
			int iEntryCharHPRate = (int)((pEntryChar->GetHP().m_fCurValue / pEntryChar->GetHP().m_fMaxValue) * 100.0f);
			// 배틀 모드 입장 완료 패킷 전달
			int iStartType = ENTRY_ACTION;	// 난입
			ioHashString szEmpty;
			szEmpty.Clear();
			SP2Packet kPacket( CTPK_BATTLE_MODE_WARTIME_START );
			kPacket << iStartType << szEntryUserName << iEntryCharHPRate << szEmpty;
			TCPNetwork::SendToServer( kPacket );

			// 난입 버튼 타이틀 변경 (카운트 횟수 줄이게...)
			BattleModeFunctionWnd *pFunctionButtonWnd = dynamic_cast< BattleModeFunctionWnd * >( g_GUIMgr.FindWnd( BATTLE_MODE_FUNCTION_WND ) );
			if( pFunctionButtonWnd )
				pFunctionButtonWnd->ResetEntryButtonTitle( m_iCurEntryCount );
		}
	}
	else if( iEntryReq == ENTRY_START_ON_BOARD_EXCEPTION)
	{
		rkPacket >> szEntryUserName;
		g_ChatMgr.SetSystemMsg( STR(1) );
		//LOG.PrintTimeAndLog( 0, " Packet - ENTRY_START_ON_BOARD_EXCEPTION" );
	}
	else if( iEntryReq == ENTRY_START_CHANCE_ONCE_EXCEPTION)
	{
		rkPacket >> szEntryUserName;
		//LOG.PrintTimeAndLog( 0, " Packet - ENTRY_START_CHANCE_ONCE_EXCEPTION" );
	}

	//LOG.PrintTimeAndLog( 0, "%s - Packet - szEntryUserName : %s", __FUNCTION__, szEntryUserName.c_str() );
}

void ioBattleMode::OnBattleModeEntryTimeOut(SP2Packet &rkPacket)
{
	ioHashString szReturnUser;
	rkPacket >> szReturnUser;

	// 데스 타임 때라면 난입 타임아웃 처리하지 않음
	if( m_bZeroHP )
		return;

	//LOG.PrintTimeAndLog( 0, "%s - Packet - szReturnUser : %s", __FUNCTION__, szReturnUser.c_str() );
	if( !szReturnUser.IsEmpty() )
	{
		ioBaseChar* pReturnChar = m_pCreator->GetBaseChar( szReturnUser );

		// 난입 제한 시간이 지나면 현재 이 패킷 받을때 난입한 용병 위치를 난입하기 전 위치로 돌려놓고 움직입 봉쇄
		if( pReturnChar && pReturnChar->GetState() != CS_DIE )
		{
			// 자신 움직임 봉쇄
			g_BattleRoomMgr.SetUserBattleFightFlag( szReturnUser, false );
			
			//SetMapPassiveBuffDuringBattle( pReturnChar, IMMOTAL_UNLIMITE_BUFF );	// 무적 버프 적용
			// 용병에 붙는 팀별 난입 이펙트
			if( pReturnChar->GetTeam() == TEAM_BLUE )
				RemoveMapPassiveBuffDuringBattle( pReturnChar, ENTRY_BLUE_EFFECT_BUFF);	// 난입 표시 이펙트 해제
			else if( pReturnChar->GetTeam() == TEAM_RED )
				RemoveMapPassiveBuffDuringBattle( pReturnChar, ENTRY_RED_EFFECT_BUFF);	// 난입 표시 이펙트 해제
			
			// 난입 상태 false
			g_BattleRoomMgr.SetUserEntryState( szReturnUser, false );

			// 죽지 않은 경우 아래 UI, 좌표 이동 상태 값 변경 되게..
			//if(pReturnChar->GetState() != CS_DIE)
			{
				D3DXVECTOR3 vScale = D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 );
				m_pCreator->CreateMapEffect( m_szEntryTimeOutEffect, pReturnChar->GetMidPositionByRate(), vScale );

				// 현재 좌표를 대기자리 좌표로 이동
				SwitchEntrySoldierPosToWaitPos( pReturnChar );
				pReturnChar->SetCurMoveSpeed( 0.0f );						// 가속도 0으로 만듦
				pReturnChar->InitForcePower();
				pReturnChar->ClearDirKey();
				pReturnChar->ClearReserveKeyInput();
				// 되돌아온 용병 맵 중앙을 보게 회전시킴
				RotateSoldierToCenter( pReturnChar );
				// 모든 버프 삭제...(특정 장비같은 경우 삭제하지 말아야할 버프가 있으면 이 함수로 모두 삭제하지 말아야함..)
				pReturnChar->RemoveAllBuff();		
				// 대기 상대(피격, 밀림, 타겟 불가)
				pReturnChar->SetState( CS_BATTLE_WAITING );

				// 대기자 버튼 UI On
				BattleModeFunctionWnd *pFunctionButtonWnd = dynamic_cast< BattleModeFunctionWnd * >( g_GUIMgr.FindWnd( BATTLE_MODE_FUNCTION_WND ) );
				if(szReturnUser == g_MyInfo.GetPublicID())
				{
					m_bFirstBlockCameraView = true;
					pFunctionButtonWnd->ShowFunctionButton(true);
					SetBlindMode( true );		// 뷰모드 블라인드 모드 적용
				}
			}
		}
	}

}

void ioBattleMode::OnBattleModeDieStart(SP2Packet &rkPacket)
{
	int iDieAfterResult = 0;
	ioHashString szNextUser;
	ioBaseChar *pNextChar = NULL;
	rkPacket >> iDieAfterResult;
	// 다음 유저가 대기중인 유저이거나 난입한 유저인 경우
	if(iDieAfterResult == WARTIME_DIE_WAIT_USER_START_OK || iDieAfterResult == WARTIME_DIE_ENTRY_USER_START_OK)				
	{
		rkPacket >> szNextUser;
		pNextChar = m_pCreator->GetBaseChar( szNextUser );

		// 출전 유무 (난입하여 출전 상태라면 아래 패킷 보내지 않도록함)
		bool bUserFight = g_BattleRoomMgr.CheckUserBattleFightFlag( szNextUser );		
		// 대기자리 인덱스 값을 전투중인 상태인 0으로 바꿈 (0 이상은 대기 상태서 대기자리 인덱스를 뜻함 - SYH)
		if( pNextChar && szNextUser == g_MyInfo.GetPublicID()  && !bUserFight)
		{
			// 죽음 체크
			if(pNextChar->GetState() == CS_DIE)
				return;

			int iNextCharHPRate = (int)((pNextChar->GetHP().m_fCurValue / pNextChar->GetHP().m_fMaxValue) * 100.0f);
			// 배틀 모드 입장 완료 패킷 전달
			int iStartType = WARTIME_DIE;	// 전투 중이었던 다른 유저 용병이 죽었을 경우
			ioHashString szEmpty;
			szEmpty.Clear();
			SP2Packet kPacket( CTPK_BATTLE_MODE_WARTIME_START );
			kPacket << iStartType << szNextUser << iNextCharHPRate << szEmpty;
			TCPNetwork::SendToServer( kPacket );
			//LOG.PrintTimeAndLog( 0, "%s -  Send CTPK_BATTLE_MODE_WARTIME_START", __FUNCTION__ );
		}
	}
	//else if (iDieAfterResult == WARTIME_DIE_ENTRY_USER_START_OK)		// 다음 유저가 난입 중인 유저일 경우
	//{
	//	rkPacket >> szNextUser;
	//	pNextChar = m_pCreator->GetBaseChar( szNextUser );

	//	if( pNextChar )
	//	{
	//		g_BattleRoomMgr.SetWaitIndex(pNextChar->GetCharName(), 0);		
	//	}
	//}
	else if (iDieAfterResult == BATTLE_MODE_EXCEPTION)					// 기타 예외
	{
		int iTeam = 0;
		rkPacket >> iTeam;
	}
}

void ioBattleMode::OnTagAcceptTimeOK( SP2Packet &rkPacket )
{
	ioHashString szEnableTagUser;
	rkPacket >> szEnableTagUser;

	ioBaseChar* pEnableTagUser = m_pCreator->GetBaseChar( szEnableTagUser );
	// 죽음 체크
	if(pEnableTagUser->GetState() == CS_DIE)
		return;

	int iTagState = g_BattleRoomMgr.GetBattleModeTag(szEnableTagUser);
	if( iTagState == TAG_IMPOSSIBLE )
		g_BattleRoomMgr.SwitchBattleModeTag(szEnableTagUser, TAG_POSSIBLE);	// 태그 불가 상태인 -1에서 태그 가능 0으로 변경 
	else if( iTagState == TAG_IMPOSSIBLE_REQ )
	{
		g_BattleRoomMgr.SwitchBattleModeTag(szEnableTagUser, TAG_POSSIBLE_REQ);	// 태그 불가 상태인 -1에서 태그 가능 0으로 변경 
		SwitchTagZone(true, szEnableTagUser);
		SwitchNonTaggedDisplay(false, szEnableTagUser);
	}
}

////////////////// 배틀 모드 전용 패킷 처리 함수

bool ioBattleMode::ProcessTCPPacket( SP2Packet &rkPacket )
{
	if( ioPlayMode::ProcessTCPPacket( rkPacket ) )
		return true;

	switch( rkPacket.GetPacketID() )
	{
	case STPK_BATTLE_MODE_WARTIME_START:
		OnBattleModeStartCheck( rkPacket );
		return true;
	case STPK_BATTLE_MODE_TAG_WAIT_FOR_REQUEST:
		OnTagWaitForRequest( rkPacket );
		return true;
	case STPK_BATTLE_MODE_TAG_ACCEPT:
		OnTagAccept( rkPacket );
		return true;
	case STPK_BATTLE_MODE_ENTRY_START:
		OnBattleModeEntryStart( rkPacket );
		return true;
	case STPK_BATTLE_MODE_ENTRY_TIMEOUT:
		OnBattleModeEntryTimeOut( rkPacket );
		return true;
	case STPK_BATTLE_MODE_WARTIME_DIE_START:
		OnBattleModeDieStart( rkPacket );
		return true;
	case STPK_BATTLE_MODE_TAG_ACCEPT_TIME_OK:
		OnTagAcceptTimeOK( rkPacket );
		return true;
	case STPK_BATTLE_MODE_ENTRY_TIMEOUT_CHECK:			// 난입 타임아웃 가능한지 체크하는 함수
		{
			ioHashString szTimeOutCheckUser;
			rkPacket >> szTimeOutCheckUser;

			if( szTimeOutCheckUser != m_pCreator->GetOwnerName() )		// 내 자신의 이름이 아니면 나감
				return true;

			ioBaseChar* pOwner = m_pCreator->GetOwnerChar();
			// 살아있으면 True 전달, 아니면 다른 곳에서 Die패킷 보낼것으로 보임
			if( pOwner->GetState() != CS_DIE/* && pOwner->GetHP().m_fCurValue <= 0.0f*/ )
			{
				int iReturnUserHPRate = (int)((pOwner->GetHP().m_fCurValue / pOwner->GetHP().m_fMaxValue) * 100.0f);
				SP2Packet kPacket( CTPK_BATTLE_MODE_ENTRY_STATE_OK );
				kPacket << m_pCreator->GetOwnerName();
				kPacket << iReturnUserHPRate;
				kPacket << true;
				TCPNetwork::SendToServer( kPacket );
			}
		}
		return true;

	//case STPK_FLAG_KILL_POINT:
	//	OnFlagKillPoint( rkPacket );
	//	return true;
	}

	return false;
}


bool ioBattleMode::ProcessUDPPacket( SP2Packet &rkPacket )
{
	if( ioPlayMode::ProcessUDPPacket( rkPacket ) )
		return true;

	return false;
}

void ioBattleMode::SetResultScoreWnd()
{
	RoundResultMainWnd *pResult = dynamic_cast<RoundResultMainWnd*>(g_GUIMgr.FindWnd( ROUND_RESULT_MAIN_WND ));
	if( !pResult )	return;

	// RoundHistory를 참조하는 곳이 많아 일단 갱신. 나중에 구조개편 검토 필요
	int iHisRoundBlue = m_iBlueRoundWinCnt;
	int iHisRoundRed  = m_iRedRoundWinCnt;

	if( P2PNetwork::IsNetworkPlaying() )
	{
		int iRoundCount = min( m_iCurRoundCount, (int)m_vRoundHistory.size() );
		if ( iRoundCount >= 1 )
		{
			for(int i = 0;i < iRoundCount - 1;i++)
			{
				iHisRoundBlue -= m_vRoundHistory[ i ].iBluePoint;
				iHisRoundRed  -= m_vRoundHistory[ i ].iRedPoint;
			}
			m_vRoundHistory[ iRoundCount - 1 ].iBluePoint = max( iHisRoundBlue, 0 );
			m_vRoundHistory[ iRoundCount - 1 ].iRedPoint  = max( iHisRoundRed, 0 );
		}
		/*
		for(int i = 0;i < m_iCurRoundCount - 1;i++)
		{
			iHisRoundBlue -= m_vRoundHistory[ i ].iBluePoint;
			iHisRoundRed  -= m_vRoundHistory[ i ].iRedPoint;
		}
		m_vRoundHistory[ m_iCurRoundCount - 1 ].iBluePoint = max( iHisRoundBlue, 0 );
		m_vRoundHistory[ m_iCurRoundCount - 1 ].iRedPoint  = max( iHisRoundRed, 0 );
		*/
	}
	//

    m_dwCurResultShowDuration = m_dwRoundResultDuration;

	DWORD dwTimeGap = FRAMEGETTIME() - m_dwPlayStartTime;
	if( m_dwCurRoundDuration > dwTimeGap )
		ContributeRoundAllPercent( m_dwRoundDuration - (m_dwCurRoundDuration - dwTimeGap), m_dwRoundDuration );
	else
		ContributeRoundAllPercent( m_dwRoundDuration, m_dwRoundDuration );

	pResult->ClearRoundInfo();
	for(int i = 0 ; i < GetRecordCharCnt(); i++ )
	{
		RoundInfo kInfo;
		if( GetRoundInfo( i, kInfo ) )
			pResult->AddRoundInfo( kInfo );
	}

	pResult->SetTeamScore( m_iBlueRoundWinCnt, m_iRedRoundWinCnt );

	SetResultBriefingWnd();
}

void ioBattleMode::SetTotalScoreWnd( IntVec &rvClassTypeList,
										   IntVec &rvClassPointList,
										   float fBlueGuildTeamBonus,
										   float fRedGuildTeamBonus,
										   float fBonusArray[BA_MAX] )
{
	TotalResultMainWnd *pTotalResult = dynamic_cast<TotalResultMainWnd*>(g_GUIMgr.FindWnd( TOTAL_RESULT_MAIN_WND ));
	if( !pTotalResult )	return;

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
			else if( g_ShuffleRoomMgr.IsShuffleRoom() )
				g_ShuffleRoomMgr.UpdateUserLevel( kInfo.m_szName, kInfo.m_iLevel );
		}
	}

	pTotalResult->SetGuildTeamBonus( fBlueGuildTeamBonus, fRedGuildTeamBonus );
	pTotalResult->SetFinalRoundPoint( rvClassTypeList, rvClassPointList, fBonusArray );

	int iHisRoundBlue = m_iBlueRoundWinCnt;
	int iHisRoundRed  = m_iRedRoundWinCnt;

	if( P2PNetwork::IsNetworkPlaying() )
	{
		int iRoundCount = min( m_iCurRoundCount, (int)m_vRoundHistory.size() );
		if ( iRoundCount >= 1 )
		{
			for(int i = 0;i < iRoundCount - 1;i++)
			{
				iHisRoundBlue -= m_vRoundHistory[ i ].iBluePoint;
				iHisRoundRed  -= m_vRoundHistory[ i ].iRedPoint;
			}
			m_vRoundHistory[ iRoundCount - 1 ].iBluePoint = max( iHisRoundBlue, 0 );
			m_vRoundHistory[ iRoundCount - 1 ].iRedPoint  = max( iHisRoundRed, 0 );
		}
		/*
		for(int i = 0;i < m_iCurRoundCount - 1;i++)
		{
			iHisRoundBlue -= m_vRoundHistory[ i ].iBluePoint;
			iHisRoundRed  -= m_vRoundHistory[ i ].iRedPoint;
		}
		m_vRoundHistory[ m_iCurRoundCount - 1 ].iBluePoint = max( iHisRoundBlue, 0 );
		m_vRoundHistory[ m_iCurRoundCount - 1 ].iRedPoint  = max( iHisRoundRed, 0 );
		*/
	}

	m_dwCurResultShowDuration = m_dwFinalRoundResultDuration;


	int iBluePoint = (int)( m_fFinalBluePoint * FLOAT100 );
	int iRedPoint = (int)( m_fFinalRedPoint * FLOAT100 );
	pTotalResult->SetTeamScore( GetOwnerTeam(), iBluePoint, iRedPoint );
	SetTotalResultBriefingWnd();
}

void ioBattleMode::CheckEndSceneLookAt()
{
	/*
	// 옵저버는 다르게 처리
	if( g_BattleRoomMgr.IsObserverUser( g_MyInfo.GetPublicID() ) )
	{
		ioBaseChar *pTarget = m_pCreator->GetCameraTargetChar();
		if( pTarget )
		{
			m_vEndLookAt		= pTarget->GetWorldPosition();
			m_szEndLookCharName = pTarget->GetCharName();
			pTarget->SetEndFocusEffect( GetRoundEndEffect() );
		}
		else
		{
			m_vEndLookAt = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
			m_szEndLookCharName.Clear();
		}
	}
	else
	{
		ioPlayMode::CheckEndSceneLookAt();
	}
	*/

	ioBaseChar *pOwner = m_pCreator->GetOwnerChar();

	if( !m_LastKillChar.IsEmpty() )
	{
		BattleModeRecord *pKillRecord = FindRecordCharName( m_LastKillChar );
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
			BattleModeRecord *pDieRecord = FindRecordCharName( m_LastDieChar );
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

void ioBattleMode::ClearModeData()
{
	m_LastDieChar.Clear();
	m_LastKillChar.Clear();
	m_vWaitingPos.clear();

	g_SoundMgr.AllSoundStop();
	g_SoundMgr.StopStream();
	m_pCreator->HidingAllFieldItem();
	m_bUserSetting = false;
}

bool ioBattleMode::IsNeedRevival( ioBaseChar *pChar )
{
	return true;
	/*
	if( !pChar ) return false;

	if( !m_LastKillChar.IsEmpty() && m_LastKillChar == pChar->GetCharName() )
		return true;

	return false;
	*/
}

ioBaseChar* ioBattleMode::CheckMostNearChar( ioBaseChar *pChar )
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

int ioBattleMode::GetCurRank( const ioHashString& szCharName )
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

int ioBattleMode::GetContributePerChar( const ioHashString& szName )
{
	int iCharCnt = m_vCharRecordList.size();

	for(int i = 0;i < iCharCnt;i++)
	{
		BattleModeRecord *pRecord = &m_vCharRecordList[i];
		if( !pRecord )	continue;
		if( !pRecord->pChar ) continue;

		if( pRecord->pChar->GetCharName() == szName )
		{
			return pRecord->Contribute.m_iPrevContributePer;
		}
	}

	return 0;
}

void ioBattleMode::SetKillPointEmoticon( TeamType eTeam, const ioHashString &szAttacker )
{
	int iCharCnt = m_vCharRecordList.size();

	for(int i = 0;i < iCharCnt;i++)
	{
		BattleModeRecord *pRecord = &m_vCharRecordList[i];
		if( !pRecord )	continue;
		if( !pRecord->pChar ) continue;

		if( pRecord->pChar->GetTeam() != eTeam ) continue;
		if( ioBaseChar::IsOnlyMyEmoticon() && !pRecord->pChar->IsOwnerChar() ) continue;
		//if( pRecord->pChar->GetCharName() == szAttacker ) continue;

		int iRate = 0;
		if( eTeam == TEAM_RED )
		{
			iRate = (int)(GetRedKillRate() * 100);
			pRecord->pChar->SetKillPointEmoticon( m_KillPointEmoticonRed, m_KillPointNum, m_KillPointPer, iRate );

			if( pRecord->pChar->IsOwnerChar() && !m_KillPointSound.IsEmpty() )
				g_SoundMgr.PlaySound( m_KillPointSound );
		}
		else if( eTeam == TEAM_BLUE )
		{
			iRate = (int)(GetBlueKillRate() * 100);
			pRecord->pChar->SetKillPointEmoticon( m_KillPointEmoticonBlue, m_KillPointNum, m_KillPointPer, iRate );

			if( pRecord->pChar->IsOwnerChar() && !m_KillPointSound.IsEmpty() )
				g_SoundMgr.PlaySound( m_KillPointSound );
		}
	}
}


void ioBattleMode::OnRoundEnd( SP2Packet &rkPacket )
{
	int iWinTeam;
	rkPacket >> iWinTeam;
	rkPacket >> m_iRedRoundWinCnt;
	rkPacket >> m_iBlueRoundWinCnt;

	rkPacket >> m_fFinalRedPoint;
	rkPacket >> m_fFinalBluePoint;

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
			for( int k=0; k < iKillCharCnt; ++k )
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

	g_GUIMgr.HideWnd( BATTLE_MODE_TIME_GAUGE_WND );
	g_GUIMgr.HideWnd( BATTLE_MODE_FUNCTION_WND );
	g_GUIMgr.HideWnd( BATTLE_MODE_BLUE_USER_INFO_WND );
	g_GUIMgr.HideWnd( BATTLE_MODE_RED_USER_INFO_WND );

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

	// 배틀모드 인게임 용 변수들 초기화
 	g_BattleRoomMgr.ClearBattleModeInfoAtEndOfBattleMode();
	m_mapFirstCheck.clear();
	// 떨궈진 장비 체크 초기화
	//g_BattleRoomMgr.ResetUnmoutEquipItem();
}

void ioBattleMode::SetMapPassiveBuff( ioBaseChar *pChar )
{
	if( !pChar ) return;

	int iBuffCnt = m_vMapPassiveBuffList.size();
	if( iBuffCnt <= 0 ) return;

	bool bFirstOrder = false;
	if(g_BattleRoomMgr.GetUserBattleOrder(pChar->GetCharName()) == BATTLE_ORDER_FIRST)
		bFirstOrder = true;
	else
		pChar->SetState( CS_BATTLE_WAITING );

	// 같은 타입의 버프는 적용되지 않도록 함
	int iBuffType = 0;
	for( int i=0; i < iBuffCnt; ++i )
	{
		if(bFirstOrder /*&& m_vMapPassiveBuffList[i] == "000_protect_State3"*/)
			continue;
		if( !m_vMapPassiveBuffList[i].m_bFirstApply )
			continue;
		//pChar->AddNewBuff( m_vMapPassiveBuffList[i].m_szBuffName, "", "", NULL );
	}
}

void ioBattleMode::SetMapPassiveBuffDuringBattle( ioBaseChar *pChar, int iBuffIdx )
{
	if( !pChar ) return;

	int iBuffCnt = m_vMapPassiveBuffList.size();
	if( iBuffCnt <= 0 ) return;

	if(iBuffIdx == -1)
	{
		for( int i=0; i < iBuffCnt; ++i )
		{
			if( !pChar->AddNewBuff( m_vMapPassiveBuffList[i].m_szBuffName, "", "", NULL ) )
				LOG.PrintTimeAndLog( 0, "%s - Failed AddNewBuff Index : %d", __FUNCTION__, i );
		}
	}
	else
	{
		if(iBuffIdx < iBuffCnt)
		{
			if( !pChar->AddNewBuff( m_vMapPassiveBuffList[ iBuffIdx ].m_szBuffName, "", "", NULL ) )
				LOG.PrintTimeAndLog( 0, "%s - Failed AddNewBuff Index : %d", __FUNCTION__, iBuffIdx );
		}
	}
}

void ioBattleMode::RemoveMapPassiveBuffDuringBattle( ioBaseChar *pChar, int iBuffIdx )
{
	if( !pChar ) return;

	int iBuffCnt = m_vMapPassiveBuffList.size();
	if( iBuffCnt <= 0 ) return;

	if(iBuffIdx == -1)
	{
		for( int i=0; i < iBuffCnt; ++i )
		{
			pChar->RemoveBuff( m_vMapPassiveBuffList[i].m_szBuffName );
		}
	}
	else
	{
		if(iBuffIdx < iBuffCnt)
			pChar->RemoveBuff( m_vMapPassiveBuffList[ iBuffIdx ].m_szBuffName );
	}
}

void ioBattleMode::ChangeCharIcon( ioBaseChar* pChangeChar )
{
	// 배틀모드 중 출전하고 있는 상태라면 용병 교체 불가
	TeamType eTeam = (TeamType)g_BattleRoomMgr.GetUserTeam(pChangeChar->GetCharName());
	if( eTeam == TEAM_BLUE)
	{
		BattleModeBlueSideUserInfoWnd *pBattleModeBlueSide = dynamic_cast< BattleModeBlueSideUserInfoWnd * >( g_GUIMgr.FindWnd( BATTLE_MODE_BLUE_USER_INFO_WND ) );
		if( pBattleModeBlueSide )
			pBattleModeBlueSide->ChangeSoldierIcon(pChangeChar);
	}
	else if( eTeam == TEAM_RED)
	{
		BattleModeRedSideUserInfoWnd *pBattleModeRedSide = dynamic_cast< BattleModeRedSideUserInfoWnd * >( g_GUIMgr.FindWnd( BATTLE_MODE_RED_USER_INFO_WND ) );
		if( pBattleModeRedSide )
			pBattleModeRedSide->ChangeSoldierIcon(pChangeChar);
	}
	///////////////////////////////
}

void ioBattleMode::SwitchTagZone( bool bOn, ioHashString szTagReqName )
{
	// 일단 내 클라에서만 켜지게 해보자...
	int iBattleWaitPos = g_BattleRoomMgr.GetUserBattleWaitPosIdx(szTagReqName);
	//if(iBattleWaitPos != 1)
	//	iBattleWaitPos -= 1;
	//else
	//	return;
	//TeamType eTeam = (TeamType)g_BattleRoomMgr.GetUserTeam(szTagReqName);
	//if(eTeam == TEAM_RED)
	//	iBattleWaitPos += 2;

	m_pCreator->SetMapCustomObject( bOn, iBattleWaitPos );
}

void ioBattleMode::SwitchNonTaggedDisplay( bool bOn, ioHashString szTagReqName )
{
	int iBattleWaitPos = g_BattleRoomMgr.GetUserBattleWaitPosIdx(szTagReqName);
	//LOG.PrintTimeAndLog( 0, "%s - iBattleWaitPos : %d, TagReqName : %s", __FUNCTION__, iBattleWaitPos, szTagReqName.c_str() );
	//m_pCreator->EnableMapDefaultEffect( bOn, iBattleWaitPos );
	m_pCreator->SetMapCustomEffect( bOn, iBattleWaitPos );
}

void ioBattleMode::SaveEntrySoldierPos( ioBaseChar* pChar )
{
	if(pChar)
	{
		TeamType eTeam = (TeamType)g_BattleRoomMgr.GetUserTeam( pChar->GetCharName() );
		if(eTeam == TEAM_BLUE)
			m_vBlueTeamEntryPos = pChar->GetWorldPosition();
		else if(eTeam == TEAM_RED)
			m_vRedTeamEntryPos = pChar->GetWorldPosition();
	}
}

void ioBattleMode::SwitchEntrySoldierPosToWaitPos( ioBaseChar* pChar )
{
	if(pChar)
	{
		TeamType eTeam = (TeamType)g_BattleRoomMgr.GetUserTeam( pChar->GetCharName() );
		if(eTeam == TEAM_BLUE)
			pChar->SetWorldPosition( m_vBlueTeamEntryPos );
		else if(eTeam == TEAM_RED)
			pChar->SetWorldPosition( m_vRedTeamEntryPos );
	}
}

void ioBattleMode::SwitchBattleState( bool bUserState )
{
	// 추후 거의 다한 후에 함수화시키자...
}

void ioBattleMode::StartTagButtonInactive()
{
	BattleModeFunctionWnd *pFunctionWnd = dynamic_cast<BattleModeFunctionWnd*>(g_GUIMgr.FindWnd( BATTLE_MODE_FUNCTION_WND ));
	if( pFunctionWnd )
	{
		m_dwTagBtnDisableTime = pFunctionWnd->GetTagBtnDisableDuration();
		m_dwStartTagBtnDisableTime = FRAMEGETTIME() - m_dwPlayStartTime;
	}
}

int ioBattleMode::GetScore( TeamType eTeam )
{
	if( eTeam == TEAM_BLUE )
		return m_iBlueTeamScore;
	else if( eTeam == TEAM_RED )
		return m_iRedTeamScore;
	
	return 0;
}

void ioBattleMode::ShowTagAlarm( ioHashString& userName )
{
	// 태그 성공하여 알림 
	TeamType eTeamType = (TeamType)g_BattleRoomMgr.GetBattleUserTeam(userName);
	if( eTeamType == TEAM_BLUE )
	{
		BattleModeBlueSideUserInfoWnd *pBattleModeBlueSide = dynamic_cast< BattleModeBlueSideUserInfoWnd * >( g_GUIMgr.FindWnd( BATTLE_MODE_BLUE_USER_INFO_WND ) );
		if( pBattleModeBlueSide )
			pBattleModeBlueSide->ShowTag( userName );
	}
	else if ( eTeamType == TEAM_RED )
	{
		BattleModeRedSideUserInfoWnd *pBattleModeRedSide = dynamic_cast< BattleModeRedSideUserInfoWnd * >( g_GUIMgr.FindWnd( BATTLE_MODE_RED_USER_INFO_WND ) );
		if( pBattleModeRedSide )
			pBattleModeRedSide->ShowTag( userName );
	}
}

void ioBattleMode::ShowEntryAlarm( ioHashString& userName )
{
	// 난입 알림
	TeamType eTeamType = (TeamType)g_BattleRoomMgr.GetBattleUserTeam(userName);
	if( eTeamType == TEAM_BLUE )
	{
		BattleModeBlueSideUserInfoWnd *pBattleModeBlueSide = dynamic_cast< BattleModeBlueSideUserInfoWnd * >( g_GUIMgr.FindWnd( BATTLE_MODE_BLUE_USER_INFO_WND ) );
		if( pBattleModeBlueSide )
			pBattleModeBlueSide->ShowEntry( userName );
	}
	else if ( eTeamType == TEAM_RED )
	{
		BattleModeRedSideUserInfoWnd *pBattleModeRedSide = dynamic_cast< BattleModeRedSideUserInfoWnd * >( g_GUIMgr.FindWnd( BATTLE_MODE_RED_USER_INFO_WND ) );
		if( pBattleModeRedSide )
			pBattleModeRedSide->ShowEntry( userName );
	}
}

int ioBattleMode::GetCurEntryCount()
{
	return m_iCurEntryCount;
}

int ioBattleMode::GetMaxEntryCount()
{
	return m_iMaxEntryCount;
}

void ioBattleMode::RotateSoldierToCenter( ioBaseChar* pOwner )
{
	D3DXVECTOR3 vCenter( 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3 vAimedDir = vCenter - pOwner->GetMidPositionByRate();
	D3DXVec3Normalize( &vAimedDir, &vAimedDir );
	pOwner->SetTargetRotToDir( vAimedDir, true );

}

void ioBattleMode::ChangeCurFighterView()
{
	ioHashString FightUserNick = g_BattleRoomMgr.FindSameTeamUserInBattle( g_MyInfo.GetPublicID() );
	if( !FightUserNick.IsEmpty() )
	{
		int iCharCnt = GetRecordCharCnt();
		m_pCreator->ChangeCurViewChar( FightUserNick, iCharCnt );
	}
}

void ioBattleMode::AddBattleModeUserInfoToUI( ioBaseChar* pAddChar )
{
	BattleModeTimeGaugeWnd *pTimeGauge = dynamic_cast< BattleModeTimeGaugeWnd * >( g_GUIMgr.FindWnd( BATTLE_MODE_TIME_GAUGE_WND ) );
	if( pTimeGauge == NULL ) return;

	if( g_BattleRoomMgr.CheckUserBattleFightFlag( pAddChar->GetCharName() ) )
	{
		TeamType eTeam = (TeamType)g_BattleRoomMgr.GetUserTeam(pAddChar->GetCharName());
		pTimeGauge->SetBattleModeUser( pAddChar->GetCharName(), eTeam );
	}
}

void ioBattleMode::ShareSkillGaugeForSameTeam( ioHashString szEndUserName, bool bDie )
{
	// [스킬 게이지 공유] 들어갈 용병 스킬 게이지 -> 출전할 용병 스킬 게이지
	ioBaseChar* pMyChar = m_pCreator->GetOwnerChar();
	
	if( pMyChar->GetCharName() == szEndUserName )
	{
		for( int iEquipType = 1; iEquipType <= 4; iEquipType++ )		// 장비는 무기, 갑옷, 투구, 망토 4가지라서... ItemType::IT_WEAPON ~ ItemType::IT_CLOAK
		{
 			ioItem *pCurItem = pMyChar->GetEquipedItem( iEquipType - 1 );
			if( !bDie )
			{
				// 반패시브는 공유되지 않음
				ioSkill *pSkill = pMyChar->GetEquipedSkill( iEquipType - 1 );
				ioPassiveSkill *pPassiveSkill = ToPassiveSkill( pSkill );
				if( pPassiveSkill && pPassiveSkill->UsePassiveGauge() )
					continue;

				if( pCurItem )
					pCurItem->ShareSkillGauge();		// 패킷 전달
				else
				{
					// 스킬 게이지 값 0.0f으로 보내기
					SP2Packet kPacket( CUPK_SYNC_SKILL_GAUGE );
					kPacket << GetOwnerName() << (iEquipType - 1);
					kPacket << 0.0f;
					P2PNetwork::SendToAllUser( kPacket );
					float fCurSkillGauge = 0.0f;
					LOG.PrintTimeAndLog( 0, "Send CUPK_SYNC_SKILL_GAUGE - Owner : %s, SkillGauge : %5.3f", GetOwnerName().c_str(), fCurSkillGauge );
				}
			}
			else
			{
				// 스킬 게이지 값 0.0f으로 보내기
				SP2Packet kPacket( CUPK_SYNC_SKILL_GAUGE );
				kPacket << GetOwnerName() << (iEquipType - 1);
				kPacket << 0.0f;
				P2PNetwork::SendToAllUser( kPacket );
				float fCurSkillGauge = 0.0f;
				LOG.PrintTimeAndLog( 0, "Send CUPK_SYNC_SKILL_GAUGE - Owner : %s, SkillGauge : %5.3f", GetOwnerName().c_str(), fCurSkillGauge );
			}
		}
	}
}

void ioBattleMode::SetUnmountingEquipItem( const ioHashString &szUserID, int iItemType, bool bUnmount )
{
	if( iItemType >= ES_WEAPON && iItemType <= ES_CLOAK)
		g_BattleRoomMgr.SetUnmoutEquipItem( szUserID, iItemType, bUnmount );
}

bool ioBattleMode::GetUnmountingEquipItem( const ioHashString &szUserID, int iItemType )
{
	// 정상적인 파라메터 값이면 IT_WEAPON : 1 ~ IT_CLOAK : 4 까지만 오므로 -1을 해줌
	iItemType -= 1;
	// 배열 인덱스 벗어나면 따로 장비 해제를 체크하지 않게 false로 반환
	if( iItemType < 0 || iItemType >= MAX_INVENTORY)
		return false;
	return g_BattleRoomMgr.GetUnmoutEquipItem( szUserID, iItemType );;
}

void ioBattleMode::ChangeScore( TeamType eTeam )
{
	if( eTeam == TEAM_BLUE )
		m_iRedTeamScore++;
	else if( eTeam == TEAM_RED )
		m_iBlueTeamScore++;
}

void ioBattleMode::SwapUserWaitPosIdxInUI( ioBaseChar* pChar_IN, ioBaseChar* pChar_OUT )
{
	if( pChar_IN && pChar_OUT && pChar_IN->GetTeam() == pChar_OUT->GetTeam() )
	{
		TeamType eTeam = pChar_IN->GetTeam();
		if( eTeam == TEAM_BLUE)
		{
			BattleModeBlueSideUserInfoWnd *pBattleModeBlueSide = dynamic_cast< BattleModeBlueSideUserInfoWnd * >( g_GUIMgr.FindWnd( BATTLE_MODE_BLUE_USER_INFO_WND ) );
			if( pBattleModeBlueSide )
				pBattleModeBlueSide->SwapUserWaitPosIdx( pChar_IN, g_BattleRoomMgr.GetUserBattleWaitPosIdx( pChar_IN->GetCharName() ), pChar_OUT, g_BattleRoomMgr.GetUserBattleWaitPosIdx( pChar_OUT->GetCharName() ) );
		}
		else if( eTeam == TEAM_RED)
		{
			BattleModeRedSideUserInfoWnd *pBattleModeRedSide = dynamic_cast< BattleModeRedSideUserInfoWnd * >( g_GUIMgr.FindWnd( BATTLE_MODE_RED_USER_INFO_WND ) );
			if( pBattleModeRedSide )
				pBattleModeRedSide->SwapUserWaitPosIdx( pChar_IN, g_BattleRoomMgr.GetUserBattleWaitPosIdx( pChar_IN->GetCharName() ), pChar_OUT, g_BattleRoomMgr.GetUserBattleWaitPosIdx( pChar_OUT->GetCharName() ) );
		}
	}
}

bool ioBattleMode::AllFirstUserCheck()
{
	bool bAllCheck = true;
	mapFirstCheck::iterator iter;
	for( iter = m_mapFirstCheck.begin(); iter != m_mapFirstCheck.end(); iter++ )
	{
		if( (*iter).second == false )
			bAllCheck = false;
	}
	return bAllCheck;
}
