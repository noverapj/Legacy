#include "stdafx.h"

#include "UserNodeManager.h"
#include "UserCopyNode.h"

UserCopyNode::UserCopyNode()
{
	InitData();
}

UserCopyNode::~UserCopyNode()
{
}

void UserCopyNode::OnCreate( ServerNode *pCreator )
{
	CopyNodeParent::OnCreate( pCreator );
	m_eCopyType = USER_TYPE;
	InitData();
}

void UserCopyNode::OnDestroy()
{
	CopyNodeParent::OnDestroy();
}

void UserCopyNode::InitData()
{
	// 기본 정보
	m_dwUserIndex	= 0;
	m_dwDBAgentID   = 0;
	m_iCampType     = 0;
	m_szPrivateID.c_str();
	m_szPublicID.c_str();
	m_iGradeLevel	= 0;
	m_iUserPos		= 0;
	m_iKillDeathLevel = 0;
	m_iLadderPoint    = 0;
	m_bSafetyLevel = false;
	m_eModeType  = MT_NONE;
	m_bDeveloper = false;
	m_iUserRank  = 0;
	m_dwPingStep = 0;
	m_dwGuildIndex = 0;
	m_dwGuildMark  = 0;
	m_vBestFriend.clear();

	m_bShuffleGlobalSearch = false;
	m_dwLadderMatchTime = 0;

	m_iBattleMode_Order = BATTLE_ORDER_RANDOM ;		// 2019-02-14 by bckim, 배틀 모드 추가

}

bool UserCopyNode::RelayPacket( SP2Packet &rkPacket )
{
	if( m_dwUserIndex == 0 )
		return false;
	// 날중계를 위해 인덱스를 패킷에 넣어 가공한다.
	SP2Packet kPacket( m_dwUserIndex, rkPacket );
	//
	return CopyNodeParent::SendMessage( kPacket );
}

void UserCopyNode::ApplySyncCreate( SP2Packet &rkPacket )
{
	rkPacket >> m_dwDBAgentID >> m_szPrivateID >> m_szPublicID >> m_iCampType >> m_iUserRank;
	rkPacket >> m_iGradeLevel >> m_iUserPos >> m_iKillDeathLevel >> m_iLadderPoint >> m_bSafetyLevel >> m_dwGuildIndex >> m_dwGuildMark;

	m_vBestFriend.clear();
	int iMaxBestFriend;
	rkPacket >> iMaxBestFriend;
	for(int i = 0;i < iMaxBestFriend;i++)
	{
		DWORD dwBestFriendIndex;
		rkPacket >> dwBestFriendIndex;
		m_vBestFriend.push_back( dwBestFriendIndex );
	}

	m_bDeveloper = g_UserNodeManager.IsDeveloper( m_szPublicID.c_str() );
}

void UserCopyNode::ApplySyncUpdate( SP2Packet &rkPacket )
{
	int iModeType;
	rkPacket >> m_iGradeLevel >> m_iUserPos >> m_dwPingStep >> m_iKillDeathLevel >> m_iLadderPoint >> m_bSafetyLevel >> iModeType;
	m_eModeType = (ModeType)iModeType;
}

void UserCopyNode::ApplySyncPos( SP2Packet &rkPacket )
{
	int iModeType;
	rkPacket >> m_iUserPos >> m_dwPingStep >> iModeType;
	m_eModeType = (ModeType)iModeType;
}

void UserCopyNode::ApplySyncGuild( SP2Packet &rkPacket )
{
	rkPacket >> m_dwGuildIndex >> m_dwGuildMark;
}

void UserCopyNode::ApplySyncCamp( SP2Packet &rkPacket )
{
	rkPacket >> m_iCampType;
}

void UserCopyNode::ApplySyncPublicID( SP2Packet &rkPacket )
{
	rkPacket >> m_szPublicID;
	//LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "%s (%d:%s)", __FUNCTION__, m_dwUserIndex, m_szPublicID );
}

void UserCopyNode::ApplySyncBestFriend( SP2Packet &rkPacket )
{
	m_vBestFriend.clear();
	int iMaxBestFriend;
	rkPacket >> iMaxBestFriend;
	for(int i = 0;i < iMaxBestFriend;i++)
	{
		DWORD dwBestFriendIndex;
		rkPacket >> dwBestFriendIndex;
		m_vBestFriend.push_back( dwBestFriendIndex );
	}
}

void UserCopyNode::ApplyUserNode( UserParent *pUser )
{
	if( pUser == NULL ) return;

	m_dwDBAgentID = pUser->GetUserDBAgentID();
	m_szPrivateID = pUser->GetPrivateID();
	m_szPublicID  = pUser->GetPublicID();
	m_iCampType   = pUser->GetUserCampPos();
	m_iGradeLevel = pUser->GetGradeLevel();
	m_iUserPos    = pUser->GetUserPos();
	m_iKillDeathLevel  = pUser->GetKillDeathLevel();
	m_iLadderPoint= pUser->GetLadderPoint();
	m_bSafetyLevel= pUser->IsSafetyLevel();
	m_iUserRank   = pUser->GetUserRanking();
	m_dwPingStep  = pUser->GetPingStep();
	m_dwGuildIndex= pUser->GetGuildIndex();
	m_dwGuildMark = pUser->GetGuildMark();
	pUser->GetBestFriend( m_vBestFriend );	
}

void UserCopyNode::ApplySyncShuffle( SP2Packet &rkPacket )
{
	rkPacket >> m_bShuffleGlobalSearch;
}

bool  UserCopyNode::IsGuild()
{
	if( m_dwGuildIndex == 0 )
		return false;
	return true;
}

DWORD UserCopyNode::GetGuildIndex()
{
	if( IsGuild() )
		return m_dwGuildIndex;
	return 0;
}

DWORD UserCopyNode::GetGuildMark()
{
	if( IsGuild() )
		return m_dwGuildMark;
	return 0;
}

bool UserCopyNode::IsBestFriend( DWORD dwUserIndex )
{
	for(int i = 0;i < (int)m_vBestFriend.size();i++)
	{
		if( dwUserIndex == m_vBestFriend[i] )
			return true;
	}
	return false;
}

void UserCopyNode::GetBestFriend( DWORDVec &rkUserIndexList )
{
	rkUserIndexList.clear();
	for(int i = 0;i < (int)m_vBestFriend.size();i++)
	{
		rkUserIndexList.push_back( m_vBestFriend[i] );
	}
}

bool UserCopyNode::IsShuffleGlboalSearch()
{
	return m_bShuffleGlobalSearch; 
}