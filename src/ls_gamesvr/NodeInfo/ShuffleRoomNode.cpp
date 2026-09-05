#include "stdafx.h"

#include "../Local/ioLocalParent.h"
#include "../EtcHelpFunc.h"
#include "../MainProcess.h"
#include "../MainServerNode/MainServerNode.h"

#include "ShuffleRoomManager.h"
#include "UserNodeManager.h"
#include "ServerNodeManager.h"
#include "RoomNodeManager.h"
#include "LevelMatchManager.h"
#include "ShuffleRoomNode.h"
#include "ioMyLevelMgr.h"
#include "Mode.h"

extern CLog P2PRelayLOG;

//////////////////////////////////////////////////////////////////////////
ShuffleRoomSync::ShuffleRoomSync()
{
	m_pCreator     = NULL;
	m_dwUpdateTime = 0;
	m_dwUpdateType = 0;
	// 업데이트 시간
	m_dwCheckTime[BRS_PLAY]			= 4000;
	m_dwCheckTime[BRS_CHANGEINFO]	= 3000;
	m_dwCheckTime[BRS_CREATE]		= 500;
	m_dwCheckTime[BRS_DESTROY]		= 0;
}

ShuffleRoomSync::~ShuffleRoomSync()
{

}

void ShuffleRoomSync::Init()
{
	m_dwUpdateTime = 0;
	m_dwUpdateType = 0;
}

void ShuffleRoomSync::SetCreator( ShuffleRoomNode *pCreator )
{
	Init();
	m_pCreator = pCreator;
}

void ShuffleRoomSync::Update( DWORD dwUpdateType )
{
	if( !COMPARE( dwUpdateType, BRS_PLAY, MAX_BRS ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ShuffleRoomSync::Update 알수 없는 업데이트 값 : %d", dwUpdateType );
		return;
	}
	if( !m_pCreator )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ShuffleRoomSync::Update m_pCreator == NULL" );
		return;
	}

	if( dwUpdateType < m_dwUpdateType )
		return;

	m_dwUpdateType = dwUpdateType;
	m_dwUpdateTime = TIMEGETTIME();
	Process();
}

void ShuffleRoomSync::Process()
{
	if( m_dwUpdateTime == 0 ) return;
	if( !COMPARE( m_dwUpdateType, BRS_PLAY, MAX_BRS ) ) return;

	DWORD dwGap = TIMEGETTIME() - m_dwUpdateTime;
	if( dwGap >= m_dwCheckTime[m_dwUpdateType] )
	{
		switch( m_dwUpdateType )
		{
		case BRS_PLAY:
			m_pCreator->SyncPlay();
			break;
		case BRS_CHANGEINFO:
			m_pCreator->SyncChangeInfo();
			break;
		case BRS_CREATE:
			m_pCreator->SyncCreate();
			break;
		case BRS_DESTROY:
			m_pCreator->SyncDestroy();
			break;
		}
		m_dwUpdateType = m_dwUpdateTime = 0;
	}
}
//////////////////////////////////////////////////////////////////////////
ShuffleRoomNode::ShuffleRoomNode( DWORD dwIndex ) : m_dwIndex( dwIndex )
{
	InitData();
}

ShuffleRoomNode::~ShuffleRoomNode()
{
}

void ShuffleRoomNode::SyncPlay()
{
	SP2Packet kPacket( SSTPK_SHUFFLEROOM_SYNC );
	PACKET_GUARD_VOID( kPacket.Write( ShuffleRoomSync::BRS_PLAY ) );
	PACKET_GUARD_VOID( kPacket.Write( GetIndex() ) );
	PACKET_GUARD_VOID( kPacket.Write( GetPlayModeType() ) );

	PACKET_GUARD_VOID( kPacket.Write( m_iCurShufflePhase ) );
	PACKET_GUARD_VOID( kPacket.Write( IsBonusPhase() ) );
	
	g_ServerNodeManager.SendMessageToPartitions( kPacket );
}

void ShuffleRoomNode::SyncChangeInfo()
{
	SP2Packet kPacket( SSTPK_SHUFFLEROOM_SYNC );
	PACKET_GUARD_VOID( kPacket.Write( ShuffleRoomSync::BRS_CHANGEINFO ) );
	PACKET_GUARD_VOID( kPacket.Write( GetIndex() ) );
	PACKET_GUARD_VOID( kPacket.Write( GetPlayModeType() ) );

	PACKET_GUARD_VOID( kPacket.Write( m_szRoomName ) );
	PACKET_GUARD_VOID( kPacket.Write( m_OwnerUserID ) );
	PACKET_GUARD_VOID( kPacket.Write( (int)m_vUserNode.size() ) );
	PACKET_GUARD_VOID( kPacket.Write( GetPlayUserCnt() ) );
	PACKET_GUARD_VOID( kPacket.Write( m_iMaxPlayerBlue ) );
	PACKET_GUARD_VOID( kPacket.Write( m_iMaxPlayerRed ) );
	PACKET_GUARD_VOID( kPacket.Write( GetAbilityMatchLevel() ) );
	PACKET_GUARD_VOID( kPacket.Write( GetRoomLevel() ) );

	g_ServerNodeManager.SendMessageToPartitions( kPacket );
}

void ShuffleRoomNode::SyncCreate()
{
	SP2Packet kPacket( SSTPK_SHUFFLEROOM_SYNC );
	kPacket << ShuffleRoomSync::BRS_CREATE;
	FillSyncCreate( kPacket );
	g_ServerNodeManager.SendMessageToPartitions( kPacket );
}

void ShuffleRoomNode::FillSyncCreate( SP2Packet &rkPacket )
{
	PACKET_GUARD_VOID( rkPacket.Write( GetIndex() ) );
	PACKET_GUARD_VOID( rkPacket.Write( GetPlayModeType() ) );

	PACKET_GUARD_VOID( rkPacket.Write( m_szRoomName ) );
	PACKET_GUARD_VOID( rkPacket.Write( m_OwnerUserID ) );
	PACKET_GUARD_VOID( rkPacket.Write( (int)m_vUserNode.size() ) );
	PACKET_GUARD_VOID( rkPacket.Write( GetPlayUserCnt() ) );
	PACKET_GUARD_VOID( rkPacket.Write( m_iMaxPlayerBlue ) );
	PACKET_GUARD_VOID( rkPacket.Write( m_iMaxPlayerRed ) );
	PACKET_GUARD_VOID( rkPacket.Write( GetAbilityMatchLevel() ) );
	PACKET_GUARD_VOID( rkPacket.Write( GetRoomLevel() ) );
	PACKET_GUARD_VOID( rkPacket.Write( GetCreateTime() ) );
	PACKET_GUARD_VOID( rkPacket.Write( m_iCurShufflePhase ) );
	PACKET_GUARD_VOID( rkPacket.Write( IsBonusPhase() ) );
}

void ShuffleRoomNode::SyncDestroy()
{
	SP2Packet kPacket( SSTPK_SHUFFLEROOM_SYNC );
	PACKET_GUARD_VOID( kPacket.Write( ShuffleRoomSync::BRS_DESTROY ) );
	PACKET_GUARD_VOID( kPacket.Write( GetIndex() ) );
	g_ServerNodeManager.SendMessageToPartitions( kPacket );
}

void ShuffleRoomNode::SyncRealTimeCreate()
{
	SyncCreate();
	m_NodeSync.Init();
}

void ShuffleRoomNode::InitData()
{
	m_dwReserveTime = 0;
	m_vUserNode.clear();

	char szBuf[MAX_PATH] = "";
	wsprintf( szBuf, "셔플룸%d", GetIndex() );
	m_szRoomName = szBuf;
	m_OwnerUserID.Clear();
	m_iMaxPlayerBlue = g_ShuffleRoomManager.GetMaxPlayer() / 2;
	m_iMaxPlayerRed  = g_ShuffleRoomManager.GetMaxPlayer() / 2;
	m_NodeSync.SetCreator( this );
	m_pShuffleRoom = NULL;

	/*if( ioLocalManager::GetSingletonPtr() )
	{
		ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
		if( pLocal && !pLocal->IsBadPingKick() )
			m_bBadPingKick = false;
	}*/

	m_szBossName.Clear();
	m_szGangsiName.Clear();

	InitRecord();
}

void ShuffleRoomNode::InitRecord()
{
	m_iBlueWin			= 0;
	m_iBlueLose			= 0;
	m_iBlueVictories	= 0;
	m_iRedWin			= 0;
	m_iRedLose			= 0;
	m_iRedVictories		= 0;
}

void ShuffleRoomNode::InitShuffleMode()
{
	vShuffleRoomInfo vShuffleRoomInfo;
	g_ShuffleRoomManager.GetShuffleModeList( vShuffleRoomInfo );

	int iLimit = vShuffleRoomInfo.size();

	m_vShuffleModeList.clear();	
	for( int i = 0; i < iLimit; ++i )
	{
		m_vShuffleModeList.push_back( vShuffleRoomInfo[i] );
	}
	
	//보너스 모드 제거
	ShuffleRoomInfo kInfo;
	g_ShuffleRoomManager.GetShuffleBonusModeSubInfo( kInfo );

	if( kInfo.m_iModeIdx != -1 )
		m_vShuffleModeList.push_back( kInfo );

	m_iMaxShufflePhase = g_ShuffleRoomManager.GetMaxPhase();
	m_iCurShufflePhase = 0;

	m_bPhaseEnd = false;

	InitShuffleRecord();
}

void ShuffleRoomNode::OnCreate()
{
	InitData();
	InitShuffleMode();
	m_dwCreateTime = TIMEGETTIME();
	m_dwReserveTime = m_dwCreateTime;

	m_NodeSync.Update( ShuffleRoomSync::BRS_CREATE );

	LOG.PrintTimeAndLog( LOG_SHUFFLE, "%s : %d번 방 생성.", __FUNCTION__, GetIndex() );
}

void ShuffleRoomNode::OnDestroy()
{
	BlockNode::Reset();

	InitData();
	m_NodeSync.Update( ShuffleRoomSync::BRS_DESTROY );
	LOG.PrintTimeAndLog( LOG_SHUFFLE, "%s : %d번 방 해제.", __FUNCTION__, GetIndex() );
}

void ShuffleRoomNode::EnterUser( const DWORD dwUserIndex, const ioHashString &szPublicID, const int iGradeLevel, const int iAbilityLevel,
							     const ioHashString &szPublicIP, const ioHashString &szPrivateIP, const ioHashString &szTransferIP, const int iClientPort, const int iTransferPort  )
{
	// 예외 처리 : 동일한 유저가 두번 입장 가능성이 있으므로
	// 입장할 때 이미 입장되어있는 아이디는 삭제한다.
	if( RemoveUser( dwUserIndex ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ERROR!!! : ShuffleRoomNode::EnterUser(%d) : %d - %s - %s:%d", GetIndex(), dwUserIndex, szPublicID.c_str(), szTransferIP.c_str(), iTransferPort );
	}

	if( m_OwnerUserID.IsEmpty() )
	{
		m_OwnerUserID = szPublicID;
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ShuffleRoomNode::EnterUser(%d) - Owner(%s)", GetIndex(), szPublicID.c_str() );
	}

	// 전투룸에서 사용할 유저 정보
	ShuffleRoomUser kEnterUser;
	kEnterUser.m_dwUserIndex	= dwUserIndex;
	kEnterUser.m_szPublicID		= szPublicID;
	kEnterUser.m_iGradeLevel	= iGradeLevel;
	kEnterUser.m_iAbilityLevel	= iAbilityLevel;	
	kEnterUser.m_szPublicIP		= szPublicIP;
	kEnterUser.m_szPrivateIP	= szPrivateIP;
	kEnterUser.m_szTransferIP   = szTransferIP;
	kEnterUser.m_iClientPort	= iClientPort;
	kEnterUser.m_iTransferPort  = iTransferPort;
	kEnterUser.m_iServerRelayCount = 0;
	kEnterUser.m_eTeamType      = CreateTeamType();

	m_dwReserveTime = 0;
	AddUser( kEnterUser );

	UserParent *pUser = g_UserNodeManager.GetGlobalUserNode( dwUserIndex );
	if( pUser == NULL ) 
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ShuffleRoomNode::EnterUserToShuffleRoom(%d) 파티에 들어온 유저 없음 : %s(%d)", GetIndex(), szPublicID.c_str(), dwUserIndex );
		return;
	}

	//들어온 유저에게 파티 정보 전송.
	SP2Packet kPacket( STPK_SHUFFLEROOM_INFO );
	FillShuffleRoomInfo( kPacket );
	pUser->RelayPacket( kPacket );

	//들어온 유저에게 모든 파티원 데이터(자신 포함) 전송.(유저 정보 전송할 때 파티 레벨도 같이 보낸다.)
	vShuffleRoomUser_iter iter, iEnd;
	iEnd = m_vUserNode.end();
	for( iter=m_vUserNode.begin() ; iter!=iEnd ; ++iter )
	{
		ShuffleRoomUser &kUser = *iter;
		SP2Packet kPacket1( STPK_SHUFFLEROOM_USER_INFO );
		{
			FillUserInfo( kPacket1, kUser );
		}
		pUser->RelayPacket( kPacket1 );
	}

	//모든 파티원에게 들어온 유저 데이터(자신 제외) 전송.
	SP2Packet kPacket2( STPK_SHUFFLEROOM_USER_INFO );
	FillUserInfo( kPacket2, kEnterUser );
	SendPacketTcp( kPacket2, kEnterUser.m_dwUserIndex );

	// 방생성 즉시 전투 생성
	CRASH_GUARD();
	if( GetOwnerName() == kEnterUser.m_szPublicID )
	{
		SP2Packet kPacket3( STPK_SHUFFLEROOM_COMMAND );
		PACKET_GUARD_VOID( kPacket3.Write( SHUFFLEROOM_CREATE_OK ) );
		CreateShuffle( kPacket3 );
		SendPacketTcp( kPacket3 );		
	}

	// 참여 유저 승무패 기록 / 옵션  전송
	if( IsChangeEnterSyncData() )
	{
		SP2Packet kPacket4( STPK_SHUFFLEROOM_ENTERUSER_SYNC );
		FillRecord( kPacket4 );
		pUser->RelayPacket( kPacket4 );
	}

	SendShufflePhase( dynamic_cast<User*>( pUser ) );

	if( GetOwnerName() != kEnterUser.m_szPublicID )
	{
		// 중간 참여 유저의 대전 세팅.
		CheckShuffleJoin( kEnterUser );
	}

	m_NodeSync.Update( ShuffleRoomSync::BRS_CHANGEINFO );
}


bool ShuffleRoomNode::LeaveUser( const DWORD dwUserIndex, const ioHashString &szPublicID  )
{
	if( !RemoveUser( dwUserIndex ) )
		return false;

	//마지막 유저가 퇴장하면 파티 해제.
	if( m_vUserNode.empty() )
	{
		g_ShuffleRoomManager.RemoveShuffleRoom( this );
	}
	else
	{
		SP2Packet kPacket( STPK_SHUFFLEROOM_LEAVE );

		PACKET_GUARD_bool( kPacket.Write( szPublicID ) );
		PACKET_GUARD_bool( kPacket.Write( GetRoomLevel() ) );

		SendPacketTcp( kPacket );

		// 방장이 나가면 방장 교체
		CRASH_GUARD();
		if( szPublicID == m_OwnerUserID  )
			SelectNewOwner();

		if( m_pShuffleRoom )
			m_pShuffleRoom->LeaveReserveUser( dwUserIndex );
	}
	m_NodeSync.Update( ShuffleRoomSync::BRS_CHANGEINFO );
	return true;
}

bool ShuffleRoomNode::ReJoinTry( const DWORD dwUserIndex, const ioHashString &szPublicID )
{
	User* pUser = g_UserNodeManager.GetUserNode( dwUserIndex );
	if( !pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s - pUser == NULL", __FUNCTION__ );
		return false;
	}
	
	pUser->EnterShuffleRoom( this, false );
	
	return true;
}

void ShuffleRoomNode::UserInfoUpdate( const DWORD dwUserIndex, const int iGradeLevel, const int iAbilityLevel, const int iClientPort, const ioHashString &szTransferIP, const int iTransferPort  )
{
	vShuffleRoomUser_iter iter, iEnd;
	iEnd = m_vUserNode.end();
	for( iter=m_vUserNode.begin() ; iter!=iEnd ; ++iter )
	{
		ShuffleRoomUser &kUser = *iter;
		if( kUser.m_dwUserIndex == dwUserIndex )
		{
			kUser.m_iGradeLevel   = iGradeLevel;
			kUser.m_iAbilityLevel = iAbilityLevel;
			kUser.m_szTransferIP  = szTransferIP;
			kUser.m_iClientPort   = iClientPort;
			kUser.m_iTransferPort = iTransferPort;
			return;
		}
	}
	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ShuffleRoomNode::UserInfoUpdate(%d) %d 유저 없음", GetIndex(), dwUserIndex );
}

void ShuffleRoomNode::UserUDPChange( const DWORD dwUserIndex, const ioHashString &szPublicID, const ioHashString &szPublicIP, const int iClientPort, 
								    const ioHashString &szPrivateIP, const ioHashString &szTransferIP, const int iTransferPort  )
{
	vShuffleRoomUser_iter iter, iEnd;
	iEnd = m_vUserNode.end();
	for( iter=m_vUserNode.begin() ; iter!=iEnd ; ++iter )
	{
		ShuffleRoomUser &kUser = *iter;
		if( kUser.m_dwUserIndex == dwUserIndex )
		{
			kUser.m_szTransferIP  = szTransferIP;
			kUser.m_iTransferPort = iTransferPort;
			kUser.m_iClientPort   = iClientPort;
			
			if( !IsShuffleModePlaying() ) // 룸에서 플레이 중이라면 이미 보냈다.
			{
				SP2Packet kPacket( STPK_CHANGE_UDP_INFO );
				PACKET_GUARD_VOID( kPacket.Write( szPublicID ) );
				PACKET_GUARD_VOID( kPacket.Write( szPublicIP ) );
				PACKET_GUARD_VOID( kPacket.Write( iClientPort ) );
				PACKET_GUARD_VOID( kPacket.Write( szPrivateIP ) );
				PACKET_GUARD_VOID( kPacket.Write( szTransferIP ) );
				PACKET_GUARD_VOID( kPacket.Write( iTransferPort ) );
				SendPacketTcp( kPacket, dwUserIndex );
			}			
			return;
		}
	}
}

void ShuffleRoomNode::UserP2PRelayInfo( const DWORD dwUserIndex, const DWORD dwRelayUserIndex, bool bRelay )
{
	ShuffleRoomUser &kRelayUser = GetUserNodeByIndex( dwRelayUserIndex );
	if( kRelayUser.m_szPublicID.IsEmpty() )
		return;

	ShuffleRoomUser &kUser = GetUserNodeByIndex( dwUserIndex );
	if( kUser.m_szPublicID.IsEmpty() )
		return;

	
	if( bRelay )
	{
		kUser.m_iServerRelayCount++;
	}
	else
	{
		kUser.m_iServerRelayCount--;				
	}
}


void ShuffleRoomNode::ShuffleEnterRandomTeam()
{
	// 팀을 다시 섞는다. 계급 순
	static vShuffleRoomUser vUserList;
	vUserList.clear();

	vShuffleRoomUser_iter iter, iEnd;
	iEnd = m_vUserNode.end();
	for( iter=m_vUserNode.begin() ; iter!=iEnd ; ++iter )
	{
		ShuffleRoomUser &kUser = *iter;
		vUserList.push_back( kUser );
	}

	std::sort( vUserList.begin(), vUserList.end(), ShuffleRoomUserSort() );
	int iBlueCount = m_iMaxPlayerBlue;
	int iRedCount  = m_iMaxPlayerRed;

	bool bSequenceValue = true;

	for(int i = 0;i < (int)vUserList.size();i++)
	{
		ShuffleRoomUser &kUser = vUserList[i];

		if( bSequenceValue && iBlueCount > 0 )
		{
			ChangeTeamType( kUser.m_szPublicID, TEAM_BLUE );
			iBlueCount--;
		}
		else if( iRedCount > 0 )
		{
			ChangeTeamType( kUser.m_szPublicID, TEAM_RED );
			iRedCount--;
		}
		else 
		{
			ChangeTeamType( kUser.m_szPublicID, TEAM_BLUE );
			iBlueCount--;
		}

		if( i % 2 == 0 )
			bSequenceValue = !bSequenceValue;
	}
	vUserList.clear();
}

void ShuffleRoomNode::ShuffleEnterRandomTeam( UserRankInfoList &rkUserRankInfoList )
{
	CRASH_GUARD();
	int i = 0;
	// 전투방에는 입장했지만 아직 룸에는 입장 못한 유저를 포함시킨다.
	// 서버이동중에 팀섞기가 완료되면 팀섞기에 해당 안되는 유저는 최초의 팀을 유지하므로 인원 불균형 발생.
	vShuffleRoomUser_iter iter, iEnd;
	iEnd = m_vUserNode.end();
	for(iter = m_vUserNode.begin();iter!=iEnd;++iter)
	{
		ShuffleRoomUser &kUser = *iter;
		for(i = 0;i < (int)rkUserRankInfoList.size();i++)
		{
			if( rkUserRankInfoList[i].szName == kUser.m_szPublicID )
				break;
		}
		//
		if( i == (int)rkUserRankInfoList.size() )
		{
			UserRankInfo kUserRankInfo;
			kUserRankInfo.szName = kUser.m_szPublicID;
			kUserRankInfo.iRank  = (int)rkUserRankInfoList.size();
			rkUserRankInfoList.push_back( kUserRankInfo );
		}
	}

	int iUserRankCnt     = rkUserRankInfoList.size();
	int iBlueTeamCount   = GetMaxPlayer( TEAM_BLUE );
	int iRedTeamCount    = GetMaxPlayer( TEAM_RED );	

	bool bSequenceValue = true;

	int iCheckCnt = 0;
	for( i = 0; i < iUserRankCnt; i++ )
	{
		if( bSequenceValue && iBlueTeamCount > 0 )
		{
			ChangeTeamType( rkUserRankInfoList[i].szName, TEAM_BLUE );
			iBlueTeamCount--;
		}
		else if( iRedTeamCount > 0 )
		{
			ChangeTeamType( rkUserRankInfoList[i].szName, TEAM_RED );
			iRedTeamCount--;
		}
		else
		{
			ChangeTeamType( rkUserRankInfoList[i].szName, TEAM_BLUE );
			iBlueTeamCount--;
		}

		if( iCheckCnt % 2 == 0 )
		{
			bSequenceValue = !bSequenceValue;

		}

		iCheckCnt++;
	}
}

void ShuffleRoomNode::CreateShuffle( SP2Packet &rkPacket )
{	
	//
	rkPacket << GetJoinUserCnt();

	int iUserCount = 0;
	vShuffleRoomUser_iter iter, iEnd;
	iEnd = m_vUserNode.end();
	for(iter = m_vUserNode.begin();iter!=iEnd;++iter,++iUserCount)
	{
		ShuffleRoomUser &kUser = *iter;
		if( kUser.m_eTeamType == TEAM_NONE )
			kUser.m_eTeamType = CreateTeamType();

		PACKET_GUARD_VOID( rkPacket.Write( kUser.m_szPublicID ) );
		PACKET_GUARD_VOID( rkPacket.Write( (int)kUser.m_eTeamType ) );
	}
}

TeamType ShuffleRoomNode::CreateTeamType()
{
	int iBlueCnt = 0;
	int iRedCnt  = 0;
	vShuffleRoomUser_iter iter, iEnd;
	iEnd = m_vUserNode.end();
	for(iter = m_vUserNode.begin();iter!=iEnd;++iter)
	{
		ShuffleRoomUser &kUser = *iter;

		if( kUser.m_eTeamType == TEAM_BLUE )
			iBlueCnt++;
		else if( kUser.m_eTeamType == TEAM_RED )
			iRedCnt++;
	}

	if( iRedCnt >= iBlueCnt && iBlueCnt < m_iMaxPlayerBlue )
		return TEAM_BLUE;
	else if( iRedCnt < m_iMaxPlayerRed )
		return TEAM_RED;
    return TEAM_BLUE;
}

void ShuffleRoomNode::CheckShuffleJoin( ShuffleRoomUser &kCheckUser )
{
	UserParent *pUserParent = g_UserNodeManager.GetGlobalUserNode( kCheckUser.m_dwUserIndex );
	if( pUserParent == NULL ) 
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ShuffleRoomNode::CheckShuffleJoin 파티에 들어온 유저 없음(%d) : %s(%d)", GetIndex(), kCheckUser.m_szPublicID.c_str(), kCheckUser.m_dwUserIndex );		return;
	}

	if( IsShuffleModePlaying() )
	{
		// 플레이중이면 룸으로 이동시킨다.
		if( pUserParent->IsUserOriginal() )
		{
			User *pUser = (User*)pUserParent;
			// 룸 입장.
			SP2Packet kPacket1( STPK_MOVING_ROOM );
			PACKET_GUARD_VOID( kPacket1.Write( m_pShuffleRoom->GetModeType() ) );
			PACKET_GUARD_VOID( kPacket1.Write( m_pShuffleRoom->GetModeSubNum() ) );
			PACKET_GUARD_VOID( kPacket1.Write( m_pShuffleRoom->GetModeMapNum() ) );
			PACKET_GUARD_VOID( kPacket1.Write( (int)m_pShuffleRoom->GetPlazaModeType() ) );
			pUser->SendMessage( kPacket1 );

			pUser->EnterRoom( m_pShuffleRoom );
		}
		else
		{
			UserCopyNode *pUser = (UserCopyNode*)pUserParent;
			m_pShuffleRoom->EnterReserveUser( pUser->GetUserIndex() );
			SP2Packet kPacket( SSTPK_MOVING_ROOM_RESULT );
			PACKET_GUARD_VOID( kPacket.Write( SS_MOVING_ROOM_JOIN ) );
			PACKET_GUARD_VOID( kPacket.Write( SS_MOVING_ROOM_JOIN_BATTLE ) );
			PACKET_GUARD_VOID( kPacket.Write( (int)m_pShuffleRoom->GetModeType() ) );
			PACKET_GUARD_VOID( kPacket.Write(  m_pShuffleRoom->GetRoomIndex() ) );
			PACKET_GUARD_VOID( kPacket.Write( pUser->GetUserIndex() ) );
			PACKET_GUARD_VOID( kPacket.Write( m_pShuffleRoom->GetModeSubNum() ) );
			PACKET_GUARD_VOID( kPacket.Write( m_pShuffleRoom->GetModeMapNum() ) );
			PACKET_GUARD_VOID( kPacket.Write( (int)m_pShuffleRoom->GetPlazaModeType() ) );
			PACKET_GUARD_VOID( kPacket.Write( m_pShuffleRoom->GetRoomNumber() ) );
			pUser->SendMessage( kPacket );
		}
	}
}

bool ShuffleRoomNode::IsShuffleTeamChangeOK( TeamType eChangeTeam )
{
	int iTeamCount = 0;
	vShuffleRoomUser_iter iter, iEnd;
	iEnd = m_vUserNode.end();
	for( iter=m_vUserNode.begin() ; iter!=iEnd ; ++iter )
	{
		ShuffleRoomUser &kUser = *iter;
		if( kUser.m_eTeamType == eChangeTeam )
			iTeamCount++;
	}

	if( eChangeTeam == TEAM_BLUE )
	{
		if( iTeamCount >= m_iMaxPlayerBlue )
			return false;
	}
	else if( eChangeTeam == TEAM_RED )
	{
		if( iTeamCount >= m_iMaxPlayerRed )
			return false;
	}
	else if( eChangeTeam == TEAM_NONE )
	{
		return false;
	}

	return true;
}

bool ShuffleRoomNode::IsShuffleModePlaying()
{
	if( m_pShuffleRoom && !m_pShuffleRoom->IsRoomEmpty() )
		return true;
	return false;
}

int ShuffleRoomNode::GetPlayModeType()
{
	if( !m_pShuffleRoom )
		return MT_NONE;

	return static_cast<int>( m_pShuffleRoom->GetModeType() );
}

void ShuffleRoomNode::AddUser(const ShuffleRoomUser &kUser )
{
	m_vUserNode.push_back( kUser );
}

bool ShuffleRoomNode::RemoveUser( const DWORD dwUserIndex )
{
    vShuffleRoomUser_iter iter, iEnd;
	iEnd = m_vUserNode.end();
	for(iter=m_vUserNode.begin();iter!=iEnd;++iter)
	{
		ShuffleRoomUser &kUser = *iter;
		if( kUser.m_dwUserIndex == dwUserIndex )
		{
			// P2P Relay Log
			P2PRelayLOG.PrintNoEnterLog( 0, "{[%d]<%s>(%d)+%d-}", GetIndex(), kUser.m_szPublicID.c_str(), kUser.m_iServerRelayCount, (int)kUser.IsNATUser() );
			m_vUserNode.erase( iter );			
			return true;
		}
	}
	return false;
}

void ShuffleRoomNode::SelectNewOwner()
{
	m_OwnerUserID.Clear();
	if( m_vUserNode.empty() )
		return;

	m_OwnerUserID = m_vUserNode[0].m_szPublicID;

	SP2Packet kPacket( STPK_SHUFFLEROOM_INFO );
	FillShuffleRoomInfo( kPacket );
	SendPacketTcp( kPacket );		
	m_NodeSync.Update( ShuffleRoomSync::BRS_CHANGEINFO );
}

bool ShuffleRoomNode::KickOutUser( const ioHashString &rkName, BYTE eKickType /* = RoomParent::RLT_NORMAL */ )
{
	CRASH_GUARD();
	vShuffleRoomUser_iter iter, iEnd;
	iEnd = m_vUserNode.end();
	for(iter=m_vUserNode.begin();iter!=iEnd;++iter)
	{
		ShuffleRoomUser &kUser = *iter;
		if( kUser.m_szPublicID == rkName )
		{
			SP2Packet kPacket( STPK_SHUFFLEROOM_COMMAND );
			PACKET_GUARD_BOOL( kPacket.Write( SHUFFLEROOM_MACRO_KICK_OUT ) );
			PACKET_GUARD_BOOL( kPacket.Write( kUser.m_szPublicID ) );
			SendPacketTcpUser( kPacket, kUser.m_szPublicID );

			UserParent *pUserParent = g_UserNodeManager.GetGlobalUserNode( rkName );
			if( pUserParent )
			{
				if( pUserParent->IsUserOriginal() )
				{
					User *pUser = (User*)pUserParent;
					pUser->ShuffleRoomKickOut( eKickType );
				}            
				else
				{
					UserCopyNode *pUser = (UserCopyNode*)pUserParent;
					SP2Packet kPacket( SSTPK_SHUFFLEROOM_KICK_OUT );
					PACKET_GUARD_BOOL( kPacket.Write( eKickType ) );
					PACKET_GUARD_BOOL( kPacket.Write( kUser.m_dwUserIndex ) );
					pUser->SendMessage( kPacket );
				}
			}    
			return true;
		}
	}
	return false;
}

int ShuffleRoomNode::GetJoinUserCnt() const
{
	return m_vUserNode.size();
}

int ShuffleRoomNode::GetPlayUserCnt()
{
	int iPlayUserCnt = 0;
	vShuffleRoomUser_iter iter, iEnd;
	iEnd = m_vUserNode.end();
	for( iter = m_vUserNode.begin() ; iter != iEnd; ++iter )
	{
		ShuffleRoomUser &kUser = *iter;

		iPlayUserCnt++;
	}

	return iPlayUserCnt;
}

int ShuffleRoomNode::GetMaxPlayer() const
{
	return m_iMaxPlayerBlue + m_iMaxPlayerRed;
}

int ShuffleRoomNode::GetMaxPlayerBlue() const
{
	return m_iMaxPlayerBlue;
}

int ShuffleRoomNode::GetMaxPlayerRed() const
{
	return m_iMaxPlayerRed;
}

int ShuffleRoomNode::GetAbilityMatchLevel()
{
	int iSize  = 0;
	int iLevel = 0;

	vShuffleRoomUser_iter iter, iEnd;
	iEnd = m_vUserNode.end();

	for( iter = m_vUserNode.begin(); iter != iEnd; ++iter )
	{
		ShuffleRoomUser &kUser = *iter;
		// 옵저버 제외 레벨
		//if( !g_UserNodeManager.IsDeveloper( kUser.m_szPublicID.c_str() ) )
		{
			iSize++;
			iLevel += kUser.m_iAbilityLevel;
		}
	}

	iSize = max( iSize, 1 );
	iLevel /= iSize;
	iLevel = min( max( iLevel, 0 ), g_LevelMatchMgr.GetRoomEnterLevelMax() );
	return iLevel;
}

int ShuffleRoomNode::GetRoomLevel()
{
	int iAbility  = GetAbilityMatchLevel();
	int iAddGrade = g_LevelMatchMgr.GetAddGradeLevel();
	int iMatchLevel = iAbility - iAddGrade;
	iMatchLevel = min( max( iMatchLevel, 0 ), g_LevelMgr.GetMaxGradeLevel() );

	return iMatchLevel;
}

int ShuffleRoomNode::GetSortLevelPoint( int iMyLevel )
{
	int iSize = m_vUserNode.size();
	if( iSize == 0 )
		return 0;

	int iGapLevel = 5;
	int iLevel    = 0;
	vShuffleRoomUser_iter iter, iEnd;
	iEnd = m_vUserNode.end();
	for(iter=m_vUserNode.begin();iter!=iEnd;++iter)
	{
		ShuffleRoomUser &kUser = *iter;
		iLevel += kUser.m_iAbilityLevel;
	}
	iLevel /= iSize;
	return ( abs( iLevel - iMyLevel ) / iGapLevel );		
}

void ShuffleRoomNode::SyncPlayEnd( bool bAutoStart )
{
	SP2Packet kPacket( SSTPK_SHUFFLEROOM_SYNC );
	PACKET_GUARD_VOID( kPacket.Write( ShuffleRoomSync::BRS_PLAY ) );
	PACKET_GUARD_VOID( kPacket.Write( GetIndex() ) );
	PACKET_GUARD_VOID( kPacket.Write( MT_NONE ) );

	g_ServerNodeManager.SendMessageToPartitions( kPacket );
}

TeamType ShuffleRoomNode::ChangeTeamType( const ioHashString &rkName, TeamType eTeam )
{
	CRASH_GUARD();
	TeamType eCurTeam = TEAM_NONE;

	vShuffleRoomUser_iter iter, iEnd;
	iEnd = m_vUserNode.end();
	for(iter=m_vUserNode.begin();iter!=iEnd;++iter)
	{
		ShuffleRoomUser &kUser = *iter;
		if( kUser.m_szPublicID == rkName )
		{
			if( IsShuffleModePlaying() )        // 모드 플레이중인 룸이면 유저도 팀을 바꾼다.
			{
				if( m_pShuffleRoom->GetRoomStyle() == RSTYLE_SHUFFLEROOM && 
					m_pShuffleRoom->IsPartyProcessEnd() )
				{
					kUser.m_eTeamType = eTeam;

					UserParent *pUserParent = g_UserNodeManager.GetGlobalUserNode( kUser.m_szPublicID );
					if( pUserParent && pUserParent->IsUserOriginal() )
					{
						User *pUser = (User*)pUserParent;
						pUser->SetTeam( eTeam );
					}

					eCurTeam = eTeam;
				}
			}
			else
			{
				kUser.m_eTeamType = eTeam;
				eCurTeam = eTeam;
			}

			break;
		}
	}

	return eCurTeam;
}

void ShuffleRoomNode::FillShuffleRoomInfo( SP2Packet &rkPacket )
{
	PACKET_GUARD_VOID( rkPacket.Write( m_dwIndex ) );
	PACKET_GUARD_VOID( rkPacket.Write( m_szRoomName ) );
	PACKET_GUARD_VOID( rkPacket.Write( m_OwnerUserID ) );
	PACKET_GUARD_VOID( rkPacket.Write( GetJoinUserCnt() ) );
	PACKET_GUARD_VOID( rkPacket.Write( GetPlayUserCnt() ) );
	PACKET_GUARD_VOID( rkPacket.Write( m_iMaxPlayerBlue ) );
	PACKET_GUARD_VOID( rkPacket.Write( m_iMaxPlayerRed ) );
	PACKET_GUARD_VOID( rkPacket.Write( GetRoomLevel() ) );

	int iCurShufflePhase = m_iCurShufflePhase - 1;
	iCurShufflePhase = max( 0, iCurShufflePhase );
	PACKET_GUARD_VOID( rkPacket.Write( iCurShufflePhase ) );

	// 룸에 있는 경우와 룸에 없는 경우 클라이언트는 다른 처리를 한다. 
	// 룸에 있으면 룸으로 이동하고 룸에 없으면 로비의 전투방으로 이동한다.
	PACKET_GUARD_VOID( rkPacket.Write( IsShuffleModePlaying() ) );
}

void ShuffleRoomNode::FillUserInfo( SP2Packet &rkPacket, const ShuffleRoomUser &rkUser )
{
	PACKET_GUARD_VOID( rkPacket.Write( GetRoomLevel() ) );
	PACKET_GUARD_VOID( rkPacket.Write( rkUser.m_szPublicID ) );
	PACKET_GUARD_VOID( rkPacket.Write( rkUser.m_iGradeLevel ) );
	PACKET_GUARD_VOID( rkPacket.Write( rkUser.m_iAbilityLevel ) );
	PACKET_GUARD_VOID( rkPacket.Write( (int)rkUser.m_eTeamType ) );
	PACKET_GUARD_VOID( rkPacket.Write( rkUser.m_dwUserIndex ) );
	PACKET_GUARD_VOID( rkPacket.Write( rkUser.m_szPublicIP ) );
	PACKET_GUARD_VOID( rkPacket.Write( rkUser.m_iClientPort ) );
	PACKET_GUARD_VOID( rkPacket.Write( rkUser.m_szPrivateIP ) );
	PACKET_GUARD_VOID( rkPacket.Write( rkUser.m_szTransferIP ) );
	PACKET_GUARD_VOID( rkPacket.Write( rkUser.m_iTransferPort ) );


	//길드 정보
	UserParent *pUserParent = g_UserNodeManager.GetGlobalUserNode( rkUser.m_dwUserIndex );
	if( pUserParent )
	{
		PACKET_GUARD_VOID( rkPacket.Write( pUserParent->GetGuildIndex() ) );
		PACKET_GUARD_VOID( rkPacket.Write( pUserParent->GetGuildMark() ) );
	}
	else
	{
		PACKET_GUARD_VOID( rkPacket.Write( 0 ) );
		PACKET_GUARD_VOID( rkPacket.Write( 0 ) );
	}
}

void ShuffleRoomNode::FillRecord( SP2Packet &rkPacket )
{
	PACKET_GUARD_VOID( rkPacket.Write( m_iBlueWin ) );
	PACKET_GUARD_VOID( rkPacket.Write( m_iBlueLose ) );
	PACKET_GUARD_VOID( rkPacket.Write( m_iBlueVictories ) );
	PACKET_GUARD_VOID( rkPacket.Write( m_iRedWin ) );
	PACKET_GUARD_VOID( rkPacket.Write( m_iRedLose ) );
	PACKET_GUARD_VOID( rkPacket.Write( m_iRedVictories ) );
}

bool ShuffleRoomNode::IsFull()
{
	int iPlayCnt = GetPlayUserCnt();
	if( iPlayCnt >= GetMaxPlayer() )
		return true;

	return false;
}

bool ShuffleRoomNode::IsEmptyShuffleRoom()
{
	return m_vUserNode.empty();
}

ShuffleRoomUser &ShuffleRoomNode::GetUserNodeByIndex( DWORD dwUserIndex )
{
	vShuffleRoomUser_iter iter, iEnd;
	iEnd = m_vUserNode.end();
	for(iter=m_vUserNode.begin();iter!=iEnd;++iter)
	{
		ShuffleRoomUser &kUser = *iter;
		if( kUser.m_dwUserIndex == dwUserIndex )
			return kUser;
	}

	static ShuffleRoomUser kReturn;
	return kReturn;
}

const ShuffleRoomUser &ShuffleRoomNode::GetUserNodeByArray( int iArray )
{
	if( COMPARE( iArray, 0, (int) m_vUserNode.size()) )
		return m_vUserNode[iArray];

	static ShuffleRoomUser kError;
	return kError;
}

UserParent *ShuffleRoomNode::GetUserNode( const ioHashString &szPublicID )
{
	CRASH_GUARD();
	vShuffleRoomUser_iter iter, iEnd;
	iEnd = m_vUserNode.end();
	for(iter=m_vUserNode.begin();iter!=iEnd;++iter)
	{
		ShuffleRoomUser &kUser = *iter;
		if( kUser.m_szPublicID == szPublicID )
			return g_UserNodeManager.GetGlobalUserNode( kUser.m_dwUserIndex );
	}
	return NULL;
}

TeamType ShuffleRoomNode::GetUserTeam( const ioHashString &szPublicID )
{
	CRASH_GUARD();
	vShuffleRoomUser_iter iter, iEnd;
	iEnd = m_vUserNode.end();
	for(iter=m_vUserNode.begin();iter!=iEnd;++iter)
	{
		ShuffleRoomUser &kUser = *iter;
		if( kUser.m_szPublicID == szPublicID )
			return kUser.m_eTeamType;
	}
	return TEAM_NONE;
}

int ShuffleRoomNode::GetUserTeamCount( TeamType eTeam )
{
	int iCount = 0;
	vShuffleRoomUser_iter iter, iEnd;
	iEnd = m_vUserNode.end();
	for(iter=m_vUserNode.begin();iter!=iEnd;++iter)
	{
		ShuffleRoomUser &kUser = *iter;

		if( eTeam != TEAM_NONE && kUser.m_eTeamType == eTeam )
			iCount++;
	}
	return iCount;
}

int ShuffleRoomNode::GetMaxPlayer( TeamType eTeam )
{
	if( eTeam == TEAM_BLUE )
		return m_iMaxPlayerBlue;
	return m_iMaxPlayerRed;
}

void ShuffleRoomNode::UpdateRecord( TeamType eWinTeam )
{
	if( eWinTeam == TEAM_BLUE )
	{
		m_iBlueWin++;
	    m_iBlueVictories++;

		m_iRedLose++;
		m_iRedVictories = 0;
	}
	else if( eWinTeam == TEAM_RED )
	{
		m_iRedWin++;
		m_iRedVictories++;

		m_iBlueLose++;
		m_iBlueVictories = 0;
	}
	else
	{
		m_iBlueVictories = 0;
		m_iRedVictories = 0;
	}
}

bool ShuffleRoomNode::IsChangeEnterSyncData()
{
	if( m_iBlueWin > 0 )
		return true;
	else if( m_iBlueLose > 0 )
		return true;
	else if( m_iBlueVictories > 0 )
		return true;
	else if( m_iRedWin > 0 )
		return true;
	else if( m_iRedLose > 0 )
		return true;
	else if( m_iRedVictories > 0 )
		return true;

	return false;
}

void ShuffleRoomNode::CreateBoss()
{
	ioHashStringVec vOriginalUserName;
	ioHashStringVec vCopyUserName;

	vShuffleRoomUser_iter iter;
	for(iter=m_vUserNode.begin();iter!=m_vUserNode.end();++iter)
	{
		ShuffleRoomUser &kUser = *iter;
		
		UserParent *pUser = g_UserNodeManager.GetGlobalUserNode( kUser.m_dwUserIndex );
		if( !pUser ) continue;
		
		if( pUser->IsUserOriginal() )
		{
			vOriginalUserName.push_back( pUser->GetPublicID() );
		}
		else
		{
			vCopyUserName.push_back( pUser->GetPublicID() );
		}
	}

	if( !vOriginalUserName.empty() )        
	{
		//동일 서버에 있는 유저중 랜덤
		int iSize = vOriginalUserName.size();
		int rSelect = rand()%iSize;
		m_szBossName = vOriginalUserName[rSelect];

		LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "ShuffleRoomNode::CreateBoss(%d) 동일 서버 - [%s]당첨!", GetIndex(), m_szBossName.c_str() );
	}
	else if( !vCopyUserName.empty() ) 
	{
		//타서버에 있는 유저중 랜덤
		int iSize = vCopyUserName.size();
		int rSelect = rand()%iSize;
		m_szBossName = vCopyUserName[rSelect];

		LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "ShuffleRoomNode::CreateBoss(%d) 타 서버 - [%s]당첨!", GetIndex(), m_szBossName.c_str() );
	}
	else
	{
		LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "ShuffleRoomNode::CreateBoss(%d) : 보스에 선택될 유저가 없다!!", GetIndex() );
	}
	vOriginalUserName.clear();
	vCopyUserName.clear();
}

const ioHashString &ShuffleRoomNode::GetBossName()
{
	return m_szBossName;
}

void ShuffleRoomNode::ClearBossName()
{
	m_szBossName.Clear();
}

void ShuffleRoomNode::CreateGangsi()
{
	ioHashStringVec vOriginalUserName;
	ioHashStringVec vCopyUserName;

	vShuffleRoomUser_iter iter;
	for(iter=m_vUserNode.begin();iter!=m_vUserNode.end();++iter)
	{
		ShuffleRoomUser &kUser = *iter;

		UserParent *pUser = g_UserNodeManager.GetGlobalUserNode( kUser.m_dwUserIndex );
		if( !pUser ) continue;

		if( pUser->IsUserOriginal() )
		{
			vOriginalUserName.push_back( pUser->GetPublicID() );
		}
		else
		{
			vCopyUserName.push_back( pUser->GetPublicID() );
		}
	}

	if( !vOriginalUserName.empty() )        
	{
		//동일 서버에 있는 유저중 랜덤
		int iSize = vOriginalUserName.size();
		int rSelect = rand()%iSize;
		m_szGangsiName = vOriginalUserName[rSelect];

		LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "ShuffleRoomNode::CreateGangsi(%d) 동일 서버 - [%s]당첨!", GetIndex(), m_szGangsiName.c_str() );
	}
	else if( !vCopyUserName.empty() ) 
	{
		//타서버에 있는 유저중 랜덤
		int iSize = vCopyUserName.size();
		int rSelect = rand()%iSize;
		m_szGangsiName = vCopyUserName[rSelect];

		LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "ShuffleRoomNode::CreateGangsi(%d) 타 서버 - [%s]당첨!", GetIndex(), m_szGangsiName.c_str() );
	}
	else
	{
		LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "ShuffleRoomNode::CreateGangsi(%d) : 보스에 선택될 유저가 없다!!", GetIndex() );
	}
	vOriginalUserName.clear();
	vCopyUserName.clear();
}

const ioHashString &ShuffleRoomNode::GetGangsiName()
{
	return m_szGangsiName;
}

void ShuffleRoomNode::ClearGangsi()
{
	m_szGangsiName.Clear();
}

void ShuffleRoomNode::SendPacketTcp( SP2Packet &rkPacket, const DWORD dwUserIndex )
{
	vShuffleRoomUser_iter iter, iEnd;
	iEnd = m_vUserNode.end();
	for(iter=m_vUserNode.begin();iter!=iEnd;++iter)
	{
		ShuffleRoomUser &kUser = *iter;
		if( kUser.m_dwUserIndex == dwUserIndex ) continue;

		UserParent *pUser = g_UserNodeManager.GetGlobalUserNode( kUser.m_dwUserIndex );
		if( pUser )
			pUser->RelayPacket( rkPacket );
	}
}

void ShuffleRoomNode::SendPacketTcpUser( SP2Packet &rkPacket, const ioHashString &rkSenderName )
{
	CRASH_GUARD();
	vShuffleRoomUser_iter iter, iEnd;
	iEnd = m_vUserNode.end();
	for(iter=m_vUserNode.begin();iter!=iEnd;++iter)
	{
		ShuffleRoomUser &kUser = *iter;
		if( kUser.m_szPublicID == rkSenderName ) 
		{
			UserParent *pUser = g_UserNodeManager.GetGlobalUserNode( rkSenderName );
			if( pUser )
				pUser->RelayPacket( rkPacket );
			return;
		}		
	}
}

void ShuffleRoomNode::SendPacketUdp( SP2Packet &rkPacket, const DWORD dwUserIndex /* = 0  */ )
{
	vShuffleRoomUser_iter iter, iEnd;
	iEnd = m_vUserNode.end();
	for(iter=m_vUserNode.begin();iter!=iEnd;++iter)
	{
		ShuffleRoomUser &kUser = *iter;
		if( kUser.m_dwUserIndex == dwUserIndex ) continue;

		g_UDPNode.SendMessage( (char*)kUser.m_szPublicIP.c_str(), kUser.m_iClientPort, rkPacket );
	}
}

void ShuffleRoomNode::Process()
{
	m_NodeSync.Process();
}

bool ShuffleRoomNode::OnProcessPacket( SP2Packet &rkPacket, UserParent *pUser )
{
	switch( rkPacket.GetPacketID() )
	{
	case CTPK_VOICE_INFO:
		OnVoiceInfo( rkPacket, pUser );
		return true;
	case CTPK_BATTLEROOM_COMMAND:
		OnShuffleRoomCommand( rkPacket, pUser );
		return true;
	}
	return false;
}

void ShuffleRoomNode::OnVoiceInfo( SP2Packet &rkPacket, UserParent *pUser )
{
	if( pUser == NULL ) return;

	int iType;	
	PACKET_GUARD_VOID( rkPacket.Read( iType ) );

	if( !COMPARE( iType, ID_VOICE_ON, ID_VOICE_PERMIT + 1) )
		return;

	ioHashString szReceiverID;
	PACKET_GUARD_VOID( rkPacket.Read( szReceiverID ) );

	SP2Packet kReturnPacket( STPK_VOICE_INFO );
	PACKET_GUARD_VOID( kReturnPacket.Write( iType ) );
	PACKET_GUARD_VOID( kReturnPacket.Write( pUser->GetPublicID() ) );
	PACKET_GUARD_VOID( kReturnPacket.Write( GetUserTeam( pUser->GetPublicID() ) ) );

	if( szReceiverID.IsEmpty() ) // ID가 비어 있으면 파티원 모두에게 전송
		SendPacketTcp( kReturnPacket, pUser->GetUserIndex() );
	else
	{
		UserParent *pReceiver = GetUserNode( szReceiverID );
		if( pReceiver )
			pReceiver->RelayPacket( kReturnPacket );
	}	

#ifdef _DEBUG
	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "%s (%d:%s)%d | %s", __FUNCTION__, pUser->GetUserIndex(), pUser->GetPublicID().c_str(), iType, szReceiverID.c_str() );
#endif
}

void ShuffleRoomNode::OnShuffleRoomCommand( SP2Packet &rkPacket, UserParent *pUser )
{

#ifdef FLAG_MODE_BY_BCKIM_DEBUG		// 전투시작 임시 값 확인   ShuffleRoomNode::OnShuffleRoomCommand
	LOG.PrintTimeAndLog(0,"[FLAG_MODE_BY_BCKIM:ShuffleRoomNode::OnShuffleRoomCommand] A_NAME[%s]",pUser->GetPrivateID().c_str());
#endif // FLAG_MODE_BY_BCKIM_DEBUG

	if( pUser == NULL ) return;

	int iCommand = 0;
	PACKET_GUARD_VOID( rkPacket.Read( iCommand ) );

	switch( iCommand )
	{
	case BATTLEROOM_TEAM_CHANGE:
		{
			int iTeamType = 0;
			PACKET_GUARD_VOID( rkPacket.Read( iTeamType ) );

			if( IsShuffleTeamChangeOK( (TeamType)iTeamType ) )
			{
				TeamType eCurTeam = ChangeTeamType( pUser->GetPublicID(), (TeamType)iTeamType );

				SP2Packet kPacket( STPK_BATTLEROOM_COMMAND );
				PACKET_GUARD_VOID( kPacket.Write( BATTLEROOM_TEAM_CHANGE_OK ) );
				PACKET_GUARD_VOID( kPacket.Write( pUser->GetPublicID() ) );
				PACKET_GUARD_VOID( kPacket.Write( (int)eCurTeam ) );
				SendPacketTcp( kPacket );

				m_NodeSync.Update( ShuffleRoomSync::BRS_CHANGEINFO );
			}				
			else
			{
				SP2Packet kPacket( STPK_BATTLEROOM_COMMAND );				
				PACKET_GUARD_VOID( kPacket.Write( BATTLEROOM_TEAM_CHANGE_FULL ) );
				PACKET_GUARD_VOID( kPacket.Write( iTeamType ) );
				pUser->RelayPacket( kPacket );
			}
		}
		break;
	case BATTLEROOM_CANCEL:
		{
			SP2Packet kPacket( STPK_BATTLEROOM_COMMAND );
			PACKET_GUARD_VOID( kPacket.Write( BATTLEROOM_CANCEL_OK ) );
			SendPacketTcp( kPacket );				
		}
		break;
	}
}

bool ShuffleRoomNode::HasModeList()
{
	if( m_vShuffleModeList.empty() )
		return false;

	if( !COMPARE( m_iCurShufflePhase, 0, static_cast<int>( m_vShuffleModeList.size() ) ) )
		return false;

	return true;
}

bool ShuffleRoomNode::ShuffleRoomReadyGo()
{
	if( !HasModeList() )
		return false;

	ioHashString szTitle;              
	if( m_pShuffleRoom && !m_pShuffleRoom->IsRoomEmpty() && m_pShuffleRoom->GetRoomStyle() == RSTYLE_SHUFFLEROOM )
	{
		// 룸에 있는 경우
		int iSubIdx  = m_vShuffleModeList[m_iCurShufflePhase].m_iSubIdx;
		int iMapIdx  = m_vShuffleModeList[m_iCurShufflePhase].m_iMapIdx;
		int iModeIdx = m_vShuffleModeList[m_iCurShufflePhase].m_iModeIdx;

		m_pShuffleRoom->SetSafetyRoom( false );
		m_pShuffleRoom->SetBroadcastRoom( false );
		m_pShuffleRoom->SetTournamentRoom( false, 0 );
		m_pShuffleRoom->SetRoomStyle( RSTYLE_SHUFFLEROOM );

		m_pShuffleRoom->NextShamShuffleRandomTeam( static_cast<ModeType>(iModeIdx) );

		// 여기서 ini 셋팅
		m_pShuffleRoom->SetShuffleModeType( iModeIdx, iSubIdx, iMapIdx );
		{
			SP2Packet kPacket( STPK_SHUFFLEROOM_COMMAND );
			PACKET_GUARD_bool( kPacket.Write( SHUFFLEROOM_READY_GO_OK ) );
			PACKET_GUARD_bool( kPacket.Write( m_pShuffleRoom->GetModeType() ) );
			PACKET_GUARD_bool( kPacket.Write( m_pShuffleRoom->GetModeSubNum() ) );
			PACKET_GUARD_bool( kPacket.Write( m_pShuffleRoom->GetModeMapNum() ) );
			PACKET_GUARD_bool( kPacket.Write( m_iCurShufflePhase ) );
			SendPacketTcp( kPacket );
		}

		if( iModeIdx == MT_BOSS )
			CreateBoss();
		else if( iModeIdx == MT_GANGSI )
			CreateGangsi();

		m_pShuffleRoom->CreateNextShamShuffle();

		if( !IncreaseShufflePhase() )
			SetShufflePhaseEnd();
	}
	else
	{
		Room *pRoom = g_RoomNodeManager.CreateNewRoom();
		if( pRoom )
		{	
			int iSubIdx  = m_vShuffleModeList[m_iCurShufflePhase].m_iSubIdx;
			int iMapIdx  = m_vShuffleModeList[m_iCurShufflePhase].m_iMapIdx; 
			int iModeIdx = m_vShuffleModeList[m_iCurShufflePhase].m_iModeIdx;

			pRoom->SetSafetyRoom( false );
			pRoom->SetBroadcastRoom( false );
			pRoom->SetTournamentRoom( false, 0 );
			pRoom->SetRoomStyle( RSTYLE_SHUFFLEROOM );
			pRoom->InitModeTypeList();

			pRoom->SetShuffleModeType( iModeIdx, iSubIdx, iMapIdx );
			if( iModeIdx != MT_SURVIVAL && iModeIdx != MT_BOSS && iModeIdx != MT_GANGSI && iModeIdx != MT_MONSTER_SURVIVAL && iModeIdx == MT_SHUFFLE_BONUS &&
				iModeIdx != MT_DUNGEON_A && iModeIdx != MT_FIGHT_CLUB && iModeIdx != MT_RAID && !Help::IsMonsterDungeonMode(static_cast<ModeType>(iModeIdx)) )
				ShuffleEnterRandomTeam();

			if( iModeIdx == MT_BOSS )
				CreateBoss();
			else if( iModeIdx == MT_GANGSI )
				CreateGangsi();

			EnterShuffleRoom( pRoom );

			if( !IncreaseShufflePhase() )
				SetShufflePhaseEnd();
		}
		else
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"ShuffleRoomNode::ShuffleRoomReadyGo() : 잔여 룸이 없다(%d)", GetIndex() );
	}

	m_NodeSync.Update( ShuffleRoomSync::BRS_PLAY );                  // 플레이 갱신

	return true;
}

void ShuffleRoomNode::EnterShuffleRoom( Room *pRoom )
{
	m_pShuffleRoom = pRoom;

	vShuffleRoomUser_iter iter, iEnd;
	iEnd = m_vUserNode.end();
	for( iter=m_vUserNode.begin() ; iter!=iEnd ; ++iter )
	{
		ShuffleRoomUser &kUser = *iter;
		UserParent *pItem = g_UserNodeManager.GetGlobalUserNode( kUser.m_dwUserIndex );
		if( pItem == NULL ) continue;

		if( pItem->IsUserOriginal() )
		{
			User *pUser = (User*)pItem;

			// 유저 로딩 상태 돌입
			SP2Packet kPacket( STPK_SHUFFLEROOM_COMMAND );
			PACKET_GUARD_VOID( kPacket.Write( SHUFFLEROOM_READY_GO_OK ) );
			PACKET_GUARD_VOID( kPacket.Write( m_pShuffleRoom->GetModeType() ) );
			PACKET_GUARD_VOID( kPacket.Write( m_pShuffleRoom->GetModeSubNum() ) );
			PACKET_GUARD_VOID( kPacket.Write( m_pShuffleRoom->GetModeMapNum() ) );
			PACKET_GUARD_VOID( kPacket.Write( m_iCurShufflePhase ) );
			pUser->SendMessage( kPacket );

			pUser->EnterRoom( m_pShuffleRoom );
		}
		else
		{
			//타서버에 있는 유저들은 전부 서버 이동 시작
			UserCopyNode *pUser = (UserCopyNode*)pItem;
			m_pShuffleRoom->EnterReserveUser( pUser->GetUserIndex() );
			SP2Packet kPacket( SSTPK_MOVING_ROOM_RESULT );
			PACKET_GUARD_VOID( kPacket.Write( SS_MOVING_ROOM_JOIN ) );
			PACKET_GUARD_VOID( kPacket.Write( SS_MOVING_ROOM_JOIN_SHUFFLE ) );
			PACKET_GUARD_VOID( kPacket.Write( (int)m_pShuffleRoom->GetModeType() ) );
			PACKET_GUARD_VOID( kPacket.Write( m_pShuffleRoom->GetRoomIndex() ) );
			PACKET_GUARD_VOID( kPacket.Write( pUser->GetUserIndex() ) );
			PACKET_GUARD_VOID( kPacket.Write( m_pShuffleRoom->GetModeSubNum() ) );
			PACKET_GUARD_VOID( kPacket.Write( m_pShuffleRoom->GetModeMapNum() ) );
			PACKET_GUARD_VOID( kPacket.Write( (int)m_pShuffleRoom->GetPlazaModeType() ) );
			PACKET_GUARD_VOID( kPacket.Write( m_pShuffleRoom->GetRoomNumber() ) );
			pUser->SendMessage( kPacket );
		}
	}
}

bool ShuffleRoomNode::IncreaseShufflePhase()
{
	m_iCurShufflePhase++;
	
	if( m_iCurShufflePhase >= m_iMaxShufflePhase )
	{
		return false;
	}

	return true;
}

void ShuffleRoomNode::SetShufflePhaseEnd()
{
	m_bPhaseEnd = true;
}

bool ShuffleRoomNode::IsShufflePhaseEnd()
{
	return m_bPhaseEnd;
}

void ShuffleRoomNode::RestartShuffleMode()
{
	InitShuffleMode();
	SendShufflePhaseAllUser();
}

void ShuffleRoomNode::InitShuffleRecord()
{
	m_vShuffleRecord.clear();
}

void ShuffleRoomNode::AddModeContributeByShuffle( const ioHashString& szPublicID, int iUserIndex, float fContributePer, DWORD dwPlayTime )
{
	if( m_bPhaseEnd )
		return;	
	
	int iSize = GetShuffleRecordCnt();
	for( int i = 0; i < iSize; ++i )
	{
		if( m_vShuffleRecord[i].m_iUserIndex == iUserIndex )
		{
			PhaseRecordMap::iterator iter = m_vShuffleRecord[i].m_PhaseRecordMap.find( m_iCurShufflePhase-1 );
			if( iter != m_vShuffleRecord[i].m_PhaseRecordMap.end() )
			{
				PhaseRecord& rkRecord  = iter->second;
				rkRecord.m_fContribtuePer = fContributePer;
				rkRecord.m_dwPlayTime     = dwPlayTime;

				LOG.PrintTimeAndLog( LOG_SHUFFLE, "%s modify - %s : phase : %d, contribute : %f, playtime : %d", __FUNCTION__, szPublicID.c_str(), m_iCurShufflePhase-1, fContributePer, dwPlayTime );
			}
			else
			{
				PhaseRecord rkNewRecord;
				rkNewRecord.m_fContribtuePer = fContributePer;
				rkNewRecord.m_dwPlayTime     = dwPlayTime;

				m_vShuffleRecord[i].m_PhaseRecordMap.insert( PhaseRecordMap::value_type( m_iCurShufflePhase-1, rkNewRecord ) );

				LOG.PrintTimeAndLog( LOG_SHUFFLE, "%s insert - %s : phase : %d, contribute : %f, playtime : %d", __FUNCTION__, szPublicID.c_str(), m_iCurShufflePhase-1, fContributePer, dwPlayTime );
			}

			return;
		}
	}

	ShuffleRecord kInfo;
	kInfo.m_iUserIndex = iUserIndex;
	kInfo.m_szPublicID = szPublicID;

	PhaseRecord rkNewRecord;
	rkNewRecord.m_fContribtuePer = fContributePer;
	rkNewRecord.m_dwPlayTime     = dwPlayTime;
	kInfo.m_PhaseRecordMap.insert( PhaseRecordMap::value_type( m_iCurShufflePhase-1, rkNewRecord ) );

	LOG.PrintTimeAndLog( LOG_SHUFFLE, "%s new - %s : phase : %d, contribute : %f, playtime : %d", __FUNCTION__, szPublicID.c_str(), m_iCurShufflePhase-1, fContributePer, dwPlayTime );

	m_vShuffleRecord.push_back( kInfo );	
}

void ShuffleRoomNode::FinalRoundhufflePlayPoint( Mode* pMode, ModeRecord* pRecord )
{
	if( !pMode )
		return;

	if( !pRecord )
		return;

	if( pRecord->eState == RS_LOADING )
		return;

	User *pUser = pRecord->pUser;
	if( !pUser )
		return;
	
	//승/패 포인트
	float fCurrPoint	= 0.0f;
	if( pMode->GetWinTeam() == pUser->GetTeam() )
		fCurrPoint = g_ShuffleRoomManager.GetWinBonus();
	else
		fCurrPoint = g_ShuffleRoomManager.GetLoseBonus();
		
	int iRecordPlayTime		= pMode->GetRecordPlayTime( pRecord );
	int iShufflePlayTime	= max( 1, pMode->GetCurRound() * pMode->GetRoundDuration() );

	//시간 포인트
	float fTimePoint		= static_cast<float>(iRecordPlayTime) / static_cast<float>(iShufflePlayTime);
	//기여도
	float fContribute		= pRecord->fContributePer * 100.0f;
	//플레이 포인트
	float fPlayPoint		= fContribute * fCurrPoint * fTimePoint;

	//연승보너스
	float fStreakPoint		= min( g_ShuffleRoomManager.GetWinningStreakBonus() * max( 0.0f, static_cast<float>(pRecord->iVictories - 1) ), g_ShuffleRoomManager.GetWinningStreakMax() );
	fStreakPoint = max( 0.0f, fStreakPoint );

	//연승 포인트
	float fWinningPoint		= fPlayPoint * fStreakPoint;
	//연속 게임보너스
	int fConsecutivePoint	= fPlayPoint * g_ShuffleRoomManager.GetModeConsecutivelyBonus( pUser );
	//수상 보너스
	int fAwardPoint			= pRecord->fBonusArray[BA_AWARD_BONUS] * 100.0f;
	//총 별 갯수
	int iStarTotal			= fPlayPoint + fWinningPoint + fConsecutivePoint + fAwardPoint;

#ifdef _DEBUG
	printf( "%s - fTimePoint : %d, fContribute : %d, fPlayPoint : %d, fStreakPoint : %d, fWinningPoint : %d, fConsecutivePoint : %d, fAwardPoint : %d, iStarTotal : %d",
		__FUNCTION__, fTimePoint, fContribute, fPlayPoint, fStreakPoint, fWinningPoint, fConsecutivePoint, fAwardPoint, iStarTotal );
#else
	LOG.PrintTimeAndLog( LOG_SHUFFLE, "%s - iTimePoint : %d, iContribute : %d, iPlayPoint : %d, iStreakPoint : %d, iWinningPoint : %d, iConsecutivePoint : %d, iAwardPoint : %d, iStarTotal : %d",
		__FUNCTION__, fTimePoint, fContribute, fPlayPoint, fStreakPoint, fWinningPoint, fConsecutivePoint, fAwardPoint, iStarTotal );
#endif	

	if( 0 < iStarTotal )
	{
		SP2Packet kPacket( STPK_SHUFFLE_REWARD_RESULT );
		PACKET_GUARD_VOID( kPacket.Write( static_cast<int>( fPlayPoint ) ) );
		PACKET_GUARD_VOID( kPacket.Write( static_cast<int>( fWinningPoint ) ) );
		PACKET_GUARD_VOID( kPacket.Write( static_cast<int>( fConsecutivePoint ) ) );
		PACKET_GUARD_VOID( kPacket.Write( static_cast<int>( fAwardPoint ) ) );
		pUser->SendMessage( kPacket );

		SendShuffleReward( pUser, iStarTotal );
	}
}

void ShuffleRoomNode::CheckShuffleReward( User *pUser, int iStarCnt, int& iStarByCalcBonusCount, float& fBonusPercent )
{
	int iRecordCnt = GetShuffleRecordCnt();
	for( int i = 0; i < iRecordCnt; ++i )
	{
		ShuffleRecord kInfo = m_vShuffleRecord[i];
		if( kInfo.m_iUserIndex != pUser->GetUserIndex() )
			continue;
		
		int iWeightCount     = 0;
		float fContributePer = 0.0f;
		float fTimeWeight    = 1;

		for( PhaseRecordMap::iterator iter = kInfo.m_PhaseRecordMap.begin(); iter != kInfo.m_PhaseRecordMap.end(); ++iter, ++iWeightCount )
		{
			PhaseRecord& rkRecord = iter->second;
			DWORD dwShuffleTimeWeight = g_ShuffleRoomManager.GetShufflePoint( iter->first );
			
			fTimeWeight    += min( 1.0f, rkRecord.m_dwPlayTime / dwShuffleTimeWeight );
			fContributePer += rkRecord.m_fContribtuePer;

			LOG.PrintTimeAndLog( LOG_SHUFFLE, "%s - %s - phase : %d, curr weight : %f(%d/%f) : curr contribute : %f", __FUNCTION__, kInfo.m_szPublicID.c_str(), iWeightCount, fTimeWeight, rkRecord.m_dwPlayTime, dwShuffleTimeWeight, fContributePer );
		}

		float fBonusPoint = fTimeWeight / g_ShuffleRoomManager.GetMaxModeCount() * fContributePer;

		LOG.PrintTimeAndLog( LOG_SHUFFLE, "%s - %s : fTimeWeight tot: %f, fBonusPoint : %f, contribute : %f", __FUNCTION__, kInfo.m_szPublicID.c_str(), fTimeWeight, fBonusPoint, fContributePer );

		fBonusPercent = fBonusPoint;
		iStarByCalcBonusCount = iStarCnt * fBonusPoint;
		
		if( 0 < iStarByCalcBonusCount )
		{
			SendShuffleReward( pUser, iStarByCalcBonusCount );
		}
	}
}

void ShuffleRoomNode::SendShuffleReward( User *pUser, int iCnt )
{
	int iType   = g_ShuffleRoomManager.GetRewardItemType();
	int iPeriod = g_ShuffleRoomManager.GetRewardItemPeriod();
	int iIndex  = g_ShuffleRoomManager.GetRewardItemIndex();

	CTimeSpan cPresentGapTime( iPeriod, 0, 0, 0 );
	CTime kPresentTime = CTime::GetCurrentTime() + cPresentGapTime;


	pUser->AddPresentMemory( g_MainServer.GetSendID(), iType, iIndex, iCnt, 0, 0, 0, kPresentTime, 0 );
	pUser->SendPresentMemory();

	g_LogDBClient.OnInsertPresent( 0, g_MainServer.GetSendID(), g_App.GetPublicIP().c_str(), pUser->GetUserIndex(), iType, iIndex, iCnt, 0, 0, LogDBClient::PST_RECIEVE, "ShuffleReward" );
}

bool ShuffleRoomNode::IsShuffleRoom()
{
	if( !m_pShuffleRoom )
		return false;

	if( m_pShuffleRoom->GetRoomStyle() == RSTYLE_SHUFFLEROOM )
		return true;

	return false;
}

void ShuffleRoomNode::SendShufflePhase( User *pUser )
{
	if( !pUser )
		return;
	
	int iSize = m_vShuffleModeList.size() - 1;
	iSize = max( 0, iSize );

	int iCurShufflePhase = m_iCurShufflePhase - 1;
	iCurShufflePhase = max( 0, iCurShufflePhase );

	SP2Packet kPacket5( STPK_SHUFFLEROOM_COMMAND );

	PACKET_GUARD_VOID( kPacket5.Write( SHUFFLEROOM_PHASE_INFO ) );
	PACKET_GUARD_VOID( kPacket5.Write( iCurShufflePhase ) );
	PACKET_GUARD_VOID( kPacket5.Write( iSize ) );

	for( int i=0; i<iSize; ++i )
	{
		PACKET_GUARD_VOID( kPacket5.Write( m_vShuffleModeList[i].m_iModeIdx ) );
	}
	pUser->RelayPacket( kPacket5 );
}

void ShuffleRoomNode::SendShufflePhaseAllUser()
{
	vShuffleRoomUser_iter iter, iEnd;
	iEnd = m_vUserNode.end();
	for( iter=m_vUserNode.begin() ; iter!=iEnd ; ++iter )
	{
		ShuffleRoomUser &kUser = *iter;
		User *pUser = g_UserNodeManager.GetUserNode( kUser.m_dwUserIndex );
		SendShufflePhase( pUser );
	}
}

int ShuffleRoomNode::GetKillDeathLevelAverage( User *pUser )
{
	if( m_vUserNode.empty() )
		return 0;

	int iKillPoint = 0;
	int iUserCount = 0;
	for( vShuffleRoomUser_iter iter = m_vUserNode.begin() ; iter != m_vUserNode.end() ; ++iter )
	{
		ShuffleRoomUser &kUser = *iter;
		User *pUserNode = g_UserNodeManager.GetUserNode( kUser.m_dwUserIndex );
		if( pUserNode && pUserNode != pUser )
		{
			iKillPoint += pUserNode->GetKillDeathLevel();
			iUserCount++;
		}
	}

	if( iUserCount == 0 )
		return 0;

	return iKillPoint/iUserCount;
}

void ShuffleRoomNode::KickOutHighLevelUser()
{
	//2명 이하는 강퇴를 진행하지 않음
	if( (int)m_vUserNode.size() <=  2 )
		return;
	
	for( vShuffleRoomUser_iter iter = m_vUserNode.begin() ; iter != m_vUserNode.end() ; ++iter )
	{
		ShuffleRoomUser &kUser = *iter;
		User *pUser = g_UserNodeManager.GetUserNode( kUser.m_dwUserIndex );

		if( !pUser )
			continue;

		int iAverage = GetKillDeathLevelAverage( pUser );
		if( g_ShuffleRoomManager.IsKickOutMaxLevel( iAverage, pUser->GetKillDeathLevel() ) )		//방 평균 레벨보다 높은지 체크
		{
			KickOutUser( pUser->GetPublicID(), RoomParent::RLT_HIGH_LEVEL );
			return;
		}		
		else if( g_ShuffleRoomManager.IsKickOutMinLevel( iAverage, pUser->GetKillDeathLevel() ) )	//방 평균 레벨 보다 낮은지 체크
		{
			KickOutUser( pUser->GetPublicID(), RoomParent::RLT_LOW_LEVEL );
			return;
		}
	}
}

void ShuffleRoomNode::KickOutModeError()
{
	for( vShuffleRoomUser_iter iter = m_vUserNode.begin() ; iter != m_vUserNode.end() ; ++iter )
	{
		ShuffleRoomUser &kUser = *iter;
		User *pUser = g_UserNodeManager.GetUserNode( kUser.m_dwUserIndex );

		if( !pUser )
			continue;

		KickOutUser( pUser->GetPublicID(), RoomParent::RLT_SYS_ERROR );		
	}
}

bool ShuffleRoomNode::IsReserveTimeOver()
{
	if( !m_vUserNode.empty() )
		return false;

	if( m_dwReserveTime == 0 )
		return false;

	if( TIMEGETTIME() - m_dwReserveTime > CREATE_RESERVE_DELAY_TIME )
		return true;

	return false;
}

bool ShuffleRoomNode::HasUser( const ioHashString& szPublicName )
{
	for( vShuffleRoomUser_iter iter = m_vUserNode.begin() ; iter != m_vUserNode.end() ; ++iter )
	{
		ShuffleRoomUser &kUser = *iter;
		User *pUser = g_UserNodeManager.GetUserNode( kUser.m_dwUserIndex );

		if( !pUser )
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s - %s - pUser == NULL", __FUNCTION__, szPublicName.c_str() );
			continue;
		}

		if( pUser->GetPublicID() == szPublicName )
			return true;
	}

	return false;
}