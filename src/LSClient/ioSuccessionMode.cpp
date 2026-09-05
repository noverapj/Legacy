

#include "stdafx.h"

#include "ioSuccessionMode.h"

#include "GUI/MiniMap.h"
#include "GUI/CenterInfoWnd.h"
#include "GUI/MiniSoldierSelectWnd.h"
#include "GUI/TimeScoreWnd.h"
#include "GUI/ioObserverWnd.h"
#include "GUI/BriefingWnd.h"
#include "GUI/TotalResultMainWnd.h"
#include "GUI/RoundStateMainWnd.h"
#include "GUI/ioSuccessionModeWnd.h"

#include "ioLadderTeamMgr.h"

ioSuccessionMode::ioSuccessionMode( ioPlayStage *pCreator ) : ioPlayMode( pCreator )
{
	m_iBlueTeamCnt = 0;
	m_iRedTeamCnt = 0;

	m_ModeType = MT_SUCCESSION;

	m_vCharRecordList.reserve( 10 );
	m_dwRevengeWndShowTime = 0;
}

ioSuccessionMode::~ioSuccessionMode()
{
	m_bZeroHP = false;
	ClearMode();
	m_dwRevengeWndShowTime = 0;
}

void ioSuccessionMode::InitMode( int iMapIndex )
{
	m_iModeMapIndex = iMapIndex;

	LoadMap();
	LoadPushStructs();
	LoadMachineStruct();
	LoadFuntionalObjects();
	LoadCharacters();
	LoadDisableSkillList();
	LoadStartBuffList();
	LoadModeGrowthRate();

	ioINILoader kLoader( GetModeINIFileName( m_ModeType ) );
	char szBuf[MAX_PATH];
	kLoader.SetTitle_e( "common_info" );
	m_dwRoundDuration = kLoader.LoadInt_e( "round_time", 300000 );
	m_dwCurRoundDuration = m_dwRoundDuration;

	m_fScoreWarningRate = kLoader.LoadFloat_e( "score_warning_rate", FLOAT05 );
	m_dwReserveRevengeWndShowTime = kLoader.LoadInt_e( "revenge_wnd_rserve_show_time", 0 );
	m_dwRevengeRequestWaitTime = kLoader.LoadInt_e( "revenge_request_wait_time", 0 );
	kLoader.LoadString_e( "revenge_request_wait_ment", "", szBuf, MAX_PATH );
	m_szRevengeRequestWaitMent = szBuf;
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

	kLoader.LoadString_e( "ui_info", "1point_up_emoticon", "", szBuf, MAX_PATH );
	m_PointUpEmoticon = szBuf;

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

	g_GUIMgr.ShowWnd( SUCCESSION_BATTLE_VERSUS_SCENE_WND );

	m_dwRevengeWndShowTime = 0;
}

void ioSuccessionMode::LoadModeResource( ioINILoader &rkLoader )
{	
	TimeScoreWnd *pTimeScoreWnd = dynamic_cast<TimeScoreWnd *>( g_GUIMgr.AddWnd( "XML/TimeScoreWnd.xml") );
	if(pTimeScoreWnd)
		pTimeScoreWnd->SetModeType( GetModeType() );

	g_ModeUIController.SetKOMsgWnd( g_GUIMgr.AddWnd( "XML/kochatmsgwnd.xml") );

	g_ModeUIController.SetCenterKillInfoWnd( NULL ); 

	CenterInfoWnd *pCenterInfoWnd = dynamic_cast<CenterInfoWnd *>( g_GUIMgr.AddWnd("XML/CenterInfoWnd.xml") );
	if(pCenterInfoWnd)
	{
		g_ModeUIController.SetCenterInfoWnd( pCenterInfoWnd );
	}

	MiniSoldierSelectWnd *pMiniSoldierWnd = dynamic_cast<MiniSoldierSelectWnd*>(g_GUIMgr.AddWnd( "XML/MiniSoldierSelectWnd.xml" ));
	if(pMiniSoldierWnd)
		pMiniSoldierWnd->SetPlayStage( m_pCreator );

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

void ioSuccessionMode::ProcessMode()
{
	ErrorReport::SetPosition( 1030, 0 );
	ProcessSelectCharacter();
	ErrorReport::SetPosition( 1030, 1 );
	ProcessGUI();
	ErrorReport::SetPosition( 1030, 2 );

	switch( m_ModeState )
	{
	case MS_READY:
		ErrorReport::SetPosition( 1030, 3 );
		ProcessReadyState();
		ErrorReport::SetPosition( 1030, 4 );
		CheckModeMapSound();
		break;
	case MS_PLAY:
		ProcessMapAreaWeapon();
		ErrorReport::SetPosition( 1030, 5 );
		ProcessPlayState();
		ErrorReport::SetPosition( 1030, 6 );
		CheckModeMapSound();
		ErrorReport::SetPosition( 1030, 7 );
		g_EventMgr.Process();
		break;
	case MS_RESULT:
		ErrorReport::SetPosition( 1030, 8 );
		ProcessResultState();
		ErrorReport::SetPosition( 1030, 9 );
		break;
	}
}

void ioSuccessionMode::ProcessGUI()
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

	if ( m_dwRevengeWndShowTime > 0 && m_dwRevengeWndShowTime < FRAMEGETTIME() )
	{
		m_dwRevengeWndShowTime = 0;
		if( !m_bOwnerWin )
			g_GUIMgr.ShowWnd( SUCCESSION_BATTLE_REQUEST_WND );
		else
		{
			WaitEffectWnd *pEffect = dynamic_cast<WaitEffectWnd*>( g_GUIMgr.FindWnd( WAIT_EFFECT_WND ) );
			if ( pEffect )
				pEffect->StartSearch( NULL, true, m_dwRevengeRequestWaitTime, m_szRevengeRequestWaitMent );
		}
	}
}

void ioSuccessionMode::UpdateMiniMapSpotPoint()
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
		if( pChar->IsDropZoneDownState() )
			pMiniMap->AddPoint( vPos.x, vPos.z, MiniMap::SPOT_DROP_USER, pChar->GetTeam(), 0 );
		else
			pMiniMap->AddPoint( vPos.x, vPos.z, MiniMap::SPOT_USER, pChar->GetTeam(), dwGapTime );		
	}

	//MyChar..
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

	// pDummy
	const GameEntityList &rkObjectList = m_pCreator->GetGameEntityList();
	GameEntityList::const_iterator iter_b;
	for( iter_b = rkObjectList.begin() ; iter_b!=rkObjectList.end() ; ++iter_b )
	{
		ioDummyChar *pDummy = ToDummyChar( *iter_b );
		if( pDummy && pDummy->IsVisibleMiniMap() )
		{
			vPos = pDummy->GetWorldPosition();
			pMiniMap->AddPoint( vPos.x, vPos.z, MiniMap::SPOT_USER, pDummy->GetTeam(), 0 );
		}
	}
}

void ioSuccessionMode::UpdateTimeScoreWnd()
{
	TimeScoreWnd *pTimeScoreWnd = dynamic_cast<TimeScoreWnd *>( g_GUIMgr.FindWnd( TIME_SCORE_WND ) );
	if(!pTimeScoreWnd) return;

	bool bRedWarning = false;
	bool bBlueWarning = false;

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
			pTimeScoreWnd->UpdateTime( m_dwCurRoundDuration - dwTimeGap, m_dwRoundDuration );

			ProcessWarningSound( m_dwCurRoundDuration );

			if( bRedWarning || bBlueWarning )
				ProcessScoreWarningSound( true );
			else
				ProcessScoreWarningSound( false );
		}
		else
		{
			pTimeScoreWnd->UpdateTime( 0, m_dwRoundDuration );
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

void ioSuccessionMode::UpdateRoundStateWnd()
{
	if( g_MyInfo.IsSortContribute() )
	{
		DWORD dwTimeGap = FRAMEGETTIME() - m_dwPlayStartTime;
		if( m_dwCurRoundDuration > dwTimeGap )
			ContributeRoundAllPercent( m_dwRoundDuration - (m_dwCurRoundDuration - dwTimeGap), m_dwRoundDuration );
		else
			ContributeRoundAllPercent( m_dwRoundDuration, m_dwRoundDuration );

		HeroMatchRecord *pRecord = FindRecordCharName( g_MyInfo.GetPublicID() );
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

void ioSuccessionMode::ProcessReadyState()
{
	if( P2PNetwork::IsNetworkPlaying() )	return;
	if( m_dwReadyStartTime + 4000 > FRAMEGETTIME() )
		return;

	SetModeState( MS_PLAY );
}

void ioSuccessionMode::ProcessPlayState()
{
	UpdateSoldierRevivalPenalty();

	m_pCreator->NotifySpringDampUseSet( true );

	if( !P2PNetwork::IsNetworkPlaying() )
	{
		CheckRoundEnd();
	}
}

void ioSuccessionMode::CheckRoundEnd()
{
	WinTeamType eWinTeam = WTT_DRAW;

	int iRedTeamCnt  = GetTeamPlayerCount( TEAM_RED );
	int iBlueTeamCnt = GetTeamPlayerCount( TEAM_BLUE );

	DWORD dwGapTime = FRAMEGETTIME() - m_dwPlayStartTime;
	if( (m_dwCurRoundDuration - dwGapTime) <= 0 )
	{
		if( IsTeamPlayerDie( TEAM_BLUE ) )
			eWinTeam = WTT_RED_TEAM;
		else if( IsTeamPlayerDie( TEAM_RED ) )
			eWinTeam = WTT_BLUE_TEAM;

		SetRoundEndInfo( eWinTeam, true );
	}
	else
	{
		if( IsTeamPlayerDie( TEAM_BLUE ) )
			eWinTeam = WTT_RED_TEAM;
		else if( IsTeamPlayerDie( TEAM_RED ) )
			eWinTeam = WTT_BLUE_TEAM;

		if( eWinTeam != WTT_DRAW )
		{
			m_dwCurRoundDuration -= dwGapTime;
			SetRoundEndInfo( eWinTeam, false );
		}
	}
}

int ioSuccessionMode::GetTeamPlayerCount( TeamType eTeam )
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

int ioSuccessionMode::GetTeamPurePlayerCount( TeamType eTeam )
{
	int iUserCnt = 0;

	int iSize = GetRecordCharCnt();
	for(int i = 0;i < iSize;i++)
	{
		ModeRecord *pRecord = FindModeRecord( i );
		if( pRecord == NULL ) continue;

		ioBaseChar *pChar = pRecord->pChar;
		if( !pChar )	continue;
		if( pChar->GetTeam() != eTeam )		continue;

		switch( pChar->GetState() )
		{
		case CS_DIE:
		case CS_VIEW:
		case CS_READY:
		case CS_LOADING:
		case CS_OBSERVER:
			break;
		default:
			iUserCnt++;
			break;
		}
	}

	return iUserCnt;
}

bool ioSuccessionMode::IsTeamPlayerDie( TeamType eTeam )
{
	int iSize = GetRecordCharCnt();
	for(int i = 0;i < iSize;i++)
	{
		ModeRecord *pRecord = FindModeRecord( i );
		if( pRecord == NULL ) continue;

		ioBaseChar *pChar = pRecord->pChar;
		if( !pChar )	continue;
		if( pChar->GetTeam() != eTeam )		continue;
		
		if( pChar->GetState() == CS_DIE )
			return true;
	}
	return false;
}

void ioSuccessionMode::ProcessResultState()
{
	ShowEndScoreWnd( true );

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

void ioSuccessionMode::RenderPreGUI()
{
	if( m_ModeState == MS_PLAY )
	{
		RenderPushEnableCursor();
	}
}

void ioSuccessionMode::RenderAfterGUI()
{
}

void ioSuccessionMode::AddNewRecord( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	HeroMatchRecord *pHeroMatchRecord = FindRecordCharName(pChar->GetCharName());
	if( !pHeroMatchRecord )
	{
		HeroMatchRecord kRecord;
		kRecord.pChar  = pChar;
		kRecord.szName = pChar->GetCharName();
		m_vCharRecordList.push_back( kRecord );		

		UpdateUserRank();

		if( pChar )
		{
			if( !g_BattleRoomMgr.IsObserverUser( pChar->GetCharName() ) )
				g_AwardMgr.AddNewAwardUser( pChar->GetCharName() );
		}
	}
	else
	{
		pHeroMatchRecord->pChar = pChar;
	}
}

void ioSuccessionMode::RemoveRecord( const ioHashString &rkName )
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


void ioSuccessionMode::RemoveRecord( int nIndex )
{
	int iCharCnt = m_vCharRecordList.size();
	if( iCharCnt > nIndex )
	{
		m_vCharRecordList.erase( m_vCharRecordList.begin() + nIndex );
		UpdateUserRank();
	}
}


ioBaseChar* ioSuccessionMode::GetRecordChar( int i )
{
	if( COMPARE( i, 0, GetRecordCharCnt() ) )
		return m_vCharRecordList[i].pChar;

	return NULL;
}

ioBaseChar* ioSuccessionMode::GetRecordChar( const ioHashString &rkName )
{
	HeroMatchRecord *pRecord = FindRecordCharName( rkName );
	if( pRecord )
		return pRecord->pChar;

	return NULL;
}

int ioSuccessionMode::GetRecordCharCnt() const
{
	return m_vCharRecordList.size();
}

ModeRecord* ioSuccessionMode::FindModeRecord( int iIdx )
{
	if( COMPARE( iIdx, 0, GetRecordCharCnt() ) )
		return &m_vCharRecordList[iIdx];

	return NULL;
}

ModeRecord* ioSuccessionMode::FindModeRecordCharName( const ioHashString &rkName )
{
	return (ModeRecord*)FindRecordCharName( rkName );
}

HeroMatchRecord* ioSuccessionMode::FindRecordCharName( const ioHashString &rkName )
{
	int iCharCnt = m_vCharRecordList.size();
	for( int i=0 ; i<iCharCnt ; i++ )
	{
		if( m_vCharRecordList[i].szName == rkName )
			return &m_vCharRecordList[i];
	}

	return NULL;
}

void ioSuccessionMode::SetRoundEndInfo( WinTeamType eTeam, bool bRoundSetEnd )
{
	CheckLastKiller();
	ioPlayMode::SetRoundEndInfo( eTeam, bRoundSetEnd );

	if( bRoundSetEnd )
	{
		// 히스토리 기록
		g_CampMgr.SetHeroMatchResult( eTeam, m_iBlueRoundWinCnt, m_iRedRoundWinCnt );		
	}
}

void ioSuccessionMode::SetTotalResultBriefingWnd()
{
	// 팀 모드 최종 결과 브리핑
	ioBaseChar *pOwner = GetOwnerChar();
	if( pOwner )
	{
		BriefingWnd *pBriefingWnd = dynamic_cast<BriefingWnd*>(g_GUIMgr.FindWnd( BRIEFING_WND ));
		if( pBriefingWnd && !pBriefingWnd->IsShow() )
		{
			TeamType eOwenerTeam = pOwner->GetTeam();
			TeamType eWinTeam = TEAM_NONE;
			if( IsBlueWin( m_WinTeamType ) )
				eWinTeam = TEAM_BLUE;
			else if( IsRedWin( m_WinTeamType ) )
				eWinTeam = TEAM_RED;

			D3DXVECTOR3 vNewPos = ioMath::VEC3_ZERO;
			int iWinCount, iLoseCount;
			int iResultType = BriefingWnd::RESULT_DRAW;
			
			if( g_BattleRoomMgr.IsObserverUser( g_MyInfo.GetPublicID() ) || m_pCreator->IsStealthMode() )
			{
				ioBaseChar *pTargetChar = m_pCreator->GetBaseChar(m_szEndLookCharName);
				if( pTargetChar && (GetModeType() == MT_SURVIVAL || GetModeType() == MT_TEAM_SURVIVAL || GetModeType() == MT_DOUBLE_CROWN || GetModeType() == MT_FIGHT_CLUB || GetModeType() == MT_TEAM_SURVIVAL_AI) )
				{
					eOwenerTeam = pTargetChar->GetTeam();

					if( eOwenerTeam == TEAM_BLUE )
					{
						if( eWinTeam == TEAM_BLUE )
							iResultType = BriefingWnd::RESULT_WIN;
						else if( eWinTeam == TEAM_RED )
							iResultType = BriefingWnd::RESULT_LOSE;
					}
					else
					{
						if( eWinTeam == TEAM_RED )
							iResultType = BriefingWnd::RESULT_WIN;
						else if( eWinTeam == TEAM_BLUE )
							iResultType = BriefingWnd::RESULT_LOSE;
					}

					if( IsNeedUpdateMapEffect( pTargetChar ) )
						vNewPos = pTargetChar->GetWorldPosition();
				}
				else
				{
					eOwenerTeam = eWinTeam;
					iResultType = BriefingWnd::RESULT_WIN;
				}
			}
			else
			{
				if( pOwner->GetTeam() == TEAM_BLUE )
				{
					iWinCount = m_iBlueRoundWinCnt;
					iLoseCount= m_iRedRoundWinCnt;
					 
					if( eWinTeam == TEAM_BLUE )
						iResultType = BriefingWnd::RESULT_WIN;
					else if( eWinTeam == TEAM_RED )
						iResultType = BriefingWnd::RESULT_LOSE;
				}
				else
				{
					iWinCount = m_iRedRoundWinCnt;
					iLoseCount= m_iBlueRoundWinCnt;

					if( eWinTeam == TEAM_RED )
						iResultType = BriefingWnd::RESULT_WIN;
					else if( eWinTeam == TEAM_BLUE )
						iResultType = BriefingWnd::RESULT_LOSE;
				}

				if( IsNeedUpdateMapEffect( pOwner ) )
					vNewPos = pOwner->GetWorldPosition();
			}

			pBriefingWnd->SetResultInfo( m_pCreator, BriefingWnd::BRIEFING_FINAL_RESULT, iResultType, eOwenerTeam, m_iCurRoundCount );

			ioHashString szMapEffect = pBriefingWnd->GetFinalResultMapEffect( eWinTeam );
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

void ioSuccessionMode::SetRoundEndMotion()
{
	WinTeamType eWinTeamType = m_WinTeamType;
	RoundEndMotion eEndMotion;	
	int iCharCnt = GetRecordCharCnt();	
	for( int i=0; i<iCharCnt; i++ )
	{
		ioBaseChar *pChar = GetRecordChar( i );
		if( !pChar ) continue;
		if( pChar->GetState() == CS_OBSERVER ) continue;
		if( g_BattleRoomMgr.IsObserverUser(pChar->GetCharName()) ) continue;

		if( eWinTeamType == WTT_DRAW )
		{
			eEndMotion = RM_DRAW;
		}
		else
		{
			if( IsWinTeam( eWinTeamType, pChar->GetTeam() ) )
				eEndMotion = RM_WIN;
			else
				eEndMotion = RM_LOSE;
		}

		pChar->SetRoundEndMotion( eEndMotion );
		pChar->StartNoInputDelayTime( 10000 );
	}	
}

bool ioSuccessionMode::GetRoundInfo( int iIndex, RoundInfo &rkInfo )
{
	if( ioPlayMode::GetRoundInfo( iIndex, rkInfo ) )
	{
		HeroMatchRecord *pRecord = (HeroMatchRecord*)FindModeRecord( iIndex );
		if( !pRecord )	return false;
		if( !pRecord->pChar ) return false;

		rkInfo.m_iContribute	= pRecord->Contribute.m_iContribute;
		rkInfo.m_iContributePer = pRecord->Contribute.m_iPrevContributePer;		
		return true;
	}
	return false;
}

bool ioSuccessionMode::GetResultInfo( int iIndex, ResultInfo &rkInfo )
{
	if( ioPlayMode::GetResultInfo( iIndex, rkInfo ) )
	{
		HeroMatchRecord *pRecord = (HeroMatchRecord*)FindModeRecord( iIndex );
		if( !pRecord )	return false;
		if( !pRecord->pChar ) return false;

		rkInfo.m_iContribute = pRecord->Contribute.m_iContribute;
		rkInfo.m_iContributePer = pRecord->Contribute.m_iPrevContributePer;
		return true;
	}
	return false;
}

bool ioSuccessionMode::GetFinalInfo( int iIndex, FinalInfo &rkInfo )
{
	if( ioPlayMode::GetFinalInfo( iIndex, rkInfo ) )
	{
		HeroMatchRecord *pRecord = (HeroMatchRecord*)FindModeRecord( iIndex );
		if( !pRecord )	return false;
		if( !pRecord->pChar ) return false;

		rkInfo.m_iContribute = pRecord->Contribute.m_iContribute;
		rkInfo.m_iContributePer = pRecord->Contribute.m_iPrevContributePer;
		return true;
	}
	return false;
}

void ioSuccessionMode::ApplyModeInfo( SP2Packet &rkPacket )
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

void ioSuccessionMode::ApplyCharModeInfo( const ioHashString &rkName, SP2Packet &rkPacket, bool bDieCheck )
{
	bool bRecord;
	rkPacket >> bRecord;

	if( !bRecord ) return;

	HeroMatchRecord *pRecord = FindRecordCharName( rkName );
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
	}
}

bool ioSuccessionMode::ProcessTCPPacket( SP2Packet &rkPacket )
{
	ErrorReport::SetPosition( 1030, 9 );
	if( ioPlayMode::ProcessTCPPacket( rkPacket ) )
		return true;

	ErrorReport::SetPosition( 1030, 10 );

	return false;
}

bool ioSuccessionMode::ProcessUDPPacket( SP2Packet &rkPacket )
{
	if( ioPlayMode::ProcessUDPPacket( rkPacket ) )
		return true;

	return false;
}

void ioSuccessionMode::NotifyEntityDie( ioPlayEntity *pEntity, ioPlayEntity *pKiller, ioBaseChar *pBestAttacker,
								       float fLastRate, float fBestRate )
{	
	ioBaseChar *pDieChar = ToBaseChar( pEntity );
	ioBaseChar *pKillChar = ToBaseChar( pKiller );

	OnNotifyCharDie( pDieChar, pKillChar, pBestAttacker, fLastRate, fBestRate );
	UpdateKillDeathRecord( pEntity, pKiller, pBestAttacker );
}

void ioSuccessionMode::NotifyCharDropDie( ioBaseChar *pDieChar, ioBaseChar *pKillChar, ioBaseChar *pBestAttacker,
										 float fLastRate, float fBestRate )
{
	OnNotifyCharDie( pDieChar, pKillChar, pBestAttacker, fLastRate, fBestRate );
	UpdateDropDieRecord( pDieChar, pKillChar, pBestAttacker );
}

void ioSuccessionMode::OnNotifyCharDie( ioBaseChar *pDieChar, ioBaseChar *pKillChar, ioBaseChar *pBestAttacker,
									   float fLastRate, float fBestRate )
{
	if( !pDieChar )
	{
		LOG.PrintTimeAndLog( 0, "ioSuccessionMode::OnNotifyCharDie - Entity is Not Char" );
		return;
	}

	HeroMatchRecord *pDieRecord = FindRecordCharName( pDieChar->GetCharName() );
	if( !pDieRecord )
	{
		LOG.PrintTimeAndLog( 0, "ioSuccessionMode::OnNotifyCharDie - %s Not Exist Char",
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

	//For EndScene
	m_LastDieChar = pDieChar->GetCharName();

	if( pKillChar )
	{
		HeroMatchRecord *pKillRecord = FindRecordCharName( pKillChar->GetCharName() );
		if( !pKillRecord )
		{
			LOG.PrintTimeAndLog( 0, "ioSuccessionMode::OnNotifyCharDie - %s Not Exist Kill Char",
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

void ioSuccessionMode::UpdateDropDieRecord( ioBaseChar *pDieChar, ioBaseChar *pKillChar, ioBaseChar *pBestAttacker )
{
	bool bDropKill = true;
	if( pKillChar )
	{
		ModeRecord *pAttRecord = FindModeRecordCharName( pKillChar->GetCharName() );
		ModeRecord *pDieRecord = FindModeRecordCharName( pDieChar->GetCharName() );

		if( pAttRecord && pDieRecord )
		{
			float fKillPoint = FLOAT05 + ( (float)GetKillPoint( pAttRecord->pChar->GetTeam() ) * FLOAT05 );
			if( pAttRecord->pChar->GetTeam() != pDieRecord->pChar->GetTeam() )
			{
				pAttRecord->AddKill( GetModeType(), fKillPoint, bDropKill, false );
				pDieRecord->AddDeath( GetModeType(), GetDeathPoint( pDieRecord->pChar->GetTeam() ) );

				g_ModeUIController.SetKOMsg( pAttRecord->pChar->GetTeam(), pAttRecord->pChar->GetCharName(), 
					                         pDieRecord->pChar->GetLastAttackerSkillName(), pDieRecord->pChar->GetLastAttackerItemCode(),
											 pDieRecord->pChar->GetTeam(), pDieRecord->pChar->GetCharName() );
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
					pAttRecord->AddKill( GetModeType(), -fKillPoint, bDropKill, false );
					pDieRecord->AddDeath( GetModeType(), GetDeathPoint( pDieRecord->pChar->GetTeam() ) );

					g_ModeUIController.SetKOMsg( TEAM_NONE, "",
						                         "", 0, pDieRecord->pChar->GetTeam(), pDieRecord->pChar->GetCharName() );
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
			pDieRecord->AddDeath( GetModeType(), GetDeathPoint( pDieRecord->pChar->GetTeam() ) );

			g_ModeUIController.SetKOMsg( TEAM_NONE, "",
				                         "", 0, pDieRecord->pChar->GetTeam(), pDieRecord->pChar->GetCharName() );

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
			pAttRecord->AddKill( GetModeType(), fKillPoint, bDropKill, true );
		}
	}
}

void ioSuccessionMode::ReStartMode()
{
	D3DXVECTOR3 vStartPos;
	int iCharCnt = m_vCharRecordList.size();
	for( int i=0 ; i<iCharCnt ; i++ )
	{
		HeroMatchRecord &rkRecord = m_vCharRecordList[i];
		if( !rkRecord.pChar ) continue;
		vStartPos = GetStartPos( rkRecord.pChar->GetTeam() );
		rkRecord.pChar->SetWorldPosition( vStartPos );

		rkRecord.dwRevivalGap = 0;
		rkRecord.dwCurDieTime = 0;
		rkRecord.iRevivalCnt = 0;

		if( m_iCurRoundCount == 1 )	// Final Round End
		{
			rkRecord.InitValues();

			m_iBlueRoundWinCnt = 0;
			m_iRedRoundWinCnt = 0;
		}

		rkRecord.pChar->SetProtectionMode( PM_NONE, 0 );
		rkRecord.pChar->SetRoundEndMotion( RM_NONE );
		rkRecord.pChar->SetProtectionMode( PM_START, 2000 );
		rkRecord.pChar->Revival();

		if( rkRecord.pChar->GetState() != CS_OBSERVER )
		{
			if( !g_BattleRoomMgr.IsObserverUser( rkRecord.pChar->GetCharName() ) )
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

D3DXVECTOR3 ioSuccessionMode::GetRevivalPos( ioBaseChar *pChar )
{
	D3DXVECTOR3 vPos = GetStartPos( pChar->GetTeam() );
	vPos.x += (float)( rand() % 100 ) / 20.0f;
	vPos.z += (float)( rand() % 100 ) / 20.0f;
	vPos.y = m_pCreator->GetMapHeightByRevial( vPos.x, vPos.z );

	return vPos;
}

void ioSuccessionMode::ContributeLOG()
{
	char szLog[MAX_PATH];

	int iCharCnt = m_vCharRecordList.size();
	for( int i=0 ;i<iCharCnt; i++ )
	{
		HeroMatchRecord *pRecord = &m_vCharRecordList[i];
		if( pRecord->pChar )
		{
			sprintf( szLog, "%s[%d] : K%d(%d) D%d(%d) A%d(%d)", pRecord->szName.c_str(), (int)pRecord->pChar->GetTeam(), //Except Extracting Hangeul
																pRecord->Contribute.m_iKillRank + 1, pRecord->Contribute.m_iKill,
																pRecord->Contribute.m_iDamageRank + 1, pRecord->Contribute.m_iDamage,
																pRecord->Contribute.m_iAttackCntRank + 1, pRecord->Contribute.m_iAttackCnt );
			g_ChatMgr.SetSystemMsg( szLog );
		}		
	}
}

void ioSuccessionMode::ContributeConvertTeam( TeamType eTeam )
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
		HeroMatchRecord *pRecord = &m_vCharRecordList[i];
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
			HeroMatchRecord *pRecord = &m_vCharRecordList[iArray];
			if( !pRecord->Contribute.m_bLastResultSync )
				pRecord->Contribute.m_iContribute = m_vContributeTable[vInfo[i].m_iRank];
			pRecord->Contribute.m_iKillRank = vInfo[i].m_iRank;
		}
	}
	vInfo.clear();

	// 데미지 & 힐링 점수
	for(i = 0;i < iCharCnt;i++)
	{
		HeroMatchRecord *pRecord = &m_vCharRecordList[i];
		if( !pRecord ) continue;
		if( !pRecord->pChar ) continue;
		if( pRecord->pChar->GetState() == CS_OBSERVER ) continue;
		if( g_BattleRoomMgr.IsObserverUser( pRecord->pChar->GetCharName() ) ) continue;

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
			HeroMatchRecord *pRecord = &m_vCharRecordList[iArray];
			if( !pRecord->Contribute.m_bLastResultSync )
				pRecord->Contribute.m_iContribute += m_vContributeTable[vInfo[i].m_iRank]  * 3;
			pRecord->Contribute.m_iDamageRank = vInfo[i].m_iRank;
		}
	}
	vInfo.clear();

	//유효 타격 횟수
	for(i = 0;i < iCharCnt;i++)
	{
		HeroMatchRecord *pRecord = &m_vCharRecordList[i];
		if( !pRecord ) continue;
		if( !pRecord->pChar ) continue;
		if( pRecord->pChar->GetState() == CS_OBSERVER ) continue;
		if( g_BattleRoomMgr.IsObserverUser( pRecord->pChar->GetCharName() ) ) continue;

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
			HeroMatchRecord *pRecord = &m_vCharRecordList[iArray];
			if( !pRecord->Contribute.m_bLastResultSync )
				pRecord->Contribute.m_iContribute += m_vContributeTable[vInfo[i].m_iRank] * 2;
			pRecord->Contribute.m_iAttackCntRank = vInfo[i].m_iRank;
		}
	}
	vInfo.clear();

	//데쓰처리
	for(i = 0;i < iCharCnt;i++)
	{
		HeroMatchRecord *pRecord = &m_vCharRecordList[i];
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
			HeroMatchRecord *pRecord = &m_vCharRecordList[iArray];
			if( !pRecord->Contribute.m_bLastResultSync )
				pRecord->Contribute.m_iContribute += m_vContributeTable[vInfo[i].m_iRank];
			pRecord->Contribute.m_iDeathRank = vInfo[i].m_iRank;
		}
	}
	vInfo.clear();
}

void ioSuccessionMode::ContributeConvert()
{
	ContributeConvertTeam( TEAM_NONE );        //TEAM_NONE는 전체 유저에 대한 기여도 평가	
}

int ioSuccessionMode::ContributeMyRank( int &rRankValue1, int &rRankValue2, int &rRankValue3, int &rRankValue4 )
{
	rRankValue1 = rRankValue2 = rRankValue3 = rRankValue4 = -1;

	bool bObserver = false;
	ioBaseChar *pOwner = GetOwnerChar();
	if( pOwner && ( g_BattleRoomMgr.IsObserverUser( pOwner->GetCharName() ) || m_pCreator->IsStealthMode() ) )
		bObserver = true;

	int i = 0;
	int iCharCnt = m_vCharRecordList.size();
	int iInfoCnt = 0;
	ContributeInfoList vInfo;
	//KO
	for(i = 0;i < iCharCnt;i++)
	{
		HeroMatchRecord *pRecord = &m_vCharRecordList[i];
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
		HeroMatchRecord *pRecord = &m_vCharRecordList[i];		
		if( !pRecord ) continue;
		if( !pRecord->pChar ) continue;
		if( pRecord->pChar->GetState() == CS_OBSERVER ) continue;
		if( g_BattleRoomMgr.IsObserverUser( pRecord->pChar->GetCharName() ) ) continue;

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

	// 데미지 & 힐링 점수
	for(i = 0;i < iCharCnt;i++)
	{
		HeroMatchRecord *pRecord = &m_vCharRecordList[i];		
		if( !pRecord ) continue;
		if( !pRecord->pChar ) continue;
		if( pRecord->pChar->GetState() == CS_OBSERVER ) continue;
		if( g_BattleRoomMgr.IsObserverUser( pRecord->pChar->GetCharName() ) ) continue;

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

	//데미지 & 힐
	for(i = 0;i < iCharCnt;i++)
	{
		HeroMatchRecord *pRecord = &m_vCharRecordList[i];
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

void ioSuccessionMode::ContributeRoundPercent( DWORD dwCurTime, DWORD dwTotalTime )
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
		HeroMatchRecord *pRecord = &m_vCharRecordList[i];
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
		HeroMatchRecord *pRecord = &m_vCharRecordList[i];

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

	int iBlueCnt       = m_iBlueRoundWinCnt;
	int iRedCnt        = m_iRedRoundWinCnt;
	TeamType eWinTeam  = TEAM_NONE;

	int iAverageClick  = 8;
	int iWinBonus      = 3;
	int iDrawBonus     = 1;
	int iLoseBonus     = 1;

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
		HeroMatchRecord *pRecord = &m_vCharRecordList[i];

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

void ioSuccessionMode::ContributeRoundAllPercent( DWORD dwCurTime, DWORD dwTotalTime )
{
	//기여도의 100분율
	int iMaxContribute	= 0;
	int iCharCnt		= m_vCharRecordList.size();
	int iPlayCharCnt	= 0;

	int i = 0;
	for(i = 0;i < iCharCnt;i++)
	{
		HeroMatchRecord *pRecord = &m_vCharRecordList[i];
		if( !pRecord ) continue;
		if( !pRecord->pChar ) continue;
		if( pRecord->pChar->GetState() == CS_OBSERVER ) continue;
		if( g_BattleRoomMgr.IsObserverUser( pRecord->pChar->GetCharName() ) ) continue;
		
		iMaxContribute += pRecord->Contribute.m_iContribute;
		iPlayCharCnt++;
	}

	int iOwnerContribute = -1;
	if( iMaxContribute > 0 )
	{
		for(i = 0;i < iCharCnt;i++)
		{
			HeroMatchRecord *pRecord = &m_vCharRecordList[i];
			if( !pRecord ) continue;
			if( !pRecord->pChar ) continue;
			if( pRecord->pChar->GetState() == CS_OBSERVER ) continue;
			if( g_BattleRoomMgr.IsObserverUser( pRecord->pChar->GetCharName() ) ) continue;
			
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

void ioSuccessionMode::DamageContribute( const ioHashString szAttacker, const ioHashString szTarget, float fDamage, DWORD dwItemCode, int iWeaponType )
{
	if( szAttacker.IsEmpty() ) return;

	HeroMatchRecord *pRecord = FindRecordCharName( szAttacker );
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
	ContributeConvert();
	ContributeRoundAllPercent();
	UpdateRoundInfoList();
	CheckTotalChamp();

	if( fDamagePoint > 0.0f && !pAttacker->IsOwnerChar() )
	{
		CheckNonePlayTime( pAttacker );
	}

	g_AwardMgr.AddDamage( pRecord->szName, fDamagePoint, fDamage );
}

void ioSuccessionMode::KillContribute( const ioHashString szKiller, const ioHashString szKilled, DWORD dwItemCode )
{
	if( szKilled.IsEmpty() ) return;

	int iKillPoint = 100;

	float fAddKillPoint = FLOAT1;
	if( IsContributeAddPointItemCode( dwItemCode ) )
		fAddKillPoint = 1.1f;

	if( szKiller.IsEmpty() || szKiller == szKilled )     //자살
	{
		HeroMatchRecord *pRecord = FindRecordCharName( szKilled );
		if( !pRecord || !pRecord->pChar ) return;

		pRecord->Contribute.m_iKill -= iKillPoint;
	}
	else
	{
		HeroMatchRecord *pKillerRecord = FindRecordCharName( szKiller );
		HeroMatchRecord *pKilledRecord = FindRecordCharName( szKilled );
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

void ioSuccessionMode::AttackCntContribute( const ioHashString szAttacker, const ioHashString szTarget, DWORD dwItemCode, int iAttackValue/* = 100 */ )
{
	if( szAttacker.IsEmpty() || szTarget.IsEmpty() ) return;

	HeroMatchRecord *pAttackerRecord = FindRecordCharName( szAttacker );
	HeroMatchRecord *pTargetRecord = FindRecordCharName( szTarget );

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

void ioSuccessionMode::CheckLastKiller()
{
}

void ioSuccessionMode::FillLastPlayRecordInfo( SP2Packet &rkPacket )
{
	int iCharCnt = m_vCharRecordList.size();
	rkPacket << iCharCnt;
	for(int i = 0;i < iCharCnt;i++)
	{
		HeroMatchRecord *pRecord = &m_vCharRecordList[i];
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

void ioSuccessionMode::FillPlayRecordInfo( SP2Packet &rkPacket )
{	
	int iCharCnt = m_vCharRecordList.size();
	rkPacket << iCharCnt;
	for(int i = 0;i < iCharCnt;i++)
	{
		HeroMatchRecord *pRecord = &m_vCharRecordList[i];
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
}

void ioSuccessionMode::ApplyPlayRecordInfo( SP2Packet &rkPacket )
{
	int iCharCnt;
	rkPacket >> iCharCnt;
	for(int i = 0;i < iCharCnt;i++)
	{
		ioHashString szName;
		int iDamage, iAttackCnt, iKill, iDeath, iPrevContributePer, iUniqueTotalKill, iUniqueTotalDeath, iVictories;
		rkPacket >> szName >> iDamage >> iAttackCnt >> iKill >> iDeath >> iPrevContributePer >> iUniqueTotalKill >> iUniqueTotalDeath >> iVictories;

		HeroMatchRecord *pRecord = FindRecordCharName( szName );
		if( pRecord )
		{
			pRecord->Contribute.m_iDamage		     = iDamage;
			pRecord->Contribute.m_iAttackCnt         = iAttackCnt;
			pRecord->Contribute.m_iKill              = iKill;
			pRecord->Contribute.m_iDeath             = iDeath;
			pRecord->Contribute.m_iPrevContributePer = iPrevContributePer;
			pRecord->iUniqueTotalKill = iUniqueTotalKill;
			pRecord->iUniqueTotalDeath= iUniqueTotalDeath;
			/*
			if( !g_LadderTeamMgr.IsLadderTeam() )
				g_VictoriesMgr.SetVictories( szName, iVictories );
				*/
		}
	}
	
	ContributeConvert();
	ContributeRoundAllPercent();
	UpdateRoundInfoList();
	CheckTotalChamp();
}

void ioSuccessionMode::ContributeResultSync( const ioHashString &szName, int iContribute )
{
	HeroMatchRecord *pRecord = FindRecordCharName( szName );
	if( !pRecord ) return;

	pRecord->Contribute.m_bLastResultSync = true;
	pRecord->Contribute.m_iContribute     = iContribute;
}

void ioSuccessionMode::CheckNonePlayTime( ioBaseChar *pChar, bool bPlayerChanged )
{
	if( !pChar )
	{
		ioBaseChar *pOwner = GetOwnerChar();

		TeamType eOwnerTeam = pOwner->GetTeam();
		TeamType eEnemyTeam = TEAM_NONE;

		if( eOwnerTeam == TEAM_BLUE )
			eEnemyTeam = TEAM_RED;
		else if( eOwnerTeam == TEAM_RED )
			eEnemyTeam = TEAM_BLUE;

		int iEnemyCount = GetTeamPurePlayerCount( eEnemyTeam );
		int iTeamCount = GetTeamPurePlayerCount( eOwnerTeam );
		int iTotalCount = iTeamCount + iEnemyCount;

		if( iTotalCount > 2 && iTeamCount == 1 )
			pOwner->CheckNonePlayTime( false, bPlayerChanged );
		else
			pOwner->CheckNonePlayTime( true, false );
	}
	else
	{
		TeamType eOwnerTeam = pChar->GetTeam();
		TeamType eEnemyTeam = TEAM_NONE;

		if( eOwnerTeam == TEAM_BLUE )
			eEnemyTeam = TEAM_RED;
		else if( eOwnerTeam == TEAM_RED )
			eEnemyTeam = TEAM_BLUE;

		int iEnemyCount = GetTeamPurePlayerCount( eEnemyTeam );
		int iTeamCount = GetTeamPurePlayerCount( eOwnerTeam );
		int iTotalCount = iTeamCount + iEnemyCount;

		if( iTotalCount > 2 && iTeamCount == 1 )
			pChar->CheckNonePlayTime( false, bPlayerChanged );
		else
			pChar->CheckNonePlayTime( true, false );
	}
}

void ioSuccessionMode::OnZeroHP( SP2Packet &rkPacket )
{
	rkPacket >> m_dwSuddenDeathTime;

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

void ioSuccessionMode::OnRoundReady( SP2Packet &rkPacket )
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

void ioSuccessionMode::OnRoundJoin( SP2Packet &rkPacket )
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
			/*
			else if( g_LadderTeamMgr.IsLadderTeam() )
				g_QuestMgr.QuestCompleteTerm( QC_ENTER_CAMP_BATTLE, GetModeType() );
			else if( g_ShuffleRoomMgr.IsShuffleRoom() )
				g_QuestMgr.QuestCompleteTerm( QC_ENTER_BATTLE_PVP, GetModeType() );
			else if( g_RankBattleMgr.IsRankBattlePlaying() )
				g_QuestMgr.QuestCompleteTerm( QC_ENTER_BATTLE_PVP, GetModeType() );
				*/
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

void ioSuccessionMode::OnRoundJoinView( SP2Packet &rkPacket )
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
		g_ModeHelpMgr.ShowHelp( ioModeHelpManager::HT_VIEW );

		SetBlindMode( true );

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

void ioSuccessionMode::OnRoundJoinObserver( SP2Packet &rkPacket )
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

void ioSuccessionMode::SetResultScoreWnd()
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
	if( m_iRedRoundWinCnt > m_iBlueRoundWinCnt )
		PointUpEvent( TEAM_RED );
	else if( m_iRedRoundWinCnt < m_iBlueRoundWinCnt )
		PointUpEvent( TEAM_BLUE );
}

void ioSuccessionMode::SetTotalScoreWnd( IntVec &rvClassTypeList,
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
			/*
			else if( g_LadderTeamMgr.IsLadderTeam() )
				g_LadderTeamMgr.UpdateUserData( kInfo.m_szName, kInfo.m_iLevel, kInfo.m_iResultLadderPoint );
			else if( g_ShuffleRoomMgr.IsShuffleRoom() )
				g_ShuffleRoomMgr.UpdateUserLevel( kInfo.m_szName, kInfo.m_iLevel );
				*/
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
//	{
//		pTotal->SetRoundHistory( i, m_vRoundHistory[i] );
//	}

	pTotalResult->SetSuccessionScore( GetOwnerTeam(), m_iBlueRoundWinCnt, m_iRedRoundWinCnt, m_iSuccessionBonusMoney );
	SetTotalResultBriefingWnd();
}

ioGameEntity* ioSuccessionMode::GetGuideTarget( const D3DXVECTOR3& vArrowPos )
{
	return NULL;
}

void ioSuccessionMode::CheckEndSceneLookAt()
{
	if( !m_LastKillChar.IsEmpty() )
	{
		HeroMatchRecord *pKillRecord = FindRecordCharName( m_LastKillChar );
		if( !pKillRecord )
		{
			m_vEndLookAt = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
			m_szEndLookCharName.Clear();
			return;
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
			HeroMatchRecord *pDieRecord = FindRecordCharName( m_LastDieChar );
			if( !pDieRecord )
			{
				m_vEndLookAt = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
				m_szEndLookCharName.Clear();
				return;
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
				m_szEndLookCharName.Clear();
			}
		}
	}
}

void ioSuccessionMode::ClearModeData()
{
	m_LastDieChar.Clear();
	m_LastKillChar.Clear();

	g_SoundMgr.AllSoundStop();
	g_SoundMgr.StopStream();
	m_pCreator->HidingAllFieldItem();
}

bool ioSuccessionMode::IsNeedRevival( ioBaseChar *pChar )
{
	if( !pChar ) return false;

	if( !m_LastKillChar.IsEmpty() && m_LastKillChar == pChar->GetCharName() )
		return true;

	return false;
}

ioBaseChar* ioSuccessionMode::CheckMostNearChar( ioBaseChar *pChar )
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
			pTarget->GetState() == CS_READY ||
			pTarget->GetState() == CS_OBSERVER ||
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

void ioSuccessionMode::UpdateRevivalTime( const ioHashString &szName, DWORD dwCurRevivalTime )
{
	ModeRecord *pDieRecord = FindModeRecordCharName( szName );
	if( !pDieRecord )
	{
		LOG.PrintTimeAndLog( 0, "ioSuccessionMode::UpdateRevivalTime() - %s is not exist to the record", szName.c_str() );
		return;
	}

	m_dwCurRevivalTime = dwCurRevivalTime;

	pDieRecord->dwCurDieTime = FRAMEGETTIME();
	pDieRecord->dwRevivalGap = m_dwCurRevivalTime;
	pDieRecord->iRevivalCnt++;

	if( pDieRecord->pChar->IsOwnerChar() )
	{
		SetBlindMode( true );
	}
}

int ioSuccessionMode::GetCurRank( const ioHashString& szCharName )
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

int ioSuccessionMode::GetContributePerChar( const ioHashString& szName )
{
	int iCharCnt = m_vCharRecordList.size();

	for(int i = 0;i < iCharCnt;i++)
	{
		HeroMatchRecord *pRecord = &m_vCharRecordList[i];
		ErrorReport::SetPosition( 3000, 1 );
		if( !pRecord )	continue;
		ErrorReport::SetPosition( 3000, 2 );
		if( !pRecord->pChar ) continue;
		ErrorReport::SetPosition( 3000, 3 );

		if( pRecord->pChar->GetCharName() == szName )
		{
			ErrorReport::SetPosition( 3000, 4 );
			return pRecord->Contribute.m_iPrevContributePer;
			ErrorReport::SetPosition( 3000, 5 );
		}
	}

	ErrorReport::SetPosition( 3000, 6 );
	return 0;
}

void ioSuccessionMode::OnRoundEnd( SP2Packet &rkPacket )
{
	//클라에서 검증하는게 아닌 서버에서 받은 패킷으로 revenge전인지 아닌지 유무 결정
	bool bRevenge;
	rkPacket >> bRevenge;
	ioPlayMode::OnRoundEnd( rkPacket );

	SetRevengeWnd( bRevenge );
}

void ioSuccessionMode::SetRevengeWnd( bool bRevenge )
{
	//이미 Revenge전이였다면 Return;
	if ( bRevenge )
		return;

	ioBaseChar *pOwner = GetOwnerChar();
	if( !pOwner )
		return;

	//혼자 남았다면( 중간에 나간경우 )
	if ( m_iRoundEndRemainUserCnt == 1 )
		return;

	TeamType eOwenerTeam = pOwner->GetTeam();
	TeamType eWinTeam = TEAM_NONE;
	if( IsBlueWin( m_WinTeamType ) )
		eWinTeam = TEAM_BLUE;
	else if( IsRedWin( m_WinTeamType ) )
		eWinTeam = TEAM_RED;

	//owner 졌다면
	if ( eOwenerTeam != eWinTeam )
		m_bOwnerWin = false;
	else
		m_bOwnerWin = true;
	m_dwRevengeWndShowTime = FRAMEGETTIME() + m_dwReserveRevengeWndShowTime;
}

void ioSuccessionMode::OnFinalRoundResult( SP2Packet &rkPacket )
{
	// 최종 결과 패킷을 받을 때 시상식 Show함 . 내부적으로 수상자가 없으면 Hide시킨다.
	g_GUIMgr.HideWnd( BRIEFING_WND );

	DWORD dwTotalResultTime;
	rkPacket >> dwTotalResultTime;
	SetAutoStartAlarmTime( dwTotalResultTime );

	float fBlueGuildTeamBonus = FLOAT1;
	float fRedGuildTeamBonus = FLOAT1;
	if( m_pCreator->GetRoomStyle() == RSTYLE_LADDERBATTLE )
	{
		DWORD dwBlueTeamIndex, dwRedTeamIndex;
		int iBluePrevRank, iBlueCurRank, iRedPrevRank, iRedCurRank;
		rkPacket >> dwBlueTeamIndex >> iBluePrevRank >> iBlueCurRank;
		rkPacket >> dwRedTeamIndex >> iRedPrevRank >> iRedCurRank;
		g_LadderTeamMgr.SetTeamRanking( dwBlueTeamIndex, iBlueCurRank, iBluePrevRank - iBlueCurRank );
		g_LadderTeamMgr.SetTeamRanking( dwRedTeamIndex, iRedCurRank, iRedPrevRank - iRedCurRank );

		// 양팀 길드팀 진영 보너스
		rkPacket >> fBlueGuildTeamBonus >> fRedGuildTeamBonus;

		// 래더전 경험치
		int iHeroExpert;
		rkPacket >> iHeroExpert;
		g_MyInfo.SetHeroExpert( iHeroExpert );
	}

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


	//연승전 보너스 획득 머니
	rkPacket >> m_iSuccessionBonusMoney;
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
			pRecord->bResultLevelUP  = bLevelUP;
			pRecord->iResultLevel    = iGradeLevel;
			pRecord->iResultTotalExp = iTotalExp;
			pRecord->iResultTotalPeso= iTotalPeso;
			pRecord->iResultTotalLadderPoint = iResultLadderPoint;
			g_UserInfoMgr.SetLadderPoint( szName, iLadderPoint );
		}
		else
		{
			LOG.PrintTimeAndLog( 0, "ioPlayMode::OnFinalRoundResult - Not Exist Record: %s", szName.c_str() );
		}
	}

	SetTotalScoreWnd( vClassTypeList, vClassPointList, fBlueGuildTeamBonus, fRedGuildTeamBonus, fBonusArray );

	//
	bool bCheckNext;
	rkPacket >> bCheckNext;

	int iNextModeType = 0;
	int iNextModeSubNum = 0;
	int iNextMapIndex = 0;

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

#if defined( USE_GA )
	char chLabel[32] = {0,};

	if ( ioLocalManager::GetLocalType() == ioLocalManager::LCT_KOREA )
		sprintf_e( chLabel, "%d", iMyMoney );
	else
		SafeSprintf( chLabel, sizeof(chLabel), "%1", iMyMoney );

	// PESO_GET_BATTLE
	g_HttpMng.GA_EventTracking( g_MyInfo.GetUserIndex()
		, "Peso"
		, "End"
		, chLabel
		, 1
		, "%2FPESO_GET_BATTLE" );
#endif
}
