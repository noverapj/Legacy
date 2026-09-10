
#include "stdafx.h"

#include "ioGuildChatMgr.h"
#include "ioGuildMarkMgr.h"
#include "ioDateHelp.h"

#include "GUI/UserListWnd.h"

ioGuildData::ioGuildData()
{
	m_dwGuildIndex					= 0;
	m_dwGuildMark					= 0;
	m_dwGuildRank					= 0;
	m_dwGuildLevel					= 0;
	m_dwGuildJoinUser				= 0;
	m_dwGuildMaxUser				= 0;
	m_dwGuildPoint					= 0;
	m_dwCurGuildPoint				= 0;
	m_dwGuildRegDate				= 0;	
	m_dwSyncTime					= 0;
	m_dwSyncTitleTime				= 0;
	m_dwSyncUserListTime			= 0;
	m_iGuildYesterdayAttendCount	= 0;
	m_iGuildTodayAttendCount		= 0;

	m_fGuildBonus					= FLOAT1;	

	m_bUserListUpdate				= false;
	m_bSync							= false;
	m_bGuildRoomOpen				= FALSE;
}

ioGuildData::~ioGuildData()
{
	m_vUserList.clear();
}

void ioGuildData::SetGuildIndex( DWORD dwGuildIndex )
{
	m_dwGuildIndex = dwGuildIndex;
}

void ioGuildData::SetGuildMark( DWORD dwGuildMark )
{
	if( dwGuildMark > m_dwGuildMark )         // 길드 마크는 변경되면 값이 항상 커진다.
	{
		if( m_dwGuildMark != 0 )
		{
			//길드마크가 변경되었다 기존 JPG를 삭제하라!!! 
			if( m_dwGuildMark > (DWORD)g_GuildMarkMgr.GetMaxReserveMarkIndex() )
			{
				char szBuf[MAX_PATH] = "";
				// 작은 마크 삭제
				sprintf( szBuf, "%s/%d_%d_s.jpg", g_GuildMarkMgr.GetGuildMarkAppDir().c_str() , m_dwGuildIndex, m_dwGuildMark );
				::DeleteFile( szBuf );

				// 큰 마크 삭제
				sprintf( szBuf, "%s/%d_%d_l.jpg", g_GuildMarkMgr.GetGuildMarkAppDir().c_str() , m_dwGuildIndex, m_dwGuildMark );
				::DeleteFile( szBuf );
			}
		}		
		m_dwGuildMark = dwGuildMark;
	}	
}

void ioGuildData::SetGuildRank( DWORD dwGuildRank )
{
	m_dwGuildRank = dwGuildRank;
}

void ioGuildData::SetGuildLevel( DWORD dwGuildLevel )
{
	m_dwGuildLevel = dwGuildLevel;
}

void ioGuildData::SetGuildJoinUser( DWORD dwGuildJoinUser )
{
	m_dwGuildJoinUser = dwGuildJoinUser;
	if( m_dwGuildMaxUser <= m_dwGuildJoinUser )
		m_dwGuildMaxUser = m_dwGuildJoinUser;
}

void ioGuildData::SetGuildMaxUser( DWORD dwGuildMaxUser )
{
	if( m_dwGuildMaxUser != dwGuildMaxUser )
		m_bUserListUpdate = true;
	m_dwGuildMaxUser = dwGuildMaxUser;
}

void ioGuildData::SetGuildPoint( DWORD dwGuildPoint )
{
	m_dwGuildPoint = dwGuildPoint;
}

void ioGuildData::SetGuildCurPoint( DWORD dwCurGuildPoint )
{
	m_dwCurGuildPoint = dwCurGuildPoint;
}

void ioGuildData::SetGuildRegDate( DWORD dwGuildRegDate )
{
	m_dwGuildRegDate = dwGuildRegDate;
}

void ioGuildData::SetGuildBonus( float fGuildBonus )
{
	m_fGuildBonus = fGuildBonus;
}

void ioGuildData::SetGuildName( const ioHashString &szGuildName )
{
	m_szGuildName = szGuildName;
}

void ioGuildData::SetGuildTitle( const ioHashString &szGuildTitle )
{
	m_szGuildTitle = szGuildTitle;
}

void ioGuildData::SetGuildRoomOpen( BOOL bGuildRoomOpen )
{
	m_bGuildRoomOpen = bGuildRoomOpen;
}

void ioGuildData::SetGuildInfo( SP2Packet &rkPacket )
{
	DWORD dwGuildMark;
	rkPacket >> dwGuildMark >> m_dwGuildRank >> m_dwGuildLevel >> m_dwGuildJoinUser >> m_dwGuildMaxUser >> m_dwGuildPoint >> m_dwGuildRegDate >> m_fGuildBonus;
	rkPacket >> m_szGuildName >> m_szGuildTitle >> m_dwGuildPoint >> m_dwCurGuildPoint;
	SetGuildMark( dwGuildMark );
	Sync();
}

void ioGuildData::SetGuildUserLogIn( const ioHashString &szUserID )
{
	int iSize = m_vUserList.size();
	for(int i = 0;i < iSize;i++)
	{
		GuildUser &kUserInfo = m_vUserList[i];
		{
			if( kUserInfo.m_szUserID == szUserID )
			{
				if( !kUserInfo.m_bOnline )
				{
					g_GUIMgr.ShowLoginPopup( szUserID );
					g_GuildChatMgr.OnLoginUser( szUserID );
				}
				g_UserInfoMgr.SetSyncTime( szUserID, 0 );

				kUserInfo.m_bOnline = true;
				UserListWnd::sg_bUserListUpdate =  true;
				return;
			}
		}
	}
}

void ioGuildData::SetGuildUserLogOut( const ioHashString &szUserID )
{
	int iSize = m_vUserList.size();
	for(int i = 0;i < iSize;i++)
	{
		GuildUser &kUserInfo = m_vUserList[i];
		{
			if( kUserInfo.m_szUserID == szUserID )
			{
				if( kUserInfo.m_bOnline )
					g_GuildChatMgr.OnLogOutUser( szUserID );

				kUserInfo.m_bOnline = false;
				UserListWnd::sg_bUserListUpdate =  true;
				return;
			}
		}
	}
}

void ioGuildData::SetGuildUserPos( const ioHashString &szUserID, int iUserPos, int iUserLevel )
{
	int iSize = m_vUserList.size();
	for(int i = 0;i < iSize;i++)
	{
		GuildUser &kUserInfo = m_vUserList[i];
		{
			if( kUserInfo.m_szUserID == szUserID )
			{
				kUserInfo.m_iUserPos   = iUserPos;
				if( iUserLevel != -1 )
				{
					kUserInfo.m_iUserLevel = iUserLevel;
					g_UserInfoMgr.SetGrade( szUserID, iUserLevel );
				}
				UserListWnd::sg_bUserListUpdate =  true;
				return;
			}
		}
	}
}

void ioGuildData::SetGuildUserList( SP2Packet &rkPacket )
{
	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );

	m_vUserList.clear();
	// 길드원 수
	// 길드원 테이블 인덱스, 길드원 인덱스, 길드원 레벨, 길드원 닉네임, 길드원 직책
	int iMaxUserList;
	rkPacket >> iMaxUserList;
	for(int i = 0;i < iMaxUserList;i++)
	{
		int iLadderPoint;
		GuildUser kUserInfo;
		rkPacket >> kUserInfo.m_dwTableIndex >> kUserInfo.m_dwUserIndex 
				 >> kUserInfo.m_iUserLevel >> kUserInfo.m_szUserID >> kUserInfo.m_szUserPos >> iLadderPoint;

		if( pLocal )
		{
			if( kUserInfo.m_szUserPos == pLocal->GetGuildMasterPostion() )
				kUserInfo.m_iSortPoint = 100;
			else if( kUserInfo.m_szUserPos == pLocal->GetGuildSecondMasterPosition() )
				kUserInfo.m_iSortPoint = 10;
		}
		m_vUserList.push_back( kUserInfo );

		g_UserInfoMgr.SetGuildIndex( kUserInfo.m_szUserID, m_dwGuildIndex );
		g_UserInfoMgr.SetGrade( kUserInfo.m_szUserID, kUserInfo.m_iUserLevel );
		g_UserInfoMgr.SetLadderPoint( kUserInfo.m_szUserID, iLadderPoint );		
	}
	m_dwGuildJoinUser = iMaxUserList;
	std::sort( m_vUserList.begin(), m_vUserList.end(), GuildUserSort() );
	m_bUserListUpdate = true;
}

void ioGuildData::SetGuildUserListEx( SP2Packet &rkPacket )
{
	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );

	int iMaxUserList;
	rkPacket >> iMaxUserList;

	bool bAllClear;
	rkPacket >> bAllClear;
	if( bAllClear )
		m_vUserList.clear();

	for(int i = 0;i < iMaxUserList;i++)
	{
		int iLadderPoint;
		GuildUser kUserInfo;
		rkPacket >> kUserInfo.m_dwTableIndex >> kUserInfo.m_dwUserIndex >> kUserInfo.m_szUserID >> kUserInfo.m_szUserPos;
		rkPacket >> kUserInfo.m_bOnline >> kUserInfo.m_iUserPos >> kUserInfo.m_iUserLevel >> iLadderPoint;

		if( GetUserIndex( kUserInfo.m_szUserID ) != 0 ) continue;

		if( pLocal )
		{
			if( kUserInfo.m_szUserPos == pLocal->GetGuildMasterPostion() )
				kUserInfo.m_iSortPoint = 100;
			else if( kUserInfo.m_szUserPos == pLocal->GetGuildSecondMasterPosition() )
				kUserInfo.m_iSortPoint = 10;
		}
		m_vUserList.push_back( kUserInfo );
		g_UserInfoMgr.SetGuildIndex( kUserInfo.m_szUserID, m_dwGuildIndex );
		g_UserInfoMgr.SetGrade( kUserInfo.m_szUserID, kUserInfo.m_iUserLevel );
		g_UserInfoMgr.SetLadderPoint( kUserInfo.m_szUserID, iLadderPoint );		
	}

	m_dwGuildJoinUser = (DWORD)m_vUserList.size();
	std::sort( m_vUserList.begin(), m_vUserList.end(), GuildUserSort() );
	m_bUserListUpdate = true;
	UserListWnd::sg_bUserListUpdate =  true;
}

void ioGuildData::LeaveGuildUser( const ioHashString &szUserName )
{
	int iSize = m_vUserList.size();
	for(int i = 0;i < iSize;i++)
	{
		GuildUser &kUserInfo = m_vUserList[i];
		{
			if( kUserInfo.m_szUserID == szUserName )
			{
				g_UserInfoMgr.SetGuildIndex( szUserName, 0 );
				m_vUserList.erase( m_vUserList.begin() + i );
				m_bUserListUpdate = true;				
				UserListWnd::sg_bUserListUpdate =  true;
				m_dwGuildJoinUser--;       //유저수 감소
				return;
			}
		}
	}
}

void ioGuildData::ProcessSync( bool bMyGuildOnlyInfo )
{
	if( m_dwSyncTime == 0 ||
		FRAMEGETTIME() - m_dwSyncTime > 30000 )          //30초에 한번만 동기화
	{
		m_dwSyncTime = FRAMEGETTIME();
		m_dwSyncTitleTime = FRAMEGETTIME();
		if( g_GuildInfo.IsMyGuild( GetGuildIndex() ) )
			m_dwSyncUserListTime = FRAMEGETTIME();

		SP2Packet kPacket( CTPK_GUILD_INFO );
		kPacket << m_dwGuildIndex << m_dwGuildJoinUser << bMyGuildOnlyInfo;
		TCPNetwork::SendToServer( kPacket );
	}
}

void ioGuildData::ProcessTitleSync()
{
	if( m_dwSyncTitleTime == 0 || 
		FRAMEGETTIME() - m_dwSyncTitleTime > 300000 )    //5분에 한번만 동기화
	{
		m_dwSyncTitleTime = FRAMEGETTIME();
		SP2Packet kPacket( CTPK_GUILD_TITLE_SYNC );
		kPacket << m_dwGuildIndex;
		TCPNetwork::SendToServer( kPacket );
	}
}

void ioGuildData::ProcessUserListSync()
{
	if( m_dwSyncUserListTime == 0 ||
		FRAMEGETTIME() - m_dwSyncUserListTime > 300000 )    //5분에 한번만 동기화
	{
		m_dwSyncUserListTime = FRAMEGETTIME();
		SP2Packet kPacket( CTPK_GUILD_USER_LIST );
		kPacket << m_dwGuildIndex << m_dwGuildJoinUser;
		TCPNetwork::SendToServer( kPacket );
	}
}

bool ioGuildData::IsUserListUpdate()
{
	bool bReturn = m_bUserListUpdate;
	m_bUserListUpdate = false;
	return bReturn;
}

DWORD ioGuildData::GetUserIndex( const ioHashString &szUserName )
{
	vGuildUser::iterator iter,iEnd;
	iEnd = m_vUserList.end();
	for(iter = m_vUserList.begin();iter != iEnd;iter++)
	{
		GuildUser &kUserInfo = *iter;
		if( kUserInfo.m_szUserID == szUserName )
		{
			return kUserInfo.m_dwUserIndex;
		}
	}
	return 0;
}

DWORD ioGuildData::GetUserTableIndex( const ioHashString &szUserName )
{
	vGuildUser::iterator iter,iEnd;
	iEnd = m_vUserList.end();
	for(iter = m_vUserList.begin();iter != iEnd;iter++)
	{
		GuildUser &kUserInfo = *iter;
		if( kUserInfo.m_szUserID == szUserName )
		{
			return kUserInfo.m_dwTableIndex;
		}
	}
	return 0;
}

const ioHashString &ioGuildData::GetUserName( DWORD dwUserIndex )
{
	vGuildUser::iterator iter,iEnd;
	iEnd = m_vUserList.end();
	for(iter = m_vUserList.begin();iter != iEnd;iter++)
	{
		GuildUser &kUserInfo = *iter;
		if( kUserInfo.m_dwUserIndex == dwUserIndex )
		{
			return kUserInfo.m_szUserID;
		}
	}
	static ioHashString szReturn = "";
	return szReturn;
}

int ioGuildData::GetOnlineUserSize()
{
	int iOnlineUserSize = 0;
	vGuildUser::iterator iter,iEnd;
	iEnd = m_vUserList.end();
	for(iter = m_vUserList.begin();iter != iEnd;iter++)
	{
		GuildUser &kUserInfo = *iter;
		if( kUserInfo.m_bOnline )
			iOnlineUserSize++;
	}
	return iOnlineUserSize;
}

void ioGuildData::GetUserInfo( int iUserArray, DWORD &dwTableIndex, DWORD &dwUserIndex, int &iUserLevel, ioHashString &szUserID, ioHashString &szUserPos )
{
	if( !COMPARE( iUserArray, 0, (int)m_vUserList.size() ) )
		return;
	
	GuildUser &kUserInfo = m_vUserList[iUserArray];
	dwTableIndex = kUserInfo.m_dwTableIndex;
	dwUserIndex  = kUserInfo.m_dwUserIndex;
	iUserLevel   = kUserInfo.m_iUserLevel;
	szUserID     = kUserInfo.m_szUserID;
	szUserPos    = kUserInfo.m_szUserPos;
}

void ioGuildData::GetUserInfo( int iUserArray, ioHashString &szUserID, int &iUserLevel, int &iUserPos, bool &bOnline )
{
	if( !COMPARE( iUserArray, 0, (int)m_vUserList.size() ) )
		return;

	GuildUser &kUserInfo = m_vUserList[iUserArray];
	szUserID     = kUserInfo.m_szUserID;
	iUserLevel   = kUserInfo.m_iUserLevel;
	iUserPos     = kUserInfo.m_iUserPos;
	bOnline		 = kUserInfo.m_bOnline;
}

void ioGuildData::GetUserInfo( int iUserArray, ioHashString &szUserID, bool &bOnline )
{
	if( !COMPARE( iUserArray, 0, (int)m_vUserList.size() ) )
		return;

	GuildUser &kUserInfo = m_vUserList[iUserArray];
	szUserID     = kUserInfo.m_szUserID;
	bOnline      = kUserInfo.m_bOnline;
}

void ioGuildData::GetUserInfo( int iUserArray, ioHashString &szUserID, int &iUserLevel, bool &bAttendYesterday, bool &bAttendToday )
{
	szUserID.Clear();
	iUserLevel = -1;
	bAttendYesterday = false;
	bAttendToday = false;
	if( !COMPARE( iUserArray, 0, (int)m_vUserList.size() ) )
		return;

	GuildUser &kUserInfo = m_vUserList[iUserArray];
	szUserID			= kUserInfo.m_szUserID;
	iUserLevel			= kUserInfo.m_iUserLevel;
	bAttendYesterday	= kUserInfo.m_bAttendedYesterday;
	bAttendToday		= kUserInfo.m_bAttendedToday;
}

void ioGuildData::GetUserPos( DWORD dwUserIndex, ioHashString& szUserPos )
{
	for( int i = 0; i <  (int)m_vUserList.size(); ++i )
	{
		GuildUser &kUserInfo = m_vUserList[i];
		if( kUserInfo.m_dwUserIndex == dwUserIndex )
		{
			szUserPos = kUserInfo.m_szUserPos;
		}
	}
}

void ioGuildData::FillUserList( SP2Packet &rkPacket )
{
	int iSize = m_vUserList.size();
	rkPacket << iSize;
	for(int i = 0;i < iSize;i++)
	{
		GuildUser &kUserInfo = m_vUserList[i];
		{
			rkPacket << kUserInfo.m_dwUserIndex;
		}
	}
}

void ioGuildData::ChangeGuildMaster( DWORD dwGuildMasterIndex )
{
	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );

	vGuildUser::iterator iter,iEnd;
	iEnd = m_vUserList.end();
	for(iter = m_vUserList.begin();iter != iEnd;iter++)
	{
		GuildUser &kUserInfo = *iter;

		if( pLocal )
		{
			if( kUserInfo.m_szUserPos == pLocal->GetGuildMasterPostion() )
			{
				kUserInfo.m_szUserPos = pLocal->GetGuildGeneralPosition();
			}
			if( kUserInfo.m_dwUserIndex == dwGuildMasterIndex )
			{
				kUserInfo.m_szUserPos = pLocal->GetGuildMasterPostion();
			}

			if( kUserInfo.m_szUserPos == pLocal->GetGuildMasterPostion() )
				kUserInfo.m_iSortPoint = 100;
			else if( kUserInfo.m_szUserPos == pLocal->GetGuildSecondMasterPosition() )
				kUserInfo.m_iSortPoint = 10;
			else if( kUserInfo.m_szUserPos == pLocal->GetGuildAdminPosition() )
				kUserInfo.m_iSortPoint = 5;
			else
				kUserInfo.m_iSortPoint = 1;
		}
	}
	std::sort( m_vUserList.begin(), m_vUserList.end(), GuildUserSort() );
	m_bUserListUpdate = true;
}

void ioGuildData::ChangeGuildPosition( DWORD dwTargetIndex, const ioHashString &szChangePosition )
{
	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );

	vGuildUser::iterator iter,iEnd;
	iEnd = m_vUserList.end();
	for(iter = m_vUserList.begin();iter != iEnd;iter++)
	{
		GuildUser &kUserInfo = *iter;
		if( kUserInfo.m_dwUserIndex == dwTargetIndex )
		{
			kUserInfo.m_szUserPos = szChangePosition;
		}

		if( pLocal )
		{
			if( kUserInfo.m_szUserPos == pLocal->GetGuildMasterPostion() )
				kUserInfo.m_iSortPoint = 100;
			else if( kUserInfo.m_szUserPos == pLocal->GetGuildSecondMasterPosition() )
				kUserInfo.m_iSortPoint = 10;
			else
				kUserInfo.m_iSortPoint = 1;
		}
	}

	std::sort( m_vUserList.begin(), m_vUserList.end(), GuildUserSort() );
	m_bUserListUpdate = true;	
}

DWORD ioGuildData::GetGuildMaster()
{
	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );

	vGuildUser::iterator iter,iEnd;
	iEnd = m_vUserList.end();
	for(iter = m_vUserList.begin();iter != iEnd;iter++)
	{
		GuildUser &kUserInfo = *iter;
		
		if( pLocal )
		{
			if( kUserInfo.m_szUserPos == pLocal->GetGuildMasterPostion() )
				return kUserInfo.m_dwUserIndex;
		}
	}
	return 0;
}

const ioHashString &ioGuildData::GetGuildMasterName()
{
	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );

	vGuildUser::iterator iter,iEnd;
	iEnd = m_vUserList.end();
	for(iter = m_vUserList.begin();iter != iEnd;iter++)
	{
		GuildUser &kUserInfo = *iter;
		if( pLocal )
		{
			if( kUserInfo.m_szUserPos == pLocal->GetGuildMasterPostion() )
				return kUserInfo.m_szUserID;
		}
	}

	static ioHashString szReturn = "";
	return szReturn;
}

DWORD ioGuildData::GetGuildSecondMaster()
{
	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	
	vGuildUser::iterator iter,iEnd;
	iEnd = m_vUserList.end();
	for(iter = m_vUserList.begin();iter != iEnd;iter++)
	{
		GuildUser &kUserInfo = *iter;
		if( pLocal )
		{
			if( kUserInfo.m_szUserPos == pLocal->GetGuildSecondMasterPosition() )
				return kUserInfo.m_dwUserIndex;
		}
	}
	return 0;
}

const ioHashString &ioGuildData::GetGuildSecondMasterName()
{
	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	
	vGuildUser::iterator iter,iEnd;
	iEnd = m_vUserList.end();
	for(iter = m_vUserList.begin();iter != iEnd;iter++)
	{
		GuildUser &kUserInfo = *iter;
		if( pLocal )
		{
			if( kUserInfo.m_szUserPos == pLocal->GetGuildSecondMasterPosition() )
				return kUserInfo.m_szUserID;
		}
	}

	static ioHashString szReturn = "";
	return szReturn;
}

void ioGuildData::ChangeUserID( const ioHashString &rszUserID, const ioHashString &rszNewUserID )
{
	vGuildUser::iterator iter,iEnd;
	iEnd = m_vUserList.end();
	for(iter = m_vUserList.begin();iter != iEnd;iter++)
	{
		GuildUser &kUserInfo = *iter;
		if( kUserInfo.m_szUserID != rszUserID )
			continue;

		kUserInfo.m_szUserID = rszNewUserID;
		m_bUserListUpdate    = true;	
		return;
	}
}

void ioGuildData::InitGuildTodayAttendedMember()
{
	for ( int i=0 ; i<(int)m_vUserList.size() ; ++i )
		m_vUserList[i].m_bAttendedToday = false;
}

void ioGuildData::InitGuildYesterdayAttendedMember()
{
	for ( int i=0 ; i<(int)m_vUserList.size() ; ++i )
		m_vUserList[i].m_bAttendedYesterday = false;
}

void ioGuildData::SetMyGuildMemberTodayAttendInfoByPacket( SP2Packet &rkPacket )
{
	if ( m_vUserList.empty() )
	{
		LOG.PrintTimeAndLog( 0, "%s : UserList is not exist ", __FUNCTION__ );
		return;
	}
	// 길드원 수, 출석한 유저 인덱스
	int iAttendedCount;
	rkPacket >> iAttendedCount;

	//초기화
	InitGuildTodayAttendedMember();
	m_iGuildTodayAttendCount = 0;

	for(int i = 0;i < iAttendedCount;i++)
	{
		DWORD dwUserIndex;
		rkPacket >> dwUserIndex;
		SetGuildTodayAttendedUser( dwUserIndex );
	}

	m_iGuildTodayAttendCount = iAttendedCount; 
}

void ioGuildData::SetMyGuildMemberYesterdayAttendInfoByPacket( SP2Packet &rkPacket )
{
	if ( m_vUserList.empty() )
	{
		LOG.PrintTimeAndLog( 0, "%s : UserList is not exist ", __FUNCTION__ );
		return;
	}
	// 길드원 수, 출석한 유저 인덱스
	int iAttendedCount;
	rkPacket >> iAttendedCount;

	//출석 초기화
	InitGuildYesterdayAttendedMember();
	m_iGuildYesterdayAttendCount = 0;

	for(int i = 0;i < iAttendedCount;i++)
	{
		DWORD dwUserIndex;
		rkPacket >> dwUserIndex;
		SetGuildYesterdayAttendedUser( dwUserIndex );
	}

	m_iGuildYesterdayAttendCount = iAttendedCount; 
}

void ioGuildData::SetGuildTodayAttendedUser( DWORD dwUserIndex )
{
	for ( int i=0 ; i<(int)m_vUserList.size() ; ++i )
	{
		if( m_vUserList[i].m_dwUserIndex == dwUserIndex )
		{
			//중복출석인지 체크
			if ( !m_vUserList[i].m_bAttendedToday )
			{
				m_vUserList[i].m_bAttendedToday = true;
				m_iGuildTodayAttendCount++;
				return;
			}
		}
	}
}

void ioGuildData::SetGuildYesterdayAttendedUser( DWORD dwUserIndex )
{
	for ( int i=0 ; i<(int)m_vUserList.size() ; ++i )
	{
		if( m_vUserList[i].m_dwUserIndex == dwUserIndex )
		{
			if ( !m_vUserList[i].m_bAttendedYesterday )
			{
				m_vUserList[i].m_bAttendedYesterday = true;
				return;
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////
template<> ioGuildInfo* Singleton< ioGuildInfo >::ms_Singleton = 0;
ioGuildInfo::ioGuildInfo() : m_iGuildCreatePeso( 25000 ), m_iGuildMarkChangePeso( 25000 ), 
							 m_bGuildMarkChanging( false ), m_iGuildCreateLimitLevel( 10 ),
							 m_iGuildKickOutPeso( 1000 ), m_iGuildLeavePeso( 1000 ),
							 m_GuildCreatedTime( 0 ), m_EnableAttendTime( 0 ), 
							 m_EnableRewardTime( 0 ), m_LoginTime( 0 ),
							 m_MemberAttendInfoRequestTime( 0 ),
							 m_iAttendRenewalHour( 0 )
{	
}

ioGuildInfo::~ioGuildInfo()
{	
	vGuildData_iter iter, iEnd;
	iEnd = m_vGuildList.end();
	for(iter = m_vGuildList.begin();iter != iEnd;++iter)
	{
		SAFEDELETE( *iter );
	}	
	m_vGuildList.clear();

	for( GuildRankRewardVector::iterator Rankiter = m_RankRewardVec.begin() ; 
		Rankiter != m_RankRewardVec.end() ; ++Rankiter )
		SAFEDELETE( *Rankiter );
	m_RankRewardVec.clear();

	for( GuildAttendRewardVector::iterator Attenditer = m_AttendRewardVec.begin() ; 
		Attenditer != m_AttendRewardVec.end() ; ++Attenditer )
		SAFEDELETE( *Attenditer );
	m_AttendRewardVec.clear();
}

ioGuildInfo& ioGuildInfo::GetSingleton()
{
	return Singleton< ioGuildInfo >::GetSingleton();
}

void ioGuildInfo::Initialize()
{
	InitMyGuildData();

	ioINILoader_e kLoader( "config/sp2_guild.ini" );
	kLoader.SetTitle_e( "info" );
	m_iGuildCreatePeso = kLoader.LoadInt_e( "guild_create_peso", 25000 );
	m_iGuildMarkChangePeso = kLoader.LoadInt_e( "guild_mark_change_peso", 25000  );
	m_iGuildCreateLimitLevel = kLoader.LoadInt_e( "guild_create_limit_level", 10 );
	m_iGuildKickOutPeso = kLoader.LoadInt_e( "guild_kick_out_peso", 1000 );
	m_iGuildLeavePeso = kLoader.LoadInt_e( "guild_leave_peso", 1000 );

	char szKey[MAX_PATH] = "";
	m_dwLevelToEntryCount.clear();
	kLoader.SetTitle_e( "guild_entry" );
	int iMaxEntry = kLoader.LoadInt_e( "entry_count", 0 );
	for(int i = 0;i < iMaxEntry;i++)
	{
		// 레벨에 따른 최대 인원
		sprintf_e( szKey, "max_entry_%d", i );
		m_dwLevelToEntryCount.push_back( (DWORD)kLoader.LoadInt( szKey, 8 ) );
	}

	m_vInvitationUser.clear();

	kLoader.SetFileName( "config/sp2_guild_reward.ini" );
	kLoader.SetTitle_e( "common" );
	m_iAttendRenewalHour = kLoader.LoadInt( "renewal_hour", 0 );
}

DWORD ioGuildInfo::GetGuildLevelToMaxEntry( DWORD dwGuildLevel )
{
	if( dwGuildLevel >= m_dwLevelToEntryCount.size() )
		return 0;

	return m_dwLevelToEntryCount[dwGuildLevel];
}

void ioGuildInfo::InitMyGuildData()
{
	m_dwGuildIndex	  = 0;
	m_szGuildName.Clear();
	m_szGuildPosition.Clear();
	g_UserInfoMgr.SetGuildIndex( g_MyInfo.GetPublicID(), 0 );

	m_GuildCreatedTime				= 0;
	m_EnableAttendTime				= 0;
	m_EnableRewardTime				= 0;
	m_LoginTime						= 0;
	m_MemberAttendInfoRequestTime	= 0;

	// 방안 유저들 동기화
	SP2Packet kPacket( CUPK_GUILD_INFO_CHANGE );
	kPacket << g_MyInfo.GetPublicID() << 0 << 0;
	P2PNetwork::SendToAllPlayingUser( kPacket );
	g_App.CloseGuildMarkPage();

	UserListWnd::sg_bUserListUpdate =  true;
}

void ioGuildInfo::SetMyGuildData( DWORD dwIndex, const ioHashString &szGuildName, const ioHashString &szGuildPosition, DWORD dwGuildMark, BOOL bGuildRoomOpen )
{
	m_dwGuildIndex	  = dwIndex;
	m_szGuildName     = szGuildName;
	m_szGuildPosition = szGuildPosition;

	m_dwEntryDelayGuildList.clear();
	g_UserInfoMgr.SetGuildIndex( g_MyInfo.GetPublicID(), m_dwGuildIndex );
	
	//
	ioGuildData *pGuildData = GetGuildData( m_dwGuildIndex );
	if( pGuildData )
	{
		pGuildData->SetGuildMark( dwGuildMark );
		pGuildData->SetGuildName( m_szGuildName );
		pGuildData->SetGuildRoomOpen( bGuildRoomOpen );
		pGuildData->InitSyncTimer();         		
	}
	DeleteEntryDelayGuildList();

	// 방안 유저들 동기화
	SP2Packet kPacket( CUPK_GUILD_INFO_CHANGE );
	kPacket << g_MyInfo.GetPublicID() << dwIndex << dwGuildMark;
	P2PNetwork::SendToAllPlayingUser( kPacket );

/*  진영과 길드 분리됨
	// 진영 처리
	if( m_dwGuildIndex != 0 )
	{
		if( g_MyInfo.GetUserCampPos() == CAMP_NONE )
		{
			// 유저 정보를 받기전에 내 길드 정보가 먼저 오므로 아래의 패킷은 유저 정보를 받은 다음부터 처리
			if( !g_MyInfo.GetPublicID().IsEmpty() )
			{
				// 진영에 속해있지않으면 자동으로 길드 진영에 들어간다
				TCPNetwork::MouseBusy( true );
				SP2Packet kPacket( CTPK_CAMP_CAHNGE_POS );
				kPacket << dwGuildCampPos << IsGuildEntryDelayListDelete() << false;     // false = Result UI Hide
				TCPNetwork::SendToServer( kPacket );
			}
		}
		else if( g_MyInfo.GetUserCampPos() != dwGuildCampPos )
		{
			// 길드와 자신의 진영이 다르면 길드 탈퇴!!!! 이것은 발생해서는 안되는 상황
			// 단, 예외는 자신이 길드장일 때는 봐주자-_-;
			ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
			if( pLocal )
			{
				if( m_szGuildPosition != pLocal->GetGuildMasterPostion() )
				{
					TCPNetwork::MouseBusy( true );
					SP2Packet kPacket( CTPK_GUILD_LEAVE );
					kPacket << 0; //예외 상황 이므로 페소 차감은 0
					TCPNetwork::SendToServer( kPacket );			
				}
			}
		}
	}
*/
}

void ioGuildInfo::SetMyGuildUserList( SP2Packet &rkPacket )
{
	DWORD dwGuildIndex;
	rkPacket >> dwGuildIndex;
	ioGuildData *pGuildData = GetGuildData( dwGuildIndex );
	if( pGuildData )
		pGuildData->SetGuildUserListEx( rkPacket );
}

void ioGuildInfo::SetMyGuildUserLogIn( const ioHashString &szUserID )
{
	ioGuildData *pGuildData = GetGuildData( m_dwGuildIndex );
	if( pGuildData )
		pGuildData->SetGuildUserLogIn( szUserID );
}

void ioGuildInfo::SetMyGuildUserLogOut( const ioHashString &szUserID )
{
	ioGuildData *pGuildData = GetGuildData( m_dwGuildIndex );
	if( pGuildData )
		pGuildData->SetGuildUserLogOut( szUserID );
}

void ioGuildInfo::SetMyGuildUserPos( const ioHashString &szUserID, int iUserPos, int iUserLevel )
{
	ioGuildData *pGuildData = GetGuildData( m_dwGuildIndex );
	if( pGuildData )
		pGuildData->SetGuildUserPos( szUserID, iUserPos, iUserLevel );
}

bool ioGuildInfo::IsMyGuild( DWORD dwGuildIndex )
{
	if( m_dwGuildIndex == dwGuildIndex )
		return true;
	return false;
}

bool ioGuildInfo::IsMyGuildUser( const ioHashString &szUserID )
{
	ioGuildData *pGuildData = GetGuildData( m_dwGuildIndex );
	if( pGuildData )
	{
		if( pGuildData->GetUserIndex( szUserID ) != 0 )
			return true;
	}
	return false;
}

bool ioGuildInfo::IsGuild()
{
	if( m_dwGuildIndex == 0 )
		return false;
	return true;
}

bool ioGuildInfo::IsGuildEntryDelay( DWORD dwGuildIndex )
{
	if( m_dwEntryDelayGuildList.empty( )) return false;
	
	int iMaxSize = m_dwEntryDelayGuildList.size();
	for(int i = 0;i < iMaxSize;i++)
	{
		if( m_dwEntryDelayGuildList[i] == dwGuildIndex )
			return true;
	}
	return false;
}

bool ioGuildInfo::IsGuildEntryDelayListDelete()
{
	if( m_dwEntryDelayGuildList.empty() ) return false;

	m_dwEntryDelayGuildList.clear();
	return true;
}

void ioGuildInfo::ApplyEntryDelayGuildList( SP2Packet &rkPacket )
{
	m_dwEntryDelayGuildList.clear();
	int iMaxList;
	rkPacket >> iMaxList;
	for(int i = 0;i < iMaxList;i++)
	{
		DWORD dwGuildIndex;
		rkPacket >> dwGuildIndex;
		m_dwEntryDelayGuildList.push_back( dwGuildIndex );
	}
}

void ioGuildInfo::AddEntryDelayGuildList( DWORD dwGuildIndex )
{
	m_dwEntryDelayGuildList.push_back( dwGuildIndex );
}


void ioGuildInfo::DeleteEntryDelayGuildList()
{
	m_dwEntryDelayGuildList.clear();
}

void ioGuildInfo::DeleteEntryDelayGuildList( DWORD dwGuildIndex )
{
	int iMaxSize = m_dwEntryDelayGuildList.size();
	for(int i = 0;i < iMaxSize;i++)
	{
		if( m_dwEntryDelayGuildList[i] == dwGuildIndex )
		{
			m_dwEntryDelayGuildList.erase( m_dwEntryDelayGuildList.begin() + i );
			return;
		}
	}
}

ioGuildData *ioGuildInfo::GetGuildData( DWORD dwGuildIndex, bool bNetwork )
{
	if( dwGuildIndex == 0 ) 
		return NULL;

	vGuildData_iter iter, iEnd;
	iEnd = m_vGuildList.end();
	for(iter = m_vGuildList.begin();iter != iEnd;++iter)
	{
		ioGuildData *pGuildData = *iter;
		if( pGuildData->GetGuildIndex() == dwGuildIndex )
		{
			if( bNetwork )
				pGuildData->ProcessSync();
			return pGuildData;
		}
	}	
	// 없으면 생성
	ioGuildData *pGuildData = new ioGuildData;
	pGuildData->SetGuildIndex( dwGuildIndex );
	m_vGuildList.push_back( pGuildData );
	if( bNetwork )
		pGuildData->ProcessSync();
	return pGuildData;
}

ioGuildData * ioGuildInfo::GetGuildDataExist( const ioHashString &rszGuildName )
{
	if( rszGuildName.IsEmpty() ) return NULL;

	vGuildData_iter iter, iEnd;
	iEnd = m_vGuildList.end();
	for(iter = m_vGuildList.begin();iter != iEnd;++iter)
	{
		ioGuildData *pGuildData = *iter;
		if( pGuildData->GetGuildName() == rszGuildName )
			return pGuildData;
	}	

	return NULL;
}

void ioGuildInfo::SetGuildUserList( SP2Packet &rkPacket )
{
	DWORD dwGuildIndex;
	rkPacket >> dwGuildIndex;
	ioGuildData *pGuildData = GetGuildData( dwGuildIndex );
	if( pGuildData )
		pGuildData->SetGuildUserList( rkPacket );
}

void ioGuildInfo::SetGuildListInfo( DWORD dwGuildIndex, DWORD dwGuildRank, DWORD dwGuildLevel, DWORD dwGuildMark, 
									ioHashString szGuildName, DWORD dwGuildJoinUser, DWORD dwGuildMaxUser, float fGuildBonus, int iGuildPoint )
{
	ioGuildData *pGuildData = GetGuildData( dwGuildIndex );
	if( pGuildData )
	{
		pGuildData->SetGuildRank( dwGuildRank );
		pGuildData->SetGuildLevel( dwGuildLevel );
		pGuildData->SetGuildMark( dwGuildMark );
		pGuildData->SetGuildName( szGuildName );
		pGuildData->SetGuildJoinUser( dwGuildJoinUser );
		pGuildData->SetGuildMaxUser( dwGuildMaxUser );
		pGuildData->SetGuildBonus( fGuildBonus );
		pGuildData->SetGuildCurPoint( iGuildPoint );
		
		if( IsGuildEntryDelay( dwGuildIndex ) )
		{
			 //내가 가입대기중인 길드 유저수가 풀이면 대기 리스트에서 삭제
			if( dwGuildJoinUser == dwGuildMaxUser )       
				DeleteEntryDelayGuildList( dwGuildIndex );
		}		
		pGuildData->Sync();
	}
}

void ioGuildInfo::SetGuildListInfo( SP2Packet &rkPacket )
{
	DWORD dwGuildIndex;
	rkPacket >> dwGuildIndex;
	ioGuildData *pGuildData = GetGuildData( dwGuildIndex );
	if( pGuildData )
	{
		pGuildData->SetGuildInfo( rkPacket );

		if( IsGuildEntryDelay( pGuildData->GetGuildIndex() ) )
		{        
			//내가 가입대기중인 길드 유저수가 풀이면 대기 리스트에서 삭제
			if( pGuildData->GetGuildJoinUser() == pGuildData->GetGuildMaxUser() )
				DeleteEntryDelayGuildList( pGuildData->GetGuildIndex() );
		}
		pGuildData->Sync();
	}
}

void ioGuildInfo::SetGuildSimpleInfo( DWORD dwGuildIndex, const ioHashString &szGuildName, DWORD dwGuildMark )
{
	ioGuildData *pGuildData = GetGuildData( dwGuildIndex );
	if( pGuildData )
	{
		pGuildData->SetGuildName( szGuildName );
		pGuildData->SetGuildMark( dwGuildMark );
	}
}

void ioGuildInfo::SetGuildMark( DWORD dwGuildIndex, DWORD dwGuildMark )
{
	ioGuildData *pGuildData = GetGuildData( dwGuildIndex );
	if( pGuildData )
	{
		pGuildData->SetGuildMark( dwGuildMark );
	}
}

void ioGuildInfo::SetGuildMark( const ioHashString &szUserID, DWORD dwGuildIndex, DWORD dwGuildMark )
{
	ioGuildData *pGuildData = GetGuildData( dwGuildIndex );
	if( pGuildData )
	{
		pGuildData->SetGuildMark( dwGuildMark );
	}
	g_UserInfoMgr.SetGuildIndex( szUserID, dwGuildIndex );	
}

void ioGuildInfo::SetGuildTitle( DWORD dwGuildIndex, const ioHashString &szGuildTitle )
{
	ioGuildData *pGuildData = GetGuildData( dwGuildIndex );
	if( pGuildData )
	{
		pGuildData->SetGuildTitle( szGuildTitle );
	}
}

void ioGuildInfo::SetGuildCampInfo( DWORD dwGuildIndex, DWORD dwGuildMark, DWORD dwGuildLevel, DWORD dwGuildRank, ioHashString szGuildName, DWORD dwCurGuildPoint )
{
	ioGuildData *pGuildData = GetGuildData( dwGuildIndex );
	if( pGuildData )
	{
		pGuildData->SetGuildMark( dwGuildMark );
		pGuildData->SetGuildLevel( dwGuildLevel );
		pGuildData->SetGuildRank( dwGuildRank );
		pGuildData->SetGuildName( szGuildName );
		pGuildData->SetGuildCurPoint( dwCurGuildPoint );
		pGuildData->Sync();
	}
}

void ioGuildInfo::SetInvitationUser( const ioHashString &szInvitationUser )
{
	InvitationUser kUser;
	kUser.m_szUserName		  = szInvitationUser;
	kUser.m_dwInvitationTimer = FRAMEGETTIME();
	m_vInvitationUser.push_back( kUser );
}

bool ioGuildInfo::IsInvitationUser( const ioHashString &szInvitationUser )
{
	int iSize = m_vInvitationUser.size();
	if( iSize == 0 )
		return false;

	for(int i = 0;i < iSize;i++)
	{
		InvitationUser &kUser = m_vInvitationUser[i];
		if( kUser.m_szUserName == szInvitationUser )
		{
			if( FRAMEGETTIME() - kUser.m_dwInvitationTimer > 300000 )
			{
				m_vInvitationUser.erase( m_vInvitationUser.begin() + i );
				return false;
			}
			else
				return true;
		}
	}
	return false;
}

void ioGuildInfo::ChangeMyGuildUserID( const ioHashString &szUserID, const ioHashString &szNewUserID )
{
	ioGuildData *pGuildData = GetGuildData( m_dwGuildIndex );
	if( pGuildData )
		pGuildData->ChangeUserID( szUserID, szNewUserID );
}

bool ioGuildInfo::IsGuildMaster()
{
	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	if( pLocal )
	{
		return ( m_szGuildPosition == pLocal->GetGuildMasterPostion() );
	}

	return false;
}

bool ioGuildInfo::IsGuildSecondMaster()
{
	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	if( pLocal )
	{
		return ( m_szGuildPosition == pLocal->GetGuildSecondMasterPosition() );
	}

	return false;
}

DWORD ioGuildInfo::GetMyGuildLevel()
{
	ioGuildData *pGuildData = GetGuildData( m_dwGuildIndex );
	if( pGuildData )
		return pGuildData->GetGuildLevel();
	return 0;
}

void ioGuildInfo::InitializeGuildReward()
{
	ioINILoader_e kLoader( "config/sp2_guild_reward.ini" );

	LoadRankReward( kLoader );
	LoadAttendReward( kLoader );
}

void ioGuildInfo::LoadRankReward( ioINILoader& rkLoader )
{
	rkLoader.SetTitle_e( "rank_reward" );

	char szBuf[MAX_PATH] = "";
	char szValue[MAX_PATH] = "";

	enum { MAX_RANK_CNT = 7, MAX_POSSIBLE_CNT = 100, };
	for ( int i=0 ; i<MAX_RANK_CNT ; ++i )
	{
		GuildRankRwardInfo* pInfo = new GuildRankRwardInfo;

		sprintf_s_e( szBuf, "reward%d_name", i + 1 );
		rkLoader.LoadString( szBuf, "", szValue, MAX_PATH );
		pInfo->m_szName = szValue;
		if ( pInfo->m_szName.IsEmpty() )
		{
			SAFEDELETE( pInfo );
			break;
		}

		ZeroMemory( szBuf,sizeof(szBuf) );
		sprintf_s_e( szBuf, "reward%d_rank", i + 1 );
		pInfo->m_iRank = rkLoader.LoadInt( szBuf, 1000 );

		ZeroMemory( szBuf,sizeof(szBuf) );
		ZeroMemory( szValue,sizeof(szValue) );
		sprintf_s_e( szBuf, "reward%d_values", i + 1 );
		rkLoader.LoadString( szBuf, "", szValue, MAX_PATH );
		StringVector kString = ioStringConverter::Split( szValue, "." );
		if ( kString.size() > 0 )
		{
			for ( int iLine=0 ; iLine<(int)kString.size() ; ++iLine )
			{
				if ( iLine== 0 )
					pInfo->m_RewardItem.m_iPresentType = atoi( kString[iLine].c_str() );
				if ( iLine== 1 )
					pInfo->m_RewardItem.m_iValue1 =atoi( kString[iLine].c_str() );
				if ( iLine== 2 )
					pInfo->m_RewardItem.m_iValue2 =atoi( kString[iLine].c_str() );
			}
		}

		if ( pInfo->m_RewardItem.m_iPresentType != PRESENT_ETC_ITEM )
		{
			LOG.PrintTimeAndLog( 0, "%s - reward Type is not etcitem Type", __FUNCTION__ );
			SAFEDELETE( pInfo );
			continue;
		}
		pInfo->m_RewardPossibleItemVec = g_GashaponMgr.FindGashaponPresentList( pInfo->m_RewardItem.m_iValue1 );
		if ( pInfo->m_RewardPossibleItemVec.empty() )
		{
			LOG.PrintTimeAndLog( 0, "%s - not exist gashapon number", __FUNCTION__ );
			SAFEDELETE( pInfo );
			continue;
		}
		m_RankRewardVec.push_back( pInfo );
	}
}

void ioGuildInfo::LoadAttendReward( ioINILoader& rkLoader )
{
	rkLoader.SetTitle_e( "attendance_reward" );

	char szBuf[MAX_PATH] = "";
	char szValue[MAX_PATH] = "";

	enum { MAX_RANK_CNT = 7, MAX_POSSIBLE_CNT = 100, };
	for ( int i=0 ; i<MAX_RANK_CNT ; ++i )
	{
		GuildAttendRwardInfo* pInfo = new GuildAttendRwardInfo;

		sprintf_s_e( szBuf, "reward%d_name", i + 1 );
		rkLoader.LoadString( szBuf, "", szValue, MAX_PATH );
		pInfo->m_szName = szValue;
		if ( pInfo->m_szName.IsEmpty() )
		{
			SAFEDELETE( pInfo );
			break;
		}

		ZeroMemory( szBuf,sizeof(szBuf) );
		ZeroMemory( szValue,sizeof(szValue) );
		sprintf_s_e( szBuf, "reward%d_values", i + 1 );
		rkLoader.LoadString( szBuf, "", szValue, MAX_PATH );
		StringVector kString = ioStringConverter::Split( szValue, "." );
		if ( kString.size() > 0 )
		{
			for ( int iLine=0 ; iLine<(int)kString.size() ; ++iLine )
			{
				if ( iLine== 0 )
					pInfo->m_RewardItem.m_iPresentType = atoi( kString[iLine].c_str() );
				if ( iLine== 1 )
					pInfo->m_RewardItem.m_iValue1 =atoi( kString[iLine].c_str() );
				if ( iLine== 2 )
					pInfo->m_RewardItem.m_iValue2 =atoi( kString[iLine].c_str() );
			}
		}
		
		if ( pInfo->m_RewardItem.m_iPresentType != PRESENT_ETC_ITEM )
		{
			LOG.PrintTimeAndLog( 0, "%s - reward Type is not etcitem Type", __FUNCTION__ );
			SAFEDELETE( pInfo );
			continue;
		}
		pInfo->m_RewardPossibleItemVec = g_GashaponMgr.FindGashaponPresentList( pInfo->m_RewardItem.m_iValue1 );
		if ( pInfo->m_RewardPossibleItemVec.empty() )
		{
			LOG.PrintTimeAndLog( 0, "%s - not exist gashapon number", __FUNCTION__ );
			SAFEDELETE( pInfo );
			continue;
		}
		m_AttendRewardVec.push_back( pInfo );
	}
}

int ioGuildInfo::GetAttendRewardSize()
{
	return m_AttendRewardVec.size();
}

int ioGuildInfo::GetRankRewardSize()
{
	return m_RankRewardVec.size();
}

const GuildAttendRwardInfo* const ioGuildInfo::GetAttendRewardInfo( int iIndex ) const
{
	if ( !COMPARE( iIndex, 0, (int)m_AttendRewardVec.size() ) )
		return NULL;

	return m_AttendRewardVec[iIndex];
}

const GuildRankRwardInfo* const ioGuildInfo::GetRankRewardInfo( int iIndex ) const
{
	if ( !COMPARE( iIndex, 0, (int)m_RankRewardVec.size() ) )
		return NULL;

	return m_RankRewardVec[iIndex];
}

void ioGuildInfo::SetMyGuildTimeSetting( DWORD dwRecievedTime, DWORD dwCreatedTime, DWORD dwLoginTime )
{
	m_LoginTime = CTime( dwLoginTime );
	m_GuildCreatedTime = CTime( dwCreatedTime );
	//길드 출석 보상 시간 세팅
	SetAttendRewardTime( CTime( dwRecievedTime ) );	
	//LOG.PrintTimeAndLog( 0, "%s create Time : %d, Enable Reward Time : %d", __FUNCTION__, m_GuildCreatedTime.GetTime(), m_EnableRewardTime.GetTime() );
}

void ioGuildInfo::SetMyAttendRewardTimeByPacket( SP2Packet &rkPacket )
{
	// 보상 받기 가능 시간
	SYSTEMTIME rkServerTime;
	g_MyInfo.GetServerTime( rkServerTime );
	CTime ServerTime( rkServerTime.wYear, rkServerTime.wMonth, rkServerTime.wDay, rkServerTime.wHour, rkServerTime.wMinute, rkServerTime.wSecond );
	SetAttendRewardTime( ServerTime );

	//LOG.PrintTimeAndLog( 0, "%s Accept Guild Attend Reward! CurTime : %d, Enable Reward Time : %d ", __FUNCTION__, ServerTime.GetTime(), m_EnableRewardTime.GetTime() );
}

void ioGuildInfo::SetAttendRewardTime( CTime RecievedTime )
{
	//dwRecievedTime이 0인 경우 ( 로그인 했을 시 출석보상을 안받은 경우) or ( 길드 처음 생성 및 가입한 경우 )
	if ( RecievedTime.GetTime() == 0 )
	{
		if ( m_GuildCreatedTime.GetTime() == 0 )
			return;
		// 가입 날짜 체크 ( 가입 후 갱신 시간이 지나야 보상 받기 가능 )
		CTime EnableTimeByCreated = 0;
		int iYear = m_GuildCreatedTime.GetYear();
		int iMonth = m_GuildCreatedTime.GetMonth();
		int iDay = m_GuildCreatedTime.GetDay();
		int iHour = m_GuildCreatedTime.GetHour();
		//가입 날짜 시간과 갱신 시간 체크
		if ( iHour >= m_iAttendRenewalHour )
		{ 
			DateHelp::IncreaseDay( iYear, iMonth, iDay );
			EnableTimeByCreated = CTime( iYear, iMonth, iDay, m_iAttendRenewalHour, 1, 0 );
		}
		else
		{
			EnableTimeByCreated = CTime( iYear, iMonth, iDay, m_iAttendRenewalHour, 1, 0 );
		}

		CTimeSpan TimeGap = EnableTimeByCreated - m_LoginTime;
		LONGLONG iSpan = TimeGap.GetTimeSpan();
		if ( iSpan <= 0 )
			m_EnableRewardTime = 0;
		else
			m_EnableRewardTime = EnableTimeByCreated;
	}
	// 보상을 이미 받았다면 길드 생성날짜를 체크할 필요가 없기 때문에 다음 날짜로 갱신
	else
	{
		int iYear = RecievedTime.GetYear();
		int iMonth = RecievedTime.GetMonth();
		int iDay = RecievedTime.GetDay();
		int iHour = RecievedTime.GetHour();
		if ( iHour >= m_iAttendRenewalHour )
		{
			DateHelp::IncreaseDay( iYear, iMonth, iDay );
			m_EnableRewardTime = CTime( iYear, iMonth, iDay, m_iAttendRenewalHour, 1, 0 );
		}
		else
		{
			m_EnableRewardTime = CTime( iYear, iMonth, iDay, m_iAttendRenewalHour, 1, 0 );
		}
	}
}

void ioGuildInfo::SetMyGuildMemberTodayAttendInfoByPacket( SP2Packet &rkPacket )
{
	if ( m_dwGuildIndex == 0 )
	{
		LOG.PrintTimeAndLog( 0, "%s : MyGuildIndex is not exist ", __FUNCTION__ );
		return;
	}

	ioGuildData *pGuildData = GetGuildData( m_dwGuildIndex );
	if( !pGuildData )
		return;
	
	pGuildData->SetMyGuildMemberTodayAttendInfoByPacket( rkPacket );

	//자신이 출석 했는지 안했는지를 체크 한다.
	int iMaxUser = pGuildData->GetGuildJoinUser();
	for ( int i=0 ; i<iMaxUser ; ++i )
	{
		int	  iUserLevel;
		ioHashString szUserID;
		bool bTodayAttend,bYesterdayAttend;
		pGuildData->GetUserInfo( i, szUserID,iUserLevel, bYesterdayAttend, bTodayAttend );
		if ( szUserID == g_MyInfo.GetPublicID() )
		{
			//출석한 상태라면
			if ( bTodayAttend )
				SetAttendTime( m_LoginTime );
			else
				SetAttendTime( CTime( 0 ) );
		}
	}

	// Setting Member AttendInfo RequestTime
	SYSTEMTIME rkServerTime;
	g_MyInfo.GetServerTime( rkServerTime );
	CTime ServerTime( rkServerTime.wYear, rkServerTime.wMonth, rkServerTime.wDay, rkServerTime.wHour, rkServerTime.wMinute, rkServerTime.wSecond );

	int iYear = ServerTime.GetYear();
	int iMonth = ServerTime.GetMonth();
	int iDay = ServerTime.GetDay();
	int iHour = ServerTime.GetHour();
	if ( iHour >= m_iAttendRenewalHour )
	{
		DateHelp::IncreaseDay( iYear, iMonth, iDay );
		m_MemberAttendInfoRequestTime = CTime( iYear, iMonth, iDay, m_iAttendRenewalHour, 1, 0 );
	}
	else
		m_MemberAttendInfoRequestTime = CTime( iYear, iMonth, iDay, m_iAttendRenewalHour, 1, 0 );

	//LOG.PrintTimeAndLog( 0, "%s Enable Attend Time : %d, Request Time : %d.%d.%d.%d", __FUNCTION__, m_EnableAttendTime.GetTime(), iYear, iMonth, iDay, m_iAttendRenewalHour );
}

void ioGuildInfo::SetMyGuildMemberYesterdayAttendInfoByPacket( SP2Packet &rkPacket )
{
	if ( m_dwGuildIndex == 0 )
	{
		LOG.PrintTimeAndLog( 0, "%s : MyGuildIndex is not exist ", __FUNCTION__ );
		return;
	}

	ioGuildData *pGuildData = GetGuildData( m_dwGuildIndex );
	if( !pGuildData )
		return;

	pGuildData->SetMyGuildMemberYesterdayAttendInfoByPacket( rkPacket );
}

void ioGuildInfo::SetMyGuildAttendedUserInfoByPacket( SP2Packet &rkPacket, bool& bIsMe )
{
	bIsMe = false;
	if ( m_dwGuildIndex == 0 )
	{
		LOG.PrintTimeAndLog( 0, "%s : MyGuildIndex is not exist ", __FUNCTION__ );
		return;
	}
	DWORD dwUserIndex;
	rkPacket >> dwUserIndex;
	ioGuildData *pGuildData = GetGuildData( m_dwGuildIndex );
	if( pGuildData )
	{
		pGuildData->SetGuildTodayAttendedUser( dwUserIndex );

		//출석한 유저가 나라면!
		if ( pGuildData->GetUserIndex( g_MyInfo.GetPublicID() ) == dwUserIndex ) 
		{
			//출석 시간 갱신.
			SYSTEMTIME rkServerTime;
			g_MyInfo.GetServerTime( rkServerTime );
			CTime ServerTime( rkServerTime.wYear, rkServerTime.wMonth, rkServerTime.wDay, rkServerTime.wHour, rkServerTime.wMinute, rkServerTime.wSecond );
			SetAttendTime( ServerTime );
			bIsMe = true;
			//LOG.PrintTimeAndLog( 0, "%s : Accept Guild Attend! Time : %d, Enable AttendTime : %d", __FUNCTION__, ServerTime.GetTime(), m_EnableAttendTime.GetTime() );
		}
	}
}

void ioGuildInfo::SetAttendTime( CTime AttendedTime )
{
	// dwRecievedTime이 0인 경우 ( 로그인 했을 시 출석보상을 안받은 경우) or ( 길드 처음 생성 및 가입한 경우 )
	if ( AttendedTime.GetTime() == 0 )
	{
		if ( m_GuildCreatedTime.GetTime() == 0 )
			return;
		// 가입 날짜 체크 ( 가입 후 갱신 시간이 지나야 출석체크 가능 )
		CTime EnableTimeByCreated = 0;
		int iYear = m_GuildCreatedTime.GetYear();
		int iMonth = m_GuildCreatedTime.GetMonth();
		int iDay = m_GuildCreatedTime.GetDay();
		int iHour = m_GuildCreatedTime.GetHour();
		//가입 날짜 시간과 갱신 시간 체크
		if ( iHour >= m_iAttendRenewalHour )
		{
			DateHelp::IncreaseDay( iYear, iMonth, iDay );
			EnableTimeByCreated = CTime( iYear, iMonth, iDay, m_iAttendRenewalHour, 1, 0 );
		}
		else
		{
			EnableTimeByCreated = CTime( iYear, iMonth, iDay, m_iAttendRenewalHour, 1, 0 );
		}

		CTimeSpan TimeGap = EnableTimeByCreated - m_LoginTime;
		LONGLONG iSpan = TimeGap.GetTimeSpan();
		if ( iSpan <= 0 )
			m_EnableAttendTime = 0;
		else
			m_EnableAttendTime = EnableTimeByCreated;
	}
	//출석을 이미 했다면 길드 생성날짜를 체크할 필요가 없기 때문에 다음 날짜로 갱신
	else
	{
		int iYear = AttendedTime.GetYear();
		int iMonth = AttendedTime.GetMonth();
		int iDay = AttendedTime.GetDay();
		int iHour = AttendedTime.GetHour();
		if ( iHour >= m_iAttendRenewalHour )
		{
			DateHelp::IncreaseDay( iYear, iMonth, iDay );
			m_EnableAttendTime = CTime( iYear, iMonth, iDay, m_iAttendRenewalHour, 1, 0 );
		}
		else
		{
			m_EnableAttendTime = CTime( iYear, iMonth, iDay, m_iAttendRenewalHour, 1, 0 );
		}
	}
}

//길드 생성시간과 현재시간 체크
bool ioGuildInfo::CheckGuildCreatedTime()
{
	if ( m_GuildCreatedTime.GetTime() == 0 )
		return false;
	CTime EnableTimeByCreated = 0;
	int iYear = m_GuildCreatedTime.GetYear();
	int iMonth = m_GuildCreatedTime.GetMonth();
	int iDay = m_GuildCreatedTime.GetDay();
	int iHour = m_GuildCreatedTime.GetHour();
	//가입 날짜 시간과 갱신 시간 체크
	if ( iHour >= m_iAttendRenewalHour )
	{
		DateHelp::IncreaseDay( iYear, iMonth, iDay );
		EnableTimeByCreated = CTime( iYear, iMonth, iDay, m_iAttendRenewalHour, 1, 0 );
	}
	else
		EnableTimeByCreated = CTime( iYear, iMonth, iDay, m_iAttendRenewalHour, 1, 0 );

	SYSTEMTIME rkServerTime;
	g_MyInfo.GetServerTime( rkServerTime );
	CTime ServerTime( rkServerTime.wYear, rkServerTime.wMonth, rkServerTime.wDay, rkServerTime.wHour, rkServerTime.wMinute, rkServerTime.wSecond );
	CTimeSpan TimeGap = ServerTime - EnableTimeByCreated;
	LONGLONG iSpan = TimeGap.GetTimeSpan();
	if ( iSpan <= 0 )
		return true;

	return false;
}

//길드 출석 가능 시간 체크
bool ioGuildInfo::CheckEnableAttendTime()
{
	SYSTEMTIME rkServerTime;
	g_MyInfo.GetServerTime( rkServerTime );
	CTime ServerTime( rkServerTime.wYear, rkServerTime.wMonth, rkServerTime.wDay, rkServerTime.wHour, rkServerTime.wMinute, rkServerTime.wSecond );
	//다음 갱신 시간 - 현재 시간 <=0 이면 갱신 요청
	CTimeSpan TimeGap = m_EnableAttendTime - ServerTime;
	LONGLONG iSpan = TimeGap.GetTimeSpan();
	if ( iSpan <= 0 )
		return true;

	return false;
}

//길드 출석 보상 가능 시간 체크
bool ioGuildInfo::CheckEnableAttendRewardTime()
{
	SYSTEMTIME rkServerTime;
	g_MyInfo.GetServerTime( rkServerTime );
	CTime ServerTime( rkServerTime.wYear, rkServerTime.wMonth, rkServerTime.wDay, rkServerTime.wHour, rkServerTime.wMinute, rkServerTime.wSecond );
	//다음 갱신 시간 - 현재 시간 <=0 이면 갱신 요청
	CTimeSpan TimeGap = m_EnableRewardTime - ServerTime;
	LONGLONG iSpan = TimeGap.GetTimeSpan();
	if ( iSpan <= 0 )
		return true;

	return false;
}

// 길드 출석 데이트 갱신 요청 시간 체크
bool ioGuildInfo::CheckMemberAttendInfoRequestTime()
{
	if ( m_MemberAttendInfoRequestTime.GetTime() <= 0 )
		return false;
	SYSTEMTIME rkServerTime;
	g_MyInfo.GetServerTime( rkServerTime );
	CTime ServerTime( rkServerTime.wYear, rkServerTime.wMonth, rkServerTime.wDay, rkServerTime.wHour, rkServerTime.wMinute, rkServerTime.wSecond );
	//다음 갱신 시간 - 현재 시간 <=0 이면 갱신 요청
	CTimeSpan TimeGap = m_MemberAttendInfoRequestTime - ServerTime;
	LONGLONG iSpan = TimeGap.GetTimeSpan();
	if ( iSpan <= 0 )
		return true;

	return false;
}