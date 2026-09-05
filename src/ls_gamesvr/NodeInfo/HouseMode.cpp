#include "stdafx.h"
//#include "../Window.h"
#include "../MainProcess.h"

#include "User.h"
#include "Room.h"
#include "RoomNodeManager.h"
#include "ModeHelp.h"
#include "HouseMode.h"
#include "ioMonsterMapLoadMgr.h"
#include "ioSetItemInfoManager.h"
#include "ioItemInfoManager.h"
#include "ioSetItemInfo.h"
#include "../EtcHelpFunc.h"
#include "../DataBase/LogDBClient.h"

HouseMode::HouseMode( Room *pCreator ) : Mode( pCreator )
{
	m_dwCreateTime	= 0;

	m_bJoinLock = false;

	m_TeamList.clear();
	m_TeamPosArray.clear();
	m_vRecordList.clear();
}

HouseMode::~HouseMode()
{
}

void HouseMode::LoadRoundCtrlValue( ioINILoader &rkLoader )
{
	m_bUseViewMode	= false;

	m_TeamList.clear();
	for( int i = TEAM_PRIVATE_1; i < TEAM_PRIVATE_190 + 1; i++ )
		m_TeamList.push_back( i );

	SetStartPosArray();
}

void HouseMode::SetStartPosArray()
{
	int iTeamPosCnt = m_TeamList.size();
	if(iTeamPosCnt <= 1) // 최소한 2개 필요
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Error 2 - HeadquartersMode::GetStartPosArray");
		return;
	}

	m_TeamPosArray.reserve(iTeamPosCnt);
	for( int i=0; i<iTeamPosCnt; i++ )
	{
		m_TeamPosArray.push_back(i);
	}

	io_random_shuffle( m_TeamPosArray.begin(), m_TeamPosArray.end() );

	m_iBluePosArray = m_TeamPosArray[0];
	m_iRedPosArray  = m_TeamPosArray[1];
}

void HouseMode::DestroyMode()
{
	Mode::DestroyMode();

	m_vRecordList.clear();
}

void HouseMode::AddNewRecord( User *pUser )
{
	HouseModeRecord kRecord;
	kRecord.pUser = pUser;
	m_vRecordList.push_back( kRecord );

	RemoveTeamType( pUser->GetTeam() );
	UpdateUserRank();
}

void HouseMode::RemoveRecord( User *pUser, bool bRoomDestroy )
{
	if(!pUser) return;
	
	int iCharCnt = m_vRecordList.size();
	for( int i=0 ; i<iCharCnt ; i++ )
	{
		if( m_vRecordList[i].pUser == pUser )
		{
			if( pUser->GetPublicID() == m_szMasterName )
				MasterExistHQ(pUser);

			SetModeEndDBLog( &m_vRecordList[i], iCharCnt, LogDBClient::PRT_EXIT_ROOM );
			m_vRecordList.erase( m_vRecordList.begin() + i );
			AddTeamType( pUser->GetTeam() );	
			break;
		}
	}

	UpdateUserRank();
	
	if( pUser )
	{
		m_KickOutVote.RemoveVoteUserList( pUser->GetPublicID() );
	}
}

void HouseMode::AddTeamType( TeamType eTeam )
{
	if( COMPARE( eTeam, TEAM_PRIVATE_1, TEAM_PRIVATE_190+1 ) )
		m_TeamList.push_back( eTeam ); 
}

void HouseMode::RemoveTeamType( TeamType eTeam )
{
	int iTeamSize = m_TeamList.size();
	for( int i = 0; i < iTeamSize; i++ )
	{
		if( m_TeamList[i] == eTeam )
		{
			m_TeamList.erase( m_TeamList.begin() + i );
			break;
		}
	}
}

void HouseMode::ProcessPlay()
{
	ProcessRevival();
	CheckFieldItemLiveTime();
	CheckItemSupply( m_dwStateChangeTime );	
	CheckBallSupply( m_dwStateChangeTime );
	CheckMachineSupply( m_dwStateChangeTime );
	CheckNeedSendPushStruct();
}

void HouseMode::UpdateRoundRecord()
{
	int iRecordCnt = GetRecordCnt();
	for( int i=0 ; i<iRecordCnt ; i++ )
	{
		ModeRecord *pRecord = FindModeRecord( i );
		if( pRecord )
		{
			if( pRecord->pUser )
			{
				pRecord->pUser->UpdateCharLimitDate();
				pRecord->pUser->UpdateEtcItemTime( __FUNCTION__ );
				pRecord->pUser->DeleteEtcItemPassedDate();
				pRecord->pUser->DeleteExtraItemPassedDate(true);
				pRecord->pUser->DeleteMedalItemPassedDate(true);
				pRecord->pUser->DeleteExMedalSlotPassedDate();
				pRecord->pUser->DeleteCharAwakePassedDate( );
				pRecord->pUser->DeleteCostumePassedDate();
				pRecord->pUser->ReleaseAccessoryPassedDate();
				pRecord->pUser->DeleteExpiredBonusCash();
			}
		}
	}

	UpdateUserRank();	
}

ModeType HouseMode::GetModeType() const
{
	return MT_HOUSE;
}

bool HouseMode::ProcessTCPPacket( User *pSend, SP2Packet &rkPacket )
{
	if( Mode::ProcessTCPPacket( pSend, rkPacket ) )
		return true;
	
	switch( rkPacket.GetPacketID() )
	{
	case CTPK_PERSONAL_HQ_INVITE:
		OnUserInvite( pSend, rkPacket );
		return true;
	}
	return false;
}

void HouseMode::OnUserInvite( User *pSend, SP2Packet &rkPacket )
{
	if( m_pCreator->IsRoomFull() )
		return;

	int i				= 0;
	int iInviteCount	= 0;

	PACKET_GUARD_VOID( rkPacket.Read(iInviteCount) );

	SP2Packet kPacket( STPK_PERSONAL_HQ_INVITE );
	PACKET_GUARD_VOID( kPacket.Write(m_szMasterName) );
	PACKET_GUARD_VOID( kPacket.Write(GetMaxPlayer()) );
	
	int iRecordCnt = GetRecordCnt();
	PACKET_GUARD_VOID( kPacket.Write(iRecordCnt) );

	for( i = 0; i < iRecordCnt; i++ )
	{
		ModeRecord *pRecord = FindModeRecord( i );
		if( !pRecord || !pRecord->pUser )
		{
			PACKET_GUARD_VOID( kPacket.Write("") );
			PACKET_GUARD_VOID( kPacket.Write(0) );
			PACKET_GUARD_VOID( kPacket.Write(0) );
			continue;
		}
		if( pRecord->pUser->IsObserver() || pRecord->pUser->IsStealth() )
		{
			PACKET_GUARD_VOID( kPacket.Write("") );
			PACKET_GUARD_VOID( kPacket.Write(0) );
			PACKET_GUARD_VOID( kPacket.Write(0) );
			continue;
		}
		
		PACKET_GUARD_VOID( kPacket.Write(pRecord->pUser->GetPublicID()) );
		PACKET_GUARD_VOID( kPacket.Write(pRecord->pUser->GetGradeLevel()) );
		PACKET_GUARD_VOID( kPacket.Write(pRecord->pUser->GetPingStep()) );
	}

    
	for( i = 0; i < iInviteCount; i++ )
	{
		ioHashString szInvitedID;
		PACKET_GUARD_VOID( rkPacket.Read(szInvitedID) );

		UserParent *pUser = g_UserNodeManager.GetGlobalUserNode( szInvitedID );
		if( pUser )
		{			
			pUser->RelayPacket( kPacket );
		}
	}
}

TeamType HouseMode::GetNextTeamType()
{
	if( m_TeamList.empty() )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "HeadquartersMode::GetNextTeamType Not Team : %d", (int)m_vRecordList.size() );
		return TEAM_NONE;
	}

	return (TeamType)m_TeamList[0];
}

void HouseMode::UpdateUserDieTime( User *pDier )
{
	ModeRecord *pDieRecord = FindModeRecord( pDier );
	if( pDieRecord && pDieRecord->bDieState )
	{
		DWORD dwRevivalGap = (DWORD)GetRevivalGapTime( pDieRecord->iRevivalCnt );
		pDieRecord->dwCurDieTime = TIMEGETTIME();
		pDieRecord->dwRevivalGap = dwRevivalGap;
		pDieRecord->iRevivalCnt++;
	}
}

void HouseMode::GetModeInfo( SP2Packet &rkPacket )
{
	PACKET_GUARD_VOID( rkPacket.Write(GetModeType()) );
	PACKET_GUARD_VOID( rkPacket.Write(m_szMasterName) );
	PACKET_GUARD_VOID( rkPacket.Write(m_bJoinLock) );
	PACKET_GUARD_VOID( rkPacket.Write(GetMaxPlayer()) );

	int iPosCnt = m_TeamPosArray.size();
	PACKET_GUARD_VOID( rkPacket.Write(iPosCnt) );

	for( int i = 0; i < iPosCnt; i++ )
		PACKET_GUARD_VOID( rkPacket.Write(m_TeamPosArray[i]) );
}

int HouseMode::GetRecordCnt() const
{
	return m_vRecordList.size();
}

const char* HouseMode::GetModeINIFileName() const
{
	return "config/headquartersmode.ini";
}

ModeRecord* HouseMode::FindModeRecord( const ioHashString &rkName )
{
	if( rkName.IsEmpty() )	return NULL;

	CRASH_GUARD();
	int iRecordCnt = m_vRecordList.size();
	for( int i=0 ; i<iRecordCnt ; i++ )
	{
		if( m_vRecordList[i].pUser->GetPublicID() == rkName )
		{
			return &m_vRecordList[i];
		}
	}

	return NULL;
}

ModeRecord* HouseMode::FindModeRecord( User *pUser )
{
	if( !pUser )	return NULL;

	int iRecordCnt = m_vRecordList.size();
	for( int i=0 ; i<iRecordCnt ; i++ )
	{
		if( m_vRecordList[i].pUser == pUser )
			return &m_vRecordList[i];
	}

	return NULL;
}

ModeRecord* HouseMode::FindModeRecord( int iIdx )
{
	if( COMPARE( iIdx, 0, GetRecordCnt() ) )
		return &m_vRecordList[iIdx];

	return NULL;
}

int HouseMode::GetUserKickVoteLimit( const ioHashString &szKickUserName )
{
	ModeRecord *pKickRecord = FindModeRecord( szKickUserName );
	if( !pKickRecord || !pKickRecord->pUser )
		return USER_KICK_VOTE_PROPOSAL_ERROR_7;

	// 인원 체크 
	if( !pKickRecord->pUser->IsObserver() )
	{
		int iPlayUserCount = 0;
		int iRecordCnt = GetRecordCnt();
		for(int i = 0;i < iRecordCnt;i++)
		{
			ModeRecord *pRecord = FindModeRecord( i );
			if( !pRecord || !pRecord->pUser ) continue;
			if( pRecord->pUser->IsObserver() ) continue;
			if( pRecord->pUser->IsStealth() ) continue;

			iPlayUserCount++;
		}
		if( iPlayUserCount <= m_KickOutVote.GetKickVoteUserPool() )
			return USER_KICK_VOTE_PROPOSAL_ERROR_11;
	}
	return 0;
}

void HouseMode::OnModeJoinLockUpdate( User *pSend, bool bJoinLock )
{
	/*if( pSend == NULL ) return;
	if( m_szMasterName != pSend->GetPublicID() ) return;

	m_bJoinLock = bJoinLock;

	SP2Packet kPacket( STPK_PERSONAL_HQ_COMMAND );
	PACKET_GUARD_VOID( kPacket.Write(PERSONAL_HQ_CMD_JOINLOCK_CHANGE) );
	PACKET_GUARD_VOID( kPacket.Write(m_bJoinLock) );
	SendRoomAllUser( kPacket );*/
}

HouseModeRecord* HouseMode::FindHQRecord( const ioHashString &rkName )
{
	return (HouseModeRecord*)FindModeRecord( rkName );
}

HouseModeRecord* HouseMode::FindHQRecord( User *pUser )
{
	return (HouseModeRecord*)FindModeRecord( pUser );
}

HouseModeRecord* HouseMode::FindHQRecordByUserID( const ioHashString &rkUserID )
{
	if( rkUserID.IsEmpty() )	return NULL;

	CRASH_GUARD();
	int iRecordCnt = m_vRecordList.size();
	for( int i=0 ; i<iRecordCnt ; i++ )
	{
		if( m_vRecordList[i].pUser->GetPublicID() == rkUserID )
		{
			return &m_vRecordList[i];
		}
	}

	return NULL;
}

void HouseMode::GetCharModeInfo( SP2Packet &rkPacket, const ioHashString &szName, bool bDieCheck )
{
	HouseModeRecord *pRecord = FindHQRecord( szName );
	if( pRecord )
	{
		// 레코드 정보 유무
		rkPacket << true;

		int iKillSize = pRecord->iKillInfoMap.size();
		rkPacket << iKillSize;

		LOOP_GUARD();
		KillDeathInfoMap::iterator iter_k = pRecord->iKillInfoMap.begin();
		while( iter_k != pRecord->iKillInfoMap.end() )
		{
			rkPacket << iter_k->first;
			rkPacket << iter_k->second;

			++iter_k;
		}
		LOOP_GUARD_CLEAR();

		int iDeathSize = pRecord->iDeathInfoMap.size();
		rkPacket << iDeathSize;

		LOOP_GUARD();
		KillDeathInfoMap::iterator iter_d = pRecord->iDeathInfoMap.begin();
		while( iter_d != pRecord->iDeathInfoMap.end() )
		{
			rkPacket << iter_d->first;
			rkPacket << iter_d->second;

			++iter_d;
		}
		LOOP_GUARD_CLEAR();

		if( bDieCheck )
		{
			rkPacket << pRecord->bDieState;
		}
		rkPacket << pRecord->bCatchState;
	}
	else
	{
		// 레코드 정보 유무
		rkPacket << false;
	}
}

void HouseMode::MasterExistHQ(User* pUser)
{
	SP2Packet kPacket( STPK_PERSONAL_HQ_COMMAND );
	PACKET_GUARD_VOID( kPacket.Write(PERSONAL_HQ_CMD_LEAVE_MASTER) );
	SendRoomAllUser( kPacket, pUser );
}

void HouseMode::SetHouseMaster(const ioHashString &rkName, const DWORD dwMasterIndex)
{
	m_szMasterName	= rkName;
	m_dwMasterIndex	= dwMasterIndex;
}

void HouseMode::SetCreateTime()
{
	m_dwCreateTime = GetTickCount();
}

BOOL HouseMode::IsDeleteTime()
{
	DWORD dwCurTime		= GetTickCount();
	DWORD dwCreateLifeTime	= 3 * 60 * 1000;

	if( dwCurTime - dwCreateLifeTime > m_dwCreateTime )
		return TRUE;

	return FALSE;
}
