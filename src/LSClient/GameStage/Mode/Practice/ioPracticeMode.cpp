#include <stdafx.h>

#include "ioPracticeMode.h"
#include "IoPracticeManager.h"

#include "Practice_1.h"
#include "Practice_2.h"
#include "Practice_3.h"
#include "Practice_4.h"

#include "../../../ioDummyChar.h"
#include "../../../ioPushStruct.h"
#include "../../../ioBall.h"

#include "../../../ioModeUIController.h"
#include "../../../RaceSetting.h"
#include "../../../ioDummyChar.h"
#include "../../../ioNpcMgr.h"
#include "../../../ioGameAnnMgr.h"

#include "../../../GUI/MiniMap.h"
#include "../../../GUI/BriefingWnd.h"
#include "../../../GUI/GlobalWnd.h"
#include "../../../GUI/CountUpWnd.h"
#include "../../../GUI/StartingSignalWnd.h"
#include "../../../GUI/Practice/PracticeInfoWnd.h"
#include "../../../GUI/Practice/PracticeResultWnd.h"

#include "../../../ioGameAnnMgr.h"
#include "../../../SingleSetting.h"
#include "../../../Encode/ioEncrypted.h"

ioPracticeMode::ioPracticeMode( ioPlayStage *pCreator )
: ioPlayMode( pCreator )
{
	m_pPractice = NULL;
	m_ModeType = MT_PRACTICE;
	m_iPracticeIndex = 0;
	m_iResultStar = 0;
	m_dwPracticeStartTime = 0;
	m_iModeSubNum = 0;
	m_iMacroStar = 0;
	m_bEnd = false;
}

ioPracticeMode::~ioPracticeMode()
{
	m_bZeroHP = false;
	ClearMode();
}

void ioPracticeMode::InitMode( int iMapIndex )
{
	m_iModeMapIndex = iMapIndex;

	ioPlayMode::LoadMap();
	g_GUIMgr.ShowWnd( MINIMAP_WND );
	
	NetworkWnd *pNetWnd = static_cast<NetworkWnd*>(g_GUIMgr.FindWnd( NETWORK_WND ));
	if( pNetWnd )
	{
		pNetWnd->SetModeShow( GetModeType() );
	}

	//g_PracticeMgr.GetINIPath();
	m_iPracticeIndex = g_PracticeMgr.GetPreIndex();
	
	LoadMap();
	LoadPushStructs();
	LoadMachineStruct();
	LoadObjectItems();
	LoadFuntionalObjects();
	LoadCharacters();
	LoadDisableSkillList();
	LoadStartBuffList();

	
	const char *pINI = g_PracticeMgr.GetINIPath( m_iPracticeIndex );
	ioINILoader kLoader( pINI );
	//ioINILoader kLoader( GetModeINIFileName(MT_PRACTICE) );
	

	LoadRevivalTime( kLoader );
	LoadModeResource( kLoader );
	LoadModeCamera( kLoader );

	LoadGUIValues();
	LoadPushCursor();
	LoadContribute();

	if( !P2PNetwork::IsNetworkPlaying() )
	{
		SetModeState( MS_READY );
	}

	g_MyInfo.ClearCharDiedInfo();
	g_MyInfo.ClearCharDiedPenaltyInfo();
	g_MyInfo.ClearCharJoinedInfo();

	g_GUIMgr.CheckShopSound( true );

	g_MyInfo.InitPraise();
	SetMapAreaWeapon();

}

void ioPracticeMode::CreatePracticeType()
{

	ioPracticeManager::PracticeType eType = g_PracticeMgr.GetPracticeType( m_iPracticeIndex );
	int iType = 0;
	switch( eType )
	{
	case ioPracticeManager::E_PRACTICE_1:
		{
			m_pPractice = new Practice_1();
			iType = 2;
		}
		break;
	case ioPracticeManager::E_PRACTICE_2:
		{
			m_pPractice = new Practice_2();
			iType = 2;
		}
		break;
	case ioPracticeManager::E_PRACTICE_3:
		{
			m_pPractice = new Practice_3();
			iType = 2;
		}
		break;
	case ioPracticeManager::E_PRACTICE_4:
		{
			m_pPractice = new Practice_4();
			iType = 1;
		}
		break;
	}

	if( m_pPractice )
		m_pPractice->Init( this );
	PracticeInfoWnd *pInfoWnd = static_cast<PracticeInfoWnd *>( g_GUIMgr.FindWnd( PRACTICE_INFO_WND ) );
	if(pInfoWnd)
		pInfoWnd->SetPracticeInfo( iType );
}

void ioPracticeMode::ProcessMode()
{
	ProcessGUI();

	switch( m_ModeState )
	{
	case MS_READY:
		CheckModeMapSound();
		break;
	case MS_PLAY:
		ProcessMapAreaWeapon();
		ProcessPlayState();
		CheckModeMapSound();
		g_EventMgr.Process();
		break;
	case MS_RESULT:
		ProcessWaitState();
	}
}

void ioPracticeMode::ProcessPlayState()
{
	UpdateSoldierRevivalPenalty();

	m_pCreator->NotifySpringDampUseSet( true );

	if( m_dwPracticeStartTime == 0 )
		return;

	if( m_pPractice )
	{
		int iGrade = 0;
		if( m_pPractice->ProcessCheckPracticeEnd( m_dwPracticeStartTime, m_iResultStar, iGrade ) || m_bEnd )
		{
			DWORD dwEndTime = FRAMEGETTIME() - m_dwPracticeStartTime;
			ioBaseChar *pOwnerChar = GetOwnerChar();
			if( !pOwnerChar )
				return;

			if( m_iResultStar != 0 )
				pOwnerChar->SetRoundEndMotion( RM_WIN );
			else
				pOwnerChar->SetRoundEndMotion( RM_LOSE );

			SetModeState( MS_RESULT );

			//if( g_GameAnnMgr.ShowAASMessage() )
				//m_iResultStar = 0;
			//LOG.PrintTimeAndLog( 0, "결과 클라에서보냄" );
			SP2Packet kPacket( CTPK_PRACTICE_RESULT );
			if(m_bEnd)
			{
				kPacket << m_iMacroStar;
				m_iResultStar = m_iMacroStar;
				//LOG.PrintTimeAndLog( 0, "macroresult : %d",m_iMacroStar );
			}
			else
				kPacket << m_iResultStar;

			char szBuf[MAX_PATH]="";
			ioEncrypted::Encode15(std::to_string( static_cast<long long>(g_MyInfo.GetUserIndex())).c_str(), g_MyInfo.GetGUID().c_str(), szBuf, ioEncrypted::NT_KOREA);
			kPacket << szBuf;
			kPacket << dwEndTime;
			//스타트 엔드 타임 전송 추가 // 수련장
			TCPNetwork::SendToServer( kPacket );
			m_bEnd = false;
		}

		PracticeInfoWnd *pInfoWnd = static_cast<PracticeInfoWnd *>( g_GUIMgr.FindWnd( PRACTICE_INFO_WND ) );
		if( pInfoWnd )
			pInfoWnd->SetStar( m_iResultStar );
	}
}

void ioPracticeMode::ProcessWaitState()
{
}
	
void ioPracticeMode::CreatePracticeNpc( Practice::PracticeNPC PracticeNpc )
{
	ioNpcChar* pNpcChar = g_NpcMgr.CreateNpcChar( PracticeNpc.m_dwMonsterID, PracticeNpc.m_dwNPCCode, m_pCreator, this );

	if( !pNpcChar ) 
		return;

	pNpcChar->FillMaxHP();

	char szName[MAX_PATH];
	StringCbPrintf( szName, MAX_PATH, "Npc_%d", PracticeNpc.m_dwMonsterID );
	pNpcChar->SetCharName( szName );
	pNpcChar->SetNpcViewName( PracticeNpc.m_szName );

	ioBaseChar* pChar = GetOwnerChar();

	if( pChar )
		pNpcChar->SetSyncUser( pChar->GetCharName() );		
	pNpcChar->SetTeam( PracticeNpc.m_eTeamType );

	pNpcChar->SetNpcMaxHpRate( 1.f );
	pNpcChar->SetSpeedRate( 1.f );

	pNpcChar->StartAI( 0 , PracticeNpc.m_StartXPos, PracticeNpc.m_StartZPos, false, PracticeNpc.m_dwMonsterID );

	if( PracticeNpc.m_bPrisoner )
	{
		pNpcChar->SetCurHPRate( 0.f );
		pNpcChar->SetPrisonerMode();
	}

	AddNewRecord( pNpcChar );
}

void ioPracticeMode::ProcessGUI()
{
	UpdateMiniMapSpotPoint();
	UpdateEndScoreWndTime();
	UpdateSoldierWndTime();
	UpdateCountUpWnd();

	ProcessPushEnableCursor();
	ProcessBlindMode();
	ProcessDeathTimeHelp();
}

void ioPracticeMode::UpdateMiniMapSpotPoint()
{
	MiniMap *pMiniMap = static_cast<MiniMap*>(g_GUIMgr.FindWnd( MINIMAP_WND ));
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
		if( pChar->IsPrisonerMode() )
		{
			pMiniMap->AddPoint( vPos.x, vPos.z, MiniMap::SPOT_PRISON_USER, pChar->GetTeam(), dwGapTime );
		}
		else
		{
			if( pChar->IsDropZoneDownState() )
				pMiniMap->AddPoint( vPos.x, vPos.z, MiniMap::SPOT_DROP_USER, pChar->GetTeam(), 0 );
			else
				pMiniMap->AddPoint( vPos.x, vPos.z, MiniMap::SPOT_USER, pChar->GetTeam(), dwGapTime );
		}
	}

	//MyChar..
	ioBaseChar *pOwner = GetOwnerChar();
	if( pOwner && pOwner->IsCanMiniMapVisible() )
	{
		dwGapTime = pOwner->GetEventGapTime();
		vPos = pOwner->GetWorldPosition();

		if( pOwner->IsPrisonerMode() )
		{
			pMiniMap->AddPoint( vPos.x, vPos.z, MiniMap::SPOT_PRISON_ME, pOwner->GetTeam(), dwGapTime );
		}
		else
		{
			if( pOwner->IsDropZoneDownState() )
				pMiniMap->AddPoint( vPos.x, vPos.z, MiniMap::SPOT_DROP_ME, pOwner->GetTeam(), 0 );
			else
				pMiniMap->AddPoint( vPos.x, vPos.z, MiniMap::SPOT_ME, pOwner->GetTeam(), dwGapTime );
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

	//CheckPoint & Goal
	if( m_pPractice )
		m_pPractice->UpdateMiniMapSpotPoint();
}

void ioPracticeMode::RenderPreGUI()
{
	if( m_ModeState == MS_PLAY )
	{
		RenderPushEnableCursor();
	}
}

void ioPracticeMode::RetryMode()
{
	if( m_ModeState == MS_PLAY ||
		m_ModeState == MS_RESULT )
	{
		int iIndex = g_PracticeMgr.GetPreIndex();

		SP2Packet kPacket( CTPK_PRACTICE_ENTER );
		kPacket << g_PracticeMgr.GetPracticeMap( iIndex );
		kPacket << iIndex;
		TCPNetwork::SendToServer( kPacket );
	}
}

void ioPracticeMode::ClearMode()
{
	SAFEDELETE( m_pPractice );
}

void ioPracticeMode::LoadModeResource( ioINILoader &rkLoader )
{
	g_ModeUIController.SetKOMsgWnd( NULL );
	g_ModeUIController.SetCenterInfoWnd( NULL );

	g_GUIMgr.AddWnd( "XML/SkillListWnd.xml" );
	g_GUIMgr.AddWnd( "XML/countupwnd.xml" );
	g_GUIMgr.AddWnd( "XML/Practice/PracticeInfoWnd.xml" );
	g_GUIMgr.AddWnd( "XML/Practice/PracticeResultWnd.xml" );
	g_GUIMgr.AddWnd( "XML/Practice/PracticeUpdateRank.xml" );
	g_GUIMgr.AddWnd( "XML/RetryModeWnd.xml" );

	StartingSignalWnd *pSignalWnd = static_cast<StartingSignalWnd *>( g_GUIMgr.AddWnd( "XML/StartingSignalWnd.xml" ) );
	if( pSignalWnd )
		pSignalWnd->SetPlayMode( this );	

	ioPlayMode::LoadModeResource(rkLoader);
}

void ioPracticeMode::OnRoundJoin( SP2Packet &rkPacket )
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
			m_bEnterMsgShow = true;
			break;
		case MS_PLAY:
			m_dwPlayStartTime = dwStartTime;
			break;
		case MS_RESULT:
			m_dwResultStartTime = dwStartTime;
			break;
		}
		//m_dwRoundJoinTime = dwStartTime;

		if( m_ModeState != MS_RESULT )
		{
			int iSelectChar = 0;

			iSelectChar = pJoinChar->GetSelectCharArray();
			pJoinChar->SetReadyState();

			DWORD dwSelectTime;
			PACKET_GUARD_VOID_READ(rkPacket, dwSelectTime);

			m_iSelectCharacter = g_MyInfo.GetAutoSelectChar( iSelectChar, iSelectChar );

			SP2Packet kPacket( CTPK_CHANGE_CHAR );
			kPacket << m_iSelectCharacter;
			kPacket << false;
			kPacket << pJoinChar->GetCharChangeUniqueIndex() + 1;
			TCPNetwork::SendToServer( kPacket );
		}
		else
		{
			SetResultJoinState();
		}

		g_ModeHelpMgr.InitModeHelp();

		rkPacket >> m_dwCurRoundDuration;
	}
	else
	{
		pJoinChar->SetReadyState();
		if( m_ModeState == MS_PLAY )
		{
			DWORD dwCurTime = FRAMEGETTIME();
		}
	}

	m_pCreator->ReCalculateDamageRate( false );
}

void ioPracticeMode::OnRoundStart( SP2Packet &rkPacket )
{
	PACKET_GUARD_VOID_READ( rkPacket, m_iCurRoundCount );
	PACKET_GUARD_VOID_READ( rkPacket, m_iPracticeIndex );

	CreatePracticeType();

	SetModeState( MS_PLAY );

	g_PracticeMgr.SetPreIndex( m_iPracticeIndex );

	CountUpWnd *pCountUpWnd = static_cast<CountUpWnd *>( g_GUIMgr.FindWnd( COUNT_UP_WND ) );
	if( pCountUpWnd )
	{
		pCountUpWnd->SetCountDown( g_PracticeMgr.IsCountDown( m_iPracticeIndex ) );
		pCountUpWnd->UpdateProgress_Practice( -1, -1 );
		pCountUpWnd->ShowWnd();		
	}

	/*PracticeInfoWnd *pInfoWnd = static_cast<PracticeInfoWnd *>( g_GUIMgr.FindWnd( PRACTICE_INFO_WND ) );
	if( pInfoWnd )
	{
		pInfoWnd->SetPracticeInfo( m_iPracticeIndex );
	}*/
	LoadPracticeINI();
}

void ioPracticeMode::ApplyCharModeInfo( const ioHashString &rkName, SP2Packet &rkPacket, bool bDieCheck )
{
	bool bRecord;
	PACKET_GUARD_VOID_READ(rkPacket, bRecord);

	if( !bRecord ) return;

	ModeRecord *pRecord = FindModeRecordCharName( rkName );
	if( pRecord )
	{
		if( bDieCheck )
		{
			bool bDie;
			PACKET_GUARD_VOID_READ(rkPacket, bDie);

			if( bDie )
			{
				pRecord->pChar->SetDieState();
			}
		}

		bool bCatch;
		PACKET_GUARD_VOID_READ(rkPacket, bCatch);
	}
	else
	{
		if( bDieCheck )
		{
			bool bDie;
			PACKET_GUARD_VOID_READ(rkPacket, bDie);
		}

		bool bCatch;
		PACKET_GUARD_VOID_READ(rkPacket, bCatch);
	}
}

void ioPracticeMode::AddNewRecord( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	ModeRecord *pRecord = FindModeRecordCharName( pChar->GetCharName() );
	if( !pRecord )
	{
		ModeRecord kRecord;
		kRecord.pChar = pChar;
		kRecord.szName= pChar->GetCharName();

		m_vCharRecordList.push_back( kRecord );
		UpdateUserRank();
	}
	else
	{
		pRecord->pChar = pChar;
	}

}

void ioPracticeMode::RemoveRecord( const ioHashString &rkName )
{
	int iCharCnt = m_vCharRecordList.size();
	for( int i=0 ; i<iCharCnt ; ++i )
	{
		if( m_vCharRecordList[i].szName == rkName )
		{
			m_vCharRecordList.erase( m_vCharRecordList.begin() + i );
			break;
		}
	}

	UpdateUserRank();
}

void ioPracticeMode::RemoveRecord( int nIndex )
{
	int iCharCnt = m_vCharRecordList.size();
	if( iCharCnt > nIndex )
	{
		m_vCharRecordList.erase( m_vCharRecordList.begin() + nIndex );
		UpdateUserRank();
	}
}

ioBaseChar* ioPracticeMode::GetRecordChar( int i )
{
	if( COMPARE( i, 0, GetRecordCharCnt() ) )
		return m_vCharRecordList[i].pChar;

	return NULL;
}

ioBaseChar* ioPracticeMode::GetRecordCharByUserIndex( DWORD dwUserIndex )
{
	int iCharCnt = m_vCharRecordList.size();
	for( int i=0 ; i<iCharCnt ; ++i )
	{
		if( m_vCharRecordList[i].pChar && m_vCharRecordList[i].pChar->GetUserIndex() == dwUserIndex )
			return m_vCharRecordList[i].pChar;
	}
	return NULL;
}

ioBaseChar* ioPracticeMode::GetRecordChar( const ioHashString &rkName )
{
	ModeRecord *pRecord = FindModeRecordCharName( rkName );
	if( pRecord )
		return pRecord->pChar;

	return NULL;
}

int ioPracticeMode::GetRecordCharCnt() const
{
	return m_vCharRecordList.size();
}

ModeRecord* ioPracticeMode::FindModeRecord( int iIdx )
{
	if( COMPARE( iIdx, 0, GetRecordCharCnt() ) )
		return &m_vCharRecordList[iIdx];

	return NULL;
}

ModeRecord* ioPracticeMode::FindModeRecordCharName( const ioHashString &rkName )
{
	int iCharCnt = m_vCharRecordList.size();
	for( int i=0 ; i<iCharCnt ; ++i )
	{
		if( m_vCharRecordList[i].szName == rkName )
			return &m_vCharRecordList[i];
	}

	return NULL;
}

D3DXVECTOR3 ioPracticeMode::GetRevivalPos( ioBaseChar *pChar )
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

	ioNpcChar *pNpcChar = ToNpcChar(pChar);
	if( pNpcChar )
		return pNpcChar->GetStartPos();

	int iMaxRevivalPos = m_vRevivalPos.size();
	if( !COMPARE( iPosArray, 0, iMaxRevivalPos ) )
		return m_vMapCenter;

	D3DXVECTOR3 vCenter = m_vRevivalPos[iPosArray];
	vCenter.y = GetMapHeightByRevial( vCenter.x, vCenter.z );

	return vCenter;
}

void ioPracticeMode::GetMostNearPosList( PositionDistanceSqList &rkResultList, const D3DXVECTOR3 &vTargPos )
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

void ioPracticeMode::LoadPracticeINI()
{
	const char *pINI = g_PracticeMgr.GetINIPath( m_iPracticeIndex );
	ioINILoader kLoader( pINI );

	if( m_pPractice )
		m_pPractice->LoadPracticeINI( kLoader );
}

void ioPracticeMode::ProcessZoneCollision()
{
	if( m_ModeState == MS_PLAY )
	{
		if( m_pPractice )
			m_pPractice->ProcessCheckPointCollision();
	}
}

bool ioPracticeMode::ApplyWeaponWounded( ioBaseChar *pChar, ioWeapon *pWeapon )
{
	if( m_pPractice )
		return m_pPractice->ApplyWeaponWounded( pChar, pWeapon );

	return false;
}

void ioPracticeMode::CheckWeaponWounded( ioBaseChar *pChar, ioWeapon *pWeapon, float fDamage )
{
	if( m_pPractice )
		m_pPractice->CheckWeaponWounded( pChar, pWeapon, fDamage );
}

void ioPracticeMode::CheckWeaponDefense( ioBaseChar *pChar, ioWeapon *pWeapon, float fDamage )
{
	if( m_pPractice )
		m_pPractice->CheckWeaponDefense( pChar, pWeapon, fDamage );
}

void ioPracticeMode::SelectRegularSoldier()
{
	SP2Packet kPacket( CTPK_PRACTICE_SELECT_REGULAR );
	TCPNetwork::SendToServer( kPacket );
}

bool ioPracticeMode::ProcessTCPPacket( SP2Packet &rkPacket )
{
	if( ioPlayMode::ProcessTCPPacket( rkPacket ) )
		return true;

	switch( rkPacket.GetPacketID() )
	{
	case STPK_PRACTICE_SELECT_REGULAR:
		OnPracticeSelectRegular( rkPacket );
		return true;
	case STPK_PRACTICE_RESULT:
		OnPracticeResult( rkPacket );
		return true;
	case STPK_PRISONER_ESCAPE:
		OnEscapePrisonerMode( rkPacket );
		return true;
	}

	return false;
}

void ioPracticeMode::OnPracticeSelectRegular( SP2Packet &rkPacket )
{
	ioBaseChar *pChar = m_pCreator->GetOwnerChar();
	if( pChar )
	{
		BYTE byGender = 1;
		PACKET_GUARD_VOID_READ( rkPacket, byGender );

		int iClass, iUnderwear, iHair, iHairColor, iFace, iSkinColor;
		
		PACKET_GUARD_VOID_READ( rkPacket, iClass );
		PACKET_GUARD_VOID_READ( rkPacket, iUnderwear );
		PACKET_GUARD_VOID_READ( rkPacket, iHair );
		PACKET_GUARD_VOID_READ( rkPacket, iHairColor );
		PACKET_GUARD_VOID_READ( rkPacket, iFace );
		PACKET_GUARD_VOID_READ( rkPacket, iSkinColor );

		CHARACTER CharInfo;
		CharInfo.Init();

		CharInfo.m_sex			= byGender;
		CharInfo.m_class_type	= iClass;
		CharInfo.m_underwear	= iUnderwear;
		CharInfo.m_hair			= iHair;
		CharInfo.m_hair_color	= iHairColor;
		CharInfo.m_face			= iFace;
		CharInfo.m_skin_color	= iSkinColor;

		pChar->SetState( CS_DELAY );
		pChar->SetWorldPosition( GetStartPos(TEAM_BLUE) );
		pChar->DestroyAllItem();

		pChar->InitRaceInfo( CharInfo );

		ioHashString szName = g_PracticeMgr.GetRegularSoldierName( m_iPracticeIndex );
		pChar->SetViewName( szName.c_str() );
		pChar->SetCharName( szName.c_str() );
		for( int i=0; i<MAX_INVENTORY; ++i )
		{
			int iItemCode = 0;
			int iCreateIdx = 0;
			PACKET_GUARD_VOID_READ( rkPacket, iItemCode );
			PACKET_GUARD_VOID_READ( rkPacket, iCreateIdx );
			if( iItemCode == 0 || iCreateIdx == 0 )
			{
				continue;
			}
			ioItem *pItem = m_pCreator->CreateItem( iItemCode );
			if( pItem )
			{
				pItem->ReLoadProperty();
				//pItem->SetOwnerName( "테스트" );
				pItem->SetOwnerName( pChar->GetCharName() );
				pItem->SetItemCreateIndex( iCreateIdx );
				pItem->SetItemReinforce( 0 );
				pItem->SetItemCustom( 0, 0 );

				ioItem *pPreItem = pChar->EquipItem( pItem, true );
				SAFEDELETE( pPreItem );

				// VoiceType Set
				int iVoiceType = 0;
				if( i == 0 )
				{
					const ioSetItemInfo *pSetInfo = g_SetItemInfoMgr.GetSetInfoByIdx( iItemCode );
					if( pSetInfo )
						iVoiceType = pSetInfo->GetVoiceType();

					pChar->SetVoiceType( iVoiceType );
				}
			}
		}

		//m_dwPlayStartTime = FRAMEGETTIME();
	}
}

void ioPracticeMode::OnPracticeResult(  SP2Packet &rkPacket )
{
	int iResult = -1, iSize = 0;
	DWORD dwIndex, dwGrade, dwRewardIndex, dwRank = 0;
	DWORDVec vRewardList;

	PACKET_GUARD_VOID_READ( rkPacket, iResult );

	if( iResult == PRACTICE_RESULT_SUCCESS )
	{
		PACKET_GUARD_VOID_READ( rkPacket, dwIndex );
		PACKET_GUARD_VOID_READ( rkPacket, dwGrade );

		g_PracticeMgr.SetGrade( dwIndex, dwGrade );


		PACKET_GUARD_VOID_READ( rkPacket, dwRank );

		for ( int i = 0; i < iSize; ++i )
		{
			PACKET_GUARD_VOID_READ( rkPacket, dwRewardIndex );

			vRewardList.push_back( dwRewardIndex );
		}

		ioBaseChar *pOwner = GetOwnerChar();
		BriefingWnd *pBriefingWnd = static_cast<BriefingWnd*>(g_GUIMgr.FindWnd( BRIEFING_WND ));
		if( pOwner && pBriefingWnd )
		{
			ioHashString szMapEffect = pBriefingWnd->GetFinalResultMapEffect( TEAM_BLUE );
			if( !szMapEffect.IsEmpty() )
				CreateMapEffect( szMapEffect, pOwner->GetWorldPosition(), ioMath::UNIT_ALL );
		}
	}

	PracticeResultWnd *pPracticeResultWnd = static_cast<PracticeResultWnd *>( g_GUIMgr.FindWnd( PRACTICE_RESULT_WND ) );
	if( pPracticeResultWnd && !pPracticeResultWnd->IsShow() )
	{
		pPracticeResultWnd->ShowWnd();
		if( iResult == PRACTICE_RESULT_FAIL )
			pPracticeResultWnd->SetResult( m_iPracticeIndex, 0, vRewardList );
		else if( iResult == PRACTICE_RESULT_SUCCESS )
			pPracticeResultWnd->SetResult( m_iPracticeIndex, m_iResultStar, vRewardList );
	}
	if(iResult == PRACTICE_RESULT_SUCCESS)
	{
		int iCurrent = g_PracticeMgr.GetMyRankInfo().iPracticeRank;
		if(dwRank < iCurrent || iCurrent == 0)
		{
			PracticeUpdateRank *pPracticeResultWnd = static_cast<PracticeUpdateRank *>( g_GUIMgr.FindWnd( PRACTICE_UPDATE_RESULT_WND ) );
			if(pPracticeResultWnd)
			{
				pPracticeResultWnd->SetUpdateRank(iCurrent, dwRank);
				pPracticeResultWnd->ShowWnd();
			}
		}
	}

	if( iResult == PRACTICE_RESULT_ADDICTION )
	{
		//g_GameAnnMgr.ShowAASMessage();
	}
}

void ioPracticeMode::OnEscapePrisonerMode( SP2Packet &rkPacket )
{
	ioHashString szPrisonerName, szLastAttackerSkill;	
	DWORD dwTargetIndex;
	PACKET_GUARD_VOID_READ(rkPacket, szPrisonerName);
	PACKET_GUARD_VOID_READ(rkPacket, dwTargetIndex);
	PACKET_GUARD_VOID_READ(rkPacket, szLastAttackerSkill);

	//ioBaseChar* pChar = FindModeRecordCharName( szPrisonerName );

	ioBaseChar* pChar = NULL;

	int iCharCnt = m_vCharRecordList.size();
	for( int i=0 ; i<iCharCnt ; ++i )
	{
		if( m_vCharRecordList[i].szName == szPrisonerName )
			pChar = m_vCharRecordList[i].pChar;
	}

	if( !pChar ) return;

	EscapePrisonerChar( pChar );
}

void ioPracticeMode::EscapePrisonerChar( ioBaseChar *pChar )
{
	TeamType eLoseTeam = TEAM_NONE;
	if( m_iRedRoundWinCnt < m_iBlueRoundWinCnt )
		eLoseTeam = TEAM_RED;
	else if( m_iBlueRoundWinCnt < m_iRedRoundWinCnt )
		eLoseTeam = TEAM_BLUE;

	pChar->EscapePrisonerMode( eLoseTeam, true, m_bZeroHP );

	if( m_pPractice )
		m_pPractice->EscapePrisonerChar( pChar );
}

void ioPracticeMode::UpdateCountUpWnd()
{
	if( GetModeState() != MS_PLAY )
		return;

	if( m_dwPracticeStartTime == 0 )
		return;

	CountUpWnd *pCountUpWnd = static_cast< CountUpWnd * >( g_GUIMgr.FindWnd( COUNT_UP_WND ) );
	if( !pCountUpWnd ) return;
	DWORD dwTimeGap = FRAMEGETTIME() - m_dwPracticeStartTime;
	DWORD dwLimitTime = g_PracticeMgr.GetLimitTime( m_iPracticeIndex ) * 1000;

	dwTimeGap = min( dwTimeGap, dwLimitTime );

	if( g_PracticeMgr.IsCountDown( m_iPracticeIndex ) )
		pCountUpWnd->UpdateTime( dwLimitTime - dwTimeGap );
	else
		pCountUpWnd->UpdateTime( dwTimeGap );

	int iProgress = -1;
	int iMaxProgress = -1;

	if( m_pPractice )
		m_pPractice->PracticeProgress( iProgress, iMaxProgress );

	pCountUpWnd->UpdateProgress_Practice( iProgress, iMaxProgress );
}

void ioPracticeMode::PracticeEndByDie()
{
	m_iResultStar = 0;

	ioBaseChar *pOwnerChar = GetOwnerChar();
	if( !pOwnerChar )
		return;

	pOwnerChar->SetRoundEndMotion( RM_LOSE );
	SetModeState( MS_RESULT );

	PracticeInfoWnd *pInfoWnd = static_cast<PracticeInfoWnd *>( g_GUIMgr.FindWnd( PRACTICE_INFO_WND ) );
	if( pInfoWnd )
		pInfoWnd->SetStar(m_iResultStar );

	PracticeResultWnd *pPracticeResultWnd = static_cast<PracticeResultWnd *>( g_GUIMgr.FindWnd( PRACTICE_RESULT_WND ) );
	if( pPracticeResultWnd && !pPracticeResultWnd->IsShow() )
	{
		DWORDVec vRewardList;
		vRewardList.clear();
		pPracticeResultWnd->ShowWnd();
		pPracticeResultWnd->SetResult( m_iPracticeIndex, m_iResultStar, vRewardList );
	}
}

float ioPracticeMode::GetExtraRecoveryGaugeRate()
{
	if( m_pPractice )
		return m_pPractice->GetExtraRecoveryGaugeRate();

	return 1.f;
}

bool ioPracticeMode::IsAllDie( TeamType eTeam )
{
	int iCharCnt = m_vCharRecordList.size();
	for( int i=0 ; i<iCharCnt ; ++i )
	{
		if( !m_vCharRecordList[i].pChar ) continue;
		if( m_vCharRecordList[i].pChar->GetTeam() != eTeam ) continue;
		if( m_vCharRecordList[i].pChar->GetState() != CS_DIE )
			return false;
	}
	return true;
}

void ioPracticeMode::ReadyPractice()
{
	StartingSignalWnd *pSignalWnd = static_cast<StartingSignalWnd *>( g_GUIMgr.FindWnd( STARTING_SIGNAL_WND ) );
	
	if( pSignalWnd )
	{ 
		pSignalWnd->SetStart();
		pSignalWnd->ShowWnd();	

		m_pCreator->SetCameraModeFixedBlock( true );
		m_pCreator->SetScreenBlindDownState();		
	}
}

void ioPracticeMode::StartPractice()
{
	m_dwPracticeStartTime = FRAMEGETTIME();


	RetryModeWnd *pRetryWnd = static_cast<RetryModeWnd *>( g_GUIMgr.FindWnd(RETRY_MODE_WND) );
	if( pRetryWnd )
		pRetryWnd->SetPlayMode( this );

	m_pCreator->SetCameraModeFixedBlock( false );
}

int ioPracticeMode::GetTeamPlayerCount( TeamType eTeam, int &iDieCount )
{
	int iUserCnt = 0;
	int iDieCnt = 0;

	int iSize = GetRecordCharCnt();
	for(int i = 0;i < iSize;i++)
	{
		ioBaseChar *pChar = GetRecordChar( i );
		if( !pChar ) continue;
		if( pChar->GetState() == CS_VIEW ) continue;
		if( pChar->GetState() == CS_LOADING ) continue;
		if( pChar->GetState() == CS_OBSERVER ) continue;

		if( pChar->GetTeam() == eTeam )
		{
			iUserCnt++;

			if( pChar->GetState() == CS_DIE )
			{
				++iDieCnt;
			}
		}
	}

	iDieCount = iDieCnt;

	return iUserCnt;
}

void ioPracticeMode::DamageContribute( const ioHashString szAttacker, const ioHashString szTarget, float fDamage, DWORD dwItemCode, int iWeaponType /*= -1 */ )
{

}

void ioPracticeMode::LoadMap()
{
	LoadModeINIPATH();

	ioINILoader kLoader( GetModeMapINIFileName( m_ModeType, m_iModeSubNum, m_iModeMapIndex ) );

	m_pCreator->LoadMap( kLoader );
	m_vMapCenter = m_pCreator->m_vMapCenter;

	kLoader.SetTitle( "drop_damage" );
	m_fDropDamageMapRate = kLoader.LoadFloat( "drop_damage_rate", 1.0f );

	
	// 시작 포지션 세팅
	kLoader.SetTitle_e( "single_start" );
	

	int iMaxStartPos = kLoader.LoadFloat_e( "max_start_pos", 0 );
	m_vStartPos.clear();
	m_vStartPos.reserve( iMaxStartPos );
	for( int i=0 ; i<iMaxStartPos; i++ )
	{
		char szStartX[MAX_PATH] = {0,};
		char szStartZ[MAX_PATH] = {0,};
		wsprintf_e(szStartX, "start%d_x", i+1);
		wsprintf_e(szStartZ, "start%d_z", i+1);

		D3DXVECTOR3 vStartPos;
		vStartPos.x = kLoader.LoadFloat( szStartX, 0.0f );
		vStartPos.y	= 0.0f;
		vStartPos.z = kLoader.LoadFloat( szStartZ, 0.0f );
		m_vStartPos.push_back(vStartPos);
	}

	// 부활 포지션 세팅
	kLoader.SetTitle_e( "single_revival" );


	
	int iMaxRevivalPos = kLoader.LoadFloat_e( "max_revival_pos", 0 );
	m_vRevivalPos.clear();
	m_vRevivalPos.reserve( iMaxRevivalPos );

	for( int i=0 ; i<iMaxRevivalPos; i++ )
	{
		char szRevivalX[MAX_PATH] = {0,};
		char szRevivalZ[MAX_PATH] = {0,};
		wsprintf_e(szRevivalX, "revival%d_x", i+1);
		wsprintf_e(szRevivalZ, "revival%d_z", i+1);

		D3DXVECTOR3 vRevivalPos;
		vRevivalPos.x = kLoader.LoadFloat( szRevivalX, 0.0f );
		vRevivalPos.y = 0.0f;
		vRevivalPos.z = kLoader.LoadFloat( szRevivalZ, 0.0f );
		m_vRevivalPos.push_back(vRevivalPos);
	}
	

	kLoader.SetTitle_e( "minimap" );

	MiniMap *pMiniMap = static_cast<MiniMap*>(g_GUIMgr.FindWnd( MINIMAP_WND ));
	if( pMiniMap )
	{
		pMiniMap->InitMapInfo( kLoader, m_pCreator->GetWorldMapBox(), this );
	}

	// PassiveBuff List
	char szBuf[MAX_PATH], szKey[MAX_PATH];
	kLoader.SetTitle_e( "map_passive_buff" );

	m_vMapPassiveBuffList.clear();

	int iBuffCnt = kLoader.LoadInt_e( "passive_buff_cnt", 0 );
	if( iBuffCnt > 0 )
	{
		m_vMapPassiveBuffList.reserve( iBuffCnt );

		for( int i=0; i < iBuffCnt; ++i )
		{
			wsprintf_e( szKey, "passive_buff%d", i+1 );

			kLoader.LoadString( szKey, "", szBuf, MAX_PATH );

			MapBuffInfo info;
			info.m_szBuffName = szBuf;
			m_vMapPassiveBuffList.push_back( info );
		}
	}

	kLoader.SetTitle_e( "event_scene" );
	m_dwMapCameraSceneID = kLoader.LoadInt_e( "camera_event_id", 0 );

	LoadMapAreaWeapon( kLoader );

	kLoader.SetTitle_e( "collision" );
	m_bMonsterVsMonsterCollision = kLoader.LoadBool_e( "monster_vs_monster", true );

	//g_ResultStageHelper.SetModeType( m_ModeType );
	//g_ResultStageHelper.SetModeMapIndex( g_App.GetCurModeMapIndex() );
}

void ioPracticeMode::LoadDisableSkillList()
{
	const char *pINI = g_PracticeMgr.GetINIPath( m_iPracticeIndex );
	ioINILoader kLoader( pINI );

	char szKey[MAX_PATH] = "";
	char szBuf[MAX_PATH] = "";

	m_DisableSkillList.clear();

	kLoader.SetTitle_e( "disable_skill" );
	int iSkillCnt = kLoader.LoadInt_e( "disable_skill_cnt", 0 );

	_ENCSTR("disable_skill", szTmp);
	for( int i=0; i < iSkillCnt; ++i )
	{
		wsprintf_e( szKey, "%s%d", szTmp, i+1 );
		kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_DisableSkillList.push_back( szBuf );
	}
}

void ioPracticeMode::LoadStartBuffList()
{
	const char *pINI = g_PracticeMgr.GetINIPath( m_iPracticeIndex );
	ioINILoader kLoader( pINI );

	char szKey[MAX_PATH] = "";
	char szBuf[MAX_PATH] = "";

	// Start Buff List
	m_vStartBuffList.clear();

	kLoader.SetTitle_e( "start_buff" );
	int iBuffCnt = kLoader.LoadInt_e( "start_buff_cnt", 0 );
	if( iBuffCnt > 0 )
	{
		m_vStartBuffList.reserve( iBuffCnt );

		for( int i=0; i < iBuffCnt; ++i )
		{
			wsprintf_e( szKey, "start_buff%d", i+1 );

			kLoader.LoadString( szKey, "", szBuf, MAX_PATH );

			ioHashString szTmp = szBuf;
			m_vStartBuffList.push_back( szTmp );
		}
	}
}

void ioPracticeMode::SetMacro( DWORD dwResult )
{
	m_bEnd = true;
	m_iMacroStar = dwResult;
}

void ioPracticeMode::SetMacroState()
{
	SetModeState(MS_MACRO);
}

void ioPracticeMode::SetResultState()
{
	SetModeState(MS_RESULT);
}