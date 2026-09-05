

#include "stdafx.h"

#include "ioHeadquartersMode.h"
#include "ioNpcChar.h"
#include "ioReservedTask.h"
#include "ioExtendSoundManager.h"

#include "GUI/MiniSoldierSelectWnd.h"
#include "GUI/MiniMap.h"
#include "GUI/RoundStateMainWnd.h"
#include "GUI/SoldierPracticeWnd.h"
#include "GUI/SoldierDisplayWnd.h"
#include "GUI/EventWnd.h"
#include "GUI/HeadquartersMainWnd.h"
#include "GUI/CenterKillInfoWnd.h"
#include "GUI/TrainingFuncMenuWnd.h"

ioHeadquartersMode::ioHeadquartersMode( ioPlayStage *pCreator ) : ioPlayMode( pCreator )
{
	m_bStopTime = true;
	m_ModeType = MT_HEADQUARTERS;
	m_dwCharState = STATE_OPEN_CHAR;
	m_SingleTeamPosArray.clear();
	m_bJoinLock = false;
	m_iMaxPlayer = MAX_PLAYER / 2;

	m_dwPrePeriodTime = 0;

	m_vCharRecordList.reserve( 100 );
}

ioHeadquartersMode::~ioHeadquartersMode()
{
	ClearMode();
	SaveCharacterPos();
	m_vDefStartPos.clear();
	m_vBackupDefStarPos.clear();
}

void ioHeadquartersMode::LoadCharacterPos()
{
	char fName[MAX_PATH];
	wsprintf_e( fName,"Save\\%s\\window.ini",g_MyInfo.GetPublicID().c_str() );

	m_vDefStartPos.assign( m_vStartPos.begin(), m_vStartPos.end() );
	ioINILoader kLoader( fName, false );
	kLoader.SetTitle_e( "character" );

	int iVersion = kLoader.LoadInt_e( "version", 0 );
	if( iVersion != CHARACTER_POS_VER ) 
		return;

	char szStartX[MAX_PATH], szStartZ[MAX_PATH];

	int iMaxStartPos = m_vStartPos.size();
	for( int i=0 ; i<iMaxStartPos; i++ )
	{
		wsprintf_e( szStartX, "start%d_x", i+1 );
		wsprintf_e( szStartZ, "start%d_z", i+1 );

		float fXPos = kLoader.LoadFloat( szStartX, -9999.0f );
		float fZPos = kLoader.LoadFloat( szStartZ, -9999.0f );

		if( fXPos != -9999.0f || fZPos != -9999.0f )
		{
			m_vStartPos[i].x = fXPos;
			m_vStartPos[i].z = fZPos;
		}
	}
}

void ioHeadquartersMode::SaveCharacterPos()
{
	char fName[MAX_PATH];
	wsprintf_e( fName,"Save\\%s\\window.ini",g_MyInfo.GetPublicID().c_str() );

	ioINILoader kLoader( fName , false );
	kLoader.SetTitle_e( "character" );

	kLoader.SaveInt( "version", CHARACTER_POS_VER );

	char szStartX[MAX_PATH], szStartZ[MAX_PATH];

	int iMaxStartPos = m_vStartPos.size();
	for( int i=0 ; i<iMaxStartPos; i++ )
	{
		wsprintf_e(szStartX, "start%d_x", i+1);
		wsprintf_e(szStartZ, "start%d_z", i+1);

		kLoader.SaveFloat( szStartX, m_vStartPos[i].x );
		kLoader.SaveFloat( szStartZ, m_vStartPos[i].z );
	}
}

void ioHeadquartersMode::LoadSound()
{
	// 로비 사운드 플레이
	ioINILoader_e kSndLoader( "config/sp2_sound.ini" );
	kSndLoader.SetTitle_e( "sound_file_name" );

	char szBuf[MAX_PATH];
	kSndLoader.LoadString_e( "myroom_sound", "", szBuf, MAX_PATH );

	if( strcmp( szBuf, "" ) != 0 )
	{
		char szName[MAX_PATH];
		wsprintf( szName, "Resource/wave/%s", szBuf );

		g_SoundMgr.OpenOggStream( szName );
		g_SoundMgr.PlayStream( true );
	}
}

void ioHeadquartersMode::PlayExtendSound()
{
	g_ExSoundMgr.PlaySound( ExSound::EST_HQ_ENTER );
}

void ioHeadquartersMode::ClearMode()
{
	g_GUIMgr.HideWnd( TIMEGATE_SEARCH );
	ioPlayMode::ClearMode();
}

void ioHeadquartersMode::LoadModeResource( ioINILoader &rkLoader )
{
	g_GUIMgr.AddWnd( "XML/SkillListWnd.xml" );
	g_GUIMgr.AddWnd( "XML/AccessorySkillListWnd.xml" );

	MiniSoldierSelectWnd *pMiniSoldierWnd = dynamic_cast<MiniSoldierSelectWnd*>(g_GUIMgr.AddWnd( "XML/MiniSoldierSelectWnd.xml" ));
	if(pMiniSoldierWnd)
		pMiniSoldierWnd->SetPlayStage( m_pCreator );

	CenterKillInfoWnd *pCenterKillInfoWnd = dynamic_cast<CenterKillInfoWnd*>( g_GUIMgr.AddWnd( "XML/centerkillinfownd.xml" ) );
	if( pCenterKillInfoWnd )
	{
		g_ModeUIController.SetCenterKillInfoWnd( pCenterKillInfoWnd );
	}

	ioPlayMode::LoadModeResource( rkLoader );
}

void ioHeadquartersMode::LoadMap()
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

void ioHeadquartersMode::InitMode( int iMapIndex )
{
	m_iModeMapIndex = iMapIndex;

	g_ModeUIController.SetKOMsgWnd( NULL );
	g_ModeUIController.SetCenterInfoWnd( NULL );
	
	m_pCreator->SetCameraModeFixedBlock( false );

	PracticeHelpWnd *pPracticeHelp = dynamic_cast<PracticeHelpWnd *>( g_GUIMgr.AddWnd( "XML/practicehelpwnd.xml" ) );
	if( pPracticeHelp )
		pPracticeHelp->SetPlayStage( m_pCreator );

	SoldierPracticeSelectWnd *pPracticeSelect = dynamic_cast<SoldierPracticeSelectWnd *>(g_GUIMgr.AddWnd( "XML/soldierpracticeselectwnd.xml" ));
	if( pPracticeSelect )
		pPracticeSelect->SetPlayStage( m_pCreator );

	SoldierDisplaySelectWnd *pDisplaySelect = dynamic_cast<SoldierDisplaySelectWnd *>(g_GUIMgr.AddWnd( "XML/soldierdisplayselectwnd.xml" ));
	if( pDisplaySelect )
		pDisplaySelect->SetPlayStage( m_pCreator );

	SoldierDisplayChangeWnd *pDisplayChange = dynamic_cast<SoldierDisplayChangeWnd*>(g_GUIMgr.AddWnd( "XML/soldierdisplaychangewnd.xml" ));
	if( pDisplayChange )
		pDisplayChange->SetPlayStage( m_pCreator );

	g_GUIMgr.AddWnd( "XML/SoldierDisplayAlarmWnd.xml" );	

	ExerciseSoldierEventWnd *pExerciseSoldierEventWnd = dynamic_cast<ExerciseSoldierEventWnd*> ( g_GUIMgr.AddWnd( "XML/exercisesoldiereventwnd.xml" ) ); 
	if( pExerciseSoldierEventWnd )
		pExerciseSoldierEventWnd->SetPlayStage( m_pCreator );

	HeadquartersMainWnd *pHeadquartersMain = dynamic_cast< HeadquartersMainWnd * >( g_GUIMgr.AddWnd( "XML/headquartersmainwnd.xml" ) );
	if( pHeadquartersMain )
		pHeadquartersMain->SetPlayStage( m_pCreator );

	g_GUIMgr.AddWnd( "XML/HeadquartersOwnerWnd.xml" );

	HeadquartersInviteListWnd *pHeadquartersInvite = dynamic_cast< HeadquartersInviteListWnd * >( g_GUIMgr.AddWnd( "XML/headquartersinvitelistwnd.xml" ) );
	if( pHeadquartersInvite )
		pHeadquartersInvite->SetPlayStage( m_pCreator );

	g_GUIMgr.AddWnd( "XML/TutorialWnd.xml" );
	g_GUIMgr.AddWnd( "XML/tutorialkeyboardwnd.xml" );

	//Party
	g_GUIMgr.AddWnd( "XML/partycreatewnd.xml" );

	g_GUIMgr.AddWnd( "XML/plazaroominfownd.xml" );
	g_GUIMgr.AddWnd( "XML/plazapasswordwnd.xml" );
	g_GUIMgr.AddWnd( "XML/plazainvitedwnd.xml" );
	g_GUIMgr.AddWnd( "XML/plazacreatewnd.xml" );
	g_GUIMgr.AddWnd( "XML/plazaownerwnd.xml" );

	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	if( pLocal && pLocal->IsShowFirstIDWnd() )
		g_GUIMgr.AddWnd( "XML/firstidchangewnd.xml" );
	if( pLocal && pLocal->IsShowFirstSoldierSelectWnd() )
		g_GUIMgr.AddWnd( "XML/firstsoldierselectwnd.xml" );

	if( g_App.GetConnectedMoveMode() != ioApplication::CMM_LOBBY )
	{	
		g_GUIMgr.AddWnd( "XML/QuickLobbyMenu.xml" );
		g_GUIMgr.AddWnd( "XML/QuickLobbyWnd.xml" );		
	}
	else
	{
		TrainingFuncMenuWnd *pTrainingFuncMenuWnd = dynamic_cast<TrainingFuncMenuWnd *>( g_GUIMgr.AddWnd( "XML/trainingfuncmenu.xml" ) );
		if( pTrainingFuncMenuWnd )
		{
			pTrainingFuncMenuWnd->SetPlayMode( this );
		}
		g_GUIMgr.AddWnd( "XML/trainingfuncwnd.xml" );
	}

	LoadMap();
	LoadPushStructs();
	LoadMachineStruct();
	LoadFuntionalObjects();
	LoadCharacters();
	LoadCharacterPos();
	LoadDisableSkillList();
	LoadStartBuffList();

	ioINILoader kLoader( GetModeINIFileName( m_ModeType ) );

	kLoader.SetTitle_e( "common_info" );
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

	LoadGUIValues();
	LoadPushCursor();
	LoadContribute();

	m_ModeState = MS_PLAY;
	m_bFirstStart = true;

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

	if( g_MyInfo.GetUserState() == 0 )
	{
		ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
		if( pLocal && pLocal->IsShowFirstIDWnd() )
		{
			char szTempPublicID[ID_NUM_PLUS_ONE]="";
			StringCbPrintf( szTempPublicID, sizeof( szTempPublicID ), "%s#", g_MyInfo.GetPrivateID() );

			if( g_MyInfo.GetPublicID() == szTempPublicID  )
				g_GUIMgr.ShowWnd( FIRST_IDCHANGE_WND );
			else
				g_MyInfo.SetFirstSolderBonus();
		}
		else if( pLocal && pLocal->IsShowFirstSoldierSelectWnd() )
		{
			g_GUIMgr.ShowWnd( FIRST_SOLDIER_SELECT_WND );
		}
		else
		{
			// 최초 용병 구입.
			g_MyInfo.SetFirstSolderBonus();
		}
	}
	else
	{		
		SP2Packet kPacket( CTPK_CHAR_LIMIT_CHECK );
		TCPNetwork::SendToServer( kPacket );
		g_App.SetMouseBusy( true );
	}

	g_GUIMgr.ShowWnd( SKILL_LIST_WND );
	g_GUIMgr.ShowWnd( ACCESSORY_SKILL_LIST_WND );
	g_QuestMgr.QuestCompleteTerm( QC_ENTER_HEADQUARTER );

	// 훈련소 들어갈때 이벤트 구조물 체크
	if( m_pCreator )
	{
		m_pCreator->CheckEventStruct( EVT_CHILDRENDAY );
		m_pCreator->CheckEventStruct( EVT_CHRISTMAS );
	}
	LoadSound();
}

void ioHeadquartersMode::ReStartMode()
{
	LOG.PrintTimeAndLog( 0, "ioHeadquartersMode::ReStartMode Is Call???" );
}

void ioHeadquartersMode::ProcessMode()
{
	ErrorReport::SetPosition( 1011, 1 );
	ProcessSelectCharacter();
	ErrorReport::SetPosition( 1011, 2 );
	ProcessGUI();
	ErrorReport::SetPosition( 1011, 3 );

	switch( m_ModeState )
	{
	case MS_READY:
		ErrorReport::SetPosition( 1011, 4 );
		ProcessReadyState();
		ErrorReport::SetPosition( 1011, 5 );
		CheckModeMapSound();
		break;
	case MS_PLAY:
		ProcessMapAreaWeapon();
		ErrorReport::SetPosition( 1011, 6 );
		ProcessPlayState();
		ErrorReport::SetPosition( 1011, 7 );
		CheckModeMapSound();
		ErrorReport::SetPosition( 1011, 8 );
		g_EventMgr.Process();
		ErrorReport::SetPosition( 1011, 9 );
		ProcessNpcErase();
		ErrorReport::SetPosition( 1011, 10 );
		
		break;
	}
}

void ioHeadquartersMode::ProcessReadyState()
{
	if( P2PNetwork::IsNetworkPlaying() )	return;
	if( m_dwReadyStartTime + 4000 > FRAMEGETTIME() )
		return;

	SetModeState( MS_PLAY );
}

void ioHeadquartersMode::ProcessPlayState()
{
	UpdateSoldierRevivalPenalty();
	UpdateTimeGrowthTime();

	ProcessOwnerDelayPosSave();
	m_pCreator->NotifySpringDampUseSet( true );

	ProcessRevival();

	if(g_KeyManager.IsCoolTimeResetPress())
	{
		if(!g_MyInfo.IsHost())
			return;
		ioHashString	szTemp = " -N1- ";
		ioBaseChar *pChar = m_pCreator->GetBaseChar( szTemp );
		if( !pChar )	return;		
		D3DXVECTOR3 vNewPos = GetRevivalPos( pChar );
		pChar->SetWorldPosition( vNewPos );
		pChar->Revival();
		pChar->SetState( CS_DELAY );
	}
}

void ioHeadquartersMode::ProcessOwnerPosSave( DWORD dwIndex )
{
	int iCharCnt = m_vCharRecordList.size();
	for( int i=0 ; i<iCharCnt ; i++ )
	{
		ioBaseChar *pChar = m_vCharRecordList[i].pChar;
		if( !pChar ) continue;

		if( pChar->GetIndex() == dwIndex )
		{			
			D3DXVECTOR3 vOwnerPos = pChar->GetWorldPosition();
			int iPosArray = pChar->GetTeam() - TEAM_PRIVATE_1;
			if( COMPARE( iPosArray, 0, (int)m_vStartPos.size() ) )
			{
				m_vStartPos[iPosArray].x = vOwnerPos.x;
				m_vStartPos[iPosArray].z = vOwnerPos.z;
			}
		}
	}
}

void ioHeadquartersMode::ProcessOwnerDelayPosSave()
{
	int iCharCnt = m_vCharRecordList.size();
	for( int i=0 ; i<iCharCnt ; i++ )
	{
		ioBaseChar *pChar = m_vCharRecordList[i].pChar;
		if( !pChar ) continue;
		if( !pChar->IsOwnerChar() ) continue;
		if( pChar->GetState() != CS_DELAY )
		{
			m_dwOwnerPosSaveTimer = FRAMEGETTIME();
			continue;
		}

		if( FRAMEGETTIME() - m_dwOwnerPosSaveTimer < 2000 ) continue;

		D3DXVECTOR3 vOwnerPos = pChar->GetWorldPosition();
		int iPosArray = pChar->GetTeam() - TEAM_PRIVATE_1;
		if( COMPARE( iPosArray, 0, (int)m_vStartPos.size() ) )
		{
			m_vStartPos[iPosArray].x = vOwnerPos.x;
			m_vStartPos[iPosArray].z = vOwnerPos.z;
		}
	}
}

void ioHeadquartersMode::ProcessGUI()
{
	UpdateMiniMapSpotPoint();
	UpdateRoundStateWnd();
	UpdateTimeScoreWnd();
	UpdateEndScoreWndTime();
	UpdateSoldierWndTime();

	ProcessPushEnableCursor();
	ProcessBlindMode();
	
	if( g_App.GetConnectedMoveMode() != ioApplication::CMM_LOBBY )
	{
		g_RserveTask.ExecuteReservedTask( RESERVEDCMD_WHEN_RECV_ATTENDANCE );
		g_RserveTask.ExecuteReservedTask( RESERVEDCMD_WHEN_RECV_ATTENDANCE_REWARD );
		g_RserveTask.ExecuteReservedTask( RESERVEDCMD_WHEN_RECV_PCROOOM );
		g_RserveTask.ExecuteReservedTask( RESERVEDCMD_WHEN_RECV_TOURNAEMNT_REWARD );
		g_RserveTask.ExecuteReservedTask( RESERVEDCMD_WHEN_RECV_GUILD_RANK_REWARD );
		g_RserveTask.ExecuteReservedTask( RESERVEDCMD_WHEN_ROLLBOOK );
	}
}

void ioHeadquartersMode::ProcessRevival()
{
	DWORD dwCurTime = FRAMEGETTIME();
	
	int iCharCnt = m_vCharRecordList.size();
	for( int i=0 ; i<iCharCnt ; i++ )
	{
		HeadquartersRecord &rkRecord = m_vCharRecordList[i];
		if( rkRecord.dwCurDieTime == 0 ) continue;
		
		if( rkRecord.pChar == m_pCreator->GetOwnerChar() )
		{
			int iRevivalTime = (int)rkRecord.dwRevivalGap - ( dwCurTime - rkRecord.dwCurDieTime );
			if( iRevivalTime < 0 ) iRevivalTime = 0;
			if( P2PNetwork::IsNetworkPlaying() )
			{
				if( rkRecord.dwCurDieTime + rkRecord.dwRevivalGap < dwCurTime )
					rkRecord.dwCurDieTime = 0;
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

void ioHeadquartersMode::UpdateMiniMapSpotPoint()
{
	MiniMap *pMiniMap = dynamic_cast<MiniMap*>(g_GUIMgr.FindWnd( MINIMAP_WND ));
	if( !pMiniMap )	return;

	ioPlayMode::UpdateMiniMapSpotPoint();

	pMiniMap->ClearPointList();

	D3DXVECTOR3 vPos;
	DWORD dwGapTime = 0;
	ioBaseChar *pOwner = GetOwnerChar();

	if( !pOwner ) return;

	// Other Char
	RecordList::iterator iter;
	for( iter=m_vCharRecordList.begin() ; iter!=m_vCharRecordList.end() ; ++iter )
	{
		ioBaseChar *pChar = iter->pChar;
		if( !pChar ) continue;
		if( pChar->IsOwnerChar() || !pChar->IsCanMiniMapVisible() )
			continue;

		dwGapTime = pChar->GetEventGapTime();
		vPos = pChar->GetWorldPosition();

		TeamType eTeam = TEAM_RED;
		if( pOwner->GetTeam() == pChar->GetTeam() )
			eTeam = TEAM_BLUE;
		
		if( pChar->IsHasCrown() )
			pMiniMap->AddPoint( vPos.x, vPos.z, MiniMap::SPOT_KING_USER, eTeam, dwGapTime );
		else
		{
			if( pChar->IsDropZoneDownState() )
				pMiniMap->AddPoint( vPos.x, vPos.z, MiniMap::SPOT_DROP_USER, eTeam, 0 );
			else
				pMiniMap->AddPoint( vPos.x, vPos.z, MiniMap::SPOT_USER, eTeam, dwGapTime );
		}
	}

	// Owner
	if( pOwner && pOwner->IsCanMiniMapVisible() )
	{
		dwGapTime = pOwner->GetEventGapTime();
		vPos = pOwner->GetWorldPosition();
		if( pOwner->IsHasCrown() )
		{
			pMiniMap->AddPoint( vPos.x, vPos.z, MiniMap::SPOT_KING_ME, TEAM_BLUE, dwGapTime );
		}
		else
		{
			if( pOwner->IsDropZoneDownState() )
				pMiniMap->AddPoint( vPos.x, vPos.z, MiniMap::SPOT_DROP_ME, TEAM_BLUE, 0 );
			else
				pMiniMap->AddPoint( vPos.x, vPos.z, MiniMap::SPOT_ME, TEAM_BLUE, dwGapTime );
		}
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

void ioHeadquartersMode::UpdateRoundStateWnd()
{
	if( m_ModeState != MS_PLAY )
	{
		if( g_GUIMgr.IsShow( ROUND_STATE_MAIN_WND ) )
		{
			g_GUIMgr.HideWnd( ROUND_STATE_MAIN_WND );
		}
		return;
	}

	if( g_Input.IsKeyDown(KEY_TAB) )
	{
		RoundStateMainWnd *pRoundStateWnd = dynamic_cast<RoundStateMainWnd*>(g_GUIMgr.FindWnd( ROUND_STATE_MAIN_WND ));
		if( pRoundStateWnd && !pRoundStateWnd->IsShow() && !pRoundStateWnd->IsSearchShow() )	// 2019-07-03
		{	
			pRoundStateWnd->ClearRoundInfo();
			for( int i = 0 ; i < GetRecordCharCnt(); i++ )
			{
				RoundInfo kInfo;
				if( GetRoundInfo( i, kInfo ) )
					pRoundStateWnd->AddRoundInfo( kInfo );
			}
			pRoundStateWnd->SetHeadequartersScore();
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

void ioHeadquartersMode::UpdateDropDieRecord( ioBaseChar *pDieChar, ioBaseChar *pKillChar, ioBaseChar *pBestAttacker )
{
	ioBaseChar *pOwner = GetOwnerChar();
	if( !pOwner ) return;

	ioHashString szDieName, szAttName;
	ioNpcChar *pKillNpc = ToNpcChar( pKillChar );
	if( pKillNpc )
	{
		szAttName = pKillNpc->GetViewName();
	}
	else if( pKillChar )
	{
		szAttName = pKillChar->GetCharName();
	}

	ioNpcChar *pDieNpc = ToNpcChar( pDieChar );
	if( pDieNpc )
	{
		szDieName = pDieNpc->GetViewName();
		if( pDieNpc->IsAI() )
		{
			AddEraseNpc( pDieNpc->GetCharName() );
		}
	}
	else if( pDieChar )
	{
		szDieName = pDieChar->GetCharName();
	}	

	if( pKillChar )
	{
		ModeRecord *pAttRecord = FindModeRecordCharName( pKillChar->GetCharName() );
		ModeRecord *pDieRecord = FindModeRecordCharName( pDieChar->GetCharName() );

		if( pAttRecord && pDieRecord )
		{
			TeamType eAttTeam, eDieTeam;

			if( pAttRecord->pChar->GetTeam() == pOwner->GetTeam() )
				eAttTeam = TEAM_BLUE;
			else 
				eAttTeam = TEAM_RED;

			if( pDieRecord->pChar->GetTeam() == pOwner->GetTeam() )
				eDieTeam = TEAM_BLUE;
			else
				eDieTeam = TEAM_RED;

			if( pAttRecord->pChar->GetTeam() != pDieRecord->pChar->GetTeam() )
			{
				pAttRecord->AddKill( GetModeType(), GetKillPoint( pAttRecord->pChar->GetTeam() ), true, false );
				pDieRecord->AddDeath( GetModeType(), GetDeathPoint( pDieRecord->pChar->GetTeam() ) );							
				
				
				g_ModeUIController.SetKOMsg( eAttTeam, szAttName,
					                         pDieRecord->pChar->GetLastAttackerSkillName(), pDieRecord->pChar->GetLastAttackerItemCode(),
											 eDieTeam, szDieName );
			}
			else
			{
				if( pAttRecord->pChar == pDieRecord->pChar )	// 자기자신의 공격
				{
					pDieRecord->AddDeath( GetModeType(), GetDeathPoint( pDieRecord->pChar->GetTeam() ) );
					g_ModeUIController.SetKOMsg( TEAM_NONE, "", "", 0, eDieTeam, szDieName );
				}
				else
				{
					pAttRecord->AddKill( GetModeType(), -GetKillPoint( pAttRecord->pChar->GetTeam() ), true, false );
					pDieRecord->AddDeath( GetModeType(), GetDeathPoint( pDieRecord->pChar->GetTeam() ) );

					g_ModeUIController.SetKOMsg( eAttTeam, szAttName,
												 pDieRecord->pChar->GetLastAttackerSkillName(), pDieRecord->pChar->GetLastAttackerItemCode(),
												 eDieTeam, szDieName );
				}
			}

			if ( g_ModeUIController.IsCenterKillInfoWnd() )
				pAttRecord->pChar->SetKillEmoticon( pAttRecord->GetNoDeathKillCount() );
			else
				pAttRecord->pChar->SetKillEmoticon( pAttRecord->GetUniqueTotalKill() );

			pDieRecord->pChar->SetDeathEmoticon();

			ioBaseChar *pOwner = GetOwnerChar();
			if( pOwner )
				g_ModeUIController.SetCenterKillUI( m_ModeType, pAttRecord->GetNoDeathKillCount(), pOwner, pAttRecord->pChar->GetTeam(), pAttRecord->pChar->GetCharName(),
													pDieRecord->pChar->GetLastAttackerItemCode(), pDieRecord->pChar->GetTeam(), pDieRecord->pChar->GetCharName(), pAttRecord->pChar->IsMale() );
		}
	}
	else
	{
		ModeRecord *pDieRecord = FindModeRecordCharName( pDieChar->GetCharName() );
		if( pDieRecord )
		{
			TeamType eDieTeam;

			if( pDieRecord->pChar->GetTeam() == pOwner->GetTeam() )
				eDieTeam = TEAM_BLUE;
			else
				eDieTeam = TEAM_RED;

			pDieRecord->AddDeath( GetModeType(), GetDeathPoint( pDieRecord->pChar->GetTeam() ) );

			g_ModeUIController.SetKOMsg( TEAM_NONE, "", "", 0, eDieTeam, szDieName );

			pDieRecord->pChar->SetDeathEmoticon();
		}
	}
}

void ioHeadquartersMode::UpdateKillDeathRecord( ioPlayEntity *pDieEntity, ioPlayEntity *pKiller, ioPlayEntity *pBestAttacker )
{
	ioBaseChar *pOwner = GetOwnerChar();
	if( !pOwner ) return;

	ioBaseChar *pKillerChar = ToBaseChar( pKiller );
	ioBaseChar *pDieChar = ToBaseChar( pDieEntity );

	ioHashString szDieName, szAttName;
	ioNpcChar *pKillNpc = ToNpcChar( pKillerChar );
	if( pKillNpc )
	{
		szAttName = pKillNpc->GetViewName();
	}
	else if( pKillerChar )
	{
		szAttName = pKillerChar->GetCharName();
	}

	ioNpcChar *pDieNpc = ToNpcChar( pDieChar );
	if( pDieNpc )
	{
		szDieName = pDieNpc->GetViewName();
		if( pDieNpc->IsAI() )
		{
			AddEraseNpc( pDieNpc->GetCharName() );
		}
	}
	else if( pDieChar )
	{
		szDieName = pDieChar->GetCharName();
	}	

	if( pKillerChar && pDieChar )
	{
		ModeRecord *pAttRecord = FindModeRecordCharName( pKillerChar->GetCharName() );
		ModeRecord *pDieRecord = FindModeRecordCharName( pDieChar->GetCharName() );

		if( pAttRecord && pDieRecord )
		{
			TeamType eAttTeam, eDieTeam;

			if( pAttRecord->pChar->GetTeam() == pOwner->GetTeam() )
				eAttTeam = TEAM_BLUE;
			else 
				eAttTeam = TEAM_RED;

			if( pDieRecord->pChar->GetTeam() == pOwner->GetTeam() )
				eDieTeam = TEAM_BLUE;
			else
				eDieTeam = TEAM_RED;

			if( pAttRecord->pChar->GetTeam() != pDieRecord->pChar->GetTeam() )
			{
				pAttRecord->AddKill( GetModeType(), GetKillPoint( pAttRecord->pChar->GetTeam() ), false, false );
				pDieRecord->AddDeath( GetModeType(), GetDeathPoint( pDieRecord->pChar->GetTeam() ) );							

				g_ModeUIController.SetKOMsg( eAttTeam, szAttName, 
					                         pDieRecord->pChar->GetLastAttackerSkillName(), pDieRecord->pChar->GetLastAttackerItemCode(),
											 eDieTeam, szDieName );
			}
			else
			{
				if( pAttRecord->pChar == pDieRecord->pChar )	// 자기자신의 공격
				{
					pDieRecord->AddDeath( GetModeType(), GetDeathPoint( pDieRecord->pChar->GetTeam() ) );
					g_ModeUIController.SetKOMsg( TEAM_NONE,	"", "", 0, eDieTeam, szDieName );
				}
				else
				{
					pAttRecord->AddKill( GetModeType(), -GetKillPoint( pAttRecord->pChar->GetTeam() ), false, false );
					pDieRecord->AddDeath( GetModeType(), GetDeathPoint( pDieRecord->pChar->GetTeam() ) );

					g_ModeUIController.SetKOMsg( eAttTeam, szAttName, 
						                         pDieRecord->pChar->GetLastAttackerSkillName(), pDieRecord->pChar->GetLastAttackerItemCode(),
												 eDieTeam, szDieName );
				}
			}

			if ( g_ModeUIController.IsCenterKillInfoWnd() )
				pAttRecord->pChar->SetKillEmoticon( pAttRecord->GetNoDeathKillCount() );
			else
				pAttRecord->pChar->SetKillEmoticon( pAttRecord->GetUniqueTotalKill() );
			pDieRecord->pChar->SetDeathEmoticon();

			ioBaseChar *pOwner = GetOwnerChar();
			if( pOwner )
				g_ModeUIController.SetCenterKillUI( m_ModeType, pAttRecord->GetNoDeathKillCount(), pOwner, pAttRecord->pChar->GetTeam(), pAttRecord->pChar->GetCharName(),
				pDieRecord->pChar->GetLastAttackerItemCode(), pDieRecord->pChar->GetTeam(), pDieRecord->pChar->GetCharName(), pAttRecord->pChar->IsMale() );
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

				if( pDieRecord->pChar->GetTeam() == pOwner->GetTeam() )
					eDieTeam = TEAM_BLUE;
				else
					eDieTeam = TEAM_RED;

				pDieRecord->AddDeath( GetModeType(), GetDeathPoint( pDieRecord->pChar->GetTeam() ) );

				g_ModeUIController.SetKOMsg( TEAM_NONE, "", "", 0, eDieTeam, szDieName );

				pDieRecord->pChar->SetDeathEmoticon();
			}
		}
	}
}

void ioHeadquartersMode::UpdateTimeScoreWnd()
{

}

void ioHeadquartersMode::RenderPreGUI()
{
	if( m_ModeState == MS_PLAY )
	{
		RenderPushEnableCursor();
	}
}

void ioHeadquartersMode::RenderAfterGUI()
{
}

D3DXVECTOR3 ioHeadquartersMode::GetStartPos( TeamType eTeamType, bool bRandom )
{
	int iPosArray = 0;
	if( eTeamType == TEAM_BLUE  )
		iPosArray = m_iBluePosArray;
	else if(eTeamType == TEAM_RED )
		iPosArray = m_iRedPosArray;
	else if( !m_vStartPos.empty() )
		iPosArray =  ( (int)eTeamType -  TEAM_PRIVATE_1 ) ;

	int iMaxStartPos = m_vStartPos.size();
	if( !COMPARE( iPosArray, 0, iMaxStartPos ) )
		return m_vMapCenter;

	D3DXQUATERNION qtRot;
	D3DXQuaternionRotationYawPitchRoll( &qtRot,
		ioMath::RangeRandom( 0.0f, D3DX_PI * 2.0f ),
		0.0f,
		0.0f );

	D3DXVECTOR3 vCenter = m_vStartPos[iPosArray];
	D3DXVECTOR3 vMoveDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, FLOAT1 );

	if( bRandom )
		vCenter += vMoveDir * ioMath::RangeRandom( FLOAT10, FLOAT10 );

	vCenter.y = GetMapHeightByRevial( vCenter.x, vCenter.z );

	return vCenter;
}

D3DXVECTOR3 ioHeadquartersMode::GetRevivalPos( ioBaseChar *pChar )
{
	if(!pChar)
	{
		LOG.PrintTimeAndLog( 0, "ioHeadquartersMode::GetRevivalPos() - Char is NULL" );
		return D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	}

	if( ToNpcChar( pChar ) )  // 용병 NPC는 시작 위치 항상 고정
	{
		ioNpcChar *pNpcChar = ToNpcChar( pChar );
		D3DXVECTOR3 vCenter( pNpcChar->GetStartXPos(), 0.0f, pNpcChar->GetStartZPos() );
		vCenter.y = GetMapHeightByRevial( vCenter.x, vCenter.z );
		return vCenter;
	}

	int iPosArray = 0;
	if( pChar->GetTeam() == TEAM_BLUE  )
		iPosArray = m_iBluePosArray;
	else if(pChar->GetTeam() == TEAM_RED )
		iPosArray = m_iRedPosArray;
	else if( !m_vDefStartPos.empty() )
		iPosArray =  ( (int)pChar->GetTeam() -  TEAM_PRIVATE_1 ) ;

	int iMaxStartPos = m_vDefStartPos.size();
	if( !COMPARE( iPosArray, 0, iMaxStartPos ) )
		return m_vMapCenter;

	D3DXQUATERNION qtRot;
	D3DXQuaternionRotationYawPitchRoll( &qtRot,
		ioMath::RangeRandom( 0.0f, D3DX_PI * 2.0f ),
		0.0f,
		0.0f );

	D3DXVECTOR3 vCenter = m_vDefStartPos[iPosArray];
	D3DXVECTOR3 vMoveDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, FLOAT1 );
	vCenter += vMoveDir * ioMath::RangeRandom( FLOAT10, FLOAT10 );
	vCenter.y = GetMapHeight( vCenter.x, vCenter.z );

	return vCenter;
}

void ioHeadquartersMode::GetMostNearPosList( PositionDistanceSqList &rkResultList, const D3DXVECTOR3 &vTargPos )
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

void ioHeadquartersMode::NotifyEntityDie( ioPlayEntity *pEntity, ioPlayEntity *pKiller, ioBaseChar *pBestAttacker,
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

void ioHeadquartersMode::NotifyCharDropDie( ioBaseChar *pDieChar, ioBaseChar *pKillChar, ioBaseChar *pBestAttacker,
									        float fLastRate, float fBestRate )
{
	OnNotifyCharDie( pDieChar, pKillChar, pBestAttacker, fLastRate, fBestRate );
	UpdateDropDieRecord( pDieChar, pKillChar, pBestAttacker );
}

void ioHeadquartersMode::OnNotifyCharDie( ioBaseChar *pDieChar, ioBaseChar *pKillChar, ioBaseChar *pBestAttacker,
								    	  float fLastRate, float fBestRate )
{   
	if( !pDieChar )
	{
		LOG.PrintTimeAndLog( 0, "ioHeadquartersMode::OnNotifyCharDie() - DieChar is NULL" );
		return;
	}

	HeadquartersRecord *pDieRecord = FindRecordCharName( pDieChar->GetCharName() );
	if( !pDieRecord )
	{
		LOG.PrintTimeAndLog( 0, "ioHeadquartersMode::OnNotifyCharDie - %s Not Exit Char",
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

void ioHeadquartersMode::AddNewRecord( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	HeadquartersRecord *pHeadquartersRecord = FindRecordCharName( pChar->GetCharName() );
	if( !pHeadquartersRecord )
	{
		HeadquartersRecord kRecord;
		kRecord.pChar = pChar;
		kRecord.szName= pChar->GetCharName();
		m_vCharRecordList.push_back( kRecord );
		UpdateUserRank();

		if( !ToNpcChar( pChar ) )
		{
			g_GUIMgr.HideWnd( PRACTICE_HELP_WND );
			g_GUIMgr.HideWnd( SOLDIER_PRACTICE_SELECT_WND );
			g_GUIMgr.HideWnd( SOLDIER_DISPLAY_CHANGE_WND );
			g_GUIMgr.HideWnd( SOLDIER_DISPLAY_SELECT_WND );
		}
	}
	else
	{
		pHeadquartersRecord->pChar = pChar;
	}

	HeadquartersMainWnd *pMainWnd = dynamic_cast<HeadquartersMainWnd*>(g_GUIMgr.FindWnd( HEADQUARTERS_MAIN_WND ));
	if( pMainWnd && pMainWnd->IsShow() )
	{
		pMainWnd->UpdateInfo();
	}
}

void ioHeadquartersMode::RemoveRecord( const ioHashString &rkName )
{
	int iCharCnt = m_vCharRecordList.size();
	bool bFind = false;
	for( int i=0 ; i<iCharCnt ; i++ )
	{
		if( m_vCharRecordList[i].szName == rkName )
		{
			m_vCharRecordList.erase( m_vCharRecordList.begin() + i );
			bFind = true;
			break;
		}
	}

	if( !bFind )
		LOG.PrintTimeAndLog( 0, "Error - ioHeadquartersMode::RemoveRecord(%s)", rkName.c_str() );
	
	UpdateUserRank();

	HeadquartersMainWnd *pMainWnd = dynamic_cast<HeadquartersMainWnd*>(g_GUIMgr.FindWnd( HEADQUARTERS_MAIN_WND ));
	if( pMainWnd && pMainWnd->IsShow() )
	{
		pMainWnd->UpdateInfo();
	}
}

void ioHeadquartersMode::RemoveRecord( int nIndex )
{
	int iCharCnt = m_vCharRecordList.size();
	if( iCharCnt > nIndex )
	{
		m_vCharRecordList.erase( m_vCharRecordList.begin() + nIndex );
		UpdateUserRank();
	}
}

ioBaseChar* ioHeadquartersMode::GetRecordChar( int i )
{
	if( COMPARE( i, 0, GetRecordCharCnt() ) )
		return m_vCharRecordList[i].pChar;

	return NULL;
}

ioBaseChar* ioHeadquartersMode::GetRecordChar( const ioHashString &rkName )
{
	HeadquartersRecord *pRecord = FindRecordCharName( rkName );
	if( pRecord )
		return pRecord->pChar;

	return NULL;
}

int ioHeadquartersMode::GetRecordCharCnt() const
{
	return m_vCharRecordList.size();
}

int ioHeadquartersMode::GetRecordPlayUserCnt()
{
	int iUserCount = 0;
	int iCharCnt = m_vCharRecordList.size();
	for( int i=0 ; i<iCharCnt ; i++ )
	{
		if( ToNpcChar( m_vCharRecordList[i].pChar ) ) continue;

		iUserCount++;
	}
	return iUserCount;
}

ModeRecord* ioHeadquartersMode::FindModeRecord( int iIdx )
{
	if( COMPARE( iIdx, 0, GetRecordCharCnt() ) )
		return &m_vCharRecordList[iIdx];

	return NULL;
}

ModeRecord* ioHeadquartersMode::FindModeRecordCharName( const ioHashString &rkName )
{
	return (ModeRecord*)FindRecordCharName( rkName );
}

HeadquartersRecord* ioHeadquartersMode::FindRecordCharName( const ioHashString &rkName )
{
	int iCharCnt = m_vCharRecordList.size();
	for( int i=0 ; i<iCharCnt ; i++ )
	{
		if( m_vCharRecordList[i].szName == rkName )
			return &m_vCharRecordList[i];
	}

	return NULL;
}

void ioHeadquartersMode::AddEraseNpc( const ioHashString &rkNpcName )
{
	NpcEraseData kEraseData;
	kEraseData.m_szEraseNpc = rkNpcName;
	kEraseData.m_dwEraseTimer = FRAMEGETTIME();
	m_NpcEraseList.push_back( kEraseData );
}

void ioHeadquartersMode::RemoveEraseNpc( const ioHashString &rkNpcName )
{
	int iSize = m_NpcEraseList.size();
	for(int i = 0;i < iSize;i++)
	{
		NpcEraseData &rkNpcData = m_NpcEraseList[i];
		if( rkNpcData.m_szEraseNpc == rkNpcName )
		{
			m_NpcEraseList.erase( m_NpcEraseList.begin() + i );
			return;
		}
	}
}

bool ioHeadquartersMode::GetRoundInfo( int iIndex, RoundInfo &rkInfo )
{
	if( ioPlayMode::GetRoundInfo( iIndex, rkInfo ) )
	{
		HeadquartersRecord *pRecord = (HeadquartersRecord*)FindModeRecord( iIndex );
		if( !pRecord )	return false;
		if( !pRecord->pChar ) return false;

		rkInfo.m_iContribute = pRecord->Contribute.m_iContribute;
		rkInfo.m_iContributePer = pRecord->Contribute.m_iPrevContributePer;
		return true;
	}
	return false;
}

bool ioHeadquartersMode::ProcessUDPPacket( SP2Packet &rkPacket )
{
	if( ioPlayMode::ProcessUDPPacket( rkPacket ) )
		return true;
	return false;
}

void ioHeadquartersMode::OnUserRevival( SP2Packet &rkPacket )
{
	DWORD dwSelectTime;
	ioHashString szName;
	rkPacket >> szName >> dwSelectTime >> m_dwRandomSeed;

	ioBaseChar *pChar = GetRecordChar( szName );
	if( !pChar )
	{
		LOG.PrintTimeAndLog( 0, "ioPlayMode::OnUserRevival - %s Not Exist User", szName.c_str() );
		return;
	}

	if( pChar->IsOwnerChar() )
	{
		LOG.PrintTimeAndLog( 0, "ioPlayMode::OnUserRevival() - CharChange: %d", FRAMEGETTIME() );
		StartChangeCharacter( pChar->GetSelectCharArray(), dwSelectTime );
	}

	D3DXVECTOR3 vRevivalPos = GetRevivalPos( pChar );
	pChar->SetWorldPosition( vRevivalPos );
	pChar->SetReadyState();
}

void ioHeadquartersMode::OnRoundJoin( SP2Packet &rkPacket )
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

			m_iSelectCharacter = g_MyInfo.GetAutoSelectChar( iSelectChar, iSelectChar );

			SP2Packet kPacket( CTPK_CHANGE_CHAR );
			kPacket << m_iSelectCharacter;
			kPacket << false;
			kPacket << pJoinChar->GetCharChangeUniqueIndex() + 1;
			TCPNetwork::SendToServer( kPacket );
		}

		g_ModeHelpMgr.InitModeHelp();
	}
	else
	{
		pJoinChar->SetReadyState();
		if( m_ModeState == MS_PLAY )
		{
			ioBaseChar *pOwner = GetOwnerChar();
			if( pOwner )
			{
				g_ChatMgr.SetJoinUserChatMsg( pJoinChar->GetCharName().c_str(), pJoinChar->GetTeam(), pJoinChar->GetLevel() );
			}
		}
	}

	m_pCreator->ReCalculateDamageRate( false );
}

void ioHeadquartersMode::ApplyModeInfo( SP2Packet &rkPacket )
{
	m_bStopTime = true;

	rkPacket >> m_szMasterName;
	rkPacket >> m_bJoinLock;
	rkPacket >> m_iMaxPlayer;

	m_SingleTeamPosArray.clear();
	int iPosCnt;
	rkPacket >> iPosCnt;
	for( int j=0; j < iPosCnt; ++j )
	{
		int iPosArray;
		rkPacket >> iPosArray;
		m_SingleTeamPosArray.push_back( iPosArray );
	}
}

void ioHeadquartersMode::ApplyCharModeInfo( const ioHashString &rkName, SP2Packet &rkPacket, bool bDieCheck )
{
	bool bRecord;
	rkPacket >> bRecord;

	if( !bRecord ) return;

	HeadquartersRecord *pRecord = FindRecordCharName( rkName );
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

int ioHeadquartersMode::GetRoundWinCnt( TeamType eTeamType )
{
	return 0;
}

void ioHeadquartersMode::ProcessNpcErase()
{
	int iSize = m_NpcEraseList.size();
	for(int i = 0;i < iSize;i++)
	{
		NpcEraseData &rkNpcData = m_NpcEraseList[i];

		// NPC 죽은 후 35초뒤에 삭제.. 한번에 삭제하면 렉이 있을 수 있으니 프레임마다 한개씩 삭제
		if( FRAMEGETTIME() - rkNpcData.m_dwEraseTimer > 35000 )         
		{
			m_pCreator->RemoveUser( rkNpcData.m_szEraseNpc, true );
			RemoveEraseNpc( rkNpcData.m_szEraseNpc );
			return;
		}
	}
}

void ioHeadquartersMode::ContributeConvert()
{
}

void ioHeadquartersMode::DamageContribute( const ioHashString szAttacker, const ioHashString szTarget, float fDamage, DWORD dwItemCode, int iWeaponType )
{	
}

void ioHeadquartersMode::ContributeLOG()
{
	char szLog[MAX_PATH];

	int iCharCnt = m_vCharRecordList.size();
	for( int i=0 ;i<iCharCnt; i++ )
	{
		HeadquartersRecord *pRecord = &m_vCharRecordList[i];
		if( pRecord->pChar )
		{
			sprintf( szLog, "%s[%d] : K%d(%d) D%d(%d) A%d(%d)", pRecord->szName.c_str(), (int)pRecord->pChar->GetTeam(),
																pRecord->Contribute.m_iKillRank + 1, pRecord->Contribute.m_iKill,
																pRecord->Contribute.m_iDamageRank + 1, pRecord->Contribute.m_iDamage,
																pRecord->Contribute.m_iAttackCntRank + 1, pRecord->Contribute.m_iAttackCnt );
			g_ChatMgr.SetSystemMsg( szLog );
		}
	}
}

void ioHeadquartersMode::FillLastPlayRecordInfo( SP2Packet &rkPacket )
{
	int iCharCnt = m_vCharRecordList.size();
	rkPacket << iCharCnt;
	for(int i = 0;i < iCharCnt;i++)
	{
		HeadquartersRecord *pRecord = &m_vCharRecordList[i];
		rkPacket << pRecord->pChar->GetCharName();
		rkPacket << pRecord->Contribute.m_iContribute;
		rkPacket << pRecord->GetUniqueTotalKill();
		rkPacket << pRecord->GetUniqueTotalDeath();
		rkPacket << g_VictoriesMgr.GetVictories( pRecord->pChar->GetCharName() );
	}
}

void ioHeadquartersMode::FillPlayRecordInfo( SP2Packet &rkPacket )
{
	int iCharCnt = m_vCharRecordList.size();
	rkPacket << iCharCnt;
	for(int i = 0;i < iCharCnt;i++)
	{
		HeadquartersRecord *pRecord = &m_vCharRecordList[i];
		rkPacket << pRecord->pChar->GetCharName();
		rkPacket << pRecord->GetUniqueTotalKill();
		rkPacket << pRecord->GetUniqueTotalDeath();
		rkPacket << pRecord->GetNoDeathKillCount();
	}
}

void ioHeadquartersMode::ApplyPlayRecordInfo( SP2Packet &rkPacket )
{
	int iCharCnt;
	rkPacket >> iCharCnt;
	for(int i = 0;i < iCharCnt;i++)
	{
		ioHashString szName;
		int iUniqueTotalKill, iUniqueTotalDeath, iNoDeathKillCount;
		rkPacket >> szName >> iUniqueTotalKill >> iUniqueTotalDeath >> iNoDeathKillCount;

		HeadquartersRecord *pRecord = FindRecordCharName( szName );
		if( pRecord )
		{
			pRecord->iUniqueTotalKill = iUniqueTotalKill;
			pRecord->iUniqueTotalDeath= iUniqueTotalDeath;
			pRecord->iNoDeathKillCount = iNoDeathKillCount;
		}
	}
}

void ioHeadquartersMode::ContributeResultSync( const ioHashString &szName, int iContribute )
{
	HeadquartersRecord *pRecord = FindRecordCharName( szName );
	if( !pRecord ) return;

	pRecord->Contribute.m_bLastResultSync = true;
	pRecord->Contribute.m_iContribute     = iContribute;
}

int ioHeadquartersMode::GetCurRank( const ioHashString& szCharName )
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

bool ioHeadquartersMode::IsOneMyChar()
{
	if( m_dwCharState == STATE_OPEN_CHAR )
		return false;
	return true;
}

int ioHeadquartersMode::GetContributePerChar( const ioHashString& szName )
{
	int iCharCnt = m_vCharRecordList.size();

	for(int i = 0;i < iCharCnt;i++)
	{
		HeadquartersRecord *pRecord = &m_vCharRecordList[i];
		ErrorReport::SetPosition( 3000, 7 );
		if( !pRecord )	continue;
		ErrorReport::SetPosition( 3000, 8 );
		if( !pRecord->pChar ) continue;
		ErrorReport::SetPosition( 3000, 9 );

		__try
		{
			if( pRecord->pChar->GetCharName() == szName )
			{
				ErrorReport::SetPosition( 3000, 10 );
				return pRecord->Contribute.m_iPrevContributePer;
				ErrorReport::SetPosition( 3000, 11 );
			}
		}
		__except (1)
		{
			RemoveRecord( i );
			return 0;
		}
	}

	ErrorReport::SetPosition( 3000, 12 );
	return 0;
}

void ioHeadquartersMode::OnChangeCharNextProcess( ioBaseChar *pChar, bool bNewChar )
{
	if( !pChar ) return;
	if( pChar->IsOwnerChar() )
	{
		if( pChar->GetExperienceMode() != EMS_NONE )
			pChar->SetExperienceMode( EMS_NONE, "", 0 );
	}
}

bool ioHeadquartersMode::ProcessTCPPacket( SP2Packet &rkPacket )
{
	ErrorReport::SetPosition( 1032, 1 );
	if( ioPlayMode::ProcessTCPPacket( rkPacket ) )
		return true;

	ErrorReport::SetPosition( 1032, 2 );

	switch( rkPacket.GetPacketID() )
	{
	case STPK_MONSTER_REVIVAL:
		ErrorReport::SetPosition( 1032, 1 );
		OnMonsterRevival( rkPacket );
		ErrorReport::SetPosition( 1032, 2 );
		return true;
	case STPK_CHANGE_CHAR_BY_NPC:
		ErrorReport::SetPosition( 1032, 3 );
		OnChangeCharByNPC( rkPacket );
		ErrorReport::SetPosition( 1032, 4 );
		return true;
	case STPK_HEADQUARTERS_STATE_CHANGE:
		ErrorReport::SetPosition( 1032, 5 );
		OnChangeCharState( rkPacket );
		ErrorReport::SetPosition( 1032, 6 );
		return true;
	case STPK_MODE_EXTRAITEM_UPDATE:
		ErrorReport::SetPosition( 1032, 7 );
		OnModeExtraItemUpdate( rkPacket );
		ErrorReport::SetPosition( 1032, 8 );
		return true;
	case STPK_MODE_MEDAL_UPDATE:
		ErrorReport::SetPosition( 1032, 9 );
		OnModeCharMedalUpdate( rkPacket );
		ErrorReport::SetPosition( 1032, 10 );
		return true;
	case STPK_MODE_CUSTOM_MEDAL_UPDATE:
		OnModeCharCustomMedalUpdate( rkPacket );
		return true;
	case STPK_MODE_CHAR_INSERT_DELETE:
		ErrorReport::SetPosition( 1032, 11 );
		OnModeCharInsertDelete( rkPacket );
		ErrorReport::SetPosition( 1032, 12 );
		return true;
	case STPK_HEADQUARTERS_COMMAND:
		ErrorReport::SetPosition( 1032, 13 );
		OnHeadquartersCommand( rkPacket );
		ErrorReport::SetPosition( 1032, 14 );
		return true;
	case STPK_MONSTER_FORCE_DIE:
		ErrorReport::SetPosition( 1032, 15 );
		OnMonsterForceDie( rkPacket );
		ErrorReport::SetPosition( 1032, 16 );
		return true;
	}

	return false;
}

ioNpcChar *ioHeadquartersMode::_OnCharacterSet( DWORD dwNpcCode, const ioHashString &rkName, const ioHashString &rkSyncName, int iTeamType, SP2Packet &rkPacket, ioNpcChar *pAlreadyNpc )
{
	ioNpcChar *pNpcChar = pAlreadyNpc;
	if( dwNpcCode == 0 )
	{
		if( m_dwCharState == STATE_OPEN_CHAR )
		{
			// - 내가 보유하고있는 용병을 기준으로 생성
			if( pNpcChar == NULL)
			{
				pNpcChar = g_NpcMgr.CreateNpcCharByMyCharacter( rkPacket, m_pCreator, this );
			}
			else
			{
				DWORD dwCharIndex;
				rkPacket >> dwCharIndex;
			}

			if( !pNpcChar ) return NULL;

			pNpcChar->SetCharName( rkName );
			pNpcChar->SetSyncUser( rkSyncName );		
			pNpcChar->SetTeam( (TeamType)iTeamType  );

			// 모션 아이템
			DWORD dwDisplayMotion;
			rkPacket >> dwDisplayMotion;
			pNpcChar->SetDisplayMotion( dwDisplayMotion );

			// 장비
			float fTemp[MAX_INVENTORY] = { 0.0f, 0.0f, 0.0f, 0.0f };
			m_pCreator->ChangeCharEquipItem( pNpcChar, fTemp, rkPacket );

			// 메달
			int k = 0;
			MedalSyncInfoVec kMedalList;
			g_MyInfo.GetMedalItemTypeVec( kMedalList, pNpcChar->GetCharacterInfo().m_class_type );			
			int iMedalSize = kMedalList.size();
			pNpcChar->ClearMedalItem();
			for(k = 0;k < iMedalSize;k++)
				pNpcChar->SetCustomMedalItem( kMedalList[k].m_iItemType, kMedalList[k].m_iCustomIndex, kMedalList[k].m_iStat, true );

			// 성장
			ioGrowthLevel *pLevel = m_pCreator->GetGrowthLevel( rkName );
			if( pLevel == NULL )
			{
				AddGrowthLevel( rkName );
			}

			pLevel = m_pCreator->GetGrowthLevel( rkName );
			if( pLevel == NULL )
			{
				LOG.PrintTimeAndLog( 0, "_OnCharacterSet : %s - None Growth", rkName.c_str() );
			}
			else
			{
				ioGrowthLevel *pGrowthLevel = g_MyInfo.GetGrowthLevel();
				if( pGrowthLevel )
				{
					for(k = 0;k < MAX_CHAR_GROWTH;k++)
					{
						pLevel->SetCharGrowthLevel( pNpcChar->GetCharacterInfo().m_class_type, k, pGrowthLevel->GetCharGrowthLevel( pNpcChar->GetCharacterInfo().m_class_type, k, false ) );
					}

					for(k = 0;k < MAX_ITEM_GROWTH;k++)
					{
						pLevel->SetItemGrowthLevel( pNpcChar->GetCharacterInfo().m_class_type, k, pGrowthLevel->GetItemGrowthLevel( pNpcChar->GetCharacterInfo().m_class_type, k, false ) );
					}
				}
				m_pCreator->UpdateGrowthDataOne( rkName );
			}

			pNpcChar->SetClassLevelShow( true );
			pNpcChar->CheckDefaultReinforce();
		}
		else
		{
			// - 서버에서 보내준 내용으로 생성
			if( pNpcChar == NULL)
			{
				pNpcChar = g_NpcMgr.CreateNpcCharByCharacter( rkPacket, m_pCreator, this );
			}
			else
			{
				int iClassLevel;
				DWORD dwCharIndex;
				rkPacket >> dwCharIndex >> iClassLevel;

				CHARACTER kCharInfo;
				rkPacket >> kCharInfo;
			}

			if( !pNpcChar ) return NULL;

			pNpcChar->SetCharName( rkName );
			pNpcChar->SetSyncUser( rkSyncName );		
			pNpcChar->SetTeam( (TeamType)iTeamType  );

			// 모션 아이템
			DWORD dwDisplayMotion;
			rkPacket >> dwDisplayMotion;
			pNpcChar->SetDisplayMotion( dwDisplayMotion );

			// 장비
			float fTemp[MAX_INVENTORY] = { 0.0f, 0.0f, 0.0f, 0.0f };
			m_pCreator->ChangeCharEquipItem( pNpcChar, fTemp, rkPacket );

			// 메달
			int k = 0;
			int iMedalSize;
			rkPacket >> iMedalSize;
			pNpcChar->ClearMedalItem();
			for(k = 0;k < iMedalSize;k++)
			{
				int iMedalCode;
				rkPacket >> iMedalCode;
				pNpcChar->SetMedalItem( iMedalCode, true );	
			}
			int iMaxCustomMedal;
			rkPacket >> iMaxCustomMedal;
			for( k = 0; k < iMaxCustomMedal; ++k )
			{
				MedalSyncInfo cInfo;
				rkPacket >> cInfo.m_iItemType >> cInfo.m_iCustomIndex;
				for( int j = 0; j < 8; j++ )
					rkPacket >> cInfo.m_iStat[j];

				pNpcChar->SetCustomMedalItem( cInfo.m_iItemType, cInfo.m_iCustomIndex, cInfo.m_iStat, true );
			}

			// 성장
			ioGrowthLevel *pLevel = m_pCreator->GetGrowthLevel( rkName );
			if( pLevel == NULL )
			{
				AddGrowthLevel( rkName );
			}

			pLevel = m_pCreator->GetGrowthLevel( rkName );
			if( pLevel == NULL )
			{
				BYTE iLevel;
				for(int i =0;i < MAX_CHAR_GROWTH + MAX_ITEM_GROWTH;++i)
					rkPacket >> iLevel;
				LOG.PrintTimeAndLog( 0, "OnTurnStart : %s - None Growth", rkName.c_str() );
			}
			else
			{
				pLevel->ApplyGrowthLevelDataRoomSync( pNpcChar->GetCharacterInfo().m_class_type, rkPacket );
				m_pCreator->UpdateGrowthDataOne( rkName );
			}

			pNpcChar->SetClassLevelShow( true );
			pNpcChar->CheckDefaultReinforce();
		}
	}
	else
	{
		if( pNpcChar == NULL )
		{
			pNpcChar = g_NpcMgr.CreateNpcChar( NPC_NOT_INDEX, dwNpcCode, m_pCreator, this );
		}
		if( !pNpcChar ) return NULL;

		pNpcChar->SetCharName( rkName );
		pNpcChar->SetSyncUser( rkSyncName );		
		pNpcChar->SetTeam( (TeamType)iTeamType  );
	}
	return pNpcChar;
}

void ioHeadquartersMode::OnTurnStart( SP2Packet &rkPacket )
{
	rkPacket >> m_dwCharState;

	ioHashStringVec kRemoveList;
	RecordList::iterator iter;
	for( iter=m_vCharRecordList.begin() ; iter!=m_vCharRecordList.end() ; ++iter )
	{
		HeadquartersRecord &rkRecord = *iter;

		ioBaseChar* pFindChar = m_pCreator->GetBaseChar( rkRecord.szName );
		if( pFindChar )
		{
			if( ToNpcChar( pFindChar ) )
				kRemoveList.push_back( rkRecord.szName );
		}
	}

	int i = 0;
	for(i = 0;i < (int)kRemoveList.size();i++)
	{
		m_pCreator->RemoveUser( kRemoveList[i], true );
	}
	kRemoveList.clear();

	int iMonsterCnt;
	rkPacket >> iMonsterCnt;
	for(i = 0;i < iMonsterCnt;i++)
	{
		int iTeamType;
		DWORD dwNpcCode, dwStartTime;
		float fStartXPos, fStartZPos;
		ioHashString kNpcName, kSyncName;
		rkPacket >> dwNpcCode >> kNpcName >> kSyncName >> dwStartTime >> fStartXPos >> fStartZPos >> iTeamType;
		ioBaseChar *pAlreadyChar = NULL;
		HeadquartersRecord *pRecord = FindRecordCharName( kNpcName );
		if( pRecord && pRecord->pChar )
			pAlreadyChar = pRecord->pChar;

		ioNpcChar *pNpcChar = _OnCharacterSet( dwNpcCode, kNpcName, kSyncName, iTeamType, rkPacket, ToNpcChar( pAlreadyChar ) );
		if( pNpcChar == NULL ) continue;

		// 용병 NPC
		if( dwNpcCode == 0 && m_dwCharState != STATE_DISPLAY_CHAR )
			fStartXPos = fStartZPos = 0.0f;

		if( fStartXPos == 0.0f && fStartZPos == 0.0f )
		{
			D3DXVECTOR3 vPos = GetStartPos( (TeamType)iTeamType, false );
			fStartXPos = vPos.x;
			fStartZPos = vPos.z;
		}

		pNpcChar->FillMaxHP();
		pNpcChar->StartAI( dwStartTime , fStartXPos, fStartZPos, true );
		pNpcChar->SetGradeUpRandomSeed( kNpcName.GetHashCode() );
		pNpcChar->ReSetGradeLevel( 0 );

		if( pAlreadyChar == NULL )
			AddNewRecord( pNpcChar );
	}
}

void ioHeadquartersMode::OnMonsterRevival( SP2Packet &rkPacket )
{
	int iTeamType;
	DWORD dwNpcCode;
	ioHashString kNpcName, kSyncName;
	float fStartXPos, fStartZPos;
	rkPacket >> dwNpcCode >> kNpcName >> kSyncName >> fStartXPos >> fStartZPos >> iTeamType;

	ioNpcChar *pNpcChar = NULL;
	HeadquartersRecord *pRecord = FindRecordCharName( kNpcName );
	if( pRecord && ToNpcChar( pRecord->pChar ) )
	{
		pNpcChar = ToNpcChar( pRecord->pChar );
		if( pNpcChar )
		{
			pNpcChar->Revival();
			pNpcChar->SetState( CS_LOADING );
			pNpcChar->FillMaxHP();
			pNpcChar->SetCharName( kNpcName );
			pNpcChar->SetSyncUser( kSyncName );		
			pNpcChar->SetTeam( (TeamType)iTeamType );			
			pNpcChar->ReSetGradeLevel( 0 );
			pNpcChar->ReEquipItem();

			if( dwNpcCode == 0 )
			{
				pNpcChar = _OnCharacterSet( dwNpcCode, kNpcName, kSyncName, iTeamType, rkPacket, pNpcChar );
			}

			if( pNpcChar )
			{
				// 용병 NPC
				if( dwNpcCode == 0 && m_dwCharState != STATE_DISPLAY_CHAR )
					fStartXPos = fStartZPos = 0.0f;

				if( fStartXPos == 0.0f && fStartZPos == 0.0f )
				{
					D3DXVECTOR3 vPos = GetRevivalPos( pNpcChar );
					fStartXPos = vPos.x;
					fStartZPos = vPos.z;
				}
				pNpcChar->StartAI( 0, fStartXPos, fStartZPos, true );
			}
		}
	}
	else
	{
		ioNpcChar *pNpcChar = _OnCharacterSet( dwNpcCode, kNpcName, kSyncName, iTeamType, rkPacket, NULL );
		if( pNpcChar == NULL ) return;

		// 용병 NPC
		if( dwNpcCode == 0 && m_dwCharState != STATE_DISPLAY_CHAR )
			fStartXPos = fStartZPos = 0.0f;

		if( fStartXPos == 0.0f && fStartZPos == 0.0f )
		{
			D3DXVECTOR3 vPos = GetRevivalPos( pNpcChar );
			fStartXPos = vPos.x;
			fStartZPos = vPos.z;
		}
		pNpcChar->FillMaxHP();
		pNpcChar->StartAI( 0, fStartXPos, fStartZPos );
		pNpcChar->ReSetGradeLevel( 0 );
		AddNewRecord( pNpcChar );
	}		
}

void ioHeadquartersMode::OnMonsterSyncChange( SP2Packet &rkPacket )
{
	int iMaxNpc;
	rkPacket >> iMaxNpc;
	for(int i = 0;i < iMaxNpc;i++)
	{
		ioHashString szNpcName, szSyncName;
		rkPacket >> szNpcName >> szSyncName;

		HeadquartersRecord *pRecord = FindRecordCharName( szNpcName );
		if( !pRecord ) continue;

		ioNpcChar *pNpc = ToNpcChar( pRecord->pChar );
		if( pNpc == NULL ) continue;

		pNpc->SetSyncUser( szSyncName );
	}
}

void ioHeadquartersMode::OnMonsterInfoSync( SP2Packet &rkPacket )
{
	rkPacket >> m_dwCharState;

	int i, iMaxNpc;
	// 일반 NPC
	rkPacket >> iMaxNpc;
	for(i = 0;i < iMaxNpc;i++)
	{
		int iTeamType;
		DWORD dwNpcCode;
		ioHashString kNpcName, kSyncName;
		rkPacket >> dwNpcCode >> kNpcName >> kSyncName >> iTeamType;

		ioNpcChar *pNpcChar = _OnCharacterSet( dwNpcCode, kNpcName, kSyncName, iTeamType, rkPacket, NULL );
		if( pNpcChar == NULL ) continue;

		pNpcChar->SetGradeUpRandomSeed( kNpcName.GetHashCode() );
		pNpcChar->SetState( CS_DELAY );
		AddNewRecord( pNpcChar );			
	}
}

void ioHeadquartersMode::OnChangeCharByNPC( SP2Packet &rkPacket )
{
	ioHashString kLeaveName;
	rkPacket >> kLeaveName;

	if( kLeaveName.IsEmpty() )
		return;

	// 상태는 동기화 시키면 안되다 상태는 내부 변수까지 맞춰야하는데 손이 너무 많이 간다.
	// 위치 이동
	D3DXVECTOR3 vOwnerPos;
	D3DXQUATERNION qtOwnerRot;
	D3DXQUATERNION qtWorldOrientation;
	float     fOwnerHP;
	HeadquartersRecord *pRecord = FindRecordCharName( kLeaveName );
	if( pRecord && pRecord->pChar )
	{
		ioBaseChar *pOwnerChar = m_pCreator->GetOwnerChar();
		if( pOwnerChar )
		{
			vOwnerPos	= pOwnerChar->GetWorldPosition();
			qtOwnerRot	= (D3DXQUATERNION)pOwnerChar->GetTargetRot();
			qtWorldOrientation= (D3DXQUATERNION)pOwnerChar->GetWorldOrientation();
			fOwnerHP    = pOwnerChar->GetHP().m_fCurValue;

			pOwnerChar->SetWorldPosition( pRecord->pChar->GetWorldPosition() );
			pOwnerChar->SetTargetRot( pRecord->pChar->GetTargetRot() );	
			pOwnerChar->SetWorldOrientation( pRecord->pChar->GetWorldOrientation() );
			pOwnerChar->SetCurHP( pRecord->pChar->GetHP().m_fCurValue );
			pOwnerChar->ChangeDirectionExplicitIfDirChanged( false );
		}
	}
	m_pCreator->RemoveUser( kLeaveName, true );
    
	int iTeamType;
	DWORD dwNpcCode;
	ioHashString kNpcName, kSyncName;
	rkPacket >> dwNpcCode >> kNpcName >> kSyncName >> iTeamType;
	if( kNpcName.IsEmpty() )
		return;

	ioNpcChar *pNpcChar = _OnCharacterSet( dwNpcCode, kNpcName, kSyncName, iTeamType, rkPacket, NULL );
	if( pNpcChar == NULL ) return;

	pNpcChar->SetGradeUpRandomSeed( kNpcName.GetHashCode() );
	pNpcChar->SetState( CS_DELAY );
	pNpcChar->SetWorldPosition( vOwnerPos );
	pNpcChar->SetTargetRot( qtOwnerRot );		
	pNpcChar->SetWorldOrientation( qtWorldOrientation );
	pNpcChar->SetCurHP( fOwnerHP );
	AddNewRecord( pNpcChar );
}

void ioHeadquartersMode::OnChangeCharState( SP2Packet &rkPacket )
{
	TCPNetwork::MouseBusy( false );

	bool bOK;
	rkPacket >> bOK;

	if( bOK == false ) return;

	rkPacket >> m_dwCharState;

	ioHashStringVec kRemoveList;
	RecordList::iterator iter;
	iter = m_vCharRecordList.begin();
	while( iter != m_vCharRecordList.end() )
	{
		HeadquartersRecord &rkRecord = *iter;
		ErrorReport::SetPosition( 1032, 17 );
		ioBaseChar* pFindChar = m_pCreator->GetBaseChar( rkRecord.szName );
		ErrorReport::SetPosition( 1032, 18 );
		if( pFindChar )
		{
			ErrorReport::SetPosition( 1032, 19 );
			if( ToNpcChar( pFindChar ) )
			{
				ErrorReport::SetPosition( 1032, 20 );
				kRemoveList.push_back( rkRecord.szName );
			}
			ErrorReport::SetPosition( 1032, 21 );
			++iter;
			ErrorReport::SetPosition( 1032, 22 );
		}
		else
		{
			ErrorReport::SetPosition( 1032, 23 );
			iter = m_vCharRecordList.erase( iter );
			ErrorReport::SetPosition( 1032, 24 );
		}
	}

	for(int i = 0;i < (int)kRemoveList.size();i++)
	{
		ErrorReport::SetPosition( 1032, 19 );
		m_pCreator->RemoveUser( kRemoveList[i], true );
		ErrorReport::SetPosition( 1032, 20 );
	}
	kRemoveList.clear();
}

void ioHeadquartersMode::OnModeExtraItemUpdate( SP2Packet &rkPacket )
{
	ioHashString kName;
	rkPacket >> kName;

	HeadquartersRecord *pRecord = FindRecordCharName( kName );
	if( pRecord && pRecord->pChar )
	{
		ioBaseChar *pChangeChar = pRecord->pChar;

		DWORD dwNewMaleCustom, dwNewFemaleCustom;
		int iGameIndex, iNewItemCode, iNewReinforce;
		rkPacket >> iGameIndex >> iNewItemCode >> iNewReinforce >> dwNewMaleCustom >> dwNewFemaleCustom;

		ioItem *pItem = NULL;
		if( iNewItemCode > 0 )
			pItem = m_pCreator->CreateItem( iNewItemCode );

		if( pItem )
		{
			pItem->ReLoadProperty();
			pItem->SetOwnerName( kName );
			pItem->SetItemCreateIndex( iGameIndex );
			pItem->SetItemReinforce( iNewReinforce );
			pItem->SetItemCustom( dwNewMaleCustom, dwNewFemaleCustom );

			// 코스튬 관련 (코스튬 동기화 : 서버에서 받을수도 있음)
			int nSubType = pItem->GetType()-1;
			if ( COMPARE( nSubType, 0, MAX_INVENTORY ) )
			{
				pItem->SetCostumeCustom( pRecord->pChar->GetCharacterInfo().m_EquipCostume[nSubType].m_Costume_Male_Custom, pRecord->pChar->GetCharacterInfo().m_EquipCostume[nSubType].m_Costume_Female_Custom );
				pItem->SetCostume( pRecord->pChar->GetCharacterInfo().m_EquipCostume[nSubType].m_CostumeCode, pRecord->pChar->IsOwnerChar() );
			}

			float fPreSkillGauge = 0.0f;
			ioItem *pPreItem = pChangeChar->EquipItem( pItem, true );
			if( pPreItem )
				fPreSkillGauge = pPreItem->GetCurSkillGuage();

			pItem->SetCurSkillGauge( fPreSkillGauge );

			// 용병교체시 총알, 레이져 충전량 관련 처리
			if( pPreItem && pItem->GetType() == ioItem::IT_WEAPON )
			{
				int iCurBullet = pPreItem->GetCurBullet();
				pItem->SetCurBullet( iCurBullet );
			}
			SAFEDELETE( pPreItem );
		}
		pChangeChar->CheckDefaultReinforce();
	}
}

void ioHeadquartersMode::OnModeCharMedalUpdate( SP2Packet &rkPacket )
{
	ioHashString kName;
	rkPacket >> kName;

	HeadquartersRecord *pRecord = FindRecordCharName( kName );
	if( pRecord && pRecord->pChar )
	{
		ioBaseChar *pChangeChar = pRecord->pChar;

		bool bEquip;
		int  iMedalType;
		rkPacket >> iMedalType >> bEquip;
		pChangeChar->SetMedalItem( iMedalType, bEquip );
	}
}

void ioHeadquartersMode::OnModeCharCustomMedalUpdate( SP2Packet &rkPacket )
{
	ioHashString kName;
	rkPacket >> kName;

	HeadquartersRecord *pRecord = FindRecordCharName( kName );
	if( pRecord && pRecord->pChar )
	{
		ioBaseChar *pChangeChar = pRecord->pChar;

		bool bEquip;
		int  iMedalType;
		int iCustomIndex;
		int iStat[8];
		rkPacket >> iMedalType >> iCustomIndex >> bEquip;
		for( int i = 0; i < 8; i++ )
			rkPacket >> iStat[i];

		pChangeChar->SetCustomMedalItem( iMedalType, iCustomIndex, iStat, bEquip );
	}
}

void ioHeadquartersMode::OnModeCharGrowthUpdate( const ioHashString &rkName, int iClassType, int iSlot, bool bItem, int iUpLevel )
{
	if( iUpLevel == 0 )
		return;

	RecordList::iterator iter;
	for( iter=m_vCharRecordList.begin() ; iter!=m_vCharRecordList.end() ; ++iter )
	{
		HeadquartersRecord &rkRecord = *iter;
		if( rkRecord.pChar == NULL ) continue;
		if( ToNpcChar( rkRecord.pChar ) == NULL ) continue;
		if( rkRecord.pChar->GetCharacterInfo().m_class_type != iClassType ) continue;

		ioGrowthLevel *pLevel = GetGrowthLevel( rkRecord.pChar->GetCharName() );
		if( pLevel )
		{
			if( bItem )
				pLevel->ApplyItemGrowthLevelUp( iClassType, iSlot, iUpLevel );
			else
				pLevel->ApplyCharGrowthLevelUp( iClassType, iSlot, iUpLevel );
		}
		m_pCreator->UpdateGrowthDataOne( rkRecord.pChar->GetCharName() );
	}

	HeadquartersRecord *pRecord = FindRecordCharName( rkName );
	if( pRecord && pRecord->pChar && ToNpcChar( pRecord->pChar ) == NULL )
	{
		m_pCreator->UpdateGrowthDataOne( rkName );
	}
}

void ioHeadquartersMode::OnModeCharInsertDelete( SP2Packet &rkPacket )
{
	bool bInsert;
	rkPacket >> bInsert;

	if( bInsert )
	{
		if( m_dwCharState != STATE_OPEN_CHAR )
			return;

		int iTeamType;
		DWORD dwNpcCode, dwStartTime;
		float fStartXPos, fStartZPos;
		ioHashString kNpcName, kSyncName;
		rkPacket >> dwNpcCode >> kNpcName >> kSyncName >> dwStartTime >> fStartXPos >> fStartZPos >> iTeamType;

		HeadquartersRecord *pRecord = FindRecordCharName( kNpcName );
		if( pRecord && pRecord->pChar )
		{
			m_pCreator->RemoveUser( kNpcName, true );
		}

		ioNpcChar *pNpcChar = _OnCharacterSet( dwNpcCode, kNpcName, kSyncName, iTeamType, rkPacket, NULL );
		if( pNpcChar == NULL ) return;

		// 용병 NPC
		if( dwNpcCode == 0 && m_dwCharState != STATE_DISPLAY_CHAR )
			fStartXPos = fStartZPos = 0.0f;

		if( fStartXPos == 0.0f && fStartZPos == 0.0f )
		{
			D3DXVECTOR3 vPos = GetStartPos( (TeamType)iTeamType, false );
			fStartXPos = vPos.x;
			fStartZPos = vPos.z;
		}

		pNpcChar->FillMaxHP();
		pNpcChar->StartAI( dwStartTime , fStartXPos, fStartZPos, true );
		pNpcChar->SetGradeUpRandomSeed( kNpcName.GetHashCode() );
		pNpcChar->ReSetGradeLevel( 0 );
		AddNewRecord( pNpcChar );
	}
	else
	{
		ioHashString kDeleteName;
		rkPacket >> kDeleteName;
		m_pCreator->RemoveUser( kDeleteName, true );
	}
}

void ioHeadquartersMode::OnHeadquartersCommand( SP2Packet &rkPacket )
{
	int iCommand;
	rkPacket >> iCommand;
	switch( iCommand )
	{
	case HEADQUARTERS_CMD_JOINLOCK_CHANGE:
		{			
			rkPacket >> m_bJoinLock;
			HeadquartersMainWnd *pMainWnd = dynamic_cast<HeadquartersMainWnd*>(g_GUIMgr.FindWnd( HEADQUARTERS_MAIN_WND ));
			if( pMainWnd && pMainWnd->IsShow() )
			{
				pMainWnd->UpdateInfo();
			}
		}
		break;
	case HEADQUARTERS_CMD_MAXPLAYER_CHANGE:
		{
			if( m_szMasterName == g_MyInfo.GetPublicID() )
				TCPNetwork::MouseBusy( false );

			rkPacket >> m_iMaxPlayer;
			HeadquartersMainWnd *pMainWnd = dynamic_cast<HeadquartersMainWnd*>(g_GUIMgr.FindWnd( HEADQUARTERS_MAIN_WND ));
			if( pMainWnd && pMainWnd->IsShow() )
			{
				pMainWnd->UpdateInfo();
			}
		}
		break;
	case HEADQUARTERS_CMD_KICK_OUT:
		{
			ioHashString kRoomKickOutUser;
			rkPacket >> kRoomKickOutUser;
			m_pCreator->SetRoomKickOutUser( kRoomKickOutUser );
			if( kRoomKickOutUser == g_MyInfo.GetPublicID() )
			{
				int iRoomIndex;
				rkPacket >> iRoomIndex;
				g_MyInfo.SetKickOutPlaza( iRoomIndex );
				g_GUIMgr.SetReserveMsgBox( MB_HEADQUARTERS_KICK_OUT );
			}
			else
			{
				ioComplexStringPrinter kPrinter;
				g_ChatMgr.PresetChatComplexString( MSG_INFO_CHAT, kPrinter );
				kPrinter.SetTextColor( TCT_DEFAULT_VIOLET );
				kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(1), kRoomKickOutUser.c_str() );
				kPrinter.SetTextColor( TCT_DEFAULT_RED );
				kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(2) );
				g_ChatMgr.SetChatComplexString( STR(3), kPrinter );
			}
		}
		break;
	case HEADQUARTERS_CMD_MASTER_LOGOUT:
		{
			if( g_App.GetConnectedMoveMode() == ioApplication::CMM_LOBBY )
			{
				// EXIT
				SP2Packet kPacket( CTPK_EXIT_ROOM );
				kPacket << EXIT_ROOM_LOBBY << 0 << true;
				TCPNetwork::SendToServer( kPacket );
				TCPNetwork::MouseBusy( true );
				ioMyRoomMode::SetMyRoomType( MRT_LOBBY );
			}
			else
			{				
				g_App.SendSearchModeJoin();				
			}			
			g_GUIMgr.SetReserveMsgBox( MB_HEADQUARTERS_OUT_MASTER_LOGOUT );
		}
		break;
	case HEADQUARTERS_CMD_MOTION_CHANGE:
		{
			int iClassType;
			DWORD dwEtcItem;
			rkPacket >> iClassType >> dwEtcItem;

			RecordList::iterator iter;
			for( iter=m_vCharRecordList.begin() ; iter!=m_vCharRecordList.end() ; ++iter )
			{
				HeadquartersRecord &rkRecord = *iter;				
				ioNpcChar *pNpcChar = ToNpcChar( rkRecord.pChar );
				if( pNpcChar == NULL ) continue;
				if( rkRecord.pChar->GetCharacterInfo().m_class_type != iClassType ) continue;

				pNpcChar->SetDisplayMotion( dwEtcItem );
				break;
			}
		}
		break;
	}
}

void ioHeadquartersMode::OnMonsterForceDie( SP2Packet &rkPacket )
{
	int iMaxNPC;
	rkPacket >> iMaxNPC;
	for(int i = 0;i < iMaxNPC;i++)
	{
		ioHashString kNpcName;
		rkPacket >> kNpcName;
		// 소환물 삭제
		m_pCreator->ClearUserCreateEntities( kNpcName );

		HeadquartersRecord *pRecord = FindRecordCharName( kNpcName );
		if( pRecord && pRecord->pChar )
		{
			pRecord->pChar->SetDieState();
			AddEraseNpc( pRecord->pChar->GetCharName() );
		}
	}
}

void ioHeadquartersMode::ChangeRecordCharName( const ioHashString &rszCharName, const ioHashString &rszNewCharName )
{
	HeadquartersRecord *pRecord = FindRecordCharName( rszCharName );
	if( pRecord )
		pRecord->szName = rszNewCharName;
}