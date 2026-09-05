#include "stdafx.h"

#include "LevelMatchManager.h"
#include "LadderTeamManager.h"
#include "LadderTeamCopyNode.h"
#include "ServerNode.h"
#include "ioMyLevelMgr.h"
#include "../Local/ioLocalParent.h"

LadderTeamCopyNode::LadderTeamCopyNode()
{
	InitData();
}

LadderTeamCopyNode::~LadderTeamCopyNode()
{
}

void LadderTeamCopyNode::OnCreate( ServerNode *pCreator )
{
	CopyNodeParent::OnCreate( pCreator );
	m_eCopyType = LADDERTEAM_TYPE;
	InitData();
}

void LadderTeamCopyNode::OnDestroy()
{
	CopyNodeParent::OnDestroy();
}

void LadderTeamCopyNode::ApplySyncChangeInfo( SP2Packet &rkPacket )
{	
	int iPrevState = m_dwTeamState;

	rkPacket >> m_szTeamName >> m_szTeamPW >> m_iCurPlayer >> m_iMaxPlayer >> m_iAverageMatchLevel >> m_iHeroMatchPoint;
	rkPacket >> m_dwJoinGuildIndex >> m_bSearchLevelMatch >> m_bSearchSameUser >> m_bBadPingKick >> m_dwTeamState >> m_dwGuildIndex >> m_dwGuildMark >> m_iSelectMode >> m_iSelectMap >> m_iSearchCount;

	if( iPrevState != TMS_SEARCH_PROCEED && TMS_SEARCH_PROCEED == m_dwTeamState )
		g_LadderTeamManager.AddCopyNodeSearchingList(GetIndex(), IsHeroMatchMode());
	else if( TMS_SEARCH_PROCEED == iPrevState && m_dwTeamState != TMS_SEARCH_PROCEED )
		g_LadderTeamManager.RemoveCopyNodeSearchingList(GetIndex(), IsHeroMatchMode());
}

void LadderTeamCopyNode::ApplySyncChangeRecord( SP2Packet &rkPacket )
{
	rkPacket >> m_szTeamName >> m_szTeamPW >> m_iCurPlayer >> m_iMaxPlayer >> m_iAverageMatchLevel >> m_iHeroMatchPoint;
	rkPacket >> m_dwJoinGuildIndex >> m_bSearchLevelMatch >> m_bSearchSameUser >> m_bBadPingKick >> m_dwTeamState >> m_dwGuildIndex >> m_dwGuildMark >> m_iSelectMode >> m_iSelectMap;
	rkPacket >> m_iWinRecord >> m_iLoseRecord >> m_iVictoriesRecord;

	// 랭킹 정렬
	g_LadderTeamManager.SortLadderTeamRank( IsHeroMatchMode() );
}

void LadderTeamCopyNode::ApplySyncCreate( SP2Packet &rkPacket )
{
	rkPacket >> m_szTeamName >> m_szTeamPW >> m_iCurPlayer >> m_iMaxPlayer >> m_iAverageMatchLevel >> m_iHeroMatchPoint;
	rkPacket >> m_dwJoinGuildIndex >> m_bSearchLevelMatch >> m_bSearchSameUser >> m_bBadPingKick >> m_dwTeamState >> m_dwGuildIndex >> m_dwGuildMark >> m_iSelectMode >> m_iSelectMap;
	rkPacket >> m_iWinRecord >> m_iLoseRecord >> m_iVictoriesRecord;
	
	// Create 때만 전송해주는 정보. 
	rkPacket >> m_iCampType >> m_bHeroMatchMode;         
}

void LadderTeamCopyNode::InitData()
{
	m_dwIndex = 0;
	m_iCampType = 0;
	m_szTeamName.Clear();
	m_szTeamPW.Clear();
	m_dwGuildIndex= 0;
	m_dwGuildMark = 0;
	m_iSelectMode = -1;
	m_iSelectMap  = -1;
	m_iMaxPlayer  = 0;
	m_iCurPlayer  = 0;
	m_iAverageMatchLevel = 0;
	m_iHeroMatchPoint = 0;
	m_iWinRecord  = 0;
	m_iLoseRecord = 0;
	m_iVictoriesRecord = 0;
	m_dwJoinGuildIndex = 0;
	m_bSearchLevelMatch = false;
	m_bSearchSameUser  = false;
	m_bHeroMatchMode   = false;
	m_bBadPingKick     = true;
	if( ioLocalManager::GetSingletonPtr() )
	{
		ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
		if( pLocal && !pLocal->IsBadPingKick( true ) )
			m_bBadPingKick = false;
	}
	m_iTeamRanking     = -1;
	m_dwTeamState = TMS_READY;
	m_iSearchCount = 0;
}

void LadderTeamCopyNode::EnterUser( const DWORD dwUserIndex, const ioHashString &szPublicID, const int iGradeLevel, const int iAbilityLevel, const int iHeroMatchPoint, const int iLadderPoint, const DWORD dwGuildIndex, const DWORD dwGuildMark,
								   const ioHashString &szPublicIP, const ioHashString &szPrivateIP, const ioHashString &szTransferIP, const int iClientPort, const int iTransferPort  )
{
	SP2Packet kPacket( SSTPK_LADDERTEAM_TRANSFER );
	kPacket << LadderTeamParent::ENTER_USER << GetIndex();
	kPacket << dwUserIndex << szPublicID << iGradeLevel << iAbilityLevel << iHeroMatchPoint << iLadderPoint << dwGuildIndex << dwGuildMark
		    << szPublicIP << szPrivateIP << szTransferIP << iClientPort << iTransferPort;
	LadderTeamCopyNode::SendMessage( kPacket );
}

bool LadderTeamCopyNode::LeaveUser( const DWORD dwUserIndex, const ioHashString &szPublicID )
{	
	SP2Packet kPacket( SSTPK_LADDERTEAM_TRANSFER ); //kyg 추후 한번에 수정 

	kPacket << LadderTeamParent::LEAVE_USER << GetIndex();
	kPacket << dwUserIndex << szPublicID;

	LadderTeamCopyNode::SendMessage( kPacket );
	return true;
}

void LadderTeamCopyNode::SendPacketTcp( SP2Packet &rkPacket, DWORD dwUserIndex )
{
	SP2Packet kPacket( SSTPK_LADDERTEAM_TRANSFER );
	kPacket << LadderTeamParent::TRANSFER_PACKET << GetIndex() << dwUserIndex << rkPacket.GetPacketID();
	kPacket.SetDataAdd( (char*)rkPacket.GetData(), rkPacket.GetDataSize() );
	SendMessage( kPacket );
}

void LadderTeamCopyNode::SendPacketTcpUser( SP2Packet &rkPacket, const ioHashString &rkSenderName )
{
	SP2Packet kPacket( SSTPK_LADDERTEAM_TRANSFER );
	kPacket << LadderTeamParent::TRANSFER_PACKET_USER << GetIndex() << rkSenderName << rkPacket.GetPacketID();
	kPacket.SetDataAdd( (char*)rkPacket.GetData(), rkPacket.GetDataSize() );
	SendMessage( kPacket );
}

void LadderTeamCopyNode::SendPacketUdp( SP2Packet &rkPacket, DWORD dwUserIndex )
{
	SP2Packet kPacket( SSTPK_LADDERTEAM_TRANSFER );
	kPacket << LadderTeamParent::TRANSFER_PACKET_UDP << GetIndex() << dwUserIndex << rkPacket.GetPacketID();
	kPacket.SetDataAdd( (char*)rkPacket.GetData(), rkPacket.GetDataSize() );
	SendMessage( kPacket );
}

void LadderTeamCopyNode::UserInfoUpdate( const DWORD dwUserIndex, const int iGradeLevel, const int iAbilityLevel, const int iHeroMatchPoint, const int iLadderPoint, const DWORD dwGuildIndex, const DWORD dwGuildMark, const int iClientPort, const ioHashString &szTransferIP, const int iTransferPort  )
{
	SP2Packet kPacket( SSTPK_LADDERTEAM_TRANSFER );
	kPacket << LadderTeamParent::USER_INFO << GetIndex();
	kPacket << dwUserIndex << iGradeLevel << iAbilityLevel << iHeroMatchPoint << iLadderPoint << dwGuildIndex << dwGuildMark << iClientPort << szTransferIP << iTransferPort;
	LadderTeamCopyNode::SendMessage( kPacket );
}

void LadderTeamCopyNode::UserUDPChange( const DWORD dwUserIndex, const ioHashString &szPublicID, const ioHashString &szPublicIP, const int iClientPort, 
    								   const ioHashString &szPrivateIP, const ioHashString &szTransferIP, const int iTransferPort  )
{
	SP2Packet kPacket( SSTPK_LADDERTEAM_TRANSFER );
	kPacket << LadderTeamParent::UDP_CHANGE << GetIndex();
	kPacket << dwUserIndex << szPublicID << szPublicIP << iClientPort << szPrivateIP << szTransferIP << iTransferPort;
	LadderTeamCopyNode::SendMessage( kPacket );
}

void LadderTeamCopyNode::UpdateRecord( TeamType ePlayTeam, TeamType eWinTeam )
{
	if( eWinTeam == TEAM_NONE ) return;

	// 복사본이어도 일단 갱신하고 원본에 보낸다.이는 다른 팀들이 랭킹에 영향을 주기 때문이다.
	if( eWinTeam == ePlayTeam )
	{
		m_iWinRecord++;
		m_iVictoriesRecord++;		
	}
	else
	{
		m_iLoseRecord++;
		m_iVictoriesRecord = 0;		
	}

	//원본 객체에 전송
	SP2Packet kPacket( SSTPK_LADDERTEAM_TRANSFER );
	kPacket << LadderTeamParent::UPDATE_RECORD << GetIndex() << (int)ePlayTeam << (int)eWinTeam;
	LadderTeamCopyNode::SendMessage( kPacket );
}

void LadderTeamCopyNode::UpdateRanking( int iTeamRanking )
{
	// 복사본이어도 갱신하고 데이터를 믿는다.
	if( m_iTeamRanking != iTeamRanking )
	{
		m_iTeamRanking = iTeamRanking;
	}
}

void LadderTeamCopyNode::SetTeamState( DWORD dwState )
{
	//원본 객체에 전송
	SP2Packet kPacket( SSTPK_LADDERTEAM_TRANSFER );
	kPacket << LadderTeamParent::TEAM_STATE << GetIndex() << dwState;
	LadderTeamCopyNode::SendMessage( kPacket );
}

void LadderTeamCopyNode::MatchRoomRequest( DWORD dwRequestIndex )
{
	//원본 객체에 전송
	SP2Packet kPacket( SSTPK_LADDERTEAM_TRANSFER );
	kPacket << LadderTeamParent::MATCH_ROOM_REQUEST << GetIndex() << dwRequestIndex;
	LadderTeamCopyNode::SendMessage( kPacket );
}

void LadderTeamCopyNode::MatchRoomRequestJoin( DWORD dwOtherTeamIndex, ModeType eModeType, int iModeSubNum, int iModeMapNum, int iCampType, int iTeamType )
{
	//원본 객체에 전송
	SP2Packet kPacket( SSTPK_LADDERTEAM_TRANSFER );
	PACKET_GUARD_VOID( kPacket.Write(LadderTeamParent::MATCH_ROOM_REQUEST_JOIN) );
	PACKET_GUARD_VOID( kPacket.Write(GetIndex()) );
	PACKET_GUARD_VOID( kPacket.Write(dwOtherTeamIndex) );
	PACKET_GUARD_VOID( kPacket.Write((int)eModeType) );
	PACKET_GUARD_VOID( kPacket.Write(iModeSubNum) );
	PACKET_GUARD_VOID( kPacket.Write(iModeMapNum) );
	PACKET_GUARD_VOID( kPacket.Write(iCampType) );
	PACKET_GUARD_VOID( kPacket.Write(iTeamType) );

	LadderTeamCopyNode::SendMessage( kPacket );
}

void LadderTeamCopyNode::MatchReserveCancel()
{
	//원본 객체에 전송
	SP2Packet kPacket( SSTPK_LADDERTEAM_TRANSFER );
	kPacket << LadderTeamParent::MATCH_ROOM_RESERVE_CANCEL << GetIndex();
	LadderTeamCopyNode::SendMessage( kPacket );
}

void LadderTeamCopyNode::MatchPlayEndSync()
{
	//원본 객체에 전송
	SP2Packet kPacket( SSTPK_LADDERTEAM_TRANSFER );
	kPacket << LadderTeamParent::MATCH_ROOM_END_SYNC << GetIndex();
	LadderTeamCopyNode::SendMessage( kPacket );
}

bool LadderTeamCopyNode::OnProcessPacket( SP2Packet &rkPacket, UserParent *pUser )
{
	if( pUser == NULL )
		return false;

	switch( rkPacket.GetPacketID() )
	{
	case CTPK_LADDERTEAM_MACRO:
	case CTPK_LADDERTEAM_INVITE:
	case CTPK_VOICE_INFO:
		{
			SP2Packet kPacket( SSTPK_LADDERTEAM_TRANSFER );
			kPacket << LadderTeamParent::USER_PACKET_TRANSFER << GetIndex() << pUser->GetUserIndex() << rkPacket.GetPacketID();
			kPacket.SetDataAdd( (char*)rkPacket.GetData(), rkPacket.GetDataSize() );
			SendMessage( kPacket );
		}
		return true;
	}
	return false;
}

void LadderTeamCopyNode::OnLadderTeamInfo( UserParent *pUser )
{
	if( pUser == NULL ) return;

	SP2Packet kPacket( SSTPK_LADDERTEAM_TRANSFER );
	kPacket << LadderTeamParent::TEAM_INFO << GetIndex() << pUser->GetUserIndex();
	LadderTeamCopyNode::SendMessage( kPacket );
}

DWORD LadderTeamCopyNode::GetGuildIndex() const
{
	return m_dwGuildIndex;
}

DWORD LadderTeamCopyNode::GetGuildMark() const
{
	return m_dwGuildMark;
}

bool LadderTeamCopyNode::IsGuildTeam() const
{
	if( GetGuildIndex() == 0 ) return false;

	return true;
}

bool LadderTeamCopyNode::IsEmptyUser()
{
	if( m_iCurPlayer == 0 )
		return true;
	return false;
}

bool LadderTeamCopyNode::IsSearchLevelMatch()
{
	return m_bSearchLevelMatch;
}

bool LadderTeamCopyNode::IsSearchSameUser()
{
	return m_bSearchSameUser;
}

bool LadderTeamCopyNode::IsBadPingKick()
{
	return m_bBadPingKick;
}

bool LadderTeamCopyNode::IsSearching()
{
	if( GetTeamState() == TMS_SEARCH_PROCEED )
		return true;
	return false;
}

bool LadderTeamCopyNode::IsMatchPlay()
{
	if( GetTeamState() == TMS_MATCH_PLAY )
		return true;
	return false;
}

bool LadderTeamCopyNode::IsHeroMatchMode()
{
	return m_bHeroMatchMode;
}

bool LadderTeamCopyNode::IsFull() const
{
	if( m_iCurPlayer >= m_iMaxPlayer )
		return true;
	return false;
}

int LadderTeamCopyNode::GetAbilityMatchLevel()
{
	int iLevel = 0;	
	if( IsHeroMatchMode() )
	{
		const int iDivide = (float)g_LevelMatchMgr.GetRoomEnterLevelMax() * 0.5f;
		if( iDivide == 0 )
			return 0;

		int iMatchPoint = GetHeroMatchPoint();
		int iAvgStep    = ( (float)g_LadderTeamManager.GetHeroMatchAveragePoint() / iDivide );
		if( iAvgStep == 0 )
			return 0;

		iLevel = (float)iMatchPoint / iAvgStep;
	}
	else
	{
		iLevel = m_iAverageMatchLevel;
	}
	return min( g_LevelMatchMgr.GetRoomEnterLevelMax() - 1, iLevel );
}

int LadderTeamCopyNode::GetHeroMatchPoint()
{
	return m_iHeroMatchPoint;
}

int LadderTeamCopyNode::GetTeamLevel()
{
	int iTeamLevel = GetAbilityMatchLevel() - g_LevelMatchMgr.GetAddGradeLevel();
	iTeamLevel = min( max( iTeamLevel, 0 ), g_LevelMgr.GetMaxGradeLevel() );

	return iTeamLevel;
}

int LadderTeamCopyNode::GetSelectMode() const
{
	return m_iSelectMode;
}

int LadderTeamCopyNode::GetSelectMap() const
{
	return m_iSelectMap;
}

int LadderTeamCopyNode::GetRankingPoint()
{
	int iRankingPoint = 0;

	int iWinRecord = GetWinRecord()  + 1;
	int iLoseRecord= GetLoseRecord() + 1;
	int iWinPoint = ( iWinRecord * g_LadderTeamManager.GetLadderBattleWinPoint() ) * 1000;
	int iLosePoint= ( iLoseRecord* g_LadderTeamManager.GetLadderBattleLosePoint() ) * 1000;
	iRankingPoint = iRankingPoint - (float)( ( iWinPoint + iLosePoint ) * iWinRecord ) / ( iWinRecord + iLoseRecord );

	return iRankingPoint;
}