

#include "stdafx.h"

#include "ioHiddenkingMode.h"
#include "ioLadderTeamMgr.h"
#include "ioRankBattleManager.h"
#include "ioShuffleRoomMgr.h"

#include "GUI/MiniSoldierSelectWnd.h"
#include "GUI/MiniMap.h"
#include "GUI/CenterInfoWnd.h"
#include "GUI/TimeScoreWnd.h"
#include "GUI/ioObserverWnd.h"
#include "GUI/TotalResultMainWnd.h"
#include "GUI/RoundStateMainWnd.h"

ioHiddenkingMode::ioHiddenkingMode( ioPlayStage *pCreator )
: ioPlayMode( pCreator )
{
	m_ModeType = MT_KING;

	m_bLastPoint = 0;
	m_iRedTeamScore = 0;
	m_iBlueTeamScore = 0;

	m_dwGivePointTime = 0;
	m_dwCrownHoldTime = 0;

	m_dwCrownShowEventTime = 0;
	m_dwCrownShowGapTime = 1000;

	m_dwSendCheckKingTime = 0;
	m_iCurSendIndex = 1;

	m_vCharRecordList.reserve( 100 );
}

ioHiddenkingMode::~ioHiddenkingMode()
{
	m_bZeroHP = false;
	ClearMode();
}

void ioHiddenkingMode::LoadMap()
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

void ioHiddenkingMode::InitMode( int iMapIndex )
{
	m_iModeMapIndex = iMapIndex;

	LoadMap();
	LoadPushStructs();
	LoadMachineStruct();
	LoadFuntionalObjects();
	LoadWear();
	LoadCharacters();
	LoadDisableSkillList();
	LoadStartBuffList();
	LoadModeGrowthRate();

	ioINILoader kLoader( GetModeINIFileName( m_ModeType ) );

	kLoader.SetTitle_e( "common_info" );
	m_dwRoundDuration   = kLoader.LoadInt_e( "round_time", 300000 );
	m_dwCurRoundDuration = m_dwRoundDuration;

	m_fScoreWarningRate = kLoader.LoadFloat_e( "score_warning_rate", FLOAT05 );
	m_dwCrownShowGapTime = kLoader.LoadInt_e( "crown_minimap_gap_time", 1000 );

	char szBuf[MAX_PATH];
	kLoader.LoadString_e( "ui_info", "1point_up_emoticon", "", szBuf, MAX_PATH );
	m_PointUpEmoticon = szBuf;

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

	kLoader.SetTitle_e( "king_ping_check" );
	m_dwKingPingTime = kLoader.LoadInt_e( "king_ping_time", 0 );

	//ini파일 변경
	LoadGUIValues();
	LoadPushCursor();
	LoadContribute();

	kLoader.SetFileName_e( "config/sp2_sound.ini" );
	kLoader.SetTitle_e( "sound_file_name" );

	kLoader.LoadString_e( "take_crown_sound", "", szBuf, MAX_PATH );
	m_TakeCrownSound = szBuf;
	kLoader.LoadString_e( "drop_crown_sound", "", szBuf, MAX_PATH );
	m_DropCrownSound = szBuf;

	if( !P2PNetwork::IsNetworkPlaying() )
	{
//		ClearModeData();	싱글모드에서 왕관이 사라져버린다...
		ReStartMode();
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

void ioHiddenkingMode::LoadModeResource( ioINILoader &rkLoader )
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

	g_ModeUIController.SetCenterKillInfoWnd( NULL ); 

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

void ioHiddenkingMode::ProcessMode()
{
	ErrorReport::SetPosition( 1040, 0 );
	ProcessSelectCharacter();
	ErrorReport::SetPosition( 1040, 1 );
	ProcessGUI();
	ErrorReport::SetPosition( 1040, 2 );

	switch( m_ModeState )
	{
	case MS_READY:
		ErrorReport::SetPosition( 1040, 3 );
		ProcessReadyState();
		ErrorReport::SetPosition( 1040, 4 );
		CheckModeMapSound();
		break;
	case MS_PLAY:
		ProcessMapAreaWeapon();
		ErrorReport::SetPosition( 1040, 5 );
		ProcessPlayState();
		ErrorReport::SetPosition( 1040, 6 );
		CheckModeMapSound();
		ErrorReport::SetPosition( 1040, 7 );
		g_EventMgr.Process();
		break;
	case MS_RESULT:
		ErrorReport::SetPosition( 1040, 8 );
		ProcessResultState();
		ErrorReport::SetPosition( 1040, 9 );
		break;
	}
}

void ioHiddenkingMode::ProcessReadyState()
{
	if( P2PNetwork::IsNetworkPlaying() )	return;
	if( m_dwReadyStartTime + 4000 > FRAMEGETTIME() )	return;

	SetModeState( MS_PLAY );
}

void ioHiddenkingMode::ProcessPlayState()
{
	UpdateSoldierRevivalPenalty();
	//UpdateTimeGrowthTime();

	m_pCreator->NotifySpringDampUseSet( true );
	
	ProcessRevival();
	ProcessKing();
	CrownHoldTime();

	if( !P2PNetwork::IsNetworkPlaying() )
	{		
		CheckRoundEnd();
	}
}

void ioHiddenkingMode::ProcessKing()
{
	ioBaseChar *pOwner= m_pCreator->GetOwnerChar();
	if( !pOwner )	return;

	if( m_dwKingPingTime == 0 )
		return;



	ioBaseChar *pKing = GetKing();	
	if( pKing == pOwner )
	{
		// 왕은 일정시간 단위로 서버에 핑을 날린다
		SendKingPingMessage();
		return;
	}

}

void ioHiddenkingMode::CheckRoundEnd()
{
	if( m_dwPlayStartTime + m_dwRoundDuration > FRAMEGETTIME() )
		return;

	WinTeamType eWinTeam = WTT_DRAW;
	if( m_iRedTeamScore > m_iBlueTeamScore )
	{
		eWinTeam = WTT_RED_TEAM;
	}
	else if( m_iRedTeamScore < m_iBlueTeamScore )
	{
		eWinTeam = WTT_BLUE_TEAM;
	}

	bool bRoundSetEnd = false;
	if( m_iCurRoundCount == m_iMaxRoundCount )
		bRoundSetEnd = true;

	SetRoundEndInfo( eWinTeam, bRoundSetEnd );
}

void ioHiddenkingMode::SetRoundEndInfo( WinTeamType eTeam, bool bRoundSetEnd )
{
	ioPlayMode::SetRoundEndInfo( eTeam, bRoundSetEnd );
}

void ioHiddenkingMode::SetResultScoreWnd()
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

void ioHiddenkingMode::SetTotalScoreWnd( IntVec &rvClassTypeList,
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
//	{
//		pTotal->SetRoundHistory( i, m_vRoundHistory[i] );
//	}

	pTotalResult->SetTeamScore( GetOwnerTeam(), m_iBlueRoundWinCnt, m_iRedRoundWinCnt );
	SetTotalResultBriefingWnd();
}

void ioHiddenkingMode::ProcessResultState()
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
			LoadWear();

			ReStartMode();

			if( m_iCurRoundCount == 1 )
			{
				UpdateUserRank();
			}
		}
	}
}

void ioHiddenkingMode::ProcessGUI()
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
	ProcessDeathTimeHelp();
	ProcessAutoStartAlarm();
}

void ioHiddenkingMode::UpdateMiniMapSpotPoint()
{
	MiniMap *pMiniMap = dynamic_cast<MiniMap*>(g_GUIMgr.FindWnd( MINIMAP_WND ));
	if( !pMiniMap )	return;

	pMiniMap->ClearPointList();

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
		if( pChar->IsHasCrown() )
			pMiniMap->AddPoint( vPos.x, vPos.z, MiniMap::SPOT_KING_USER, pChar->GetTeam(), dwGapTime );
		else
		{
			if( pChar->IsDropZoneDownState() )
				pMiniMap->AddPoint( vPos.x, vPos.z, MiniMap::SPOT_DROP_USER, pChar->GetTeam(), 0 );
			else
				pMiniMap->AddPoint( vPos.x, vPos.z, MiniMap::SPOT_USER, pChar->GetTeam(), dwGapTime );
		}
	}

	// Owner
	ioBaseChar *pOwner = GetOwnerChar();
	if( pOwner && pOwner->IsCanMiniMapVisible() )
	{
		dwGapTime = pOwner->GetEventGapTime();
		vPos = pOwner->GetWorldPosition();
		if( pOwner->IsHasCrown() )
		{
			pMiniMap->AddPoint( vPos.x, vPos.z, MiniMap::SPOT_KING_ME, pOwner->GetTeam(), dwGapTime );
		}
		else
		{
			if( pOwner->IsDropZoneDownState() )
				pMiniMap->AddPoint( vPos.x, vPos.z, MiniMap::SPOT_DROP_ME, pOwner->GetTeam(), 0 );
			else
				pMiniMap->AddPoint( vPos.x, vPos.z, MiniMap::SPOT_ME, pOwner->GetTeam(), dwGapTime );
		}
	}

	if( CheckMyStartPositionEventFire() )
	{
		m_dwCrownShowEventTime = FRAMEGETTIME() + m_dwCrownShowGapTime;
	}

	// Crown
	ioFieldItem *pCrown = m_pCreator->GetCrownFieldItem( ioItem::MCT_HIDDEN_KING );
	if( pCrown )
	{
		dwGapTime = pCrown->GetEventGapTime();
		vPos = pCrown->GetWorldPosition();
		pMiniMap->AddPoint( vPos.x, vPos.z, MiniMap::SPOT_CROWN, TEAM_NONE, dwGapTime );

		if( m_dwCrownShowEventTime > 0 && m_dwCrownShowEventTime < FRAMEGETTIME() )
		{
			SetMiniMapEvent( vPos, MiniMap::EVENT_CROWN_START_POSITION, TEAM_NONE );
			m_dwCrownShowEventTime = 0;
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

void ioHiddenkingMode::UpdateTimeScoreWnd()
{
	TimeScoreWnd *pTimeScoreWnd = dynamic_cast<TimeScoreWnd*>( g_GUIMgr.FindWnd( TIME_SCORE_WND ) );
	if(!pTimeScoreWnd) return;	
	
	ioBaseChar *pKingUser = GetKing();
	if( pKingUser && GetModeState() == MS_PLAY )
	{		
		g_ModeUIController.SetKingGauge( true, pKingUser->GetTeam(), GetOwnerTeam(), m_dwGivePointTime, m_dwCrownHoldTime, m_fScoreWarningRate );
		if( g_ModeUIController.IsHiddenKingWarning() )
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
		g_ModeUIController.SetKingGauge( false, TEAM_NONE, TEAM_NONE, 0, 0, 0.0f );
		ProcessScoreWarningSound( false );
	}

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

void ioHiddenkingMode::UpdateRoundStateWnd()
{
	if( g_MyInfo.IsSortContribute() )
	{
		DWORD dwTimeGap = FRAMEGETTIME() - m_dwPlayStartTime;
		if( m_dwCurRoundDuration > dwTimeGap )
			ContributeRoundAllPercent( m_dwRoundDuration - (m_dwCurRoundDuration - dwTimeGap), m_dwRoundDuration );
		else
			ContributeRoundAllPercent( m_dwRoundDuration, m_dwRoundDuration );

		HiddenkingRecord *pRecord = FindRecordCharName( g_MyInfo.GetPublicID() );
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

void ioHiddenkingMode::ProcessRevival()
{
    DWORD dwCurTime = FRAMEGETTIME();
	
	int iCharCnt = m_vCharRecordList.size();
	for( int i=0 ; i<iCharCnt ; i++ )
	{
		HiddenkingRecord &rkRecord = m_vCharRecordList[i];
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

void ioHiddenkingMode::RenderPreGUI()
{
	if( m_ModeState == MS_PLAY )
	{
		RenderPushEnableCursor();
	}
}

void ioHiddenkingMode::RenderAfterGUI()
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
		g_FontMgr.PrintText( iXPos, iYPos, 2.5f, STR(1), iContributePer );
	}
	*/
}

D3DXVECTOR3 ioHiddenkingMode::GetRevivalPos( ioBaseChar *pChar )
{
	int iPosArray = 0;

	IORandom random;
	random.SetRandomSeed( m_dwRandomSeed );
	iPosArray = random.Random( m_vRevivalPos.size() );

	int iMaxRevivalPos = m_vRevivalPos.size();
	if( !COMPARE( iPosArray, 0, iMaxRevivalPos ) )
	{
		return m_vMapCenter;
	}

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

void ioHiddenkingMode::NotifyEntityDie( ioPlayEntity *pEntity, ioPlayEntity *pKiller, ioBaseChar *pBestAttacker,
									   float fLastRate, float fBestRate )
{
	ioBaseChar *pDieChar = ToBaseChar( pEntity );
	ioBaseChar *pKillChar = ToBaseChar( pKiller );

	OnNotifyCharDie( pDieChar, pKillChar, pBestAttacker, fLastRate, fBestRate );
	UpdateKillDeathRecord( pEntity, pKiller, pBestAttacker );
}

void ioHiddenkingMode::NotifyCharDropDie( ioBaseChar *pDieChar, ioBaseChar *pKillChar, ioBaseChar *pBestAttacker,
										  float fLastRate, float fBestRate )
{
	OnNotifyCharDie( pDieChar, pKillChar, pBestAttacker, fLastRate, fBestRate );
	UpdateDropDieRecord( pDieChar, pKillChar, pBestAttacker );
}

void ioHiddenkingMode::OnNotifyCharDie( ioBaseChar *pDieChar, ioBaseChar *pKillChar, ioBaseChar *pBestAttacker,
									   float fLastRate, float fBestRate )
{
	if( !pDieChar )
	{
		LOG.PrintTimeAndLog( 0, "ioHiddenkingMode::OnNotifyCharDie() - DieChar is NULL" );
		return;
	}

	HiddenkingRecord *pDieRecord = FindRecordCharName( pDieChar->GetCharName() );
	if( !pDieRecord )
	{
		LOG.PrintTimeAndLog( 0, "ioHiddenkingMode::OnNotifyCharDie - %s Not Exit Char",
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

void ioHiddenkingMode::AddNewRecord( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	HiddenkingRecord *pHiddenkingRecord = FindRecordCharName( pChar->GetCharName() );
	if( !pHiddenkingRecord )
	{
		HiddenkingRecord kRecord;
		kRecord.pChar = pChar;
		kRecord.szName= pChar->GetCharName();
		m_vCharRecordList.push_back( kRecord );

		UpdateUserRank();

		if( pChar )
		{
			if( !g_BattleRoomMgr.IsObserverUser( pChar->GetCharName() ) )
				g_AwardMgr.AddNewAwardUser( pChar->GetCharName() );
			else if( g_ShuffleRoomMgr.IsShuffleRoom() )
				g_AwardMgr.AddNewAwardUser( pChar->GetCharName() );
			else if( g_RankBattleMgr.IsRankBattlePlaying() )
				g_AwardMgr.AddNewAwardUser( pChar->GetCharName() );
		}
	}
	else
	{
		pHiddenkingRecord->pChar = pChar;
	}
}

void ioHiddenkingMode::RemoveRecord( const ioHashString &rkName )
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


void ioHiddenkingMode::RemoveRecord( int nIndex )
{
	int iCharCnt = m_vCharRecordList.size();
	if( iCharCnt > nIndex )
	{
		m_vCharRecordList.erase( m_vCharRecordList.begin() + nIndex );
		UpdateUserRank();
	}
}


ioBaseChar* ioHiddenkingMode::GetRecordChar( int i )
{
	if( COMPARE( i, 0, GetRecordCharCnt() ) )
		return m_vCharRecordList[i].pChar;

	return NULL;
}

ioBaseChar* ioHiddenkingMode::GetRecordChar( const ioHashString &rkName )
{
	HiddenkingRecord *pRecord = FindRecordCharName( rkName );
	if( pRecord )
		return pRecord->pChar;

	return NULL;
}

int ioHiddenkingMode::GetRecordCharCnt() const
{
	return m_vCharRecordList.size();
}

ModeRecord* ioHiddenkingMode::FindModeRecord( int iIdx )
{
	if( COMPARE( iIdx, 0, GetRecordCharCnt() ) )
		return &m_vCharRecordList[iIdx];

	return NULL;
}

ModeRecord* ioHiddenkingMode::FindModeRecordCharName( const ioHashString &rkName )
{
	return (ModeRecord*)FindRecordCharName( rkName );
}

HiddenkingRecord* ioHiddenkingMode::FindRecordCharName( const ioHashString &rkName )
{
	int iCharCnt = m_vCharRecordList.size();
	for( int i=0 ; i<iCharCnt ; i++ )
	{
		if( m_vCharRecordList[i].szName == rkName )
			return &m_vCharRecordList[i];
	}

	return NULL;
}

bool ioHiddenkingMode::GetRoundInfo( int iIndex, RoundInfo &rkInfo )
{
	if( ioPlayMode::GetRoundInfo( iIndex, rkInfo ) )
	{
		HiddenkingRecord *pRecord = (HiddenkingRecord*)FindModeRecord( iIndex );
		if( !pRecord )	return false;
		if( !pRecord->pChar ) return false;

		rkInfo.m_iContribute = pRecord->Contribute.m_iContribute;
		rkInfo.m_iContributePer = pRecord->Contribute.m_iPrevContributePer;
		return true;
	}
	return false;
}

bool ioHiddenkingMode::GetResultInfo( int iIndex, ResultInfo &rkInfo )
{
	if( ioPlayMode::GetResultInfo( iIndex, rkInfo ) )
	{
		HiddenkingRecord *pRecord = (HiddenkingRecord*)FindModeRecord( iIndex );
		if( !pRecord )	return false;
		if( !pRecord->pChar ) return false;

		rkInfo.m_iContribute = pRecord->Contribute.m_iContribute;
		rkInfo.m_iContributePer = pRecord->Contribute.m_iPrevContributePer;
		return true;
	}
	return false;
}

bool ioHiddenkingMode::GetFinalInfo( int iIndex, FinalInfo &rkInfo )
{
	if( ioPlayMode::GetFinalInfo( iIndex, rkInfo ) )
	{
		HiddenkingRecord *pRecord = (HiddenkingRecord*)FindModeRecord( iIndex );
		if( !pRecord )	return false;
		if( !pRecord->pChar ) return false;

		rkInfo.m_iContribute = pRecord->Contribute.m_iContribute;
		rkInfo.m_iContributePer = pRecord->Contribute.m_iPrevContributePer;
		return true;
	}
	return false;
}

void ioHiddenkingMode::ReStartMode()
{
	D3DXVECTOR3 vStartPos;

	int iCharCnt = m_vCharRecordList.size();
	for( int i=0 ; i<iCharCnt ; i++ )
	{
		HiddenkingRecord &rkRecord = m_vCharRecordList[i];
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
			m_iRedRoundWinCnt  = 0;
		}

		rkRecord.pChar->SetProtectionMode( PM_NONE, 0 );
		rkRecord.pChar->SetRoundEndMotion( RM_NONE );
		rkRecord.pChar->SetProtectionMode( PM_START, 2000 );
		rkRecord.pChar->Revival();

		if( rkRecord.pChar->GetState() != CS_OBSERVER )
		{
			if( !g_BattleRoomMgr.IsObserverUser( rkRecord.pChar->GetCharName() ) )
				rkRecord.pChar->SetReadyState();
			else if( g_ShuffleRoomMgr.IsShuffleRoom() )
				rkRecord.pChar->SetReadyState();
			else if( g_RankBattleMgr.IsRankBattlePlaying() )
				rkRecord.pChar->SetReadyState();
			else
				rkRecord.pChar->SetState( CS_OBSERVER );
		}
	}

	m_bLastPoint = 0;
	m_iBlueTeamScore = 0;
	m_iRedTeamScore  = 0;

	m_dwCurRevivalTime = 0;
	m_dwCheckWarningTime = 0;
	m_bScoreWarning = false;
	m_bFirstSelect = true;

	//
	m_dwSendCheckKingTime = 0;
	m_iCurSendIndex = 1;
	//

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

void ioHiddenkingMode::ContributeLOG()
{
	char szLog[MAX_PATH];
	int iCharCnt = m_vCharRecordList.size();
	for( int i=0 ;i<iCharCnt;i++)
	{
		HiddenkingRecord *pRecord = &m_vCharRecordList[i];
		if( pRecord->pChar )
		{
			sprintf( szLog, "%s[%d] : C%d(%d) M%d(%d) K%d(%d) D%d(%d) A%d(%d)", pRecord->szName.c_str(), (int)pRecord->pChar->GetTeam(),
																				pRecord->Contribute.m_iKingTimeRank + 1, pRecord->Contribute.m_iKingTime,
																				pRecord->Contribute.m_iKingPointRank + 1, pRecord->Contribute.m_iKingPoint,
																				pRecord->Contribute.m_iKillRank + 1, pRecord->Contribute.m_iKill,
                                                                                pRecord->Contribute.m_iDamageRank + 1, pRecord->Contribute.m_iDamage,
                                                                                pRecord->Contribute.m_iAttackCntRank + 1, pRecord->Contribute.m_iAttackCnt );
			g_ChatMgr.SetSystemMsg( szLog );
		}		
	}
}

void ioHiddenkingMode::ContributeConvertTeam( TeamType eTeam )
{
	if( m_vContributeTable.size() < MAX_PLAYER )
		return;

	int i = 0;
	int iCharCnt = m_vCharRecordList.size();
	int iInfoCnt = 0;
	ContributeInfoList vInfo;

	//왕관 보유 시간
	for(i = 0;i < iCharCnt;i++)
	{
		HiddenkingRecord *pRecord = &m_vCharRecordList[i];
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
		Contribute.m_iContributePoint = pRecord->Contribute.m_iKingTime;
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
			HiddenkingRecord *pRecord = &m_vCharRecordList[iArray];
			if( !pRecord->Contribute.m_bLastResultSync )
				pRecord->Contribute.m_iContribute = (float)m_vContributeTable[vInfo[i].m_iRank] * 1.5f;
			pRecord->Contribute.m_iKingTimeRank = vInfo[i].m_iRank;
		}
	}
	vInfo.clear();

	//왕관 점수 획득
	for(i = 0;i < iCharCnt;i++)
	{
		HiddenkingRecord *pRecord = &m_vCharRecordList[i];
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
		Contribute.m_iContributePoint = pRecord->Contribute.m_iKingPoint;
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
			HiddenkingRecord *pRecord = &m_vCharRecordList[iArray];
			if( !pRecord->Contribute.m_bLastResultSync )
				pRecord->Contribute.m_iContribute += (float)m_vContributeTable[vInfo[i].m_iRank] * 1.5f;
			pRecord->Contribute.m_iKingPointRank = vInfo[i].m_iRank;
		}
	}
	vInfo.clear();

	//KO
	for(i = 0;i < iCharCnt;i++)
	{
		HiddenkingRecord *pRecord = &m_vCharRecordList[i];
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
			HiddenkingRecord *pRecord = &m_vCharRecordList[iArray];
			if( !pRecord->Contribute.m_bLastResultSync )
				pRecord->Contribute.m_iContribute += m_vContributeTable[vInfo[i].m_iRank] * 2;
			pRecord->Contribute.m_iKillRank = vInfo[i].m_iRank;
		}
	}
	vInfo.clear();

	//데미지 & 힐
	for(i = 0;i < iCharCnt;i++)
	{
		HiddenkingRecord *pRecord = &m_vCharRecordList[i];
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
			HiddenkingRecord *pRecord = &m_vCharRecordList[iArray];
			if( !pRecord->Contribute.m_bLastResultSync )
				pRecord->Contribute.m_iContribute += m_vContributeTable[vInfo[i].m_iRank] * 2;
			pRecord->Contribute.m_iDamageRank = vInfo[i].m_iRank;
		}
	}
	vInfo.clear();

	//유효 타격 횟수
	for(i = 0;i < iCharCnt;i++)
	{
		HiddenkingRecord *pRecord = &m_vCharRecordList[i];
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
			HiddenkingRecord *pRecord = &m_vCharRecordList[iArray];
			if( !pRecord->Contribute.m_bLastResultSync )
				pRecord->Contribute.m_iContribute += m_vContributeTable[vInfo[i].m_iRank] * 2;
			pRecord->Contribute.m_iAttackCntRank = vInfo[i].m_iRank;
		}
	}
	vInfo.clear();
}

void ioHiddenkingMode::ContributeConvert()
{
	ContributeConvertTeam( TEAM_NONE );        //TEAM_NONE는 전체 유저에 대한 기여도 평가
}

int ioHiddenkingMode::ContributeMyRank( int &rRankValue1, int &rRankValue2, int &rRankValue3, int &rRankValue4 )
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

	//왕관 보유 시간
	for(i = 0;i < iCharCnt;i++)
	{
		HiddenkingRecord *pRecord = &m_vCharRecordList[i];
		if( !pRecord ) continue;
		if( !pRecord->pChar ) continue;
		if( pRecord->pChar->GetState() == CS_OBSERVER ) continue;
		if( g_BattleRoomMgr.IsObserverUser(pRecord->pChar->GetCharName()) ) continue;

		ContributeInfo Contribute;
		Contribute.m_iRecordArray     = i;
		Contribute.m_iContributePoint = pRecord->Contribute.m_iKingTime;
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
		HiddenkingRecord *pRecord = &m_vCharRecordList[i];
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
		HiddenkingRecord *pRecord = &m_vCharRecordList[i];
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
		HiddenkingRecord *pRecord = &m_vCharRecordList[i];
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

void ioHiddenkingMode::ContributeRoundPercent( DWORD dwCurTime, DWORD dwTotalTime )
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
		HiddenkingRecord *pRecord = &m_vCharRecordList[i];
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
		HiddenkingRecord *pRecord = &m_vCharRecordList[i];

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
		HiddenkingRecord *pRecord = &m_vCharRecordList[i];

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

void ioHiddenkingMode::ContributeRoundAllPercent( DWORD dwCurTime, DWORD dwTotalTime )
{
	//기여도의 100분율
	int iMaxContribute	= 0;
	int iCharCnt		= m_vCharRecordList.size();
	int iPlayCharCnt	= 0;

	int i = 0;
	for(i = 0;i < iCharCnt;i++)
	{
		HiddenkingRecord *pRecord = &m_vCharRecordList[i];
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
			HiddenkingRecord *pRecord = &m_vCharRecordList[i];
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

bool ioHiddenkingMode::IsInKingRange( ioBaseChar *pChar, float fRange )
{
	ioBaseChar *pKing = GetKing();
	if( !pKing || !pChar ) return false;

	if( pChar->GetState() == CS_DIE ||
		pChar->GetState() == CS_READY ||
		pChar->GetState() == CS_LOADING ||
		pChar->GetState() == CS_OBSERVER ||
		pChar->GetState() == CS_CHANGE_WAIT ||
		pChar->GetState() == CS_VIEW ) 
		return false;

    float fRangeSq = fRange * fRange;
	D3DXVECTOR3 vDiff = pChar->GetWorldPosition() - pKing->GetWorldPosition();
	if( D3DXVec3LengthSq( &vDiff ) > fRangeSq )
		return false;

	return true;
}

void ioHiddenkingMode::DamageContribute( const ioHashString szAttacker, const ioHashString szTarget, float fDamage, DWORD dwItemCode, int iWeaponType )
{
	if( szAttacker.IsEmpty() ) return;

	HiddenkingRecord *pRecord = FindRecordCharName( szAttacker );
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

	g_AwardMgr.AddDamage( pRecord->szName, fDamagePoint, fDamage );

	HiddenkingRecord *pTargetRecord = FindRecordCharName( szTarget );
	if( !pTargetRecord || !pTargetRecord->pChar ) return;
	
	ioBaseChar *pTargetChar = m_pCreator->GetBaseChar( szTarget );
	if( !pTargetChar ) return;

	if( pTargetRecord->pChar == GetKing() )    //왕 타격 데미지 추가 
		pRecord->Contribute.m_iDamage += (float)( fDamagePoint * 2 ) * fAddDamagePoint;

	if( fDamagePoint > 0.0f && !pAttacker->IsOwnerChar() )
	{
		CheckNonePlayTime( pAttacker );
	}

	ContributeConvert();
	ContributeRoundAllPercent();
	UpdateRoundInfoList();
	CheckTotalChamp();
}

void ioHiddenkingMode::KillContribute( const ioHashString szKiller, const ioHashString szKilled, DWORD dwItemCode )
{
	if( szKilled.IsEmpty() ) return;

	int iKillPoint = 100;
	
	float fAddKillPoint = FLOAT1;
	if( IsContributeAddPointItemCode( dwItemCode ) )
		fAddKillPoint = 1.1f;

	if( szKiller.IsEmpty() || szKiller == szKilled )     //자살
	{
		HiddenkingRecord *pRecord = FindRecordCharName( szKilled );
		if( !pRecord || !pRecord->pChar ) return;

		pRecord->Contribute.m_iKill -= iKillPoint;
	}
	else
	{
		HiddenkingRecord *pKillerRecord = FindRecordCharName( szKiller );
		HiddenkingRecord *pKilledRecord = FindRecordCharName( szKilled );
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

void ioHiddenkingMode::AttackCntContribute( const ioHashString szAttacker, const ioHashString szTarget, DWORD dwItemCode, int iAttackValue/* = 100 */ )
{
	if( szAttacker.IsEmpty() || szTarget.IsEmpty() ) return;

	HiddenkingRecord *pAttackerRecord = FindRecordCharName( szAttacker );
	HiddenkingRecord *pTargetRecord = FindRecordCharName( szTarget );

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
			if( GetKing() == pTargetRecord->pChar )    //왕 타격 횟수 추가 
			{
				pAttackerRecord->Contribute.m_iAttackCnt += (float)(iAttackValue * 2) * fAddAttackPoint;
				g_AwardMgr.AddOtherKingAttack( szAttacker );

				if( pAttackerRecord->pChar->IsOwnerChar() )
					g_QuestMgr.QuestCompleteTerm( QC_OTHER_KING_ATTACK );
			}
		}
	}

	ContributeConvert();
	ContributeRoundAllPercent();
	UpdateRoundInfoList();
	CheckTotalChamp();
}

void ioHiddenkingMode::CrownHoldTime()
{
	ioBaseChar *pKing = GetKing();
	if( !pKing ) return;

	HiddenkingRecord *pRecord = FindRecordCharName( pKing->GetCharName() );
	if( !pRecord ) return;

	if( FRAMEGETTIME() - m_dwCrownHoldTime > 500 )
	{
		m_dwCrownHoldTime = FRAMEGETTIME();		
		pRecord->Contribute.m_iKingTime += 1000;            //왕관 보유유저
		g_AwardMgr.AddKing( pKing->GetCharName(), 500 );

		if( pKing->IsOwnerChar() )
		{
			g_QuestMgr.QuestCompleteTerm( QC_CROWN_HOLD_TIME, 500 );
		}

		//1000Pixel 이내의 유저들에게 0.75f 적용
		int iCharCnt = m_vCharRecordList.size();
		for(int i = 0;i < iCharCnt;i++)
		{
			HiddenkingRecord *pUserRecord = &m_vCharRecordList[i];
			if( !pUserRecord ) continue;
			if( pUserRecord == pRecord ) continue;
			if( !pUserRecord->pChar ) continue;
			if( !IsInKingRange( pUserRecord->pChar, 750.0f ) ) continue;

            pUserRecord->Contribute.m_iKingTime += (float)500 * 0.60f;				

			if( pUserRecord->pChar != pKing && pUserRecord->pChar->GetTeam() == pKing->GetTeam() )
				g_AwardMgr.AddKingGuard( pUserRecord->szName, 500 );
		}		
		
		ContributeConvert();
		ContributeRoundAllPercent();
		UpdateRoundInfoList();
		CheckTotalChamp();
	}
}

void ioHiddenkingMode::CrownHoldScore( const ioHashString szKing, bool bLastPoint )
{
	if( szKing.IsEmpty() ) return;

	HiddenkingRecord *pRecord = FindRecordCharName( szKing );
	if( !pRecord ) return;

	if( bLastPoint )
	{
		pRecord->Contribute.m_iKingPoint += 300;
		g_AwardMgr.AddKing( szKing, 10000 );
		//1000Pixel 이내의 유저들에게 0.75f 적용
		int iCharCnt = m_vCharRecordList.size();
		for(int i = 0;i < iCharCnt;i++)
		{
			HiddenkingRecord *pUserRecord = &m_vCharRecordList[i];
			if( !pUserRecord ) continue;
			if( pUserRecord == pRecord ) continue;
			if( !pUserRecord->pChar ) continue;
			if( pUserRecord->pChar->GetTeam() != pRecord->pChar->GetTeam() ) continue;
			if( !IsInKingRange( pUserRecord->pChar, 750.0f ) ) continue;

			pUserRecord->Contribute.m_iKingPoint += (float)300 * 0.60f;				
		}		
	}
	else
	{
		pRecord->Contribute.m_iKingPoint += 100;
		g_AwardMgr.AddKing( szKing, 10000 );
		//1000Pixel 이내의 유저들에게 0.75f 적용
		int iCharCnt = m_vCharRecordList.size();
		for(int i = 0;i < iCharCnt;i++)
		{
			HiddenkingRecord *pUserRecord = &m_vCharRecordList[i];
			if( !pUserRecord ) continue;
			if( pUserRecord == pRecord ) continue;
			if( !pUserRecord->pChar ) continue;
			if( pUserRecord->pChar->GetTeam() != pRecord->pChar->GetTeam() ) continue;
			if( !IsInKingRange( pUserRecord->pChar, 750.0f ) ) continue;

			pUserRecord->Contribute.m_iKingPoint += (float)100 * 0.60f;				
		}		
	}

	ContributeConvert();
	ContributeRoundAllPercent();
	UpdateRoundInfoList();
	CheckTotalChamp();
}

void ioHiddenkingMode::CrownDrop( const ioHashString szAttacker, const ioHashString szKing )
{
	if( szAttacker.IsEmpty() || szKing.IsEmpty() ) return;

	//
	m_dwSendCheckKingTime = 0;
	m_iCurSendIndex = 1;
	//

	HiddenkingRecord *pAttackerRecord = FindRecordCharName( szAttacker );
	HiddenkingRecord *pKingRecord = FindRecordCharName( szKing );
	if( pAttackerRecord && pKingRecord )
	{
		if( pAttackerRecord->pChar && pKingRecord->pChar )
		{
			if( pAttackerRecord->pChar->GetTeam() == pKingRecord->pChar->GetTeam() )  
			{
				pAttackerRecord->Contribute.m_iKingPoint -= 100;
				g_AwardMgr.AddTeamKingAttack( szAttacker );
			}
			else
			{
				pAttackerRecord->Contribute.m_iKingPoint += 100;
				g_AwardMgr.AddOtherKingAttack( szAttacker, 10 );

				if( pAttackerRecord->pChar->IsOwnerChar() )
					g_QuestMgr.QuestCompleteTerm( QC_OTHER_KING_ATTACK );
			}

			// 왕관드롭 메세지
			g_ChatMgr.SetDropCrownMsg( pAttackerRecord->pChar->GetTeam(),
									   szAttacker,
									   pKingRecord->pChar->GetTeam(),
									   szKing );
		}
	}

	ContributeConvert();
	ContributeRoundAllPercent();
	UpdateRoundInfoList();
	CheckTotalChamp();
}

void ioHiddenkingMode::CrownBadPingDrop( const ioHashString szKing )
{
	if( szKing.IsEmpty() ) return;

	//
	m_dwSendCheckKingTime = 0;
	m_iCurSendIndex = 1;
	//

	HiddenkingRecord *pKingRecord = FindRecordCharName( szKing );
	if( pKingRecord )
	{
		// 왕관드롭 메세지
		g_ChatMgr.SetBadPingCrownDropMsg( pKingRecord->pChar->GetTeam(), szKing );
	}

	ContributeConvert();
	ContributeRoundAllPercent();
	UpdateRoundInfoList();
	CheckTotalChamp();
}

void ioHiddenkingMode::FillLastPlayRecordInfo( SP2Packet &rkPacket )
{
	int iCharCnt = m_vCharRecordList.size();
	rkPacket << iCharCnt;
	for(int i = 0;i < iCharCnt;i++)
	{
		HiddenkingRecord *pRecord = &m_vCharRecordList[i];
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

void ioHiddenkingMode::FillPlayRecordInfo( SP2Packet &rkPacket )
{
	int iCharCnt = m_vCharRecordList.size();
	rkPacket << iCharCnt;
	for(int i = 0;i < iCharCnt;i++)
	{
		HiddenkingRecord *pRecord = &m_vCharRecordList[i];
		rkPacket << pRecord->pChar->GetCharName();
		rkPacket << pRecord->Contribute.m_iDamage;
		rkPacket << pRecord->Contribute.m_iAttackCnt;
		rkPacket << pRecord->Contribute.m_iKill;
		rkPacket << pRecord->Contribute.m_iKingTime;
		rkPacket << pRecord->Contribute.m_iKingPoint;
		rkPacket << pRecord->Contribute.m_iPrevContributePer;
		rkPacket << pRecord->GetUniqueTotalKill();
		rkPacket << pRecord->GetUniqueTotalDeath();
		rkPacket << g_VictoriesMgr.GetVictories( pRecord->pChar->GetCharName() );
	}
}

void ioHiddenkingMode::ApplyPlayRecordInfo( SP2Packet &rkPacket )
{
	int iCharCnt;
	rkPacket >> iCharCnt;
	for(int i = 0;i < iCharCnt;i++)
	{
		ioHashString szName;
		int iDamage, iAttackCnt, iKill, iKingTime, iKingPoint, iPrevContributePer, iUniqueTotalKill, iUniqueTotalDeath, iVictories;
		rkPacket >> szName >> iDamage >> iAttackCnt >> iKill >> iKingTime >> iKingPoint >> iPrevContributePer >> iUniqueTotalKill >> iUniqueTotalDeath >> iVictories;

		HiddenkingRecord *pRecord = FindRecordCharName( szName );
		if( pRecord )
		{
			pRecord->Contribute.m_iDamage		     = iDamage;
			pRecord->Contribute.m_iAttackCnt	     = iAttackCnt;
			pRecord->Contribute.m_iKill              = iKill;
			pRecord->Contribute.m_iKingTime		     = iKingTime;
			pRecord->Contribute.m_iKingPoint         = iKingPoint;
			pRecord->Contribute.m_iPrevContributePer = iPrevContributePer;
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

void ioHiddenkingMode::ContributeResultSync( const ioHashString &szName, int iContribute )
{
	HiddenkingRecord *pRecord = FindRecordCharName( szName );
	if( !pRecord ) return;

	pRecord->Contribute.m_bLastResultSync = true;
	pRecord->Contribute.m_iContribute     = iContribute;
}

bool ioHiddenkingMode::ProcessTCPPacket( SP2Packet &rkPacket )
{
	ErrorReport::SetPosition( 1040, 9 );

	if( ioPlayMode::ProcessTCPPacket( rkPacket ) )
		return true;

	ErrorReport::SetPosition( 1040, 10 );

	switch( rkPacket.GetPacketID() )
	{
	case STPK_KING_TAKE:
		ErrorReport::SetPosition( 1040, 11 );
		OnKingTake( rkPacket );
		ErrorReport::SetPosition( 1040, 12 );
		return true;
	}

	return false;
}

bool ioHiddenkingMode::ProcessUDPPacket( SP2Packet &rkPacket )
{
	if( ioPlayMode::ProcessUDPPacket( rkPacket ) )
		return true;

	return false;
}

void ioHiddenkingMode::ApplyModeInfo( SP2Packet &rkPacket )
{
	rkPacket >> m_iCurRoundCount;
	rkPacket >> m_iMaxRoundCount;

	rkPacket >> m_dwRoundDuration;
	//m_dwCurRoundDuration = m_dwRoundDuration;

	rkPacket >> m_iBlueRoundWinCnt;
	rkPacket >> m_iRedRoundWinCnt;

	rkPacket >> m_iBluePosArray;
	rkPacket >> m_iRedPosArray;

	DWORD dwRemainTime;
	int iKingTeam;

	rkPacket >> m_iBlueTeamScore;
	rkPacket >> m_iRedTeamScore;
	rkPacket >> iKingTeam;
	rkPacket >> m_dwGivePointTime;
	rkPacket >> dwRemainTime;

	m_vRoundHistory.erase( m_vRoundHistory.begin(), m_vRoundHistory.end() );
	m_vRoundHistory.reserve( m_iCurRoundCount );

	for(int i = 0; i < m_iCurRoundCount; i++)
	{
		RoundHistory rh;
		rkPacket >> rh.iBluePoint;
		rkPacket >> rh.iRedPoint;
		m_vRoundHistory.push_back( rh );
	}
	UpdateRoundStateWnd();
	UpdateTimeScoreWnd();
}

void ioHiddenkingMode::ApplyCharModeInfo( const ioHashString &rkName, SP2Packet &rkPacket, bool bDieCheck )
{
	bool bRecord;
	rkPacket >> bRecord;

	if( !bRecord ) return;

	HiddenkingRecord *pRecord = FindRecordCharName( rkName );
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

void ioHiddenkingMode::OnZeroHP( SP2Packet &rkPacket )
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

void ioHiddenkingMode::ClearKing()
{
	int iCharCnt = m_vCharRecordList.size();
	for( int i=0; i<iCharCnt; i++ )
	{
		ioBaseChar *pChar = GetRecordChar( i );
		if(pChar)	
		{
			ioWearItem *pWear = pChar->GetWear();
			if( pWear )
			{
				ioItem *pItem = pChar->ReleaseItemExplicit( pWear->GetItemCode(),
															pWear->GetItemCreateIndex(),
															ES_WEAR );

				SAFEDELETE( pItem );
			}
		}
	}
}

void ioHiddenkingMode::SetEventTakeKing(const ioHashString &rkNewKingName)
{
	if( rkNewKingName.IsEmpty() )
	{

	}
	else
	{
		ioBaseChar *pChar = GetRecordChar( rkNewKingName );
		if( pChar )
		{
			g_ChatMgr.SetGetCrownMsg( pChar->GetTeam(), rkNewKingName );

			SetMiniMapEvent( pChar->GetWorldPosition(), MiniMap::EVENT_CROWN_ACQUIRE, pChar->GetTeam() );
		}
		else
		{
		}
	}
}

ioBaseChar* ioHiddenkingMode::GetKing()
{
	int iCharCnt = m_vCharRecordList.size();

	for(int i = 0; i < iCharCnt; i++)
	{
		HiddenkingRecord &rkRecord = m_vCharRecordList[i];

		ioBaseChar *pChar = m_pCreator->GetBaseChar( rkRecord.szName );
		if( pChar && rkRecord.pChar )
		{
			if( rkRecord.pChar->IsHasCrown() )
				return rkRecord.pChar;
		}
	}

	return NULL;
}

void ioHiddenkingMode::OnScore( SP2Packet &rkPacket )
{
	int iBackBlueScore = m_iBlueTeamScore;
	int iBackRedScore  = m_iRedTeamScore;
	
	ScoreSound eScoreSound = SS_DEFAULT;

	// 모드 : 09.06.03
	//int iTeamType;

	rkPacket >> m_bLastPoint;
	// 모드 : 09.06.03
	//rkPacket >> iTeamType;
	rkPacket >> m_iBlueTeamScore;
	rkPacket >> m_iRedTeamScore;
	rkPacket >> m_dwGivePointTime;

	if( m_iBlueTeamScore > 0 && m_iRedTeamScore > 0 &&
		m_iBlueTeamScore == m_iRedTeamScore )
	{
		eScoreSound = SS_EQUAL;
	}

	ioBaseChar *pKing = GetKing();
	if( m_iBlueTeamScore > iBackBlueScore )
	{
		if( pKing ) 
		{
			if( pKing->GetTeam() == TEAM_BLUE )
			{			
				if( iBackBlueScore == 0 && m_iBlueTeamScore > 0 )
				{
					eScoreSound = SS_FIRST;
				}
			}

			CrownHoldScore( pKing->GetCharName(), m_bLastPoint );
		}

		PointUpEvent( TEAM_BLUE );

		// 스코어 획득 메세지
		if( m_bLastPoint )
		{
			if( pKing )
				g_ChatMgr.SetLastScoreMsg( TEAM_BLUE, pKing->GetCharName() );
			else
				g_ChatMgr.SetLastScoreMsg( TEAM_BLUE, "" );
		}
		
		{
			g_ChatMgr.SetScoreMsg( TEAM_BLUE, "", false );
		}
	}
	else if( m_iRedTeamScore > iBackRedScore )
	{
		if( pKing ) 
		{
			if( pKing->GetTeam() == TEAM_RED )
			{			
				if( iBackRedScore == 0 && m_iRedTeamScore > 0 )
				{
					eScoreSound = SS_FIRST;
				}
			}

			CrownHoldScore( pKing->GetCharName(), m_bLastPoint );
		}

		PointUpEvent( TEAM_RED );

		// 스코어 획득 메세지
		if( m_bLastPoint )
		{
			if( pKing )
				g_ChatMgr.SetLastScoreMsg( TEAM_RED, pKing->GetCharName() );
			else
				g_ChatMgr.SetLastScoreMsg( TEAM_RED, "" );
		}

		{
			g_ChatMgr.SetScoreMsg( TEAM_RED, "", false );
		}
	}

	if( m_bLastPoint )
	{
		eScoreSound = SS_LAST;
	}

	UpdateRoundStateWnd();
	UpdateTimeScoreWnd();

	if( pKing )
	{
		ProcessScoreSound( pKing->GetTeam(), eScoreSound );
	}
}

void ioHiddenkingMode::OnRoundReady( SP2Packet &rkPacket )
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

void ioHiddenkingMode::OnRoundJoin( SP2Packet &rkPacket )
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

void ioHiddenkingMode::OnRoundJoinView( SP2Packet &rkPacket )
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

void ioHiddenkingMode::OnRoundJoinObserver( SP2Packet &rkPacket )
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

void ioHiddenkingMode::OnKingTake( SP2Packet &rkPacket )
{
	ioHashString szKingName;
	rkPacket >> szKingName;
	rkPacket >> m_dwGivePointTime;

	SetEventTakeKing( szKingName );
	m_dwCrownHoldTime = FRAMEGETTIME();
}

void ioHiddenkingMode::NotifyDropItem( ioItem *pItem, ioBaseChar *pChar )
{
	if( !pItem ) return;
	if( pItem->GetCrownType() != ioItem::MCT_HIDDEN_KING ) return;

	ioFieldItem *pCrown = m_pCreator->GetCrownFieldItem( ioItem::MCT_HIDDEN_KING );
	if( !pCrown )	return;

	if( pChar )
	{
		SetMiniMapEvent( pCrown->GetWorldPosition(), MiniMap::EVENT_CROWN_DROP, pChar->GetTeam() );

		if( pChar->GetTeam() == GetOwnerTeam() )
		{
			g_SoundMgr.PlaySound( m_DropCrownSound );
		}
	}
}

void ioHiddenkingMode::NotifyPickItem( ioItem *pItem, ioBaseChar *pChar )
{
	if( !pItem || !pChar ) return;
	if( pItem->GetCrownType() == ioItem::MCT_NONE )
		return;

	//pChar->RemoveActiveBuff();
	m_dwSendCheckKingTime = 0;
	m_iCurSendIndex = 1;

	if( pChar->GetTeam() == GetOwnerTeam() )
	{
		g_SoundMgr.PlaySound( m_TakeCrownSound );
	}

	if( !P2PNetwork::IsNetworkPlaying() )
	{
		m_dwGivePointTime = 60000;	// 서버에서 신호를 못받기때문에 0이라서 UI가 안나옴;;
		m_dwCrownHoldTime = FRAMEGETTIME();

		SetEventTakeKing( pChar->GetCharName() );
	}
}

void ioHiddenkingMode::LoadWear()
{
	if( !m_pCreator || P2PNetwork::IsNetworkPlaying() )
		return;

	ioINILoader kLoader( GetModeMapINIFileName( m_ModeType, m_iModeSubNum, m_iModeMapIndex ) );
	kLoader.SetTitle_e( "interaction_object_list" );

	int iWearItemCnt = kLoader.LoadInt_e( "wear_item_cnt", 0 );
	ioHashStringVec vItemNameList;
	vItemNameList.reserve( iWearItemCnt );

	char szBuf[MAX_PATH], szTitle[MAX_PATH];
	for(int i=0; i<iWearItemCnt; i++ )
	{
		memset(szTitle, 0, MAX_PATH);
		memset(szBuf, 0, MAX_PATH);

		wsprintf_e( szTitle, "wear_item%d_name", i+1 );
		kLoader.LoadString( szTitle, "", szBuf, MAX_PATH );

		vItemNameList.push_back( ioHashString(szBuf) );
	}

	int iItemPosCnt = kLoader.LoadInt_e( "wear_generate", "pos_cnt", 0 );

	Vector3Vec vWearPosList;
	vWearPosList.reserve( iItemPosCnt );
	for( i=0 ; i<iItemPosCnt ; i++ )
	{
		D3DXVECTOR3 vPos;
		memset(szBuf, 0, MAX_PATH);
		wsprintf_e( szBuf, "pos%d_x", i+1 );
		vPos.x = kLoader.LoadFloat( "wear_generate", szBuf, 0.0f );

		vPos.y = 0.0f;

		memset(szBuf, 0, MAX_PATH);
		wsprintf_e( szBuf, "pos%d_z", i+1 );
		vPos.z = kLoader.LoadFloat( "wear_generate", szBuf, 0.0f );
		vWearPosList.push_back( vPos );
	}

	std::random_shuffle( vWearPosList.begin(), vWearPosList.end() );

	for( i=0 ; i<iWearItemCnt ; i++ )
	{
		ioItem *pItem = m_pCreator->CreateItem( vItemNameList[i] );
		if( !pItem )	continue;

		pItem->ReLoadProperty();

		ioFieldItem *pField = m_pCreator->CreateFieldItem( pItem );
		if( pField )
		{
			pField->SetWorldPositionByServerSide( vWearPosList[i] );
			pField->CreateItemEffect();
		}
	}
}

void ioHiddenkingMode::UpdateRoundWinRecord()
{
	if( P2PNetwork::IsNetworkPlaying() )	return;

	UpdateUserRank();

	if( IsRedWin( m_WinTeamType ) )
		m_iRedRoundWinCnt++;
	else if( IsBlueWin( m_WinTeamType ) )
		m_iBlueRoundWinCnt++;
}

int ioHiddenkingMode::GetRoundWinCnt( TeamType eTeamType )
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

void ioHiddenkingMode::GetMostNearPosList( PositionDistanceSqList &rkResultList, const D3DXVECTOR3 &vTargPos )
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

ioGameEntity* ioHiddenkingMode::GetGuideTarget( const D3DXVECTOR3& vArrowPos )
{
	ioBaseChar *pKing = GetKing();
	if( pKing )
	{
		if( pKing == GetOwnerChar() )
			return NULL;
		
		return pKing;
	}

	// Crown
	ioFieldItem *pCrown = m_pCreator->GetCrownFieldItem( ioItem::MCT_HIDDEN_KING );
	if( pCrown )
		return pCrown;

	return NULL;
}

void ioHiddenkingMode::CheckEndSceneLookAt()
{
	ioBaseChar *pKing = GetKing();
	if( pKing )
	{
		m_vEndLookAt = pKing->GetWorldPosition();
		m_szEndLookCharName = pKing->GetCharName();
		pKing->SetEndFocusEffect( GetRoundEndEffect() );
		return;
	}

	ioFieldItem *pCrown = m_pCreator->GetCrownFieldItem( ioItem::MCT_HIDDEN_KING );
	if( pCrown )
	{
		m_vEndLookAt = pCrown->GetWorldPosition();
		m_szEndLookCharName.Clear();
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
		m_szEndLookCharName.Clear();
	}
}

void ioHiddenkingMode::ClearModeData()
{
	ClearKing();

	g_SoundMgr.AllSoundStop();
	g_SoundMgr.StopStream();
	m_pCreator->HidingAllFieldItem();
}

int ioHiddenkingMode::GetCurRank( const ioHashString& szCharName )
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

int ioHiddenkingMode::GetContributePerChar( const ioHashString& szName )
{
	int iCharCnt = m_vCharRecordList.size();

	for(int i = 0;i < iCharCnt;i++)
	{
		HiddenkingRecord *pRecord = &m_vCharRecordList[i];
		if( !pRecord )	continue;
		if( !pRecord->pChar ) continue;

		if( pRecord->pChar->GetCharName() == szName )
		{
			return pRecord->Contribute.m_iPrevContributePer;
		}
	}

	return 0;
}

void ioHiddenkingMode::SendKingPingMessage()
{
	DWORD dwCurTime = REALGETTIME();
	if( m_dwSendCheckKingTime != 0 )
	{
		DWORD dwSendTimeGap = dwCurTime - m_dwSendCheckKingTime;
		if( dwSendTimeGap < m_dwKingPingTime )
			return;
	}

	SP2Packet kPacket( CUPK_CHECK_KING_PING );
	kPacket << g_MyInfo.GetPublicID();
	kPacket << g_GlobalTimer.GetGlobalTime();
	kPacket << m_iCurSendIndex;
	P2PNetwork::SendToServer( kPacket );

	m_dwSendCheckKingTime = dwCurTime;
	m_iCurSendIndex++;
}

