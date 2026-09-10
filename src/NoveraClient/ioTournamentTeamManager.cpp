
#include "stdafx.h"

#include "ioTournamentTeamManager.h"

#include "GUI/ioMessageBox.h"

TeamGlobalData::TeamGlobalData()
{
	m_Position    = 0;
	m_MaxPlayer   = 0;
	m_iCheerPoint = 0;
	m_TourPos	  = 0;
	m_iLadderPoint= 0;
	m_CampPos	  = 0;

	m_dwSyncTime  = 0;

	m_bUpdateSubUI	  = false;
	m_bUpdateMainUI	  = false;
	m_bUpdateInfoUI   = false;
}

TeamGlobalData::~TeamGlobalData()
{

}

bool TeamGlobalData::IsAlreadyUser( DWORD dwUserIndex )
{
	TeamUserVec::iterator iter = m_DefaultData.m_TeamUserList.begin();
	for(;iter != m_DefaultData.m_TeamUserList.end();iter++)
	{
		TeamUserData &rkUserData = *iter;
		if( rkUserData.m_dwUserIndex == dwUserIndex )
			return true;
	}
	return false;
}

void TeamGlobalData::SetDefaultData( TeamData &rkTeamData )
{
	m_DefaultData.m_dwTourIndex = rkTeamData.m_dwTourIndex;
	m_DefaultData.m_dwTeamIndex = rkTeamData.m_dwTeamIndex;
	m_DefaultData.m_szTeamName  = rkTeamData.m_szTeamName;
	m_DefaultData.m_dwTeamOwnerIndex = rkTeamData.m_dwTeamOwnerIndex;

	for(int i = 0;i < (int)rkTeamData.m_TeamUserList.size();i++)
	{
		if( IsAlreadyUser( rkTeamData.m_TeamUserList[i].m_dwUserIndex ) ) continue;

		m_DefaultData.m_TeamUserList.push_back( rkTeamData.m_TeamUserList[i] );
	}
}

void TeamGlobalData::SetSyncData( SP2Packet &rkPacket )
{
	m_bUpdateSubUI	  = true;
	m_bUpdateMainUI	  = true;
	m_bUpdateInfoUI	  = true;
	m_dwSyncTime = FRAMEGETTIME();

	rkPacket >> m_Position >> m_MaxPlayer >> m_iCheerPoint >> m_TourPos >> m_iLadderPoint >> m_CampPos;

	// 내 팀이 아니면 팀원 정보를 갱신
	ioUserTournament *pUserTournament = g_MyInfo.GetTournament();
	if( pUserTournament )
	{
		if( !pUserTournament->IsAlreadyTeam( GetTeamIndex() ) )
		{
			// 유저 정보 요청
			bool bEntryMemberCall = false;
			SP2Packet kPacket( CTPK_TOURNAMENT_TEAM_USER_LIST );
			kPacket << m_DefaultData.m_dwTourIndex << m_DefaultData.m_dwTeamIndex << bEntryMemberCall;
			TCPNetwork::SendToServer( kPacket );
		}
	}
}

void TeamGlobalData::ClearUserList()
{
	m_bUpdateSubUI	  = true;
	m_bUpdateMainUI	  = true;
	m_bUpdateInfoUI	  = true;

	m_DefaultData.m_TeamUserList.clear();
}

void TeamGlobalData::AddUserList( DWORD dwUserIndex, const ioHashString &rkUserNick )
{
	if( IsAlreadyUser( dwUserIndex ) ) return;

	m_bUpdateSubUI	  = true;
	m_bUpdateMainUI	  = true;
	m_bUpdateInfoUI	  = true;
	
	TeamUserData kUserData;
	kUserData.m_dwUserIndex = dwUserIndex;
	kUserData.m_szNick		= rkUserNick;
	m_DefaultData.m_TeamUserList.push_back( kUserData );
}

void TeamGlobalData::GetUserList( TeamUserVec &rkUserList )
{
	TeamUserVec::iterator iter = m_DefaultData.m_TeamUserList.begin();
	for(;iter != m_DefaultData.m_TeamUserList.end();++iter)
	{
		TeamUserData &rkUserData = *iter;
		if( rkUserData.m_dwUserIndex == m_DefaultData.m_dwTeamOwnerIndex )
			rkUserList.insert( rkUserList.begin(), rkUserData );
		else
			rkUserList.push_back( rkUserData );
	}
}

void TeamGlobalData::DeleteTeamUserData( DWORD dwUserIndex )
{
	m_bUpdateSubUI	  = true;
	m_bUpdateMainUI	  = true;
	m_bUpdateInfoUI	  = true;

	TeamUserVec::iterator iter = m_DefaultData.m_TeamUserList.begin();
	for(;iter != m_DefaultData.m_TeamUserList.end();++iter)
	{
		TeamUserData &rkUserData = *iter;
		if( rkUserData.m_dwUserIndex == dwUserIndex )
		{
			m_DefaultData.m_TeamUserList.erase( iter );
			return;
		}
	}
}

bool TeamGlobalData::IsUpdateMainUI()
{
	bool bReturn = m_bUpdateMainUI;
	m_bUpdateMainUI = false;
	return bReturn;
}

bool TeamGlobalData::IsUpdateSubUI()
{
	bool bReturn = m_bUpdateSubUI;
	m_bUpdateSubUI = false;
	return bReturn;
}

bool TeamGlobalData::IsUpdateInfoUI()
{
	bool bReturn = m_bUpdateInfoUI;
	m_bUpdateInfoUI = false;
	return bReturn;
}

bool TeamGlobalData::IsDataSync()
{
	if( m_dwSyncTime == 0 ) return false;

	ioUserTournament *pTournament = g_MyInfo.GetTournament();
	if( pTournament && pTournament->IsAlreadyTeam( GetTeamIndex() ) )
	{
		if( FRAMEGETTIME() - m_dwSyncTime > DATA_SYNC_TIME_OWNER ) return false;
	}
	else
	{
		if( FRAMEGETTIME() - m_dwSyncTime > DATA_SYNC_TIME_OTHER ) return false;
	}

	return true;
}

void TeamGlobalData::SendDataSync()
{
	m_dwSyncTime = FRAMEGETTIME();

	SP2Packet kPacket( CTPK_TOURNAMENT_TEAM_INFO );
	kPacket << m_DefaultData.m_dwTourIndex << m_DefaultData.m_dwTeamIndex;
	TCPNetwork::SendToServer( kPacket );
}
//////////////////////////////////////////////////////////////////////////
template<> ioTournamentTeamManager* Singleton< ioTournamentTeamManager >::ms_Singleton = 0;
ioTournamentTeamManager::ioTournamentTeamManager()
{
}

ioTournamentTeamManager::~ioTournamentTeamManager()
{
	TeamGlobalDataVec::iterator iter = m_TournamentTeamList.begin();
	for(;iter != m_TournamentTeamList.end();++iter)
	{
		SAFEDELETE( *iter );
	}
	m_TournamentTeamList.clear();
}

ioTournamentTeamManager& ioTournamentTeamManager::GetSingleton()
{
	return Singleton< ioTournamentTeamManager >::GetSingleton();
}


TeamGlobalData *ioTournamentTeamManager::GetTeamData( DWORD dwIndex, bool bDataSync )
{
	TeamGlobalDataVec::iterator iter = m_TournamentTeamList.begin();
	for(;iter != m_TournamentTeamList.end();++iter)
	{
		TeamGlobalData *pTeam = *iter;
		if( pTeam == NULL ) continue;

		if( pTeam->GetTeamIndex() == dwIndex )
		{
			if( bDataSync )
			{
				if( !pTeam->IsDataSync() )
					pTeam->SendDataSync();
			}
			return pTeam;
		}
	}
	return NULL;
}

TeamGlobalData *ioTournamentTeamManager::SetDefaultData( TeamData &rkTeamData, bool bNewSync /*= false*/ )
{
	TeamGlobalDataVec::iterator iter = m_TournamentTeamList.begin();
	for(;iter != m_TournamentTeamList.end();++iter)
	{
		TeamGlobalData *pTeam = *iter;
		if( pTeam == NULL ) continue;

		if( pTeam->GetTeamIndex() == rkTeamData.m_dwTeamIndex )
			return pTeam;      // 이미 존재한다.
	}

	// 없으면 추가
	TeamGlobalData *pTeamGlobalData = new TeamGlobalData;
	pTeamGlobalData->SetDefaultData( rkTeamData );
	m_TournamentTeamList.push_back( pTeamGlobalData );
	if( bNewSync )
	{
		pTeamGlobalData->SendDataSync();
	}
	return pTeamGlobalData;
}

void ioTournamentTeamManager::NewTeamGlobalData( DWORD dwTourIdx, DWORD dwTeamIdx )
{
	TeamGlobalDataVec::iterator iter = m_TournamentTeamList.begin();
	for( ; iter != m_TournamentTeamList.end(); ++iter )
	{
		TeamGlobalData *pTeam = *iter;
		if( pTeam == NULL ) 
			continue;

		if( pTeam->GetTeamIndex() == dwTeamIdx )
			return;
	}

	SP2Packet kPacket( CTPK_TOURNAMENT_TEAM_INFO );
	kPacket << dwTourIdx << dwTeamIdx;
	TCPNetwork::SendToServer( kPacket );
}

TeamGlobalData *ioTournamentTeamManager::SetSyncData( DWORD dwTeamIndex, SP2Packet &rkPacket )
{
	TeamGlobalData *pTeamData = GetTeamData( dwTeamIndex, false );
	if( pTeamData )
	{
		pTeamData->SetSyncData( rkPacket );
	}
	return pTeamData;
}

void ioTournamentTeamManager::ClearTeamUserData( DWORD dwTeamIndex )
{
	TeamGlobalData *pTeamData = GetTeamData( dwTeamIndex, false );
	if( pTeamData )
	{
		pTeamData->ClearUserList();
	}
}

TeamGlobalData *ioTournamentTeamManager::AddTeamUserData( DWORD dwTeamIndex, DWORD dwUserIndex, const ioHashString &rkUserNick )
{
	TeamGlobalData *pTeamData = GetTeamData( dwTeamIndex, false );
	if( pTeamData )
	{
		pTeamData->AddUserList( dwUserIndex, rkUserNick );
	}
	return pTeamData;
}

void ioTournamentTeamManager::DeleteTeamUserData( DWORD dwTeamIndex, DWORD dwUserIndex )
{
	TeamGlobalData *pTeamData = GetTeamData( dwTeamIndex, false );
	if( pTeamData )
	{
		pTeamData->DeleteTeamUserData( dwUserIndex );
	}
}

void ioTournamentTeamManager::DeleteTournamentTeam( DWORD dwTourIndex )
{
	TeamGlobalDataVec::iterator iter = m_TournamentTeamList.begin();
	for(;iter != m_TournamentTeamList.end();)
	{
		TeamGlobalData *pTeam = *iter;
		if( pTeam == NULL )
		{
			++iter;
			continue;
		}

		if( pTeam->GetTourIndex() == dwTourIndex )
		{
			SAFEDELETE( pTeam );
			iter = m_TournamentTeamList.erase( iter );
		}
		else
		{
			++iter;
		}
	}
}

void ioTournamentTeamManager::SetFullData( SP2Packet &rkPacket )
{
	TeamData kTeamData;
	rkPacket >> kTeamData.m_dwTourIndex >> kTeamData.m_dwTeamIndex >> kTeamData.m_szTeamName >> kTeamData.m_dwTeamOwnerIndex;
	
	SetDefaultData( kTeamData );
	SetSyncData( kTeamData.m_dwTeamIndex, rkPacket );
}

void ioTournamentTeamManager::UpdateCheerPoint( DWORD dwTeamIdx, int iCheerPoint )
{
	TeamGlobalData* pTeamData = GetTeamData( dwTeamIdx, false );
	if( pTeamData && pTeamData->GetCheerPoint() < iCheerPoint )
		pTeamData->SetCheerPoint( iCheerPoint );
}