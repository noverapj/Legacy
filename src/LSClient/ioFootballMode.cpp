

#include "stdafx.h"

#include "ioFootballMode.h"
#include "ioLadderTeamMgr.h"
#include "ioRankBattleManager.h"
#include "ioShuffleRoomMgr.h"

#include "GUI/CenterInfoWnd.h"
#include "GUI/MiniMap.h"
#include "GUI/MiniSoldierSelectWnd.h"
#include "GUI/TimeScoreWnd.h"
#include "GUI/ioObserverWnd.h"
#include "GUI/TotalResultMainWnd.h"
#include "GUI/RoundStateMainWnd.h"

ioFootballMode::ioFootballMode( ioPlayStage *pCreator )
: ioPlayMode( pCreator )
{
	m_ModeType = MT_FOOTBALL;

	m_vCharRecordList.reserve( 100 );
}

ioFootballMode::~ioFootballMode()
{
	m_bZeroHP = false;
	ClearMode();
}

void ioFootballMode::InitMode( int iMapIndex )
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

	kLoader.SetTitle_e( "common_info" );
	m_dwRoundDuration = kLoader.LoadInt_e( "round_time", 300000 );
	m_dwCurRoundDuration = m_dwRoundDuration;

	m_fScoreWarningRate = kLoader.LoadFloat_e( "score_warning_rate", FLOAT05 );

	m_bNoDamage = kLoader.LoadBool_e( "no_damage_mode", false );
	m_bDisableAttack = kLoader.LoadBool_e( "disable_attack_mode", false );

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
}


void ioFootballMode::LoadMap()
{
	ioPlayMode::LoadMap();

	ioINILoader kLoader( GetModeMapINIFileName( m_ModeType, m_iModeSubNum, m_iModeMapIndex ) );

	kLoader.SetTitle_e( "goal_gate_red" );
	D3DXVECTOR3 vMin, vMax;
	vMin.x = kLoader.LoadFloat_e( "gate_min_x", 0.0f );
	vMin.y = kLoader.LoadFloat_e( "gate_min_y", 0.0f );
	vMin.z = kLoader.LoadFloat_e( "gate_min_z", 0.0f );

	vMax.x = kLoader.LoadFloat_e( "gate_max_x", 0.0f );
	vMax.y = kLoader.LoadFloat_e( "gate_max_y", 0.0f );
	vMax.z = kLoader.LoadFloat_e( "gate_max_z", 0.0f );

	m_GoalGateRed.m_Team = TEAM_RED;
	m_GoalGateRed.m_GateBox.SetBoxByMinMax( vMin, vMax );
	m_GoalGateRed.m_vGateDir = D3DXVECTOR3( -FLOAT1, 0.0f, 0.0f );

	kLoader.SetTitle_e( "goal_gate_blue" );
	vMin.x = kLoader.LoadFloat_e( "gate_min_x", 0.0f );
	vMin.y = kLoader.LoadFloat_e( "gate_min_y", 0.0f );
	vMin.z = kLoader.LoadFloat_e( "gate_min_z", 0.0f );

	vMax.x = kLoader.LoadFloat_e( "gate_max_x", 0.0f );
	vMax.y = kLoader.LoadFloat_e( "gate_max_y", 0.0f );
	vMax.z = kLoader.LoadFloat_e( "gate_max_z", 0.0f );

	m_GoalGateBlue.m_Team = TEAM_BLUE;
	m_GoalGateBlue.m_GateBox.SetBoxByMinMax( vMin, vMax );
	m_GoalGateBlue.m_vGateDir = D3DXVECTOR3( FLOAT1, 0.0f, 0.0f );
}

void ioFootballMode::LoadModeResource( ioINILoader &rkLoader )
{
	TimeScoreWnd *pTimeScoreWnd = dynamic_cast<TimeScoreWnd *>( g_GUIMgr.AddWnd( "XML/TimeScoreWnd.xml") );
	if(pTimeScoreWnd)
		pTimeScoreWnd->SetModeType( GetModeType() );

	CenterInfoWnd *pCenterInfoWnd = dynamic_cast<CenterInfoWnd *>( g_GUIMgr.AddWnd("XML/CenterInfoWnd.xml") );
	if(pCenterInfoWnd)
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

void ioFootballMode::ProcessMode()
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
		break;
	case MS_RESULT:
		ProcessResultState();
		break;
	}
}

void ioFootballMode::ProcessReadyState()
{
	if( P2PNetwork::IsNetworkPlaying() )	return;
	if( m_dwReadyStartTime + 4000 > FRAMEGETTIME() )
		return;

	SetModeState( MS_PLAY );
}

void ioFootballMode::ProcessPlayState()
{
	UpdateSoldierRevivalPenalty();
	//UpdateTimeGrowthTime();

	m_pCreator->NotifySpringDampUseSet( true );

	ProcessRevival();
	
	CheckBallSurround();
	
	if( !P2PNetwork::IsNetworkPlaying() )
	{		
		CheckRoundEnd();
	}
}

void ioFootballMode::CheckRoundEnd()
{
	if( m_dwPlayStartTime + m_dwRoundDuration > FRAMEGETTIME() )
		return;

	bool bRoundSetEnd = false;
	if( m_iCurRoundCount == m_iMaxRoundCount )
		bRoundSetEnd = true;

	SetRoundEndInfo( WTT_DRAW, bRoundSetEnd );
}

void ioFootballMode::ProcessResultState()
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

void ioFootballMode::ProcessGUI()
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

void ioFootballMode::UpdateMiniMapSpotPoint()
{
	MiniMap *pMiniMap = dynamic_cast<MiniMap*>(g_GUIMgr.FindWnd( MINIMAP_WND ));
	if( !pMiniMap )	return;

	ioPlayMode::UpdateMiniMapSpotPoint();

	pMiniMap->ClearPointList();

	D3DXVECTOR3 vPos;
	DWORD dwGapTime = 0;

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

	// Ball &Dummy
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

void ioFootballMode::UpdateTimeScoreWnd()
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

void ioFootballMode::UpdateRoundStateWnd()
{
	if( g_MyInfo.IsSortContribute() )
	{
		DWORD dwTimeGap = FRAMEGETTIME() - m_dwPlayStartTime;
		if( m_dwCurRoundDuration > dwTimeGap )
			ContributeRoundAllPercent( m_dwRoundDuration - (m_dwCurRoundDuration - dwTimeGap), m_dwRoundDuration );
		else
			ContributeRoundAllPercent( m_dwRoundDuration, m_dwRoundDuration );

		FootballRecord *pRecord = FindRecordCharName( g_MyInfo.GetPublicID() );
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

void ioFootballMode::ProcessRevival()
{
	DWORD dwCurTime = FRAMEGETTIME();
	
	int iCharCnt = m_vCharRecordList.size();
	for( int i=0 ; i<iCharCnt ; i++ )
	{
		FootballRecord &rkRecord = m_vCharRecordList[i];
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

void ioFootballMode::RenderPreGUI()
{
	if( m_ModeState == MS_PLAY )
	{
		RenderPushEnableCursor();
	}
}

void ioFootballMode::RenderAfterGUI()
{
}

void ioFootballMode::ClearMode()
{
	ioPlayMode::ClearMode();
}

D3DXVECTOR3 ioFootballMode::GetRevivalPos( ioBaseChar *pChar )
{
	if( !pChar )
	{
		LOG.PrintTimeAndLog( 0, "ioFootballMode::GetRevivalPos() - Char is NULL" );
		return D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	}

	return GetStartPos( pChar->GetTeam() );
}

void ioFootballMode::NotifyEntityDie( ioPlayEntity *pEntity, ioPlayEntity *pKiller, ioBaseChar *pBestAttacker,
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
	}
}

void ioFootballMode::NotifyCharDropDie( ioBaseChar *pDieChar, ioBaseChar *pKillChar, ioBaseChar *pBestAttacker,
									    float fLastRate, float fBestRate )
{
	OnNotifyCharDie( pDieChar, pKillChar, pBestAttacker, fLastRate, fBestRate );
	UpdateDropDieRecord( pDieChar, pKillChar, pBestAttacker );
}

void ioFootballMode::OnNotifyCharDie( ioBaseChar *pDieChar, ioBaseChar *pKillChar, ioBaseChar *pBestAttacker,
									 float fLastRate, float fBestRate )
{
	if( !pDieChar )
	{
		LOG.PrintTimeAndLog( 0, "ioFootballMode::OnNotifyCharDie() - DieChar is NULL" );
		return;
	}

	FootballRecord *pDieRecord = FindRecordCharName( pDieChar->GetCharName() );
	if( !pDieRecord )
	{
		LOG.PrintTimeAndLog( 0, "ioFootballMode::OnNotifyCharDie - %s Not Exit Char",
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

void ioFootballMode::OnZeroHP( SP2Packet &rkPacket )
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

	const GameEntityList &rkObjectList = m_pCreator->GetGameEntityList();
	GameEntityList::const_iterator iter_b;
	for( iter_b = rkObjectList.begin() ; iter_b!=rkObjectList.end() ; ++iter_b )
	{
		ioBall *pBall = ToBallStruct( *iter_b );
		if( !pBall ) continue;

		pBall->SetZeroHP();
	}
	g_ModeUIController.SetDeathTime();
}

void ioFootballMode::OnRoundReady( SP2Packet &rkPacket )
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

void ioFootballMode::OnRoundJoin( SP2Packet &rkPacket )
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
			LOG.PrintTimeAndLog( 0, "ioFootballMode::OnRoundJoin MS_READY" );
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

void ioFootballMode::OnRoundJoinView( SP2Packet &rkPacket )
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

void ioFootballMode::OnRoundJoinObserver( SP2Packet &rkPacket )
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

void ioFootballMode::AddNewRecord( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	FootballRecord *pFootballRecord = FindRecordCharName( pChar->GetCharName() );
	if( !pFootballRecord )
	{
		FootballRecord kRecord;
		kRecord.pChar = pChar;
		kRecord.szName= pChar->GetCharName();
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
		pFootballRecord->pChar = pChar;
	}
}

void ioFootballMode::RemoveRecord( const ioHashString &rkName )
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


void ioFootballMode::RemoveRecord( int nIndex )
{
	int iCharCnt = m_vCharRecordList.size();
	if( iCharCnt > nIndex )
	{
		m_vCharRecordList.erase( m_vCharRecordList.begin() + nIndex );
		UpdateUserRank();
	}
}


ioBaseChar* ioFootballMode::GetRecordChar( int i )
{
	if( COMPARE( i, 0, GetRecordCharCnt() ) )
		return m_vCharRecordList[i].pChar;

	return NULL;
}

ioBaseChar* ioFootballMode::GetRecordChar( const ioHashString &rkName )
{
	FootballRecord *pRecord = FindRecordCharName( rkName );
	if( pRecord )
		return pRecord->pChar;

	return NULL;
}

int ioFootballMode::GetRecordCharCnt() const
{
	return m_vCharRecordList.size();
}

ModeRecord* ioFootballMode::FindModeRecord( int iIdx )
{
	if( COMPARE( iIdx, 0, GetRecordCharCnt() ) )
		return &m_vCharRecordList[iIdx];

	return NULL;
}

ModeRecord* ioFootballMode::FindModeRecordCharName( const ioHashString &rkName )
{
	return (ModeRecord*)FindRecordCharName( rkName );
}

FootballRecord* ioFootballMode::FindRecordCharName( const ioHashString &rkName )
{
	int iCharCnt = m_vCharRecordList.size();
	for( int i=0 ; i<iCharCnt ; i++ )
	{
		if( m_vCharRecordList[i].szName == rkName )
			return &m_vCharRecordList[i];
	}

	return NULL;
}

bool ioFootballMode::GetRoundInfo( int iIndex, RoundInfo &rkInfo )
{
	if( ioPlayMode::GetRoundInfo( iIndex, rkInfo ) )
	{
		FootballRecord *pRecord = (FootballRecord*)FindModeRecord( iIndex );
		if( !pRecord )	return false;
		if( !pRecord->pChar ) return false;

		rkInfo.m_iContribute = pRecord->Contribute.m_iContribute;
		rkInfo.m_iContributePer = pRecord->Contribute.m_iPrevContributePer;
		rkInfo.m_iGoal = pRecord->m_iGoal;
		rkInfo.m_iGoalAssist = pRecord->m_iGoalAssist;
		return true;
	}
	return false;
}

bool ioFootballMode::GetResultInfo( int iIndex, ResultInfo &rkInfo )
{
	if( ioPlayMode::GetResultInfo( iIndex, rkInfo ) )
	{
		FootballRecord *pRecord = (FootballRecord*)FindModeRecord( iIndex );
		if( !pRecord )	return false;
		if( !pRecord->pChar ) return false;

		rkInfo.m_iContribute = pRecord->Contribute.m_iContribute;
		rkInfo.m_iContributePer = pRecord->Contribute.m_iPrevContributePer;
		rkInfo.m_iGoal = pRecord->m_iGoal;
		return true;
	}
	return false;
}

bool ioFootballMode::GetFinalInfo( int iIndex, FinalInfo &rkInfo )
{
	if( ioPlayMode::GetFinalInfo( iIndex, rkInfo ) )
	{
		FootballRecord *pRecord = (FootballRecord*)FindModeRecord( iIndex );
		if( !pRecord )	return false;
		if( !pRecord->pChar ) return false;

		rkInfo.m_iContribute = pRecord->Contribute.m_iContribute;
		rkInfo.m_iContributePer = pRecord->Contribute.m_iPrevContributePer;
		rkInfo.m_iGoal = pRecord->m_iGoal;
		return true;
	}
	return false;
}

void ioFootballMode::ReStartMode()
{
	int i=0;

	int iCharCnt = m_vCharRecordList.size();
	for( i=0 ; i<iCharCnt ; i++ )
	{
		FootballRecord &rkRecord = m_vCharRecordList[i];
		if( !rkRecord.pChar ) continue;

		D3DXVECTOR3 vStartPos = GetStartPos( rkRecord.pChar->GetTeam() );
		rkRecord.pChar->SetWorldPosition( vStartPos );

		rkRecord.dwRevivalGap = 0;
		rkRecord.dwCurDieTime = 0;
		rkRecord.iRevivalCnt = 0;

		if( m_iCurRoundCount == 1 )
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

	g_BallStructMgr.DestroyBallStructList();

	m_dwCurRevivalTime = 0;
	m_dwCheckWarningTime = 0;
	m_bScoreWarning = false;
	m_bFirstSelect = true;

	m_GoalPlayer.Clear();

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

void ioFootballMode::ContributeLOG()
{
	char szLog[MAX_PATH];
	int iCharCnt = m_vCharRecordList.size();

	for( int i = 0;i < iCharCnt;i++)
	{
		FootballRecord *pRecord = &m_vCharRecordList[i];
		if( pRecord->pChar )
		{
			StringCbPrintf( szLog, sizeof( szLog), "%s[%d] : Goal%d(%d) Touch%d(%d) Time%d(%d) Assist%d(%d) Defense%d(%d)", //Except Extracting Hangeul
							pRecord->szName.c_str(), (int)pRecord->pChar->GetTeam(),
							pRecord->Contribute.m_iGoalRank+ 1, pRecord->Contribute.m_iGoal,
							pRecord->Contribute.m_iBallTouchRank + 1, pRecord->Contribute.m_iBallTouch,
							pRecord->Contribute.m_iBallPointRank + 1, pRecord->Contribute.m_iBallPoint,
							pRecord->Contribute.m_iAssistRank + 1, pRecord->Contribute.m_iAssist,
							pRecord->Contribute.m_iSaveTouchRank + 1, pRecord->Contribute.m_iSaveTouch );

			g_ChatMgr.SetSystemMsg( szLog );
		}
	}
}

void ioFootballMode::ContributeConvertTeam( TeamType eTeam )
{
	if( m_vContributeTable.size() < MAX_PLAYER )
		return;

	int i = 0;
	int iCharCnt = m_vCharRecordList.size();
	int iInfoCnt = 0;
	ContributeInfoList vInfo;


	// BallPoint
	for(i = 0;i < iCharCnt;i++)
	{
		FootballRecord *pRecord = &m_vCharRecordList[i];
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
		Contribute.m_iContributePoint = pRecord->Contribute.m_iBallPoint;
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
			FootballRecord *pRecord = &m_vCharRecordList[iArray];
			if( !pRecord->Contribute.m_bLastResultSync )
				pRecord->Contribute.m_iContribute = (float)m_vContributeTable[vInfo[i].m_iRank] * 1.5f;

			pRecord->Contribute.m_iBallPointRank = vInfo[i].m_iRank;
		}
	}
	vInfo.clear();


	// BallTouch
	for(i = 0;i < iCharCnt;i++)
	{
		FootballRecord *pRecord = &m_vCharRecordList[i];
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
		Contribute.m_iContributePoint = pRecord->Contribute.m_iBallTouch;
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
			FootballRecord *pRecord = &m_vCharRecordList[iArray];
			if( !pRecord->Contribute.m_bLastResultSync )
				pRecord->Contribute.m_iContribute += (float)m_vContributeTable[vInfo[i].m_iRank] * 1.5f;

			pRecord->Contribute.m_iBallTouchRank = vInfo[i].m_iRank;
		}
	}
	vInfo.clear();


	// SaveTouch
	for(i = 0;i < iCharCnt;i++)
	{
		FootballRecord *pRecord = &m_vCharRecordList[i];
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
		Contribute.m_iContributePoint = pRecord->Contribute.m_iSaveTouch;
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
			FootballRecord *pRecord = &m_vCharRecordList[iArray];
			if( !pRecord->Contribute.m_bLastResultSync )
				pRecord->Contribute.m_iContribute += (float)m_vContributeTable[vInfo[i].m_iRank] * 2.5f;

			pRecord->Contribute.m_iSaveTouchRank = vInfo[i].m_iRank;
		}
	}
	vInfo.clear();


	// Goal
	for(i = 0;i < iCharCnt;i++)
	{
		FootballRecord *pRecord = &m_vCharRecordList[i];
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
		Contribute.m_iContributePoint = pRecord->Contribute.m_iGoal;
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
			FootballRecord *pRecord = &m_vCharRecordList[iArray];
			if( !pRecord->Contribute.m_bLastResultSync )
				pRecord->Contribute.m_iContribute += (float)m_vContributeTable[vInfo[i].m_iRank] * 2.5f;

			pRecord->Contribute.m_iGoalRank = vInfo[i].m_iRank;
		}
	}
	vInfo.clear();

	// Assist
	for(i = 0;i < iCharCnt;i++)
	{
		FootballRecord *pRecord = &m_vCharRecordList[i];
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
		Contribute.m_iContributePoint = pRecord->Contribute.m_iAssist;
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
			FootballRecord *pRecord = &m_vCharRecordList[iArray];
			if( !pRecord->Contribute.m_bLastResultSync )
				pRecord->Contribute.m_iContribute += m_vContributeTable[vInfo[i].m_iRank] * 2;

			pRecord->Contribute.m_iAssistRank = vInfo[i].m_iRank;
		}
	}
	vInfo.clear();
}

void ioFootballMode::ContributeConvert()
{
	ContributeConvertTeam( TEAM_NONE );        //TEAM_NONE는 전체 유저에 대한 기여도 평가
}

int ioFootballMode::ContributeMyRank( int &rRankValue1, int &rRankValue2, int &rRankValue3, int &rRankValue4 )
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

	// Goal
	for(i = 0;i < iCharCnt;i++)
	{
		FootballRecord *pRecord = &m_vCharRecordList[i];
		if( !pRecord ) continue;
		if( !pRecord->pChar ) continue;
		if( pRecord->pChar->GetState() == CS_OBSERVER ) continue;
		if( g_BattleRoomMgr.IsObserverUser(pRecord->pChar->GetCharName()) ) continue;

		ContributeInfo Contribute;
		Contribute.m_iRecordArray     = i;
		Contribute.m_iContributePoint = pRecord->Contribute.m_iGoal;
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

	// BallTouch
	for(i = 0;i < iCharCnt;i++)
	{
		FootballRecord *pRecord = &m_vCharRecordList[i];
		if( !pRecord ) continue;
		if( !pRecord->pChar ) continue;
		if( pRecord->pChar->GetState() == CS_OBSERVER ) continue;
		if( g_BattleRoomMgr.IsObserverUser(pRecord->pChar->GetCharName()) ) continue;

		ContributeInfo Contribute;
		Contribute.m_iRecordArray = i;
		Contribute.m_iContributePoint = pRecord->Contribute.m_iBallTouch;
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

	// BallPoint
	for(i = 0;i < iCharCnt;i++)
	{
		FootballRecord *pRecord = &m_vCharRecordList[i];
		if( !pRecord ) continue;
		if( !pRecord->pChar ) continue;
		if( pRecord->pChar->GetState() == CS_OBSERVER ) continue;
		if( g_BattleRoomMgr.IsObserverUser(pRecord->pChar->GetCharName()) ) continue;

		ContributeInfo Contribute;
		Contribute.m_iRecordArray = i;
		Contribute.m_iContributePoint = pRecord->Contribute.m_iBallPoint;
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

void ioFootballMode::ContributeRoundPercent( DWORD dwCurTime, DWORD dwTotalTime )
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
		FootballRecord *pRecord = &m_vCharRecordList[i];
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
		FootballRecord *pRecord = &m_vCharRecordList[i];

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
		FootballRecord *pRecord = &m_vCharRecordList[i];

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

void ioFootballMode::ContributeRoundAllPercent( DWORD dwCurTime, DWORD dwTotalTime )
{
	//기여도의 100분율
	int iMaxContribute	= 0;
	int iCharCnt		= m_vCharRecordList.size();
	int iPlayCharCnt	= 0;

	int i = 0;
	for(i = 0;i < iCharCnt;i++)
	{
		FootballRecord *pRecord = &m_vCharRecordList[i];
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
			FootballRecord *pRecord = &m_vCharRecordList[i];
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

void ioFootballMode::DamageContribute( const ioHashString szAttacker, const ioHashString szTarget, float fDamage, DWORD dwItemCode, int iWeaponType )
{
	return;
}

void ioFootballMode::KillContribute( const ioHashString szKiller, const ioHashString szKilled, DWORD dwItemCode )
{
	return;
}

void ioFootballMode::AttackCntContribute( const ioHashString szAttacker, const ioHashString szTarget, DWORD dwItemCode, int iAttackValue/* = 100 */ )
{
	return;
}

void ioFootballMode::FillLastPlayRecordInfo( SP2Packet &rkPacket )
{
	int iCharCnt = m_vCharRecordList.size();
	rkPacket << iCharCnt;
	for(int i = 0;i < iCharCnt;i++)
	{
		FootballRecord *pRecord = &m_vCharRecordList[i];
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

void ioFootballMode::FillPlayRecordInfo( SP2Packet &rkPacket )
{
	int iCharCnt = m_vCharRecordList.size();
	rkPacket << iCharCnt;
	for(int i = 0;i < iCharCnt;i++)
	{
		FootballRecord *pRecord = &m_vCharRecordList[i];
		rkPacket << pRecord->pChar->GetCharName();
		rkPacket << pRecord->Contribute.m_iDamage;
		rkPacket << pRecord->Contribute.m_iAttackCnt;
		rkPacket << pRecord->Contribute.m_iKill;
		rkPacket << pRecord->Contribute.m_iGoal;
		rkPacket << pRecord->Contribute.m_iBallTouch;
		rkPacket << pRecord->Contribute.m_iBallPoint;
		rkPacket << pRecord->Contribute.m_iPrevContributePer;
		rkPacket << pRecord->m_iGoal;
		rkPacket << pRecord->m_iGoalAssist;
		rkPacket << g_VictoriesMgr.GetVictories( pRecord->pChar->GetCharName() );
	}
}

void ioFootballMode::ApplyPlayRecordInfo( SP2Packet &rkPacket )
{
	int iCharCnt;
	rkPacket >> iCharCnt;
	for(int i = 0;i < iCharCnt;i++)
	{
		ioHashString szName;
		int iDamage, iAttackCnt, iKill, iGoal, iTouch, iPoint, iPrevContributePer, iUniqueGoal, iUniqueGoalAssist, iVictories;
		rkPacket >> szName >> iDamage >> iAttackCnt >> iKill >> iGoal >> iTouch >> iPoint
				 >> iPrevContributePer >> iUniqueGoal >> iUniqueGoalAssist >> iVictories;

		FootballRecord *pRecord = FindRecordCharName( szName );
		if( pRecord )
		{
			pRecord->Contribute.m_iDamage			= iDamage;
			pRecord->Contribute.m_iAttackCnt		= iAttackCnt;
			pRecord->Contribute.m_iKill				= iKill;
			pRecord->Contribute.m_iGoal				= iGoal;
			pRecord->Contribute.m_iBallTouch		= iTouch;
			pRecord->Contribute.m_iBallPoint		= iPoint;
			pRecord->Contribute.m_iPrevContributePer= iPrevContributePer;
			pRecord->m_iGoal = iUniqueGoal;
			pRecord->m_iGoalAssist = iUniqueGoalAssist;

			if( !g_LadderTeamMgr.IsLadderTeam() )
				g_VictoriesMgr.SetVictories( szName, iVictories );
		}
	}
	
	ContributeConvert();
	ContributeRoundAllPercent();
	UpdateRoundInfoList();
	CheckTotalChamp();
}

void ioFootballMode::ContributeResultSync( const ioHashString &szName, int iContribute )
{
	FootballRecord *pRecord = FindRecordCharName( szName );
	if( !pRecord ) return;

	pRecord->Contribute.m_bLastResultSync = true;
	pRecord->Contribute.m_iContribute     = iContribute;
}

bool ioFootballMode::ProcessTCPPacket( SP2Packet &rkPacket )
{
	if( ioPlayMode::ProcessTCPPacket( rkPacket ) )
		return true;

	switch( rkPacket.GetPacketID() )
	{
	case STPK_FOOTBALL_GOAL:
		OnGoal( rkPacket );
		return true;
	}

	return false;
}

bool ioFootballMode::ProcessUDPPacket( SP2Packet &rkPacket )
{
	if( ioPlayMode::ProcessUDPPacket( rkPacket ) )
		return true;

	return false;
}

void ioFootballMode::OnGoal( SP2Packet &rkPacket )
{
	int iState;
	rkPacket >> iState;

	if( iState == FOOTBALL_GOAL_OK )
	{
		int iTeam;
		rkPacket >> iTeam;

		rkPacket >> m_GoalPlayer;

		ioHashString szAssistPlayer;
		rkPacket >> szAssistPlayer;

		ioBaseChar *pKiller = m_pCreator->GetBaseChar( m_GoalPlayer );
		if( !pKiller )	return;

		ioBall *pBall = pKiller->FindBall();
		if( pBall )
		{
			pBall->SetGoalState( true );
		}

		FootballRecord *pRecord = FindRecordCharName( m_GoalPlayer );
		if( pRecord )
		{
			if( iTeam == pKiller->GetTeam() )
				pRecord->m_iGoal += 1;
		}

		GoalContribute( (TeamType)iTeam, m_GoalPlayer );

		if( iTeam == pKiller->GetTeam() )
		{
			g_AwardMgr.AddGoal( m_GoalPlayer, FLOAT100 * ContributeTeamPerA( (TeamType)iTeam, FLOAT1, 0.1f ) );
			if( pKiller->IsOwnerChar() )
				g_QuestMgr.QuestCompleteTerm( QC_SOCCER_GOAL, FLOAT100 * ContributeTeamPerA( (TeamType)iTeam, FLOAT1, 0.1f ) );
		}
		else
		{
			g_AwardMgr.AddOwnerGoal( m_GoalPlayer, FLOAT100 * ContributeTeamPerA( (TeamType)iTeam, FLOAT1, 0.1f ) );
		}

		TeamType eAssistTeam = TEAM_NONE;
		FootballRecord *pAssistRecord = FindRecordCharName( szAssistPlayer );
		if( pAssistRecord && pAssistRecord->pChar )
		{
			if( m_GoalPlayer != szAssistPlayer )
			{
				eAssistTeam = pAssistRecord->pChar->GetTeam();

				if( pKiller->GetTeam() == eAssistTeam )
				{
					AssistContribute( (TeamType)iTeam, szAssistPlayer );

					if( iTeam == eAssistTeam )
					{
						g_AwardMgr.AddAssist( szAssistPlayer, FLOAT100 * ContributeTeamPerA( (TeamType)iTeam, FLOAT1, 0.1f ) );
						if( pAssistRecord->pChar->IsOwnerChar() )
							g_QuestMgr.QuestCompleteTerm( QC_SOCCER_ASSIST, FLOAT100 * ContributeTeamPerA( (TeamType)iTeam, FLOAT1, 0.1f ) );
						pAssistRecord->m_iGoalAssist += 1;
					}
				}
			}
		}

		g_ChatMgr.SetGoalMsg( (TeamType)iTeam, pKiller->GetTeam(), m_GoalPlayer, eAssistTeam, szAssistPlayer );
	}
	else if( iState == FOOTBALL_GOAL_FAIL )
	{
		ioHashString szCharName;
		rkPacket >> szCharName;

		FootballRecord *pRecord = FindRecordCharName( szCharName );
		if( pRecord && pRecord->pChar )
		{
			ioBall *pBall = pRecord->pChar->FindBall();
			if( pBall )
				pBall->ClearSendData();
		}
	}
}

void ioFootballMode::ApplyModeInfo( SP2Packet &rkPacket )
{
	rkPacket >> m_iCurRoundCount;
	rkPacket >> m_iMaxRoundCount;

	rkPacket >> m_dwRoundDuration;

	rkPacket >> m_iBlueRoundWinCnt;
	rkPacket >> m_iRedRoundWinCnt;

	rkPacket >> m_iBluePosArray;
	rkPacket >> m_iRedPosArray;
	//

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

void ioFootballMode::ApplyCharModeInfo( const ioHashString &rkName,
									  SP2Packet &rkPacket,
									  bool bDieCheck )
{
	bool bRecord;
	rkPacket >> bRecord;

	if( !bRecord ) return;

	FootballRecord *pRecord = FindRecordCharName( rkName );
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

void ioFootballMode::GetModeSpecialInfo( SP2Packet &rkPacket )
{
	ioPlayMode::GetModeSpecialInfo( rkPacket );
}

void ioFootballMode::OnModeSpecialInfo( SP2Packet &rkPacket )
{
	ioPlayMode::OnModeSpecialInfo( rkPacket );
}

int ioFootballMode::GetRoundWinCnt( TeamType eTeamType )
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

void ioFootballMode::SetResultScoreWnd()
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

void ioFootballMode::SetTotalScoreWnd( IntVec &rvClassTypeList,
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
//		pTotal->SetRoundHistory( i, m_vRoundHistory[i] );

	pTotalResult->SetTeamScore( GetOwnerTeam(), m_iBlueRoundWinCnt, m_iRedRoundWinCnt );
	SetTotalResultBriefingWnd();
}

ioGameEntity* ioFootballMode::GetGuideTarget( const D3DXVECTOR3& vArrowPos )
{
	ioBaseChar *pOwner = GetOwnerChar();
	if( !pOwner ) 	return NULL;

	// 축구공
	ioBall *pBall = NULL;
	float fBallDistSq = std::numeric_limits<float>::infinity();

	const GameEntityList &rkObjectList = m_pCreator->GetGameEntityList();
	GameEntityList::const_iterator iter_b;
	for( iter_b = rkObjectList.begin() ; iter_b!=rkObjectList.end() ; ++iter_b )
	{
		pBall = ToBallStruct( *iter_b );

		if( pBall )
			return pBall;
	}

	return NULL;
}

void ioFootballMode::CheckEndSceneLookAt()
{
	m_szEndLookCharName.Clear();

	// 골 넣은 캐릭터를 우선 대상으로 하고 없으면 볼을 보여준다
	ioBaseChar *pOwner = m_pCreator->GetOwnerChar();
	if( !m_GoalPlayer.IsEmpty() )
	{
		FootballRecord *pKillRecord = FindRecordCharName( m_GoalPlayer );
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
		const GameEntityList &rkObjectList = m_pCreator->GetGameEntityList();
		GameEntityList::const_iterator iter_b;
		for( iter_b = rkObjectList.begin() ; iter_b!=rkObjectList.end() ; ++iter_b )
		{
			ioBall *pBall = ToBallStruct( *iter_b );
			if( !pBall ) continue;

			m_vEndLookAt = pBall->GetWorldPosition();
			pBall->SetEndFocusEffect( GetRoundEndEffect() );
			return;
		}
	}
}

void ioFootballMode::ClearModeData()
{
	g_SoundMgr.AllSoundStop();
	g_SoundMgr.StopStream();
	m_pCreator->HidingAllFieldItem();
}

int ioFootballMode::GetCurRank( const ioHashString& szCharName )
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

int ioFootballMode::GetContributePerChar( const ioHashString& szName )
{
	int iCharCnt = m_vCharRecordList.size();

	for(int i = 0;i < iCharCnt;i++)
	{
		FootballRecord *pRecord = &m_vCharRecordList[i];
		if( !pRecord )	continue;
		if( !pRecord->pChar ) continue;

		if( pRecord->pChar->GetCharName() == szName )
		{
			return pRecord->Contribute.m_iPrevContributePer;
		}
	}

	return 0;
}

void ioFootballMode::GetGoalGate( IN TeamType eTeam, OUT GoalGate &rkGate )
{
	switch( eTeam )
	{
	case TEAM_RED:
		rkGate.m_Team = TEAM_RED;
		rkGate.m_GateBox = m_GoalGateRed.m_GateBox;
		rkGate.m_vGateDir = m_GoalGateRed.m_vGateDir;
		break;
	case TEAM_BLUE:
		rkGate.m_Team = TEAM_BLUE;
		rkGate.m_GateBox = m_GoalGateBlue.m_GateBox;
		rkGate.m_vGateDir = m_GoalGateBlue.m_vGateDir;
		break;
	}
}

bool ioFootballMode::IsInBallRange( ioBaseChar *pChar, float fRange )
{
	if( !pChar ) return false;

	const GameEntityList &rkObjectList = m_pCreator->GetGameEntityList();
	GameEntityList::const_iterator iter_b;
	for( iter_b = rkObjectList.begin() ; iter_b!=rkObjectList.end() ; ++iter_b )
	{
		ioBall *pBall = ToBallStruct( *iter_b );
		if( !pBall ) continue;

		if( pChar->GetState() == CS_DIE ||
			pChar->GetState() == CS_READY ||
			pChar->GetState() == CS_LOADING ||
			pChar->GetState() == CS_OBSERVER ||
			pChar->GetState() == CS_CHANGE_WAIT ||
			pChar->GetState() == CS_VIEW ) 
			return false;

		float fRangeSq = fRange * fRange;
		D3DXVECTOR3 vDiff = pChar->GetWorldPosition() - pBall->GetWorldPosition();
		if( D3DXVec3LengthSq( &vDiff ) > fRangeSq )
			return false;

		return true;
	}

	return false;
}

void ioFootballMode::CheckBallSurround()
{
	if( FRAMEGETTIME() - m_dwBallSurroundTime > 500 )
	{
		m_dwBallSurroundTime = FRAMEGETTIME();

		//500Pixel 이내의 유저들
		int iCharCnt = m_vCharRecordList.size();
		for(int i = 0;i < iCharCnt;i++)
		{
			FootballRecord *pUserRecord = &m_vCharRecordList[i];
			if( !pUserRecord ) continue;
			if( !pUserRecord->pChar ) continue;
			if( !IsInBallRange( pUserRecord->pChar, FLOAT500 ) ) continue;

			pUserRecord->Contribute.m_iBallPoint += 500;
		}

		ContributeConvert();
		ContributeRoundAllPercent();
		UpdateRoundInfoList();
		CheckTotalChamp();
	}
}

void ioFootballMode::BallTouchContribute( const ioHashString szAttacker )
{
	if( szAttacker.IsEmpty() ) return;

	FootballRecord *pRecord = FindRecordCharName( szAttacker );
	if( !pRecord || !pRecord->pChar ) return;

	int iPoint = FLOAT100 * ContributeTeamPer( pRecord->pChar->GetTeam(), FLOAT1 );
	pRecord->Contribute.m_iBallTouch += iPoint;

	if( pRecord->pChar->IsOwnerChar() )
	{
		g_QuestMgr.QuestCompleteTerm( QC_SOCCER_BALL_HIT, iPoint );
	}

	ContributeConvert();
	ContributeRoundAllPercent();
	UpdateRoundInfoList();
	CheckTotalChamp();
}

void ioFootballMode::GoalContribute( TeamType eGoalTeam, const ioHashString szAttacker )
{
	if( szAttacker.IsEmpty() ) return;

	FootballRecord *pRecord = FindRecordCharName( szAttacker );
	if( !pRecord || !pRecord->pChar ) return;

	int iPoint = FLOAT100 * ContributeTeamPer( pRecord->pChar->GetTeam(), FLOAT1 );

	if( eGoalTeam == pRecord->pChar->GetTeam() )
	{
		pRecord->Contribute.m_iGoal += iPoint;
	}
	else
	{
		pRecord->Contribute.m_iGoal -= iPoint;
	}

	ContributeConvert();
	ContributeRoundAllPercent();
	UpdateRoundInfoList();
	CheckTotalChamp();
}

void ioFootballMode::SaveTouchContribute( const ioHashString szAttacker )
{
	if( szAttacker.IsEmpty() ) return;

	FootballRecord *pRecord = FindRecordCharName( szAttacker );
	if( !pRecord || !pRecord->pChar ) return;

	int iPoint = FLOAT100 * ContributeTeamPer( pRecord->pChar->GetTeam(), FLOAT1 );
	pRecord->Contribute.m_iSaveTouch += iPoint;

	ContributeConvert();
	ContributeRoundAllPercent();
	UpdateRoundInfoList();
	CheckTotalChamp();
}

bool ioFootballMode::FillAllDamagedList( SP2Packet &rkPacket )
{
	const GameEntityList &rkObjectList = m_pCreator->GetGameEntityList();
	GameEntityList::const_iterator iter_b;
	for( iter_b = rkObjectList.begin() ; iter_b!=rkObjectList.end() ; ++iter_b )
	{
		ioBall *pBall = ToBallStruct( *iter_b );
		if( !pBall ) continue;

		return pBall->FillAllDamagedList( rkPacket );
	}

	rkPacket << 0;
	return false;
}

void ioFootballMode::AssistContribute( TeamType eGoalTeam, const ioHashString szAttacker )
{
	if( szAttacker.IsEmpty() ) return;

	FootballRecord *pRecord = FindRecordCharName( szAttacker );
	if( !pRecord || !pRecord->pChar ) return;

	int iPoint = FLOAT100 * ContributeTeamPer( pRecord->pChar->GetTeam(), FLOAT1 );

	if( eGoalTeam == pRecord->pChar->GetTeam() )
	{
		pRecord->Contribute.m_iAssist += iPoint;
	}

	ContributeConvert();
	ContributeRoundAllPercent();
	UpdateRoundInfoList();
	CheckTotalChamp();
}

void ioFootballMode::SetZeroHP( bool bZero )
{
	m_bZeroHP = bZero;

	if( m_bZeroHP )
	{
		const GameEntityList &rkObjectList = m_pCreator->GetGameEntityList();
		GameEntityList::const_iterator iter_b;
		for( iter_b = rkObjectList.begin() ; iter_b!=rkObjectList.end() ; ++iter_b )
		{
			ioBall *pBall = ToBallStruct( *iter_b );
			if( !pBall ) continue;

			pBall->SetZeroHP();
		}
	}
}




