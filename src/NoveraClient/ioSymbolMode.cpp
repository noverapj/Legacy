

#include "stdafx.h"

#include "ioSymbolMode.h"
#include "ioLadderTeamMgr.h"
#include "ioShuffleRoomMgr.h"
#include "ioRankBattleManager.h"

#include "GUI/CenterInfoWnd.h"
#include "GUI/MiniMap.h"
#include "GUI/MiniSoldierSelectWnd.h"
#include "GUI/TimeScoreWnd.h"
#include "GUI/ioObserverWnd.h"
#include "GUI/TotalResultMainWnd.h"
#include "GUI/RoundStateMainWnd.h"
#include "GUI/CenterKillInfoWnd.h"

ioSymbolMode::ioSymbolMode( ioPlayStage *pCreator )
: ioPlayMode( pCreator )
{
	m_fNoneTeamSymbolHP = FLOAT100;
	m_iSymbolAlphaRate  = 127;

	m_iRedSymbolCnt = 0;
	m_iBlueSymbolCnt = 0;

	m_dwBlueSymbolAttackedTime = 0;
	m_dwRedSymbolAttackedTime  = 0;
	m_dwLastSymbolAttackedTime = 0;
	m_LastSymbolTeam = TEAM_NONE;

	m_pAttackCursor = NULL;
	m_fSymbolAttackMarkRange = 0.0f;

	m_iMinVictoryActiveSymbolCnt = 3;

	m_fHalfSymbolHPRate = FLOAT05;

	// For Gauge
	m_dwTicCheckTime = 0;

	m_fRedGauge = 0.0f;
	m_fBlueGauge = 0.0f;
	m_fCurRecoverGauge = 0.0f;
	m_AscendancyTeam = TEAM_NONE;

	m_pLastChangeSymbol = NULL;

	m_ModeType = MT_SYMBOL;

	m_bNoLimiteSymbol = false;

	m_dwCenterUIDelayTime = 0;

	m_vCharRecordList.reserve( 100 );
}

ioSymbolMode::~ioSymbolMode()
{
	m_bZeroHP = false;
	ClearMode();
}

void ioSymbolMode::InitMode( int iMapIndex )
{
	m_iModeMapIndex = iMapIndex;

	LoadMap();
	LoadSymbols();
	LoadPushStructs();
	LoadMachineStruct();
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

	m_fSymbolAttackMarkRange = kLoader.LoadFloat_e( "attack_mark_range", FLOAT1000 );
	m_fNoneTeamSymbolHP = kLoader.LoadFloat_e( "none_team_symbol_hp", FLOAT100 );
	m_iSymbolAlphaRate  = kLoader.LoadInt_e( "symbol_alpharate", 127 );

	m_bNoLimiteSymbol = kLoader.LoadBool_e( "no_limite_symbol", false );

	char szBuf[MAX_PATH];

	m_vSymbolHPList.clear();
	m_vExtraHPList.clear();
	int iSymbolHPCnt = kLoader.LoadInt_e( "symbol_hp_cnt", 0 );
	for( int i=0; i < iSymbolHPCnt; i++ )
	{
		wsprintf_e( szBuf, "symbol_hp%d", i+1 );
		float fSymbolHP = kLoader.LoadFloat( szBuf, 0.0f );
		m_vSymbolHPList.push_back( fSymbolHP );

		wsprintf_e( szBuf, "symbol_extra_hp%d", i+1 );
		float fExtraHP = kLoader.LoadFloat( szBuf, 0.0f );
		m_vExtraHPList.push_back( fExtraHP );
	}
	LoadModeCamera( kLoader );

	kLoader.SetTitle_e( "ko_recover" );
	m_fKORecoverRate = kLoader.LoadFloat_e( "ko_recover_rate", 0.0f );
	kLoader.LoadString_e( "ko_recover_effect", "", szBuf, MAX_PATH );
	m_KORecoverEffect = szBuf;
	kLoader.LoadString_e( "ko_perfect_effect", "", szBuf, MAX_PATH );
	m_PerfectRecoverEffect = szBuf;
	kLoader.LoadString_e( "ko_buff", "", szBuf, MAX_PATH );
	m_KOBuff = szBuf;

	kLoader.SetTitle_e( "half_symbol_hp" );
	m_fHalfSymbolHPRate = kLoader.LoadFloat_e( "half_hp_rate", FLOAT05 );

	kLoader.SetTitle_e( "revival_list_red" );
	int iListCnt = kLoader.LoadInt_e( "list_cnt", 0 );

	m_RedRevivalPosList.clear();
	for( int j=0; j < iListCnt; j++ )
	{
		wsprintf_e( szBuf, "revival_symbol%d", j+1 );
		int iSymbolIndex = kLoader.LoadInt( szBuf, 0 );
		if( iSymbolIndex > 0 )
		{
			m_RedRevivalPosList.push_back( iSymbolIndex );
		}
	}

	kLoader.SetTitle_e( "revival_list_blue" );
	iListCnt = kLoader.LoadInt_e( "list_cnt", 0 );

	m_BlueRevivalPosList.clear();
	for( j=0; j < iListCnt; j++ )
	{
		wsprintf_e( szBuf, "revival_symbol%d", j+1 );
		
		int iSymbolIndex = kLoader.LoadInt( szBuf, 0 );
		if( iSymbolIndex > 0 )
		{
			m_BlueRevivalPosList.push_back( iSymbolIndex );
		}
	}

	// For Gauge
	kLoader.SetTitle_e( "symbol_gauge" );

	m_dwRecoverTic = kLoader.LoadInt_e( "symbol_gauge_tic", 0 );
	m_fMaxGauge = kLoader.LoadFloat_e( "symbol_gauge_max", 0.0f );

	kLoader.SetTitle_e( "ui_info" );
	kLoader.LoadString_e( "1point_up_emoticon", "", szBuf, MAX_PATH );
	m_PointUpEmoticon = szBuf;

	LoadRevivalTime( kLoader );
	LoadModeResource( kLoader );

	kLoader.SetTitle_e( "end_scene_camera" );
	m_fEndHeight = kLoader.LoadFloat_e( "end_camera_height", 0.0f );
	m_fEndDistance = kLoader.LoadFloat_e( "end_camera_distance", 0.0f );
	m_fEndFov = kLoader.LoadFloat_e( "end_camera_fov", 0.0f );
	kLoader.LoadString_e( "round_end_focus_effect", "", szBuf, MAX_PATH );
	m_RoundEndEffect = szBuf;

	LoadGUIValues();
	LoadAttackCursor();
	LoadPushCursor();
	LoadContribute();

	kLoader.SetFileName_e( "config/sp2_sound.ini" );
	kLoader.SetTitle_e( "sound_file_name" );

	kLoader.LoadString_e( "last_symbol_attack", "", szBuf, MAX_PATH );
	m_LastSymbolAttack = szBuf;

	kLoader.LoadString_e( "last_symbol_damaged", "", szBuf, MAX_PATH );
	m_LastSymbolDamaged = szBuf;

	kLoader.LoadString_e( "symbol_damaged", "", szBuf, MAX_PATH );
	m_SymbolDamaged = szBuf;

	kLoader.LoadString_e( "symbol_occupy", "", szBuf, MAX_PATH );
	m_SymbolOccupy = szBuf;

	kLoader.LoadString_e( "symbol_destroy", "", szBuf, MAX_PATH );
	m_SymbolDestroy = szBuf;	
	
	if( !P2PNetwork::IsNetworkPlaying() )
	{
		ClearModeData();
		ReStartMode();

		LoadObjectItems();
	}

	int iSymbolCnt = m_vSymbolList.size();
	for( i=0 ; i<iSymbolCnt ; i++ )
	{
		m_vSymbolList[i]->RestoreOrgTeam();
	}

	UpdateSymbolHP();

	for( i=0 ; i<iSymbolCnt ; i++ )
	{
		ioSymbolStruct *pSymbol = m_vSymbolList[i];
		pSymbol->SetRevivalHP( 0.0f, true );
		pSymbol->Revival();

		if( m_iCurRoundCount == 1 )
		{
			pSymbol->SetRevivalCount( 0 );
		}
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

void ioSymbolMode::LoadAttackCursor()
{
	SAFEDELETE( m_pAttackCursor );
	m_pAttackCursor = new ioAnimate2DImageRender;

	ioINILoader_e kLoader( "config/sp2_gui.ini" );

	DWORD dwFrameTime = kLoader.LoadInt_e( "symbol_attack_enable", "frame_time", 100 );
	
	m_pAttackCursor->SetFrameTime( dwFrameTime );
	m_pAttackCursor->SetAnimateType( ioAnimate2DImageRender::FT_DEFAULT );
	
	m_pAttackCursor->AddImage( "BaseImage013", "TargetSword01" );
	m_pAttackCursor->AddImage( "BaseImage013", "TargetSword02" );
	m_pAttackCursor->AddImage( "BaseImage013", "TargetSword03" );
	m_pAttackCursor->SetHorzAlign( "Center" );
	m_pAttackCursor->SetVertAlign( "Bottom" );
	m_pAttackCursor->ShowRender();
}

void ioSymbolMode::LoadModeResource( ioINILoader &rkLoader )
{	
	TimeScoreWnd *pTimeScoreWnd = dynamic_cast<TimeScoreWnd *>( g_GUIMgr.AddWnd( "XML/TimeScoreWnd.xml") );
	if(pTimeScoreWnd)
		pTimeScoreWnd->SetModeType( GetModeType() );

	g_ModeUIController.SetKOMsgWnd( g_GUIMgr.AddWnd( "XML/kochatmsgwnd.xml") );

	CenterInfoWnd *pCenterInfoWnd = dynamic_cast<CenterInfoWnd*>( g_GUIMgr.AddWnd( "XML/CenterInfoWnd.xml" ) );
	if( pCenterInfoWnd )
	{	
		g_ModeUIController.SetCenterInfoWnd( pCenterInfoWnd );
	}

	CenterKillInfoWnd *pCenterKillInfoWnd = dynamic_cast<CenterKillInfoWnd*>( g_GUIMgr.AddWnd( "XML/centerkillinfownd.xml" ) );
	if( pCenterKillInfoWnd )
	{
		g_ModeUIController.SetCenterKillInfoWnd( pCenterKillInfoWnd );
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
}

void ioSymbolMode::LoadSymbols()
{
	ioINILoader kLoader( GetModeMapINIFileName( m_ModeType, m_iModeSubNum, m_iModeMapIndex ) );
	kLoader.SetTitle_e( "symbol_common_info" );

	D3DXVECTOR3 vScale;
	vScale.x = kLoader.LoadFloat_e( "scale_x", FLOAT05 );
	vScale.y = kLoader.LoadFloat_e( "scale_y", FLOAT05 );
	vScale.z = kLoader.LoadFloat_e( "scale_z", FLOAT05 );

	char szEffectFile[MAX_PATH];
	kLoader.LoadString_e( "red_team_change", "", szEffectFile, MAX_PATH );
	m_RedSymbolChangeEffect = szEffectFile;

	kLoader.LoadString_e( "blue_team_change", "", szEffectFile, MAX_PATH );
	m_BlueSymbolChangeEffect = szEffectFile;

	kLoader.LoadString_e( "apply_effect", "", szEffectFile, MAX_PATH );
	m_ApplyEffect = szEffectFile;

	m_iMinVictoryActiveSymbolCnt = kLoader.LoadInt_e( "min_victory_symbol_cnt", 3 );

	int iActiveCharCnt = 0;
	m_vSymbolActiveCharCntList.clear();

	iActiveCharCnt = kLoader.LoadInt_e( "symbolmode_lev1", 6 );
	m_vSymbolActiveCharCntList.push_back( iActiveCharCnt );

	iActiveCharCnt = kLoader.LoadInt_e( "symbolmode_lev2", 12 );
	m_vSymbolActiveCharCntList.push_back( iActiveCharCnt );

	char szRedSymbol[MAX_PATH], szBlueSymbol[MAX_PATH], szNoneActivitySymbol[MAX_PATH];

	int iSymbolCnt = kLoader.LoadInt_e( "symbol_cnt", 0 );

	for( int i=0 ; i<iSymbolCnt; i++ )
	{
		char szTitle[MAX_PATH];
		wsprintf_e( szTitle, "symbol%d", i+1 );
		kLoader.SetTitle( szTitle );

		kLoader.LoadString_e( "red_symbol", "staticmodel/symbol_red.txt", szRedSymbol, MAX_PATH );
		kLoader.LoadString_e( "blue_symbol", "staticmodel/symbol_blue.txt", szBlueSymbol, MAX_PATH );
		kLoader.LoadString_e( "none_activity_symbol", "staticmodel/symbol_disable.txt", szNoneActivitySymbol, MAX_PATH );

		int iPriority = kLoader.LoadInt_e( "symbol_priority", 0 );

		char szTeamType[MAX_PATH];
		kLoader.LoadString_e( "team", "NONE", szTeamType, MAX_PATH );
		TeamType eSymbolTeam = ConvertStringToTeamType( szTeamType );

		ioEntityGroup *pGrp = NULL;
		switch( eSymbolTeam )
		{
		case TEAM_NONE:
			pGrp = m_pCreator->CreateEntityGrpAndSceneNode( szNoneActivitySymbol );
			break;
		case TEAM_RED:
			pGrp = m_pCreator->CreateEntityGrpAndSceneNode( szRedSymbol );
			break;
		case TEAM_BLUE:
			pGrp = m_pCreator->CreateEntityGrpAndSceneNode( szBlueSymbol );
			break;
		}

		if( !pGrp )
		{
			LOG.PrintTimeAndLog( 0, "ioSymbolMode::LoadSymbols - %d Symbol File wrong!", i+1 );
			continue;
		}

		ioSymbolStruct *pSymbol = new ioSymbolStruct( pGrp, this );
		m_pCreator->AddGameEntity( pSymbol );

		//HARDCODE: activity 설정을 team 설정 보다 먼저 해줘야 한다.
		pSymbol->InitResource();
		pSymbol->LoadProperty( kLoader );
		pSymbol->SetSymbolActivity( false );
		pSymbol->SetOrgTeam( eSymbolTeam );
		pSymbol->SetSymbolPriority( iPriority );

		D3DXVECTOR3 vPos;
		vPos.x = kLoader.LoadFloat_e( "pos_x", FLOAT1000 );
		vPos.z = kLoader.LoadFloat_e( "pos_z", FLOAT1000 );
		vPos.y = m_pCreator->GetMapHeight( vPos.x, vPos.z, pSymbol, false );
		pSymbol->InitSymbolInfo( i, vPos, vScale );

		m_vSymbolList.push_back( pSymbol );
	}
}

void ioSymbolMode::ProcessMode()
{
	ErrorReport::SetPosition( 1020, 1 );
	ProcessSymbolZoneCollision();
	ErrorReport::SetPosition( 1020, 2 );
	ProcessSymbolAlphaRate();
	ErrorReport::SetPosition( 1020, 3 );

	ProcessSelectCharacter();
	ErrorReport::SetPosition( 1020, 4 );
	ProcessScoreGauge();
	ErrorReport::SetPosition( 1020, 5 );
	ProcessGUI();
	ErrorReport::SetPosition( 1020, 6 );

	switch( m_ModeState )
	{
	case MS_READY:
		ErrorReport::SetPosition( 1020, 7 );
		ProcessReadyState();
		ErrorReport::SetPosition( 1020, 8 );
		CheckModeMapSound();
		break;
	case MS_PLAY:
		ProcessMapAreaWeapon();
		ErrorReport::SetPosition( 1020, 9 );
		ProcessPlayState();
		ErrorReport::SetPosition( 1020, 10 );
		CheckModeMapSound();
		ErrorReport::SetPosition( 1020, 11 );
		g_EventMgr.Process();
		break;
	case MS_RESULT:
		ErrorReport::SetPosition( 1020, 12 );
		ProcessResultState();
		ErrorReport::SetPosition( 1020, 13 );
		break;
	}
}

void ioSymbolMode::ProcessSymbolAlphaRate()
{
	ioBaseChar *pMyChar = GetOwnerChar();
	if( !pMyChar )	return;

	ioCamera *pCamera = g_App.GetCamera();
	if( !pCamera )	return;

	D3DXVECTOR3 vMyPos = pMyChar->GetMidPositionByRate();
	D3DXVECTOR3 vDiff = pCamera->GetPosition() - vMyPos;
	ioRay kCamRay( vMyPos, vDiff );

	int iSymbolCount = m_vSymbolList.size();
	for( int i=0 ;i<iSymbolCount ; i++ )
	{
		ioSymbolStruct *pSymbol = m_vSymbolList[i];

		if( !pSymbol->IsNowAlphaChanging() )
		{
			if( ioMath::TestIntersection( kCamRay, pSymbol->GetWorldCollisionBox() ) )
			{
				pSymbol->SetTargetAlphaRate( m_iSymbolAlphaRate );
			}
			else
			{
				pSymbol->SetTargetAlphaRate( MAX_ALPHA_RATE );
			}
		}
	}
}

void ioSymbolMode::ProcessReadyState()
{
	if( P2PNetwork::IsNetworkPlaying() )	return;
	if( m_dwReadyStartTime + 4000 > FRAMEGETTIME() )
		return;

	SetModeState( MS_PLAY );
}

void ioSymbolMode::ProcessPlayState()
{
	UpdateSoldierRevivalPenalty();
	//UpdateTimeGrowthTime();

	m_pCreator->NotifySpringDampUseSet( true );

	ProcessRevival();
	
	if( !P2PNetwork::IsNetworkPlaying() )
	{		
		CheckRoundEnd();
	}
}

void ioSymbolMode::ReduceSymbolHP()
{
	int iSymbolCnt = m_vSymbolList.size();
	for( int i=0 ; i<iSymbolCnt ; i++ )
	{
		m_vSymbolList[i]->ReduceMaxHP( m_fHalfSymbolHPRate );
	}
}

void ioSymbolMode::CheckRoundEnd()
{
	if( m_dwPlayStartTime + m_dwRoundDuration > FRAMEGETTIME() )
		return;

	int iVictorySymbolCnt = GetCurActiveSymbolCnt();
	if( iVictorySymbolCnt < m_iMinVictoryActiveSymbolCnt )
		iVictorySymbolCnt = m_vSymbolList.size();

	WinTeamType eWinTeam = WTT_DRAW;
	if( m_iRedSymbolCnt > m_iBlueSymbolCnt )
	{
		if( m_iRedSymbolCnt == iVictorySymbolCnt )
			eWinTeam = WTT_VICTORY_RED_TEAM;
		else
			eWinTeam = WTT_RED_TEAM;
	}
	else if( m_iRedSymbolCnt < m_iBlueSymbolCnt )
	{
		if( m_iBlueSymbolCnt == iVictorySymbolCnt )
			eWinTeam = WTT_VICTORY_BLUE_TEAM;
		else
			eWinTeam = WTT_BLUE_TEAM;
	}

	bool bRoundSetEnd = false;
	if( m_iCurRoundCount == m_iMaxRoundCount )
		bRoundSetEnd = true;

	SetRoundEndInfo( eWinTeam, bRoundSetEnd );
}

void ioSymbolMode::ProcessResultState()
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

void ioSymbolMode::ProcessGUI()
{
	UpdateMiniMapSpotPoint();
	UpdateTimeScoreWnd();
	UpdateCenterInfoWnd();
	UpdateObserverWnd();
	UpdateRoundStateWnd();
	UpdateEndScoreWndTime();
	UpdateSoldierWndTime();


	ProcessModeGuide();
	if( m_pAttackCursor )
	{
		m_pAttackCursor->Update();
	}

	ProcessPushEnableCursor();
	ProcessBlindMode();
	ProcessDeathTimeHelp();
	ProcessAutoStartAlarm();
}

void ioSymbolMode::UpdateMiniMapSpotPoint()
{
	MiniMap *pMiniMap = dynamic_cast<MiniMap*>(g_GUIMgr.FindWnd( MINIMAP_WND ));
	if( !pMiniMap )	return;

	ioPlayMode::UpdateMiniMapSpotPoint();

	pMiniMap->ClearPointList();

	D3DXVECTOR3 vPos;
	DWORD dwGapTime = 0;

	// Symbol
	SymbolList::iterator iSymbol;
	for( iSymbol=m_vSymbolList.begin() ; iSymbol!=m_vSymbolList.end() ; ++iSymbol )
	{
		ioSymbolStruct *pSymbol = *iSymbol;

		if( !pSymbol || !pSymbol->GetSymbolActivity() ) continue;

		dwGapTime = pSymbol->GetEventGapTime();
		vPos = pSymbol->GetSpotPosition( pMiniMap->GetSpotPositionRate() );
		pMiniMap->AddPoint( vPos.x, vPos.z, MiniMap::SPOT_SYMBOL, pSymbol->GetTeam(), dwGapTime );
	}

	// Char..
	RecordList::iterator iter;
	for( iter=m_vCharRecordList.begin() ; iter!=m_vCharRecordList.end() ; ++iter )
	{
		ioBaseChar *pChar = iter->pChar;
		if( !pChar ) continue;
		if( pChar->IsOwnerChar() || !pChar->IsCanMiniMapVisible() )
			continue;

		dwGapTime = pChar->GetEventGapTime();
		vPos = pChar->GetWorldPosition();

		if( pChar->IsDropZoneDownState() )
			pMiniMap->AddPoint( vPos.x, vPos.z, MiniMap::SPOT_DROP_USER, pChar->GetTeam(), 0 );
		else
			pMiniMap->AddPoint( vPos.x, vPos.z, MiniMap::SPOT_USER, pChar->GetTeam(), dwGapTime );
	}

	// owner
	ioBaseChar *pOwner = GetOwnerChar();
	if( pOwner && pOwner->IsCanMiniMapVisible() )
	{
		dwGapTime = pOwner->GetEventGapTime();
		vPos = pOwner->GetWorldPosition();

		if( pOwner->IsDropZoneDownState() )
			pMiniMap->AddPoint( vPos.x, vPos.z, MiniMap::SPOT_DROP_ME, pOwner->GetTeam(), 0 );
		else
			pMiniMap->AddPoint( vPos.x, vPos.z, MiniMap::SPOT_ME, pOwner->GetTeam(), dwGapTime );
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

void ioSymbolMode::UpdateCenterInfoWnd()
{
	ioBaseChar *pOwnerChar = GetOwnerChar();
	if( !pOwnerChar ) return;
	if( pOwnerChar->GetState() == CS_LOADING ||
		pOwnerChar->GetState() == CS_READY ) return;

	if( g_GUIMgr.IsShow( TIME_SCORE_WND ) )
	{
		if( m_dwCenterUIDelayTime == 0 )
			m_dwCenterUIDelayTime = FRAMEGETTIME();
	}

	if( GetModeState() != MS_PLAY )
	{
		g_ModeUIController.SetSymbolGaugeRate( false, 0.0f, 0.0f );
		m_dwCenterUIDelayTime = 0;
	}
	else if( m_dwCenterUIDelayTime > 0 )
	{
		// 첫 시작시 1.5초간 표시 안함
		if( FRAMEGETTIME() - m_dwCenterUIDelayTime > 1500 )
		{
			float fBlueSymbolRate, fRedSymbolRate;
			fBlueSymbolRate = fRedSymbolRate = 0.0f;
			int iSymbolCount = m_vSymbolList.size();
			for( int i=0 ;i<iSymbolCount ; i++ )
			{
				ioSymbolStruct *pSymbol = m_vSymbolList[i];
				if( !pSymbol ) continue;
				
				if( pSymbol->GetTeam() == TEAM_BLUE )
					fBlueSymbolRate = pSymbol->GetCurHPRate();
				else if( pSymbol->GetTeam() == TEAM_RED )
					fRedSymbolRate = pSymbol->GetCurHPRate();
			}
			g_ModeUIController.SetSymbolGaugeRate( true, fBlueSymbolRate, fRedSymbolRate );
		}
	}
}

// 모드 : 09.06.03
void ioSymbolMode::UpdateTimeScoreWnd()
{
	TimeScoreWnd *pTimeScoreWnd = dynamic_cast<TimeScoreWnd *>( g_GUIMgr.FindWnd( TIME_SCORE_WND ) );
	if(!pTimeScoreWnd) return;

	//
	ioSymbolStruct *pRed = GetStartPosSymbol( TEAM_RED );
	ioSymbolStruct *pBlue = GetStartPosSymbol( TEAM_BLUE );

	float fRedRate = 0.0f;
	float fBlueRate = 0.0f;

	if( pRed )
		fRedRate = pRed->GetCurHPRate();

	if( pBlue )
		fBlueRate = pBlue->GetCurHPRate();
	//

	bool bRedWarning = false;
	if( fRedRate <= (1.0f-m_fScoreWarningRate) )
		bRedWarning = true;

	bool bBlueWarning = false;
	if( fBlueRate <= (1.0f-m_fScoreWarningRate) )
		bBlueWarning = true;

	pTimeScoreWnd->UpdateScore( m_iBlueRoundWinCnt, m_iRedRoundWinCnt );

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
			pTimeScoreWnd->UpdateTime( m_dwCurRoundDuration  - dwTimeGap, m_dwRoundDuration );
			ProcessWarningSound( m_dwCurRoundDuration );

			if( bRedWarning || bBlueWarning )
			{
				ProcessScoreWarningSound( true );
			}
			else
			{
				ProcessScoreWarningSound( false );
			}
		}
		else
		{
			if( g_BattleRoomMgr.IsTournamentModeType() )
			{
				dwTimeGap = FRAMEGETTIME() - m_dwSuddenDaathStartTime;
				pTimeScoreWnd->UpdateSuddenDeathTime( m_dwSuddenDeathTime - dwTimeGap, m_dwSuddenDeathTime );
			}
			else
			{	
				pTimeScoreWnd->UpdateTime( 0, m_dwRoundDuration );
			}
		}
		break;
	case MS_RESULT:
		pTimeScoreWnd->UpdateTime( 0, m_dwRoundDuration );
		break;
	}

	if( m_ModeState != MS_PLAY )
	{
		ProcessScoreWarningSound( false );
	}
}

void ioSymbolMode::UpdateSymbolHP()
{
	// 상징물 체력값 재계산
	float fRedRate, fBlueRate;
	fRedRate = fBlueRate = FLOAT1;

	int iBlueCnt, iRedCnt;
	iBlueCnt = iRedCnt = 0;

	int iBlueTeamCnt, iRedTeamCnt;
	iBlueTeamCnt = iRedTeamCnt = 0;

	for( int i = 0 ; i < (int)m_vRoundHistory.size() ; i++ )
	{
		iBlueCnt += m_vRoundHistory[i].iBluePoint;
		iRedCnt  += m_vRoundHistory[i].iRedPoint;
	}

	// 스코어 보정
	fRedRate = g_MathManager.GetSymbolExtraRate( iRedCnt, iBlueCnt );
	fBlueRate = g_MathManager.GetSymbolExtraRate( iBlueCnt, iRedCnt );

	int iCharCnt = 0;
	int iSize = GetRecordCharCnt();
	for( int i=0 ; i<iSize; i++ )
	{
		ModeRecord *pRecord = FindModeRecord( i );
		if( pRecord == NULL ) continue;
		if( !pRecord->pChar ) continue;
		if( pRecord->pChar->GetState() == CS_OBSERVER ) continue;
		if( g_BattleRoomMgr.IsObserverUser(pRecord->pChar->GetCharName()) ) continue;

		iCharCnt++;

		if( pRecord->pChar->GetTeam() == TEAM_RED )
			iRedTeamCnt++;
		else if( pRecord->pChar->GetTeam() == TEAM_BLUE )
			iBlueTeamCnt++;
	}

	int iSymbolCnt = m_vSymbolList.size();

	int iBlueSymbolCnt = GetTeamSymbolCnt( TEAM_BLUE );
	int iRedSymbolCnt  = GetTeamSymbolCnt( TEAM_RED );

	float fBlueTeamHP, fRedTeamHP;
	fBlueTeamHP = fRedTeamHP = 0.0f;;

	// 토탈 인원보정
	if( iSymbolCnt > 0 && COMPARE( iBlueSymbolCnt, 1, iSymbolCnt+1 ) )
	{
		fBlueTeamHP = m_vSymbolHPList[iBlueSymbolCnt-1] + ( m_vExtraHPList[iBlueSymbolCnt-1] * iCharCnt );
		fBlueTeamHP *= fBlueRate;
	}
	
	if( iSymbolCnt > 0 && COMPARE( iRedSymbolCnt, 1, iSymbolCnt+1 ) )
	{
		fRedTeamHP = m_vSymbolHPList[iRedSymbolCnt-1] + ( m_vExtraHPList[iRedSymbolCnt-1] * iCharCnt );
		fRedTeamHP *= fRedRate;
	}

	// 상징물 체력 회복값 재계산
	float fRedRecoveryRate, fBlueRecoveryRate;
	fRedRecoveryRate = fBlueRecoveryRate = 0.0f;


	// 인원에 따른 회복 보정
	if( iSymbolCnt > 0 )
	{
		fRedRecoveryRate = g_MathManager.GetSymbolRecoveryRate( iRedTeamCnt, iBlueTeamCnt );
		fBlueRecoveryRate = g_MathManager.GetSymbolRecoveryRate( iBlueTeamCnt, iRedTeamCnt );
	}

	for( int i=0; i < iSymbolCnt; ++i )
	{
		TeamType eTeam = m_vSymbolList[i]->GetTeam();

		if( eTeam == TEAM_RED )
		{
			m_vSymbolList[i]->UpdateMaxHP( fRedTeamHP );
			m_vSymbolList[i]->SetRecoveryRate( fRedRecoveryRate );
		}
		else if( eTeam == TEAM_BLUE )
		{
			m_vSymbolList[i]->UpdateMaxHP( fBlueTeamHP );
			m_vSymbolList[i]->SetRecoveryRate( fBlueRecoveryRate );
		}
		else
		{
			m_vSymbolList[i]->UpdateMaxHP( m_fNoneTeamSymbolHP );
			m_vSymbolList[i]->SetRecoveryRate( FLOAT1 );
		}
	}
}

void ioSymbolMode::UpdateRoundStateWnd()
{
	if( g_MyInfo.IsSortContribute() )
	{
		DWORD dwTimeGap = FRAMEGETTIME() - m_dwPlayStartTime;
		if( m_dwCurRoundDuration > dwTimeGap )
			ContributeRoundAllPercent( m_dwRoundDuration - (m_dwCurRoundDuration - dwTimeGap), m_dwRoundDuration );
		else
			ContributeRoundAllPercent( m_dwRoundDuration, m_dwRoundDuration );

		SymbolRecord *pRecord = FindRecordCharName( g_MyInfo.GetPublicID() );
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

			pRoundStateWnd->SetTeamScore( m_iBlueRoundWinCnt, m_iRedRoundWinCnt );
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

void ioSymbolMode::ProcessRevival()
{
	DWORD dwCurTime = FRAMEGETTIME();
	
	int iCharCnt = m_vCharRecordList.size();
	for( int i=0 ; i<iCharCnt ; i++ )
	{
		SymbolRecord &rkRecord = m_vCharRecordList[i];
		if( rkRecord.dwCurDieTime == 0 ) continue;

		if( rkRecord.pChar == m_pCreator->GetOwnerChar() )
		{
			int iRevivalTime = (int)rkRecord.dwRevivalGap - ( dwCurTime - rkRecord.dwCurDieTime );
			if( iRevivalTime < 0 ) iRevivalTime = 0;
			if( P2PNetwork::IsNetworkPlaying() )
			{
				if( rkRecord.dwCurDieTime + rkRecord.dwRevivalGap < dwCurTime )
				{
//					rkRecord.dwCurDieTime = 0;
					SetViewRoundState( false );
				}
				else if( rkRecord.dwCurDieTime + rkRecord.dwRevivalGap < dwCurTime + 3000 )
					SetViewRoundState( true );
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

void ioSymbolMode::ProcessSymbolZoneCollision()
{
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	int iCharCnt = m_vCharRecordList.size();	
	for( int i=0 ; i<iCharCnt ; i++ )
	{
		SymbolRecord &rkRecord = m_vCharRecordList[i];

		ioBaseChar *pChar = rkRecord.pChar;
		ioSymbolStruct *pApplyZone = NULL;

		int iZoneCnt = m_vSymbolList.size();
		for( int j=0 ; j<iZoneCnt ; j++ )
		{
			ioSymbolStruct *pSymbol = m_vSymbolList[j];

			if( pSymbol->IsInSymbolRange( pChar ) )
			{
				pApplyZone = pSymbol;				
				break;
			}
		}

		if( pApplyZone )
		{
			if( !rkRecord.pSymbol )
			{
				rkRecord.pSymbol = pApplyZone;

				pChar->AttachEffect( m_ApplyEffect );
				pChar->SetExtraSkillGaugeRate( pApplyZone->GetSkillGaugeRate() );
				pChar->SetExtraDamageRate( pApplyZone->GetCharDamageRate() );
			}
		}
		else
		{
			if( rkRecord.pSymbol )
			{
				rkRecord.pSymbol = NULL;

				pChar->EndEffect( m_ApplyEffect );
				pChar->SetExtraSkillGaugeRate( FLOAT1 );
				pChar->SetExtraDamageRate( FLOAT1 );
			}
		}
	}
}

void ioSymbolMode::RenderPreGUI()
{
	if( m_ModeState == MS_PLAY )
	{
		RenderAttackEnableSymbolCursor();
		RenderPushEnableCursor();
	}
}

void ioSymbolMode::RenderAttackEnableSymbolCursor()
{
	if( !m_pAttackCursor )	return;

	ioBaseChar *pOwner = GetOwnerChar();
	if(!pOwner) return;
	
	D3DXVECTOR3 vOwnerPos = pOwner->GetWorldPosition();
	TeamType eOwnerTeam = pOwner->GetTeam();

	int iSymbolCnt = m_vSymbolList.size();
	for( int i=0 ; i<iSymbolCnt ; i++ )
	{
		ioSymbolStruct *pSymbol = m_vSymbolList[i];

		if( eOwnerTeam == pSymbol->GetTeam() )	continue;
		if( !pSymbol->GetSymbolActivity() )	continue;

		D3DXVECTOR3 vDiff = vOwnerPos - pSymbol->GetWorldPosition();
		vDiff.y = 0.0f;

		if( m_fSymbolAttackMarkRange < D3DXVec3Length( &vDiff ) )
			continue;

		const ioOrientBox &rkColBox = pSymbol->GetWorldCollisionBox();

		D3DXVECTOR3 vTop = rkColBox.GetCenter();
		vTop.y += rkColBox.GetExtents( 1 );

		int xPos = 0, yPos = 0;
		if( Setting::Check2DInScreen( vTop, xPos, yPos ) )
		{
			m_pAttackCursor->Render( xPos, yPos );
		}
	}
}

void ioSymbolMode::RenderAfterGUI()
{
	/*
	if( m_ModeState == MS_PLAY )
	{
		float fDefCenterXRate = Setting::Width() / 800.0f;
		float fDefCenterYRate = Setting::Height() / 600.0f;

		int iXPos, iYPos;
		iXPos = Setting::Width() - ( 50.0f * fDefCenterXRate );
		iYPos = Setting::Height() - ( 150.0f * fDefCenterYRate );

		int iContributePer = g_MyInfo.GetMyContribute();

		g_FontMgr.SetAlignType( TAT_RIGHT );
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
		//g_FontMgr.PrintText( iXPos, iYPos, 2.5f, STR(1), iContributePer );
	}
	*/
}

void ioSymbolMode::ClearMode()
{
	SAFEDELETE( m_pAttackCursor );

	ioPlayMode::ClearMode();
}

ioSymbolStruct* ioSymbolMode::GetStartPosSymbol( TeamType eTeam )
{
	ioSymbolStruct *pStartSymbol = NULL;
	D3DXVECTOR3 vStartPos = GetStartPos( eTeam );

	float fSymbolDistSq = std::numeric_limits<float>::infinity();

	int iSymbolCnt = m_vSymbolList.size();
	for( int i=0 ; i<iSymbolCnt ; i++ )
	{
		ioSymbolStruct *pSymbol = m_vSymbolList[i];

		D3DXVECTOR3 vDiff = vStartPos - pSymbol->GetWorldPosition();
		float fDistSq = D3DXVec3LengthSq( &vDiff );

		if( fDistSq < fSymbolDistSq )
		{
			fSymbolDistSq = fDistSq;
			pStartSymbol = pSymbol;
		}
	}

	return pStartSymbol;
}

D3DXVECTOR3 ioSymbolMode::GetRevivalPos( ioBaseChar *pChar )
{
	if( !pChar )
	{
		LOG.PrintTimeAndLog( 0, "ioSymbolMode::GetRevivalPos() - Char is NULL" );
		return D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	}

	return GetStartPos( pChar->GetTeam() );
}

void ioSymbolMode::NotifyEntityDie( ioPlayEntity *pEntity, ioPlayEntity *pKiller, ioBaseChar *pBestAttacker,
								   float fLastRate, float fBestRate )
{
	ioBaseChar *pDieChar = ToBaseChar( pEntity );
	ioBaseChar *pKillChar = ToBaseChar( pKiller );

	switch( pEntity->GetSubType() )
	{
	case ioPlayEntity::PST_CHAR:
		OnNotifyCharDie( pDieChar, pKillChar, pBestAttacker, fLastRate, fBestRate );
		UpdateKillDeathRecord( pEntity, pKiller, pBestAttacker );
		break;
	case ioPlayEntity::PST_SYMBOL:
		OnNotifySymbolDie( pEntity, pKiller );
		break;
	}
}

void ioSymbolMode::NotifyCharDropDie( ioBaseChar *pDieChar, ioBaseChar *pKillChar, ioBaseChar *pBestAttacker,
									 float fLastRate, float fBestRate )
{
	OnNotifyCharDie( pDieChar, pKillChar, pBestAttacker, fLastRate, fBestRate );
	UpdateDropDieRecord( pDieChar, pKillChar, pBestAttacker );
}

void ioSymbolMode::OnNotifyCharDie( ioBaseChar *pDieChar, ioBaseChar *pKillChar, ioBaseChar *pBestAttacker,
								   float fLastRate, float fBestRate )
{
	if( !pDieChar )
	{
		LOG.PrintTimeAndLog( 0, "ioSymbolMode::OnNotifyCharDie() - DieChar is NULL" );
		return;
	}

	SymbolRecord *pDieRecord = FindRecordCharName( pDieChar->GetCharName() );
	if( !pDieRecord )
	{
		LOG.PrintTimeAndLog( 0, "ioSymbolMode::OnNotifyCharDie - %s Not Exit Char",
								pDieChar->GetCharName().c_str() );
		return;
	}

	pDieRecord->vRecentDiePos = pDieChar->GetWorldPosition();

	if( pKillChar && pKillChar->GetTeam() != pDieChar->GetTeam() )
	{
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
	}

	if( P2PNetwork::IsNetworkPlaying() )
	{
		if( g_MyInfo.GetPublicID() == pDieChar->GetCharName() )
		{
			g_MyInfo.SetCharDied( pDieChar->GetSelectCharArray(), true );
		}
	}
}

void ioSymbolMode::OnNotifySymbolDie( ioPlayEntity *pDieEntity, ioPlayEntity *pKiller )
{
	ioSymbolStruct *pSymbol = ToSymbolStruct( pDieEntity );
	if( !pSymbol )	return;

	ioBaseChar *pKillChar = ToBaseChar( pKiller );
	if( !pKillChar )	return;

	float fCurHP = fabs( pSymbol->GetHP().m_fCurValue );

	if( pKillChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CTPK_SYMBOL_DIE );
		kPacket << pSymbol->GetSymbolArrayIdx();
		kPacket << pSymbol->GetRevivalCount();
		kPacket << fCurHP;
		TCPNetwork::SendToServer( kPacket );
	}
	else
	{
		pSymbol->SetRevivalHP( fCurHP );

		ProcessSymbolKill( pSymbol, pKillChar );
		SymbolDestroyAnnounce( pKillChar->GetTeam() );
	}

	if( pKillChar->GetTeam() == GetOwnerTeam() )
	{
		ioGameEntity *pTarget = GetGuideTarget( pSymbol->GetWorldPosition() );
		ioSymbolStruct *pNext = ToSymbolStruct( pTarget );
		if( pNext )
		{
			MiniMap *pMiniMap = dynamic_cast<MiniMap*>( g_GUIMgr.FindWnd(MINIMAP_WND) );
			if( pMiniMap )
			{
				D3DXVECTOR3 vPos = ( pSymbol->GetWorldPosition() + pNext->GetWorldPosition() ) * FLOAT05;
				D3DXVECTOR3 vDir = pNext->GetWorldPosition() - pSymbol->GetWorldPosition();
				D3DXVec3Normalize( &vDir, &vDir );

				pMiniMap->AddNextSymbolEvent( vPos, vDir, pKillChar->GetTeam() );
			}
		}
	}
}

void ioSymbolMode::OnZeroHP( SP2Packet &rkPacket )
{
	rkPacket >> m_dwSuddenDeathTime;
	m_dwSuddenDaathStartTime = FRAMEGETTIME();

	int iCharCnt = m_vCharRecordList.size();
	for( int i=0 ; i<iCharCnt ; i++ )
	{
		m_vCharRecordList[i].pChar->SetCurHP( 0.0f );
		m_vCharRecordList[i].pChar->SetCurHPRate( 0.0f );
	}

	m_bZeroHP = true;
	g_SoundMgr.PlaySound( m_TimeOutSound, DSBVOLUME_MAX, PSM_LOOP | PSM_THREAD );
	g_ModeUIController.SetDeathTime();
}

void ioSymbolMode::OnRoundReady( SP2Packet &rkPacket )
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

void ioSymbolMode::OnRoundJoin( SP2Packet &rkPacket )
{
	rkPacket >> m_fCurRecoverGauge;

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
			LOG.PrintTimeAndLog( 0, "ioSymbolMode::OnRoundJoin MS_READY" );
			break;
		case MS_PLAY:
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

	m_pCreator->ReCalculateDamageRate( false );
}

void ioSymbolMode::OnRoundJoinView( SP2Packet &rkPacket )
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

		g_ModeHelpMgr.InitModeHelp();
		g_ModeHelpMgr.HideHelp( ioModeHelpManager::HT_VIEW );

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

void ioSymbolMode::OnRoundJoinObserver( SP2Packet &rkPacket )
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

void ioSymbolMode::OnScore( SP2Packet &rkPacket )
{
	int iTeamType = TEAM_NONE;
	bool bLast;

	ScoreSound eScoreSound = SS_DEFAULT;

	int iBackBlueScore = m_iBlueSymbolCnt;
	int iBackRedScore  = m_iRedSymbolCnt;

	rkPacket >> m_iBlueSymbolCnt;
	rkPacket >> m_iRedSymbolCnt;
	rkPacket >> iTeamType;
	rkPacket >> bLast;
	rkPacket >> m_fCurRecoverGauge;

	if( m_iBlueSymbolCnt > 0 && m_iRedSymbolCnt > 0 &&
		m_iBlueSymbolCnt == m_iRedSymbolCnt )
	{
		eScoreSound = SS_EQUAL;
	}

	UpdateRoundStateWnd();
	UpdateTimeScoreWnd();

	UpdateSymbolHP();

	PointUpEvent( (TeamType)iTeamType );

	// 스코어 획득 메세지	
	{
		g_ChatMgr.SetScoreMsg( (TeamType)iTeamType, "", false );
	}

	if( bLast )
	{
		eScoreSound = SS_LAST;
		ProcessScoreSound( (TeamType)iTeamType, eScoreSound );
	}
	else
	{
		if( m_iBlueSymbolCnt != iBackBlueScore )
		{
			if( iBackBlueScore == 0 && m_iBlueSymbolCnt > 0 )
				eScoreSound = SS_FIRST;
		}

		if( m_iRedSymbolCnt != iBackRedScore )
		{
			if( iBackRedScore == 0 && m_iRedSymbolCnt > 0 )
				eScoreSound = SS_FIRST;
		}

		ProcessScoreSound( (TeamType)iTeamType, eScoreSound );
	}
}

void ioSymbolMode::ProcessSymbolKill( ioSymbolStruct *pSymbol, ioBaseChar *pKillChar )
{
	if( !pSymbol )
	{
		LOG.PrintTimeAndLog( 0, "ioSymbolMode::ProcessSymbolKill - Symbol is NULL" );
		return;
	}

	if( !pKillChar )
	{
		LOG.PrintTimeAndLog( 0, "ioSymbolMode::ProcessSymbolKill - KillChar is NULL" );
		return;
	}

	TeamType eKillTeam = pKillChar->GetTeam();
	pSymbol->SetTeam( eKillTeam );

	UpdateSymbolHP();

	pSymbol->SetRevivalHP( 0.0f );
	pSymbol->Revival();
	pSymbol->SetChangeEvent();

	CreateSymbolTeamChangeEffect( pSymbol->GetWorldPosition(), eKillTeam );
	SetMiniMapEvent( pSymbol->GetWorldPosition(), MiniMap::EVENT_SYMBOL_DESTROY, eKillTeam );

	m_pLastChangeSymbol = pSymbol;
}

void ioSymbolMode::AddNewRecord( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	SymbolRecord *pSymbolRecord = FindRecordCharName( pChar->GetCharName() );
	if( !pSymbolRecord )
	{
		SymbolRecord kRecord;
		kRecord.pChar = pChar;
		kRecord.szName= pChar->GetCharName();
		m_vCharRecordList.push_back( kRecord );

		CheckSymbolActivity();
		UpdateSymbolHP();
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
		pSymbolRecord->pChar = pChar;
	}
}

void ioSymbolMode::RemoveRecord( const ioHashString &rkName )
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

	CheckSymbolActivity();
	UpdateSymbolHP();
	UpdateUserRank();
}

void ioSymbolMode::RemoveRecord( int nIndex )
{
	int iCharCnt = m_vCharRecordList.size();
	if( iCharCnt > nIndex )
	{
		m_vCharRecordList.erase( m_vCharRecordList.begin() + nIndex );
		CheckSymbolActivity();
		UpdateSymbolHP();
		UpdateUserRank();
	}
}


ioBaseChar* ioSymbolMode::GetRecordChar( int i )
{
	if( COMPARE( i, 0, GetRecordCharCnt() ) )
		return m_vCharRecordList[i].pChar;

	return NULL;
}

ioBaseChar* ioSymbolMode::GetRecordChar( const ioHashString &rkName )
{
	SymbolRecord *pRecord = FindRecordCharName( rkName );
	if( pRecord )
		return pRecord->pChar;

	return NULL;
}

int ioSymbolMode::GetRecordCharCnt() const
{
	return m_vCharRecordList.size();
}

ModeRecord* ioSymbolMode::FindModeRecord( int iIdx )
{
	if( COMPARE( iIdx, 0, GetRecordCharCnt() ) )
		return &m_vCharRecordList[iIdx];

	return NULL;
}

ModeRecord* ioSymbolMode::FindModeRecordCharName( const ioHashString &rkName )
{
	return (ModeRecord*)FindRecordCharName( rkName );
}

SymbolRecord* ioSymbolMode::FindRecordCharName( const ioHashString &rkName )
{
	int iCharCnt = m_vCharRecordList.size();
	for( int i=0 ; i<iCharCnt ; i++ )
	{
		if( m_vCharRecordList[i].szName == rkName )
			return &m_vCharRecordList[i];
	}

	return NULL;
}

bool ioSymbolMode::GetRoundInfo( int iIndex, RoundInfo &rkInfo )
{
	if( ioPlayMode::GetRoundInfo( iIndex, rkInfo ) )
	{
		SymbolRecord *pRecord = (SymbolRecord*)FindModeRecord( iIndex );
		if( !pRecord )	return false;
		if( !pRecord->pChar ) return false;

		rkInfo.m_iContribute = pRecord->Contribute.m_iContribute;
		rkInfo.m_iContributePer = pRecord->Contribute.m_iPrevContributePer;
		return true;
	}
	return false;
}

bool ioSymbolMode::GetResultInfo( int iIndex, ResultInfo &rkInfo )
{
	if( ioPlayMode::GetResultInfo( iIndex, rkInfo ) )
	{
		SymbolRecord *pRecord = (SymbolRecord*)FindModeRecord( iIndex );
		if( !pRecord )	return false;
		if( !pRecord->pChar ) return false;

		rkInfo.m_iContribute = pRecord->Contribute.m_iContribute;
		rkInfo.m_iContributePer = pRecord->Contribute.m_iPrevContributePer;
		return true;
	}
	return false;
}

bool ioSymbolMode::GetFinalInfo( int iIndex, FinalInfo &rkInfo )
{
	if( ioPlayMode::GetFinalInfo( iIndex, rkInfo ) )
	{
		SymbolRecord *pRecord = (SymbolRecord*)FindModeRecord( iIndex );
		if( !pRecord )	return false;
		if( !pRecord->pChar ) return false;

		rkInfo.m_iContribute = pRecord->Contribute.m_iContribute;
		rkInfo.m_iContributePer = pRecord->Contribute.m_iPrevContributePer;
		return true;
	}
	return false;
}

ioSymbolStruct* ioSymbolMode::GetSymbol( int iArray )
{
	int iSymbolCnt = m_vSymbolList.size();
	if( COMPARE( iArray, 0, iSymbolCnt ) )
		return m_vSymbolList[iArray];

	return NULL;
}

void ioSymbolMode::ReStartMode()
{
	int i=0;

	int iCharCnt = m_vCharRecordList.size();
	for( i=0 ; i<iCharCnt ; i++ )
	{
		SymbolRecord &rkRecord = m_vCharRecordList[i];
		if( !rkRecord.pChar ) continue;
		rkRecord.pChar->EndEffect( m_ApplyEffect );
		rkRecord.pSymbol = NULL;

		D3DXVECTOR3 vStartPos = GetStartPos( rkRecord.pChar->GetTeam() );
		rkRecord.pChar->SetWorldPosition( vStartPos );

		rkRecord.dwRevivalGap = 0;
		rkRecord.dwCurDieTime = 0;
		rkRecord.iRevivalCnt = 0;
		rkRecord.iNoDeathKillCount = 0;

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

	int iSymbolCnt = m_vSymbolList.size();
	for( i=0 ; i<iSymbolCnt ; i++ )
	{
		ioSymbolStruct *pSymbol = m_vSymbolList[i];

		pSymbol->RestoreOrgTeam();
	}

	UpdateSymbolHP();

	for( i=0 ; i<iSymbolCnt ; i++ )
	{
		ioSymbolStruct *pSymbol = m_vSymbolList[i];

		pSymbol->SetRevivalHP( 0.0f, true );
		pSymbol->Revival();
		if( m_iCurRoundCount == 1 )
			pSymbol->SetRevivalCount( 0 );
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

	m_iRedSymbolCnt = 0;
	m_iBlueSymbolCnt = 0;

	m_dwBlueSymbolAttackedTime = 0;
	m_dwRedSymbolAttackedTime  = 0;
	m_dwLastSymbolAttackedTime = 0;
	m_LastSymbolTeam = TEAM_NONE;

	// For Gauge
	m_dwTicCheckTime = 0;

	m_fRedGauge = 0.0f;
	m_fBlueGauge = 0.0f;
	m_fCurRecoverGauge = 0.0f;
	m_AscendancyTeam = TEAM_NONE;
	//

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

void ioSymbolMode::CreateSymbolTeamChangeEffect( const D3DXVECTOR3 &vPos, TeamType eTeam )
{
	if( eTeam != TEAM_BLUE && eTeam != TEAM_RED )
		return;

	if( eTeam == TEAM_BLUE && !m_BlueSymbolChangeEffect.IsEmpty() )
	{
		m_pCreator->CreateMapEffect( m_BlueSymbolChangeEffect,
									 vPos,
									 ioMath::UNIT_ALL );
	}
	else if( eTeam == TEAM_RED && !m_RedSymbolChangeEffect.IsEmpty() )
	{
		m_pCreator->CreateMapEffect( m_RedSymbolChangeEffect,
									 vPos,
									 ioMath::UNIT_ALL );
	}
}

void ioSymbolMode::SymbolDestroyAnnounce( TeamType eKillTeam )
{
	if( m_WinTeamType != WTT_NONE || eKillTeam == TEAM_NONE )
		return;

	if( GetOwnerTeam() == eKillTeam )
		g_SoundMgr.PlaySound( m_SymbolOccupy );
	else
		g_SoundMgr.PlaySound( m_SymbolDestroy );
}

void ioSymbolMode::ContributeLOG()
{
	char szLog[MAX_PATH];
	int iCharCnt = m_vCharRecordList.size();

	for( int i = 0;i < iCharCnt;i++)
	{
		SymbolRecord *pRecord = &m_vCharRecordList[i];
		if( pRecord->pChar )
		{
			sprintf( szLog, "%s[%d] : P%d(%d) K%d(%d) D%d(%d) A%d(%d)", pRecord->szName.c_str(), (int)pRecord->pChar->GetTeam(),
																		pRecord->Contribute.m_iSymbolDamageRank + 1, pRecord->Contribute.m_iSymbolDamage,
																		pRecord->Contribute.m_iKillRank + 1, pRecord->Contribute.m_iKill,
																		pRecord->Contribute.m_iDamageRank + 1, pRecord->Contribute.m_iDamage,
																		pRecord->Contribute.m_iAttackCntRank + 1, pRecord->Contribute.m_iAttackCnt );
			g_ChatMgr.SetSystemMsg( szLog );
		}
	}
}

void ioSymbolMode::ContributeConvertTeam( TeamType eTeam )
{
	if( m_vContributeTable.size() < MAX_PLAYER )
		return;

	int i = 0;
	int iCharCnt = m_vCharRecordList.size();
	int iInfoCnt = 0;
	ContributeInfoList vInfo;

	//상징물 타격
	for(i = 0;i < iCharCnt;i++)
	{
		SymbolRecord *pRecord = &m_vCharRecordList[i];
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
		Contribute.m_iContributePoint = pRecord->Contribute.m_iSymbolDamage;
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
			SymbolRecord *pRecord = &m_vCharRecordList[iArray];
			if( !pRecord->Contribute.m_bLastResultSync )
				pRecord->Contribute.m_iContribute = m_vContributeTable[vInfo[i].m_iRank] * 4;
			pRecord->Contribute.m_iSymbolDamageRank = vInfo[i].m_iRank;
		}
	}
	vInfo.clear();

	//KO
	for(i = 0;i < iCharCnt;i++)
	{
		SymbolRecord *pRecord = &m_vCharRecordList[i];
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
			SymbolRecord *pRecord = &m_vCharRecordList[iArray];
			if( !pRecord->Contribute.m_bLastResultSync )
				pRecord->Contribute.m_iContribute += m_vContributeTable[vInfo[i].m_iRank] * 2;
			pRecord->Contribute.m_iKillRank = vInfo[i].m_iRank;
		}
	}
	vInfo.clear();

	//데미지 & 힐
	for(i = 0;i < iCharCnt;i++)
	{
		SymbolRecord *pRecord = &m_vCharRecordList[i];
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
			SymbolRecord *pRecord = &m_vCharRecordList[iArray];
			if( !pRecord->Contribute.m_bLastResultSync )
				pRecord->Contribute.m_iContribute += m_vContributeTable[vInfo[i].m_iRank] * 2;
			pRecord->Contribute.m_iDamageRank = vInfo[i].m_iRank;
		}
	}
	vInfo.clear();

	//유효 타격 횟수
	for(i = 0;i < iCharCnt;i++)
	{
		SymbolRecord *pRecord = &m_vCharRecordList[i];
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
			SymbolRecord *pRecord = &m_vCharRecordList[iArray];
			if( !pRecord->Contribute.m_bLastResultSync )
				pRecord->Contribute.m_iContribute += m_vContributeTable[vInfo[i].m_iRank] * 2;
			pRecord->Contribute.m_iAttackCntRank = vInfo[i].m_iRank;
		}
	}
	vInfo.clear();
}

void ioSymbolMode::ContributeConvert()
{
	ContributeConvertTeam( TEAM_NONE );        //TEAM_NONE는 전체 유저에 대한 기여도 평가
}

int ioSymbolMode::ContributeMyRank( int &rRankValue1, int &rRankValue2, int &rRankValue3, int &rRankValue4 )
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

	//상징물 타격
	for(i = 0;i < iCharCnt;i++)
	{
		SymbolRecord *pRecord = &m_vCharRecordList[i];
		if( !pRecord ) continue;
		if( !pRecord->pChar ) continue;
		if( pRecord->pChar->GetState() == CS_OBSERVER ) continue;
		if( g_BattleRoomMgr.IsObserverUser(pRecord->pChar->GetCharName()) ) continue;

		ContributeInfo Contribute;
		Contribute.m_iRecordArray     = i;
		Contribute.m_iContributePoint = pRecord->Contribute.m_iSymbolDamage;
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

	//KO
	for(i = 0;i < iCharCnt;i++)
	{
		SymbolRecord *pRecord = &m_vCharRecordList[i];
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
		for(rRankValue2 = 0;rRankValue2 < iInfoCnt;rRankValue2++)
		{
			ContributeInfo &kContribute = vInfo[rRankValue2];
			if( kContribute.m_szName == g_MyInfo.GetPublicID() )
				break;
		}
	}

	vInfo.clear();	

	//유효 타격 횟수
	for(i = 0;i < iCharCnt;i++)
	{
		SymbolRecord *pRecord = &m_vCharRecordList[i];
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
		for(rRankValue3 = 0;rRankValue3 < iInfoCnt;rRankValue3++)
		{
			ContributeInfo &kContribute = vInfo[rRankValue3];
			if( kContribute.m_szName == g_MyInfo.GetPublicID() )
				break;
		}
	}

	vInfo.clear();	

	//데미지 & 힐
	for(i = 0;i < iCharCnt;i++)
	{
		SymbolRecord *pRecord = &m_vCharRecordList[i];
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
		for(rRankValue4 = 0;rRankValue4 < iInfoCnt;rRankValue4++)
		{
			ContributeInfo &kContribute = vInfo[rRankValue4];
			if( kContribute.m_szName == g_MyInfo.GetPublicID() )
				break;
		}
	}

	vInfo.clear();	
	return iInfoCnt;
}

void ioSymbolMode::ContributeRoundPercent( DWORD dwCurTime, DWORD dwTotalTime )
{
	//기여도의 100분율
	int iMaxBlueContribute	= 0;
	int iMaxRedContribute	= 0;
	int iMaxBlueUser		= 0;
	int iMaxRedUser			= 0;
	int iCharCnt			= m_vCharRecordList.size();
	int i = 0;
	for(i = 0;i < iCharCnt;i++)
	{
		SymbolRecord *pRecord = &m_vCharRecordList[i];
		if( !pRecord->pChar ) continue;
		if( pRecord->pChar->GetTeam() == TEAM_RED )
		{
			iMaxRedContribute += pRecord->Contribute.m_iContribute;
			iMaxRedUser++;
		}
		else if( pRecord->pChar->GetTeam() == TEAM_BLUE )
		{
			iMaxBlueContribute += pRecord->Contribute.m_iContribute;
			iMaxBlueUser++;
		}		
	}

	for(i = 0;i < iCharCnt;i++)
	{
		SymbolRecord *pRecord = &m_vCharRecordList[i];

		if( pRecord->pChar->GetTeam() == TEAM_RED )
		{
			if( iMaxRedContribute > 0 )
				pRecord->Contribute.m_iPrevContributePer = ((float)pRecord->Contribute.m_iContribute / iMaxRedContribute ) * 100;
		}
		else if( pRecord->pChar->GetTeam() == TEAM_BLUE )
		{
			if( iMaxBlueContribute > 0 )
				pRecord->Contribute.m_iPrevContributePer = ((float)pRecord->Contribute.m_iContribute / iMaxBlueContribute ) * 100;
		}		
	}

	int iBlueCnt       = 0;
	int iRedCnt        = 0;
	int iRoundCount    = min( m_iCurRoundCount, (int)m_vRoundHistory.size() );
	for(i = 0;i < iRoundCount;i++)
	{
		iBlueCnt += m_vRoundHistory[i].iBluePoint;
		iRedCnt  += m_vRoundHistory[i].iRedPoint;
	}

	int iAverageClick  = 8;
	int iWinBonus      = 3;
	int iDrawBonus     = 1;
	int iLoseBonus     = 1;

	TeamType eWinTeam  = TEAM_NONE;
	if( iBlueCnt > iRedCnt )
		eWinTeam = TEAM_BLUE;
	else if( iRedCnt > iBlueCnt )
		eWinTeam = TEAM_RED;

	int iDiffCnt       = min( abs( iBlueCnt - iRedCnt ), iAverageClick );
	int iWinCnt        = iAverageClick + iDiffCnt;
	int iLoseCnt       = iAverageClick - iDiffCnt;
	int iClickCnt      = 0;
	float fPer         = 0.0f;
	if( dwTotalTime > 0 )
		fPer = (float)dwCurTime / dwTotalTime;

	for(i = 0;i < iCharCnt;i++)
	{
		SymbolRecord *pRecord = &m_vCharRecordList[i];

		if( pRecord->pChar->GetTeam() == eWinTeam )
			iClickCnt = iWinCnt;
		else 
			iClickCnt = iLoseCnt;

		if( eWinTeam == TEAM_NONE )  //비겼을 경우 보너스.
			iClickCnt += iDrawBonus;
		else if( pRecord->pChar->GetTeam() == eWinTeam )       // 승리팀 보너스.
			iClickCnt += iWinBonus;
		else	//패배팀 보너스
			iClickCnt += iLoseBonus;

		if( pRecord->pChar->GetTeam() == TEAM_BLUE )
			pRecord->Contribute.m_iPrevContributePer = (float)((float)(pRecord->Contribute.m_iPrevContributePer * ( iMaxBlueUser * iClickCnt )) * 0.35f) * fPer;
		else if( pRecord->pChar->GetTeam() == TEAM_RED )
			pRecord->Contribute.m_iPrevContributePer = (float)((float)(pRecord->Contribute.m_iPrevContributePer * ( iMaxRedUser * iClickCnt )) * 0.35f) * fPer;		
	}	
}

void ioSymbolMode::ContributeRoundAllPercent( DWORD dwCurTime, DWORD dwTotalTime )
{
	//기여도의 100분율
	int iMaxContribute	= 0;
	int iCharCnt		= m_vCharRecordList.size();
	int iPlayCharCnt	= 0;

	int i = 0;
	for(i = 0;i < iCharCnt;i++)
	{
		SymbolRecord *pRecord = &m_vCharRecordList[i];
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
			SymbolRecord *pRecord = &m_vCharRecordList[i];
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

void ioSymbolMode::DamageContribute( const ioHashString szAttacker, const ioHashString szTarget, float fDamage, DWORD dwItemCode, int iWeaponType )
{
	if( szAttacker.IsEmpty() ) return;

	SymbolRecord *pRecord = FindRecordCharName( szAttacker );
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

	float fDamagePoint = (float)((fDamage + fBonusDamage) * 100) * pow( ContributeTeamPerA( pRecord->pChar->GetTeam(), 8.0f, FLOAT1 ), 2 );
	pRecord->Contribute.m_iDamage += (float)fDamagePoint * fAddDamagePoint;	

	if( fDamagePoint > 0.0f && !pAttacker->IsOwnerChar() )
	{
		CheckNonePlayTime( pAttacker );
	}
	
	ContributeConvert();
	ContributeRoundAllPercent();
	UpdateRoundInfoList();
	CheckTotalChamp();

	g_AwardMgr.AddDamage( pRecord->szName, fDamagePoint, fDamage );
}

void ioSymbolMode::SymbolDamageContribute( TeamType eSymbolTeam, const ioHashString szAttacker, float fDamage, int iWeaponType )
{
	if( szAttacker.IsEmpty() ) return;

	SymbolRecord *pRecord = FindRecordCharName( szAttacker );
	if( !pRecord || !pRecord->pChar ) return;

	float fBonusDamage = 0;
	switch( iWeaponType )
	{
	case ioWeapon::WT_BLIZZARD:
		fBonusDamage = fDamage;
		break;
	}

	float fDamagePoint = (float)((fDamage+fBonusDamage) * 100) * ContributeTeamPer( pRecord->pChar->GetTeam(), FLOAT1 );
	pRecord->Contribute.m_iSymbolDamage += fDamagePoint;	
	
	ContributeConvert();
	ContributeRoundAllPercent();
	UpdateRoundInfoList();
	CheckTotalChamp();

	if( pRecord->pChar->GetTeam() == eSymbolTeam )
		g_AwardMgr.AddTeamStoneDamage( pRecord->szName, fDamagePoint, fDamage );
	else 
		g_AwardMgr.AddOtherTeamStoneDamage( pRecord->szName, fDamagePoint, fDamage );

	//
	if( pRecord->pChar->IsOwnerChar() )
	{
		if( pRecord->pChar->GetTeam() == TEAM_BLUE ||
			pRecord->pChar->GetTeam() == TEAM_RED )
		{
			g_QuestMgr.QuestCompleteTerm( QC_STONE_ATTACK, fDamagePoint, (int)pRecord->pChar->GetTeam(), (int)eSymbolTeam );
		}
	}
}

void ioSymbolMode::SymbolDieContribute( const ioHashString szKiller )
{
	if( szKiller.IsEmpty() ) return;

	SymbolRecord *pRecord = FindRecordCharName( szKiller );
	if( !pRecord || !pRecord->pChar ) return;

	int iDamagePoint = 250.0f * ContributeTeamPer( pRecord->pChar->GetTeam(), FLOAT1 );
	pRecord->Contribute.m_iSymbolDamage += iDamagePoint;	   
	
	ContributeConvert();
	ContributeRoundAllPercent();
	UpdateRoundInfoList();
	CheckTotalChamp();
}

void ioSymbolMode::KillContribute( const ioHashString szKiller, const ioHashString szKilled, DWORD dwItemCode )
{
	if( szKilled.IsEmpty() ) return;

	int iKillPoint = 100;

	float fAddKillPoint = FLOAT1;
	if( IsContributeAddPointItemCode( dwItemCode ) )
		fAddKillPoint = 1.1f;

	if( szKiller.IsEmpty() || szKiller == szKilled )     //자살
	{
		SymbolRecord *pRecord = FindRecordCharName( szKilled );
		if( !pRecord || !pRecord->pChar ) return;
		
		pRecord->Contribute.m_iKill -= iKillPoint;
	}
	else
	{
		SymbolRecord *pKillerRecord = FindRecordCharName( szKiller );
		SymbolRecord *pKilledRecord = FindRecordCharName( szKilled );
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

void ioSymbolMode::AttackCntContribute( const ioHashString szAttacker, const ioHashString szTarget, DWORD dwItemCode, int iAttackValue/* = 100 */ )
{
	if( szAttacker.IsEmpty() || szTarget.IsEmpty() ) return;

	SymbolRecord *pAttackerRecord = FindRecordCharName( szAttacker );
	SymbolRecord *pTargetRecord = FindRecordCharName( szTarget );

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

void ioSymbolMode::FillLastPlayRecordInfo( SP2Packet &rkPacket )
{
	int iCharCnt = m_vCharRecordList.size();
	rkPacket << iCharCnt;
	for(int i = 0;i < iCharCnt;i++)
	{
		SymbolRecord *pRecord = &m_vCharRecordList[i];
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

void ioSymbolMode::FillPlayRecordInfo( SP2Packet &rkPacket )
{
	int iCharCnt = m_vCharRecordList.size();
	rkPacket << iCharCnt;
	for(int i = 0;i < iCharCnt;i++)
	{
		SymbolRecord *pRecord = &m_vCharRecordList[i];
		rkPacket << pRecord->pChar->GetCharName();
		rkPacket << pRecord->Contribute.m_iDamage;
		rkPacket << pRecord->Contribute.m_iAttackCnt;
		rkPacket << pRecord->Contribute.m_iKill;
		rkPacket << pRecord->Contribute.m_iSymbolDamage;
		rkPacket << pRecord->Contribute.m_iPrevContributePer;
		rkPacket << pRecord->GetUniqueTotalKill();
		rkPacket << pRecord->GetUniqueTotalDeath();
		rkPacket << g_VictoriesMgr.GetVictories( pRecord->pChar->GetCharName() );
		rkPacket << pRecord->GetNoDeathKillCount();
	}
}

void ioSymbolMode::ApplyPlayRecordInfo( SP2Packet &rkPacket )
{
	int iCharCnt;
	rkPacket >> iCharCnt;
	for(int i = 0;i < iCharCnt;i++)
	{
		ioHashString szName;
		int iDamage, iAttackCnt, iKill, iSymbolDamage, iPrevContributePer, iUniqueTotalKill, iUniqueTotalDeath, iVictories, iNoDeathKillCount;
		rkPacket >> szName >> iDamage >> iAttackCnt >> iKill >> iSymbolDamage >> iPrevContributePer >> iUniqueTotalKill >> iUniqueTotalDeath >> iVictories >> iNoDeathKillCount;

		SymbolRecord *pRecord = FindRecordCharName( szName );
		if( pRecord )
		{
			pRecord->Contribute.m_iDamage			= iDamage;
			pRecord->Contribute.m_iAttackCnt		= iAttackCnt;
			pRecord->Contribute.m_iKill				= iKill;
			pRecord->Contribute.m_iSymbolDamage		= iSymbolDamage;
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

void ioSymbolMode::ContributeResultSync( const ioHashString &szName, int iContribute )
{
	SymbolRecord *pRecord = FindRecordCharName( szName );
	if( !pRecord ) return;

	pRecord->Contribute.m_bLastResultSync = true;
	pRecord->Contribute.m_iContribute     = iContribute;
}

bool ioSymbolMode::ProcessTCPPacket( SP2Packet &rkPacket )
{
	ErrorReport::SetPosition( 1020, 20 );

	if( ioPlayMode::ProcessTCPPacket( rkPacket ) )
		return true;

	ErrorReport::SetPosition( 1020, 21 );

	switch( rkPacket.GetPacketID() )
	{
	case STPK_SYMBOL_DIE:
		ErrorReport::SetPosition( 1020, 22 );
		OnSymbolDie( rkPacket );
		ErrorReport::SetPosition( 1020, 23 );
		return true;
	case STPK_SYMBOL_ACTIVITY:
		ErrorReport::SetPosition( 1020, 24 );
		OnSymbolActivity( rkPacket );
		ErrorReport::SetPosition( 1020, 25 );
		return true;
	case STPK_UPDATE_SCORE_GAUGE:
		OnScoreGaugeUpdate( rkPacket );
		return true;
	}

	return false;
}

void ioSymbolMode::OnSymbolDie( SP2Packet &rkPacket )
{
	ioHashString szKiller;
	rkPacket >> szKiller;

	ioBaseChar *pKiller = m_pCreator->GetBaseChar( szKiller );
	if( !pKiller )	return;

	int iSymbolIdx;
	rkPacket >> iSymbolIdx;

	ioSymbolStruct *pSymbol = GetSymbol( iSymbolIdx );
	if( !pSymbol )	return;

	float fRevivalHP;
	rkPacket >> fRevivalHP;
	pSymbol->SetRevivalHP( fRevivalHP );

	ProcessSymbolKill( pSymbol, pKiller );

	int iRevivalCnt;
	rkPacket >> iRevivalCnt;
	pSymbol->SetRevivalCount( iRevivalCnt );

	int iTeamType, iAscendancyTeam;
	rkPacket >> iTeamType;

	rkPacket >> iAscendancyTeam;
	m_AscendancyTeam = (TeamType)iAscendancyTeam;

	rkPacket >> m_fCurRecoverGauge;
	rkPacket >> m_fBlueGauge;
	rkPacket >> m_fRedGauge;

	SymbolDestroyAnnounce( (TeamType)iTeamType );
	SymbolDieContribute( szKiller );

	// 상징물 파괴
	g_ChatMgr.SetSymbolDieMsg( pKiller->GetTeam(), szKiller );
}

void ioSymbolMode::OnSymbolActivity( SP2Packet &rkPacket )
{
	int iPreActive = GetCurActiveSymbolCnt();

	int iSymbolCnt;
	rkPacket >> iSymbolCnt;

	iSymbolCnt = min( iSymbolCnt, (int)m_vSymbolList.size() );
	for( int i=0 ; i<iSymbolCnt ; i++ )
	{
		ioSymbolStruct *pSymbol = m_vSymbolList[i];

		bool bActive;
		rkPacket >> bActive;

		if( pSymbol->GetSymbolActivity() != bActive )
		{
			pSymbol->SetSymbolActivity( bActive );
		}
	}
}

void ioSymbolMode::OnScoreGaugeUpdate( SP2Packet &rkPacket )
{
	rkPacket >> m_fCurRecoverGauge;
}

bool ioSymbolMode::ProcessUDPPacket( SP2Packet &rkPacket )
{
	ErrorReport::SetPosition( 1020, 26 );

	if( ioPlayMode::ProcessUDPPacket( rkPacket ) )
		return true;

	ErrorReport::SetPosition( 1020, 27 );

	switch( rkPacket.GetPacketID() )
	{
	case CUPK_SYMBOL_DAMAMGED:
		ErrorReport::SetPosition( 1020, 30 );
		OnSymbolDamaged( rkPacket );
		ErrorReport::SetPosition( 1020, 31 );
		return true;
	}
	
	return false;
}

void ioSymbolMode::ApplyModeInfo( SP2Packet &rkPacket )
{
	rkPacket >> m_iCurRoundCount;
	rkPacket >> m_iMaxRoundCount;

	rkPacket >> m_dwRoundDuration;

	rkPacket >> m_iBlueRoundWinCnt;
	rkPacket >> m_iRedRoundWinCnt;

	rkPacket >> m_iBluePosArray;
	rkPacket >> m_iRedPosArray;
	//

	rkPacket >> m_iBlueSymbolCnt;
	rkPacket >> m_iRedSymbolCnt;

	int iAscendancyTeam;
	rkPacket >> iAscendancyTeam;
	m_AscendancyTeam = (TeamType)iAscendancyTeam;

	rkPacket >> m_fBlueGauge;
	rkPacket >> m_fRedGauge;
	rkPacket >> m_fCurRecoverGauge;

	int iSymbolCnt;
	int iMaxSymbolCnt = m_vSymbolList.size();
	rkPacket >> iSymbolCnt;

	bool bActive;
	int iTeamType, iRevivalCnt;

	for( int i=0 ; i<iSymbolCnt ; i++ )
	{
		rkPacket >> bActive >> iTeamType >> iRevivalCnt;

		if( !COMPARE( i, 0, iMaxSymbolCnt ) )
			continue;

		ioSymbolStruct *pSymbol = m_vSymbolList[i];
		pSymbol->SetSymbolActivity( bActive );
		pSymbol->SetTeam( (TeamType)iTeamType );
		pSymbol->SetRevivalCount( iRevivalCnt );
	}

	m_vRoundHistory.erase( m_vRoundHistory.begin(), m_vRoundHistory.end() );
	m_vRoundHistory.reserve( m_iCurRoundCount );

	for(int j = 0; j < m_iCurRoundCount; j++)
	{
		RoundHistory rh;
		rkPacket >> rh.iBluePoint;
		rkPacket >> rh.iRedPoint;
		m_vRoundHistory.push_back( rh );
	}

	UpdateRoundStateWnd();
	UpdateTimeScoreWnd();
}

void ioSymbolMode::ApplyCharModeInfo( const ioHashString &rkName,
									  SP2Packet &rkPacket,
									  bool bDieCheck )
{
	bool bRecord;
	rkPacket >> bRecord;

	if( !bRecord ) return;

	SymbolRecord *pRecord = FindRecordCharName( rkName );
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

void ioSymbolMode::GetModeSpecialInfo( SP2Packet &rkPacket )
{
	ioPlayMode::GetModeSpecialInfo( rkPacket );

	int iSymbolCnt = m_vSymbolList.size();
	rkPacket << iSymbolCnt;

	for( int i=0 ; i<iSymbolCnt ; i++ )
	{
		rkPacket << m_vSymbolList[i]->GetHP().m_fCurValue;
		rkPacket << m_vSymbolList[i]->GetHP().m_fMaxValue;
		rkPacket << m_vSymbolList[i]->GetCurDecreaseRate();
	}
}

void ioSymbolMode::OnModeSpecialInfo( SP2Packet &rkPacket )
{
	ioPlayMode::OnModeSpecialInfo( rkPacket );

	int iSymbolCnt;
	rkPacket >> iSymbolCnt;
	
	iSymbolCnt = min( iSymbolCnt, (int)m_vSymbolList.size() );
	for( int i=0 ; i<iSymbolCnt ; i++ )
	{
		float fCurHP, fMaxHP, fDecreaseCurRate;
		rkPacket >> fCurHP >> fMaxHP >> fDecreaseCurRate;

		m_vSymbolList[i]->SetHP( fCurHP, fMaxHP, fDecreaseCurRate );
	}
}

void ioSymbolMode::OnSymbolDamaged( SP2Packet &rkPacket )
{
	ioHashString szAttacker;
	rkPacket >> szAttacker;

	ioBaseChar *pAttacker = GetRecordChar( szAttacker );
	if( !pAttacker )
	{
		LOG.PrintTimeAndLog( 0, "ioSymbolMode::OnSymbolDamaged - %s Not Exist", szAttacker.c_str() );
		return;
	}

	int iSymbolArray;
	rkPacket >> iSymbolArray;
	
	ioSymbolStruct *pSymbol = GetSymbol( iSymbolArray );
	if( pSymbol )
	{
		pSymbol->OnSymbolDamaged( rkPacket, pAttacker );
	}
	else
	{
		LOG.PrintTimeAndLog( 0, "ioSymbolMode::onSymbolDamaged - %d Not Exist Symbol", iSymbolArray );
	}
}

void ioSymbolMode::CheckSymbolActivity()
{
	if( P2PNetwork::IsNetworkPlaying() )
		return;

	int iCurSymbolActiveLv = GetCurSymbolActiveLevel();
	if( iCurSymbolActiveLv == 0 )
		return;

	int iSymbolCnt = m_vSymbolList.size();
	int iPreActive = GetCurActiveSymbolCnt();

	int iCurActive = 0;
	for( int i=0 ; i<iSymbolCnt ; i++ )
	{
		ioSymbolStruct *pSymbol = m_vSymbolList[i];

		if( pSymbol->GetSymbolPriority() <= iCurSymbolActiveLv )
		{
			if( !pSymbol->GetSymbolActivity() )
			{
				pSymbol->SetSymbolActivity( true );
			}

			iCurActive++;
		}
		else
		{
			if( pSymbol->GetSymbolActivity() )
			{
				pSymbol->SetSymbolActivity( false );
			}
		}
	}

	if( iPreActive > iCurActive )
	{
		SetSymbolActiveEvent( false, iPreActive - iCurActive );
	}
	else if( iPreActive < iCurActive )
	{
		SetSymbolActiveEvent( true, iCurActive - iPreActive );
	}
}

int ioSymbolMode::GetCurSymbolActiveLevel()
{
	int iRecordCnt = m_vCharRecordList.size();
	if( iRecordCnt <= 0 )
		return 0;

	int iMaxLimitsLevelCnt = m_vSymbolActiveCharCntList.size();
	for( int i=0 ; i<iMaxLimitsLevelCnt ; i++ )
	{
		if( iRecordCnt <= m_vSymbolActiveCharCntList[i] )
			return i+1;
	}

	return ( iMaxLimitsLevelCnt + 1 );
}

int ioSymbolMode::GetCurActiveSymbolCnt()
{
	int iActiveSymbolCnt = 0;

	int iSymbolCnt = m_vSymbolList.size();
	for( int i=0 ; i<iSymbolCnt ; i++ )
	{
		if( m_vSymbolList[i]->GetSymbolActivity() )
			iActiveSymbolCnt++;
	}

	return iActiveSymbolCnt;
}

void ioSymbolMode::SetSymbolActiveEvent( bool bActive, int iChangeCnt )
{
}

void ioSymbolMode::UpdateEndScoreWndTime()
{
	if( m_ModeState != MS_RESULT )	return;

	DWORD dwPastTime = FRAMEGETTIME() - m_dwResultStartTime;

	DWORD dwRemainTime = 0;
	if( m_dwCurResultShowDuration > dwPastTime )
	{
		dwRemainTime = ( m_dwCurResultShowDuration - dwPastTime ) / 1000;
	}
}

void ioSymbolMode::ShowEndScoreWnd( bool bShow )
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

int ioSymbolMode::GetRoundWinCnt( TeamType eTeamType )
{
	int iBlueCnt, iRedCnt;
	iBlueCnt = iRedCnt = 0;
	for( int i = 0 ; i < m_iCurRoundCount ; i++ )
	{
		iBlueCnt += m_vRoundHistory[i].iBluePoint;
		iRedCnt  += m_vRoundHistory[i].iRedPoint;
	}

	switch( eTeamType )
	{
	case TEAM_RED:
		return iRedCnt;
	case TEAM_BLUE:
		return iBlueCnt;
	}

	return 0;
}

void ioSymbolMode::SetResultScoreWnd()
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

void ioSymbolMode::SetTotalScoreWnd( IntVec &rvClassTypeList,
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

//	pTotal->SetMaxRound( m_vRoundHistory.size() );
//	for( i = 0 ; i < (int)m_vRoundHistory.size() ; i++ )
//		pTotal->SetRoundHistory( i, m_vRoundHistory[i] );

	pTotalResult->SetTeamScore( GetOwnerTeam(), m_iBlueRoundWinCnt, m_iRedRoundWinCnt );
	SetTotalResultBriefingWnd();
}

bool ioSymbolMode::IsEnableSymbolDamage( ioSymbolStruct *pSymbol, TeamType eAttackTeam )
{
	if( !pSymbol ) return false;
	if( pSymbol->GetTeam() == TEAM_NONE ) return true;

	if( m_bNoLimiteSymbol )
		return true;

	// 상징물 하나도 없으면 자기팀 시작점 상징물 타격 가능
	if( GetTeamSymbolCnt(eAttackTeam) == 0 )
	{
		ioSymbolStruct *pStartSymbol = GetStartPosSymbol( eAttackTeam );
		if( pStartSymbol && pStartSymbol == pSymbol )
			return true;
	}

	int iSymbolCnt = m_vSymbolList.size();
	int iSymbolIndex, iBeforeSymbolIndex, iAfterSymbolIndex;
	
	iSymbolIndex = pSymbol->GetSymbolArrayIdx();
	iBeforeSymbolIndex = iSymbolIndex -1;
	iAfterSymbolIndex = iSymbolIndex + 1;

	if( COMPARE( iBeforeSymbolIndex, 0, iSymbolCnt ) )
	{
		if( !m_vSymbolList[iBeforeSymbolIndex]->GetSymbolActivity() )
		{
			iBeforeSymbolIndex--;

			if( COMPARE( iBeforeSymbolIndex, 0, iSymbolCnt ) && 
				m_vSymbolList[iBeforeSymbolIndex]->GetTeam() == eAttackTeam )
				return true;
		}
		else
		{
			if( m_vSymbolList[iBeforeSymbolIndex]->GetTeam() == eAttackTeam )
				return true;
		}
	}

	if( COMPARE( iAfterSymbolIndex, 0, iSymbolCnt ) )
	{
		if( !m_vSymbolList[iAfterSymbolIndex]->GetSymbolActivity() )
		{
			iAfterSymbolIndex++;

			if( COMPARE( iAfterSymbolIndex, 0, iSymbolCnt ) && 
				m_vSymbolList[iAfterSymbolIndex]->GetTeam() == eAttackTeam )
				return true;
		}
		else
		{
			if( m_vSymbolList[iAfterSymbolIndex]->GetTeam() == eAttackTeam )
				return true;
		}
	}

	return false;
}

void ioSymbolMode::NotifySymbolWounded( ioSymbolStruct *pSymbol, ioBaseChar *pAttacker, float fDamage )
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( m_LastSymbolTeam != TEAM_NONE )
	{
		m_dwLastSymbolAttackedTime = dwCurTime;
		return;
	}

	TeamType eSymbolTeam = pSymbol->GetTeam();
	if( eSymbolTeam == TEAM_NONE )
		return;

	bool bNowAttackedAnnounce = false;
	DWORD dwMinAnnounceTime   = 5000;

	if( eSymbolTeam == TEAM_BLUE )
	{
		if( m_dwBlueSymbolAttackedTime + dwMinAnnounceTime < dwCurTime )
			bNowAttackedAnnounce = true;

		m_dwBlueSymbolAttackedTime = dwCurTime;
	}
	else
	{
		if( m_dwRedSymbolAttackedTime + dwMinAnnounceTime < dwCurTime )
			bNowAttackedAnnounce = true;

		m_dwRedSymbolAttackedTime = dwCurTime;
	}

	if( bNowAttackedAnnounce )
	{
		if( eSymbolTeam == GetOwnerTeam() )
		{
			g_SoundMgr.PlaySound( m_SymbolDamaged );
		}
	}

	if( eSymbolTeam != pAttacker->GetTeam() )
	{
		float fCurHP = pSymbol->GetHP().m_fCurValue;
		float fHalfMaxHP = pSymbol->GetHP().m_fMaxValue * FLOAT05;

		if( fCurHP + fDamage >= fHalfMaxHP && fCurHP < fHalfMaxHP )
		{
			SetMiniMapEvent( pSymbol->GetWorldPosition(), MiniMap::EVENT_SYMBOL_DAMAGED_HALF, eSymbolTeam );
		}
	}
}

void ioSymbolMode::ProcessScoreGauge()
{
	if( m_AscendancyTeam == TEAM_NONE )
	{
		m_dwTicCheckTime = 0;
		return;
	}

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwTicCheckTime == 0 )
		m_dwTicCheckTime = dwCurTime;

	DWORD dwGapTime = dwCurTime - m_dwTicCheckTime;
	
	while( dwGapTime > m_dwRecoverTic )
	{
		dwGapTime -= m_dwRecoverTic;

		if( m_AscendancyTeam == TEAM_BLUE )
		{
			if( m_fRedGauge > 0.0f )
			{
				if( m_fRedGauge > m_fCurRecoverGauge )
				{
					m_fRedGauge -= m_fCurRecoverGauge;
				}
				else
				{
					m_fBlueGauge += m_fCurRecoverGauge - m_fRedGauge;
					m_fRedGauge = 0.0f;
				}
			}
			else
			{
				m_fBlueGauge += m_fCurRecoverGauge;
			}

			if( m_fBlueGauge >= m_fMaxGauge )
				m_fBlueGauge = m_fMaxGauge;
		}
		else if( m_AscendancyTeam == TEAM_RED )
		{
			if( m_fBlueGauge > 0.0f )
			{
				if( m_fBlueGauge > m_fCurRecoverGauge )
				{
					m_fBlueGauge -= m_fCurRecoverGauge;
				}
				else
				{
					m_fRedGauge += m_fCurRecoverGauge - m_fBlueGauge;
					m_fBlueGauge = 0.0f;
				}
			}
			else
			{
				m_fRedGauge += m_fCurRecoverGauge;
			}

			if( m_fRedGauge >= m_fMaxGauge )
				m_fRedGauge = m_fMaxGauge;
		}
	}

	m_dwTicCheckTime = dwCurTime - dwGapTime;
}

int ioSymbolMode::GetTeamSymbolCnt( TeamType eTeam )
{
	int iRedCnt, iBlueCnt, iNoneCnt;
	iRedCnt = iBlueCnt = iNoneCnt = 0;

	int iSymbolCnt = m_vSymbolList.size();
	for( int i=0; i < iSymbolCnt; i++ )
	{
		ioSymbolStruct *pSymbol = m_vSymbolList[i];
		if( !pSymbol ) continue;
		if( !pSymbol->GetSymbolActivity() ) continue;

		if( pSymbol->GetTeam() == TEAM_RED )
			iRedCnt++;
		else if( pSymbol->GetTeam() == TEAM_BLUE )
			iBlueCnt++;
		else
			iNoneCnt++;
	}

	int iResult = 0;
	switch( eTeam )
	{
	case TEAM_RED:
		iResult = iRedCnt;
		break;
	case TEAM_BLUE:
		iResult = iBlueCnt;
		break;
	case TEAM_NONE:
		iResult = iNoneCnt;
		break;
	}

	return iResult;
}

ioGameEntity* ioSymbolMode::GetGuideTarget( const D3DXVECTOR3& vArrowPos )
{
	ioBaseChar *pOwner = GetOwnerChar();
	if( !pOwner ) 	return NULL;

	ioSymbolStruct* pTarget = NULL;

	// 가장 가까운 파괴가능한 상징물
	float fSymbolDistSq = std::numeric_limits<float>::infinity();
	int iSymbolCnt = m_vSymbolList.size();
	for( int i=0; i < iSymbolCnt; i++ )
	{
		ioSymbolStruct *pSymbol = m_vSymbolList[i];
		if( !pSymbol ) continue;
		if( !pSymbol->GetSymbolActivity() ) continue;
		if( pSymbol->GetTeam() == pOwner->GetTeam() ) continue;

		if( !IsEnableSymbolDamage( pSymbol, pOwner->GetTeam() ) ) continue;

		D3DXVECTOR3 vDiff = pSymbol->GetWorldPosition() - vArrowPos;
		float fCurDistSq = D3DXVec3LengthSq( &vDiff );
		if( fCurDistSq < fSymbolDistSq )
		{
			pTarget = pSymbol;
			fSymbolDistSq = fCurDistSq;
		}
	}

	if( pTarget )
		return pTarget;

	// 팀 마지막 상징물
	if( pOwner->GetTeam() == TEAM_RED )
		pTarget = GetStartPosSymbol( TEAM_BLUE );
	else if( pOwner->GetTeam() == TEAM_BLUE )
		pTarget = GetStartPosSymbol( TEAM_RED );

	return pTarget;
}

void ioSymbolMode::CheckEndSceneLookAt()
{
	m_szEndLookCharName.Clear();

	if( m_pLastChangeSymbol )
	{
		m_vEndLookAt = m_pLastChangeSymbol->GetWorldPosition();
		m_pLastChangeSymbol->SetEndFocusEffect( GetRoundEndEffect() );
		return;
	}

	ioSymbolStruct *pLowHP = NULL;
	int iSymbolCnt = m_vSymbolList.size();
	for( int i=0; i < iSymbolCnt; i++ )
	{
		if( m_vSymbolList[i]->GetCurHPRate() >= FLOAT1 )
			continue;

		if( pLowHP )
		{
			if( pLowHP->GetCurHPRate() > m_vSymbolList[i]->GetCurHPRate() )
				pLowHP = m_vSymbolList[i];
		}
		else
			pLowHP = m_vSymbolList[i];
	}

	if( pLowHP )
	{
		m_vEndLookAt = pLowHP->GetWorldPosition();
		pLowHP->SetEndFocusEffect( GetRoundEndEffect() );
		return;
	}

	ioBaseChar *pOwner = m_pCreator->GetOwnerChar();
	if( pOwner )
	{
		m_vEndLookAt = pOwner->GetWorldPosition();
		m_szEndLookCharName = pOwner->GetCharName();
		pOwner->SetEndFocusEffect( GetRoundEndEffect() );
	}
	else
	{
		m_vEndLookAt = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	}
}

void ioSymbolMode::ClearModeData()
{
	m_fBlueGauge = 0.0f;
	m_fRedGauge = 0.0f;

	m_pLastChangeSymbol = NULL;

	g_SoundMgr.AllSoundStop();
	g_SoundMgr.StopStream();
	m_pCreator->HidingAllFieldItem();
}

int ioSymbolMode::GetCurRank( const ioHashString& szCharName )
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

int ioSymbolMode::GetContributePerChar( const ioHashString& szName )
{
	int iCharCnt = m_vCharRecordList.size();

	for(int i = 0;i < iCharCnt;i++)
	{
		SymbolRecord *pRecord = &m_vCharRecordList[i];
		if( !pRecord )	continue;
		if( !pRecord->pChar ) continue;

		if( pRecord->pChar->GetCharName() == szName )
		{
			return pRecord->Contribute.m_iPrevContributePer;
		}
	}

	return 0;
}
