
#include "stdafx.h"

#include "ioUserTournament.h"

#include "ioTournamentTeamManager.h"
#include "GUI/TournamentMainWnd.h"

ioUserTournament::ioUserTournament()
{
	m_bUpdateMainUI= false;
	m_bUpdateSubUI = false;
}

ioUserTournament::~ioUserTournament()
{
	TeamDataVec::iterator iter = m_TeamDataList.begin();
	for(;iter != m_TeamDataList.end();iter++)
	{
		TeamData &rkTeamData = *iter;
		rkTeamData.m_TeamUserList.clear();
	}
	m_TeamDataList.clear();
}

bool ioUserTournament::IsAlreadyTeam( DWORD dwTeamIndex )
{
	TeamDataVec::iterator iter = m_TeamDataList.begin();
	for(;iter != m_TeamDataList.end();iter++)
	{
		TeamData &rkTeamData = *iter;
		if( rkTeamData.m_dwTeamIndex == dwTeamIndex )
			return true;
	}
	return false;
}

bool ioUserTournament::IsAlreadyUser( DWORD dwTeamIndex, DWORD dwUserIndex )
{
	TeamDataVec::iterator iter = m_TeamDataList.begin();
	for(;iter != m_TeamDataList.end();iter++)
	{
		TeamData &rkTeamData = *iter;
		if( rkTeamData.m_dwTeamIndex == dwTeamIndex )
		{
			TeamUserVec::iterator iter2 = rkTeamData.m_TeamUserList.begin();
			for(;iter2 != rkTeamData.m_TeamUserList.end();iter2++)
			{
				TeamUserData &rkUserData = *iter2;
				if( rkUserData.m_dwUserIndex == dwUserIndex )
					return true;
			}
		}
	}
	return false;
}

void ioUserTournament::ApplyTeamData( SP2Packet &rkPacket )
{
	// 로그인시에 1회만 발생하는 패킷

	int iTeamSize;
	rkPacket >> iTeamSize;

	for(int i = 0;i < iTeamSize;i++)
	{
		BYTE  TourPos;
		SHORT Position;
		TeamData kTeamData;
		rkPacket >> kTeamData.m_dwTourIndex >> kTeamData.m_dwTeamIndex >> kTeamData.m_szTeamName >> kTeamData.m_dwTeamOwnerIndex >> Position >> TourPos;		

		if( IsAlreadyTeam( kTeamData.m_dwTeamIndex ) ) continue;

		m_TeamDataList.push_back( kTeamData );

		// 추가
		TeamGlobalData *pTeamGlobalData = g_TournamentTeamMgr.SetDefaultData( kTeamData );
		if( pTeamGlobalData )
		{
			pTeamGlobalData->SetPosition( Position );
			pTeamGlobalData->SetTourPos( TourPos );
		}
		
		// 유저 정보 요청
		bool bEntryMemberCall = false;
		if( kTeamData.m_dwTeamOwnerIndex == g_MyInfo.GetUserIndex() )
			bEntryMemberCall = true;
		
		SP2Packet kPacket( CTPK_TOURNAMENT_TEAM_USER_LIST );
		kPacket << kTeamData.m_dwTourIndex << kTeamData.m_dwTeamIndex << bEntryMemberCall;
		TCPNetwork::SendToServer( kPacket );
	}
	m_bUpdateMainUI = true;
	m_bUpdateSubUI  = true;
}

void ioUserTournament::ApplyUserData( DWORD dwTeamIndex, SP2Packet &rkPacket )
{
	TeamData &rkTeamData = GetTeamData( dwTeamIndex );

	if( rkTeamData.m_dwTeamIndex == dwTeamIndex )
		rkTeamData.m_TeamUserList.clear();
	
	g_TournamentTeamMgr.ClearTeamUserData( dwTeamIndex );

	int iUserSize;
	rkPacket >> iUserSize;
	for(int i = 0;i < iUserSize;i++)
	{
		ioHashString szNick;
		DWORD dwTableIndex, dwUserIndex;
		int iGradeLevel, iLadderPoint, dwGuildIndex;

		rkPacket >> dwTableIndex >> dwUserIndex >> szNick >> iGradeLevel >> iLadderPoint >> dwGuildIndex;

		g_UserInfoMgr.SetGrade( szNick, iGradeLevel );
		g_UserInfoMgr.SetGuildIndex( szNick, dwGuildIndex );
		g_UserInfoMgr.SetLadderPoint( szNick, iLadderPoint );
		if( rkTeamData.m_dwTeamIndex == dwTeamIndex )                 // 내 팀일 때만
		{
			if( !IsAlreadyUser( dwTeamIndex, dwUserIndex ) )
			{
				TeamUserData kUserData;
				kUserData.m_szNick       = szNick;
				kUserData.m_dwTableIndex = dwTableIndex;
				kUserData.m_dwUserIndex  = dwUserIndex;
				rkTeamData.m_TeamUserList.push_back( kUserData );		
			}
		}
		g_TournamentTeamMgr.AddTeamUserData( dwTeamIndex, dwUserIndex, szNick );
	}
	m_bUpdateMainUI = true;
	m_bUpdateSubUI  = true;
}

void ioUserTournament::CreateTeamData( SP2Packet &rkPacket )
{
	TeamData kTeamData;
	rkPacket >> kTeamData.m_dwTourIndex >> kTeamData.m_dwTeamIndex >> kTeamData.m_szTeamName >> kTeamData.m_dwTeamOwnerIndex;

	if( IsAlreadyTeam( kTeamData.m_dwTeamIndex ) ) return;

	m_TeamDataList.push_back( kTeamData );

	g_TournamentTeamMgr.SetDefaultData( kTeamData );
	TeamGlobalData *pSynData = g_TournamentTeamMgr.SetSyncData( kTeamData.m_dwTeamIndex, rkPacket );

	m_bUpdateMainUI = true;
	m_bUpdateSubUI  = true;

	bool bEntryMemberCall = false;
	SP2Packet kPacket( CTPK_TOURNAMENT_TEAM_USER_LIST );
	kPacket << kTeamData.m_dwTourIndex << kTeamData.m_dwTeamIndex << bEntryMemberCall;
	TCPNetwork::SendToServer( kPacket );

	if( pSynData )
	{
		TournamentTeamCreateComplete *pCreateComplete = dynamic_cast< TournamentTeamCreateComplete * >( g_GUIMgr.FindWnd( TOURNAMENT_TEAM_CREATE_COMPLETE ) );
		if( pCreateComplete )
		{
			pCreateComplete->SetTeamData( pSynData->GetTourIndex(), pSynData->GetTeamIndex(), pSynData->GetTeamName(), pSynData->GetCampPos() );
		}
	}
}

void ioUserTournament::JoinTeamData( SP2Packet &rkPacket )
{
	BYTE  TourPos;
	SHORT Position;
	TeamData kTeamData;
	rkPacket >> kTeamData.m_dwTourIndex >> kTeamData.m_dwTeamIndex >> kTeamData.m_szTeamName >> kTeamData.m_dwTeamOwnerIndex >> Position >> TourPos;	

	TeamGlobalData *pTeamGlobalData = g_TournamentTeamMgr.GetTeamData( kTeamData.m_dwTeamIndex, false );
	if( pTeamGlobalData )
	{
		pTeamGlobalData->SetPosition( Position );
		pTeamGlobalData->SetTourPos( TourPos );
	}

	if( IsAlreadyTeam( kTeamData.m_dwTeamIndex ) ) return;

	m_TeamDataList.push_back( kTeamData );
	
	m_bUpdateMainUI = true;
	m_bUpdateSubUI  = true;

	bool bEntryMemberCall = false;
	SP2Packet kPacket( CTPK_TOURNAMENT_TEAM_USER_LIST );
	kPacket << kTeamData.m_dwTourIndex << kTeamData.m_dwTeamIndex << bEntryMemberCall;
	TCPNetwork::SendToServer( kPacket );

	// 가입 UI 표시
	TournamentTeamEventWnd *pTeamEventWnd = dynamic_cast< TournamentTeamEventWnd * >( g_GUIMgr.FindWnd( TOURNAMENT_TEAM_EVENT_WND ) );
	if( pTeamEventWnd )
	{
		pTeamEventWnd->SetTeamEventData( kTeamData.m_dwTourIndex, kTeamData.m_dwTeamIndex, TournamentTeamEventWnd::EVENT_TYPE_JOIN );
	}
}

void ioUserTournament::SetTeamUserAdd( DWORD dwTeamIndex, SP2Packet &rkPacket )
{
	TeamDataVec::iterator iter = m_TeamDataList.begin();
	for(;iter != m_TeamDataList.end();++iter)
	{
		TeamData &rkTeamData = *iter;
		if( rkTeamData.m_dwTeamIndex == dwTeamIndex )
		{
			ioHashString szNick;
			DWORD dwTableIndex, dwUserIndex;
			int iGradeLevel, iLadderPoint, dwGuildIndex;
			rkPacket >> dwTableIndex >> dwUserIndex >> szNick >> iGradeLevel >> iLadderPoint >> dwGuildIndex;

			g_UserInfoMgr.SetGrade( szNick, iGradeLevel );
			g_UserInfoMgr.SetGuildIndex( szNick, dwGuildIndex );
			g_UserInfoMgr.SetLadderPoint( szNick, iLadderPoint );

			if( !IsAlreadyUser( dwTeamIndex, dwUserIndex ) )
			{
				TeamUserData kUserData;
				kUserData.m_szNick       = szNick;
				kUserData.m_dwTableIndex = dwTableIndex;
				kUserData.m_dwUserIndex  = dwUserIndex;
				rkTeamData.m_TeamUserList.push_back( kUserData );		

				g_TournamentTeamMgr.AddTeamUserData( dwTeamIndex, dwUserIndex, szNick );
			}

			if( rkTeamData.m_dwTeamOwnerIndex == g_MyInfo.GetUserIndex() )
			{
				// 팀원 가입 UI 표시
				g_GUIMgr.SetMsgBox( MB_OK, NULL, "가입 수락이 완료되었습니다" );
			}
			else if( dwUserIndex != g_MyInfo.GetUserIndex() )
			{
				TournamentTeamAlarmWnd *pTeamAlarmWnd = dynamic_cast< TournamentTeamAlarmWnd * >( g_GUIMgr.FindWnd( TOURNAMENT_TEAM_ALARM_WND ) );
				if( pTeamAlarmWnd )
				{
					pTeamAlarmWnd->SetAlarmData( dwTeamIndex, dwUserIndex, TournamentTeamAlarmWnd::ALARM_TYPE_JOIN_USER );
				}
			}

			m_bUpdateMainUI = true;
			m_bUpdateSubUI  = true;
			return;
		}
	}
}

void ioUserTournament::DeleteTeam( DWORD dwTeamIndex )
{
	TeamDataVec::iterator iter = m_TeamDataList.begin();
	for(;iter != m_TeamDataList.end();++iter)
	{
		TeamData &rkTeamData = *iter;
		if( rkTeamData.m_dwTeamIndex == dwTeamIndex )
		{
			m_TeamDataList.erase( iter );
			m_bUpdateMainUI = true;
			m_bUpdateSubUI  = true;

			g_TournamentTeamMgr.DeleteTeamUserData( dwTeamIndex, g_MyInfo.GetUserIndex() );
			return;
		}
	}
}

bool ioUserTournament::TournamentEndDeleteTeam( DWORD dwTourIndex )
{
	TeamDataVec::iterator iter = m_TeamDataList.begin();
	for(;iter != m_TeamDataList.end();++iter)
	{
		TeamData &rkTeamData = *iter;
		if( rkTeamData.m_dwTourIndex == dwTourIndex )
		{
			TournamentTeamEventWnd *pTeamEventWnd = dynamic_cast< TournamentTeamEventWnd * >( g_GUIMgr.FindWnd( TOURNAMENT_TEAM_EVENT_WND ) );
			if( pTeamEventWnd )
			{				
				// 먼저 세팅한 뒤에 정보를 삭제한다.
				pTeamEventWnd->SetTeamEventData( rkTeamData.m_dwTourIndex, rkTeamData.m_dwTeamIndex, TournamentTeamEventWnd::EVENT_TYPE_TEAM_DELETE );
			}

			m_TeamDataList.erase( iter );
			m_bUpdateMainUI = true;
			m_bUpdateSubUI  = true;

			g_TournamentTeamMgr.DeleteTournamentTeam( dwTourIndex );
			DeleteCheerTeam( dwTourIndex ); 

			return true;
		}
	}
	return false;
}

void ioUserTournament::LeaveTeamUser( DWORD dwTeamIndex, DWORD dwLeaveIndex )
{
	TeamDataVec::iterator iter = m_TeamDataList.begin();
	for(;iter != m_TeamDataList.end();++iter)
	{
		TeamData &rkTeamData = *iter;
		if( rkTeamData.m_dwTeamIndex == dwTeamIndex )
		{
			TeamUserVec::iterator iter2 = rkTeamData.m_TeamUserList.begin();
			for(;iter2 != rkTeamData.m_TeamUserList.end();++iter2)
			{
				TeamUserData &rkUserData = *iter2;
				if( rkUserData.m_dwUserIndex == dwLeaveIndex )
				{
					g_TournamentTeamMgr.DeleteTeamUserData( rkTeamData.m_dwTeamIndex, rkUserData.m_dwUserIndex );

					rkTeamData.m_TeamUserList.erase( iter2 );
					m_bUpdateMainUI = true;
					m_bUpdateSubUI  = true;
					return;
				}
			}		
		}
	}
}

TeamData &ioUserTournament::GetTeamData( DWORD dwTeamIndex )
{
	TeamDataVec::iterator iter = m_TeamDataList.begin();
	for(;iter != m_TeamDataList.end();++iter)
	{
		TeamData &rkTeamData = *iter;
		if( rkTeamData.m_dwTeamIndex == dwTeamIndex )
			return rkTeamData;
	}

	static TeamData stNull;
	return stNull;
}

DWORD ioUserTournament::GetTeamIndex( DWORD dwTourIndex )
{
	TeamDataVec::iterator iter = m_TeamDataList.begin();
	for(;iter != m_TeamDataList.end();++iter)
	{
		TeamData &rkTeamData = *iter;
		if( rkTeamData.m_dwTourIndex == dwTourIndex )
			return rkTeamData.m_dwTeamIndex;
	}
	return 0;
}

TeamUserData ioUserTournament::GetTeamUserData( DWORD dwTeamIndex, const ioHashString &rkName )
{
	TeamDataVec::iterator iter = m_TeamDataList.begin();
	for(;iter != m_TeamDataList.end();++iter)
	{
		TeamData &rkTeamData = *iter;
		if( rkTeamData.m_dwTeamIndex == dwTeamIndex )
		{
			TeamUserVec::iterator iter2 = rkTeamData.m_TeamUserList.begin();
			for(;iter2 != rkTeamData.m_TeamUserList.end();++iter2)
			{
				TeamUserData &rkUserData = *iter2;
				if( rkUserData.m_szNick == rkName )
					return rkUserData;
			}
		}
	}

	static TeamUserData stNull;
	return stNull;
}

ioHashString &ioUserTournament::GetTeamUserName( DWORD dwTeamIndex, DWORD dwUserIndex )
{
	TeamDataVec::iterator iter = m_TeamDataList.begin();
	for(;iter != m_TeamDataList.end();++iter)
	{
		TeamData &rkTeamData = *iter;
		if( rkTeamData.m_dwTeamIndex == dwTeamIndex )
		{
			TeamUserVec::iterator iter2 = rkTeamData.m_TeamUserList.begin();
			for(;iter2 != rkTeamData.m_TeamUserList.end();++iter2)
			{
				TeamUserData &rkUserData = *iter2;
				if( rkUserData.m_dwUserIndex == dwUserIndex )
					return rkUserData.m_szNick;
			}
		}
	}

	static ioHashString stNull;
	return stNull;
}

void ioUserTournament::GetTeamList( TeamDataVec &rkTeamList )
{
	TeamDataVec::iterator iter = m_TeamDataList.begin();
	for(;iter != m_TeamDataList.end();++iter)
	{
		rkTeamList.push_back( *iter );
	}
}

int ioUserTournament::GetCurrentTeamUserCount( DWORD dwTourIndex )
{
	TeamDataVec::iterator iter = m_TeamDataList.begin();
	for(;iter != m_TeamDataList.end();++iter)
	{
		TeamData &rkTeamData = *iter;
		if( rkTeamData.m_dwTourIndex == dwTourIndex )
			return rkTeamData.m_TeamUserList.size();
	}
	return 0;
}

bool ioUserTournament::IsTourTeam( DWORD dwTourIndex )
{
	TeamDataVec::iterator iter = m_TeamDataList.begin();
	for(;iter != m_TeamDataList.end(); ++iter)
	{
		TeamData &rkTeamData = *iter;
		if( rkTeamData.m_dwTourIndex == dwTourIndex )
			return true;
	}
	return false;
}

bool ioUserTournament::IsUpdateMainUI()
{
	bool bReturn = m_bUpdateMainUI;
	m_bUpdateMainUI = false;

	return bReturn;
}

bool ioUserTournament::IsUpdateSubUI()
{
	bool bReturn = m_bUpdateSubUI;
	m_bUpdateSubUI = false;

	return bReturn;
}

bool ioUserTournament::IsCheerTeam( DWORD dwTourIdx, DWORD dwTeamIdx )
{
	CheerTeamInfoVec::const_iterator iter = m_CheerList.begin();
	for( ; iter != m_CheerList.end(); ++iter )
	{
		const CheerTeamInfo& rkCheerData = *iter;
		if( rkCheerData.m_dwTourIndex == dwTourIdx && rkCheerData.m_dwTeamIndex == dwTeamIdx )
			return true;
	}

	return false;
}

bool ioUserTournament::IsCheerTournament( DWORD dwTourIdx )
{
	CheerTeamInfoVec::const_iterator iter = m_CheerList.begin();
	for( ; iter != m_CheerList.end(); ++iter )
	{
		const CheerTeamInfo& rkCheerData = *iter;
		if( rkCheerData.m_dwTourIndex == dwTourIdx )
			return true;
	}

	return false;
}

void ioUserTournament::AddCheerTeamList( DWORD dwTourIdx, DWORD dwTeamIdx )
{
	if( IsCheerTournament( dwTourIdx ) )
		return;

	CheerTeamInfo CheerData;
	CheerData.m_dwTourIndex = dwTourIdx;
	CheerData.m_dwTeamIndex = dwTeamIdx;
	m_CheerList.push_back( CheerData );
}

void ioUserTournament::UpdateCheerTeamPage( DWORD dwTourIdx, int iUpdatePage )
{
	if( !IsCheerTournament( dwTourIdx ) )
		return;

	if( iUpdatePage < 0 )
		return;

	CheerTeamInfo& kCheerTeam = GetCheerTeamNotConst( dwTourIdx );	
	kCheerTeam.m_iMyCheerTeamPage = iUpdatePage;
}

const CheerTeamInfo& ioUserTournament::GetCheerTeam( DWORD dwTourIdx )
{
	static CheerTeamInfo kNnone;
	CheerTeamInfoVec::const_iterator iter = m_CheerList.begin();
	for( ; iter != m_CheerList.end(); ++iter )
	{
		const CheerTeamInfo& rkCheerData = *iter;
		if( rkCheerData.m_dwTourIndex == dwTourIdx )
			return rkCheerData;
	}

	return kNnone;
}

CheerTeamInfo& ioUserTournament::GetCheerTeamNotConst( DWORD dwTourIdx )
{
	static CheerTeamInfo kNnone;
	CheerTeamInfoVec::iterator iter = m_CheerList.begin();
	for( ; iter != m_CheerList.end(); ++iter )
	{
		CheerTeamInfo& rkCheerData = *iter;
		if( rkCheerData.m_dwTourIndex == dwTourIdx )
			return rkCheerData;
	}

	return kNnone;
}

void ioUserTournament::DeleteCheerTeam( DWORD dwTourIdx )
{
	for(CheerTeamInfoVec::iterator iter = m_CheerList.begin(); iter != m_CheerList.end(); )
	{
		CheerTeamInfo& rkCheerData = *iter;
		if( rkCheerData.m_dwTourIndex == dwTourIdx )
		{
			iter = m_CheerList.erase( iter );
		}
		else
		{
			++iter;
		}
	}
}