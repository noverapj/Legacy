

#include "stdafx.h"

#include "ioHouseMode.h"
#include "ioPlayStage.h"
#include "ioReservedTask.h"
#include "ioBaseChar.h"

#include "GUI/GameChatInput.h"
#include "GUI/SoldierSelectWnd.h"
#include "GUI/SkillListWnd.h"
#include "GUI/MiniSoldierSelectWnd.h"

#include "GUI/RoundStateMainWnd.h"
#include "GUI/GlobalWnd.h"
#include "GUI/HouseMainWnd.h"
#include "GUI/CenterKillInfoWnd.h"
#include "GUI/MiniMap.h"
#include "GUI/TrainingFuncMenuWnd.h"

#include "ioExtendSoundManager.h"

ioHouseMode::ioHouseMode( ioPlayStage *pCreator ) : ioPlayMode( pCreator )
{
	m_ModeType			= MT_HOUSE;
	m_iMaxPlayer		= MAX_PLAYER / 2;
	m_dwPrePeriodTime	= 0;

	m_SingleTeamPosArray.clear();
	m_vCharRecordList.reserve( 100 );
}

ioHouseMode::~ioHouseMode()
{
	ClearMode();

}

void ioHouseMode::LoadSound()
{
	// 로비 사운드 플레이
	ioINILoader_e kSndLoader( "config/sp2_sound.ini" );
	kSndLoader.SetTitle_e( "sound_file_name" );

	char szBuf[MAX_PATH];
	kSndLoader.LoadString_e( "house_sound", "", szBuf, MAX_PATH );

	if( strcmp( szBuf, "" ) != 0 )
	{
		char szName[MAX_PATH];
		wsprintf( szName, "Resource/wave/%s", szBuf );

		g_SoundMgr.OpenOggStream( szName );
		g_SoundMgr.PlayStream( true );
	}
}

void ioHouseMode::PlayExtendSound()
{
	g_ExSoundMgr.PlaySound( ExSound::EST_HOME_ENTER );
}

void ioHouseMode::ClearMode()
{
	g_GUIMgr.HideWnd( TIMEGATE_SEARCH );
	ioPlayMode::ClearMode();
}

void ioHouseMode::LoadModeResource( ioINILoader &rkLoader )
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

void ioHouseMode::LoadMap()
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

void ioHouseMode::InitMode( int iMapIndex )
{
	m_iModeMapIndex = iMapIndex;

	g_ModeUIController.SetKOMsgWnd( NULL );
	g_ModeUIController.SetCenterInfoWnd( NULL );
	
	m_pCreator->SetCameraModeFixedBlock( false );

	HouseMainWnd *pMain = dynamic_cast< HouseMainWnd * >( g_GUIMgr.AddWnd( "XML/house/housemainwnd.xml" ) );
	if( pMain )
		pMain->SetPlayStage( m_pCreator );
	
	HouseInviteListWnd *pInvite = dynamic_cast< HouseInviteListWnd * >( g_GUIMgr.AddWnd( "XML/house/houseinvitelistwnd.xml" ) );
	if( pInvite )
		pInvite->SetPlayStage( m_pCreator );

	//Party
	g_GUIMgr.AddWnd( "XML/partycreatewnd.xml" );

	g_GUIMgr.AddWnd( "XML/plazaroominfownd.xml" );
	g_GUIMgr.AddWnd( "XML/plazapasswordwnd.xml" );
	g_GUIMgr.AddWnd( "XML/plazainvitedwnd.xml" );
	g_GUIMgr.AddWnd( "XML/plazacreatewnd.xml" );
	g_GUIMgr.AddWnd( "XML/plazaownerwnd.xml" );

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

	m_ModeState		= MS_PLAY;
	m_bFirstStart	= true;

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

	g_GUIMgr.ShowWnd( SKILL_LIST_WND );
	g_GUIMgr.ShowWnd( ACCESSORY_SKILL_LIST_WND );
	LoadSound();
}

void ioHouseMode::ReStartMode()
{
	LOG.PrintTimeAndLog( 0, "ioHouseMode::ReStartMode Is Call???" );
}

void ioHouseMode::ProcessMode()
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
	}
}

void ioHouseMode::ProcessReadyState()
{
	if( P2PNetwork::IsNetworkPlaying() )
		return;

	if( m_dwReadyStartTime + 4000 > FRAMEGETTIME() )
		return;

	SetModeState( MS_PLAY );
}

void ioHouseMode::ProcessPlayState()
{
	UpdateSoldierRevivalPenalty();
	UpdateTimeGrowthTime();

	m_pCreator->NotifySpringDampUseSet( true );

	ProcessRevival();
	if( !P2PNetwork::IsNetworkPlaying() )
	{
		CheckRoundEnd();
	}
}

void ioHouseMode::ProcessResultState()
{
}

void ioHouseMode::ProcessGUI()
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

void ioHouseMode::ProcessRevival()
{
	DWORD dwCurTime = FRAMEGETTIME();
	
	int iCharCnt = m_vCharRecordList.size();
	for( int i=0 ; i<iCharCnt ; i++ )
	{
		HouseRecord &rkRecord = m_vCharRecordList[i];
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


void ioHouseMode::UpdateMiniMapSpotPoint()
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

void ioHouseMode::UpdateRoundStateWnd()
{
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

void ioHouseMode::UpdateDropDieRecord( ioBaseChar *pDieChar, ioBaseChar *pKillChar, ioBaseChar *pBestAttacker )
{
	ioBaseChar *pOwner = GetOwnerChar();
	if( !pOwner )
		return;

	ioHashString szDieName, szAttName;
	if( pKillChar )
	{
		szAttName = pKillChar->GetCharName();
	}

	if( pDieChar )
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
				g_ModeUIController.SetKOMsg( eAttTeam, szAttName, pDieRecord->pChar->GetLastAttackerSkillName(), pDieRecord->pChar->GetLastAttackerItemCode(), eDieTeam, szDieName );
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

					g_ModeUIController.SetKOMsg( eAttTeam, szAttName, pDieRecord->pChar->GetLastAttackerSkillName(), pDieRecord->pChar->GetLastAttackerItemCode(), eDieTeam, szDieName );
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

void ioHouseMode::UpdateKillDeathRecord( ioPlayEntity *pDieEntity, ioPlayEntity *pKiller, ioPlayEntity *pBestAttacker )
{
	ioBaseChar *pOwner = GetOwnerChar();
	if( !pOwner )
		return;

	ioBaseChar *pKillerChar = ToBaseChar( pKiller );
	ioBaseChar *pDieChar = ToBaseChar( pDieEntity );

	ioHashString szDieName, szAttName;
	if( pKillerChar )
	{
		szAttName = pKillerChar->GetCharName();
	}

	if( pDieChar )
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

				g_ModeUIController.SetKOMsg( eAttTeam, szAttName, pDieRecord->pChar->GetLastAttackerSkillName(), pDieRecord->pChar->GetLastAttackerItemCode(), eDieTeam, szDieName );
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

					g_ModeUIController.SetKOMsg( eAttTeam, szAttName, pDieRecord->pChar->GetLastAttackerSkillName(), pDieRecord->pChar->GetLastAttackerItemCode(), eDieTeam, szDieName );
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

void ioHouseMode::UpdateTimeScoreWnd()
{

}

void ioHouseMode::RenderPreGUI()
{
	if( m_ModeState == MS_PLAY )
	{
		RenderPushEnableCursor();
	}
}

void ioHouseMode::RenderAfterGUI()
{
}

D3DXVECTOR3 ioHouseMode::GetRevivalPos( ioBaseChar *pChar )
{
	D3DXVECTOR3 vPos = GetStartPos( pChar->GetTeam() );
	vPos.x += (float)( rand() % 100 ) / 20.0f;
	vPos.z += (float)( rand() % 100 ) / 20.0f;
	vPos.y = m_pCreator->GetMapHeightByRevial( vPos.x, vPos.z );

	return vPos;
}

void ioHouseMode::GetMostNearPosList( PositionDistanceSqList &rkResultList, const D3DXVECTOR3 &vTargPos )
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

void ioHouseMode::NotifyEntityDie( ioPlayEntity *pEntity, ioPlayEntity *pKiller, ioBaseChar *pBestAttacker, float fLastRate, float fBestRate )
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

void ioHouseMode::NotifyCharDropDie( ioBaseChar *pDieChar, ioBaseChar *pKillChar, ioBaseChar *pBestAttacker, float fLastRate, float fBestRate )
{
	OnNotifyCharDie( pDieChar, pKillChar, pBestAttacker, fLastRate, fBestRate );
	UpdateDropDieRecord( pDieChar, pKillChar, pBestAttacker );
}

void ioHouseMode::OnNotifyCharDie( ioBaseChar *pDieChar, ioBaseChar *pKillChar, ioBaseChar *pBestAttacker, float fLastRate, float fBestRate )
{   
	if( !pDieChar )
	{
		LOG.PrintTimeAndLog( 0, "ioHouseMode::OnNotifyCharDie() - DieChar is NULL" );
		return;
	}

	HouseRecord *pDieRecord = FindRecordCharName( pDieChar->GetCharName() );
	if( !pDieRecord )
	{
		LOG.PrintTimeAndLog( 0, "ioHouseMode::OnNotifyCharDie - %s Not Exit Char", pDieChar->GetCharName().c_str() );
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

void ioHouseMode::AddNewRecord( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	HouseRecord *pHouseRecord = FindRecordCharName( pChar->GetCharName() );
	if( !pHouseRecord )
	{
		HouseRecord kRecord;
		kRecord.pChar	= pChar;
		kRecord.szName	= pChar->GetCharName();
		m_vCharRecordList.push_back( kRecord );
		UpdateUserRank();
	}
	else
	{
		pHouseRecord->pChar = pChar;
	}

	HouseMainWnd *pMainWnd = dynamic_cast<HouseMainWnd*>( g_GUIMgr.FindWnd( HOUSE_MAIN_WND ) );
	if( pMainWnd && pMainWnd->IsShow() )
	{
		pMainWnd->UpdateInfo();
	}
}

void ioHouseMode::RemoveRecord( const ioHashString &rkName )
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
		LOG.PrintTimeAndLog( 0, "Error - ioHouseMode::RemoveRecord(%s)", rkName.c_str() );


	UpdateUserRank();

	HouseMainWnd *pMainWnd = dynamic_cast<HouseMainWnd*>( g_GUIMgr.FindWnd( HOUSE_MAIN_WND ) );
	if( pMainWnd && pMainWnd->IsShow() )
	{
		pMainWnd->UpdateInfo();
	}
}


void ioHouseMode::RemoveRecord( int nIndex )
{
	int iCharCnt = m_vCharRecordList.size();
	if( iCharCnt > nIndex )
	{
		m_vCharRecordList.erase( m_vCharRecordList.begin() + nIndex );
		UpdateUserRank();
	}
}


ioBaseChar* ioHouseMode::GetRecordChar( int i )
{
	if( COMPARE( i, 0, GetRecordCharCnt() ) )
		return m_vCharRecordList[i].pChar;

	return NULL;

}

ioBaseChar* ioHouseMode::GetRecordChar( const ioHashString &rkName )
{
	HouseRecord *pRecord = FindRecordCharName( rkName );
	if( pRecord )
		return pRecord->pChar;

	return NULL;
}

int ioHouseMode::GetRecordCharCnt() const
{
	return m_vCharRecordList.size();
}

int ioHouseMode::GetRecordPlayUserCnt()
{
	return (int)m_vCharRecordList.size();
}

ModeRecord* ioHouseMode::FindModeRecord( int iIdx )
{
	if( COMPARE( iIdx, 0, GetRecordCharCnt() ) )
		return &m_vCharRecordList[iIdx];

	return NULL;
}

ModeRecord* ioHouseMode::FindModeRecordCharName( const ioHashString &rkName )
{
	return (ModeRecord*)FindRecordCharName( rkName );
}

HouseRecord* ioHouseMode::FindRecordCharName( const ioHashString &rkName )
{
	int iCharCnt = m_vCharRecordList.size();
	for( int i=0 ; i<iCharCnt ; i++ )
	{
		if( m_vCharRecordList[i].szName == rkName )
			return &m_vCharRecordList[i];
	}

	return NULL;
}

bool ioHouseMode::GetRoundInfo( int iIndex, RoundInfo &rkInfo )
{
	if( ioPlayMode::GetRoundInfo( iIndex, rkInfo ) )
	{
		HouseRecord *pRecord = (HouseRecord*)FindModeRecord( iIndex );
		if( !pRecord )	return false;
		if( !pRecord->pChar ) return false;

		rkInfo.m_iContribute	= pRecord->Contribute.m_iContribute;
		rkInfo.m_iContributePer	= pRecord->Contribute.m_iPrevContributePer;
		return true;
	}
	return false;
}

bool ioHouseMode::ProcessUDPPacket( SP2Packet &rkPacket )
{
	if( ioPlayMode::ProcessUDPPacket( rkPacket ) )
		return true;

	return false;
}

void ioHouseMode::OnUserRevival( SP2Packet &rkPacket )
{
	DWORD dwSelectTime;
	ioHashString szName;
	rkPacket >> szName >> dwSelectTime >> m_dwRandomSeed;

	ioBaseChar *pChar = GetRecordChar( szName );
	if( !pChar )
	{
		LOG.PrintTimeAndLog( 0, "ioHouseMode::OnUserRevival - %s Not Exist User", szName.c_str() );
		return;
	}

	if( pChar->IsOwnerChar() )
	{
		LOG.PrintTimeAndLog( 0, "ioHouseMode::OnUserRevival() - CharChange: %d", FRAMEGETTIME() );
		StartChangeCharacter( pChar->GetSelectCharArray(), dwSelectTime );
	}

	D3DXVECTOR3 vRevivalPos = GetRevivalPos( pChar );
	pChar->SetWorldPosition( vRevivalPos );
	pChar->SetReadyState();
}

void ioHouseMode::OnRoundJoin( SP2Packet &rkPacket )
{
	ioHashString szJoinChar;
	int iModeState;

	rkPacket >> szJoinChar;
	rkPacket >> iModeState;

	if( szJoinChar.IsEmpty() )
		return;

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

void ioHouseMode::ApplyModeInfo( SP2Packet &rkPacket )
{
	bool bJoinLock;
	rkPacket >> m_szMasterName;
	rkPacket >> bJoinLock;
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

void ioHouseMode::ApplyCharModeInfo( const ioHashString &rkName, SP2Packet &rkPacket, bool bDieCheck )
{
	bool bRecord;
	rkPacket >> bRecord;

	if( !bRecord ) return;

	HouseRecord *pRecord = FindRecordCharName( rkName );
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

int ioHouseMode::GetRoundWinCnt( TeamType eTeamType )
{
	return 0;
}

void ioHouseMode::CheckRoundEnd()
{
}

void ioHouseMode::ContributeConvertTeam( TeamType eTeam )
{	
}

void ioHouseMode::ContributeRoundAllPercent( DWORD dwCurTime, DWORD dwTotalTime )
{
}

void ioHouseMode::ContributeConvert()
{
	ContributeConvertTeam( TEAM_NONE );
}

void ioHouseMode::DamageContribute( const ioHashString szAttacker, const ioHashString szTarget, float fDamage, DWORD dwItemCode, int iWeaponType )
{	
}

void ioHouseMode::KillContribute( const ioHashString szKiller, const ioHashString szKilled, DWORD dwItemCode )
{
}

void ioHouseMode::AttackCntContribute( const ioHashString szAttacker, const ioHashString szTarget, DWORD dwItemCode, int iAttackValue/* = 100 */ )
{
}

void ioHouseMode::ContributeLOG()
{
	char szLog[MAX_PATH];

	int iCharCnt = m_vCharRecordList.size();
	for( int i=0 ;i<iCharCnt; i++ )
	{
		HouseRecord *pRecord = &m_vCharRecordList[i];
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


void ioHouseMode::FillLastPlayRecordInfo( SP2Packet &rkPacket )
{
	int iCharCnt = m_vCharRecordList.size();
	rkPacket << iCharCnt;
	for(int i = 0;i < iCharCnt;i++)
	{
		HouseRecord *pRecord = &m_vCharRecordList[i];
		rkPacket << pRecord->pChar->GetCharName();
		rkPacket << pRecord->Contribute.m_iContribute;
		rkPacket << pRecord->GetUniqueTotalKill();
		rkPacket << pRecord->GetUniqueTotalDeath();
		rkPacket << g_VictoriesMgr.GetVictories( pRecord->pChar->GetCharName() );
	}
}

void ioHouseMode::FillPlayRecordInfo( SP2Packet &rkPacket )
{
	int iCharCnt = m_vCharRecordList.size();
	rkPacket << iCharCnt;
	for(int i = 0;i < iCharCnt;i++)
	{
		HouseRecord *pRecord = &m_vCharRecordList[i];
		rkPacket << pRecord->pChar->GetCharName();
		rkPacket << pRecord->GetUniqueTotalKill();
		rkPacket << pRecord->GetUniqueTotalDeath();
		rkPacket << pRecord->GetNoDeathKillCount();
	}
}

void ioHouseMode::ApplyPlayRecordInfo( SP2Packet &rkPacket )
{
	int iCharCnt;
	rkPacket >> iCharCnt;
	for(int i = 0;i < iCharCnt;i++)
	{
		ioHashString szName;
		int iUniqueTotalKill, iUniqueTotalDeath, iNoDeathKillCount;
		rkPacket >> szName >> iUniqueTotalKill >> iUniqueTotalDeath >> iNoDeathKillCount;

		HouseRecord *pRecord = FindRecordCharName( szName );
		if( pRecord )
		{
			pRecord->iUniqueTotalKill = iUniqueTotalKill;
			pRecord->iUniqueTotalDeath= iUniqueTotalDeath;
			pRecord->iNoDeathKillCount = iNoDeathKillCount;
		}
	}
}

void ioHouseMode::ContributeResultSync( const ioHashString &szName, int iContribute )
{
	HouseRecord *pRecord = FindRecordCharName( szName );
	if( !pRecord ) return;

	pRecord->Contribute.m_bLastResultSync = true;
	pRecord->Contribute.m_iContribute     = iContribute;
}

void ioHouseMode::GetModeSpecialInfo( SP2Packet &rkPacket )
{
	ioPlayMode::GetModeSpecialInfo( rkPacket );
}

void ioHouseMode::OnModeSpecialInfo( SP2Packet &rkPacket )
{
	ioPlayMode::OnModeSpecialInfo( rkPacket );
}

int ioHouseMode::GetCurRank( const ioHashString& szCharName )
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

int ioHouseMode::GetContributePerChar( const ioHashString& szName )
{
	int iCharCnt = m_vCharRecordList.size();

	for(int i = 0;i < iCharCnt;i++)
	{
		HouseRecord *pRecord = &m_vCharRecordList[i];
		if( !pRecord )
			continue;
		if( !pRecord->pChar )
			continue;
		__try
		{
			if( pRecord->pChar->GetCharName() == szName )
			{
				return pRecord->Contribute.m_iPrevContributePer;
			}
		}
		__except (1)
		{
			RemoveRecord( i );
			return 0;
		}
	}

	return 0;
}

void ioHouseMode::OnChangeCharNextProcess( ioBaseChar *pChar, bool bNewChar )
{
	if( !pChar ) return;
	if( pChar->IsOwnerChar() )
	{
		if( pChar->GetExperienceMode() != EMS_NONE )
			pChar->SetExperienceMode( EMS_NONE, "", 0 );
	}
}

bool ioHouseMode::ProcessTCPPacket( SP2Packet &rkPacket )
{
	if( ioPlayMode::ProcessTCPPacket( rkPacket ) )
		return true;

	switch( rkPacket.GetPacketID() )
	{
	case STPK_PERSONAL_HQ_COMMAND:
		OnHouseCommand( rkPacket );
		return true;
	}

	return false;
}

void ioHouseMode::OnHouseCommand( SP2Packet &rkPacket )
{
	int iCommand;
	rkPacket >> iCommand;
	switch( iCommand )
	{
	case PERSONAL_HQ_CMD_MAXPLAYER_CHANGE:
		{
			if( m_szMasterName == g_MyInfo.GetPublicID() )
				TCPNetwork::MouseBusy( false );

			rkPacket >> m_iMaxPlayer;
			HouseMainWnd *pMainWnd = dynamic_cast<HouseMainWnd*>(g_GUIMgr.FindWnd( HOUSE_MAIN_WND ));
			if( pMainWnd && pMainWnd->IsShow() )
			{
				pMainWnd->UpdateInfo();
			}
		}
		break;
	case PERSONAL_HQ_CMD_KICK_OUT:
		{
			ioHashString kRoomKickOutUser;
			rkPacket >> kRoomKickOutUser;
			m_pCreator->SetRoomKickOutUser( kRoomKickOutUser );
			if( kRoomKickOutUser == g_MyInfo.GetPublicID() )
			{
				int iRoomIndex;
				rkPacket >> iRoomIndex;
				g_MyInfo.SetKickOutPlaza( iRoomIndex );
				g_GUIMgr.SetReserveMsgBox( MB_HOUSE_KICK_OUT );
			}
			else
			{
				ioComplexStringPrinter kPrinter;
				g_ChatMgr.PresetChatComplexString( MSG_INFO_CHAT, kPrinter );
				kPrinter.SetTextColor( TCT_DEFAULT_VIOLET );
				kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), "%s", kRoomKickOutUser.c_str() );
				kPrinter.SetTextColor( TCT_DEFAULT_RED );
				kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), "님, 강제퇴장" );
				g_ChatMgr.SetChatComplexString( "정보", kPrinter );
			}
		}
		break;
	case PERSONAL_HQ_CMD_LEAVE_MASTER:
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
			g_GUIMgr.SetReserveMsgBox( MB_HOUSE_MASTER_OUT );
		}
		break;
	}
}