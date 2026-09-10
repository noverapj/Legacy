

#include "stdafx.h"

#include "ioShuffleBonusMode.h"
#include "ioShuffleRoomMgr.h"
#include "ioModeItem.h"
#include "ioShuffleStarModeItem.h"
#include "ioShuffleBuffItem.h"
#include "ioModeGuideArrow.h"
#include "ioModeItemManager.h"
#include "ioNpcChar.h"
#include "ioShuffleModeUIRender.h"

#include "GUI/BriefingWnd.h"
#include "GUI/MiniMap.h"
#include "GUI/ShuffleRankWnd.h"
#include "GUI/RoundStateMainWnd.h"
#include "GUI/TimeScoreWnd.h"
#include "GUI/MiniSoldierSelectWnd.h"
#include "GUI/TotalResultMainWnd.h"

ioShuffleBonusMode::ioShuffleBonusMode( ioPlayStage *pCreator )
: ioPlayMode( pCreator )
{
	m_ModeType = MT_SHUFFLE_BONUS;

	m_SingleTeamPosArray.clear();
	m_dwRoundTimeWarning      = 0;
	m_dwDropZoneMoveStartTime = 0;	

	m_vCharRecordList.reserve( 100 );
}

ioShuffleBonusMode::~ioShuffleBonusMode()
{
	ClearMode();
}

void ioShuffleBonusMode::LoadModeResource( ioINILoader &rkLoader )
{
	TimeScoreWnd *pTimeScoreWnd = dynamic_cast<TimeScoreWnd *>( g_GUIMgr.AddWnd( "XML/TimeScoreWnd.xml") );
	if(pTimeScoreWnd)
		pTimeScoreWnd->SetModeType( GetModeType() );

	g_ModeUIController.SetKOMsgWnd( g_GUIMgr.AddWnd( "XML/kochatmsgwnd.xml") );
	g_ModeUIController.SetCenterInfoWnd( g_GUIMgr.AddWnd( "XML/CenterInfoWnd.xml" ) );
	g_ModeUIController.SetCenterKillInfoWnd( NULL );

	MiniSoldierSelectWnd *pMiniSoldierWnd = dynamic_cast<MiniSoldierSelectWnd*>(g_GUIMgr.AddWnd( "XML/MiniSoldierSelectWnd.xml" ));
	if(pMiniSoldierWnd)
		pMiniSoldierWnd->SetPlayStage( m_pCreator );

	g_GUIMgr.AddWnd( "XML/SkillListWnd.xml" );
	g_GUIMgr.AddWnd( "XML/AccessorySkillListWnd.xml" );

	g_GUIMgr.AddWnd( "XML/shufflebonusresultwnd.xml" );
	g_GUIMgr.AddWnd( "XML/shufflecenterhelpwnd.xml" );
	g_GUIMgr.AddWnd( "XML/shufflerankwnd.xml" );

	ioPlayMode::LoadModeResource( rkLoader );
}

void ioShuffleBonusMode::LoadMap()
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

void ioShuffleBonusMode::LoadStarItemInfo( ioINILoader &rkLoader )
{
	rkLoader.SetTitle_e( "star_item" );

	m_fDropDieStarDecreaseRate = rkLoader.LoadFloat_e( "drop_die_decrease_rate", 0.50f );
	m_fDropStarDecreaseRate    = rkLoader.LoadFloat_e( "drop_decrease_rate", 0.25f );
	m_dwEnableDropStarTime     = rkLoader.LoadInt_e( "enable_shuffle_star_drop_time", 100 );
	m_dwStarGuideTime		   = rkLoader.LoadInt_e( "star_guide_time", 3000 );

	char szBuf[MAX_PATH];
	rkLoader.LoadString_e( "use_drop_effect", "", szBuf, MAX_PATH );
	m_szUserDropStarEffect = szBuf;
	rkLoader.LoadString_e( "generate_effect", "", szBuf, MAX_PATH );
	m_szStarGenerateEffect = szBuf;

	m_dwStarGenerateEffetTime = rkLoader.LoadInt_e( "generate_effect_duration", 100 );

	rkLoader.LoadString_e( "generate_ment", "", szBuf, MAX_PATH );
	m_AddMonsterMent.m_szGenerateMent = szBuf;	
	m_AddMonsterMent.m_dwGenerateMentTime = rkLoader.LoadInt_e( "generate_ment_time", 3000 );

	char *szColor = NULL;
	rkLoader.LoadString_e( "generate_ment_color", "0xffffffff", szBuf, MAX_PATH );
	m_AddMonsterMent.m_dwGenerateColor = strtoul( szBuf, &szColor, 16 );

	m_AddMonsterMent.m_fGenerateScale  = rkLoader.LoadFloat_e( "generate_ment_scale", 0.83f );
}

void ioShuffleBonusMode::LoadBuffItemInfo( ioINILoader &rkLoader )
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

void ioShuffleBonusMode::InitMode( int iMapIndex )
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

	LoadRevivalTime( kLoader );
	LoadModeResource( kLoader );
	LoadModeCamera( kLoader );
	LoadStarItemInfo( kLoader );
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

	if( g_ShuffleRoomMgr.IsShuffleRoom() )
		g_ShuffleRoomMgr.ModeStartPlayerCount();

	ClearModeItem();

	m_vStarPosMiniMap = ioMath::VEC3_ZERO;
	m_dwStarMiniMapTime = 0;
	m_dwCurrStarGenerateEffetTime = 0;
	m_dwStarGenerateEffect = -1;
}

void ioShuffleBonusMode::ReStartMode()
{
	LOG.PrintTimeAndLog( 0, "ioShuffleBonusMode::ReStartMode Is Call???" );
}

void ioShuffleBonusMode::ProcessMode()
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
		ProcessModeItem();
		g_EventMgr.Process();
		break;
	case MS_RESULT:
		ProcessResultState();
		break;
	}
}

void ioShuffleBonusMode::ProcessReadyState()
{
	if( P2PNetwork::IsNetworkPlaying() )	
		return;	

	if( m_dwReadyStartTime + 4000 > FRAMEGETTIME() )
		return;	
		
	SetModeState( MS_PLAY );
	
	m_dwDropZoneMoveStartTime = FRAMEGETTIME();
}

void ioShuffleBonusMode::ProcessPlayState()
{
	UpdateSoldierRevivalPenalty();

	m_pCreator->NotifySpringDampUseSet( true );

	ProcessRevival();	
	
	if( m_pCreator && m_dwStarGenerateEffect != -1 && m_dwCurrStarGenerateEffetTime <= FRAMEGETTIME() )
	{
		m_pCreator->DestroyMapEffect( m_dwStarGenerateEffect );
		m_dwStarGenerateEffect = -1;
	}
}

void ioShuffleBonusMode::ProcessResultState()
{
	ShuffleCenterHelpWnd* pHelpWnd = dynamic_cast<ShuffleCenterHelpWnd*>( g_GUIMgr.FindWnd( SHUFFLE_BONUS_CENTER_HELP_WND ) );
	if( pHelpWnd )
		pHelpWnd->HideWnd();

	ShuffleRankWnd* pRankWnd = dynamic_cast<ShuffleRankWnd*>( g_GUIMgr.FindWnd( SHUFFLE_RANK_WND ) );
	if( pRankWnd )
		pRankWnd->Hide();
	
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

void ioShuffleBonusMode::ProcessGUI()
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
	UpdateCharStar();
}

void ioShuffleBonusMode::ProcessRevival()
{
	DWORD dwCurTime = FRAMEGETTIME();
	
	int iCharCnt = m_vCharRecordList.size();
	for( int i=0 ; i<iCharCnt ; i++ )
	{
		ShuffleBonusRecord &rkRecord = m_vCharRecordList[i];
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

void ioShuffleBonusMode::UpdateMiniMapSpotPoint()
{
	MiniMap *pMiniMap = dynamic_cast<MiniMap*>( g_GUIMgr.FindWnd( MINIMAP_WND ) );
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

		if( ToNpcChar( pChar ) )
		{
			pMiniMap->AddPoint( vPos.x, vPos.z, MiniMap::SPOT_SHUFFFLE_MONSTER, TEAM_NONE, dwGapTime );
		}
		else
		{
			if( pChar->IsDropZoneDownState() )
				pMiniMap->AddPoint( vPos.x, vPos.z, MiniMap::SPOT_DROP_USER, TEAM_RED, 0 );
			else
				pMiniMap->AddPoint( vPos.x, vPos.z, MiniMap::SPOT_USER, TEAM_RED, dwGapTime );
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
		case MIT_SHUFFLE_STAR:
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

void ioShuffleBonusMode::UpdateRoundStateWnd()
{
	if( g_MyInfo.IsSortContribute() )
	{
		DWORD dwTimeGap = FRAMEGETTIME() - m_dwPlayStartTime;
		if( m_dwCurRoundDuration > dwTimeGap )
			ContributeRoundAllPercent( m_dwRoundDuration - (m_dwCurRoundDuration - dwTimeGap), m_dwRoundDuration );
		else
			ContributeRoundAllPercent( m_dwRoundDuration, m_dwRoundDuration );

		ShuffleBonusRecord *pRecord = FindRecordCharName( g_MyInfo.GetPublicID() );
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

			pRoundStateWnd->SetStarCountScore();
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

void ioShuffleBonusMode::UpdateDropDieRecord( ioBaseChar *pDieChar, ioBaseChar *pKillChar, ioBaseChar *pBestAttacker )
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

void ioShuffleBonusMode::UpdateKillDeathRecord( ioPlayEntity *pDieEntity, ioPlayEntity *pKiller, ioPlayEntity *pBestAttacker )
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

void ioShuffleBonusMode::UpdateTimeScoreWnd()
{
	TimeScoreWnd *pTimeScoreWnd = dynamic_cast<TimeScoreWnd *>( g_GUIMgr.FindWnd( TIME_SCORE_WND ) );
	if( !pTimeScoreWnd ) return;

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
			pTimeScoreWnd->UpdateTime( 0, m_dwRoundDuration );
		}

		if( m_dwCurRoundDuration - dwTimeGap <= m_dwRoundTimeWarning )
		{
			ProcessScoreWarningSound( true );
		}
		else
		{	
			ProcessScoreWarningSound( false );
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
	UpdateUIRank();
}

bool ioShuffleBonusMode::GetRoundInfo( int iIndex, RoundInfo &rkInfo )
{
	if( ioPlayMode::GetRoundInfo( iIndex, rkInfo ) )
	{
		ShuffleBonusRecord *pRecord = (ShuffleBonusRecord*)FindModeRecord( iIndex );
		if( !pRecord )	return false;
		if( !pRecord->pChar ) return false;

		rkInfo.m_iStarCnt = pRecord->m_iStarCount;
		return true;
	}
	return false;
}

bool ioShuffleBonusMode::GetResultInfo( int iIndex, ResultInfo &rkInfo )
{
	if( ioPlayMode::GetResultInfo( iIndex, rkInfo ) )
	{
		ShuffleBonusRecord *pRecord = (ShuffleBonusRecord*)FindModeRecord( iIndex );
		if( !pRecord )	return false;
		if( !pRecord->pChar ) return false;

		rkInfo.m_iContribute    = pRecord->Contribute.m_iContribute;
		rkInfo.m_iContributePer = pRecord->Contribute.m_iPrevContributePer;
		rkInfo.m_iStarCount		= pRecord->m_iStarCount;
		return true;
	}
	return false;
}

bool ioShuffleBonusMode::GetFinalInfo( int iIndex, FinalInfo &rkInfo )
{
	if( ioPlayMode::GetFinalInfo( iIndex, rkInfo ) )
	{
		ShuffleBonusRecord *pRecord = (ShuffleBonusRecord*)FindModeRecord( iIndex );
		if( !pRecord )	return false;
		if( !pRecord->pChar ) return false;

		rkInfo.m_iContribute	= pRecord->Contribute.m_iContribute;
		rkInfo.m_iContributePer = pRecord->Contribute.m_iPrevContributePer;
		rkInfo.m_iStarCount		= pRecord->m_iStarCountByCalcBonus;
		return true;
	}
	return false;
}

void ioShuffleBonusMode::SetRoundEndMotion()
{
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
}

void ioShuffleBonusMode::SetRoundEndVictories()
{
	int iCharCnt = GetRecordCharCnt();	
	for( int i=0; i<iCharCnt; i++ )
	{
		ShuffleBonusRecord *pRecord = &m_vCharRecordList[i];
		if( !pRecord->pChar ) continue;

		// 기여도 100%이상이 승리
		g_VictoriesMgr.SetVictories( pRecord->szName, ( pRecord->Contribute.m_iPrevContributePer >= 100 ) );
	}	
}

void ioShuffleBonusMode::SetResultScoreWnd()
{	
	// 중간 결과가 존재하지 않는다.
}

void ioShuffleBonusMode::SetTotalScoreWnd( IntVec &rvClassTypeList, IntVec &rvClassPointList, float fBlueGuildTeamBonus, float fRedGuildTeamBonus, float fBonusArray[BA_MAX] )
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
			if( g_ShuffleRoomMgr.IsShuffleRoom() )
				g_ShuffleRoomMgr.UpdateUserLevel( kInfo.m_szName, kInfo.m_iLevel );
		}
	}

	pTotalResult->SetGuildTeamBonus( fBlueGuildTeamBonus, fRedGuildTeamBonus );
	pTotalResult->SetFinalRoundPoint( rvClassTypeList, rvClassPointList, fBonusArray );
	pTotalResult->SetSingleScore();

	m_dwCurResultShowDuration = m_dwFinalRoundResultDuration;

	SetTotalResultBriefingWnd();
}

void ioShuffleBonusMode::SetResultBriefingWnd()
{
	// 없음
}

void ioShuffleBonusMode::SetTotalResultBriefingWnd()
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


			ShuffleBonusRecord* pRecord = FindRecordCharName( pOwner->GetCharName() );
			if( pRecord )
			{
				pBriefingWnd->SetShuffleStarCount( pRecord->m_iStarCount );
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

void ioShuffleBonusMode::RenderPreGUI()
{
	if( m_ModeState == MS_PLAY )
	{
		RenderPushEnableCursor();
	}
}

D3DXVECTOR3 ioShuffleBonusMode::GetRevivalPos( ioBaseChar *pChar )
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

void ioShuffleBonusMode::GetMostNearPosList( PositionDistanceSqList &rkResultList, const D3DXVECTOR3 &vTargPos )
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

void ioShuffleBonusMode::NotifyEntityDie( ioPlayEntity *pEntity, ioPlayEntity *pKiller, ioBaseChar *pBestAttacker, float fLastRate, float fBestRate )
{
	ioBaseChar *pDieChar = ToBaseChar( pEntity );
	ioBaseChar *pKillChar = ToBaseChar( pKiller );

	OnNotifyCharDie( pDieChar, pKillChar, pBestAttacker, fLastRate, fBestRate );
	UpdateKillDeathRecord( pEntity, pKiller, pBestAttacker );
}

void ioShuffleBonusMode::NotifyCharDropDie( ioBaseChar *pDieChar, ioBaseChar *pKillChar, ioBaseChar *pBestAttacker, float fLastRate, float fBestRate )
{
	OnNotifyCharDie( pDieChar, pKillChar, pBestAttacker, fLastRate, fBestRate );
	UpdateDropDieRecord( pDieChar, pKillChar, pBestAttacker );

	if( !pDieChar )
		return;

	//캐릭터 이펙트	
	ioMapEffect *pEffect = m_pCreator->CreateMapEffect( m_szUserDropStarEffect, pDieChar->GetWorldPosition(), D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 ) );	
}

void ioShuffleBonusMode::NotifyCharDrop( ioPlayEntity *pEntity, float fDropDamage )
{
	ioBaseChar *pChar = ToBaseChar( pEntity );

	if( !pChar )
		return;

	//캐릭터 이펙트	
	ioMapEffect *pEffect = m_pCreator->CreateMapEffect( m_szUserDropStarEffect, pChar->GetWorldPosition(), D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 ) );

	if( !pChar->IsNeedProcess() )
		return;
	
	SP2Packet kPacket( CTPK_SHUFFLEROOM_DROPZONE );
	kPacket << SHUFFLEROOM_DROP_USER;
	kPacket << pChar->GetCharName().c_str();	
	TCPNetwork::SendToServer( kPacket );
}

void ioShuffleBonusMode::OnNotifyCharDie( ioBaseChar *pDieChar, ioBaseChar *pKillChar, ioBaseChar *pBestAttacker, float fLastRate, float fBestRate )
{
	if( !pDieChar )
	{
		LOG.PrintTimeAndLog( 0, "ioShuffleBonusMode::OnNotifyCharDie() - DieChar is NULL" );
		return;
	}

	ShuffleBonusRecord *pDieRecord = FindRecordCharName( pDieChar->GetCharName() );
	if( !pDieRecord )
	{
		LOG.PrintTimeAndLog( 0, "ioShuffleBonusMode::OnNotifyCharDie - %s Not Exit Char",
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
}

bool ioShuffleBonusMode::ProcessTCPPacket( SP2Packet &rkPacket )
{
	if( ioPlayMode::ProcessTCPPacket( rkPacket ) )
		return true;

	switch( rkPacket.GetPacketID() )
	{
	case STPK_SHUFFLEROOM_USER_BLOW:
		OnUserBlow( rkPacket );
		return true;
	case STPK_SHUFFLEROOM_DROPZONE:
		OnDropZone( rkPacket );
		return true;		
	case STPK_SPAWN_MONSTER:
		OnSpawnMonster(rkPacket);
		return true;
	case STPK_SHUFFLE_REWARD_RESULT:
		OnShuffleBonusRewardResult( rkPacket );
		return true;
	}

	return false;
}

void ioShuffleBonusMode::AddNewRecord( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	ShuffleBonusRecord *pShuffleBonusRecord = FindRecordCharName( pChar->GetCharName() );
	if( !pShuffleBonusRecord )
	{
		ShuffleBonusRecord kRecord;
		kRecord.pChar = pChar;
		kRecord.szName= pChar->GetCharName();
		m_vCharRecordList.push_back( kRecord );

		UpdateUserRank();

		if( pChar )
		{
			if( g_ShuffleRoomMgr.IsShuffleRoom() )
				g_AwardMgr.AddNewAwardUser( pChar->GetCharName() );
		}
	}
	else
	{
		pShuffleBonusRecord->pChar = pChar;
	}
}

void ioShuffleBonusMode::RemoveRecord( const ioHashString &rkName )
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

void ioShuffleBonusMode::RemoveRecord( int nIndex )
{
	int iCharCnt = m_vCharRecordList.size();
	if( iCharCnt > nIndex )
	{
		m_vCharRecordList.erase( m_vCharRecordList.begin() + nIndex );
		UpdateUserRank();
	}
}

ioBaseChar* ioShuffleBonusMode::GetRecordChar( int i )
{
	if( COMPARE( i, 0, GetRecordCharCnt() ) )
		return m_vCharRecordList[i].pChar;

	return NULL;
}

ioBaseChar* ioShuffleBonusMode::GetRecordChar( const ioHashString &rkName )
{
	ShuffleBonusRecord *pRecord = FindRecordCharName( rkName );
	if( pRecord )
		return pRecord->pChar;

	return NULL;
}

int ioShuffleBonusMode::GetRecordCharCnt() const
{
	return m_vCharRecordList.size();
}

ModeRecord* ioShuffleBonusMode::FindModeRecord( int iIdx )
{
	if( COMPARE( iIdx, 0, GetRecordCharCnt() ) )
		return &m_vCharRecordList[iIdx];

	return NULL;
}

ModeRecord* ioShuffleBonusMode::FindModeRecordCharName( const ioHashString &rkName )
{
	return (ModeRecord*)FindRecordCharName( rkName );
}

ShuffleBonusRecord* ioShuffleBonusMode::FindRecordCharName( const ioHashString &rkName )
{
	int iCharCnt = m_vCharRecordList.size();
	for( int i=0 ; i<iCharCnt ; i++ )
	{
		if( m_vCharRecordList[i].szName == rkName )
			return &m_vCharRecordList[i];
	}

	return NULL;
}

void ioShuffleBonusMode::ApplyModeInfo( SP2Packet &rkPacket )
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

void ioShuffleBonusMode::ApplyCharModeInfo( const ioHashString &rkName, SP2Packet &rkPacket, bool bDieCheck )
{
	bool bRecord;
	rkPacket >> bRecord;

	if( !bRecord ) return;

	ShuffleBonusRecord *pRecord = FindRecordCharName( rkName );
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

		int iStarCount;
		rkPacket >> iStarCount;
		pRecord->m_iStarCount = iStarCount;

		if( rkName == g_MyInfo.GetPublicID() )
		{
			g_ShuffleRoomMgr.SetShuffleStarCnt( pRecord->m_iStarCount );			
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

		int iStarCount;
		rkPacket >> iStarCount;
	}
}

void ioShuffleBonusMode::OnRoundEnd( SP2Packet &rkPacket )
{
	ShuffleBonusRecord kFulStarChar;
	ShuffleBonusRecord kLessStarChar;

	int iCharCnt = m_vCharRecordList.size();
	for( int iChar = 0; iChar < iCharCnt; ++iChar )
	{
		ShuffleBonusRecord &rkRecord = m_vCharRecordList[iChar];

		if( !rkRecord.pChar )
			continue;
		
		//별 많은 사람
		if( kFulStarChar.m_iStarCountByCalcBonus < rkRecord.m_iStarCountByCalcBonus )
		{
			kFulStarChar = rkRecord;			
		}
		else if( kFulStarChar.m_iStarCountByCalcBonus == rkRecord.m_iStarCountByCalcBonus )
		{
			if( kFulStarChar.Contribute.m_iContribute <  rkRecord.Contribute.m_iContribute )
			{
				kFulStarChar = rkRecord;
			}
		}

		//별 적은 사람
		if( kLessStarChar.m_iStarCountByCalcBonus > rkRecord.m_iStarCountByCalcBonus )
		{
			kLessStarChar = rkRecord;
		}
		else if( kLessStarChar.m_iStarCountByCalcBonus == rkRecord.m_iStarCountByCalcBonus )
		{
			if( kLessStarChar.Contribute.m_iContribute < rkRecord.Contribute.m_iContribute )
			{
				kLessStarChar = rkRecord;
			}
		}
	}

	if( kFulStarChar.pChar )
	{
		g_AwardMgr.AddShuffleFulStar( kFulStarChar.pChar->GetCharName(), kFulStarChar.m_iStarCountByCalcBonus );
	}

	if( kLessStarChar.pChar )
	{
		g_AwardMgr.AddShuffleLessStar( kLessStarChar.pChar->GetCharName(), kLessStarChar.m_iStarCountByCalcBonus );
	}

	ioPlayMode::OnRoundEnd( rkPacket );
}

void ioShuffleBonusMode::OnFinalRoundResult( SP2Packet &rkPacket )
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

int ioShuffleBonusMode::GetRoundWinCnt( TeamType eTeamType )
{
	return 0;
}

void ioShuffleBonusMode::ContributeConvertTeam( TeamType eTeam )
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
		ShuffleBonusRecord *pRecord = &m_vCharRecordList[i];
		if( !pRecord ) continue;
		if( !pRecord->pChar ) continue;

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
			ShuffleBonusRecord *pRecord = &m_vCharRecordList[iArray];
			if( !pRecord->Contribute.m_bLastResultSync )
				pRecord->Contribute.m_iContribute = m_vContributeTable[vInfo[i].m_iRank];
			pRecord->Contribute.m_iKillRank = vInfo[i].m_iRank;
		}
	}
	vInfo.clear();

	//데미지 & 힐
	for(i = 0;i < iCharCnt;i++)
	{
		ShuffleBonusRecord *pRecord = &m_vCharRecordList[i];
		if( !pRecord ) continue;
		if( !pRecord->pChar ) continue;

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
			ShuffleBonusRecord *pRecord = &m_vCharRecordList[iArray];
			if( !pRecord->Contribute.m_bLastResultSync )
				pRecord->Contribute.m_iContribute += m_vContributeTable[vInfo[i].m_iRank] * 6;
			pRecord->Contribute.m_iDamageRank = vInfo[i].m_iRank;
		}
	}
	vInfo.clear();

	//유효 타격 횟수
	for(i = 0;i < iCharCnt;i++)
	{
		ShuffleBonusRecord *pRecord = &m_vCharRecordList[i];
		if( !pRecord ) continue;
		if( !pRecord->pChar ) continue;

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
			ShuffleBonusRecord *pRecord = &m_vCharRecordList[iArray];
			if( !pRecord->Contribute.m_bLastResultSync )
				pRecord->Contribute.m_iContribute += m_vContributeTable[vInfo[i].m_iRank] * 2;
			pRecord->Contribute.m_iAttackCntRank = vInfo[i].m_iRank;
		}
	}
	vInfo.clear();

	//데쓰처리
	for(i = 0;i < iCharCnt;i++)
	{
		ShuffleBonusRecord *pRecord = &m_vCharRecordList[i];
		if( !pRecord ) continue;
		if( !pRecord->pChar ) continue;

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
			ShuffleBonusRecord *pRecord = &m_vCharRecordList[iArray];
			if( !pRecord->Contribute.m_bLastResultSync )
				pRecord->Contribute.m_iContribute += m_vContributeTable[vInfo[i].m_iRank];

			pRecord->Contribute.m_iDeathRank = vInfo[i].m_iRank;
		}
	}
	vInfo.clear();
}

void ioShuffleBonusMode::ContributeRoundAllPercent( DWORD dwCurTime, DWORD dwTotalTime )
{
	//기여도의 100분율
	int iMaxContribute	= 0;
	int iCharCnt		= m_vCharRecordList.size();
	int iPlayCharCnt	= 0;

	int i = 0;
	for(i = 0;i < iCharCnt;i++)
	{
		ShuffleBonusRecord *pRecord = &m_vCharRecordList[i];
		if( !pRecord ) continue;
		if( !pRecord->pChar ) continue;

		iMaxContribute += pRecord->Contribute.m_iContribute;
		iPlayCharCnt++;
	}

	int iOwnerContribute = -1;
	if( iMaxContribute > 0 )
	{
		for(i = 0;i < iCharCnt;i++)
		{
			ShuffleBonusRecord *pRecord = &m_vCharRecordList[i];
			if( !pRecord ) continue;
			if( !pRecord->pChar ) continue;

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
			g_ModeHelpMgr.ShowContributePraise( iPraise );
		}
	}
}

void ioShuffleBonusMode::ContributeConvert()
{
	ContributeConvertTeam( TEAM_NONE );
}

int ioShuffleBonusMode::ContributeMyRank( int &rRankValue1, int &rRankValue2, int &rRankValue3, int &rRankValue4 )
{
	rRankValue1 = rRankValue2 = rRankValue3 = rRankValue4 = -1;

	ioBaseChar *pOwner = GetOwnerChar();

	int i = 0;
	int iCharCnt = m_vCharRecordList.size();
	int iInfoCnt = 0;
	ContributeInfoList vInfo;

	//KO
	for(i = 0;i < iCharCnt;i++)
	{
		ShuffleBonusRecord *pRecord = &m_vCharRecordList[i];
		if( !pRecord ) continue;
		if( !pRecord->pChar ) continue;

		ContributeInfo Contribute;
		Contribute.m_iRecordArray     = i;
		Contribute.m_iContributePoint = pRecord->Contribute.m_iKill;
		Contribute.m_iGradeLevel      = pRecord->pChar->GetLevel();
		Contribute.m_szName           = pRecord->szName;
		vInfo.push_back( Contribute );
	}
	std::sort( vInfo.begin(), vInfo.end(), ContributeRankSort() );
	iInfoCnt = vInfo.size();

	for( rRankValue1=0 ; rRankValue1<iInfoCnt ; ++rRankValue1 )
	{
		ContributeInfo &kContribute = vInfo[rRankValue1];
		if( kContribute.m_szName == g_MyInfo.GetPublicID() )
			break;
	}

	vInfo.clear();	

	//유효 타격 횟수
	for(i = 0;i < iCharCnt;i++)
	{
		ShuffleBonusRecord *pRecord = &m_vCharRecordList[i];
		if( !pRecord ) continue;
		if( !pRecord->pChar ) continue;

		ContributeInfo Contribute;
		Contribute.m_iRecordArray = i;
		Contribute.m_iContributePoint = pRecord->Contribute.m_iAttackCnt;
		Contribute.m_iGradeLevel      = pRecord->pChar->GetLevel();
		Contribute.m_szName           = pRecord->szName;
		vInfo.push_back( Contribute );
	}
	std::sort( vInfo.begin(), vInfo.end(), ContributeRankSort() );
	iInfoCnt = vInfo.size();

	for(rRankValue2 = 0;rRankValue2 < iInfoCnt;rRankValue2++)
	{
		ContributeInfo &kContribute = vInfo[rRankValue2];
		if( kContribute.m_szName == g_MyInfo.GetPublicID() )
			break;
	}

	vInfo.clear();	

	//데미지 & 힐
	for(i = 0;i < iCharCnt;i++)
	{
		ShuffleBonusRecord *pRecord = &m_vCharRecordList[i];
		if( !pRecord ) continue;
		if( !pRecord->pChar ) continue;

		ContributeInfo Contribute;
		Contribute.m_iRecordArray = i;
		Contribute.m_iContributePoint = pRecord->Contribute.m_iDamage;
		Contribute.m_iGradeLevel      = pRecord->pChar->GetLevel();
		Contribute.m_szName           = pRecord->szName;
		vInfo.push_back( Contribute );
	}
	std::sort( vInfo.begin(), vInfo.end(), ContributeRankSort() );
	iInfoCnt = vInfo.size();

	for(rRankValue3 = 0;rRankValue3 < iInfoCnt;rRankValue3++)
	{
		ContributeInfo &kContribute = vInfo[rRankValue3];
		if( kContribute.m_szName == g_MyInfo.GetPublicID() )
			break;
	}

	vInfo.clear();	
	return iInfoCnt;
}

void ioShuffleBonusMode::DamageContribute( const ioHashString szAttacker, const ioHashString szTarget, float fDamage, DWORD dwItemCode, int iWeaponType )
{
	if( szAttacker.IsEmpty() ) return;

	ShuffleBonusRecord *pRecord = FindRecordCharName( szAttacker );
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

void ioShuffleBonusMode::KillContribute( const ioHashString szKiller, const ioHashString szKilled, DWORD dwItemCode )
{
	if( szKilled.IsEmpty() )
		return;

	int iKillPoint = 100;

	float fAddKillPoint = FLOAT1;
	if( IsContributeAddPointItemCode( dwItemCode ) )
		fAddKillPoint = 1.1f;

    if( szKiller.IsEmpty() || szKiller == szKilled )     //자살
	{
		ShuffleBonusRecord *pRecord = FindRecordCharName( szKilled );
		if( !pRecord || !pRecord->pChar ) return;

		pRecord->Contribute.m_iKill -= iKillPoint;
		pRecord->Contribute.m_iDeath++;
	}
	else
	{
		ShuffleBonusRecord *pKillerRecord = FindRecordCharName( szKiller );
		ShuffleBonusRecord *pKilledRecord = FindRecordCharName( szKilled );
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

void ioShuffleBonusMode::AttackCntContribute( const ioHashString szAttacker, const ioHashString szTarget, DWORD dwItemCode, int iAttackValue/* = 100 */ )
{
	if( szAttacker.IsEmpty() || szTarget.IsEmpty() ) return;

	ShuffleBonusRecord *pAttackerRecord = FindRecordCharName( szAttacker );
	ShuffleBonusRecord *pTargetRecord   = FindRecordCharName( szTarget );

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

void ioShuffleBonusMode::ContributeLOG()
{
	char szLog[MAX_PATH];

	int iCharCnt = m_vCharRecordList.size();
	for( int i=0 ;i<iCharCnt; i++ )
	{
		ShuffleBonusRecord *pRecord = &m_vCharRecordList[i];
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

void ioShuffleBonusMode::FillLastPlayRecordInfo( SP2Packet &rkPacket )
{
	int iCharCnt = m_vCharRecordList.size();
	rkPacket << iCharCnt;
	for(int i = 0;i < iCharCnt;i++)
	{
		ShuffleBonusRecord *pRecord = &m_vCharRecordList[i];
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

void ioShuffleBonusMode::FillPlayRecordInfo( SP2Packet &rkPacket )
{
	int iCharCnt = m_vCharRecordList.size();
	rkPacket << iCharCnt;
	for(int i = 0;i < iCharCnt;i++)
	{
		ShuffleBonusRecord *pRecord = &m_vCharRecordList[i];
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

void ioShuffleBonusMode::ApplyPlayRecordInfo( SP2Packet &rkPacket )
{
	int iCharCnt;
	rkPacket >> iCharCnt;
	for(int i = 0;i < iCharCnt;i++)
	{
		ioHashString szName;
		int iDamage, iAttackCnt, iKill, iDeath, iPrevContributePer, iUniqueTotalKill, iUniqueTotalDeath, iVictories;
		rkPacket >> szName >> iDamage >> iAttackCnt >> iKill >> iDeath >> iPrevContributePer >> iUniqueTotalKill >> iUniqueTotalDeath >> iVictories;

		ShuffleBonusRecord *pRecord = FindRecordCharName( szName );
		if( pRecord )
		{
			pRecord->Contribute.m_iDamage			= iDamage;
			pRecord->Contribute.m_iAttackCnt		= iAttackCnt;
			pRecord->Contribute.m_iKill             = iKill;
			pRecord->Contribute.m_iDeath            = iDeath;
			pRecord->Contribute.m_iPrevContributePer= iPrevContributePer;
			pRecord->iUniqueTotalKill = iUniqueTotalKill;
			pRecord->iUniqueTotalDeath= iUniqueTotalDeath;
		}
	}

	ContributeConvert();
	ContributeRoundAllPercent();
	UpdateRoundInfoList();
	CheckTotalChamp();
}

void ioShuffleBonusMode::ContributeResultSync( const ioHashString &szName, int iContribute )
{
	ShuffleBonusRecord *pRecord = FindRecordCharName( szName );
	if( !pRecord ) return;

	pRecord->Contribute.m_bLastResultSync = true;
	pRecord->Contribute.m_iContribute     = iContribute;
}

void ioShuffleBonusMode::ShowEndScoreWnd( bool bShow )
{
}

D3DXVECTOR3 ioShuffleBonusMode::GetStartPos( TeamType eTeamType, bool bRandom )
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

void ioShuffleBonusMode::UpdateRoundInfoList()
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

	std::sort( m_RoundInfoList.begin(), m_RoundInfoList.end(), RoundInfoStarSort() );

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

int ioShuffleBonusMode::GetCurRank( const ioHashString& szCharName )
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

int ioShuffleBonusMode::GetContributePerChar( const ioHashString& szName )
{
	int iCharCnt = m_vCharRecordList.size();

	for(int i = 0;i < iCharCnt;i++)
	{
		ShuffleBonusRecord *pRecord = &m_vCharRecordList[i];
		if( !pRecord )	continue;
		if( !pRecord->pChar ) continue;

		if( pRecord->pChar->GetCharName() == szName )
		{
			return pRecord->Contribute.m_iPrevContributePer;
		}
	}

	return 0;
}

void ioShuffleBonusMode::OnCreateModeItem( SP2Packet &rkPacket )
{
	int iType;
	rkPacket >> iType;

	switch( iType )
	{
	case MIT_SHUFFLE_STAR:		
		OnCreateModeStarItem( rkPacket );
		break;
	case MIT_BUFF:		
		OnCreateModeBuffItem( rkPacket );
		break;
	}	
}

void ioShuffleBonusMode::OnCreateModeStarItem( SP2Packet &rkPacket )
{
	DWORD dwAddTime = 0;
	m_dwStarMiniMapTime = FRAMEGETTIME();

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
		m_vStarPosMiniMap = D3DXVECTOR3( fXPos, 0.0f, fZPos );

		int iAngle;
		rkPacket >> iAngle;

		float fSpeed, fPower;
		rkPacket >> fSpeed >> fPower;

		DWORD dwActivateTime;
		rkPacket >> dwActivateTime;
		dwAddTime += dwActivateTime;
				
		ioShuffleStarModeItem *pItem = ToShuffleStarModeItem( g_ModeItemManager.CreateModeItem( MIT_SHUFFLE_STAR ) );
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
		
	m_vStarPosMiniMap.y = m_pCreator->GetMapHeight( m_vStarPosMiniMap.x, m_vStarPosMiniMap.z, NULL );
	ioMapEffect *pEffect = m_pCreator->CreateMapEffect( m_szStarGenerateEffect, m_vStarPosMiniMap, D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 ) );
	if( pEffect )
	{
		m_dwStarGenerateEffect = pEffect->GetUniqueID();
		m_dwCurrStarGenerateEffetTime = FRAMEGETTIME() + m_dwStarGenerateEffetTime;

	}
}

void ioShuffleBonusMode::OnCreateModeBuffItem( SP2Packet &rkPacket )
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

void ioShuffleBonusMode::OnGetModeItem( SP2Packet &rkPacket )
{
	ioHashString szName;
	DWORD dwUserIdx, dwModeItemIndex;
	int iType;
	rkPacket >> dwUserIdx >> szName >> iType >> dwModeItemIndex;

	switch( iType )
	{
	case MIT_SHUFFLE_STAR:
		{
			int iStarCount;
			rkPacket >> iStarCount;
			OnGetModeItemByStar( dwUserIdx, iStarCount, szName );
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

void ioShuffleBonusMode::OnGetModeItemByStar( DWORD dwUserIdx, int iStarCount, const ioHashString& szName )
{
	if( dwUserIdx == g_MyInfo.GetUserIndex() )
		g_ShuffleRoomMgr.SetShuffleStarCnt( iStarCount );

	for( int i = 0; i< GetRecordCharCnt(); ++i )
	{
		ShuffleBonusRecord *pRecord = (ShuffleBonusRecord*)FindModeRecord( i );
		if( pRecord && pRecord->szName == szName )
			pRecord->m_iStarCount = iStarCount;
	}

	UpdateShuffleRankWnd();
}

void ioShuffleBonusMode::OnGetModeItemByBuff( const ioHashString& szName, DWORD dwModeItemIndex, DWORD dwCallBuff )
{
	ioBaseChar* pOwner = NULL;
	for( int i = 0; i< GetRecordCharCnt(); ++i )
	{
		ShuffleBonusRecord *pRecord = (ShuffleBonusRecord*)FindModeRecord( i );
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

void ioShuffleBonusMode::OnUserBlow( SP2Packet &rkPacket )
{
	DWORD dwUserIndex = 0;
	rkPacket >> dwUserIndex;

	ioHashString szName;
	rkPacket >> szName;
	
	int iBlowType = BTT_NONE;
	rkPacket >> iBlowType;

	D3DXVECTOR3 vPos = ioMath::VEC3_ZERO;

	rkPacket >> vPos.x;
	rkPacket >> vPos.y;
	rkPacket >> vPos.z;

	int iDropCount = 0;
	rkPacket >> iDropCount;
	
	for( int i = 0; i < iDropCount; ++i )
	{
		ioModeItem *pItem = g_ModeItemManager.CreateModeItem( MIT_SHUFFLE_STAR );
		ioShuffleStarModeItem *pShuffleStarItem = ToShuffleStarModeItem( pItem );
		if( pShuffleStarItem )
		{
			DWORD dwModeItemIdx = 0;
			int iAngle = 0;
			float fSpeed = 0.0f;
			float fPower = 0.0f;
			
			rkPacket >> dwModeItemIdx >> iAngle >> fSpeed >> fPower;				

			pShuffleStarItem->SetPlayStage( m_pCreator );			
			pShuffleStarItem->SetItemValue( dwModeItemIdx, vPos, iAngle, fSpeed, fPower, 0 );
			m_vModeItemList.push_back( pItem );
		}
	}

	int iCharCnt = m_vCharRecordList.size();
	for( int iChar = 0; iChar < iCharCnt; ++iChar )
	{
		ShuffleBonusRecord *pRecord = &m_vCharRecordList[iChar];
		if( pRecord && pRecord->szName == szName )
		{
			pRecord->m_iStarCount = max(0, pRecord->m_iStarCount - iDropCount );

			if( szName == g_MyInfo.GetPublicID() )
			{
				g_ShuffleRoomMgr.SetShuffleStarCnt( pRecord->m_iStarCount );
			}
		}
	}

	UpdateShuffleRankWnd();
}

void ioShuffleBonusMode::ProcessModeItem()
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
		ioShuffleStarModeItem *pShuffleStarItem = ToShuffleStarModeItem( pItem );
		if( !pShuffleStarItem )
			continue;

		if( pShuffleStarItem->GetPosition().y - pShuffleStarItem->GetHeightOffset() >= 0.1f )
			continue;

		dwDropVec.push_back( pShuffleStarItem->GetUniqueID() );		
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

void ioShuffleBonusMode::OnDropZone( SP2Packet &rkPacket )
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
			OnDropZoneByStar( rkPacket );
		}
		break;
	}	
}

void ioShuffleBonusMode::OnDropZoneByUser( SP2Packet &rkPacket )
{	
	int iStartCount = 0;
	ioHashString szUserName;
	rkPacket >> szUserName >> iStartCount;

	ShuffleBonusRecord *pRecord = FindRecordCharName( szUserName );
	if( pRecord )
		pRecord->m_iStarCount = iStartCount;

	if( g_MyInfo.GetPublicID() == szUserName )
		g_ShuffleRoomMgr.SetShuffleStarCnt( pRecord->m_iStarCount );
}

void ioShuffleBonusMode::OnDropZoneByStar( SP2Packet &rkPacket )
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

void ioShuffleBonusMode::OnShuffleBonusRewardResult( SP2Packet &rkPacket )
{
	int iCount = 0;
	rkPacket >> iCount;

	for( int i = 0; i < iCount; ++i )
	{		
		ioHashString szName;
		int iStarCount            = 0;
		int iStarCountByCalcBonus = 0;
		float fStarBonusPercent   = 0.0f;

		rkPacket >> szName >> iStarCount >> iStarCountByCalcBonus >> fStarBonusPercent;

		ShuffleBonusRecord* pRecord = FindRecordCharName( szName );
		if( pRecord )
		{
			pRecord->m_iStarCount            = iStarCount;
			pRecord->m_iStarCountByCalcBonus = iStarCountByCalcBonus;
			pRecord->m_fStarBonusPercent     = fStarBonusPercent;			
		}

		if( g_MyInfo.GetPublicID() == szName )
		{
			g_ShuffleRoomMgr.SetShuffleStarCnt( iStarCount );
			g_ShuffleRoomMgr.SetShuffleStarByCalcBonus( iStarCountByCalcBonus );
			g_ShuffleRoomMgr.SetShuffleBonusPercent( fStarBonusPercent );
		}
	}
}

void ioShuffleBonusMode::ProcessNpcErase()
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

void ioShuffleBonusMode::AddEraseNpc( const ioHashString &rkNpcName )
{
	NpcEraseData kEraseData;
	kEraseData.m_szEraseNpc   = rkNpcName;
	kEraseData.m_dwEraseTimer = FRAMEGETTIME();
	m_NpcEraseList.push_back( kEraseData );
}

void ioShuffleBonusMode::RemoveEraseNpc( const ioHashString &rkNpcName )
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

ioNpcChar* ioShuffleBonusMode::AddNPC( DWORD dwNpcCode, const ioHashString& rkNpcName, const ioHashString& rkSyncName, TeamType eTeam, DWORD dwStartTime, float fStartXPos, float fStartZPos, int nGrowthLvl )
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

	ShuffleCenterHelpWnd* pWnd = dynamic_cast<ShuffleCenterHelpWnd*>( g_GUIMgr.FindWnd( SHUFFLE_BONUS_CENTER_HELP_WND ) );
	if( pWnd )
	{			
		pWnd->SetCenterHelp( ShuffleCenterHelpWnd::ST_ADD_MONSTER, pNpcChar->GetMonsterMent(), pNpcChar->GetMonsterMentScale(), pNpcChar->GetMonsterMentColor(), 0, pNpcChar->GetMonsterMentTime() );
	}

	return pNpcChar;
}

void ioShuffleBonusMode::OnSpawnMonster(SP2Packet &rkPacket)
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
		AddNPC( dwNpcCode, kNpcName, kSyncName, (TeamType)nTeam, dwStartTime, fStartXPos, fStartZPos, nGrowthLvl );
	}
}

void ioShuffleBonusMode::OnMonsterInfoSync( SP2Packet &rkPacket )
{
	//중간난입 유저 동기화
	OnSpawnMonster( rkPacket );
}

void ioShuffleBonusMode::GetModeCondition( const ioHashString &szTarget, ioHashString &rkReturnName )
{
	ioHashString szNewTarget;
	int iCurrStar = -1;
	int iCharCnt = m_vCharRecordList.size();
	for( int i = 0; i< iCharCnt ; ++i )
	{
		if( iCurrStar < m_vCharRecordList[i].m_iStarCount )
		{
			szNewTarget = m_vCharRecordList[i].szName;
			iCurrStar   = m_vCharRecordList[i].m_iStarCount;
		}
	}
		
	rkReturnName = szNewTarget;	
}

void ioShuffleBonusMode::UpdateCharStar()
{
	int iCharCnt = m_vCharRecordList.size();
	for( int iChar = 0; iChar < iCharCnt; ++iChar )
	{
		ShuffleBonusRecord &rkRecord = m_vCharRecordList[iChar];

		if( !rkRecord.pChar )
			continue;
		
		ioBaseChar* pChar = rkRecord.pChar;
		ioShuffleCharStateUI* pUI = pChar->GetShuffleCharUI();

		if( !pUI )
			continue;
		
		pUI->SetStarCount( rkRecord.m_iStarCount );
	}
}

void ioShuffleBonusMode::DropShuffleStarItem( ioWeapon* pWeapon, ioBaseChar* pChar )
{

	if( !pChar || !pWeapon )
		return;

	if( !pChar->IsNeedProcess() )
		return;
	
	ShuffleBonusRecord* pRecord = FindRecordCharName( pChar->GetCharName() );
	if( !pRecord )
		return;

	if( pRecord->m_iStarCount <= 0 )
		return;

	if( pRecord->m_dwLastDropStarTime + m_dwEnableDropStarTime > FRAMEGETTIME() )
		return;

	pRecord->m_dwLastDropStarTime = FRAMEGETTIME();
	
	CharState eState = pChar->GetState();
	if( eState == CS_WOUNDED                 ||
		eState == CS_BLOW_EXTEND_WOUND       ||
		eState == CS_BLOW_WOUNDED            ||
		eState == CS_GRAPPLING_WOUNDED       ||
		eState == CS_GRAPPLING_WOUNDED_ITEM  ||
		eState == CS_GRAPPLING_WOUNDED_BUFF  ||
		eState == CS_GRAPPLING_WOUNDED_SKILL ||
		eState == CS_GRAPPLING_WOUNDED_SWING ||
		eState == CS_JUMP_GRAPPLING_WOUNDED  ||
		eState == CS_GRAPPLING_PUSHED        ||
		eState == CS_GRAPPLING_PUSHED_BUFF )
	{
		if( pChar->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CTPK_SHUFFLEROOM_USER_BLOW );
			kPacket << g_MyInfo.GetUserIndex();
			kPacket << (int)pWeapon->GetBlowType();
			kPacket << pChar->GetWorldPosition().x;
			kPacket << pChar->GetWorldPosition().y;
			kPacket << pChar->GetWorldPosition().z;
			TCPNetwork::SendToServer( kPacket );
		}
	}
}

void ioShuffleBonusMode::ProcessModeGuide()
{
	if( !m_pGuideArrow )
		return;
	
	ioBaseChar *pOwner = GetOwnerChar();
	if( !pOwner )
		return;	

	if( !g_GUIMgr.IsNoneUIMode() && COMPARE( FRAMEGETTIME(), m_dwStarMiniMapTime, m_dwStarMiniMapTime + m_dwStarGuideTime ) )
	{
		m_pGuideArrow->Process( pOwner, m_vStarPosMiniMap, TEAM_BLUE );
	}
	else
	{
		m_pGuideArrow->SetVisible( false );
	}
}

void ioShuffleBonusMode::UpdateShuffleRankWnd()
{
	ShuffleRankWnd* pWnd = dynamic_cast<ShuffleRankWnd*>( g_GUIMgr.FindWnd( SHUFFLE_RANK_WND ) );
	if( pWnd )
	{
		pWnd->ClearRank();

		for( int i = 0; i < GetRecordCharCnt(); ++i )
		{
			ShuffleBonusRecord *pRecord = (ShuffleBonusRecord*)FindModeRecord( i );			

			if( !pRecord )
				continue;

			if( ToNpcChar( pRecord->pChar ) )
				continue;

			if( pRecord->szName == g_MyInfo.GetPublicID() )
				pWnd->AddRankUser( pRecord->szName, TEAM_BLUE, pRecord->m_iStarCount );
			else
				pWnd->AddRankUser( pRecord->szName, TEAM_RED, pRecord->m_iStarCount );
		}

		pWnd->Show();
	}
}