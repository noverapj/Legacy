

#include "stdafx.h"

#include "ioFlagCaptureMode.h"
#include "ioLadderTeamMgr.h"
#include "ioRankBattleManager.h"
#include "ioModeGuideArrow.h"
#include "ioShuffleRoomMgr.h"
#include "ioCloakWearItem.h"

#include "GUI/MiniMap.h"
#include "GUI/MiniSoldierSelectWnd.h"
#include "GUI/CenterInfoWnd.h"
#include "GUI/TimeScoreWnd.h"
#include "GUI/ioObserverWnd.h"
#include "GUI/TotalResultMainWnd.h"
#include "GUI/RoundStateMainWnd.h"
#include "GUI/CenterKillInfoWnd.h"

ioFlagCaptureMode::ioFlagCaptureMode( ioPlayStage *pCreator )
: ioPlayMode( pCreator )
{
	m_ModeType = MT_FLAG_CAPTURE;
	
	m_fCurRedFlagPoint  = 0.0f;
	m_fCurBlueFlagPoint = 0.0f;

	m_pGuideBlueArrow = NULL;
	m_pGuideRedArrow  = NULL;

	m_bSendSync   = false;

	m_dwBlueContribute = 0;
	m_dwRedContribute  = 0;

	m_dwCheckFlagPingTime = 0;

	m_dwSyncRequestTime		  = 0;

	m_dwCaptureFlagTime	= 0;

	m_iCurSendIndex = 1;

	m_dwBlueReturnPointID = -1;
	m_dwRedReturnPointID = -1;

	m_fReturnPointRadius = 0.0f;

	m_bFlagReturnSend = false;

	m_pResultChar = NULL;

	m_szFlagCaptureChar.Clear();

	m_vCharRecordList.reserve( 100 );
}

ioFlagCaptureMode::~ioFlagCaptureMode()
{
	m_bZeroHP = false;
	ClearMode();

	SAFEDELETE( m_pGuideBlueArrow );
	SAFEDELETE( m_pGuideRedArrow );
}

void ioFlagCaptureMode::InitMode( int iMapIndex )
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
	m_dwRoundDuration	 = kLoader.LoadInt_e( "round_time", 300000 );
	m_dwCurRoundDuration = m_dwRoundDuration;

	m_fScoreWarningRate  = kLoader.LoadFloat_e( "score_warning_rate", FLOAT05 );
	
	kLoader.SetTitle_e( "flag_point" );
	m_fDefaultFlagPoint = kLoader.LoadFloat_e( "max_flag_point", FLOAT100 );

	m_fWinningTextRate = kLoader.LoadFloat_e( "winning_text_rate", 0.15f );

	LoadModeGuide( kLoader );
	LoadRevivalTime( kLoader );
	LoadModeResource( kLoader );
	LoadModeCamera( kLoader );
	LoadMap();

	char szBuf[MAX_PATH];
	kLoader.SetTitle_e( "ko_recover" );
	m_fKORecoverRate = kLoader.LoadFloat_e( "ko_recover_rate", 0.0f );
	kLoader.LoadString_e( "ko_recover_effect", "", szBuf, MAX_PATH );
	m_KORecoverEffect = szBuf;
	kLoader.LoadString_e( "ko_perfect_effect", "", szBuf, MAX_PATH );
	m_PerfectRecoverEffect = szBuf;
	kLoader.LoadString_e( "ko_buff", "", szBuf, MAX_PATH );
	m_KOBuff = szBuf;

	kLoader.SetTitle_e( "flag_info" );
	kLoader.LoadString_e( "flag_buff_name", "", szBuf, MAX_PATH );
	m_szFlagBuff = szBuf;
	m_fFlagPointPerSec = kLoader.LoadFloat_e( "flag_own_time_point", 0.0f );
	m_fBaseFlagPoint = kLoader.LoadFloat_e( "flag_own_time_point_base", 0.0f );
	m_fMaxFlagPoint = kLoader.LoadFloat_e( "flag_own_time_point_max_values", 0.0f );
	m_dwFlagReturnTerm = kLoader.LoadInt( "flag_return_term", 0 );

	kLoader.SetTitle_e( "end_scene_camera" );
	m_fEndHeight = kLoader.LoadFloat_e( "end_camera_height", 0.0f );
	m_fEndDistance = kLoader.LoadFloat_e( "end_camera_distance", 0.0f );
	m_fEndFov = kLoader.LoadFloat_e( "end_camera_fov", 0.0f );
	kLoader.LoadString_e( "round_end_focus_effect", "", szBuf, MAX_PATH );
	m_RoundEndEffect = szBuf;

	LoadGUIValues();
	LoadPushCursor();
	LoadContribute();

	kLoader.SetTitle_e( "flag_ping_check" );
	m_dwFlagPingTime = kLoader.LoadInt_e( "flag_ping_time", 0 );

	kLoader.SetTitle_e( "ui_info" );
	kLoader.LoadString_e( "1point_up_emoticon", "", szBuf, MAX_PATH );
	m_PointUpEmoticon = szBuf;

	kLoader.SetFileName_e( "config/sp2_sound.ini" );
	kLoader.SetTitle_e( "sound_file_name" );
	kLoader.LoadString_e( "take_flag_sound", "", szBuf, MAX_PATH );
	m_TakeFlagSound = szBuf;
	kLoader.LoadString_e( "drop_flag_sound", "", szBuf, MAX_PATH );
	m_DropFlagSound = szBuf;

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

	CreateReturnPoint();

	if( g_BattleRoomMgr.IsBattleRoom() )
		g_BattleRoomMgr.ModeStartPlayerCount();
}

void ioFlagCaptureMode::LoadModeGuide( ioINILoader &rkLoader )
{
	rkLoader.SetTitle_e( "common_info" );

	SAFEDELETE( m_pGuideBlueArrow );
	m_pGuideBlueArrow = new ioModeGuideArrow( this );
	m_pGuideBlueArrow->Initialize( rkLoader );
	
	SAFEDELETE( m_pGuideRedArrow );
	m_pGuideRedArrow = new ioModeGuideArrow( this );
	m_pGuideRedArrow->Initialize( rkLoader );
}

void ioFlagCaptureMode::LoadMap()
{
	ioPlayMode::LoadMap();

	ioINILoader kLoader( GetModeMapINIFileName( m_ModeType, m_iModeSubNum, m_iModeMapIndex ) );
	kLoader.SetTitle_e( "team_revival" );

#ifndef SHIPPING
	LOG.PrintConsoleLog( 0, "ioFlagCaptureMode::LoadMap - m_iModeMapIndex = %d, m_iModeSubNum = %d", m_iModeMapIndex, m_iModeSubNum );
#endif

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

	kLoader.SetTitle_e( "flag_point" );

	m_BlueFlagReturnPoint.x = kLoader.LoadFloat_e( "blue_flag_point_x", 0.0f );
	m_BlueFlagReturnPoint.y = kLoader.LoadFloat_e( "blue_flag_point_y", 0.0f );
	m_BlueFlagReturnPoint.z = kLoader.LoadFloat_e( "blue_flag_point_z", 0.0f );

	m_RedFlagReturnPoint.x = kLoader.LoadFloat_e( "red_flag_point_x", 0.0f );
	m_RedFlagReturnPoint.y = kLoader.LoadFloat_e( "red_flag_point_y", 0.0f );
	m_RedFlagReturnPoint.z = kLoader.LoadFloat_e( "red_flag_point_z", 0.0f );

	if( m_BlueFlagReturnPoint.y == 0 )
		m_BlueFlagReturnPoint.y = GetMapHeightByRevial( m_BlueFlagReturnPoint.x, m_BlueFlagReturnPoint.z );
	if( m_RedFlagReturnPoint.y == 0 )
		m_RedFlagReturnPoint.y = GetMapHeightByRevial( m_RedFlagReturnPoint.x, m_RedFlagReturnPoint.z );

	m_fReturnPointRadius = kLoader.LoadFloat_e( "point_radius", 0.0f );

	char szBuf[MAX_PATH] = "";
	kLoader.LoadString_e( "point_effect_blue", "", szBuf, MAX_PATH );
	m_szReturnPointEffectBlue = szBuf;

	kLoader.LoadString_e( "point_effect_red", "", szBuf, MAX_PATH );
	m_szReturnPointEffectRed = szBuf;

	kLoader.LoadString_e( "return_effect", "", szBuf, MAX_PATH );
	m_szFlagReturnEffect = szBuf;
}

void ioFlagCaptureMode::CreateReturnPoint()
{
	if( m_dwBlueReturnPointID == -1 )
	{
		ioMapEffect *pMapEffect = CreateMapEffect( m_szReturnPointEffectBlue, m_BlueFlagReturnPoint, ioMath::UNIT_ALL );

		if( pMapEffect )
			m_dwBlueReturnPointID = pMapEffect->GetUniqueID();
	}

	if( m_dwRedReturnPointID == -1 )
	{
		ioMapEffect *pMapEffect = CreateMapEffect( m_szReturnPointEffectRed, m_RedFlagReturnPoint, ioMath::UNIT_ALL );

		if( pMapEffect )
			m_dwRedReturnPointID = pMapEffect->GetUniqueID();
	}
}

void ioFlagCaptureMode::DestroyReturnPoint()
{
#ifndef SHIPPING
	LOG.PrintConsoleLog( 0, "ioFlagCaptureMode::DestroyReturnPoint - m_dwBlueReturnPointID = %d, m_dwRedReturnPointID = %d", m_dwBlueReturnPointID, m_dwRedReturnPointID );
#endif
	if( m_dwBlueReturnPointID != -1 )
	{
		DestroyMapEffect( m_dwBlueReturnPointID );
		m_dwBlueReturnPointID = -1;
	}

	if( m_dwRedReturnPointID != -1 )
	{
		DestroyMapEffect( m_dwRedReturnPointID );
		m_dwRedReturnPointID = -1;
	}
}

bool ioFlagCaptureMode::CheckReturnPoint( ioBaseChar *pChar )
{
	if( !pChar )
		return false;

	if( m_bFlagReturnSend )
		return false;

	D3DXVECTOR3 vPos = pChar->GetWorldPosition();
	D3DXVECTOR3 vFlagPos, vDir;

	if( pChar->GetTeam() == TEAM_BLUE )
	{
		vFlagPos = m_BlueFlagReturnPoint;
	}
	else if( pChar->GetTeam() == TEAM_RED )
	{
		vFlagPos = m_RedFlagReturnPoint;
	}
	vDir = vFlagPos - vPos;
	
	float fLength = D3DXVec3Length( &vDir );

	if( fLength <= m_fReturnPointRadius )
		return true;

	return false;
}

void ioFlagCaptureMode::SendFlagReturn()
{
	ioBaseChar *pChar = GetFlagOwner();
	if( !pChar )
		return;

	D3DXVECTOR3 vPos = pChar->GetWorldPosition();

	ioWearItem *pWear = pChar->GetWear();
	if( !pWear || pWear->GetSubType() != ioWearItem::WRT_CLOAK )
		return;
	int iCreateIndex = pWear->GetItemCreateIndex();
	DWORD iItemCode = pWear->GetItemCode();

	DWORD dwFlagTime = REALGETTIME() - m_dwCaptureFlagTime;

	SP2Packet kPacket( CTPK_FLAG_POINT_IDENTIFY );
	kPacket << m_pCreator->GetRoomIndex();
	kPacket << pChar->GetCharName().c_str();
	kPacket << vPos;
	kPacket << iCreateIndex;
	kPacket << iItemCode;
	kPacket << dwFlagTime;
	kPacket << pChar->GetTeam();
	TCPNetwork::SendToServer( kPacket );

	m_bFlagReturnSend = true;
}

void ioFlagCaptureMode::LoadModeResource( ioINILoader &rkLoader )
{
	TimeScoreWnd *pTimeScoreWnd = dynamic_cast<TimeScoreWnd *>( g_GUIMgr.AddWnd( "XML/TimeScoreWnd.xml") );
	if( pTimeScoreWnd )
		pTimeScoreWnd->SetModeType( GetModeType() );

	g_ModeUIController.SetKOMsgWnd( g_GUIMgr.AddWnd( "XML/kochatmsgwnd.xml") );

	CenterInfoWnd *pCenterInfoWnd = dynamic_cast<CenterInfoWnd *>( g_GUIMgr.AddWnd("XML/CenterInfoWnd.xml") );
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
	if( pMiniSoldierWnd )
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

void ioFlagCaptureMode::ProcessMode()
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
		ProcessFlagPoint();
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

void ioFlagCaptureMode::ProcessGUI()
{
	UpdateMiniMapSpotPoint();
	UpdateTimeScoreWnd();
	UpdateWinningTeam();

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

void ioFlagCaptureMode::ProcessFlagPoint()
{
	if( GetModeState() != MS_PLAY )
		return;

	ioBaseChar *pFlagOwner = GetFlagOwner();
	if( pFlagOwner == GetOwnerChar() && CheckReturnPoint( pFlagOwner ) )
	{
		if( REALGETTIME() - m_dwFlagReturnStartTime >= m_dwFlagReturnTerm )
			SendFlagReturn();
	}
	else
	{
		m_dwFlagReturnStartTime = REALGETTIME();
	}

	SendSyncRequest();
}

void ioFlagCaptureMode::UpdateMiniMapSpotPoint()
{
	MiniMap *pMiniMap = dynamic_cast<MiniMap*>(g_GUIMgr.FindWnd( MINIMAP_WND ));
	if( !pMiniMap )	return;

	ioPlayMode::UpdateMiniMapSpotPoint();
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

		ioWearItem *pWear = pChar->GetWear();
		ioCloakWearItem *pCloak = NULL;
		if( pWear )
			pCloak = ToCloakWearItem( pWear );
		
		//if( m_szFlagCaptureChar == pChar->GetCharName() )
		if( pCloak && pCloak->GetCloakWearItemType() == ioCloakWearItem::CWT_FLAG )
			pMiniMap->AddPoint( vPos.x, vPos.z, MiniMap::SPOT_FLAG, pChar->GetTeam(), dwGapTime );
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

		ioWearItem *pWear = pOwner->GetWear();
		ioCloakWearItem *pCloak = NULL;
		if( pWear )
			pCloak = ToCloakWearItem( pWear );

		//if( m_szFlagCaptureChar == pOwner->GetCharName() )
		if( pCloak && pCloak->GetCloakWearItemType() == ioCloakWearItem::CWT_FLAG )
		{
			pMiniMap->AddPoint( vPos.x, vPos.z, MiniMap::SPOT_FLAG_ME, pOwner->GetTeam(), dwGapTime );
		}
		else
		{
			if( pOwner->IsDropZoneDownState() )
				pMiniMap->AddPoint( vPos.x, vPos.z, MiniMap::SPOT_DROP_ME, pOwner->GetTeam(), 0 );
			else
				pMiniMap->AddPoint( vPos.x, vPos.z, MiniMap::SPOT_ME, pOwner->GetTeam(), dwGapTime );
		}
	}

	ioFieldItem *pItem = m_pCreator->GetFlagFieldItem();
	if( pItem )
	{
		dwGapTime = pItem->GetEventGapTime();
		vPos = pItem->GetWorldPosition();

		pMiniMap->AddPoint( vPos.x, vPos.z, MiniMap::SPOT_FLAG_NEUTRAL, TEAM_NONE, dwGapTime );
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
}

void ioFlagCaptureMode::UpdateTimeScoreWnd()
{
	TimeScoreWnd *pTimeScoreWnd = dynamic_cast<TimeScoreWnd *>( g_GUIMgr.FindWnd( TIME_SCORE_WND ) );
	if( !pTimeScoreWnd )
		return;

	bool bRedWarning = false;
	if( m_fCurRedFlagPoint >= m_fScoreWarningRate * FLOAT100 )
		bRedWarning = true;

	bool bBlueWarning = false;
	if( m_fCurBlueFlagPoint >= m_fScoreWarningRate * FLOAT100 )
		bBlueWarning = true;

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
				g_ModeUIController.SetFlagCaptureWarning( m_fCurBlueFlagPoint, m_fCurRedFlagPoint );
			}
			else
				ProcessScoreWarningSound( false );
		}
		else
		{
			pTimeScoreWnd->UpdateSuddenDeathTime( m_dwSuddenDeathTime - dwTimeGap, m_dwSuddenDeathTime );
		}

		pTimeScoreWnd->UpdateScore( (int)m_fCurBlueFlagPoint, (int)m_fCurRedFlagPoint );
		break;
	case MS_RESULT:
		pTimeScoreWnd->UpdateTime( 0, m_dwRoundDuration );
		break;
	}
}

void ioFlagCaptureMode::UpdateWinningTeam()
{
	TimeScoreWnd *pTimeScoreWnd = dynamic_cast<TimeScoreWnd *>( g_GUIMgr.FindWnd( TIME_SCORE_WND ) );
	if( !pTimeScoreWnd )
		return;

	float fBluePoint = m_fCurBlueFlagPoint / m_fDefaultFlagPoint;
	float fRedPoint  = m_fCurRedFlagPoint / m_fDefaultFlagPoint;
	TeamType eWinningTeam = TEAM_NONE;
		
	float fGap = fabs( fBluePoint - fRedPoint );


	eWinningTeam = TEAM_NONE;
	if( m_fWinningTextRate < fGap )
	{
		if( fBluePoint < fRedPoint )
		{
			eWinningTeam = TEAM_RED;
		}
		else if( fRedPoint < fBluePoint )
		{
			eWinningTeam = TEAM_BLUE;
		}
	}
	
	pTimeScoreWnd->UpdateWinningTeam( eWinningTeam );

	if( m_ModeState != MS_PLAY )
	{
		ProcessScoreWarningSound( false );
	}
}

void ioFlagCaptureMode::UpdateRoundStateWnd()
{
	if( g_MyInfo.IsSortContribute() )
	{
		DWORD dwTimeGap = FRAMEGETTIME() - m_dwPlayStartTime;
		if( m_dwCurRoundDuration > dwTimeGap )
			ContributeRoundAllPercent( m_dwRoundDuration - (m_dwCurRoundDuration - dwTimeGap), m_dwRoundDuration );
		else
			ContributeRoundAllPercent( m_dwRoundDuration, m_dwRoundDuration );

		FlagCaptureRecord *pRecord = FindRecordCharName( g_MyInfo.GetPublicID() );
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

			int iBluePoint = (int)m_fCurBlueFlagPoint;
			int iRedPoint  = (int)m_fCurRedFlagPoint;
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

void ioFlagCaptureMode::ProcessReadyState()
{
	if( P2PNetwork::IsNetworkPlaying() )
		return;

	if( m_dwReadyStartTime + 4000 > FRAMEGETTIME() )
		return;

	SetModeState( MS_PLAY );
}

void ioFlagCaptureMode::ProcessPlayState()
{
	UpdateSoldierRevivalPenalty();
	
	ProcessFlagOwner();

	m_pCreator->NotifySpringDampUseSet( true );	
	
}

int ioFlagCaptureMode::GetTeamPlayerCount( TeamType eTeam )
{
	int iUserCnt = 0;

	int iSize = GetRecordCharCnt();
	for( int i = 0;i < iSize; i++ )
	{
		ioBaseChar *pChar = GetRecordChar( i );
		if( !pChar ) 
			continue;

		if( pChar->GetState() == CS_VIEW )
			continue;

		if( pChar->GetState() == CS_LOADING )
			continue;

		if( pChar->GetState() == CS_OBSERVER )
			continue;
		
		if( pChar->GetTeam() == eTeam )
			iUserCnt++;
	}

	return iUserCnt;
}

void ioFlagCaptureMode::ProcessResultState()
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
			//LoadWear();

			ReStartMode();

			if( m_iCurRoundCount == 1 )
			{
				UpdateUserRank();
			}
		}
	}
}

void ioFlagCaptureMode::RenderPreGUI()
{
	if( m_ModeState == MS_PLAY )
	{
		RenderPushEnableCursor();
	}
}

void ioFlagCaptureMode::RenderAfterGUI()
{
}

void ioFlagCaptureMode::AddNewRecord( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	FlagCaptureRecord *pRecord = FindRecordCharName( pChar->GetCharName() );
	if( !pRecord )
	{
		FlagCaptureRecord kRecord;
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
		pRecord->pChar = pChar;
	}
}

void ioFlagCaptureMode::RemoveRecord( const ioHashString &rkName )
{
	
	ioBaseChar* pChar = GetRecordChar( rkName );
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


void ioFlagCaptureMode::RemoveRecord( int nIndex )
{
	int iCharCnt = m_vCharRecordList.size();
	if( iCharCnt > nIndex )
	{
		m_vCharRecordList.erase( m_vCharRecordList.begin() + nIndex );
		UpdateUserRank();
	}
}


int ioFlagCaptureMode::GetRecordCharCnt() const
{
	return m_vCharRecordList.size();
}

ioBaseChar* ioFlagCaptureMode::GetRecordChar( int i )
{
	if( COMPARE( i, 0, GetRecordCharCnt() ) )
		return m_vCharRecordList[i].pChar;

	return NULL;
}

ioBaseChar* ioFlagCaptureMode::GetRecordChar( const ioHashString &rkName )
{
	FlagCaptureRecord *pRecord = FindRecordCharName( rkName );
	if( pRecord )
		return pRecord->pChar;

	return NULL;
}

ioBaseChar* ioFlagCaptureMode::GetFlagOwner()
{
	RecordList::const_iterator iter = m_vCharRecordList.begin();
	for( ; iter != m_vCharRecordList.end(); ++iter )
	{
		ModeRecord Record = *iter;
		ioBaseChar *pChar = m_pCreator->GetBaseChar( Record.szName );

		if( pChar && pChar->GetCharName() == m_szFlagCaptureChar )
		{
			return pChar;
		}
	}

	return NULL;
}

ModeRecord* ioFlagCaptureMode::FindModeRecord( int iIdx )
{
	if( COMPARE( iIdx, 0, GetRecordCharCnt() ) )
		return &m_vCharRecordList[iIdx];

	return NULL;
}

ModeRecord* ioFlagCaptureMode::FindModeRecordCharName( const ioHashString &rkName )
{
	return (ModeRecord*)FindRecordCharName( rkName );
}

FlagCaptureRecord* ioFlagCaptureMode::FindRecordCharName( const ioHashString &rkName )
{
	int iCharCnt = m_vCharRecordList.size();
	for( int i=0 ; i<iCharCnt ; i++ )
	{
		if( m_vCharRecordList[i].szName == rkName )
			return &m_vCharRecordList[i];
	}

	return NULL;
}

void ioFlagCaptureMode::SetRoundEndInfo( WinTeamType eTeam, bool bRoundSetEnd )
{
	ioPlayMode::SetRoundEndInfo( eTeam, bRoundSetEnd );

	if( bRoundSetEnd && m_pCreator )
	{
		ioBaseChar *pOwner = m_pCreator->GetOwnerChar();
		if( pOwner )
		{
			ioComplexStringPrinter kPrinter;
			g_ChatMgr.PresetChatComplexString( MSG_INFO_CHAT, kPrinter );

			TeamType eViewTeam = pOwner->GetTeam();            
			if( eTeam == WTT_DRAW )
				eViewTeam = TEAM_NONE;
			else if( g_BattleRoomMgr.IsObserverUser( pOwner->GetCharName() ) || m_pCreator->IsStealthMode() )
			{
				if( eTeam == WTT_BLUE_TEAM )
					eViewTeam = TEAM_BLUE;
				else
					eViewTeam = TEAM_RED;
			}		

			if( 0 < m_dwBlueContribute || 0 < m_dwRedContribute )
			{
				kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), "기여도 판정에 의한 " );
			}

			// 팀 표시
			if( eViewTeam == TEAM_BLUE )
			{
				kPrinter.SetTextColor( TCT_DEFAULT_BLUE );
				kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), "블루팀 " );
			}
			else if( eViewTeam == TEAM_RED )
			{
				kPrinter.SetTextColor( TCT_DEFAULT_RED );
				kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), "레드팀 " );
			}

			kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
			if( IsWinTeam( eTeam, eViewTeam ) )
				kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), "승리" );
			else if( eTeam == WTT_DRAW )
				kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), "무승부" );
			else
				kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), "패배" );

			if( 0 < m_dwBlueContribute || 0 < m_dwRedContribute )
			{
				kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), "(%d％ vs %d％)", m_dwBlueContribute, m_dwRedContribute );
			}

			g_ChatMgr.SetChatComplexString( "정보", kPrinter );
		}
	}
}

void ioFlagCaptureMode::ShowEndScoreWnd( bool bShow )
{
	if( g_GUIMgr.IsShow( BRIEFING_WND ) ) 
		return;

	if( g_BattleRoomMgr.IsBattleRoom() )
	{
		if( g_GUIMgr.IsShow( PARTY_SHAM_BATTLE_WND ) )
			return;
	}

	if( m_bRoundSetEnd ) 
		return;

	if( g_GUIMgr.IsShow( MIDDLE_JOINDELAY_WND ) ) 
		return;

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

bool ioFlagCaptureMode::GetRoundInfo( int iIndex, RoundInfo &rkInfo )
{
	if( ioPlayMode::GetRoundInfo( iIndex, rkInfo ) )
	{
		FlagCaptureRecord *pRecord = (FlagCaptureRecord*)FindModeRecord( iIndex );
		if( !pRecord )	
			return false;

		if( !pRecord->pChar ) 
			return false;

		rkInfo.m_iContribute	= pRecord->Contribute.m_iContribute;
		rkInfo.m_iContributePer = pRecord->Contribute.m_iPrevContributePer;		

		return true;
	}

	return false;
}

bool ioFlagCaptureMode::GetResultInfo( int iIndex, ResultInfo &rkInfo )
{
	if( ioPlayMode::GetResultInfo( iIndex, rkInfo ) )
	{
		FlagCaptureRecord *pRecord = (FlagCaptureRecord*)FindModeRecord( iIndex );
		if( !pRecord )	
			return false;

		if( !pRecord->pChar ) 
			return false;

		rkInfo.m_iContribute	= pRecord->Contribute.m_iContribute;
		rkInfo.m_iContributePer = pRecord->Contribute.m_iPrevContributePer;

		return true;
	}

	return false;
}

bool ioFlagCaptureMode::GetFinalInfo( int iIndex, FinalInfo &rkInfo )
{
	if( ioPlayMode::GetFinalInfo( iIndex, rkInfo ) )
	{
		FlagCaptureRecord *pRecord = (FlagCaptureRecord*)FindModeRecord( iIndex );
		if( !pRecord )
			return false;

		if( !pRecord->pChar )
			return false;

		rkInfo.m_iContribute    = pRecord->Contribute.m_iContribute;
		rkInfo.m_iContributePer = pRecord->Contribute.m_iPrevContributePer;

		return true;
	}
	return false;
}

bool ioFlagCaptureMode::ProcessTCPPacket( SP2Packet &rkPacket )
{
	if( ioPlayMode::ProcessTCPPacket( rkPacket ) )
		return true;

	switch( rkPacket.GetPacketID() )
	{
	case STPK_ROUND_END_CONTRIBUTE:
		OnRoundEndCallContribute( rkPacket );
		return true;
	case STPK_FLAG_SYNC_REQUEST:
		OnFlagCaptureSyncRequest( rkPacket );
		return true;

	case STPK_FLAG_POINT_RESULT:
		OnFlagPointResult( rkPacket );
		return true;

	case STPK_FLAG_KILL_POINT:
		OnFlagKillPoint( rkPacket );
		return true;
	}

	return false;
}


bool ioFlagCaptureMode::ProcessUDPPacket( SP2Packet &rkPacket )
{
	if( ioPlayMode::ProcessUDPPacket( rkPacket ) )
		return true;

	return false;
}

void ioFlagCaptureMode::ApplyModeInfo( SP2Packet &rkPacket )
{
	rkPacket >> m_iCurRoundCount;
	rkPacket >> m_iMaxRoundCount;

	rkPacket >> m_dwRoundDuration;

	rkPacket >> m_iBlueRoundWinCnt;
	rkPacket >> m_iRedRoundWinCnt;

	rkPacket >> m_iBluePosArray;
	rkPacket >> m_iRedPosArray;

	rkPacket >> m_fCurRedFlagPoint;
	rkPacket >> m_fCurBlueFlagPoint;
	
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

void ioFlagCaptureMode::ApplyExtraModeInfo( SP2Packet &rkPacket )
{
	rkPacket >> m_fCurRedFlagPoint;
	rkPacket >> m_fCurBlueFlagPoint;
}

void ioFlagCaptureMode::ApplyCharModeInfo( const ioHashString &rkName, SP2Packet &rkPacket, bool bDieCheck )
{
	bool bRecord;
	rkPacket >> bRecord;

	if( !bRecord ) return;

	FlagCaptureRecord *pRecord = FindRecordCharName( rkName );
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

void ioFlagCaptureMode::NotifyEntityDie( ioPlayEntity *pEntity, ioPlayEntity *pKiller, ioBaseChar *pBestAttacker,
										 float fLastRate, float fBestRate )
{	
	ioBaseChar *pDieChar = ToBaseChar( pEntity );
	ioBaseChar *pKillChar = ToBaseChar( pKiller );

	// 순서 주의 이모티콘 출력때문
	UpdateKillDeathRecord( pEntity, pKiller, pBestAttacker );
	OnNotifyCharDie( pDieChar, pKillChar, pBestAttacker, fLastRate, fBestRate );
}

void ioFlagCaptureMode::NotifyCharDropDie( ioBaseChar *pDieChar, ioBaseChar *pKillChar, ioBaseChar *pBestAttacker,
										   float fLastRate, float fBestRate )
{
	// 순서 주의 이모티콘 출력때문
	UpdateDropDieRecord( pDieChar, pKillChar, pBestAttacker );
	OnNotifyCharDie( pDieChar, pKillChar, pBestAttacker, fLastRate, fBestRate );
}

void ioFlagCaptureMode::OnNotifyCharDie( ioBaseChar *pDieChar, ioBaseChar *pKillChar, ioBaseChar *pBestAttacker,
										 float fLastRate, float fBestRate )
{
	if( !pDieChar )
	{
		LOG.PrintTimeAndLog( 0, "ioFlagCaptureMode::OnNotifyCharDie - Entity is Not Char" );
		return;
	}

	FlagCaptureRecord *pDieRecord = FindRecordCharName( pDieChar->GetCharName() );
	if( !pDieRecord )
	{
		LOG.PrintTimeAndLog( 0, "ioFlagCaptureMode::OnNotifyCharDie - %s Not Exist Char",
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

	if( pKillChar && pDieChar )
	{
		if( !pKillChar->IsOwnerChar() && !pDieChar->IsOwnerChar() )
		{
			m_pCreator->StartScreenBlind( SBT_CATCH_KILL, NULL, true );
		}
	}
	else if( pDieChar )
	{
		if( !pDieChar->IsOwnerChar() )
		{
			m_pCreator->StartScreenBlind( SBT_CATCH_KILL, NULL, true );
		}
	}

	//For EndScene
	m_LastDieChar = pDieChar->GetCharName();

	if( pKillChar )
	{
		FlagCaptureRecord *pKillRecord = FindRecordCharName( pKillChar->GetCharName() );
		if( !pKillRecord )
		{
			LOG.PrintTimeAndLog( 0, "ioFlagCaptureMode::OnNotifyCharDie - %s Not Exist Kill Char", pKillChar->GetCharName().c_str() );
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

void ioFlagCaptureMode::NotifyDropItem( ioItem *pItem, ioBaseChar *pChar )
{
	if( !pItem ) return;

	ioFieldItem *pFlag = m_pCreator->GetFlagFieldItem();
	if( !pFlag )
		return;

	if( pItem->GetName() != pFlag->GetItemName() )
		return;
	
	if( pChar )
	{
		SetMiniMapEvent( pFlag->GetWorldPosition(), MiniMap::EVENT_CROWN_DROP, pChar->GetTeam() );

		if( pChar->GetTeam() == GetOwnerTeam() )
		{
			g_SoundMgr.PlaySound( m_DropFlagSound );
		}

		if( !m_szFlagCaptureChar.IsEmpty() && m_szFlagCaptureChar != pChar->GetCharName() )
			return;

		FlagCaptureRecord *pFlagCaptureRecord = FindRecordCharName( pChar->GetCharName() );
		if( pFlagCaptureRecord )
		{
			pFlagCaptureRecord->Contribute.m_iFlagTime += FRAMEGETTIME() - m_dwCaptureFlagTime;
		}

		DropFlag( pChar->GetLastAttackerName(), pChar->GetCharName() );

		m_szFlagCaptureChar.Clear();

		g_ModeUIController.SetFlagCaptureFlagState( pChar->GetTeam(), pChar->GetCharName(), false );

		pChar->SetEnableFlagPointGauge( false );
	}
}

void ioFlagCaptureMode::NotifyPickItem( ioItem *pItem, ioBaseChar *pChar )
{
	if( !pItem || !pChar )
		return;

	ioCloakWearItem* pFlagItem = ToCloakWearItem( pItem );
	if( !pFlagItem || pFlagItem->GetCloakWearItemType() != ioCloakWearItem::CWT_FLAG )
		return;

	m_szFlagCaptureChar = pChar->GetCharName();
	
	m_dwCaptureFlagTime = FRAMEGETTIME();

	g_ModeUIController.SetFlagCaptureFlagState( pChar->GetTeam(), pChar->GetCharName(), true );

	pChar->SetEnableFlagPointGauge( true );
}

void ioFlagCaptureMode::NotifyFieldItemSupply( ioItem *pItem )
{
	if( !pItem )
		return;
	
	ioCloakWearItem* pFlagItem = ToCloakWearItem( pItem );
	if( !pFlagItem )
		return;
	
	g_ModeUIController.SetFlagCaptureNewFlag();
}

void ioFlagCaptureMode::NottifyWeaponWounded( ioBaseChar* pChar )
{
	if( !pChar )
		return;
}

void ioFlagCaptureMode::ReStartMode()
{
	D3DXVECTOR3 vStartPos;

	int iCharCnt = m_vCharRecordList.size();
	for( int i=0 ; i<iCharCnt ; i++ )
	{
		FlagCaptureRecord &rkRecord = m_vCharRecordList[i];
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

	m_dwBlueContribute = 0;
	m_dwRedContribute  = 0;

	m_dwCheckFlagPingTime = 0;

	m_dwSyncRequestTime	= 0;

	m_iCurSendIndex = 1;

	m_dwCurRevivalTime   = 0;
	m_dwCheckWarningTime = 0;
	m_bScoreWarning      = false;

	m_bFirstSelect = true;
	m_bSendSync    = false;

	m_pResultChar = NULL;

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

	SetMapAreaWeapon();

	SetMapDummy();
}

D3DXVECTOR3 ioFlagCaptureMode::GetRevivalPos( ioBaseChar *pChar )
{
	/*int iPosArray = 0;
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

	return vCenter;*/
	D3DXVECTOR3 vPos = GetStartPos( pChar->GetTeam() );
	vPos.x += (float)( rand() % 100 ) / 20.0f;
	vPos.z += (float)( rand() % 100 ) / 20.0f;
	vPos.y = m_pCreator->GetMapHeightByRevial( vPos.x, vPos.z );

	return vPos;
}

void ioFlagCaptureMode::ContributeLOG()
{
	char szLog[MAX_PATH];
	int iCharCnt = m_vCharRecordList.size();
	for( int i = 0 ; i < iCharCnt; ++i )
	{
		FlagCaptureRecord *pRecord = &m_vCharRecordList[i];
		if( pRecord->pChar )
		{
			sprintf( szLog, "%s[%d] : C%d(%d) M%d(%d) K%d(%d) D%d(%d) A%d(%d)", pRecord->szName.c_str(), (int)pRecord->pChar->GetTeam(),
				pRecord->Contribute.m_iFlagTimeRank + 1, pRecord->Contribute.m_iFlagTime,
				pRecord->Contribute.m_iFlagPointRank + 1, pRecord->Contribute.m_iFlagPoint,
				pRecord->Contribute.m_iKillRank + 1, pRecord->Contribute.m_iKill,
				pRecord->Contribute.m_iDamageRank + 1, pRecord->Contribute.m_iDamage,
				pRecord->Contribute.m_iAttackCntRank + 1, pRecord->Contribute.m_iAttackCnt );
			g_ChatMgr.SetSystemMsg( szLog );
		}		
	}
}

void ioFlagCaptureMode::ContributeConvertTeam( TeamType eTeam )
{
	if( m_vContributeTable.size() < MAX_PLAYER )
		return;

	int i = 0;
	int iCharCnt = m_vCharRecordList.size();
	int iInfoCnt = 0;
	ContributeInfoList vInfo;

	//깃발 점수 획득
	for(i = 0;i < iCharCnt;i++)
	{
		FlagCaptureRecord *pRecord = &m_vCharRecordList[i];
		if( !pRecord ) 
			continue;

		if( !pRecord->pChar ) 
			continue;

		if( pRecord->pChar->GetState() == CS_OBSERVER ) 
			continue;

		if( g_BattleRoomMgr.IsObserverUser(pRecord->pChar->GetCharName()) ) 
			continue;

		if( eTeam != TEAM_NONE )
		{
			if( pRecord->pChar && pRecord->pChar->GetTeam() != eTeam ) 
				continue;
		}

		ContributeInfo Contribute;
		Contribute.m_iRecordArray     = i;
		Contribute.m_iContributePoint = pRecord->Contribute.m_iFlagPoint;
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
			FlagCaptureRecord *pRecord = &m_vCharRecordList[iArray];

			if( !pRecord->Contribute.m_bLastResultSync )
				pRecord->Contribute.m_iContribute = (float)m_vContributeTable[vInfo[i].m_iRank] * 10.0f;

			pRecord->Contribute.m_iFlagPointRank = vInfo[i].m_iRank;

#ifndef SHIPPING
			LOG.PrintConsoleLog( 0, "ioFlagCaptureMode::ContributeConvertTeam - (Flag) iArray = %d, m_iRank = %d, m_iContribute = %d", iArray, vInfo[i].m_iRank, pRecord->Contribute.m_iContribute );
#endif
		}
	}
	vInfo.clear();

	//KO
	for(i = 0;i < iCharCnt;i++)
	{
		FlagCaptureRecord *pRecord = &m_vCharRecordList[i];
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
			FlagCaptureRecord *pRecord = &m_vCharRecordList[iArray];
			if( !pRecord->Contribute.m_bLastResultSync )
				pRecord->Contribute.m_iContribute += m_vContributeTable[vInfo[i].m_iRank];
			pRecord->Contribute.m_iKillRank = vInfo[i].m_iRank;

#ifndef SHIPPING
			LOG.PrintConsoleLog( 0, "ioFlagCaptureMode::ContributeConvertTeam - (KO) iArray = %d, m_iRank = %d, m_iContribute = %d", iArray, vInfo[i].m_iRank, pRecord->Contribute.m_iContribute );
#endif
		}
	}
	vInfo.clear();

	//데미지 & 힐
	for(i = 0;i < iCharCnt;i++)
	{
		FlagCaptureRecord *pRecord = &m_vCharRecordList[i];
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
			FlagCaptureRecord *pRecord = &m_vCharRecordList[iArray];
			if( !pRecord->Contribute.m_bLastResultSync )
				pRecord->Contribute.m_iContribute += m_vContributeTable[vInfo[i].m_iRank] * 6;
			pRecord->Contribute.m_iDamageRank = vInfo[i].m_iRank;

#ifndef SHIPPING
			LOG.PrintConsoleLog( 0, "ioFlagCaptureMode::ContributeConvertTeam - (Damage) iArray = %d, m_iRank = %d, m_iContribute = %d", iArray, vInfo[i].m_iRank, pRecord->Contribute.m_iContribute );
#endif
		}
	}
	vInfo.clear();

	//유효 타격 횟수
	for(i = 0;i < iCharCnt;i++)
	{
		FlagCaptureRecord *pRecord = &m_vCharRecordList[i];
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
			FlagCaptureRecord *pRecord = &m_vCharRecordList[iArray];
			if( !pRecord->Contribute.m_bLastResultSync )
				pRecord->Contribute.m_iContribute += m_vContributeTable[vInfo[i].m_iRank] * 2;
			pRecord->Contribute.m_iAttackCntRank = vInfo[i].m_iRank;

#ifndef SHIPPING
			LOG.PrintConsoleLog( 0, "ioFlagCaptureMode::ContributeConvertTeam - (Attack) iArray = %d, m_iRank = %d, m_iContribute = %d", iArray, vInfo[i].m_iRank, pRecord->Contribute.m_iContribute );
#endif
		}
	}
	vInfo.clear();

	//데쓰처리
	for(i = 0;i < iCharCnt;i++)
	{
		FlagCaptureRecord *pRecord = &m_vCharRecordList[i];
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
			FlagCaptureRecord *pRecord = &m_vCharRecordList[iArray];
			if( !pRecord->Contribute.m_bLastResultSync )
				pRecord->Contribute.m_iContribute += m_vContributeTable[vInfo[i].m_iRank];
			pRecord->Contribute.m_iDeathRank = vInfo[i].m_iRank;

#ifndef SHIPPING
			LOG.PrintConsoleLog( 0, "ioFlagCaptureMode::ContributeConvertTeam - (Death) iArray = %d, m_iRank = %d, m_iContribute = %d", iArray, vInfo[i].m_iRank, pRecord->Contribute.m_iContribute );
#endif
		}
	}
	vInfo.clear();
}

void ioFlagCaptureMode::ContributeConvert()
{
	ContributeConvertTeam( TEAM_NONE );        //TEAM_NONE는 전체 유저에 대한 기여도 평가
}

int ioFlagCaptureMode::ContributeMyRank( int &rRankValue1, int &rRankValue2, int &rRankValue3, int &rRankValue4 )
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

	//깃발 획득 점수
	for(i = 0;i < iCharCnt;i++)
	{
		FlagCaptureRecord *pRecord = &m_vCharRecordList[i];
		if( !pRecord ) 
			continue;

		if( !pRecord->pChar ) 
			continue;

		if( pRecord->pChar->GetState() == CS_OBSERVER ) 
			continue;

		if( g_BattleRoomMgr.IsObserverUser(pRecord->pChar->GetCharName()) ) 
			continue;

		ContributeInfo Contribute;
		Contribute.m_iRecordArray     = i;
		Contribute.m_iContributePoint = pRecord->Contribute.m_iFlagPoint;
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
	/*for(i = 0;i < iCharCnt;i++)
	{
		FlagCaptureRecord *pRecord = &m_vCharRecordList[i];
		if( !pRecord ) 
			continue;

		if( !pRecord->pChar ) 
			continue;

		if( pRecord->pChar->GetState() == CS_OBSERVER ) 
			continue;

		if( g_BattleRoomMgr.IsObserverUser(pRecord->pChar->GetCharName()) ) 
			continue;

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

	vInfo.clear();*/

	//유효 타격 횟수
	for(i = 0;i < iCharCnt;i++)
	{
		FlagCaptureRecord *pRecord = &m_vCharRecordList[i];
		if( !pRecord ) 
			continue;

		if( !pRecord->pChar ) 
			continue;

		if( pRecord->pChar->GetState() == CS_OBSERVER ) 
			continue;

		if( g_BattleRoomMgr.IsObserverUser(pRecord->pChar->GetCharName()) ) 
			continue;

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
		FlagCaptureRecord *pRecord = &m_vCharRecordList[i];
		if( !pRecord ) 
			continue;

		if( !pRecord->pChar ) 
			continue;

		if( pRecord->pChar->GetState() == CS_OBSERVER ) 
			continue;

		if( g_BattleRoomMgr.IsObserverUser( pRecord->pChar->GetCharName() ) ) 
			continue;

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

void ioFlagCaptureMode::ContributeRoundAllPercent( DWORD dwCurTime, DWORD dwTotalTime )
{
	//기여도의 100분율
	int iMaxContribute	= 0;
	int iCharCnt		= m_vCharRecordList.size();
	int iPlayCharCnt	= 0;

	int i = 0;
	for(i = 0;i < iCharCnt;i++)
	{
		FlagCaptureRecord *pRecord = &m_vCharRecordList[i];
		if( !pRecord ) 
			continue;

		if( !pRecord->pChar ) 
			continue;

		if( pRecord->pChar->GetState() == CS_OBSERVER ) 
			continue;

		if( g_BattleRoomMgr.IsObserverUser(pRecord->pChar->GetCharName()) ) 
			continue;
		
		iMaxContribute += pRecord->Contribute.m_iContribute;
		iPlayCharCnt++;
	}

	int iOwnerContribute = -1;
	if( iMaxContribute > 0 )
	{
		for(i = 0;i < iCharCnt;i++)
		{
			FlagCaptureRecord *pRecord = &m_vCharRecordList[i];
			if( !pRecord ) 
				continue;

			if( !pRecord->pChar ) 
				continue;

			if( pRecord->pChar->GetState() == CS_OBSERVER ) 
				continue;

			if( g_BattleRoomMgr.IsObserverUser(pRecord->pChar->GetCharName()) ) 
				continue;
			
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
			g_ChatMgr.SetSystemMsg( "기여도 %d 이상 돌파!", iPraise );
			g_ModeHelpMgr.ShowContributePraise( iPraise );
		}
	}
}

void ioFlagCaptureMode::DamageContribute( const ioHashString szAttacker, const ioHashString szTarget, float fDamage, DWORD dwItemCode, int iWeaponType )
{
	if( szAttacker.IsEmpty() ) 
		return;

	FlagCaptureRecord *pRecord = FindRecordCharName( szAttacker );
	if( !pRecord || !pRecord->pChar ) 
		return;
	
	ioBaseChar *pAttacker = m_pCreator->GetBaseChar( szAttacker );
	if( !pAttacker ) 
		return;

	ioBaseChar *pTarget = m_pCreator->GetBaseChar( szTarget );
	if( !pTarget ) 
		return;

	// 자신에 의한 타격 데미지 제외
	if( pAttacker->GetCharName() == pTarget->GetCharName() ) 
		return;

	float fBonusDamage = 0;
	switch( iWeaponType )
	{
	case ioWeapon::WT_BLIZZARD:
		fBonusDamage = fDamage;
		break;
	}

	float fAddDamagePoint = FLOAT1;    
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

void ioFlagCaptureMode::KillContribute( const ioHashString szKiller, const ioHashString szKilled, DWORD dwItemCode )
{
	if( szKilled.IsEmpty() ) return;

	int iKillPoint = 100;

	float fAddKillPoint = FLOAT1;
	if( IsContributeAddPointItemCode( dwItemCode ) )
		fAddKillPoint = 1.1f;

	if( szKiller.IsEmpty() || szKiller == szKilled )     //자살
	{
		FlagCaptureRecord *pRecord = FindRecordCharName( szKilled );
		if( !pRecord || !pRecord->pChar ) return;
		
		pRecord->Contribute.m_iKill -= iKillPoint;
	}
	else
	{
		FlagCaptureRecord *pKillerRecord = FindRecordCharName( szKiller );
		FlagCaptureRecord *pKilledRecord = FindRecordCharName( szKilled );
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

void ioFlagCaptureMode::AttackCntContribute( const ioHashString szAttacker, const ioHashString szTarget, DWORD dwItemCode, int iAttackValue/* = 100 */ )
{
	if( szAttacker.IsEmpty() || szTarget.IsEmpty() ) return;

	FlagCaptureRecord *pAttackerRecord = FindRecordCharName( szAttacker );
	FlagCaptureRecord *pTargetRecord   = FindRecordCharName( szTarget );

	ioBaseChar *pAttack = m_pCreator->GetBaseChar( szAttacker );
	ioBaseChar *pTarget = m_pCreator->GetBaseChar( szTarget );

	if( !pAttack || !pTarget )
		return;

	if( pAttack != pTarget )
		CheckNonePlayTime( pAttack );

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

void ioFlagCaptureMode::FlagCapturePointContribute( const ioHashString szChar, float fFlagPoint )
{
	if( szChar.IsEmpty() )
		return;

	FlagCaptureRecord *pFlagCapture = FindRecordCharName( szChar );
	if( !pFlagCapture )
		return;

	pFlagCapture->Contribute.m_iFlagPoint += (int)fFlagPoint;

	g_AwardMgr.AddFlagCapture( pFlagCapture->szName, 1 );

	ContributeConvert();
	ContributeRoundAllPercent();
	UpdateRoundInfoList();
	CheckTotalChamp();
}

void ioFlagCaptureMode::FillLastPlayRecordInfo( SP2Packet &rkPacket )
{
	int iCharCnt = m_vCharRecordList.size();
	rkPacket << iCharCnt;
	for(int i = 0;i < iCharCnt;i++)
	{
		FlagCaptureRecord *pRecord = &m_vCharRecordList[i];
		rkPacket << pRecord->pChar->GetCharName();
		rkPacket << pRecord->Contribute.m_iContribute;		
		rkPacket << pRecord->GetUniqueTotalKill();
		rkPacket << pRecord->GetUniqueTotalDeath();
		rkPacket << g_VictoriesMgr.GetVictories( pRecord->pChar->GetCharName() );

#ifndef SHIPPING
		LOG.PrintConsoleLog( 0, "ioFlagCaptureMode::FillLastPlayRecordInfo - Name = %s, Contribute = %d, Kill = %d, Death = %d", pRecord->pChar->GetCharName().c_str(), pRecord->Contribute.m_iContribute,
			pRecord->GetUniqueTotalKill(), pRecord->GetUniqueTotalDeath() );
#endif

#if defined( USE_GA )
		if( g_MyInfo.GetPublicID() == pRecord->pChar->GetCharName() )
		{
			g_HttpMng.SetKill( pRecord->GetUniqueTotalKill() );
			g_HttpMng.SetDeath( pRecord->GetUniqueTotalDeath() );
		}
#endif
	}
}

void ioFlagCaptureMode::FillPlayRecordInfo( SP2Packet &rkPacket )
{
	int iCharCnt = m_vCharRecordList.size();
	rkPacket << iCharCnt;
	for(int i = 0;i < iCharCnt;i++)
	{
		FlagCaptureRecord *pRecord = &m_vCharRecordList[i];
		rkPacket << pRecord->pChar->GetCharName();
		rkPacket << pRecord->Contribute.m_iDamage;
		rkPacket << pRecord->Contribute.m_iAttackCnt;
		rkPacket << pRecord->Contribute.m_iKill;
		rkPacket << pRecord->Contribute.m_iFlagTime;
		rkPacket << pRecord->Contribute.m_iFlagPoint;
		rkPacket << pRecord->Contribute.m_iPrevContributePer;
		rkPacket << pRecord->GetUniqueTotalKill();
		rkPacket << pRecord->GetUniqueTotalDeath();
		rkPacket << g_VictoriesMgr.GetVictories( pRecord->pChar->GetCharName() );
		rkPacket << pRecord->GetNoDeathKillCount();
	}
}

void ioFlagCaptureMode::ApplyPlayRecordInfo( SP2Packet &rkPacket )
{
	int iCharCnt;
	rkPacket >> iCharCnt;
	for(int i = 0;i < iCharCnt;i++)
	{
		ioHashString szName;
		int iDamage, iAttackCnt, iKill, iFlagTime, iFlagPoint, iPrevContributePer, iUniqueTotalKill, iUniqueTotalDeath, iVictories, iNoDeathKillCount;
		rkPacket >> szName >> iDamage >> iAttackCnt >> iKill >> iFlagTime >> iFlagPoint >> iPrevContributePer >> iUniqueTotalKill >> iUniqueTotalDeath >> iVictories >> iNoDeathKillCount;

		FlagCaptureRecord *pRecord = FindRecordCharName( szName );
		if( pRecord )
		{
			pRecord->Contribute.m_iDamage		     = iDamage;
			pRecord->Contribute.m_iAttackCnt	     = iAttackCnt;
			pRecord->Contribute.m_iKill              = iKill;
			pRecord->Contribute.m_iFlagTime		     = iFlagTime;
			pRecord->Contribute.m_iFlagPoint         = iFlagPoint;
			pRecord->Contribute.m_iPrevContributePer = iPrevContributePer;
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

void ioFlagCaptureMode::ContributeResultSync( const ioHashString &szName, int iContribute )
{
	FlagCaptureRecord *pRecord = FindRecordCharName( szName );
	if( !pRecord ) return;

	pRecord->Contribute.m_bLastResultSync = true;
	pRecord->Contribute.m_iContribute     = iContribute;
}

void ioFlagCaptureMode::OnZeroHP( SP2Packet &rkPacket )
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

void ioFlagCaptureMode::OnRoundReady( SP2Packet &rkPacket )
{
	ShowEndScoreWnd( false );
	SetBlindMode( false );

	rkPacket >> m_iCurRoundCount;
	rkPacket >> m_iBluePosArray;
	rkPacket >> m_iRedPosArray;
	rkPacket >> m_dwCurRoundDuration;

	rkPacket >> m_fCurRedFlagPoint;
	rkPacket >> m_fCurBlueFlagPoint;
	
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

void ioFlagCaptureMode::OnRoundStart( SP2Packet &rkPacket )
{
	ioPlayMode::OnRoundStart( rkPacket );

	CreateReturnPoint();
}

void ioFlagCaptureMode::OnRoundJoin( SP2Packet &rkPacket )
{
	ioHashString szJoinChar;
	int iModeState;

	rkPacket >> szJoinChar;
	rkPacket >> iModeState;

	if( szJoinChar.IsEmpty() )
		return;

	ioBaseChar *pJoinChar = m_pCreator->GetBaseChar( szJoinChar );
	if( !pJoinChar )
		return;

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
		rkPacket >> m_fCurRedFlagPoint;
		rkPacket >> m_fCurBlueFlagPoint;
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
		rkPacket >> m_fCurRedFlagPoint;
		rkPacket >> m_fCurBlueFlagPoint;
	}

	m_pCreator->IncreaseTeamCntForBalance( pJoinChar->GetTeam() );
	m_pCreator->ReCalculateDamageRate( false );
}

void ioFlagCaptureMode::OnRoundJoinView( SP2Packet &rkPacket )
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

		g_ChatMgr.SetInfomationMsg( "다음 라운드가 시작될때까지 대기해주세요." );
		g_ChatMgr.SetInfomationMsg( "방어키(S)로 시점을 변경할 수 있습니다." );

		rkPacket >> m_dwCurRoundDuration;
		rkPacket >> m_fCurRedFlagPoint;
		rkPacket >> m_fCurBlueFlagPoint;
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
		rkPacket >> m_fCurRedFlagPoint;
		rkPacket >> m_fCurBlueFlagPoint;
	}

	m_pCreator->ReCalculateDamageRate( false );
}

void ioFlagCaptureMode::OnRoundJoinObserver( SP2Packet &rkPacket )
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

		g_ChatMgr.SetInfomationMsg( "관전모드" );
		g_ChatMgr.SetInfomationMsg( "방어키(S)로 시점을 변경할 수 있습니다." );

		rkPacket >> m_dwCurRoundDuration;
		rkPacket >> m_fCurRedFlagPoint;
		rkPacket >> m_fCurBlueFlagPoint;

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
		rkPacket >> m_fCurRedFlagPoint;
		rkPacket >> m_fCurBlueFlagPoint;
	}

	m_pCreator->ReCalculateDamageRate( false );
}

void ioFlagCaptureMode::SetResultScoreWnd()
{
	RoundResultMainWnd *pResult = dynamic_cast<RoundResultMainWnd*>( g_GUIMgr.FindWnd( ROUND_RESULT_MAIN_WND ) );
	if( !pResult )
	{
		return;
	}

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

void ioFlagCaptureMode::SetTotalScoreWnd( IntVec &rvClassTypeList, IntVec &rvClassPointList, float fBlueGuildTeamBonus, float fRedGuildTeamBonus, float fBonusArray[BA_MAX] )
{
	TotalResultMainWnd *pTotalResult = dynamic_cast<TotalResultMainWnd*>(g_GUIMgr.FindWnd( TOTAL_RESULT_MAIN_WND ));
	if( !pTotalResult )	return;

	if( g_GUIMgr.IsShow( ROUND_RESULT_MAIN_WND ) )
		g_GUIMgr.HideWnd( ROUND_RESULT_MAIN_WND );

	ContributeRoundAllPercent( m_dwRoundDuration, m_dwRoundDuration );

	int i = 0;
	int iRecordCnt = GetRecordCharCnt();
	for( i = 0 ; i < iRecordCnt ; ++i )
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
		
		for(int i = 0;i < m_iCurRoundCount - 1;++i)
		{
			iHisRoundBlue -= m_vRoundHistory[ i ].iBluePoint;
			iHisRoundRed  -= m_vRoundHistory[ i ].iRedPoint;
		}
		m_vRoundHistory[ m_iCurRoundCount - 1 ].iBluePoint = max( iHisRoundBlue, 0 );
		m_vRoundHistory[ m_iCurRoundCount - 1 ].iRedPoint  = max( iHisRoundRed, 0 );
	}

	m_dwCurResultShowDuration = m_dwFinalRoundResultDuration;

	pTotalResult->SetTeamScore( GetOwnerTeam(), m_iBlueRoundWinCnt, m_iRedRoundWinCnt );
	SetTotalResultBriefingWnd();
}

void ioFlagCaptureMode::CheckEndSceneLookAt()
{
	if( m_pResultChar )
	{
		m_vEndLookAt = m_pResultChar->GetWorldPosition();
		m_szEndLookCharName = m_pResultChar->GetCharName();
		m_pResultChar->SetEndFocusEffect( GetRoundEndEffect() );
		return;
	}

	ioFieldItem *pFlagItem = m_pCreator->GetFlagFieldItem();
	if( pFlagItem )
	{
		m_vEndLookAt = pFlagItem->GetWorldPosition();
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

void ioFlagCaptureMode::ClearModeData()
{
	m_LastDieChar.Clear();
	m_LastKillChar.Clear();

	g_SoundMgr.AllSoundStop();
	g_SoundMgr.StopStream();
	m_pCreator->HidingAllFieldItem();
}

bool ioFlagCaptureMode::IsNeedRevival( ioBaseChar *pChar )
{
	return true;
}

int ioFlagCaptureMode::GetCurRank( const ioHashString& szCharName )
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

int ioFlagCaptureMode::GetContributePerChar( const ioHashString& szName )
{
	int iCharCnt = m_vCharRecordList.size();

	for(int i = 0;i < iCharCnt;i++)
	{
		FlagCaptureRecord *pRecord = &m_vCharRecordList[i];
		if( !pRecord )	continue;
		if( !pRecord->pChar ) continue;

		if( pRecord->pChar->GetCharName() == szName )
		{
			return pRecord->Contribute.m_iPrevContributePer;
		}
	}

	return 0;
}

void ioFlagCaptureMode::OnRoundEnd( SP2Packet &rkPacket )
{
	float fCurRedCrownPoint  = m_fCurRedFlagPoint;
	float fCurBlueCrownPoint = m_fCurBlueFlagPoint;
	int iWinTeam = CheckFlagPointWinTeam( fCurBlueCrownPoint, fCurRedCrownPoint );

	rkPacket >> m_iWinTeam;
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

	rkPacket >> m_fCurRedFlagPoint;
	rkPacket >> m_fCurBlueFlagPoint;

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
	for( i = 0; i < iInfoCnt; i++ )
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
			for( int k = 0; k < iKillCharCnt; ++k )
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

	SetRoundEndInfo( (WinTeamType)m_iWinTeam, bRoundSetEnd );

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
			g_AwardMgr.SetFinalResult( (WinTeamType)m_iWinTeam );		
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

	DestroyReturnPoint();
}

void ioFlagCaptureMode::DropFlag( const ioHashString szAttacker, const ioHashString szTarget )
{
	if( szAttacker.IsEmpty() || szTarget.IsEmpty() ) return;

	FlagCaptureRecord *pAttackerRecord = FindRecordCharName( szAttacker );
	FlagCaptureRecord *pTargetRecord   = FindRecordCharName( szTarget );

	if( pTargetRecord && pTargetRecord->pChar )
	{
		m_dwCheckFlagPingTime = 0;
		m_iCurSendIndex = 1;
	}

	if( pTargetRecord && pTargetRecord->pChar )
	{
		// 깃발드롭 메세지
		g_ChatMgr.SetDropFlagMsg( pTargetRecord->pChar->GetTeam(), szTarget );
	}

	ContributeConvert();
	ContributeRoundAllPercent();
	UpdateRoundInfoList();
	CheckTotalChamp();
}

void ioFlagCaptureMode::PickupFlag( const ioHashString szChar )
{
	if( szChar.IsEmpty() ) return;

	FlagCaptureRecord *pRecord = FindRecordCharName( szChar );

	if( pRecord && pRecord->pChar )
	{
		m_dwCheckFlagPingTime = 0;
		m_iCurSendIndex = 1;

		g_ChatMgr.SetPickupFlagMsg( pRecord->pChar->GetTeam(), szChar );
	}

	ContributeConvert();
	ContributeRoundAllPercent();
	UpdateRoundInfoList();
	CheckTotalChamp();
}

void ioFlagCaptureMode::ProcessModeGuide()
{
	if( !m_pGuideBlueArrow || !m_pGuideRedArrow )
		return;

	if( g_GUIMgr.IsNoneUIMode() )
	{
		m_pGuideBlueArrow->SetVisible( false );
		m_pGuideRedArrow->SetVisible( false );
	}
	else
	{
		ioBaseChar *pOwner = GetOwnerChar();
		if( !pOwner )
			return;

		TeamType eEnemy;
		TeamType eTeam = pOwner->GetTeam();

		//Red,Blue팀으로만 적판단, 그외 팀은 대상에서 제외
		switch( eTeam )
		{
		case TEAM_BLUE:
			eEnemy = TEAM_RED;
			break;
		case TEAM_RED:
			eEnemy = TEAM_BLUE;
			break;
		default:
			eEnemy = TEAM_NONE;
			break;
		}

		// NULL 타겟도 의미가 있어서 체크없이 실행
		ioGameEntity* pTeamTarget  = GetGuideTarget();
		
		switch( pOwner->GetTeam() )
		{
		case TEAM_BLUE:
			{
				m_pGuideBlueArrow->Process( pOwner, pTeamTarget, TEAM_BLUE );				
			}
			break;
		case TEAM_RED:
			{
				m_pGuideRedArrow->Process( pOwner, pTeamTarget, TEAM_RED );
			}
			break;
		default:
			{
			}
			break;
		}
	}
}

ioGameEntity* ioFlagCaptureMode::GetGuideTarget()
{
	ioBaseChar* pOwner = GetOwnerChar();
	if( !pOwner )
		return NULL;
		
	ioFieldItem* pFlag = m_pCreator->GetFlagFieldItem();
	ioBaseChar* pFlagOwner = GetRecordChar( m_szFlagCaptureChar );
	
	if( pFlagOwner )
	{
		if( pFlagOwner == pOwner )
			return NULL;

		return pFlagOwner;
	}
	else if( pFlag )
	{
		return pFlag;
	}

	return NULL;
}

void ioFlagCaptureMode::OnRoundEndCallContribute( SP2Packet &rkPacket )
{	
	int iCommand;
	rkPacket >> iCommand;

	switch( iCommand )
	{
	case DOUBLECROWN_CONTRIBUTE_END:
		{
			// 현재 기여도를 서버로 전송
			ContributeConvert();
			ContributeRoundAllPercent();

			SP2Packet kPacket( CTPK_ROUND_END_CONTRIBUTE );	
			kPacket << DOUBLECROWN_CONTRIBUTE_END;
			FillLastPlayRecordInfo( kPacket );
			TCPNetwork::SendToServer( kPacket );
		}
		break;
	case DOUBLECROWN_CONTRIBUTE_END_RESULT:
		{
			rkPacket >> m_dwBlueContribute;
			rkPacket >> m_dwRedContribute;
		}
		break;
	}
}

void ioFlagCaptureMode::OnFlagCaptureSyncRequest( SP2Packet &rkPacket )
{	
	int iRoomIndex;
	ioHashString szUserIndex;
	D3DXVECTOR3 vPos;		// 깃발 혹은 깃발 운반자 위치

	rkPacket >> iRoomIndex;
	rkPacket >> szUserIndex;
	rkPacket >> vPos;
	rkPacket >> m_fCurBlueFlagPoint;
	rkPacket >> m_fCurRedFlagPoint;

	if( !m_pCreator || m_pCreator->GetRoomIndex() != iRoomIndex )
		return;

	if( m_szFlagCaptureChar.IsEmpty() )
	{

	}
	else
	{
		ioBaseChar *pChar = GetFlagOwner();
		if( !pChar || pChar->GetCharName() != szUserIndex )
			return;
	}
}

void ioFlagCaptureMode::OnFlagPointResult( SP2Packet &rkPacket )
{
	m_bFlagReturnSend = false;

	int iCommand, iGameIndex, iTeam;
	ioHashString szUserName;
	rkPacket >> iCommand;

#ifndef SHIPPING
	LOG.PrintConsoleLog( 0, "ioFlagCaptureMode::OnFlagPointResult - iCommand = %d", iCommand );
#endif
	switch( iCommand )
	{
	case FLAG_POINT_IDENTIFY_SUCCESS:		// 성공
		{
			D3DXVECTOR3 vPos;
			int iCreateIndex, iFlagWaitTime;
			DWORD iItemCode;

			rkPacket >> iGameIndex;
			rkPacket >> szUserName;
			rkPacket >> vPos;

			ioBaseChar *pChar = GetRecordChar( szUserName );
			if( !pChar )
				return;

			float fAddPoint = 0.0f;
			if( pChar->GetTeam() == TEAM_BLUE )
				fAddPoint = m_fCurBlueFlagPoint;
			else if( pChar->GetTeam() == TEAM_RED )
				fAddPoint = m_fCurRedFlagPoint;

			rkPacket >> m_fCurBlueFlagPoint;
			rkPacket >> m_fCurRedFlagPoint;
			rkPacket >> iCreateIndex;
			rkPacket >> iItemCode;
			rkPacket >> iFlagWaitTime;

			if( pChar->GetTeam() == TEAM_BLUE )
				fAddPoint = m_fCurBlueFlagPoint - fAddPoint;
			else if( pChar->GetTeam() == TEAM_RED )
				fAddPoint = m_fCurRedFlagPoint - fAddPoint;

			ioItem *pPreItem = NULL;
			pPreItem = pChar->ReleaseItemExplicit( iItemCode, iCreateIndex, ES_WEAR );
			SAFEDELETE( pPreItem );

			m_szFlagCaptureChar.Clear();

			iFlagWaitTime = iFlagWaitTime * 1000;
			g_ModeUIController.SetFlagCapture( pChar->GetTeam(), pChar->GetCharName(), m_fCurBlueFlagPoint, m_fCurRedFlagPoint, (DWORD)iFlagWaitTime );

			FlagCapturePointContribute( pChar->GetCharName(), fAddPoint );

			pChar->SetEnableFlagPointGauge( false );

			m_pResultChar = pChar;
			 
			ioMapEffect *pMapEffect = CreateMapEffect( m_szFlagReturnEffect, vPos, ioMath::UNIT_ALL );
		}
		break;

	case FLAG_POINT_IDENTIFY_FAIL:			// 깃발 쟁탈전 모드에서 발생하는 실패
		{
			rkPacket >> iGameIndex;
			rkPacket >> szUserName;

			if( szUserName == GetOwnerName() )
			{
				ioBaseChar *pChar = GetOwnerChar();
				if( pChar )
					pChar->CheckWearItemDrop();
				/*m_pCreator->SendOwnerDamageList();
				g_App.KickToHeadQuater( EXIT_ROOM_SPEEDHACK );
				g_GUIMgr.SetReserveMsgBox( MB_UDP_FAIL_EXIT_ROOM );*/
			}
		}
		break;

	case FLAG_POINT_DISTANCE_GAP_ERROR:		// 깃발 골인 위치와 깃발의 거리가 너무 멀다
		{
			float fDistance;

			rkPacket >> szUserName;
			rkPacket >> fDistance;

			if( szUserName == GetOwnerName() )
			{
				ioBaseChar *pChar = GetOwnerChar();
				if( pChar )
					pChar->CheckWearItemDrop();
				/*m_pCreator->SendOwnerDamageList();
				g_App.KickToHeadQuater( EXIT_ROOM_SPEEDHACK );
				g_GUIMgr.SetReserveMsgBox( MB_UDP_FAIL_EXIT_ROOM );*/
			}
		}
		break;

	case FLAG_ITEM_OWNER_ERROR:				// 깃발을 골인시킨 유저가 깃발을 들고있던 유저가 아니다
		{
			ioHashString szFlaguser;

			rkPacket >> szUserName;
			rkPacket >> szFlaguser;

			if( szUserName == GetOwnerName() )
			{
				ioBaseChar *pChar = GetOwnerChar();
				if( pChar )
					pChar->CheckWearItemDrop();
				/*m_pCreator->SendOwnerDamageList();
				g_App.KickToHeadQuater( EXIT_ROOM_SPEEDHACK );
				g_GUIMgr.SetReserveMsgBox( MB_UDP_FAIL_EXIT_ROOM );*/
			}
		}
		break;

	case FLAG_ITEM_OWN_TIME_ERROR:			// 깃발을 들고있던 시간이 차이가 많이 난다
		{
			DWORD dwTime;

			rkPacket >> iGameIndex;
			rkPacket >> szUserName;
			rkPacket >> dwTime;

			if( szUserName == GetOwnerName() )
			{
				ioBaseChar *pChar = GetOwnerChar();
				if( pChar )
					pChar->CheckWearItemDrop();
				/*m_pCreator->SendOwnerDamageList();
				g_App.KickToHeadQuater( EXIT_ROOM_SPEEDHACK );
				g_GUIMgr.SetReserveMsgBox( MB_UDP_FAIL_EXIT_ROOM );*/
			}
		}
		break;

	case FLAG_MODE_ERROR:					// 모든 전투모드에서 발생할 수 있는 일반적인 에러
		{
			rkPacket >> szUserName;
			rkPacket >> iTeam;

			if( szUserName == GetOwnerName() )
			{
				ioBaseChar *pChar = GetOwnerChar();
				if( pChar )
					pChar->CheckWearItemDrop();
				/*m_pCreator->SendOwnerDamageList();
				g_App.KickToHeadQuater( EXIT_ROOM_SPEEDHACK );
				g_GUIMgr.SetReserveMsgBox( MB_UDP_FAIL_EXIT_ROOM );*/
			}
		}
		break;
	}
}

void ioFlagCaptureMode::OnFlagKillPoint( SP2Packet &rkPacket )
{
	int iRoomIndex;

	rkPacket >> iRoomIndex;
	rkPacket >> m_fCurBlueFlagPoint;
	rkPacket >> m_fCurRedFlagPoint;

	ioBaseChar *pKiller, *pDier;
	pKiller = GetRecordChar( m_LastKillChar );
	pDier = GetRecordChar( m_LastDieChar );

	if( pKiller && pDier )
	{
		g_ModeUIController.SetFlagCaptureKO( pKiller->GetTeam(), pKiller->GetCharName(), pDier->GetTeam(), pDier->GetCharName(), 
			m_fCurBlueFlagPoint, m_fCurRedFlagPoint );
	}
	else if( pDier )
	{
		g_ModeUIController.SetFlagCaptureKO( TEAM_NONE, "", pDier->GetTeam(), pDier->GetCharName(), 
			m_fCurBlueFlagPoint, m_fCurRedFlagPoint );
	}

	m_pResultChar = pKiller;
}

void ioFlagCaptureMode::FlagBadPingDrop( const ioHashString szFlagOwner )
{
	if( szFlagOwner.IsEmpty() ) 
		return;
	
	FlagCaptureRecord* pFlagRecord = FindRecordCharName( szFlagOwner );
	if( !pFlagRecord )
		return;

	if( !pFlagRecord->pChar )
		return;

	m_dwCheckFlagPingTime = 0;
	m_iCurSendIndex = 1;
	
	// 깃발드롭 메세지
	g_ChatMgr.SetBadPingFlagDropMsg( pFlagRecord->pChar->GetTeam(), szFlagOwner );

	pFlagRecord->pChar->SetEnableFlagPointGauge( false );
	
	ContributeConvert();
	ContributeRoundAllPercent();
	UpdateRoundInfoList();
	CheckTotalChamp();
}

void ioFlagCaptureMode::SendFlagPingMessage( DWORD& dwCheckTime, int& iCurSendIndex )
{
	DWORD dwCurTime = REALGETTIME();
	if( dwCheckTime != 0 )
	{
		DWORD dwSendTimeGap = dwCurTime - dwCheckTime;
		if( dwSendTimeGap < m_dwFlagPingTime )
			return;
	}

	SP2Packet kPacket( CUPK_CHECK_FLAG_PING );
	kPacket << g_MyInfo.GetPublicID();
	kPacket << g_GlobalTimer.GetGlobalTime();
	kPacket << iCurSendIndex;
	P2PNetwork::SendToServer( kPacket );

	dwCheckTime = dwCurTime;
	iCurSendIndex++;
}

void ioFlagCaptureMode::ProcessFlagOwner()
{
	ioBaseChar *pOwner = m_pCreator->GetOwnerChar();
	if( !pOwner )	
		return;

	if( m_dwFlagPingTime == 0 )
		return;

	ioBaseChar *pFlagOwner = GetRecordChar( m_szFlagCaptureChar );
	if( pFlagOwner )
	{
		SendFlagPingMessage( m_dwCheckFlagPingTime, m_iCurSendIndex );
	}
}

TeamType ioFlagCaptureMode::CheckFlagPointWinTeam( float fCurBlueCrownPoint, float fCurRedCrownPoint )
{
	TeamType ePointTeam = TEAM_NONE;

	if( fCurBlueCrownPoint > fCurRedCrownPoint && fCurRedCrownPoint == 0.0f )
		ePointTeam = TEAM_BLUE;
	else if( m_fCurRedFlagPoint > fCurBlueCrownPoint && fCurBlueCrownPoint == 0.0f )
		ePointTeam = TEAM_RED;

	return ePointTeam;
}


void ioFlagCaptureMode::SendSyncRequest( bool bMustSend /* = false */ )
{
	DWORD dwCurrTime = FRAMEGETTIME();
	bool bSend = false;
	if(  m_dwSyncRequestTime + 2000 < dwCurrTime )
		bSend = true;

	ioBaseChar *pChar = GetOwnerChar();
	if( !pChar || !m_pCreator )
		return;

	D3DXVECTOR3 vPos;
	vPos.x = 0.0f;
	vPos.y = 0.0f;
	vPos.z = 0.0f;

	if( m_szFlagCaptureChar.IsEmpty() )
	{
		ioFieldItem *pFlag = m_pCreator->GetFlagFieldItem();
		if( pFlag )
			vPos = pFlag->GetWorldPosition();
	}
	else
	{
		ioBaseChar* pFlagOwner = GetFlagOwner();
		if( pFlagOwner )
			vPos = pFlagOwner->GetWorldPosition();
	}
	
	if( bMustSend || bSend )
	{	
		m_dwSyncRequestTime = dwCurrTime;
		SP2Packet kPacket( CTPK_FLAG_SYNC_REQUEST );
		kPacket << m_pCreator->GetRoomIndex();
		kPacket << pChar->GetCharName().c_str();
		kPacket << m_szFlagCaptureChar.c_str();
		kPacket << vPos;
		TCPNetwork::SendToServer( kPacket );
	}
}

void ioFlagCaptureMode::SetModeState( ModeState eState )
{
	ioPlayMode::SetModeState( eState );
}

void ioFlagCaptureMode::ResetResultChar()
{
	m_pResultChar = NULL;
}