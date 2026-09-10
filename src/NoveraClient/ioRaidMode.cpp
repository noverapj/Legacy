
#include "stdafx.h"

#include "ioRaidMode.h"

#include "ioNpcChar.h"
#include "ioPlayStage.h"

#include "ioComplexStringPrinter.h"

#include "ioModeItemManager.h"
#include "ioModeItem.h"
#include "ioHunterCoinModeItem.h"
#include "ioShuffleBuffItem.h"

#include "ioSlotMgr.h"

#include "GUI/TowerDefWnd.h"
#include "GUI/BriefingWnd.h"
#include "GUI/MiniMap.h"
#include "GUI/CenterHelpWnd.h"
#include "GUI/RoundStateMainWnd.h"
#include "GUI/MiniSoldierSelectWnd.h"
#include "GUI/TotalResultMainWnd.h"
#include "GUI/PlazaNPCGaugeWnd.h"
#include "GUI/ioObserverWndManager.h"
#include "GUI/ioObserverWnd.h"
#include "GUI/LadderTeamWnd.h"

ioRaidMode::ioRaidMode( ioPlayStage *pCreator )
: ioPlayMode( pCreator )
{
	m_ModeType = MT_RAID;

	m_SingleTeamPosArray.clear();
	m_dwDropZoneMoveStartTime = 0;	

	m_vCharRecordList.reserve( 100 );

	m_szNPCName.Clear();
	m_TreasureCardRandomIndex = 0;
}

ioRaidMode::~ioRaidMode()
{
	ClearMode();
}

void ioRaidMode::LoadModeResource( ioINILoader &rkLoader )
{
	g_ModeUIController.SetKOMsgWnd( g_GUIMgr.AddWnd( "XML/kochatmsgwnd.xml") );
	g_ModeUIController.SetCenterInfoWnd( g_GUIMgr.AddWnd( "XML/CenterInfoWnd.xml" ) );
	g_ModeUIController.SetCenterKillInfoWnd( NULL );

	g_GUIMgr.AddWnd( "XML/TowerDefWnd.xml");

	MiniSoldierSelectWnd *pMiniSoldierWnd = dynamic_cast<MiniSoldierSelectWnd*>(g_GUIMgr.AddWnd( "XML/MiniSoldierSelectWnd.xml" ));
	if(pMiniSoldierWnd)
		pMiniSoldierWnd->SetPlayStage( m_pCreator );

	g_GUIMgr.AddWnd( "XML/SkillListWnd.xml" );
	g_GUIMgr.AddWnd( "XML/AccessorySkillListWnd.xml" );
	g_GUIMgr.AddWnd( "XML/plazamonstergaugewnd.xml");

	PlazaNPCGaugeWnd *pPlazaNPCGaugeWnd = dynamic_cast<PlazaNPCGaugeWnd *>( g_GUIMgr.FindWnd( PLAZA_NPC_GAUGE_WND ) );
	if(pPlazaNPCGaugeWnd)
		pPlazaNPCGaugeWnd->InitAniState();

	RaidReadyCountWnd *pRaidReadyCountWnd = dynamic_cast<RaidReadyCountWnd *>( g_GUIMgr.AddWnd( "XML/raidreadycountwnd.xml" ) );
	if( pRaidReadyCountWnd )
		pRaidReadyCountWnd->SetPlayMode( this );

	ioPlayMode::LoadModeResource( rkLoader );
}

void ioRaidMode::LoadMap()
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

	MiniMap *pMiniMap = dynamic_cast<MiniMap*>(g_GUIMgr.FindWnd( MINIMAP_WND ));
	if( pMiniMap )
		pMiniMap->SetTimeImage(kLoader);
}

void ioRaidMode::LoadContribute()
{
	ioPlayMode::LoadContribute();
}

void ioRaidMode::LoadCoinItemInfo( ioINILoader &rkLoader )
{
	rkLoader.SetTitle_e( "Coin_item" );
	
	char szBuf[MAX_PATH];
	rkLoader.LoadString_e( "generate_ment", "", szBuf, MAX_PATH );
	m_AddMonsterMent.m_szGenerateMent = szBuf;	
	m_AddMonsterMent.m_dwGenerateMentTime = rkLoader.LoadInt_e( "generate_ment_time", 3000 );

	char *szColor = NULL;
	rkLoader.LoadString_e( "generate_ment_color", "0xffffffff", szBuf, MAX_PATH );
	m_AddMonsterMent.m_dwGenerateColor = strtoul( szBuf, &szColor, 16 );

	m_AddMonsterMent.m_fGenerateScale  = rkLoader.LoadFloat_e( "generate_ment_scale", 0.83f );
}

void ioRaidMode::LoadBuffItemInfo( ioINILoader &rkLoader )
{
	rkLoader.SetTitle_e( "buff_item" );

	char szBuf[MAX_PATH];
	rkLoader.LoadString_e( "generate_ment", "", szBuf, MAX_PATH );
	m_AddBuffItemMent.m_szGenerateMent = szBuf;
	m_AddBuffItemMent.m_dwGenerateMentTime = rkLoader.LoadInt_e( "generate_ment_time", 3000 );

	char *szColor = NULL;
	rkLoader.LoadString_e( "generate_ment_color", "0xffffffff", szBuf, MAX_PATH );
	m_AddBuffItemMent.m_dwGenerateColor = strtoul( szBuf, &szColor, 16 );

	m_AddBuffItemMent.m_fGenerateScale  = rkLoader.LoadFloat_e( "generate_ment_scale", 0.83f );
}

void ioRaidMode::InitMode( int iMapIndex )
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

	LoadRevivalTime( kLoader );
	LoadModeResource( kLoader );
	LoadModeCamera( kLoader );
	LoadCoinItemInfo( kLoader );
	LoadBuffItemInfo( kLoader );

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

	ClearModeItem();
	
	m_szNPCName.Clear();
	m_LastDieChar.Clear();
	m_LastKillChar.Clear();

	kLoader.SetTitle_e( "TreasureCard" );
	m_TreasureCardRandomIndex = kLoader.LoadInt_e( "TreasureRandomIndex", 0 );
}

void ioRaidMode::ReStartMode()
{
	LOG.PrintTimeAndLog( 0, "ioRaidMode::ReStartMode Is Call???" );
}

void ioRaidMode::ProcessMode()
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
		UpdateUserGauge();
		ProcessMapAreaWeapon();
		ProcessPlayState();
		CheckModeMapSound();
		ProcessModeItem();
		ProcessNpcErase();
		g_EventMgr.Process();
		break;
	case MS_RESULT:
		ProcessResultState();
		break;
	}
}

void ioRaidMode::ProcessReadyState()
{
	if( P2PNetwork::IsNetworkPlaying() )	
		return;	

	if( m_dwReadyStartTime + 4000 > FRAMEGETTIME() )
		return;	
		
	SetModeState( MS_PLAY );
	
	m_dwDropZoneMoveStartTime = FRAMEGETTIME();
}

void ioRaidMode::ProcessPlayState()
{
	UpdateSoldierRevivalPenalty();

	m_pCreator->NotifySpringDampUseSet( true );

	ProcessRevival();
}

void ioRaidMode::ProcessResultState()
{
	ShuffleCenterHelpWnd* pHelpWnd = dynamic_cast<ShuffleCenterHelpWnd*>( g_GUIMgr.FindWnd( SHUFFLE_BONUS_CENTER_HELP_WND ) );
	if( pHelpWnd )
		pHelpWnd->HideWnd();
	
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

void ioRaidMode::ProcessGUI()
{
	UpdateMiniMapSpotPoint();
	UpdateRoundStateWnd();
	UpdateEndScoreWndTime();
	UpdateSoldierWndTime();
	UpdateNPCGaugeWnd();
	//UpdateObserverWnd();

	ProcessPushEnableCursor();
	ProcessBlindMode();
	ProcessAutoStartAlarm();
}

void ioRaidMode::ProcessRevival()
{
	DWORD dwCurTime = FRAMEGETTIME();
	
	int iCharCnt = m_vCharRecordList.size();
	for( int i=0 ; i<iCharCnt ; i++ )
	{
		RaidRecord &rkRecord = m_vCharRecordList[i];
		if( rkRecord.dwCurDieTime == 0 )
			continue;
		
		if( rkRecord.pChar == m_pCreator->GetOwnerChar() )
		{
			int iRevivalTime = (int)rkRecord.dwRevivalGap - ( dwCurTime - rkRecord.dwCurDieTime );
			if( iRevivalTime < 0 )
				iRevivalTime = 0;

			if( P2PNetwork::IsNetworkPlaying() )
			{
				if( rkRecord.dwCurDieTime + rkRecord.dwRevivalGap < dwCurTime )
				{
					rkRecord.dwCurDieTime = 0;
					SetViewRoundState( false );
				}
				else if( rkRecord.dwCurDieTime + rkRecord.dwRevivalGap < dwCurTime + 3000 )
				{
					SetViewRoundState( true );
				}
			}
		}
		
		if( !P2PNetwork::IsNetworkPlaying() )
		{
			if( rkRecord.dwCurDieTime + rkRecord.dwRevivalGap < dwCurTime )
			{
				rkRecord.iRevivalCnt++;
				rkRecord.dwCurDieTime = 0;

				ioBaseChar *pChar = rkRecord.pChar;

				D3DXVECTOR3 vNewPos = GetRevivalPos( pChar );
				pChar->SetWorldPosition( vNewPos );
				pChar->Revival();
				pChar->SetStartMotionState();
			}
		}
	}
}

void ioRaidMode::UpdateMiniMapSpotPoint()
{
	MiniMap *pMiniMap = dynamic_cast<MiniMap*>( g_GUIMgr.FindWnd( MINIMAP_WND ) );
	if( !pMiniMap )	return;

	ioPlayMode::UpdateMiniMapSpotPoint();

	pMiniMap->ClearPointList();

	if( m_ModeState == MS_RESULT)
		pMiniMap->UpdateTime(m_dwResultStartTime - m_dwPlayStartTime);
	else
		pMiniMap->UpdateTime(FRAMEGETTIME() - m_dwPlayStartTime);

	D3DXVECTOR3 vPos;
	DWORD dwGapTime = 0;

	// Other Char
	RecordList::iterator iter;
	for( iter=m_vCharRecordList.begin() ; iter!=m_vCharRecordList.end() ; ++iter )
	{
		ioBaseChar *pChar = iter->pChar;
		if( pChar->IsOwnerChar() || !pChar->IsCanMiniMapVisible() )
			continue;
		
		dwGapTime = pChar->GetEventGapTime();
		vPos = pChar->GetWorldPosition();

		if( ToNpcChar( pChar ) && pChar->GetCharName() == m_szNPCName )
		{
			pMiniMap->AddPoint( vPos.x, vPos.z, MiniMap::SPOT_SHUFFFLE_MONSTER, TEAM_NONE, dwGapTime );
		}
		else
		{
			if( pChar->IsDropZoneDownState() )
				pMiniMap->AddPoint( vPos.x, vPos.z, MiniMap::SPOT_DROP_USER, TEAM_BLUE, 0 );
			else
				pMiniMap->AddPoint( vPos.x, vPos.z, MiniMap::SPOT_USER, TEAM_BLUE, dwGapTime );
		}
	}

	// My Char
	ioBaseChar *pOwner = GetOwnerChar();
	if( pOwner && pOwner->IsCanMiniMapVisible() )
	{
		dwGapTime = pOwner->GetEventGapTime();
		vPos = pOwner->GetWorldPosition();

		if( pOwner->IsDropZoneDownState() )
			pMiniMap->AddPoint( vPos.x, vPos.z, MiniMap::SPOT_DROP_ME, TEAM_BLUE, 0 );
		else
			pMiniMap->AddPoint( vPos.x, vPos.z, MiniMap::SPOT_ME, TEAM_BLUE, dwGapTime );
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

	//Mode Item
	for( vModeItem::iterator iter = m_vModeItemList.begin(); iter != m_vModeItemList.end(); ++iter )
	{
		ioModeItem* pItem = *iter;
		if( !pItem )
			continue;
		
		if( !pItem->IsVisible() )
			continue;

		switch( pItem->GetTypeID() )				
		{
		case MIT_HUNTER_COIN:
			{
				pMiniMap->AddPoint( pItem->GetPosition().x, pItem->GetPosition().z, MiniMap::SPOT_STAR, TEAM_NONE, 0 );
			}
			break;
		case MIT_BUFF:
			{
				pMiniMap->AddPoint( pItem->GetPosition().x, pItem->GetPosition().z, MiniMap::SPOT_SHUFFLE_BUFFBOX, TEAM_NONE, 0 );
			}
			break;
		}
	}
}

void ioRaidMode::UpdateRoundStateWnd()
{
	if( g_MyInfo.IsSortContribute() )
	{
		DWORD dwTimeGap = FRAMEGETTIME() - m_dwPlayStartTime;
		if( m_dwCurRoundDuration > dwTimeGap )
			ContributeRoundAllPercent( m_dwRoundDuration - (m_dwCurRoundDuration - dwTimeGap), m_dwRoundDuration );
		else
			ContributeRoundAllPercent( m_dwRoundDuration, m_dwRoundDuration );

		RaidRecord *pRecord = FindRecordCharName( g_MyInfo.GetPublicID() );
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

			pRoundStateWnd->SetRaidCoinCountScore();
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

void ioRaidMode::UpdateNPCGaugeWnd()
{
	PlazaNPCGaugeWnd *pPlazaNPCGaugeWnd = dynamic_cast< PlazaNPCGaugeWnd * >( g_GUIMgr.FindWnd( PLAZA_NPC_GAUGE_WND ) );
	if( !pPlazaNPCGaugeWnd ) 
		return;

	// 보스 게이지
	if( pPlazaNPCGaugeWnd->IsShow() )
	{
		ioNpcChar *pNPCChar = ToNpcChar(GetRecordChar(m_szNPCName));
		if( pNPCChar )
		{	
			if( pNPCChar->GetState() == CS_DIE )
				pPlazaNPCGaugeWnd->HideWnd();
			else
			{
				StatusValue &rkStatus = pNPCChar->GetHP();
				pPlazaNPCGaugeWnd->UpdateRaidBossMonster( rkStatus.m_fCurValue, rkStatus.m_fMaxValue, GetCurRank( g_MyInfo.GetPublicID() ), (int)m_RoundInfoList.size() );
			}			
		}
		else
			pPlazaNPCGaugeWnd->HideWnd();
	}
}

void ioRaidMode::UpdateDropDieRecord( ioBaseChar *pDieChar, ioBaseChar *pKillChar, ioBaseChar *pBestAttacker )
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

			ioHashString szAttName = "";
			ioHashString szDieName = "";
			if( ToNpcChar(pAttRecord->pChar) )
				szAttName = ToNpcChar(pAttRecord->pChar)->GetViewName();
			else
				szAttName = pAttRecord->pChar->GetCharName();

			if( ToNpcChar(pDieRecord->pChar) )
			{
				szDieName = ToNpcChar(pDieRecord->pChar)->GetViewName();
				AddEraseNpc(ToNpcChar(pDieRecord->pChar)->GetCharName());
			}
			else
				szDieName = pDieRecord->pChar->GetCharName();

			if( pAttRecord->pChar->GetTeam() != pDieRecord->pChar->GetTeam() )
			{
				pAttRecord->AddKill( GetModeType(), fKillPoint, true, false );
				pDieRecord->AddDeath( GetModeType(), GetDeathPoint( pDieRecord->pChar->GetTeam() ) );
			}
			else
			{
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

void ioRaidMode::UpdateKillDeathRecord( ioPlayEntity *pDieEntity, ioPlayEntity *pKiller, ioPlayEntity *pBestAttacker )
{
	ioBaseChar *pKillerChar = ToBaseChar( pKiller );
	ioBaseChar *pDieChar = ToBaseChar( pDieEntity );
	if( pKillerChar && pDieChar )
	{
		if( ToNpcChar(pDieChar) )
		{			
			AddEraseNpc( ToNpcChar(pDieChar)->GetCharName() );
		}

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
			}
			else
			{
				if( pAttRecord->pChar == pDieRecord->pChar )	// 자기자신의 공격
				{
					pDieRecord->AddDeath( GetModeType(), GetDeathPoint( pDieRecord->pChar->GetTeam() ) );
				}
				else
				{
					pAttRecord->AddKill( GetModeType(), -fKillPoint, false, false );
					pDieRecord->AddDeath( GetModeType(), GetDeathPoint( pDieRecord->pChar->GetTeam() ) );
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

bool ioRaidMode::GetRoundInfo( int iIndex, RoundInfo &rkInfo )
{
	if( ioPlayMode::GetRoundInfo( iIndex, rkInfo ) )
	{
		RaidRecord *pRecord = (RaidRecord*)FindModeRecord( iIndex );
		if( !pRecord )	
			return false;

		if( !pRecord->pChar ) 
			return false;

		rkInfo.m_iContribute	= pRecord->Contribute.m_iContribute;
		rkInfo.m_iContributePer = pRecord->Contribute.m_iPrevContributePer;
		rkInfo.m_iHunterCoinCnt = pRecord->m_iHunterCoinCount;
		return true;
	}
	return false;
}

bool ioRaidMode::GetResultInfo( int iIndex, ResultInfo &rkInfo )
{
	if( ioPlayMode::GetResultInfo( iIndex, rkInfo ) )
	{
		RaidRecord *pRecord = (RaidRecord*)FindModeRecord( iIndex );
		if( !pRecord )	return false;
		if( !pRecord->pChar ) return false;

		rkInfo.m_iContribute    = pRecord->Contribute.m_iContribute;
		rkInfo.m_iContributePer = pRecord->Contribute.m_iPrevContributePer;
		rkInfo.m_iHunterCoinCnt	= pRecord->m_iHunterCoinCount;
		return true;
	}
	return false;
}

bool ioRaidMode::GetFinalInfo( int iIndex, FinalInfo &rkInfo )
{
	if( ioPlayMode::GetFinalInfo( iIndex, rkInfo ) )
	{
		RaidRecord *pRecord = (RaidRecord*)FindModeRecord( iIndex );
		if( !pRecord )	return false;
		if( !pRecord->pChar ) return false;

		rkInfo.m_iContribute	= pRecord->Contribute.m_iContribute;
		rkInfo.m_iContributePer = pRecord->Contribute.m_iPrevContributePer;
		rkInfo.m_iHunterCoinCnt	= pRecord->m_iHunterCoinCount;
		return true;
	}
	return false;
}

void ioRaidMode::SetRoundEndMotion()
{
	TowerDefWnd *pUserGauge = dynamic_cast<TowerDefWnd*>( g_GUIMgr.FindWnd( TOWERDEF_USER_GAUGE_WND ));
	if(pUserGauge && pUserGauge->IsShow())
		pUserGauge->HideWnd();

	PlazaNPCGaugeWnd *pPlazaNPCGaugeWnd = dynamic_cast< PlazaNPCGaugeWnd * >( g_GUIMgr.FindWnd( PLAZA_NPC_GAUGE_WND ) );
	if( pPlazaNPCGaugeWnd && pPlazaNPCGaugeWnd->IsShow() )
		pPlazaNPCGaugeWnd->HideWnd();

	ContributeRoundAllPercent( 0, 0 );
			
	int iCharCnt = GetRecordCharCnt();	
	for( int i=0; i<iCharCnt; i++ )
	{
		ioBaseChar *pChar = GetRecordChar( i );
		if( !pChar ) 
			continue;

		if( pChar->GetState() == CS_OBSERVER ) 
			continue;

		if( g_BattleRoomMgr.IsObserverUser( pChar->GetCharName() ) ) 
			continue;

		pChar->SetRoundEndMotion( RM_WIN );
		pChar->StartNoInputDelayTime( 10000 );
	}

	if( !g_BattleRoomMgr.IsObserverUser( g_MyInfo.GetPublicID() ) && !m_pCreator->IsStealthMode() )
	{
		g_QuestMgr.QuestCompleteTerm( QC_PVE_ROUND_CLEAR, 1, g_BattleRoomMgr.GetModeStartPlayerCount(), (float)(FRAMEGETTIME() - m_dwPlayStartTime)/1000 );
		g_QuestMgr.QuestOccurTerm( QO_PVE_ROUND_CLEAR, 1 );
	}
}

void ioRaidMode::SetRoundEndVictories()
{
	int iCharCnt = GetRecordCharCnt();	
	for( int i=0; i<iCharCnt; i++ )
	{
		RaidRecord *pRecord = &m_vCharRecordList[i];
		if( !pRecord->pChar ) continue;

		// 기여도 100%이상이 승리
		g_VictoriesMgr.SetVictories( pRecord->szName, ( pRecord->Contribute.m_iPrevContributePer >= 100 ) );
	}	
}

void ioRaidMode::SetResultScoreWnd()
{	
	// 중간 결과가 존재하지 않는다.
}

void ioRaidMode::SetTotalScoreWnd( IntVec &rvClassTypeList, IntVec &rvClassPointList, float fBlueGuildTeamBonus, float fRedGuildTeamBonus, float fBonusArray[BA_MAX] )
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
		}
	}

	pTotalResult->SetGuildTeamBonus( fBlueGuildTeamBonus, fRedGuildTeamBonus );
	pTotalResult->SetFinalRoundPoint( rvClassTypeList, rvClassPointList, fBonusArray );
	pTotalResult->SetSingleScore();

	ModeRecord *pRecord = FindModeRecordCharName( g_MyInfo.GetPublicID() );
	if( pRecord )
		pTotalResult->SetStartRaidTreasure( pRecord->iResultTreasureCardCount );

	m_dwCurResultShowDuration = m_dwFinalRoundResultDuration;

	SetTotalResultBriefingWnd();
}

void ioRaidMode::SetResultBriefingWnd()
{
	// 없음
}

void ioRaidMode::SetTotalResultBriefingWnd()
{
	// 개임 모드 최종 결과 브리핑
	ioBaseChar *pOwner = GetOwnerChar();
	if( pOwner )
	{
		BriefingWnd *pBriefingWnd = dynamic_cast<BriefingWnd*>(g_GUIMgr.FindWnd( BRIEFING_WND ));
		if( pBriefingWnd && !pBriefingWnd->IsShow() )
		{
			D3DXVECTOR3 vNewPos = ioMath::VEC3_ZERO;
			if( IsNeedUpdateMapEffect( pOwner ) )
				vNewPos = pOwner->GetWorldPosition();
			
			pBriefingWnd->SetResultInfo( m_pCreator, BriefingWnd::BRIEFING_FINAL_RESULT, BriefingWnd::RESULT_WIN, pOwner->GetTeam(), m_iCurRoundCount );


			RaidRecord* pRecord = FindRecordCharName( pOwner->GetCharName() );
			if( pRecord )
			{
				pBriefingWnd->SetHunterCoinCount( pRecord->m_iHunterCoinCount );
			}

			// 맵 이펙트 작렬!
			ioHashString szMapEffect = pBriefingWnd->GetFinalResultMapEffect( TEAM_BLUE );

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

void ioRaidMode::RenderPreGUI()
{
	if( m_ModeState == MS_PLAY )
	{
		RenderPushEnableCursor();
	}
}

D3DXVECTOR3 ioRaidMode::GetRevivalPos( ioBaseChar *pChar )
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
	vCenter.y = GetMapHeight( vCenter.x, vCenter.z );

	return vCenter;
}

void ioRaidMode::NotifyEntityDie( ioPlayEntity *pEntity, ioPlayEntity *pKiller, ioBaseChar *pBestAttacker, float fLastRate, float fBestRate )
{
	ioBaseChar *pDieChar = ToBaseChar( pEntity );
	ioBaseChar *pKillChar = ToBaseChar( pKiller );

	OnNotifyCharDie( pDieChar, pKillChar, pBestAttacker, fLastRate, fBestRate );
	UpdateKillDeathRecord( pEntity, pKiller, pBestAttacker );
}

void ioRaidMode::NotifyCharDropDie( ioBaseChar *pDieChar, ioBaseChar *pKillChar, ioBaseChar *pBestAttacker, float fLastRate, float fBestRate )
{
	if( !pDieChar )
		return;

	OnNotifyCharDie( pDieChar, pKillChar, pBestAttacker, fLastRate, fBestRate );
	UpdateDropDieRecord( pDieChar, pKillChar, pBestAttacker );	
}

void ioRaidMode::NotifyCharDrop( ioPlayEntity *pEntity, float fDropDamage )
{
	ioBaseChar *pChar = ToBaseChar( pEntity );

	if( !pChar )
		return;
	
	if( !pChar->IsNeedProcess() )
		return;
	
	SP2Packet kPacket( CTPK_SHUFFLEROOM_DROPZONE );
	kPacket << SHUFFLEROOM_DROP_USER;
	kPacket << pChar->GetCharName().c_str();	
	TCPNetwork::SendToServer( kPacket );
}

void ioRaidMode::OnNotifyCharDie( ioBaseChar *pDieChar, ioBaseChar *pKillChar, ioBaseChar *pBestAttacker, float fLastRate, float fBestRate )
{
	if( !pDieChar )
	{
		LOG.PrintTimeAndLog( 0, "ioRaidMode::OnNotifyCharDie() - DieChar is NULL" );
		return;
	}

	RaidRecord *pDieRecord = FindRecordCharName( pDieChar->GetCharName() );
	if( !pDieRecord )
	{
		LOG.PrintTimeAndLog( 0, "ioRaidMode::OnNotifyCharDie - %s Not Exit Char",
								pDieChar->GetCharName().c_str() );
		return;
	}

	if( pKillChar && pKillChar->GetTeam() != pDieChar->GetTeam() )
	{
		if( pBestAttacker && pBestAttacker->GetCharName() == pKillChar->GetCharName() )
		{
			pKillChar->SetKOHPRecover( m_fKORecoverRate * 2.0f, m_PerfectRecoverEffect );
			
			if( m_pCreator && m_pCreator->CheckRoomKOEffect() )
			{
				SetRecoveryGauge( pKillChar, fLastRate );				
			}
		}
		else
		{
			pKillChar->SetKOHPRecover( m_fKORecoverRate, m_KORecoverEffect );
			
			if( m_pCreator && m_pCreator->CheckRoomKOEffect() )
			{
				SetRecoveryGauge( pKillChar, fLastRate );				
			}

			if( pBestAttacker )
			{
				pBestAttacker->SetKOHPRecover( m_fKORecoverRate, m_KORecoverEffect );
				
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

	m_LastDieChar = pDieChar->GetCharName();

	if( pKillChar )
	{
		RaidRecord *pKillRecord = FindRecordCharName( pKillChar->GetCharName() );
		if( !pKillRecord )
			m_LastKillChar.Clear();
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

bool ioRaidMode::ProcessTCPPacket( SP2Packet &rkPacket )
{
	if( ioPlayMode::ProcessTCPPacket( rkPacket ) )
		return true;

	switch( rkPacket.GetPacketID() )
	{
	case STPK_SHUFFLEROOM_DROPZONE:
		OnDropZone( rkPacket );
		return true;		
	case STPK_SPAWN_MONSTER:
		OnSpawnMonster(rkPacket);
		return true;
	case STPK_SLOT_BUFF:
		OnUseConsumption(rkPacket);
		return true;
	case STPK_RAID_PHASE_INFO:
		OnRaidPhaseInfo(rkPacket);
		return true;
	case STPK_TREASURE_CARD_COMMAND:
		OnTreasureCardCommand( rkPacket );
		return true;
	}

	return false;
}

void ioRaidMode::AddNewRecord( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	RaidRecord *pRaidRecord = FindRecordCharName(pChar->GetCharName());
	if( !pRaidRecord )
	{
		RaidRecord kRecord;
		kRecord.pChar  = pChar;
		kRecord.szName = pChar->GetCharName();
		m_vCharRecordList.push_back( kRecord );		

		UpdateUserRank();

		if( !ToNpcChar( pChar ) && !g_BattleRoomMgr.IsObserverUser(pChar->GetCharName()) )
			g_AwardMgr.AddNewAwardUser( pChar->GetCharName() );
	}
	else
	{
		pRaidRecord->pChar = pChar;
	}
}

void ioRaidMode::RemoveRecord( const ioHashString &rkName )
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

void ioRaidMode::RemoveRecord( int nIndex )
{
	int iCharCnt = m_vCharRecordList.size();
	if( iCharCnt > nIndex )
	{
		m_vCharRecordList.erase( m_vCharRecordList.begin() + nIndex );
		UpdateUserRank();
	}
}

ioBaseChar* ioRaidMode::GetRecordChar( int i )
{
	if( COMPARE( i, 0, GetRecordCharCnt() ) )
		return m_vCharRecordList[i].pChar;

	return NULL;
}

ioBaseChar* ioRaidMode::GetRecordChar( const ioHashString &rkName )
{
	RaidRecord *pRecord = FindRecordCharName( rkName );
	if( pRecord )
		return pRecord->pChar;

	return NULL;
}

int ioRaidMode::GetRecordCharCnt() const
{
	return m_vCharRecordList.size();
}

ModeRecord* ioRaidMode::FindModeRecord( int iIdx )
{
	if( COMPARE( iIdx, 0, GetRecordCharCnt() ) )
		return &m_vCharRecordList[iIdx];

	return NULL;
}

ModeRecord* ioRaidMode::FindModeRecordCharName( const ioHashString &rkName )
{
	return (ModeRecord*)FindRecordCharName( rkName );
}

RaidRecord* ioRaidMode::FindRecordCharName( const ioHashString &rkName )
{
	int iCharCnt = m_vCharRecordList.size();
	for( int i=0 ; i<iCharCnt ; i++ )
	{
		if( m_vCharRecordList[i].szName == rkName )
			return &m_vCharRecordList[i];
	}

	return NULL;
}

void ioRaidMode::FindBaseCharRecordNameList( ioHashStringVec &rkRecordNameList )
{
	rkRecordNameList.clear();
	int iCharCnt = m_vCharRecordList.size();
	for( int i=0 ; i<iCharCnt ; i++ )
	{
		RaidRecord &rkRecord = m_vCharRecordList[i];
		if( rkRecord.pChar == NULL ) continue;
		if( ToNpcChar( rkRecord.pChar ) ) continue;

		rkRecordNameList.push_back( rkRecord.szName );
	}
}

bool ioRaidMode::ProcessUDPPacket( SP2Packet &rkPacket )
{
	if( ioPlayMode::ProcessUDPPacket( rkPacket ) )
		return true;

	return false;
}

void ioRaidMode::ApplyModeInfo( SP2Packet &rkPacket )
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

void ioRaidMode::ApplyCharModeInfo( const ioHashString &rkName, SP2Packet &rkPacket, bool bDieCheck )
{
	bool bRecord;
	rkPacket >> bRecord;

	if( !bRecord ) return;

	RaidRecord *pRecord = FindRecordCharName( rkName );
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

void ioRaidMode::OnRoundJoin( SP2Packet &rkPacket )
{
	ioPlayMode::OnRoundJoin(rkPacket);

	if(!g_BattleRoomMgr.IsObserverUser(g_MyInfo.GetPublicID()))
		ShowUserGauge();
}

void ioRaidMode::OnRoundEnd( SP2Packet &rkPacket )
{
	ioPlayMode::OnRoundEnd( rkPacket );
}

void ioRaidMode::OnFinalRoundResult( SP2Packet &rkPacket )
{
	// 최종 결과 패킷을 받을 때 시상식 Show함 . 내부적으로 수상자가 없으면 Hide시킨다.
	g_GUIMgr.HideWnd( BRIEFING_WND );

	DWORD dwTotalResultTime;
	rkPacket >> dwTotalResultTime;
	SetAutoStartAlarmTime( dwTotalResultTime );

	float fBlueGuildTeamBonus = FLOAT1;
	float fRedGuildTeamBonus  = FLOAT1;

	g_MyInfo.ApplyRoundEndInfo( rkPacket );
	
	IntVec vClassTypeList;
	IntVec vClassPointList;
	
	// 클래스 경험치
	int i = 0;
	int iCount;
	rkPacket >> iCount;
	for( i=0;i < iCount;i++ )
	{
		int iClassType, iClassPoint;
		rkPacket >> iClassType >> iClassPoint;
		if( iClassType == 0 )
			continue;

		vClassTypeList.push_back( iClassType );
		vClassPointList.push_back( iClassPoint );

		g_MyInfo.AddClassExp( iClassType, iClassPoint );
		float fGradeExp = g_MyInfo.GetSoldierExpBonusByGradeExp( iClassType, iClassPoint );
		g_MyInfo.AddGradeExp( fGradeExp );
	}	
	g_MyInfo.GetFinalResultChar( vClassTypeList );

	// 내 정보
	__int64 iMyMoney;
	rkPacket >> iMyMoney;
	g_MyInfo.SetMoney( iMyMoney );

	float fBonusArray[ioPlayMode::BA_MAX];
	for (i = 0; i < ioPlayMode::BA_MAX; i++)
		rkPacket >> fBonusArray[i];

	// 플레이 유저 결과 정보
	int iPlayUserCnt;
	rkPacket >> iPlayUserCnt;

	for(i = 0;i < iPlayUserCnt;i++)
	{
		bool bLevelUP;
		ioHashString szName;
		int iTotalExp, iTotalPeso, iGradeLevel, iLadderPoint, iResultLadderPoint;
		rkPacket >> szName >> iTotalExp >> iTotalPeso >> bLevelUP >> iGradeLevel >> iLadderPoint >> iResultLadderPoint;

		ModeRecord *pRecord = FindModeRecordCharName( szName );
		if( pRecord )
		{
			pRecord->bResultLevelUP          = bLevelUP;
			pRecord->iResultLevel            = iGradeLevel;
			pRecord->iResultTotalExp         = iTotalExp;
			pRecord->iResultTotalPeso        = iTotalPeso;
			pRecord->iResultTotalLadderPoint = iResultLadderPoint;
			g_UserInfoMgr.SetLadderPoint( szName, iLadderPoint );
		}
		else
		{
			LOG.PrintTimeAndLog( 0, "ioPlayMode::OnFinalRoundResult - Not Exist Record: %s", szName.c_str() );
		}
	}

	//이놈 대신 해서 랭크 표시를 뛰울 예정
	SetTotalScoreWnd( vClassTypeList, vClassPointList, fBlueGuildTeamBonus, fRedGuildTeamBonus, fBonusArray );
	
	bool bCheckNext;
	rkPacket >> bCheckNext;

	int iNextModeType   = 0;
	int iNextModeSubNum = 0;
	int iNextMapIndex   = 0;

	if( bCheckNext )
	{
		rkPacket >> iNextModeType;
		rkPacket >> iNextModeSubNum;
		rkPacket >> iNextMapIndex;
	}

	if( m_pCreator )
	{
		m_pCreator->ReserveNextModeType( (ModeType)iNextModeType, iNextModeSubNum, iNextMapIndex );
	}
}

void ioRaidMode::OnFinalResultUserInfo( SP2Packet &rkPacket )
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

int ioRaidMode::GetRoundWinCnt( TeamType eTeamType )
{
	return 0;
}

void ioRaidMode::ContributeConvertTeam( TeamType eTeam )
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
		RaidRecord *pRecord = FindRecordCharName( kCharReocrdNameList[i] );
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
			RaidRecord *pRecord = FindRecordCharName( kCharReocrdNameList[iArray] );
			if( !pRecord->Contribute.m_bLastResultSync )
				pRecord->Contribute.m_iContribute = m_vContributeTable[vInfo[i].m_iRank];
			pRecord->Contribute.m_iKillRank = vInfo[i].m_iRank;
		}
	}
	vInfo.clear();

	//데미지 & 힐
	for(i = 0;i < iCharCnt;i++)
	{
		RaidRecord *pRecord = FindRecordCharName( kCharReocrdNameList[i] );
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
			RaidRecord *pRecord = FindRecordCharName( kCharReocrdNameList[iArray] );
			if( !pRecord->Contribute.m_bLastResultSync )
				pRecord->Contribute.m_iContribute += m_vContributeTable[vInfo[i].m_iRank] * 6;
			pRecord->Contribute.m_iDamageRank = vInfo[i].m_iRank;
		}
	}
	vInfo.clear();

	//유효 타격 횟수
	for(i = 0;i < iCharCnt;i++)
	{
		RaidRecord *pRecord = FindRecordCharName( kCharReocrdNameList[i] );
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
			RaidRecord *pRecord = FindRecordCharName( kCharReocrdNameList[iArray] );
			if( !pRecord->Contribute.m_bLastResultSync )
				pRecord->Contribute.m_iContribute += m_vContributeTable[vInfo[i].m_iRank] * 2;
			pRecord->Contribute.m_iAttackCntRank = vInfo[i].m_iRank;
		}
	}
	vInfo.clear();

	//데쓰처리
	for(i = 0;i < iCharCnt;i++)
	{
		RaidRecord *pRecord = FindRecordCharName( kCharReocrdNameList[i] );
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
			RaidRecord *pRecord = FindRecordCharName( kCharReocrdNameList[iArray] );
			if( !pRecord->Contribute.m_bLastResultSync )
				pRecord->Contribute.m_iContribute += m_vContributeTable[vInfo[i].m_iRank];
			pRecord->Contribute.m_iDeathRank = vInfo[i].m_iRank;
		}
	}
	vInfo.clear();
	kCharReocrdNameList.clear();
}

void ioRaidMode::ContributeRoundAllPercent( DWORD dwCurTime, DWORD dwTotalTime )
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
		RaidRecord *pRecord = FindRecordCharName( kCharReocrdNameList[i] );
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
			RaidRecord *pRecord = FindRecordCharName( kCharReocrdNameList[i] );
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

void ioRaidMode::ContributeConvert()
{
	ContributeConvertTeam( TEAM_NONE );
}

int ioRaidMode::ContributeMyRank( int &rRankValue1, int &rRankValue2, int &rRankValue3, int &rRankValue4 )
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
		RaidRecord *pRecord = FindRecordCharName( kCharReocrdNameList[i] );
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
		RaidRecord *pRecord = FindRecordCharName( kCharReocrdNameList[i] );
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
		RaidRecord *pRecord = FindRecordCharName( kCharReocrdNameList[i] );
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

void ioRaidMode::DamageContribute( const ioHashString szAttacker, const ioHashString szTarget, float fDamage, DWORD dwItemCode, int iWeaponType )
{
	if( szAttacker.IsEmpty() ) return;

	RaidRecord *pRecord = FindRecordCharName( szAttacker );
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

void ioRaidMode::KillContribute( const ioHashString szKiller, const ioHashString szKilled, DWORD dwItemCode )
{
	if( szKilled.IsEmpty() ) return;

	int iKillPoint = 100;

	float fAddKillPoint = FLOAT1;
	if( IsContributeAddPointItemCode( dwItemCode ) )
		fAddKillPoint = 1.1f;

	if( szKiller.IsEmpty() || szKiller == szKilled )     //자살
	{
		RaidRecord *pRecord = FindRecordCharName( szKilled );
		if( !pRecord || !pRecord->pChar ) return;

		pRecord->Contribute.m_iKill -= iKillPoint;
	}
	else
	{
		RaidRecord *pKillerRecord = FindRecordCharName( szKiller );
		RaidRecord *pKilledRecord = FindRecordCharName( szKilled );
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

void ioRaidMode::AttackCntContribute( const ioHashString szAttacker, const ioHashString szTarget, DWORD dwItemCode, int iAttackValue/* = 100 */ )
{
	if( szAttacker.IsEmpty() || szTarget.IsEmpty() ) return;

	RaidRecord *pAttackerRecord = FindRecordCharName( szAttacker );
	RaidRecord *pTargetRecord = FindRecordCharName( szTarget );

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

void ioRaidMode::ContributeLOG()
{
	char szLog[MAX_PATH];

	int iCharCnt = m_vCharRecordList.size();
	for( int i=0 ;i<iCharCnt; i++ )
	{
		RaidRecord *pRecord = &m_vCharRecordList[i];
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

void ioRaidMode::FillLastPlayRecordInfo( SP2Packet &rkPacket )
{
	ioHashStringVec kCharReocrdNameList;
	FindBaseCharRecordNameList( kCharReocrdNameList );
	int iCharCnt = kCharReocrdNameList.size();
	rkPacket << iCharCnt;
	for(int i = 0;i < iCharCnt;i++)
	{
		RaidRecord *pRecord = FindRecordCharName( kCharReocrdNameList[i] );
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

void ioRaidMode::FillPlayRecordInfo( SP2Packet &rkPacket )
{
	ioHashStringVec kCharReocrdNameList;
	FindBaseCharRecordNameList( kCharReocrdNameList );
	int iCharCnt = kCharReocrdNameList.size();
	rkPacket << iCharCnt;
	for(int i = 0;i < iCharCnt;i++)
	{
		RaidRecord *pRecord = FindRecordCharName( kCharReocrdNameList[i] );
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

void ioRaidMode::ApplyPlayRecordInfo( SP2Packet &rkPacket )
{
	int iCharCnt;
	rkPacket >> iCharCnt;
	for(int i = 0;i < iCharCnt;i++)
	{
		ioHashString szName;
		int iDamage, iAttackCnt, iKill, iDeath, iPrevContributePer, iUniqueTotalKill, iUniqueTotalDeath, iVictories;
		rkPacket >> szName >> iDamage >> iAttackCnt >> iKill >> iDeath >> iPrevContributePer >> iUniqueTotalKill >> iUniqueTotalDeath >> iVictories;

		RaidRecord *pRecord = FindRecordCharName( szName );
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

void ioRaidMode::ContributeResultSync( const ioHashString &szName, int iContribute )
{
	RaidRecord *pRecord = FindRecordCharName( szName );
	if( !pRecord ) return;

	pRecord->Contribute.m_bLastResultSync = true;
	pRecord->Contribute.m_iContribute     = iContribute;
}

void ioRaidMode::ShowEndScoreWnd( bool bShow )
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

D3DXVECTOR3 ioRaidMode::GetStartPos( TeamType eTeamType, bool bRandom )
{
	int iPosArray = -1;
	int iTestTeam = (int) eTeamType;

	int iSinglePosCnt = m_SingleTeamPosArray.size();
	int iStartPosCnt = m_vStartPos.size();
	
	if( eTeamType == TEAM_BLUE  )
	{
		iPosArray = m_iBluePosArray;
	}
	else if(eTeamType == TEAM_RED )
	{
		iPosArray = m_iRedPosArray;
	}
	else if( iStartPosCnt > 0 && iSinglePosCnt > 0 )
	{
		int iTeamNum = (int)eTeamType;
		iTeamNum -= 3;
		iTeamNum = max( 0, iTeamNum );

		iTeamNum = iTeamNum % iSinglePosCnt;
		iPosArray = m_SingleTeamPosArray[iTeamNum];
	}

	if( m_vStartPos.empty() || m_SingleTeamPosArray.empty() )
		return m_vMapCenter;

	int iMaxStartPos = m_vStartPos.size();
	iPosArray = iPosArray % iMaxStartPos;

	D3DXVECTOR3 vCenter = m_vStartPos[iPosArray];
	D3DXQUATERNION qtRot;
	D3DXQuaternionRotationYawPitchRoll( &qtRot,
										ioMath::RangeRandom( 0.0f, D3DX_PI * 2.0f ),
										0.0f,
										0.0f );

	D3DXVECTOR3 vMoveDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, FLOAT1 );
    
	if( bRandom )
		vCenter += vMoveDir * ioMath::RangeRandom( FLOAT10, m_fRandomRange );
	
	vCenter.y = GetMapHeight( vCenter.x, vCenter.z );
	return vCenter;
}

void ioRaidMode::UpdateRoundInfoList()
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
	for( iter=m_RoundInfoList.begin() ; iter!= m_RoundInfoList.end() ; ++iter, ++iRank )
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

int ioRaidMode::GetCurRank( const ioHashString& szCharName )
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

int ioRaidMode::GetContributePerChar( const ioHashString& szName )
{
	int iCharCnt = m_vCharRecordList.size();

	for(int i = 0;i < iCharCnt;i++)
	{
		RaidRecord *pRecord = &m_vCharRecordList[i];
		if( !pRecord )	continue;
		if( !pRecord->pChar ) continue;

		if( pRecord->pChar->GetCharName() == szName )
		{
			return pRecord->Contribute.m_iPrevContributePer;
		}
	}

	return 0;
}

void ioRaidMode::CheckEndSceneLookAt()
{
	ioBaseChar *pOwner = m_pCreator->GetOwnerChar();

	if( !m_LastKillChar.IsEmpty() )
	{
		RaidRecord *pKillRecord = FindRecordCharName( m_LastKillChar );
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
			RaidRecord *pDieRecord = FindRecordCharName( m_LastDieChar );
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

ioBaseChar* ioRaidMode::CheckMostNearChar( ioBaseChar *pChar )
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

void ioRaidMode::OnCreateModeItem( SP2Packet &rkPacket )
{
	int iType;
	rkPacket >> iType;

	switch( iType )
	{
	case MIT_HUNTER_COIN:		
		OnCreateModeCoinItem( rkPacket );
		break;
	case MIT_BUFF:		
		OnCreateModeBuffItem( rkPacket );
		break;
	}	
}

void ioRaidMode::OnCreateModeCoinItem( SP2Packet &rkPacket )
{
	int iCnt;
	rkPacket >> iCnt;

	DWORD dwStartWaitTime;
	rkPacket >> dwStartWaitTime;

	for( int i = 0; i< iCnt; ++i )
	{
		DWORD dwIndex;
		rkPacket >> dwIndex;

		float fXPos, fZPos;
		rkPacket >> fXPos >> fZPos;

		int iAngle;
		rkPacket >> iAngle;

		float fSpeed, fPower;
		rkPacket >> fSpeed >> fPower;

		DWORD dwActivateTime;
		rkPacket >> dwActivateTime;
				
		ioHunterCoinModeItem *pItem = ToHunterCoinModeItem( g_ModeItemManager.CreateModeItem( MIT_HUNTER_COIN ) );
		if( pItem )
		{
			pItem->SetPlayStage( m_pCreator );
			pItem->SetItemValue( dwIndex, fXPos, fZPos, iAngle, fSpeed, fPower, dwStartWaitTime + dwActivateTime );

			m_vModeItemList.push_back( pItem );
		}
	}
	
	ShuffleCenterHelpWnd* pWnd = dynamic_cast<ShuffleCenterHelpWnd*>( g_GUIMgr.FindWnd( SHUFFLE_BONUS_CENTER_HELP_WND ) );
	if( pWnd )
	{			
		pWnd->SetCenterHelp( ShuffleCenterHelpWnd::ST_ADD_STAR, m_AddMonsterMent.m_szGenerateMent, m_AddMonsterMent.m_fGenerateScale, m_AddMonsterMent.m_dwGenerateColor, 0, m_AddMonsterMent.m_dwGenerateMentTime );
	}		
}

void ioRaidMode::OnCreateModeBuffItem( SP2Packet &rkPacket )
{
	DWORD dwActivateTime = 0;
	rkPacket >> dwActivateTime;

	int iCount = 0;
	rkPacket >> iCount;

	for( int i = 0; i < iCount; ++i )
	{
		DWORD dwIndex = 0;
		rkPacket >> dwIndex;

		float fXPos, fZPos;
		rkPacket >> fXPos >> fZPos;		

		ioModeItem* pItem = g_ModeItemManager.CreateModeItem( MIT_BUFF );
		ioShuffleBuffItem *pBuffItem = ToShuffleBuffItem( pItem );
		if( pBuffItem )
		{
			pBuffItem->SetPlayStage( m_pCreator );			
			pBuffItem->SetItemValue( dwIndex, fXPos, fZPos, dwActivateTime );
			m_vModeItemList.push_back( pBuffItem );
						
			ShuffleCenterHelpWnd* pWnd = dynamic_cast<ShuffleCenterHelpWnd*>( g_GUIMgr.FindWnd( SHUFFLE_BONUS_CENTER_HELP_WND ) );
			if( pWnd )
			{				
				pWnd->SetCenterHelp( ShuffleCenterHelpWnd::ST_ADD_BUFF_BOX, m_AddBuffItemMent.m_szGenerateMent, m_AddBuffItemMent.m_fGenerateScale, m_AddBuffItemMent.m_dwGenerateColor, 0, m_AddBuffItemMent.m_dwGenerateMentTime );
			}
		}
	}
}

void ioRaidMode::OnGetModeItem( SP2Packet &rkPacket )
{
	ioHashString szName;
	DWORD dwUserIdx, dwModeItemIndex;
	int iType;
	rkPacket >> dwUserIdx >> szName >> iType >> dwModeItemIndex;
	
	switch( iType )
	{
	case MIT_HUNTER_COIN:
		{
			int iCoinCount;
			rkPacket >> iCoinCount;
			OnGetModeItemByCoin( dwUserIdx, iCoinCount, szName );
		}
		break;
	case MIT_BUFF:
		{
			DWORD dwCallBuff = 0;
			rkPacket >> dwCallBuff;
			OnGetModeItemByBuff( szName, dwModeItemIndex, dwCallBuff );
		}
		break;
	}	

	ioModeItem* pItem = FindModeItem( dwModeItemIndex );
	if( pItem )
		pItem->SetReserveDestroy();
}

void ioRaidMode::OnGetModeItemByCoin( DWORD dwUserIdx, int iCoinCount, const ioHashString& szName )
{
	for( int i = 0; i< GetRecordCharCnt(); ++i )
	{
		RaidRecord *pRecord = (RaidRecord*)FindModeRecord( i );
		if( pRecord && pRecord->szName == szName )
			pRecord->m_iHunterCoinCount = iCoinCount;
	}
}

void ioRaidMode::OnGetModeItemByBuff( const ioHashString& szName, DWORD dwModeItemIndex, DWORD dwCallBuff )
{
	ioBaseChar* pOwner = NULL;
	for( int i = 0; i< GetRecordCharCnt(); ++i )
	{
		RaidRecord *pRecord = (RaidRecord*)FindModeRecord( i );
		if( pRecord && pRecord->szName == szName )
		{
			pOwner = pRecord->pChar;
			break;
		}
	}

	ioShuffleBuffItem* pItem = ToShuffleBuffItem( FindModeItem( dwModeItemIndex ) );
	if( pItem && m_pCreator )
	{
		const ioShuffleBuffItem::CallBuffInfo* pInfo = pItem->OnBuffCall( pOwner, m_pCreator->GetBaseCharList(), dwCallBuff );
		ShuffleCenterHelpWnd* pWnd = dynamic_cast<ShuffleCenterHelpWnd*>( g_GUIMgr.FindWnd( SHUFFLE_BONUS_CENTER_HELP_WND ) );
		if( pWnd && pInfo )
		{
			pWnd->SetCenterHelpGetBuffItem( ShuffleCenterHelpWnd::ST_GET_BUFF_BOX, 
											szName.c_str(),
											pInfo->m_CallBuffMentInfo.m_szGenerateMent,
											pInfo->m_szCallBuffIcon,
											pInfo->m_CallBuffMentInfo.m_fGenerateScale,
											pInfo->m_CallBuffMentInfo.m_dwGenerateColor,
											0,
											pInfo->m_CallBuffMentInfo.m_dwGenerateMentTime );
		}
	}
}

void ioRaidMode::ProcessModeItem()
{
	ioPlayMode::ProcessModeItem();

	if( !g_MyInfo.IsHost() )
		return;

	//주기마다 체크
	if( FRAMEGETTIME() < m_dwDropZoneMoveStartTime + 1000 )
		return;

	DWORDVec dwDropVec;
	vModeItem_iter iter = m_vModeItemList.begin();
	for( ; iter != m_vModeItemList.end(); ++iter )
	{
		ioModeItem *pItem = *iter;
		ioHunterCoinModeItem *pRaidCoinModeItem = ToHunterCoinModeItem( pItem );
		if( !pRaidCoinModeItem )
			continue;

		if( pRaidCoinModeItem->GetPosition().y - pRaidCoinModeItem->GetHeightOffset() >= 0.1f )
			continue;

		dwDropVec.push_back( pRaidCoinModeItem->GetUniqueID() );		
	}
	
	if( dwDropVec.empty() )
		return;

	SP2Packet kPacket( CTPK_SHUFFLEROOM_DROPZONE );
	kPacket << SHUFFLEROOM_DROP_STAR;
	kPacket << (int)dwDropVec.size();
	for( int i = 0; i < (int)dwDropVec.size(); ++i )
	{
		kPacket << dwDropVec[i];		
	}
	TCPNetwork::SendToServer( kPacket );

	m_dwDropZoneMoveStartTime = FRAMEGETTIME();
}

void ioRaidMode::OnDropZone( SP2Packet &rkPacket )
{
	int iDropType = 0;
	rkPacket >> iDropType;

	switch( iDropType )
	{
	case SHUFFLEROOM_DROP_USER:
		{
			OnDropZoneByUser( rkPacket );
		}
		break;
	case SHUFFLEROOM_DROP_STAR:
		{
			OnDropZoneByCoin( rkPacket );
		}
		break;
	}	
}

void ioRaidMode::OnDropZoneByUser( SP2Packet &rkPacket )
{	
	int iRaidCount = 0;
	ioHashString szUserName;
	rkPacket >> szUserName >> iRaidCount;

	RaidRecord *pRecord = FindRecordCharName( szUserName );
	if( pRecord )
		pRecord->m_iHunterCoinCount = iRaidCount;
}

void ioRaidMode::OnDropZoneByCoin( SP2Packet &rkPacket )
{
	int iCount = 0;
	rkPacket >> iCount;

	for( int i = 0; i < iCount; ++i )
	{
		DWORD dwModeItemIdx = 0;
		rkPacket >> dwModeItemIdx;

		if( dwModeItemIdx == 0 )
			continue;

		ioModeItem* pItem = FindModeItem( dwModeItemIdx );
		if( pItem )
			pItem->SetReserveDestroy();
	}
}

void ioRaidMode::ProcessNpcErase()
{
	int iSize = m_NpcEraseList.size();
	for(int i = 0; i < iSize; ++i )
	{
		NpcEraseData &rkNpcData = m_NpcEraseList[i];

		// NPC 죽은 후 35초뒤에 삭제 : 한번에 삭제하면 렉이 있을 수 있으니 프레임마다 한개씩 삭제
		if( FRAMEGETTIME() - rkNpcData.m_dwEraseTimer > 10000 )         
		{
			m_pCreator->RemoveUser( rkNpcData.m_szEraseNpc, true );
			RemoveEraseNpc( rkNpcData.m_szEraseNpc );
			return;
		}
	}
}

void ioRaidMode::AddEraseNpc( const ioHashString &rkNpcName )
{
	NpcEraseData kEraseData;
	kEraseData.m_szEraseNpc   = rkNpcName;
	kEraseData.m_dwEraseTimer = FRAMEGETTIME();
	m_NpcEraseList.push_back( kEraseData );
}

void ioRaidMode::RemoveEraseNpc( const ioHashString &rkNpcName )
{
	int iSize = m_NpcEraseList.size();
	for( int i = 0; i < iSize; ++i )
	{
		NpcEraseData &rkNpcData = m_NpcEraseList[i];
		if( rkNpcData.m_szEraseNpc == rkNpcName )
		{
			m_NpcEraseList.erase( m_NpcEraseList.begin() + i );
			return;
		}
	}
}

ioNpcChar* ioRaidMode::AddNPC( DWORD dwNpcCode, const ioHashString& rkNpcName, const ioHashString& rkSyncName, TeamType eTeam, DWORD dwStartTime, float fStartXPos, float fStartZPos, int nGrowthLvl )
{
	ioNpcChar *pNpcChar = g_NpcMgr.CreateNpcChar( NPC_NOT_INDEX, dwNpcCode, m_pCreator, this );
	if( !pNpcChar )
		return NULL;

	pNpcChar->FillMaxHP();
	pNpcChar->SetCharName( rkNpcName );
	pNpcChar->SetSyncUser( rkSyncName );
	pNpcChar->SetTeam( eTeam );
	pNpcChar->StartAI( dwStartTime, fStartXPos, fStartZPos );
	pNpcChar->SetNpcGradeShow(true);
	pNpcChar->SetGradeUpRandomSeed( g_BattleRoomMgr.GetIndex() + rkNpcName.GetHashCode() );
	pNpcChar->ReSetGradeLevel( nGrowthLvl );
	pNpcChar->SetDisableGauge(true);

	AddNewRecord( pNpcChar );

	return pNpcChar;
}

void ioRaidMode::OnSpawnMonster(SP2Packet &rkPacket)
{
	int iNpc = 0;
	rkPacket >> iNpc;
	
	for(int i = 0; i < iNpc; ++i)
	{
		DWORD dwNpcCode, dwStartTime, dwAliveTime;
		float fStartXPos, fStartZPos;
		ioHashString kNpcName, kSyncName;
		int nTeam;
		int nGrowthLvl;		
		bool bGroupBoss = false;

		rkPacket >> dwNpcCode >> kNpcName >> kSyncName >> nTeam >> nGrowthLvl >> bGroupBoss >> dwStartTime >> fStartXPos >> fStartZPos >> dwAliveTime;

		ioNpcChar *pNpcChar = AddNPC( dwNpcCode, kNpcName, kSyncName, (TeamType)nTeam, dwStartTime, fStartXPos, fStartZPos, nGrowthLvl );
		PlazaNPCGaugeWnd *pPlazaNPCGaugeWnd = dynamic_cast<PlazaNPCGaugeWnd *>( g_GUIMgr.FindWnd( PLAZA_NPC_GAUGE_WND ) );
		if( (TeamType)nTeam == TEAM_RED && pNpcChar && pPlazaNPCGaugeWnd )
		{
			m_szNPCName = kNpcName;

			pPlazaNPCGaugeWnd->InitAniState();
			pPlazaNPCGaugeWnd->SetRaidBossMonster( PlazaNPCGaugeWnd::GAUGE_AWAKENING_NPC, pNpcChar->GetCharName(), pNpcChar->GetMonsterIcon(), pNpcChar->GetImgName() );
			pPlazaNPCGaugeWnd->ChangeShowAniState( ioAnimationState::SAT_START_ROUND );
			pPlazaNPCGaugeWnd->ShowWnd();
		}

		if( (TeamType)nTeam == TEAM_RED)
		{
			vModeItem_iter iter = m_vModeItemList.begin();
			for( ; iter != m_vModeItemList.end(); ++iter )
			{
				ioModeItem *pItem = *iter;
				ioHunterCoinModeItem *pRaidCoinModeItem = ToHunterCoinModeItem( pItem );
				if( !pRaidCoinModeItem )
					continue;

				pRaidCoinModeItem->SetReserveDestroy();
			}
		}
	}
}

void ioRaidMode::OnMonsterInfoSync( SP2Packet &rkPacket )
{
	//중간난입 유저 동기화
	OnSpawnMonster( rkPacket );
}

void ioRaidMode::OnMonsterSyncChange( SP2Packet &rkPacket )
{
	int iMaxNpc;
	rkPacket >> iMaxNpc;

	for(int i = 0;i < iMaxNpc;i++)
	{
		ioHashString szNpcName, szSyncName;
		rkPacket >> szNpcName >> szSyncName;

		RaidRecord *pRecord = FindRecordCharName( szNpcName );
		if( !pRecord ) 
			continue;

		ioNpcChar *pNpc = ToNpcChar( pRecord->pChar );
		if( pNpc == NULL ) 
			continue;

		pNpc->SetSyncUser( szSyncName );
	}
}

void ioRaidMode::OnUseConsumption(SP2Packet &rkPacket)
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

			// HP 풀 회복 + 아이템 풀 착용
			RaidRecord *pRecord = FindRecordCharName( szUseUser );
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
				
				pRecord->dwCurDieTime = 0;

				g_SlotMgr.UpdateAllItem();
			}
			else
			{
				LOG.PrintTimeAndLog( 0, "ioPlayStage::OnUseMonsterCoin - %s Not Exist", szUseUser.c_str() );
			}
		}
	}
}

void ioRaidMode::OnRaidPhaseInfo(SP2Packet &rkPacket)
{
	byte bType;
	rkPacket >> bType;

	switch(bType)
	{
	case RAID_PHASE_INFO_COUNT_START:
		UICountTime(10000);
		break;
	}
}

void ioRaidMode::OnTreasureCardCommand( SP2Packet &rkPacket )
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

void ioRaidMode::ShowUserGauge()
{
	//UNDONE : User Gauage 
	if( m_ModeState != MS_PLAY )
		return;

	TowerDefWnd *pUserGauge = dynamic_cast< TowerDefWnd * >( g_GUIMgr.FindWnd( TOWERDEF_USER_GAUGE_WND ) );
	if( pUserGauge )
	{
		pUserGauge->ResetAllGauge();
		pUserGauge->ShowWnd();

		RaidRecord *pRecord = FindRecordCharName( g_MyInfo.GetPublicID() );
		if( pRecord )
		{
			if(pRecord->pChar && !g_BattleRoomMgr.IsObserverUser(g_MyInfo.GetPublicID()))
				pUserGauge->SetUser(pRecord->pChar, pRecord->pChar->GetClassLevel());
		}
	}
}

void ioRaidMode::UpdateUserGauge()
{
	TowerDefWnd *pUserGauge = dynamic_cast< TowerDefWnd * >( g_GUIMgr.FindWnd( TOWERDEF_USER_GAUGE_WND ) );
	if( !pUserGauge ) 
		return;

	//UNDONE : 유저 게이지
	if( pUserGauge->IsShow() )
	{
		ioHashStringVec vecName;
		pUserGauge->GetUserNameList(vecName);
		for(unsigned int i = 0; i < vecName.size(); i++)
		{
			RaidRecord *pRecord = FindRecordCharName( vecName[i].c_str() );
			if( pRecord )
			{	
				StatusValue &rkStatus = pRecord->pChar->GetHP();
				pUserGauge->UpdateGauge(vecName[i].c_str(), rkStatus.m_fCurValue, rkStatus.m_fMaxValue, pRecord->pChar->GetClassLevel() );
			}
		}
	}
}

void ioRaidMode::UICountTime( DWORD dwMaxCountTime )
{
	RaidReadyCountWnd *pRaidReadyCountWnd = dynamic_cast<RaidReadyCountWnd*>( g_GUIMgr.FindWnd( RAID_READY_COUNT_WND ) );
	if( pRaidReadyCountWnd )
		pRaidReadyCountWnd->SetCountState( dwMaxCountTime );
}