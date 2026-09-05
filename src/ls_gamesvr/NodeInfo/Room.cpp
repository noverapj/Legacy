// Room.cpp: implementation of the Room class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "../Window.h"
#include "../MainProcess.h"
#include "Room.h"
#include "RoomNodeManager.h"
#include "../EtcHelpFunc.h"
#include "../MainServerNode/MainServerNode.h"

#include "Mode.h"
#include "CatchMode.h"
#include "UnderwearMode.h"
#include "CBTMode.h"
#include "HiddenkingMode.h"
#include "TrainingMode.h"
#include "HeroMatchMode.h"
#include "HeadquartersMode.h"
#include "GangsiMode.h"
#include "FightClubMode.h"
#include "DoubleCrownMode.h"
#include "SuccessionMode.h"
#include "ModeCreator.h"
#include "ioItemInfoManager.h"
#include "LevelMatchManager.h"
#include "ModeSelectManager.h"
#include "ServerNodeManager.h"
#include "LadderTeamManager.h"
#include "ShuffleRoomManager.h"
#include "BattleRoomNode.h"
#include "ioMyLevelMgr.h"
#include "ioEtcItemManager.h"
#include "ioExcavationManager.h"
#include "GuildRoomsBlockManager.h"
#include "HouseMode.h"
#include "HomeModeBlockManager.h"
#include "MatchManager.h"
#include "FlagMode.h" // 2018-03-15 by bckim, 깃발모드 추가
#include "ArenaMode.h"	// 2018-07-25 by bckim, 아레나 모드 추가
#include "BattleMode.h"	// 2019-02-14 by bckim, 배틀 모드 추가
#include "FarmingMode.h"	// 2019-07-03 by bckim, 파밍모드 추가



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
RoomSync::RoomSync()
{
	m_pCreator     = NULL;
	m_dwUpdateTime = 0;
	m_dwUpdateType = 0;
	// 업데이트 시간
	m_dwCheckTime[RS_MODE]		= 6000;
	m_dwCheckTime[RS_CURUSER]	= 5000;
	m_dwCheckTime[RS_PLAZAINFO] = 4000;
	m_dwCheckTime[RS_CREATE]	= 2000;
	m_dwCheckTime[RS_DESTROY]	= 0;
}

RoomSync::~RoomSync()
{

}

void RoomSync::SetCreator( Room *pCreator )
{
	m_dwUpdateTime = 0;
	m_dwUpdateType = 0;
	m_pCreator = pCreator;
}

void RoomSync::Update( DWORD dwUpdateType )
{
	if( !COMPARE( dwUpdateType, RS_MODE, MAX_RS ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "RoomSync::Update 알수 없는 업데이트 값 : %d", dwUpdateType );
		return;
	}
	if( !m_pCreator )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "RoomSync::Update m_pCreator == NULL" );
		return;
	}

	if( dwUpdateType < m_dwUpdateType )
		return;

	m_dwUpdateType = dwUpdateType;
	m_dwUpdateTime = TIMEGETTIME();
	Process();
}

void RoomSync::Process()
{
	if( m_dwUpdateTime == 0 ) return;
	if( !COMPARE( m_dwUpdateType, RS_MODE, MAX_RS ) ) return;
	if( m_pCreator->IsRoomEnterUserDelay() ) return;

	DWORD dwGap = TIMEGETTIME() - m_dwUpdateTime;
	if( dwGap >= m_dwCheckTime[m_dwUpdateType] )
	{
		switch( m_dwUpdateType )
		{
		case RS_MODE:
			m_pCreator->SyncMode();
			break;
		case RS_CURUSER:
			m_pCreator->SyncCurUser();
			break;
		case RS_PLAZAINFO:
			m_pCreator->SyncPlazaInfo();
			break;
		case RS_CREATE:
			m_pCreator->SyncCreate();
			break;
		case RS_DESTROY:
			m_pCreator->SyncDestroy();
			break;
		}
		m_dwUpdateType = m_dwUpdateTime = 0;
	}
}
//////////////////////////////////////////////////////////////////////////
Room::Room( int iIndex ) : m_iRoomIndex( iIndex )
{
	m_vUserNode.reserve( MAX_PLAZA_PLAYER );

	m_SyncUpdate.SetCreator( this );
	m_iRelayServerIndex = 0; //for relay
	m_pMode = NULL;
	m_pItemMaker = NULL;
	m_room_style = RSTYLE_NONE;
	m_bRoomProcess = true;
	m_bPartyProcessEnd = false;
	m_bSafetyLevelRoom = false;
	m_bBoradcastRoom   = false;
	m_bTournamentRoom  = false;
	m_dwTournamentIndex= 0;
	m_bTeamSequence = true;
	m_bCharChangeToUDP = false;
	m_bOnlyServerRelay = false;
	m_iSubState	= 0;
	m_ePlazaType = PT_NONE;
	m_pModeSelector = NULL;
	
	m_MainLadderTeam.Init();
	m_SubLadderTeam.Init();
}

Room::~Room()
{
	DestroyMode();
	SAFEDELETE(m_pItemMaker);
	SAFEDELETE(m_pModeSelector);
	m_vRoomEnterTeamRate.clear();
}

void Room::OnCreate()
{
	m_iRelayServerIndex = 0; //for relay
	m_iRelayServerPort = 0;
	m_HostUserID.Clear();
	m_szMasterUserID.Clear();
	m_szRoomName.Clear();
	m_szRoomPW.Clear();
	m_iRoomNum = -1;
	m_bRoomProcess = true;
	m_pItemMaker = g_ItemInfoMgr.CreateItemMaker();
	m_bSafetyLevelRoom = false;
	m_bBoradcastRoom   = false;
	m_bTournamentRoom  = false;
	m_dwTournamentIndex= 0;
	m_bPartyProcessEnd = false;
	m_iSubState = 0;
	m_ePlazaType   = PT_NONE;
	m_MainLadderTeam.Init();
	m_SubLadderTeam.Init();
	m_vReserveUser.clear();
	LoadMatchInfo();
	m_bCharChangeToUDP = Help::IsCharChangeToUDP();
	m_bOnlyServerRelay = Help::IsOnlyServerRelay();
	m_SyncUpdate.Update( RoomSync::RS_CREATE );
	m_dwCreateTime = TIMEGETTIME();
	m_dwRoomPlayTime = TIMEGETTIME();
}

void Room::OnDestroy()
{
	BlockNode::Reset();

	g_Relay.RemoveRelayGroupReserve( this );
	m_room_style = RSTYLE_NONE;
	m_vReserveUser.clear();
	m_vRoomEnterTeamRate.clear();
	DestroyMode();
	SAFEDELETE(m_pItemMaker);
	m_SyncUpdate.Update( RoomSync::RS_DESTROY );
	m_iRelayServerIndex = 0;
}

void Room::SyncMode()
{
}

void Room::SyncCurUser()
{
	SP2Packet kPacket( SSTPK_ROOM_SYNC );
	kPacket << (int)RoomSync::RS_CURUSER << GetRoomIndex();
	kPacket << (int)GetModeType() << GetModeSubNum()  << GetModeMapNum() << GetJoinUserCnt() << GetPlayUserCnt() << GetMaxPlayer();
	kPacket << GetAverageLevel() << GetTeamRatePoint();
	g_ServerNodeManager.SendMessageToPartitions( kPacket );
}

void Room::SyncPlazaInfo()
{
	SP2Packet kPacket( SSTPK_ROOM_SYNC );
	kPacket << (int)RoomSync::RS_PLAZAINFO << GetRoomIndex();
	kPacket << (int)GetModeType() << GetModeSubNum()  << GetModeMapNum() << GetJoinUserCnt() << GetPlayUserCnt() << GetMaxPlayer();
	kPacket << GetAverageLevel() << GetTeamRatePoint();
	kPacket << GetRoomNumber() << GetMasterLevel() << GetRoomName() << GetMasterName() << GetRoomPW() << GetSubState();
	g_ServerNodeManager.SendMessageToPartitions( kPacket );
}

void Room::SyncCreate()
{
	SP2Packet kPacket( SSTPK_ROOM_SYNC );
	kPacket << (int)RoomSync::RS_CREATE;
	FillSyncCreate( kPacket );
	g_ServerNodeManager.SendMessageToPartitions( kPacket );
}

void Room::FillSyncCreate( SP2Packet &rkPacket )
{
	rkPacket << GetRoomIndex();
	rkPacket << (int)GetRoomStyle() << IsSafetyLevelRoom() << (int)GetModeType() << GetModeSubNum() << GetModeMapNum();
	rkPacket << GetJoinUserCnt() << GetPlayUserCnt() << GetMaxPlayer();
	rkPacket << GetAverageLevel() << GetTeamRatePoint();
	if( GetModeType() == MT_TRAINING )
	{		
		rkPacket << GetRoomNumber() << GetMasterLevel() << GetRoomName() << GetMasterName() << GetRoomPW();
		rkPacket << (int)GetPlazaModeType();
	}		
}

void Room::SyncDestroy()
{
	SP2Packet kPacket( SSTPK_ROOM_SYNC );
	kPacket << (int)RoomSync::RS_DESTROY << m_iRoomIndex;
	g_ServerNodeManager.SendMessageToPartitions( kPacket );
}

void Room::LoadMatchInfo()
{
	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	// 팀원 비율에 따른 
	ioINILoader kLoader( "config/sp2_level_match.ini" );
	kLoader.SetTitle( "team_info" );

	int iMaxTeamRate = kLoader.LoadInt( "max_team_rate", 0 );
	m_vRoomEnterTeamRate.clear();
	m_vRoomEnterTeamRate.reserve( iMaxTeamRate );

	for( int i=0 ;i<iMaxTeamRate; i++ )
	{
		EnterTeamRate etr;
		sprintf_s( szKey, "team_rate%d_point", i+1 );
		etr.iRatePoint = kLoader.LoadInt( szKey, 0 );
		sprintf_s( szKey, "team_rate%d", i+1 );
		kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		etr.szRate = szBuf;
		m_vRoomEnterTeamRate.push_back( etr );
	}
}

void Room::SetRoomNum( int iRoomNum )
{
	if( iRoomNum != -1 )
		m_iRoomNum = iRoomNum;
}

void Room::SetRoomName( const ioHashString &szName )
{
	m_szRoomName = szName;
	m_SyncUpdate.Update( RoomSync::RS_PLAZAINFO );
}

void Room::SetRoomPW( const ioHashString &szPW )
{
	m_szRoomPW = szPW;
	m_SyncUpdate.Update( RoomSync::RS_PLAZAINFO );
}

void Room::SetRoomMasterID( const ioHashString &szMasterID )
{
	m_szMasterUserID = szMasterID;
	m_SyncUpdate.Update( RoomSync::RS_PLAZAINFO );
}

void Room::SetPlazaModeType( PlazaType ePlazaType )
{
	m_ePlazaType = ePlazaType;
}

void Room::SetSubState(bool bNpc)
{
	if( bNpc )
		m_iSubState = SortPlazaRoom::PRS_SUB_NPC_EVENT;
	else
		m_iSubState = SortPlazaRoom::PRS_SUB_NONE;

	m_SyncUpdate.Update( RoomSync::RS_PLAZAINFO );
}

int Room::GetMasterLevel()
{
	vUser_iter iter, iEnd;
	iEnd = m_vUserNode.end();
	for( iter=m_vUserNode.begin() ; iter!=iEnd ; ++iter )
	{
		User *pMaster = *iter;
		CRASH_GUARD();
		if( pMaster->GetPublicID() == m_szMasterUserID )
			return pMaster->GetGradeLevel();
	}
	return 0;
}

int Room::GetPlazaRoomLevel()
{
	int iMatchLevel = GetAverageLevel() - g_LevelMatchMgr.GetAddGradeLevel();

	return min( max( iMatchLevel, 0 ), g_LevelMgr.GetMaxGradeLevel() );
}

bool Room::IsOpenPlazaRoom()
{
	if( GetModeType() != MT_TRAINING ) return false;
	
	return ( !IsRoomMasterID() );
}

bool Room::RoomKickOut( const ioHashString &szKickOutUser )
{
	if( GetModeType() != MT_TRAINING && GetModeType() != MT_HEADQUARTERS && GetModeType() != MT_HOUSE ) return false;

	CRASH_GUARD();
	User *pKickOutUser = NULL;
	vUser_iter iter, iEnd;
	iEnd = m_vUserNode.end();
	for( iter=m_vUserNode.begin() ; iter!=iEnd ; ++iter )
	{
		User *pUser = *iter;
		if( pUser->GetPublicID() == szKickOutUser )
		{
			pKickOutUser = pUser;
			break;
		}
	}

	if( pKickOutUser )
	{
		if( GetModeType() == MT_TRAINING )
		{
			SP2Packet kPacket( STPK_PLAZA_COMMAND );
			kPacket << PLAZA_CMD_KICK_OUT << pKickOutUser->GetPublicID();
			RoomSendPacketTcp( kPacket, pKickOutUser );

			SP2Packet kPacket1( STPK_PLAZA_COMMAND );
			kPacket1 << PLAZA_CMD_KICK_OUT << pKickOutUser->GetPublicID() << GetRoomIndex();
			pKickOutUser->SendMessage( kPacket1 );
		}
		else if( GetModeType() == MT_HEADQUARTERS )
		{
			SP2Packet kPacket( STPK_HEADQUARTERS_COMMAND );
			kPacket << HEADQUARTERS_CMD_KICK_OUT << pKickOutUser->GetPublicID();
			RoomSendPacketTcp( kPacket, pKickOutUser );

			SP2Packet kPacket1( STPK_HEADQUARTERS_COMMAND );
			kPacket1 << HEADQUARTERS_CMD_KICK_OUT << pKickOutUser->GetPublicID() << GetRoomIndex();
			pKickOutUser->SendMessage( kPacket1 );
		}
		else
		{
			SP2Packet kPacket( STPK_PERSONAL_HQ_COMMAND );
			PACKET_GUARD_bool( kPacket.Write(PERSONAL_HQ_CMD_KICK_OUT) );
			PACKET_GUARD_bool( kPacket.Write(pKickOutUser->GetPublicID()) );
			RoomSendPacketTcp( kPacket, pKickOutUser );

			SP2Packet kPacket1( STPK_HEADQUARTERS_COMMAND );
			PACKET_GUARD_bool( kPacket1.Write(PERSONAL_HQ_CMD_KICK_OUT) );
			PACKET_GUARD_bool( kPacket1.Write(pKickOutUser->GetPublicID()) );
			PACKET_GUARD_bool( kPacket1.Write(GetRoomIndex()) );
			pKickOutUser->SendMessage( kPacket1 );
		}

		//로비로 나감
		pKickOutUser->ExitRoomToTraining( EXIT_ROOM_LOBBY, false );
		return true;
	}
	return false;
}

void Room::SetSafetyRoom( bool bSafetyRoom )
{
	m_bSafetyLevelRoom = bSafetyRoom;
}

void Room::SetBroadcastRoom( bool bBoradcastRoom )
{
	m_bBoradcastRoom = bBoradcastRoom;
}

void Room::SetTournamentRoom( bool bTournamentRoom, DWORD dwTourIndex )
{
	m_bTournamentRoom	= bTournamentRoom;
	m_dwTournamentIndex = dwTourIndex;
}

void Room::SetModeType( ModeType eMode, int iRoundType, int iRoundTimeType )
{
	DestroyMode();

	if( GetRoomStyle() == RSTYLE_MATCHROOM )
	{
		m_pMode = ModeCreator::CreateMode( this, eMode );
		m_pModeSelector->MatchingSelectMapNum( eMode );
		int iSubNum = m_pModeSelector->GetCurSubModeType();
		int iMapIndex = m_pModeSelector->GetCurMapNum();

		m_pMode->SetModeSubNum( iSubNum );
		m_pMode->SetModeMapNum( iMapIndex );
		m_pMode->SetRoundType( iRoundType, iRoundTimeType );
		m_pMode->InitMode();

		m_SyncUpdate.Update( RoomSync::RS_MODE );
		return;
	}

	if( !m_pModeSelector )
	{
		m_pMode = ModeCreator::CreateMode( this, MT_TRAINING );

		if( GetPlazaModeType() == PT_COMMUNITY )
			m_pMode->SetModeSubNum( 2 );
		else
			m_pMode->SetModeSubNum( 1 );

		m_pMode->SetModeMapNum( 0 );
		m_pMode->InitMode();
	}
	else if( eMode == MT_NONE )
	{
		m_pMode = ModeCreator::CreateMode( this, MT_TRAINING );

		int iSubNum = m_pModeSelector->GetCurSubModeType();
		int iMapIndex = m_pModeSelector->GetCurMapNum();

		m_pMode->SetModeSubNum( iSubNum );
		m_pMode->SetModeMapNum( iMapIndex );
		m_pMode->InitMode();
	}
	else
	{
		m_pMode = ModeCreator::CreateMode( this, eMode );
		int iSubNum = m_pModeSelector->GetCurSubModeType();
		int iMapIndex = m_pModeSelector->GetCurMapNum();

		m_pMode->SetModeSubNum( iSubNum );
		m_pMode->SetModeMapNum( iMapIndex );
		m_pMode->SetRoundType( iRoundType, iRoundTimeType );
		m_pMode->InitMode();
	}	
	m_SyncUpdate.Update( RoomSync::RS_MODE );
}

void Room::SetModeType( ModeType eMode, int nMapID, int iRoundType, int iRoundTimeType )
{
	DestroyMode();

	m_pMode = ModeCreator::CreateMode( this, eMode );
	m_pMode->SetModeSubNum( 1 );
	m_pMode->SetModeMapNum(nMapID);
	m_pMode->SetRoundType( iRoundType, iRoundTimeType );
	m_pMode->InitMode();

	m_SyncUpdate.Update( RoomSync::RS_MODE );

}

void Room::SetShuffleModeType( int iMode, int iSubNum, int iMapNum )
{
	DestroyMode();

	m_pMode = ModeCreator::CreateMode( this, static_cast<ModeType>(iMode) );

	m_pMode->SetModeSubNum( iSubNum );
	m_pMode->SetModeMapNum( iMapNum );
	m_pMode->SetRoundType( -1, -1 );
	m_pMode->InitMode();

	m_SyncUpdate.Update( RoomSync::RS_MODE );
}

void Room::CheckUseFightNPC()
{
	if( m_pMode && m_pMode->GetModeType() == MT_FIGHT_CLUB )
	{
		FightClubMode *pFightMode = ToFightClubMode( m_pMode );
		if( pFightMode )
		{
			pFightMode->SetUseFightNPC( true );
		}
	}
}

int Room::GetLadderTeamCampTypeByTeamIndex(const int iIndex)
{
	if( iIndex == m_MainLadderTeam.m_dwTeamIndex )
		return m_MainLadderTeam.m_iCampType;

	return m_SubLadderTeam.m_iCampType;
}

int Room::GetLadderTeamTeamTypeByTeamIndex(const int iIndex)
{
	if( iIndex == m_MainLadderTeam.m_dwTeamIndex )
		return m_MainLadderTeam.m_eTeamType;

	return m_SubLadderTeam.m_eTeamType;
}

void Room::SetLadderTeam( DWORD dwMainTeam, DWORD dwSubTeam )
{
	LadderTeamParent *pLadderTeam = NULL;

	m_MainLadderTeam.Init();
	m_MainLadderTeam.m_dwTeamIndex = dwMainTeam;
	pLadderTeam = g_LadderTeamManager.GetGlobalLadderTeamNode( m_MainLadderTeam.m_dwTeamIndex );
	if( pLadderTeam )
	{
		m_MainLadderTeam.m_iCampType          = pLadderTeam->GetCampType();
		m_MainLadderTeam.m_iAbilityMatchLevel = pLadderTeam->GetAbilityMatchLevel();
		m_MainLadderTeam.m_szTeamName		  = pLadderTeam->GetTeamName();
		m_MainLadderTeam.m_dwGuildIndex		  = pLadderTeam->GetGuildIndex();
		m_MainLadderTeam.m_iPrevTeamRank	  = pLadderTeam->GetTeamRanking();
		m_MainLadderTeam.m_iWinRecord		  = pLadderTeam->GetWinRecord();
		m_MainLadderTeam.m_iLoseRecord		  = pLadderTeam->GetLoseRecord();
		m_MainLadderTeam.m_iVictoriesRecord   = pLadderTeam->GetVictoriesRecord();
		if( m_MainLadderTeam.m_iCampType == CAMP_BLUE )
			m_MainLadderTeam.m_eTeamType = TEAM_BLUE;
		else
			m_MainLadderTeam.m_eTeamType = TEAM_RED;
	}
	else
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Matching Ladder Team Main Team None: %d", m_MainLadderTeam.m_dwTeamIndex );
	}

	m_SubLadderTeam.Init();
	m_SubLadderTeam.m_dwTeamIndex  = dwSubTeam;
	pLadderTeam = g_LadderTeamManager.GetGlobalLadderTeamNode( m_SubLadderTeam.m_dwTeamIndex );
	if( pLadderTeam )
	{
		m_SubLadderTeam.m_iCampType          = pLadderTeam->GetCampType();
		m_SubLadderTeam.m_iAbilityMatchLevel = pLadderTeam->GetAbilityMatchLevel();
		m_SubLadderTeam.m_szTeamName	     = pLadderTeam->GetTeamName();
		m_SubLadderTeam.m_dwGuildIndex	     = pLadderTeam->GetGuildIndex();
		m_SubLadderTeam.m_iPrevTeamRank	     = pLadderTeam->GetTeamRanking();
		m_SubLadderTeam.m_iWinRecord	     = pLadderTeam->GetWinRecord();
		m_SubLadderTeam.m_iLoseRecord	     = pLadderTeam->GetLoseRecord();
		m_SubLadderTeam.m_iVictoriesRecord   = pLadderTeam->GetVictoriesRecord();
		if( m_SubLadderTeam.m_iCampType == CAMP_BLUE )
			m_SubLadderTeam.m_eTeamType = TEAM_BLUE;
		else
			m_SubLadderTeam.m_eTeamType = TEAM_RED;

		if( m_MainLadderTeam.m_eTeamType == m_SubLadderTeam.m_eTeamType )
		{
			if( m_MainLadderTeam.m_eTeamType == TEAM_BLUE )
				m_SubLadderTeam.m_eTeamType = TEAM_RED;
			else
				m_SubLadderTeam.m_eTeamType = TEAM_BLUE;
		}
	}
	else
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Matching Ladder Team Sub Team None: %d", m_SubLadderTeam.m_dwTeamIndex );
	}

	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "Matching Ladder Team : %d(%d:%d:%d) vs %d(%d:%d:%d)", m_MainLadderTeam.m_dwTeamIndex, (int)m_MainLadderTeam.m_eTeamType, m_MainLadderTeam.m_iAbilityMatchLevel, 
							m_MainLadderTeam.m_dwGuildIndex, m_SubLadderTeam.m_dwTeamIndex, (int)m_SubLadderTeam.m_eTeamType, m_SubLadderTeam.m_iAbilityMatchLevel, m_SubLadderTeam.m_dwGuildIndex );

	// 양진영 세력포인트 및 길드 보너스 요청
	SP2Packet kPacket( MSTPK_CAMP_ROOM_BATTLE_INFO );
	kPacket << GetRoomIndex() << m_MainLadderTeam.m_dwGuildIndex << m_SubLadderTeam.m_dwGuildIndex;
	g_MainServer.SendMessage( kPacket );
}

void Room::SetLadderCurrentRank()
{
	LadderTeamParent *pLadderTeam = NULL;
	pLadderTeam = g_LadderTeamManager.GetGlobalLadderTeamNode( m_MainLadderTeam.m_dwTeamIndex );
	if( pLadderTeam )
	{
		m_MainLadderTeam.m_iCurTeamRank= pLadderTeam->GetTeamRanking();
	}
	else
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Matching Ladder Rank Main Team None: %d(%d)", m_MainLadderTeam.m_dwTeamIndex, m_MainLadderTeam.m_iAbilityMatchLevel );
	}

	pLadderTeam = g_LadderTeamManager.GetGlobalLadderTeamNode( m_SubLadderTeam.m_dwTeamIndex );
	if( pLadderTeam )
	{
		m_SubLadderTeam.m_iCurTeamRank= pLadderTeam->GetTeamRanking();
	}
	else
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Matching Ladder Rank Sub Team None: %d(%d)", m_SubLadderTeam.m_dwTeamIndex, m_SubLadderTeam.m_iAbilityMatchLevel );
	}
}

void Room::SetCampRoomInfo( DWORD dwBlueCampPoint, DWORD dwRedCampPoint, DWORD dwMainGuildIndex, float fMainGuildBonus, DWORD dwSubGuildIndex, float fSubGuildBonus )
{
	if( GetRoomStyle() != RSTYLE_LADDERBATTLE ) return;

	// 길드 정보 적용
	bool bChange = false;
	if( m_MainLadderTeam.m_dwGuildIndex == dwMainGuildIndex )
	{
		if( m_MainLadderTeam.m_fGuildBonus != fMainGuildBonus )
		{
			bChange = true;
			m_MainLadderTeam.m_fGuildBonus = fMainGuildBonus;
		}
	}

	if( m_SubLadderTeam.m_dwGuildIndex == dwSubGuildIndex )
	{
		if( m_SubLadderTeam.m_fGuildBonus != fSubGuildBonus )
		{
			bChange = true;
			m_SubLadderTeam.m_fGuildBonus = fSubGuildBonus;
		}
	}

	if( bChange )
	{
		CheckUserBonusTable( NULL );
	}

	// 세력비에 따른 진영 포인트 보너스 적용
	if( m_MainLadderTeam.m_iCampType == CAMP_BLUE )
	{
		m_MainLadderTeam.m_dwCampPoint = dwBlueCampPoint;
		m_SubLadderTeam.m_dwCampPoint  = dwRedCampPoint;
	}
	else
	{
		m_MainLadderTeam.m_dwCampPoint = dwRedCampPoint;
		m_SubLadderTeam.m_dwCampPoint  = dwBlueCampPoint;
	}

	DWORD dwTotalPoint = m_MainLadderTeam.m_dwCampPoint + m_SubLadderTeam.m_dwCampPoint;
	if( dwTotalPoint > 0 )
	{
		float fMainInfluence = ( ( (float)m_MainLadderTeam.m_dwCampPoint / (float)dwTotalPoint ) + 0.005f ) * 100.0f;
		float fSubInfluence  = ( ( (float)m_SubLadderTeam.m_dwCampPoint / (float)dwTotalPoint ) + 0.005f ) * 100.0f;
		float fGapInfluence  = fabs( fMainInfluence - fSubInfluence );

		float fCampPointBonus = 1.0f;
		if( m_pMode )
			fCampPointBonus = m_pMode->GetCampInfluenceBonus( fGapInfluence );
		if( fMainInfluence > fSubInfluence )
			m_SubLadderTeam.m_fCampPointBonus = fCampPointBonus;
		else
			m_MainLadderTeam.m_fCampPointBonus = fCampPointBonus;
	}
}

void Room::NotifyChangeCharToMode( User *pUser, int iSelectChar, int iPrevCharType )
{
	if( m_pMode )
	{
		m_pMode->NotifyChangeChar( pUser, iSelectChar, iPrevCharType );
	}
}

void Room::EnterReserveUser( DWORD dwUserIndex )
{
	ReserveUser kUser;
	kUser.m_dwUserIndex		= dwUserIndex;
	kUser.m_dwReserveTime	= TIMEGETTIME();
	LeaveReserveUser( dwUserIndex );            //기존에 있던 동일한 유저는 삭제하고 새로 넣는다.
	m_vReserveUser.push_back( kUser );
}

void Room::LeaveReserveUser( DWORD dwUserIndex )
{
	LOOP_GUARD();
	vReserveUser::iterator iter = m_vReserveUser.begin();
	while( m_vReserveUser.end() != iter )
	{
		ReserveUser &kUser = *iter;
		if( kUser.m_dwUserIndex == dwUserIndex )
		{
			m_vReserveUser.erase( iter );
			return;
		}
		iter++;
	}
	LOOP_GUARD_CLEAR();
}

bool Room::IsReserveUser( DWORD dwUserIndex )
{
	LOOP_GUARD();
	vReserveUser::iterator iter = m_vReserveUser.begin();
	while( m_vReserveUser.end() != iter )
	{
		ReserveUser &kUser = *iter++;
		if( kUser.m_dwUserIndex == dwUserIndex )
		{
			return true;
		}
	}
	LOOP_GUARD_CLEAR();
	return false;
}

void Room::ProcessReserveUser()
{
	DWORD dwReserveTime = 90000;
	LOOP_GUARD();
	vReserveUser::iterator iter = m_vReserveUser.begin();
	while( m_vReserveUser.end() != iter )
	{
		ReserveUser &kUser = *iter;
		if( TIMEGETTIME() - kUser.m_dwReserveTime >= dwReserveTime )
		{
			iter = m_vReserveUser.erase( iter );
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Room::ProcessReserveUser(%d) %d초간 입장하지 않아 룸에서 제외!!", GetRoomIndex(), dwReserveTime / 1000 );
		}
		else
			++iter;
	}
	LOOP_GUARD_CLEAR();
}

void Room::SendEnterUserRoomData( User *pUser )
{
	if( pUser == NULL ) return;
	if( m_pMode == NULL ) return;

	// 접속한 유저에게 룸 정보 전송.
	SP2Packet kJoinRoomPk( STPK_JOIN_ROOMDATA );
	kJoinRoomPk<<m_iRoomIndex;
	kJoinRoomPk<<m_iRoomNum;
	kJoinRoomPk<<(int)m_room_style;
	kJoinRoomPk<<m_bCharChangeToUDP;
	kJoinRoomPk<<m_bOnlyServerRelay;
	kJoinRoomPk<<m_pMode->GetModeSubNum();
	kJoinRoomPk<<m_pMode->GetModeMapNum();	
	kJoinRoomPk<<m_pMode->IsZeroHP();
	kJoinRoomPk<<m_bSafetyLevelRoom;
	if( m_room_style == RSTYLE_PLAZA )
	{
		kJoinRoomPk<<(int)GetPlazaModeType();
		kJoinRoomPk<< IsOpenPlazaRoom();
	}
	m_pMode->GetModeInfo( kJoinRoomPk );
	pUser->SendMessage( kJoinRoomPk );

#ifdef FLAG_MODE_BY_BCKIM_DEBUG		// STPK_JOIN_ROOMDATA  // SendEnterUserRoomData
	int iSubnum = m_pMode->GetModeSubNum();
	int iMapnum = m_pMode->GetModeMapNum();
	LOG.PrintTimeAndLog(0,"[FLAG_MODE_BY_BCKIM:SendEnterUserRoomData[STPK_JOIN_ROOMDATA] m_iModeSubNum[%d],m_iModeMapNum[%d]",iSubnum,iMapnum);
#endif // FLAG_MODE_BY_BCKIM_DEBUG


}

bool Room::SendEnterUserHostUser( User *pUser )
{
	if( pUser == NULL ) return false;
	//방장이 없으면 현재 들어온 유저를 방장으로 한다.
	bool bStopRoundTime = false;
	if( m_HostUserID.IsEmpty() )    
	{
		m_HostUserID = pUser->GetPublicID();
		bStopRoundTime = true;
	}

	// 호스트 이름 전송.
	SP2Packet kHostIDPk( STPK_HOST_USERID );
	kHostIDPk<<m_HostUserID;
	pUser->SendMessage( kHostIDPk );

	return bStopRoundTime;
}

void Room::SendEnterUserPlayerData( User *pUser )
{
	if( pUser == NULL ) return;
	if( m_pMode == NULL ) return;

	// 이미 룸에 있는 유저가 있으면 룸에 있는 유저의 데이터를 새로 들어온 유저에게 전송.
	vUser_iter iter;
	for( iter=m_vUserNode.begin() ; iter!=m_vUserNode.end() ; ++iter )
	{
		User *pExist = *iter;

		SP2Packet kExistUserPk( STPK_PLAYING_USER_DATA );
		//캐릭터 정보
		pExist->FillJoinUserData( kExistUserPk, IsExperienceUser( pExist ) );
		kExistUserPk << pExist->GetMyVictories();
		kExistUserPk << m_pMode->IsUserPlayState( pExist );
		kExistUserPk << pExist->GetKillDeathLevel() << pExist->GetLadderPoint();
		kExistUserPk << m_pMode->IsChatModeState( pExist );
		kExistUserPk << m_pMode->IsFishingState( pExist->GetPublicID() );
		kExistUserPk << pExist->GetFishingRodType();
		kExistUserPk << pExist->GetFishingBaitType();
		kExistUserPk << pExist->IsStealth();

		// 체험 상태
		m_pMode->FillExperienceMode( pExist, kExistUserPk );

		//장착 아이템 정보
		pExist->FillEquipItemData( kExistUserPk );
		pExist->FillEquipMedalItem( kExistUserPk );
		pExist->FillEquipCustomMedalItem( kExistUserPk );
		pExist->FillGrowthLevelData( kExistUserPk );
		
		int iClassType = 0;
		ioCharacter *rkChar = pExist->GetCharacter( pExist->GetSelectChar() );
		if( rkChar )	iClassType = rkChar->GetCharInfo().m_class_type;
		pExist->FillExMedalSlotByClassType( iClassType, kExistUserPk );

		//모드 플레이 정보
		kExistUserPk << pExist->GetPublicID();
		m_pMode->GetCharModeInfo( kExistUserPk, pExist->GetPublicID(), true );

		pUser->SendMessage( kExistUserPk );

		SP2Packet kExistUserPetPk( STPK_PET_EQUIP_INFO );
		
#if defined( DIVIDE_PET ) 
		if( !pExist->FillEquipPetData( kExistUserPetPk, iClassType ) )
		{
			PACKET_GUARD_VOID( kExistUserPetPk.Write(  pExist->GetPublicID() ) );
			PACKET_GUARD_VOID( kExistUserPetPk.Write( 0 ) );
			PACKET_GUARD_VOID( kExistUserPetPk.Write( 0 ) );
			PACKET_GUARD_VOID( kExistUserPetPk.Write( 0 ) );
			PACKET_GUARD_VOID( kExistUserPetPk.Write( iClassType ) );
			PACKET_GUARD_VOID( kExistUserPetPk.Write( false ) );
		}
#else
		if( pExist->FillEquipPetData( kExistUserPetPk ) )
#endif
			pUser->SendMessage( kExistUserPetPk );
			
		//장착 중인 칭호 정보.
		SP2Packet kExistUsertitle(STPK_TITLE_EQUIP_INFO);

		if( pExist->FillEquipTitleData( kExistUsertitle ) )
			pUser->SendMessage( kExistUsertitle );
	}

	// 다른 유저의 ETC item 정보
	for( iter=m_vUserNode.begin() ; iter!=m_vUserNode.end() ; ++iter )
	{
		User *pExist = *iter;
		if( !pExist )
			continue;
		g_EtcItemMgr.SendJoinUser( pExist, pUser );
	}	
}

void Room::SendEnterUserStructData( User *pUser )
{
	if( pUser == NULL ) return;
	if( m_pMode == NULL ) return;

	// 구조물
	pUser->SetNeedSendPushStruct( true );
	pUser->SetSendPushStructIndex( 0 );
	pUser->SetSendPushStructCheckTime( 0 );

	SP2Packet kBallPacket( STPK_BALLSTRUCT_INFO );
	if( m_pMode->GetBallStructInfo( kBallPacket ) )
	{
		pUser->SendMessage( kBallPacket );
	}

	SP2Packet kMachinePacket( STPK_MACHINESTRUCT );
	if( m_pMode->GetMachineStructInfo( kMachinePacket ) )
	{
		pUser->SendMessage( kMachinePacket );
	}

	// 필드아이템
	SendFieldItemInfo( pUser );
}

void Room::EnterUser( User *pUser )
{
	if( pUser == NULL ) return;

	// 길드 본부 입장 유저 일 경우 해당 길드 블락정보 전송.
	SendGuildBlocksInfo(pUser);

	// 개인 본부 입장 유저 일 경우 해당 본부 블락정보 전송.
	SendPersonalHQBlockInfo(pUser);

	// 접속한 유저에게 룸 정보 전송.
	SendEnterUserRoomData( pUser );

	bool bStopRoundTime = SendEnterUserHostUser( pUser );
	bool bChatMode = false;

	SendEnterUserPlayerData( pUser );	

	// User Info 초기화
	pUser->ClearCharJoinedInfo();
	pUser->ClearExitRoomReserve();

	switch ( GetRoomStyle() )
	{
	case RSTYLE_PLAZA:
		{
			/*if( GetPlazaModeType() == PT_GUILD )
			{
				pUser->SetTeam( GetNextGuildUserTeamType( pUser->GetGuildIndex() ) );
			}
			else
			{
				pUser->SetTeam( GetNextTeamType() );
			}*/
			pUser->SetTeam( GetNextTeamType() );

			if( GetPlazaModeType() == PT_COMMUNITY )
			{
				bChatMode = true;
			}
		}
		break;
	case RSTYLE_HEADQUARTERS:
		{
			pUser->SetTeam( GetNextTeamType() );
		}
		break;
	case RSTYLE_MATCHROOM:
		{
			if( !m_pMode )
			{
				pUser->SetTeam( TEAM_NONE );
			}
			else
			{
				SuccessionMode* pMode = static_cast<SuccessionMode*>( m_pMode );
				if( pMode )
					pUser->SetTeam( pMode->GetTeamTypeByUserIndex( pUser->GetUserIndex() ) );
				else
					pUser->SetTeam( TEAM_NONE );
			}
		}
		break;
	case RSTYLE_BATTLEROOM:
		{
			if( pUser->IsStealth() )
			{
				pUser->SetTeam( TEAM_NONE );
				LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "스텔스 입장 유저 : [%d] - %d - %s", 
					GetRoomIndex(), (int)GetModeType(), pUser->GetPublicID().c_str() );
			}
			else if( GetModeType() == MT_SURVIVAL || GetModeType() == MT_MONSTER_SURVIVAL || GetModeType() == MT_DUNGEON_A || GetModeType() == MT_FIGHT_CLUB || 
				GetModeType() == MT_RAID ||
					 Help::IsMonsterDungeonMode(GetModeType()) )
			{
				if( pUser->IsObserver() )
					pUser->SetTeam( TEAM_NONE );
				else
					pUser->SetTeam( GetNextTeamType() );
			}
			else if( GetModeType() == MT_BOSS )
			{
				if( pUser->IsObserver() )
					pUser->SetTeam( TEAM_NONE );
				else
				{
					TeamType eTeam = TEAM_RED;
					BattleRoomParent *pBattleRoom = pUser->GetMyBattleRoom();
					if( pBattleRoom && pBattleRoom->IsOriginal() )
					{
						// 보스 선정된 유저는 블루팀
						BattleRoomNode *pOriginal = (BattleRoomNode*)pBattleRoom;
						//블루 팀은 오직 한명만 가능
						if( pOriginal && pOriginal->GetBossName() == pUser->GetPublicID() && m_pMode->GetTeamUserCnt( TEAM_BLUE ) == 0 )
						{
							eTeam = TEAM_BLUE;
							pOriginal->ClearBossName();
						}
					}
					else
					{
						LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "전투방 플레이 입장시 파티가 없음 : [%d] - %d - %s", 
							GetRoomIndex(), (int)GetModeType(), pUser->GetPublicID().c_str() );
					}
					pUser->SetTeam( eTeam );
				}
			}
			else if( GetModeType() == MT_GANGSI )
			{
				if( pUser->IsObserver() )
					pUser->SetTeam( TEAM_NONE );
				else
				{
					TeamType eTeam = TEAM_RED;
					BattleRoomParent *pBattleRoom = pUser->GetMyBattleRoom();
					if( pBattleRoom && pBattleRoom->IsOriginal() )
					{
						// 강시 선정된 유저는 블루팀
						BattleRoomNode *pOriginal = (BattleRoomNode*)pBattleRoom;
						//블루 팀은 오직 한명만 가능
						if( pOriginal && pOriginal->GetGangsiName() == pUser->GetPublicID() && m_pMode->GetTeamUserCnt( TEAM_BLUE ) == 0 )
						{
							eTeam = TEAM_BLUE;
							pOriginal->ClearGangsi();
						}
					}
					else
					{
						LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "전투방 플레이 입장시 파티가 없음 : [%d] - %d - %s", 
							GetRoomIndex(), (int)GetModeType(), pUser->GetPublicID().c_str() );
					}
					pUser->SetTeam( eTeam );
				}
			}
			else if( GetModeType() == MT_TEAM_SURVIVAL_AI )
			{
				if( pUser->IsObserver() )
					pUser->SetTeam( TEAM_NONE );
				else
					pUser->SetTeam( GetNextTeamType() );
			}
			else
			{
				BattleRoomParent *pBattleRoom = pUser->GetMyBattleRoom();
				if( pBattleRoom && pBattleRoom->IsOriginal() )
				{
					BattleRoomNode *pOriginal = (BattleRoomNode*)pBattleRoom;
					if( pOriginal )
					{
						TeamType eTeam = pOriginal->GetUserTeam( pUser->GetPublicID() );
						pUser->SetTeam( eTeam );

						if( !pUser->IsObserver() )
						{
							if( eTeam != TEAM_BLUE && eTeam != TEAM_RED )
							{
								LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "전투방 플레이 입장시 팀이없음 : [%d] - %d - %s - %d", 
									GetRoomIndex(), (int)GetModeType(), pUser->GetPublicID().c_str(), (int)eTeam );
							}
						}
					}
				}
				else
				{
					LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "전투방 플레이 입장시 파티가 없음 : [%d] - %d - %s", 
						GetRoomIndex(), (int)GetModeType(), pUser->GetPublicID().c_str() );
				}
			}
		}
		break;
	case RSTYLE_SHUFFLEROOM:
		{
			ShuffleRoomParent *pShuffleRoom = pUser->GetMyShuffleRoom();
			ShuffleRoomNode *pOriginal = NULL;
			if( pShuffleRoom && pShuffleRoom->IsOriginal() )
				pOriginal = dynamic_cast<ShuffleRoomNode*>( pShuffleRoom );

			if( GetModeType() == MT_SURVIVAL || GetModeType() == MT_MONSTER_SURVIVAL || GetModeType() == MT_DUNGEON_A || GetModeType() == MT_FIGHT_CLUB || GetModeType() == MT_SHUFFLE_BONUS ||
				GetModeType() == MT_RAID ||
				Help::IsMonsterDungeonMode(GetModeType()) )
			{
				pUser->SetTeam( GetNextTeamType() );
			}
			else if( GetModeType() == MT_BOSS )
			{
				TeamType eTeam = TEAM_RED;
				if( pOriginal )
				{
					// 보스 선정된 유저는 블루팀
					if( pOriginal->GetBossName() == pUser->GetPublicID() && m_pMode->GetTeamUserCnt( TEAM_BLUE ) == 0 )
					{
						eTeam = TEAM_BLUE;
						pOriginal->ClearBossName();
					}
				}
				else
				{
					LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "전투방 플레이 입장시 파티가 없음 : [%d] - %d - %s", 
														  GetRoomIndex(), (int)GetModeType(), pUser->GetPublicID().c_str() );
				}
				pUser->SetTeam( eTeam );
			}
			else if( GetModeType() == MT_GANGSI )
			{
				TeamType eTeam = TEAM_RED;
				if( pOriginal )
				{
					// 강시 선정된 유저는 블루팀
					if( pOriginal->GetGangsiName() == pUser->GetPublicID() && m_pMode->GetTeamUserCnt( TEAM_BLUE ) == 0 )
					{
						eTeam = TEAM_BLUE;
						pOriginal->ClearGangsi();
					}
				}
				else
				{
					LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "전투방 플레이 입장시 파티가 없음 : [%d] - %d - %s", 
						GetRoomIndex(), (int)GetModeType(), pUser->GetPublicID().c_str() );
				}
				pUser->SetTeam( eTeam );
			}
			else
			{
				if( pOriginal )
				{
					TeamType eTeam = pOriginal->GetUserTeam( pUser->GetPublicID() );
					pUser->SetTeam( eTeam );

					if( eTeam != TEAM_BLUE && eTeam != TEAM_RED )
					{
						LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "전투방 플레이 입장시 팀이없음 : [%d] - %d - %s - %d", 
															  GetRoomIndex(), (int)GetModeType(), pUser->GetPublicID().c_str(), (int)eTeam );
					}
				}
				else
				{
					LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "전투방 플레이 입장시 파티가 없음 : [%d] - %d - %s", 
														  GetRoomIndex(), (int)GetModeType(), pUser->GetPublicID().c_str() );
				}
			}
		}
		break;
	case RSTYLE_LADDERBATTLE:
		{
			if( pUser->IsStealth() )
			{
				pUser->SetTeam( TEAM_NONE );
				LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "스텔스 입장 유저 : [%d] - %d - %s", 
													 GetRoomIndex(), (int)GetModeType(), pUser->GetPublicID().c_str() );
			}
			else 
			{
				LadderTeamParent *pLadderTeam = pUser->GetMyLadderTeam();
				if( pLadderTeam )
				{
					if( pLadderTeam->GetIndex() == m_MainLadderTeam.m_dwTeamIndex )
						pUser->SetTeam( m_MainLadderTeam.m_eTeamType );
					else if( pLadderTeam->GetIndex() == m_SubLadderTeam.m_dwTeamIndex )
						pUser->SetTeam( m_SubLadderTeam.m_eTeamType );
					else
						LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "래더팀 플레이 입장시 팀 정해지지 않음 : [%d] - B[%d] - R[%d] - (%d)%s", GetRoomIndex(), m_MainLadderTeam.m_dwTeamIndex, m_SubLadderTeam.m_dwTeamIndex, pLadderTeam->GetIndex(), pUser->GetPublicID().c_str() );
				}
				else
					LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "래더팀 플레이 입장시 팀 포인터 없음 : [%d] - %d - %s", GetRoomIndex(), (int)GetModeType(), pUser->GetPublicID().c_str() );			
			}
		}
		break;
	}

	AddUser( pUser );

	if( !bStopRoundTime )
	{
		m_pMode->SetStopTime( false );
	}
	else if( m_pMode->GetModeType() == MT_TRAINING || m_pMode->GetModeType() == MT_HEADQUARTERS || m_pMode->GetModeType() == MT_HOUSE )
	{
		m_pMode->SetStopTime( true );        //시간이 흐를 필요 없어요.
	}

	SendEnterUserStructData( pUser );

	//pUser 캐릭터 정보
	SP2Packet kJoinUserPk( STPK_JOIN_NEW_USERDATA );
	pUser->FillJoinUserData( kJoinUserPk, IsExperienceUser( pUser ) );

	PACKET_GUARD_VOID( kJoinUserPk.Write( pUser->GetMyVictories() ) );
	PACKET_GUARD_VOID( kJoinUserPk.Write( false ) );						// Now Not Playing..
	PACKET_GUARD_VOID( kJoinUserPk.Write( pUser->GetKillDeathLevel() ) );
	PACKET_GUARD_VOID( kJoinUserPk.Write( pUser->GetLadderPoint() ) );
	PACKET_GUARD_VOID( kJoinUserPk.Write( bChatMode ) );					// chatmode
	PACKET_GUARD_VOID( kJoinUserPk.Write( false ) );						// experience mode
	PACKET_GUARD_VOID( kJoinUserPk.Write( false ) );						// fishing
	PACKET_GUARD_VOID( kJoinUserPk.Write( 0 ) );							// fishing rod type
	PACKET_GUARD_VOID( kJoinUserPk.Write( 0 ) );							// fishing bait type
	PACKET_GUARD_VOID( kJoinUserPk.Write( pUser->IsStealth() ) );

	m_pMode->GetNewUserModeInfo( kJoinUserPk, pUser );
	RoomSendPacketTcp( kJoinUserPk );

	/*SP2Packet kJoinUserPetPk( STPK_PET_EQUIP_INFO );
	if( pUser->FillEquipPetData( kJoinUserPetPk ) )
	{
		RoomSendPacketTcp( kJoinUserPetPk );
	}*/
	/*SP2Packet JoinUserTitle(STPK_TITLE_EQUIP_INFO);
	if( pUser->FillEquipTitleData( JoinUserTitle ) )
	{
		RoomSendPacketTcp( JoinUserTitle );
	}*/

	if( !m_pMode->CheckRoundJoin( pUser ) )
	{
		SP2Packet kJoinInfoEndPk( STPK_JOIN_ROOM_INFO_END );
		pUser->SendMessage( kJoinInfoEndPk );
	}

	LeaveReserveUser( pUser->GetUserIndex() );
	// 광장 입장이라면 광장 정보 전송
	CRASH_GUARD();
	if( GetModeType() == MT_TRAINING )
	{
		SP2Packet kPacket( STPK_PLAZA_COMMAND );
		kPacket << PLAZA_CMD_INFO;
		kPacket << GetRoomIndex();
		kPacket << GetRoomNumber();
		FillPlazaInfo( kPacket );
		kPacket << GetJoinUserCnt() << GetPlayUserCnt() << GetMasterLevel() << (int)GetPlazaModeType();
		if( GetMasterName() == pUser->GetPublicID() )       //방장에게만 비밀번호 전송
			kPacket << m_szRoomPW;
		pUser->SendMessage( kPacket );

		SP2Packet kPacket1( STPK_PLAZA_COMMAND );
		kPacket1 << PLAZA_CMD_ROOM_LEVEL << GetPlazaRoomLevel();
		RoomSendPacketTcp( kPacket1, pUser );

#ifdef EXCAVATION_EX_BYBCKIM
		// nobody 
#else
		g_ExcavationMgr.CheckSendCreateArtifact( GetRoomIndex() );		// 2018-01-09 by bckim, 주석 처리 하며 되겠습니다. 
		g_ExcavationMgr.EnterUser( pUser, GetRoomIndex() );				// 2018-01-09 by bckim, 주석 처리 하며 되겠습니다. 
#endif // EXCAVATION_EX_BYBCKIM
	}
	m_SyncUpdate.Update( RoomSync::RS_CURUSER );

	if( GetRoomStyle() == RSTYLE_BATTLEROOM && IsPartyProcessEnd() )
	{
		// 룸에 입장한 후 파티 연습 UI 출력
		SP2Packet kBattleRoomPacket( STPK_BATTLEROOM_COMMAND );
		kBattleRoomPacket << BATTLEROOM_UI_SHOW_OK << false << false;
		pUser->SendMessage( kBattleRoomPacket );
	}

	m_pMode->AddNewRecordEtcItemSync( pUser );
	CheckUserBonusTable( pUser );
	EnterUserLadderTeamOtherInfo( pUser );
}

void Room::EnterUserLadderTeamOtherInfo( User *pUser )
{
	if( GetRoomStyle() != RSTYLE_LADDERBATTLE ) return;
	if( !pUser ) return;

	LadderTeamParent *pLadderTeam = pUser->GetMyLadderTeam();
	if( !pLadderTeam )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Room::EnterUserLadderTeamOtherInfo Team NULL:(%s)", pUser->GetPublicID().c_str() );
		return;
	}

	if( m_MainLadderTeam.m_dwTeamIndex != pLadderTeam->GetIndex() )
	{
		//
		if( m_MainLadderTeam.m_dwTeamIndex != 0 )
		{
			//Send
			SP2Packet kPacket( STPK_LADDER_OTHER_TEAM_INFO );
			kPacket << m_MainLadderTeam.m_dwTeamIndex << m_MainLadderTeam.m_iCampType << (int)m_MainLadderTeam.m_eTeamType;
			kPacket << m_MainLadderTeam.m_szTeamName << m_MainLadderTeam.m_dwGuildIndex;
			kPacket << m_MainLadderTeam.m_iWinRecord << m_MainLadderTeam.m_iLoseRecord << m_MainLadderTeam.m_iVictoriesRecord;
			kPacket << m_MainLadderTeam.m_iPrevTeamRank;
			pUser->SendMessage( kPacket );
		}
		else
		{
			//Error
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Room::EnterUserLadderTeamOtherInfo 상대팀 인덱스 Zero!!:(%s)", pUser->GetPublicID().c_str() );
		}
	}
	else if( m_SubLadderTeam.m_dwTeamIndex != pLadderTeam->GetIndex() )
	{
		//
		if( m_SubLadderTeam.m_dwTeamIndex != 0 )
		{
			//Send
			SP2Packet kPacket( STPK_LADDER_OTHER_TEAM_INFO );
			kPacket << m_SubLadderTeam.m_dwTeamIndex << m_SubLadderTeam.m_iCampType << (int)m_SubLadderTeam.m_eTeamType;
			kPacket << m_SubLadderTeam.m_szTeamName << m_SubLadderTeam.m_dwGuildIndex;
			kPacket << m_SubLadderTeam.m_iWinRecord << m_SubLadderTeam.m_iLoseRecord << m_SubLadderTeam.m_iVictoriesRecord;
			kPacket << m_SubLadderTeam.m_iPrevTeamRank;
			pUser->SendMessage( kPacket );
		}
		else
		{
			//Error
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Room::EnterUserLadderTeamOtherInfo 상대팀 인덱스 Zero!!:(%s)", pUser->GetPublicID().c_str() );
		}
	}
	else
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Room::EnterUserLadderTeamOtherInfo 알 수 없는 에러:(%s)", pUser->GetPublicID().c_str() );
	}
}

void Room::LeaveUser( User *pUser, BYTE eType )
{
	if(pUser == NULL) return;

	// 용병 기간 업데이트
	pUser->UpdateCharLimitDate();
	pUser->UpdateEtcItemTime( __FUNCTION__ );
	pUser->LeaveRoomEtcItemTime();
	pUser->DeleteEtcItemPassedDate();
	pUser->DeleteExtraItemPassedDate(false);
	pUser->DeleteMedalItemPassedDate(false);
	pUser->DeleteExMedalSlotPassedDate();
	pUser->DeleteCostumePassedDate();
	pUser->ReleaseAccessoryPassedDate();

	pUser->DeleteCharAwakePassedDate();
	pUser->DeleteExpiredBonusCash();

	// 임시 : 시간육성
	pUser->CheckTimeGrowth();

	// 룸에서 업데이트된 이벤트 정보 전송
	pUser->SendEventData();

	//방장 새로 변경.
	CRASH_GUARD();
	if( pUser->GetPublicID() == m_HostUserID  )
	{
		SelectNewHost( pUser );
	}

	//관리자 새로 변경.
	CRASH_GUARD();
	if( pUser->GetPublicID() == m_szMasterUserID )
	{
		SelectNewMaster( pUser );
	}
	
	//길드 본부 모드 일 경우 설치 모드 체크.
	if( GetPlazaModeType() == PT_GUILD )
	{
		if( g_GuildRoomBlockMgr.IsConstructingUser(pUser->GetGuildIndex(), pUser->GetUserIndex()) )
			g_GuildRoomBlockMgr.SetConstructingState(pUser->GetGuildIndex(), FALSE);

		pUser->SetFisheryBlockInfo(0);
		g_GuildRoomBlockMgr.UpdateGuildRoomLeaveTime(pUser->GetGuildIndex());
	}

	RemoveUser( pUser );

#ifdef ARENA_MODE_BY_BCKIM		// 2018-07-25 by bckim, 아레나 모드 추가		Room::LeaveUser
#ifdef ARENA_MODE_BY_BCKIM_DEBUG
	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"[ARENA_MODE] LeaveUser Clent[%s] >> DELETE !!! ", pUser->GetPublicID().c_str());
#endif	//	ARENA_MODE_BY_BCKIM_DEBUG
	pUser->Arena_Mode_CharDelete();
#endif 	// ARENA_MODE_BY_BCKIM	// End. 2018-07-25 by bckim, 아레나 모드 추가

	if( !m_vUserNode.empty() )
	{
		LeaveUserRoomProcess();

		//방이 계속 존재하면 유저에게 퇴장 알림
		SP2Packet kPacket( STPK_LEAVE_USERDATA );

		PACKET_GUARD_VOID( kPacket.Write( pUser->GetPublicID() ) );
		PACKET_GUARD_VOID( kPacket.Write( eType ) );
		GetExtraModeInfo( kPacket );
		RoomSendPacketTcp( kPacket );

		if( m_pMode )
		{
			m_pMode->DestroyPushStructByLeave( pUser->GetPublicID() );
		}

		// 광장 퇴장이라면 광장 정보 전송
		if( GetModeType() == MT_TRAINING )
		{
			SP2Packet kPacket1( STPK_PLAZA_COMMAND );
			PACKET_GUARD_VOID( kPacket1.Write(PLAZA_CMD_ROOM_LEVEL) );
			PACKET_GUARD_VOID( kPacket1.Write(GetPlazaRoomLevel()) );
			RoomSendPacketTcp( kPacket1 );
		}
		CheckUserBonusTable( NULL );
		LeaveUserLadderProcess();
	}
	else
	{
		//마지막 유저가 퇴장하면 룸 해제는 RoomProcess에서 한다.
	}
	m_SyncUpdate.Update( RoomSync::RS_CURUSER );
}

void Room::NetworkCloseLeaveCall( User *pUser )
{
	if( !pUser ) return;

	if( m_pMode )
	{
		if( m_pMode->SetLeaveUserPenalty( pUser ) )
		{
			LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "Room::NetworkCloseLeaveCall : [%d] - %d - %s", GetRoomIndex(), m_pMode->GetModeType(), pUser->GetPublicID().c_str() );
		}
	}
	// 여기서 룸 이탈을 처리하지 않는다. 
}

void Room::PrivatelyLeaveUser( User *pUser ) 
{
	if( pUser == NULL ) return;

	// 용병 기간 업데이트
	pUser->UpdateCharLimitDate();
	pUser->UpdateEtcItemTime( __FUNCTION__ );
	pUser->LeaveRoomEtcItemTime();
	pUser->DeleteEtcItemPassedDate();
	pUser->DeleteExtraItemPassedDate(false);
	pUser->DeleteMedalItemPassedDate(false);
	pUser->DeleteExMedalSlotPassedDate();
	pUser->DeleteCharAwakePassedDate( );
	pUser->DeleteCostumePassedDate();
	pUser->ReleaseAccessoryPassedDate();
	pUser->DeleteExpiredBonusCash();

	// 임시 : 시간육성
	pUser->CheckTimeGrowth();

	// 룸에서 업데이트된 이벤트 정보 전송
	pUser->SendEventData();

	if( m_pMode )
	{
		m_pMode->RemoveRecord( pUser, true );
	}

	vUser_iter iter, iEnd;
	iEnd = m_vUserNode.end();
	for( iter=m_vUserNode.begin() ; iter!=iEnd ; ++iter )
	{
		if( *iter == pUser )
		{
			m_vUserNode.erase( iter );
			break;
		}
	}

	pUser->ClearCharJoinedInfo();
	pUser->ClearExitRoomReserve();
	pUser->ReleaseEquipAllChar();

	ClearRemoveUserOwnItem( pUser );
	CheckUserBonusTable( NULL );
	LeaveUserLadderProcess();

	// 중계 해제
	if( pUser )
	{
		g_Relay.RemoveRelayGroupReserve( this, pUser->GetUserIndex() );
	}
}

void Room::LeaveUserLadderProcess()
{
	if( GetRoomStyle() != RSTYLE_LADDERBATTLE ) return;
	if( !m_pMode ) return;
	if( m_bRoomProcess ) return;
	if( m_MainLadderTeam.m_dwTeamIndex == 0 && m_SubLadderTeam.m_dwTeamIndex == 0 ) return; //이미 팀 캔슬이 된 상태라면 작동하지 않음.


	// 한쪽팀이 전부 나갔으면 광장으로 이탈
	int iBlueCnt, iRedCnt;
	iBlueCnt = m_pMode->GetTeamUserCnt( TEAM_BLUE );
	iRedCnt  = m_pMode->GetTeamUserCnt( TEAM_RED );    

	// 래더팀 레디 상태로 전환
	if( iBlueCnt == 0 || iRedCnt == 0 )
	{
		SP2Packet kPacket( STPK_LADDERTEAM_MACRO );

		PACKET_GUARD_VOID( kPacket.Write(LADDERTEAM_MACRO_MODE_END) );
		PACKET_GUARD_VOID( kPacket.Write(iBlueCnt) ); 
		PACKET_GUARD_VOID( kPacket.Write(iRedCnt) );

		RoomSendPacketTcp( kPacket );

		LadderTeamLeaveRoom();
	}
}

void Room::CheckUserBonusTable( User *pJoinUser )
{
	if( m_pMode == NULL ) return;

	static vUser vChangeUserBonus;
	vChangeUserBonus.clear();

	int i = 0;
	int iJoinUserCnt = m_vUserNode.size();
	for(int i=0 ; i<iJoinUserCnt ; i++ )
	{
		User *pUser = m_vUserNode[i];
		if( !pUser ) continue;

		UserBonusTable kUserBonusTable;
		kUserBonusTable.m_bPCRoomBonus = pUser->IsPCRoomAuthority();
		if( m_pMode->GetSameFriendUserCnt( pUser ) > 0 )
			kUserBonusTable.m_bFriendBonus = true;
		else
			kUserBonusTable.m_bFriendBonus = false;

		pUser->SetBonusTable( kUserBonusTable );
		if( pUser->ChangeBonusTable() )
			vChangeUserBonus.push_back( pUser );
	}

	// 이미 플레이중인 유저에게는 변경된 유저의 정보만 보낸다.
	if( !vChangeUserBonus.empty() )
	{
		int iChangeUserCnt = vChangeUserBonus.size();
		SP2Packet kChangePacket( STPK_USER_BONUS_SYNC );
		kChangePacket << iChangeUserCnt;
		for(int i=0 ; i<iChangeUserCnt ; i++ )
		{
			User *pUser = vChangeUserBonus[i];
			if( pUser )
			{
				const UserBonusTable &kUserBonusTable = pUser->GetBonusTable();
				kChangePacket << pUser->GetPublicID() << kUserBonusTable.m_bPCRoomBonus << kUserBonusTable.m_bGuildBonus << kUserBonusTable.m_bFriendBonus;
			}
			else
				kChangePacket << "" << false << false << false;
		}
		RoomSendPacketTcp( kChangePacket, pJoinUser );
		vChangeUserBonus.clear();
	}	

	// 입장한 유저에게는 전체 정보를 보낸다.
	if( pJoinUser )
	{
		SP2Packet kJoinUserPacket( STPK_USER_BONUS_SYNC );
		kJoinUserPacket << iJoinUserCnt;
		for(int i=0 ; i<iJoinUserCnt ; i++ )
		{
			User *pUser = m_vUserNode[i];
			if( pUser )
			{
				const UserBonusTable &kUserBonusTable = pUser->GetBonusTable();
				kJoinUserPacket << pUser->GetPublicID() << kUserBonusTable.m_bPCRoomBonus << kUserBonusTable.m_bGuildBonus << kUserBonusTable.m_bFriendBonus;
			}
			else
				kJoinUserPacket << "" << false << false << false;
		}
		pJoinUser->SendMessage( kJoinUserPacket );
	}
	// 모든 동기화가 완료되었으면 백업
	for(int i=0 ; i<iJoinUserCnt ; i++ )
	{
		User *pUser = m_vUserNode[i];
		if( pUser )
			pUser->BackupBonusTable();
	}
}

void Room::LeaveUserRoomProcess()
{
	if( m_pMode )
	{
		m_pMode->SendScoreGauge();
		
		// 예약 유저가 없는 상태에서 유저가 이탈하면 종료 체크를 한다. 
		if( m_vReserveUser.empty() )
		{
			m_pMode->CheckUserLeaveEnd();
		}
	}
}

void Room::SelectNewHost( User *pOutUser )
{
	m_HostUserID.Clear();

	ioHashString szObserver;

	vUser_iter iter, iEnd;
	iEnd = m_vUserNode.end();
	for( iter=m_vUserNode.begin() ; iter!=iEnd ; ++iter )
	{
		User *pCurUser = *iter;
		if( pCurUser->GetPublicID() != pOutUser->GetPublicID() )
		{
			if( !pCurUser->IsObserver() && !pCurUser->IsStealth() )
			{
				m_HostUserID = pCurUser->GetPublicID();
				break;
			}
			else if( szObserver.IsEmpty() )
			{
				szObserver = pCurUser->GetPublicID();
			}
		}
	}

	// 플레이어가 없고 옵저버만 있으면 옵저버가 방장
	if( m_HostUserID.IsEmpty() && !szObserver.IsEmpty() )
	{
		m_HostUserID = szObserver;

		SP2Packet kPacket( STPK_HOST_USERID );

		PACKET_GUARD_VOID( kPacket.Write(m_HostUserID) );

		RoomSendPacketTcp( kPacket, pOutUser );
	}
	else if( !m_HostUserID.IsEmpty() )
	{
		SP2Packet kPacket( STPK_HOST_USERID );

		PACKET_GUARD_VOID( kPacket.Write(m_HostUserID) );

		RoomSendPacketTcp( kPacket, pOutUser );
	}
}

void Room::SelectNewMaster( User *pOutUser )
{
	if( GetModeType() != MT_TRAINING ) return;
	if( IsOpenPlazaRoom() ) return;    // 공개방은 관리자 세팅하지 않는다.

	ioHashString szMasterID;
	int iTempLevel = 0;
	
	vUser_iter iter, iEnd;
	iEnd = m_vUserNode.end();
	for( iter=m_vUserNode.begin() ; iter!=iEnd ; ++iter )
	{
		User *pCurUser = *iter;
		if( pCurUser->GetPublicID() != pOutUser->GetPublicID() )
		{
			int iUserLevel = pCurUser->GetKillDeathLevel();
			if( iUserLevel >= iTempLevel )
			{
				iTempLevel = iUserLevel;
				szMasterID = pCurUser->GetPublicID();
			}
		}
	}

	// 관리자가 나가면 비번은 없어진다.
	m_szRoomPW.Clear();
	SetRoomMasterID( szMasterID );
	if( !m_szMasterUserID.IsEmpty() )
	{
		// 광장 유저에게 관리자 변경 전송
		SP2Packet kPacket( STPK_PLAZA_COMMAND );
		kPacket << PLAZA_CMD_MASTER_CHANGE << m_szMasterUserID;
		RoomSendPacketTcp( kPacket, pOutUser );
	}
}

void Room::AddUser( User *pUser )
{
	m_vUserNode.push_back( pUser );

	if( m_pMode )
	{
		m_pMode->AddNewRecord( pUser );
	}

	// 중계 등록
	if( pUser )
	{
#ifndef ANTIHACK
		g_Relay.InsertRelayGroupReserve( this, pUser->GetUserIndex(), pUser->GetPublicIP(), pUser->GetUDP_port(), pUser->GetPublicID() );
#else
		SendRUDPUserInfo( pUser );
#endif
	}
}

// 2019-02-14 by bckim, 배틀 모드 추가
void Room::SetBattleModeOrder( 	DWORD dwUserIDX, int iOrder )
{
	if( m_pMode)
	{
		m_pMode->SetOrder(dwUserIDX, iOrder);
	}
}
// End. 2019-02-14 by bckim, 배틀 모드 추가


void Room::RemoveUser( User *pUser )
{
	if( m_pMode )
	{
		m_pMode->CheckLeaveMatch( pUser );
		m_pMode->RemoveRecord( pUser );
	}

	vUser_iter iter, iEnd;
	iEnd = m_vUserNode.end();
	for( iter=m_vUserNode.begin() ; iter!=iEnd ; ++iter )
	{
		if( *iter == pUser )
		{
			m_vUserNode.erase( iter );
			break;
		}
	}

	pUser->ClearCharJoinedInfo();
	pUser->ClearExitRoomReserve();
	pUser->ReleaseEquipAllChar();

	pUser->SetNeedSendPushStruct( false );
	pUser->SetNeedSendPushStructIndex( 0 );
	pUser->SetSendPushStructIndex( 0 );
	pUser->SetSendPushStructCheckTime( 0 );

	ClearRemoveUserOwnItem( pUser );

	// 중계 해제
	if( pUser )
	{
		g_Relay.RemoveRelayGroupReserve( this, pUser->GetUserIndex() );
	}
}

void Room::RemoveUser( int iUserIndex, int iRoomIndex )
{
	if( iUserIndex < 0 )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Room::RemoveUser() UserIndex Is Empty" );
		return;
	}
	vUser_iter iter, iEnd;
	iEnd = m_vUserNode.end();
	for(iter=m_vUserNode.begin();iter!=iEnd;++iter)
	{
		User *pUser = (User*)(*iter);
		if( pUser->GetUserIndex() == iUserIndex )
		{
			// P2P Relay Log
			//P2PRelayLOG.PrintNoEnterLog( 0, "{[%d]<%s>(%d)+%d-}", iRoomIndex, rkUser.GetPublicID().c_str(), kUser.m_iServerRelayCount, (int)kUser.IsNATUser() );
			m_vUserNode.erase( iter );			
			return;
		}
	}
	return;
}

void Room::DestroyMode()
{
	DestroyAllFieldItems();

	if( m_pMode )
	{
		m_pMode->DestroyMode();
		SAFEDELETE( m_pMode );
	}
}
		
// 2018-07-25 by bckim, 아레나 모드 추가		void Room::RoomSendPacketTcp_ArenaMode(int ModeType, int Roomnum)
void Room::RoomSendPacketTcp_ArenaModeChar_Delete()
{
	vUser_iter iter = m_vUserNode.begin();
	vUser_iter iter_Prev;

	LOOP_GUARD();
	while(iter != m_vUserNode.end())
	{
		iter_Prev = iter++;
		User *item = *iter_Prev;
		if(item == NULL) continue;

		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"[ARENA_MODE]+[STPK_JOIN_ROOMDATA]+DELETE Clent[%s]", item->GetPublicID().c_str());
		item->Arena_Mode_CharDelete();
	}
	LOOP_GUARD_CLEAR();
}
// End. 2018-07-25 by bckim, 아레나 모드 추가

void Room::RoomSendPacketTcp(SP2Packet &packet,User *pOwner)
{
	vUser_iter iter = m_vUserNode.begin();
	vUser_iter iter_Prev;
	
	LOOP_GUARD();
	while(iter != m_vUserNode.end())
	{
		iter_Prev = iter++;
		User *item = *iter_Prev;
		if(item == NULL) continue;

		if(item != pOwner)
			item->SendMessage(packet);
	}
	LOOP_GUARD_CLEAR();
}

void Room::RoomSendPacketTcpSenderLast( SP2Packet &packet, User *pLast )
{
	vUser_iter iter = m_vUserNode.begin();
	vUser_iter iter_Prev;

	LOOP_GUARD();
	while(iter != m_vUserNode.end())
	{
		iter_Prev = iter++;
		User *item = *iter_Prev;
		if(item == NULL) continue;

		if(item != pLast)
			item->SendMessage(packet);
	}
	LOOP_GUARD_CLEAR();

	pLast->SendMessage( packet );
}

void Room::RoomSendPacketTcpSenderUser( SP2Packet &rkPacket, const ioHashString &szName )
{
	vUser_iter iter = m_vUserNode.begin();
	vUser_iter iter_Prev;
	CRASH_GUARD();
	while(iter != m_vUserNode.end())
	{
		iter_Prev = iter++;
		User *item = *iter_Prev;
		if(item == NULL) continue;
		if(item->GetPublicID() == szName )
		{
			item->SendMessage(rkPacket);
			return;
		}
	}
}

void Room::RoomSendPacketUdp(SP2Packet &packet,User *pOwner)
{
	vUser_iter iter = m_vUserNode.begin();
	vUser_iter iter_Prev;                          
	
	LOOP_GUARD();
	while(iter != m_vUserNode.end())
	{
		iter_Prev = iter++;
		User *item = *iter_Prev;
		if(item == NULL) continue;
		
		if(item != pOwner)
			g_UDPNode.SendMessage(item->GetPublicIP(),item->GetUDP_port(),packet);
	}
	LOOP_GUARD_CLEAR();
}

void Room::RoomSendPacketUdp(ioHashString &rkName, SP2Packet &packet)
{
	vUser_iter iter = m_vUserNode.begin();
	vUser_iter iter_Prev;                  
	CRASH_GUARD(); 
	LOOP_GUARD();
	while(iter != m_vUserNode.end())
	{
		iter_Prev = iter++;
		User *item = *iter_Prev;
		if(item == NULL) continue;
		
		if(item->GetPublicID() == rkName)
		{
			g_UDPNode.SendMessage(item->GetPublicIP(),item->GetUDP_port(),packet);
			break;
		}
	}
	LOOP_GUARD_CLEAR();
}

void Room::RoomTeamSendPacketTcp(SP2Packet &packet, TeamType eSendTeam ,User *pOwner)
{
	vUser_iter iter = m_vUserNode.begin();
	vUser_iter iter_Prev;
	
	LOOP_GUARD();
	while(iter != m_vUserNode.end())
	{
		iter_Prev = iter++;
		User *item = *iter_Prev;
		if(item == NULL) continue;
		if(item == pOwner) continue;

		if(item->GetTeam() == eSendTeam)
			item->SendMessage(packet);
	}	
	LOOP_GUARD_CLEAR();
}

void Room::RoomSendPacketTcpExceptBattleRoomUser( SP2Packet &packet, User *pOwner/*=NULL */ )
{
	vUser_iter iter = m_vUserNode.begin();
	vUser_iter iter_Prev;

	LOOP_GUARD();
	while(iter != m_vUserNode.end())
	{
		iter_Prev = iter++;
		User *item = *iter_Prev;
		if(item == NULL) continue;

		if(item != pOwner && !item->IsBattleRoom() )
			item->SendMessage(packet);
	}
	LOOP_GUARD_CLEAR();
}

void Room::RoomProcess()
{
	if(!m_bRoomProcess) return;

	if( m_pMode )
	{
		m_pMode->ProcessTime();

		if( m_pMode->IsRequestDestroy() )
		{
			switch ( GetRoomStyle() )
			{
			case RSTYLE_BATTLEROOM:
				NextShamBattle();
				break;
			case RSTYLE_SHUFFLEROOM:
				NextShamShuffle();
				break;
			case RSTYLE_LADDERBATTLE:
				NextLadderBattle();
				break;
			case RSTYLE_MATCHROOM:
				NextMatchBattle();
				break;
			}
		}
	}
	m_SyncUpdate.Process();
	ProcessReserveUser();
}

void Room::NextMatchBattle()
{
	m_bRoomProcess = false;

	SP2Packet kPacket( STPK_SUCCESSION_MATCHING_GAMEEND );
	RoomSendPacketTcp( kPacket );
}

void Room::NextShamBattle()
{
	m_bRoomProcess = false;          //더이상 룸 프로세시를 돌리지 않는다.
	m_bPartyProcessEnd = true;

	int iBlueCnt = 0;
	int iRedCnt  = 0;

	BattleRoomParent *pBattleRoom = NULL;

	vUser_iter iter = m_vUserNode.begin();
	vUser_iter iter_Prev;

	LOOP_GUARD();
	while(iter != m_vUserNode.end())
	{
		iter_Prev = iter++;
		User *item = *iter_Prev;
		if(item == NULL) continue;
		if(item->IsStealth() ) continue;

		if(item->GetTeam() == TEAM_BLUE && !item->IsObserver() )
		{
			if( item->GetMyBattleRoom() )
			{
				iBlueCnt++;
				pBattleRoom = item->GetMyBattleRoom();
			}			
			else
			{
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Room::NextShamBattle() BLUE PARTY ERROR : %s 유저 파티 포인터 NULL!!", item->GetPublicID().c_str() );
			}
		}
		else if(item->GetTeam() == TEAM_RED && !item->IsObserver() )
		{
			if( item->GetMyBattleRoom() )
			{
				iRedCnt++;
				pBattleRoom  = item->GetMyBattleRoom();
			}
			else
			{
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Room::NextShamBattle() RED PARTY ERROR : %s 유저 파티 포인터 NULL!!", item->GetPublicID().c_str() );
			}
		}
		else if( item->GetMyBattleRoom() && !item->IsObserver() )
		{
			pBattleRoom = item->GetMyBattleRoom();
			if( iBlueCnt > iRedCnt )
				iRedCnt++;
			else
				iBlueCnt++;
		}
		else if( item->GetMyBattleRoom() && item->IsObserver() )
		{
			pBattleRoom = item->GetMyBattleRoom();
		}
	}
	LOOP_GUARD_CLEAR();

//	if( CheckAutoNextMode( pBattleRoom, iBlueCnt, iRedCnt ) )
//		return;

	SP2Packet rkPacket( STPK_BATTLE_ROOM_END );
	rkPacket << iBlueCnt << iRedCnt;
	RoomSendPacketTcp( rkPacket );

#ifdef ARENA_MODE_BY_BCKIM			// 2018-07-25 by bckim, 아레나 모드 추가	RoomSendPacketTcp_ArenaModeChar_Delete
	RoomSendPacketTcp_ArenaModeChar_Delete();
#endif		//  ARENA_MODE_BY_BCKIM			// End. 2018-07-25 by bckim, 아레나 모드 추가	

	if( !pBattleRoom )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Room::NextShamBattle() NONE PARTY POINTER!!!!" );
		return;
	}

	if( pBattleRoom->GetBattleEventType() != BET_TOURNAMENT_BATTLE )
	{
		// 경기방에서는 경기 종료시 바로 이탈 시킨다.
		pBattleRoom->SyncPlayEnd( CheckAutoNextMode( pBattleRoom, iBlueCnt, iRedCnt ) );		
	}
}

bool Room::CheckAutoNextMode( BattleRoomParent *pBattleParent, int iBlueTeam, int iRedTeam )
{
	if( !pBattleParent ) return false;
	if( !pBattleParent->IsOriginal() ) return false;

	BattleRoomNode *pBattleRoom = static_cast<BattleRoomNode*>(pBattleParent);
	if( !pBattleRoom->IsAutoModeStart() ) return false;             // 자동 시작 X
	if( GetModeType() == MT_SURVIVAL || GetModeType() == MT_BOSS || GetModeType() == MT_GANGSI || GetModeType() == MT_FIGHT_CLUB )
	{
		if( iBlueTeam + iRedTeam >= 2 )      // 2인 이상이면 가능
			return true;
	}
	else if( GetModeType() == MT_MONSTER_SURVIVAL || GetModeType() == MT_DUNGEON_A || 
		Help::IsMonsterDungeonMode(GetModeType()) )
	{
		if( iBlueTeam + iRedTeam >= 1 )      // 1인 이상이면 가능
			return true;
	}
	else if(GetModeType() == MT_RAID)
	{
		if( iBlueTeam + iRedTeam >= 4 )      // 4인 이상이면 가능
			return true;
	}
	else    // 팀플
	{
		if( iBlueTeam + iRedTeam < 2 ) 
			return false; // 최소 2명 이상이어야함

		if( iBlueTeam == 0 || iRedTeam == 0 )
		{
			// 서바이벌이 아니고 한쪽팀에 유저수가 0명인데 랜덤 팀이 설정되어 있으면 가능.
			if( pBattleRoom->IsRandomTeamMode() )
				return true;  
		}
		else
		{
			return true;
		}
	}
	return false;
}

void Room::NextLadderBattle()
{
	if( m_pMode == NULL ) return;

	m_bRoomProcess = false;          //더이상 룸 프로세시를 돌리지 않는다.

	LOOP_GUARD();
	int iBlueCnt = 0;
	int iRedCnt  = 0;
	vUser_iter iter = m_vUserNode.begin();
	vUser_iter iter_Prev;
	while(iter != m_vUserNode.end())
	{
		iter_Prev = iter++;
		User *item = *iter_Prev;
		if(item == NULL) continue;
		if(item->IsStealth()) continue;

		if(item->GetTeam() == TEAM_BLUE )
		{
			iBlueCnt++;			
		}
		else if(item->GetTeam() == TEAM_RED )
		{
			iRedCnt++;
		}

		//
		// 길드팀 인덱스 동기화
		LadderTeamParent *pLadderTeam = item->GetMyLadderTeam();
		if( pLadderTeam )
		{
			SP2Packet kPacket( STPK_LADDERTEAM_MACRO );
			kPacket << LADDERTEAM_MACRO_CHANGE_GUILD << pLadderTeam->GetGuildIndex();
			item->SendMessage( kPacket );
		}
	}
	LOOP_GUARD_CLEAR();

	{   // 양팀 플레이 종료시 본부로 강제 이동
		SP2Packet kPacket( STPK_LADDERTEAM_MACRO );
		kPacket << LADDERTEAM_MACRO_MODE_END << 0 << 0;
		RoomSendPacketTcp( kPacket );

		LadderTeamLeaveRoom();
		m_MainLadderTeam.Init();
		m_SubLadderTeam.Init();
	}

/*	{   // 룸 종료 알림
		SP2Packet kPacket( STPK_LADDERTEAM_MACRO );
		kPacket << LADDERTEAM_MACRO_MODE_END << iBlueCnt << iRedCnt;
		RoomSendPacketTcp( kPacket );

		// 래더팀 레디 상태로 전환
		if( iBlueCnt == 0 || iRedCnt == 0 )
			LadderTeamLeaveRoom();
	}
*/
}

void Room::NextShamShuffle()
{
	m_bRoomProcess = false;          //더이상 룸 프로세시를 돌리지 않는다.
	m_bPartyProcessEnd = true;

	int iBlueCnt = 0;
	int iRedCnt  = 0;

	ShuffleRoomParent *pShuffleRoom = NULL;

	vUser_iter iter = m_vUserNode.begin();
	vUser_iter iter_Prev;

	LOOP_GUARD();
	while( iter != m_vUserNode.end() )
	{
		iter_Prev = iter++;
		User *item = *iter_Prev;
		if(item == NULL) continue;

		pShuffleRoom = item->GetMyShuffleRoom();
		if( pShuffleRoom )
		{
			if(item->GetTeam() == TEAM_BLUE )
			{
				iBlueCnt++;
			}
			else if(item->GetTeam() == TEAM_RED )
			{
				iRedCnt++;
			}
			else
			{
				if( iBlueCnt > iRedCnt )
					iRedCnt++;
				else
					iBlueCnt++;
			}
		}
		else
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Room::NextShamShuffle() BLUE PARTY ERROR : %s 유저 파티 포인터 NULL!!", item->GetPublicID().c_str() );
		}
	}
	LOOP_GUARD_CLEAR();

	ShuffleRoomNode *pNode = dynamic_cast<ShuffleRoomNode*>( pShuffleRoom );
	if( pNode )
	{
		if( GetJoinUserCnt() <= 1 )
		{
			SP2Packet rkPacket( STPK_BATTLE_ROOM_END );
			RoomSendPacketTcp( rkPacket );
		}
		else if( pNode->IsShufflePhaseEnd() )
		{
			pNode->KickOutHighLevelUser();
			pNode->RestartShuffleMode();

			//게임시작 실패시 강퇴처리
			if( !pNode->ShuffleRoomReadyGo() )
				pNode->KickOutModeError();
		}
		else
		{			
			//게임시작 실패시 강퇴처리
			if( !pNode->ShuffleRoomReadyGo() )
				pNode->KickOutModeError();			
		}
	}
}

User* Room::FindUserInRoom( const ioHashString &rkName )
{
	CRASH_GUARD();
	int iUserCnt = GetJoinUserCnt();
	for( int i=0 ; i<iUserCnt ; i++ )
	{
		if( m_vUserNode[i]->GetPublicID() == rkName )
			return m_vUserNode[i];
	}

	return NULL;
}

void Room::FillSearchRoomInfo( SP2Packet &rkPacket )
{
	rkPacket << m_iRoomIndex;
	rkPacket << (int)m_pMode->GetModeType();

	if( m_pMode->GetModeType() == MT_TRAINING )
		rkPacket << m_iRoomNum;

	if( m_pMode )
	{
		rkPacket << m_pMode->GetCurRound();
		rkPacket << m_pMode->GetMaxRound();
		rkPacket << GetJoinUserCnt();
		rkPacket << GetPlayUserCnt();
	}
	else
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Room::FillSearchRoomInfo - %d Room Not Has Mode", m_iRoomIndex );
		rkPacket << 0 << 0 << 0 << 0;
	}
}

void Room::FillRoomAndUserInfo( SP2Packet &rkPacket )
{
	rkPacket << GetAverageLevel() << GetJoinUserCnt();

	vUser_iter iter, iEnd;
	iEnd = m_vUserNode.end();
	for( iter=m_vUserNode.begin() ; iter!=iEnd ; ++iter )
	{
		User *pUser = *iter;
		if( pUser )
			pUser->FillRoomAndUserInfo( rkPacket );
	}
}

void Room::FillPlazaInfo( SP2Packet &rkPacket )
{
	rkPacket << m_szRoomName << m_szMasterUserID << IsRoomPW() << GetMaxPlayer() << GetPlazaRoomLevel();
}

void Room::FillHeadquartersInfo( const ioHashString &rkMasterName, bool bLock, SP2Packet &rkPacket )
{
	HeadquartersMode *pHeadquarters = ToHeadquartersMode( m_pMode );
	if( pHeadquarters == NULL )
	{
		rkPacket << rkMasterName << MAX_PLAZA_PLAYER << bLock << 0;  
	}
	else
	{
		rkPacket << pHeadquarters->GetHeadquartersMaster() << pHeadquarters->GetMaxPlayer() << pHeadquarters->IsHeadquartersJoinLock();
		rkPacket << (int)m_vUserNode.size();

		vUser_iter iter, iEnd;
		iEnd = m_vUserNode.end();
		for( iter=m_vUserNode.begin() ; iter!=iEnd ; ++iter )
		{
			User *pUser = *iter;
			if( pUser )
			{
				rkPacket << pUser->GetGradeLevel() << pUser->GetPublicID() << pUser->GetGuildIndex() << pUser->GetGuildMark() << pUser->GetPingStep();
			}
			else
			{
				rkPacket << 0 << "" << 0 << 0 << 0;
			}
		}
	}
}

ioItem* Room::CreateItem( const ITEM_DATA &rkData,
						  const ioHashString &rkOwner )
{
	ioItem *pItem = m_pItemMaker->CreateItem( rkData.m_item_code );
	if( pItem )
	{
		pItem->SetItemData( rkData );
		pItem->SetOwnerName( rkOwner );
	}

	return pItem;
}

ioItem* Room::CreateItemByName( const ioHashString &rkName )
{
	return m_pItemMaker->CreateItem( rkName );
}

ioItem* Room::CreateItemByCode( int iItemCode )
{
	return m_pItemMaker->CreateItem( iItemCode );
}

void Room::DropItemOnField( User *pDroper,
						    ioItem *pDropItem,
							int iSlot,
							float fCurGauge,
							int iCurBullet )
{
	if( !pDroper || !pDropItem )
		return;

	AddFieldItem( pDropItem );

	SP2Packet kPacket( STPK_DROP_ITEM );
	kPacket << pDroper->GetPublicID();
	kPacket << iSlot;
	pDropItem->FillFieldItemInfo( kPacket );
	kPacket << iCurBullet;
	RoomSendPacketTcp( kPacket );

#ifdef FLAG_MODE_BY_BCKIM_DEBUG		// OnDropItem	
	if(pDropItem->GetItemCode() == 500004 )
	{
		LOG.PrintTimeAndLog(0,"[FLAG_MODE_BY_BCKIM:DropItemOnField] INFO>>CTPK_DROP_ITEM_333[%s] ",pDropItem->GetItemName().c_str());	
	}
#endif // FLAG_MODE_BY_BCKIM_DEBUG

}

void Room::DropItemOnField( const ioHashString &rkDroper, ioItem *pDropItem, int iSlot, float fCurGauge, int iCurBullet )
{
	if( !pDropItem )
		return;

	AddFieldItem( pDropItem );

	SP2Packet kPacket( STPK_DROP_ITEM );
	kPacket << rkDroper;
	kPacket << iSlot;
	pDropItem->FillFieldItemInfo( kPacket );
	kPacket << iCurBullet;
	RoomSendPacketTcp( kPacket );

#ifdef FLAG_MODE_BY_BCKIM_DEBUG		// OnDropItem	
	if(pDropItem->GetItemCode() == 500004 )
	{
		LOG.PrintTimeAndLog(0,"[FLAG_MODE_BY_BCKIM:DropItemOnField] INFO>>CTPK_DROP_ITEM_444[%s] ",pDropItem->GetItemName().c_str());	
	}
#endif // FLAG_MODE_BY_BCKIM_DEBUG

}

void Room::MoveDropItemOnField( User *pDroper, ioItem *pDropItem, int iSlot, float fCurGauge, int iCurBullet,
							    const ioHashString &szAttacker, const ioHashString &szSkillName, Vector3 vTargetPos, float fMoveSpeed )
{
	if( !pDroper || !pDropItem )
		return;

	AddFieldItem( pDropItem );

	SP2Packet kPacket( STPK_ITEM_MOVE_DROP );
	kPacket << pDroper->GetPublicID();
	kPacket << iSlot;
	pDropItem->FillFieldItemInfo( kPacket );
	kPacket << iCurBullet;
	kPacket << szAttacker;
	kPacket << szSkillName;
	kPacket << vTargetPos;
	kPacket << fMoveSpeed;
	RoomSendPacketTcp( kPacket );
}

void Room::MoveDropItemOnField( const ioHashString &rkDroper, ioItem *pDropItem, int iSlot, float fCurGauge, int iCurBullet,
							    const ioHashString &szAttacker, const ioHashString &szSkillName, Vector3 vTargetPos, float fMoveSpeed )
{
	if( !pDropItem )
		return;

	AddFieldItem( pDropItem );

	SP2Packet kPacket( STPK_ITEM_MOVE_DROP );
	kPacket << rkDroper;
	kPacket << iSlot;
	pDropItem->FillFieldItemInfo( kPacket );
	kPacket << iCurBullet;
	kPacket << szAttacker;
	kPacket << szSkillName;
	kPacket << vTargetPos;
	kPacket << fMoveSpeed;
	RoomSendPacketTcp( kPacket );
}

void Room::AddFieldItem( ioItem *pItem )
{
	if( pItem )
	{
		pItem->SetDropTime( TIMEGETTIME() );
		m_FieldItemList.push_back( pItem );
	}
}

void Room::RemoveFieldItem( ioItem *pItem )
{
	if( pItem )
	{
		pItem->SetDropTime( 0 );
		m_FieldItemList.remove( pItem );
	}
}

ioItem* Room::FindFieldItem( int iGameIndex )
{
	ItemList::iterator iter=m_FieldItemList.begin();
	for( ; iter!=m_FieldItemList.end() ; ++iter )
	{
		ioItem *pItem = *iter;
		if( pItem->GetGameIndex() == iGameIndex )
			return pItem;
	}

	return NULL;
}

void Room::DestroyAllFieldItems()
{
	ItemList::iterator iter=m_FieldItemList.begin();
	for( ; iter!=m_FieldItemList.end() ; ++iter )
	{
		delete *iter;
	}
	m_FieldItemList.clear();
}

void Room::ClearRemoveUserOwnItem( User *pOuter )
{
	ioHashString szOuter = pOuter->GetPublicID();

	vUser_iter iter;
	for( iter=m_vUserNode.begin() ; iter!=m_vUserNode.end() ; ++iter )
	{
		(*iter)->ClearRealEquipItemOwner( szOuter );
	}

	ItemList::iterator iItem;
	for( iItem=m_FieldItemList.begin() ; iItem!=m_FieldItemList.end() ; ++iItem )
	{
		(*iItem)->ClearOwnerNameIf( szOuter );
	}
}

void Room::SendFieldItemInfo( User *pSend /* = NULL  */ )
{
	// 필드 아이템 1개당 64byte : 15 * Item = 960byte - 15개씩 나눠 보냄
	const int iSendListSize = 15;
	ItemList::iterator iter = m_FieldItemList.begin();
	int iFieldItemCnt = m_FieldItemList.size();
	for(int iStartArray = 0;iStartArray < iFieldItemCnt;)
	{
		int iLoop = iStartArray;
		int iSendSize = min( iFieldItemCnt - iStartArray, iSendListSize );

		SP2Packet kPacket( STPK_FIELD_ITEM_LIST );
		kPacket << iSendSize;
		for(;iLoop < iStartArray + iSendSize;iLoop++,iter++)
		{
			ioItem *pItem = *iter;
			if( pItem == NULL )
			{
				kPacket << 0 << 0 << 0 << 0;
				kPacket << 0;
				kPacket << "";
				kPacket << 0.0f << 0.0f << 0.0f;
				kPacket << 0.0f;
			}
			else
			{
                pItem->FillFieldItemInfo( kPacket );
			}
		}
		if( pSend )
			pSend->SendMessage( kPacket );		
		else
			RoomSendPacketTcp( kPacket );;
		iStartArray = iLoop;
	}
}

int Room::GetJoinUserCnt()
{
	return m_vUserNode.size();
}

int Room::GetPlayUserCnt()
{
	int iPlayUserCnt = 0;
	vUser_iter iter, iEnd;
	iEnd = m_vUserNode.end();
	for( iter=m_vUserNode.begin() ; iter!=iEnd ; ++iter )
	{
		User *pUser = *iter;
		if( !pUser ) continue;
		if( pUser->IsObserver() ) continue;
		if( pUser->IsStealth() ) continue;

		iPlayUserCnt++;
	}

	return iPlayUserCnt;
}

bool Room::IsPartyProcessEnd() const
{
	return m_bPartyProcessEnd;
}

bool Room::IsRoomFull()
{
	HeadquartersMode *pHeadquarters = ToHeadquartersMode( m_pMode );
	if( pHeadquarters )
	{
		int iReserveUser = 1;  // 본부 주인은 예약되어있다.
		if( pHeadquarters->IsMasterJoin() )
			iReserveUser = 0;

		if( GetPlayUserCnt() + GetReserveUserSize() + iReserveUser >= GetMaxPlayer() )
			return true;
	}
	else
	{
		if( GetPlayUserCnt() + GetReserveUserSize() >= GetMaxPlayer() )
			return true;
	}

	return false;
}

int Room::GetMaxPlayer()
{
	if( m_pMode == NULL ) 
		return 0;

	return max( GetPlayUserCnt() + GetReserveUserSize(), m_pMode->GetMaxPlayer() );
}

bool Room::IsCharLimitCheck( User *pOwner )
{
	if( !m_pMode ) return false;

	ModeRecord *pRecord = m_pMode->FindModeRecord( pOwner );
	if( !pRecord ) return false;
	if( pRecord->eState != RS_PLAY ) return false;

	if( m_pMode->GetState() == Mode::MS_READY || m_pMode->GetState() == Mode::MS_PLAY ) 
	{
		return !m_pMode->IsPlayCharHireTimeStop( pRecord );
	}

	return false; 
}

DWORD Room::GetCharLimitCheckTime()
{
	if( !m_pMode ) return 0;
	
	return m_pMode->GetCharLimitCheckTime();
}

void Room::SetMaxPlayer( int iMaxPlayer )
{
	if( m_pMode )
	{
		m_pMode->SetMaxPlayer( max( GetPlayUserCnt() + GetReserveUserSize(), iMaxPlayer ) );			
		m_SyncUpdate.Update( RoomSync::RS_CURUSER );
	}
}

bool Room::IsRoomEnterUserDelay()
{
	/* 유저가 다른 서버에 룸을 생성하고 서버 이동할 때 까지의 룸들은 대기 상태여야한다.
	   룸검색이나 룸삭제가 되어버리면 안된다. */
	if( GetRoomStyle() == RSTYLE_NONE ) return false;
	if( m_vReserveUser.empty() ) return false;
	if( m_pMode ) 
	{
		if( m_pMode->GetState() != Mode::MS_READY ) return false;
		if( m_pMode->GetCurRound() > 1 ) return false;
	}

	return true;
}

bool Room::IsTimeCloseRoom() const
{
	if( m_pMode == NULL ) 
		return true;
	
	return m_pMode->IsTimeClose();
}

PlazaType Room::GetPlazaModeType() const
{
	if( !m_pMode || m_pMode->GetModeType() != MT_TRAINING )
		return PT_NONE;
	return m_ePlazaType;
}

bool Room::PassCreateGuildRoomDelayTime()
{
	if( TIMEGETTIME() - m_dwCreateTime < 90000 )        // 1분 30초간 유예시간 
	{
		return false;
	}

	return true;
}

bool Room::IsRoomEmpty()
{
	if( TIMEGETTIME() - m_dwCreateTime < 90000 )        // 1분 30초간 유예시간 
	{
		if( IsRoomEnterUserDelay() ) return false;
	}

	return m_vUserNode.empty();
}

bool Room::IsFinalRound() const
{
	if( m_pMode )
	{
		if( ToCatchMode(m_pMode) )
			return false;

		if( ToUnderwearMode(m_pMode) )
			return false;

#ifdef FARMING_MODE_BY_BCKIM		// 2019-07-03 by bckim, 파밍모드 추가 
		if( ToFarmingMode(m_pMode) )
			return false;
#endif // End. 2019-07-03 by bckim, 파밍모드 추가

		if( ToCBTMode(m_pMode) )
			return false;

		if( m_pMode->GetCurRound() == m_pMode->GetMaxRound() )
			return true;
	}

	return false;
}

bool Room::IsFinalRoundResult()
{
	if( m_pMode )
	{
		return m_pMode->IsRoundSetEnd();
	}
	return false;
}

int Room::GetAverageLevel()
{	
	if( m_bSafetyLevelRoom )
		return 0;

	int iSize  = 0;
	int iLevel = 0;
	int iObserverSize = 0;
	int iObserverLevel = 0;

	vUser_iter iter, iEnd;
	iEnd = m_vUserNode.end();

	for(iter=m_vUserNode.begin();iter!=iEnd;++iter)
	{
		User *pUser = *iter;
		if( pUser )
		{
			// 옵저버 제외 레벨
			if( !pUser->IsDeveloper() && !pUser->IsObserver() && !pUser->IsStealth() )
			{
				iLevel += pUser->GetKillDeathLevel();
				iSize++;
			}
		}
	}

	for(iter=m_vUserNode.begin();iter!=iEnd;++iter)
	{
		User *pUser = *iter;
		if( pUser )
		{
			// 옵저버 제외 레벨
			if( !pUser->IsDeveloper() && pUser->IsObserver() )
			{
				iObserverLevel += pUser->GetKillDeathLevel();
				iObserverSize++;
			}
		}
	}

	if( iSize <= 0 && iObserverSize > 0 )
	{
		iObserverLevel /= iObserverSize;
		iObserverLevel = max( g_LevelMatchMgr.GetRoomEnterSafetyLevel(), iObserverLevel );
		return iObserverLevel;
	}
	else if( iSize <= 0 && iObserverSize <= 0 )
	{
		return 0;
	}

	iLevel /= iSize;
	iLevel = max( g_LevelMatchMgr.GetRoomEnterSafetyLevel(), iLevel );
	return iLevel;
}

int Room::GetGapLevel( int iMyLevel )
{
	return abs( GetAverageLevel() - iMyLevel );
}

bool Room::IsSafetyLevelCheck( int iMyLevel, int iSafetyLevel )
{
	if( iMyLevel < iSafetyLevel )    //쪼랩
	{
		if( GetAverageLevel() < iSafetyLevel )
			return true;
	}
	else                                                          //고랩 
	{
		if( GetAverageLevel() >= iSafetyLevel )
			return true;
	}
	return false;
}

int Room::GetTeamRatePoint()
{
	if( m_vUserNode.empty() )
		return -1;    //들어갈 수 없는 방.
	if( m_vRoomEnterTeamRate.empty() )
		return 0;     //무조건 들어갈 수 있는 방.

	if( GetModeType() != MT_SURVIVAL && GetModeType() != MT_BOSS && GetModeType() != MT_GANGSI && GetModeType() != MT_FIGHT_CLUB && GetModeType() == MT_SHUFFLE_BONUS )
	{
		int iBlueTeam = 0;
		int iRedTeam  = 0;
		vUser_iter iter, iEnd;
		iEnd = m_vUserNode.end();
		for( iter=m_vUserNode.begin() ; iter!=iEnd ; ++iter )
		{
			User *pUser = *iter;
			if( !pUser ) continue;

			if( pUser->GetTeam() == TEAM_BLUE )
				iBlueTeam++;
			else if( pUser->GetTeam() == TEAM_RED )
				iRedTeam++;			
		}
		if( iBlueTeam == 0 || iRedTeam == 0 )
			return -1;

		// 5:1 -> 5:2 -> 4:1 -> 4:2 -> 4:3 -> 3:2 -> 3:1 -> 3:3 -> 2:1 -> 2:2 -> 5:3 -> 1:1 -> 5:4 -> 4:4
		char szTeamRate[MAX_PATH] = "";
		if( iBlueTeam > iRedTeam )
			sprintf_s( szTeamRate, "%d:%d", iBlueTeam, iRedTeam );
		else
			sprintf_s( szTeamRate, "%d:%d", iRedTeam, iBlueTeam );

		CRASH_GUARD();
		int iSize = m_vRoomEnterTeamRate.size();
		for(int i = 0;i < iSize;i++)
		{
			EnterTeamRate &kETR = m_vRoomEnterTeamRate[i];
			if( kETR.szRate == szTeamRate )
				return kETR.iRatePoint;
		}

		return -1;
	}

	// 서바이벌 모드
	return 1;
}

void Room::SetRoomStyle( RoomStyle style )
{
	m_room_style = style;
}

void Room::SetHeadquartersMaster( const ioHashString &rkName )
{
	if( m_pMode )
	{
		HeadquartersMode *pHeadquarters = ToHeadquartersMode( m_pMode );
		if( pHeadquarters )
		{	
			pHeadquarters->SetHeadquartersMaster( rkName );
		}
		else
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Room::SetHeadquartersMaster None HQ Mode :%s - %d", rkName.c_str(), (int)GetModeType() );
		}
	}
	else
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Room::SetHeadquartersMaster None Mode :%s", rkName.c_str() );
	}
}

void Room::CreateHeadquartersCharacter( User *pUser )
{
	if( pUser == NULL )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Room::CreateHeadquartersCharacter Null User" );
		return;
	}

	if( m_pMode )
	{
		HeadquartersMode *pHeadquarters = ToHeadquartersMode( m_pMode );
		if( pHeadquarters )
		{	
			pHeadquarters->CreateCharacter( pUser );
		}
		else
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Room::CreateHeadquartersCharacter None HQ Mode :%s - %d", pUser->GetPublicID().c_str(), (int)GetModeType() );
		}
	}
	else
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Room::CreateHeadquartersCharacter None Mode :%s", pUser->GetPublicID().c_str() );
	}
}

void Room::SetHeadquartersCharState( DWORD dwState )
{
	if( m_pMode )
	{
		HeadquartersMode *pHeadquarters = ToHeadquartersMode( m_pMode );
		if( pHeadquarters )
		{	
			pHeadquarters->SetCharState( dwState );
		}
		else
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Room::SetHeadquartersCharState None HQ Mode :%d - %d", dwState, (int)GetModeType() );
		}
	}
	else
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Room::SetHeadquartersCharState None Mode :%d", dwState );
	}
}

void Room::AddTeamKillPoint( TeamType eTeam, int iKillPoint )
{
	if( m_pMode )
		m_pMode->AddTeamKillPoint( eTeam, iKillPoint );
}

void Room::AddTeamDeathPoint( TeamType eTeam, int iDeathPoint )
{
	if( m_pMode )
		m_pMode->AddTeamDeathPoint( eTeam, iDeathPoint );
}

bool Room::ProcessTCPPacket( User *pSend, SP2Packet &rkPacket )
{
	switch( rkPacket.GetPacketID() )
	{
	case CTPK_DROP_DIE:
		OnDropDie( pSend, rkPacket );
#ifdef ANTIHACK
		pSend->SetDieState();
#endif
		return true;
	case CTPK_WEAPON_DIE:
		OnWeaponDie( pSend, rkPacket );
#ifdef ANTIHACK
		pSend->SetDieState();
#endif
		return true;
	case CTPK_PASSAGE:
		OnPassage( pSend, rkPacket );
		return true;
	case CTPK_REQUEST_REVIVAL_TIME:
		OnRequestRevivalTime( pSend, rkPacket );
		return true;
	case CTPK_LADDER_BATTLE_RESTART:
		OnLadderBattleRestart( pSend, rkPacket );
		return true;
	case CTPK_ETCITEM_MOTION_STATE:
		OnEtcItemMotionState( pSend, rkPacket );
		return true;
	case CTPK_MACRO_NOTIFY:
		OnMacroNotify( pSend, rkPacket );
		return true;
	default:
		if( m_pMode )
		{
			return m_pMode->ProcessTCPPacket( pSend, rkPacket );
		}
		return false;
	}

	return false;
}

void Room::OnDropDie( User *pSend, SP2Packet &rkPacket )
{
	if( m_pMode == NULL ) return;

	ioHashString szDieChar;
	rkPacket >> szDieChar;
	ModeRecord *pRecord = m_pMode->FindModeRecord( szDieChar );
	if( pRecord )
	{
		// 유저가 죽었다
		m_pMode->OnDropDieUser( pRecord->pUser, rkPacket );
	}
	else
	{
		// NPC가 죽었다
		m_pMode->OnDropDieNpc( szDieChar, rkPacket );
	}
}

void Room::OnWeaponDie( User *pSend, SP2Packet &rkPacket )
{
	if( m_pMode == NULL ) return;

	ioHashString szDieChar;
	rkPacket >> szDieChar;
	ModeRecord *pRecord = m_pMode->FindModeRecord( szDieChar );
	if( pRecord )
	{
		// 유저가 죽었다
		m_pMode->OnWeaponDieUser( pRecord->pUser, rkPacket );

#ifdef FLAG_MODE_BY_BCKIM
		if ( m_pMode->GetModeType() == MT_FLAG ) 
	{
			FlagMode *pFlag = ToFlagMode( m_pMode );
			pFlag->FlagModeAddPoint(pRecord->pUser->GetTeam(), pSend);

#ifdef FLAG_MODE_BY_BCKIM_DEBUG
			LOG.PrintTimeAndLog(0,"[FLAG_MODE_BY_BCKIM:OnWeaponDie]:ID(%s), DIE_ID(%s)",pSend->GetPublicID().c_str(), szDieChar.c_str());
#endif // FLAG_MODE_BY_BCKIM_DEBUG

		}
#endif 

	}
	else
	{	// NPC가 죽었다
		m_pMode->OnWeaponDieNpc( szDieChar, rkPacket );
	}
}

void Room::OnPassage( User *pSend, SP2Packet &rkPacket )
{
	ioHashString szTargetUser;
	rkPacket >> szTargetUser;
	
	User *pUser = FindUserInRoom( szTargetUser );
	if( !pUser )
	{
		int iSubType;
		ioHashString szSender;
		rkPacket >> szSender >> iSubType;
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Room::OnPassage() RoomIndex:%d - (%s) is Not Exist : Sender:%s - SubType:%d", GetRoomIndex(), szTargetUser.c_str(), szSender.c_str(), iSubType );
		return;
	}

	SP2Packet kReturnPacket( STPK_PASSAGE );
	kReturnPacket.SetDataAdd( (char*)rkPacket.GetData(), rkPacket.GetDataSize() );
	pUser->SendMessage( kReturnPacket );
}

void Room::OnRequestRevivalTime( User *pSend, SP2Packet &rkPacket )
{
	if( !m_pMode ) return;

	m_pMode->UpdateUserDieTime( pSend );

	SP2Packet kReturn( STPK_ANSWER_REVIVAL_TIME );
	kReturn << pSend->GetPublicID();
	m_pMode->GetRevivalTime( kReturn, pSend->GetPublicID() );
	RoomSendPacketTcp( kReturn );
}

void Room::OnLadderBattleRestart( User *pSend, SP2Packet &rkPacket )
{
	bool bRestart;
	rkPacket >> bRestart;

	if( GetRoomStyle() != RSTYLE_LADDERBATTLE ) return;

	if( bRestart )
	{
		LadderTeamParent *pLadderTeam = pSend->GetMyLadderTeam();
		if( pLadderTeam )
		{
			if( pLadderTeam->GetIndex() == m_MainLadderTeam.m_dwTeamIndex )
				m_MainLadderTeam.m_bRestart = true;
			else if( pLadderTeam->GetIndex() == m_SubLadderTeam.m_dwTeamIndex )
				m_SubLadderTeam.m_bRestart = true;
		}			

		if( m_MainLadderTeam.m_bRestart && m_SubLadderTeam.m_bRestart )
		{
			LadderTeamParent *pLadderMainTeam = g_LadderTeamManager.GetGlobalLadderTeamNode( m_MainLadderTeam.m_dwTeamIndex );
			LadderTeamParent *pLadderSubTeam  = g_LadderTeamManager.GetGlobalLadderTeamNode( m_SubLadderTeam.m_dwTeamIndex );
			
			if( !pLadderMainTeam || !pLadderSubTeam || !pLadderMainTeam->IsOriginal() )
			{
				// 양팀 모두 광장으로 이동 : 예외
				SP2Packet kPacket( STPK_LADDERTEAM_MACRO );
				kPacket << LADDERTEAM_MACRO_MODE_END << 0 << 0;
				RoomSendPacketTcp( kPacket );

				LadderTeamLeaveRoom();
				return;
			}

			LadderTeamNode *pOriginalNode = (LadderTeamNode*)pLadderMainTeam;
			pOriginalNode->MatchReStartRoom( this, pLadderSubTeam->GetIndex(), pLadderSubTeam->GetSelectMode(), pLadderSubTeam->GetSelectMap() );
		}
	}
	else if( m_MainLadderTeam.m_dwTeamIndex != 0 || m_SubLadderTeam.m_dwTeamIndex != 0 )
	{
		// 양팀 모두 광장으로 이동
		SP2Packet kPacket( STPK_LADDERTEAM_MACRO );
		kPacket << LADDERTEAM_MACRO_MODE_END << 0 << 0;
		RoomSendPacketTcp( kPacket );

		LadderTeamLeaveRoom();

		m_MainLadderTeam.Init();
		m_SubLadderTeam.Init();
	}
}

void Room::OnEtcItemMotionState( User *pSend, SP2Packet &rkPacket )
{
	int iEtcItemCode;
	ioHashString kSyncName;
	rkPacket >> kSyncName >> iEtcItemCode;

	SP2Packet kPacket( STPK_ETCITEM_MOTION_STATE );
	kPacket << pSend->GetPublicID() << iEtcItemCode;
	RoomSendPacketTcpSenderUser( kPacket, kSyncName );
}

void Room::OnMacroNotify( User *pSend, SP2Packet &rkPacket )
{
	if( pSend == NULL ) return;

	ioHashString kUserID;
	rkPacket >> kUserID;

	User *pUser = GetUserNode( kUserID );
	if( pUser == NULL ) return;

	SP2Packet kPacket( STPK_MACRO_NOTIFY );
	kPacket << pSend->GetPublicID();
	pUser->SendMessage( kPacket );
}

bool Room::OnModeChangeChar( User *pSend, int iCharArray, bool bWait, int iSelectCharArray, DWORD dwCharChangeIndex )
{
	if( m_pMode == NULL ) return false;

	return m_pMode->OnModeChangeChar( pSend, iCharArray, bWait, iSelectCharArray, dwCharChangeIndex );
}

void Room::OnModeChangeDisplayMotion( User *pSend, DWORD dwEtcItem, int iClassType )
{
	if( m_pMode == NULL ) return;

	return m_pMode->OnModeChangeDisplayMotion( pSend, dwEtcItem, iClassType );
}

void Room::OnModeCharDecoUpdate( User *pSend, ioCharacter *pCharacter )
{
	if( m_pMode == NULL ) return;

	m_pMode->OnModeCharDecoUpdate( pSend, pCharacter );
}

void Room::OnModeCharExtraItemUpdate( User *pSend, DWORD dwCharIndex, int iSlot, int iNewIndex )
{
	if( m_pMode == NULL ) return;

	m_pMode->OnModeCharExtraItemUpdate( pSend, dwCharIndex, iSlot, iNewIndex );
}

void Room::OnModeCharMedalUpdate( User *pSend, DWORD dwCharIndex, int iMedalType, bool bEquip )
{
	if( m_pMode == NULL ) return;

	m_pMode->OnModeCharMedalUpdate( pSend, dwCharIndex, iMedalType, bEquip );
}

void Room::OnModeCharCustomMedalUpdate( User *pSend, DWORD dwCharIndex, CustomMedal* pInfo )
{
	if( m_pMode == NULL ) return;

	m_pMode->OnModeCharCustomMedalUpdate( pSend, dwCharIndex, pInfo );
}

void Room::OnModeCharGrowthUpdate( User *pSend, int iClassType, int iSlot, bool bItem, int iUpLevel )
{
	if( m_pMode == NULL ) return;

	m_pMode->OnModeCharGrowthUpdate( pSend, iClassType, iSlot, bItem, iUpLevel );
}

void Room::OnModeCharInsert( User *pSend, ioCharacter *pCharacter )
{
	if( m_pMode == NULL ) return;

	m_pMode->OnModeCharInsert( pSend, pCharacter );
}

void Room::OnModeCharDelete( User *pSend, DWORD dwCharIndex )
{
	if( m_pMode == NULL ) return;

	m_pMode->OnModeCharDelete( pSend, dwCharIndex );
}

void Room::OnModeCharDisplayUpdate( User *pSend, DWORD dwCharIndex )
{
	if( m_pMode == NULL ) return;

	m_pMode->OnModeCharDisplayUpdate( pSend, dwCharIndex );
}

void Room::OnModeJoinLockUpdate( User *pSend, bool bJoinLock )
{
	if( m_pMode == NULL ) return;

	m_pMode->OnModeJoinLockUpdate( pSend, bJoinLock );
}

void Room::OnModeLogoutAlarm( const ioHashString &rkMasterName )
{
	if( m_pMode == NULL ) return;

	m_pMode->OnModeLogoutAlarm( rkMasterName );
}

void Room::LadderTeamLeaveRoom()
{
	// 래더팀 레디 상태로 전환
	LadderTeamParent *pLadderMainTeam = g_LadderTeamManager.GetGlobalLadderTeamNode( m_MainLadderTeam.m_dwTeamIndex );
	if( pLadderMainTeam )
		pLadderMainTeam->MatchPlayEndSync();

	LadderTeamParent *pLadderSubTeam = g_LadderTeamManager.GetGlobalLadderTeamNode( m_SubLadderTeam.m_dwTeamIndex );
	if( pLadderSubTeam )
		pLadderSubTeam->MatchPlayEndSync();
}

ModeType Room::CheckNextMode()
{
	if( !m_pModeSelector )
		return MT_TRAINING;

	ModeType eModeType = m_pModeSelector->GetCurModeType();
	
	if( eModeType == MT_NONE )
		return MT_TRAINING;

	return eModeType;
}

int Room::GetModeMinUserCnt()
{
	if( m_pModeSelector )
		return m_pModeSelector->GetModeMinUserCnt();

	return 0;
}

int Room::GetLadderTeamLevel( TeamType eTeam )
{
	if( GetRoomStyle() != RSTYLE_LADDERBATTLE ) return 0;

	if( m_MainLadderTeam.m_eTeamType == eTeam )
		return m_MainLadderTeam.m_iAbilityMatchLevel;
	else if( m_SubLadderTeam.m_eTeamType == eTeam )
		return m_SubLadderTeam.m_iAbilityMatchLevel;
	return 0;
}

float Room::GetCampPointBonus( TeamType eTeam )
{
	if( GetRoomStyle() != RSTYLE_LADDERBATTLE ) return 1.0f;

	if( m_MainLadderTeam.m_eTeamType == eTeam )
		return m_MainLadderTeam.m_fCampPointBonus;
	else if( m_SubLadderTeam.m_eTeamType == eTeam )
		return m_SubLadderTeam.m_fCampPointBonus;
	return 1.0f;
}

float Room::GetGuildBonus( TeamType eTeam )
{
	if( GetRoomStyle() != RSTYLE_LADDERBATTLE ) return 0.0f;

	if( m_MainLadderTeam.m_eTeamType == eTeam )
		return m_MainLadderTeam.m_fGuildBonus;
	else if( m_SubLadderTeam.m_eTeamType == eTeam )
		return m_SubLadderTeam.m_fGuildBonus;
	return 0.0f;
}

bool Room::IsLadderGuildTeam( TeamType eTeam )
{
	if( GetRoomStyle() != RSTYLE_LADDERBATTLE ) return false;

	if( m_MainLadderTeam.m_eTeamType == eTeam )
	{
		if( m_MainLadderTeam.m_dwGuildIndex != 0 )
			return true;
	}
	else if( m_SubLadderTeam.m_eTeamType == eTeam )
	{
		if( m_SubLadderTeam.m_dwGuildIndex != 0 )
			return true;
	}
	return false;

}

int Room::GetModeSubNum()
{
	if( m_pMode )
		return m_pMode->GetModeSubNum();

	return 0;
}


int Room::GetModeMapNum()
{
	if( m_pMode )
		return m_pMode->GetModeMapNum();

	return 0;
}

int Room::GetBlueWinCnt()
{
	if( m_pMode )
		return m_pMode->GetBlueTeamWinCnt();

	return 0;
}

int Room::GetRedWinCnt()
{
	if( m_pMode )
		return m_pMode->GetRedTeamWinCnt();

	return 0;
}

DWORD Room::GetRemainPlayTime()
{
	if( m_pMode )
		return m_pMode->GetRemainPlayTime();
	return 0;
}

int Room::GetNextModeSubNum() const
{
	if( !m_pModeSelector )
		return 0;

	int iSubModeType = m_pModeSelector->GetCurSubModeType();
	
	if( iSubModeType == -1 )
		iSubModeType = 0;
	
	return iSubModeType;
}

int Room::GetNextModeMapNum() const
{
	if( !m_pModeSelector )
		return 0;

	int iMapNum = m_pModeSelector->GetCurMapNum();
	
	if( iMapNum == -1 )
		iMapNum = 0;
	
	return iMapNum;
}

TeamType Room::GetNextTeamType()
{
	if( !m_pMode )
		return TEAM_NONE;
	return m_pMode->GetNextTeamType();
}

TeamType Room::GetNextGuildUserTeamType( DWORD dwGuildIndex )
{
	vUser_iter iter = m_vUserNode.begin();
	for(;iter != m_vUserNode.end();++iter)
	{
		User *pExist = *iter;
		if( pExist->GetGuildIndex() == dwGuildIndex )
			return pExist->GetTeam();
	}
	return GetNextTeamType();
}

void Room::NextShamBattleRandomTeam( ModeType eModeType )
{
	// 팀원 섞어서 시작 옵션 활성화 처리	
	if( eModeType == MT_SURVIVAL || eModeType == MT_BOSS || eModeType == MT_GANGSI || eModeType == MT_FIGHT_CLUB ) return;
	
	if( m_vUserNode.empty() ) return;

	User *pFirstUser = m_vUserNode[0];
	if( !pFirstUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Room::NextShamBattleRandomTeam() First User NULL!!!" );
		return;
	}

	BattleRoomParent *pBattleRoom = pFirstUser->GetMyBattleRoom();
	if( pBattleRoom && pBattleRoom->IsOriginal() )		
	{
		bool bRandom = false;
		if( pBattleRoom->GetMaxPlayerBlue() != pBattleRoom->GetMaxPlayerRed() )
			bRandom = true;

		UserRankInfoList kUserRankList;
		m_pMode->GetUserRankByNextTeam( kUserRankList, bRandom );

		BattleRoomNode *pOriginal = (BattleRoomNode*)pBattleRoom;
		pOriginal->BattleEnterRandomTeam( kUserRankList );
	}	
	else
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Room::NextShamBattleRandomTeam() BattleRoom NULL!!!" );
}

void Room::CreateNextShamBattle()
{
	m_bRoomProcess = true;
	if( !m_pMode ) return;

	m_bCharChangeToUDP = Help::IsCharChangeToUDP();
	m_bOnlyServerRelay = Help::IsOnlyServerRelay();
	int iSubNum = m_pMode->GetModeSubNum();
	int iMapIndex = m_pMode->GetModeMapNum();

	SP2Packet kStartSetNextMode( STPK_START_SET_NEXT_MODE );
	kStartSetNextMode << GetModeType();
	kStartSetNextMode << iSubNum;
	kStartSetNextMode << iMapIndex;
	kStartSetNextMode << (int)GetPlazaModeType();
	RoomSendPacketTcp(kStartSetNextMode);

	// 룸 정보 전송.
	SP2Packet kJoinRoomPk( STPK_JOIN_ROOMDATA );
	kJoinRoomPk << m_iRoomIndex;
	kJoinRoomPk << m_iRoomNum;
	kJoinRoomPk << (int)m_room_style;
	kJoinRoomPk << m_bCharChangeToUDP;
	kJoinRoomPk << m_bOnlyServerRelay;
	kJoinRoomPk << iSubNum;
	kJoinRoomPk << iMapIndex;
	kJoinRoomPk << m_pMode->IsZeroHP();
	kJoinRoomPk << false;
	if( m_room_style == RSTYLE_PLAZA )
		kJoinRoomPk<<(int)GetPlazaModeType();
	m_pMode->GetModeInfo( kJoinRoomPk );
	RoomSendPacketTcp( kJoinRoomPk );
#ifdef ANTIHACK
	NextShamBattleRUDP();
#endif

	vUser_iter iter, iEnd;
	iEnd = m_vUserNode.end();
	for( iter=m_vUserNode.begin() ; iter!=iEnd ; ++iter )
	{
		User *pExist = *iter;

		if( !pExist )
			continue;

		pExist->ReleaseEquipAllChar();
		pExist->ClearCharJoinedInfo();

		if( pExist->IsStealth() )
		{
			pExist->SetTeam( TEAM_NONE );
			LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "스텔스 입장 유저 : [%d] - %d - %s", GetRoomIndex(), (int)GetModeType(), pExist->GetPublicID().c_str() );
		}
		else if( GetModeType() == MT_SURVIVAL || GetModeType() == MT_MONSTER_SURVIVAL || GetModeType() == MT_DUNGEON_A || GetModeType() == MT_FIGHT_CLUB || 
			GetModeType() == MT_RAID ||
				 Help::IsMonsterDungeonMode(GetModeType()))
		{
			if( pExist->IsObserver() )
				pExist->SetTeam( TEAM_NONE );
			else
				pExist->SetTeam( GetNextTeamType() );
		}
		else if( GetModeType() == MT_BOSS )
		{
			if( pExist->IsObserver() )
				pExist->SetTeam( TEAM_NONE );
			else
			{
				TeamType eTeam = TEAM_RED;
				BattleRoomParent *pBattleRoom = pExist->GetMyBattleRoom();
				if( pBattleRoom && pBattleRoom->IsOriginal() )
				{
					// 보스 선정된 유저는 블루팀
					BattleRoomNode *pOriginal = (BattleRoomNode*)pBattleRoom;
					if( pOriginal && pOriginal->GetBossName() == pExist->GetPublicID() && m_pMode->GetTeamUserCnt( TEAM_BLUE ) == 0 )          //블루 팀은 오직 한명만 가능
					{
						eTeam = TEAM_BLUE;
						pOriginal->ClearBossName();
					}
				}
				else
				{
					LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "전투방 플레이 입장시 파티가 없음 : [%d] - %d - %s", GetRoomIndex(), (int)GetModeType(), pExist->GetPublicID().c_str() );
				}
				pExist->SetTeam( eTeam );
			}
		}
		else if( GetModeType() == MT_GANGSI )
		{
			if( pExist->IsObserver() )
				pExist->SetTeam( TEAM_NONE );
			else
			{
				TeamType eTeam = TEAM_RED;
				BattleRoomParent *pBattleRoom = pExist->GetMyBattleRoom();
				if( pBattleRoom && pBattleRoom->IsOriginal() )
				{
					// 강시 선정된 유저는 블루팀
					BattleRoomNode *pOriginal = (BattleRoomNode*)pBattleRoom;
					if( pOriginal && pOriginal->GetGangsiName() == pExist->GetPublicID() && 
						m_pMode->GetTeamUserCnt( TEAM_BLUE ) == 0 )          //블루 팀은 오직 한명만 가능
					{
						eTeam = TEAM_BLUE;
						pOriginal->ClearGangsi();
					}
				}
				else
				{
					LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "전투방 플레이 입장시 파티가 없음 : [%d] - %d - %s", 
						GetRoomIndex(), (int)GetModeType(), pExist->GetPublicID().c_str() );
				}
				pExist->SetTeam( eTeam );
			}
		}
		else if( GetModeType() == MT_TEAM_SURVIVAL_AI )
		{
			if( pExist->IsObserver() )
				pExist->SetTeam( TEAM_NONE );
			else
				pExist->SetTeam( GetNextTeamType() );
		}
		else
		{
			BattleRoomParent *pBattleRoom = pExist->GetMyBattleRoom();
			if( pBattleRoom && pBattleRoom->IsOriginal() )
			{
				BattleRoomNode *pOriginal = (BattleRoomNode*)pBattleRoom;
				if( pOriginal )
				{
					TeamType eTeam = pOriginal->GetUserTeam( pExist->GetPublicID() );
					pExist->SetTeam( eTeam );
					if( eTeam != TEAM_BLUE && eTeam != TEAM_RED )
					{
						LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "전투방 플레이 다음 세트 시작시 팀이없음 : [%d] - %d - %s - %d - %d", 
							GetRoomIndex(), (int)GetModeType(), pExist->GetPublicID().c_str(), (int)eTeam, (int)pBattleRoom->GetIndex() );
					}
				}
			}
			else
			{
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "전투방 플레이 다음 세트 시작시 파티가 없음 : [%d] - %d - %s", GetRoomIndex(), (int)GetModeType(), pExist->GetPublicID().c_str() );
			}
		}		

		SP2Packet kExistUserPk( STPK_JOIN_USERDATA );
		pExist->FillJoinUserData( kExistUserPk, IsExperienceUser( pExist ) );
		kExistUserPk << pExist->GetMyVictories();
		kExistUserPk << false;		// Now Not Playing..
		kExistUserPk << pExist->GetKillDeathLevel() << pExist->GetLadderPoint();
		kExistUserPk << false;		// chatmode
		kExistUserPk << false;      // experience mode
		kExistUserPk << false;		// fishing
		kExistUserPk << 0;			// fishing rod type
		kExistUserPk << 0;			// fishing bait type
		kExistUserPk << pExist->IsStealth();

		RoomSendPacketTcp( kExistUserPk );

		SP2Packet kJoinUserPetPk( STPK_PET_EQUIP_INFO );
#if defined( DIVIDE_PET ) 
		int iClassType = 0;
		ioCharacter *rkChar = pExist->GetCharacter( pExist->GetSelectChar() );
		if( rkChar )	
			iClassType = rkChar->GetCharInfo().m_class_type;

		if( !pExist->FillEquipPetData( kJoinUserPetPk, iClassType ) )
		{
			PACKET_GUARD_VOID( kJoinUserPetPk.Write(  pExist->GetPublicID() ) );
			PACKET_GUARD_VOID( kJoinUserPetPk.Write( 0 ) );
			PACKET_GUARD_VOID( kJoinUserPetPk.Write( 0 ) );
			PACKET_GUARD_VOID( kJoinUserPetPk.Write( 0 ) );
			PACKET_GUARD_VOID( kJoinUserPetPk.Write( iClassType ) );
			PACKET_GUARD_VOID( kJoinUserPetPk.Write( false ) );
		}
		RoomSendPacketTcp( kJoinUserPetPk );
#else
		if( pExist->FillEquipPetData( kJoinUserPetPk ) )
		{
			RoomSendPacketTcp( kJoinUserPetPk );
		}
#endif

		SP2Packet JoinUsertitle(STPK_TITLE_EQUIP_INFO);
		if( pExist->FillEquipTitleData( JoinUsertitle ) )
		{
			RoomSendPacketTcp( JoinUsertitle );
		}

		m_pMode->AddNewRecord(pExist);

		g_LogDBClient.OnInsertTime( pExist, LogDBClient::TT_WAIT_NEXT );
		pExist->SetStartTimeLog( 0 ); // 초기화
	}

	if( m_vUserNode.size() > 1 )
		m_pMode->SetStopTime( false );
	else if( m_pMode->GetModeType() == MT_TRAINING || m_pMode->GetModeType() == MT_HEADQUARTERS || m_pMode->GetModeType() == MT_HOUSE )
		m_pMode->SetStopTime( false );

	SP2Packet kPushStructInfo( STPK_PUSHSTRUCT_INFO );
	if( m_pMode->GetPushStructInfo( kPushStructInfo ) )
	{
		RoomSendPacketTcp( kPushStructInfo );
	}

	SP2Packet kBallPacket( STPK_BALLSTRUCT_INFO );
	if( m_pMode->GetBallStructInfo( kBallPacket ) )
	{
		RoomSendPacketTcp( kBallPacket );
	}

	SP2Packet kMachinePacket( STPK_MACHINESTRUCT );
	if( m_pMode->GetMachineStructInfo( kMachinePacket ) )
	{
		RoomSendPacketTcp( kMachinePacket );
	}

	SendFieldItemInfo( NULL );

	SP2Packet kEndSetNextMode( STPK_END_SET_NEXT_MODE );
	RoomSendPacketTcp(kEndSetNextMode);

	//
	m_pMode->NextBattleRoomComplete();

	m_bPartyProcessEnd = false;
}

void Room::CreateNextLadderBattle()
{
	m_bRoomProcess = true;
	if( !m_pMode ) return;

	m_bCharChangeToUDP = Help::IsCharChangeToUDP();
	m_bOnlyServerRelay = Help::IsOnlyServerRelay();
	int iSubNum = m_pMode->GetModeSubNum();
	int iMapIndex = m_pMode->GetModeMapNum();
	// 길드팀 양팀 유저 전부에게 전송.
	{
		SP2Packet kPacket( STPK_LADDERTEAM_MACRO );
		kPacket << LADDERTEAM_MACRO_MODE_READY_GO << GetModeType() << iSubNum << iMapIndex;
		RoomSendPacketTcp( kPacket );		
	}
	SP2Packet kStartSetNextMode( STPK_START_SET_NEXT_MODE );
	kStartSetNextMode << GetModeType();
	kStartSetNextMode << iSubNum;
	kStartSetNextMode << iMapIndex;
	kStartSetNextMode << (int)GetPlazaModeType();
	RoomSendPacketTcp(kStartSetNextMode);

	// 룸 정보 전송.
	SP2Packet kJoinRoomPk( STPK_JOIN_ROOMDATA );
	kJoinRoomPk<<m_iRoomIndex;
	kJoinRoomPk<<m_iRoomNum;
	kJoinRoomPk<<(int)m_room_style;
	kJoinRoomPk<<m_bCharChangeToUDP;
	kJoinRoomPk<<m_bOnlyServerRelay;
	kJoinRoomPk<<iSubNum;
	kJoinRoomPk<<iMapIndex;
	kJoinRoomPk<<m_pMode->IsZeroHP();
	kJoinRoomPk<<m_bSafetyLevelRoom;
	if( m_room_style == RSTYLE_PLAZA )
		kJoinRoomPk<<(int)GetPlazaModeType();
	m_pMode->GetModeInfo( kJoinRoomPk );
	RoomSendPacketTcp( kJoinRoomPk );

#ifdef ANTIHACK
	NextShamBattleRUDP();
#endif

	vUser_iter iter, iEnd;
	iEnd = m_vUserNode.end();
	for( iter=m_vUserNode.begin() ; iter!=iEnd ; ++iter )
	{
		User *pExist = *iter;

		pExist->ReleaseEquipAllChar();
		pExist->ClearCharJoinedInfo();
		
		if( pExist->IsStealth() )
		{
			pExist->SetTeam( TEAM_NONE );
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "스텔스 입장 유저 : [%d] - %d - %s", 
									GetRoomIndex(), (int)GetModeType(), pExist->GetPublicID().c_str() );
		}
		else 
		{
			LadderTeamParent *pLadderTeam = pExist->GetMyLadderTeam();
			if( pLadderTeam )
			{
				if( pLadderTeam->GetIndex() == m_MainLadderTeam.m_dwTeamIndex )
					pExist->SetTeam( m_MainLadderTeam.m_eTeamType );
				else if( pLadderTeam->GetIndex() == m_SubLadderTeam.m_dwTeamIndex )
					pExist->SetTeam( m_SubLadderTeam.m_eTeamType );
				else
					LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "래더팀 팀 정해지지 않음 : [%d] - B[%d] - R[%d] - (%d)%s", GetRoomIndex(), m_MainLadderTeam.m_dwTeamIndex, m_SubLadderTeam.m_dwTeamIndex, pLadderTeam->GetIndex(), pExist->GetPublicID().c_str() );
			}
			else
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "래더팀 팀 포인터 없음 : [%d] - %d - %s", GetRoomIndex(), (int)GetModeType(), pExist->GetPublicID().c_str() );
		}

		SP2Packet kExistUserPk( STPK_JOIN_USERDATA );
		pExist->FillJoinUserData( kExistUserPk, IsExperienceUser( pExist ) );
		kExistUserPk << pExist->GetMyVictories();
		kExistUserPk << false;	    // Now Not Playing..
		kExistUserPk << pExist->GetKillDeathLevel() << pExist->GetLadderPoint();
		kExistUserPk << false;		// chatmode
		kExistUserPk << false;      // experience mode
		kExistUserPk << false;		// fishing
		kExistUserPk << 0;			// fishing rod type
		kExistUserPk << 0;			// fishing bait type
		kExistUserPk << pExist->IsStealth();
		
		RoomSendPacketTcp( kExistUserPk );

		SP2Packet kJoinUserPetPk( STPK_PET_EQUIP_INFO );
#if defined( DIVIDE_PET ) 
		int iClassType = 0;
		ioCharacter *rkChar = pExist->GetCharacter( pExist->GetSelectChar() );
		if( rkChar )	iClassType = rkChar->GetCharInfo().m_class_type;
		if( !pExist->FillEquipPetData( kJoinUserPetPk, iClassType ) )
		{
			PACKET_GUARD_VOID( kJoinUserPetPk.Write(  pExist->GetPublicID() ) );
			PACKET_GUARD_VOID( kJoinUserPetPk.Write( 0 ) );
			PACKET_GUARD_VOID( kJoinUserPetPk.Write( 0 ) );
			PACKET_GUARD_VOID( kJoinUserPetPk.Write( 0 ) );
			PACKET_GUARD_VOID( kJoinUserPetPk.Write( iClassType ) );
			PACKET_GUARD_VOID( kJoinUserPetPk.Write( false ) );
		}

#else
		if( pExist->FillEquipPetData( kJoinUserPetPk ) )
#endif
			RoomSendPacketTcp( kJoinUserPetPk );


		SP2Packet JoinUserTitle(STPK_TITLE_EQUIP_INFO);
		if( pExist->FillEquipTitleData( JoinUserTitle ) )
		{
			RoomSendPacketTcp( JoinUserTitle );
		}

		m_pMode->AddNewRecord(pExist);

		g_LogDBClient.OnInsertTime( pExist, LogDBClient::TT_WAIT_LADDER_NEXT );
		pExist->SetStartTimeLog( 0 ); // 초기화
	}

	if( m_vUserNode.size() > 1 )
		m_pMode->SetStopTime( false );
	else if( m_pMode->GetModeType() == MT_TRAINING || m_pMode->GetModeType() == MT_HEADQUARTERS || m_pMode->GetModeType() == MT_HOUSE )
		m_pMode->SetStopTime( false );

	SP2Packet kPushStructInfo( STPK_PUSHSTRUCT_INFO );
	if( m_pMode->GetPushStructInfo( kPushStructInfo ) )
	{
		RoomSendPacketTcp( kPushStructInfo );
	}

	SP2Packet kBallPacket( STPK_BALLSTRUCT_INFO );
	if( m_pMode->GetBallStructInfo( kBallPacket ) )
	{
		RoomSendPacketTcp( kBallPacket );
	}

	SP2Packet kMachinePacket( STPK_MACHINESTRUCT );
	if( m_pMode->GetMachineStructInfo( kMachinePacket ) )
	{
		RoomSendPacketTcp( kMachinePacket );
	}

	SendFieldItemInfo( NULL );

	SP2Packet kEndSetNextMode( STPK_END_SET_NEXT_MODE );
	RoomSendPacketTcp(kEndSetNextMode);
}

void Room::NextShamShuffleRandomTeam( ModeType eModeType )
{
	// 팀원 섞어서 시작 옵션 활성화 처리	
	if( eModeType == MT_SURVIVAL || eModeType == MT_BOSS || eModeType == MT_GANGSI || eModeType == MT_FIGHT_CLUB || eModeType == MT_SHUFFLE_BONUS ) return;

	if( m_vUserNode.empty() ) return;

	User *pFirstUser = m_vUserNode[0];
	if( !pFirstUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Room::NextShamShuffleRandomTeam() First User NULL!!!" );
		return;
	}

	ShuffleRoomParent *pShuffleRoom = pFirstUser->GetMyShuffleRoom();
	if( pShuffleRoom && pShuffleRoom->IsOriginal() )		
	{
		UserRankInfoList kUserRankList;
		m_pMode->GetUserRankByNextTeam( kUserRankList, true );

		ShuffleRoomNode *pOriginal = dynamic_cast<ShuffleRoomNode*>( pShuffleRoom );
		pOriginal->ShuffleEnterRandomTeam( kUserRankList );
	}	
	else
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Room::NextShamShuffleRandomTeam() ShuffleRoom NULL!!!" );
}

void Room::CreateNextShamShuffle()
{
	m_bRoomProcess = true;
	if( !m_pMode ) return;

	m_bCharChangeToUDP = Help::IsCharChangeToUDP();
	m_bOnlyServerRelay = Help::IsOnlyServerRelay();
	int iSubNum = m_pMode->GetModeSubNum();
	int iMapIndex = m_pMode->GetModeMapNum();

	SP2Packet kStartSetNextMode( STPK_START_SET_NEXT_MODE );
	kStartSetNextMode << GetModeType();
	kStartSetNextMode << iSubNum;
	kStartSetNextMode << iMapIndex;
	kStartSetNextMode << (int)GetPlazaModeType();
	RoomSendPacketTcp(kStartSetNextMode);

	// 룸 정보 전송.
	SP2Packet kJoinRoomPk( STPK_JOIN_ROOMDATA );
	kJoinRoomPk << m_iRoomIndex;
	kJoinRoomPk << m_iRoomNum;
	kJoinRoomPk << (int)m_room_style;
	kJoinRoomPk << m_bCharChangeToUDP;
	kJoinRoomPk << m_bOnlyServerRelay;
	kJoinRoomPk << iSubNum;
	kJoinRoomPk << iMapIndex;
	kJoinRoomPk << m_pMode->IsZeroHP();
	kJoinRoomPk << false;
	if( m_room_style == RSTYLE_PLAZA )
		kJoinRoomPk<<(int)GetPlazaModeType();
	m_pMode->GetModeInfo( kJoinRoomPk );
	RoomSendPacketTcp( kJoinRoomPk );
#ifdef ANTIHACK
	NextShamBattleRUDP();
#endif

	vUser_iter iter, iEnd;
	iEnd = m_vUserNode.end();
	for( iter=m_vUserNode.begin() ; iter!=iEnd ; ++iter )
	{
		User *pExist = *iter;

		if( !pExist )
			continue;

		pExist->ReleaseEquipAllChar();
		pExist->ClearCharJoinedInfo();

		if( pExist->IsStealth() )
		{
			pExist->SetTeam( TEAM_NONE );
			LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "스텔스 입장 유저 : [%d] - %d - %s", GetRoomIndex(), (int)GetModeType(), pExist->GetPublicID().c_str() );
		}
		else if( GetModeType() == MT_SURVIVAL || GetModeType() == MT_MONSTER_SURVIVAL || GetModeType() == MT_DUNGEON_A || GetModeType() == MT_FIGHT_CLUB || GetModeType() == MT_SHUFFLE_BONUS ||
			GetModeType() == MT_RAID ||
				 Help::IsMonsterDungeonMode(GetModeType()))
		{
			pExist->SetTeam( GetNextTeamType() );
		}
		else if( GetModeType() == MT_BOSS )
		{
			TeamType eTeam = TEAM_RED;
			ShuffleRoomParent *pShuffleRoom = pExist->GetMyShuffleRoom();
			if( pShuffleRoom && pShuffleRoom->IsOriginal() )
			{
				// 보스 선정된 유저는 블루팀
				ShuffleRoomNode *pOriginal = dynamic_cast<ShuffleRoomNode*>( pShuffleRoom );
				if( pOriginal && pOriginal->GetBossName() == pExist->GetPublicID() && 
					m_pMode->GetTeamUserCnt( TEAM_BLUE ) == 0 )          //블루 팀은 오직 한명만 가능
				{
					eTeam = TEAM_BLUE;
					pOriginal->ClearBossName();
				}
			}
			else
			{
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "전투방 플레이 입장시 파티가 없음 : [%d] - %d - %s", GetRoomIndex(), (int)GetModeType(), pExist->GetPublicID().c_str() );
			}
			pExist->SetTeam( eTeam );
		}
		else if( GetModeType() == MT_GANGSI )
		{
			TeamType eTeam = TEAM_RED;
			ShuffleRoomParent *pShuffleRoom = pExist->GetMyShuffleRoom();
			if( pShuffleRoom && pShuffleRoom->IsOriginal() )
			{
				// 강시 선정된 유저는 블루팀
				ShuffleRoomNode *pOriginal = dynamic_cast<ShuffleRoomNode*>( pShuffleRoom );
				if( pOriginal && pOriginal->GetGangsiName() == pExist->GetPublicID() && 
					m_pMode->GetTeamUserCnt( TEAM_BLUE ) == 0 )          //블루 팀은 오직 한명만 가능
				{
					eTeam = TEAM_BLUE;
					pOriginal->ClearGangsi();
				}
			}
			else
			{
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "전투방 플레이 입장시 파티가 없음 : [%d] - %d - %s", GetRoomIndex(), (int)GetModeType(), pExist->GetPublicID().c_str() );
			}
			pExist->SetTeam( eTeam );
		}
		else
		{
			ShuffleRoomParent *pShuffleRoom = pExist->GetMyShuffleRoom();
			if( pShuffleRoom && pShuffleRoom->IsOriginal() )
			{
				ShuffleRoomNode *pOriginal = dynamic_cast<ShuffleRoomNode*>( pShuffleRoom );
				if( pOriginal )
				{
					TeamType eTeam = pOriginal->GetUserTeam( pExist->GetPublicID() );
					pExist->SetTeam( eTeam );
					if( eTeam != TEAM_BLUE && eTeam != TEAM_RED )
					{
						LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "전투방 플레이 다음 세트 시작시 팀이없음 : [%d] - %d - %s - %d - %d", 
							GetRoomIndex(), (int)GetModeType(), pExist->GetPublicID().c_str(), (int)eTeam, (int)pShuffleRoom->GetIndex() );
					}
				}
			}
			else
			{
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "전투방 플레이 다음 세트 시작시 파티가 없음 : [%d] - %d - %s", 
					GetRoomIndex(), (int)GetModeType(), pExist->GetPublicID().c_str() );
			}
		}		

		SP2Packet kExistUserPk( STPK_JOIN_USERDATA );
		pExist->FillJoinUserData( kExistUserPk, IsExperienceUser( pExist ) );
		kExistUserPk << pExist->GetMyVictories();
		kExistUserPk << false;		// Now Not Playing..
		kExistUserPk << pExist->GetKillDeathLevel() << pExist->GetLadderPoint();
		kExistUserPk << false;		// chatmode
		kExistUserPk << false;      // experience mode
		kExistUserPk << false;		// fishing
		kExistUserPk << 0;			// fishing rod type
		kExistUserPk << 0;			// fishing bait type
		kExistUserPk << pExist->IsStealth();

		RoomSendPacketTcp( kExistUserPk );

		SP2Packet kJoinUserPetPk( STPK_PET_EQUIP_INFO );
#if defined( DIVIDE_PET )
		int iClassType = 0;
		ioCharacter *rkChar = pExist->GetCharacter( pExist->GetSelectChar() );
		if( rkChar )	
			iClassType = rkChar->GetCharInfo().m_class_type;
		if( !pExist->FillEquipPetData( kJoinUserPetPk, iClassType ) )
		{
			PACKET_GUARD_VOID( kJoinUserPetPk.Write(  pExist->GetPublicID() ) );
			PACKET_GUARD_VOID( kJoinUserPetPk.Write( 0 ) );
			PACKET_GUARD_VOID( kJoinUserPetPk.Write( 0 ) );
			PACKET_GUARD_VOID( kJoinUserPetPk.Write( 0 ) );
			PACKET_GUARD_VOID( kJoinUserPetPk.Write( iClassType ) );
			PACKET_GUARD_VOID( kJoinUserPetPk.Write( false ) );
		}
#else
		if( pExist->FillEquipPetData( kJoinUserPetPk ) )
#endif
			RoomSendPacketTcp( kJoinUserPetPk );

		SP2Packet JoinUserTitle(STPK_TITLE_EQUIP_INFO);
		if( pExist->FillEquipTitleData( JoinUserTitle ) )
		{
			RoomSendPacketTcp( JoinUserTitle );
		}

		m_pMode->AddNewRecord(pExist);

		g_LogDBClient.OnInsertTime( pExist, LogDBClient::TT_WAIT_NEXT );
		pExist->SetStartTimeLog( 0 ); // 초기화
	}

	if( m_vUserNode.size() > 1 )
		m_pMode->SetStopTime( false );
	else if( m_pMode->GetModeType() == MT_TRAINING || m_pMode->GetModeType() == MT_HEADQUARTERS || m_pMode->GetModeType() == MT_HOUSE )
		m_pMode->SetStopTime( false );

	SP2Packet kPushStructInfo( STPK_PUSHSTRUCT_INFO );
	if( m_pMode->GetPushStructInfo( kPushStructInfo ) )
	{
		RoomSendPacketTcp( kPushStructInfo );
	}

	SP2Packet kBallPacket( STPK_BALLSTRUCT_INFO );
	if( m_pMode->GetBallStructInfo( kBallPacket ) )
	{
		RoomSendPacketTcp( kBallPacket );
	}

	SP2Packet kMachinePacket( STPK_MACHINESTRUCT );
	if( m_pMode->GetMachineStructInfo( kMachinePacket ) )
	{
		RoomSendPacketTcp( kMachinePacket );
	}

	SendFieldItemInfo( NULL );

	SP2Packet kEndSetNextMode( STPK_END_SET_NEXT_MODE );
	RoomSendPacketTcp(kEndSetNextMode);

	//
	m_pMode->NextBattleRoomComplete();

	m_bPartyProcessEnd = false;
}

void Room::NotifyDropItemToMode( ioItem *pItem )
{
	if( !m_pMode )
		return;

#ifdef FLAG_MODE_BY_BCKIM_DEBUG		// NotifyDropItemToMode
	if(m_pMode->GetModeType() == MT_FLAG )
		LOG.PrintTimeAndLog(0,"[FLAG_MODE_BY_BCKIM:NotifyDropItemToMode]ID[    ] INFO>>NotifyDropItemToMode MODE_TYPE[%d]",m_pMode->GetModeType());	
#endif // FLAG_MODE_BY_BCKIM_DEBUG

	switch( m_pMode->GetModeType() )
	{
	case MT_KING:
		{
			HiddenkingMode *pKingMode = ToKingMode( m_pMode );
			if( pKingMode )
				pKingMode->CheckDropCrown( pItem );
		}
		break;
	case MT_DOBULE_CROWN:
		{
			DoubleCrownMode *pDoubleCrown = ToDoubleCrownMode( m_pMode );
			if( pDoubleCrown )
				pDoubleCrown->CheckDropCrown( pItem );
		}
		break;
#ifdef FLAG_MODE_BY_BCKIM
	case MT_FLAG:
		{
			FlagMode *pFlag = ToFlagMode( m_pMode );
			if( pFlag && pItem->GetItemCode() == 500004 )
				pFlag->Set_Ownerless_Status();
		}
		break;
#endif // FLAG_MODE_BY_BCKIM
	}
}

bool Room::IsPickItemToModeUse( ioItem *pItem, User *pUser )
{
	if( !m_pMode )
		return true;

#ifdef FLAG_MODE_BY_BCKIM_DEBUG		// IsPickItemToModeUse
	if(m_pMode->GetModeType() == MT_FLAG )
	LOG.PrintTimeAndLog(0,"[FLAG_MODE_BY_BCKIM:IsPickItemToModeUse]ID[%s] INFO>>IsPickItemToModeUse MODE_TYPE[%d]",pUser->GetPublicID().c_str(),m_pMode->GetModeType());	
#endif // FLAG_MODE_BY_BCKIM_DEBUG

	switch( m_pMode->GetModeType() )
	{
	case MT_DOBULE_CROWN:
		{
			DoubleCrownMode *pDoubleCrown = ToDoubleCrownMode( m_pMode );
			if( pDoubleCrown )
				return pDoubleCrown->CheckPrePickCrown( pItem, pUser );
		}
		break;
	}

	return true;
}

void Room::NotifyPickItemToMode( ioItem *pItem, User *pUser )
{
	if( !m_pMode )
		return;


#ifdef FLAG_MODE_BY_BCKIM_DEBUG		// NotifyPickItemToMode
	if(m_pMode->GetModeType() == MT_FLAG )
	LOG.PrintTimeAndLog(0,"[FLAG_MODE_BY_BCKIM:NotifyPickItemToMode]ID[%s] INFO>>NotifyPickItemToMode MODE_TYPE[%d]",pUser->GetPublicID().c_str(),m_pMode->GetModeType());	
#endif // FLAG_MODE_BY_BCKIM_DEBUG

	switch( m_pMode->GetModeType() )
	{
	case MT_KING:
		{
			HiddenkingMode *pKingMode = ToKingMode( m_pMode );
			if( pKingMode )
				pKingMode->CheckPickCrown( pItem, pUser );
		}
		break;
	case MT_DOBULE_CROWN:
		{
			DoubleCrownMode *pDoubleCrown = ToDoubleCrownMode( m_pMode );
			if( pDoubleCrown )
				pDoubleCrown->CheckPickCrown( pItem, pUser );
		}
#ifdef FLAG_MODE_BY_BCKIM	// 2018-03-15 by bckim, 깃발모드 추가	NotifyPickItemToMode
	case MT_FLAG:
		{
			//FlagMode *pFlagMode = ToFlagMode( m_pMode );
			//if( pFlagMode )

#ifdef FLAG_MODE_BY_BCKIM_DEBUG		// NotifyPickItemToMode
			if(m_pMode->GetModeType() == MT_FLAG )
			LOG.PrintTimeAndLog(0,"[FLAG_MODE_BY_BCKIM:NotifyPickItemToMode]ID[%s] INFO>>NotifyPickItemToMode MODE_TYPE[%d]",pUser->GetPublicID().c_str(),m_pMode->GetModeType());	
#endif // FLAG_MODE_BY_BCKIM_DEBUG
		}
		break;
#endif
	}
}

ModeType Room::GetModeType()
{
	if(!m_pMode)
		return MT_NONE;

	return m_pMode->GetModeType();
}

ioItem* Room::GetFieldItem( int iListArray )
{
	int iSize = m_FieldItemList.size();
	if(!COMPARE( iListArray, 0, iSize))
		return NULL;

	ItemList::iterator iter = m_FieldItemList.begin();
	std::advance(iter, iListArray);
	
	return *iter;
}

void Room::InitModeTypeList(int iSelectValue)
{
	SAFEDELETE(m_pModeSelector);

	m_pModeSelector = new ModeSelectManager(this);
	if( !m_pModeSelector )
		return;

	m_pModeSelector->InitModeInfoList(iSelectValue);
}

void Room::SetPreSelectModeInfo( ModeType eModeType, int iSubType, int iMapNum )
{
	if( !m_pModeSelector ) return;

	m_pModeSelector->SetPreModeInfo( eModeType, iSubType, iMapNum );
}

ModeType Room::SelectNextMode( ModeType eModeType, int iSubModeType, int iModeMapNum )
{
	if( !m_pModeSelector )
	{
		return MT_NONE;
	}

	if( MT_TRAINING == eModeType && 16 == iSubModeType )
		m_pModeSelector->SelectGuildRoomNextModeInfo();
	else
		m_pModeSelector->SelectNextModeInfo( eModeType, iSubModeType, iModeMapNum );

	return m_pModeSelector->GetCurModeType();
}

User *Room::GetUserNode(ioHashString &szName)
{
	CRASH_GUARD();
	vUser_iter iter, iEnd;
	iEnd = m_vUserNode.end();
	for( iter=m_vUserNode.begin() ; iter!=iEnd ; ++iter )
	{
		User *pCurUser = *iter;
		if( pCurUser->GetPublicID() == szName )
			return pCurUser;
	}

	return NULL;
}

User * Room::GetUserNodeByArray( const int iArray )
{
	if( COMPARE( iArray, 0, (int) m_vUserNode.size()) )
		return m_vUserNode[iArray];

	return NULL;
}

bool Room::IsCanPickItemState()
{
	if( m_pMode )
		return m_pMode->IsCanPickItemState();

	return false;
}

void Room::LogRoomInfo()
{
	char szLog[MAX_PATH*2];
	memset( szLog, 0, sizeof( szLog ) );
	char szBuf[MAX_PATH] = "";
	sprintf_s( szBuf, "Index[%d] Style[%d] Mode[%d] : ", GetRoomIndex(), (int)GetRoomStyle(), (int)GetModeType() );
	strcat_s( szLog, szBuf );
	vUser_iter iter, iEnd;
	iEnd = m_vUserNode.end();
	for( iter=m_vUserNode.begin() ; iter!=iEnd ; ++iter )
	{
		User *pCurUser = *iter;
		if( pCurUser )
		{
			sprintf_s( szBuf, "%s, ", pCurUser->GetPublicID().c_str() );
			strcat_s( szLog, szBuf );
		}
	}

	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "%s", szLog );
}

bool Room::IsEnableState( User *pUser )
{
	if( m_pMode )
		return m_pMode->IsEnableState( pUser );

	return false;
}

bool Room::IsRoundEndState()
{
	if( m_pMode )
	{
		if( m_pMode->GetState() == Mode::MS_RESULT || m_pMode->GetState() == Mode::MS_RESULT_WAIT )
			return true;
	}

	return false;
}

bool Room::IsRoomLoadingState( User *pUser )
{
	if( !m_pMode ) return false;
	if( !pUser ) return false;

	ModeRecord *pRecord = m_pMode->FindModeRecord( pUser );
	if( !pRecord ) return false;

	if( pRecord->eState == RS_LOADING ) return true;	

	return false;
}

void Room::OnPlazaRoomInfo( UserParent *pUser )
{
	if( pUser == NULL ) return;

	SP2Packet kPacket( STPK_PLAZA_JOIN_INFO );
	FillPlazaRoomInfo( kPacket );
	FillPlazaRoomJoinState( kPacket, pUser );
	FillUserList( kPacket );
	pUser->RelayPacket( kPacket );
}

void Room::FillPlazaRoomInfo( SP2Packet &rkPacket )
{
	rkPacket << GetRoomIndex();
	rkPacket << GetRoomName();
	rkPacket << GetMasterName();
	rkPacket << GetJoinUserCnt();
	rkPacket << GetPlayUserCnt();
	rkPacket << GetMaxPlayer();
	rkPacket << GetRoomPW();
	rkPacket << GetPlazaRoomLevel();
	rkPacket << (int)GetPlazaModeType();
	rkPacket << GetModeSubNum();
}

void Room::FillPlazaRoomJoinState( SP2Packet &rkPacket, UserParent *pUserParent )
{
	int iReturnState = SortPlazaRoom::PRS_ACTIVE;
	if( !pUserParent || GetPlayUserCnt() == GetMaxPlayer() )
		iReturnState = SortPlazaRoom::PRS_FULL_USER;
    else if( !g_LevelMatchMgr.IsPlazaLevelJoin( GetAverageLevel(), pUserParent->GetKillDeathLevel(), JOIN_CHECK_MIN_LEVEL ) )
		iReturnState = SortPlazaRoom::PRS_NOT_MIN_LEVEL_MATCH;
	else if( !g_LevelMatchMgr.IsPlazaLevelJoin( GetAverageLevel(), pUserParent->GetKillDeathLevel(), JOIN_CHECK_MAX_LEVEL ) )
		iReturnState = SortPlazaRoom::PRS_NOT_MAX_LEVEL_MATCH;

	rkPacket << iReturnState;
}

void Room::FillUserList( SP2Packet &rkPacket, bool bTeamInfo )
{
	int iSize = m_vUserNode.size();
	rkPacket << iSize;

	vUser_iter iter, iEnd;
	iEnd = m_vUserNode.end();
	for( iter=m_vUserNode.begin() ; iter!=iEnd ; ++iter )
	{
		User *pCurUser = *iter;
		rkPacket << pCurUser->GetGradeLevel();
		rkPacket << pCurUser->GetPublicID();
		rkPacket << pCurUser->GetGuildIndex();
		rkPacket << pCurUser->GetGuildMark();
		rkPacket << pCurUser->GetPingStep();
	}
}

void Room::FillLadderTeamRank( SP2Packet &rkPacket )
{
	rkPacket << m_MainLadderTeam.m_dwTeamIndex << m_MainLadderTeam.m_iPrevTeamRank << m_MainLadderTeam.m_iCurTeamRank;
	rkPacket << m_SubLadderTeam.m_dwTeamIndex << m_SubLadderTeam.m_iPrevTeamRank << m_SubLadderTeam.m_iCurTeamRank;
}

void Room::CheckCreateCrown( User *pUser )
{
	TrainingMode *pMode = ToTrainingMode( m_pMode );
	if( pMode )
		pMode->CheckCreateCrown( pUser );
}

DWORD Room::GetModeStartTime() const
{
	if( m_pMode )
	{
		return m_pMode->GetModeStartTime();
	}

	return 0;
}

void Room::SetChatModeState( const ioHashString &rkName, bool bChatMode )
{
	if( m_pMode )
	{
		m_pMode->SetChatModeState( rkName, bChatMode );
	}
}

void Room::SetFishingState( const ioHashString &rkName, bool bFishing )
{
	if( m_pMode )
	{
		m_pMode->SetFishingState( rkName, bFishing );
	}
}

void Room::GetExtraModeInfo( SP2Packet &rkPacket )
{
	if( m_pMode )
	{
		if( m_pMode->GetModeType() == MT_TEAM_SURVIVAL || m_pMode->GetModeType() == MT_DOBULE_CROWN )
		{
			PACKET_GUARD_VOID( rkPacket.Write(true) );

			m_pMode->GetExtraModeInfo( rkPacket );
			return;
		}
	}
	
	PACKET_GUARD_VOID( rkPacket.Write(false) );
}

bool Room::IsFishingState( const ioHashString &rkName )
{
	if( m_pMode )
		return m_pMode->IsFishingState( rkName );

	return false;
}

int Room::GetExcavatingUserCnt()
{
	int iExcavateUserCnt = 0;
	vUser_iter iter, iEnd;
	iEnd = m_vUserNode.end();
	for( iter=m_vUserNode.begin() ; iter!=iEnd ; ++iter )
	{
		User *pUser = *iter;
		if( pUser && !pUser->IsRealExcavating() )
			continue;

		iExcavateUserCnt++;
	}

	return iExcavateUserCnt;
}

bool Room::IsExperienceUser( User *pUser )
{
	if( m_pMode == NULL )
		return false;
	return m_pMode->IsExperienceModeState( pUser );
}

void Room::SetInstantSpawnNpc(bool bSpawn)
{
	TrainingMode *pTrainingMode = ToTrainingMode( m_pMode );

	if( !pTrainingMode ) return;

	if( bSpawn )
		pTrainingMode->InstantSpawnNpc();
	else
		pTrainingMode->InstantKillNpc();
}

void Room::ChangeGangsiUser( User *pUser )
{
	GangsiMode *pGangsiMode = ToGangsiMode( m_pMode );
	if( pGangsiMode )
		pGangsiMode->ChangeGangsiUser( pUser );
}

int Room::GetGangsiItem( int iSlot )
{
	GangsiMode *pGangsiMode = ToGangsiMode( m_pMode );
	if( pGangsiMode )
		return pGangsiMode->GetGangsiItem( iSlot );
	return 0;
}

void Room::SetNagleAlgorithm( bool bOn )
{
	vUser_iter iter;
	for(iter = m_vUserNode.begin();iter != m_vUserNode.end();++iter)
	{
		User *pUser = *iter;
		if( !pUser ) continue;

		pUser->SetNagleAlgorithm( bOn );
	}
}

bool Room::IsNoBattleModeType()
{
	if( GetModeType() == MT_TRAINING || GetModeType() == MT_HEADQUARTERS || GetModeType() == MT_HOUSE )
		return true;
	return false;
}

void Room::SetExitRoomByCheckValue( User *pSend )
{
	if( m_pMode )
	{
		m_pMode->SetExitRoomByCheckValue( pSend );
	}
}


bool Room::ResetRevive( User *pUser )
{
	if( !m_pMode ) return false;
	if( !pUser ) return false;

	ModeRecord *pRecord = m_pMode->FindModeRecord( pUser );
	if( !pRecord ) return false;

	pRecord->dwCurDieTime = 0;

	return false;
}

void Room::FillPlayingUserData( SP2Packet &kPacket, User *pTargetUser )
{
	PACKET_GUARD_VOID( kPacket.Write( PLAYING_USER_INFO ) );
	pTargetUser->FillJoinUserData( kPacket, IsExperienceUser( pTargetUser ) );
	PACKET_GUARD_VOID( kPacket.Write( pTargetUser->GetMyVictories() ) );
	PACKET_GUARD_VOID( kPacket.Write( false ) );	// Now Not Playing.
	PACKET_GUARD_VOID( kPacket.Write( pTargetUser->GetKillDeathLevel() ) );
	PACKET_GUARD_VOID( kPacket.Write( pTargetUser->GetLadderPoint() ) );
	PACKET_GUARD_VOID( kPacket.Write( false ) );	// chatmode
	PACKET_GUARD_VOID( kPacket.Write( false ) );	// experience mode
	PACKET_GUARD_VOID( kPacket.Write( false ) );	// fishing
	PACKET_GUARD_VOID( kPacket.Write( 0 ) );		// fishing rod type
	PACKET_GUARD_VOID( kPacket.Write( 0 ) );		// fishing bait type
	PACKET_GUARD_VOID( kPacket.Write( pTargetUser->IsStealth() ) );
}

void Room::SendGuildBlocksInfo(User* pUser)
{
	if( !pUser )
		return;

	if( GetPlazaModeType() != PT_GUILD )
		return;


	g_GuildRoomBlockMgr.SendBlockInfos(pUser->GetGuildIndex(), pUser);
}

void Room::SetHouseModeMaster( const ioHashString &rkName, const DWORD dwMasterIndex )
{
	if( m_pMode )
	{
		if( m_pMode->GetModeType() == MT_HOUSE )
		{	
			HouseMode* pInfo = static_cast<HouseMode*>(m_pMode);
			pInfo->SetHouseMaster( rkName, dwMasterIndex );
		}
	}
}

void Room::FillPersonalHQInfo( const ioHashString &rkMasterName, SP2Packet &rkPacket )
{
	HouseMode *pInfo= ToHouseMode( m_pMode );
	if( !pInfo )
	{
		PACKET_GUARD_VOID( rkPacket.Write(rkMasterName) );
		PACKET_GUARD_VOID( rkPacket.Write(MAX_PLAZA_PLAYER) );
		PACKET_GUARD_VOID( rkPacket.Write(0) );
	}
	else
	{
		PACKET_GUARD_VOID( rkPacket.Write(pInfo->GetMasterName()) );
		PACKET_GUARD_VOID( rkPacket.Write(pInfo->GetMaxPlayer()) );
		PACKET_GUARD_VOID( rkPacket.Write((int)m_vUserNode.size()) );

		vUser_iter iter, iEnd;
		iEnd = m_vUserNode.end();
		for( iter=m_vUserNode.begin() ; iter!=iEnd ; ++iter )
		{
			User *pUser = *iter;
			if( pUser )
			{
				PACKET_GUARD_VOID( rkPacket.Write(pUser->GetGradeLevel()) );
				PACKET_GUARD_VOID( rkPacket.Write(pUser->GetPublicID()) );
				PACKET_GUARD_VOID( rkPacket.Write(pUser->GetGuildIndex()) );
				PACKET_GUARD_VOID( rkPacket.Write(pUser->GetGuildMark()) );
				PACKET_GUARD_VOID( rkPacket.Write(pUser->GetPingStep()) );
			}
			else
			{
				rkPacket << 0 << "" << 0 << 0 << 0;
			}
		}
	}
}

void Room::SendPersonalHQBlockInfo(User* pUser)
{
	if( !pUser || !m_pMode )
		return;

	if( GetModeType() != MT_HOUSE )
		return;

	HouseMode* pMode	= static_cast<HouseMode*>(m_pMode);

	if( !pMode )
		return;

	
	g_PersonalRoomBlockMgr.SendBlockInfo(pMode->GetMasterIndex(), pUser);
	//g_GuildRoomBlockMgr.SendBlockInfos(pUser->GetGuildIndex(), pUser);
}

#ifdef ANTIHACK
void Room::NextShamBattleRUDP()
{
	vUser_iter iter, iEnd;
	iEnd = m_vUserNode.end();
	for( iter=m_vUserNode.begin() ; iter!=iEnd ; ++iter )
	{
		User* pUser = *iter;
		CRASH_GUARD();
		if( pUser )
		{
			SendRUDPUserInfo( pUser );
		}	
	}
}

void Room::SendRUDPUserInfo( User* pUser )
{
	DWORD dwRand = rand();
	pUser->UpdateSeed( dwRand );

	int iCoolType = 4;
	int iModeType = GetModeType();
	int iRoomStyle = GetRoomStyle();
	BattleRoomParent *pBattleRoom = pUser->GetMyBattleRoom();
	if( pBattleRoom && pBattleRoom->IsOriginal() )
	{
		BattleRoomNode *pOriginal = (BattleRoomNode*)pBattleRoom;
		if( pOriginal )
		{
			if( pOriginal->IsUseExtraOption() )
				iCoolType = pOriginal->GetCoolTimeType();
		}
	}

	int iTemaType = (int)pUser->GetTeam();

	// update도 이걸로 처리
	g_Relay.InsertRelayGroupReserve( this, pUser->GetUserIndex(), pUser->GetPublicIP(), pUser->GetUDP_port(), pUser->GetPublicID(), pUser->GetUserSeed(), pUser->GetNPCSeed(), iCoolType, iModeType, iRoomStyle, iTemaType );
}
#endif

void Room::SendUserDataTo( User* pUser )
{
	vUser_iter iter, iEnd;
	iEnd = m_vUserNode.end();
	for( iter=m_vUserNode.begin() ; iter!=iEnd ; ++iter )
	{
		User *pRoomUser = *iter;
		if( pRoomUser->GetUserIndex() == pUser->GetUserIndex() ) continue; //자신의 정보는 빼고 전달
		
		SP2Packet kPacket( STPK_LADDERROOM_USER_INFO );
		PACKET_GUARD_VOID( kPacket.Write( 0 ) ); //GetRoomLevel()
		PACKET_GUARD_VOID( kPacket.Write( pRoomUser->GetPublicID()) );
		PACKET_GUARD_VOID( kPacket.Write( pRoomUser->GetGradeLevel()) ); 
		PACKET_GUARD_VOID( kPacket.Write( 0 ) ); //m_iAbilityLevel
		PACKET_GUARD_VOID( kPacket.Write( false ) ); //m_bSafetyLevel
		PACKET_GUARD_VOID( kPacket.Write( true ) ); //m_bObserver
		PACKET_GUARD_VOID( kPacket.Write( (int)pRoomUser->GetTeam()) );
		PACKET_GUARD_VOID( kPacket.Write( pRoomUser->GetUserIndex()) );
		pRoomUser->FillUserNetworkInfo( kPacket ); //ip, port등 정보 추가

		int iGuildMark = pRoomUser->GetGuildMark();
		if( iGuildMark < 0 )
			PACKET_GUARD_VOID( kPacket.Write( 0 ) )
		else
			PACKET_GUARD_VOID( kPacket.Write( iGuildMark ) );

		pUser->RelayPacket( kPacket );
	}
}

void Room::SetModSubIndex( int iSub )
{
	m_pMode->SetModeSubNum(iSub);
}
