

#include "stdafx.h"

#include "ioFightClubMode.h"
#include "ioLadderTeamMgr.h"
#include "ioNpcChar.h"

#include "GUI/SoldierSelectWnd.h"
#include "GUI/RoundStateMainWnd.h"
#include "GUI/FightClubWnd.h"
#include "GUI/TutorialWnd.h"
#include "GUI/AbuseProblemWnd.h"

ioFightClubMode::ioFightClubMode( ioPlayStage *pCreator )
: ioPlayMode( pCreator )
{
	m_ModeType = MT_FIGHT_CLUB;

	m_SingleTeamPosArray.clear();
	m_dwRoundTimeWarning = 0;

	m_iRoundReadyCharCount = 0;
	
	m_bSetReadyGoEventEndTime = false;
	m_dwReadyGoEventEndTime = 0;

	m_ChampionMapEffectID  = -1;
	m_ChallengerMapEffectID= -1;

	m_iAbuseLoseCount = 3;
	m_iKeyboardShowGradeLimit = 1;

	m_bUseFightNPC = false;
	m_iCurFightNPCStage = 0;

	m_vCharRecordList.reserve( 100 );
}

ioFightClubMode::~ioFightClubMode()
{
	m_pCreator->SetScreenBlindDownState();

	ClearMode();
}

void ioFightClubMode::LoadModeResource( ioINILoader &rkLoader )
{
	g_ModeUIController.SetKOMsgWnd( NULL );
	g_ModeUIController.SetCenterKillInfoWnd( NULL );
	g_GUIMgr.AddWnd( "XML/SkillListWnd.xml" );
	g_GUIMgr.AddWnd( "XML/AccessorySkillListWnd.xml" );

	FightClubTimeGaugeWnd *pFightClubTimeGauge = dynamic_cast< FightClubTimeGaugeWnd * >( g_GUIMgr.AddWnd( "XML/fightclubtimegaugewnd.xml" ) );
	if( pFightClubTimeGauge )
	{
		pFightClubTimeGauge->SetPlayStage( m_pCreator );
	}

	FightClubReadyWnd *pFightClubReadyWnd = dynamic_cast< FightClubReadyWnd * >( g_GUIMgr.AddWnd( "XML/fightclubreadywnd.xml" ) );
	if( pFightClubReadyWnd )
	{
		pFightClubReadyWnd->SetPlayStage( m_pCreator );
	}

	g_GUIMgr.AddWnd( "XML/fightclubroundendwnd.xml" );
	g_GUIMgr.AddWnd( "XML/fightclubchallengerwnd.xml" );
	g_GUIMgr.AddWnd( "XML/tutorialkeyboardwnd.xml" );
	g_GUIMgr.AddWnd( "XML/fightclubfightnpccontinuewnd.xml" );

	ioPlayMode::LoadModeResource( rkLoader );
}

void ioFightClubMode::LoadMap()
{
	ioPlayMode::LoadMap();

	ioINILoader kLoader( GetModeMapINIFileName( m_ModeType, m_iModeSubNum, m_iModeMapIndex ) );
	kLoader.SetTitle_e( "team_revival" );

	if( P2PNetwork::IsNetworkPlaying() )
	{
		int iMaxRevivalPos = kLoader.LoadFloat_e( "max_revival_pos", 0 );
		m_vRevivalPos.clear();
		m_vRevivalPos.reserve( iMaxRevivalPos );
		for( int i=0 ; i<iMaxRevivalPos; i++ )
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
}

void ioFightClubMode::InitMode( int iMapIndex )
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

	ioINILoader kLoader( GetModeINIFileName( m_ModeType ) );

	kLoader.SetTitle_e( "common_info" );
	m_dwRoundDuration = kLoader.LoadInt_e( "round_time", 300000 );
	m_dwCurRoundDuration = m_dwRoundDuration;

	m_fScoreWarningRate = kLoader.LoadFloat_e( "score_warning_rate", FLOAT05 );
	m_dwRoundTimeWarning= kLoader.LoadInt_e( "round_time_warning", 0 );

	m_iAbuseLoseCount   = kLoader.LoadInt_e( "abuse_lose_count", 3 );
	m_iKeyboardShowGradeLimit = kLoader.LoadInt_e( "keyboard_show_grade", 1 );

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

	kLoader.LoadString_e( "result_end_effect", "result_effect_blue.txt", szBuf, MAX_PATH );
	m_szFightAIClearEffect = szBuf;
	
	kLoader.LoadString_e( "result_end_sound", "Resource/wave/bgm/bgm_crown_02.ogg", szBuf, MAX_PATH );
	m_szFightAIClearSound = szBuf;

	LoadGUIValues();
	LoadPushCursor();
	LoadContribute();

	kLoader.LoadString_e( "ui_info", "1point_up_emoticon", "", szBuf, MAX_PATH );
	m_PointUpEmoticon = szBuf;

	if( !P2PNetwork::IsNetworkPlaying() )
	{
		SetModeState( MS_READY );
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

	CreateStartPosEffect();

	g_GUIMgr.HideWnd( MINIMAP_WND );

	m_pCreator->SetCameraModeFixedBlock( true );
}

void ioFightClubMode::CreateStartPosEffect()
{
	if( m_ChampionMapEffectID != -1 )
	{
		DestroyMapEffect( m_ChampionMapEffectID );
		m_ChampionMapEffectID = -1;
	}

	if( m_ChallengerMapEffectID != -1 )
	{
		DestroyMapEffect( m_ChallengerMapEffectID );
		m_ChallengerMapEffectID = -1;
	}

	D3DXVECTOR3 vScale( FLOAT1, FLOAT1, FLOAT1 );
	ioMapEffect *pMapEffect = CreateMapEffect( "fightclub_start_pos.txt", GetStartPos( 0, false ), vScale );
	if( pMapEffect )
		m_ChampionMapEffectID = pMapEffect->GetUniqueID();

	pMapEffect = CreateMapEffect( "fightclub_start_pos.txt", GetStartPos( 1, false ), vScale );
	if( pMapEffect )
		m_ChallengerMapEffectID = pMapEffect->GetUniqueID();
}

void ioFightClubMode::ReStartMode()
{
	FightClubTimeGaugeWnd *pFightClubTimeGauge = dynamic_cast< FightClubTimeGaugeWnd * >( g_GUIMgr.FindWnd( FIGHTCLUB_TIME_GAUGE_WND ) );
	m_pCreator->SetScreenBlindDownState();

	D3DXVECTOR3 vStartPos;
	int iCharCnt = m_vCharRecordList.size();
	for( int i=0 ; i<iCharCnt ; i++ )
	{
		FightClubRecord &rkRecord = m_vCharRecordList[i];
		if( !rkRecord.pChar ) continue;

		vStartPos = GetStartPos( rkRecord.pChar->GetTeam() );
		rkRecord.pChar->SetWorldPosition( vStartPos );

		rkRecord.dwRevivalGap = 0;
		rkRecord.dwCurDieTime = 0;
		rkRecord.iRevivalCnt = 0;
		rkRecord.InitValues();

		rkRecord.pChar->SetProtectionMode( PM_NONE, 0 );
		rkRecord.pChar->SetRoundEndMotion( RM_NONE );
		rkRecord.pChar->Revival();

		if( g_BattleRoomMgr.IsObserverUser( rkRecord.pChar->GetCharName() ) )
			rkRecord.pChar->SetState( CS_OBSERVER );
		else if( pFightClubTimeGauge )
		{
			if( pFightClubTimeGauge->GetChampionUserName() != rkRecord.szName && 
				pFightClubTimeGauge->GetChallengerUserName() != rkRecord.szName )
			{
				// 비전투 유저들 관전 상태
				rkRecord.pChar->SetState( CS_VIEW );
			}
			else
			{
				// 전투 유저들 용병 선택 상태
				rkRecord.pChar->SetReadyState();
			}
		}		
	}

	m_bSetReadyGoEventEndTime = false;
	m_dwReadyGoEventEndTime = 0;

	m_dwCurRevivalTime = 0;
	m_dwCheckWarningTime = 0;
	m_bScoreWarning = false;
	m_bFirstSelect = true;

	m_NewChallengerName.Clear();

	// 라운드제
	m_bZeroHP = false;

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

	SetMapDummy();

	m_iRoundReadyCharCount = 0;

	CreateStartPosEffect();

	m_pCreator->SetCameraModeFixedBlock( true );
	g_GUIMgr.HideWnd( FIGHTCLUB_ROUND_END_WND );
	g_GUIMgr.HideWnd( FIGHTCLUB_CHALLENGER_WND );
	g_GUIMgr.HideWnd( FIGHTCLUB_CONTINUE_WND );
	g_GUIMgr.ShowWnd( FIGHTCLUB_TIME_GAUGE_WND );
}

bool ioFightClubMode::ShowSoldierSettingUI( ioBaseChar *pOwnerChar, int iArray )
{
	if( !pOwnerChar ) return false;
	if( GetModeState() != MS_PLAY ) return false;
	if( pOwnerChar->GetState() != CS_DIE && pOwnerChar->GetState() != CS_VIEW ) return false;

	SoldierSelectWnd *pSoldier = dynamic_cast<SoldierSelectWnd*>(g_GUIMgr.FindWnd( SOLDIER_SELECT_WND ));
	if( !pSoldier ) return false;

	if( pSoldier->IsShow() )
	{
		// 다음 라운드 출전 용병 예약 및 용병 선택
		int iSlotToArray = g_MyInfo.GetCharSlotIndexToArray( iArray );
		pSoldier->SetFunctionKeySelect( iSlotToArray, IsNetworkMode() );
	}
	else
	{
		// 용병 설정 UI 활성화
		pSoldier->SetSettingSoldierShow( iArray, IsNetworkMode() );
	}
	return true;
}

void ioFightClubMode::ProcessMode()
{
	ProcessSelectCharacter();
	ProcessGUI();
	ProcessDestroyMapEffect();

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
		break;
	case MS_RESULT:
		ProcessResultState();
		break;
	case MS_NEW_CHALLENGER:
		break;
	case MS_NPC_CONTINUE:
		break;
	}
}

void ioFightClubMode::ProcessReadyState()
{
	if( P2PNetwork::IsNetworkPlaying() )	return;
	if( m_dwReadyStartTime + 4000 > FRAMEGETTIME() ) return;

	SetModeState( MS_PLAY );
}

void ioFightClubMode::ProcessPlayState()
{
	UpdateSoldierRevivalPenalty();

	m_pCreator->NotifySpringDampUseSet( true );
}

void ioFightClubMode::ProcessResultState()
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

	if( !P2PNetwork::IsNetworkPlaying() )
	{
		DWORD dwTimeGap = FRAMEGETTIME() - m_dwResultStartTime;
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

void ioFightClubMode::ProcessGUI()
{
	UpdateMiniMapSpotPoint();
	UpdateRoundStateWnd();
	UpdateTimeScoreWnd();
	UpdateObserverWnd();
	UpdateEndScoreWndTime();
	UpdateSoldierWndTime();

	ProcessModeGuide();
	ProcessPushEnableCursor();
	ProcessBlindMode();
	ProcessAutoStartAlarm();
	ProcessReadyGoEvent();
}

void ioFightClubMode::ProcessReadyGoEvent()
{
	DWORD dwCurTime = FRAMEGETTIME();

	bool bCheck = false;
	ioBaseChar *pChampion = GetRecordChar( GetChampionName() );
	ioBaseChar *pChallenger = GetRecordChar( GetChallengerName() );

	if( m_bUseFightNPC && pChampion )
	{
		if( pChampion->GetState() != CS_READY && pChampion->GetState() != CS_LOADING )
			bCheck = true;
	}
	else if( pChampion && pChallenger )
	{
		bool bChampCheck = false;
		bool bChallCheck = false;

		if( pChampion->GetState() != CS_READY && pChampion->GetState() != CS_LOADING )
			bChampCheck = true;

		if( pChallenger->GetState() != CS_READY && pChallenger->GetState() != CS_LOADING )
			bChallCheck = true;

		if( bChampCheck && bChallCheck )
			bCheck = true;
	}

	if( bCheck && !m_bSetReadyGoEventEndTime && m_dwReadyGoEventEndTime == 0 )
	{
		m_bSetReadyGoEventEndTime = true;
		m_dwReadyGoEventEndTime = dwCurTime + 2800;
	}

	if( m_dwReadyGoEventEndTime > 0 && m_dwReadyGoEventEndTime < dwCurTime )
	{
		m_dwReadyGoEventEndTime = 0;
		RemoveStartBuffList();
	}

	if( m_bUseFightNPC )
	{
		if( m_iRoundReadyCharCount < SHOW_READY_CHAMP_COUNT )
			return;
	}
	else
	{
		if( m_iRoundReadyCharCount < SHOW_READY_CHAR_COUNT )
			return;
	}

	if( bCheck )
	{
		m_iRoundReadyCharCount = 0;

		FightClubReadyWnd *pFightClubReadyWnd = dynamic_cast< FightClubReadyWnd * >( g_GUIMgr.FindWnd( FIGHTCLUB_READY_WND ) );
		if( pFightClubReadyWnd )
		{
			pFightClubReadyWnd->ShowReadyWnd( m_bUseFightNPC, (int)m_FightNPCRecord.size(), m_iCurFightNPCStage );
		}
	}
}

void ioFightClubMode::ProcessDestroyMapEffect()
{
	if( m_ChampionMapEffectID != -1 )
	{
		ioBaseChar *pChampion = GetRecordChar( GetChampionName() );
		if( pChampion )
		{
			if( pChampion->GetState() != CS_READY && pChampion->GetState() != CS_LOADING )
			{
				DestroyMapEffect( m_ChampionMapEffectID );
				m_ChampionMapEffectID = -1;
			}
		}
	}

	if( m_ChallengerMapEffectID != -1 )
	{
		ioBaseChar *pChallenger = GetRecordChar( GetChallengerName() );
		if( pChallenger )
		{
			if( pChallenger->GetState() != CS_READY && pChallenger->GetState() != CS_LOADING )
			{
				DestroyMapEffect( m_ChallengerMapEffectID );
				m_ChallengerMapEffectID = -1;
			}
		}
	}
}

void ioFightClubMode::UpdateMiniMapSpotPoint()
{
}

void ioFightClubMode::UpdateRoundStateWnd()
{
	if( g_MyInfo.IsSortContribute() )
	{
		DWORD dwTimeGap = FRAMEGETTIME() - m_dwPlayStartTime;
		if( m_dwCurRoundDuration > dwTimeGap )
			ContributeRoundAllPercent( m_dwRoundDuration - (m_dwCurRoundDuration - dwTimeGap), m_dwRoundDuration );
		else
			ContributeRoundAllPercent( m_dwRoundDuration, m_dwRoundDuration );

		FightClubRecord *pRecord = FindRecordCharName( g_MyInfo.GetPublicID() );
		if( pRecord )
			g_MyInfo.SetMyContribute( pRecord->Contribute.m_iPrevContributePer );
	}

	if( m_ModeState != MS_PLAY )
	{
		if( g_GUIMgr.IsShow( ROUND_STATE_MAIN_WND ) )
		{
			g_GUIMgr.HideWnd( ROUND_STATE_MAIN_WND );
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

			pRoundStateWnd->SetSingleScore();
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

void ioFightClubMode::UpdateDropDieRecord( ioBaseChar *pDieChar, ioBaseChar *pKillChar, ioBaseChar *pBestAttacker )
{
	if( pKillChar )
	{
		ModeRecord *pAttRecord = FindModeRecordCharName( pKillChar->GetCharName() );
		ModeRecord *pDieRecord = FindModeRecordCharName( pDieChar->GetCharName() );

		if( pAttRecord && pDieRecord )
		{
			float fKillPoint = FLOAT05 + ( (float)GetKillPoint( pAttRecord->pChar->GetTeam() ) * FLOAT05 );

			TeamType eAttTeam, eDieTeam;

			if( pAttRecord->pChar->IsOwnerChar() )
				eAttTeam = TEAM_BLUE;
			else 
				eAttTeam = TEAM_RED;

			if( pDieRecord->pChar->IsOwnerChar() )
				eDieTeam = TEAM_BLUE;
			else
				eDieTeam = TEAM_RED;

			if( pAttRecord->pChar->GetTeam() != pDieRecord->pChar->GetTeam() )
			{
				pAttRecord->AddKill( GetModeType(), fKillPoint, true, false );
				pDieRecord->AddDeath( GetModeType(), GetDeathPoint( pDieRecord->pChar->GetTeam() ) );							

				g_ModeUIController.SetKOMsg( eAttTeam, pAttRecord->pChar->GetCharName(),
					                         pDieRecord->pChar->GetLastAttackerSkillName(), pDieRecord->pChar->GetLastAttackerItemCode(),
											 eDieTeam, pDieRecord->pChar->GetCharName()  );
			}
			else
			{
				if( pAttRecord->pChar == pDieRecord->pChar )	// 자기자신의 공격
				{
					pDieRecord->AddDeath( GetModeType(), GetDeathPoint( pDieRecord->pChar->GetTeam() ) );
					g_ModeUIController.SetKOMsg( TEAM_NONE, "", 
						                         "", 0, pDieRecord->pChar->GetTeam(), pDieRecord->pChar->GetCharName() );
				}
				else
				{
					pAttRecord->AddKill( GetModeType(), -fKillPoint, true, false );
					pDieRecord->AddDeath( GetModeType(), GetDeathPoint( pDieRecord->pChar->GetTeam() ) );

					g_ModeUIController.SetKOMsg( eAttTeam, pAttRecord->pChar->GetCharName(),
						                         pDieRecord->pChar->GetLastAttackerSkillName(), pDieRecord->pChar->GetLastAttackerItemCode(),
												 eDieTeam, pDieRecord->pChar->GetCharName() );
				}
			}

			pAttRecord->pChar->SetKillEmoticon( pAttRecord->GetUniqueTotalKill() );
			pDieRecord->pChar->SetDeathEmoticon();
		}
	}
	else
	{
		ModeRecord *pDieRecord = FindModeRecordCharName( pDieChar->GetCharName() );
		if( pDieRecord )
		{
			TeamType eDieTeam;

			if( pDieRecord->pChar->IsOwnerChar() )
				eDieTeam = TEAM_BLUE;
			else
				eDieTeam = TEAM_RED;

			pDieRecord->AddDeath( GetModeType(), GetDeathPoint( pDieRecord->pChar->GetTeam() ) );

			g_ModeUIController.SetKOMsg( TEAM_NONE, "", 
				                         "", 0, eDieTeam, pDieRecord->pChar->GetCharName() );

			pDieRecord->pChar->SetDeathEmoticon();
		}
	}

	// Best Attacker 처리
	ioBaseChar *pBestAttChar = ToBaseChar( pBestAttacker );
	if( pBestAttChar && pDieChar )
	{
		ModeRecord *pAttRecord = FindModeRecordCharName( pBestAttChar->GetCharName() );
		if( pAttRecord && pDieChar->GetTeam() != pBestAttChar->GetTeam() )
		{
			float fKillPoint = FLOAT05 + ( (float)GetKillPoint( pBestAttChar->GetTeam() ) * FLOAT05 );
			pAttRecord->AddKill( GetModeType(), fKillPoint, true, true );
		}
	}
}

void ioFightClubMode::UpdateKillDeathRecord( ioPlayEntity *pDieEntity, ioPlayEntity *pKiller, ioPlayEntity *pBestAttacker )
{
	ioBaseChar *pKillerChar = ToBaseChar( pKiller );
	ioBaseChar *pDieChar = ToBaseChar( pDieEntity );
	if( pKillerChar && pDieChar )
	{
		ModeRecord *pAttRecord = FindModeRecordCharName( pKillerChar->GetCharName() );
		ModeRecord *pDieRecord = FindModeRecordCharName( pDieChar->GetCharName() );

		if( pAttRecord && pDieRecord )
		{
			float fKillPoint = FLOAT05 + ( (float)GetKillPoint( pAttRecord->pChar->GetTeam() ) * FLOAT05 );

			TeamType eAttTeam, eDieTeam;

			if( pAttRecord->pChar->IsOwnerChar() )
				eAttTeam = TEAM_BLUE;
			else 
				eAttTeam = TEAM_RED;

			if( pDieRecord->pChar->IsOwnerChar() )
				eDieTeam = TEAM_BLUE;
			else
				eDieTeam = TEAM_RED;

			if( pAttRecord->pChar->GetTeam() != pDieRecord->pChar->GetTeam() )
			{
				pAttRecord->AddKill( GetModeType(), fKillPoint, false, false );
				pDieRecord->AddDeath( GetModeType(), GetDeathPoint( pDieRecord->pChar->GetTeam() ) );							

				g_ModeUIController.SetKOMsg( eAttTeam, pAttRecord->pChar->GetCharName(),
					                         pDieRecord->pChar->GetLastAttackerSkillName(), pDieRecord->pChar->GetLastAttackerItemCode(),
											 eDieTeam, pDieRecord->pChar->GetCharName()  );
			}
			else
			{
				if( pAttRecord->pChar == pDieRecord->pChar )	// 자기자신의 공격
				{
					pDieRecord->AddDeath( GetModeType(), GetDeathPoint( pDieRecord->pChar->GetTeam() ) );
					g_ModeUIController.SetKOMsg( TEAM_NONE, "",
												 "", 0, eDieTeam, pDieRecord->pChar->GetCharName()  );
				}
				else
				{
					pAttRecord->AddKill( GetModeType(), -fKillPoint, false, false );
					pDieRecord->AddDeath( GetModeType(), GetDeathPoint( pDieRecord->pChar->GetTeam() ) );

					g_ModeUIController.SetKOMsg( eAttTeam, pAttRecord->pChar->GetCharName(),
						                         pDieRecord->pChar->GetLastAttackerSkillName(), pDieRecord->pChar->GetLastAttackerItemCode(),
												 eDieTeam, pDieRecord->pChar->GetCharName() );
				}
			}

			pAttRecord->pChar->SetKillEmoticon( pAttRecord->GetUniqueTotalKill() );
			pDieRecord->pChar->SetDeathEmoticon();
		}
	}
	else
	{
		if( pDieChar )
		{
			ModeRecord *pDieRecord = FindModeRecordCharName( pDieChar->GetCharName() );
			if( pDieRecord )
			{
				TeamType eDieTeam;

				if( pDieRecord->pChar->IsOwnerChar() )
					eDieTeam = TEAM_BLUE;
				else
					eDieTeam = TEAM_RED;

				pDieRecord->AddDeath( GetModeType(), GetDeathPoint( pDieRecord->pChar->GetTeam() ) );

				g_ModeUIController.SetKOMsg( TEAM_NONE,	"",
					                         "", 0, eDieTeam, pDieRecord->pChar->GetCharName() );

				pDieRecord->pChar->SetDeathEmoticon();
			}
		}
	}

	// Best Attacker 처리
	ioBaseChar *pBestAttChar = ToBaseChar( pBestAttacker );
	if( pBestAttChar && pDieChar )
	{
		ModeRecord *pAttRecord = FindModeRecordCharName( pBestAttChar->GetCharName() );
		if( pAttRecord && pDieChar->GetTeam() != pBestAttChar->GetTeam() )
		{
			float fKillPoint = FLOAT05 + ( (float)GetKillPoint( pBestAttChar->GetTeam() ) * FLOAT05 );
			pAttRecord->AddKill( GetModeType(), fKillPoint, false, true );
		}
	}
}

void ioFightClubMode::UpdateTimeScoreWnd()
{
	FightClubTimeGaugeWnd *pTimeGauge = dynamic_cast< FightClubTimeGaugeWnd * >( g_GUIMgr.FindWnd( FIGHTCLUB_TIME_GAUGE_WND ) );
	if( pTimeGauge == NULL ) return;

	DWORD dwTimeGap = 0;
	switch( m_ModeState )
	{
	case MS_READY:
		pTimeGauge->UpdateTime( m_dwRoundDuration, m_dwRoundDuration );
		break;
	case MS_PLAY:
		dwTimeGap = FRAMEGETTIME() - m_dwPlayStartTime;
		if( m_dwCurRoundDuration > dwTimeGap )
		{
			pTimeGauge->UpdateTime( m_dwCurRoundDuration - dwTimeGap, m_dwRoundDuration );
		}
		else
		{
			pTimeGauge->UpdateTime( 0, m_dwRoundDuration );
		}
		break;
	case MS_RESULT:
		pTimeGauge->UpdateTime( 0, m_dwRoundDuration );
		break;
	}
}

bool ioFightClubMode::GetRoundInfo( int iIndex, RoundInfo &rkInfo )
{
	if( ioPlayMode::GetRoundInfo( iIndex, rkInfo ) )
	{
		FightClubRecord *pRecord = (FightClubRecord*)FindModeRecord( iIndex );
		if( !pRecord )	return false;
		if( !pRecord->pChar ) return false;

		rkInfo.m_iContribute = pRecord->Contribute.m_iContribute;
		rkInfo.m_iContributePer = pRecord->Contribute.m_iPrevContributePer;
		
		rkInfo.m_iFightWin      = pRecord->iFightWin;
		rkInfo.m_iFightLose     = pRecord->iFightLose;
		rkInfo.m_iFightVictory  = pRecord->iFightVictories;
		return true;
	}
	return false;
}

bool ioFightClubMode::GetResultInfo( int iIndex, ResultInfo &rkInfo )
{
	if( ioPlayMode::GetResultInfo( iIndex, rkInfo ) )
	{
		FightClubRecord *pRecord = (FightClubRecord*)FindModeRecord( iIndex );
		if( !pRecord )	return false;
		if( !pRecord->pChar ) return false;

		rkInfo.m_iContribute = pRecord->Contribute.m_iContribute;
		rkInfo.m_iContributePer = pRecord->Contribute.m_iPrevContributePer;
		return true;
	}
	return false;
}

bool ioFightClubMode::GetFinalInfo( int iIndex, FinalInfo &rkInfo )
{
	if( ioPlayMode::GetFinalInfo( iIndex, rkInfo ) )
	{
		FightClubRecord *pRecord = (FightClubRecord*)FindModeRecord( iIndex );
		if( !pRecord )	return false;
		if( !pRecord->pChar ) return false;

		rkInfo.m_iContribute = pRecord->Contribute.m_iContribute;
		rkInfo.m_iContributePer = pRecord->Contribute.m_iPrevContributePer;
		return true;
	}
	return false;
}

void ioFightClubMode::SetRoundEndMotion()
{
	// 사용 안함
}

void ioFightClubMode::SetRoundEndVictories()
{
	// 사용 안함
}

void ioFightClubMode::SetResultScoreWnd()
{	
	// 중간 결과가 존재하지 않는다.
}

void ioFightClubMode::SetTotalScoreWnd( IntVec &rvClassTypeList,
									   IntVec &rvClassPointList,
									   float fBlueGuildTeamBonus,
									   float fRedGuildTeamBonus,
									   float fBonusArray[BA_MAX] )
{
	// 사용하지 않음
}

void ioFightClubMode::SetResultBriefingWnd()
{
	// 없음
}

void ioFightClubMode::SetTotalResultBriefingWnd()
{
	// 사용하지 않음
}

void ioFightClubMode::RenderPreGUI()
{
	if( m_ModeState == MS_PLAY )
	{
		RenderPushEnableCursor();
	}
}

D3DXVECTOR3 ioFightClubMode::GetRevivalPos( ioBaseChar *pChar )
{
	if( m_vRevivalPos.empty() )
		return m_vMapCenter;

	int iPosArray = 0;

	IORandom random;
	random.SetRandomSeed( m_dwRandomSeed );
	iPosArray = random.Random( m_vRevivalPos.size() );

	int iMaxRevivalPos = m_vRevivalPos.size();

	if( !COMPARE( iPosArray, 0, iMaxRevivalPos ) )
		iPosArray = 0;

	D3DXVECTOR3 vCenter = m_vRevivalPos[iPosArray];
	D3DXQUATERNION qtRot;
	D3DXQuaternionRotationYawPitchRoll( &qtRot,
										ioMath::RangeRandom( 0.0f, D3DX_PI * 2.0f ),
										0.0f,
										0.0f );

	D3DXVECTOR3 vMoveDir = qtRot * ioMath::UNIT_Z;
	vCenter += vMoveDir * ioMath::RangeRandom( FLOAT10, m_fRandomRange );
	vCenter.y = GetMapHeightByRevial( vCenter.x, vCenter.z );

	return vCenter;
}

void ioFightClubMode::GetMostNearPosList( PositionDistanceSqList &rkResultList, const D3DXVECTOR3 &vTargPos )
{
	if( m_vRevivalPos.empty() )
		return;

	int iCnt = m_vRevivalPos.size();
	rkResultList.clear();
	rkResultList.reserve( iCnt );

	for( int i=0; i < iCnt; ++i )
	{
		D3DXVECTOR3 vDiff = m_vRevivalPos[i] - vTargPos;
		float fCurDistSq = D3DXVec3LengthSq( &vDiff );

		PositionDistanceSq kPositionDistance;
		kPositionDistance.fDistanceSq = fCurDistSq;
		kPositionDistance.iIndex = i;

		rkResultList.push_back( kPositionDistance );
	}

	std::sort( rkResultList.begin(), rkResultList.end() );
}

void ioFightClubMode::NotifyEntityDie( ioPlayEntity *pEntity, ioPlayEntity *pKiller, ioBaseChar *pBestAttacker,
									 float fLastRate, float fBestRate )
{
	ioBaseChar *pDieChar = ToBaseChar( pEntity );
	ioBaseChar *pKillChar = ToBaseChar( pKiller );

	OnNotifyCharDie( pDieChar, pKillChar, pBestAttacker, fLastRate, fBestRate );
	UpdateKillDeathRecord( pEntity, pKiller, pBestAttacker );
}

void ioFightClubMode::NotifyCharDropDie( ioBaseChar *pDieChar, ioBaseChar *pKillChar, ioBaseChar *pBestAttacker,
									   float fLastRate, float fBestRate )
{
	OnNotifyCharDie( pDieChar, pKillChar, pBestAttacker, fLastRate, fBestRate );
	UpdateDropDieRecord( pDieChar, pKillChar, pBestAttacker );
}

void ioFightClubMode::OnNotifyCharDie( ioBaseChar *pDieChar, ioBaseChar *pKillChar, ioBaseChar *pBestAttacker,
									 float fLastRate, float fBestRate )
{
	if( !pDieChar )
	{
		LOG.PrintTimeAndLog( 0, "ioFightClubMode::OnNotifyCharDie() - DieChar is NULL" );
		return;
	}

	FightClubRecord *pDieRecord = FindRecordCharName( pDieChar->GetCharName() );
	if( !pDieRecord )
	{
		LOG.PrintTimeAndLog( 0, "ioFightClubMode::OnNotifyCharDie - %s Not Exit Char",
								pDieChar->GetCharName().c_str() );
		return;
	}

	if( pKillChar && pKillChar->GetTeam() != pDieChar->GetTeam() )
	{
		if( pBestAttacker && pBestAttacker->GetCharName() == pKillChar->GetCharName() )
		{
			if( m_pCreator && m_pCreator->CheckRoomKOEffect() )
			{
				SetRecoveryGauge( pKillChar, fLastRate );
			}
		}
		else
		{
			if( m_pCreator && m_pCreator->CheckRoomKOEffect() )
			{
				SetRecoveryGauge( pKillChar, fLastRate );
			}

			if( pBestAttacker )
			{
				if( m_pCreator && m_pCreator->CheckRoomKOEffect() )
				{
					SetRecoveryGauge( pBestAttacker, fBestRate );
				}

				if( pBestAttacker->IsOwnerChar() )
				{
					m_pCreator->StartScreenBlind( SBT_KILL, NULL, true );
				}
			}
		}
	}

	if( P2PNetwork::IsNetworkPlaying() )
	{
		if( g_MyInfo.GetPublicID() == pDieChar->GetCharName() )
		{
			g_MyInfo.SetCharDied( pDieChar->GetSelectCharArray(), true );
		}
	}
}

void ioFightClubMode::AddNewRecord( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	FightClubRecord *pFightClubRecord = FindRecordCharName( pChar->GetCharName() );
	if( !pFightClubRecord )
	{
		FightClubRecord kRecord;
		kRecord.pChar = pChar;
		kRecord.szName= pChar->GetCharName();
		m_vCharRecordList.push_back( kRecord );

		UpdateUserRank();

		if( pChar && !g_BattleRoomMgr.IsObserverUser(pChar->GetCharName()) )
			g_AwardMgr.AddNewAwardUser( pChar->GetCharName() );
	}
	else
	{
		pFightClubRecord->pChar = pChar;
	}
}

void ioFightClubMode::RemoveRecord( const ioHashString &rkName )
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

	FightClubTimeGaugeWnd *pTimeGauge = dynamic_cast< FightClubTimeGaugeWnd * >( g_GUIMgr.FindWnd( FIGHTCLUB_TIME_GAUGE_WND ) );
	if( pTimeGauge )
		pTimeGauge->RemoveCharSeqInfo( rkName );
}

void ioFightClubMode::RemoveRecord( int nIndex )
{
	int iCharCnt = m_vCharRecordList.size();
	if( iCharCnt > nIndex )
	{
		m_vCharRecordList.erase( m_vCharRecordList.begin() + nIndex );
		UpdateUserRank();
	}
}

ioBaseChar* ioFightClubMode::GetRecordChar( int i )
{
	if( COMPARE( i, 0, GetRecordCharCnt() ) )
		return m_vCharRecordList[i].pChar;

	return NULL;
}

ioBaseChar* ioFightClubMode::GetRecordChar( const ioHashString &rkName )
{
	FightClubRecord *pRecord = FindRecordCharName( rkName );
	if( pRecord )
		return pRecord->pChar;

	return NULL;
}

int ioFightClubMode::GetRecordCharCnt() const
{
	return m_vCharRecordList.size();
}

ModeRecord* ioFightClubMode::FindModeRecord( int iIdx )
{
	if( COMPARE( iIdx, 0, GetRecordCharCnt() ) )
		return &m_vCharRecordList[iIdx];

	return NULL;
}

ModeRecord* ioFightClubMode::FindModeRecordCharName( const ioHashString &rkName )
{
	return (ModeRecord*)FindRecordCharName( rkName );
}

FightClubRecord* ioFightClubMode::FindRecordCharName( const ioHashString &rkName )
{
	int iCharCnt = m_vCharRecordList.size();
	for( int i=0 ; i<iCharCnt ; i++ )
	{
		if( m_vCharRecordList[i].szName == rkName )
			return &m_vCharRecordList[i];
	}

	return NULL;
}

bool ioFightClubMode::ProcessUDPPacket( SP2Packet &rkPacket )
{
	if( ioPlayMode::ProcessUDPPacket( rkPacket ) )
		return true;

	return false;
}

void ioFightClubMode::ApplyModeInfo( SP2Packet &rkPacket )
{
	rkPacket >> m_iCurRoundCount;
	rkPacket >> m_iMaxRoundCount;

	rkPacket >> m_dwRoundDuration;

	m_SingleTeamPosArray.clear();
	int iPosCnt;
	rkPacket >> iPosCnt;
	for( int i=0; i < iPosCnt; ++i )
	{
		int iPosArray;
		rkPacket >> iPosArray;
		m_SingleTeamPosArray.push_back( iPosArray );
	}
}

void ioFightClubMode::ApplyCharModeInfo( const ioHashString &rkName, SP2Packet &rkPacket, bool bDieCheck )
{
	bool bRecord;
	rkPacket >> bRecord;

	if( !bRecord ) return;

	FightClubRecord *pRecord = FindRecordCharName( rkName );
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
		
		rkPacket >> pRecord->iFightState >> pRecord->iFightSeq >> pRecord->iFightVictories >> pRecord->iFightCheer;
		FightClubTimeGaugeWnd *pTimeGauge = dynamic_cast< FightClubTimeGaugeWnd * >( g_GUIMgr.FindWnd( FIGHTCLUB_TIME_GAUGE_WND ) );
		if( pTimeGauge )
		{
			pTimeGauge->InsertCharSeqInfo( pRecord->szName, pRecord->iFightState, pRecord->iFightSeq, pRecord->iFightVictories, pRecord->iFightCheer, pRecord->pChar->GetLevel() );
		}
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

		int iFightState, iFightSeq, iFightVictories, iFightCheer;
		rkPacket >> iFightState >> iFightSeq >> iFightVictories >> iFightCheer;
	}
}

void ioFightClubMode::ApplyNewUserModeInfo( const ioHashString &rkName, SP2Packet &rkPacket )
{
	FightClubRecord *pRecord = FindRecordCharName( rkName );
	if( pRecord )
	{
		rkPacket >> pRecord->iFightState >> pRecord->iFightSeq >> pRecord->iFightVictories >> pRecord->iFightCheer;
		FightClubTimeGaugeWnd *pTimeGauge = dynamic_cast< FightClubTimeGaugeWnd * >( g_GUIMgr.FindWnd( FIGHTCLUB_TIME_GAUGE_WND ) );
		if( pTimeGauge && pRecord->pChar )
		{
			pTimeGauge->InsertCharSeqInfo( pRecord->szName, pRecord->iFightState, pRecord->iFightSeq, pRecord->iFightVictories, pRecord->iFightCheer, pRecord->pChar->GetLevel() );		
		}
	}
}

void ioFightClubMode::SetRoundEndInfo( WinTeamType eTeam, bool bRoundSetEnd )
{
	g_SoundMgr.StopSound( m_TimeOutSound, 0 );
	g_SoundMgr.StopStream();

	m_bRoundSetEnd = bRoundSetEnd;

	ioBaseChar *pOwner = GetOwnerChar();
	if( pOwner )
	{
		if( pOwner->GetState() != CS_DIE )
		{
			g_MyInfo.SetCharDied( pOwner->GetSelectCharArray(), false );
		}
	}

	SetModeState( MS_RESULT );
	UpdateRoundWinRecord();
	SetRoundEndMotion();
	SetRoundEndVictories();
	ClearRoundEndAllBuff();

	g_ObserverWndMgr.ObserverWndHide();
	
	g_PushStructListMgr.DestroyStructList();
	g_MachineStructMgr.DestroyMachineStructList();
	g_DummyCharMgr.DestroyDummyCharList();

	m_pCreator->DestroyWeaponList();
	m_pCreator->EnableWorldEntityFunctions( false );

	if( !m_bRoundSetEnd )
	{
		if( P2PNetwork::IsNetworkPlaying() )
		{
			SetResultScoreWnd();
		}
	}
	else
	{
		// 전투방 옵저버X.
		if( g_BattleRoomMgr.IsBattleRoom() && !g_BattleRoomMgr.IsObserverUser( g_MyInfo.GetPublicID() ) )
		{
			if( GetModeType() != MT_MONSTER_SURVIVAL && GetModeType() != MT_DUNGEON_A )
			{
				g_QuestMgr.QuestOccurTerm( QO_BATTLE_PVP_FINAL_RESULT );
			}

			g_QuestMgr.QuestOccurTerm( QO_BATTLE_FINAL_RESULT );
		}
	}
	ClearObjectItem();
}

void ioFightClubMode::OnRoundReady( SP2Packet &rkPacket )
{
	ShowEndScoreWnd( false );
	SetBlindMode( false );

	// fight club만 사용
	bool bUseFightNPC;
	rkPacket >> bUseFightNPC;

	if( m_bUseFightNPC )
	{
		if( !bUseFightNPC )
		{
			m_bUseFightNPC = false;
			
			// remove fight npc
			RemoveFightNPCRecord();
		}
	}
	else
	{
		m_bUseFightNPC = bUseFightNPC;
	}

	rkPacket >> m_iCurRoundCount;
	rkPacket >> m_iBluePosArray;
	rkPacket >> m_iRedPosArray;
	rkPacket >> m_dwCurRoundDuration;

	// 새로운 시작
	FightClubTimeGaugeWnd *pTimeGauge = dynamic_cast< FightClubTimeGaugeWnd * >( g_GUIMgr.FindWnd( FIGHTCLUB_TIME_GAUGE_WND ) );
	if( pTimeGauge )
	{
		pTimeGauge->SetUseFightNPC( m_bUseFightNPC, m_iRoundReadyCharCount );
		pTimeGauge->ClearCharSeqInfo();	
	}

	int iRecordCount;
	rkPacket >> iRecordCount;
	for(int i = 0;i < iRecordCount;i++)
	{
		ioHashString kName;
		rkPacket >> kName;

		int iFightState, iFightSeq, iFightVictories, iFightCheer;
		rkPacket >> iFightState >> iFightSeq >> iFightVictories >> iFightCheer;

		FightClubRecord *pRecord = FindRecordCharName( kName );
		if( pRecord && pRecord->pChar )
		{
			pRecord->iFightState = iFightState;
			pRecord->iFightSeq = iFightSeq;
			pRecord->iFightVictories = iFightVictories;
			pRecord->iFightCheer = iFightCheer;

			if( pTimeGauge )
			{
				pTimeGauge->InsertCharSeqInfo( pRecord->szName, pRecord->iFightState, pRecord->iFightSeq, pRecord->iFightVictories, pRecord->iFightCheer, pRecord->pChar->GetLevel() );		
			}
		}
	}

	g_MyInfo.ClearCharDiedInfo();
	g_MyInfo.ClearCharDiedPenaltyInfo();
	g_MyInfo.ClearCharJoinedInfo();

	ClearModeData();
	ReStartMode();

	g_GUIMgr.HideWnd( MIDDLE_JOINDELAY_WND );
}

void ioFightClubMode::OnRoundEnd( SP2Packet &rkPacket )
{	
	bool bRoundSetEnd;
	rkPacket >> bRoundSetEnd;

	EndChangeCharacter();
	
	ioHashString kLeaveName;
	int iRoundEndType = FightClubRoundEndWnd::ROUNDEND_KO;
	float fChampHPRate = FLOAT1;
	float fChallHPRate = FLOAT1;

	// 챔피언과 도전자의 HP를 전송
	SP2Packet kPacket( CTPK_FIGHTCLUB_RESULT_INFO );
	FightClubTimeGaugeWnd *pTimeGauge = dynamic_cast< FightClubTimeGaugeWnd * >( g_GUIMgr.FindWnd( FIGHTCLUB_TIME_GAUGE_WND ) );
	if( pTimeGauge )
	{
		float fHPRate = 0.0f;
		// 챔피언
		ioBaseChar *pChampChar = m_pCreator->GetBaseChar( pTimeGauge->GetChampionUserName() );
		if( pChampChar )
		{
			float fMax = pChampChar->GetHP().m_fMaxValue;
			float fCur = pChampChar->GetHP().m_fCurValue;
			if( fMax > 0.0f )
				fHPRate = fCur / fMax;

			if( fHPRate >= FLOAT1 )
				iRoundEndType = FightClubRoundEndWnd::ROUNDEND_PERFECT;

#if defined( USE_GA )
			bool bWin = false;
			if( g_MyInfo.GetPublicID() == pTimeGauge->GetChampionUserName() )
				bWin = true;

			g_HttpMng.SetWin( bWin );
#endif
		}
		else 
		{
			fHPRate = -FLOAT1000;        // 이탈 유저

			kLeaveName = pTimeGauge->GetChampionUserName();
			iRoundEndType = FightClubRoundEndWnd::ROUNDEND_LEAVEROOM;
		}
		kPacket << fHPRate;

		fChampHPRate = fHPRate;

		// 도전자
		fHPRate = 0.0f;
		ioBaseChar *pChallChar = m_pCreator->GetBaseChar( pTimeGauge->GetChallengerUserName() );
		if( pChallChar )
		{
			float fMax = pChallChar->GetHP().m_fMaxValue;
			float fCur = pChallChar->GetHP().m_fCurValue;

			if( fMax > 0.0f )
				fHPRate = fCur / fMax;

			if( fHPRate >= FLOAT1 && iRoundEndType != FightClubRoundEndWnd::ROUNDEND_LEAVEROOM )
				iRoundEndType = FightClubRoundEndWnd::ROUNDEND_PERFECT;
		}
		else 
		{
			fHPRate = -FLOAT1000;        // 이탈 유저

			kLeaveName = pTimeGauge->GetChallengerUserName();
			iRoundEndType = FightClubRoundEndWnd::ROUNDEND_LEAVEROOM;
		}
		kPacket << fHPRate;

		fChallHPRate = fHPRate;

		if( pChallChar && pChampChar )
		{
			if( pChallChar->GetState() != CS_DIE && pChampChar->GetState() != CS_DIE )
			{
				iRoundEndType = FightClubRoundEndWnd::ROUNDEND_TIMEOUT;
			}
		}
	}
	TCPNetwork::SendToServer( kPacket );

	if( m_bUseFightNPC && bRoundSetEnd && iRoundEndType == FightClubRoundEndWnd::ROUNDEND_TIMEOUT )
	{
		if( fChampHPRate < fChallHPRate )
			bRoundSetEnd = false;
	}

	SendOwnerDamageList( bRoundSetEnd );

	ioBaseChar *pOwner = GetOwnerChar();
	if( pOwner )
	{
		g_MyInfo.SetCharDiedPenalty( pOwner->GetSelectCharArray(), false );
	}

	SetRoundEndInfo( WTT_DRAW, bRoundSetEnd );            // 처리만 중요하지 누가 있겼는지는 따로 온다.

	FightClubRoundEndWnd *pRoundEnd = dynamic_cast< FightClubRoundEndWnd * >( g_GUIMgr.FindWnd( FIGHTCLUB_ROUND_END_WND ) );
	if( pRoundEnd )
	{
		pRoundEnd->SetRoundEnd( iRoundEndType, kLeaveName );
	}

	g_GUIMgr.HideWnd( FIGHTCLUB_TIME_GAUGE_WND );

	TutorialKeyboardWnd *pKeyboardWnd = dynamic_cast< TutorialKeyboardWnd * >( g_GUIMgr.FindWnd( TUTORIAL_KEYBOARD_WND ) );
	if( pKeyboardWnd && pKeyboardWnd->IsShow() )
	{
		pKeyboardWnd->SetHideProcess();
	}
}

void ioFightClubMode::OnRoundJoin( SP2Packet &rkPacket )
{
	ioHashString szJoinChar;
	int iModeState;

	rkPacket >> szJoinChar;
	rkPacket >> iModeState;

	if( szJoinChar.IsEmpty() ) return;

	ioBaseChar *pJoinChar = m_pCreator->GetBaseChar( szJoinChar );
	if( !pJoinChar ) return;
	if( pJoinChar->IsOwnerChar() )
	{
		SetModeState( (ModeState)iModeState );

		DWORD dwStateGap;
		rkPacket >> dwStateGap;

		DWORD dwStartTime = FRAMEGETTIME() - dwStateGap;
		switch( m_ModeState )
		{
		case MS_READY:
			m_dwReadyStartTime = dwStartTime;
			break;
		case MS_PLAY:
		case MS_NEW_CHALLENGER:
			m_dwPlayStartTime = dwStartTime;
			break;
		case MS_RESULT:
			m_dwResultStartTime = dwStartTime;
			break;
		}

		if( m_ModeState != MS_RESULT )
		{
			int iSelectChar = 0;
			
			iSelectChar = pJoinChar->GetSelectCharArray();
			pJoinChar->SetReadyState();

			DWORD dwSelectTime;
			rkPacket >> dwSelectTime;

			if( m_ModeState != MS_NEW_CHALLENGER )
			{
				StartChangeCharacter( iSelectChar, dwSelectTime );
			}

			if( g_BattleRoomMgr.IsBattleRoom() )
			{
				g_QuestMgr.QuestCompleteTerm( QC_ENTER_BATTLE_PVP, GetModeType() );
			}
			else if( g_LadderTeamMgr.IsLadderTeam() )
			{
				g_QuestMgr.QuestCompleteTerm( QC_ENTER_CAMP_BATTLE, GetModeType() );
			}
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
			DWORD dwCurTime = FRAMEGETTIME();
			ioBaseChar *pOwner = GetOwnerChar();
			if( pOwner )
			{
				if( dwCurTime > m_dwPlayStartTime && dwCurTime - m_dwPlayStartTime >= 10000 )
				{
					g_ChatMgr.SetJoinUserChatMsg( pJoinChar->GetCharName().c_str(), pJoinChar->GetTeam(), pJoinChar->GetLevel() );
				}
			}
		}
	}

	m_pCreator->ReCalculateDamageRate( false );
}

void ioFightClubMode::OnRoundJoinView( SP2Packet &rkPacket )
{
	ioHashString szJoinChar;
	int iModeState;

	rkPacket >> szJoinChar;
	rkPacket >> iModeState;

	if( szJoinChar.IsEmpty() ) return;

	ioBaseChar *pJoinChar = m_pCreator->GetBaseChar( szJoinChar );
	if( !pJoinChar ) return;

	if( pJoinChar->IsOwnerChar() )
	{
		SetModeState( (ModeState)iModeState );

		DWORD dwStateGap;
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
		
		rkPacket >> m_dwCurRoundDuration;
	}
	else if( m_ModeState == MS_PLAY )
	{
		ioBaseChar *pOwner = GetOwnerChar();
		if( pOwner )
		{
			g_ChatMgr.SetJoinUserChatMsg( pJoinChar->GetCharName().c_str(), pJoinChar->GetTeam(), pJoinChar->GetLevel() );
		}

		pJoinChar->SetState( CS_VIEW );
	}

	m_pCreator->ReCalculateDamageRate( false );
}

void ioFightClubMode::OnRoundJoinObserver( SP2Packet &rkPacket )
{
	ioPlayMode::OnRoundJoinObserver( rkPacket );
}

void ioFightClubMode::OnFinalRoundResult( SP2Packet &rkPacket )
{
	// 사용하지 않음. 
}

int ioFightClubMode::GetRoundWinCnt( TeamType eTeamType )
{
	return 0;
}

void ioFightClubMode::ContributeConvertTeam( TeamType eTeam )
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
		FightClubRecord *pRecord = &m_vCharRecordList[i];
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
			FightClubRecord *pRecord = &m_vCharRecordList[iArray];
			if( !pRecord->Contribute.m_bLastResultSync )
				pRecord->Contribute.m_iContribute = m_vContributeTable[vInfo[i].m_iRank];
			pRecord->Contribute.m_iKillRank = vInfo[i].m_iRank;
		}
	}
	vInfo.clear();

	//데미지 & 힐
	for(i = 0;i < iCharCnt;i++)
	{
		FightClubRecord *pRecord = &m_vCharRecordList[i];
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
			FightClubRecord *pRecord = &m_vCharRecordList[iArray];
			if( !pRecord->Contribute.m_bLastResultSync )
				pRecord->Contribute.m_iContribute += m_vContributeTable[vInfo[i].m_iRank] * 6;
			pRecord->Contribute.m_iDamageRank = vInfo[i].m_iRank;
		}
	}
	vInfo.clear();

	//유효 타격 횟수
	for(i = 0;i < iCharCnt;i++)
	{
		FightClubRecord *pRecord = &m_vCharRecordList[i];
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
			FightClubRecord *pRecord = &m_vCharRecordList[iArray];
			if( !pRecord->Contribute.m_bLastResultSync )
				pRecord->Contribute.m_iContribute += m_vContributeTable[vInfo[i].m_iRank] * 2;
			pRecord->Contribute.m_iAttackCntRank = vInfo[i].m_iRank;
		}
	}
	vInfo.clear();

	//데쓰처리
	for(i = 0;i < iCharCnt;i++)
	{
		FightClubRecord *pRecord = &m_vCharRecordList[i];
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
			FightClubRecord *pRecord = &m_vCharRecordList[iArray];
			if( !pRecord->Contribute.m_bLastResultSync )
				pRecord->Contribute.m_iContribute += m_vContributeTable[vInfo[i].m_iRank];
			pRecord->Contribute.m_iDeathRank = vInfo[i].m_iRank;
		}
	}
	vInfo.clear();
}

void ioFightClubMode::ContributeRoundAllPercent( DWORD dwCurTime, DWORD dwTotalTime )
{
	//기여도의 100분율
	int iMaxContribute	= 0;
	int iCharCnt		= m_vCharRecordList.size();
	int iPlayCharCnt	= 0;

	int i = 0;
	for(i = 0;i < iCharCnt;i++)
	{
		FightClubRecord *pRecord = &m_vCharRecordList[i];
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
			FightClubRecord *pRecord = &m_vCharRecordList[i];
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
}

void ioFightClubMode::ContributeConvert()
{
	ContributeConvertTeam( TEAM_NONE );
}

int ioFightClubMode::ContributeMyRank( int &rRankValue1, int &rRankValue2, int &rRankValue3, int &rRankValue4 )
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
		FightClubRecord *pRecord = &m_vCharRecordList[i];
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
		FightClubRecord *pRecord = &m_vCharRecordList[i];
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
		FightClubRecord *pRecord = &m_vCharRecordList[i];
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

void ioFightClubMode::DamageContribute( const ioHashString szAttacker, const ioHashString szTarget, float fDamage, DWORD dwItemCode, int iWeaponType )
{
	if( szAttacker.IsEmpty() ) return;

	FightClubRecord *pRecord = FindRecordCharName( szAttacker );
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
	float fDamagePoint = (fDamage + fBonusDamage) * 100;
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

void ioFightClubMode::KillContribute( const ioHashString szKiller, const ioHashString szKilled, DWORD dwItemCode )
{
	if( szKilled.IsEmpty() ) return;

	int iKillPoint = 100;

	float fAddKillPoint = FLOAT1;
	if( IsContributeAddPointItemCode( dwItemCode ) )
		fAddKillPoint = 1.1f;

    if( szKiller.IsEmpty() || szKiller == szKilled )     //자살
	{
		FightClubRecord *pRecord = FindRecordCharName( szKilled );
		if( !pRecord || !pRecord->pChar ) return;

		pRecord->Contribute.m_iKill -= iKillPoint;
		pRecord->Contribute.m_iDeath++;
	}
	else
	{
		FightClubRecord *pKillerRecord = FindRecordCharName( szKiller );
		FightClubRecord *pKilledRecord = FindRecordCharName( szKilled );
		if( pKillerRecord && pKilledRecord )
		{
			if( pKillerRecord->pChar && pKilledRecord->pChar )
			{
				if( pKillerRecord->pChar->GetTeam() == pKilledRecord->pChar->GetTeam() )    //팀킬
					pKillerRecord->Contribute.m_iKill -= (float)iKillPoint * fAddKillPoint;					
				else
					pKillerRecord->Contribute.m_iKill += (float)iKillPoint * fAddKillPoint;
				pKilledRecord->Contribute.m_iDeath++;
			}
		}
	}

	ContributeConvert();
	ContributeRoundAllPercent();
	UpdateRoundInfoList();
	CheckTotalChamp();
}

void ioFightClubMode::AttackCntContribute( const ioHashString szAttacker, const ioHashString szTarget, DWORD dwItemCode, int iAttackValue/* = 100 */ )
{
	if( szAttacker.IsEmpty() || szTarget.IsEmpty() ) return;

	FightClubRecord *pAttackerRecord = FindRecordCharName( szAttacker );
	FightClubRecord *pTargetRecord = FindRecordCharName( szTarget );

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

void ioFightClubMode::ContributeLOG()
{
	char szLog[MAX_PATH];

	int iCharCnt = m_vCharRecordList.size();
	for( int i=0 ;i<iCharCnt; i++ )
	{
		FightClubRecord *pRecord = &m_vCharRecordList[i];
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

void ioFightClubMode::FillLastPlayRecordInfo( SP2Packet &rkPacket )
{
	int iCharCnt = m_vCharRecordList.size();
	rkPacket << iCharCnt;
	for(int i = 0;i < iCharCnt;i++)
	{
		FightClubRecord *pRecord = &m_vCharRecordList[i];
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

void ioFightClubMode::FillPlayRecordInfo( SP2Packet &rkPacket )
{
	int iCharCnt = m_vCharRecordList.size();
	rkPacket << iCharCnt;
	for(int i = 0;i < iCharCnt;i++)
	{
		FightClubRecord *pRecord = &m_vCharRecordList[i];
		rkPacket << pRecord->pChar->GetCharName();
		rkPacket << pRecord->Contribute.m_iDamage;
		rkPacket << pRecord->Contribute.m_iAttackCnt;
		rkPacket << pRecord->Contribute.m_iKill;
		rkPacket << pRecord->Contribute.m_iDeath;
		rkPacket << pRecord->Contribute.m_iPrevContributePer;
		rkPacket << pRecord->iFightWin;
		rkPacket << pRecord->iFightLose;
	}
}

void ioFightClubMode::ApplyPlayRecordInfo( SP2Packet &rkPacket )
{
	int iCharCnt;
	rkPacket >> iCharCnt;
	for(int i = 0;i < iCharCnt;i++)
	{
		ioHashString szName;
		int iDamage, iAttackCnt, iKill, iDeath, iPrevContributePer, iFightWin, iFightLose;
		rkPacket >> szName >> iDamage >> iAttackCnt >> iKill >> iDeath >> iPrevContributePer >> iFightWin >> iFightLose;

		FightClubRecord *pRecord = FindRecordCharName( szName );
		if( pRecord )
		{
			pRecord->Contribute.m_iDamage			= iDamage;
			pRecord->Contribute.m_iAttackCnt		= iAttackCnt;
			pRecord->Contribute.m_iKill             = iKill;
			pRecord->Contribute.m_iDeath            = iDeath;
			pRecord->Contribute.m_iPrevContributePer= iPrevContributePer;
			pRecord->iFightWin						= iFightWin;
			pRecord->iFightLose						= iFightLose;
		}
	}

	ContributeConvert();
	ContributeRoundAllPercent();
	UpdateRoundInfoList();
	CheckTotalChamp();
}

void ioFightClubMode::ContributeResultSync( const ioHashString &szName, int iContribute )
{
	FightClubRecord *pRecord = FindRecordCharName( szName );
	if( !pRecord ) return;

	pRecord->Contribute.m_bLastResultSync = true;
	pRecord->Contribute.m_iContribute     = iContribute;
}

void ioFightClubMode::ShowEndScoreWnd( bool bShow )
{
}

void ioFightClubMode::UpdateRoundInfoList()
{
	m_RoundInfoList.clear();
	m_RoundInfoList.reserve( MAX_PLAYER );

	for( int i = 0 ; i<GetRecordCharCnt(); i++ )
	{
		RoundInfo kInfo;
		if( GetRoundInfo( i, kInfo ) )
			m_RoundInfoList.push_back( kInfo );
	}

	if( m_RoundInfoList.size() == 0 )
		return;

	std::sort( m_RoundInfoList.begin(), m_RoundInfoList.end(), RoundInfoSort() );

	int iRank = 1;
	RoundInfoList::iterator iter;
	for( iter=m_RoundInfoList.begin() ; iter!=m_RoundInfoList.end() ; ++iter, ++iRank )
	{
		ModeRecord *pRecord = FindModeRecordCharName( iter->m_szName );

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

void ioFightClubMode::UpdateUIRank()
{
	FightClubRecord *pRecord = FindRecordCharName( g_MyInfo.GetPublicID() );
	if( g_BattleRoomMgr.IsObserverUser( g_MyInfo.GetPublicID() ) || m_pCreator->IsStealthMode() )
	{
		ioBaseChar *pTarget = m_pCreator->GetCameraTargetChar();
		if( pTarget )
		{
			pRecord = FindRecordCharName( pTarget->GetCharName() );
		}
	}
	else if( pRecord )
	{
		// 중앙 UI는 관전자에게 표시 안함
		if( m_ModeState == MS_PLAY )
		{
			g_ModeUIController.SetDeathMatch( pRecord->iCurRank );
		}
	}	
}

int ioFightClubMode::GetCurRank( const ioHashString& szCharName )
{
	ModeRecord *pTargetRecord = FindModeRecordCharName( szCharName );
	if( !pTargetRecord )	return -1;
	if( !pTargetRecord->pChar ) return -1;

	int iSize = m_RoundInfoList.size();
	for( int i=0; i < iSize; i++ )
	{
		const RoundInfo &kRoundInfo = m_RoundInfoList[i];

		if( kRoundInfo.m_szName == szCharName )
			return i+1;
	}

	return -1;
}

int ioFightClubMode::GetContributePerChar( const ioHashString& szName )
{
	int iCharCnt = m_vCharRecordList.size();

	for(int i = 0;i < iCharCnt;i++)
	{
		FightClubRecord *pRecord = &m_vCharRecordList[i];
		if( !pRecord )	continue;
		if( !pRecord->pChar ) continue;

		if( pRecord->pChar->GetCharName() == szName )
		{
			return pRecord->Contribute.m_iPrevContributePer;
		}
	}

	return 0;
}

void ioFightClubMode::CheckEndSceneLookAt()
{
	// 옵저버는 다르게 처리
	if( g_BattleRoomMgr.IsObserverUser( g_MyInfo.GetPublicID() ) || m_pCreator->IsStealthMode() )
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
}

void ioFightClubMode::OnChangeCharNextProcess( ioBaseChar *pChar, bool bNewChar )
{
	FightClubTimeGaugeWnd *pTimeGauge = dynamic_cast< FightClubTimeGaugeWnd * >( g_GUIMgr.FindWnd( FIGHTCLUB_TIME_GAUGE_WND ) );
	if( pTimeGauge )
	{
		pTimeGauge->SetCharChange( pChar );
	}

	if( pChar && bNewChar )
	{
		if( pChar->GetCharName() == GetChampionName() || pChar->GetCharName() == GetChallengerName() )
		{
			m_iRoundReadyCharCount++;
		}
	}

	if( pChar && pChar->IsOwnerChar() )
	{
		if( pChar->GetLevel() <= m_iKeyboardShowGradeLimit )
		{
			g_GUIMgr.ShowWnd( TUTORIAL_KEYBOARD_WND );
		}
	}
}

void ioFightClubMode::ItemEquipAfterUIProcess( ioBaseChar *pChar, int iItemType )
{
	if( iItemType != ioItem::IT_HELMET )
		return;

	FightClubTimeGaugeWnd *pTimeGauge = dynamic_cast< FightClubTimeGaugeWnd * >( g_GUIMgr.FindWnd( FIGHTCLUB_TIME_GAUGE_WND ) );
	if( pTimeGauge )
	{
		pTimeGauge->SetHelmChange( pChar );
	}
}

void ioFightClubMode::ItemReleaseAfterUIProcess( ioBaseChar *pChar, int iItemType )
{
	if( iItemType != ioItem::IT_HELMET )
		return;

	FightClubTimeGaugeWnd *pTimeGauge = dynamic_cast< FightClubTimeGaugeWnd * >( g_GUIMgr.FindWnd( FIGHTCLUB_TIME_GAUGE_WND ) );
	if( pTimeGauge )
	{
		pTimeGauge->SetHelmChange( pChar );
	}
}

void ioFightClubMode::ItemEntityChangeAfterUIProcess( ioBaseChar *pChar, int iItemType )
{
	if( iItemType != ioItem::IT_HELMET )
		return;

	FightClubTimeGaugeWnd *pTimeGauge = dynamic_cast< FightClubTimeGaugeWnd * >( g_GUIMgr.FindWnd( FIGHTCLUB_TIME_GAUGE_WND ) );
	if( pTimeGauge )
	{
		pTimeGauge->SetHelmChange( pChar );
	}
}

const ioHashString &ioFightClubMode::GetChampionName()
{
	FightClubTimeGaugeWnd *pTimeGauge = dynamic_cast< FightClubTimeGaugeWnd * >( g_GUIMgr.FindWnd( FIGHTCLUB_TIME_GAUGE_WND ) );
	if( pTimeGauge )
	{
		return pTimeGauge->GetChampionUserName();
	}
	return GetOwnerName();
}

const ioHashString &ioFightClubMode::GetChallengerName()
{
	FightClubTimeGaugeWnd *pTimeGauge = dynamic_cast< FightClubTimeGaugeWnd * >( g_GUIMgr.FindWnd( FIGHTCLUB_TIME_GAUGE_WND ) );
	if( pTimeGauge )
	{
		return pTimeGauge->GetChallengerUserName();
	}
	return GetOwnerName();
}

bool ioFightClubMode::ProcessTCPPacket( SP2Packet &rkPacket )
{
	ErrorReport::SetPosition( 1100, 1 );
	if( ioPlayMode::ProcessTCPPacket( rkPacket ) )
		return true;

	switch( rkPacket.GetPacketID() )
	{
	case STPK_FIGHTCLUB_RESULT:
		ErrorReport::SetPosition( 1100, 2 );
		OnFightClubResult_Renew( rkPacket );
		ErrorReport::SetPosition( 1100, 3 );
		return true;
	case STPK_FIGHTCLUB_SEQUENCE:
		ErrorReport::SetPosition( 1100, 4 );
		OnFightClubSequence( rkPacket );
		ErrorReport::SetPosition( 1100, 5 );
		return true;
	case STPK_FIGHTCLUB_CHEER:
		ErrorReport::SetPosition( 1100, 6 );
		OnFightClubCheer( rkPacket );
		ErrorReport::SetPosition( 1100, 7 );
		return true;
	case STPK_FIGHT_NPC:
		OnFightClubNPC( rkPacket );
		return true;
	}

	ErrorReport::SetPosition( 1100, 100 );
	return false;
}

void ioFightClubMode::OnFightClubResult_Renew( SP2Packet &rkPacket )
{
	FightClubTimeGaugeWnd *pTimeGauge  = dynamic_cast< FightClubTimeGaugeWnd * >( g_GUIMgr.FindWnd( FIGHTCLUB_TIME_GAUGE_WND ) );
	FightClubRoundEndWnd *pRoundResult = dynamic_cast< FightClubRoundEndWnd * >( g_GUIMgr.FindWnd( FIGHTCLUB_ROUND_END_WND ) );
	if( pTimeGauge == NULL || pRoundResult == NULL )
		 return;

	int resultType = 0, championGrade = 0, challengerGrade = 0, victoryCount = 0, targetType = 0;
	int winnerGrade = 0, loserGrade = 0;
	DWORD dwResultTime = 0;
	ioHashString kChampionName, kChallengerName, winner, loser;
	rkPacket >> dwResultTime >> resultType >> kChampionName >> kChallengerName >> championGrade >> challengerGrade >> victoryCount >> targetType;

	bool bChampionWin = (1 == resultType) ? true : false;
	if(bChampionWin)
	{
		winner		= kChampionName;
		winnerGrade = championGrade;
		loser		= kChallengerName;
		loserGrade	= challengerGrade;
	}
	else
	{
		winner		= kChallengerName;
		winnerGrade = challengerGrade;
		loser		= kChampionName;
		loserGrade	= championGrade;
	}

	if((1 == targetType) || (2 == targetType))
	{
		// 챔피온 or 도전자
		int iChampExp = 0, iChampPeso = 0, iChampClassList = 0;
		__int64 iChampMoney = 0;

		rkPacket >> iChampExp >> iChampPeso >> iChampMoney >> iChampClassList;
		for(int i = 0;i < iChampClassList;i++)
		{
			int iClassType, iClassExp;
			rkPacket >> iClassType >> iClassExp;
			if( iClassType == 0 ) continue;

			g_MyInfo.AddClassExp( iClassType, iClassExp );
			float fGradeExp = g_MyInfo.GetSoldierExpBonusByGradeExp( iClassType, iClassExp );
			g_MyInfo.AddGradeExp( fGradeExp );

			pRoundResult->SetPlayClassType( iClassType );
		}

		float fBonusArray[ioPlayMode::BA_MAX];
		for (i = 0; i < ioPlayMode::BA_MAX; i++)
			rkPacket >> fBonusArray[i];

		g_MyInfo.SetMoney( iChampMoney );

		// 결과 UI 반영
		pRoundResult->SetRoundResult(	dwResultTime, 
										kChampionName, kChallengerName, 
										iChampExp, iChampPeso, victoryCount, bChampionWin, fBonusArray,
										pTimeGauge->GetMeSeqCount() );
	}
	else
	{
		// 대기자 혹은 옵저버
		// 결과 UI 반영
		int iCheerCode = 0, iPeso = 0;
		rkPacket >> iCheerCode >> iPeso;

		g_MyInfo.SetMoney( g_MyInfo.GetMoney() + iPeso );
		pRoundResult->SetRoundResult(	dwResultTime, 
										kChampionName, kChallengerName, 
										0, iPeso, victoryCount, bChampionWin, NULL,
										pTimeGauge->GetMeSeqCount() );

	}

	if( m_bUseFightNPC )
	{
		pRoundResult->SetFightNPCInfo( m_bUseFightNPC, (int)m_FightNPCRecord.size(), m_iCurFightNPCStage );

		bool bOwnerWin = false;
		ioBaseChar *pEffectOwner = NULL;
		if( GetChampionName() == winner )
		{
			// 승자
			ioBaseChar *pWinner = GetRecordChar( GetChampionName() );
			if( pWinner )
			{
				pEffectOwner = pWinner;
				bOwnerWin = pWinner->IsOwnerChar();
				pWinner->StartNoInputDelayTime( 10000 );
				pWinner->SetRoundEndMotion( RM_WIN );		
				pWinner->SetLevel( winnerGrade );
				FightClubRecord *pWinnerRecord = FindRecordCharName( GetChampionName() );
				if( pWinnerRecord )
				{
					pWinnerRecord->iFightWin++;
					pWinnerRecord->iFightSeqLoseCount = 0;
				}
			}

			// 패자
			ioBaseChar *pLoser = GetRecordChar( m_FightNPCName );
			if( pLoser )
			{
				pLoser->StartNoInputDelayTime( 10000 );

				if( pLoser->GetState() != CS_DIE )
					pLoser->SetRoundEndMotion( RM_LOSE );		
			
				pLoser->SetLevel( loserGrade );

				FightClubRecord *pLoserRecord = FindRecordCharName( m_FightNPCName );
				if( pLoserRecord )
				{
					pLoserRecord->iFightLose++;
					pLoserRecord->iFightSeqLoseCount++;
				}
			}
		}
		else
		{
			// 승자
			ioBaseChar *pWinner = GetRecordChar( m_FightNPCName );
			if( pWinner )
			{
				pWinner->StartNoInputDelayTime( 10000 );
				pWinner->SetRoundEndMotion( RM_WIN );		
				pWinner->SetLevel( winnerGrade );

				FightClubRecord *pWinnerRecord = FindRecordCharName( m_FightNPCName );
				if( pWinnerRecord )
				{
					pWinnerRecord->iFightWin++;
					pWinnerRecord->iFightSeqLoseCount = 0;
				}
			}

			// 패자
			ioBaseChar *pLoser = GetRecordChar( GetChampionName() );
			if( pLoser )
			{
				pLoser->StartNoInputDelayTime( 10000 );

				if( pLoser->GetState() != CS_DIE )
					pLoser->SetRoundEndMotion( RM_LOSE );		
			
				pLoser->SetLevel( loserGrade );

				FightClubRecord *pLoserRecord = FindRecordCharName( GetChampionName() );
				if( pLoserRecord )
				{
					pLoserRecord->iFightLose++;
					pLoserRecord->iFightSeqLoseCount++;
				}
			}
		}

		if( bOwnerWin )
		{
			if( m_iCurFightNPCStage == FightClubTimeGaugeWnd::MAX_FIGHT_NPC_WIN_COUNT )
			{
				// 맵 이펙트 작렬!
				if( !m_szFightAIClearEffect.IsEmpty() )
				{
					D3DXVECTOR3 vNewPos = ioMath::VEC3_ZERO;
					if( pEffectOwner )
					{
						if( IsNeedUpdateMapEffect( pEffectOwner ) )
							vNewPos = pEffectOwner->GetWorldPosition();
					}

					if( vNewPos == ioMath::VEC3_ZERO )
						CreateMapEffect( m_szFightAIClearEffect, GetEndSceneLookAt(), ioMath::UNIT_ALL );
					else
						CreateMapEffect( m_szFightAIClearEffect, vNewPos, ioMath::UNIT_ALL );
				}

				if( !m_szFightAIClearSound.IsEmpty() )
				{
					g_SoundMgr.StopStream();

					g_SoundMgr.OpenOggStream( m_szFightAIClearSound.c_str() );
					g_SoundMgr.PlayStream( true );
				}
			}
		}
	}
	else
	{
		// 승자
		ioBaseChar *pWinner = m_pCreator->GetBaseChar( winner );
		if( pWinner )
		{
			pWinner->StartNoInputDelayTime( 10000 );
			pWinner->SetRoundEndMotion( RM_WIN );		
			pWinner->SetLevel( winnerGrade );

			FightClubRecord *pWinnerRecord = FindRecordCharName( winner );
			if( pWinnerRecord )
			{
				pWinnerRecord->iFightWin++;
				pWinnerRecord->iFightSeqLoseCount = 0;
			}
		}

		// 패자
		ioBaseChar *pLoser = m_pCreator->GetBaseChar( loser );
		if( pLoser )
		{
			pLoser->StartNoInputDelayTime( 10000 );

			if( pLoser->GetState() != CS_DIE )
				pLoser->SetRoundEndMotion( RM_LOSE );		
			
			pLoser->SetLevel( loserGrade );

			FightClubRecord *pLoserRecord = FindRecordCharName( loser );
			if( pLoserRecord )
			{
				pLoserRecord->iFightLose++;
				pLoserRecord->iFightSeqLoseCount++;

				if( pLoser->IsOwnerChar() )
				{
					if( pLoserRecord->iFightSeqLoseCount > m_iAbuseLoseCount )
					{
						// 매크로 뷰
						pLoserRecord->iFightSeqLoseCount = 0;
						
						if( g_BattleRoomMgr.IsBattleRoom() )
						{
							g_MyInfo.SetBattleLockType( BattleLockProblemWnd::LT_MACRO_EXTENSION );
						}
					}
				}
			}
		}
	}

	ContributeRoundAllPercent( 0, 0 );	
	SetBlindMode( true, false );
}

void ioFightClubMode::OnFightClubSequence( SP2Packet &rkPacket )
{
	// 새로운 시작
	FightClubTimeGaugeWnd *pTimeGauge = dynamic_cast< FightClubTimeGaugeWnd * >( g_GUIMgr.FindWnd( FIGHTCLUB_TIME_GAUGE_WND ) );
	if( pTimeGauge )
		pTimeGauge->ClearCharSeqInfo();	

	int iRecordCount;
	rkPacket >> iRecordCount;
	for(int i = 0;i < iRecordCount;i++)
	{
		ioHashString kName;
		rkPacket >> kName;

		int iFightState, iFightSeq, iFightVictories, iFightCheer;
		rkPacket >> iFightState >> iFightSeq >> iFightVictories >> iFightCheer;

		FightClubRecord *pRecord = FindRecordCharName( kName );
		if( pRecord && pRecord->pChar )
		{
			pRecord->iFightState = iFightState;
			pRecord->iFightSeq = iFightSeq;
			pRecord->iFightVictories = iFightVictories;
			pRecord->iFightCheer = iFightCheer;

			if( pTimeGauge )
			{
				pTimeGauge->InsertCharSeqInfo( pRecord->szName, pRecord->iFightState, pRecord->iFightSeq, pRecord->iFightVictories, pRecord->iFightCheer, pRecord->pChar->GetLevel() );		
			}
		}
	}
}

void ioFightClubMode::OnFightClubCheer( SP2Packet &rkPacket )
{
	bool bOK;
	rkPacket >> bOK;

	if( bOK )
	{
		// 성공
		ioHashString rkName;
		rkPacket >> rkName;

		FightClubRecord *pRecord = FindRecordCharName( rkName );
		if( pRecord )
		{
			rkPacket >> pRecord->iFightCheer;

			FightClubTimeGaugeWnd *pTimeGauge = dynamic_cast< FightClubTimeGaugeWnd * >( g_GUIMgr.FindWnd( FIGHTCLUB_TIME_GAUGE_WND ) );
			if( pTimeGauge )
			{
				pTimeGauge->InsertCharSeqInfo( pRecord->szName, pRecord->iFightState, pRecord->iFightSeq, pRecord->iFightVictories, pRecord->iFightCheer, pRecord->pChar->GetLevel() );
			}
		}
	}
	else
	{
		// 실패		
		FightClubRecord *pRecord = FindRecordCharName( g_MyInfo.GetPublicID() );      // 나!~!!
		if( pRecord )
		{
			rkPacket >> pRecord->iFightCheer;

			FightClubTimeGaugeWnd *pTimeGauge = dynamic_cast< FightClubTimeGaugeWnd * >( g_GUIMgr.FindWnd( FIGHTCLUB_TIME_GAUGE_WND ) );
			if( pTimeGauge )
			{
				pTimeGauge->InsertCharSeqInfo( pRecord->szName, pRecord->iFightState, pRecord->iFightSeq, pRecord->iFightVictories, pRecord->iFightCheer, pRecord->pChar->GetLevel() );
			}
		}
	}
}

float ioFightClubMode::GetCurTimeHPDamage( const ioHashString &szOwner, const ioHashString &szEnemy, float fCurDamage, float fBaseDamage )
{
	float fResult = fCurDamage;

	ioBaseChar *pOwner = m_pCreator->GetBaseChar( szOwner );
	ioBaseChar *pEnemy = m_pCreator->GetBaseChar( szEnemy );

	if( !pOwner || !pEnemy )
		return fResult;

	float fOwnerHP = pOwner->GetHP().m_fCurValue;
	float fEnemyHP = pEnemy->GetHP().m_fCurValue;

	DWORD dwTimeGap = FRAMEGETTIME() - m_dwPlayStartTime;
	DWORD dwCurTime = (m_dwCurRoundDuration - dwTimeGap);

	fResult = g_MathManager.GetFightClubTimeHPDamage( dwCurTime, m_dwCurRoundDuration,
													  fOwnerHP, fEnemyHP,
													  fCurDamage, fBaseDamage );

	return fResult;
}

float ioFightClubMode::GetCurTimeHPGauge( const ioHashString &szOwner )
{
	float fResult = FLOAT1;

	ioBaseChar *pChamp = NULL;
	ioBaseChar *pChall = NULL;

	FightClubRecord *pChampRecord = FindRecordCharName( GetChampionName() );
	if( pChampRecord && pChampRecord->pChar )
		pChamp = pChampRecord->pChar;

	FightClubRecord *pChallRecord = FindRecordCharName( GetChallengerName() );
	if( pChallRecord && pChallRecord->pChar )
		pChall = pChallRecord->pChar;

	if( !pChamp || !pChall )
		return fResult;

	float fChampHP = pChamp->GetHP().m_fCurValue;
	float fChallHP = pChall->GetHP().m_fCurValue;

	DWORD dwTimeGap = FRAMEGETTIME() - m_dwPlayStartTime;
	DWORD dwCurTime = (m_dwCurRoundDuration - dwTimeGap);

	if( GetChampionName() == szOwner )
	{
		fResult = g_MathManager.GetFightClubTimeHPGauge( dwCurTime, m_dwCurRoundDuration, fChampHP, fChallHP );
	}
	else if( GetChallengerName() == szOwner )
	{
		fResult = g_MathManager.GetFightClubTimeHPGauge( dwCurTime, m_dwCurRoundDuration, fChallHP, fChampHP );
	}

	return fResult;
}

float ioFightClubMode::GetCurVictoryDamage( const ioHashString &szOwner, const ioHashString &szEnemy, float fCurDamage, float fBaseDamage )
{
	float fResult = fCurDamage;

	int iOwnerVictory = 0;
	int iEnemyVictory = 0;

	FightClubRecord *pOwnerRecord = FindRecordCharName( szOwner );
	if( pOwnerRecord && pOwnerRecord->pChar )
		iOwnerVictory = pOwnerRecord->iFightVictories;

	FightClubRecord *pEnemyRecord = FindRecordCharName( szEnemy );
	if( pEnemyRecord && pEnemyRecord->pChar )
		iEnemyVictory = pEnemyRecord->iFightVictories;

	fResult = g_MathManager.GetFightClubVictoryDamage( iOwnerVictory, iEnemyVictory, fCurDamage, fBaseDamage );

	return fResult;
}

float ioFightClubMode::GetCurVictoryGaugeRate( const ioHashString &szOwner )
{
	float fResult = FLOAT1;

	int iChampVictory = 0;
	int iChallVictory = 0;

	FightClubRecord *pChampRecord = FindRecordCharName( GetChampionName() );
	if( pChampRecord && pChampRecord->pChar )
		iChampVictory = pChampRecord->iFightVictories;

	FightClubRecord *pChallRecord = FindRecordCharName( GetChallengerName() );
	if( pChallRecord && pChallRecord->pChar )
		iChallVictory = pChallRecord->iFightVictories;

	if( GetChampionName() == szOwner )
	{
		fResult = g_MathManager.GetFightClubVictoryGauge( iChampVictory, iChallVictory );
	}
	else if( GetChallengerName() == szOwner )
	{
		fResult = g_MathManager.GetFightClubVictoryGauge( iChallVictory, iChampVictory );
	}
	
	return fResult;
}

void ioFightClubMode::RemoveStartBuffList()
{
	int iBuffCnt = m_vStartBuffList.size();
	if( iBuffCnt <= 0 ) return;

	ioBaseChar *pChampion = GetRecordChar( GetChampionName() );
	ioBaseChar *pChallenger = GetRecordChar( GetChallengerName() );

	if( pChampion )
	{
		for( int i=0; i < iBuffCnt; ++i )
		{
			pChampion->RemoveBuff( m_vStartBuffList[i] );
		}
	}

	if( pChallenger )
	{
		for( int i=0; i < iBuffCnt; ++i )
		{
			pChallenger->RemoveBuff( m_vStartBuffList[i] );
		}
	}
}

void ioFightClubMode::OnFightClubNPC( SP2Packet &rkPacket )
{
	int iType;
	rkPacket >> iType;

	switch( iType )
	{
	case FIGHT_NPC_INFO:
		OnFightClubNPCInfo( rkPacket );
		break;
	case FIGHT_NPC_CREATE:
		OnFightClubNPCCreate( rkPacket );
		break;
	case FIGHT_NPC_NEWCHALLENGER:
		OnFightClubNewChallenger( rkPacket );
		break;
	case FIGHT_NPC_CONTINUE:
		OnFightClubContinue( rkPacket );
		break;
	}
}

void ioFightClubMode::OnFightClubNPCInfo( SP2Packet &rkPacket )
{
	m_FightNPCRecord.clear();

	FightClubTimeGaugeWnd *pTimeGauge = dynamic_cast< FightClubTimeGaugeWnd * >( g_GUIMgr.FindWnd( FIGHTCLUB_TIME_GAUGE_WND ) );
	if( !pTimeGauge )
		return;

	rkPacket >> m_bUseFightNPC;
	pTimeGauge->SetUseFightNPC( m_bUseFightNPC, m_iRoundReadyCharCount );

	int iNPCSize;
	rkPacket >> iNPCSize;

	m_FightNPCRecord.reserve( iNPCSize );
	for( int i=0; i < iNPCSize; ++i )
	{
		int iTeam;
		FightNPCRecord kRecord;
		rkPacket >> kRecord.dwCode;
		rkPacket >> kRecord.szName;
		rkPacket >> iTeam;
		kRecord.eTeam = (TeamType)iTeam;

		m_FightNPCRecord.push_back( kRecord );
	}
}

void ioFightClubMode::OnFightClubNPCCreate( SP2Packet &rkPacket )
{
	int iNPCArray;
	rkPacket >> iNPCArray;

	ioHashString szSyncUser;
	rkPacket >> szSyncUser;

	int iNPCSize = m_FightNPCRecord.size();
	if( !COMPARE( iNPCArray, 0, iNPCSize ) )
		return;

	m_bUseFightNPC = true;

	DWORD dwNpcCode = m_FightNPCRecord[iNPCArray].dwCode;
	ioHashString kNpcName = m_FightNPCRecord[iNPCArray].szName;
	TeamType eTeam = m_FightNPCRecord[iNPCArray].eTeam;

	RemoveFightNPCRecord();

	FightClubRecord *pRecord = FindRecordCharName( kNpcName );
	if( !pRecord )
	{
		ioNpcChar *pNpcChar = g_NpcMgr.CreateNpcChar( NPC_NOT_INDEX, dwNpcCode, m_pCreator, this );
		if( !pNpcChar ) return;

		D3DXVECTOR3 vPos = GetStartPos( 1, false );

		pNpcChar->FillMaxHP();
		pNpcChar->SetCharName( kNpcName );
		pNpcChar->SetSyncUser( szSyncUser );
		pNpcChar->SetTeam( eTeam );
		pNpcChar->StartAI( 0, vPos.x, vPos.z );

		AddNewRecord( pNpcChar );
	}
	else if( pRecord->pChar )
	{
		m_pCreator->RemoveUser( pRecord->pChar->GetCharName(), true );

		ioNpcChar *pNpcChar = g_NpcMgr.CreateNpcChar( NPC_NOT_INDEX, dwNpcCode, m_pCreator, this );
		if( !pNpcChar ) return;

		D3DXVECTOR3 vPos = GetStartPos( 1, false );

		pNpcChar->FillMaxHP();
		pNpcChar->SetCharName( kNpcName );
		pNpcChar->SetSyncUser( szSyncUser );
		pNpcChar->SetTeam( eTeam );
		pNpcChar->StartAI( 0, vPos.x, vPos.z );

		AddNewRecord( pNpcChar );
	}

	FightClubRecord *pNewRecord = FindRecordCharName( kNpcName );
	if( pNewRecord )
	{
		m_iCurFightNPCStage = iNPCArray+1;

		FightClubTimeGaugeWnd *pTimeGauge = dynamic_cast< FightClubTimeGaugeWnd * >( g_GUIMgr.FindWnd( FIGHTCLUB_TIME_GAUGE_WND ) );
		if( pTimeGauge )
		{
			m_FightNPCName = kNpcName;

			pNewRecord->iFightState = FIGHTCLUB_CHALLENGER;

			pTimeGauge->SetUseFightNPC( m_bUseFightNPC, m_iRoundReadyCharCount );
			pTimeGauge->InsertNPCSeqInfo( pNewRecord->szName, FIGHTCLUB_CHALLENGER, m_iCurFightNPCStage );

			if( pNewRecord->pChar )
				pTimeGauge->SetCharChange( pNewRecord->pChar );
		}
	}
}

void ioFightClubMode::OnFightClubNewChallenger( SP2Packet &rkPacket )
{
	rkPacket >> m_NewChallengerName;

	FightClubChallengerWnd *pChallengerWnd = dynamic_cast< FightClubChallengerWnd * >( g_GUIMgr.FindWnd( FIGHTCLUB_CHALLENGER_WND ) );
	if( pChallengerWnd )
	{
		pChallengerWnd->SetChallenger();
	}

	g_GUIMgr.HideWnd( FIGHTCLUB_TIME_GAUGE_WND );

	SetModeState( MS_NEW_CHALLENGER );
	
	m_dwNewChallengerTime = FRAMEGETTIME();
	m_pCreator->StartScreenBlind( SBT_FIXED_BLIND, NULL, 0 );
}

void ioFightClubMode::OnFightClubContinue( SP2Packet &rkPacket )
{
	g_GUIMgr.HideWnd( FIGHTCLUB_TIME_GAUGE_WND );
	g_GUIMgr.HideWnd( FIGHTCLUB_ROUND_END_WND );

	SetModeState( MS_NPC_CONTINUE );

	// 시간 체크할 UI 처리...
	FightClubFightNPCContinueWnd *pContinueWnd = dynamic_cast< FightClubFightNPCContinueWnd * >( g_GUIMgr.FindWnd( FIGHTCLUB_CONTINUE_WND ) );
	if( pContinueWnd )
	{
		pContinueWnd->SetContinue( m_iCurFightNPCStage, GetChampionName() );

		m_pCreator->StartScreenBlind( SBT_FIXED_BLIND, NULL, 0 );
	}
}

void ioFightClubMode::RemoveFightNPCRecord()
{
	if( m_FightNPCRecord.empty() )
		return;

	int iSize = m_FightNPCRecord.size();
	for( int i=0; i < iSize; ++i )
	{
		m_pCreator->RemoveUser( m_FightNPCRecord[i].szName, true );
	}
}

bool ioFightClubMode::IsEntityUpdateState()
{
	if( !ioPlayMode::IsEntityUpdateState() )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_ModeState )
	{
	case MS_NEW_CHALLENGER:
		if( g_MyInfo.GetPublicID() != m_NewChallengerName )
			return false;
		break;
	}

	return true;
}
