#include "stdafx.h"

#include "ioTeamSurvivalAIMode.h"
#include "ioNpcChar.h"
#include "ioLadderTeamMgr.h"
#include "ioShuffleRoomMgr.h"
#include "ioRankBattleManager.h"

#include "GUI/MiniMap.h"
#include "GUI/MiniSoldierSelectWnd.h"
#include "GUI/CenterInfoWnd.h"
#include "GUI/TimeScoreWnd.h"
#include "GUI/ioObserverWnd.h"
#include "GUI/TotalResultMainWnd.h"
#include "GUI/RoundStateMainWnd.h"
#include "GUI/CenterKillInfoWnd.h"

ioTeamSurvivalAIMode::ioTeamSurvivalAIMode( ioPlayStage *pCreator )
: ioPlayMode( pCreator )
{
	m_iBlueKillPoint = 0;
	m_iRedKillPoint = 0;

	m_ModeType = MT_TEAM_SURVIVAL_AI;

	m_vCharRecordList.reserve( 100 );
}

ioTeamSurvivalAIMode::~ioTeamSurvivalAIMode()
{
	m_bZeroHP = false;
	ClearMode();
}

void ioTeamSurvivalAIMode::InitMode( int iMapIndex )
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

	char szKey[MAX_PATH] = "";
	kLoader.SetTitle_e( "npc_name" );
	m_iNPCNameCnt = kLoader.LoadInt_e( "name_cnt", 0 );
	m_vNPCName.reserve( m_iNPCNameCnt );

	for( int i=0; i<m_iNPCNameCnt; ++i )
	{
		wsprintf_e( szKey, "name%d", i+1 );
		kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_vNPCName.push_back( ioHashString(szBuf) );
	}
	std::random_shuffle( m_vNPCName.begin(), m_vNPCName.end() );
	m_iCurName = 0;

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

	kLoader.SetTitle_e( "AI_Level" );
	m_fEasyHP = kLoader.LoadFloat( "EasyHP", FLOAT1 );
	m_fEasyRecoveryTick = kLoader.LoadFloat( "EasyRecoveryTick", FLOAT1 );
	m_fHardHP = kLoader.LoadFloat( "NormalHP", FLOAT1 );
	m_fHardRecoveryTick = kLoader.LoadFloat( "NormalRecoveryTick", FLOAT1 );

	m_iLevel = g_BattleRoomMgr.GetAiLevel();

	//훈련병일 경우
	if ( g_MyInfo.GetGradeLevel() == 0 )
	{
		ioWnd* ptutorialkeyboardwnd = g_GUIMgr.AddWnd( "XML/tutorialkeyboardwnd.xml" );
		if ( ptutorialkeyboardwnd )
			ptutorialkeyboardwnd->ShowWnd();
	}
}

void ioTeamSurvivalAIMode::LoadModeResource( ioINILoader &rkLoader )
{
	TimeScoreWnd *pTimeScoreWnd = dynamic_cast<TimeScoreWnd *>( g_GUIMgr.AddWnd( "XML/TimeScoreWnd.xml") );
	if(pTimeScoreWnd)
		pTimeScoreWnd->SetModeType( GetModeType() );

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

void ioTeamSurvivalAIMode::ProcessMode()
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

void ioTeamSurvivalAIMode::ProcessGUI()
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

void ioTeamSurvivalAIMode::UpdateMiniMapSpotPoint()
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

void ioTeamSurvivalAIMode::UpdateTimeScoreWnd()
{
	TimeScoreWnd *pTimeScoreWnd = dynamic_cast<TimeScoreWnd *>( g_GUIMgr.FindWnd( TIME_SCORE_WND ) );
	if(!pTimeScoreWnd) return;

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
	/*
	int iBluePoint = (int)( GetBlueKillRate() * FLOAT100);
	int iRedPoint = (int)( GetRedKillRate() * FLOAT100 );
	*/

	pTimeScoreWnd->UpdateScore( iBluePoint, iRedPoint );

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

void ioTeamSurvivalAIMode::UpdateRoundStateWnd()
{
	if( g_MyInfo.IsSortContribute() )
	{
		DWORD dwTimeGap = FRAMEGETTIME() - m_dwPlayStartTime;
		if( m_dwCurRoundDuration > dwTimeGap )
			ContributeRoundAllPercent( m_dwRoundDuration - (m_dwCurRoundDuration - dwTimeGap), m_dwRoundDuration );
		else
			ContributeRoundAllPercent( m_dwRoundDuration, m_dwRoundDuration );

		TeamSurvivalAIRecord *pRecord = FindRecordCharName( g_MyInfo.GetPublicID() );
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
			/*
			int iBluePoint = (int)( GetBlueKillRate() * FLOAT100 );
			int iRedPoint = (int)( GetRedKillRate() * FLOAT100 );
			*/
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

void ioTeamSurvivalAIMode::ProcessReadyState()
{
	if( P2PNetwork::IsNetworkPlaying() )	return;
	if( m_dwReadyStartTime + 4000 > FRAMEGETTIME() )
		return;

	SetModeState( MS_PLAY );
}

void ioTeamSurvivalAIMode::ProcessPlayState()
{
	UpdateSoldierRevivalPenalty();
	//UpdateTimeGrowthTime();

	m_pCreator->NotifySpringDampUseSet( true );

	if( !P2PNetwork::IsNetworkPlaying() )
	{
		CheckRoundEnd();
	}
}

void ioTeamSurvivalAIMode::CheckRoundEnd()
{
}

int ioTeamSurvivalAIMode::GetTeamPlayerCount( TeamType eTeam )
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

float ioTeamSurvivalAIMode::GetBlueKillRate()
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

float ioTeamSurvivalAIMode::GetRedKillRate()
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

float ioTeamSurvivalAIMode::GetScoreKillPoint( TeamType eTeam )
{
	float fScoreKillPoint = GetTeamPlayerCount( eTeam ) * m_fWinScoreConstant * 100;

	return fScoreKillPoint;
}

void ioTeamSurvivalAIMode::ProcessResultState()
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

void ioTeamSurvivalAIMode::RenderPreGUI()
{
	if( m_ModeState == MS_PLAY )
	{
		RenderPushEnableCursor();
	}
}

void ioTeamSurvivalAIMode::RenderAfterGUI()
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

void ioTeamSurvivalAIMode::AddNewRecord( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	TeamSurvivalAIRecord *pTeamSurvivalAIRecord = FindRecordCharName(pChar->GetCharName());
	if( !pTeamSurvivalAIRecord )
	{
		TeamSurvivalAIRecord kRecord;
		kRecord.pChar  = pChar;
		kRecord.szName = pChar->GetCharName();


		ioNpcChar* pNpcChar = ToNpcChar( pChar );
		if( pNpcChar )
		{
			// aineed - ai npc type
			//kRecord.m_iType = pNpcChar->GetBossMonsterType();

			if( COMPARE( m_iCurName, 0, m_iNPCNameCnt ) )
			{
				pNpcChar->SetNpcViewName( m_vNPCName[m_iCurName] );
				m_iCurName++;
				if( !COMPARE( m_iCurName, 0, m_iNPCNameCnt ) )
					m_iCurName = 0;
			}
		}


		m_vCharRecordList.push_back( kRecord );		

		UpdateUserRank();

		if( pChar && !ToNpcChar( pChar ) )
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
		// ai need - npc type
// 		ioNpcChar* pNpcChar = ToNpcChar( pChar );
// 		if( pNpcChar )
// 			pTeamSurvivalAIRecord->m_iType = pNpcChar->GetBossMonsterType();

		pTeamSurvivalAIRecord->pChar = pChar;
	}
}

void ioTeamSurvivalAIMode::RemoveRecord( const ioHashString &rkName )
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


void ioTeamSurvivalAIMode::RemoveRecord( int nIndex )
{
	int iCharCnt = m_vCharRecordList.size();
	if( iCharCnt > nIndex )
	{
		m_vCharRecordList.erase( m_vCharRecordList.begin() + nIndex );
		UpdateUserRank();
	}
}


ioBaseChar* ioTeamSurvivalAIMode::GetRecordChar( int i )
{
	if( COMPARE( i, 0, GetRecordCharCnt() ) )
		return m_vCharRecordList[i].pChar;

	return NULL;
}

ioBaseChar* ioTeamSurvivalAIMode::GetRecordChar( const ioHashString &rkName )
{
	TeamSurvivalAIRecord *pRecord = FindRecordCharName( rkName );
	if( pRecord )
		return pRecord->pChar;

	return NULL;
}

int ioTeamSurvivalAIMode::GetRecordCharCnt() const
{
	return m_vCharRecordList.size();
}

ModeRecord* ioTeamSurvivalAIMode::FindModeRecord( int iIdx )
{
	if( COMPARE( iIdx, 0, GetRecordCharCnt() ) )
		return &m_vCharRecordList[iIdx];

	return NULL;
}

ModeRecord* ioTeamSurvivalAIMode::FindModeRecordCharName( const ioHashString &rkName )
{
	return (ModeRecord*)FindRecordCharName( rkName );
}

TeamSurvivalAIRecord* ioTeamSurvivalAIMode::FindRecordCharName( const ioHashString &rkName )
{
	int iCharCnt = m_vCharRecordList.size();
	for( int i=0 ; i<iCharCnt ; i++ )
	{
		if( m_vCharRecordList[i].szName == rkName )
			return &m_vCharRecordList[i];
	}

	return NULL;
}

void ioTeamSurvivalAIMode::SetRoundEndInfo( WinTeamType eTeam, bool bRoundSetEnd )
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

void ioTeamSurvivalAIMode::ShowEndScoreWnd( bool bShow )
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

bool ioTeamSurvivalAIMode::GetRoundInfo( int iIndex, RoundInfo &rkInfo )
{
	TeamSurvivalAIRecord *pRecord = static_cast<TeamSurvivalAIRecord*>( FindModeRecord(iIndex) );
	if( !pRecord )	return false;
	if( !pRecord->pChar ) return false;

	if( g_BattleRoomMgr.IsObserverUser( pRecord->pChar->GetCharName() ) || pRecord->pChar->GetState() == CS_OBSERVER )
		return false;

	rkInfo.m_bNPC = IsNpcChar( pRecord->pChar );
	if( rkInfo.m_bNPC )
	{
		if( ToNpcChar(pRecord->pChar) )
			rkInfo.m_szName	= ToNpcChar(pRecord->pChar)->GetViewName();
	}
	else
	{
		rkInfo.m_szName	= pRecord->pChar->GetCharName();
	}

	rkInfo.m_Team		        = pRecord->pChar->GetTeam();
	rkInfo.m_iLevel		        = pRecord->pChar->GetLevel();
	rkInfo.m_iTotalKill			= pRecord->GetAllTotalKill();
	rkInfo.m_iTotalDeath		= pRecord->GetAllTotalDeath();
	rkInfo.m_iUniqueTotalKill   = pRecord->GetUniqueTotalKill();
	rkInfo.m_iUniqueTotalDeath  = pRecord->GetUniqueTotalDeath();
	rkInfo.m_iLadderPoint       = g_UserInfoMgr.GetLadderPoint( rkInfo.m_szName );
	rkInfo.m_iVictories         = g_VictoriesMgr.GetVictories( rkInfo.m_szName );

	if( g_BattleRoomMgr.IsBattleRoom() )
	{
		rkInfo.m_iMonsterCoin = g_BattleRoomMgr.GetUserMonsterCoin( rkInfo.m_szName );
		rkInfo.m_iGoldMonsterCoin = g_BattleRoomMgr.GetUserGoldMonsterCoin( rkInfo.m_szName );
	}

	if( rkInfo.m_szName == g_MyInfo.GetPublicID() )
		rkInfo.m_bOwner = true;
	else
		rkInfo.m_bOwner = false;

	if (rkInfo.m_bOwner)
	{
		rkInfo.m_bEnable = true;
	}
	else
	{
		switch( pRecord->pChar->GetState() )
		{
		case CS_DIE:
		case CS_VIEW:
		case CS_LOADING:
		case CS_READY:
			rkInfo.m_bEnable = false;
			break;
		default:
			rkInfo.m_bEnable = true;
			break;
		}
	}
	g_PlayBonusMgr.GetUserPlayBonus( rkInfo.m_szName, rkInfo.m_bPCRoomBonus, rkInfo.m_bGuildBonus, rkInfo.m_bFriendBonus );
	g_UserInfoMgr.GetGuildInfo( rkInfo.m_szName, rkInfo.m_dwGuildIndex, rkInfo.m_dwGuildMark );

	//rkInfo.m_fHPRate = pRecord->pChar->GetHP().m_fCurValue / pRecord->pChar->GetHP().m_fMaxValue;

	rkInfo.m_iContribute	= pRecord->Contribute.m_iContribute;
	rkInfo.m_iContributePer = pRecord->Contribute.m_iPrevContributePer;

	return true;



	/*if( ioPlayMode::GetRoundInfo( iIndex, rkInfo ) )
	{
		TeamSurvivalAIRecord *pRecord = (TeamSurvivalAIRecord*)FindModeRecord( iIndex );
		if( !pRecord )	return false;
		if( !pRecord->pChar ) return false;

		rkInfo.m_iContribute	= pRecord->Contribute.m_iContribute;
		rkInfo.m_iContributePer = pRecord->Contribute.m_iPrevContributePer;		
		return true;
	}
	return false;*/
}

bool ioTeamSurvivalAIMode::GetResultInfo( int iIndex, ResultInfo &rkInfo )
{
	TeamSurvivalAIRecord *pRecord = static_cast<TeamSurvivalAIRecord*>( FindModeRecord(iIndex) );
	if( !pRecord )	return false;
	if( !pRecord->pChar ) return false;

	if( P2PNetwork::IsNetworkPlaying() && !pRecord->bResultShow )
		return false;

	if( pRecord->pChar->GetState() == CS_OBSERVER )
		return false;

	rkInfo.m_bNPC               = IsNpcChar( pRecord->pChar );
	rkInfo.m_szName	            = pRecord->pChar->GetCharName();
	rkInfo.m_Team		        = pRecord->pChar->GetTeam();
	rkInfo.m_iLevel             = pRecord->pChar->GetLevel();
	rkInfo.m_iTotalKill			= pRecord->GetAllTotalKill();
	rkInfo.m_iTotalDeath		= pRecord->GetAllTotalDeath();
	rkInfo.m_iUniqueTotalKill   = pRecord->GetUniqueTotalKill();
	rkInfo.m_iUniqueTotalDeath  = pRecord->GetUniqueTotalDeath();
	rkInfo.m_iLadderPoint       = g_UserInfoMgr.GetLadderPoint( rkInfo.m_szName );
	rkInfo.m_iVictories         = g_VictoriesMgr.GetVictories( rkInfo.m_szName );
	rkInfo.m_iRankDiff          = pRecord->iPreRank - pRecord->iCurRank;

	if( rkInfo.m_szName == g_MyInfo.GetPublicID() )
		rkInfo.m_bOwner = true;
	else
		rkInfo.m_bOwner = false;

	rkInfo.m_iContribute    = pRecord->Contribute.m_iContribute;
	rkInfo.m_iContributePer = pRecord->Contribute.m_iPrevContributePer;

	g_PlayBonusMgr.GetUserPlayBonus( rkInfo.m_szName, rkInfo.m_bPCRoomBonus, rkInfo.m_bGuildBonus, rkInfo.m_bFriendBonus );
	g_UserInfoMgr.GetGuildInfo( rkInfo.m_szName, rkInfo.m_dwGuildIndex, rkInfo.m_dwGuildMark );

	return true;

	/*if( ioPlayMode::GetResultInfo( iIndex, rkInfo ) )
	{
		TeamSurvivalAIRecord *pRecord = (TeamSurvivalAIRecord*)FindModeRecord( iIndex );
		if( !pRecord )	return false;
		if( !pRecord->pChar ) return false;

		rkInfo.m_iContribute = pRecord->Contribute.m_iContribute;
		rkInfo.m_iContributePer = pRecord->Contribute.m_iPrevContributePer;
		return true;
	}
	return false;*/
}

bool ioTeamSurvivalAIMode::GetFinalInfo( int iIndex, FinalInfo &rkInfo )
{
	TeamSurvivalAIRecord *pRecord = static_cast<TeamSurvivalAIRecord*>( FindModeRecord(iIndex) );
	if( !pRecord )
		return false;

	if( !pRecord->pChar )
		return false;

	if( g_BattleRoomMgr.IsObserverUser( pRecord->pChar->GetCharName() ) )
		return false;

	if( pRecord->pChar->GetState() == CS_OBSERVER )
		return false;

	if( P2PNetwork::IsNetworkPlaying() && !pRecord->bResultShow )
		return false;

	rkInfo.m_bNPC               = IsNpcChar( pRecord->pChar );
	rkInfo.m_szName		        = pRecord->pChar->GetCharName();
	rkInfo.m_Team		        = pRecord->pChar->GetTeam();
	rkInfo.m_bLevelUP           = pRecord->bResultLevelUP;
	rkInfo.m_iResultExp         = pRecord->iResultTotalExp;
	rkInfo.m_iResultPeso        = pRecord->iResultTotalPeso;
	rkInfo.m_iResultLadderPoint = pRecord->iResultTotalLadderPoint;
	rkInfo.m_iTreasureCardCnt   = pRecord->iResultTreasureCardCount;
	rkInfo.vResultClassType     = pRecord->vResultClassType;
	rkInfo.vResultClassGender   = pRecord->vResultClassGender;

	if( rkInfo.m_bLevelUP )
		rkInfo.m_iLevel  = pRecord->iResultLevel;
	else
		rkInfo.m_iLevel  = pRecord->pChar->GetLevel();

	rkInfo.m_iRank              = pRecord->iCurRank;
	rkInfo.m_iPvEClassType		= pRecord->iPvEClassType;
	rkInfo.m_dwPvECharIndex		= pRecord->dwPvECharIndex;
	rkInfo.m_iTotalKill			= pRecord->GetAllTotalKill();
	rkInfo.m_iTotalDeath		= pRecord->GetAllTotalDeath();
	rkInfo.m_iUniqueTotalKill   = pRecord->GetUniqueTotalKill();
	rkInfo.m_iUniqueTotalDeath  = pRecord->GetUniqueTotalDeath();
	rkInfo.m_iLadderPoint       = g_UserInfoMgr.GetLadderPoint( rkInfo.m_szName );
	rkInfo.m_iVictories         = g_VictoriesMgr.GetVictories( rkInfo.m_szName );

	if( rkInfo.m_szName == g_MyInfo.GetPublicID() )
		rkInfo.m_bOwner = true;
	else
		rkInfo.m_bOwner = false;

	rkInfo.m_iContribute    = pRecord->Contribute.m_iContribute;
	rkInfo.m_iContributePer = pRecord->Contribute.m_iPrevContributePer;

	if( rkInfo.m_bNPC && ToNpcChar(pRecord->pChar) )
	{
		rkInfo.m_szNPCName = ToNpcChar(pRecord->pChar)->GetViewName();
	}

	g_PlayBonusMgr.GetUserPlayBonus( rkInfo.m_szName, rkInfo.m_bPCRoomBonus, rkInfo.m_bGuildBonus, rkInfo.m_bFriendBonus );
	g_UserInfoMgr.GetGuildInfo( rkInfo.m_szName, rkInfo.m_dwGuildIndex, rkInfo.m_dwGuildMark );

	return true;

	/*if( ioPlayMode::GetFinalInfo( iIndex, rkInfo ) )
	{
		TeamSurvivalAIRecord *pRecord = (TeamSurvivalAIRecord*)FindModeRecord( iIndex );
		if( !pRecord )	return false;
		if( !pRecord->pChar ) return false;

		rkInfo.m_iContribute = pRecord->Contribute.m_iContribute;
		rkInfo.m_iContributePer = pRecord->Contribute.m_iPrevContributePer;
		return true;
	}
	return false;*/
}

void ioTeamSurvivalAIMode::ApplyModeInfo( SP2Packet &rkPacket )
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

void ioTeamSurvivalAIMode::ApplyExtraModeInfo( SP2Packet &rkPacket )
{
	rkPacket >> m_fWinScoreConstant;

	rkPacket >> m_iRedKillPoint;
	rkPacket >> m_iBlueKillPoint;
}

void ioTeamSurvivalAIMode::ApplyCharModeInfo( const ioHashString &rkName, SP2Packet &rkPacket, bool bDieCheck )
{
	bool bRecord;
	rkPacket >> bRecord;

	if( !bRecord ) return;

	TeamSurvivalAIRecord *pRecord = FindRecordCharName( rkName );
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

bool ioTeamSurvivalAIMode::ProcessUDPPacket( SP2Packet &rkPacket )
{
	if( ioPlayMode::ProcessUDPPacket( rkPacket ) )
		return true;

	return false;
}

void ioTeamSurvivalAIMode::NotifyEntityDie( ioPlayEntity *pEntity, ioPlayEntity *pKiller, ioBaseChar *pBestAttacker,
										 float fLastRate, float fBestRate )
{	
	ioBaseChar *pDieChar = ToBaseChar( pEntity );
	ioBaseChar *pKillChar = ToBaseChar( pKiller );

	// 순서 주의 이모티콘 출력때문
	UpdateKillDeathRecord( pEntity, pKiller, pBestAttacker );
	OnNotifyCharDie( pDieChar, pKillChar, pBestAttacker, fLastRate, fBestRate );
}

void ioTeamSurvivalAIMode::NotifyCharDropDie( ioBaseChar *pDieChar, ioBaseChar *pKillChar, ioBaseChar *pBestAttacker,
										   float fLastRate, float fBestRate )
{
	// 순서 주의 이모티콘 출력때문
	UpdateDropDieRecord( pDieChar, pKillChar, pBestAttacker );
	OnNotifyCharDie( pDieChar, pKillChar, pBestAttacker, fLastRate, fBestRate );
}

void ioTeamSurvivalAIMode::OnNotifyCharDie( ioBaseChar *pDieChar, ioBaseChar *pKillChar, ioBaseChar *pBestAttacker,
										 float fLastRate, float fBestRate )
{
	if( !pDieChar )
	{
		LOG.PrintTimeAndLog( 0, "ioTeamSurvivalAIMode::OnNotifyCharDie - Entity is Not Char" );
		return;
	}

	TeamSurvivalAIRecord *pDieRecord = FindRecordCharName( pDieChar->GetCharName() );
	if( !pDieRecord )
	{
		LOG.PrintTimeAndLog( 0, "ioTeamSurvivalAIMode::OnNotifyCharDie - %s Not Exist Char",
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
		ioHashString szKillViewName;
		ioHashString szDieViewName;
		ioNpcChar* pNPC = ToNpcChar( pKillChar );
		if( pNPC )
			szKillViewName = pNPC->GetViewName();
		else
			szKillViewName = pKillChar->GetCharName();

		pNPC = ToNpcChar( pDieChar );
		if( pNPC )
			szDieViewName = pNPC->GetViewName();
		else
			szDieViewName = pDieChar->GetCharName();

		g_ModeUIController.SetTeamDeathKO( pKillChar->GetTeam(), szKillViewName, pDieChar->GetTeam(), szDieViewName, 
										   GetBlueKillRate(), GetRedKillRate() );

		if( !pKillChar->IsOwnerChar() && !pDieChar->IsOwnerChar() )
		{
			m_pCreator->StartScreenBlind( SBT_CATCH_KILL, NULL, true );
		}
	}
	else if( pDieChar )
	{
		ioHashString szDieViewName;
		ioNpcChar* pNPC = ToNpcChar( pDieChar );
		if( pNPC )
			szDieViewName = pNPC->GetViewName();
		else
			szDieViewName = pDieChar->GetCharName();

		g_ModeUIController.SetTeamDeathKO( TEAM_NONE, "", pDieChar->GetTeam(), szDieViewName, 
										   GetBlueKillRate(), GetRedKillRate() );
		if( !pDieChar->IsOwnerChar() )
		{
			m_pCreator->StartScreenBlind( SBT_CATCH_KILL, NULL, true );
		}
	}

	//For EndScene
	if( ToNpcChar(pDieChar) )
		m_LastDieChar = ToNpcChar(pDieChar)->GetViewName();
	else
		m_LastDieChar = pDieChar->GetCharName();

	if( pKillChar )
	{
		TeamSurvivalAIRecord *pKillRecord = FindRecordCharName( pKillChar->GetCharName() );
		if( !pKillRecord )
		{
			LOG.PrintTimeAndLog( 0, "ioTeamSurvivalAIMode::OnNotifyCharDie - %s Not Exist Kill Char",
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

void ioTeamSurvivalAIMode::ReStartMode()
{
	D3DXVECTOR3 vStartPos;

	int iCharCnt = m_vCharRecordList.size();
	for( int i=0 ; i<iCharCnt ; i++ )
	{
		TeamSurvivalAIRecord &rkRecord = m_vCharRecordList[i];
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

D3DXVECTOR3 ioTeamSurvivalAIMode::GetRevivalPos( ioBaseChar *pChar )
{
	D3DXVECTOR3 vPos = GetStartPos( pChar->GetTeam() );
	vPos.x += (float)( rand() % 100 ) / 20.0f;
	vPos.z += (float)( rand() % 100 ) / 20.0f;
	vPos.y = m_pCreator->GetMapHeightByRevial( vPos.x, vPos.z );

	return vPos;
}

void ioTeamSurvivalAIMode::ContributeLOG()
{
	char szLog[MAX_PATH];

	int iCharCnt = m_vCharRecordList.size();
	for( int i=0 ;i<iCharCnt; i++ )
	{
		TeamSurvivalAIRecord *pRecord = &m_vCharRecordList[i];
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

void ioTeamSurvivalAIMode::ContributeConvertTeam( TeamType eTeam )
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
		TeamSurvivalAIRecord *pRecord = &m_vCharRecordList[i];
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
			TeamSurvivalAIRecord *pRecord = &m_vCharRecordList[iArray];
			if( !pRecord->Contribute.m_bLastResultSync )
				pRecord->Contribute.m_iContribute = m_vContributeTable[vInfo[i].m_iRank];
			pRecord->Contribute.m_iKillRank = vInfo[i].m_iRank;
		}
	}
	vInfo.clear();

	//데미지 & 힐
	for(i = 0;i < iCharCnt;i++)
	{
		TeamSurvivalAIRecord *pRecord = &m_vCharRecordList[i];
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
			TeamSurvivalAIRecord *pRecord = &m_vCharRecordList[iArray];
			if( !pRecord->Contribute.m_bLastResultSync )
				pRecord->Contribute.m_iContribute += m_vContributeTable[vInfo[i].m_iRank] * 6;
			pRecord->Contribute.m_iDamageRank = vInfo[i].m_iRank;
		}
	}
	vInfo.clear();

	//유효 타격 횟수
	for(i = 0;i < iCharCnt;i++)
	{
		TeamSurvivalAIRecord *pRecord = &m_vCharRecordList[i];
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
			TeamSurvivalAIRecord *pRecord = &m_vCharRecordList[iArray];
			if( !pRecord->Contribute.m_bLastResultSync )
				pRecord->Contribute.m_iContribute += m_vContributeTable[vInfo[i].m_iRank] * 2;
			pRecord->Contribute.m_iAttackCntRank = vInfo[i].m_iRank;
		}
	}
	vInfo.clear();

	//데쓰처리
	for(i = 0;i < iCharCnt;i++)
	{
		TeamSurvivalAIRecord *pRecord = &m_vCharRecordList[i];
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
			TeamSurvivalAIRecord *pRecord = &m_vCharRecordList[iArray];
			if( !pRecord->Contribute.m_bLastResultSync )
				pRecord->Contribute.m_iContribute += m_vContributeTable[vInfo[i].m_iRank];
			pRecord->Contribute.m_iDeathRank = vInfo[i].m_iRank;
		}
	}
	vInfo.clear();
}

void ioTeamSurvivalAIMode::ContributeConvert()
{
	ContributeConvertTeam( TEAM_NONE );        //TEAM_NONE는 전체 유저에 대한 기여도 평가	
}

int ioTeamSurvivalAIMode::ContributeMyRank( int &rRankValue1, int &rRankValue2, int &rRankValue3, int &rRankValue4 )
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
		TeamSurvivalAIRecord *pRecord = &m_vCharRecordList[i];
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
		TeamSurvivalAIRecord *pRecord = &m_vCharRecordList[i];
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
		TeamSurvivalAIRecord *pRecord = &m_vCharRecordList[i];
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

void ioTeamSurvivalAIMode::ContributeRoundAllPercent( DWORD dwCurTime, DWORD dwTotalTime )
{
	//기여도의 100분율
	int iMaxContribute	= 0;
	int iCharCnt		= m_vCharRecordList.size();
	int iPlayCharCnt	= 0;

	int i = 0;
	for(i = 0;i < iCharCnt;i++)
	{
		TeamSurvivalAIRecord *pRecord = &m_vCharRecordList[i];
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
			TeamSurvivalAIRecord *pRecord = &m_vCharRecordList[i];
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

void ioTeamSurvivalAIMode::DamageContribute( const ioHashString szAttacker, const ioHashString szTarget, float fDamage, DWORD dwItemCode, int iWeaponType )
{
	if( szAttacker.IsEmpty() ) return;

	TeamSurvivalAIRecord *pRecord = FindRecordCharName( szAttacker );
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

void ioTeamSurvivalAIMode::KillContribute( const ioHashString szKiller, const ioHashString szKilled, DWORD dwItemCode )
{
	if( szKilled.IsEmpty() ) return;

	int iKillPoint = 100;

	float fAddKillPoint = FLOAT1;
	if( IsContributeAddPointItemCode( dwItemCode ) )
		fAddKillPoint = 1.1f;

	if( szKiller.IsEmpty() || szKiller == szKilled )     //자살
	{
		TeamSurvivalAIRecord *pRecord = FindRecordCharName( szKilled );
		if( !pRecord || !pRecord->pChar ) return;
		
		pRecord->Contribute.m_iKill -= iKillPoint;
	}
	else
	{
		TeamSurvivalAIRecord *pKillerRecord = FindRecordCharName( szKiller );
		TeamSurvivalAIRecord *pKilledRecord = FindRecordCharName( szKilled );
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

void ioTeamSurvivalAIMode::AttackCntContribute( const ioHashString szAttacker, const ioHashString szTarget, DWORD dwItemCode, int iAttackValue/* = 100 */ )
{
	if( szAttacker.IsEmpty() || szTarget.IsEmpty() ) return;

	TeamSurvivalAIRecord *pAttackerRecord = FindRecordCharName( szAttacker );
	TeamSurvivalAIRecord *pTargetRecord = FindRecordCharName( szTarget );

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

void ioTeamSurvivalAIMode::FillLastPlayRecordInfo( SP2Packet &rkPacket )
{
	int iCharCnt = m_vCharRecordList.size();
	rkPacket << iCharCnt;
	for(int i = 0;i < iCharCnt;i++)
	{
		TeamSurvivalAIRecord *pRecord = &m_vCharRecordList[i];
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

void ioTeamSurvivalAIMode::FillPlayRecordInfo( SP2Packet &rkPacket )
{
	int iCharCnt = m_vCharRecordList.size();
	rkPacket << iCharCnt;
	for(int i = 0;i < iCharCnt;i++)
	{
		TeamSurvivalAIRecord *pRecord = &m_vCharRecordList[i];
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

void ioTeamSurvivalAIMode::ApplyPlayRecordInfo( SP2Packet &rkPacket )
{
	int iCharCnt;
	rkPacket >> iCharCnt;
	for(int i = 0;i < iCharCnt;i++)
	{
		ioHashString szName;
		int iDamage, iAttackCnt, iKill, iDeath, iPrevContributePer, iUniqueTotalKill, iUniqueTotalDeath, iVictories, iNoDeathKillCount;
		rkPacket >> szName >> iDamage >> iAttackCnt >> iKill >> iDeath >> iPrevContributePer >> iUniqueTotalKill >> iUniqueTotalDeath >> iVictories >> iNoDeathKillCount;

		TeamSurvivalAIRecord *pRecord = FindRecordCharName( szName );
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

void ioTeamSurvivalAIMode::ContributeResultSync( const ioHashString &szName, int iContribute )
{
	TeamSurvivalAIRecord *pRecord = FindRecordCharName( szName );
	if( !pRecord ) return;

	pRecord->Contribute.m_bLastResultSync = true;
	pRecord->Contribute.m_iContribute     = iContribute;
}

void ioTeamSurvivalAIMode::OnZeroHP( SP2Packet &rkPacket )
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

void ioTeamSurvivalAIMode::OnRoundReady( SP2Packet &rkPacket )
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

void ioTeamSurvivalAIMode::OnRoundJoin( SP2Packet &rkPacket )
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

		// 더미
		rkPacket >> dwStateGap;
		rkPacket >> dwSelectTime;

		rkPacket >> m_dwCurRoundDuration;

		rkPacket >> m_fWinScoreConstant;
		rkPacket >> m_iRedKillPoint;
		rkPacket >> m_iBlueKillPoint;
	}

	m_pCreator->IncreaseTeamCntForBalance( pJoinChar->GetTeam() );
	m_pCreator->ReCalculateDamageRate( false );
}

void ioTeamSurvivalAIMode::OnRoundJoinView( SP2Packet &rkPacket )
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

	m_pCreator->ReCalculateDamageRate( false );
}

void ioTeamSurvivalAIMode::OnRoundJoinObserver( SP2Packet &rkPacket )
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

	m_pCreator->ReCalculateDamageRate( false );
}

void ioTeamSurvivalAIMode::SetResultScoreWnd()
{
	RoundResultMainWnd *pResult = dynamic_cast<RoundResultMainWnd*>(g_GUIMgr.FindWnd( ROUND_RESULT_MAIN_WND ));
	if( !pResult )	return;

	// RoundHistory를 참조하는 곳이 많아 일단 갱신. 나중에 구조개편 검토 필요
	int iHisRoundBlue = m_iBlueRoundWinCnt;
	int iHisRoundRed  = m_iRedRoundWinCnt;

	if( P2PNetwork::IsNetworkPlaying() )
	{
		for(int i = 0;i < m_iCurRoundCount - 1;i++)
		{
			iHisRoundBlue -= m_vRoundHistory[ i ].iBluePoint;
			iHisRoundRed  -= m_vRoundHistory[ i ].iRedPoint;
		}
		m_vRoundHistory[ m_iCurRoundCount - 1 ].iBluePoint = max( iHisRoundBlue, 0 );
		m_vRoundHistory[ m_iCurRoundCount - 1 ].iRedPoint  = max( iHisRoundRed, 0 );
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

void ioTeamSurvivalAIMode::SetTotalScoreWnd( IntVec &rvClassTypeList,
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
		for(i = 0;i < m_iCurRoundCount - 1;i++)
		{
			iHisRoundBlue -= m_vRoundHistory[ i ].iBluePoint;
			iHisRoundRed  -= m_vRoundHistory[ i ].iRedPoint;
		}
		m_vRoundHistory[ m_iCurRoundCount - 1 ].iBluePoint = max( iHisRoundBlue, 0 );
		m_vRoundHistory[ m_iCurRoundCount - 1 ].iRedPoint  = max( iHisRoundRed, 0 );
	}

	m_dwCurResultShowDuration = m_dwFinalRoundResultDuration;


	int iBluePoint = (int)( m_fFinalBluePoint * FLOAT100 );
	int iRedPoint = (int)( m_fFinalRedPoint * FLOAT100 );
	pTotalResult->SetTeamScore( GetOwnerTeam(), iBluePoint, iRedPoint );
	SetTotalResultBriefingWnd();
}

void ioTeamSurvivalAIMode::CheckEndSceneLookAt()
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
		TeamSurvivalAIRecord *pKillRecord = FindRecordCharName( m_LastKillChar );
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
			TeamSurvivalAIRecord *pDieRecord = FindRecordCharName( m_LastDieChar );
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

void ioTeamSurvivalAIMode::ClearModeData()
{
	m_LastDieChar.Clear();
	m_LastKillChar.Clear();

	g_SoundMgr.AllSoundStop();
	g_SoundMgr.StopStream();
	m_pCreator->HidingAllFieldItem();
}

bool ioTeamSurvivalAIMode::IsNeedRevival( ioBaseChar *pChar )
{
	return true;
	/*
	if( !pChar ) return false;

	if( !m_LastKillChar.IsEmpty() && m_LastKillChar == pChar->GetCharName() )
		return true;

	return false;
	*/
}

ioBaseChar* ioTeamSurvivalAIMode::CheckMostNearChar( ioBaseChar *pChar )
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

int ioTeamSurvivalAIMode::GetCurRank( const ioHashString& szCharName )
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

int ioTeamSurvivalAIMode::GetContributePerChar( const ioHashString& szName )
{
	int iCharCnt = m_vCharRecordList.size();

	for(int i = 0;i < iCharCnt;i++)
	{
		TeamSurvivalAIRecord *pRecord = &m_vCharRecordList[i];
		if( !pRecord )	continue;
		if( !pRecord->pChar ) continue;

		if( pRecord->pChar->GetCharName() == szName )
		{
			return pRecord->Contribute.m_iPrevContributePer;
		}
	}

	return 0;
}

void ioTeamSurvivalAIMode::SetKillPointEmoticon( TeamType eTeam, const ioHashString &szAttacker )
{
	int iCharCnt = m_vCharRecordList.size();

	for(int i = 0;i < iCharCnt;i++)
	{
		TeamSurvivalAIRecord *pRecord = &m_vCharRecordList[i];
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

float ioTeamSurvivalAIMode::GetCurExtraScoreGaugeRate( TeamType eTeam )
{
	float fExtraRate = FLOAT1;
	if( eTeam == TEAM_RED )
	{
		fExtraRate = g_MathManager.GetTeamSurvivalBalanceGauge( GetRedKillRate(), GetBlueKillRate() );
		fExtraRate = max( 0.0, fExtraRate );
	}
	else if( eTeam == TEAM_BLUE )
	{
		fExtraRate = g_MathManager.GetTeamSurvivalBalanceGauge( GetBlueKillRate(), GetRedKillRate() );
		fExtraRate = max( 0.0, fExtraRate );
	}

	return fExtraRate;
}

float ioTeamSurvivalAIMode::GetCurExtraScoreDamage( TeamType eTeam, float fCurDamage, float fBaseDamage )
{
	float fResult = fCurDamage;

	if( eTeam == TEAM_RED )
		fResult = g_MathManager.GetTeamSurvivalBalanceDamage( fCurDamage, fBaseDamage, GetRedKillRate(), GetBlueKillRate() );
	else if( eTeam == TEAM_BLUE )
		fResult = g_MathManager.GetTeamSurvivalBalanceDamage( fCurDamage, fBaseDamage, GetBlueKillRate(), GetRedKillRate() );

	return fResult;
}

void ioTeamSurvivalAIMode::OnRoundEnd( SP2Packet &rkPacket )
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

	//훈련병일 경우
	if ( g_MyInfo.GetGradeLevel() == 0 )
		g_GUIMgr.HideWnd( TUTORIAL_KEYBOARD_WND );
}

bool ioTeamSurvivalAIMode::ProcessTCPPacket( SP2Packet &rkPacket )
{
	int iID = rkPacket.GetPacketID();

	switch( iID )
	{
	case STPK_MONSTER_FORCE_DIE:
		ErrorReport::SetPosition( 1033, 115 );
		//		OnMonsterForceDie( rkPacket );
		ErrorReport::SetPosition( 1033, 116 );
		return true;
	case STPK_SPAWN_AI_MONSTER:
		ErrorReport::SetPosition( 1033, 123 );
		OnRespawnMonster( rkPacket );
		ErrorReport::SetPosition( 1033, 124 );
		return true;
	}

	ErrorReport::SetPosition( 1033, 101 );
	if( ioPlayMode::ProcessTCPPacket( rkPacket ) )
		return true;
	ErrorReport::SetPosition( 1033, 102 );	

	return false;
}

void ioTeamSurvivalAIMode::OnRespawnMonster( SP2Packet &rkPacket )
{
	int iMaxNpc;
	rkPacket >> iMaxNpc;

	for(int i = 0;i < iMaxNpc;i++)
	{
		int iTeamType;
		DWORD dwNpcCode;
		DWORD dwMonsterID;
		float fStartXPos, fStartZPos;
		ioHashString kNpcName, kSyncName;
		rkPacket >> dwNpcCode >> dwMonsterID >> iTeamType >> kNpcName >> kSyncName >> fStartXPos >> fStartZPos;

		//추가로 장비인덱스.
		int iEquipGameIndex[4] = {0,};
		for( int i = 0; i < 4; ++i )
			rkPacket >> iEquipGameIndex[i];

		ioHashString szNPCName;
		int iUniqueTotalKill   = 0;
		int iUniqueTotalDeath  = 0;
		int iCurRank           = 0;
		int iPreRank           = 0;
		IntVec vClassType;
		IntVec vGenderType;
		ModeRecord *pTempRecord = FindModeRecordCharName( kNpcName );
		if( pTempRecord && ToNpcChar(pTempRecord->pChar) )
		{
			szNPCName          = ToNpcChar(pTempRecord->pChar)->GetViewName();
			iUniqueTotalKill   = pTempRecord->GetUniqueTotalKill();
			iUniqueTotalDeath  = pTempRecord->GetUniqueTotalDeath();
			iCurRank           = pTempRecord->iCurRank;
			iPreRank           = pTempRecord->iPreRank;

			for( size_t i=0; i<pTempRecord->vResultClassType.size(); ++i )
				vClassType.push_back( pTempRecord->vResultClassType[i] );

			for( size_t i=0; i<pTempRecord->vResultClassGender.size(); ++i )
				vGenderType.push_back( pTempRecord->vResultClassGender[i] );
		}

		//있으면 지움
		//LOG.PrintTimeAndLog( 0, "[AI] OnRespawn Remove - Name(%s)", kNpcName.c_str() );
		m_pCreator->RemoveUser( kNpcName, true );

		ioNpcChar* pNpcChar = g_NpcMgr.CreateNpcChar( dwMonsterID, dwNpcCode, m_pCreator, this );
		if( !pNpcChar )
		{
			//LOG.PrintTimeAndLog( 0, "[AI] OnRespawnMonster npc create fail - Name(%s)m code(%u)", kNpcName.c_str(), dwNpcCode );
			continue;		
		}

		pNpcChar->FillMaxHP();
		pNpcChar->SetCharName( kNpcName );
		pNpcChar->SetSyncUser( kSyncName );		
		pNpcChar->SetTeam( (TeamType)iTeamType );

		//pNpcChar->SetNpcMaxHpRate( FLOAT1 );
		//pNpcChar->SetSpeedRate( FLOAT1 );

		D3DXVECTOR3 vStartPos = GetStartPos( (TeamType)iTeamType );

		pNpcChar->StartAI( 0, vStartPos.x, vStartPos.z, false, dwMonsterID );

		if( ioPlayMode::IsZeroHP() )
		{
			pNpcChar->SetCurHP( 0.0f );
			pNpcChar->SetCurHPRate( 0.0f );
		}

		for( int i = 0; i < 4; ++i )
		{
			ioItem* pItem = pNpcChar->GetEquipedItem( i );
			if( pItem )
				pItem->SetItemCreateIndex( iEquipGameIndex[i] );
		}


		AddNewRecord( pNpcChar );
		ModeRecord *pNPCRecord = FindModeRecordCharName( kNpcName );
		if( pNPCRecord )
		{
			if( !szNPCName.IsEmpty() )
				pNpcChar->SetNpcViewName( szNPCName );
			pNPCRecord->iUniqueTotalKill   = iUniqueTotalKill;
			pNPCRecord->iUniqueTotalDeath  = iUniqueTotalDeath;
			pNPCRecord->iCurRank           = iCurRank;
			pNPCRecord->iPreRank           = iPreRank;
			pNPCRecord->vResultClassType   = vClassType;
			pNPCRecord->vResultClassGender = vGenderType;

			IntVec::iterator iter = std::find( pNPCRecord->vResultClassType.begin(), pNPCRecord->vResultClassType.end(), pNPCRecord->iPvEClassType );
			if( iter == pNPCRecord->vResultClassType.end() )
			{
				pNPCRecord->vResultClassType.push_back( pNpcChar->GetCharacterInfo().m_class_type );
				pNPCRecord->vResultClassGender.push_back( pNpcChar->IsMale() == true ? 1 : 2 );		// man, woman >> 1, 2
			}
		}
	}
}

void ioTeamSurvivalAIMode::UpdateDropDieRecord( ioBaseChar *pDieChar, ioBaseChar *pKillChar, ioBaseChar *pBestAttacker )
{
	if( pKillChar )
	{
		ModeRecord *pAttRecord = FindModeRecordCharName( pKillChar->GetCharName() );
		ModeRecord *pDieRecord = FindModeRecordCharName( pDieChar->GetCharName() );

		if( pAttRecord && pDieRecord )
		{
			ioHashString szAttName = GetViewName( pAttRecord->pChar );
			ioHashString szDieName = GetViewName( pDieRecord->pChar );

			float fKillPoint = FLOAT05 + ( (float)GetKillPoint( pAttRecord->pChar->GetTeam() ) * FLOAT05 );
			if( pAttRecord->pChar->GetTeam() != pDieRecord->pChar->GetTeam() )
			{
				pAttRecord->AddKill( GetModeType(), fKillPoint, true, false );
				pDieRecord->AddDeath( GetModeType(), GetDeathPoint( pDieRecord->pChar->GetTeam() ) );

				g_ModeUIController.SetKOMsg( pAttRecord->pChar->GetTeam(), szAttName,
					pDieRecord->pChar->GetLastAttackerSkillName(), pDieRecord->pChar->GetLastAttackerItemCode(),
					pDieRecord->pChar->GetTeam(), szDieName );
			}
			else
			{
				if( pAttRecord->pChar == pDieRecord->pChar )	// 자기자신의 공격
				{
					pDieRecord->AddDeath( GetModeType(), GetDeathPoint( pDieRecord->pChar->GetTeam() ) );
					g_ModeUIController.SetKOMsg( TEAM_NONE, "",
						"", 0, pDieRecord->pChar->GetTeam(), szDieName );
				}
				else
				{
					pAttRecord->AddKill( GetModeType(), -fKillPoint, true, false );
					pDieRecord->AddDeath( GetModeType(), GetDeathPoint( pDieRecord->pChar->GetTeam() ) );

					g_ModeUIController.SetKOMsg( TEAM_NONE,	"",
						"", 0, pDieRecord->pChar->GetTeam(), szDieName );
				}
			}

			if ( g_ModeUIController.IsCenterKillInfoWnd() )
				pAttRecord->pChar->SetKillEmoticon( pAttRecord->GetNoDeathKillCount() );
			else
				pAttRecord->pChar->SetKillEmoticon( pAttRecord->GetUniqueTotalKill() );
			pDieRecord->pChar->SetDeathEmoticon();

			ioBaseChar *pOwner = GetOwnerChar();
			if( pOwner )
				g_ModeUIController.SetCenterKillUI( m_ModeType, pAttRecord->GetNoDeathKillCount(), pOwner, pAttRecord->pChar->GetTeam(), szAttName,
					pDieRecord->pChar->GetLastAttackerItemCode(), pDieRecord->pChar->GetTeam(), szDieName, pAttRecord->pChar->IsMale() );
		}
	}
	else
	{
		ModeRecord *pDieRecord = FindModeRecordCharName( pDieChar->GetCharName() );
		if( pDieRecord && pDieRecord->pChar )
		{
			ioHashString szDieName = GetViewName( pDieRecord->pChar );

			pDieRecord->AddDeath( GetModeType(), GetDeathPoint( pDieRecord->pChar->GetTeam() ) );

			g_ModeUIController.SetKOMsg( TEAM_NONE, "",
				"", 0, pDieRecord->pChar->GetTeam(), szDieName );

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

void ioTeamSurvivalAIMode::UpdateKillDeathRecord( ioPlayEntity *pDieEntity, ioPlayEntity *pKiller, ioPlayEntity *pBestAttacker )
{
	ioBaseChar *pKillerChar = ToBaseChar( pKiller );
	ioBaseChar *pDieChar = ToBaseChar( pDieEntity );

	if( pKillerChar && pDieChar )
	{
		ModeRecord *pAttRecord = FindModeRecordCharName( pKillerChar->GetCharName() );
		ModeRecord *pDieRecord = FindModeRecordCharName( pDieChar->GetCharName() );

		if( pAttRecord && pDieRecord )
		{
			ioHashString szAttName = GetViewName( pAttRecord->pChar );
			ioHashString szDieName = GetViewName( pDieRecord->pChar );

			float fKillPoint = FLOAT05 + ( (float)GetKillPoint( pAttRecord->pChar->GetTeam() ) * FLOAT05 );
			if( pAttRecord->pChar->GetTeam() != pDieRecord->pChar->GetTeam() )
			{
				pAttRecord->AddKill( GetModeType(), fKillPoint, false, false );
				pDieRecord->AddDeath( GetModeType(), GetDeathPoint( pDieRecord->pChar->GetTeam() ) );

				g_ModeUIController.SetKOMsg( pAttRecord->pChar->GetTeam(), szAttName,
					pDieRecord->pChar->GetLastAttackerSkillName(), pDieRecord->pChar->GetLastAttackerItemCode(),
					pDieRecord->pChar->GetTeam(), szDieName );
			}
			else
			{
				if( pAttRecord->pChar == pDieRecord->pChar )	// 자기자신의 공격
				{
					pDieRecord->AddDeath( GetModeType(), GetDeathPoint( pDieRecord->pChar->GetTeam() ) );
					g_ModeUIController.SetKOMsg( TEAM_NONE, "",
						"", 0, pDieRecord->pChar->GetTeam(), szDieName );
				} 
				else
				{
					pAttRecord->AddKill( GetModeType(), -fKillPoint, false, false );
					pDieRecord->AddDeath( GetModeType(), GetDeathPoint( pDieRecord->pChar->GetTeam() ) );

					g_ModeUIController.SetKOMsg( TEAM_NONE, "",
						"", 0, pDieRecord->pChar->GetTeam(), szDieName );
				}
			}

			if ( g_ModeUIController.IsCenterKillInfoWnd() )
				pAttRecord->pChar->SetKillEmoticon( pAttRecord->GetNoDeathKillCount() );
			else
				pAttRecord->pChar->SetKillEmoticon( pAttRecord->GetUniqueTotalKill() );
			pDieRecord->pChar->SetDeathEmoticon();

			ioBaseChar *pOwner = GetOwnerChar();
			if( pOwner )
				g_ModeUIController.SetCenterKillUI( m_ModeType, pAttRecord->GetNoDeathKillCount(), pOwner, pAttRecord->pChar->GetTeam(), szAttName,
						pDieRecord->pChar->GetLastAttackerItemCode(), pDieRecord->pChar->GetTeam(), szDieName, pAttRecord->pChar->IsMale() );
		}
	}
	else
	{
		ModeRecord *pDieRecord = FindModeRecordCharName( pDieChar->GetCharName() );
		if( pDieRecord && pDieRecord->pChar )
		{
			ioHashString szDieName = GetViewName( pDieRecord->pChar );

			pDieRecord->AddDeath( GetModeType(), GetDeathPoint( pDieRecord->pChar->GetTeam() ) );

			g_ModeUIController.SetKOMsg( TEAM_NONE, "",
				"", 0, pDieRecord->pChar->GetTeam(), szDieName );

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
			pAttRecord->AddKill( GetModeType(), fKillPoint, false, true );
		}
	}
}

ioHashString ioTeamSurvivalAIMode::GetViewName( ioBaseChar* pChar )
{
	ioNpcChar* pNPC = ToNpcChar( pChar );
	if( pNPC )
		return pNPC->GetViewName();

	return pChar->GetCharName();
}

void ioTeamSurvivalAIMode::OnMonsterSyncChange( SP2Packet &rkPacket )
{
	int iMaxNpc;
	rkPacket >> iMaxNpc;
	for(int i = 0;i < iMaxNpc;i++)
	{
		ioHashString szNpcName, szSyncName;
		rkPacket >> szNpcName >> szSyncName;

		TeamSurvivalAIRecord* pRecord = FindRecordCharName( szNpcName );
		if( !pRecord ) continue;

		ioNpcChar* pNpc = ToNpcChar( pRecord->pChar );
		if( pNpc == NULL ) continue;

		pNpc->SetSyncUser( szSyncName );
	}
}

float ioTeamSurvivalAIMode::GetLevelHP()
{
	if( m_iLevel == 0 )
		return m_fEasyHP;
	return m_fHardHP;
}

float ioTeamSurvivalAIMode::GetLevelRecoveryTick()
{
	if( m_iLevel == 0 )
		return m_fEasyRecoveryTick;
	return m_fHardRecoveryTick;
}
