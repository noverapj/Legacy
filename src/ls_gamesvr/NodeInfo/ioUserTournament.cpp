
#include "stdafx.h"

//#include "../Window.h"
#include "../MainProcess.h"
#include "../EtcHelpFunc.h"
#include "../QueryData/QueryResultData.h"
#include "../DataBase/DBClient.h"
#include "../MainServerNode/MainServerNode.h"

#include "User.h"
#include "ioUserTournament.h"
#include "TournamentManager.h"
#include "UserNodeManager.h"


ioUserTournament::ioUserTournament()
{
	Initialize( NULL );
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

bool ioUserTournament::IsInviteCheckTeamSend( DWORD dwBlueIndex, DWORD dwRedIndex )
{
	TeamDataVec::iterator iter = m_TeamDataList.begin();
	for(;iter != m_TeamDataList.end();iter++)
	{
		TeamData &rkTeamData = *iter;
		if( rkTeamData.m_dwTeamIndex == dwBlueIndex ||
			rkTeamData.m_dwTeamIndex == dwRedIndex )
		{
			rkTeamData.m_InvitePosition = rkTeamData.m_Position;
			return true;
		}
	}
	return false;
}

bool ioUserTournament::IsInviteCheckTeamSend( DWORD dwTeamIndex )
{
	TeamDataVec::iterator iter = m_TeamDataList.begin();
	for(;iter != m_TeamDataList.end();iter++)
	{
		TeamData &rkTeamData = *iter;
		if( rkTeamData.m_dwTeamIndex == dwTeamIndex )
		{
			rkTeamData.m_InvitePosition = rkTeamData.m_Position;
			return true;
		}
	}
	return false;
}

bool ioUserTournament::IsTourTeam( DWORD dwTourIndex )
{
	TeamDataVec::iterator iter = m_TeamDataList.begin();
	for(;iter != m_TeamDataList.end();iter++)
	{
		TeamData &rkTeamData = *iter;
		if( rkTeamData.m_dwTourIndex == dwTourIndex )
			return true;
	}
	return false;
}

void ioUserTournament::Initialize( User *pUser )
{
	m_pUser = pUser;

	TeamDataVec::iterator iter = m_TeamDataList.begin();
	for(;iter != m_TeamDataList.end();iter++)
	{
		TeamData &rkTeamData = *iter;
		rkTeamData.m_TeamUserList.clear();
	}
	m_TeamDataList.clear();
}

void ioUserTournament::DBtoData( CQueryResultData *query_data )
{
	if( !m_pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioUserTournament::DBtoData() User NULL!!"); 
		return;
	}
		
	TeamDataVec kTeamList;

	LOOP_GUARD();
	DWORD dwLastIndex = 0;
	while( query_data->IsExist() )
	{		
		// - 팀인덱스, 리그인덱스, 팀이름, 팀장인덱스, 리그포지션, 응원포인트, 토너먼트위치, 진영포인트, 진영타입

		SHORT LeaguePos;
		BYTE TourPos, CampPos;
		int iCheerPoint, iLadderPoint;
		DWORD dwTeamIndex, dwTourIndex, dwOwnerIndex;
		char szTeamName[TOURNAMENT_TEAM_NAME_NUM_PLUS_ONE] = "";

		PACKET_GUARD_BREAK( query_data->GetValue( dwTeamIndex, sizeof(DWORD) ) );
		PACKET_GUARD_BREAK( query_data->GetValue( dwTourIndex, sizeof(DWORD) ) );
		PACKET_GUARD_BREAK( query_data->GetValue( szTeamName, TOURNAMENT_TEAM_NAME_NUM_PLUS_ONE ) );
		PACKET_GUARD_BREAK( query_data->GetValue( dwOwnerIndex, sizeof(DWORD) ) );
		PACKET_GUARD_BREAK( query_data->GetValue( LeaguePos, sizeof(SHORT) ) );
		PACKET_GUARD_BREAK( query_data->GetValue( iCheerPoint, sizeof(int) ) );
		PACKET_GUARD_BREAK( query_data->GetValue( TourPos, sizeof(char) ) );
		PACKET_GUARD_BREAK( query_data->GetValue( iLadderPoint, sizeof(int) ) );
		PACKET_GUARD_BREAK( query_data->GetValue( CampPos, sizeof(char) ) );

		if( IsAlreadyTeam( dwTeamIndex ) )
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioUserTournament::DBtoData() Already Team  : %s - %d - %d", m_pUser->GetPublicID().c_str(), dwTeamIndex, dwTourIndex ); 
			return;        // Error
		}

		TeamData kTeamData;
		kTeamData.m_dwTourIndex      = dwTourIndex;
		kTeamData.m_dwTeamIndex		 = dwTeamIndex;
		kTeamData.m_szTeamName		 = szTeamName;
		kTeamData.m_dwTeamOwnerIndex = dwOwnerIndex;

		kTeamData.m_Position = LeaguePos;
		kTeamData.m_TourPos  = TourPos;
		kTeamList.push_back( kTeamData );

		dwLastIndex = dwTeamIndex;
	}	
	LOOP_GUARD_CLEAR();

	int iCreateSize = kTeamList.size();
	if( iCreateSize == 0 ) return;         // 가입된 팀이 없음
	if( iCreateSize >= TOURNAMENT_TEAM_MAX_LOAD )
	{
		g_DBClient.OnSelectTournamentTeamList( m_pUser->GetUserDBAgentID(), m_pUser->GetAgentThreadID(), m_pUser->GetGUID(), m_pUser->GetUserIndex(), dwLastIndex, TOURNAMENT_TEAM_MAX_LOAD );
	}

	// 유저에게 전송
	SP2Packet kPacket( STPK_TOURNAMENT_TEAM_LIST );
	PACKET_GUARD_VOID( kPacket.Write(iCreateSize) );
	for(int i = 0;i < iCreateSize;i++)
	{
		TeamData kTeamData = kTeamList[i];
		PACKET_GUARD_VOID( kPacket.Write(kTeamData.m_dwTourIndex) );
		PACKET_GUARD_VOID( kPacket.Write(kTeamData.m_dwTeamIndex) ); 
		PACKET_GUARD_VOID( kPacket.Write(kTeamData.m_szTeamName) ); 
		PACKET_GUARD_VOID( kPacket.Write(kTeamData.m_dwTeamOwnerIndex) ); 
		PACKET_GUARD_VOID( kPacket.Write(kTeamData.m_Position) ); 
		PACKET_GUARD_VOID( kPacket.Write(kTeamData.m_TourPos) );		

		m_TeamDataList.push_back( kTeamData );

		// 초대 발송 가능하면 발송한다.
		g_TournamentManager.CheckTournamentBattleInvite( m_pUser, kTeamData.m_dwTourIndex, kTeamData.m_dwTeamIndex );
	}
	m_pUser->SendMessage( kPacket );
	kTeamList.clear();
}

void ioUserTournament::DBtoUserData( DWORD dwTeamIndex, CQueryResultData *query_data )
{
	if( !m_pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioUserTournament::DBtoUserData() User NULL!!"); 
		return;
	}

	TeamUserVec kUserList;

	LOOP_GUARD();
	while( query_data->IsExist() )
	{		
		// - 테이블인덱스, 유저인덱스, 닉네임, 레벨, 진영포인트, 길드인덱스
		TeamUserData kUserData;

		query_data->GetValue( kUserData.m_dwTableIndex, sizeof(DWORD) );
		query_data->GetValue( kUserData.m_dwUserIndex, sizeof(DWORD) );

		char szUserNick[ID_NUM_PLUS_ONE] = "";
		query_data->GetValue( szUserNick, ID_NUM_PLUS_ONE );
		kUserData.m_szNick = szUserNick;

		query_data->GetValue( kUserData.m_iGradeLevel, sizeof(int) );
		query_data->GetValue( kUserData.m_iLadderPoint, sizeof(int) );
		query_data->GetValue( kUserData.m_dwGuildIndex, sizeof(int) );

		kUserList.push_back( kUserData );
	}	
	LOOP_GUARD_CLEAR();

	int i = 0;
	int iTeamSize = (int)m_TeamDataList.size();
	for(i = 0;i < iTeamSize;i++)
	{
		TeamData &rkTeamData = m_TeamDataList[i];
		if( rkTeamData.m_dwTeamIndex == dwTeamIndex )   // 내 팀인지 확인
		{
			rkTeamData.m_TeamUserList.clear();
			int iUserSize = (int)kUserList.size();
			for(int j = 0;j < iUserSize;j++)
			{
				if( IsAlreadyUser( rkTeamData, kUserList[j].m_dwUserIndex ) )
				{
					LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DBtoUserData Already User : %d - %s", dwTeamIndex, kUserList[j].m_szNick.c_str() );
				}
				else
				{
					rkTeamData.m_TeamUserList.push_back( kUserList[j] );
				}
			}
			break;
		}
	}

	// 요청자에게 동기화
	int iUserSize = (int)kUserList.size();
	SP2Packet kPacket( STPK_TOURNAMENT_TEAM_USER_LIST );
	kPacket << dwTeamIndex << iUserSize;
	for(i = 0;i < iUserSize;i++)
	{
		TeamUserData kUserData = kUserList[i];
		kPacket << kUserData.m_dwTableIndex << kUserData.m_dwUserIndex << kUserData.m_szNick;

		UserParent *pUserParent = g_UserNodeManager.GetGlobalUserNode( kUserData.m_dwUserIndex );
		if( pUserParent )
		{
			kUserData.m_iGradeLevel = pUserParent->GetGradeLevel();
			kUserData.m_iLadderPoint= pUserParent->GetLadderPoint();
			kUserData.m_dwGuildIndex= pUserParent->GetGuildIndex();
		}
		kPacket << kUserData.m_iGradeLevel << kUserData.m_iLadderPoint << kUserData.m_dwGuildIndex;
	}
	m_pUser->SendMessage( kPacket );

	kUserList.clear();
}

ioUserTournament::TeamData &ioUserTournament::GetTeamData( DWORD dwTeamIndex )
{
	TeamDataVec::iterator iter = m_TeamDataList.begin();
	for(;iter != m_TeamDataList.end();++iter)
	{
		TeamData &rkTeamData = *iter;
		if( rkTeamData.m_dwTeamIndex == dwTeamIndex )
			return rkTeamData;
	}

	static TeamData kNullTeam;
	return kNullTeam;
}

ioUserTournament::TeamData &ioUserTournament::GetTournamentTeamData( DWORD dwTourIndex )
{

	TeamDataVec::iterator iter = m_TeamDataList.begin();
	for(;iter != m_TeamDataList.end();++iter)
	{
		TeamData &rkTeamData = *iter;
		if( rkTeamData.m_dwTourIndex == dwTourIndex )
			return rkTeamData;
	}

	static TeamData kNullTeam;
	return kNullTeam;
}

void ioUserTournament::FillMoveData( SP2Packet &rkPacket )
{
	int iTeamSize = (int)m_TeamDataList.size();
	rkPacket << iTeamSize;
	for(int i = 0;i < iTeamSize;i++)
	{
		TeamData &rkTeamData = m_TeamDataList[i];
		rkPacket << rkTeamData.m_dwTourIndex << rkTeamData.m_dwTeamIndex << rkTeamData.m_dwTeamOwnerIndex << rkTeamData.m_szTeamName << rkTeamData.m_Position << rkTeamData.m_InvitePosition << rkTeamData.m_TourPos;

		int iUserSize = (int)rkTeamData.m_TeamUserList.size();
		rkPacket << iUserSize;
		for(int j = 0;j < iUserSize;j++)
		{
			TeamUserData &rkUserData = rkTeamData.m_TeamUserList[j];
			rkPacket << rkUserData.m_dwTableIndex << rkUserData.m_dwUserIndex << rkUserData.m_szNick;
		}
	}

	int iCheerSize = (int)m_CheerList.size();
	rkPacket << iCheerSize;
	for( int i = 0; i < iCheerSize; i++ )
	{
		CheerTeamInfo &rkCheerData = m_CheerList[i];
		rkPacket << rkCheerData.m_dwTourIndex << rkCheerData.m_dwTeamIndex;
	}
}

void ioUserTournament::ApplyMoveData( SP2Packet &rkPacket, bool bDummyNode )
{
	int iTeamSize;
	rkPacket >> iTeamSize;
	for(int i = 0;i < iTeamSize;i++)
	{
		TeamData kTeamData;
		rkPacket >> kTeamData.m_dwTourIndex >> kTeamData.m_dwTeamIndex >> kTeamData.m_dwTeamOwnerIndex >> kTeamData.m_szTeamName >> kTeamData.m_Position >> kTeamData.m_InvitePosition >> kTeamData.m_TourPos;

		int iUserSize;
		rkPacket >> iUserSize;
		for(int j = 0;j < iUserSize;j++)
		{
			TeamUserData kUserData;
			rkPacket >> kUserData.m_dwTableIndex >> kUserData.m_dwUserIndex >> kUserData.m_szNick;
			
			kTeamData.m_TeamUserList.push_back( kUserData );
		}
		
		// 초대 발송 체크
		if( kTeamData.m_Position != kTeamData.m_InvitePosition )
		{
			kTeamData.m_InvitePosition = kTeamData.m_Position;
			g_TournamentManager.CheckTournamentBattleInvite( m_pUser, kTeamData.m_dwTourIndex, kTeamData.m_dwTeamIndex );
		}
		m_TeamDataList.push_back( kTeamData );
	}

	m_CheerList.clear();
	int iCheerSize;
	rkPacket >> iCheerSize;	
	for( int i = 0; i < iCheerSize; i++ )
	{
		CheerTeamInfo rkCheerData;		
		rkPacket >> rkCheerData.m_dwTourIndex >> rkCheerData.m_dwTeamIndex;
		m_CheerList.push_back( rkCheerData );
	}
}

bool ioUserTournament::CreateTeamData( DWORD dwTourIndex, DWORD dwTeamIndex, DWORD dwOwnerIndex, ioHashString &rkTeamName, SHORT Position, BYTE TourPos )
{
	if( m_pUser == NULL ) return false;

	if( IsAlreadyTeam( dwTeamIndex ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioUserTournament::CreateTeamData Failed : %s - %d - %d", m_pUser->GetPublicID().c_str(), dwTourIndex, dwTeamIndex );
		return false;
	}

	TeamData kTeamData;
	kTeamData.m_dwTourIndex = dwTourIndex;
	kTeamData.m_dwTeamIndex = dwTeamIndex;
	kTeamData.m_dwTeamOwnerIndex = dwOwnerIndex;
	kTeamData.m_szTeamName  = rkTeamName;

	kTeamData.m_Position    = Position;
	kTeamData.m_TourPos     = TourPos;
	m_TeamDataList.push_back( kTeamData );

	return true;
}

bool ioUserTournament::JoinTeamData( DWORD dwTourIndex, DWORD dwTeamIndex, DWORD dwOwnerIndex, const ioHashString &rkTeamName, SHORT Position, BYTE TourPos )
{
	if( m_pUser == NULL ) return false;

	if( IsAlreadyTeam( dwTeamIndex ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioUserTournament::JoinTeamData Failed : %s - %d - %d", m_pUser->GetPublicID().c_str(), dwTourIndex, dwTeamIndex );
		return false;
	}

	TeamData kTeamData;
	kTeamData.m_dwTourIndex = dwTourIndex;
	kTeamData.m_dwTeamIndex = dwTeamIndex;
	kTeamData.m_dwTeamOwnerIndex = dwOwnerIndex;
	kTeamData.m_szTeamName  = rkTeamName;

	kTeamData.m_Position    = Position;
	kTeamData.m_TourPos     = TourPos;
	m_TeamDataList.push_back( kTeamData );
	return true;
}

void ioUserTournament::DeleteTeamData( DWORD dwTeamIndex )
{
	TeamDataVec::iterator iter = m_TeamDataList.begin();
	for(;iter != m_TeamDataList.end();++iter)
	{
		TeamData &rkTeamData = *iter;
		if( rkTeamData.m_dwTeamIndex == dwTeamIndex )
		{
			m_TeamDataList.erase( iter );
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
			m_TeamDataList.erase( iter );
			return true;
		}
	}
	return false;
}

void ioUserTournament::TournamentTeamPosSync( DWORD dwTeamIndex, SHORT Position, BYTE TourPos, bool bSync )
{
	TeamDataVec::iterator iter = m_TeamDataList.begin();
	for(;iter != m_TeamDataList.end();++iter)
	{
		TeamData &rkTeamData = *iter;
		if( rkTeamData.m_dwTeamIndex == dwTeamIndex )
		{
			rkTeamData.m_Position = Position;
			rkTeamData.m_TourPos  = TourPos;

			if( m_pUser && bSync )
			{
				// 유저에게 전송
				SP2Packet kPacket( STPK_TOURNAMENT_TEAM_POSITION );
				kPacket << dwTeamIndex << Position << TourPos;
				m_pUser->SendMessage( kPacket );
			}
		}
	}
}

void ioUserTournament::LeaveTeamUserNode( DWORD dwTeamIndex, DWORD dwLeaveUserIndex )
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
				if( rkUserData.m_dwUserIndex == dwLeaveUserIndex )
				{
					rkTeamData.m_TeamUserList.erase( iter2 );
					return;
				}
			}
		}
	}
}

void ioUserTournament::LeaveTeamUser( DWORD dwSenderIndex, DWORD dwTeamIndex, DWORD dwLeaveUserIndex )
{
	if( m_pUser == NULL ) return;

	TeamData &rkTeamData = GetTeamData( dwTeamIndex );
	if( rkTeamData.m_dwTeamIndex != dwTeamIndex )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioUserTournament::LeaveTeamUser Null Team : %s - %d", m_pUser->GetPublicID().c_str(), dwTeamIndex );
		return;
	}

	// SenderIndex유저가 동기화 주체가 된다.
	if( m_pUser->GetUserIndex() == dwSenderIndex )
	{
		// 팀원들에게 유저 탈퇴 전송
		TeamUserVec::iterator iter2 = rkTeamData.m_TeamUserList.begin();
		for(;iter2 != rkTeamData.m_TeamUserList.end();++iter2)
		{
			TeamUserData &rkUserData = *iter2;
			if( rkUserData.m_dwUserIndex == dwSenderIndex ) continue;

			UserParent *pUserParent = g_UserNodeManager.GetGlobalUserNode( rkUserData.m_dwUserIndex );
			if( pUserParent )
			{
				if( pUserParent->IsUserOriginal() )
				{
					User *pUser = static_cast< User * >( pUserParent );
					ioUserTournament *pUserTournament = pUser->GetUserTournament();
					if( pUserTournament )
					{
						pUserTournament->LeaveTeamUser( dwSenderIndex, dwTeamIndex, dwLeaveUserIndex );
					}
				}
				else
				{
					UserCopyNode *pUser = static_cast< UserCopyNode * >( pUserParent );
					SP2Packet kPacket( SSTPK_TOURNAMENT_TEAM_LEAVE );
					kPacket << pUser->GetUserIndex() << dwSenderIndex << dwTeamIndex << dwLeaveUserIndex;
					pUser->SendMessage( kPacket );
				}
			}
		}
	}

	// 유저 탈퇴 처리
	LeaveTeamUserNode( dwTeamIndex, dwLeaveUserIndex );

	// 팀이 삭제되어야하는지 체크
	bool bTeamDelete = false;
	if( rkTeamData.m_TeamUserList.empty() )         
	{
		// 메인 서버에 삭제 요청
		SP2Packet kPacket( MSTPK_TOURNAMENT_TEAM_DELETE );
		kPacket << rkTeamData.m_dwTourIndex << rkTeamData.m_dwTeamIndex;
		g_MainServer.SendMessage( kPacket );

		// 리스트에서 삭제
		bTeamDelete = true;
		DeleteTeamData( dwTeamIndex );
		if( rkTeamData.m_dwTeamOwnerIndex != m_pUser->GetUserIndex() )
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioUserTournament::LeaveTeamUser Team Delete Not Master!!! : %s - %d", m_pUser->GetPublicID().c_str(), rkTeamData.m_dwTeamIndex );
		}
	}
	else if( m_pUser->GetUserIndex() == dwLeaveUserIndex )
	{
		// 내가 강퇴되면 팀을 제거한다.
		DeleteTeamData( dwTeamIndex );
	}

	// 유저에게 전송
	SP2Packet kPacket( STPK_TOURNAMENT_TEAM_LEAVE );
	kPacket << TOURNAMENT_TEAM_LEAVE_OK << dwSenderIndex << dwTeamIndex << dwLeaveUserIndex << bTeamDelete;
	m_pUser->SendMessage( kPacket );
}

bool ioUserTournament::IsAlreadyUser( ioUserTournament::TeamData &rkTeamData, DWORD dwUserIndex )
{	
	TeamUserVec::iterator iter = rkTeamData.m_TeamUserList.begin();
	for(;iter != rkTeamData.m_TeamUserList.end();++iter)
	{
		TeamUserData &rkUserData = *iter;
		if( rkUserData.m_dwUserIndex == dwUserIndex )
			return true;
	}

	return false;
}

void ioUserTournament::AddTeamUserData( DWORD dwTeamIndex, ioUserTournament::TeamUserData &rkUserData )
{
	if( m_pUser == NULL ) return;

	int i = 0;
	int iTeamSize = (int)m_TeamDataList.size();
	for(i = 0;i < iTeamSize;i++)
	{
		TeamData &rkTeamData = m_TeamDataList[i];
		if( rkTeamData.m_dwTeamIndex == dwTeamIndex )   // 내 팀인지 확인
		{
			if( IsAlreadyUser( rkTeamData, rkUserData.m_dwUserIndex ) )
			{
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "AddTeamUserData Already User : %d - %s", dwTeamIndex, rkUserData.m_szNick.c_str() );
				return;
			}

			if( m_pUser->GetUserIndex() == rkTeamData.m_dwTeamOwnerIndex )
			{
				// 팀원들에게 전송 --
				AddTeamUserAgreeServerSync( rkTeamData, rkUserData );
			}
	
			// 가입
			rkTeamData.m_TeamUserList.push_back( rkUserData );

			// 가입 내역 전송
			SP2Packet kPacket( STPK_TOURNAMENT_TEAM_ENTRY_AGREE );
			kPacket << TOURNAMENT_TEAM_ENTRY_AGREE_OK << rkTeamData.m_dwTeamIndex << rkUserData.m_dwTableIndex << rkUserData.m_dwUserIndex << rkUserData.m_szNick;
			kPacket << rkUserData.m_iGradeLevel << rkUserData.m_iLadderPoint << rkUserData.m_dwGuildIndex; 
			m_pUser->SendMessage( kPacket );
			break;
		}
	}
}

void ioUserTournament::AddTeamUserAgreeServerSync( ioUserTournament::TeamData &rkTeamData, ioUserTournament::TeamUserData &rkAddData )
{
	if( m_pUser == NULL ) return;

	// 방장은 이미 동기화 되었으므로 팀원들에게만 전송
	TeamUserVec::iterator iter = rkTeamData.m_TeamUserList.begin();
	for(;iter != rkTeamData.m_TeamUserList.end();++iter)
	{
		TeamUserData &rkUserData = *iter;
		if( rkUserData.m_dwUserIndex == m_pUser->GetUserIndex() ) continue;

		UserParent *pUserParent = g_UserNodeManager.GetGlobalUserNode( rkUserData.m_dwUserIndex );
		if( pUserParent )
		{
			if( pUserParent->IsUserOriginal() )
			{
				User *pUser = static_cast< User * >( pUserParent );
				ioUserTournament *pUserTournament = pUser->GetUserTournament();
				if( pUserTournament )
				{
					pUserTournament->AddTeamUserData( rkTeamData.m_dwTeamIndex, rkAddData );
				}
			}
			else
			{
				UserCopyNode *pUser = static_cast< UserCopyNode * >( pUserParent );
				SP2Packet kPacket( SSTPK_TOURNAMENT_TEAM_ENTRY_AGREE_OK );
				kPacket << pUser->GetUserIndex() << rkTeamData.m_dwTeamIndex << rkAddData.m_dwTableIndex << rkAddData.m_dwUserIndex << rkAddData.m_szNick;
				kPacket << rkAddData.m_iGradeLevel << rkAddData.m_iLadderPoint << rkAddData.m_dwGuildIndex; 
				pUser->SendMessage( kPacket );
			}

			LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "AddTeamUserAgreeServerSync :%d - %d", rkTeamData.m_dwTeamIndex, rkAddData.m_dwUserIndex );		
		}
	}
}

void ioUserTournament::DBtoCheerData( CQueryResultData *query_data )
{
	if( !m_pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s User NULL!!", __FUNCTION__ ); 
		return;
	}
	
	//우승응원 및 승부예측 정보
	CheerTeamInfoVec InfoVec;

	LOOP_GUARD();
	DWORD dwTableIndex = 0;
	while( query_data->IsExist() )
	{
		CheerTeamInfo TeamInfo;
		query_data->GetValue( TeamInfo.m_dwTourIndex, sizeof( DWORD ) );
		query_data->GetValue( TeamInfo.m_dwTeamIndex, sizeof( DWORD ) );
		query_data->GetValue( dwTableIndex, sizeof( DWORD ) );
		InfoVec.push_back( TeamInfo );
		
	}
	LOOP_GUARD_CLEAR();

	int iCreateSize = InfoVec.size();
	if( iCreateSize == 0 ) return;
	if( iCreateSize >= TOURNAMENT_CHEER_MAX_LOAD )
	{
		g_DBClient.OnSelectTournamentCheerList( m_pUser->GetUserDBAgentID(), m_pUser->GetAgentThreadID(), m_pUser->GetGUID(), m_pUser->GetUserIndex(), TOURNAMENT_CHEER_MAX_LOAD, dwTableIndex );
	}
	// 유저에게 전송
	SP2Packet kPacket( STPK_TOURNAMENT_CHEER_LIST );
	kPacket << iCreateSize;
	for( int i = 0; i < iCreateSize; ++i )
	{
		CheerTeamInfo TeamInfo = InfoVec[i];
		kPacket << TeamInfo.m_dwTourIndex << TeamInfo.m_dwTeamIndex;
		m_CheerList.push_back( TeamInfo );
	}
	m_pUser->SendMessage( kPacket );
	InfoVec.clear();

	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Send", __FUNCTION__ ); 
}

void ioUserTournament::TournamentEndDeleteCheerData( DWORD dwTourIndex )
{
	LOOP_GUARD();
	for( CheerTeamInfoVec::iterator iter = m_CheerList.begin(); iter != m_CheerList.end(); )
	{
		CheerTeamInfo rkInfo = *iter;
		if( rkInfo.m_dwTourIndex == dwTourIndex )
		{
			iter = m_CheerList.erase( iter );
		}
		else
		{
			++iter;
		}
	}
	LOOP_GUARD_CLEAR();
}

bool ioUserTournament::IsCheerTeam( DWORD dwTourIndex )
{
	if( m_CheerList.empty() )
		return false;
	
	for( CheerTeamInfoVec::iterator iter = m_CheerList.begin(); iter != m_CheerList.end(); ++iter )
	{
		CheerTeamInfo rkInfo = *iter;
		if( rkInfo.m_dwTourIndex == dwTourIndex )
		{
			return true;
		}
	}

	return false;
}

void ioUserTournament::SetRegularRewardData( DWORD dwTableIndex, DWORD dwStartDate, BYTE TourPos, int iMyCampPos, int iWinCampPos, int iLadderBonusPeso, int iLadderRank, int iLadderPoint )
{
	m_RegularRewardDBData.m_dwTableIndex	 = dwTableIndex;
	m_RegularRewardDBData.m_dwStartDate		 = dwStartDate;
	m_RegularRewardDBData.m_TourPos			 = TourPos;
	m_RegularRewardDBData.m_iMyCampPos		 = iMyCampPos;
	m_RegularRewardDBData.m_iWinCampPos		 = iWinCampPos;
	m_RegularRewardDBData.m_iLadderBonusPeso = iLadderBonusPeso;
	m_RegularRewardDBData.m_iLadderRank		 = iLadderRank;
	m_RegularRewardDBData.m_iLadderPoint	 = iLadderPoint;
}