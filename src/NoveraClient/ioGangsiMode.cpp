

#include "stdafx.h"

#include "ioGangsiMode.h"
#include "ioLadderTeamMgr.h"
#include "ioRankBattleManager.h"
#include "ioShuffleRoomMgr.h"

#include "GUI/MiniSoldierSelectWnd.h"
#include "GUI/MiniMap.h"
#include "GUI/TimeScoreWnd.h"
#include "GUI/BriefingWnd.h"
#include "GUI/ioObserverWnd.h"
#include "GUI/TotalResultMainWnd.h"
#include "GUI/RoundStateMainWnd.h"
#include "GUI/CenterKillInfoWnd.h"

ioGangsiMode::ioGangsiMode( ioPlayStage *pCreator ) : ioPlayMode( pCreator )
{
	m_ModeType = MT_GANGSI;

	m_SingleTeamPosArray.clear();
	m_dwCenterUIDelayTime = 0;
	m_eResultMotionWinTeam = TEAM_NONE;

	m_fChangeGangsiHPRate = 0;
	m_dwGangsiBuffTime = m_dwUserBuffTime = 0;
	m_dwGangsiSurvivorTime = 0;
	m_iChangeGangsiSkinMan = m_iChangeGangsiSkinWoman = 0;
	m_dwAlarmTickTime = m_dwNextAlarmTick = m_dwAlarmWarningTime = m_dwWarningSoundTime = 0;

	m_vCharRecordList.reserve( 100 );
}

ioGangsiMode::~ioGangsiMode()
{
	ClearMode();
}

void ioGangsiMode::LoadModeResource( ioINILoader &rkLoader )
{	
	TimeScoreWnd *pTimeScoreWnd = dynamic_cast<TimeScoreWnd *>( g_GUIMgr.AddWnd( "XML/TimeScoreWnd.xml") );
	if( pTimeScoreWnd )
		pTimeScoreWnd->SetModeType( GetModeType() );

	g_ModeUIController.SetKOMsgWnd( g_GUIMgr.AddWnd( "XML/kochatmsgwnd.xml") );
	g_ModeUIController.SetCenterInfoWnd( g_GUIMgr.AddWnd( "XML/CenterInfoWnd.xml" ) );

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

void ioGangsiMode::LoadMap()
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

void ioGangsiMode::LoadContribute()
{
	ioPlayMode::LoadContribute();
}

void ioGangsiMode::InitMode( int iMapIndex )
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

	kLoader.LoadString_e( "ui_info", "1point_up_emoticon", "", szBuf, MAX_PATH );
	m_PointUpEmoticon = szBuf;

	kLoader.SetTitle_e( "gangsi" ); //
	m_fChangeGangsiHPRate = kLoader.LoadFloat_e( "change_gangsi_hp_rate", 0 );
	m_iChangeGangsiSkinMan  = kLoader.LoadFloat_e( "change_gangsi_skin_m", 0 );
	m_iChangeGangsiSkinWoman= kLoader.LoadFloat_e( "change_gangsi_skin_w", 0 );
	m_dwAlarmTickTime     = kLoader.LoadInt_e( "alarm_tick_time", 0 );
	m_dwAlarmWarningTime  = kLoader.LoadInt_e( "alarm_warning_time", 0 );
	m_dwWarningSoundTime  = kLoader.LoadInt_e( "warning_sound_time", 0 );

	kLoader.LoadString_e( "host_skeleton_m", "", szBuf, MAX_PATH );
	m_szHostGangsiSkeletonMan = szBuf;
	kLoader.LoadString_e( "host_skeleton_w", "", szBuf, MAX_PATH );
	m_szHostGangsiSkeletonWoman = szBuf;
	kLoader.LoadString_e( "infection_skeleton_m", "", szBuf, MAX_PATH );
	m_szInfectionGangsiSkeletonMan = szBuf;
	kLoader.LoadString_e( "infection_skeleton_w", "", szBuf, MAX_PATH );
	m_szInfectionGangsiSkeletonWoman = szBuf;
	
	int i = 0;
	m_HostGangsiItemCode.clear();
	m_InfectionGangsiItemCode.clear();
	int iMaxItem = kLoader.LoadInt_e( "max_host_item", 0 );
	for(i = 0;i < iMaxItem;i++)
	{
		char szKey[MAX_PATH] = "";
		sprintf_e( szKey, "host_item_%d", i + 1 );
		m_HostGangsiItemCode.push_back( kLoader.LoadInt( szKey, 0 ) );
	}

	iMaxItem = kLoader.LoadInt_e( "max_infection_item", 0 );
	for(i = 0;i < iMaxItem;i++)
	{
		char szKey[MAX_PATH] = "";
		sprintf_e( szKey, "infection_item_%d", i + 1 );
		m_InfectionGangsiItemCode.push_back( kLoader.LoadInt( szKey, 0 ) );
	}

	{
		// 강시 버프
		kLoader.SetTitle_e( "gangsi_buff" );
		int iMaxBuff = kLoader.LoadInt_e( "max_buff_list", 0 );
		for(i = 0;i < iMaxBuff;i++)
		{
			char szKey[MAX_PATH] = "";

			BuffList kBuffList;
			sprintf_e( szKey, "buff%d_time", i + 1 );
			kBuffList.m_dwCheckTime = kLoader.LoadInt( szKey, 0 );

			sprintf_e( szKey, "max_buff%d", i + 1 );
			int iMaxList = kLoader.LoadInt( szKey, 0 );
			for(int j = 0;j < iMaxList;j++)
			{
				sprintf_e( szKey, "buff%d_list%d", i + 1, j + 1 );
				kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
				kBuffList.m_BuffList.push_back( szBuf );
			}
			m_GangsiBuff.push_back( kBuffList );
		}
	}

	{
		// 유저 버프
		kLoader.SetTitle_e( "user_buff" );
		int iMaxBuff = kLoader.LoadInt_e( "max_buff_list", 0 );
		for(i = 0;i < iMaxBuff;i++)
		{
			char szKey[MAX_PATH] = "";

			BuffList kBuffList;
			sprintf_e( szKey, "buff%d_time", i + 1 );
			kBuffList.m_dwCheckTime = kLoader.LoadInt( szKey, 0 );

			sprintf_e( szKey, "max_buff%d", i + 1 );
			int iMaxList = kLoader.LoadInt( szKey, 0 );
			for(int j = 0;j < iMaxList;j++)
			{
				sprintf_e( szKey, "buff%d_list%d", i + 1, j + 1 );
				kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
				kBuffList.m_BuffList.push_back( szBuf );
			}
			m_UserBuff.push_back( kBuffList );
		}
	}

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

void ioGangsiMode::ReStartMode()
{
	LOG.PrintTimeAndLog( 0, "ioGangsiMode::ReStartMode Is Call???" );
}

void ioGangsiMode::ClearModeData()
{
	ioPlayMode::ClearModeData();
	m_LastDieChar.Clear();
	m_LastKillChar.Clear();
}

void ioGangsiMode::ProcessMode()
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

void ioGangsiMode::ProcessReadyState()
{
	if( P2PNetwork::IsNetworkPlaying() )	return;
	if( m_dwReadyStartTime + 4000 > FRAMEGETTIME() ) return;

	SetModeState( MS_PLAY );
}

void ioGangsiMode::ProcessPlayState()
{
	UpdateSoldierRevivalPenalty();
	//UpdateTimeGrowthTime();

	m_pCreator->NotifySpringDampUseSet( true );

	ProcessChangeGangsiState();
	ProcessAlarmTickTime();
	ProcessGangsiSurvivorTime();
	ProcessRevival();
	UserBuffProcess();
	GangsiBuffProcess();
	
	if( !P2PNetwork::IsNetworkPlaying() )
	{
		CheckRoundEnd();
	}
}

void ioGangsiMode::ProcessResultState()
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

void ioGangsiMode::ProcessGUI()
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

void ioGangsiMode::ProcessRevival()
{
	DWORD dwCurTime = FRAMEGETTIME();
	
	int iCharCnt = m_vCharRecordList.size();
	for( int i=0 ; i<iCharCnt ; i++ )
	{
		GangsiRecord &rkRecord = m_vCharRecordList[i];
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


void ioGangsiMode::ProcessCharDropZoneCollision()
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


void ioGangsiMode::FillOwnerDamagedListByOtherTeam( const ioHashString &szTarget, TeamType eTeam, SP2Packet &rkPacket, bool bAttach )
{
	// 데미지 리스트 합치기
	if( bAttach )
	{
		DamageTable *pDamageTable = FindDamageList( szTarget );
		if( pDamageTable )
			pDamageTable->AttachAllDamage();
	}
}

void ioGangsiMode::UpdateMiniMapSpotPoint()
{
	MiniMap *pMiniMap = dynamic_cast<MiniMap*>(g_GUIMgr.FindWnd( MINIMAP_WND ));
	if( !pMiniMap )	return;

	ioPlayMode::UpdateMiniMapSpotPoint();

	pMiniMap->ClearPointList();

	D3DXVECTOR3 vPos;
	DWORD dwGapTime = 0;

	TeamType eMyTeam = GetOwnerTeam();
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

	// Dummy
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

void ioGangsiMode::UpdateRoundStateWnd()
{
	if( g_MyInfo.IsSortContribute() )
	{
		DWORD dwTimeGap = FRAMEGETTIME() - m_dwPlayStartTime;
		if( m_dwCurRoundDuration > dwTimeGap )
			ContributeRoundAllPercent( m_dwRoundDuration - (m_dwCurRoundDuration - dwTimeGap), m_dwRoundDuration );
		else
			ContributeRoundAllPercent( m_dwRoundDuration, m_dwRoundDuration );

		GangsiRecord *pRecord = FindRecordCharName( g_MyInfo.GetPublicID() );
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

			pRoundStateWnd->SetGangsiScore();
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

void ioGangsiMode::UpdateCenterInfoWnd()
{
}

void ioGangsiMode::UpdateTimeScoreWnd()
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
			pTimeScoreWnd->UpdateTime( 0, m_dwRoundDuration );
		}

		// 30초 전부터 긴박음
		if( m_dwCurRoundDuration - dwTimeGap <= m_dwWarningSoundTime )
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
		ProcessScoreWarningSound( false );
		break;
	}

	UpdateUIRank();
}

bool ioGangsiMode::GetRoundInfo( int iIndex, RoundInfo &rkInfo )
{
	if( ioPlayMode::GetRoundInfo( iIndex, rkInfo ) )
	{
		GangsiRecord *pRecord = (GangsiRecord*)FindModeRecord( iIndex );
		if( !pRecord )	return false;
		if( !pRecord->pChar ) return false;

		rkInfo.m_iContribute = pRecord->Contribute.m_iContribute;
		rkInfo.m_iContributePer = pRecord->Contribute.m_iPrevContributePer;
		return true;
	}
	return false;
}

bool ioGangsiMode::GetResultInfo( int iIndex, ResultInfo &rkInfo )
{
	if( ioPlayMode::GetResultInfo( iIndex, rkInfo ) )
	{
		GangsiRecord *pRecord = (GangsiRecord*)FindModeRecord( iIndex );
		if( !pRecord )	return false;
		if( !pRecord->pChar ) return false;

		rkInfo.m_iContribute = pRecord->Contribute.m_iContribute;
		rkInfo.m_iContributePer = pRecord->Contribute.m_iPrevContributePer;
		return true;
	}
	return false;
}

bool ioGangsiMode::GetFinalInfo( int iIndex, FinalInfo &rkInfo )
{
	if( ioPlayMode::GetFinalInfo( iIndex, rkInfo ) )
	{
		GangsiRecord *pRecord = (GangsiRecord*)FindModeRecord( iIndex );
		if( !pRecord )	return false;
		if( !pRecord->pChar ) return false;
		if( pRecord->pChar->GetState() == CS_VIEW )
		{
			// 예외 처리  - 관전자는 블루팀
			rkInfo.m_Team = TEAM_BLUE;			 
		}
		rkInfo.m_iContribute = pRecord->Contribute.m_iContribute;
		rkInfo.m_iContributePer = pRecord->Contribute.m_iPrevContributePer;
		return true;
	}
	return false;
}

void ioGangsiMode::SetRoundEndInfo( WinTeamType eTeam, bool bRoundSetEnd )
{
	if( bRoundSetEnd )
	{
		// 죽은 유저 강시로 부활
		int iCharCnt = GetRecordCharCnt();	
		for( int i=0; i<iCharCnt; i++ )
		{
			ioBaseChar *pChar = GetRecordChar( i );
			if( !pChar ) continue;
			if( pChar->GetState() != CS_DIE ) continue;

			if( !pChar->IsGangsi() )          // 강시가 아닌 유저는 강시로 부활
			{
				// 아이템 해제
				pChar->DestroyAllItem();

				// 강시 아이템 착용
				for(int i = 0;i < (int)m_InfectionGangsiItemCode.size();i++)
				{
					DWORD dwItemCode = m_InfectionGangsiItemCode[i];				
					if( dwItemCode > 0 )
					{
						ioItem *pItem = m_pCreator->CreateItem( dwItemCode );
						if( pItem )
						{
							pItem->ReLoadProperty();
							pItem->SetOwnerName( pChar->GetCharName() );
							pItem->SetItemCreateIndex( pChar->GetIndex() + i );
							pItem->SetItemReinforce( 0 );
							pItem->SetItemCustom( 0, 0 );

							ioItem *pPreItem = pChar->EquipItem( pItem, true );
							SAFEDELETE( pPreItem );

							// VoiceType Set
							int iVoiceType = 0;
							if( i == 0 )
							{
								const ioSetItemInfo *pSetInfo = g_SetItemInfoMgr.GetSetInfoByIdx( dwItemCode - 1 );
								if( pSetInfo )
									iVoiceType = pSetInfo->GetVoiceType();

								pChar->SetVoiceType( iVoiceType );
							}
						}
					}
				}

				pChar->SetGangsi( true );
				if( pChar->IsMale() && !m_szInfectionGangsiSkeletonMan.IsEmpty() )
				{
					pChar->ChangeSkeleton( m_szInfectionGangsiSkeletonMan );
				}
				else if( !m_szInfectionGangsiSkeletonWoman.IsEmpty() )
				{
					pChar->ChangeSkeleton( m_szInfectionGangsiSkeletonWoman );
				}

				// 강시 피부 변경
				if( pChar->IsMale() )
					pChar->ChangeSkinNumber( m_iChangeGangsiSkinMan );
				else
					pChar->ChangeSkinNumber( m_iChangeGangsiSkinWoman );
			}

			// 부활 세팅
			D3DXVECTOR3 vNewPos = GetRevivalPos( pChar );
			pChar->SetWorldPosition( vNewPos );
			pChar->Revival();
		}	


		// 인간 승리 퀘스트
		if( IsWinTeam( eTeam, TEAM_RED ) )
		{
			ioBaseChar *pOwnerChar = GetOwnerChar();
			if( pOwnerChar )
			{
				if( pOwnerChar->GetTeam() == TEAM_RED )
				{
					CharState eOwnerState = pOwnerChar->GetState();
					if( eOwnerState != CS_VIEW && eOwnerState != CS_DIE && eOwnerState != CS_OBSERVER &&
						eOwnerState != CS_READY && eOwnerState != CS_LOADING )
					{
						g_QuestMgr.QuestCompleteTerm( QC_GANGSI_HUMAN_WIN );
					}
				}
			}
		}
	}

	ioPlayMode::SetRoundEndInfo( eTeam, bRoundSetEnd );
}

void ioGangsiMode::SetRoundEndMotion()
{
	ContributeRoundAllPercent( 0, 0 );	

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

	g_ModeUIController.SetGangsiAlarm( (GetOwnerTeam() == TEAM_BLUE), 0, m_dwAlarmWarningTime );
}

void ioGangsiMode::SetRoundEndVictories()
{
	// 연승 적용 안함
}

void ioGangsiMode::SetResultScoreWnd()
{	
	// 중간 결과가 존재하지 않는다.
}

void ioGangsiMode::SetTotalScoreWnd( IntVec &rvClassTypeList,
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
	pTotalResult->SetGangsiScore();
	m_dwCurResultShowDuration = m_dwFinalRoundResultDuration;

	SetTotalResultBriefingWnd();
}

void ioGangsiMode::SetResultBriefingWnd()
{
	// 없음
}

void ioGangsiMode::SetTotalResultBriefingWnd()
{
	// 팀 모드 최종 결과 브리핑
	ioBaseChar *pOwner = GetOwnerChar();
	if( pOwner )
	{
		BriefingWnd *pBriefingWnd = dynamic_cast<BriefingWnd*>(g_GUIMgr.FindWnd( BRIEFING_WND ));
		if( pBriefingWnd && !pBriefingWnd->IsShow() )
		{
			TeamType eWinTeam = TEAM_NONE;
			if( IsWinTeam( m_WinTeamType, TEAM_BLUE ) )
				eWinTeam = TEAM_BLUE;
			else if( IsWinTeam( m_WinTeamType, TEAM_RED ) )
				eWinTeam = TEAM_RED;

			D3DXVECTOR3 vNewPos = ioMath::VEC3_ZERO;
			int iResultType = BriefingWnd::RESULT_DRAW;
			TeamType eOwenerTeam = pOwner->GetTeam();

			if( g_BattleRoomMgr.IsObserverUser( g_MyInfo.GetPublicID() ) || m_pCreator->IsStealthMode() )
			{
				ioBaseChar *pTargetChar = m_pCreator->GetBaseChar(m_szEndLookCharName);
				if( pTargetChar && 
					( GetModeType() == MT_SURVIVAL || 
					GetModeType() == MT_TEAM_SURVIVAL || 
					GetModeType() == MT_DOUBLE_CROWN || 
					GetModeType() == MT_FIGHT_CLUB || 
					GetModeType() == MT_TEAM_SURVIVAL_AI || 
					GetModeType() == MT_FLAG_CAPTURE ) )
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

void ioGangsiMode::RenderPreGUI()
{
	if( m_ModeState == MS_PLAY )
	{
		RenderPushEnableCursor();
	}
}

void ioGangsiMode::NotifyEntityDie( ioPlayEntity *pEntity, ioPlayEntity *pKiller, ioBaseChar *pBestAttacker,
									float fLastRate, float fBestRate )
{
	ioBaseChar *pDieChar = ToBaseChar( pEntity );
	ioBaseChar *pKillChar = ToBaseChar( pKiller );

	OnNotifyCharDie( pDieChar, pKillChar, pBestAttacker, fLastRate, fBestRate );
	UpdateKillDeathRecord( pEntity, pKiller, pBestAttacker );
}

void ioGangsiMode::NotifyCharDropDie( ioBaseChar *pDieChar, ioBaseChar *pKillChar, ioBaseChar *pBestAttacker,
									  float fLastRate, float fBestRate )
{
	// 드랍 KO는 미리 처리되었다
	if( pDieChar )  
	{
		if( pDieChar->GetTeam() == TEAM_RED )
		{
			SetInfectionProcess( pDieChar->GetCharName(), pDieChar->GetLastAttackerName(), pDieChar->GetLastAttackerSkillName(), pDieChar->GetLastAttackerItemCode() );
		}
	}

	OnNotifyCharDie( pDieChar, pKillChar, pBestAttacker, fLastRate, fBestRate );
	UpdateDropDieRecord( pDieChar, pKillChar, pBestAttacker );
}

void ioGangsiMode::UpdateDropDieRecord( ioBaseChar *pDieChar, ioBaseChar *pKillChar, ioBaseChar *pBestAttacker )
{
	if( pKillChar )
	{
		ModeRecord *pAttRecord = FindModeRecordCharName( pKillChar->GetCharName() );
		ModeRecord *pDieRecord = FindModeRecordCharName( pDieChar->GetCharName() );

		if( pAttRecord && pDieRecord )
		{
			TeamType eAttackerTeam = pDieChar->GetLastAttackerTeam();
			if( eAttackerTeam != pDieRecord->pChar->GetTeam() )
			{
				if( eAttackerTeam != pAttRecord->pChar->GetTeam() )
				{
					if( pDieRecord->pChar->GetTeam() != TEAM_RED )
					{
						g_ModeUIController.SetKOMsg( TEAM_NONE, "",
													 "", 0, pDieRecord->pChar->GetTeam(), pDieRecord->pChar->GetCharName() );
					}
				}
				else
				{
					if( pDieRecord->pChar->GetTeam() != TEAM_RED )
					{
						g_ModeUIController.SetKOMsg( eAttackerTeam, pAttRecord->pChar->GetCharName(), 
													 pDieRecord->pChar->GetLastAttackerSkillName(), pDieRecord->pChar->GetLastAttackerItemCode(),
													 pDieRecord->pChar->GetTeam(), pDieRecord->pChar->GetCharName() );
					}
				}
			}
			else
			{
				if( pAttRecord->pChar == pDieRecord->pChar )	// 자기자신의 공격
				{
					if( pDieRecord->pChar->GetTeam() != TEAM_RED )
					{	
						g_ModeUIController.SetKOMsg( TEAM_NONE, "", 
													 "", 0, pDieRecord->pChar->GetTeam(), pDieRecord->pChar->GetCharName() );
					}
				}
				else
				{
					if( pDieRecord->pChar->GetTeam() != TEAM_RED )
					{
						g_ModeUIController.SetKOMsg( TEAM_NONE, "", 
													 "", 0, pDieRecord->pChar->GetTeam(), pDieRecord->pChar->GetCharName() );
					}
				}
			}
		}
	}
	else
	{
		ModeRecord *pDieRecord = FindModeRecordCharName( pDieChar->GetCharName() );
		if( pDieRecord )
		{
			if( pDieRecord->pChar->GetTeam() != TEAM_RED )
			{
				g_ModeUIController.SetKOMsg( TEAM_NONE, "", 
					                         "", 0, pDieRecord->pChar->GetTeam(), pDieRecord->pChar->GetCharName() );
			}
		}
	}
}

void ioGangsiMode::UpdateKillDeathRecord( ioPlayEntity *pDieEntity, ioPlayEntity *pKiller, ioPlayEntity *pBestAttacker )
{
	ioBaseChar *pKillerChar = ToBaseChar( pKiller );
	ioBaseChar *pDieChar = ToBaseChar( pDieEntity );

	if( pKillerChar && pDieChar )
	{
		ModeRecord *pAttRecord = FindModeRecordCharName( pKillerChar->GetCharName() );
		ModeRecord *pDieRecord = FindModeRecordCharName( pDieChar->GetCharName() );

		if( pAttRecord && pDieRecord )
		{
			if( pAttRecord->pChar->GetTeam() != pDieRecord->pChar->GetTeam() )
			{
				g_ModeUIController.SetKOMsg( pAttRecord->pChar->GetTeam(), pAttRecord->pChar->GetCharName(),
									 		 pDieRecord->pChar->GetLastAttackerSkillName(), pDieRecord->pChar->GetLastAttackerItemCode(),
											 pDieRecord->pChar->GetTeam(), pDieRecord->pChar->GetCharName() );
			}
			else
			{
				if( pAttRecord->pChar == pDieRecord->pChar )	// 자기자신의 공격
				{
					g_ModeUIController.SetKOMsg( TEAM_NONE, "",
												 "", 0, pDieRecord->pChar->GetTeam(), pDieRecord->pChar->GetCharName() );
				} 
				else
				{
					g_ModeUIController.SetKOMsg( TEAM_NONE, "",
												 "", 0, pDieRecord->pChar->GetTeam(), pDieRecord->pChar->GetCharName() );
				}
			}
		}
	}
	else
	{
		ModeRecord *pDieRecord = FindModeRecordCharName( pDieChar->GetCharName() );
		if( pDieRecord )
		{
			g_ModeUIController.SetKOMsg( TEAM_NONE, "",
										 "", 0, pDieRecord->pChar->GetTeam(), pDieRecord->pChar->GetCharName() );
		}
	}
}

void ioGangsiMode::OnNotifyCharDie( ioBaseChar *pDieChar, ioBaseChar *pKillChar, ioBaseChar *pBestAttacker,
								    float fLastRate, float fBestRate )
{
	if( !pDieChar )
	{
		LOG.PrintTimeAndLog( 0, "ioGangsiMode::OnNotifyCharDie() - DieChar is NULL" );
		return;
	}

	GangsiRecord *pDieRecord = FindRecordCharName( pDieChar->GetCharName() );
	if( !pDieRecord )
	{
		LOG.PrintTimeAndLog( 0, "ioGangsiMode::OnNotifyCharDie - %s Not Exit Char",
								pDieChar->GetCharName().c_str() );
		return;
	}

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
	}

	//For EndScene
	m_LastDieChar = pDieChar->GetCharName();

	if( pKillChar )
	{
		GangsiRecord *pKillRecord = FindRecordCharName( pKillChar->GetCharName() );
		if( !pKillRecord )
		{
			LOG.PrintTimeAndLog( 0, "ioGangsiMode::OnNotifyCharDie - %s Not Exist Kill Char",
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

	if( pKillChar && pKillChar->IsOwnerChar() )
	{
		if( pDieChar && pDieChar->IsGangsi() )
		{
			// 강시 잡았삼
			g_QuestMgr.QuestCompleteTerm( QC_GANGSI_KILL );
		}
	}
}

void ioGangsiMode::AddNewRecord( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	GangsiRecord *pGangsiRecord = FindRecordCharName( pChar->GetCharName() );
	if( !pGangsiRecord )
	{
		GangsiRecord kRecord;
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
		pGangsiRecord->pChar = pChar;
	}
}

void ioGangsiMode::RemoveRecord( const ioHashString &rkName )
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


void ioGangsiMode::RemoveRecord( int nIndex )
{
	int iCharCnt = m_vCharRecordList.size();
	if( iCharCnt > nIndex )
	{
		m_vCharRecordList.erase( m_vCharRecordList.begin() + nIndex );
		UpdateUserRank();
	}
}


ioBaseChar* ioGangsiMode::GetRecordChar( int i )
{
	if( COMPARE( i, 0, GetRecordCharCnt() ) )
		return m_vCharRecordList[i].pChar;

	return NULL;
}

ioBaseChar* ioGangsiMode::GetRecordChar( const ioHashString &rkName )
{
	GangsiRecord *pRecord = FindRecordCharName( rkName );
	if( pRecord )
		return pRecord->pChar;

	return NULL;
}

int ioGangsiMode::GetRecordCharCnt() const
{
	return m_vCharRecordList.size();
}

ioBaseChar* ioGangsiMode::GetRecordCharTeamArray( TeamType eTeam, int iArray )
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
		
		if( iCurCnt == iArray )
			return pRecord->pChar;
		iCurCnt++;
	}

	return NULL;
}

ModeRecord* ioGangsiMode::FindModeRecord( int iIdx )
{
	if( COMPARE( iIdx, 0, GetRecordCharCnt() ) )
		return &m_vCharRecordList[iIdx];

	return NULL;
}

ModeRecord* ioGangsiMode::FindModeRecordCharName( const ioHashString &rkName )
{
	return (ModeRecord*)FindRecordCharName( rkName );
}

GangsiRecord* ioGangsiMode::FindRecordCharName( const ioHashString &rkName )
{
	int iCharCnt = m_vCharRecordList.size();
	for( int i=0 ; i<iCharCnt ; i++ )
	{
		if( m_vCharRecordList[i].szName == rkName )
			return &m_vCharRecordList[i];
	}

	return NULL;
}

bool ioGangsiMode::ProcessUDPPacket( SP2Packet &rkPacket )
{
	if( ioPlayMode::ProcessUDPPacket( rkPacket ) )
		return true;

	return false;
}

void ioGangsiMode::ApplyModeInfo( SP2Packet &rkPacket )
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

void ioGangsiMode::ApplyCharModeInfo( const ioHashString &rkName, SP2Packet &rkPacket, bool bDieCheck )
{
	bool bRecord;
	rkPacket >> bRecord;

	if( !bRecord ) return;

	GangsiRecord *pRecord = FindRecordCharName( rkName );
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

		if( bDieCheck )       // 죽음 체크하는 구문이 룸 입장시 룸에 있는 유저들 정보를 가져올 때.
		{
			bool bDie;
			rkPacket >> bDie;

			if( bDie )
			{
				pRecord->pChar->SetDieState();
			}

			bool bHostGangsi, bInfectionGangsi;
			rkPacket >> bHostGangsi >> bInfectionGangsi;
			if( pRecord->pChar )
			{
				if( bHostGangsi )
				{
					pRecord->pChar->SetGangsi( true );
					if( pRecord->pChar->IsMale() && !m_szHostGangsiSkeletonMan.IsEmpty() )
					{
						pRecord->pChar->ChangeSkeleton( m_szHostGangsiSkeletonMan );
					}
					else if( !m_szHostGangsiSkeletonWoman.IsEmpty() )
					{
						pRecord->pChar->ChangeSkeleton( m_szHostGangsiSkeletonWoman );
					}

					// 강시 피부 변경
					if( pRecord->pChar->IsMale() )
						pRecord->pChar->ChangeSkinNumber( m_iChangeGangsiSkinMan );
					else
						pRecord->pChar->ChangeSkinNumber( m_iChangeGangsiSkinWoman );

					// 강시 버프
					SetGangsiBuff( pRecord->pChar );
				}
				else if( bInfectionGangsi )
				{
					pRecord->pChar->SetGangsi( true );
					if( pRecord->pChar->IsMale() && !m_szInfectionGangsiSkeletonMan.IsEmpty() )
					{
						pRecord->pChar->ChangeSkeleton( m_szInfectionGangsiSkeletonMan );
					}
					else if( !m_szInfectionGangsiSkeletonWoman.IsEmpty() )
					{
						pRecord->pChar->ChangeSkeleton( m_szInfectionGangsiSkeletonWoman );
					}

					// 강시 피부 변경
					if( pRecord->pChar->IsMale() )
						pRecord->pChar->ChangeSkinNumber( m_iChangeGangsiSkinMan );
					else
						pRecord->pChar->ChangeSkinNumber( m_iChangeGangsiSkinWoman );

					// 강시 버프
					SetGangsiBuff( pRecord->pChar );
				}
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

		if( bDieCheck )				  // 죽음 체크하는 구문이 룸 입장시 룸에 있는 유저들 정보를 가져올 때.
		{
			bool bDie;
			rkPacket >> bDie;

			bool bHostGangsi, bInfectionGangsi;
			rkPacket >> bHostGangsi >> bInfectionGangsi;
		}
	}
}

void ioGangsiMode::OnFinalRoundResult( SP2Packet &rkPacket )
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
			LOG.PrintTimeAndLog( 0, "ioGangsiMode::OnFinalRoundResult - Not Exist Record: %s", szName.c_str() );
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

int ioGangsiMode::GetRoundWinCnt( TeamType eTeamType )
{
	return 0;
}

void ioGangsiMode::CheckRoundEnd()
{
}

void ioGangsiMode::ContributeConvertTeam( TeamType eTeam )
{
	if( m_vContributeTable.size() < MAX_PLAYER )
		return;

	int i = 0;
	int iCharCnt = m_vCharRecordList.size();
	int iInfoCnt = 0;
	ContributeInfoList vInfo;

	//강시 감염 횟수
	for(i = 0;i < iCharCnt;i++)
	{
		GangsiRecord *pRecord = &m_vCharRecordList[i];
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
		Contribute.m_iContributePoint = pRecord->Contribute.m_iGangsiInfection;
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
			GangsiRecord *pRecord = &m_vCharRecordList[iArray];
			if( !pRecord->Contribute.m_bLastResultSync )
				pRecord->Contribute.m_iContribute += m_vContributeTable[vInfo[i].m_iRank] * 3;
			pRecord->Contribute.m_iGangsiInfectionRank = vInfo[i].m_iRank;
		}
	}
	vInfo.clear();

	//생존 시간
	for(i = 0;i < iCharCnt;i++)
	{
		GangsiRecord *pRecord = &m_vCharRecordList[i];
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
		Contribute.m_iContributePoint = pRecord->Contribute.m_iGangsiSurvivor;
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
			GangsiRecord *pRecord = &m_vCharRecordList[iArray];
			if( !pRecord->Contribute.m_bLastResultSync )
				pRecord->Contribute.m_iContribute += m_vContributeTable[vInfo[i].m_iRank] * 5;
			pRecord->Contribute.m_iGangsiSurvivorRank = vInfo[i].m_iRank;
		}
	}
	vInfo.clear();

	//데미지
	for(i = 0;i < iCharCnt;i++)
	{
		GangsiRecord *pRecord = &m_vCharRecordList[i];
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
			GangsiRecord *pRecord = &m_vCharRecordList[iArray];
			if( !pRecord->Contribute.m_bLastResultSync )
				pRecord->Contribute.m_iContribute += m_vContributeTable[vInfo[i].m_iRank] * 2;
			pRecord->Contribute.m_iDamageRank = vInfo[i].m_iRank;
		}
	}
	vInfo.clear();
}

void ioGangsiMode::ContributeRoundAllPercent( DWORD dwCurTime, DWORD dwTotalTime )
{
	//기여도의 100분율
	int iMaxContribute	= 0;
	int iCharCnt		= m_vCharRecordList.size();
	int iPlayCharCnt	= 0;

	int i = 0;
	for(i = 0;i < iCharCnt;i++)
	{
		GangsiRecord *pRecord = &m_vCharRecordList[i];
		if( !pRecord ) continue;
		if( !pRecord->pChar ) continue;
		if( pRecord->pChar->GetState() == CS_OBSERVER ) continue;
		if( g_BattleRoomMgr.IsObserverUser(pRecord->pChar->GetCharName()) ) continue;
		
		iMaxContribute += pRecord->Contribute.m_iContribute;
		iPlayCharCnt++;
	}

	CharState eOwnerState = CS_LOADING;
	int iOwnerContribute = -1;
	if( iMaxContribute > 0 )
	{
		for(i = 0;i < iCharCnt;i++)
		{
			GangsiRecord *pRecord = &m_vCharRecordList[i];
			if( !pRecord ) continue;
			if( !pRecord->pChar ) continue;
			if( pRecord->pChar->GetState() == CS_OBSERVER ) continue;
			if( g_BattleRoomMgr.IsObserverUser(pRecord->pChar->GetCharName()) ) continue;
			
			float fContributePer = (float)iPlayCharCnt * ((float)pRecord->Contribute.m_iContribute / iMaxContribute );
			pRecord->Contribute.m_iPrevContributePer = ( fContributePer + 0.001f ) * FLOAT100;
			if( pRecord->pChar->IsOwnerChar() )
			{
				iOwnerContribute = pRecord->Contribute.m_iPrevContributePer;
				eOwnerState = pRecord->pChar->GetState();
			}

			g_AwardMgr.AddContribute( pRecord->szName, pRecord->Contribute.m_iPrevContributePer );
		}
	}

	if( m_ModeState != MS_PLAY ) return;
	if( iOwnerContribute <= 0 ) return;
	if( eOwnerState == CS_DIE || eOwnerState == CS_VIEW ||
		eOwnerState == CS_OBSERVER || eOwnerState == CS_READY || eOwnerState == CS_LOADING ) return;

	int iPraise = g_MyInfo.CheckContributePraise( iOwnerContribute );
	if( iPraise > 0 )
	{
		g_ChatMgr.SetSystemMsg( STR(1), iPraise );
		g_ModeHelpMgr.ShowContributePraise( iPraise );
	}
}

void ioGangsiMode::ContributeConvert()
{
	ContributeConvertTeam( TEAM_NONE );
}

int ioGangsiMode::ContributeMyRank( int &rRankValue1, int &rRankValue2, int &rRankValue3, int &rRankValue4 )
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

	//강시 감염
	for(i = 0;i < iCharCnt;i++)
	{
		GangsiRecord *pRecord = &m_vCharRecordList[i];
		if( !pRecord ) continue;
		if( !pRecord->pChar ) continue;
		if( pRecord->pChar->GetState() == CS_OBSERVER ) continue;
		if( g_BattleRoomMgr.IsObserverUser(pRecord->pChar->GetCharName()) ) continue;

		ContributeInfo Contribute;
		Contribute.m_iRecordArray = i;
		Contribute.m_iContributePoint = pRecord->Contribute.m_iGangsiInfection;
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

	//생존 시간
	for(i = 0;i < iCharCnt;i++)
	{
		GangsiRecord *pRecord = &m_vCharRecordList[i];
		if( !pRecord ) continue;
		if( !pRecord->pChar ) continue;
		if( pRecord->pChar->GetState() == CS_OBSERVER ) continue;
		if( g_BattleRoomMgr.IsObserverUser(pRecord->pChar->GetCharName()) ) continue;

		ContributeInfo Contribute;
		Contribute.m_iRecordArray = i;
		Contribute.m_iContributePoint = pRecord->Contribute.m_iGangsiSurvivor;
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

	//데미지
	for(i = 0;i < iCharCnt;i++)
	{
		GangsiRecord *pRecord = &m_vCharRecordList[i];		
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
	return iInfoCnt;
}

void ioGangsiMode::DamageContribute( const ioHashString szAttacker, const ioHashString szTarget, float fDamage, DWORD dwItemCode, int iWeaponType )
{
	if( szAttacker.IsEmpty() ) return;

	GangsiRecord *pRecord = FindRecordCharName( szAttacker );
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

	// 보정만큼 증가해준다.	
	if( !pAttacker->IsGangsi() )  
	{
		float fBalance = pAttacker->GetMemberBalanceRateForDamage( TEAM_RED );
		if( fBalance > 0.000000f )
			fDamage /= fBalance;
	}
	float fDamagePoint = (fDamage + fBonusDamage) * 100;
	pRecord->Contribute.m_iDamage += (float)fDamagePoint;

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

void ioGangsiMode::KillContribute( const ioHashString szKiller, const ioHashString szKilled, DWORD dwItemCode )
{
}

void ioGangsiMode::AttackCntContribute( const ioHashString szAttacker, const ioHashString szTarget, DWORD dwItemCode, int iAttackValue/* = 100 */ )
{
/* if( szAttacker.IsEmpty() || szTarget.IsEmpty() ) return;

	GangsiRecord *pAttackerRecord = FindRecordCharName( szAttacker );
	GangsiRecord *pTargetRecord = FindRecordCharName( szTarget );

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

	// 강시 페널티
	float fGangsiPenalty = FLOAT1;
	if( pAttack->IsGangsi() )
		fGangsiPenalty = ContributeTeamPerA( TEAM_RED, 15.0f, FLOAT1 );

	if( pAttackerRecord && pTargetRecord )
	{
		if( pAttackerRecord->pChar && pTargetRecord->pChar )
		{
			pAttackerRecord->Contribute.m_iAttackCnt += (float)iAttackValue * fAddAttackPoint * fGangsiPenalty;
		}
	}

	ContributeConvert();
	ContributeRoundAllPercent();
	UpdateRoundInfoList();
	CheckTotalChamp();
*/
}

void ioGangsiMode::InfectionCntContribute( const ioHashString szAttacker, const ioHashString szTarget )
{
	if( szAttacker.IsEmpty() || szTarget.IsEmpty() ) return;

	GangsiRecord *pAttackerRecord = FindRecordCharName( szAttacker );
	GangsiRecord *pTargetRecord = FindRecordCharName( szTarget );
	if( pAttackerRecord && pTargetRecord )
	{
		if( pAttackerRecord->pChar && pTargetRecord->pChar )
		{
			if( !pAttackerRecord->pChar->IsGangsi() ) return;			

			int iInfectionValue = 100;
			pAttackerRecord->Contribute.m_iGangsiInfection += iInfectionValue;
			g_AwardMgr.AddGangsiKing( pAttackerRecord->pChar->GetCharName() );
		}
	}

	ContributeConvert();
	ContributeRoundAllPercent();
	UpdateRoundInfoList();
	CheckTotalChamp();

	if( pAttackerRecord && pAttackerRecord->pChar )
	{
		if( pAttackerRecord->pChar->IsOwnerChar() )
		{
			// 강시 감염
			g_QuestMgr.QuestCompleteTerm( QC_GANGSI_HUMAN_KILL );
		}
	}
}

void ioGangsiMode::SurvivorCntContribute( const ioHashString szTarget, DWORD dwTime )
{
	if( szTarget.IsEmpty() ) return;

	GangsiRecord *pTargetRecord = FindRecordCharName( szTarget );
	if( pTargetRecord && pTargetRecord->pChar )
	{
		pTargetRecord->Contribute.m_iGangsiSurvivor += (int)dwTime;
		g_AwardMgr.AddGangsiSurvivor( pTargetRecord->pChar->GetCharName(), dwTime );
	}

	ContributeConvert();
	ContributeRoundAllPercent();
	UpdateRoundInfoList();
	CheckTotalChamp();
}

void ioGangsiMode::ContributeLOG()
{
	char szLog[MAX_PATH];

	int iCharCnt = m_vCharRecordList.size();
	for( int i=0 ;i<iCharCnt; i++ )
	{
		GangsiRecord *pRecord = &m_vCharRecordList[i];
		if( pRecord->pChar )
		{
			sprintf( szLog, "%s[%d] : I%d(%d) S%d(%d) D%d(%d)", pRecord->szName.c_str(), (int)pRecord->pChar->GetTeam(),
																pRecord->Contribute.m_iGangsiInfectionRank + 1, pRecord->Contribute.m_iGangsiInfection,
																pRecord->Contribute.m_iGangsiSurvivorRank + 1, pRecord->Contribute.m_iGangsiSurvivor,
																pRecord->Contribute.m_iDamageRank + 1, pRecord->Contribute.m_iDamage );
			g_ChatMgr.SetSystemMsg( szLog );
		}
	}
}


void ioGangsiMode::FillLastPlayRecordInfo( SP2Packet &rkPacket )
{
	int iCharCnt = m_vCharRecordList.size();
	rkPacket << iCharCnt;
	for(int i = 0;i < iCharCnt;i++)
	{
		GangsiRecord *pRecord = &m_vCharRecordList[i];
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

void ioGangsiMode::FillPlayRecordInfo( SP2Packet &rkPacket )
{
	int i = 0;
	int iCharCnt = m_vCharRecordList.size();
	rkPacket << iCharCnt;
	for(i = 0;i < iCharCnt;i++)
	{
		GangsiRecord *pRecord = &m_vCharRecordList[i];
		rkPacket << pRecord->pChar->GetCharName();
		rkPacket << pRecord->Contribute.m_iGangsiInfection;
		rkPacket << pRecord->Contribute.m_iGangsiSurvivor;
		rkPacket << pRecord->Contribute.m_iDamage;
		rkPacket << pRecord->Contribute.m_iPrevContributePer;
		rkPacket << pRecord->GetUniqueTotalKill();
		rkPacket << pRecord->GetUniqueTotalDeath();
		rkPacket << g_VictoriesMgr.GetVictories( pRecord->pChar->GetCharName() );
		rkPacket << pRecord->GetNoDeathKillCount();
	}
}

void ioGangsiMode::ApplyPlayRecordInfo( SP2Packet &rkPacket )
{
	int iCharCnt;
	rkPacket >> iCharCnt;
	for(int i = 0;i < iCharCnt;i++)
	{
		ioHashString szName;
		int iDamage, iGangsiInfection, iGangsiSurvivor, iPrevContributePer, iUniqueTotalKill, iUniqueTotalDeath, iVictories, iNoDeathKillCount;
		rkPacket >> szName >> iGangsiInfection >> iGangsiSurvivor >> iDamage >> iPrevContributePer >> iUniqueTotalKill >> iUniqueTotalDeath >> iVictories >> iNoDeathKillCount;

		GangsiRecord *pRecord = FindRecordCharName( szName );
		if( pRecord )
		{
			pRecord->Contribute.m_iGangsiInfection  = iGangsiInfection;
			pRecord->Contribute.m_iGangsiSurvivor   = iGangsiSurvivor;
			pRecord->Contribute.m_iDamage           = iDamage;
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

void ioGangsiMode::ContributeResultSync( const ioHashString &szName, int iContribute )
{
	GangsiRecord *pRecord = FindRecordCharName( szName );
	if( !pRecord ) return;

	pRecord->Contribute.m_bLastResultSync = true;
	pRecord->Contribute.m_iContribute     = iContribute;
}

void ioGangsiMode::ShowEndScoreWnd( bool bShow )
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

D3DXVECTOR3 ioGangsiMode::GetStartPos( TeamType eTeamType, bool bRandom )
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


D3DXVECTOR3 ioGangsiMode::GetRevivalPos( ioBaseChar *pChar )
{
	if(!pChar)
	{
		LOG.PrintTimeAndLog( 0, "ioGangsiMode::GetRevivalPos() - Char is NULL" );
		return m_vMapCenter;
	}
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

void ioGangsiMode::GetMostNearPosList( PositionDistanceSqList &rkResultList, const D3DXVECTOR3 &vTargPos )
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

void ioGangsiMode::UpdateRoundInfoList()
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

void ioGangsiMode::UpdateUIRank()
{
	TimeScoreWnd *pTimeScoreWnd = dynamic_cast<TimeScoreWnd *>( g_GUIMgr.FindWnd( TIME_SCORE_WND ) );
	if( !pTimeScoreWnd ) return;

	GangsiRecord *pRecord = FindRecordCharName( g_MyInfo.GetPublicID() );
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
		int iLiveUser = 0;
		RecordList::iterator iter;
		for( iter=m_vCharRecordList.begin() ; iter!=m_vCharRecordList.end() ; ++iter )
		{
			ioBaseChar *pChar = iter->pChar;			
			if( pChar->GetTeam() != TEAM_RED ) continue;
			if( pChar->GetState() == CS_READY || pChar->GetState() == CS_VIEW || pChar->GetState() == CS_OBSERVER ) continue;

			iLiveUser++;
		}
		pTimeScoreWnd->UpdateScore( 0, iLiveUser );
	}
}

int ioGangsiMode::GetCurRank( const ioHashString& szCharName )
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

int ioGangsiMode::GetContributePerChar( const ioHashString& szName )
{
	int iCharCnt = m_vCharRecordList.size();

	for(int i = 0;i < iCharCnt;i++)
	{
		GangsiRecord *pRecord = &m_vCharRecordList[i];
		if( !pRecord )	continue;
		if( !pRecord->pChar ) continue;

		if( pRecord->pChar->GetCharName() == szName )
		{
			return pRecord->Contribute.m_iPrevContributePer;
		}
	}

	return 0;
}

bool ioGangsiMode::IsNeedRevival( ioBaseChar *pChar )
{
	return false;
}

void ioGangsiMode::CheckEndSceneLookAt()
{
	ioBaseChar *pOwner = m_pCreator->GetOwnerChar();
	if( pOwner && pOwner->GetState() != CS_VIEW && pOwner->GetState() != CS_OBSERVER )
	{
		m_vEndLookAt = pOwner->GetWorldPosition();				
		m_szEndLookCharName = pOwner->GetCharName();
		pOwner->SetEndFocusEffect( GetRoundEndEffect() );
	}
	else if( g_BattleRoomMgr.IsObserverUser( g_MyInfo.GetPublicID() ) || m_pCreator->IsStealthMode() )
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
		m_vEndLookAt = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );				
		m_szEndLookCharName.Clear();
	}
}


ioBaseChar* ioGangsiMode::CheckMostNearChar( ioBaseChar *pChar )
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

void ioGangsiMode::ProcessAlarmTickTime()
{
	if( m_dwAlarmTickTime == 0 ) return;

	DWORD dwGapTime = FRAMEGETTIME() - m_dwPlayStartTime;
	if( m_dwNextAlarmTick == 0 )
	{
		m_dwNextAlarmTick = max( 0, (int)m_dwCurRoundDuration - ( m_dwAlarmTickTime * ( (float)( dwGapTime / m_dwAlarmTickTime ) + 1 ) ) );
	}
    
	if( m_dwCurRoundDuration - dwGapTime <= m_dwAlarmWarningTime ) // 상시 표시
	{
		m_dwNextAlarmTick = max( 0, m_dwCurRoundDuration - dwGapTime );
		g_ModeUIController.SetGangsiAlarm( (GetOwnerTeam() == TEAM_BLUE), m_dwNextAlarmTick, m_dwAlarmWarningTime );

	}
	else if( m_dwCurRoundDuration - dwGapTime < m_dwNextAlarmTick )  // 반복 표시
	{
		g_ModeUIController.SetGangsiAlarm( (GetOwnerTeam() == TEAM_BLUE), m_dwNextAlarmTick, m_dwAlarmWarningTime );
		m_dwNextAlarmTick = max( 0, (int)m_dwNextAlarmTick - m_dwAlarmTickTime );
	}
}

void ioGangsiMode::ProcessChangeGangsiState()
{
	ioBaseChar *pOwnerChar = GetOwnerChar();
	if( !pOwnerChar ) return;
	if( pOwnerChar->GetState() == CS_VIEW ||
		pOwnerChar->GetState() == CS_OBSERVER ||
		pOwnerChar->GetState() == CS_READY ||
		pOwnerChar->GetState() == CS_LOADING ||
		pOwnerChar->GetState() == CS_DIE ) return;
	if( pOwnerChar->GetTeam() != TEAM_RED ) return;
	if( pOwnerChar->GetHP().m_fCurValue > 0.0f ) return;

	pOwnerChar->SetTeam( TEAM_BLUE );
	SendChangeGangsi( pOwnerChar );
}

void ioGangsiMode::ProcessGangsiSurvivorTime()
{
	if( m_dwGangsiSurvivorTime == 0 )
		m_dwGangsiSurvivorTime = FRAMEGETTIME();
	
	DWORD dwCheckTime = 500; 
	if( FRAMEGETTIME() - m_dwGangsiSurvivorTime < dwCheckTime ) return;
    
	m_dwGangsiSurvivorTime = FRAMEGETTIME();
	int iCharCnt = m_vCharRecordList.size();
	for(int i = 0;i < iCharCnt;i++)
	{
		GangsiRecord *pUserRecord = &m_vCharRecordList[i];
		if( !pUserRecord ) continue;
		if( !pUserRecord->pChar ) continue;
		if( pUserRecord->pChar->GetState() == CS_DIE ||
			pUserRecord->pChar->GetState() == CS_VIEW ||
			pUserRecord->pChar->GetState() == CS_OBSERVER ||
			pUserRecord->pChar->GetState() == CS_READY ||
			pUserRecord->pChar->GetState() == CS_LOADING ) continue;

		if( pUserRecord->pChar->GetTeam() != TEAM_RED )
			continue;

		SurvivorCntContribute( pUserRecord->pChar->GetCharName(), dwCheckTime );
		
		if( pUserRecord->pChar->IsOwnerChar() )
		{
			g_QuestMgr.QuestCompleteTerm( QC_GANGSI_ALIVE_TIME, dwCheckTime );
		}
	}		
}

void ioGangsiMode::SetInfectionProcess( ioHashString szDier, ioHashString szKiller, ioHashString szKillerSkillName, DWORD dwKillerItemCode )
{
	// 마지막 타격자를 강시왕으로..
	InfectionCntContribute( szKiller, szDier );

	// 강시 전환 Msg
	g_ModeUIController.SetInfectionMsg( TEAM_BLUE, szKiller, szKillerSkillName,	dwKillerItemCode, TEAM_RED, szDier );
}

void ioGangsiMode::UserBuffProcess()
{
	DWORD dwTimeSec = (float)( FRAMEGETTIME() - m_dwPlayStartTime ) / 1000;
	if( dwTimeSec < m_dwUserBuffTime ) return;

	int iCharCnt = m_vCharRecordList.size();
	for( int i=0 ; i<iCharCnt ; i++ )
	{
		ioBaseChar *pTarget = m_vCharRecordList[i].pChar;
		if( !pTarget ) continue;
		if( pTarget->GetTeam() != TEAM_RED ) continue;
		if( pTarget->GetState() == CS_DIE ||
			pTarget->GetState() == CS_VIEW ||
			pTarget->GetState() == CS_OBSERVER ||
			pTarget->GetState() == CS_READY ||
			pTarget->GetState() == CS_LOADING ) continue;

		SetUserBuff( pTarget );
	}

	DWORD dwPrevNextTime = m_dwUserBuffTime;
	// 다음 버프 시간
	vBuffList::iterator iter;
	for(iter = m_UserBuff.begin();iter != m_UserBuff.end();iter++)
	{
		BuffList &kBuffData = *iter;
		if( dwTimeSec < kBuffData.m_dwCheckTime )
		{
			m_dwUserBuffTime = kBuffData.m_dwCheckTime;
			break;
		}
	}
	if( dwPrevNextTime == m_dwUserBuffTime )  // 다음 시간이 없으면
		m_dwUserBuffTime = 0xffffffff;
}

void ioGangsiMode::SetUserBuff( ioBaseChar *pChar )
{
	if( pChar == NULL ) return;
	
	DWORD dwTimeSec = (float)( FRAMEGETTIME() - m_dwPlayStartTime ) / 1000;
	
	vBuffList::reverse_iterator riter;
	for(riter = m_UserBuff.rbegin();riter != m_UserBuff.rend();riter++)
	{
		BuffList &kBuffData = *riter;
		if( dwTimeSec >= kBuffData.m_dwCheckTime )
		{
			for(int i = 0;i < (int)kBuffData.m_BuffList.size();i++)
			{
				ioHashString &rkBuff = kBuffData.m_BuffList[i];
				if( rkBuff.IsEmpty() ) continue;
				if( pChar->HasBuff( rkBuff ) ) continue;

				pChar->AddNewBuff( rkBuff.c_str(), "", "", NULL, false );
			}
			break;
		}
	}
}

void ioGangsiMode::GangsiBuffProcess()
{
	DWORD dwTimeSec = (float)( FRAMEGETTIME() - m_dwPlayStartTime ) / 1000;
	if( dwTimeSec < m_dwGangsiBuffTime ) return;

	int iCharCnt = m_vCharRecordList.size();
	for( int i=0 ; i<iCharCnt ; i++ )
	{
		ioBaseChar *pTarget = m_vCharRecordList[i].pChar;
		if( !pTarget ) continue;
		if( pTarget->GetTeam() != TEAM_BLUE ) continue;
		if( pTarget->GetState() == CS_DIE ||
			pTarget->GetState() == CS_VIEW ||
			pTarget->GetState() == CS_OBSERVER ||
			pTarget->GetState() == CS_READY ||
			pTarget->GetState() == CS_LOADING ) continue;

		SetGangsiBuff( pTarget );
	}

	DWORD dwPrevNextTime = m_dwGangsiBuffTime;
	// 다음 버프 시간
	vBuffList::iterator iter;
	for(iter = m_GangsiBuff.begin();iter != m_GangsiBuff.end();iter++)
	{
		BuffList &kBuffData = *iter;
		if( dwTimeSec < kBuffData.m_dwCheckTime )
		{
			m_dwGangsiBuffTime = kBuffData.m_dwCheckTime;
			break;
		}
	}
	
	if( dwPrevNextTime == m_dwGangsiBuffTime )  // 다음 시간이 없으면
		m_dwGangsiBuffTime = 0xffffffff;
}

void ioGangsiMode::SetGangsiBuff( ioBaseChar *pChar )
{
	if( pChar == NULL ) return;

	DWORD dwTimeSec = (float)( FRAMEGETTIME() - m_dwPlayStartTime ) / 1000;

	vBuffList::reverse_iterator riter;
	for(riter = m_GangsiBuff.rbegin();riter != m_GangsiBuff.rend();riter++)
	{
		BuffList &kBuffData = *riter;
		if( dwTimeSec >= kBuffData.m_dwCheckTime )
		{
			for(int i = 0;i < (int)kBuffData.m_BuffList.size();i++)
			{
				ioHashString &rkBuff = kBuffData.m_BuffList[i];
				if( rkBuff.IsEmpty() ) continue;
				if( pChar->HasBuff( rkBuff ) ) continue;

				pChar->AddNewBuff( rkBuff.c_str(), "", "", NULL, false );
			}
			break;
		}
	}
}

void ioGangsiMode::OnChangeCharNextProcess( ioBaseChar *pChar, bool bNewChar )
{
	SendChangeGangsi( pChar );

	// 일반 유저 버프 처리
	if( bNewChar && pChar && pChar->GetTeam() == TEAM_RED )
	{
		SetUserBuff( pChar );
	}
}

void ioGangsiMode::SendChangeGangsi( ioBaseChar *pChar )
{
	if( !pChar ) return;
	if( !pChar->IsOwnerChar() ) return;
	if( pChar->GetTeam() != TEAM_BLUE ) return; 

	if( !pChar->IsGangsi() )
	{
		pChar->ReleaseObejctEquipState();

		// 강시로 변경 요청
		SP2Packet kPacket( CTPK_CHANGE_GANGSI );

		// 드랍 아이템 전송
		pChar->FillAllItemDrop( kPacket );

		TCPNetwork::SendToServer( kPacket );

		// 강시로 변신 시작?
		pChar->SetGangsi( true );

		pChar->SetProtectionMode( PM_REVIVAL, 2000 );
	}
}

bool ioGangsiMode::ProcessTCPPacket( SP2Packet &rkPacket )
{
	ErrorReport::SetPosition( 1090, 1 );
	if( ioPlayMode::ProcessTCPPacket( rkPacket ) )
		return true;

	switch( rkPacket.GetPacketID() )
	{
	case STPK_SELECT_GANGSI:
		ErrorReport::SetPosition( 1090, 2 );
		OnSelectGangsi( rkPacket );
		ErrorReport::SetPosition( 1090, 3 );
		return true;
	case STPK_CHANGE_GANGSI:
		ErrorReport::SetPosition( 1090, 4 );
		OnChangeGangsi( rkPacket );
		ErrorReport::SetPosition( 1090, 5 );
		return true;
	}

	ErrorReport::SetPosition( 1090, 100 );
	return false;
}

void ioGangsiMode::OnSelectGangsi( SP2Packet &rkPacket )
{
	ioBaseChar *pChar = GetRecordChar( g_MyInfo.GetPublicID() );
	if( !pChar )
	{
		LOG.PrintTimeAndLog( 0, "ioGangsiMode::OnSelectGangsi - %s Not Exist User", g_MyInfo.GetPublicID().c_str() );
		return;
	}

	if( pChar->GetState() == CS_READY || pChar->GetState() == CS_LOADING ||
		pChar->GetState() == CS_OBSERVER || pChar->GetState() == CS_VIEW )
	{
		LOG.PrintTimeAndLog( 0, "ioGangsiMode::OnSelectGangsi - %s Not Exist State %d", g_MyInfo.GetPublicID().c_str(), (int)pChar->GetState() );
		return;
	}

	// 필드에있는 캐릭터면 강시로 전환
	pChar->SetTeam( TEAM_BLUE );
	SendChangeGangsi( pChar );
}

void ioGangsiMode::OnChangeGangsi( SP2Packet &rkPacket )
{
	ioHashString szName;
	rkPacket >> szName;

	ioBaseChar *pChar = GetRecordChar( szName );
	if( !pChar )
	{
		LOG.PrintTimeAndLog( 0, "ioGangsiMode::OnChangeGangsi - %s Not Exist User", szName.c_str() );
		return;
	}

	pChar->ReleaseObejctEquipState();
	
	int iItemCnt;
	rkPacket >> iItemCnt;
	for( int i=0; i < iItemCnt; ++i )
	{
		int iSlot;
		rkPacket >> iSlot;

		int iItemCreateIndex;
		DWORD dwItemCode;
		int iItemReinforce;
		DWORD dwItemMaleCustom, dwItemFemaleCustom;

		ioHashString szOwnerName;
		D3DXVECTOR3 vDropStartPos;
		float fCurItemSkillGauge;
		int iCurBullet;

		rkPacket >> dwItemCode;
		if( dwItemCode == 0 )
		{
			rkPacket >> iItemReinforce >> dwItemMaleCustom >> dwItemFemaleCustom;
			rkPacket >> szName >> vDropStartPos >>  fCurItemSkillGauge >> iCurBullet;
			LOG.PrintTimeAndLog( 0, "ioGangsiMode::OnChangeGangsi - %s Not Has Item(%d)", szName.c_str(), dwItemCode );
			continue;
		}

		rkPacket >> iItemReinforce >> dwItemMaleCustom >> dwItemFemaleCustom >> iItemCreateIndex;		
		rkPacket >> szOwnerName >> vDropStartPos >> fCurItemSkillGauge;
		rkPacket >> iCurBullet;

		ioItem *pPreItem = pChar->ReleaseItemExplicit( dwItemCode, iItemCreateIndex, iSlot );
		if( !pPreItem )
		{
			LOG.PrintTimeAndLog( 0, "ioGangsiMode::OnChangeGangsi - %s Not Has Item(%d)", szName.c_str(), dwItemCode );
			continue;
		}

		pPreItem->SetItemReinforce( iItemReinforce );
		pPreItem->SetItemCustom( dwItemMaleCustom, dwItemFemaleCustom );
		pPreItem->SetCurSkillGauge( fCurItemSkillGauge );
		pPreItem->SetCurBullet( iCurBullet );

		ioFieldItem *pDropItem = m_pCreator->DropFieldItem( vDropStartPos, pPreItem, pChar );
		if( !pDropItem )
		{
			LOG.PrintTimeAndLog( 0, "ioGangsiMode::OnChangeGangsi - Create FieldItem Error" );
			SAFEDELETE( pPreItem );
			continue;
		}
	}

	pChar->SetTeam( TEAM_BLUE );
	pChar->SetGangsi( true );
	// 강시 될 때 자신이 만들어놓은 아이템 파괴
	m_pCreator->ClearUserCreateEntities( pChar->GetCharName() );
	// 강시 피부 변경
	if( pChar->IsMale() )
		pChar->ChangeSkinNumber( m_iChangeGangsiSkinMan );
	else
		pChar->ChangeSkinNumber( m_iChangeGangsiSkinWoman );

	// 강시 아이템 착용
	// 스킬 게이지 초기화	
	float fPrevSkillGauge[MAX_INVENTORY] = { 0.0f, 0.0f, 0.0f, 0.0f, };
	m_pCreator->ChangeCharEquipItem( pChar, fPrevSkillGauge, rkPacket );
	pChar->FillMaxHP();

	// 강시
	pChar->SetChangeGangsiState();

	// 강시 버프
	SetGangsiBuff( pChar );

	int iGangsiUser = 0;
	int iLiveUser = 0;
	RecordList::iterator iter;
	for( iter=m_vCharRecordList.begin() ; iter!=m_vCharRecordList.end() ; ++iter )
	{
		ioBaseChar *pTempChar = iter->pChar;
		
		if( pTempChar->GetTeam() == TEAM_RED )
		{
			if( pTempChar->GetState() == CS_VIEW ||
				pTempChar->GetState() == CS_OBSERVER ||
				pTempChar->GetState() == CS_READY ||
				pTempChar->GetState() == CS_LOADING ) continue;

			iLiveUser++;
		}
		else if( pTempChar->GetTeam() == TEAM_BLUE )
			iGangsiUser++;
	}

	// 화면 블라인드
	if( pChar->IsOwnerChar() )
	{
		m_pCreator->SetScreenOveray();

		// 비강시 유저들 중 - 숨은 유저들 보이지 않도록 처리
		for( iter=m_vCharRecordList.begin() ; iter!=m_vCharRecordList.end() ; ++iter )
		{
			ioBaseChar *pTempChar = iter->pChar;
			if( pTempChar->GetTeam() != TEAM_RED ) continue;
			if( pTempChar->HasHideBuff() )
			{
				pTempChar->SetHideChar( true );
			}
		}
		//g_GUIMgr.HideWnd( SKILL_LIST_WND );
		g_GUIMgr.HideWnd( MINI_SOLDIER_SELECT_WND );
	}

	// 첫 강시만 HP Full이고 다음 강시부터는 부활 HP 적용
	if( iGangsiUser > 1 )
	{
		pChar->GetHP().m_fCurValue = pChar->GetHP().m_fMaxValue * m_fChangeGangsiHPRate;

		g_ModeUIController.SetGangsiLiveUser( iLiveUser, pChar->GetLastAttackerName(), pChar->GetCharName() );
		if( !pChar->GetLastAttackerName().IsEmpty() )
		{
			SetInfectionProcess( pChar->GetCharName(), pChar->GetLastAttackerName(), pChar->GetLastAttackerSkillName(), pChar->GetLastAttackerItemCode() );
		}

        if( pChar->IsMale() && !m_szInfectionGangsiSkeletonMan.IsEmpty() )
		{
			pChar->ChangeSkeleton( m_szInfectionGangsiSkeletonMan );
		}
		else if( !m_szInfectionGangsiSkeletonWoman.IsEmpty() )
		{
			pChar->ChangeSkeleton( m_szInfectionGangsiSkeletonWoman );
		}
	}
	else
	{
		if( pChar->IsMale() && !m_szHostGangsiSkeletonMan.IsEmpty() )
		{
			pChar->ChangeSkeleton( m_szHostGangsiSkeletonMan );
		}
		else if( !m_szHostGangsiSkeletonWoman.IsEmpty() )
		{
			pChar->ChangeSkeleton( m_szHostGangsiSkeletonWoman );
		}	
	}
	m_pCreator->ReCalculateDamageRate( false );
}

void ioGangsiMode::OnZeroHP( SP2Packet &rkPacket )
{
	rkPacket >> m_dwSuddenDeathTime;

	m_bZeroHP = true;
	g_SoundMgr.PlaySound( m_TimeOutSound, DSBVOLUME_MAX, PSM_LOOP | PSM_THREAD );
	g_ModeUIController.SetDeathTime();
}

void ioGangsiMode::UpdateRevivalTime( const ioHashString &szName, DWORD dwCurRevivalTime )
{
	ModeRecord *pDieRecord = FindModeRecordCharName( szName );
	if( !pDieRecord )
	{
		LOG.PrintTimeAndLog( 0, "ioGangsiMode::UpdateRevivalTime() - %s is not exist to the record", szName.c_str() );
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


void ioGangsiMode::OnRoundJoinView( SP2Packet &rkPacket )
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