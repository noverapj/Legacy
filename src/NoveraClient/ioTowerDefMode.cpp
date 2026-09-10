#include "StdAfx.h"

#include "ioTowerDefMode.h"

#include "ioNpcChar.h"
#include "ioPlayStage.h"
#include "ioSlotMgr.h"

#include "GUI/TowerDefWnd.h"
#include "GUI/MiniMap.h"
#include "GUI/CenterHelpWnd.h"
#include "GUI/MonsterCoinWnd.h"
#include "GUI/RoundStateMainWnd.h"
#include "GUI/MiniSoldierSelectWnd.h"
#include "GUI/ioObserverWnd.h"
#include "GUI/LadderTeamWnd.h"
#include "GUI/TimeScoreWnd.h"
#include "GUI/TotalResultMainWnd.h"

#define MONSTER_ARRIVAL_CHECK		2000

ioTowerDefMode::ioTowerDefMode( ioPlayStage *pCreator, ModeType eMode ) : ioMonsterSurvivalMode( pCreator )
{
	m_ModeType = eMode;
	m_bShowTime = false;
	m_pKeyBack = NULL;
	m_pReviveMsg  = NULL;
	m_pMsgBack  = NULL;
	m_szReviveKey = "";
}

ioTowerDefMode::~ioTowerDefMode()
{
	ioGrowthLevel *pLevel = m_pCreator->GetGrowthLevel( g_MyInfo.GetPublicID());

	if( pLevel )
		pLevel->ResetInstantExtraGrowthLevel();

	m_pCreator->UpdateGrowthDataOne( m_pCreator->GetOwnerName() );

	SAFEDELETE(m_pKeyBack);
	SAFEDELETE(m_pReviveMsg);
	SAFEDELETE(m_pMsgBack);

	ClearMode();
}

void ioTowerDefMode::ClearMode()
{
	ioPlayMode::ClearMode();
}

void ioTowerDefMode::LoadNaviInfo(ioINILoader &rkLoader)
{
	char szBuf[MAX_PATH];

	rkLoader.LoadString_e( "navigation_map", "xml/pve_navi.xml", szBuf, MAX_PATH );

	if(!LoadBoxSpaceFromFile(m_naviInfo, szBuf))
		LOG.PrintTimeAndLog(0, "Error : Loading XML navi Failed");
}

void ioTowerDefMode::LoadMap()
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
	LoadNaviInfo(kLoader);

	m_dwRankTime[RANK_S] = kLoader.LoadInt_e("time_s", 420000);
	m_dwRankTime[RANK_A] = kLoader.LoadInt_e("time_a", 900000);
	m_dwRankTime[RANK_B] = kLoader.LoadInt_e("time_b", 1200000);

	m_iAveragePlayer = kLoader.LoadInt_e( "average_player", 2 );
	m_fBalanceMaxRate= kLoader.LoadFloat_e( "balance_max_value", 4.0f );
	m_iUseMonsterCoinCnt = kLoader.LoadInt_e( "use_monster_coin_cnt", 0 );

	char szEffectFile[MAX_PATH];
	kLoader.LoadString_e( "red_team_change", "sp2_red_symbol_change.txt", szEffectFile, MAX_PATH );
	m_strRedTeamFX = szEffectFile;

	kLoader.LoadString_e( "blue_team_change", "sp2_blue_symbol_change.txt", szEffectFile, MAX_PATH );
	m_strBlueTeamFX = szEffectFile;

	kLoader.SetTitle_e( "TreasureCard" );
	m_TreasureCardRandomIndex = kLoader.LoadInt_e( "TreasureRandomIndex", 0 );

	MiniMap *pMiniMap = dynamic_cast<MiniMap*>(g_GUIMgr.FindWnd( MINIMAP_WND ));

	if( pMiniMap )
		pMiniMap->SetTimeImage(kLoader);
}

void ioTowerDefMode::InitMode( int iMapIndex )
{
	m_iModeMapIndex = iMapIndex;

	LoadMap();
	LoadPushStructs();
	LoadMachineStruct();
	LoadObjectItems();
	LoadFuntionalObjects();
//	LoadCharacters();
	LoadDisableSkillList();
	LoadTurnHelp();
	LoadHighTurn();
	LoadModeGrowthRate();

	ioINILoader kLoader(GetModeINIFileName(m_ModeType));

	char szBuf[MAX_PATH];
	kLoader.SetTitle_e( "common_info" );
	m_dwRoundDuration = kLoader.LoadInt_e( "round_time", 300000 );
	m_dwCurRoundDuration = m_dwRoundDuration;

	m_fScoreWarningRate = kLoader.LoadFloat_e( "score_warning_rate", FLOAT05 );
	m_fRevivalRecoveryHP = kLoader.LoadFloat_e( "revival_recovery_hp_rate", FLOAT05 );

	m_dwStartCoinTime = kLoader.LoadInt_e( "start_coin_use_time", 0 );
	kLoader.LoadString_e( "coin_revival_buff", "", szBuf, MAX_PATH );
	m_szCoinRevivalBuff = szBuf;

	m_dwKeepLiveTime = kLoader.LoadInt_e( "npc_death_time", 3000 );

	LoadRevivalTime( kLoader );
	LoadModeResource( kLoader );

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

	m_szMapName = g_ModeSelectInfo.GetBattleMapTitle( g_BattleRoomMgr.GetSelectModeIndex(), g_BattleRoomMgr.GetSelectMapIndex() );

	g_SlotMgr.UpdateAllItem();

	//훈련병일 경우
	if ( g_MyInfo.GetGradeLevel() == 0 )
	{
		ioWnd* ptutorialkeyboardwnd = g_GUIMgr.AddWnd( "XML/tutorialkeyboardwnd.xml" );
		if ( ptutorialkeyboardwnd )
			ptutorialkeyboardwnd->ShowWnd();
	}
}

void ioTowerDefMode::LoadModeResource( ioINILoader &rkLoader )
{
	MonsterSurvivalHelWnd *pHelpWnd = dynamic_cast<MonsterSurvivalHelWnd *>( g_GUIMgr.AddWnd( "XML/monstersurvivalhelpwnd.xml" ) );
	if( pHelpWnd )
		pHelpWnd->SetPlayMode( this );


	//g_ModeUIController.SetKOMsgWnd( g_GUIMgr.AddWnd( "XML/kochatmsgwnd.xml") );
	g_ModeUIController.SetKOMsgWnd( NULL );
	g_ModeUIController.SetCenterInfoWnd( g_GUIMgr.AddWnd( "XML/CenterInfoWnd.xml" ) );
	g_ModeUIController.SetCenterKillInfoWnd( NULL ); 

	g_GUIMgr.AddWnd( "XML/TowerDefWnd.xml");

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

	m_pKeyBack = g_GUIMgr.CreateFrame("slot_keyoutline");
	m_pMsgBack = g_GUIMgr.CreateFrame("gameui_15px_textback");
	m_pReviveMsg = g_UIImageSetMgr.CreateImage( "BaseImage_Defense003", "pve_text_revival" );

	m_vRoundHistory.clear();
}

void ioTowerDefMode::OnNotifyCharDie( ioBaseChar *pDieChar, ioBaseChar *pKillChar, ioBaseChar *pBestAttacker,
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

	ioNpcChar *pNpc = ToNpcChar(pDieChar);

	if(pNpc && pKillChar)
	{
		SetCallbyAllNpcBuff(pNpc->GetNpcCode(), false);

		if( !ToNpcChar( pKillChar ) ) //&& !g_BattleRoomMgr.IsObserverUser(pKillChar->GetCharName()) )
		{
			MonsterSurvivalRecord *pRecord = FindRecordCharName( pKillChar->GetCharName() );

			if( pRecord)
			{
				pRecord->Contribute.m_nInstantLevel++;

				ioGrowthLevel *pLevel = m_pCreator->GetGrowthLevel( pKillChar->GetCharName() );

				if( pLevel )
					pLevel->IncreaseExtraGrowthLevel();

				m_pCreator->UpdateGrowthDataOne( pKillChar->GetCharName() );

#ifndef SHIPPING
				LOG.PrintTimeAndLog(0,
					"monster kill %s intstant level : %d  Player Exp : %d",
					pKillChar->GetCharName().c_str(), pRecord->Contribute.m_nInstantLevel, pKillChar->GetLevel());
#endif
			}
		}
	}

}

// 
// void ioTowerDefMode::NotifyEntityDie(ioPlayEntity *pEntity, ioPlayEntity *pKiller, ioBaseChar *pBestAttacker,
// 								   float fLastRate, float fBestRate )
// {
// 	ioBaseChar *pDieChar = ToBaseChar( pEntity );
// 	ioBaseChar *pKillChar = ToBaseChar( pKiller );
// 
// 	ioNpcChar *pNpc = ToNpcChar(pDieChar);
// 
// 	switch( pEntity->GetSubType() )
// 	{
// 	case ioPlayEntity::PST_CHAR:
// 		if(pNpc)
// 		{
// 			if( pNpc->GetNpcType() == NT_BIGTOWER)
// 			{
// 				pNpc->FillMaxHP();
// 				pNpc->SetTeam(pKiller->GetTeam());
// 				pNpc->SetStartMotionState();
// 			}
// 			else
// 			{
// 				OnNotifyCharDie( pDieChar, pKillChar, pBestAttacker, fLastRate, fBestRate );
// 				UpdateKillDeathRecord( pEntity, pKiller, pBestAttacker );
// 			}
// 
// 			if( !ToNpcChar( pKillChar ) && !g_BattleRoomMgr.IsObserverUser(pKillChar->GetCharName()) )
// 			{
// 				MonsterSurvivalRecord *pRecord = FindRecordCharName( pKillChar->GetCharName() );
// 				pRecord->Contribute.m_nInstantLevel++;
// 
// 				ioGrowthLevel *pLevel = m_pCreator->GetGrowthLevel( pKillChar->GetCharName() );
// 
// 				if( pLevel )
// 					pLevel->IncreaseExtraGrowthLevel();
// 
// 				m_pCreator->UpdateGrowthDataOne( pKillChar->GetCharName() );
// 
// #ifndef SHIPPING
// 				LOG.PrintTimeAndLog(0,
// 					"monster kill %s intstant level : %d  Player Exp : %d",
// 					pKillChar->GetCharName().c_str(), pRecord->Contribute.m_nInstantLevel, pKillChar->GetLevel());
// #endif
// 			}
// 		}
// 		break;
// 
// 	default:
// 		break;
// 	}
// }

void ioTowerDefMode::ProcessLvlup()
{
	if(GetOwnerChar() )
	{
	}
}

void ioTowerDefMode::ProcessSymbolAlphaRate()
{
	ioBaseChar *pMyChar = GetOwnerChar();
	if( !pMyChar )	return;

	ioCamera *pCamera = g_App.GetCamera();
	if( !pCamera )	return;

	D3DXVECTOR3 vMyPos = pMyChar->GetMidPositionByRate();
	D3DXVECTOR3 vDiff = pCamera->GetPosition() - vMyPos;
	ioRay kCamRay( vMyPos, vDiff );


	int iCharCount = GetRecordCharCnt();
	for( int i=0 ; i<iCharCount ; i++ )
	{
		ioBaseChar *pChar = GetRecordChar(i);
		if( !pChar ) continue;
		ioNpcChar *pNpcChar = ToNpcChar(pChar);
		if(!pNpcChar) continue;

		if(pNpcChar->GetNpcType() == NT_LAIR || pNpcChar->GetNpcType() == NT_BIGTOWER || 
			pNpcChar->GetNpcType() == NT_NEXUS)
		{
			if(!pNpcChar->IsNowAlphaChanging())
			{
				if( ioMath::TestIntersection( kCamRay, pNpcChar->GetWorldCollisionBox() ) )
					pNpcChar->SetTargetAlphaRate( 80 );
				else
					pNpcChar->SetTargetAlphaRate( MAX_ALPHA_RATE );
			}
		}
	}
}

void ioTowerDefMode::ProcessMode()
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
		ProcessSymbolAlphaRate();
		UpdateGauge();
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

void ioTowerDefMode::UpdateGauge()
{
	TowerDefWnd *pUserGauge = dynamic_cast< TowerDefWnd * >( g_GUIMgr.FindWnd( TOWERDEF_USER_GAUGE_WND ) );
	if( !pUserGauge ) return;

	//UNDONE : 유저 게이지
	if( pUserGauge->IsShow() )
	{
		ioHashStringVec vecName;
		pUserGauge->GetUserNameList(vecName);
		for(unsigned int i = 0; i < vecName.size(); i++)
		{
			MonsterSurvivalRecord *pRecord = FindRecordCharName( vecName[i].c_str() );
			if( pRecord )
			{	
				StatusValue &rkStatus = pRecord->pChar->GetHP();
				pUserGauge->UpdateGauge(vecName[i].c_str(), rkStatus.m_fCurValue, rkStatus.m_fMaxValue, pRecord->Contribute.m_nInstantLevel );
			}
		}
	}

	MonsterSurvivalRecord *pRecord = FindRecordCharName( pUserGauge->GetNpcName());
	if( pRecord )
	{	
		StatusValue &rkStatus = pRecord->pChar->GetHP();
		pUserGauge->UpdateNpcGauge(pUserGauge->GetNpcName(), rkStatus.m_fCurValue, rkStatus.m_fMaxValue, pRecord->Contribute.m_nInstantLevel );
	}
}


void ioTowerDefMode::ProcessArrivalMonster()  // 안씀
{
	if( FRAMEGETTIME() < m_dwMobCheckTime)
		return;

	m_dwMobCheckTime += MONSTER_ARRIVAL_CHECK;

	DWORD dwCurrTime = FRAMEGETTIME() - m_dwStartTurnTime;


	std::vector <stMonster>::iterator itr = m_vecReserveMonster.begin();
	for(; itr != m_vecReserveMonster.end(); ++itr)
	{
		if((itr->bStart) && (itr->dwStartTime < dwCurrTime))
		{
			ioNpcChar *pNpcChar = g_NpcMgr.CreateNpcChar( NPC_NOT_INDEX, itr->dwNpcCode, m_pCreator, this );
			if( !pNpcChar ) continue;

			pNpcChar->FillMaxHP();
			pNpcChar->SetCharName( itr->kNpcName );
			pNpcChar->SetSyncUser( itr->kSyncName );	

			TeamType e_Team = TEAM_RED;

			if( itr->nGroupIdx != 0)
			{
				std::hash_map<int, ioHashString>::iterator itr_group;
				itr_group = m_mapGroup.find(itr->nGroupIdx);
				if(itr_group != m_mapGroup.end() )
				{
					MonsterSurvivalRecord *pRecord = FindRecordCharName( itr_group->second );
					if( pRecord != NULL )
					{
						ioNpcChar *pNpcLair = ToNpcChar( pRecord->pChar );

						if( pNpcLair)
						{
							e_Team = pNpcLair->GetTeam();
#ifndef SHIPPING
							if(e_Team == TEAM_RED)
								LOG.PrintTimeAndLog(0, "Name : %s Team Red: %d", itr_group->second.c_str(), (int)e_Team);
							else if(e_Team == TEAM_BLUE)
								LOG.PrintTimeAndLog(0, "Name : %s Team Blue: %d", itr_group->second.c_str(), (int)e_Team);
#endif // _SHIP
						}
					}
				}
			}

			pNpcChar->SetTeam( e_Team );
			pNpcChar->StartAI( 0 , itr->fStartXPos, itr->fStartZPos );
			pNpcChar->SetGradeUpRandomSeed( g_BattleRoomMgr.GetIndex() + itr->kNpcName.GetHashCode() );
			pNpcChar->ReSetGradeLevel( 0 );
			AddNewRecord( pNpcChar );
			itr->bStart = false;
#ifndef SHIPPING
			if(e_Team == TEAM_RED)
				LOG.PrintTimeAndLog(0, "Name : %s Team Red group: %d", pNpcChar->GetCharName().c_str(), itr->nGroupIdx);
			else if(e_Team == TEAM_BLUE)
				LOG.PrintTimeAndLog(0, "Name : %s Team Blue group: %d", pNpcChar->GetCharName().c_str(), itr->nGroupIdx);

			LOG.PrintTimeAndLog(0, "start time %d, currTime %d ", itr->dwStartTime, dwCurrTime  );
#endif
		}
	}
}

void ioTowerDefMode::RemoveRecord( const ioHashString &rkName )
{
	bool bUser = false;
	int iCharCnt = m_vCharRecordList.size();
	for( int i=0 ; i<iCharCnt ; i++ )
	{
		if( m_vCharRecordList[i].szName == rkName )
		{
			if(m_vCharRecordList[i].pChar)
			{
				ioBaseChar *pChar = m_vCharRecordList[i].pChar;
				if( !ToNpcChar(pChar) && !g_BattleRoomMgr.IsObserverUser(pChar->GetCharName()) )
					bUser = true;
			}

			m_vCharRecordList.erase( m_vCharRecordList.begin() + i );
			break;
		}
	}
	if( bUser)
		ShowUserGauage();

	UpdateUserRank();
}

void ioTowerDefMode::OnResetMonster(SP2Packet &rkPacket)
{
	DWORD dwNpcCode;
	ioHashString kNpcName, kSyncName;
	int	nTeam = TEAM_RED;
	int nDeadNpc = 0;
	int nMaxNpc = 0;
	float fStartXPos, fStartZPos;
	rkPacket >> nDeadNpc >> nMaxNpc;

	for(int i = 0; i < nDeadNpc; i++)
	{
		rkPacket >> kNpcName;
		MonsterSurvivalRecord *pRecord = FindRecordCharName( kNpcName );

		if( pRecord && ToNpcChar( pRecord->pChar ))
		{
			ioNpcChar* pNpc = ToNpcChar( pRecord->pChar );
			pNpc->SetDieState();
			SetCallbyAllNpcBuff(pNpc->GetNpcCode(), false);
			AddEraseNpc(kNpcName);
		}
	}

	for(int i = 0; i < nMaxNpc; i++)
	{
		rkPacket >> dwNpcCode >> kNpcName >> nTeam >> kSyncName >> fStartXPos >> fStartZPos;

		RemoveEraseNpc( kNpcName );
		ioNpcChar *pNpcChar = NULL;
		MonsterSurvivalRecord *pRecord = FindRecordCharName( kNpcName );
		if( pRecord && ToNpcChar( pRecord->pChar ) )
		{
			pNpcChar = ToNpcChar( pRecord->pChar );
			if( pNpcChar )
			{
				pNpcChar->Revival();
				pNpcChar->SetState( CS_CREATEMOTION );
				pNpcChar->FillMaxHP();
				pNpcChar->SetCharName( kNpcName );
				pNpcChar->SetSyncUser( kSyncName );
				pNpcChar->SetTeam( (TeamType)nTeam );
				pNpcChar->StartAI( 0, fStartXPos, fStartZPos, true );
				CreateLairTeamChangeEffect(pNpcChar->GetWorldPosition(), (TeamType)nTeam);

				SetCallbyAllNpcBuff(pNpcChar->GetNpcCode(), true);
			}
		}
		else
		{
			ioNpcChar *pNpcChar = g_NpcMgr.CreateNpcChar( NPC_NOT_INDEX, dwNpcCode, m_pCreator, this );
			if( !pNpcChar ) 
				continue;

			pNpcChar->FillMaxHP();
			pNpcChar->SetState( CS_CREATEMOTION );
			pNpcChar->SetCharName( kNpcName );
			pNpcChar->SetSyncUser( kSyncName );
			pNpcChar->SetTeam( (TeamType)nTeam );
			pNpcChar->StartAI( 0, fStartXPos, fStartZPos, true );
			pNpcChar->SetDisableGauge(true);
			AddNewRecord( pNpcChar );
			CreateLairTeamChangeEffect(pNpcChar->GetWorldPosition(), (TeamType)nTeam);
			SetCallbyAllNpcBuff(pNpcChar->GetNpcCode(), true);
		}
	}
}

void ioTowerDefMode::SetReviveShortKey()
{
	if( 0xff == g_SlotMgr.GetReviveKey())
		m_szReviveKey.Clear();
	else
		m_szReviveKey = g_KeyManager.GetKeyText(g_SlotMgr.GetReviveKey());
}

void ioTowerDefMode::OnUseConsumption(SP2Packet &rkPacket)
{
	DWORD dwType;
	rkPacket >> dwType;

	if( COMPARE(dwType, ioEtcItem::EIT_ETC_CONSUMPTION_BUFF01, ioEtcItem::EIT_ETC_CONSUMPTION_SKILL01))
	{
		ioHashString szChar;
		ioHashString szBuff;
		rkPacket >> szChar >> szBuff;
		ioBaseChar* pChar = m_pCreator->GetBaseChar(szChar.c_str());
		if(pChar)
			pChar->AddNewBuff(szBuff, szChar, "", NULL );

		g_SlotMgr.UpdateAllItem();
	}
	else if( COMPARE(dwType, ioEtcItem::EIT_ETC_CONSUMPTION_SKILL01, ioEtcItem::EIT_ETC_CONSUMPTION_SKILL32 + 1))
	{
		ioHashString szChar;
		ioHashString szBuff;
		rkPacket >> szChar >> szBuff;
		ioBaseChar* pChar = m_pCreator->GetBaseChar(szChar.c_str());
		if(pChar)
			pChar->SetUseSlotSkill(szBuff, SUT_NORMAL);

		g_SlotMgr.UpdateAllItem();
	}
	else
	{
		if(dwType == ioEtcItem::EIT_ETC_CONSUMPTION_REVIVE)
		{
			ioHashString szUseUser;
			
			rkPacket >> szUseUser;

			if( szUseUser == g_MyInfo.GetPublicID() )
			{
				if( m_CurrentMagicZone.m_dwCollisionTime == 0 )
					m_pCreator->SetScreenBlindDownState();
			}

			// HP 풀 회복 + 아이템 풀 착용
			MonsterSurvivalRecord *pRecord = FindRecordCharName( szUseUser );
			if( pRecord && pRecord->pChar )
			{
				pRecord->pChar->DestroyAllItem();

				// 드랍존에 있으면 부활 위치로 이동
				if( pRecord->pChar->IsCharDropZonePos() )
				{
					pRecord->pChar->SetWorldPosition( GetRevivalPos( pRecord->pChar ) );
					LOG.PrintTimeAndLog(0, "포션 : 드랍존 부활");
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

				ioHashString szItemUser;
				rkPacket >> szItemUser;

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
								pItem->SetCurSkillGauge( 0.0f );

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

				pRecord->dwCurDieTime = 0;

				// 채팅 알림
// 				ioComplexStringPrinter kPrinter;
// 				g_ChatMgr.PresetChatComplexString( MSG_INFO_CHAT, kPrinter );
// 
// 				kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(),STR(5), pRecord->pChar->GetCharName().c_str() );
// 				kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(),STR(6) );
// 				g_ChatMgr.SetChatComplexString( STR(2), kPrinter );

				g_SlotMgr.UpdateAllItem();
			}
			else
			{
				LOG.PrintTimeAndLog( 0, "ioPlayStage::OnUseMonsterCoin - %s Not Exist", szUseUser.c_str() );
			}
		}
	}
}

bool ioTowerDefMode::ProcessTCPPacket(SP2Packet &rkPacket)
{
	if( ioMonsterSurvivalMode::ProcessTCPPacket( rkPacket ) )
		return true;

	switch( rkPacket.GetPacketID() )
	{
	case STPK_RESET_NPCTOWER:
		OnResetMonster(rkPacket);
		return true;

	case STPK_EXP_UP_BY_TIME:
//		OnAllPlayerExpUp(rkPacket);
		return true;

	case STPK_SPAWN_MONSTER:
		OnSpawnMonster(rkPacket);
		return true;

	case STPK_SLOT_BUFF:
		OnUseConsumption(rkPacket);
		return true;

	case STPK_TREASURE_CARD_COMMAND:
		ErrorReport::SetPosition( 1031, 23 );
		OnTreasureCardCommand( rkPacket );
		ErrorReport::SetPosition( 1031, 24 );
		return true;
	}

	return false;
}

void ioTowerDefMode::OnSpawnMonster(SP2Packet &rkPacket)
{
	int nNpc = 0;
	rkPacket >> nNpc;

	for(int i = 0; i < nNpc; i++)
	{
		DWORD dwNpcCode, dwStartTime;
		float fStartXPos, fStartZPos;
		ioHashString kNpcName, kSyncName;
		int nTeam;
		int nGrowthLvl;
		int nGroupIdx = 0 ;
		bool bGroupBoss = false;

		rkPacket >> dwNpcCode >> kNpcName >> kSyncName >> nTeam >> nGrowthLvl >> bGroupBoss >> nGroupIdx >>
			dwStartTime >> fStartXPos >> fStartZPos;

		ioNpcChar *pNpcChar = g_NpcMgr.CreateNpcChar( NPC_NOT_INDEX, dwNpcCode, m_pCreator, this );
		if( !pNpcChar ) continue;

		pNpcChar->FillMaxHP();
		pNpcChar->SetCharName( kNpcName );
		pNpcChar->SetSyncUser( kSyncName );
		pNpcChar->SetTeam( (TeamType)nTeam );
		pNpcChar->StartAI( dwStartTime , fStartXPos, fStartZPos );
		pNpcChar->SetNpcGradeShow(true);
		pNpcChar->SetGradeUpRandomSeed( g_BattleRoomMgr.GetIndex() + kNpcName.GetHashCode() );
		pNpcChar->ReSetGradeLevel( nGrowthLvl );
		pNpcChar->SetDisableGauge(true);
//		pNpcChar->SetGaugeScale(0.5f, FLOAT05);

		AddNewRecord( pNpcChar );
	}
}

void ioTowerDefMode::OnRoundJoin( SP2Packet &rkPacket )
{
	ioMonsterSurvivalMode::OnRoundJoin(rkPacket);

	if(!g_BattleRoomMgr.IsObserverUser(g_MyInfo.GetPublicID()))
		ShowUserGauage();

	m_szMapName = g_ModeSelectInfo.GetBattleMapTitle( g_BattleRoomMgr.GetSelectModeIndex(), g_BattleRoomMgr.GetSelectMapIndex() );
}

void ioTowerDefMode::OnRoundEnd( SP2Packet &rkPacket )
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
		}
	}

	//훈련병일 경우
	if ( g_MyInfo.GetGradeLevel() == 0 )
		g_GUIMgr.HideWnd( TUTORIAL_KEYBOARD_WND );
}


void ioTowerDefMode::OnUseMonsterCoin( SP2Packet &rkPacket )
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

			g_MyInfo.UseModeStartMonsterCoin( m_iUseMonsterCoinCnt, false, nUseCoinCount, nUseGoldCoinCount );
			g_ChatMgr.SetSystemMsg("몬스터던전 플레이를 위해 몬스터코인 %d개 소모.", m_iUseMonsterCoinCnt );
		}
		break;

	case USE_MONSTER_COIN_FAIL_CNT:
		{
			g_ChatMgr.SetSystemMsg("보유중인 코인이 없습니다.");
			m_bUsingMonsterCoin = false;
			int iMonsterCoin, iGoldMonsterCoin;
			rkPacket >> iMonsterCoin >> iGoldMonsterCoin;
			// 코인 리플레쉬
			g_MyInfo.SetEtcMonsterCoin( iMonsterCoin );

			// 유저 이탈
			SP2Packet kPacket( CTPK_EXIT_ROOM );
			kPacket << EXIT_ROOM_MONSTER_COIN_LACK << 0 << true;
			TCPNetwork::SendToServer( kPacket );
			g_BattleRoomMgr.LeaveRoom();
		}		
		break;
	case USE_MONSTER_COIN_LIVE_USER:
		break;
	}
}


void ioTowerDefMode::OnMonsterInfoSync( SP2Packet &rkPacket )
{
	ShowUserGauage();

	if( m_iUseMonsterCoinCnt > 0 )        // 코인 사용하는 난이도
	{
		m_dwCurStartCoinTime = FRAMEGETTIME();
		m_dwCurStartCoinSec  = 0xFFFFFFFF;
	}

	int i, iMaxNpc;
	// 일반 NPC
	rkPacket >> iMaxNpc;

	if( iMaxNpc == 0 )
	{
		// 중간 참여를 했는데 살아있는 몬스터가 없으면 마법진 연다.
	}
	else
	{
		for(i = 0;i < iMaxNpc;i++)
		{
			DWORD dwNpcCode, dwStartTime;
			float fStartXPos, fStartZPos;
			ioHashString kNpcName, kSyncName;
			int nTeam;
			int nGroupIdx = 0;
			bool bGroupBoss = false;

			rkPacket >> dwNpcCode >> kNpcName >> kSyncName >> nTeam >> bGroupBoss >> nGroupIdx >>
				dwStartTime >> fStartXPos >> fStartZPos;

			ioNpcChar *pNpcChar = g_NpcMgr.CreateNpcChar( NPC_NOT_INDEX, dwNpcCode, m_pCreator, this );
			if( !pNpcChar ) continue;

			pNpcChar->FillMaxHP();
			pNpcChar->SetCharName( kNpcName );
			pNpcChar->SetSyncUser( kSyncName );
			pNpcChar->SetTeam( (TeamType)nTeam );
			pNpcChar->StartAI( dwStartTime , fStartXPos, fStartZPos );
			pNpcChar->SetGradeUpRandomSeed( g_BattleRoomMgr.GetIndex() + kNpcName.GetHashCode() );
			pNpcChar->ReSetGradeLevel( 0 );
			pNpcChar->SetDisableGauge(true);
// 			if( pNpcChar->GetNpcType() == 0)
// 				pNpcChar->SetGaugeScale(0.5f, FLOAT05);

			AddNewRecord( pNpcChar );
		}
	}
}

void ioTowerDefMode::OnAllPlayerExpUp(SP2Packet &rkPacket)
{
	int nExp = 0;
	rkPacket >> nExp;

	RecordList::iterator it = m_vCharRecordList.begin();

	for(; it != m_vCharRecordList.end(); ++it)
	{
		ioBaseChar* pChar = it->pChar;
		if( !ToNpcChar( pChar ) && !g_BattleRoomMgr.IsObserverUser(pChar->GetCharName()) )
		{
			it->Contribute.m_nInstantExp += nExp;
			it->Contribute.m_nInstantLevel++;

			ioGrowthLevel *pLevel = m_pCreator->GetGrowthLevel( pChar->GetCharName() );

			if( pLevel )
				pLevel->IncreaseExtraGrowthLevel();

			m_pCreator->UpdateGrowthDataOne( pChar->GetCharName() );
#ifndef SHIPPING
			LOG.PrintTimeAndLog(0, "%s intstant level : %d  Player Exp : %d", it->pChar->GetCharName().c_str(), it->Contribute.m_nInstantLevel, pChar->GetLevel());
#endif
		}
	}
}

void ioTowerDefMode::ProcessPlayState()
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
		// 시작 코인 사용
		CheckStartCoinUse();
	}
}

void ioTowerDefMode::CheckStartCoinUse()
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
			int iMonsterCoinCount = g_MyInfo.GetEtcMonsterCoin();
			if( iMonsterCoinCount < m_iUseMonsterCoinCnt )
			{
				MonsterCoinUseWnd *pCoinUseWnd = dynamic_cast< MonsterCoinUseWnd *>( g_GUIMgr.FindWnd( MONSTER_COIN_USE_WND ) );
				if( pCoinUseWnd )
				{
					char szMent[MAX_PATH] = "";
					sprintf_e( szMent, "%s - %s", g_ModeSelectInfo.GetBattleModeTitle( g_BattleRoomMgr.GetSelectModeIndex() ).c_str(), 
						g_ModeSelectInfo.GetBattleMapTitle( g_BattleRoomMgr.GetSelectModeIndex(), g_BattleRoomMgr.GetSelectMapIndex() ).c_str() );
					m_bUsingMonsterCoin = pCoinUseWnd->UpdateUseStartCoinLack( szMent, m_iUseMonsterCoinCnt, m_dwCurStartCoinSec, dwMaxSec );
				}
				else
				{
/*
					//UJ 140120, 보안강화: 몬스터 코인 없이 계속 플레이할 수 있는 치팅을 막는다
					SP2Packet kPacket( CTPK_EXIT_ROOM );
					kPacket << EXIT_ROOM_MONSTER_COIN_LACK << 0 << true;
					TCPNetwork::SendToServer( kPacket );
					g_BattleRoomMgr.LeaveRoom();*/
				}
			}
			else if( m_dwCurStartCoinSec > dwMaxSec )
			{				
/*
				// 시작 코인 사용
				SP2Packet kPacket( CTPK_USE_MONSTER_COIN );
				kPacket << USE_MONSTER_COIN_START << m_iUseMonsterCoinCnt;
				TCPNetwork::SendToServer( kPacket );
				g_GUIMgr.HideWnd( MONSTER_COIN_USE_WND );
				m_bUsingMonsterCoin = true;*/
			}
		}
	}
}

void ioTowerDefMode::ShowUserGauage()
{
	//UNDONE : User Gauage 
 	if( m_ModeState != MS_PLAY )
		return;

	TowerDefWnd *pUserGauge = dynamic_cast< TowerDefWnd * >( g_GUIMgr.FindWnd( TOWERDEF_USER_GAUGE_WND ) );
	if( pUserGauge )
	{
		pUserGauge->ResetAllGauge();
		pUserGauge->ShowWnd();

		MonsterSurvivalRecord *pRecord = FindRecordCharName( g_MyInfo.GetPublicID() );
		if( pRecord )
		{
			if(pRecord->pChar && !g_BattleRoomMgr.IsObserverUser(g_MyInfo.GetPublicID()))
				pUserGauge->SetUser(pRecord->pChar, pRecord->Contribute.m_nInstantLevel);
		}

		RecordList::iterator it = m_vCharRecordList.begin();

		for(; it != m_vCharRecordList.end(); ++it)
		{
			ioBaseChar* pChar = it->pChar;
			if( !ToNpcChar( pChar ) && !g_BattleRoomMgr.IsObserverUser(pChar->GetCharName()) )
			{
				if( pChar->GetCharName() != g_MyInfo.GetPublicID() && pChar->GetState() != CS_VIEW)
					pUserGauge->SetUser(pChar, it->Contribute.m_nInstantLevel);
			}
		}
	}
}

void ioTowerDefMode::DamageContribute( const ioHashString szAttacker, const ioHashString szTarget, 
										float fDamage, DWORD dwItemCode, int iWeaponType )
{
	ioMonsterSurvivalMode::DamageContribute(szAttacker, szTarget, fDamage, dwItemCode, iWeaponType );


	ioHashString szTargetbyObserver = "";

	bool bShowOb = false;

	if( g_BattleRoomMgr.IsObserverUser( g_MyInfo.GetPublicID() ) || m_pCreator->IsStealthMode() )
	{
		ioBaseChar *pTarget = m_pCreator->GetCameraTargetChar();
		if( pTarget )
			szTargetbyObserver = pTarget->GetCharName();
	}

	if( (szAttacker == g_MyInfo.GetPublicID()) || (szAttacker == szTargetbyObserver) )
	{
		TowerDefWnd *pUserGauge = dynamic_cast< TowerDefWnd * >( g_GUIMgr.FindWnd( TOWERDEF_USER_GAUGE_WND ) );
		if( pUserGauge )
		{
			MonsterSurvivalRecord *pRecord = FindRecordCharName(szTarget);
			if( pRecord )
			{
				if(pRecord->pChar && ToNpcChar(pRecord->pChar))
				{
					ioNpcChar* pNpc = ToNpcChar(pRecord->pChar);
					if((pNpc->GetTeam() == TEAM_RED) && (pUserGauge->GetNpcName() != pRecord->szName))
					{
						if(pNpc->GetNpcType() == NT_LAIR ||  pNpc->GetNpcType() == NT_BIGTOWER || pNpc->GetNpcType() == NT_NEXUS )	
							pUserGauge->SetNPC( pNpc, 1, true);
						else
							pUserGauge->SetNPC( pNpc, 1);
					}
				}
			}
		}
	}

	MonsterSurvivalRecord *pRecord = FindRecordCharName(szTarget);
	if( !pRecord || !pRecord->pChar ) return;
	if(ToNpcChar(pRecord->pChar)) return;

	pRecord->Contribute.m_iWounded += fDamage;
}

void ioTowerDefMode::OnTurnStart( SP2Packet &rkPacket )
{
	ShowUserGauage();

	g_GUIMgr.HideWnd( MONSTER_BOSS_GAUGE_WND );

	DWORD dwPrevHighTurn = m_dwCurrentHighTurn;
	DWORD dwPrevLowTurn  = m_dwCurrentLowTurn;

	if( m_bBossTurn )
	{
		StartTurnCharHPRecovery();
	}

	rkPacket >> m_bBossTurn >> m_dwAllCurTurn >> m_dwAllMaxTurn >> m_dwCurrentTurnHelpID >> m_dwCurRoundDuration
		>> m_dwCurrentHighTurn >> m_dwCurrentLowTurn >> m_dwReduceNpcCreateTime;

	m_StartPosRandom.SetRandomSeed( g_BattleRoomMgr.GetIndex() + m_dwAllCurTurn );
	m_RevivalPosRandom.SetRandomSeed( g_BattleRoomMgr.GetIndex() + m_dwAllCurTurn );

	m_dwPlayStartTime = FRAMEGETTIME();
	m_dwRoundDuration = m_dwCurRoundDuration;
	m_dwCheckWarningTime = 0;

//	TurnStartEvent( dwPrevHighTurn, dwPrevLowTurn );

	int i = 0;
	// 데스몬 죽음!!
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
		m_dwStartTurnTime = m_dwMobCheckTime = FRAMEGETTIME();
		m_dwMobCheckTime += MONSTER_ARRIVAL_CHECK;
	}	// 두번째 턴부터는 이벤트 작동
	else if( COMPARE( m_dwCurrentTurnHelpID, 0, (DWORD)m_TurnHelpList.size() ) )
	{
		StartTurnViewCharStart();
	}
}

void ioTowerDefMode::StartMonsterData( SP2Packet &rkPacket, DWORD dwCameraEventTime, ioHashString &rFirstMonsterName )
{
	DWORD dwFirstStartTime = 0xFFFFFFFF;
	int iMaxNpc;
	rkPacket >> iMaxNpc;

	for(int i = 0;i < iMaxNpc;i++)
	{
		DWORD dwNpcCode, dwStartTime;
		float fStartXPos, fStartZPos;
		ioHashString kNpcName, kSyncName;
		int nTeam;
		int nGroupIdx = 0 ;
		bool bGroupBoss = false;

		rkPacket >> dwNpcCode >> kNpcName >> kSyncName >> nTeam >> bGroupBoss >> nGroupIdx >>
			dwStartTime >> fStartXPos >> fStartZPos;

		if( dwNpcCode == 0)
			continue;
		ioNpcChar *pNpcChar = g_NpcMgr.CreateNpcChar( NPC_NOT_INDEX, dwNpcCode, m_pCreator, this );
		if( !pNpcChar ) continue;

		dwStartTime += dwCameraEventTime;
		pNpcChar->FillMaxHP();
		pNpcChar->SetCharName( kNpcName );
		pNpcChar->SetSyncUser( kSyncName );
		pNpcChar->SetTeam( (TeamType)nTeam );
		pNpcChar->StartAI( dwStartTime , fStartXPos, fStartZPos );
		pNpcChar->SetGradeUpRandomSeed( g_BattleRoomMgr.GetIndex() + kNpcName.GetHashCode() );
		pNpcChar->ReSetGradeLevel( 0 );
		pNpcChar->SetDisableGauge(true);

		AddNewRecord( pNpcChar );

		if( dwStartTime < dwFirstStartTime )
		{
			dwFirstStartTime  = dwStartTime;
			rFirstMonsterName = kNpcName;
		}
	}
}

bool ioTowerDefMode::IsAliveNpcbyCode(const DWORD dwNpcCode)
{
	int iCharCnt = m_vCharRecordList.size();
	for( int i=0 ; i<iCharCnt ; i++ )
	{
		MonsterSurvivalRecord &rkRecord = m_vCharRecordList[i];
		if( rkRecord.pChar == NULL ) continue;

		ioNpcChar *pNpc = ToNpcChar( rkRecord.pChar );

		if(!pNpc) continue;

		if( pNpc->GetNpcCode() == dwNpcCode)
		{
			if(pNpc->GetState() != CS_DIE && pNpc->GetState() != CS_LOADING)
				return true;
			else
				return false;
		}
	}

	return false;
}

void ioTowerDefMode::SetCallbyAllNpcBuff(const DWORD dwNpcCode, bool bAlive)
{
	int iCharCnt = m_vCharRecordList.size();
	for( int i=0 ; i<iCharCnt ; i++ )
	{
		MonsterSurvivalRecord &rkRecord = m_vCharRecordList[i];
		if( rkRecord.pChar == NULL ) continue;

		ioNpcChar *pNpc = ToNpcChar( rkRecord.pChar );

		if(!pNpc) continue;

		pNpc->SetCallByNpcBuff(dwNpcCode, bAlive);
	}
}

void ioTowerDefMode::ItemEquipAfterUIProcess( ioBaseChar *pChar, int iItemType )
{
	if( iItemType != ioItem::IT_HELMET )
		return;

	TowerDefWnd *pUserGauge = dynamic_cast< TowerDefWnd * >( g_GUIMgr.FindWnd( TOWERDEF_USER_GAUGE_WND ) );
	if( pUserGauge && pUserGauge->IsShow())
		pUserGauge->SetHelmChange(pChar);
}

void ioTowerDefMode::ItemReleaseAfterUIProcess( ioBaseChar *pChar, int iItemType )
{
	if( iItemType != ioItem::IT_HELMET )
		return;

	TowerDefWnd *pUserGauge = dynamic_cast< TowerDefWnd * >( g_GUIMgr.FindWnd( TOWERDEF_USER_GAUGE_WND ) );
	if( pUserGauge && pUserGauge->IsShow())
		pUserGauge->SetHelmChange(pChar);
}

void ioTowerDefMode::ItemEntityChangeAfterUIProcess( ioBaseChar *pChar, int iItemType )
{
	if( iItemType != ioItem::IT_HELMET )
		return;

	TowerDefWnd *pUserGauge = dynamic_cast< TowerDefWnd * >( g_GUIMgr.FindWnd( TOWERDEF_USER_GAUGE_WND ) );
	if( pUserGauge && pUserGauge->IsShow())
		pUserGauge->SetHelmChange(pChar);
}



void ioTowerDefMode::CreateLairTeamChangeEffect( const D3DXVECTOR3 &vPos, TeamType eTeam )
{
	if( eTeam != TEAM_BLUE && eTeam != TEAM_RED )
		return;

	if( eTeam == TEAM_BLUE && !m_strBlueTeamFX.IsEmpty() )
	{
		m_pCreator->CreateMapEffect( m_strBlueTeamFX, vPos, ioMath::UNIT_ALL );
	}
	else if( eTeam == TEAM_RED && !m_strRedTeamFX.IsEmpty() )
	{
		m_pCreator->CreateMapEffect( m_strRedTeamFX, vPos, ioMath::UNIT_ALL );
	}
}

void ioTowerDefMode::SetResultScoreWnd()
{

}

bool ioTowerDefMode::IsNeedRevival( ioBaseChar *pChar )
{
	if(ToNpcChar(pChar))
	{
		ioNpcChar* pNpc = ToNpcChar(pChar);
		if(pNpc->GetNpcType() == NT_NEXUS)
		{
			pChar->Revival();
			return true;
		}
	}
	return false;
}

void ioTowerDefMode::SetRoundEndMotion()
{	
	int iCharCnt = GetRecordCharCnt();	
	for( int i=0; i<iCharCnt; i++ )
	{
		ioBaseChar *pChar = GetRecordChar( i );
		if( !pChar ) continue;
		if( pChar->GetState() == CS_OBSERVER ) continue;
		if( g_BattleRoomMgr.IsObserverUser(pChar->GetCharName()) ) continue;

		if( m_WinTeamType == WTT_BLUE_TEAM)
		{
			if(pChar->GetTeam() == TEAM_BLUE)
				pChar->SetRoundEndMotion( RM_WIN );
			else
				pChar->SetRoundEndMotion( RM_LOSE );
		}
		else
		{
			if(pChar->GetTeam() == TEAM_BLUE)
				pChar->SetRoundEndMotion( RM_LOSE );
			else
				pChar->SetRoundEndMotion( RM_WIN );
		}

		pChar->StartNoInputDelayTime( 10000 );
	}	

	TowerDefWnd *pUserGauge = dynamic_cast<TowerDefWnd*>( g_GUIMgr.FindWnd( TOWERDEF_USER_GAUGE_WND ));
	if(pUserGauge && pUserGauge->IsShow())
		pUserGauge->HideWnd();

	if(m_WinTeamType == WTT_BLUE_TEAM)
	{	// 플레이중인 유저만 체크
		if( !g_BattleRoomMgr.IsObserverUser( g_MyInfo.GetPublicID() ) && !m_pCreator->IsStealthMode() )
		{
			g_QuestMgr.QuestCompleteTerm( QC_PVE_ROUND_CLEAR, GetModeSubNum(), g_BattleRoomMgr.GetModeStartPlayerCount(), (float)(FRAMEGETTIME() - m_dwStartTurnTime)/1000 );
			g_QuestMgr.QuestOccurTerm( QO_PVE_ROUND_CLEAR, GetModeSubNum() );
		}
	}
}


void ioTowerDefMode::UpdateMiniMapSpotPoint()
{
	MiniMap *pMiniMap = dynamic_cast<MiniMap*>(g_GUIMgr.FindWnd( MINIMAP_WND ));
	if( !pMiniMap )	return;

	ioPlayMode::UpdateMiniMapSpotPoint();

	pMiniMap->ClearPointList();

	if( m_ModeState == MS_RESULT)
		pMiniMap->UpdateTime(m_dwResultStartTime - m_dwPlayStartTime);
	else
		pMiniMap->UpdateTime(FRAMEGETTIME() - m_dwPlayStartTime);

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

			switch (pNpcChar->GetNpcType())
			{
			case NT_NEXUSGUARD:
			case NT_LAIRGUARD:
			case NT_TOWERGUARD:
				break;

			case NT_BIGTOWER:
				pMiniMap->AddPoint( vPos.x, vPos.z, MiniMap::SPOT_TOWER, pNpcChar->GetTeam(), dwGapTime );
				break;

			case NT_NEXUS:
				pMiniMap->AddPoint( vPos.x, vPos.z, MiniMap::SPOT_NEXUS, pNpcChar->GetTeam(), dwGapTime );
				break;

			case NT_LAIR:
				pMiniMap->AddPoint( vPos.x, vPos.z, MiniMap::SPOT_LAIR, pNpcChar->GetTeam(), dwGapTime );
				break;

			default:
				if( pNpcChar->IsDropZoneDownState() )
					pMiniMap->AddPoint( vPos.x, vPos.z, MiniMap::SPOT_NPC, pNpcChar->GetTeam(), 0 );
				else if( pNpcChar->IsDeathMonster() )
					pMiniMap->AddPoint( vPos.x, vPos.z, MiniMap::SPOT_NPC, pNpcChar->GetTeam(), dwGapTime );			
				else
					pMiniMap->AddPoint( vPos.x, vPos.z, MiniMap::SPOT_NPC, pNpcChar->GetTeam(), dwGapTime );
				break;
			}
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

void ioTowerDefMode::FillPlayRecordInfo( SP2Packet &rkPacket )
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
		rkPacket << pRecord->Contribute.m_iWounded;
		rkPacket << pRecord->Contribute.m_iPrevContributePer;
		rkPacket << pRecord->Contribute.m_nInstantLevel;
		rkPacket << pRecord->GetUniqueTotalKill();
		rkPacket << pRecord->GetUniqueTotalDeath();
		rkPacket << g_VictoriesMgr.GetVictories( pRecord->pChar->GetCharName() );
	}
	kCharReocrdNameList.clear();
}

void ioTowerDefMode::ApplyPlayRecordInfo( SP2Packet &rkPacket )
{
	int iCharCnt;
	rkPacket >> iCharCnt;
	for(int i = 0;i < iCharCnt;i++)
	{
		ioHashString szName;
		int iDamage, iAttackCnt, iKill, iDeath, iWounded, iPrevContributePer, nInstantLv, iUniqueTotalKill, iUniqueTotalDeath, iVictories;
		rkPacket >> szName >> iDamage >> iAttackCnt >> iKill >> iDeath >> iWounded >> iPrevContributePer >> nInstantLv >> iUniqueTotalKill >> iUniqueTotalDeath >> iVictories;

		MonsterSurvivalRecord *pRecord = FindRecordCharName( szName );
		if( pRecord )
		{
			pRecord->Contribute.m_iDamage			= iDamage;
			pRecord->Contribute.m_iAttackCnt		= iAttackCnt;
			pRecord->Contribute.m_iKill             = iKill;
			pRecord->Contribute.m_iDeath            = iDeath;
			pRecord->Contribute.m_iWounded			= iWounded;
			pRecord->Contribute.m_iPrevContributePer= iPrevContributePer;
			pRecord->Contribute.m_nInstantLevel		= nInstantLv;
			pRecord->iUniqueTotalKill = iUniqueTotalKill;
			pRecord->iUniqueTotalDeath= iUniqueTotalDeath;

			if( !g_LadderTeamMgr.IsLadderTeam() )
				g_VictoriesMgr.SetVictories( szName, iVictories );

			ioGrowthLevel *pLevel = m_pCreator->GetGrowthLevel( szName );

			if( pLevel )
				pLevel->SetExtraGrowthLevel(nInstantLv);

			m_pCreator->UpdateGrowthDataOne( szName );
		}
	}

	ContributeConvert();
	ContributeRoundAllPercent();
	UpdateRoundInfoList();
	CheckTotalChamp();
}


void ioTowerDefMode::ContributeConvert()
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
				pRecord->Contribute.m_iContribute = (int)(m_vContributeTable[vInfo[i].m_iRank] * 1.2f);
			pRecord->Contribute.m_iKillRank = vInfo[i].m_iRank;
		}
	}
	vInfo.clear();

	// 피격 처리
	for(i = 0;i < iCharCnt;i++)
	{
		MonsterSurvivalRecord *pRecord = FindRecordCharName( kCharReocrdNameList[i] );
		if( !pRecord ) continue;
		if( !pRecord->pChar ) continue;
		if( pRecord->pChar->GetState() == CS_OBSERVER ) continue;
		if( g_BattleRoomMgr.IsObserverUser(pRecord->pChar->GetCharName()) ) continue;

		ContributeInfo Contribute;
		Contribute.m_iRecordArray = i;
		Contribute.m_iContributePoint = 10000000 - pRecord->Contribute.m_iWounded;
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
			pRecord->Contribute.m_iWoundedRank = vInfo[i].m_iRank;
		}
	}
	vInfo.clear();
	kCharReocrdNameList.clear();
}

// treasure card
void ioTowerDefMode::OnTreasureCardCommand( SP2Packet &rkPacket )
{
	int iResult;
	rkPacket >> iResult;

	switch( iResult )
	{
	case TREASURE_CARD_CMD_TIME_OUT:
		{
			// 종료 !!!
			TotalResultMainWnd *pTotalResultMainWnd = dynamic_cast< TotalResultMainWnd * >( g_GUIMgr.FindWnd( TOTAL_RESULT_MAIN_WND ) );
			if( pTotalResultMainWnd && pTotalResultMainWnd->IsShow() )
				pTotalResultMainWnd->TreasureCardTimeOut( rkPacket );
		}
		break;
	case TREASURE_CARD_CMD_CLICK:
		{
			// 카드 뒤집기
			TotalResultMainWnd *pTotalResultMainWnd = dynamic_cast< TotalResultMainWnd * >( g_GUIMgr.FindWnd( TOTAL_RESULT_MAIN_WND ) );
			if( pTotalResultMainWnd && pTotalResultMainWnd->IsShow() )
				pTotalResultMainWnd->TreasureCardClick( rkPacket );
			else
			{
				DWORD dwID;
				ioHashString kName;
				rkPacket >> dwID >> kName;
				if( dwID == 0 || kName == g_MyInfo.GetPublicID() )
					TCPNetwork::MouseBusy( false );
			}
		}
		break;
	case TREASURE_CARD_MAX_COUNT_CLICK:
		{
			// 카드 뒤집기
			TotalResultMainWnd *pTotalResultMainWnd = dynamic_cast< TotalResultMainWnd * >( g_GUIMgr.FindWnd( TOTAL_RESULT_MAIN_WND ) );
			if( pTotalResultMainWnd && pTotalResultMainWnd->IsShow() )
				pTotalResultMainWnd->TreasureCardClick( rkPacket );
			else
			{
				DWORD dwID;
				ioHashString kName;
				rkPacket >> dwID >> kName;
				if( dwID == 0 || kName == g_MyInfo.GetPublicID() )
					TCPNetwork::MouseBusy( false );
			}

			g_GUIMgr.SetMsgBox( MB_OK, NULL, "갯수초과" );
		}
		break;
	case TREASURE_CARD_MAX_SLOT_CLICK:
		{
			// 카드 뒤집기
			TotalResultMainWnd *pTotalResultMainWnd = dynamic_cast< TotalResultMainWnd * >( g_GUIMgr.FindWnd( TOTAL_RESULT_MAIN_WND ) );
			if( pTotalResultMainWnd && pTotalResultMainWnd->IsShow() )
				pTotalResultMainWnd->TreasureCardClick( rkPacket );
			else
			{
				DWORD dwID;
				ioHashString kName;
				rkPacket >> dwID >> kName;
				if( dwID == 0 || kName == g_MyInfo.GetPublicID() )
					TCPNetwork::MouseBusy( false );
			}

			g_GUIMgr.SetMsgBox( MB_OK, NULL, "빈칸없음" );
		}
		break;
	}
}


int ioTowerDefMode::ContributeMyRank( int &rRankValue1, int &rRankValue2, int &rRankValue3, int &rRankValue4 )
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
		Contribute.m_iContributePoint = pRecord->iUniqueTotalKill;//  Contribute.m_iKill;
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

	if(iCharCnt < 4)
		rRankValue1++;

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
		Contribute.m_iContributePoint = pRecord->Contribute.m_iWounded;
		Contribute.m_iGradeLevel      = pRecord->pChar->GetLevel();
		Contribute.m_szName           = pRecord->szName;
		vInfo.push_back( Contribute );
	}
	std::sort( vInfo.begin(), vInfo.end(), R_ContributeRankSort() );
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

	if(iCharCnt < 4)
		rRankValue2++;

	vInfo.clear();
	kCharReocrdNameList.clear();

	DWORD dwPlayingTime = 0;

	if( m_ModeState == MS_RESULT)
		dwPlayingTime = m_dwResultStartTime - m_dwPlayStartTime;
	else
		dwPlayingTime = FRAMEGETTIME() - m_dwPlayStartTime;

	if(dwPlayingTime < m_dwRankTime[RANK_S])
		rRankValue3 = 0;
	else if(dwPlayingTime < m_dwRankTime[RANK_A])
		rRankValue3 = 1;
	else if(dwPlayingTime < m_dwRankTime[RANK_B])
		rRankValue3 = 2;
	else
		rRankValue3 = 3;

	return iInfoCnt;
}


int ioTowerDefMode::ContributeRank( const ioHashString &rkName, int &rRankValue1, int &rRankValue2, int &rRankValue3, int &rRankValue4 )
{
	rRankValue1 = rRankValue2 = rRankValue3 = rRankValue4 = -1;

	bool bObserver = false;
	ioBaseChar *pOwner = m_pCreator->GetBaseChar( rkName );
	if( pOwner && ( g_BattleRoomMgr.IsObserverUser( pOwner->GetCharName() ) || m_pCreator->IsStealthMode() ) )
		bObserver = true;

	ioHashStringVec kCharReocrdNameList;
	FindBaseCharRecordNameList( kCharReocrdNameList );

	int i = 0;
	int iCharCnt = kCharReocrdNameList.size();
	int iInfoCnt = 0;
	ContributeInfoList vInfo;

	for(i = 0;i < iCharCnt;i++)
	{
		MonsterSurvivalRecord *pRecord = FindRecordCharName( kCharReocrdNameList[i] );
		if( !pRecord ) continue;
		if( !pRecord->pChar ) continue;
		if( pRecord->pChar->GetState() == CS_OBSERVER ) continue;
		if( g_BattleRoomMgr.IsObserverUser(pRecord->pChar->GetCharName()) ) continue;

		ContributeInfo Contribute;
		Contribute.m_iRecordArray     = i;
		Contribute.m_iContributePoint = pRecord->iUniqueTotalKill;
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
			if( kContribute.m_szName == rkName )
				break;
		}
	}

	if(iCharCnt < 4)
		rRankValue1++;

	vInfo.clear();	

	for(i = 0;i < iCharCnt;i++)
	{
		MonsterSurvivalRecord *pRecord = FindRecordCharName( kCharReocrdNameList[i] );
		if( !pRecord ) continue;
		if( !pRecord->pChar ) continue;
		if( pRecord->pChar->GetState() == CS_OBSERVER ) continue;
		if( g_BattleRoomMgr.IsObserverUser(pRecord->pChar->GetCharName()) ) continue;

		ContributeInfo Contribute;
		Contribute.m_iRecordArray = i;
		Contribute.m_iContributePoint = pRecord->Contribute.m_iWounded;
		Contribute.m_iGradeLevel      = pRecord->pChar->GetLevel();
		Contribute.m_szName           = pRecord->szName;
		vInfo.push_back( Contribute );
	}
	std::sort( vInfo.begin(), vInfo.end(), R_ContributeRankSort() );
	iInfoCnt = vInfo.size();

	if( !bObserver )
	{
		for(rRankValue2 = 0;rRankValue2 < iInfoCnt;rRankValue2++)
		{
			ContributeInfo &kContribute = vInfo[rRankValue2];
			if( kContribute.m_szName == rkName )
				break;
		}
	}

	if(iCharCnt < 4)
		rRankValue2++;

	vInfo.clear();	
	kCharReocrdNameList.clear();

	DWORD dwPlayingTime = 0;

	if( m_ModeState == MS_RESULT)
		dwPlayingTime = m_dwResultStartTime - m_dwPlayStartTime;
	else
		dwPlayingTime = FRAMEGETTIME() - m_dwPlayStartTime;

	if(dwPlayingTime < m_dwRankTime[RANK_S])
		rRankValue3 = 0;
	else if(dwPlayingTime < m_dwRankTime[RANK_A])
		rRankValue3 = 1;
	else if(dwPlayingTime < m_dwRankTime[RANK_B])
		rRankValue3 = 2;
	else
		rRankValue3 = 3;

	return iInfoCnt;
}

void ioTowerDefMode::OnFinalResultUserInfo( SP2Packet &rkPacket )
{
	TCPNetwork::MouseBusy( false );

	// MVP 보상 개수
	int iMVPTreasureCard;
	rkPacket >> iMVPTreasureCard;	
	Help::SetRaidModeMVPTreasureCard( iMVPTreasureCard );

	// 유저당 획득
	int iUserSize;
	rkPacket >> iUserSize;
	for(int i = 0;i < iUserSize;i++)
	{
		ioHashString szName;
		int iContribute, iRank, iKillDeathLevel, iUniqueTotalKill, iUniqueTotalDeath, iVictories;
		bool bSafetyLevel;
		rkPacket >> szName >> iRank >> iContribute >> iUniqueTotalKill >> iUniqueTotalDeath >> iVictories >> iKillDeathLevel >> bSafetyLevel;

		// 가장 긴 시간 플레이한 용병 정보
		int iClassType;
		DWORD dwCharIndex;
		rkPacket >> iClassType >> dwCharIndex;

		int iTreasureClearCount;
		rkPacket >> iTreasureClearCount;

		ModeRecord *pRecord = FindModeRecordCharName( szName );
		if( pRecord )
		{
			ContributeResultSync( szName, iContribute );
			pRecord->iUniqueTotalKill  = iUniqueTotalKill;
			pRecord->iUniqueTotalDeath = iUniqueTotalDeath;
			pRecord->iCurRank          = iRank;
			pRecord->iPvEClassType     = iClassType;
			pRecord->dwPvECharIndex    = dwCharIndex;
			pRecord->iResultTreasureCardCount = iTreasureClearCount;
			g_VictoriesMgr.SetVictories( szName, iVictories );
			ioGrowthLevel *pLevel = m_pCreator->GetGrowthLevel( szName );
			pLevel->ResetInstantExtraGrowthLevel();
		}		

		if( g_BattleRoomMgr.IsBattleRoom() )
		{
			g_BattleRoomMgr.UpdateUserAbilityLevel( szName, iKillDeathLevel );
			g_BattleRoomMgr.UpdateUserSafetyLevel( szName, bSafetyLevel );
		}
	}
}


void ioTowerDefMode::UpdateRoundStateWnd()
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
//			pRoundStateWnd->SetDungeonsScore( GetDungeonFloor() );
			pRoundStateWnd->SetTowerDefScore();
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

void ioTowerDefMode::SetTotalScoreWnd( IntVec &rvClassTypeList,
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
	pTotalResult->SetTowerDefScore();

	ModeRecord *pRecord = FindModeRecordCharName( g_MyInfo.GetPublicID() );
	if( pRecord )
		pTotalResult->SetStartRaidTreasure( pRecord->iResultTreasureCardCount );

	m_dwCurResultShowDuration = m_dwFinalRoundResultDuration;

	SetTotalResultBriefingWnd();
}

void ioTowerDefMode::RenderBlind()
{
	if( m_dwSetBlindModeTime == 0 && m_dwEndBlindModeStartTime == 0 )
		return;
	if( !m_bBlindRender )
		return;

	float fCurUpHeight = m_fUpBlindHeight * m_fCurBlindHeightRate;
	float fCurBottomHeight = m_fBottomBlindHeight * m_fCurBlindHeightRate;

	float fXPos, fYPos;
	fXPos = (float)Setting::Width() * FLOAT05;

	fYPos = fCurUpHeight * FLOAT05;
	if( m_pBlindUp )
	{
		m_pBlindUp->SetScale( (float)Setting::Width(), fCurUpHeight );
		m_pBlindUp->Render( fXPos, fYPos );
	}

	fYPos = Setting::Height() - fCurBottomHeight * FLOAT05;
	if( m_pBlindDown )
	{
		m_pBlindDown->SetScale( (float)Setting::Width(), fCurBottomHeight );
		m_pBlindDown->Render( fXPos, fYPos );
	}

	// 블라인드 애니메이션이 끝난 다음부터 표시
	if( m_fCurBlindHeightRate == FLOAT1 )
	{
		ModeRecord *pDieRecord = FindModeRecordCharName( g_MyInfo.GetPublicID() );
		if( !pDieRecord ) return;
		if( !pDieRecord->pChar ) return;
		if( !m_pRevivalGaugeBack || !m_pRevivalGauge || !m_pTextRevival || !m_pTextRoundDelay || 
			!m_pKeyBack || !m_pMsgBack || !m_pReviveMsg ) return;

		int iGaugeWidth = Setting::Width() - 56;
		int iGaugeHeight= 27;
		if( pDieRecord->pChar->GetState() == CS_DIE )
		{
			// 부활 게이지
			float fGaugeRate = 0.0f;
			if( pDieRecord->dwRevivalGap > 0 && pDieRecord->dwCurDieTime > 0 )
				fGaugeRate = min( FLOAT1, (float)(FRAMEGETTIME() - pDieRecord->dwCurDieTime) / pDieRecord->dwRevivalGap );

			if( fGaugeRate <= FLOAT1 )
			{
				float fScreenWidth = Setting::Width();
				float fScreenHeight = Setting::Height();

				m_pRevivalGaugeBack->SetSize( iGaugeWidth, iGaugeHeight );			
				m_pRevivalGaugeBack->Render( 28, fScreenHeight - 43 );
				if( fGaugeRate > 0.0f )
				{
					m_pRevivalGauge->SetSize( max( 11, (float)iGaugeWidth * fGaugeRate ), iGaugeHeight );			
					m_pRevivalGauge->Render( 28, fScreenHeight - 43 );
				}

				m_pTextRevival->Render( fScreenWidth * FLOAT05, fScreenHeight - 37, UI_RENDER_ADD );

				if(!m_szReviveKey.IsEmpty())
				{
					float fWidth = g_FontMgr.GetTextWidth( m_szReviveKey.c_str(), TS_NORMAL, FONT_SIZE_15 );
					float fFullWidth = fWidth + 20 + m_pReviveMsg->GetWidth() + 32;

					float fPosX = (fScreenWidth - fFullWidth) * FLOAT05;

					m_pKeyBack->SetSize(fWidth + 20, 18);
					m_pKeyBack->Render( fPosX, fScreenHeight * FLOAT05 - 9 );

					g_FontMgr.SetAlignType( TAT_CENTER );
					g_FontMgr.SetTextStyle( TS_NORMAL );
					g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
					g_FontMgr.PrintText( fPosX + 15, fScreenHeight * FLOAT05 - 8, FONT_SIZE_15, m_szReviveKey.c_str() );

					m_pMsgBack->SetSize(fFullWidth, 50);
					m_pMsgBack->Render(fPosX - 1, fScreenHeight * FLOAT05 -25, UI_RENDER_SCREEN);

					m_pReviveMsg->Render(fPosX + fWidth + 22, fScreenHeight * FLOAT05 - 15, UI_RENDER_NORMAL);
				}
			}
		}
	}
}

void ioTowerDefMode::RenderAfterGUI()
{
#ifndef SHIPPING
	RenderPathBox();
#endif
}

#ifndef SHIPPING
void ioTowerDefMode::RenderPathBox()
{
	if(m_ModeState == MS_PLAY)
	{
		int nBox = GetNaviInfo()->GetBoxSize();

		for(int i = 0; i < nBox; i++)
		{
			ioOrientBox cBox;
			D3DXVECTOR3 vVtxArray[8];
			D3DXVECTOR3 vMin, vMax, vCenter;
			vMin.x = GetNaviInfo()->getBoxData(i)->posMin.x;
			vMin.y = GetNaviInfo()->getBoxData(i)->posMin.z;
			vMin.z = GetNaviInfo()->getBoxData(i)->posMin.y;

			vMax.x = GetNaviInfo()->getBoxData(i)->posMax.x;
			vMax.y = GetNaviInfo()->getBoxData(i)->posMax.z;
			vMax.z = GetNaviInfo()->getBoxData(i)->posMax.y;

			vCenter.x = GetNaviInfo()->getBoxData(i)->posCenter.x;
			vCenter.y = GetNaviInfo()->getBoxData(i)->posCenter.z + FLOAT40;
			vCenter.z = GetNaviInfo()->getBoxData(i)->posCenter.y;

			cBox.SetBoxByMinMax(vMin, vMax);
			cBox.GetVertices( vVtxArray );

			m_pCreator->RenderBox(vVtxArray, 0xff0000ff);

			int iXPos = 0;
			int iYPos = 0;

			if( Setting::Check2DInScreen( vCenter, iXPos, iYPos ) )
			{
				g_FontMgr.SetAlignType( TAT_RIGHT );
				g_FontMgr.SetTextStyle( TS_NORMAL );
				g_FontMgr.SetBkColor( 0, 0, 0 );
				g_FontMgr.SetTextColor( TCT_DEFAULT_ORANGE );
				g_FontMgr.PrintText( iXPos, iYPos, FONT_SIZE_18, GetNaviInfo()->getBoxData(i)->name.c_str());
			}
		}
	}
}

void ioTowerDefMode::IncreaseLevel_150()
{
	RecordList::iterator it = m_vCharRecordList.begin();

	for(; it != m_vCharRecordList.end(); ++it)
	{
		ioBaseChar* pChar = it->pChar;
		if( !ToNpcChar( pChar ) && !g_BattleRoomMgr.IsObserverUser(pChar->GetCharName()) )
		{
			if( it->Contribute.m_nInstantLevel > 100)
			{
				pChar->FillMaxHP();
				return;
			}

			ioGrowthLevel *pLevel = m_pCreator->GetGrowthLevel( pChar->GetCharName() );

			for(int i = 0; i < 150; i++)
			{
				it->Contribute.m_nInstantLevel++;
				if( pLevel )
					pLevel->IncreaseExtraGrowthLevel();
			}
// 			pChar->SetLevel(it->Contribute.m_nInstantLevel);
// 			pChar->SetClassLevel(it->Contribute.m_nInstantLevel);

			m_pCreator->UpdateGrowthDataOne( pChar->GetCharName() );
		}
	}
}

#endif