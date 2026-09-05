#include "stdafx.h"

#include "MatchManager.h"
#include "RoomNodeManager.h"
#include "BattleRoomManager.h"
#include "SuccessionMode.h"
#include "../QueryData/QueryResultData.h"

MatchManager::MatchManager()
{
	LoadINI();

	m_vTopRankList.clear();
}

MatchManager::~MatchManager()
{
}

void MatchManager::LoadINI()
{
	ioINILoader kLoader;
	kLoader.ReloadFile( "config/sp2_succession.ini" );

	kLoader.SetTitle( "common" );

	m_iDefaultMMR = kLoader.LoadInt( "default_mmr", 1000 );
	m_iMinimumMMR = kLoader.LoadInt( "minimum_mmr", 0 );
	m_iLosePoint = kLoader.LoadInt( "lose_point", 5 );
	m_iWinPoint = kLoader.LoadInt( "win_point", 5 );
	m_bUseELO = kLoader.LoadBool( "use_ELO", false );
	m_bCheckMinimumMMR = kLoader.LoadBool( "check_minimum_mmr", false );
	
	m_iSuccessionRate = kLoader.LoadFloat( "succession_mmr_rate", 0 );

	kLoader.SetTitle( "peso_bonus" );
	m_vPesoBonus.clear();
	char szKey[MAX_PATH] = "";
	int iCnt = kLoader.LoadInt( "peso_bonus_count", 0 );
	for( int i=0; i<iCnt; ++i )
	{
		wsprintf( szKey, "peso_bonus_%d", i+1 );
		int iBonus = kLoader.LoadInt( szKey, 0 );
		m_vPesoBonus.push_back( iBonus );
	}

	kLoader.SetTitle( "open" );
	for( int i=0; i<7; ++i )
	{
		wsprintf( szKey, "week%d_open_time", i+1 );
		m_dwOpenTime[i] = kLoader.LoadInt( szKey, 1000 );

		wsprintf( szKey, "week%d_close_time", i+1 );
		m_dwCloseTime[i] = kLoader.LoadInt( szKey, 2300 );

		if( m_dwOpenTime[i] >= m_dwCloseTime[i] ||
			!COMPARE( m_dwOpenTime[i], 1000, 2401 ) ||
			!COMPARE( m_dwCloseTime[i], 1000, 2401 ))
		{
			m_dwOpenTime[i] = 0;
			m_dwCloseTime[i] = 0;
		}
	}
}

void MatchManager::SendUserInfo( DWORD dwBlueUserIndex, DWORD dwRedUserIndex, int iStartTier, int iEndTier )
{
	UserParent* pBlueUserParent = g_UserNodeManager.GetGlobalUserNode( dwBlueUserIndex );
	UserParent* pRedUserParent = g_UserNodeManager.GetGlobalUserNode( dwRedUserIndex );

	if( !pBlueUserParent || !pRedUserParent )
	{
		// 실패 패킷.
		return;
	}

	if( pBlueUserParent->IsUserOriginal() )
	{
		User *pBlueUser = static_cast<User*>( pBlueUserParent );
		pBlueUser->SetMatchTier(iStartTier, iEndTier);

		if( pRedUserParent->IsUserOriginal() )
		{
			// 블루의 정보를 레드에게 준다.
			User *pRedUser = static_cast<User*>( pRedUserParent );
			pRedUser->SetMatchTier(iStartTier, iEndTier);

			SP2Packet kPacket( STPK_SUCCESSION_OTHER_USER_INFO );
			PACKET_GUARD_VOID( kPacket.Write((int)TEAM_BLUE) );
			pBlueUser->FillSuccessionMatchInfo( kPacket );
			pRedUser->SendMessage( kPacket );
		}
		else
		{
			UserCopyNode *pRedUser = static_cast<UserCopyNode*>( pRedUserParent );

			// 블루의 정보를 레드에게 준다.
			SP2Packet kPacket( SSTPK_SUCCESSION_OTHER_USER_INFO );
			PACKET_GUARD_VOID( kPacket.Write((BYTE)SEND_TO_OTHER_USER) );
			PACKET_GUARD_VOID( kPacket.Write(dwRedUserIndex) );
			PACKET_GUARD_VOID( kPacket.Write((int)TEAM_BLUE) );
			PACKET_GUARD_VOID( kPacket.Write(iStartTier) );
			PACKET_GUARD_VOID( kPacket.Write(iEndTier) );
			pBlueUser->FillSuccessionMatchInfo( kPacket );
			pRedUser->SendMessage( kPacket );
		}
	}
	else
	{
		UserCopyNode *pBlueUser = static_cast<UserCopyNode*>( pBlueUserParent );

		SP2Packet kPacket( SSTPK_SUCCESSION_OTHER_USER_INFO );
		PACKET_GUARD_VOID( kPacket.Write((BYTE)SEND_TO_SELF) );
		PACKET_GUARD_VOID( kPacket.Write(dwBlueUserIndex) );
		PACKET_GUARD_VOID( kPacket.Write(dwRedUserIndex) );
		PACKET_GUARD_VOID( kPacket.Write((int)TEAM_BLUE) );
		PACKET_GUARD_VOID( kPacket.Write(iStartTier) );
		PACKET_GUARD_VOID( kPacket.Write(iEndTier) );
		pBlueUser->SendMessage( kPacket );
	}

	if( pRedUserParent->IsUserOriginal() )
	{
		User *pRedUser = static_cast<User*>( pRedUserParent );
		pRedUser->SetMatchTier(iStartTier, iEndTier);

		if( pBlueUserParent->IsUserOriginal() )
		{
			User *pBlueUser = static_cast<User*>( pBlueUserParent );
			pBlueUser->SetMatchTier(iStartTier, iEndTier);

			// 레드의 정보를 블루에게 준다.
			SP2Packet kPacket( STPK_SUCCESSION_OTHER_USER_INFO );
			PACKET_GUARD_VOID( kPacket.Write((int)TEAM_RED) );
			pRedUser->FillSuccessionMatchInfo( kPacket );
			pBlueUser->SendMessage( kPacket );
		}
		else
		{
			UserCopyNode *pBlueUser = static_cast<UserCopyNode*>( pBlueUserParent );

			// 레드의 정보를 블루에게 준다.
			SP2Packet kPacket( SSTPK_SUCCESSION_OTHER_USER_INFO );
			PACKET_GUARD_VOID( kPacket.Write((BYTE)SEND_TO_OTHER_USER) );
			PACKET_GUARD_VOID( kPacket.Write(dwBlueUserIndex) );
			PACKET_GUARD_VOID( kPacket.Write((int)TEAM_RED) );
			PACKET_GUARD_VOID( kPacket.Write(iStartTier) );
			PACKET_GUARD_VOID( kPacket.Write(iEndTier) );
			pRedUser->FillSuccessionMatchInfo( kPacket );
			pBlueUser->SendMessage( kPacket );
		}
	}
	else
	{
		UserCopyNode *pRedUser = static_cast<UserCopyNode*>( pRedUserParent );

		SP2Packet kPacket( SSTPK_SUCCESSION_OTHER_USER_INFO );
		PACKET_GUARD_VOID( kPacket.Write((BYTE)SEND_TO_SELF) );
		PACKET_GUARD_VOID( kPacket.Write(dwRedUserIndex) );
		PACKET_GUARD_VOID( kPacket.Write(dwBlueUserIndex) );
		PACKET_GUARD_VOID( kPacket.Write((int)TEAM_RED) );
		PACKET_GUARD_VOID( kPacket.Write(iStartTier) );
		PACKET_GUARD_VOID( kPacket.Write(iEndTier) );
		pRedUser->SendMessage( kPacket );
	}
}

bool MatchManager::MatchEnterRoom( DWORD dwBlueUserIndex, DWORD dwRedUserIndex, bool bRevenge )
{
	if( bRevenge )
	{
		LOG.PrintTimeAndLog( 0, "[info][1v1] match enter room - revenge." );
	}
	Room *pRoom = g_RoomNodeManager.CreateNewRoom();
	if( !pRoom )
		return false;

	int iMapNum = -1;
	int iSubType = -1;
	ModeType eModeType = MT_NONE;
	int iMapIndex  = 0;

	pRoom->SetRoomStyle( RSTYLE_MATCHROOM );
	pRoom->InitModeTypeList();

	pRoom->SelectNextMode( MT_SUCCESSION, -1, 1 );
	pRoom->SetModeType( MT_SUCCESSION, -1, -1 );

	SuccessionMode *pMode = static_cast<SuccessionMode*>( pRoom->GetModeInfo() );
	if( pMode )
	{
		pMode->SetTeam( dwBlueUserIndex, dwRedUserIndex );
		pMode->SetReserveRevengeMatch( bRevenge );
	}

	UserParent* pBlueUserParent = g_UserNodeManager.GetGlobalUserNode( dwBlueUserIndex );
	if( pBlueUserParent )
	{
		if( pBlueUserParent->IsUserOriginal() )
		{
			User *pUser = (User*)pBlueUserParent;

			SP2Packet kPacket( STPK_SUCCESSION_MATCHING_GAMESTART );
			PACKET_GUARD_BOOL( kPacket.Write(pRoom->GetModeType()) );
			PACKET_GUARD_BOOL( kPacket.Write(pRoom->GetModeSubNum()) );
			PACKET_GUARD_BOOL( kPacket.Write(pRoom->GetModeMapNum()) );
			PACKET_GUARD_BOOL( kPacket.Write(bRevenge) );
			pUser->SendMessage( kPacket );
			pUser->EnterRoom( pRoom );

			g_LogDBClient.OnInsertMatchInfo( pUser, pUser->GetStartTier(), pUser->GetEndTier(), pUser->GetUserMatch()->GetMatchDelayTime(), 
				pUser->GetUserMatch()->GetMMR(),
				pUser->GetUserMatch()->GetMaxWinCount(), LogDBClient::MT_SUCCESS, pRoom->GetRoomIndex(), (int)bRevenge );

			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"[info][1v1] match enter room[%d] blue user[%d] ", pRoom->GetRoomIndex(), pUser->GetUserIndex() );
		}
		else        
		{
			//타서버에 있는 유저들은 전부 서버 이동 시작
			UserCopyNode *pUser = (UserCopyNode*)pBlueUserParent;
			pRoom->EnterReserveUser( pUser->GetUserIndex() );
			SP2Packet kPacket( SSTPK_MOVING_ROOM_RESULT );
			PACKET_GUARD_BOOL( kPacket.Write(SS_MOVING_ROOM_JOIN) );
			PACKET_GUARD_BOOL( kPacket.Write(SS_MOVING_ROOM_JOIN_MATCH) );
			PACKET_GUARD_BOOL( kPacket.Write((int)pRoom->GetModeType()) );
			PACKET_GUARD_BOOL( kPacket.Write(pRoom->GetRoomIndex()) );
			PACKET_GUARD_BOOL( kPacket.Write(pUser->GetUserIndex()) );
			PACKET_GUARD_BOOL( kPacket.Write(pRoom->GetModeSubNum()) );
			PACKET_GUARD_BOOL( kPacket.Write(pRoom->GetModeMapNum()) );
			PACKET_GUARD_BOOL( kPacket.Write((int)pRoom->GetPlazaModeType()) );
			PACKET_GUARD_BOOL( kPacket.Write(pRoom->GetRoomNumber()) );
			PACKET_GUARD_BOOL( kPacket.Write(bRevenge) );

			pUser->SendMessage( kPacket );
		}
	}

	UserParent* pRedUserParent = g_UserNodeManager.GetGlobalUserNode( dwRedUserIndex );
	if( pRedUserParent )
	{
		if( pRedUserParent->IsUserOriginal() )
		{
			User *pUser = (User*)pRedUserParent;

			SP2Packet kPacket( STPK_SUCCESSION_MATCHING_GAMESTART );
			PACKET_GUARD_BOOL( kPacket.Write(pRoom->GetModeType()) );
			PACKET_GUARD_BOOL( kPacket.Write(pRoom->GetModeSubNum()) );
			PACKET_GUARD_BOOL( kPacket.Write(pRoom->GetModeMapNum()) );
			PACKET_GUARD_BOOL( kPacket.Write(bRevenge) );
			pUser->SendMessage( kPacket );

			pUser->EnterRoom( pRoom );

			g_LogDBClient.OnInsertMatchInfo( pUser, pUser->GetStartTier(), pUser->GetEndTier(), pUser->GetUserMatch()->GetMatchDelayTime(), 
				pUser->GetUserMatch()->GetMMR(),
				pUser->GetUserMatch()->GetMaxWinCount(), LogDBClient::MT_SUCCESS, pRoom->GetRoomIndex(), (int)bRevenge );

			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"[info][1v1] match enter room[%d] red user[%d] ", pRoom->GetRoomIndex(), pUser->GetUserIndex() );
		}
		else        
		{
			//타서버에 있는 유저들은 전부 서버 이동 시작
			UserCopyNode *pUser = (UserCopyNode*)pRedUserParent;
			pRoom->EnterReserveUser( pUser->GetUserIndex() );
			SP2Packet kPacket( SSTPK_MOVING_ROOM_RESULT );
			PACKET_GUARD_BOOL( kPacket.Write(SS_MOVING_ROOM_JOIN) );
			PACKET_GUARD_BOOL( kPacket.Write(SS_MOVING_ROOM_JOIN_MATCH) );
			PACKET_GUARD_BOOL( kPacket.Write((int)pRoom->GetModeType()) );
			PACKET_GUARD_BOOL( kPacket.Write(pRoom->GetRoomIndex()) );
			PACKET_GUARD_BOOL( kPacket.Write(pUser->GetUserIndex()) );
			PACKET_GUARD_BOOL( kPacket.Write(pRoom->GetModeSubNum()) );
			PACKET_GUARD_BOOL( kPacket.Write(pRoom->GetModeMapNum()) );
			PACKET_GUARD_BOOL( kPacket.Write((int)pRoom->GetPlazaModeType()) );
			PACKET_GUARD_BOOL( kPacket.Write(pRoom->GetRoomNumber()) );
			PACKET_GUARD_BOOL( kPacket.Write(bRevenge) );

			pUser->SendMessage( kPacket );
		}
	}

	return true;
}

void MatchManager::DBToData( CQueryResultData *query_data )
{
	m_vTopRankList.clear();

	while( query_data->IsExist() )
	{
		stTopRank st;
		char szNick[MAX_PATH]="";
		PACKET_GUARD_BREAK( query_data->GetValue( szNick, ID_NUM_PLUS_ONE ) );
		PACKET_GUARD_BREAK( query_data->GetValue( st.m_iUserIndex, sizeof(st.m_iUserIndex) ) );
		PACKET_GUARD_BREAK( query_data->GetValue( st.m_iRanking, sizeof(st.m_iRanking) ) );
		PACKET_GUARD_BREAK( query_data->GetValue( st.m_iMatchPoint, sizeof(st.m_iMatchPoint) ) );
		PACKET_GUARD_BREAK( query_data->GetValue( st.m_iUserLevel, sizeof(st.m_iUserLevel) ) );
		PACKET_GUARD_BREAK( query_data->GetValue( st.m_iWinCount, sizeof(st.m_iWinCount) ) );
		PACKET_GUARD_BREAK( query_data->GetValue( st.m_iLoseCount, sizeof(st.m_iLoseCount) ) );

		st.m_szNickname = szNick;
		m_vTopRankList.push_back( st );

		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"[info][1v1] db to topranklist user[%d] nick[%s] rank[%d] point[%d] level[%d] win[%d] lose[%d]"
			, st.m_iUserIndex, szNick, st.m_iRanking, st.m_iMatchPoint, st.m_iUserLevel, st.m_iWinCount, st.m_iLoseCount );
	}
}

int MatchManager::GetPesoBonus( int iWinStreakCount )
{
	if( m_vPesoBonus.empty() )
		return 0;

	if( iWinStreakCount == 0 )
		return 0;

	int iArray = iWinStreakCount-1;
	if( COMPARE( iArray, 0, (int)m_vPesoBonus.size() ) )
	{
		return m_vPesoBonus[iArray];
	}

	IntVec::reverse_iterator iter = m_vPesoBonus.rbegin();
	if( iter != m_vPesoBonus.rend() )
		return (*iter);

	return 0;
}

bool MatchManager::IsEnableMatch()
{
	CTime current_time = CTime::GetCurrentTime();
	int iWeek = current_time.GetDayOfWeek() - 1;

	if( !COMPARE( iWeek, 0, 7 ) )
		return true;

	if( m_dwOpenTime[iWeek] == 0 && m_dwCloseTime[iWeek] == 0 )
		return true;

	DWORD dwCurrentTime = 0;
	dwCurrentTime += current_time.GetHour() * 100;
	dwCurrentTime += current_time.GetMinute();

	return COMPARE( dwCurrentTime, m_dwOpenTime[iWeek], m_dwCloseTime[iWeek] );
}

DWORD MatchManager::GetOpenTime( int iDay )
{
	if( !COMPARE( iDay, 0, 7 ) )
		return 1000;

	return m_dwOpenTime[iDay];
}

DWORD MatchManager::GetCloseTime( int iDay )
{
	if( !COMPARE( iDay, 0, 7 ) )
		return 2300;

	return m_dwCloseTime[iDay];
}