

#include "stdafx.h"

#include "ioDoubleCrownMode.h"
#include "ioLadderTeamMgr.h"
#include "ioRankBattleManager.h"
#include "ioModeGuideArrow.h"
#include "ioShuffleRoomMgr.h"

#include "GUI/MiniMap.h"
#include "GUI/MiniSoldierSelectWnd.h"
#include "GUI/CenterInfoWnd.h"
#include "GUI/TimeScoreWnd.h"
#include "GUI/ioObserverWnd.h"
#include "GUI/TotalResultMainWnd.h"
#include "GUI/RoundStateMainWnd.h"
#include "GUI/CenterKillInfoWnd.h"

ioDoubleCrownMode::ioDoubleCrownMode( ioPlayStage *pCreator )
: ioPlayMode( pCreator )
{
	m_ModeType = MT_DOUBLE_CROWN;

	m_dwBlueCrownShowEventTime = 0;
	m_dwRedCrownShowEventTime  = 0;

	m_dwCrownShowGapTime  = 1000;
	m_dwCenterUIDelayTime = 0;
	
	m_fCurRedCrownPoint  = 0.0f;
	m_fCurBlueCrownPoint = 0.0f;
	
	m_pGuideBlueArrow = NULL;
	m_pGuideRedArrow  = NULL;

	m_dwCurProcessTickTime = 0;	

	m_bSendSync   = false;

	m_dwBlueContribute = 0;
	m_dwRedContribute  = 0;

	m_dwBlueCheckKingPingTime = 0;
	m_dwRedCheckKingPingTime  = 0;

	m_dwSyncRequestTime		  = 0;

	m_iBlueCurSendIndex = 1;
	m_iRedCurSendIndex  = 1;

	m_vCharRecordList.reserve( 100 );
}

ioDoubleCrownMode::~ioDoubleCrownMode()
{
	m_bZeroHP = false;
	ClearMode();

	SAFEDELETE( m_pGuideBlueArrow );
	SAFEDELETE( m_pGuideRedArrow );
}

void ioDoubleCrownMode::InitMode( int iMapIndex )
{
	m_iModeMapIndex = iMapIndex;

	LoadMap();
	LoadPushStructs();
	LoadMachineStruct();
	LoadObjectItems();
	LoadFuntionalObjects();
	LoadWear();
	LoadCharacters();
	LoadDisableSkillList();
	LoadStartBuffList();
	LoadModeGrowthRate();

	ioINILoader kLoader( GetModeINIFileName( m_ModeType ) );

	kLoader.SetTitle_e( "common_info" );
	m_dwRoundDuration	 = kLoader.LoadInt_e( "round_time", 300000 );
	m_dwCurRoundDuration = m_dwRoundDuration;

	m_fScoreWarningRate  = kLoader.LoadFloat_e( "score_warning_rate", FLOAT05 );
	m_dwCrownShowGapTime = kLoader.LoadInt_e( "crown_minimap_gap_time", 1000 );
	
	kLoader.SetTitle_e( "crown_point" );
	m_fDefaultCrownPoint = kLoader.LoadFloat_e( "max_crown_point", FLOAT100 );
	m_fCurRedCrownPoint  = m_fDefaultCrownPoint;
	m_fCurBlueCrownPoint = m_fDefaultCrownPoint;

	m_dwDecreaseTickTime = kLoader.LoadInt_e( "crown_decrease_time", 1000 );
	m_dwDecreaseTickTime = max( 1000, m_dwDecreaseTickTime );

	m_fDecreaseCrownPoint = kLoader.LoadFloat_e( "crown_decrease_point", FLOAT1 );
	m_fCurRedDecreaseCrownPoint  = m_fDecreaseCrownPoint;
	m_fCurBlueDecreaseCrownPoint = m_fDecreaseCrownPoint;

	m_fWinningTextRate = kLoader.LoadFloat_e( "winning_text_rate", 0.15f );

	LoadModeGuide( kLoader );
	LoadBalance( kLoader );	
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

	kLoader.SetTitle_e( "end_scene_camera" );
	m_fEndHeight = kLoader.LoadFloat_e( "end_camera_height", 0.0f );
	m_fEndDistance = kLoader.LoadFloat_e( "end_camera_distance", 0.0f );
	m_fEndFov = kLoader.LoadFloat_e( "end_camera_fov", 0.0f );
	kLoader.LoadString_e( "round_end_focus_effect", "", szBuf, MAX_PATH );
	m_RoundEndEffect = szBuf;

	LoadGUIValues();
	LoadPushCursor();
	LoadContribute();

	kLoader.SetTitle_e( "king_ping_check" );
	m_dwKingPingTime = kLoader.LoadInt_e( "king_ping_time", 0 );

	kLoader.SetTitle_e( "ui_info" );
	kLoader.LoadString_e( "1point_up_emoticon", "", szBuf, MAX_PATH );
	m_PointUpEmoticon = szBuf;

	kLoader.SetFileName_e( "config/sp2_sound.ini" );
	kLoader.SetTitle_e( "sound_file_name" );
	kLoader.LoadString_e( "take_crown_sound", "", szBuf, MAX_PATH );
	m_TakeCrownSound = szBuf;
	kLoader.LoadString_e( "drop_crown_sound", "", szBuf, MAX_PATH );
	m_DropCrownSound = szBuf;

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
}


void ioDoubleCrownMode::LoadBalance( ioINILoader &rkLoader )
{
	rkLoader.SetTitle_e( "crown_point" );
	char szBuf[MAX_PATH];
	int iCount = rkLoader.LoadInt_e( "crown_member_balance_count", 0 );

	m_MemberBalanceVec.clear();
	if( 0 < iCount )
		m_MemberBalanceVec.reserve( iCount );

	for( int i = 0; i < iCount; ++i )
	{
		wsprintf_e( szBuf, "crown_member_balance%d", i );
		float fBalance = rkLoader.LoadFloat( szBuf, i * 0.1f );
		m_MemberBalanceVec.push_back( fBalance );
	}
}

float ioDoubleCrownMode::GetMemberBalanceRate( TeamType eTeam )
{
	ioPlayStage* pStage = GetCreator();
	if( !pStage )
		return 0.0f;

	int iGap	 = 0;
	int iRedCnt  = pStage->GetRedMember();
	int iBlueCnt = pStage->GetBlueMember();

	switch( eTeam )
	{
	case TEAM_RED:
		{
			iGap = iRedCnt - iBlueCnt;
		}
		break;
	case TEAM_BLUE:
		{
			iGap = iBlueCnt - iRedCnt;
		}
		break;
	default:
		return 0.0f;
	}

	return GetMemberBalanceRateByGap( iGap );
}

float ioDoubleCrownMode::GetMemberBalanceRateByGap( int iGap )
{
	if( iGap < 0 )
		return 0;

	if( (int)m_MemberBalanceVec.size() <= iGap )
		return m_MemberBalanceVec.back();
	else
		return m_MemberBalanceVec[iGap];
}

void ioDoubleCrownMode::LoadWear()
{
	if( !m_pCreator || P2PNetwork::IsNetworkPlaying() )
		return;

	ioINILoader kLoader( GetModeMapINIFileName( m_ModeType, m_iModeSubNum, m_iModeMapIndex ) );
	kLoader.SetTitle_e( "interaction_object_list" );

	int iWearItemCnt = kLoader.LoadInt_e( "wear_item_cnt", 0 );
	ioHashStringVec vItemNameList;
	vItemNameList.reserve( iWearItemCnt );

	char szBuf[MAX_PATH], szTitle[MAX_PATH];
	for(int i = 0; i < iWearItemCnt; i++ )
	{
		memset(szTitle, 0, MAX_PATH);
		memset(szBuf, 0, MAX_PATH);

		wsprintf_e( szTitle, "wear_item%d_name", i+1 );
		kLoader.LoadString( szTitle, "", szBuf, MAX_PATH );

		vItemNameList.push_back( ioHashString(szBuf) );
	}
	
	Vector3Vec vBlueWear, vRedWear, vNoneWear;
	LoadWearPos( kLoader, vBlueWear, TEAM_BLUE );
	LoadWearPos( kLoader, vRedWear, TEAM_RED );
	LoadWearPos( kLoader, vNoneWear, TEAM_NONE );

	UINT iBlue = 0;
	UINT iRed  = 0;
	UINT iNone = 0;
	for( i = 0; i < iWearItemCnt; i++ )
	{
		ioItem *pItem = m_pCreator->CreateItem( vItemNameList[i] );
		if( !pItem ) continue;

		pItem->ReLoadProperty();

		ioFieldItem *pField = m_pCreator->CreateFieldItem( pItem );
		if( pField )
			pField->CreateItemEffect();

		switch( pItem->GetItemTeamType() )
		{
		case TEAM_BLUE:
			if( iBlue < vBlueWear.size() )
			{
				pField->SetWorldPositionByServerSide( vBlueWear[iBlue] );
				iBlue++;
			}			
			break;
		case TEAM_RED:
			if( iRed < vRedWear.size() )
			{
				pField->SetWorldPositionByServerSide( vRedWear[iRed] );
				iRed++;
			}
			break;
		default:		
			if( iNone < vNoneWear.size() )
			{
				pField->SetWorldPositionByServerSide( vNoneWear[iNone] );
				iNone++;
			}	
			break;
		}
	}
}

void ioDoubleCrownMode::LoadWearPos( ioINILoader& kLoader, Vector3Vec vWearPosList, int iTeamType )
{
	char szTitle[MAX_PATH];
	char szBuf[MAX_PATH];

	switch( iTeamType )
	{
	case TEAM_BLUE:
		wsprintf_e( szTitle, "wear_generate_blue" );
		break;
	case TEAM_RED:
		wsprintf_e( szTitle, "wear_generate_red" );
		break;
	default:
		wsprintf_e( szTitle, "wear_generate" );
		break;
	}

	int iItemPosCnt = kLoader.LoadInt( szTitle, "pos_cnt", 0 );

	vWearPosList.reserve( iItemPosCnt );
	for( int i= 0 ; i< iItemPosCnt ; i++ )
	{
		D3DXVECTOR3 vPos;
		memset(szBuf, 0, MAX_PATH);
		wsprintf_e( szBuf, "pos%d_x", i+1 );
		vPos.x = kLoader.LoadFloat( szTitle, szBuf, 0.0f );

		vPos.y = 0.0f;

		memset(szBuf, 0, MAX_PATH);
		wsprintf_e( szBuf, "pos%d_z", i+1 );
		vPos.z = kLoader.LoadFloat( szTitle, szBuf, 0.0f );
		vWearPosList.push_back( vPos );
	}

	std::random_shuffle( vWearPosList.begin(), vWearPosList.end() );
}

void ioDoubleCrownMode::LoadModeGuide( ioINILoader &rkLoader )
{
	rkLoader.SetTitle_e( "common_info" );

	SAFEDELETE( m_pGuideBlueArrow );
	m_pGuideBlueArrow = new ioModeGuideArrow( this );
	m_pGuideBlueArrow->Initialize( rkLoader );
	
	SAFEDELETE( m_pGuideRedArrow );
	m_pGuideRedArrow = new ioModeGuideArrow( this );
	m_pGuideRedArrow->Initialize( rkLoader );
}

void ioDoubleCrownMode::LoadMap()
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

void ioDoubleCrownMode::LoadModeResource( ioINILoader &rkLoader )
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

void ioDoubleCrownMode::ProcessMode()
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
		ProcessCrownPoint();
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

void ioDoubleCrownMode::ProcessGUI()
{
	UpdateMiniMapSpotPoint();
	UpdateTimeScoreWnd();
	UpdateCenterInfoWnd();
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

void ioDoubleCrownMode::ProcessCrownPoint()
{
	if( GetModeState() != MS_PLAY )
		return;

	DWORD dwTime = REALGETTIME();
	DWORD dwGapTime	= dwTime - m_dwCurProcessTickTime;
	float fQuotient	= (float)dwGapTime / (float)m_dwDecreaseTickTime;
		
	if( !GetKing( TEAM_RED ) )
	{
		float fPoint = ( m_fCurRedDecreaseCrownPoint + GetMemberBalanceRate( TEAM_RED ) ) * fQuotient;
		m_fCurRedCrownPoint -= fPoint;
		m_fCurRedCrownPoint = max( 0.0f, m_fCurRedCrownPoint );
	}

	if( !GetKing( TEAM_BLUE ) )
	{
		float fPoint = ( m_fCurBlueDecreaseCrownPoint + GetMemberBalanceRate( TEAM_BLUE ) ) * fQuotient;
		m_fCurBlueCrownPoint -= fPoint;
		m_fCurBlueCrownPoint = max( 0.0f, m_fCurBlueCrownPoint );
	}

	m_dwCurProcessTickTime = dwTime;

	//동기화 요청(10초에 한번 요청, 단 왕관이 벗겨지거나 유저가 왕관을 집었을때 시간이 갱신됨)
	SendSyncRequest();
}

void ioDoubleCrownMode::UpdateCenterInfoWnd()
{
	ioBaseChar *pOwnerChar = GetOwnerChar();
	if( !pOwnerChar ) 
		return;

	if( pOwnerChar->GetState() == CS_LOADING || pOwnerChar->GetState() == CS_READY ) 
		return;
	
	bool bRedKing = false;
	if( GetKing( TEAM_RED ) )
		bRedKing = true;

	bool bBlueKing = false;
	if( GetKing( TEAM_BLUE ) )
		bBlueKing = true;

	if( g_GUIMgr.IsShow( TIME_SCORE_WND ) && m_dwCenterUIDelayTime == 0 )
	{
		m_dwCenterUIDelayTime = FRAMEGETTIME();
	}

	if( GetModeState() != MS_PLAY )
	{
		g_ModeUIController.SetDoubleCrownGaugeRate( false, 0.0f, 0.0f, bRedKing, bBlueKing );
		m_dwCenterUIDelayTime = 0;
	}
	else
	{
		if( m_dwCenterUIDelayTime > 0 && FRAMEGETTIME() - m_dwCenterUIDelayTime > m_dwDecreaseTickTime )
		{
			float fBluePoint = m_fCurBlueCrownPoint / m_fDefaultCrownPoint;
			float fRedPoint  = m_fCurRedCrownPoint / m_fDefaultCrownPoint;

			g_ModeUIController.SetDoubleCrownGaugeRate( true, fBluePoint, fRedPoint, bBlueKing, bRedKing );
			m_dwCenterUIDelayTime = FRAMEGETTIME();
		}
	}
}

void ioDoubleCrownMode::UpdateMiniMapSpotPoint()
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

		if( pChar->IsHasCrown( ioItem::MCT_DOUBLE_CROWN ) )
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
		if( pOwner->IsHasCrown( ioItem::MCT_DOUBLE_CROWN ) )
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
		m_dwBlueCrownShowEventTime = FRAMEGETTIME() + m_dwCrownShowGapTime;
		m_dwRedCrownShowEventTime  = FRAMEGETTIME() + m_dwCrownShowGapTime;
	}

	// Crown
	ioFieldItemVec CrownVec;		
	m_pCreator->GetCrownFieldItemByAll( ioItem::MCT_DOUBLE_CROWN, CrownVec );

	for( ioFieldItemVec::iterator iter = CrownVec.begin(); iter != CrownVec.end(); ++iter )
	{
		ioFieldItem* pCrown = *iter;
		if( pCrown )
		{
			dwGapTime = pCrown->GetEventGapTime();
			vPos = pCrown->GetWorldPosition();
			switch( pCrown->GetItemTeamType() )
			{
			case TEAM_BLUE:
				{
					pMiniMap->AddPoint( vPos.x, vPos.z, MiniMap::SPOT_DOUBLECROWN_DROP_BLUE, pCrown->GetItemTeamType(), dwGapTime );

					if( m_dwBlueCrownShowEventTime > 0 && m_dwBlueCrownShowEventTime < FRAMEGETTIME() )
					{
						SetMiniMapEvent( vPos, MiniMap::EVENT_CROWN_START_POSITION, TEAM_BLUE );
						m_dwBlueCrownShowEventTime = 0;
					}
				}
				break;
			case TEAM_RED:
				{
					pMiniMap->AddPoint( vPos.x, vPos.z, MiniMap::SPOT_DOUBLECROWN_DROP_RED, pCrown->GetItemTeamType(), dwGapTime );

					if( m_dwRedCrownShowEventTime > 0 && m_dwRedCrownShowEventTime < FRAMEGETTIME() )
					{
						SetMiniMapEvent( vPos, MiniMap::EVENT_CROWN_START_POSITION, TEAM_RED );
						m_dwRedCrownShowEventTime = 0;
					}
				}
				break;
			}
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
}

void ioDoubleCrownMode::UpdateTimeScoreWnd()
{
	TimeScoreWnd *pTimeScoreWnd = dynamic_cast<TimeScoreWnd *>( g_GUIMgr.FindWnd( TIME_SCORE_WND ) );
	if( !pTimeScoreWnd )
		return;

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
		break;
	case MS_RESULT:
		pTimeScoreWnd->UpdateTime( 0, m_dwRoundDuration );
		break;
	}
}

void ioDoubleCrownMode::UpdateWinningTeam()
{
	TimeScoreWnd *pTimeScoreWnd = dynamic_cast<TimeScoreWnd *>( g_GUIMgr.FindWnd( TIME_SCORE_WND ) );
	if( !pTimeScoreWnd )
		return;

	float fBluePoint = m_fCurBlueCrownPoint / m_fDefaultCrownPoint;
	float fRedPoint  = m_fCurRedCrownPoint / m_fDefaultCrownPoint;
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
	
	if( GetOwnerChar() )
	{
		TeamType OwnerTeam = GetOwnerChar()->GetTeam();
		ioBaseChar* pKing = GetKing( OwnerTeam );
		if( eWinningTeam != TEAM_NONE && OwnerTeam != eWinningTeam && !pKing )
		{
			switch( OwnerTeam )
			{
			case TEAM_BLUE:
				if( fBluePoint < FLOAT05 )
				{
					ProcessScoreWarningSound( true );
				}
				break;
			case TEAM_RED:
				if( fRedPoint < FLOAT05 )
				{
					ProcessScoreWarningSound( true );					
				}
				break;
			}
		}
		else
		{
			ProcessScoreWarningSound( false );
		}
	}
	
	pTimeScoreWnd->UpdateWinningTeam( eWinningTeam );
	g_ModeUIController.SetDoubleCrownWinningTeam( eWinningTeam );

	if( m_ModeState != MS_PLAY )
	{
		ProcessScoreWarningSound( false );
	}
}

void ioDoubleCrownMode::UpdateRoundStateWnd()
{
	if( g_MyInfo.IsSortContribute() )
	{
		DWORD dwTimeGap = FRAMEGETTIME() - m_dwPlayStartTime;
		if( m_dwCurRoundDuration > dwTimeGap )
			ContributeRoundAllPercent( m_dwRoundDuration - (m_dwCurRoundDuration - dwTimeGap), m_dwRoundDuration );
		else
			ContributeRoundAllPercent( m_dwRoundDuration, m_dwRoundDuration );

		DoubleCrownRecord *pRecord = FindRecordCharName( g_MyInfo.GetPublicID() );
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

			int iBluePoint = (int)m_fCurBlueCrownPoint;
			int iRedPoint  = (int)m_fCurRedCrownPoint;
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

void ioDoubleCrownMode::ProcessReadyState()
{
	if( P2PNetwork::IsNetworkPlaying() )
		return;

	if( m_dwReadyStartTime + 4000 > FRAMEGETTIME() )
		return;

	SetModeState( MS_PLAY );
}

void ioDoubleCrownMode::ProcessPlayState()
{
	UpdateSoldierRevivalPenalty();
	
	CrownHoldTime();
	ProcessKing();

	m_pCreator->NotifySpringDampUseSet( true );	
	
}

int ioDoubleCrownMode::GetTeamPlayerCount( TeamType eTeam )
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

void ioDoubleCrownMode::ProcessResultState()
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
			LoadWear();

			ReStartMode();

			if( m_iCurRoundCount == 1 )
			{
				UpdateUserRank();
			}
		}
	}
}

void ioDoubleCrownMode::RenderPreGUI()
{
	if( m_ModeState == MS_PLAY )
	{
		RenderPushEnableCursor();
	}
}

void ioDoubleCrownMode::RenderAfterGUI()
{
}

void ioDoubleCrownMode::AddNewRecord( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	DoubleCrownRecord *pRecord = FindRecordCharName( pChar->GetCharName() );
	if( !pRecord )
	{
		DoubleCrownRecord kRecord;
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

void ioDoubleCrownMode::RemoveRecord( const ioHashString &rkName )
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


void ioDoubleCrownMode::RemoveRecord( int nIndex )
{
	int iCharCnt = m_vCharRecordList.size();
	if( iCharCnt > nIndex )
	{
		m_vCharRecordList.erase( m_vCharRecordList.begin() + nIndex );
		UpdateUserRank();
	}
}


int ioDoubleCrownMode::GetRecordCharCnt() const
{
	return m_vCharRecordList.size();
}

ioBaseChar* ioDoubleCrownMode::GetRecordChar( int i )
{
	if( COMPARE( i, 0, GetRecordCharCnt() ) )
		return m_vCharRecordList[i].pChar;

	return NULL;
}

ioBaseChar* ioDoubleCrownMode::GetRecordChar( const ioHashString &rkName )
{
	DoubleCrownRecord *pRecord = FindRecordCharName( rkName );
	if( pRecord )
		return pRecord->pChar;

	return NULL;
}

ioBaseChar* ioDoubleCrownMode::GetKing( TeamType eTeam )
{
	RecordList::const_iterator iter = m_vCharRecordList.begin();
	for( ; iter != m_vCharRecordList.end(); ++iter )
	{
		ModeRecord Record = *iter;
		ioBaseChar *pChar = m_pCreator->GetBaseChar( Record.szName );

		if( pChar && pChar->IsHasCrown( ioItem::MCT_DOUBLE_CROWN ) && pChar->GetTeam() == eTeam )
		{
			return pChar;
		}
	}

	return NULL;
}

ModeRecord* ioDoubleCrownMode::FindModeRecord( int iIdx )
{
	if( COMPARE( iIdx, 0, GetRecordCharCnt() ) )
		return &m_vCharRecordList[iIdx];

	return NULL;
}

ModeRecord* ioDoubleCrownMode::FindModeRecordCharName( const ioHashString &rkName )
{
	return (ModeRecord*)FindRecordCharName( rkName );
}

DoubleCrownRecord* ioDoubleCrownMode::FindRecordCharName( const ioHashString &rkName )
{
	int iCharCnt = m_vCharRecordList.size();
	for( int i=0 ; i<iCharCnt ; i++ )
	{
		if( m_vCharRecordList[i].szName == rkName )
			return &m_vCharRecordList[i];
	}

	return NULL;
}

bool ioDoubleCrownMode::isKingByTeam( TeamType eType )
{
	RecordList::const_iterator iter = m_vCharRecordList.begin();
	for( ; iter != m_vCharRecordList.end(); ++iter )
	{
		ModeRecord Record = *iter;
		ioBaseChar *pChar = m_pCreator->GetBaseChar( Record.szName );

		if( pChar && pChar->GetTeam() == eType && pChar->IsHasCrown( ioItem::MCT_DOUBLE_CROWN ) )
			return true;
	}

	return false;
}

void ioDoubleCrownMode::SetRoundEndInfo( WinTeamType eTeam, bool bRoundSetEnd )
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

void ioDoubleCrownMode::ShowEndScoreWnd( bool bShow )
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

bool ioDoubleCrownMode::GetRoundInfo( int iIndex, RoundInfo &rkInfo )
{
	if( ioPlayMode::GetRoundInfo( iIndex, rkInfo ) )
	{
		DoubleCrownRecord *pRecord = (DoubleCrownRecord*)FindModeRecord( iIndex );
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

bool ioDoubleCrownMode::GetResultInfo( int iIndex, ResultInfo &rkInfo )
{
	if( ioPlayMode::GetResultInfo( iIndex, rkInfo ) )
	{
		DoubleCrownRecord *pRecord = (DoubleCrownRecord*)FindModeRecord( iIndex );
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

bool ioDoubleCrownMode::GetFinalInfo( int iIndex, FinalInfo &rkInfo )
{
	if( ioPlayMode::GetFinalInfo( iIndex, rkInfo ) )
	{
		DoubleCrownRecord *pRecord = (DoubleCrownRecord*)FindModeRecord( iIndex );
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

bool ioDoubleCrownMode::ProcessTCPPacket( SP2Packet &rkPacket )
{
	if( ioPlayMode::ProcessTCPPacket( rkPacket ) )
		return true;

	switch( rkPacket.GetPacketID() )
	{
	case STPK_KING_TAKE:
		OnKingTake( rkPacket );
		return true;
	case STPK_ROUND_END_CONTRIBUTE:
		OnRoundEndCallContribute( rkPacket );
		return true;
	case STPK_DOUBLE_CROWN_SYNC_REQUEST:
		OnDoubleCrownSyncRequest( rkPacket );
		return true;
	}

	return false;
}


bool ioDoubleCrownMode::ProcessUDPPacket( SP2Packet &rkPacket )
{
	if( ioPlayMode::ProcessUDPPacket( rkPacket ) )
		return true;

	return false;
}

void ioDoubleCrownMode::ApplyModeInfo( SP2Packet &rkPacket )
{
	rkPacket >> m_iCurRoundCount;
	rkPacket >> m_iMaxRoundCount;

	rkPacket >> m_dwRoundDuration;

	rkPacket >> m_iBlueRoundWinCnt;
	rkPacket >> m_iRedRoundWinCnt;

	rkPacket >> m_iBluePosArray;
	rkPacket >> m_iRedPosArray;

	rkPacket >> m_fCurRedCrownPoint;
	rkPacket >> m_fCurBlueCrownPoint;
	rkPacket >> m_fCurRedDecreaseCrownPoint;
	rkPacket >> m_fCurBlueDecreaseCrownPoint;
	
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

void ioDoubleCrownMode::ApplyExtraModeInfo( SP2Packet &rkPacket )
{
	rkPacket >> m_fCurRedCrownPoint;
	rkPacket >> m_fCurBlueCrownPoint;
	rkPacket >> m_fCurRedDecreaseCrownPoint;
	rkPacket >> m_fCurBlueDecreaseCrownPoint;	
}

void ioDoubleCrownMode::ApplyCharModeInfo( const ioHashString &rkName, SP2Packet &rkPacket, bool bDieCheck )
{
	bool bRecord;
	rkPacket >> bRecord;

	if( !bRecord ) return;

	DoubleCrownRecord *pRecord = FindRecordCharName( rkName );
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

void ioDoubleCrownMode::NotifyEntityDie( ioPlayEntity *pEntity, ioPlayEntity *pKiller, ioBaseChar *pBestAttacker,
										 float fLastRate, float fBestRate )
{	
	ioBaseChar *pDieChar = ToBaseChar( pEntity );
	ioBaseChar *pKillChar = ToBaseChar( pKiller );

	// 순서 주의 이모티콘 출력때문
	UpdateKillDeathRecord( pEntity, pKiller, pBestAttacker );
	OnNotifyCharDie( pDieChar, pKillChar, pBestAttacker, fLastRate, fBestRate );
}

void ioDoubleCrownMode::NotifyCharDropDie( ioBaseChar *pDieChar, ioBaseChar *pKillChar, ioBaseChar *pBestAttacker,
										   float fLastRate, float fBestRate )
{
	// 순서 주의 이모티콘 출력때문
	UpdateDropDieRecord( pDieChar, pKillChar, pBestAttacker );
	OnNotifyCharDie( pDieChar, pKillChar, pBestAttacker, fLastRate, fBestRate );
}

void ioDoubleCrownMode::OnNotifyCharDie( ioBaseChar *pDieChar, ioBaseChar *pKillChar, ioBaseChar *pBestAttacker,
										 float fLastRate, float fBestRate )
{
	if( !pDieChar )
	{
		LOG.PrintTimeAndLog( 0, "ioDoubleCrownMode::OnNotifyCharDie - Entity is Not Char" );
		return;
	}

	DoubleCrownRecord *pDieRecord = FindRecordCharName( pDieChar->GetCharName() );
	if( !pDieRecord )
	{
		LOG.PrintTimeAndLog( 0, "ioDoubleCrownMode::OnNotifyCharDie - %s Not Exist Char",
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

	//For EndScene
	m_LastDieChar = pDieChar->GetCharName();

	if( pKillChar )
	{
		DoubleCrownRecord *pKillRecord = FindRecordCharName( pKillChar->GetCharName() );
		if( !pKillRecord )
		{
			LOG.PrintTimeAndLog( 0, "ioDoubleCrownMode::OnNotifyCharDie - %s Not Exist Kill Char", pKillChar->GetCharName().c_str() );
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

void ioDoubleCrownMode::NotifyDropItem( ioItem *pItem, ioBaseChar *pChar )
{
	if( !pItem ) return;
	if( pItem->GetCrownType() != ioItem::MCT_DOUBLE_CROWN ) return;

	ioFieldItem *pCrown = m_pCreator->GetCrownFieldItem( ioItem::MCT_DOUBLE_CROWN );
	if( !pCrown )
	{
		return;
	}

	if( pChar )
	{
		SetMiniMapEvent( pCrown->GetWorldPosition(), MiniMap::EVENT_CROWN_DROP, pChar->GetTeam() );

		if( pChar->GetTeam() == GetOwnerTeam() )
		{
			g_SoundMgr.PlaySound( m_DropCrownSound );
		}
	}
}

void ioDoubleCrownMode::NotifyPickItem( ioItem *pItem, ioBaseChar *pChar )
{
	if( !pItem || !pChar )
		return;

	if( pItem->GetCrownType() == ioItem::MCT_NONE )
		return;
	
	switch( pChar->GetTeam() )
	{
	case TEAM_BLUE:
		m_dwBlueCheckKingPingTime = 0;
		m_iBlueCurSendIndex = 1;
		break;
	case TEAM_RED:			
		m_dwRedCheckKingPingTime  = 0;
		m_iRedCurSendIndex = 1;
		break;
	}

	if( pChar->GetTeam() == GetOwnerTeam() )
	{
		g_SoundMgr.PlaySound( m_TakeCrownSound );
	}
}

void ioDoubleCrownMode::NottifyWeaponWounded( ioBaseChar* pChar )
{
	if( !pChar )
		return;

	ioBaseChar* pRedKing  = GetKing( TEAM_RED );
	ioBaseChar* pBlueKing = GetKing( TEAM_BLUE );
	
	if( pRedKing && pRedKing == pChar )
	{		
		g_ModeUIController.SetDoubleCrownAttack( TEAM_RED );
	}

	if( pBlueKing && pBlueKing == pChar )
	{		
		g_ModeUIController.SetDoubleCrownAttack( TEAM_BLUE );
	}
}

void ioDoubleCrownMode::ReStartMode()
{
	D3DXVECTOR3 vStartPos;

	int iCharCnt = m_vCharRecordList.size();
	for( int i=0 ; i<iCharCnt ; i++ )
	{
		DoubleCrownRecord &rkRecord = m_vCharRecordList[i];
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

	m_fCurRedCrownPoint  = m_fDefaultCrownPoint;
	m_fCurBlueCrownPoint = m_fDefaultCrownPoint;	
	m_fCurRedDecreaseCrownPoint  = m_fDecreaseCrownPoint;
	m_fCurBlueDecreaseCrownPoint = m_fDecreaseCrownPoint;	

	m_dwBlueContribute = 0;
	m_dwRedContribute  = 0;

	m_dwBlueCheckKingPingTime = 0;
	m_dwRedCheckKingPingTime  = 0;

	m_dwSyncRequestTime	= 0;

	m_iBlueCurSendIndex = 1;
	m_iRedCurSendIndex  = 1;

	m_dwCurRevivalTime   = 0;
	m_dwCheckWarningTime = 0;
	m_bScoreWarning      = false;

	m_bFirstSelect = true;
	m_bSendSync    = false;

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

	SetMapDummy();
}

D3DXVECTOR3 ioDoubleCrownMode::GetRevivalPos( ioBaseChar *pChar )
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

void ioDoubleCrownMode::ContributeLOG()
{
	char szLog[MAX_PATH];
	int iCharCnt = m_vCharRecordList.size();
	for( int i = 0 ; i < iCharCnt; ++i )
	{
		DoubleCrownRecord *pRecord = &m_vCharRecordList[i];
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

void ioDoubleCrownMode::ContributeConvertTeam( TeamType eTeam )
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
		DoubleCrownRecord *pRecord = &m_vCharRecordList[i];
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
			DoubleCrownRecord *pRecord = &m_vCharRecordList[iArray];

			if( !pRecord->Contribute.m_bLastResultSync )
				pRecord->Contribute.m_iContribute = (float)m_vContributeTable[vInfo[i].m_iRank] * 1.5f;
			pRecord->Contribute.m_iKingTimeRank = vInfo[i].m_iRank;
		}
	}
	vInfo.clear();

	//왕관 점수 획득
	for(i = 0;i < iCharCnt;i++)
	{
		DoubleCrownRecord *pRecord = &m_vCharRecordList[i];
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
			DoubleCrownRecord *pRecord = &m_vCharRecordList[iArray];

			if( !pRecord->Contribute.m_bLastResultSync )
				pRecord->Contribute.m_iContribute += (float)m_vContributeTable[vInfo[i].m_iRank] * 1.5f;

			pRecord->Contribute.m_iKingPointRank = vInfo[i].m_iRank;
		}
	}
	vInfo.clear();

	//KO
	for(i = 0;i < iCharCnt;i++)
	{
		DoubleCrownRecord *pRecord = &m_vCharRecordList[i];
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
			DoubleCrownRecord *pRecord = &m_vCharRecordList[iArray];
			if( !pRecord->Contribute.m_bLastResultSync )
				pRecord->Contribute.m_iContribute += m_vContributeTable[vInfo[i].m_iRank] * 2;
			pRecord->Contribute.m_iKillRank = vInfo[i].m_iRank;
		}
	}
	vInfo.clear();

	//데미지 & 힐
	for(i = 0;i < iCharCnt;i++)
	{
		DoubleCrownRecord *pRecord = &m_vCharRecordList[i];
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
			DoubleCrownRecord *pRecord = &m_vCharRecordList[iArray];

			if( !pRecord->Contribute.m_bLastResultSync )
				pRecord->Contribute.m_iContribute += m_vContributeTable[vInfo[i].m_iRank] * 2;

			pRecord->Contribute.m_iDamageRank = vInfo[i].m_iRank;
		}
	}
	vInfo.clear();

	//유효 타격 횟수
	for(i = 0;i < iCharCnt;i++)
	{
		DoubleCrownRecord *pRecord = &m_vCharRecordList[i];
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
			DoubleCrownRecord *pRecord = &m_vCharRecordList[iArray];
			if( !pRecord->Contribute.m_bLastResultSync )
				pRecord->Contribute.m_iContribute += m_vContributeTable[vInfo[i].m_iRank] * 2;
			pRecord->Contribute.m_iAttackCntRank = vInfo[i].m_iRank;
		}
	}
	vInfo.clear();
}

void ioDoubleCrownMode::ContributeConvert()
{
	ContributeConvertTeam( TEAM_NONE );        //TEAM_NONE는 전체 유저에 대한 기여도 평가
}

int ioDoubleCrownMode::ContributeMyRank( int &rRankValue1, int &rRankValue2, int &rRankValue3, int &rRankValue4 )
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
		DoubleCrownRecord *pRecord = &m_vCharRecordList[i];
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
		DoubleCrownRecord *pRecord = &m_vCharRecordList[i];
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

	vInfo.clear();

	//유효 타격 횟수
	for(i = 0;i < iCharCnt;i++)
	{
		DoubleCrownRecord *pRecord = &m_vCharRecordList[i];
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
		DoubleCrownRecord *pRecord = &m_vCharRecordList[i];
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

void ioDoubleCrownMode::ContributeRoundAllPercent( DWORD dwCurTime, DWORD dwTotalTime )
{
	//기여도의 100분율
	int iMaxContribute	= 0;
	int iCharCnt		= m_vCharRecordList.size();
	int iPlayCharCnt	= 0;

	int i = 0;
	for(i = 0;i < iCharCnt;i++)
	{
		DoubleCrownRecord *pRecord = &m_vCharRecordList[i];
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
			DoubleCrownRecord *pRecord = &m_vCharRecordList[i];
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

void ioDoubleCrownMode::DamageContribute( const ioHashString szAttacker, const ioHashString szTarget, float fDamage, DWORD dwItemCode, int iWeaponType )
{
	if( szAttacker.IsEmpty() ) 
		return;

	DoubleCrownRecord *pRecord = FindRecordCharName( szAttacker );
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

	DoubleCrownRecord *pTargetRecord = FindRecordCharName( szTarget );
	if( !pTargetRecord || !pTargetRecord->pChar ) 
		return;

	ioBaseChar *pTargetChar = m_pCreator->GetBaseChar( szTarget );
	if( !pTargetChar ) 
		return;

	//왕 타격시 추가 점수
	if( pTargetRecord->pChar == GetKing( pTargetRecord->pChar->GetTeam() ) )
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

void ioDoubleCrownMode::KillContribute( const ioHashString szKiller, const ioHashString szKilled, DWORD dwItemCode )
{
	if( szKilled.IsEmpty() ) return;

	int iKillPoint = 100;

	float fAddKillPoint = FLOAT1;
	if( IsContributeAddPointItemCode( dwItemCode ) )
		fAddKillPoint = 1.1f;

	if( szKiller.IsEmpty() || szKiller == szKilled )     //자살
	{
		DoubleCrownRecord *pRecord = FindRecordCharName( szKilled );
		if( !pRecord || !pRecord->pChar ) return;
		
		pRecord->Contribute.m_iKill -= iKillPoint;
	}
	else
	{
		DoubleCrownRecord *pKillerRecord = FindRecordCharName( szKiller );
		DoubleCrownRecord *pKilledRecord = FindRecordCharName( szKilled );
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

void ioDoubleCrownMode::AttackCntContribute( const ioHashString szAttacker, const ioHashString szTarget, DWORD dwItemCode, int iAttackValue/* = 100 */ )
{
	if( szAttacker.IsEmpty() || szTarget.IsEmpty() ) return;

	DoubleCrownRecord *pAttackerRecord = FindRecordCharName( szAttacker );
	DoubleCrownRecord *pTargetRecord   = FindRecordCharName( szTarget );

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

			//왕 타격 횟수 추가 
			if( GetKing( pTarget->GetTeam() ) == pTargetRecord->pChar )
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

void ioDoubleCrownMode::CrownHoldTime()
{
	ioBaseChar* pBlueKing = GetKing( TEAM_BLUE );
	ioBaseChar* pRedKing  = GetKing( TEAM_RED );

	if( pBlueKing )
	{
		DoubleCrownRecord *pBlueKingRecord = FindRecordCharName( pBlueKing->GetCharName() );
		if( pBlueKingRecord )
			CheckCrownHoldTime( pBlueKingRecord, pBlueKing );	
	}
	
	if( pRedKing )
	{
		DoubleCrownRecord *pRedKingRecord = FindRecordCharName( pRedKing->GetCharName() );
		if( pRedKingRecord )
			CheckCrownHoldTime( pRedKingRecord, pRedKing );
	}
	
}

void ioDoubleCrownMode::CheckCrownHoldTime( DoubleCrownRecord* pRecord, ioBaseChar* pKing )
{
	if( !pRecord )
		return;

	if( !pKing )
		return;

	if( FRAMEGETTIME() - pRecord->m_dwCrownHoldTime > 500 )
	{
		//왕관 보유유저
		pRecord->m_dwCrownHoldTime = FRAMEGETTIME();
		pRecord->Contribute.m_iKingTime += 1000;
		g_AwardMgr.AddKing( pKing->GetCharName(), 500 );

		if( pKing->IsOwnerChar() )
		{
			g_QuestMgr.QuestCompleteTerm( QC_CROWN_HOLD_TIME, 500 );
		}

		//1000Pixel 이내의 유저들에게 0.75f 적용
		int iCharCnt = m_vCharRecordList.size();
		for(int i = 0;i < iCharCnt;i++)
		{
			DoubleCrownRecord *pUserRecord = &m_vCharRecordList[i];
			if( !pUserRecord ) 
				continue;

			if( pUserRecord == pRecord ) 
				continue;

			if( !pUserRecord->pChar ) 
				continue;

			if( !IsInKingRange( pUserRecord->pChar, 750.0f ) ) 
				continue;

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

void ioDoubleCrownMode::CrownHoldRoundSetEnd()
{
	ioBaseChar* pBlueKing = GetKing( TEAM_BLUE );
	ioBaseChar* pRedKing  = GetKing( TEAM_RED );

	if( pBlueKing )
	{
		DoubleCrownRecord *pBlueKingRecord = FindRecordCharName( pBlueKing->GetCharName() );
		if( pBlueKingRecord )
			CrownHoldRoundEnd( pBlueKingRecord, pBlueKing );	
	}

	if( pRedKing )
	{
		DoubleCrownRecord *pRedKingRecord = FindRecordCharName( pRedKing->GetCharName() );
		if( pRedKingRecord )
			CrownHoldRoundEnd( pRedKingRecord, pRedKing );
	}

}

void ioDoubleCrownMode::CrownHoldRoundEnd( DoubleCrownRecord* pRecord, ioBaseChar* pKing )
{
	if( !pRecord )
		return;

	if( !pKing )
		return;
	
	pRecord->Contribute.m_iKingPoint += 300;
	g_AwardMgr.AddKing( pKing->GetCharName(), 10000 );

	//1000Pixel 이내의 유저들에게 0.75f 적용
	int iCharCnt = m_vCharRecordList.size();
	for(int i = 0;i < iCharCnt;i++)
	{
		DoubleCrownRecord *pUserRecord = &m_vCharRecordList[i];
		if( !pUserRecord ) 
			continue;

		if( pUserRecord == pRecord ) 
			continue;

		if( !pUserRecord->pChar ) 
			continue;

		if( pUserRecord->pChar->GetTeam() != pRecord->pChar->GetTeam() ) 
			continue;

		if( !IsInKingRange( pUserRecord->pChar, 750.0f ) ) 
			continue;

		pUserRecord->Contribute.m_iKingPoint += (float)300 * 0.60f;
	}		
	
	ContributeConvert();
	ContributeRoundAllPercent();
	UpdateRoundInfoList();
	CheckTotalChamp();
}

bool ioDoubleCrownMode::IsInKingRange( ioBaseChar *pChar, float fRange )
{	
	if( !pChar )
		return false;

	ioBaseChar *pKing = GetKing( pChar->GetTeam() );
	if( !pKing )
		return false;

	if( pChar->GetState() == CS_DIE			||
		pChar->GetState() == CS_READY		||
		pChar->GetState() == CS_LOADING		||
		pChar->GetState() == CS_OBSERVER	||
		pChar->GetState() == CS_CHANGE_WAIT ||
		pChar->GetState() == CS_VIEW ) 
		return false;

	float fRangeSq = fRange * fRange;
	D3DXVECTOR3 vDiff = pChar->GetWorldPosition() - pKing->GetWorldPosition();
	if( D3DXVec3LengthSq( &vDiff ) > fRangeSq )
		return false;

	return true;
}

void ioDoubleCrownMode::FillLastPlayRecordInfo( SP2Packet &rkPacket )
{
	int iCharCnt = m_vCharRecordList.size();
	rkPacket << iCharCnt;
	for(int i = 0;i < iCharCnt;i++)
	{
		DoubleCrownRecord *pRecord = &m_vCharRecordList[i];
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

void ioDoubleCrownMode::FillPlayRecordInfo( SP2Packet &rkPacket )
{
	int iCharCnt = m_vCharRecordList.size();
	rkPacket << iCharCnt;
	for(int i = 0;i < iCharCnt;i++)
	{
		DoubleCrownRecord *pRecord = &m_vCharRecordList[i];
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
		rkPacket << pRecord->GetNoDeathKillCount();
	}
}

void ioDoubleCrownMode::ApplyPlayRecordInfo( SP2Packet &rkPacket )
{
	int iCharCnt;
	rkPacket >> iCharCnt;
	for(int i = 0;i < iCharCnt;i++)
	{
		ioHashString szName;
		int iDamage, iAttackCnt, iKill, iKingTime, iKingPoint, iPrevContributePer, iUniqueTotalKill, iUniqueTotalDeath, iVictories, iNoDeathKillCount;
		rkPacket >> szName >> iDamage >> iAttackCnt >> iKill >> iKingTime >> iKingPoint >> iPrevContributePer >> iUniqueTotalKill >> iUniqueTotalDeath >> iVictories >> iNoDeathKillCount;

		DoubleCrownRecord *pRecord = FindRecordCharName( szName );
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

void ioDoubleCrownMode::ContributeResultSync( const ioHashString &szName, int iContribute )
{
	DoubleCrownRecord *pRecord = FindRecordCharName( szName );
	if( !pRecord ) return;

	pRecord->Contribute.m_bLastResultSync = true;
	pRecord->Contribute.m_iContribute     = iContribute;
}

void ioDoubleCrownMode::OnZeroHP( SP2Packet &rkPacket )
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

void ioDoubleCrownMode::OnRoundReady( SP2Packet &rkPacket )
{
	ShowEndScoreWnd( false );
	SetBlindMode( false );

	rkPacket >> m_iCurRoundCount;
	rkPacket >> m_iBluePosArray;
	rkPacket >> m_iRedPosArray;
	rkPacket >> m_dwCurRoundDuration;

	rkPacket >> m_fCurRedCrownPoint;
	rkPacket >> m_fCurBlueCrownPoint;
	rkPacket >> m_fCurRedDecreaseCrownPoint;
	rkPacket >> m_fCurBlueDecreaseCrownPoint;
	
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

void ioDoubleCrownMode::OnRoundJoin( SP2Packet &rkPacket )
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
		rkPacket >> m_fCurRedCrownPoint;
		rkPacket >> m_fCurBlueCrownPoint;
		rkPacket >> m_fCurRedDecreaseCrownPoint;
		rkPacket >> m_fCurBlueDecreaseCrownPoint;
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
		rkPacket >> m_fCurRedCrownPoint;
		rkPacket >> m_fCurBlueCrownPoint;
		rkPacket >> m_fCurRedDecreaseCrownPoint;
		rkPacket >> m_fCurBlueDecreaseCrownPoint;
	}

	m_pCreator->IncreaseTeamCntForBalance( pJoinChar->GetTeam() );
	m_pCreator->ReCalculateDamageRate( false );
}

void ioDoubleCrownMode::OnRoundJoinView( SP2Packet &rkPacket )
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
		rkPacket >> m_fCurRedCrownPoint;
		rkPacket >> m_fCurBlueCrownPoint;
		rkPacket >> m_fCurRedDecreaseCrownPoint;
		rkPacket >> m_fCurBlueDecreaseCrownPoint;
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
		rkPacket >> m_fCurRedCrownPoint;
		rkPacket >> m_fCurBlueCrownPoint;
		rkPacket >> m_fCurRedDecreaseCrownPoint;
		rkPacket >> m_fCurBlueDecreaseCrownPoint;
	}

	m_pCreator->ReCalculateDamageRate( false );
}

void ioDoubleCrownMode::OnRoundJoinObserver( SP2Packet &rkPacket )
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
		rkPacket >> m_fCurRedCrownPoint;
		rkPacket >> m_fCurBlueCrownPoint;
		rkPacket >> m_fCurRedDecreaseCrownPoint;
		rkPacket >> m_fCurBlueDecreaseCrownPoint;

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
		rkPacket >> m_fCurRedCrownPoint;
		rkPacket >> m_fCurBlueCrownPoint;
		rkPacket >> m_fCurRedDecreaseCrownPoint;
		rkPacket >> m_fCurBlueDecreaseCrownPoint;
	}

	m_pCreator->ReCalculateDamageRate( false );
}

void ioDoubleCrownMode::OnKingTake( SP2Packet &rkPacket )
{
	ioHashString szKingName;
	rkPacket >> szKingName;
	rkPacket >> m_fCurRedCrownPoint;
	rkPacket >> m_fCurBlueCrownPoint;
	rkPacket >> m_fCurRedDecreaseCrownPoint;
	rkPacket >> m_fCurBlueDecreaseCrownPoint;

	//동기화 주기 / 점수 보정시간 갱신
	m_dwSyncRequestTime		= FRAMEGETTIME();
	m_dwCurProcessTickTime	= FRAMEGETTIME();

	SetEventTakeKing( szKingName );
}

void ioDoubleCrownMode::SetResultScoreWnd()
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

void ioDoubleCrownMode::SetTotalScoreWnd( IntVec &rvClassTypeList, IntVec &rvClassPointList, float fBlueGuildTeamBonus, float fRedGuildTeamBonus, float fBonusArray[BA_MAX] )
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
		/*
		for(int i = 0;i < m_iCurRoundCount - 1;++i)
		{
			iHisRoundBlue -= m_vRoundHistory[ i ].iBluePoint;
			iHisRoundRed  -= m_vRoundHistory[ i ].iRedPoint;
		}
		m_vRoundHistory[ m_iCurRoundCount - 1 ].iBluePoint = max( iHisRoundBlue, 0 );
		m_vRoundHistory[ m_iCurRoundCount - 1 ].iRedPoint  = max( iHisRoundRed, 0 );
		*/
	}

	m_dwCurResultShowDuration = m_dwFinalRoundResultDuration;

	pTotalResult->SetTeamScore( GetOwnerTeam(), m_iBlueRoundWinCnt, m_iRedRoundWinCnt );
	SetTotalResultBriefingWnd();
}

void ioDoubleCrownMode::CheckEndSceneLookAt()
{
	ioBaseChar* pKing = NULL;
	TeamType eTeam = TEAM_NONE;
	switch( m_iWinTeam )
	{
	case WTT_BLUE_TEAM:
	case WTT_VICTORY_RED_TEAM:		
		{
			pKing = GetKing( TEAM_BLUE );
			eTeam = TEAM_BLUE;
		}
		break;
	case WTT_RED_TEAM:
	case WTT_VICTORY_BLUE_TEAM:
		{
			pKing = GetKing( TEAM_RED );
			eTeam = TEAM_RED;
		}
		break;
	}
		
	if( pKing )
	{
		m_vEndLookAt = pKing->GetWorldPosition();
		m_szEndLookCharName = pKing->GetCharName();
		pKing->SetEndFocusEffect( GetRoundEndEffect() );
		return;
	}

	// Crown
	ioFieldItemVec CrownVec;		
	m_pCreator->GetCrownFieldItemByAll( ioItem::MCT_DOUBLE_CROWN, CrownVec );

	for( ioFieldItemVec::iterator iter = CrownVec.begin(); iter != CrownVec.end(); ++iter )
	{
		ioFieldItem* pCrown = *iter;
		if( pCrown && pCrown->GetItemTeamType() == eTeam )
		{
			m_vEndLookAt = pCrown->GetWorldPosition();
			m_szEndLookCharName.Clear();
			return;
		}
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

void ioDoubleCrownMode::ClearModeData()
{
	m_LastDieChar.Clear();
	m_LastKillChar.Clear();

	g_SoundMgr.AllSoundStop();
	g_SoundMgr.StopStream();
	m_pCreator->HidingAllFieldItem();
}

bool ioDoubleCrownMode::IsNeedRevival( ioBaseChar *pChar )
{
	return true;
}

ioBaseChar* ioDoubleCrownMode::CheckMostNearChar( ioBaseChar *pChar )
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

int ioDoubleCrownMode::GetCurRank( const ioHashString& szCharName )
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

int ioDoubleCrownMode::GetContributePerChar( const ioHashString& szName )
{
	int iCharCnt = m_vCharRecordList.size();

	for(int i = 0;i < iCharCnt;i++)
	{
		DoubleCrownRecord *pRecord = &m_vCharRecordList[i];
		if( !pRecord )	continue;
		if( !pRecord->pChar ) continue;

		if( pRecord->pChar->GetCharName() == szName )
		{
			return pRecord->Contribute.m_iPrevContributePer;
		}
	}

	return 0;
}

float ioDoubleCrownMode::GetCurExtraScoreGaugeRate( TeamType eTeam )
{
	float fExtraRate = FLOAT1;
	if( eTeam == TEAM_RED )
	{
		fExtraRate = g_MathManager.GetTeamSurvivalBalanceGauge( m_fCurRedCrownPoint, m_fCurBlueCrownPoint );
		fExtraRate = max( 0.0, fExtraRate );
	}
	else if( eTeam == TEAM_BLUE )
	{
		fExtraRate = g_MathManager.GetTeamSurvivalBalanceGauge( m_fCurBlueCrownPoint, m_fCurRedCrownPoint );
		fExtraRate = max( 0.0, fExtraRate );
	}

	return fExtraRate;
}

float ioDoubleCrownMode::GetCurExtraScoreDamage( TeamType eTeam, float fCurDamage, float fBaseDamage )
{
	float fResult = fCurDamage;

	if( eTeam == TEAM_RED )
		fResult = g_MathManager.GetTeamSurvivalBalanceDamage( fCurDamage, fBaseDamage, m_fCurRedCrownPoint, m_fCurBlueCrownPoint );
	else if( eTeam == TEAM_BLUE )
		fResult = g_MathManager.GetTeamSurvivalBalanceDamage( fCurDamage, fBaseDamage, m_fCurBlueCrownPoint, m_fCurRedCrownPoint );

	return fResult;
}

void ioDoubleCrownMode::OnRoundEnd( SP2Packet &rkPacket )
{
	float fCurRedCrownPoint  = m_fCurRedCrownPoint;
	float fCurBlueCrownPoint = m_fCurBlueCrownPoint;
	int iWinTeam = CheckCrownPointWinTeam( fCurBlueCrownPoint, fCurRedCrownPoint );

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

	rkPacket >> m_fCurRedCrownPoint;
	rkPacket >> m_fCurBlueCrownPoint;
	rkPacket >> m_fCurRedDecreaseCrownPoint;
	rkPacket >> m_fCurBlueDecreaseCrownPoint;

	CrownHoldRoundSetEnd();

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
}

void ioDoubleCrownMode::CrownDrop( const ioHashString szAttacker, const ioHashString szKing )
{
	if( szAttacker.IsEmpty() || szKing.IsEmpty() ) return;

	DoubleCrownRecord *pAttackerRecord = FindRecordCharName( szAttacker );
	DoubleCrownRecord *pKingRecord	   = FindRecordCharName( szKing );

	if( pKingRecord && pKingRecord->pChar )
	{
		switch( pKingRecord->pChar->GetTeam() )
		{
		case TEAM_BLUE:
			m_dwBlueCheckKingPingTime = 0;
			m_iBlueCurSendIndex = 1;	
			break;
		case TEAM_RED:			
			m_dwRedCheckKingPingTime  = 0;
			m_iRedCurSendIndex = 1;
			break;
		}
	}

	if( pAttackerRecord && pAttackerRecord->pChar && pKingRecord && pKingRecord->pChar )
	{
		switch( pKingRecord->pChar->GetTeam() )
		{
		case TEAM_BLUE:
			m_dwBlueCrownShowEventTime = FRAMEGETTIME() + m_dwCrownShowGapTime;
			break;
		case TEAM_RED:			
			m_dwRedCrownShowEventTime = FRAMEGETTIME() + m_dwCrownShowGapTime;
			break;
		}

		if( pAttackerRecord->pChar->GetTeam() == pKingRecord->pChar->GetTeam() )
		{			
			g_AwardMgr.AddTeamKingAttack( szAttacker );
		}
		else
		{
			g_AwardMgr.AddOtherKingAttack( szAttacker, 10 );
			if( pAttackerRecord->pChar->IsOwnerChar() )
				g_QuestMgr.QuestCompleteTerm( QC_OTHER_KING_ATTACK );
		}

		// 왕관드롭 메세지
		g_ChatMgr.SetDropCrownMsg( pAttackerRecord->pChar->GetTeam(), szAttacker, pKingRecord->pChar->GetTeam(), szKing );
	}

	ContributeConvert();
	ContributeRoundAllPercent();
	UpdateRoundInfoList();
	CheckTotalChamp();
}

void ioDoubleCrownMode::SetEventTakeKing( const ioHashString &rkNewKingName )
{	
	if( !rkNewKingName.IsEmpty() )
	{
		ioBaseChar *pChar = GetRecordChar( rkNewKingName );
		if( pChar )
		{
			g_ChatMgr.SetGetCrownMsg( pChar->GetTeam(), rkNewKingName );
			SetMiniMapEvent( pChar->GetWorldPosition(), MiniMap::EVENT_CROWN_ACQUIRE, pChar->GetTeam() );
		}
	}
}

void ioDoubleCrownMode::ProcessModeGuide()
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
		ioGameEntity* pTeamTarget  = GetGuideTarget( eTeam );
		ioGameEntity* pEnemyTarget = GetGuideTarget( eEnemy );
		
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

ioGameEntity* ioDoubleCrownMode::GetGuideTarget( TeamType eTeam )
{
	ioBaseChar* pOwner = GetOwnerChar();
	if( !pOwner )
		return NULL;
		
	ioFieldItem* pCrown = GetFieldCrown( eTeam );
	ioBaseChar* pKing = GetKing( eTeam );

	if( pKing )
	{
		if( pKing == pOwner )
			return NULL;

		return pKing;
	}
	else if( pCrown )
	{
		return pCrown;
	}

	return NULL;
}

ioFieldItem* ioDoubleCrownMode::GetFieldCrown( TeamType eTeam )
{
	// Field Crown
	ioFieldItemVec CrownVec;
	m_pCreator->GetCrownFieldItemByAll( ioItem::MCT_DOUBLE_CROWN, CrownVec );

	for( ioFieldItemVec::iterator iter = CrownVec.begin(); iter != CrownVec.end(); ++iter )
	{
		ioFieldItem* pCrown = *iter;
		if( pCrown && pCrown->GetItemTeamType() == eTeam )
		{
			return pCrown;
		}
	}

	return NULL;
}

void ioDoubleCrownMode::OnRoundEndCallContribute( SP2Packet &rkPacket )
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

void ioDoubleCrownMode::OnDoubleCrownSyncRequest( SP2Packet &rkPacket )
{	
	rkPacket >> m_fCurRedCrownPoint;
	rkPacket >> m_fCurBlueCrownPoint;
	rkPacket >> m_fCurRedDecreaseCrownPoint;
	rkPacket >> m_fCurBlueDecreaseCrownPoint;

	//동기화 주기 / 점수 보정시간 갱신
	m_dwSyncRequestTime		= FRAMEGETTIME();
	m_dwCurProcessTickTime	= FRAMEGETTIME();
}

void ioDoubleCrownMode::CrownBadPingDrop( const ioHashString szKing )
{
	if( szKing.IsEmpty() ) 
		return;
	
	DoubleCrownRecord* pKingRecord = FindRecordCharName( szKing );
	if( !pKingRecord )
		return;

	if( !pKingRecord->pChar )
		return;

	switch( pKingRecord->pChar->GetTeam() )
	{
	case TEAM_BLUE:
		m_dwBlueCheckKingPingTime = 0;
		m_iBlueCurSendIndex = 1;
		break;
	case TEAM_RED:			
		m_dwRedCheckKingPingTime  = 0;
		m_iRedCurSendIndex = 1;
		break;
	}
	
	// 왕관드롭 메세지
	g_ChatMgr.SetBadPingCrownDropMsg( pKingRecord->pChar->GetTeam(), szKing );
	
	ContributeConvert();
	ContributeRoundAllPercent();
	UpdateRoundInfoList();
	CheckTotalChamp();
}

void ioDoubleCrownMode::SendKingPingMessage( DWORD& dwCheckTime, int& iCurSendIndex )
{
	DWORD dwCurTime = REALGETTIME();
	if( dwCheckTime != 0 )
	{
		DWORD dwSendTimeGap = dwCurTime - dwCheckTime;
		if( dwSendTimeGap < m_dwKingPingTime )
			return;
	}

	SP2Packet kPacket( CUPK_CHECK_KING_PING );
	kPacket << g_MyInfo.GetPublicID();
	kPacket << g_GlobalTimer.GetGlobalTime();
	kPacket << iCurSendIndex;
	P2PNetwork::SendToServer( kPacket );

	dwCheckTime = dwCurTime;
	iCurSendIndex++;
}

void ioDoubleCrownMode::ProcessKing()
{
	ioBaseChar *pOwner = m_pCreator->GetOwnerChar();
	if( !pOwner )	
		return;

	if( m_dwKingPingTime == 0 )
		return;

	switch( pOwner->GetTeam() )
	{
	case TEAM_BLUE:
		{
			ioBaseChar *pBlueKing = GetKing( TEAM_BLUE );
			if( pBlueKing == pOwner )
			{		
				SendKingPingMessage( m_dwBlueCheckKingPingTime, m_iBlueCurSendIndex );
			}
		}
		break;
	case TEAM_RED:
		{
			ioBaseChar *pRedKing = GetKing( TEAM_RED );
			if( pRedKing == pOwner )
			{
				SendKingPingMessage( m_dwRedCheckKingPingTime, m_iRedCurSendIndex );
			}
		}
		break;
	}
}

TeamType ioDoubleCrownMode::CheckCrownPointWinTeam( float fCurBlueCrownPoint, float fCurRedCrownPoint )
{
	TeamType ePointTeam = TEAM_NONE;

	if( fCurBlueCrownPoint > fCurRedCrownPoint && fCurRedCrownPoint == 0.0f )
		ePointTeam = TEAM_BLUE;
	else if( m_fCurRedCrownPoint > fCurBlueCrownPoint && fCurBlueCrownPoint == 0.0f )
		ePointTeam = TEAM_RED;

	return ePointTeam;
}


void ioDoubleCrownMode::SendSyncRequest( bool bMustSend /* = false */ )
{
	DWORD dwCurrTime = FRAMEGETTIME();
	bool bSend = false;
	if(  m_dwSyncRequestTime + 10000 < dwCurrTime )
		bSend = true;

	if( bMustSend || bSend )
	{	
		m_dwSyncRequestTime = dwCurrTime;
		SP2Packet kPacket( CTPK_DOUBLE_CROWN_SYNC_REQUEST );
		TCPNetwork::SendToServer( kPacket );
	}
}

void ioDoubleCrownMode::SetModeState( ModeState eState )
{
	ioPlayMode::SetModeState( eState );
	if( eState == MS_PLAY )
	{
		m_dwCurProcessTickTime	= FRAMEGETTIME();
	}
}