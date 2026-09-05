
#include "stdafx.h"

#include "ioTrainingMode.h"

#include "ioPlayStage.h"
#include "ioNpcChar.h"
#include "ioComplexStringPrinter.h"

#include "ioExcavationManager.h"
#include "ioReservedTask.h"

#include "GUI/MiniMap.h"
#include "GUI/ExcavationWnd.h"
#include "GUI/RunningManNameWnd.h"
#include "GUI/PlazaNPCGaugeWnd.h"
#include "GUI/AniTextWnd.h"
#include "GUI/RoundStateMainWnd.h"
#include "GUI/PlazaMainWnd.h"
#include "GUI/MiniSoldierSelectWnd.h"
#include "GUI/TrainingFuncMenuWnd.h"

#include "ioExtendSoundManager.h"

ioTrainingMode::ioTrainingMode( ioPlayStage *pCreator )
: ioPlayMode( pCreator )
{
	m_iSymbolAlphaRate  = 127;

	m_bStopTime = true;
	m_ModeType = MT_TRAINING;

	m_SingleTeamPosArray.clear();

	m_dwPrePeriodTime = 0;

	m_pRunningManTitleLeft  = NULL;
	m_pRunningManTitleCenter= NULL;
	m_pRunningManTitleRight = NULL;

	m_dwBugBearDieTimer = 0;

	m_bAwakening = false;
	m_bNameTitleUse = false;

	m_vCharRecordList.reserve( 100 );
}

ioTrainingMode::~ioTrainingMode()
{
	ClearRunningManDeco();

	ClearMode();
}

void ioTrainingMode::ClearRunningManDeco()
{
	m_RunningManDecoList.clear();
	SAFEDELETE( m_pRunningManTitleLeft );
	SAFEDELETE( m_pRunningManTitleCenter );
	SAFEDELETE( m_pRunningManTitleRight );
}

void ioTrainingMode::ClearMode()
{
	g_ExcavationMgr.SetPlayStage(NULL);
	g_GUIMgr.HideWnd( PLAZA_WND );
	g_GUIMgr.HideWnd( TIMEGATE_SEARCH );

	ioPlayMode::ClearMode();
}

void ioTrainingMode::LoadModeResource( ioINILoader &rkLoader )
{
	g_ModeUIController.SetKOMsgWnd( g_GUIMgr.AddWnd( "XML/kochatmsgwnd.xml") );
	g_GUIMgr.AddWnd( "XML/ChatModeWnd.xml" );
	g_GUIMgr.AddWnd( "XML/SkillListWnd.xml" );
	g_GUIMgr.AddWnd( "XML/AccessorySkillListWnd.xml" );
	g_GUIMgr.AddWnd( "XML/plazamonstergaugewnd.xml");
	g_GUIMgr.AddWnd( "XML/AniTextWnd.xml");
	g_ModeUIController.SetCenterKillInfoWnd( NULL ); 

	MiniSoldierSelectWnd *pMiniSoldierWnd = dynamic_cast<MiniSoldierSelectWnd*>( g_GUIMgr.AddWnd( "XML/MiniSoldierSelectWnd.xml" ) );
	if(pMiniSoldierWnd)
		pMiniSoldierWnd->SetPlayStage( m_pCreator );

	g_GUIMgr.AddWnd( "XML/excavation_wnd.xml" );
	g_GUIMgr.AddWnd( "XML/excavation_result_wnd.xml" );
	g_GUIMgr.AddWnd( "XML/excavation_shovel_wnd.xml" );
	g_GUIMgr.AddWnd( "XML/excavation_levelup_wnd.xml" );
	
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

	if( g_App.GetConnectedMoveMode() == ioApplication::CMM_LOBBY )
	{
		g_GUIMgr.AddWnd( "XML/itemcustomselectwnd.xml" );
		g_GUIMgr.AddWnd( "XML/itemcustommakewnd.xml" );
		g_GUIMgr.AddWnd( "XML/itemcustommakedownloadwnd.xml" );
		g_GUIMgr.AddWnd( "XML/itemcustommakeuploadwnd.xml" );
		g_GUIMgr.AddWnd( "XML/itemcustommakeconfirmwnd.xml" );
		g_GUIMgr.AddWnd( "XML/itemcustomdetailviewwnd.xml" );
		g_GUIMgr.AddWnd( "XML/itemcustomchangeconfirmwnd.xml" );
		g_GUIMgr.AddWnd( "XML/itemcustommakecompletewnd.xml" );
		g_GUIMgr.AddWnd( "XML/itemcustomdeletewnd.xml" );

		g_GUIMgr.AddWnd( "XML/itemcustomtrainselectwnd.xml" );
		g_GUIMgr.AddWnd( "XML/itemcustomtrainmakewnd.xml" );
		g_GUIMgr.AddWnd( "XML/itemcustomwantetcitemwnd.xml" );
		g_GUIMgr.AddWnd( "XML/itemcustomselecttooltip.xml" );

		g_GUIMgr.AddWnd( "XML/costumecustomchangeconfirmwnd.xml" );
		g_GUIMgr.AddWnd( "XML/costumecustomdeletewnd.xml" );
		g_GUIMgr.AddWnd( "XML/costumecustomdetailviewwnd.xml" );
		g_GUIMgr.AddWnd( "XML/costumecustommakecompletewnd.xml" );
		g_GUIMgr.AddWnd( "XML/costumecustommakeconfirmwnd.xml" );
		g_GUIMgr.AddWnd( "XML/costumecustommakewnd.xml" );
		g_GUIMgr.AddWnd( "XML/costumecustomselecttooltip.xml" );
		g_GUIMgr.AddWnd( "XML/costumecustomselectwnd.xml" );
	}

	ioPlayMode::LoadModeResource( rkLoader );
}

void ioTrainingMode::LoadMap()
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

void ioTrainingMode::LoadModeDeco( ioINILoader &rkLoader )
{
	ClearRunningManDeco();

	char szBuf[MAX_PATH] = "";
	rkLoader.SetTitle_e( "runningman_info" );
	m_bNameTitleUse = rkLoader.LoadBool_e( "title_use", false );

	rkLoader.LoadString_e( "title_name_left", "", szBuf, MAX_PATH );
	m_pRunningManTitleLeft	= g_UIImageSetMgr.CreateImageByFullName( szBuf, true );

	rkLoader.LoadString_e( "title_name_center", "", szBuf, MAX_PATH );
	m_pRunningManTitleCenter= g_UIImageSetMgr.CreateImageByFullName( szBuf, true );

	rkLoader.LoadString_e( "title_name_right", "", szBuf, MAX_PATH );
	m_pRunningManTitleRight	= g_UIImageSetMgr.CreateImageByFullName( szBuf, true );
	
	int iMaxRunningMan = rkLoader.LoadInt_e( "max_runningman", 0 );
	for(int i = 0;i < iMaxRunningMan;i++)
	{
		char szTitle[MAX_PATH] = "";
		sprintf_e( szTitle, "runningman%d", i + 1 );
		rkLoader.SetTitle( szTitle );

		RunningManDeco kDeco;		
		kDeco.m_iManHair = rkLoader.LoadInt_e( "hair_m", 0 );
		kDeco.m_iManHairColor = rkLoader.LoadInt_e( "hair_color_m", 0 );
		kDeco.m_iManFace = rkLoader.LoadInt_e( "face_m", 0 );
		kDeco.m_iManSkinColor = rkLoader.LoadInt_e( "skin_color_m", 0 );
		kDeco.m_iManUnderwearBlue = rkLoader.LoadInt_e( "underwear_blue_m", 0 );
		kDeco.m_iManUnderwearRed  = rkLoader.LoadInt_e( "underwear_red_m", 0 );
		rkLoader.LoadString_e( "skeleton_m", "", szBuf, MAX_PATH );
		kDeco.m_ManSkeletonName = szBuf;

		kDeco.m_iWomanHair = rkLoader.LoadInt_e( "hair_w", 0 );
		kDeco.m_iWomanHairColor = rkLoader.LoadInt_e( "hair_color_w", 0 );
		kDeco.m_iWomanFace = rkLoader.LoadInt_e( "face_w", 0 );
		kDeco.m_iWomanSkinColor = rkLoader.LoadInt_e( "skin_color_w", 0 );
		kDeco.m_iWomanUnderwearBlue = rkLoader.LoadInt_e( "underwear_blue_w", 0 );
		kDeco.m_iWomanUnderwearRed  = rkLoader.LoadInt_e( "underwear_red_w", 0 );
		rkLoader.LoadString_e( "skeleton_w", "", szBuf, MAX_PATH );
		kDeco.m_WomanSkeletonName = szBuf;

		m_RunningManDecoList.insert( RunningManDecoMap::value_type( i + 1, kDeco ) );
	}
	
}

void ioTrainingMode::InitMode( int iMapIndex )
{
	m_iModeMapIndex = iMapIndex;
	
	// Init GUI
	g_GUIMgr.AddWnd( "XML/TutorialWnd.xml" );

	g_GUIMgr.HideWnd( PLAZA_WND );

	//Party	
	g_GUIMgr.AddWnd( "XML/partycreatewnd.xml" );

	g_GUIMgr.AddWnd( "XML/plazaroominfownd.xml" );
	g_GUIMgr.AddWnd( "XML/plazapasswordwnd.xml" );
	PlazaMainWnd *pPlazaMainWnd = dynamic_cast<PlazaMainWnd*>( g_GUIMgr.AddWnd( "XML/plazamainwnd.xml" ) );
	if( pPlazaMainWnd )
		pPlazaMainWnd->SetPlayStage( m_pCreator );

	g_GUIMgr.AddWnd( "XML/plazainvitedwnd.xml" );
	g_GUIMgr.AddWnd( "XML/plazacreatewnd.xml" );
	g_GUIMgr.AddWnd( "XML/plazaownerwnd.xml" );
	g_GUIMgr.AddWnd( "XML/plazainvitelistwnd.xml" );
	g_GUIMgr.AddWnd( "XML/fishingautosellquestion.xml" );
	g_GUIMgr.AddWnd( "XML/fishingdropzoneinducewnd.xml" );
	g_GUIMgr.AddWnd( "XML/runningmannamewnd.xml" );
	g_GUIMgr.AddWnd( "XML/roulette/roulettewnd.xml" );
	g_GUIMgr.AddWnd( "XML/roulette/roulettehelpwnd.xml" );

	LoadMap();
	LoadSymbols();
	LoadPushStructs();
	LoadMachineStruct();
	LoadFuntionalObjects();
	LoadCharacters();
	LoadDisableSkillList();
	LoadStartBuffList();

	if( g_RouletteMgr.IsRouletteEvent() )
	{
		g_RouletteMgr.LoadRouletteMapInfo( GetModeMapINIFileName( GetModeType(), GetModeSubNum(), GetModeMapIndex() ) );
		g_RouletteMgr.CheckRouletteStruct();
		if( !g_GUIMgr.IsShow( ROULETTE_WND ) )
			g_GUIMgr.ShowWnd( ROULETTE_WND );
	}

	ioINILoader kLoader( GetModeINIFileName( m_ModeType ) );

	kLoader.SetTitle_e( "common_info" );
	LoadRevivalTime( kLoader );
	LoadModeResource( kLoader );
	LoadModeDeco( kLoader );
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

	m_iRoomNum = -1;

	if( g_App.GetRoomNum() != -1 )
	{
		m_iRoomNum = g_App.GetRoomNum();
		g_App.SetRoomNum( -1 );
	}

	if( g_BattleRoomMgr.IsBattleRoom() )
	{
		g_GUIMgr.ShowWnd( PARTY_SHAM_BATTLE_WND );
	}

	kLoader.SetFileName_e( "config/sp2_sound.ini" );
	kLoader.SetTitle_e( "sound_file_name" );

	kLoader.LoadString_e( "symbol_damaged", "", szBuf, MAX_PATH );
	m_SymbolDamaged = szBuf;

	kLoader.LoadString_e( "symbol_occupy", "", szBuf, MAX_PATH );
	m_SymbolOccupy = szBuf;

	kLoader.LoadString_e( "symbol_destroy", "", szBuf, MAX_PATH );
	m_SymbolDestroy = szBuf;	


	int iSymbolCnt = m_vSymbolList.size();
	for( int i=0 ; i<iSymbolCnt ; i++ )
	{
		m_vSymbolList[i]->RestoreOrgTeam();
	}

	for( int i=0 ; i<iSymbolCnt ; i++ )
	{
		ioSymbolStruct *pSymbol = m_vSymbolList[i];
		pSymbol->SetRevivalHP( 0.0f, true );
		pSymbol->Revival();

		if( m_iCurRoundCount == 1 )
		{
			pSymbol->SetRevivalCount( 0 );
		}
	}

	g_GUIMgr.CheckShopSound( true );

	g_MyInfo.InitPraise();

	g_ExcavationMgr.SetPlayStage( m_pCreator );

	ClearGrowthLevel();

	// LoadMap() 이후 사용해야함
	SetMapAreaWeapon();

	SetMapDummy();

	PlazaNPCGaugeWnd *pPlazaNPCGaugeWnd = dynamic_cast<PlazaNPCGaugeWnd *>( g_GUIMgr.FindWnd( PLAZA_NPC_GAUGE_WND ) );
	if(pPlazaNPCGaugeWnd)
		pPlazaNPCGaugeWnd->InitAniState();	
}

void ioTrainingMode::PlayExtendSound()
{
	if( m_pCreator&& m_pCreator->GetPlazaModeType() == PT_GUILD )
		g_ExSoundMgr.PlaySound( ExSound::EST_GUILD_HQ_ENTER );
}

void ioTrainingMode::ReStartMode()
{
	LOG.PrintTimeAndLog( 0, "ioTrainingMode::ReStartMode Is Call???" );
}

void ioTrainingMode::ProcessMode()
{
	ProcessSymbolZoneCollision();
	ProcessSymbolAlphaRate();

	ErrorReport::SetPosition( 1010, 1 );
	ProcessSelectCharacter();
	ErrorReport::SetPosition( 1010, 2 );
	ProcessGUI();
	ErrorReport::SetPosition( 1010, 3 );

	switch( m_ModeState )
	{
	case MS_READY:
		ErrorReport::SetPosition( 1010, 4 );
		ProcessReadyState();
		ErrorReport::SetPosition( 1010, 5 );
		CheckModeMapSound();
		break;
	case MS_PLAY:
		ProcessMapAreaWeapon();
		ErrorReport::SetPosition( 1010, 6 );
		ProcessPlayState();
		ErrorReport::SetPosition( 1010, 7 );
		CheckModeMapSound();
		ErrorReport::SetPosition( 1010, 8 );
		g_EventMgr.Process();
		ProcessNpcErase();
		break;
	}

	g_RouletteMgr.ProcessRoulette();
}

void ioTrainingMode::ProcessReadyState()
{
	if( P2PNetwork::IsNetworkPlaying() )	return;
	if( m_dwReadyStartTime + 4000 > FRAMEGETTIME() )
		return;

	SetModeState( MS_PLAY );
}

void ioTrainingMode::ProcessPlayState()
{
	UpdateSoldierRevivalPenalty();
	UpdateTimeGrowthTime();

	m_pCreator->NotifySpringDampUseSet( true );

	ProcessRevival();
	if( !P2PNetwork::IsNetworkPlaying() )
	{
		CheckRoundEnd();
	}

	g_ExcavationMgr.ProcessExcavtion();
}

void ioTrainingMode::ProcessResultState()
{
}

void ioTrainingMode::ProcessGUI()
{
	UpdateMiniMapSpotPoint();
	UpdateRoundStateWnd();
	UpdateTimeScoreWnd();
	UpdateEndScoreWndTime();
	UpdateSoldierWndTime();
	UpdatePlazaUserInfo();

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

void ioTrainingMode::ProcessNpcErase()
{
	int iSize = m_NpcEraseList.size();
	for(int i = 0;i < iSize;i++)
	{
		NpcEraseData &rkNpcData = m_NpcEraseList[i];

		// NPC 죽은 후 35초뒤에 삭제.. 한번에 삭제하면 렉이 있을 수 있으니 프레임마다 한개씩 삭제
		if( FRAMEGETTIME() - rkNpcData.m_dwEraseTimer > 10000 )         
		{
			m_pCreator->RemoveUser( rkNpcData.m_szEraseNpc, true );
			RemoveEraseNpc( rkNpcData.m_szEraseNpc );
			return;
		}
	}
}

void ioTrainingMode::ProcessRevival()
{
	DWORD dwCurTime = FRAMEGETTIME();
	
	int iCharCnt = m_vCharRecordList.size();
	for( int i=0 ; i<iCharCnt ; i++ )
	{
		TrainingRecord &rkRecord = m_vCharRecordList[i];
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


void ioTrainingMode::UpdateMiniMapSpotPoint()
{
	MiniMap *pMiniMap = dynamic_cast<MiniMap*>(g_GUIMgr.FindWnd( MINIMAP_WND ));
	if( !pMiniMap )	return;

	ioPlayMode::UpdateMiniMapSpotPoint();

	pMiniMap->ClearPointList();

	D3DXVECTOR3 vPos;
	DWORD dwGapTime = 0;
	ioBaseChar *pOwner = GetOwnerChar();

	if( !pOwner ) return;

	// Symbol
	SymbolList::iterator iSymbol;
	for( iSymbol=m_vSymbolList.begin() ; iSymbol!=m_vSymbolList.end() ; ++iSymbol )
	{
		ioSymbolStruct *pSymbol = *iSymbol;

		if( !pSymbol || !pSymbol->GetSymbolActivity() ) continue;

		dwGapTime = pSymbol->GetEventGapTime();
		vPos = pSymbol->GetSpotPosition( pMiniMap->GetSpotPositionRate() );

		bool bOwnerTeam = false;
		if( pOwner->GetTeam() == pSymbol->GetTeam() )
			bOwnerTeam = true;

		TeamType eSymbolTeam = pSymbol->GetTeam();

		switch( eSymbolTeam )
		{
		case TEAM_NONE:
		case TEAM_RED:
		case TEAM_BLUE:
			pMiniMap->AddPoint( vPos.x, vPos.z, MiniMap::SPOT_SYMBOL, pSymbol->GetTeam(), dwGapTime );
			break;
		default:
			if( bOwnerTeam )
				pMiniMap->AddPoint( vPos.x, vPos.z, MiniMap::SPOT_SYMBOL, TEAM_BLUE, dwGapTime );
			else
				pMiniMap->AddPoint( vPos.x, vPos.z, MiniMap::SPOT_SYMBOL, TEAM_RED, dwGapTime );
			break;
		}
	}

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
			if( ToNpcChar(pChar) )
			{
				pMiniMap->AddPoint( vPos.x, vPos.z, MiniMap::SPOT_NEXUS, eTeam, dwGapTime );
			}
			else
			{
				if( pChar->IsDropZoneDownState() )
					pMiniMap->AddPoint( vPos.x, vPos.z, MiniMap::SPOT_DROP_USER, eTeam, 0 );
				else
					pMiniMap->AddPoint( vPos.x, vPos.z, MiniMap::SPOT_USER, eTeam, dwGapTime );
			}
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

	// Crown
	ioFieldItem *pCrown = m_pCreator->GetCrownFieldItem( ioItem::MCT_HIDDEN_KING );
	if( pCrown )
	{
		dwGapTime = pCrown->GetEventGapTime();
		vPos = pCrown->GetWorldPosition();
		pMiniMap->AddPoint( vPos.x, vPos.z, MiniMap::SPOT_CROWN, TEAM_NONE, dwGapTime );
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

		ioRoulette *pRoulette = ToRoulette( *iter_b );
		if( pRoulette && g_RouletteMgr.IsRouletteEvent() )
		{
			if( pRoulette->GetRouletteType() == ioRouletteManager::RT_STAND )
			{
				vPos = pRoulette->GetWorldPosition();
				pMiniMap->AddPoint( vPos.x, vPos.z, MiniMap::SPOT_ROULETTE, TEAM_NONE, 0 );
			}
		}
	}
	//
}

void ioTrainingMode::UpdateRoundStateWnd()
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
			pRoundStateWnd->SetPlazaScore();
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

void ioTrainingMode::UpdatePlazaUserInfo()
{
	PlazaMainWnd *pPlazaMain = dynamic_cast<PlazaMainWnd*>(g_GUIMgr.FindWnd( PLAZA_MAIN_WND ));
	if( pPlazaMain && pPlazaMain->IsUserUpdate() )
	{
		pPlazaMain->InitUserInfo();
		for( int i = 0 ; i < GetRecordCharCnt(); i++ )
		{
			RoundInfo kInfo;
			if( GetRoundInfo( i, kInfo ) )
				pPlazaMain->SetUserInfo( kInfo );
		}
		pPlazaMain->UserInfoUpdate();
	}
}

void ioTrainingMode::UpdateDropDieRecord( ioBaseChar *pDieChar, ioBaseChar *pKillChar, ioBaseChar *pBestAttacker )
{
	ioBaseChar *pOwner = GetOwnerChar();
	if( !pOwner ) return;

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
					g_ModeUIController.SetKOMsg( TEAM_NONE, "", 
												 "", 0, eDieTeam, szDieName );
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

			if( pDieRecord->pChar->GetTeam() == pOwner->GetTeam() )
				eDieTeam = TEAM_BLUE;
			else
				eDieTeam = TEAM_RED;

			pDieRecord->AddDeath( GetModeType(), GetDeathPoint( pDieRecord->pChar->GetTeam() ) );

			g_ModeUIController.SetKOMsg( TEAM_NONE, "",
				                         "", 0, eDieTeam, pDieRecord->pChar->GetCharName() );

			pDieRecord->pChar->SetDeathEmoticon();
		}
	}

	// Best Attacker 처리 안함
}

void ioTrainingMode::UpdateKillDeathRecord( ioPlayEntity *pDieEntity, ioPlayEntity *pKiller, ioPlayEntity *pBestAttacker )
{
	ioBaseChar *pOwner = GetOwnerChar();
	if( !pOwner ) return;

	ioBaseChar *pKillerChar = ToBaseChar( pKiller );
	ioBaseChar *pDieChar = ToBaseChar( pDieEntity );
	if( pKillerChar && pDieChar )
	{
		if( ToNpcChar(pDieChar) )
		{
			OnChangeTeam(false);
			AddEraseNpc(ToNpcChar(pDieChar)->GetCharName());
		}

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

			ioHashString szAttName = "";
			ioHashString szDieName = "";
			if( ToNpcChar(pAttRecord->pChar) )
				szAttName = ToNpcChar(pAttRecord->pChar)->GetViewName();
			else
				szAttName = pAttRecord->pChar->GetCharName();

			if( ToNpcChar(pDieRecord->pChar) )
				szDieName = ToNpcChar(pDieRecord->pChar)->GetViewName();
			else
				szDieName = pDieRecord->pChar->GetCharName();

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
					g_ModeUIController.SetKOMsg( TEAM_NONE,	"",
						                         "", 0, eDieTeam, pDieRecord->pChar->GetCharName() );
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

				if( pDieRecord->pChar->GetTeam() == pOwner->GetTeam() )
					eDieTeam = TEAM_BLUE;
				else
					eDieTeam = TEAM_RED;

				pDieRecord->AddDeath( GetModeType(), GetDeathPoint( pDieRecord->pChar->GetTeam() ) );

				g_ModeUIController.SetKOMsg( TEAM_NONE, "",
					                         "", 0, eDieTeam, pDieRecord->pChar->GetCharName() );

				pDieRecord->pChar->SetDeathEmoticon();
			}
		}
	}

	// Best Attacker 처리 안함
}

void ioTrainingMode::UpdateTimeScoreWnd()
{
	PlazaNPCGaugeWnd *pPlazaNPCGaugeWnd = dynamic_cast< PlazaNPCGaugeWnd * >( g_GUIMgr.FindWnd( PLAZA_NPC_GAUGE_WND ) );
	if( !pPlazaNPCGaugeWnd ) return;

	// 보스 게이지
	if( pPlazaNPCGaugeWnd->IsShow() )
	{
		TrainingRecord *pRecord = FindRecordCharName( pPlazaNPCGaugeWnd->GetMonsterName() );

		if( pRecord && ToNpcChar( pRecord->pChar ) )
		{	
			if( pRecord->pChar->GetState() == CS_DIE )
			{
				g_SoundMgr.StopStream();

				if( !m_ModeMapSound.IsEmpty() )
				{
					char szName[MAX_PATH];
					wsprintf( szName, "Resource/wave/%s", m_ModeMapSound.c_str() );
					g_SoundMgr.OpenOggStream( szName );
					g_SoundMgr.PlayStream( true );
				}

				pPlazaNPCGaugeWnd->HideWnd();

				AniTextWnd *pAniTextWnd = dynamic_cast<AniTextWnd *>( g_GUIMgr.FindWnd(ANI_TEXT_WND) );

				if(pAniTextWnd)
				{
					pAniTextWnd->ShowWnd();
					pAniTextWnd->StartAniText(AniTextWnd::DESTROY_SUBWND);
				}
			}
			else
			{
				StatusValue &rkStatus = pRecord->pChar->GetHP();
				pPlazaNPCGaugeWnd->UpdateBossMonster( rkStatus.m_fCurValue, rkStatus.m_fMaxValue );
			}			
		}
	}
}

void ioTrainingMode::RenderPreGUI()
{
	if( m_ModeState == MS_PLAY )
	{
		RenderPushEnableCursor();
	}
}

void ioTrainingMode::RenderAfterGUI()
{
}

bool ioTrainingMode::IsBugBearEmerge()
{
	int iCharCnt = m_vCharRecordList.size();
	for(int i = 0;i < iCharCnt;i++)
	{
		TrainingRecord *pRecord = &m_vCharRecordList[i];
		if( !pRecord )	continue;
		if( !pRecord->pChar ) continue;

		ioNpcChar *pNpcChar = ToNpcChar( pRecord->pChar );
		if( pNpcChar )
		{
			if( pNpcChar->GetState() != CS_DIE )
			{
				m_dwBugBearDieTimer = FRAMEGETTIME();
				return true;
			}
		}
	}	

	if( m_dwBugBearDieTimer == 0 )
		return false;
	else if( FRAMEGETTIME() - m_dwBugBearDieTimer < 60000 )        // 버그 베어가 죽고 1분간은 강퇴가 되지 않음.
		return true;
	return false;
}

D3DXVECTOR3 ioTrainingMode::GetStartPos( TeamType eTeamType, bool bRandom )
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
	{
		return m_vMapCenter;
	}

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

D3DXVECTOR3 ioTrainingMode::GetRevivalPos( ioBaseChar *pChar )
{
	if(!pChar)
	{
		return D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	}

	int iPosArray = 0;
	PositionDistanceSqList kPosDistance;
	TeamType eCharTeam = pChar->GetTeam();

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

void ioTrainingMode::GetMostNearPosList( PositionDistanceSqList &rkResultList, const D3DXVECTOR3 &vTargPos )
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

void ioTrainingMode::NotifyEntityDie( ioPlayEntity *pEntity, ioPlayEntity *pKiller, ioBaseChar *pBestAttacker,
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
	case ioPlayEntity::PST_SYMBOL:
		OnNotifySymbolDie( pEntity, pKiller );
		break;
	}
}

void ioTrainingMode::NotifyCharDropDie( ioBaseChar *pDieChar, ioBaseChar *pKillChar, ioBaseChar *pBestAttacker,
									   float fLastRate, float fBestRate )
{
	OnNotifyCharDie( pDieChar, pKillChar, pBestAttacker, fLastRate, fBestRate );
	UpdateDropDieRecord( pDieChar, pKillChar, pBestAttacker );
}

void ioTrainingMode::OnNotifyCharDie( ioBaseChar *pDieChar, ioBaseChar *pKillChar, ioBaseChar *pBestAttacker,
									 float fLastRate, float fBestRate )
{
	if( !pDieChar )
	{
		LOG.PrintTimeAndLog( 0, "ioTrainingMode::OnNotifyCharDie() - DieChar is NULL" );
		return;
	}

	TrainingRecord *pDieRecord = FindRecordCharName( pDieChar->GetCharName() );
	if( !pDieRecord )
	{
		LOG.PrintTimeAndLog( 0, "ioTrainingMode::OnNotifyCharDie - %s Not Exit Char",
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

void ioTrainingMode::OnNotifySymbolDie( ioPlayEntity *pDieEntity, ioPlayEntity *pKiller )
{
	ioSymbolStruct *pSymbol = ToSymbolStruct( pDieEntity );
	if( !pSymbol )	return;

	ioBaseChar *pKillChar = ToBaseChar( pKiller );
	if( !pKillChar )	return;

	float fCurHP = fabs( pSymbol->GetHP().m_fCurValue );

	if( pKillChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CTPK_SYMBOL_DIE );
		kPacket << pSymbol->GetSymbolArrayIdx();
		kPacket << pSymbol->GetRevivalCount();
		kPacket << fCurHP;
		TCPNetwork::SendToServer( kPacket );
	}
	else
	{
		pSymbol->SetRevivalHP( fCurHP );

		ProcessSymbolKill( pSymbol, pKillChar );
		SymbolDestroyAnnounce( pKillChar->GetTeam() );
	}

	if( pKillChar->GetTeam() == GetOwnerTeam() )
	{
		ioGameEntity *pTarget = GetGuideTarget( pSymbol->GetWorldPosition() );
		ioSymbolStruct *pNext = ToSymbolStruct( pTarget );
		if( pNext )
		{
			MiniMap *pMiniMap = dynamic_cast<MiniMap*>( g_GUIMgr.FindWnd(MINIMAP_WND) );
			if( pMiniMap )
			{
				D3DXVECTOR3 vPos = ( pSymbol->GetWorldPosition() + pNext->GetWorldPosition() ) * FLOAT05;
				D3DXVECTOR3 vDir = pNext->GetWorldPosition() - pSymbol->GetWorldPosition();
				D3DXVec3Normalize( &vDir, &vDir );

				pMiniMap->AddNextSymbolEvent( vPos, vDir, pKillChar->GetTeam() );
			}
		}
	}
}


void ioTrainingMode::AddNewRecord( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	TrainingRecord *pTrainingRecord = FindRecordCharName( pChar->GetCharName() );
	if( !pTrainingRecord )
	{
		TrainingRecord kRecord;
		kRecord.pChar = pChar;
		kRecord.szName= pChar->GetCharName();

		m_vCharRecordList.push_back( kRecord );
		UpdateUserRank();

		if( pChar->IsOwnerChar() )
		{
			// 입장하면 상징물 팀을 다시 세팅한다. 이유는 상징물보다 유저가 늦게 세팅되므로 . . . .
			int iSymbolCnt = m_vSymbolList.size();
			for( int i=0 ; i<iSymbolCnt ; i++ )
				m_vSymbolList[i]->ResetTeam();
		}
	}
	else
	{
		pTrainingRecord->pChar = pChar;
	}

	if( !ToNpcChar(pChar) )
	{
		PlazaNPCGaugeWnd *pPlazaNPCGaugeWnd = dynamic_cast<PlazaNPCGaugeWnd *>( g_GUIMgr.FindWnd( PLAZA_NPC_GAUGE_WND ) );

		if( pPlazaNPCGaugeWnd && pPlazaNPCGaugeWnd->IsShow())
			OnChangeTeam(true);
	}

	if( ToNpcChar( pChar ) )
		m_dwBugBearDieTimer = FRAMEGETTIME();
}

void ioTrainingMode::AddEraseNpc( const ioHashString &rkNpcName )
{
	NpcEraseData kEraseData;
	kEraseData.m_szEraseNpc = rkNpcName;
	kEraseData.m_dwEraseTimer = FRAMEGETTIME();
	m_NpcEraseList.push_back( kEraseData );
}

void ioTrainingMode::RemoveEraseNpc( const ioHashString &rkNpcName )
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

void ioTrainingMode::RemoveRecord( const ioHashString &rkName )
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

	g_RouletteMgr.RemoveJoinPlayer( rkName );
}


void ioTrainingMode::RemoveRecord( int nIndex )
{
	int iCharCnt = m_vCharRecordList.size();
	if( iCharCnt > nIndex )
	{
		m_vCharRecordList.erase( m_vCharRecordList.begin() + nIndex );
		UpdateUserRank();
	}
}


ioBaseChar* ioTrainingMode::GetRecordChar( int i )
{
	if( COMPARE( i, 0, GetRecordCharCnt() ) )
		return m_vCharRecordList[i].pChar;

	return NULL;
}

ioBaseChar* ioTrainingMode::GetRecordChar( const ioHashString &rkName )
{
	TrainingRecord *pRecord = FindRecordCharName( rkName );
	if( pRecord )
		return pRecord->pChar;

	return NULL;
}

int ioTrainingMode::GetRecordCharCnt() const
{
	return m_vCharRecordList.size();
}

ModeRecord* ioTrainingMode::FindModeRecord( int iIdx )
{
	if( COMPARE( iIdx, 0, GetRecordCharCnt() ) )
		return &m_vCharRecordList[iIdx];

	return NULL;
}

ModeRecord* ioTrainingMode::FindModeRecordCharName( const ioHashString &rkName )
{
	return (ModeRecord*)FindRecordCharName( rkName );
}

TrainingRecord* ioTrainingMode::FindRecordCharName( const ioHashString &rkName )
{
	int iCharCnt = m_vCharRecordList.size();
	for( int i=0 ; i<iCharCnt ; i++ )
	{
		if( m_vCharRecordList[i].szName == rkName )
			return &m_vCharRecordList[i];
	}

	return NULL;
}

bool ioTrainingMode::GetRoundInfo( int iIndex, RoundInfo &rkInfo )
{
	if( ioPlayMode::GetRoundInfo( iIndex, rkInfo ) )
	{
		TrainingRecord *pRecord = (TrainingRecord*)FindModeRecord( iIndex );
		if( !pRecord )	return false;
		if( !pRecord->pChar ) return false;

		rkInfo.m_iContribute = pRecord->Contribute.m_iContribute;
		rkInfo.m_iContributePer = pRecord->Contribute.m_iPrevContributePer;
		return true;
	}
	return false;
}

void ioTrainingMode::SkeletonEtcItemSync( const ioHashString &rkName, int iEtcItem )
{
	TrainingRecord *pRecord = dynamic_cast<TrainingRecord*>(FindModeRecordCharName( rkName ));
	if( pRecord )
	{
		pRecord->iSkeletonEtcItem = iEtcItem;
		if( pRecord->pChar )
			pRecord->pChar->ChangeEtcItemSkeleton( pRecord->iSkeletonEtcItem );
	}
	else
	{
		LOG.PrintTimeAndLog( 0, "%s Error %s : %d", __FUNCTION__, rkName.c_str(), iEtcItem );
	}
}

void ioTrainingMode::ChangeSkeletonEtcItem( const ioHashString &rkName )
{
	TrainingRecord *pRecord = dynamic_cast<TrainingRecord*>(FindModeRecordCharName( rkName ));
	if( pRecord )
	{
		if( pRecord->pChar && pRecord->iSkeletonEtcItem )
			pRecord->pChar->ChangeEtcItemSkeleton( pRecord->iSkeletonEtcItem );
	}
	else
	{
		LOG.PrintTimeAndLog( 0, "%s Error %s", __FUNCTION__, rkName.c_str() );
	}
}

bool ioTrainingMode::ProcessTCPPacket( SP2Packet &rkPacket )
{
	ErrorReport::SetPosition( 1010, 8 );

	if( ioPlayMode::ProcessTCPPacket( rkPacket ) )
		return true;

	ErrorReport::SetPosition( 1010, 9 );

	switch( rkPacket.GetPacketID() )
	{
	case STPK_SPAWN_MONSTER:
		ErrorReport::SetPosition( 1010, 20 );
		OnSpawnMonster(rkPacket);
		ErrorReport::SetPosition( 1010, 21 );
		return true;
	case STPK_SYMBOL_DIE:
		ErrorReport::SetPosition( 1010, 10 );
		OnSymbolDie( rkPacket );
		ErrorReport::SetPosition( 1010, 11 );
		return true;
	case STPK_SYMBOL_DIE_LIST:
		ErrorReport::SetPosition( 1010, 12 );
		OnSymbolDieList( rkPacket );
		ErrorReport::SetPosition( 1010, 13 );
		return true;
	case STPK_SYMBOL_ACTIVITY:
		ErrorReport::SetPosition( 1010, 14 );
		OnSymbolActivity( rkPacket );
		ErrorReport::SetPosition( 1010, 15 );
		return true;
	case STPK_RUNNINGMAN_DECO_SYNC:
		ErrorReport::SetPosition( 1010, 16 );
		OnRunningManDecoSync( rkPacket );
		ErrorReport::SetPosition( 1010, 17 );
		return true;
	case STPK_RUNNINGMAN_NAME_SYNC:
		ErrorReport::SetPosition( 1010, 18 );
		OnRunningManNameSync( rkPacket );
		ErrorReport::SetPosition( 1010, 19 );
		return true;
	case STPK_MONSTER_DIE_REWARD:
		ErrorReport::SetPosition( 1031, 19 );
		OnMonsterDieReward( rkPacket );
		ErrorReport::SetPosition( 1031, 20 );
		return true;
	case STPK_MONSTER_DICE_RESULT:
		ErrorReport::SetPosition( 1031, 21 );
		OnMonsterDiceResult( rkPacket );
		ErrorReport::SetPosition( 1031, 22 );
		return true;
	case STPK_MONSTER_FORCE_DIE:
		ErrorReport::SetPosition( 1031, 15 );
		OnMonsterForceDie( rkPacket );
		ErrorReport::SetPosition( 1031, 16 );
		return true;
	}

	return false;
}

void ioTrainingMode::OnMonsterForceDie( SP2Packet &rkPacket )
{
	int iMaxNPC;
	rkPacket >> iMaxNPC;
	for(int i = 0;i < iMaxNPC;i++)
	{
		ioHashString kNpcName;
		rkPacket >> kNpcName;
		// 소환물 삭제
		m_pCreator->ClearUserCreateEntities( kNpcName );

		TrainingRecord *pRecord = FindRecordCharName( kNpcName );
		if( pRecord && pRecord->pChar )
		{
			if(!m_RedSymbolChangeEffect.IsEmpty() )
				m_pCreator->CreateMapEffect( m_RedSymbolChangeEffect, pRecord->pChar->GetWorldPosition(), ioMath::UNIT_ALL );

			pRecord->pChar->SetDieState();
			m_pCreator->RemoveUser( kNpcName, true );

			OnChangeTeam(false);

			PlazaNPCGaugeWnd *pPlazaNPCGaugeWnd = dynamic_cast<PlazaNPCGaugeWnd *>( g_GUIMgr.FindWnd( PLAZA_NPC_GAUGE_WND ) );
			if( pPlazaNPCGaugeWnd && pPlazaNPCGaugeWnd->IsShow())
			{
				pPlazaNPCGaugeWnd->InitAniState();
				pPlazaNPCGaugeWnd->HideWnd();

				g_SoundMgr.StopStream();

				if( !m_ModeMapSound.IsEmpty() )
				{
					char szName[MAX_PATH];
					wsprintf( szName, "Resource/wave/%s", m_ModeMapSound.c_str() );
					g_SoundMgr.OpenOggStream( szName );
					g_SoundMgr.PlayStream( true );
				}
			}

			AniTextWnd *pAniTextWnd = dynamic_cast<AniTextWnd *>( g_GUIMgr.FindWnd(ANI_TEXT_WND) );

			if(pAniTextWnd)
			{
				pAniTextWnd->ShowWnd();
				pAniTextWnd->StartAniText(AniTextWnd::RUNAWAY_SUBWND);
			}
		}
	}
}

void ioTrainingMode::OnMonsterDiceResult( SP2Packet &rkPacket )
{
	// 주사위 결과
	int			nSize;
	ioHashString kPrizeWinner;

	rkPacket >> nSize;

	for( int i = 0; i < nSize; i++ )
	{
		rkPacket >> kPrizeWinner;

		short iPresentType;
		int iPresentValue1, iPresentValue2;
		rkPacket >> iPresentType >> iPresentValue1 >> iPresentValue2;

		ioBaseChar *pChar = m_pCreator->GetBaseChar( kPrizeWinner );

		if( pChar )
			pChar->SetNamedEmoticon(g_PresentMgr.GetPresentIconText( iPresentType, iPresentValue1, iPresentValue2 ), "", 0xffffffff );
	}
}

void ioTrainingMode::OnMonsterDieReward( SP2Packet &rkPacket )
{
	int i = 0;
	// 주사위 선물 보상
	bool bDiceStart;
	rkPacket >> bDiceStart;

	if( bDiceStart )
	{
		ioHashString kMonsterName;
		rkPacket >> kMonsterName;

		short iPresentType;
		int iPresentValue1, iPresentValue2;
		rkPacket >> iPresentType >> iPresentValue1 >> iPresentValue2;

		ioHashStringVec kRankUserList;
		int iDamageRankSize;
		rkPacket >> iDamageRankSize;
		for(i = 0;i < iDamageRankSize;i++)
		{
			ioHashString kRankUser;
			rkPacket >> kRankUser;
			kRankUserList.push_back( kRankUser );
		}		
	}

	// 페소 & 경험치 보상
	int iRewardSize;
	rkPacket >> iRewardSize;

	for(i = 0;i < iRewardSize;i++)
	{
		ioHashString szRewardUser;
		rkPacket >> szRewardUser;

		int iRewardExp, iRewardPeso;
		rkPacket >> iRewardExp >> iRewardPeso;		

		ioBaseChar *pChar = m_pCreator->GetBaseChar( szRewardUser );
		if( szRewardUser == g_MyInfo.GetPublicID() )
		{
			__int64 iMoney;
			int iGradeLevel, iGradeExpert;
			rkPacket >> iMoney >> iGradeLevel >> iGradeExpert;

			int iPrevGradeLevel = g_MyInfo.GetGradeLevel();
			g_MyInfo.SetMoney( iMoney );
			g_MyInfo.SetGrade( iGradeLevel, iGradeExpert );
			if( iPrevGradeLevel != g_MyInfo.GetGradeLevel() )
			{				
				if( pChar )
					pChar->SetLevel( iGradeLevel );
			}

			int iMaxClassReward;
			rkPacket >> iMaxClassReward;
			for(int k = 0;k < iMaxClassReward;k++)
			{
				int iClassType, iClassPoint;
				rkPacket >> iClassType >> iClassPoint;
				g_MyInfo.AddClassExp( iClassType, iClassPoint );
			}

			if( iRewardExp > 0 )
			{
				g_ChatMgr.SetGradeExpObtainMsg( iRewardExp );
			}

			if( iRewardPeso > 0 )
			{
				g_ChatMgr.SetPesoObtainMsg( iRewardPeso );
			}
		}

		if( pChar )
		{
			// 획득 애니메이션
			pChar->SetMonsterReward( iRewardExp, iRewardPeso );

		}
	}	
}


void ioTrainingMode::OnSpawnMonster(SP2Packet &rkPacket)
{
	int nNpc = 0;
	rkPacket >> nNpc;

	for(int i = 0; i < nNpc; i++)
	{
		DWORD dwNpcCode, dwStartTime, dwAliveTime;
		float fStartXPos, fStartZPos;
		ioHashString kNpcName, kSyncName;
		int nTeam;
		int nGrowthLvl;
		int nGroupIdx = 0 ;
		bool bGroupBoss = false;

		rkPacket >> dwNpcCode >> kNpcName >> kSyncName >> nTeam >> nGrowthLvl >> bGroupBoss >> nGroupIdx >>
			dwStartTime >> fStartXPos >> fStartZPos >> dwAliveTime >> m_bAwakening;
		ioNpcChar *pNpcChar = g_NpcMgr.CreateNpcChar( NPC_NOT_INDEX, dwNpcCode, m_pCreator, this );
		if( !pNpcChar ) continue;

		pNpcChar->FillMaxHP();
		pNpcChar->SetCharName( kNpcName );
		pNpcChar->SetSyncUser( kSyncName );
		pNpcChar->SetTeam( (TeamType)nTeam );
		pNpcChar->StartAI( dwStartTime , fStartXPos, fStartZPos );
		pNpcChar->SetNpcGradeShow(true);
		pNpcChar->SetGradeUpRandomSeed( g_BattleRoomMgr.GetIndex() + kNpcName.GetHashCode() );
		pNpcChar->ReSetGradeLevel( nGrowthLvl );
		pNpcChar->SetDisableGauge(true);

		AddNewRecord( pNpcChar );

		PlazaNPCGaugeWnd *pPlazaNPCGaugeWnd = dynamic_cast<PlazaNPCGaugeWnd *>( g_GUIMgr.FindWnd( PLAZA_NPC_GAUGE_WND ) );
		if(pPlazaNPCGaugeWnd)
		{
			pPlazaNPCGaugeWnd->InitAniState();

			PlazaNPCGaugeWnd::GaugeType eType = PlazaNPCGaugeWnd::GAUGE_NORMAL_NPC;

			if( m_bAwakening )
				eType = PlazaNPCGaugeWnd::GAUGE_AWAKENING_NPC;

			pPlazaNPCGaugeWnd->SetBossMonster( eType, pNpcChar->GetCharName(), pNpcChar->GetMonsterIcon(), 
						pNpcChar->GetImgName(), dwAliveTime );

			OnChangeTeam(true);

			AniTextWnd *pAniTextWnd = dynamic_cast<AniTextWnd *>( g_GUIMgr.FindWnd(ANI_TEXT_WND) );

			if(pAniTextWnd)
			{
				pAniTextWnd->ShowWnd();
				if( m_bAwakening )
					pAniTextWnd->StartAniText(AniTextWnd::AWAKENING_SUBWND);
				else
					pAniTextWnd->StartAniText(AniTextWnd::ARRIVAL_SUBWND);
			}
		}
	}
}

void ioTrainingMode::OnChangeTeam(bool bBlue)
{
	TeamType eTeam = TEAM_PRIVATE_1;

	if( bBlue )
		eTeam = TEAM_BLUE;

	int iCharCnt = m_vCharRecordList.size();

	for(int i = 0;i < iCharCnt;i++)
	{
		TrainingRecord *pRecord = &m_vCharRecordList[i];
		if( !pRecord )	continue;
		if( !pRecord->pChar ) continue;
		if( ToNpcChar(pRecord->pChar) ) continue;

		pRecord->pChar->SetTeam( eTeam );
		if( !bBlue )
			eTeam = TeamType((int)eTeam + 1);
	}
}

void ioTrainingMode::OnRunningManDecoSync( SP2Packet &rkPacket )
{
	ioHashString kName;
	rkPacket >> kName;

	TrainingRecord *pRecord = FindRecordCharName( kName );
	if( pRecord )
	{
		rkPacket >> pRecord->dwRunningManDeco;
	}
}

void ioTrainingMode::OnRunningManNameSync( SP2Packet &rkPacket )
{
	ioHashString kName;
	rkPacket >> kName;

	TrainingRecord *pRecord = FindRecordCharName( kName );
	if( pRecord )
	{
		rkPacket >> pRecord->szRunningManName;

		if( pRecord->pChar )
			pRecord->pChar->SetRunningManName( pRecord->szRunningManName );
	}
}

bool ioTrainingMode::ProcessUDPPacket( SP2Packet &rkPacket )
{
	if( ioPlayMode::ProcessUDPPacket( rkPacket ) )
		return true;
	
	switch( rkPacket.GetPacketID() )
	{
	case CUPK_SYMBOL_DAMAMGED:
		OnSymbolDamaged( rkPacket );
		return true;
	}

	return false;
}

void ioTrainingMode::OnUserRevival( SP2Packet &rkPacket )
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
//		LOG.PrintTimeAndLog( 0, "ioPlayMode::OnUserRevival() - CharChange: %d", FRAMEGETTIME() );
		StartChangeCharacter( pChar->GetSelectCharArray(), dwSelectTime );
	}

	D3DXVECTOR3 vRevivalPos = GetRevivalPos( pChar );
	pChar->SetWorldPosition( vRevivalPos );
	pChar->SetReadyState();
}


void ioTrainingMode::OnRoundJoin( SP2Packet &rkPacket )
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
			{
				m_dwPlayStartTime = dwStartTime;
				PlazaNPCGaugeWnd *pPlazaNPCGaugeWnd = dynamic_cast< PlazaNPCGaugeWnd * >( g_GUIMgr.FindWnd( PLAZA_NPC_GAUGE_WND ) );
				if( !pPlazaNPCGaugeWnd ) return;

				// 보스 게이지
				if( !pPlazaNPCGaugeWnd->IsShow() )
				{
					TrainingRecord *pRecord = FindRecordCharName( pPlazaNPCGaugeWnd->GetMonsterName() );

					if( pRecord && ToNpcChar( pRecord->pChar ) )
					{	
						if( pRecord->pChar->GetState() != CS_DIE )
						{
							AniTextWnd *pAniTextWnd = dynamic_cast<AniTextWnd *>( g_GUIMgr.FindWnd(ANI_TEXT_WND) );

							if(pAniTextWnd)
							{
								pAniTextWnd->ShowWnd();
								if( m_bAwakening )
									pAniTextWnd->StartAniText(AniTextWnd::AWAKENING_SUBWND);
								else
									pAniTextWnd->StartAniText(AniTextWnd::ARRIVAL_SUBWND);
							}
						}
					}
				}
			}
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

			//
			g_QuestMgr.QuestCompleteTerm( QC_ENTER_PLAZA );

			//
			g_QuestMgr.QuestOccurTerm( QO_PLAZA_ENTER );
			g_QuestMgr.QuestOccurTerm( QO_LOBBYNPLAZA_ENTER );
		}

		g_ModeHelpMgr.InitModeHelp();

		
		if( pJoinChar->IsChatModeReserve() )
		{
			g_ChatMacroMgr.MacroProcess( "/대화" );
			pJoinChar->SetChatModeReserve( false );
		}		
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

void ioTrainingMode::ApplyModeInfo( SP2Packet &rkPacket )
{
	m_bStopTime = true;

	int iSymbolCnt;
	int iMaxSymbolCnt = m_vSymbolList.size();
	
	rkPacket >> iSymbolCnt;

	bool bActive;
	int iTeamType, iRevivalCnt;

	for( int i=0 ; i<iSymbolCnt ; i++ )
	{
		rkPacket >> bActive >> iTeamType >> iRevivalCnt;

		if( !COMPARE( i, 0, iMaxSymbolCnt ) )
			continue;

		// 싱글팀에서 팀 컬러를 판단할 수 없다. 이유는 유저 정보를 가져오기 전에 상징물 정보를 먼저 받으므로....
		ioSymbolStruct *pSymbol = m_vSymbolList[i];
		pSymbol->SetSymbolActivity( bActive );
		pSymbol->SetTeam( (TeamType)iTeamType );
		pSymbol->SetRevivalCount( iRevivalCnt );
	}

	m_SingleTeamPosArray.clear();
	int iPosCnt;
	rkPacket >> iPosCnt;

	for( int j=0; j < iPosCnt; ++j )
	{
		int iPosArray;
		rkPacket >> iPosArray;
		m_SingleTeamPosArray.push_back( iPosArray );
	}

	// 룰렛동기화
	if( g_RouletteMgr.IsRouletteEvent() )
		g_RouletteMgr.ApplyRouletteInfo( rkPacket );
}

void ioTrainingMode::ApplyCharModeInfo( const ioHashString &rkName,
									  SP2Packet &rkPacket, bool bDieCheck )
{
	bool bRecord;
	rkPacket >> bRecord;

	if( !bRecord ) return;

	TrainingRecord *pRecord = FindRecordCharName( rkName );
	if( pRecord )
	{
		rkPacket >> pRecord->dwRunningManDeco >> pRecord->szRunningManName;
		if( pRecord->pChar && pRecord->dwRunningManDeco > 0 )
		{
			pRecord->pChar->SetRunningManDecoIndex( pRecord->dwRunningManDeco, true );
			pRecord->pChar->SetRunningManName( pRecord->szRunningManName );
		}

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

void ioTrainingMode::GetModeFixedDeco( DWORD dwIndex, ioBaseChar *pChar, CHARACTER &rkCharInfo, TeamType eTeam )
{
	if( dwIndex == 0 ) return;
	if( pChar == NULL ) return;

	RunningManDecoMap::iterator iter = m_RunningManDecoList.find( dwIndex );
	if( iter == m_RunningManDecoList.end() )
		return;

	RunningManDeco &rkDeco = iter->second;

	if( ( rkCharInfo.m_sex - 1 ) == RDT_HUMAN_MAN )		// 남자
	{
		// 얼굴
		if( rkDeco.m_iManFace > 0 )
			rkCharInfo.m_face = rkDeco.m_iManFace;

		// 피부색
		if( rkDeco.m_iManSkinColor > 0 )
			rkCharInfo.m_skin_color = rkDeco.m_iManSkinColor;

		// 머리 
		if( rkDeco.m_iManHair > 0 )
			rkCharInfo.m_hair = rkDeco.m_iManHair;

		// 머리색
		if( rkDeco.m_iManHairColor > 0 )
			rkCharInfo.m_hair_color = rkDeco.m_iManHairColor;

		// 속옷
		if( rkDeco.m_iManUnderwearBlue > 0 )
		{
			rkCharInfo.m_underwear = rkDeco.m_iManUnderwearBlue;
		}

		// 본 교체
		if( !rkDeco.m_ManSkeletonName.IsEmpty() )
		{
			pChar->ChangeSkeleton( rkDeco.m_ManSkeletonName );
		}
	}
	else												// 여자
	{
		// 얼굴
		if( rkDeco.m_iWomanFace > 0 )
			rkCharInfo.m_face = rkDeco.m_iWomanFace;

		// 피부색
		if( rkDeco.m_iWomanSkinColor > 0 )
			rkCharInfo.m_skin_color = rkDeco.m_iWomanSkinColor;

		// 머리 
		if( rkDeco.m_iWomanHair > 0 )
			rkCharInfo.m_hair = rkDeco.m_iWomanHair;

		// 머리색
		if( rkDeco.m_iWomanHairColor > 0 )
			rkCharInfo.m_hair_color = rkDeco.m_iWomanHairColor;

		// 속옷
		if( rkDeco.m_iWomanUnderwearBlue > 0 )
			rkCharInfo.m_underwear = rkDeco.m_iWomanUnderwearBlue;

		// 본 교체
		if( !rkDeco.m_WomanSkeletonName.IsEmpty() )
		{
			pChar->ChangeSkeleton( rkDeco.m_WomanSkeletonName );
		}
	}
}

int ioTrainingMode::GetRoundWinCnt( TeamType eTeamType )
{
	return 0;
}

void ioTrainingMode::CheckRoundEnd()
{
}

void ioTrainingMode::ContributeConvertTeam( TeamType eTeam )
{	
}

void ioTrainingMode::ContributeRoundAllPercent( DWORD dwCurTime, DWORD dwTotalTime )
{
}

void ioTrainingMode::ContributeConvert()
{
	ContributeConvertTeam( TEAM_NONE );
}

void ioTrainingMode::DamageContribute( const ioHashString szAttacker, const ioHashString szTarget, float fDamage, DWORD dwItemCode, int iWeaponType )
{	
}

void ioTrainingMode::KillContribute( const ioHashString szKiller, const ioHashString szKilled, DWORD dwItemCode )
{
}

void ioTrainingMode::AttackCntContribute( const ioHashString szAttacker, const ioHashString szTarget, DWORD dwItemCode, int iAttackValue/* = 100 */ )
{
}

void ioTrainingMode::ContributeLOG()
{
	char szLog[MAX_PATH];

	int iCharCnt = m_vCharRecordList.size();
	for( int i=0 ;i<iCharCnt; i++ )
	{
		TrainingRecord *pRecord = &m_vCharRecordList[i];
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


void ioTrainingMode::FillLastPlayRecordInfo( SP2Packet &rkPacket )
{
	int iCharCnt = m_vCharRecordList.size();
	rkPacket << iCharCnt;
	for(int i = 0;i < iCharCnt;i++)
	{
		TrainingRecord *pRecord = &m_vCharRecordList[i];
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

void ioTrainingMode::FillPlayRecordInfo( SP2Packet &rkPacket )
{
	int iCharCnt = m_vCharRecordList.size();
	rkPacket << iCharCnt;
	for(int i = 0;i < iCharCnt;i++)
	{
		TrainingRecord *pRecord = &m_vCharRecordList[i];
		rkPacket << pRecord->pChar->GetCharName();
		rkPacket << pRecord->GetUniqueTotalKill();
		rkPacket << pRecord->GetUniqueTotalDeath();
	}
}

void ioTrainingMode::ApplyPlayRecordInfo( SP2Packet &rkPacket )
{
	int iCharCnt;
	rkPacket >> iCharCnt;
	for(int i = 0;i < iCharCnt;i++)
	{
		ioHashString szName;
		int iUniqueTotalKill, iUniqueTotalDeath;
		rkPacket >> szName >> iUniqueTotalKill >> iUniqueTotalDeath;

		TrainingRecord *pRecord = FindRecordCharName( szName );
		if( pRecord )
		{
			pRecord->iUniqueTotalKill = iUniqueTotalKill;
			pRecord->iUniqueTotalDeath= iUniqueTotalDeath;
		}
	}
}

void ioTrainingMode::ContributeResultSync( const ioHashString &szName, int iContribute )
{
	TrainingRecord *pRecord = FindRecordCharName( szName );
	if( !pRecord ) return;

	pRecord->Contribute.m_bLastResultSync = true;
	pRecord->Contribute.m_iContribute     = iContribute;
}

void ioTrainingMode::OnSymbolDie( SP2Packet &rkPacket )
{
	ioHashString szKiller;
	rkPacket >> szKiller;

	ioBaseChar *pKiller = m_pCreator->GetBaseChar( szKiller );
	if( !pKiller )	return;

	int iSymbolIdx;
	rkPacket >> iSymbolIdx;

	ioSymbolStruct *pSymbol = GetSymbol( iSymbolIdx );
	if( !pSymbol )	return;

	float fRevivalHP;
	rkPacket >> fRevivalHP;
	pSymbol->SetRevivalHP( fRevivalHP );

	ProcessSymbolKill( pSymbol, pKiller );

	int iRevivalCnt;
	rkPacket >> iRevivalCnt;
	pSymbol->SetRevivalCount( iRevivalCnt );

	int iTeamType;
	rkPacket >> iTeamType;

	SymbolDestroyAnnounce( (TeamType)iTeamType );

	// 상징물 파괴
	g_ChatMgr.SetSymbolDieMsg( pKiller->GetTeam(), szKiller );
}

void ioTrainingMode::OnSymbolDieList( SP2Packet &rkPacket )
{
	int iDieCnt;
	rkPacket >> iDieCnt;

	if( iDieCnt <= 0 ) return;

	for( int i=0; i < iDieCnt; ++i )
	{
		int iArrayIndex, iRevivalCnt, iKillTeam;
		rkPacket >> iArrayIndex >> iRevivalCnt >> iKillTeam;

		ioSymbolStruct *pSymbol = GetSymbol( iArrayIndex );
		if( !pSymbol )	continue;

		pSymbol->SetRevivalCount( iRevivalCnt );
		pSymbol->SetTeam( (TeamType)iKillTeam );
		pSymbol->SetRevivalHP( 0.0f );
		pSymbol->Revival();
	}
}

void ioTrainingMode::ProcessSymbolKill( ioSymbolStruct *pSymbol, ioBaseChar *pKillChar )
{
	if( !pSymbol )
	{
		LOG.PrintTimeAndLog( 0, "ioTrainingMode::ProcessSymbolKill - Symbol is NULL" );
		return;
	}

	if( !pKillChar )
	{
		LOG.PrintTimeAndLog( 0, "ioTrainingMode::ProcessSymbolKill - KillChar is NULL" );
		return;
	}

	TeamType eKillTeam = pKillChar->GetTeam();
	pSymbol->SetTeam( eKillTeam );

	pSymbol->SetRevivalHP( 0.0f );
	pSymbol->Revival();
	pSymbol->SetChangeEvent();

	CreateSymbolTeamChangeEffect( pSymbol->GetWorldPosition(), eKillTeam );
	SetMiniMapEvent( pSymbol->GetWorldPosition(), MiniMap::EVENT_SYMBOL_DESTROY, eKillTeam );
}

void ioTrainingMode::OnSymbolActivity( SP2Packet &rkPacket )
{
	int iSymbolCnt;
	rkPacket >> iSymbolCnt;

	iSymbolCnt = min( iSymbolCnt, (int)m_vSymbolList.size() );
	for( int i=0 ; i<iSymbolCnt ; i++ )
	{
		ioSymbolStruct *pSymbol = m_vSymbolList[i];

		bool bActive;
		rkPacket >> bActive;

		if( pSymbol->GetSymbolActivity() != bActive )
		{
			pSymbol->SetSymbolActivity( bActive );
		}
	}
}

void ioTrainingMode::CreateSymbolTeamChangeEffect( const D3DXVECTOR3 &vPos, TeamType eTeam )
{
	if( eTeam == GetOwnerTeam() && !m_BlueSymbolChangeEffect.IsEmpty() )
	{
		m_pCreator->CreateMapEffect( m_BlueSymbolChangeEffect,
									 vPos,
									 ioMath::UNIT_ALL );
	}
	else if( eTeam != GetOwnerTeam() && !m_RedSymbolChangeEffect.IsEmpty() )
	{
		m_pCreator->CreateMapEffect( m_RedSymbolChangeEffect,
									 vPos,
									 ioMath::UNIT_ALL );
	}
}

void ioTrainingMode::LoadSymbols()
{
	ioINILoader kLoader( GetModeMapINIFileName( m_ModeType, m_iModeSubNum, m_iModeMapIndex ) );
	kLoader.SetTitle_e( "symbol_common_info" );

	D3DXVECTOR3 vScale;
	vScale.x = kLoader.LoadFloat_e( "scale_x", FLOAT05 );
	vScale.y = kLoader.LoadFloat_e( "scale_y", FLOAT05 );
	vScale.z = kLoader.LoadFloat_e( "scale_z", FLOAT05 );

	char szEffectFile[MAX_PATH];
	kLoader.LoadString_e( "red_team_change", "sp2_red_symbol_change.txt", szEffectFile, MAX_PATH );
	m_RedSymbolChangeEffect = szEffectFile;

	kLoader.LoadString_e( "blue_team_change", "", szEffectFile, MAX_PATH );
	m_BlueSymbolChangeEffect = szEffectFile;

	kLoader.LoadString_e( "apply_effect", "", szEffectFile, MAX_PATH );
	m_ApplyEffect = szEffectFile;

	char szRedSymbol[MAX_PATH], szBlueSymbol[MAX_PATH], szNoneActivitySymbol[MAX_PATH];

	int iSymbolCnt = kLoader.LoadInt_e( "symbol_cnt", 0 );

	for( int i=0 ; i<iSymbolCnt; i++ )
	{
		char szTitle[MAX_PATH];
		wsprintf_e( szTitle, "symbol%d", i+1 );
		kLoader.SetTitle( szTitle );

		kLoader.LoadString_e( "red_symbol", "", szRedSymbol, MAX_PATH );
		kLoader.LoadString_e( "blue_symbol", "", szBlueSymbol, MAX_PATH );
		kLoader.LoadString_e( "none_activity_symbol", "", szNoneActivitySymbol, MAX_PATH );

		int iPriority = kLoader.LoadInt_e( "symbol_priority", 0 );

		char szTeamType[MAX_PATH];
		kLoader.LoadString_e( "team", "NONE", szTeamType, MAX_PATH );
		TeamType eSymbolTeam = ConvertStringToTeamType( szTeamType );

		ioEntityGroup *pGrp = NULL;
		switch( eSymbolTeam )
		{
		case TEAM_NONE:
			pGrp = m_pCreator->CreateEntityGrpAndSceneNode( szNoneActivitySymbol );
			break;
		case TEAM_RED:
			pGrp = m_pCreator->CreateEntityGrpAndSceneNode( szRedSymbol );
			break;
		case TEAM_BLUE:
			pGrp = m_pCreator->CreateEntityGrpAndSceneNode( szBlueSymbol );
			break;
		}

		if( !pGrp )
		{
			LOG.PrintTimeAndLog( 0, "ioTrainingMode::LoadSymbols - %d Symbol File wrong!", i+1 );
			continue;
		}

		ioSymbolStruct *pSymbol = new ioSymbolStruct( pGrp, this );
		m_pCreator->AddGameEntity( pSymbol );

		//HARDCODE: activity 설정을 team 설정 보다 먼저 해줘야 한다.
		pSymbol->InitResource();
		pSymbol->LoadProperty( kLoader );
		pSymbol->SetSymbolActivity( false );
		pSymbol->SetOrgTeam( eSymbolTeam );
		pSymbol->SetSymbolPriority( iPriority );

		D3DXVECTOR3 vPos;
		vPos.x = kLoader.LoadFloat_e( "pos_x", FLOAT1000 );
		vPos.z = kLoader.LoadFloat_e( "pos_z", FLOAT1000 );
		vPos.y = m_pCreator->GetMapHeight( vPos.x, vPos.z, pSymbol, false );
		pSymbol->InitSymbolInfo( i, vPos, vScale );

		m_vSymbolList.push_back( pSymbol );
	}
}

ioSymbolStruct* ioTrainingMode::GetSymbol( int iArray )
{
	int iSymbolCnt = m_vSymbolList.size();
	if( COMPARE( iArray, 0, iSymbolCnt ) )
		return m_vSymbolList[iArray];

	return NULL;
}

void ioTrainingMode::OnSymbolDamaged( SP2Packet &rkPacket )
{
	ioHashString szAttacker;
	rkPacket >> szAttacker;

	ioBaseChar *pAttacker = GetRecordChar( szAttacker );
	if( !pAttacker )
	{
		LOG.PrintTimeAndLog( 0, "ioTrainingMode::OnSymbolDamaged - %s Not Exist", szAttacker.c_str() );
		return;
	}

	int iSymbolArray;
	rkPacket >> iSymbolArray;
	
	ioSymbolStruct *pSymbol = GetSymbol( iSymbolArray );
	if( pSymbol )
	{
		pSymbol->OnSymbolDamaged( rkPacket, pAttacker );
	}
	else
	{
		LOG.PrintTimeAndLog( 0, "ioTrainingMode::onSymbolDamaged - %d Not Exist Symbol", iSymbolArray );
	}
}

void ioTrainingMode::GetModeSpecialInfo( SP2Packet &rkPacket )
{
	ioPlayMode::GetModeSpecialInfo( rkPacket );

	int iSymbolCnt = m_vSymbolList.size();
	rkPacket << iSymbolCnt;

	for( int i=0 ; i<iSymbolCnt ; i++ )
	{
		rkPacket << m_vSymbolList[i]->GetHP().m_fCurValue;
		rkPacket << m_vSymbolList[i]->GetHP().m_fMaxValue;
		rkPacket << m_vSymbolList[i]->GetCurDecreaseRate();
	}
}

void ioTrainingMode::OnModeSpecialInfo( SP2Packet &rkPacket )
{
	ioPlayMode::OnModeSpecialInfo( rkPacket );

	int iSymbolCnt;
	rkPacket >> iSymbolCnt;
	
	iSymbolCnt = min( iSymbolCnt, (int)m_vSymbolList.size() );
	for( int i=0 ; i<iSymbolCnt ; i++ )
	{
		float fCurHP, fMaxHP, fDecreaseCurRate;
		rkPacket >> fCurHP >> fMaxHP >> fDecreaseCurRate;

		m_vSymbolList[i]->SetHP( fCurHP, fMaxHP, fDecreaseCurRate );
	}
}

void ioTrainingMode::ProcessSymbolAlphaRate()
{
	ioBaseChar *pMyChar = GetOwnerChar();
	if( !pMyChar )	return;

	ioCamera *pCamera = g_App.GetCamera();
	if( !pCamera )	return;

	D3DXVECTOR3 vMyPos = pMyChar->GetMidPositionByRate();
	D3DXVECTOR3 vDiff = pCamera->GetPosition() - vMyPos;
	ioRay kCamRay( vMyPos, vDiff );

	int iSymbolCount = m_vSymbolList.size();
	for( int i=0 ;i<iSymbolCount ; i++ )
	{
		ioSymbolStruct *pSymbol = m_vSymbolList[i];

		if( !pSymbol->IsNowAlphaChanging() )
		{
			if( ioMath::TestIntersection( kCamRay, pSymbol->GetWorldCollisionBox() ) )
			{
				pSymbol->SetTargetAlphaRate( m_iSymbolAlphaRate );
			}
			else
			{
				pSymbol->SetTargetAlphaRate( MAX_ALPHA_RATE );
			}
		}
	}
}

void ioTrainingMode::ProcessSymbolZoneCollision()
{
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	int iCharCnt = m_vCharRecordList.size();	
	for( int i=0 ; i<iCharCnt ; i++ )
	{
		TrainingRecord &rkRecord = m_vCharRecordList[i];

		ioBaseChar *pChar = rkRecord.pChar;
		ioSymbolStruct *pApplyZone = NULL;

		int iZoneCnt = m_vSymbolList.size();
		for( int j=0 ; j<iZoneCnt ; j++ )
		{
			ioSymbolStruct *pSymbol = m_vSymbolList[j];

			if( pSymbol->IsInSymbolRange( pChar ) )
			{
				pApplyZone = pSymbol;				
				break;
			}
		}

		if( pApplyZone )
		{
			if( !rkRecord.pSymbol )
			{
				rkRecord.pSymbol = pApplyZone;

				pChar->AttachEffect( m_ApplyEffect );
				pChar->SetExtraSkillGaugeRate( pApplyZone->GetSkillGaugeRate() );
				pChar->SetExtraDamageRate( pApplyZone->GetCharDamageRate() );
			}
		}
		else
		{
			if( rkRecord.pSymbol )
			{
				rkRecord.pSymbol = NULL;

				pChar->EndEffect( m_ApplyEffect );
				pChar->SetExtraSkillGaugeRate( FLOAT1 );
				pChar->SetExtraDamageRate( FLOAT1 );
			}
		}
	}
}

void ioTrainingMode::SymbolDestroyAnnounce( TeamType eKillTeam )
{
	if( m_WinTeamType != WTT_NONE || eKillTeam == TEAM_NONE )
		return;

	if( GetOwnerTeam() == eKillTeam )
		g_SoundMgr.PlaySound( m_SymbolOccupy );
	else
		g_SoundMgr.PlaySound( m_SymbolDestroy );
}

int ioTrainingMode::GetCurRank( const ioHashString& szCharName )
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

int ioTrainingMode::GetContributePerChar( const ioHashString& szName )
{
	int iCharCnt = m_vCharRecordList.size();

	for(int i = 0;i < iCharCnt;i++)
	{
		TrainingRecord *pRecord = &m_vCharRecordList[i];
		if( !pRecord )	continue;
		if( !pRecord->pChar ) continue;

		if( pRecord->pChar->GetCharName() == szName )
		{
			return pRecord->Contribute.m_iPrevContributePer;
		}
	}

	return 0;
}

bool ioTrainingMode::CheckCreateMapAreaWeapon( DWORD dwCheckTime, DWORD dwCreateTime, bool bFirst )
{
	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwGapTime = dwCurTime - m_dwPlayStartTime;

	DWORD dwPeriodTime = 0;
	if( m_dwPrePeriodTime > 0 )
	{
		dwPeriodTime = m_dwPrePeriodTime;
	}

	while( dwPeriodTime < dwGapTime )
	{
		dwPeriodTime += dwCreateTime;

		DWORD dwTestTime = m_dwPlayStartTime + dwPeriodTime;

		if( !bFirst && dwTestTime < dwCheckTime+(dwCreateTime/2) )
			continue;

		if( COMPARE( dwTestTime, dwCheckTime, dwCurTime ) )
		{
			m_dwPrePeriodTime = dwPeriodTime;
			return true;
		}
	}

	return false;
}

void ioTrainingMode::OnChangeCharNextProcess( ioBaseChar *pChar, bool bNewChar )
{
	if( !pChar ) return;

	ChangeSkeletonEtcItem( pChar->GetCharName() );

	if( pChar->IsOwnerChar() )
	{
		if( pChar->GetExperienceMode() != EMS_NONE )
			pChar->SetExperienceMode( EMS_NONE, "", 0 );
	}

	TrainingRecord *pRecord = FindRecordCharName( pChar->GetCharName() );
	if( pRecord == NULL ) return;
	if( pRecord->dwRunningManDeco == 0 ) return;

	pChar->SetRunningManDecoIndex( pRecord->dwRunningManDeco, true );
	pChar->SetRunningManName( pRecord->szRunningManName );


	if( pChar->IsOwnerChar() && IsRunningMainNameTitleUse() )
	{
		RunningManNameWnd *pNameSelectWnd = dynamic_cast< RunningManNameWnd * >( g_GUIMgr.FindWnd( RUNNING_MAN_NAME_WND ) );
		if( pNameSelectWnd )
		{
			if( !pNameSelectWnd->IsShow() && pNameSelectWnd->IsSelectName() )
				pNameSelectWnd->ShowWnd();
		}
	}
}

void ioTrainingMode::OnMonsterSyncChange( SP2Packet &rkPacket )
{
	int iMaxNpc;
	rkPacket >> iMaxNpc;
	for(int i = 0;i < iMaxNpc;i++)
	{
		ioHashString szNpcName, szSyncName;
		rkPacket >> szNpcName >> szSyncName;

		TrainingRecord *pRecord = FindRecordCharName( szNpcName );
		if( !pRecord ) continue;

		ioNpcChar *pNpc = ToNpcChar( pRecord->pChar );
		if( pNpc == NULL ) continue;

		pNpc->SetSyncUser( szSyncName );
	}
}


void ioTrainingMode::OnMonsterInfoSync( SP2Packet &rkPacket )
{
	int i, iMaxNpc;
	rkPacket >> iMaxNpc;

	if( iMaxNpc == 0 )
	{

	}
	else
	{
		for(i = 0;i < iMaxNpc;i++)
		{
			DWORD dwNpcCode, dwStartTime, dwAliveTime;
			float fStartXPos, fStartZPos;
			ioHashString kNpcName, kSyncName;
			int nTeam;
			int nGroupIdx = 0;
			bool bGroupBoss = false;

			rkPacket >> dwNpcCode >> kNpcName >> kSyncName >> nTeam >> bGroupBoss >> nGroupIdx >>
				dwStartTime >> fStartXPos >> fStartZPos >> dwAliveTime >> m_bAwakening;
			ioNpcChar *pNpcChar = g_NpcMgr.CreateNpcChar( NPC_NOT_INDEX, dwNpcCode, m_pCreator, this );
			if( !pNpcChar ) continue;

			pNpcChar->FillMaxHP();
			pNpcChar->SetCharName( kNpcName );
			pNpcChar->SetSyncUser( kSyncName );
			pNpcChar->SetTeam( (TeamType)nTeam );
			pNpcChar->StartAI( dwStartTime , fStartXPos, fStartZPos );
			pNpcChar->SetGradeUpRandomSeed( g_BattleRoomMgr.GetIndex() + kNpcName.GetHashCode() );
			pNpcChar->ReSetGradeLevel( 0 );
			pNpcChar->SetDisableGauge(true);

			AddNewRecord( pNpcChar );

			PlazaNPCGaugeWnd *pPlazaNPCGaugeWnd = dynamic_cast< PlazaNPCGaugeWnd * >( g_GUIMgr.FindWnd( PLAZA_NPC_GAUGE_WND ) );
			if( pPlazaNPCGaugeWnd )
			{
				PlazaNPCGaugeWnd::GaugeType eType = PlazaNPCGaugeWnd::GAUGE_NORMAL_NPC;

				if( m_bAwakening )
					eType = PlazaNPCGaugeWnd::GAUGE_AWAKENING_NPC;

				pPlazaNPCGaugeWnd->SetBossMonster( eType, pNpcChar->GetCharName(), pNpcChar->GetMonsterIcon(), 
					pNpcChar->GetImgName(), dwAliveTime );

				OnChangeTeam(true);
			}
		}
	}
}
