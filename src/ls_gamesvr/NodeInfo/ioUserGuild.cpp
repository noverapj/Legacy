
#include "stdafx.h"
//#include "../Window.h"
#include "../MainProcess.h"
#include "../EtcHelpFunc.h"

#include "../QueryData/QueryResultData.h"
#include "../DataBase/DBClient.h"
#include "../DataBase/LogDBClient.h"
#include "../MainServerNode/MainServerNode.h"
#include "GuildRewardManager.h"

#include "UserNodeManager.h"
#include "ioUserGuild.h"
#include "../Local/ioLocalParent.h"
#include "../Local/ioLocalManager.h"


ioUserGuild::ioUserGuild()
{
	Initialize( NULL );
}

ioUserGuild::~ioUserGuild()
{
}

void ioUserGuild::Initialize( User *pUser )
{
	m_pUser = pUser;
	InitMyGuildData( false );
}

void ioUserGuild::InitMyGuildData( bool bSync )
{
	DWORD dwTempData			= m_dwGuildIndex;

	m_dwRecvAttendRewardDate	= 0;
	m_dwRecvRankRewardDate		= 0;
	m_dwGuildIndex				= 0;
	m_dwGuildMark				= 0;
	m_dwJoinDate				= 0;
	m_iGuildLevel				= GUILD_RANK_F;
	m_iYesterdayAttendance		= 0;
	m_iYesterdayDate			= 0;
	m_bTodayAttendance			= FALSE;
	m_bActiveGuildRoom			= FALSE;

	m_szGuildName.Clear();
	m_szGuildPosition.Clear();
	m_UserList.clear();

	if( bSync && m_pUser )
	{
		m_pUser->SyncUserGuild();
		m_pUser->LadderTeamMyInfo();					 //래더팀에 속해있으면 래더 길드팀 갱신
		m_pUser->LeaveGuildToLeaveGuildPlaza(dwTempData);          //길드광장에 속해있으면 길드광장에서 이탈한다.
		m_pUser->CheckRoomBonusTable();                  //길드 보너스를 다시 체크한다.
		m_pUser->LeaveGuildToQuestClear();               //길드 전용 퀘스트 삭제
	}
}

void ioUserGuild::LeaveGuildInitInfo( bool bSync )
{
	InitMyGuildData( bSync );
}

void ioUserGuild::SetGuildData( DWORD dwIndex, const ioHashString &szGuildName, const ioHashString &szGuildPosition, DWORD dwGuildMark, bool bSync )
{
	if( !m_pUser ) return;

	m_dwGuildIndex	  = dwIndex;
	m_szGuildName     = szGuildName;
	m_szGuildPosition = szGuildPosition;
	m_dwGuildMark     = dwGuildMark;

	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "GuildData :%s - %d - %s - %s - %d", m_pUser->GetPublicID().c_str(), dwIndex, szGuildName.c_str(), szGuildPosition.c_str(), m_dwGuildMark );

	if( bSync && m_pUser )
	{
		m_pUser->SyncUserGuild();
		m_pUser->LadderTeamMyInfo();          //래더팀에 속해있으면 래더 길드팀 갱신
		m_pUser->CheckRoomBonusTable();
	}
}

void ioUserGuild::SetGuildDataEntryAgree( DWORD dwIndex, const ioHashString &szGuildName, const ioHashString &szGuildPosition, DWORD dwGuildMark, bool bSync )
{
	if( !m_pUser ) return;
	
	SetGuildData( dwIndex, szGuildName, szGuildPosition, dwGuildMark, bSync );
}

void ioUserGuild::SendRelayPacketTcp( SP2Packet &rkPacket, BOOL bMe )
{
	if( !m_pUser ) return;

	vGuildUserData_iter iter, iEnd;
	iEnd = m_UserList.end();
	for(iter = m_UserList.begin();iter != iEnd;iter++)
	{
		GuildUserData &kUserData = *iter;
		if( !bMe )
		{
			if( kUserData.m_dwUserIndex == m_pUser->GetUserIndex() ) continue;
		}
		
		UserParent *pUserParent = g_UserNodeManager.GetGlobalUserNode( kUserData.m_dwUserIndex );
		if( pUserParent )
			pUserParent->RelayPacket( rkPacket );	
	}
}

void ioUserGuild::SetGuildUserData( CQueryResultData *query_data )
{
	if( !m_pUser ) return;

	LOOP_GUARD();
	static vGuildUserData vDBList;
	static vGuildUserData vNewUser;
	vDBList.clear();
	vNewUser.clear();

	while( query_data->IsExist() )
	{
		char szStringVal[MAX_PATH] = "";
		GuildUserData kUserData;
		PACKET_GUARD_BREAK( query_data->GetValue( kUserData.m_dwTableIndex, sizeof(DWORD) ) );	//테이블 인덱스
		PACKET_GUARD_BREAK( query_data->GetValue( kUserData.m_dwUserIndex, sizeof(DWORD) ) );		//유저 인덱스
		PACKET_GUARD_BREAK( query_data->GetValue( kUserData.m_iUserLevel, sizeof(int) ) );		//유저 레벨
		PACKET_GUARD_BREAK( query_data->GetValue( szStringVal, ID_NUM_PLUS_ONE ) );				//유저 닉네임
		kUserData.m_szUserID = szStringVal;
		memset( szStringVal, 0, MAX_PATH );
		PACKET_GUARD_BREAK( query_data->GetValue( szStringVal, GUILD_POS_NUM_PLUS_ONE ) );       //유저 직책
		kUserData.m_szGuildPosition = szStringVal;
		PACKET_GUARD_BREAK( query_data->GetValue( kUserData.m_iLadderPoint, sizeof(int) ) );		//유저 진영 포인트

		vDBList.push_back( kUserData );                                     
		if( !IsGuildUser( kUserData.m_dwUserIndex, kUserData.m_szUserID ) )
			vNewUser.push_back( kUserData );                  
	}
	LOOP_GUARD_CLEAR();

	// 탈퇴한 유저
	CheckGuildLeaveUser( vDBList );
	vDBList.clear();

	if( !vNewUser.empty() )
	{
		// 유저에게 리스트 전송.
		SP2Packet kPacket( STPK_MYGUILD_MEMBER_LIST );
 		PACKET_GUARD_VOID( kPacket.Write(GetGuildIndex()) );
 		PACKET_GUARD_VOID( kPacket.Write( (int)vNewUser.size()) );

		 //클라이언트의 길드원 정보를 전부 클리어하고 추가한다.
		if( m_UserList.empty() )
		{
			PACKET_GUARD_VOID( kPacket.Write(true) );
		}
		else
		{
			PACKET_GUARD_VOID( kPacket.Write(false) );
		}

		for(int i = 0;i < (int)vNewUser.size();i++)
		{
			GuildUserData &kUserData = vNewUser[i];
			m_UserList.push_back( kUserData );					  // 새로 가입한 유저 추가

			PACKET_GUARD_VOID( kPacket.Write(kUserData.m_dwTableIndex) );
			PACKET_GUARD_VOID( kPacket.Write(kUserData.m_dwUserIndex) );
			PACKET_GUARD_VOID( kPacket.Write(kUserData.m_szUserID) );
			PACKET_GUARD_VOID( kPacket.Write(kUserData.m_szGuildPosition) );
			
			if( m_pUser->GetUserIndex() == kUserData.m_dwUserIndex )
			{
				kUserData.m_iUserLevel	= m_pUser->GetGradeLevel();
				kUserData.m_iLadderPoint= m_pUser->GetLadderPoint();
				PACKET_GUARD_VOID( kPacket.Write(true) );
				PACKET_GUARD_VOID( kPacket.Write(m_pUser->GetUserPos()) );
				PACKET_GUARD_VOID( kPacket.Write(kUserData.m_iUserLevel) );
				PACKET_GUARD_VOID( kPacket.Write(kUserData.m_iLadderPoint) );
			}
			else
			{
				UserParent *pUserParent = g_UserNodeManager.GetGlobalUserNode( kUserData.m_dwUserIndex );
				if( pUserParent )
				{
					kUserData.m_iUserLevel	= pUserParent->GetGradeLevel();
					kUserData.m_iLadderPoint= pUserParent->GetLadderPoint();

					PACKET_GUARD_VOID( kPacket.Write(true) );
					PACKET_GUARD_VOID( kPacket.Write(pUserParent->GetUserPos()) );
					PACKET_GUARD_VOID( kPacket.Write(kUserData.m_iUserLevel) );
					PACKET_GUARD_VOID( kPacket.Write(kUserData.m_iLadderPoint) );
					
					{   //같은 길드원에게 로그인 알림
						SP2Packet kLoginPacket( STPK_USER_LOGIN );
						PACKET_GUARD_VOID( kLoginPacket.Write(1) );
						PACKET_GUARD_VOID( kLoginPacket.Write( m_pUser->GetPublicID()) );
						PACKET_GUARD_VOID( kLoginPacket.Write(true) );
						PACKET_GUARD_VOID( kLoginPacket.Write(pUserParent->GetGradeLevel()) );
						PACKET_GUARD_VOID( kLoginPacket.Write(GetGuildIndex()) );
						PACKET_GUARD_VOID( kLoginPacket.Write(GetGuildMark()) );
						PACKET_GUARD_VOID( kLoginPacket.Write(m_pUser->IsBestFriend( pUserParent->GetUserIndex())) );

						pUserParent->RelayPacket( kLoginPacket );
					}
				}
				else
				{
					PACKET_GUARD_VOID( kPacket.Write(false) );
					PACKET_GUARD_VOID( kPacket.Write(UP_LOBBY) );
					PACKET_GUARD_VOID( kPacket.Write(kUserData.m_iUserLevel) );
					PACKET_GUARD_VOID( kPacket.Write(kUserData.m_iLadderPoint) );
				}			
			}			
		}
		m_pUser->SendMessage( kPacket );

		//어제, 오늘 출석 정보 GET
		SYSTEMTIME sYesterDay = {0,0,0,0,0,0,0,0};
		SYSTEMTIME sToday = {0,0,0,0,0,0,0,0};

		m_pUser->GetAttendStandardDate(sYesterDay, sToday);
		g_DBClient.OnSelectGuildAttendanceInfo(m_pUser->GetUserDBAgentID(), m_pUser->GetAgentThreadID(), GetGuildIndex(), m_pUser->GetUserIndex(), sYesterDay, YESTERDAY_ATTENDANCE_MEMEBER);
		g_DBClient.OnSelectGuildAttendanceInfo(m_pUser->GetUserDBAgentID(), m_pUser->GetAgentThreadID(), GetGuildIndex(), m_pUser->GetUserIndex(), sToday, TODAY_ATTENDACE_MEMBER);		
	}
}

void ioUserGuild::SetGuildAttendanceData( CQueryResultData *query_data )
{
	if( !m_pUser ) return;

	int iSelectType		= 0;
	DWORD dwSelectDate	= 0;
	static DWORDVec vMyGuildAttendanceInfo;
	vMyGuildAttendanceInfo.clear();

	PACKET_GUARD_VOID( query_data->GetValue( iSelectType, sizeof(int) ) );
	PACKET_GUARD_VOID( query_data->GetValue( dwSelectDate, sizeof(DWORD) ) );

	//유저 길드 출석 정보 Get  어제자 인원수 저장, 시간, 나 보상 받아야하는지 여부 체크
	DWORD dwUserIndex	= 0;

	if( TODAY_ATTENDACE_MEMBER == iSelectType )
		m_bTodayAttendance	= FALSE;

	while( query_data->IsExist() )
	{
		PACKET_GUARD_VOID( query_data->GetValue( dwUserIndex, sizeof(DWORD) ) );
		vMyGuildAttendanceInfo.push_back(dwUserIndex);
		if( m_pUser->GetUserIndex() == dwUserIndex && TODAY_ATTENDACE_MEMBER == iSelectType )
			m_bTodayAttendance	= TRUE;
	}

	if( YESTERDAY_ATTENDANCE_MEMEBER == iSelectType )
	{
		m_iYesterdayAttendance		= 0;
		m_iYesterdayDate			= 0;

		m_iYesterdayDate		= dwSelectDate;
		m_iYesterdayAttendance	= vMyGuildAttendanceInfo.size();
	}

	////////////////////////정보 SEND
	int iCount = vMyGuildAttendanceInfo.size();

	SP2Packet kPacket(STPK_GUILD_MEMBER_ATTEND_INFO);
	PACKET_GUARD_VOID( kPacket.Write(iSelectType) );
	PACKET_GUARD_VOID( kPacket.Write(iCount) );
	for( int i = 0; i < iCount; i++ )
	{
		PACKET_GUARD_VOID( kPacket.Write(vMyGuildAttendanceInfo[i]) );
	}

	m_pUser->SendMessage(kPacket);
}

void ioUserGuild::CheckGuildLeaveUser( ioUserGuild::vGuildUserData &rkNewUserList )
{
	if( !m_pUser ) return;

	vGuildUserData vLeaveUser;
	vGuildUserData_iter iter, iEnd;
	iEnd = m_UserList.end();
	for(iter = m_UserList.begin();iter != iEnd;iter++)
	{
		GuildUserData &kUserData = *iter;

		int i = 0;
		int iNewSize = rkNewUserList.size();
		for(;i < iNewSize;i++)
		{
			if( rkNewUserList[i].m_dwUserIndex == kUserData.m_dwUserIndex )
				break;
		}
		if( i >= iNewSize )
			vLeaveUser.push_back( kUserData );
	}

	if( vLeaveUser.empty() ) return;

    //유저에게 탈퇴 유저 리스트 전송.
	SP2Packet kPacket( STPK_MYGUILD_LEAVE_LIST );
	kPacket << GetGuildIndex() << (int)vLeaveUser.size();
	iEnd = vLeaveUser.end();
	for(iter = vLeaveUser.begin();iter != iEnd;iter++)
	{
		GuildUserData &kUserData = *iter;
		DeleteGuildUser( kUserData.m_dwUserIndex );  //탈퇴한 유저가있으면 리스트에서 제거
		kPacket << kUserData.m_szUserID;		
	}
	m_pUser->SendMessage( kPacket );
	vLeaveUser.clear();
}

void ioUserGuild::DeleteGuildUser( const ioHashString &szUserID )
{
	CRASH_GUARD();
	vGuildUserData_iter iter, iEnd;
	iEnd = m_UserList.end();
	for(iter = m_UserList.begin();iter != iEnd;iter++)
	{
		GuildUserData &kUserData = *iter;
		if( kUserData.m_szUserID == szUserID )
		{
			m_UserList.erase( iter );
			return;
		}
	}
}

void ioUserGuild::DeleteGuildUser( const DWORD &dwUserIndex )
{
	vGuildUserData_iter iter, iEnd;
	iEnd = m_UserList.end();
	for(iter = m_UserList.begin();iter != iEnd;iter++)
	{
		GuildUserData &kUserData = *iter;
		if( kUserData.m_dwUserIndex == dwUserIndex )
		{
			m_UserList.erase( iter );
			return;
		}
	}
}

bool ioUserGuild::IsGuildUser( const ioHashString &szUserID )
{
	CRASH_GUARD();
	vGuildUserData_iter iter, iEnd;
	iEnd = m_UserList.end();
	for(iter = m_UserList.begin();iter != iEnd;iter++)
	{
		GuildUserData &kUserData = *iter;
		if( kUserData.m_szUserID == szUserID )
			return true;
	}
	return false;
}

bool ioUserGuild::IsGuildUser( const DWORD &dwUserIndex, const ioHashString &szUserID )
{
	vGuildUserData_iter iter, iEnd;
	iEnd = m_UserList.end();
	for(iter = m_UserList.begin();iter != iEnd;iter++)
	{
		GuildUserData &kUserData = *iter;
		if( kUserData.m_dwUserIndex == dwUserIndex )
		{
			// 유저 아이디가 변경된거라면 바꾼다
			kUserData.m_szUserID = szUserID;
			return true;
		}
	}
	return false;
}

void ioUserGuild::LeaveGuildUserSync()
{
	if( !m_pUser ) return;

	vGuildUserData_iter iter, iEnd;
	iEnd = m_UserList.end();
	for(iter = m_UserList.begin();iter != iEnd;iter++)
	{
		GuildUserData &kUserData = *iter;
		if( kUserData.m_dwUserIndex == m_pUser->GetUserIndex() ) continue;

		UserParent *pUserParent = g_UserNodeManager.GetGlobalUserNode( kUserData.m_dwUserIndex );
		if( pUserParent )
		{
			if( pUserParent->IsUserOriginal() )
			{
				User *pUserOriginal = (User*)pUserParent;				
				ioUserGuild *pUserGuild = pUserOriginal->GetUserGuild();
				if( pUserGuild )
				{
					if( pUserGuild->GetGuildIndex() == GetGuildIndex() )
					{
						pUserGuild->DeleteGuildUser( m_pUser->GetPublicID() );

						// 유저에게 전송
						SP2Packet kPacket( STPK_GUILD_LEAVE );
						kPacket << GUILD_LEAVE_OK << GetGuildIndex() << m_pUser->GetPublicID();
						pUserOriginal->SendMessage( kPacket );
					}
				}
			}
			else
			{
				UserCopyNode *pUserCopy = (UserCopyNode*)pUserParent;
				SP2Packet kPacket( SSTPK_GUILD_USER_DELETE );
				kPacket << pUserCopy->GetUserIndex() << m_pUser->GetPublicID() << GetGuildIndex();
				kPacket << STPK_GUILD_LEAVE << GUILD_LEAVE_OK;
				pUserCopy->SendMessage( kPacket );
			}
		}
	}
}

void ioUserGuild::KickOutGuildUserSync( const ioHashString &szUserID )
{
	if( !m_pUser ) return;

	vGuildUserData_iter iter, iEnd;
	iEnd = m_UserList.end();
	for(iter = m_UserList.begin();iter != iEnd;iter++)
	{
		GuildUserData &kUserData = *iter;
		if( kUserData.m_dwUserIndex == m_pUser->GetUserIndex() ) continue;

		UserParent *pUserParent = g_UserNodeManager.GetGlobalUserNode( kUserData.m_dwUserIndex );
		if( pUserParent )
		{
			if( pUserParent->IsUserOriginal() )
			{
				User *pUserOriginal = (User*)pUserParent;				
				ioUserGuild *pUserGuild = pUserOriginal->GetUserGuild();
				if( pUserGuild )
				{
					if( pUserGuild->GetGuildIndex() == GetGuildIndex() )
					{
						pUserGuild->DeleteGuildUser( m_pUser->GetPublicID() );

						// 유저에게 전송
						SP2Packet kPacket( STPK_GUILD_KICK_OUT );
						kPacket << GUILD_KICK_OUT_OK << GetGuildIndex() << szUserID << false;
						pUserOriginal->SendMessage( kPacket );
					}
				}
			}
			else
			{
				UserCopyNode *pUserCopy = (UserCopyNode*)pUserParent;
				SP2Packet kPacket( SSTPK_GUILD_USER_DELETE );
				kPacket << pUserCopy->GetUserIndex() << szUserID << GetGuildIndex();
				kPacket << STPK_GUILD_KICK_OUT << GUILD_KICK_OUT_OK;
				pUserCopy->SendMessage( kPacket );
			}
		}
	}
}

void ioUserGuild::GuildNameChangeSync()
{
	if( !m_pUser ) return;

	vGuildUserData_iter iter, iEnd;
	iEnd = m_UserList.end();
	for(iter = m_UserList.begin();iter != iEnd;iter++)
	{
		GuildUserData &kUserData = *iter;
		if( kUserData.m_dwUserIndex == m_pUser->GetUserIndex() ) continue;

		UserParent *pUserParent = g_UserNodeManager.GetGlobalUserNode( kUserData.m_dwUserIndex );
		if( pUserParent )
		{
			if( pUserParent->IsUserOriginal() )
			{
				User *pUserOriginal = (User*)pUserParent;				
				ioUserGuild *pUserGuild = pUserOriginal->GetUserGuild();
				if( pUserGuild )
				{
					if( pUserGuild->GetGuildIndex() == GetGuildIndex() )
					{
						pUserGuild->SetGuildName( GetGuildName() );

						// 유저에게 전송
						SP2Packet kUserPacket( STPK_GUILD_NAME_CHANGE );
						kUserPacket << GUILD_NAME_CHANGE_OK << m_pUser->GetUserIndex() << GetGuildIndex() << GetGuildName();
						pUserOriginal->SendMessage( kUserPacket );
					}
				}
			}
			else
			{
				// 타 서버 유저에게 전송
				UserCopyNode *pUserCopy = (UserCopyNode*)pUserParent;
				SP2Packet kPacket( SSTPK_GUILD_NAME_CHANGE );
				kPacket << pUserCopy->GetUserIndex() << m_pUser->GetUserIndex() << GetGuildIndex() << GetGuildName();
				pUserCopy->SendMessage( kPacket );
			}
		}
	}
}

void ioUserGuild::GuildMarkChangeSync( bool bBlock )
{
	if( !m_pUser ) return;

	vGuildUserData_iter iter, iEnd;
	iEnd = m_UserList.end();
	for(iter = m_UserList.begin();iter != iEnd;iter++)
	{
		GuildUserData &kUserData = *iter;
		if( kUserData.m_dwUserIndex == m_pUser->GetUserIndex() ) continue;

		UserParent *pUserParent = g_UserNodeManager.GetGlobalUserNode( kUserData.m_dwUserIndex );
		if( pUserParent )
		{
			if( pUserParent->IsUserOriginal() )
			{
				User *pUser = (User*)pUserParent;
				pUser->_OnGuildMarkChange( GetGuildIndex(), GetGuildMark(), bBlock );
			}
			else
			{
				UserCopyNode *pUser = (UserCopyNode*)pUserParent;
				SP2Packet kServerPacket( SSTPK_GUILD_MARK_CHANGE );
				kServerPacket << pUser->GetUserIndex() << GetGuildIndex() << GetGuildMark() << bBlock;
				pUser->SendMessage( kServerPacket );
			}
		}
	}			
}

void ioUserGuild::GuildEntryAgreeSync()
{
	if( !m_pUser ) return;

	vGuildUserData_iter iter, iEnd;
	iEnd = m_UserList.end();
	for(iter = m_UserList.begin();iter != iEnd;iter++)
	{
		GuildUserData &kUserData = *iter;
		UserParent *pUserParent = g_UserNodeManager.GetGlobalUserNode( kUserData.m_dwUserIndex );
		if( pUserParent )
		{
			if( pUserParent->IsUserOriginal() )
				g_DBClient.OnSelectGuildMemberListEx( pUserParent->GetUserDBAgentID(), pUserParent->GetAgentThreadID(), pUserParent->GetUserIndex(), GetGuildIndex(), 0, IsGuildMaster() );
			else
			{
				UserCopyNode *pUser = (UserCopyNode*)pUserParent;
				SP2Packet kPacket( SSTPK_GUILD_MEMBER_LIST_EX );
				PACKET_GUARD_VOID(kPacket.Write(pUser->GetUserIndex()));
				PACKET_GUARD_VOID(kPacket.Write(SSTPK_GUILD_MEMBER_LIST_ME));
				pUser->SendMessage( kPacket );
			}
		}
	}			
}

void ioUserGuild::FillMoveData( SP2Packet &rkPacket )
{
	PACKET_GUARD_VOID( rkPacket.Write(m_dwGuildIndex) );
	PACKET_GUARD_VOID( rkPacket.Write(m_szGuildName) );
	PACKET_GUARD_VOID( rkPacket.Write(m_szGuildPosition) );
	PACKET_GUARD_VOID( rkPacket.Write(m_dwGuildMark) );
	PACKET_GUARD_VOID( rkPacket.Write(m_iGuildLevel) );
	PACKET_GUARD_VOID( rkPacket.Write(m_dwJoinDate) );
	PACKET_GUARD_VOID( rkPacket.Write(m_dwRecvAttendRewardDate) );
	PACKET_GUARD_VOID( rkPacket.Write(m_dwRecvRankRewardDate) );
	PACKET_GUARD_VOID( rkPacket.Write(m_iYesterdayAttendance) );
	PACKET_GUARD_VOID( rkPacket.Write(m_iYesterdayDate) );
	PACKET_GUARD_VOID( rkPacket.Write(m_bTodayAttendance) );
	PACKET_GUARD_VOID( rkPacket.Write(m_bActiveGuildRoom) );

	int iUserSize = m_UserList.size();
	PACKET_GUARD_VOID( rkPacket.Write(iUserSize) );

	for(int i = 0;i < iUserSize;i++)
	{
		GuildUserData &kUserData = m_UserList[i];
		PACKET_GUARD_VOID( rkPacket.Write(kUserData.m_dwTableIndex));
		PACKET_GUARD_VOID( rkPacket.Write(kUserData.m_dwUserIndex));
		PACKET_GUARD_VOID( rkPacket.Write(kUserData.m_iUserLevel));
		PACKET_GUARD_VOID( rkPacket.Write(kUserData.m_szUserID));
		PACKET_GUARD_VOID( rkPacket.Write(kUserData.m_szGuildPosition));
		PACKET_GUARD_VOID( rkPacket.Write(kUserData.m_iLadderPoint));
	}
}

void ioUserGuild::ApplyMoveData( SP2Packet &rkPacket, bool bDummyNode )
{
	PACKET_GUARD_VOID( rkPacket.Read(m_dwGuildIndex) );
	PACKET_GUARD_VOID( rkPacket.Read(m_szGuildName) );
	PACKET_GUARD_VOID( rkPacket.Read(m_szGuildPosition) );
	PACKET_GUARD_VOID( rkPacket.Read(m_dwGuildMark) );
	PACKET_GUARD_VOID( rkPacket.Read(m_iGuildLevel) );
	PACKET_GUARD_VOID( rkPacket.Read(m_dwJoinDate) );
	PACKET_GUARD_VOID( rkPacket.Read(m_dwRecvAttendRewardDate) );
	PACKET_GUARD_VOID( rkPacket.Read(m_dwRecvRankRewardDate) );
	PACKET_GUARD_VOID( rkPacket.Read(m_iYesterdayAttendance) );
	PACKET_GUARD_VOID( rkPacket.Read(m_iYesterdayDate) );
	PACKET_GUARD_VOID( rkPacket.Read(m_bTodayAttendance) );
	PACKET_GUARD_VOID( rkPacket.Read(m_bActiveGuildRoom) );

	int iUserSize	= 0;
	rkPacket >> iUserSize;
	for(int i = 0;i < iUserSize;i++)
	{
		GuildUserData kUserData;
		rkPacket >> kUserData.m_dwTableIndex >> kUserData.m_dwUserIndex >> kUserData.m_iUserLevel
			     >> kUserData.m_szUserID >> kUserData.m_szGuildPosition >> kUserData.m_iLadderPoint;
		m_UserList.push_back( kUserData );
	}
}

bool ioUserGuild::IsGuildMaster()
{
	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	if( pLocal )
	{
		return ( m_szGuildPosition == pLocal->GetGuildMasterPostion() );
	}
	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "bool ioUserGuild::IsGuildMaster()!! : myPosition : %s, master : %s", m_szGuildPosition.c_str(), pLocal->GetGuildMasterPostion() );
	return false;
}

bool ioUserGuild::IsGuildSecondMaster()
{
	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	if( pLocal )
	{
		return ( m_szGuildPosition == pLocal->GetGuildSecondMasterPosition() );
	}

	return false;
}


void ioUserGuild::SetRecvRewardDate( DBTIMESTAMP& AttendRewardRecvDate, DBTIMESTAMP& RankRewardRecvDate )
{
	m_dwRecvAttendRewardDate	= g_GuildRewardMgr.RecvDateConvertDBTIMESTAMPToDWORD(AttendRewardRecvDate);
	m_dwRecvRankRewardDate		= g_GuildRewardMgr.RecvDateConvertDBTIMESTAMPToDWORD(RankRewardRecvDate);
}

void ioUserGuild::SetJoinDate(DBTIMESTAMP& JoinDate)
{
	if( !Help::IsAvailableDate(JoinDate.year - 2000, JoinDate.month, JoinDate.day) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[warning][guildreward]Guild join date is wrong : [%d] [%d] [%d]", JoinDate.year, JoinDate.month, JoinDate.day );
		return;
	}

	CTime cJoinDate(JoinDate.year, JoinDate.month, JoinDate.day, JoinDate.hour, JoinDate.minute, 0);

	m_dwJoinDate = cJoinDate.GetTime();
}

int ioUserGuild::CanAttending( SP2Packet &rkPacket )
{
	if( !m_pUser )
		return GUILD_ATTEND_FAIL;

	if( 0 == m_dwJoinDate )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[warning][guildreward]Guild join date is none : [%d] [%d]", m_pUser->GetUserIndex(), GetGuildIndex() );
		return GUILD_ATTEND_FAIL;
	}
	
	if( IsPrevAttendanceInfo() )
		return GUILD_ATTEND_DATE_PREV;
	
	if( m_bTodayAttendance )
		return GUILD_ATTEND_FAIL;

	//가입 기간이 하루 지났는지 체크
	int iStayDay = 0;

	CTime cCurTime = CTime::GetCurrentTime();
	CTime cJoinDate(m_dwJoinDate);

	CTimeSpan cGap = cCurTime - cJoinDate;
	int iRenewalHour = g_GuildRewardMgr.GetRenewalHour();
	iStayDay = cGap.GetDays();

	if( 0 == iStayDay ) 
	{
		if( cCurTime.GetHour() < iRenewalHour )
			return GUILD_STAY_DAY_SCARCITY;

		if( cJoinDate.GetDay() == cCurTime.GetDay() )
		{
			if( cJoinDate.GetHour() >= iRenewalHour )
				return GUILD_STAY_DAY_SCARCITY;
		}
	}

	return GUILD_ATTEND_OK;
}

void ioUserGuild::DoRecvGuildRankReward()
{
	if( !m_pUser )
		return;

	//길드에 가입하지 않음
	if( 0 == m_dwGuildIndex )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[warning][guildreward]guild index 0" );
		return;
	}

	DWORD dwActiveCampDate = g_GuildRewardMgr.GetActiveCampDate();

	if( 0 == dwActiveCampDate )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[warning][guildreward]Active camp date is invalid : [%d]", dwActiveCampDate );
		return;
	}

	//가입날이 진행중인 진영전 보다 전인지 체크
	if( m_dwJoinDate >= dwActiveCampDate )
		return;

	//보상을 받은 유저인지 체크
	if( dwActiveCampDate <= m_dwRecvRankRewardDate )
		return;

	//보상 받을 수 있는 길드 랭크 인지 확인
	if( !g_GuildRewardMgr.IsRecvGuildRank(GetGuildLevel()) )
		return;

	int iGuildLevel = GetGuildLevel();
	if( g_GuildRewardMgr.SendRankReward(m_pUser, iGuildLevel) )
	{
		//DB 보상시간 update
		SQLUpdateRankRewardDate();

		CTime cCurTime = CTime::GetCurrentTime();
		CTime cRecvTime( cCurTime.GetYear(), cCurTime.GetMonth(), cCurTime.GetDay(), cCurTime.GetHour(), cCurTime.GetMinute(), 0 );
		SetRecvRankRewardDate(cRecvTime.GetTime());
	}
}

DWORD ioUserGuild::GetStandardDate(DWORD dwDate)
{
	CTime cDate(dwDate);
	int iRenewalHour = g_GuildRewardMgr.GetRenewalHour();

	CTime cStandardDate(cDate.GetYear(), cDate.GetMonth(), cDate.GetDay(), iRenewalHour, 0, 0);
	CTimeSpan cGap(1,0,0,0);

	if( cDate.GetHour() < iRenewalHour )
		cStandardDate = cStandardDate - cGap;

	return (DWORD)cStandardDate.GetTime();
}

BOOL ioUserGuild::IsRecvDate(DWORD dwPrevRecvDate)
{
	CTime cCurTime = CTime::GetCurrentTime();
	
	if( 0 == dwPrevRecvDate )
		return TRUE;

	DWORD dwTodayStandardDate = GetStandardDate(cCurTime.GetTime());
	DWORD dwPrevStandardDate = GetStandardDate(dwPrevRecvDate);

	if( dwTodayStandardDate > dwPrevStandardDate )
		return TRUE;

	return FALSE;
}

BOOL ioUserGuild::DidTakeAction(DWORD dwActionDate)
{
	CTime cCurTime = CTime::GetCurrentTime();
	int iRenewalHour = g_GuildRewardMgr.GetRenewalHour();

	if( 0 == dwActionDate )
		return TRUE;

	CTime cCheckTime(cCurTime.GetYear(), cCurTime.GetMonth(), cCurTime.GetDay(), iRenewalHour, 0, 0);
	CTime cAgreeDate( dwActionDate );

	if( cAgreeDate >= cCheckTime )
		return FALSE;

	CTimeSpan cGap = cCheckTime - cAgreeDate;
	if( cGap.GetDays() >= 2 )
		return TRUE;
	else if( cGap.GetDays() >= 1 )
	{
		if( cCurTime.GetHour() >= iRenewalHour )
			return TRUE;
	}

	if( cCurTime.GetHour() < iRenewalHour )
		return FALSE;

	return TRUE;
}

int ioUserGuild::DoRecvGuildAttendReward()
{
	if( !m_pUser )
		return GUILD_ATTEND_REWARD_FAIL;
	
	// 로그인 후 다음날 갱신 시간으로 바꼈는지 검사. ( 다음날로 변경 시 전날 출석 인원 수도 바꿔 줘야함 )
	if( m_iYesterdayDate == 0 )
	{
		//로그인 시에 길드 정보가 있으면 무조건 호출.
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[warning][guildreward]Prev attendance date is wrong : [%d] [%d]", m_pUser->GetUserIndex(), GetGuildIndex() );
		return GUILD_ATTEND_REWARD_FAIL;
	}

	//날이 바뀌면서 출석정보가 갱신 안되었는지 체크
	if( IsPrevAttendanceInfo() )
		return GUILD_ATTEND_DATE_PREV;

	// 가입 일로 하루 지났는지 확인.
	if( 0 == m_dwJoinDate )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[warning][guildreward]Guild join date is none : [%d] [%d]", m_pUser->GetUserIndex(), GetGuildIndex() );
		return GUILD_ATTEND_REWARD_FAIL;
	}

	if( 0 == m_iYesterdayAttendance )
		return GUILD_ATTEND_REWARD_FAIL;

	//가입 기간이 하루 지났는지 체크
	int iStayDay = 0;

	CTime cCurTime = CTime::GetCurrentTime();
	CTime cJoinDate(m_dwJoinDate);

	CTimeSpan cGap = cCurTime - cJoinDate;
	int iRenewalHour = g_GuildRewardMgr.GetRenewalHour();
	iStayDay = cGap.GetDays();

	if( 0 == iStayDay )
	{
		if( cCurTime.GetHour() < iRenewalHour )
			return GUILD_ATTEND_REWARD_FAIL;

		if( cJoinDate.GetDay() == cCurTime.GetDay() )
		{
			if( cJoinDate.GetHour() > iRenewalHour )
				return GUILD_ATTEND_REWARD_FAIL;
		}
	}
	
	if( !IsRecvDate(GetRecvAttendRewardDate()) )
		return GUILD_ATTEND_REWARD_FAIL;

	return GUILD_ATTEND_REWARD_OK;
}

BOOL ioUserGuild::IsPrevAttendanceInfo()
{
	// 로그인 후 다음날 갱신 시간으로 바꼈는지 검사. ( 다음날로 변경 시 전날 출석 인원 수도 바꿔 줘야함 )
	if( m_iYesterdayDate == 0 )
	{
		//로그인 시에 길드 정보가 있으면 무조건 호출.
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[warning][guildreward]Prev attendance date is wrong : [%d] [%d]", m_pUser->GetUserIndex(), GetGuildIndex() );
		return TRUE;
	}

	CTime cCurTime = CTime::GetCurrentTime();
	CTime cYesteradayAttendDate(m_iYesterdayDate);
	CTimeSpan cGap = cCurTime - cYesteradayAttendDate;
	if( cGap.GetDays() <= 1 )
		return FALSE;

	return TRUE;
}

void ioUserGuild::SQLUpdateAttendDate()
{
	if( !m_pUser )
		return;

	g_DBClient.OnInsertUserGuildAttendanceInfo(m_pUser->GetUserDBAgentID(), m_pUser->GetAgentThreadID(), GetGuildIndex(), m_pUser->GetUserIndex());
	g_LogDBClient.OnInsertGameLogInfo(LogDBClient::GLT_GUILD_ATTENDANCE, m_pUser, 0, GetGuildIndex(), 0, 0, 0, 0, 0, NULL);
}

void ioUserGuild::SQLUpdateAttendRewardDate()
{
	if( !m_pUser )
		return;

	g_DBClient.OnUpdateGuildAttendanceRewardDate(m_pUser->GetUserDBAgentID(), m_pUser->GetAgentThreadID(), m_pUser->GetUserIndex());
	
	RewardInfo stRewardInfo;
	g_GuildRewardMgr.GetAttendReward(m_iYesterdayAttendance, stRewardInfo);
	g_LogDBClient.OnInsertGameLogInfo(LogDBClient::GLT_GUILD_ATTENDANCE_REWARD, m_pUser, 0, GetGuildIndex(), 0, stRewardInfo.iType, stRewardInfo.iValue1, stRewardInfo.iValue2, 0, NULL);
}

void ioUserGuild::SQLUpdateRankRewardDate()
{
	if( !m_pUser )
		return;

	g_DBClient.OnUpdateGuildRankRewardDate(m_pUser->GetUserDBAgentID(), m_pUser->GetAgentThreadID(), m_pUser->GetUserIndex());

	RewardInfo stRewardInfo;
	g_GuildRewardMgr.GetRankReward(GetGuildLevel(), stRewardInfo);
	g_LogDBClient.OnInsertGameLogInfo(LogDBClient::GLT_GUILD_RANK_REWARD, m_pUser, 0, GetGuildIndex(), 0, stRewardInfo.iType, stRewardInfo.iValue1, stRewardInfo.iValue2, 0, NULL);
}

bool ioUserGuild::IsBuilder()
{
	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	if( pLocal )
	{
		if( m_szGuildPosition == pLocal->GetGuildMasterPostion() || m_szGuildPosition == pLocal->GetGuildSecondMasterPosition() )
			return true;
		else if( m_szGuildPosition == pLocal->GetGuildBuilderPosition() )
			return true;
	}

	return false;
}

void ioUserGuild::ActiveGuildRoom()
{
	m_bActiveGuildRoom	= TRUE;
}

void ioUserGuild::NotifyGuildRoomActive(const DWORD dwUserIndex)
{
	vGuildUserData_iter iter, iEnd;
	iEnd = m_UserList.end();
	for( iter = m_UserList.begin(); iter != iEnd; iter++ )
	{
		GuildUserData &kUserData = *iter;
		
		if( kUserData.m_dwUserIndex == dwUserIndex ) continue;
		
		UserParent *pUserParent = g_UserNodeManager.GetGlobalUserNode( kUserData.m_dwUserIndex );
		if( pUserParent )
		{
			if( pUserParent->IsUserOriginal() )
			{
				User *pUser = static_cast< User * >( pUserParent );
				pUser->ActiveUserGuildRoom();
			}
			else
			{
				SP2Packet kPacket(SSTPK_ACTIVE_GUILD_ROOM);
				PACKET_GUARD_VOID( kPacket.Write(GetGuildIndex()) );
				pUserParent->RelayPacket( kPacket );	
			}
		}
	}
}