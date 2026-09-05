

#include "stdafx.h"

#include "ioBossMode.h"
#include "ioLadderTeamMgr.h"
#include "ioRankBattleManager.h"
#include "ioShuffleRoomMgr.h"

#include "GUI/MiniSoldierSelectWnd.h"
#include "GUI/MiniMap.h"
#include "GUI/CenterInfoWnd.h"
#include "GUI/TimeScoreWnd.h"
#include "GUI/BriefingWnd.h"
#include "GUI/ioObserverWnd.h"
#include "GUI/TotalResultMainWnd.h"
#include "GUI/RoundStateMainWnd.h"
#include "GUI/CenterKillInfoWnd.h"

ioBossMode::ioBossMode( ioPlayStage *pCreator )
: ioPlayMode( pCreator )
{
	m_ModeType = MT_BOSS;

	m_iCurBossLevel = 1;
	m_iMyBossLevel  = 1;
	m_iMaxBossLevel = 10;
	m_dwBossChangeProtectedTime = 0;
	m_dwBossProtectedTime = 0;
	m_dwBossProtectedLevelGapTime = 0;
	m_fBossChangeRecoverHP = 0.0f;
	m_fBossRecoverHP = 0.0f;
	m_fBossRecoverLevelGapHP = 0.0f;
	m_SingleTeamPosArray.clear();
	m_dwCenterUIDelayTime = 0;
	m_eResultMotionWinTeam = TEAM_NONE;

	m_vCharRecordList.reserve( 100 );
}

ioBossMode::~ioBossMode()
{
	ClearMode();
}

void ioBossMode::LoadModeResource( ioINILoader &rkLoader )
{	
	TimeScoreWnd *pTimeScoreWnd = dynamic_cast<TimeScoreWnd *>( g_GUIMgr.AddWnd( "XML/TimeScoreWnd.xml") );
	if( pTimeScoreWnd )
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
}

void ioBossMode::LoadMap()
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

void ioBossMode::LoadContribute()
{
	ioPlayMode::LoadContribute();
}

void ioBossMode::InitMode( int iMapIndex )
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

	m_iMaxBossLevel       = kLoader.LoadInt_e( "boss_max_level", 10 );
	m_dwBossChangeProtectedTime = kLoader.LoadInt_e( "boss_change_protect_time", 5500 );
	m_dwBossProtectedTime = kLoader.LoadInt_e( "boss_protect_time", 5500 );
	m_dwBossProtectedLevelGapTime = kLoader.LoadInt_e( "boss_protect_level_gap_time", 500 );	
	
	m_fBossChangeRecoverHP = kLoader.LoadFloat_e( "boss_change_recover_hp", FLOAT1 );
	m_fBossRecoverHP = kLoader.LoadFloat_e( "boss_recover_hp", FLOAT1 );
	m_fBossRecoverLevelGapHP = kLoader.LoadFloat_e( "boss_recover_level_gap_hp", 0.1f );

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

	// MapAreaWeapon Set
	SetMapAreaWeapon();

	SetMapDummy();

	if( g_BattleRoomMgr.IsBattleRoom() )
		g_BattleRoomMgr.ModeStartPlayerCount();
}

void ioBossMode::ReStartMode()
{
	LOG.PrintTimeAndLog( 0, "ioBossMode::ReStartMode Is Call???" );
}

void ioBossMode::ClearModeData()
{
	ioPlayMode::ClearModeData();
	m_LastDieChar.Clear();
	m_LastKillChar.Clear();
	m_vBossHistory.clear();
}

void ioBossMode::ProcessMode()
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

void ioBossMode::ProcessReadyState()
{
	if( P2PNetwork::IsNetworkPlaying() )	return;
	if( m_dwReadyStartTime + 4000 > FRAMEGETTIME() ) return;

	SetModeState( MS_PLAY );
}

void ioBossMode::ProcessPlayState()
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

void ioBossMode::ProcessResultState()
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
			// 내캐릭터에 카메라 고정
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

void ioBossMode::ProcessGUI()
{
	UpdateMiniMapSpotPoint();
	UpdateRoundStateWnd();
	UpdateCenterInfoWnd();
	UpdateTimeScoreWnd();
	UpdateObserverWnd();
	UpdateEndScoreWndTime();
	UpdateSoldierWndTime();

	ProcessModeGuide();
	ProcessPushEnableCursor();
	ProcessBlindMode();
	ProcessAutoStartAlarm();
	ProcessDeathTimeHelp();
}

void ioBossMode::ProcessRevival()
{
	DWORD dwCurTime = FRAMEGETTIME();
	
	int iCharCnt = m_vCharRecordList.size();
	for( int i=0 ; i<iCharCnt ; i++ )
	{
		BossRecord &rkRecord = m_vCharRecordList[i];
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


void ioBossMode::ProcessCharDropZoneCollision()
{
	int iCharCount = GetRecordCharCnt();
	for( int i=0 ; i<iCharCount ; i++ )
	{
		ioBaseChar *pChar = GetRecordChar( i );
		if( !pChar ) continue;
		if( !pChar->IsNeedProcess() )	continue;

		bool bDummyDrop = false;
		CharState eState = pChar->GetState();
		switch( eState )
		{
		case CS_DIE:
		case CS_DROP_ZONE_DOWN:
		case CS_ESCAPE_PRISONER:
		case CS_READY:
		case CS_LOADING:
		case CS_VIEW:
		case CS_OBSERVER:
		case CS_CHANGE_WAIT:
		case CS_STARTMOTION:
		case CS_ENDMOTION:
		case CS_CREATEMOTION:
		case CS_LOCKUP_BUFF:		
			continue;
		case CS_CANNON_SPECIAL:
			bDummyDrop = g_DummyCharMgr.IsDropZoneCollision( pChar );
			break;
		default:
			break;
		}

		if( pChar->GetWorldPosition().y >= 0.1f && bDummyDrop != true )
			continue;

		float fDropDamage = 0.0f;
		fDropDamage = m_fDropZoneDamage;

		if( IsSafetyLevelRoom() )
		{
			fDropDamage *= m_fDropProtectRate;
		}

		float fExtraRate = FLOAT1;

		TeamType eTeam = pChar->GetTeam();
		if( eTeam == TEAM_RED )
		{
			if( m_pCreator->GetMemberBalance( TEAM_RED ) != m_pCreator->GetMemberBalance( TEAM_BLUE ) )
			{
				fDropDamage = m_pCreator->GetMemberBalanceForDropDamage( eTeam, fDropDamage );
			}
		}

		if( fDropDamage > 0.0f )
		{
			fDropDamage = min( fDropDamage, m_fDropZoneMaxDamage );
			fDropDamage = max( m_fDropZoneMinDamage, fDropDamage );
		}

		pChar->ApplyDropDamage( fDropDamage );
	}
}


void ioBossMode::FillOwnerDamagedListByOtherTeam( const ioHashString &szTarget, TeamType eTeam, SP2Packet &rkPacket, bool bAttach )
{
	// 데미지 리스트 합치기
	if( bAttach )
	{
		DamageTable *pDamageTable = FindDamageList( szTarget );
		if( pDamageTable )
			pDamageTable->AttachAllDamage();
	}

	// 보스 기여도를 전송한다.
	int i = 0;
	int iTotalContributeCnt = 0;
	int iCharCnt = m_vCharRecordList.size();
	for(i = 0;i < iCharCnt;i++ )
	{
		BossRecord &rkRecord = m_vCharRecordList[i];
		if( rkRecord.pChar == NULL ) continue;
		if( rkRecord.pChar->GetTeam() == eTeam ) continue;
		if( rkRecord.iBossContribute > 0 )
		{
			iTotalContributeCnt++;
		}
	}
	rkPacket << iTotalContributeCnt;
	for(i = 0;i < iCharCnt;i++ )
	{
		BossRecord &rkRecord = m_vCharRecordList[i];
		if( rkRecord.pChar == NULL ) continue;
		if( rkRecord.pChar->GetTeam() == eTeam ) continue;
		if( rkRecord.iBossContribute > 0 )
		{
			rkPacket << rkRecord.szName << rkRecord.iBossContribute;
		}
	}
}

void ioBossMode::UpdateMiniMapSpotPoint()
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

		if( pChar->IsDropZoneDownState() )
			pMiniMap->AddPoint( vPos.x, vPos.z, MiniMap::SPOT_DROP_USER, pChar->GetTeam(), 0 );
		else
			pMiniMap->AddPoint( vPos.x, vPos.z, MiniMap::SPOT_USER, pChar->GetTeam(), dwGapTime );
	}

	// My Char
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

void ioBossMode::UpdateRoundStateWnd()
{
	if( g_MyInfo.IsSortContribute() )
	{
		DWORD dwTimeGap = FRAMEGETTIME() - m_dwPlayStartTime;
		if( m_dwCurRoundDuration > dwTimeGap )
			ContributeRoundAllPercent( m_dwRoundDuration - (m_dwCurRoundDuration - dwTimeGap), m_dwRoundDuration );
		else
			ContributeRoundAllPercent( m_dwRoundDuration, m_dwRoundDuration );

		BossRecord *pRecord = FindRecordCharName( g_MyInfo.GetPublicID() );
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

int ioBossMode::GetMyBossContributeRank( ioBaseChar *pOwnerChar, int iMaxPlayer )
{
	if( !pOwnerChar ) return iMaxPlayer;
	if( pOwnerChar->IsBoss() ) return 0;    

	// 보스에 대한 내 기여도 순위
	BossContributeInfoList vBossContributeList;
	RecordList::iterator iter;
	for( iter=m_vCharRecordList.begin() ; iter!=m_vCharRecordList.end() ; ++iter )
	{
		BossRecord &rkRecord = *iter;
		ioBaseChar *pChar = rkRecord.pChar;
		if( pChar == NULL ) continue;
		if( pChar->GetState() == CS_OBSERVER ) continue;
		if( g_BattleRoomMgr.IsObserverUser( pChar->GetCharName() ) ) continue;
		if( rkRecord.iBossContribute == 0 )  continue;

		BossContributeInfo kInfo;
		kInfo.m_szName = rkRecord.szName;
		kInfo.m_iContribute = rkRecord.iBossContribute;
		vBossContributeList.push_back( kInfo );		
	}

	if( !vBossContributeList.empty() )
	{
		std::sort( vBossContributeList.begin(), vBossContributeList.end(), BossContributeInfoSort() );

		for(int i = 0;i < (int)vBossContributeList.size();i++)
		{
			BossContributeInfo &rkInfo = vBossContributeList[i];
			if( rkInfo.m_szName == pOwnerChar->GetCharName() )
				return i;
		}
	}	
	return iMaxPlayer;
}

void ioBossMode::UpdateCenterInfoWnd()
{
	if( g_BattleRoomMgr.IsObserverUser( g_MyInfo.GetPublicID() ) ) return;
	if( m_pCreator->IsStealthMode() ) return;
	
	ioBaseChar *pOwnerChar = GetOwnerChar();
	if( !pOwnerChar ) return;
	if( pOwnerChar->GetState() == CS_VIEW ||
		pOwnerChar->GetState() == CS_LOADING ||
		pOwnerChar->GetState() == CS_READY||
		pOwnerChar->GetState() == CS_OBSERVER ) return;

	if( g_GUIMgr.IsShow( TIME_SCORE_WND ) )
	{
		if( m_dwCenterUIDelayTime == 0 )
			m_dwCenterUIDelayTime = FRAMEGETTIME();
	}

	if( pOwnerChar && pOwnerChar->IsBoss() )
	{
		//보스는 표시 안함
		g_ModeUIController.SetBossRaidMode( false, 0, 0, 0 );
	}
	else if( GetModeState() != MS_PLAY )
	{
		g_ModeUIController.SetBossRaidMode( false, 0, 0, 0 );
		m_dwCenterUIDelayTime = 0;
	}
	else if( m_dwCenterUIDelayTime > 0 )
	{
		// 첫 시작시 1.5초간 표시 안함
		if( FRAMEGETTIME() - m_dwCenterUIDelayTime > 1500 )
		{
			// 보스전 최대 참여 인원
			int iMaxPlayer = 0;
			RecordList::iterator iter;
			for( iter=m_vCharRecordList.begin() ; iter!=m_vCharRecordList.end() ; ++iter )
			{
				BossRecord &rkRecord = *iter;
				ioBaseChar *pChar = rkRecord.pChar;
				if( pChar == NULL ) continue;
				if( pChar->IsBoss() ) continue;
				if( pChar->GetState() == CS_OBSERVER ) continue;
				if( g_BattleRoomMgr.IsObserverUser( pChar->GetCharName() ) ) continue;

				iMaxPlayer++;
			}
			g_ModeUIController.SetBossRaidMode( true, GetMyBossContributeRank( pOwnerChar, iMaxPlayer ), iMaxPlayer, GetMyBossLevel() );
		}
	}
}

void ioBossMode::UpdateTimeScoreWnd()
{
	TimeScoreWnd *pTimeScoreWnd = dynamic_cast<TimeScoreWnd *>( g_GUIMgr.FindWnd( TIME_SCORE_WND ) );
	if(!pTimeScoreWnd) return;

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

	UpdateUIRank();
}

bool ioBossMode::GetRoundInfo( int iIndex, RoundInfo &rkInfo )
{
	if( ioPlayMode::GetRoundInfo( iIndex, rkInfo ) )
	{
		BossRecord *pRecord = (BossRecord*)FindModeRecord( iIndex );
		if( !pRecord )	return false;
		if( !pRecord->pChar ) return false;

		rkInfo.m_iContribute = pRecord->Contribute.m_iContribute;
		rkInfo.m_iContributePer = pRecord->Contribute.m_iPrevContributePer;
		return true;
	}
	return false;
}

bool ioBossMode::GetResultInfo( int iIndex, ResultInfo &rkInfo )
{
	if( ioPlayMode::GetResultInfo( iIndex, rkInfo ) )
	{
		BossRecord *pRecord = (BossRecord*)FindModeRecord( iIndex );
		if( !pRecord )	return false;
		if( !pRecord->pChar ) return false;

		rkInfo.m_iContribute = pRecord->Contribute.m_iContribute;
		rkInfo.m_iContributePer = pRecord->Contribute.m_iPrevContributePer;
		return true;
	}
	return false;
}

bool ioBossMode::GetFinalInfo( int iIndex, FinalInfo &rkInfo )
{
	if( ioPlayMode::GetFinalInfo( iIndex, rkInfo ) )
	{
		BossRecord *pRecord = (BossRecord*)FindModeRecord( iIndex );
		if( !pRecord )	return false;
		if( !pRecord->pChar ) return false;

		rkInfo.m_iContribute = pRecord->Contribute.m_iContribute;
		rkInfo.m_iContributePer = pRecord->Contribute.m_iPrevContributePer;
		return true;
	}
	return false;
}

void ioBossMode::SetRoundEndMotion()
{
	ContributeRoundAllPercent( 0, 0 );	

	int i = 0;
	int iCharCnt = GetRecordCharCnt();	
	for( i=0; i<iCharCnt; i++ )
	{
		BossRecord *pRecord = &m_vCharRecordList[i];
		if( !pRecord ) continue;
		if( !pRecord->pChar ) continue;
		if( pRecord->pChar->GetState() == CS_OBSERVER ) continue;
		if( g_BattleRoomMgr.IsObserverUser(pRecord->pChar->GetCharName()) ) continue;

		// 살아있으면 무조건 승리 ^^v
		if( pRecord->pChar->GetState() != CS_DIE )
		{
			m_eResultMotionWinTeam = pRecord->pChar->GetTeam();
			break;
		}
	}	

	for( i=0; i<iCharCnt; i++ )
	{
		BossRecord *pRecord = &m_vCharRecordList[i];
		if( !pRecord ) continue;
		if( !pRecord->pChar ) continue;
		if( pRecord->pChar->GetState() == CS_OBSERVER ) continue;
		if( g_BattleRoomMgr.IsObserverUser(pRecord->pChar->GetCharName()) ) continue;

		if( pRecord->pChar->GetTeam() == m_eResultMotionWinTeam )
		{
			pRecord->pChar->SetRoundEndMotion( RM_WIN );
		}
		else
		{
			pRecord->pChar->SetRoundEndMotion( RM_LOSE );
		}
		pRecord->pChar->StartNoInputDelayTime( 10000 );
	}
}

void ioBossMode::SetRoundEndVictories()
{
	int iCharCnt = GetRecordCharCnt();	
	for( int i=0; i<iCharCnt; i++ )
	{
		BossRecord *pRecord = &m_vCharRecordList[i];
		if( !pRecord->pChar ) continue;

		// 기여도 100%이상이 승리
		g_VictoriesMgr.SetVictories( pRecord->szName, ( pRecord->Contribute.m_iPrevContributePer >= 100 ) );
	}	
}

void ioBossMode::SetResultScoreWnd()
{	
	// 중간 결과가 존재하지 않는다.
}

void ioBossMode::SetTotalScoreWnd( IntVec &rvClassTypeList,
								   IntVec &rvClassPointList,
								   float fBlueGuildTeamBonus,
								   float fRedGuildTeamBonus,
								   float fBonusArray[BA_MAX] )
{
	TotalResultMainWnd *pTotalResult = dynamic_cast<TotalResultMainWnd *>(g_GUIMgr.FindWnd( TOTAL_RESULT_MAIN_WND ));
	if( !pTotalResult ) return;

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
	pTotalResult->SetSingleScore();
	m_dwCurResultShowDuration = m_dwFinalRoundResultDuration;

	SetTotalResultBriefingWnd();
}

void ioBossMode::SetResultBriefingWnd()
{
	// 없음
}

void ioBossMode::SetTotalResultBriefingWnd()
{
	// 개임 모드 최종 결과 브리핑
	ioBaseChar *pOwner = GetOwnerChar();
	if( pOwner )
	{
		BriefingWnd *pBriefingWnd = dynamic_cast<BriefingWnd*>(g_GUIMgr.FindWnd( BRIEFING_WND ));
		if( pBriefingWnd && !pBriefingWnd->IsShow() )
		{
			D3DXVECTOR3 vNewPos = ioMath::VEC3_ZERO;
			int iResultType = BriefingWnd::RESULT_DRAW;
			TeamType eOwenerTeam = pOwner->GetTeam();

			if( g_BattleRoomMgr.IsObserverUser( g_MyInfo.GetPublicID() ) || m_pCreator->IsStealthMode() )
			{
				ioBaseChar *pTargetChar = m_pCreator->GetBaseChar(m_szEndLookCharName);
				if( pTargetChar )
				{
					eOwenerTeam = pTargetChar->GetTeam();

					if(  m_eResultMotionWinTeam == eOwenerTeam )
						iResultType = BriefingWnd::RESULT_WIN;
					else 
						iResultType = BriefingWnd::RESULT_LOSE;
				}
				else
				{
					eOwenerTeam = eOwenerTeam;
					iResultType = BriefingWnd::RESULT_WIN;
				}

				if( IsNeedUpdateMapEffect( pTargetChar ) )
					vNewPos = pTargetChar->GetWorldPosition();
			}
			else
			{
				if(  m_eResultMotionWinTeam == eOwenerTeam )
					iResultType = BriefingWnd::RESULT_WIN;
				else 
					iResultType = BriefingWnd::RESULT_LOSE;

				if( IsNeedUpdateMapEffect( pOwner ) )
					vNewPos = pOwner->GetWorldPosition();
			}
			
			pBriefingWnd->SetResultInfo( m_pCreator, BriefingWnd::BRIEFING_FINAL_RESULT, iResultType, eOwenerTeam, m_iCurRoundCount );

			// 맵 이펙트 작렬!
			ioHashString szMapEffect = pBriefingWnd->GetFinalResultMapEffect( m_eResultMotionWinTeam );
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

void ioBossMode::RenderPreGUI()
{
	if( m_ModeState == MS_PLAY )
	{
		RenderPushEnableCursor();
	}
}

void ioBossMode::NotifyEntityDie( ioPlayEntity *pEntity, ioPlayEntity *pKiller, ioBaseChar *pBestAttacker,
								  float fLastRate, float fBestRate )
{
	ioBaseChar *pDieChar = ToBaseChar( pEntity );
	ioBaseChar *pKillChar = ToBaseChar( pKiller );

	OnNotifyCharDie( pDieChar, pKillChar, pBestAttacker, fLastRate, fBestRate );
	UpdateKillDeathRecord( pEntity, pKiller, pBestAttacker );
}

void ioBossMode::NotifyCharDropDie( ioBaseChar *pDieChar, ioBaseChar *pKillChar, ioBaseChar *pBestAttacker,
									float fLastRate, float fBestRate )
{
	OnNotifyCharDie( pDieChar, pKillChar, pBestAttacker, fLastRate, fBestRate );
	UpdateDropDieRecord( pDieChar, pKillChar, pBestAttacker );
}

void ioBossMode::UpdateDropDieRecord( ioBaseChar *pDieChar, ioBaseChar *pKillChar, ioBaseChar *pBestAttacker )
{
	if( pKillChar )
	{
		ModeRecord *pAttRecord = FindModeRecordCharName( pKillChar->GetCharName() );
		ModeRecord *pDieRecord = FindModeRecordCharName( pDieChar->GetCharName() );

		if( pAttRecord && pDieRecord )
		{
			TeamType eAttackerTeam = pDieChar->GetLastAttackerTeam();
			float fKillPoint = FLOAT05 + ( (float)GetKillPoint( eAttackerTeam ) * FLOAT05 );
			if( eAttackerTeam != pDieRecord->pChar->GetTeam() )
			{
				pAttRecord->AddKill( GetModeType(), fKillPoint, true, false );
				pDieRecord->AddDeath( GetModeType(), GetDeathPoint( pDieRecord->pChar->GetTeam() ) );

				if( eAttackerTeam != pAttRecord->pChar->GetTeam() )
				{
					g_ModeUIController.SetKOMsg( TEAM_NONE, "",
												 "", 0, pDieRecord->pChar->GetTeam(), pDieRecord->pChar->GetCharName() );
				}
				else
				{
					g_ModeUIController.SetKOMsg( eAttackerTeam, pAttRecord->pChar->GetCharName(), 
												 pDieRecord->pChar->GetLastAttackerSkillName(), pDieRecord->pChar->GetLastAttackerItemCode(),
												 pDieRecord->pChar->GetTeam(), pDieRecord->pChar->GetCharName() );
				}
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

					g_ModeUIController.SetKOMsg( TEAM_NONE, "", 
												 "", 0, pDieRecord->pChar->GetTeam(), pDieRecord->pChar->GetCharName() );
				}
			}

			if ( g_ModeUIController.IsCenterKillInfoWnd() )
				pAttRecord->pChar->SetKillEmoticon( pAttRecord->GetNoDeathKillCount() );
			else
				pAttRecord->pChar->SetKillEmoticon( pAttRecord->GetUniqueTotalKill() );
			pDieRecord->pChar->SetDeathEmoticon();

			ioBaseChar *pOwner = GetOwnerChar();
			if( pOwner )
				g_ModeUIController.SetCenterKillUI( m_ModeType, pAttRecord->GetNoDeathKillCount(), pOwner,pKillChar->GetTeam(), pKillChar->GetCharName(),
													pDieChar->GetLastAttackerItemCode(), pDieChar->GetTeam(), pDieChar->GetCharName(), pKillChar->IsMale() );

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
			pAttRecord->AddKill( GetModeType(), fKillPoint, true, true );
		}
	}
}

void ioBossMode::OnNotifyCharDie( ioBaseChar *pDieChar, ioBaseChar *pKillChar, ioBaseChar *pBestAttacker,
								  float fLastRate, float fBestRate )
{
	if( !pDieChar )
	{
		LOG.PrintTimeAndLog( 0, "ioBossMode::OnNotifyCharDie() - DieChar is NULL" );
		return;
	}

	BossRecord *pDieRecord = FindRecordCharName( pDieChar->GetCharName() );
	if( !pDieRecord )
	{
		LOG.PrintTimeAndLog( 0, "ioBossMode::OnNotifyCharDie - %s Not Exit Char",
								pDieChar->GetCharName().c_str() );
		return;
	}

	SetBossLevelUP();
	// 죽은 유저가 한번도 보스가 된적이 없으면 보스 기여도 초기화하지 않는다.
	if( IsBossHistory( pDieChar->GetCharName() ) )
		pDieRecord->iBossContribute = 0;
	

	if( P2PNetwork::IsNetworkPlaying() )
	{
		if( g_MyInfo.GetPublicID() == pDieChar->GetCharName() )
		{
			g_MyInfo.SetCharDied( pDieChar->GetSelectCharArray(), true );
		}
	}

	if( pKillChar && g_MyInfo.GetPublicID() != pDieChar->GetCharName() )
	{
		if( pBestAttacker && pBestAttacker->GetCharName() == pKillChar->GetCharName() )
		{

		}
		else
		{
			if( pBestAttacker )
			{
				if( pBestAttacker->IsOwnerChar() )
				{
					m_pCreator->StartScreenBlind( SBT_KILL, NULL, true );
				}
			}
		}
		
		if( pKillChar->IsBoss() && pKillChar->IsOwnerChar() )
			g_QuestMgr.QuestCompleteTerm( QC_BOSSMODE_BOSS_WITH_KILL );
	}

	//For EndScene
	m_LastDieChar = pDieChar->GetCharName();

	if( pKillChar )
	{
		BossRecord *pKillRecord = FindRecordCharName( pKillChar->GetCharName() );
		if( !pKillRecord )
		{
			LOG.PrintTimeAndLog( 0, "ioBossMode::OnNotifyCharDie - %s Not Exist Kill Char",
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

void ioBossMode::AddNewRecord( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	BossRecord *pBossRecord = FindRecordCharName( pChar->GetCharName() );
	if( !pBossRecord )
	{
		BossRecord kRecord;
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
		pBossRecord->pChar = pChar;
	}
}

void ioBossMode::RemoveRecord( const ioHashString &rkName )
{
	int iCharCnt = m_vCharRecordList.size();
	for( int i=0 ; i<iCharCnt ; i++ )
	{
		if( m_vCharRecordList[i].szName == rkName )
		{
			m_vCharRecordList.erase( m_vCharRecordList.begin() + i );
			RemoveBossHistory( rkName );
			break;
		}
	}

	UpdateUserRank();
}


void ioBossMode::RemoveRecord( int nIndex )
{
	int iCharCnt = m_vCharRecordList.size();
	if( iCharCnt > nIndex )
		m_vCharRecordList.erase( m_vCharRecordList.begin() + nIndex );
}

ioBaseChar* ioBossMode::GetRecordChar( int i )
{
	if( COMPARE( i, 0, GetRecordCharCnt() ) )
		return m_vCharRecordList[i].pChar;

	return NULL;
}

ioBaseChar* ioBossMode::GetRecordChar( const ioHashString &rkName )
{
	BossRecord *pRecord = FindRecordCharName( rkName );
	if( pRecord )
		return pRecord->pChar;

	return NULL;
}

int ioBossMode::GetRecordCharCnt() const
{
	return m_vCharRecordList.size();
}

ioBaseChar* ioBossMode::GetRecordCharTeamArray( TeamType eTeam, int iArray )
{
	int iCurCnt = 0;
	int iRecordCnt = GetRecordCharCnt();
	for( int i=0 ; i<iRecordCnt ; i++ )
	{
		ModeRecord *pRecord = FindModeRecord( i );
		if( !pRecord ) continue;
		if( !pRecord->pChar ) continue;
		if( pRecord->pChar->GetState() == CS_OBSERVER ) continue;
		if( g_BattleRoomMgr.IsObserverUser( pRecord->pChar->GetCharName() ) ) continue;
		
		//첫번째는 무조건 보스다
		if( iArray == 0 && pRecord->pChar->IsBoss() )     
			return pRecord->pChar;
		else if( pRecord->pChar->GetTeam() == TEAM_RED )
		{
			if( iCurCnt == iArray - 1 )
				return pRecord->pChar;
			iCurCnt++;
		}
	}

	return NULL;
}

ModeRecord* ioBossMode::FindModeRecord( int iIdx )
{
	if( COMPARE( iIdx, 0, GetRecordCharCnt() ) )
		return &m_vCharRecordList[iIdx];

	return NULL;
}

ModeRecord* ioBossMode::FindModeRecordCharName( const ioHashString &rkName )
{
	return (ModeRecord*)FindRecordCharName( rkName );
}

BossRecord* ioBossMode::FindRecordCharName( const ioHashString &rkName )
{
	int iCharCnt = m_vCharRecordList.size();
	for( int i=0 ; i<iCharCnt ; i++ )
	{
		if( m_vCharRecordList[i].szName == rkName )
			return &m_vCharRecordList[i];
	}

	return NULL;
}

bool ioBossMode::ProcessUDPPacket( SP2Packet &rkPacket )
{
	if( ioPlayMode::ProcessUDPPacket( rkPacket ) )
		return true;

	return false;
}

void ioBossMode::ApplyModeInfo( SP2Packet &rkPacket )
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

void ioBossMode::ApplyCharModeInfo( const ioHashString &rkName, SP2Packet &rkPacket, bool bDieCheck )
{
	bool bRecord;
	rkPacket >> bRecord;

	if( !bRecord ) return;

	BossRecord *pRecord = FindRecordCharName( rkName );
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

void ioBossMode::OnFinalRoundResult( SP2Packet &rkPacket )
{
	// 최종 결과 패킷을 받을 때 시상식 Show함 . 내부적으로 수상자가 없으면 Hide시킨다.
	DWORD dwTotalResultTime;
	rkPacket >> dwTotalResultTime;
	SetAutoStartAlarmTime( dwTotalResultTime );

	float fBlueGuildTeamBonus = FLOAT1;
	float fRedGuildTeamBonus = FLOAT1;
	if( m_pCreator->GetRoomStyle() == RSTYLE_LADDERBATTLE )
	{
		// 양팀 랭킹 
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
			pRecord->bResultLevelUP = bLevelUP;
			pRecord->iResultLevel   = iGradeLevel;
			pRecord->iResultTotalExp= iTotalExp;
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
}

int ioBossMode::GetRoundWinCnt( TeamType eTeamType )
{
	return 0;
}

void ioBossMode::CheckRoundEnd()
{
}

void ioBossMode::ContributeConvertTeam( TeamType eTeam )
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
		BossRecord *pRecord = &m_vCharRecordList[i];
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
			BossRecord *pRecord = &m_vCharRecordList[iArray];
			if( !pRecord->Contribute.m_bLastResultSync )
				pRecord->Contribute.m_iContribute = m_vContributeTable[vInfo[i].m_iRank];
			pRecord->Contribute.m_iKillRank = vInfo[i].m_iRank;
		}
	}
	vInfo.clear();

	//데미지 & 힐
	for(i = 0;i < iCharCnt;i++)
	{
		BossRecord *pRecord = &m_vCharRecordList[i];
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
			BossRecord *pRecord = &m_vCharRecordList[iArray];
			if( !pRecord->Contribute.m_bLastResultSync )
				pRecord->Contribute.m_iContribute += m_vContributeTable[vInfo[i].m_iRank] * 6;
			pRecord->Contribute.m_iDamageRank = vInfo[i].m_iRank;
		}
	}
	vInfo.clear();

	//유효 타격 횟수
	for(i = 0;i < iCharCnt;i++)
	{
		BossRecord *pRecord = &m_vCharRecordList[i];
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
			BossRecord *pRecord = &m_vCharRecordList[iArray];
			if( !pRecord->Contribute.m_bLastResultSync )
				pRecord->Contribute.m_iContribute += m_vContributeTable[vInfo[i].m_iRank] * 2;
			pRecord->Contribute.m_iAttackCntRank = vInfo[i].m_iRank;
		}
	}
	vInfo.clear();

	//데쓰처리
	for(i = 0;i < iCharCnt;i++)
	{
		BossRecord *pRecord = &m_vCharRecordList[i];
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
			BossRecord *pRecord = &m_vCharRecordList[iArray];
			if( !pRecord->Contribute.m_bLastResultSync )
				pRecord->Contribute.m_iContribute += m_vContributeTable[vInfo[i].m_iRank];
			pRecord->Contribute.m_iDeathRank = vInfo[i].m_iRank;
		}
	}
	vInfo.clear();
}

void ioBossMode::ContributeRoundAllPercent( DWORD dwCurTime, DWORD dwTotalTime )
{
	//기여도의 100분율
	int iMaxContribute	= 0;
	int iCharCnt		= m_vCharRecordList.size();
	int iPlayCharCnt	= 0;

	int i = 0;
	for(i = 0;i < iCharCnt;i++)
	{
		BossRecord *pRecord = &m_vCharRecordList[i];
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
			BossRecord *pRecord = &m_vCharRecordList[i];
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

void ioBossMode::ContributeConvert()
{
	ContributeConvertTeam( TEAM_NONE );
}

int ioBossMode::ContributeMyRank( int &rRankValue1, int &rRankValue2, int &rRankValue3, int &rRankValue4 )
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
		BossRecord *pRecord = &m_vCharRecordList[i];
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
		BossRecord *pRecord = &m_vCharRecordList[i];
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
		BossRecord *pRecord = &m_vCharRecordList[i];
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

void ioBossMode::DamageContribute( const ioHashString szAttacker, const ioHashString szTarget, float fDamage, DWORD dwItemCode, int iWeaponType )
{
	if( szAttacker.IsEmpty() ) return;

	BossRecord *pRecord = FindRecordCharName( szAttacker );
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

	// 보스 타격시 기여 처리
	if( pTarget && pRecord && pTarget->GetTeam() == TEAM_BLUE )
	{
		/********************************/
		/* A = 보스잡이 인원 - 1        */
		/* B = FLOAT05                     */
		/* C = ( ( A - 랭킹 ) / A ) x B */
		/********************************/
		int iMaxBossCatchUser = 0;
		RecordList::iterator iter;
		for( iter=m_vCharRecordList.begin() ; iter!=m_vCharRecordList.end() ; ++iter )
		{
			BossRecord &rkRecord = *iter;
			ioBaseChar *pChar = rkRecord.pChar;
			if( pChar == NULL ) continue;
			if( pChar->IsBoss() ) continue;
			if( pChar->GetState() == CS_OBSERVER ) continue;
			if( g_BattleRoomMgr.IsObserverUser( pChar->GetCharName() ) ) continue;

			iMaxBossCatchUser++;
		}

		//데미지의 10%만이 기여에 적용된다.
		float fBossContribute = (float)fDamage * 0.1f; 
		if( iMaxBossCatchUser > 1 )
		{			
			int   iBossRank = GetMyBossContributeRank( pRecord->pChar, iMaxBossCatchUser );						
			int   iA = iMaxBossCatchUser - 1;		
			float fB = FLOAT05;
			float fC = FLOAT1 - ( ( (float)( iA - iBossRank ) / iA ) * fB );
			fBossContribute = fBossContribute * fC;
		}
		pRecord->iBossContribute += fBossContribute; 
	}
}

void ioBossMode::KillContribute( const ioHashString szKiller, const ioHashString szKilled, DWORD dwItemCode )
{
	if( szKilled.IsEmpty() ) return;

	int iKillPoint = 100;

	float fAddKillPoint = FLOAT1;
	if( IsContributeAddPointItemCode( dwItemCode ) )
		fAddKillPoint = 1.1f;

    if( szKiller.IsEmpty() || szKiller == szKilled )     //자살
	{
		BossRecord *pRecord = FindRecordCharName( szKilled );
		if( !pRecord || !pRecord->pChar ) return;

		pRecord->Contribute.m_iKill -= iKillPoint;
		pRecord->Contribute.m_iDeath++;
	}
	else
	{
		BossRecord *pKillerRecord = FindRecordCharName( szKiller );
		BossRecord *pKilledRecord = FindRecordCharName( szKilled );
		if( pKillerRecord && pKilledRecord )
		{
			if( pKillerRecord->pChar && pKilledRecord->pChar )
			{
				TeamType eAttackerTeam = pKilledRecord->pChar->GetLastAttackerTeam();
				if( eAttackerTeam == pKilledRecord->pChar->GetTeam() )    //팀킬
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

void ioBossMode::AttackCntContribute( const ioHashString szAttacker, const ioHashString szTarget, DWORD dwItemCode, int iAttackValue/* = 100 */ )
{
	if( szAttacker.IsEmpty() || szTarget.IsEmpty() ) return;

	BossRecord *pAttackerRecord = FindRecordCharName( szAttacker );
	BossRecord *pTargetRecord = FindRecordCharName( szTarget );

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

	// 보스 타격시 기여 처리
	if( pTarget && pAttackerRecord && pTarget->GetTeam() == TEAM_BLUE )
	{
		pAttackerRecord->iBossContribute++;
	}
}

void ioBossMode::ContributeLOG()
{
	char szLog[MAX_PATH];

	int iCharCnt = m_vCharRecordList.size();
	for( int i=0 ;i<iCharCnt; i++ )
	{
		BossRecord *pRecord = &m_vCharRecordList[i];
		if( pRecord->pChar )
		{
			sprintf( szLog, "%s[%d] : K%d(%d) D%d(%d) A%d(%d) D%d(%d) Boss(%d)", pRecord->szName.c_str(), (int)pRecord->pChar->GetTeam(),
																                 pRecord->Contribute.m_iKillRank + 1, pRecord->Contribute.m_iKill,
																                 pRecord->Contribute.m_iDamageRank + 1, pRecord->Contribute.m_iDamage,
																                 pRecord->Contribute.m_iAttackCntRank + 1, pRecord->Contribute.m_iAttackCnt,
																		         pRecord->Contribute.m_iDeathRank + 1, pRecord->Contribute.m_iDeath,
																				 pRecord->iBossContribute );
			g_ChatMgr.SetSystemMsg( szLog );
		}
	}
}


void ioBossMode::FillLastPlayRecordInfo( SP2Packet &rkPacket )
{
	int iCharCnt = m_vCharRecordList.size();
	rkPacket << iCharCnt;
	for(int i = 0;i < iCharCnt;i++)
	{
		BossRecord *pRecord = &m_vCharRecordList[i];
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

void ioBossMode::FillPlayRecordInfo( SP2Packet &rkPacket )
{
	ioBaseChar *pBossChar = NULL;

	int i = 0;
	int iCharCnt = m_vCharRecordList.size();
	rkPacket << iCharCnt;
	for(i = 0;i < iCharCnt;i++)
	{
		BossRecord *pRecord = &m_vCharRecordList[i];
		rkPacket << pRecord->pChar->GetCharName();
		rkPacket << pRecord->Contribute.m_iDamage;
		rkPacket << pRecord->Contribute.m_iAttackCnt;
		rkPacket << pRecord->Contribute.m_iKill;
		rkPacket << pRecord->Contribute.m_iDeath;
		rkPacket << pRecord->Contribute.m_iPrevContributePer;
		rkPacket << pRecord->GetUniqueTotalKill();
		rkPacket << pRecord->GetUniqueTotalDeath();
		rkPacket << g_VictoriesMgr.GetVictories( pRecord->pChar->GetCharName() );
		rkPacket << pRecord->iBossContribute;       // 보스 공략에 기여한 수치
		rkPacket << pRecord->GetNoDeathKillCount();

		// 보스 정보는 마지막에 보낸다.
		if( pRecord->pChar && pRecord->pChar->GetTeam() == TEAM_BLUE )
			pBossChar = pRecord->pChar;
	}

	// 보스 히스토리
	int iBossHistorySize = m_vBossHistory.size();
	rkPacket << iBossHistorySize;
	for(i = 0;i < iBossHistorySize;i++)
		rkPacket << m_vBossHistory[i];
	
	// 보스 정보
	if( pBossChar )
	{
		rkPacket << pBossChar->GetCharName() << GetCurBossLevel();
	}
}

void ioBossMode::ApplyPlayRecordInfo( SP2Packet &rkPacket )
{
	int iCharCnt;
	rkPacket >> iCharCnt;
	for(int i = 0;i < iCharCnt;i++)
	{
		ioHashString szName;
		int iDamage, iAttackCnt, iKill, iDeath, iPrevContributePer, iUniqueTotalKill, iUniqueTotalDeath, iVictories, iBossContribute, iNoDeathKillCount;
		rkPacket >> szName >> iDamage >> iAttackCnt >> iKill >> iDeath >> iPrevContributePer >> iUniqueTotalKill >> iUniqueTotalDeath >> iVictories >> iBossContribute >> iNoDeathKillCount;

		BossRecord *pRecord = FindRecordCharName( szName );
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
			pRecord->iBossContribute  = iBossContribute;
		}
	}

	// 보스 히스토리
	int iBossHistorySize;
	rkPacket >> iBossHistorySize;
	for(i = 0;i < iBossHistorySize;i++)
	{
		ioHashString szHisName;
		rkPacket >> szHisName;
		InsertBossHistory( szHisName );
	}

	ContributeConvert();
	ContributeRoundAllPercent();
	UpdateRoundInfoList();
	CheckTotalChamp();

	// 보스 처리
	ioHashString szBossName;
	rkPacket >> szBossName;

	BossRecord *pRecord = FindRecordCharName( szBossName );
	if( !pRecord || !pRecord->pChar ) return;
    if( pRecord->pChar->GetTeam() != TEAM_BLUE ) return;

	rkPacket >> m_iCurBossLevel;
	if( szBossName == g_MyInfo.GetPublicID() )
		m_iMyBossLevel = m_iCurBossLevel;
}

void ioBossMode::ContributeResultSync( const ioHashString &szName, int iContribute )
{
	BossRecord *pRecord = FindRecordCharName( szName );
	if( !pRecord ) return;

	pRecord->Contribute.m_bLastResultSync = true;
	pRecord->Contribute.m_iContribute     = iContribute;
}

void ioBossMode::ShowEndScoreWnd( bool bShow )
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

D3DXVECTOR3 ioBossMode::GetStartPos( TeamType eTeamType, bool bRandom )
{
	int iPosArray = -1;
	int iSinglePosCnt = m_SingleTeamPosArray.size();
	int iStartPosCnt  = m_vStartPos.size();
	
	// 랜덤 시작 위치
	if( m_RandomPosArray.empty() )
	{
		for(int i = 0;i < iSinglePosCnt;i++)
			m_RandomPosArray.push_back( m_SingleTeamPosArray[i] );
	}
	
	if( iStartPosCnt > 0 && iSinglePosCnt > 0 && !m_RandomPosArray.empty() )
	{
		int iStartPos = m_RandomPosArray[0];
		iPosArray = m_SingleTeamPosArray[iStartPos];
		m_RandomPosArray.erase( m_RandomPosArray.begin() );
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
	
	vCenter.y = GetMapHeightByRevial( vCenter.x, vCenter.z );
	return vCenter;
}


D3DXVECTOR3 ioBossMode::GetRevivalPos( ioBaseChar *pChar )
{
	if(!pChar)
	{
		LOG.PrintTimeAndLog( 0, "ioBossMode::GetRevivalPos() - Char is NULL" );
		return m_vMapCenter;
	}

	// 마지막 타격 유저가 있는 보스의 부활 위치
	if( pChar->GetTeam() == TEAM_BLUE )
	{
		int iPosArray = 0;
		PositionDistanceSqList kPosDistance;
		TeamType eCharTeam = pChar->GetTeam();

		ioBaseChar *pLastAttacker = GetRecordChar( pChar->GetLastAttackerName() );
		if( pLastAttacker )
			GetMostNearPosList( kPosDistance, pLastAttacker->GetWorldPosition() );
		else
			GetMostNearPosList( kPosDistance, pChar->GetWorldPosition() );

		if( kPosDistance.empty() )
			return GetStartPos( eCharTeam );

		iPosArray = kPosDistance.front().iIndex;

		int iMaxRevivalPos = m_vRevivalPos.size();
		if( !COMPARE( iPosArray, 0, iMaxRevivalPos ) )
			return m_vMapCenter;

		D3DXVECTOR3 vCenter = m_vRevivalPos[iPosArray];
		vCenter.y = GetMapHeightByRevial( vCenter.x, vCenter.z );

		return vCenter;
	}
	
	// 일반 유저 부활 위치
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

void ioBossMode::GetMostNearPosList( PositionDistanceSqList &rkResultList, const D3DXVECTOR3 &vTargPos )
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

void ioBossMode::UpdateRoundInfoList()
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

void ioBossMode::UpdateUIRank()
{
	TimeScoreWnd *pTimeScoreWnd = dynamic_cast<TimeScoreWnd *>( g_GUIMgr.FindWnd( TIME_SCORE_WND ) );
	if( !pTimeScoreWnd ) return;

	BossRecord *pRecord = FindRecordCharName( g_MyInfo.GetPublicID() );
	if( g_BattleRoomMgr.IsObserverUser( g_MyInfo.GetPublicID() ) || m_pCreator->IsStealthMode() )
	{
		ioBaseChar *pTarget = m_pCreator->GetCameraTargetChar();
		if( pTarget )
		{
			pRecord = FindRecordCharName( pTarget->GetCharName() );
			pTimeScoreWnd->UpdateMyTeam( pTarget->GetTeam() );
		}
	}
	else
	{
		ioBaseChar *pOwnerChar = m_pCreator->GetOwnerChar();
		if( pOwnerChar )
			pTimeScoreWnd->UpdateMyTeam( pOwnerChar->GetTeam() );
	}

	if( pRecord )
	{
		pTimeScoreWnd->UpdateScore( pRecord->iCurRank, 0 );
	}
}

int ioBossMode::GetCurRank( const ioHashString& szCharName )
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

int ioBossMode::GetContributePerChar( const ioHashString& szName )
{
	int iCharCnt = m_vCharRecordList.size();

	for(int i = 0;i < iCharCnt;i++)
	{
		BossRecord *pRecord = &m_vCharRecordList[i];
		if( !pRecord )	continue;
		if( !pRecord->pChar ) continue;

		if( pRecord->pChar->GetCharName() == szName )
		{
			return pRecord->Contribute.m_iPrevContributePer;
		}
	}

	return 0;
}

bool ioBossMode::IsNeedRevival( ioBaseChar *pChar )
{
	return true;
}

void ioBossMode::CheckEndSceneLookAt()
{
/*	// 옵저버는 다르게 처리
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
		BossRecord *pKillRecord = FindRecordCharName( m_LastKillChar );
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
			BossRecord *pDieRecord = FindRecordCharName( m_LastDieChar );
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


ioBaseChar* ioBossMode::CheckMostNearChar( ioBaseChar *pChar )
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

void ioBossMode::SetBossLevelUP()
{
	ioBaseChar *pBossChar = NULL;
	RecordList::iterator iter;
	for( iter=m_vCharRecordList.begin() ; iter!=m_vCharRecordList.end() ; ++iter )
	{
		ioBaseChar *pCurChar = iter->pChar;
		if( !pCurChar ) continue;
		if( pCurChar->GetState() == CS_OBSERVER ) continue;
		if( g_BattleRoomMgr.IsObserverUser( pCurChar->GetCharName() ) ) continue;
		if( pCurChar->GetState() == CS_DIE ) continue;
		if( pCurChar->GetTeam() == TEAM_RED ) continue;

		pBossChar = pCurChar;
		break;
	}

	if( pBossChar )
	{
		// 레벨업
		m_iCurBossLevel = min( m_iCurBossLevel + 1, m_iMaxBossLevel );
		float fLevel = (float)GetCurBossLevel() - 1;

		// 레벨에 따른 HP 회복
		float fRecoverPer = m_fBossRecoverHP - ( m_fBossRecoverLevelGapHP * fLevel );
		pBossChar->SetKOHPRecover( fRecoverPer, m_PerfectRecoverEffect );
		pBossChar->SetBossLevelEmoticon( GetCurBossLevel() );

		// 레벨에 따른 무적
		DWORD dwProtectTime = m_dwBossProtectedTime - min( m_dwBossProtectedTime, ( m_dwBossProtectedLevelGapTime * fLevel ) );
		pBossChar->SetProtectionMode( PM_BOSS, dwProtectTime );

		// 밸런스 적용
		m_pCreator->ReCalculateDamageRate( false );

		if( pBossChar->IsOwnerChar() )
			m_iMyBossLevel = m_iCurBossLevel;
	}
}

void ioBossMode::SetBossProtected( ioBaseChar *pChar )
{	
	if( !pChar ) return;
	if( pChar->GetTeam() != TEAM_BLUE ) return;

	if( pChar )
	{
		pChar->SetProtectionMode( PM_BOSS, m_dwBossChangeProtectedTime );
	}
}

void ioBossMode::InsertBossHistory( const ioHashString &rkName )
{
	if( IsBossHistory( rkName ) ) return;

	m_vBossHistory.push_back( rkName );
}

void ioBossMode::RemoveBossHistory( const ioHashString &rkName )
{
	int iSize = m_vBossHistory.size();
	for(int i = 0;i < iSize;i++)
	{
		if( m_vBossHistory[i] == rkName )
		{
			m_vBossHistory.erase( m_vBossHistory.begin() + i );
			return;
		}
	}
}

bool ioBossMode::IsBossHistory( const ioHashString &rkName )
{
	int iSize = m_vBossHistory.size();
	for(int i = 0;i < iSize;i++)
	{
		if( m_vBossHistory[i] == rkName )
			return true;
	}
	return false;
}

bool ioBossMode::ProcessTCPPacket( SP2Packet &rkPacket )
{
	ErrorReport::SetPosition( 1060, 1 );
	if( ioPlayMode::ProcessTCPPacket( rkPacket ) )
		return true;

	ErrorReport::SetPosition( 1060, 2 );

	switch( rkPacket.GetPacketID() )
	{
	case STPK_CHANGE_BOSS:
		OnChangeBoss( rkPacket );
		return true;
	}

	return false;
}

void ioBossMode::OnChangeBoss( SP2Packet &rkPacket )
{
	ioHashString szBossName;
	rkPacket >> szBossName >> m_iCurBossLevel;

	RecordList::iterator iter;
	for( iter=m_vCharRecordList.begin() ; iter!=m_vCharRecordList.end() ; ++iter )
	{
		BossRecord &rkRecord = *iter;
		ioBaseChar *pChar = rkRecord.pChar;
		if( !pChar ) continue;
		if( pChar->GetState() == CS_OBSERVER ) continue;
		if( g_BattleRoomMgr.IsObserverUser(pChar->GetCharName()) ) continue;

		if( szBossName == pChar->GetCharName() )
		{			
			// 보스 세팅
			rkRecord.iBossContribute = 0;    // 보스만 보스 기여도 초기화
			pChar->SetTeam( TEAM_BLUE );
			pChar->SetKOHPRecover( m_fBossChangeRecoverHP, m_PerfectRecoverEffect ); //FULL HP
			pChar->SetBossLevelEmoticon( GetCurBossLevel() );
			SetBossProtected( pChar );
			InsertBossHistory( szBossName );

			if( pChar->IsOwnerChar() )
			{
				m_iMyBossLevel = m_iCurBossLevel;
				g_QuestMgr.QuestCompleteTerm( QC_BOSSMODE_BECOME_BOSS );
			}
		}
		else
		{
			// 비보스 세팅
			pChar->SetTeam( TEAM_RED );
		}
	}
	m_pCreator->ReCalculateDamageRate( false );
}

void ioBossMode::OnZeroHP( SP2Packet &rkPacket )
{
	rkPacket >> m_dwSuddenDeathTime;
	m_dwSuddenDaathStartTime = FRAMEGETTIME();

	int iCharCnt = m_vCharRecordList.size();
	for( int i=0 ; i<iCharCnt ; i++ )
	{
		m_vCharRecordList[i].pChar->SetCurHP( 0.0f );
		m_vCharRecordList[i].pChar->SetCurHPRate( 0.0f );
		m_vCharRecordList[i].dwCurDieTime = 0;
		g_ModeHelpMgr.HideHelp( ioModeHelpManager::HT_REVIVING );
	}
	m_bZeroHP = true;
	g_SoundMgr.PlaySound( m_TimeOutSound, DSBVOLUME_MAX, PSM_LOOP | PSM_THREAD );
	g_ModeUIController.SetDeathTime();
}

void ioBossMode::UpdateRevivalTime( const ioHashString &szName, DWORD dwCurRevivalTime )
{
	ModeRecord *pDieRecord = FindModeRecordCharName( szName );
	if( !pDieRecord )
	{
		LOG.PrintTimeAndLog( 0, "ioBossMode::UpdateRevivalTime() - %s is not exist to the record", szName.c_str() );
		return;
	}

	m_dwCurRevivalTime = dwCurRevivalTime;

	pDieRecord->dwCurDieTime = FRAMEGETTIME();
	pDieRecord->dwRevivalGap = m_dwCurRevivalTime;
	pDieRecord->iRevivalCnt++;

	if( pDieRecord->pChar )
	{
		if( pDieRecord->pChar->IsOwnerChar() )
		{
			if( IsZeroHP() )
				SetBlindMode( true );
			else
				g_ModeHelpMgr.ShowHelp( ioModeHelpManager::HT_REVIVING,  FRAMEGETTIME() + m_dwCurRevivalTime );
		}
	}
}


void ioBossMode::OnRoundJoinView( SP2Packet &rkPacket )
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