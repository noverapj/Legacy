

#include "stdafx.h"

#include "ioCatchRunningManMode.h"
#include "ioRankBattleManager.h"
#include "ioShuffleRoomMgr.h"

#include "GUI/CenterInfoWnd.h"
#include "GUI/MiniSoldierSelectWnd.h"
#include "GUI/TimeScoreWnd.h"
#include "GUI/ioObserverWnd.h"
#include "GUI/RunningManNameWnd.h"

ioCatchRunningManMode::ioCatchRunningManMode( ioPlayStage *pCreator ) : ioCatchMode( pCreator )
{
	m_ModeType = MT_CATCH_RUNNINGMAN;

	m_pRunningManTitleLeft  = NULL;
	m_pRunningManTitleCenter= NULL;
	m_pRunningManTitleRight = NULL;

	m_bNameTitleUse = false;

	m_vCharRecordList.reserve( 100 );
}

ioCatchRunningManMode::~ioCatchRunningManMode()
{
	ClearRunningManDeco();
}

void ioCatchRunningManMode::ClearRunningManDeco()
{
	m_RunningManDecoList.clear();
	SAFEDELETE( m_pRunningManTitleLeft );
	SAFEDELETE( m_pRunningManTitleCenter );
	SAFEDELETE( m_pRunningManTitleRight );
}

void ioCatchRunningManMode::InitMode( int iMapIndex )
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

void ioCatchRunningManMode::LoadModeResource( ioINILoader &rkLoader )
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

	g_GUIMgr.AddWnd( "XML/SkillListWnd.xml" );
	g_GUIMgr.AddWnd( "XML/AccessorySkillListWnd.xml" );
	g_GUIMgr.AddWnd( "XML/runningmannamewnd.xml" );

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

void ioCatchRunningManMode::LoadModeDeco( ioINILoader &rkLoader )
{
	ClearRunningManDeco();

	char szBuf[MAX_PATH] = "";
	rkLoader.SetTitle_e( "runningman_info" );

	SAFEDELETE( m_pRunningManTitleLeft );
	SAFEDELETE( m_pRunningManTitleCenter );
	SAFEDELETE( m_pRunningManTitleRight );
	m_bNameTitleUse = rkLoader.LoadBool_e( "title_use", false );

	rkLoader.LoadString_e( "title_name_left", "", szBuf, MAX_PATH );
	m_pRunningManTitleLeft	= g_UIImageSetMgr.CreateImageByFullName( szBuf, true );

	rkLoader.LoadString_e( "title_name_center", "", szBuf, MAX_PATH );
	m_pRunningManTitleCenter= g_UIImageSetMgr.CreateImageByFullName( szBuf, true );

	rkLoader.LoadString_e( "title_name_right", "", szBuf, MAX_PATH );
	m_pRunningManTitleRight	= g_UIImageSetMgr.CreateImageByFullName( szBuf, true );

	rkLoader.LoadString_e( "prisoner_rope", "runningman_rope", szBuf, MAX_PATH );
	m_PrisonerRope = szBuf;

	int iMaxRunningMan = rkLoader.LoadInt_e( "max_runningman", 0 );
	for(int i = 0;i < iMaxRunningMan;i++)
	{
		char szTitle[MAX_PATH] = "";
		sprintf_e( szTitle, "runningman%d", i + 1 );
		rkLoader.SetTitle( szTitle );

		RunningManDeco kDeco;		
		rkLoader.LoadString_e( "prisoner_effect", "", szBuf, MAX_PATH );
		kDeco.m_PrisonerEffect = szBuf;

		rkLoader.LoadString_e( "escape_effect", "", szBuf, MAX_PATH );
		kDeco.m_EscapeEffect   = szBuf;

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

void ioCatchRunningManMode::AddNewRecord( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	CatchRecord *pCatChRecord = FindRecordCharName(pChar->GetCharName());
	if( !pCatChRecord )
	{
		CatchRecord kRecord;
		kRecord.pChar  = pChar;
		kRecord.szName = pChar->GetCharName();
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
		pCatChRecord->pChar = pChar;
	}
}

void ioCatchRunningManMode::RemoveRecord( const ioHashString &rkName )
{
	int iCharCnt = m_vCharRecordList.size();
	for( int i=0 ; i<iCharCnt ; i++ )
	{
		if( m_vCharRecordList[i].szName == rkName )
		{
			ioBaseChar *pChar = m_vCharRecordList[i].pChar;
			if( pChar->IsPrisonerMode() || pChar->GetState() == CS_DIE )
			{
				if( pChar->GetTeam() == TEAM_RED )
					m_iBlueCatchRedPlayer--;
				else if( pChar->GetTeam() == TEAM_BLUE )
					m_iRedCatchBluePlayer--;
			}

			m_vCharRecordList.erase( m_vCharRecordList.begin() + i );
			break;
		}
	}

	UpdateUserRank();
}


void ioCatchRunningManMode::RemoveRecord( int nIndex )
{
	int iCharCnt = m_vCharRecordList.size();
	if( iCharCnt > nIndex )
	{
		m_vCharRecordList.erase( m_vCharRecordList.begin() + nIndex );
		UpdateUserRank();
	}
}


void ioCatchRunningManMode::ApplyCharModeInfo( const ioHashString &rkName, SP2Packet &rkPacket, bool bDieCheck )
{
	bool bRecord;
	rkPacket >> bRecord;

	if( !bRecord ) return;

	CatchRecord *pRecord = FindRecordCharName( rkName );
	if( pRecord )
	{
		// 런닝맨 치장 인덱스
		rkPacket >> pRecord->dwRunningManDeco >> pRecord->szRunningManName;
		if( pRecord->pChar )
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

		bool bPrisoner, bCatch;
		rkPacket >> bPrisoner >> bCatch;

		if( pRecord->pChar )
		{
			if( bPrisoner )
			{
				pRecord->pChar->SetPrisonerMode();
			}
		}
	}
	else
	{
		DWORD dwRunningManDeco;
		ioHashString szRunningManName;
		rkPacket >> dwRunningManDeco >> szRunningManName;

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

		bool bPrisoner, bCatch;
		rkPacket >> bPrisoner >> bCatch;
	}
}

void ioCatchRunningManMode::GetModeFixedDeco( DWORD dwIndex, ioBaseChar *pChar, CHARACTER &rkCharInfo, TeamType eTeam )
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
		if( eTeam == TEAM_BLUE )
		{
			if( rkDeco.m_iManUnderwearBlue > 0 )
			{
				rkCharInfo.m_underwear = rkDeco.m_iManUnderwearBlue;
			}
		}
		else
		{
			if( rkDeco.m_iManUnderwearRed > 0 )
			{
				rkCharInfo.m_underwear = rkDeco.m_iManUnderwearRed;
			}
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
		if( eTeam == TEAM_BLUE )
		{
			if( rkDeco.m_iWomanUnderwearBlue > 0 )
				rkCharInfo.m_underwear = rkDeco.m_iWomanUnderwearBlue;
		}
		else
		{
			if( rkDeco.m_iWomanUnderwearRed > 0 )
				rkCharInfo.m_underwear = rkDeco.m_iWomanUnderwearRed;
		}

		// 본 교체
		if( !rkDeco.m_WomanSkeletonName.IsEmpty() )
		{
			pChar->ChangeSkeleton( rkDeco.m_WomanSkeletonName );
		}
	}
}

const ioHashString &ioCatchRunningManMode::GetRunningManPrisonerEffect( DWORD dwIndex )
{
	static ioHashString kReturn;

	RunningManDecoMap::iterator iter = m_RunningManDecoList.find( dwIndex );
	if( iter == m_RunningManDecoList.end() )
		return kReturn;

	RunningManDeco &rkDeco = iter->second;
	return rkDeco.m_PrisonerEffect;
}

const ioHashString &ioCatchRunningManMode::GetRunningManEscapeEffect( DWORD dwIndex )
{
	static ioHashString kReturn;

	RunningManDecoMap::iterator iter = m_RunningManDecoList.find( dwIndex );
	if( iter == m_RunningManDecoList.end() )
		return kReturn;

	RunningManDeco &rkDeco = iter->second;
	return rkDeco.m_EscapeEffect;
}

bool ioCatchRunningManMode::ProcessTCPPacket( SP2Packet &rkPacket )
{
	ErrorReport::SetPosition( 1180, 1 );
	if( ioCatchMode::ProcessTCPPacket( rkPacket ) )
		return true;

	switch( rkPacket.GetPacketID() )
	{
	case STPK_RUNNINGMAN_DECO_SYNC:
		ErrorReport::SetPosition( 1180, 2 );
		OnRunningManDecoSync( rkPacket );
		ErrorReport::SetPosition( 1180, 3 );
		return true;
	case STPK_RUNNINGMAN_NAME_SYNC:
		ErrorReport::SetPosition( 1180, 4 );
		OnRunningManNameSync( rkPacket );
		ErrorReport::SetPosition( 1180, 5 );
		return true;
	}

	return false;
}

void ioCatchRunningManMode::OnRunningManDecoSync( SP2Packet &rkPacket )
{
	ioHashString kName;
	rkPacket >> kName;

	CatchRecord *pRecord = FindRecordCharName( kName );
	if( pRecord )
	{
		rkPacket >> pRecord->dwRunningManDeco;
	}
}

void ioCatchRunningManMode::OnRunningManNameSync( SP2Packet &rkPacket )
{
	ioHashString kName;
	rkPacket >> kName;

	CatchRecord *pRecord = FindRecordCharName( kName );
	if( pRecord )
	{
		rkPacket >> pRecord->szRunningManName;

		if( pRecord->pChar )
			pRecord->pChar->SetRunningManName( pRecord->szRunningManName );
	}
}

void ioCatchRunningManMode::OnChangeCharNextProcess( ioBaseChar *pChar, bool bNewChar )
{
	if( pChar == NULL ) return;

	CatchRecord *pRecord = FindRecordCharName( pChar->GetCharName() );
	if( pRecord )
	{
		pChar->SetRunningManDecoIndex( pRecord->dwRunningManDeco, true );
		pChar->SetRunningManName( pRecord->szRunningManName );
	}


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