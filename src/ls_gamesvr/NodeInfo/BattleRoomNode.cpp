#include "stdafx.h"

#include "../EtcHelpFunc.h"
//#include "../Window.h"
#include "../MainProcess.h"
#include "../MainServerNode/MainServerNode.h"

#include "BattleRoomManager.h"
#include "UserNodeManager.h"
#include "ServerNodeManager.h"
#include "RoomNodeManager.h"
#include "LevelMatchManager.h"
#include "BattleRoomNode.h"
#include "ioMyLevelMgr.h"
#include "TournamentManager.h"
#include "../Local/ioLocalParent.h"
#include "TeamSurvivalAIMode.h"

extern CLog P2PRelayLOG;

//////////////////////////////////////////////////////////////////////////
BattleRoomSync::BattleRoomSync()
{
	m_pCreator     = NULL;
	m_dwUpdateTime = 0;
	m_dwUpdateType = 0;
	// 업데이트 시간
	m_dwCheckTime[BRS_SELECTMODE]	= 4500;
	m_dwCheckTime[BRS_PLAY]			= 4000;
	m_dwCheckTime[BRS_CHANGEINFO]	= 3000;
	m_dwCheckTime[BRS_CREATE]		= 500;
	m_dwCheckTime[BRS_DESTROY]		= 0;
}

BattleRoomSync::~BattleRoomSync()
{

}

void BattleRoomSync::Init()
{
	m_dwUpdateTime = 0;
	m_dwUpdateType = 0;
}

void BattleRoomSync::SetCreator( BattleRoomNode *pCreator )
{
	Init();
	m_pCreator = pCreator;
}

void BattleRoomSync::Update( DWORD dwUpdateType )
{
	if( !COMPARE( dwUpdateType, BRS_SELECTMODE, MAX_BRS ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "BattleRoomSync::Update 알수 없는 업데이트 값 : %d", dwUpdateType );
		return;
	}
	if( !m_pCreator )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "BattleRoomSync::Update m_pCreator == NULL" );
		return;
	}

	if( dwUpdateType < m_dwUpdateType )
		return;

	m_dwUpdateType = dwUpdateType;
	m_dwUpdateTime = TIMEGETTIME();
	Process();
}

void BattleRoomSync::Process()
{
	if( m_dwUpdateTime == 0 ) return;
	if( !COMPARE( m_dwUpdateType, BRS_SELECTMODE, MAX_BRS ) ) return;

	DWORD dwGap = TIMEGETTIME() - m_dwUpdateTime;
	if( dwGap >= m_dwCheckTime[m_dwUpdateType] )
	{
		switch( m_dwUpdateType )
		{
		case BRS_SELECTMODE:
			m_pCreator->SyncSelectMode();
			break;
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
BattleRoomNode::BattleRoomNode( DWORD dwIndex ) : m_dwIndex( dwIndex )
{
	m_bRandomTeamSequence = true;
	InitData();
}

BattleRoomNode::~BattleRoomNode()
{
}

void BattleRoomNode::SyncSelectMode()
{
	SP2Packet kPacket( SSTPK_BATTLEROOM_SYNC );
	PACKET_GUARD_VOID( kPacket.Write(BattleRoomSync::BRS_SELECTMODE) );
	PACKET_GUARD_VOID( kPacket.Write(GetIndex()) );
	PACKET_GUARD_VOID( kPacket.Write(GetSelectModeTerm()) );
	PACKET_GUARD_VOID( kPacket.Write(GetSelectMode()) );
	PACKET_GUARD_VOID( kPacket.Write(GetSelectMap()) );

	g_ServerNodeManager.SendMessageToPartitions( kPacket );
}

void BattleRoomNode::SyncPlay()
{
	SP2Packet kPacket( SSTPK_BATTLEROOM_SYNC );
	PACKET_GUARD_VOID( kPacket.Write(BattleRoomSync::BRS_PLAY) );
	PACKET_GUARD_VOID( kPacket.Write(GetIndex()) );
	PACKET_GUARD_VOID( kPacket.Write(GetSelectModeTerm()) );
	PACKET_GUARD_VOID( kPacket.Write(GetSelectMode()) );
	PACKET_GUARD_VOID( kPacket.Write(GetSelectMap()) );
	PACKET_GUARD_VOID( kPacket.Write(GetPlayModeType()) );
	PACKET_GUARD_VOID( kPacket.Write(IsBattleTimeClose()) );
	PACKET_GUARD_VOID( kPacket.Write(m_bRandomTeamMode) );
	PACKET_GUARD_VOID( kPacket.Write(IsStartRoomEnterX()) );
	PACKET_GUARD_VOID( kPacket.Write(m_bUseExtraOption) );
	PACKET_GUARD_VOID( kPacket.Write(m_bNoChallenger) );
			/*
			<< m_bUseExtraOption << m_iChangeCharType
			<< m_iCoolTimeType << m_iRedHPType << m_iBlueHPType
			<< m_iDropDamageType << m_iGravityType
			<< m_iPreSetModeType << m_iTeamAttackType << m_iGetUpType
			<< m_iKOType << m_iRedBlowType << m_iBlueBlowType
			<< m_iRedMoveSpeedType << m_iBlueMoveSpeedType << m_iKOEffectType
			<< m_iRedEquipType << m_iBlueEquipType;
			*/
	g_ServerNodeManager.SendMessageToPartitions( kPacket );
}

void BattleRoomNode::SyncChangeInfo()
{
	SP2Packet kPacket( SSTPK_BATTLEROOM_SYNC );
	PACKET_GUARD_VOID( kPacket.Write(BattleRoomSync::BRS_CHANGEINFO) );
	PACKET_GUARD_VOID( kPacket.Write(GetIndex()) );
	PACKET_GUARD_VOID( kPacket.Write(GetSelectModeTerm()) );
	PACKET_GUARD_VOID( kPacket.Write(GetSelectMode()) );
	PACKET_GUARD_VOID( kPacket.Write(GetSelectMap()) );
	PACKET_GUARD_VOID( kPacket.Write(GetPlayModeType()) );
	PACKET_GUARD_VOID( kPacket.Write(IsBattleTimeClose()) );
	PACKET_GUARD_VOID( kPacket.Write(m_bRandomTeamMode) );
	PACKET_GUARD_VOID( kPacket.Write(IsStartRoomEnterX()) );
	PACKET_GUARD_VOID( kPacket.Write(m_bUseExtraOption) );
	PACKET_GUARD_VOID( kPacket.Write(m_bNoChallenger) );
			/*
			<< m_bUseExtraOption << m_iChangeCharType
			<< m_iCoolTimeType << m_iRedHPType << m_iBlueHPType
			<< m_iDropDamageType << m_iGravityType
			<< m_iPreSetModeType << m_iTeamAttackType << m_iGetUpType
			<< m_iKOType << m_iRedBlowType << m_iBlueBlowType
			<< m_iRedMoveSpeedType << m_iBlueMoveSpeedType << m_iKOEffectType
			<< m_iRedEquipType << m_iBlueEquipType;
			*/
	PACKET_GUARD_VOID( kPacket.Write(m_szRoomName) );
	PACKET_GUARD_VOID( kPacket.Write(m_szRoomPW) );
	PACKET_GUARD_VOID( kPacket.Write(m_OwnerUserID) );
	PACKET_GUARD_VOID( kPacket.Write((int)m_vUserNode.size()) );
	PACKET_GUARD_VOID( kPacket.Write(GetPlayUserCnt()) );
	PACKET_GUARD_VOID( kPacket.Write(m_iMaxPlayerBlue) );
	PACKET_GUARD_VOID( kPacket.Write(m_iMaxPlayerRed) );
	PACKET_GUARD_VOID( kPacket.Write(m_iMaxObserver) );
	PACKET_GUARD_VOID( kPacket.Write(GetAbilityMatchLevel()) );
	PACKET_GUARD_VOID( kPacket.Write(GetRoomLevel()) );
	PACKET_GUARD_VOID( kPacket.Write(GetBattleEventType()) );

	g_ServerNodeManager.SendMessageToPartitions( kPacket );
}

void BattleRoomNode::SyncCreate()
{
	SP2Packet kPacket( SSTPK_BATTLEROOM_SYNC );
	PACKET_GUARD_VOID( kPacket.Write(BattleRoomSync::BRS_CREATE) );
	FillSyncCreate( kPacket );
	g_ServerNodeManager.SendMessageToPartitions( kPacket );
}

void BattleRoomNode::FillSyncCreate( SP2Packet &rkPacket )
{
	PACKET_GUARD_VOID( rkPacket.Write(GetIndex()) );
	PACKET_GUARD_VOID( rkPacket.Write(GetTournamentIndex()) );
	PACKET_GUARD_VOID( rkPacket.Write(GetSelectModeTerm()) );
	PACKET_GUARD_VOID( rkPacket.Write(GetSelectMode()) );
	PACKET_GUARD_VOID( rkPacket.Write(GetSelectMap()) );
	PACKET_GUARD_VOID( rkPacket.Write(GetPlayModeType()) );
	PACKET_GUARD_VOID( rkPacket.Write(IsBattleTimeClose()) );
	PACKET_GUARD_VOID( rkPacket.Write(m_bRandomTeamMode) );
	PACKET_GUARD_VOID( rkPacket.Write(IsStartRoomEnterX()) );
	PACKET_GUARD_VOID( rkPacket.Write(m_bUseExtraOption) );
	PACKET_GUARD_VOID( rkPacket.Write(m_bNoChallenger) );
			 /*
			 << m_bUseExtraOption << m_iChangeCharType
			 << m_iCoolTimeType << m_iRedHPType << m_iBlueHPType
			 << m_iDropDamageType << m_iGravityType
			 << m_iPreSetModeType << m_iTeamAttackType << m_iGetUpType
			 << m_iKOType << m_iRedBlowType << m_iBlueBlowType
			 << m_iRedMoveSpeedType << m_iBlueMoveSpeedType << m_iKOEffectType
			 << m_iRedEquipType << m_iBlueEquipType;
			 */
	PACKET_GUARD_VOID( rkPacket.Write(m_szRoomName) );
	PACKET_GUARD_VOID( rkPacket.Write(m_szRoomPW) );
	PACKET_GUARD_VOID( rkPacket.Write(m_OwnerUserID) );
	PACKET_GUARD_VOID( rkPacket.Write((int)m_vUserNode.size()) );
	PACKET_GUARD_VOID( rkPacket.Write(GetPlayUserCnt()) );
	PACKET_GUARD_VOID( rkPacket.Write(m_iMaxPlayerBlue) );
	PACKET_GUARD_VOID( rkPacket.Write(m_iMaxPlayerRed) );
	PACKET_GUARD_VOID( rkPacket.Write(m_iMaxObserver) );
	PACKET_GUARD_VOID( rkPacket.Write(GetAbilityMatchLevel()) );
	PACKET_GUARD_VOID( rkPacket.Write(GetRoomLevel()) );
	PACKET_GUARD_VOID( rkPacket.Write(GetBattleEventType()) );
}

void BattleRoomNode::SyncDestroy()
{
	SP2Packet kPacket( SSTPK_BATTLEROOM_SYNC );
	PACKET_GUARD_VOID( kPacket.Write(BattleRoomSync::BRS_DESTROY) );
	PACKET_GUARD_VOID( kPacket.Write(GetIndex()) );
	g_ServerNodeManager.SendMessageToPartitions( kPacket );
}

void BattleRoomNode::SyncRealTimeCreate()
{
	SyncCreate();
	m_NodeSync.Init();
}

void BattleRoomNode::InitData()
{
	m_vUserNode.clear();

	m_vBMCopyUserNode.clear();		// 2019-02-14 by bckim, 배틀 모드 추가

	m_szRoomName.Clear();
	m_szRoomPW.Clear(); 
	m_OwnerUserID.Clear();
	m_iMaxPlayerBlue = 0; 
	m_iMaxPlayerRed  = 0; 
	m_iMaxObserver = 0;
	m_iSelectMode = -1;
	m_iSelectMap  = -1;
	m_iPreSubType = 0;
	m_iPreMapNum = 0;
	m_PreModeType = MT_NONE;
	m_bSafetyLevelRoom = false;
	m_NodeSync.SetCreator( this );
	m_pBattleRoom = NULL;
	m_bRandomTeamMode  = true;
	m_bStartRoomEnterX = false;
	m_bAutoModeStart   = false;
	m_bBadPingKick     = true;

	if( ioLocalManager::GetSingletonPtr() )
	{
		ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
		if( pLocal && !pLocal->IsBadPingKick( false ) )
			m_bBadPingKick = false;
	}
	m_bUseExtraOption = false;
	m_bNoChallenger = true;

	m_iTeamAttackType = 0;
	m_iChangeCharType = 0;
	m_iCoolTimeType = 0;
	m_iRedHPType = 0;
	m_iBlueHPType = 0;
	m_iDropDamageType = 0;
	m_iGravityType = 0;
	m_iGetUpType = 0;
	m_iRedMoveSpeedType = 0;
	m_iBlueMoveSpeedType = 0;
	m_iKOType = 0;
	m_iKOEffectType = 0;
	m_iRedBlowType = 0;
	m_iBlueBlowType = 0;
	m_iRedEquipType = 0;
	m_iBlueEquipType = 0;

	m_iCatchModeRoundType = -1;
	m_iCatchModeRoundTimeType = -1;

	m_iGrowthUseType = 0;
	m_iExtraItemUseType = 0;

	m_iPreSetModeType = 0;

	m_iBattleEventType = BET_NORMAL;
	m_vInviteUser.clear();
	m_dwReserveTime    = 0;
	m_szBossName.Clear();
	m_szGangsiName.Clear();

	m_TournamentRoundData.Init();

	m_iAILevel = 0;

	InitRecord();
}

void BattleRoomNode::InitRecord()
{
	m_iBlueWin = 0;
	m_iBlueLose = 0;
	m_iBlueVictories = 0;
	m_iRedWin = 0;
	m_iRedLose = 0;
	m_iRedVictories = 0;
}

void BattleRoomNode::OnCreate()
{
	InitData();
	m_NodeSync.Update( BattleRoomSync::BRS_CREATE );
	m_dwReserveTime = TIMEGETTIME();
	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "BattleRoomNode::OnCreate (%d)", GetIndex() );
}

void BattleRoomNode::OnDestroy()
{
	BlockNode::Reset();

	InitData();
	m_NodeSync.Update( BattleRoomSync::BRS_DESTROY );
	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "BattleRoomNode::OnDestroy (%d)", GetIndex() );
}

void BattleRoomNode::EnterUser( const DWORD dwUserIndex, const ioHashString &szPublicID, const int iGradeLevel, const int iAbilityLevel, const bool bSafetyLevel, const bool bObserver,
							    const ioHashString &szPublicIP, const ioHashString &szPrivateIP, const ioHashString &szTransferIP, const int iClientPort, const int iTransferPort  )
{
	// 예외 처리 : 동일한 유저가 두번 입장 가능성이 있으므로
	// 입장할 때 이미 입장되어있는 아이디는 삭제한다.
	if( RemoveUser( dwUserIndex ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ERROR!!! : BattleRoomNode::EnterUser(%d) : %d - %s - %s:%d", 
								GetIndex(), dwUserIndex, szPublicID.c_str(), szTransferIP.c_str(), iTransferPort );
	}
	m_dwReserveTime = 0;

	if( m_OwnerUserID.IsEmpty() )
	{
		m_OwnerUserID = szPublicID;
		LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "BattleRoomNode::EnterUser(%d) - Owner(%s)", GetIndex(), szPublicID.c_str() );

		if( IsRegularTournamentBattle() )
		{
			LOG.PrintTimeAndLog( 0, "[대회로그] %s 입장하여 방장이 됨 : %s(%d)", __FUNCTION__, szPublicID.c_str(), GetIndex() );
		}
	}

	// 전투룸에서 사용할 유저 정보
	BattleRoomUser kEnterUser;
	kEnterUser.m_dwUserIndex	= dwUserIndex;
	kEnterUser.m_szPublicID		= szPublicID;
	kEnterUser.m_iGradeLevel	= iGradeLevel;
	kEnterUser.m_iAbilityLevel	= iAbilityLevel;	
	kEnterUser.m_bSafetyLevel   = bSafetyLevel;
	kEnterUser.m_bObserver		= bObserver;
	kEnterUser.m_szPublicIP		= szPublicIP;
	kEnterUser.m_szPrivateIP	= szPrivateIP;
	kEnterUser.m_szTransferIP   = szTransferIP;
	kEnterUser.m_iClientPort	= iClientPort;
	kEnterUser.m_iTransferPort  = iTransferPort;
	kEnterUser.m_iServerRelayCount = 0;

	// 2019-02-14 by bckim, 배틀 모드 추가
	kEnterUser.m_eSelect_Order = BATTLE_ORDER_RANDOM;		
	// End. 2019-02-14 by bckim, 배틀 모드 추가

	// 경기방 팀은 고정
	if( !bObserver && GetBattleEventType() == BET_TOURNAMENT_BATTLE )
	{
		UserParent *pUser = g_UserNodeManager.GetGlobalUserNode( dwUserIndex );
		if( pUser && IsRegularTournamentBattle() )
		{
			LOG.PrintTimeAndLog( 0, "[대회로그] %s 대회입장 : %s(%d)", __FUNCTION__, pUser->GetPublicID().c_str(), GetIndex() );
		}

		if( IsTournamentTeam( dwUserIndex, TEAM_BLUE ) )
			kEnterUser.m_eTeamType = TEAM_BLUE;
		else if( IsTournamentTeam( dwUserIndex, TEAM_RED ) )
			kEnterUser.m_eTeamType = TEAM_RED;
		else
		{
			kEnterUser.m_eTeamType = TEAM_NONE;       // 팀 없으면 옵저버
			kEnterUser.m_bObserver = true;
		}
	}
	else
	{
		kEnterUser.m_eTeamType      = CreateTeamType();
	}

	if( bObserver )
	{
		kEnterUser.m_eTeamType = TEAM_NONE;
	}

	AddUser( kEnterUser );

	UserParent *pUser = g_UserNodeManager.GetGlobalUserNode( dwUserIndex );
	if( pUser == NULL ) 
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "BattleRoomNode::EnterUser(%d) 파티에 들어온 유저 없음 : %s(%d)", GetIndex(), szPublicID.c_str(), dwUserIndex );
		return;
	}

	//들어온 유저에게 파티 정보 전송.
	SP2Packet kPacket( STPK_BATTLEROOM_INFO );
	FillBattleRoomInfo( kPacket );
	pUser->RelayPacket( kPacket );
	
	//들어온 유저에게 모든 파티원 데이터(자신 포함) 전송.(유저 정보 전송할 때 파티 레벨도 같이 보낸다.)
	vBattleRoomUser_iter iter, iEnd;
	iEnd = m_vUserNode.end();
	for( iter=m_vUserNode.begin() ; iter!=iEnd ; ++iter )
	{
		BattleRoomUser &kUser = *iter;
		SP2Packet kPacket1( STPK_BATTLEROOM_USER_INFO );
		{
			FillUserInfo( kPacket1, kUser );
		}
		pUser->RelayPacket( kPacket1 );
	}

	//모든 파티원에게 들어온 유저 데이터(자신 제외) 전송.
	SP2Packet kPacket2( STPK_BATTLEROOM_USER_INFO );
	FillUserInfo( kPacket2, kEnterUser );
	SendPacketTcp( kPacket2, kEnterUser.m_dwUserIndex );

	// 방생성 즉시 전투 생성
	CRASH_GUARD();
	if( GetOwnerName() == kEnterUser.m_szPublicID )
	{
		SP2Packet kPacket( STPK_BATTLEROOM_COMMAND );
		PACKET_GUARD_VOID( kPacket.Write(BATTLEROOM_CREATE_OK) );
		CreateBattle( kPacket );
		SendPacketTcp( kPacket );		
	}	
	
	// 참여 유저 승무패 기록 / 옵션  전송
	if( IsChangeEnterSyncData() )
	{
		SP2Packet kPacket3( STPK_BATTLEROOM_ENTERUSER_SYNC );
		FillRecord( kPacket3 );
		PACKET_GUARD_VOID( kPacket3.Write(m_bRandomTeamMode) );
		PACKET_GUARD_VOID( kPacket3.Write(m_bStartRoomEnterX) );
		PACKET_GUARD_VOID( kPacket3.Write(m_bAutoModeStart) );
		PACKET_GUARD_VOID( kPacket3.Write(m_bBadPingKick) );
		PACKET_GUARD_VOID( kPacket3.Write(m_bUseExtraOption) );
		PACKET_GUARD_VOID( kPacket3.Write(m_bNoChallenger) );
		PACKET_GUARD_VOID( kPacket3.Write(m_iChangeCharType) );
		PACKET_GUARD_VOID( kPacket3.Write(m_iCoolTimeType) );
		PACKET_GUARD_VOID( kPacket3.Write(m_iRedHPType) );
		PACKET_GUARD_VOID( kPacket3.Write(m_iBlueHPType) );
		PACKET_GUARD_VOID( kPacket3.Write(m_iDropDamageType) );
		PACKET_GUARD_VOID( kPacket3.Write(m_iGravityType) );
		PACKET_GUARD_VOID( kPacket3.Write(m_iPreSetModeType) );
		PACKET_GUARD_VOID( kPacket3.Write(m_iTeamAttackType) );
		PACKET_GUARD_VOID( kPacket3.Write(m_iGetUpType) );
		PACKET_GUARD_VOID( kPacket3.Write(m_iKOType) );
		PACKET_GUARD_VOID( kPacket3.Write(m_iRedBlowType) );
		PACKET_GUARD_VOID( kPacket3.Write(m_iBlueBlowType) );
		PACKET_GUARD_VOID( kPacket3.Write(m_iRedMoveSpeedType) );
		PACKET_GUARD_VOID( kPacket3.Write(m_iBlueMoveSpeedType) );
		PACKET_GUARD_VOID( kPacket3.Write(m_iKOEffectType) );
		PACKET_GUARD_VOID( kPacket3.Write(m_iRedEquipType) );
		PACKET_GUARD_VOID( kPacket3.Write(m_iBlueEquipType) );
		PACKET_GUARD_VOID( kPacket3.Write(m_iCatchModeRoundType) );
		PACKET_GUARD_VOID( kPacket3.Write(m_iCatchModeRoundTimeType) );
		PACKET_GUARD_VOID( kPacket3.Write(m_iGrowthUseType) );
		PACKET_GUARD_VOID( kPacket3.Write(m_iExtraItemUseType) );
		PACKET_GUARD_VOID( kPacket3.Write(m_iBattleEventType) );

		pUser->RelayPacket( kPacket3 );
	}

	if( GetOwnerName() != kEnterUser.m_szPublicID )
	{
		// 중간 참여 유저의 대전 세팅.
		CheckBattleJoin( kEnterUser );
	}

	SP2Packet kPacket4( STPK_BATTLEROOM_COMMAND );
	PACKET_GUARD_VOID( kPacket4.Write(BATTLEROOM_MODE_SEL_OK) );
	PACKET_GUARD_VOID( kPacket4.Write(m_iSelectMode) );
	PACKET_GUARD_VOID( kPacket4.Write(m_iSelectMap) );
	PACKET_GUARD_VOID( kPacket4.Write(m_bSafetyLevelRoom) );
	pUser->RelayPacket( kPacket4 );

	// 경기방 데이터 전송
	if( GetBattleEventType() == BET_TOURNAMENT_BATTLE )
	{
		SP2Packet kPacket5( STPK_BATTLEROOM_COMMAND );
		PACKET_GUARD_VOID( kPacket5.Write(BATTLEROOM_TOURNAMENT_INFO) );
		PACKET_GUARD_VOID( kPacket5.Write(m_TournamentRoundData.m_dwTourIndex) );
		PACKET_GUARD_VOID( kPacket5.Write(m_TournamentRoundData.m_dwBlueIndex) );
		PACKET_GUARD_VOID( kPacket5.Write(m_TournamentRoundData.m_dwRedIndex) );

		// 시작 남은 시간 전송
		DWORD dwGapTime = ( TIMEGETTIME() - m_TournamentRoundData.m_dwInviteTimer );
		if( dwGapTime > TournamentRoundData::INVITE_DELAY_TIME )
			dwGapTime = 0;
		else
			dwGapTime = TournamentRoundData::INVITE_DELAY_TIME - dwGapTime;

		PACKET_GUARD_VOID( kPacket5.Write(dwGapTime) );

		if( IsRegularTournamentBattle() )
		{
			LOG.PrintTimeAndLog( 0, "[대회로그] %s 대회 전투방 정보 Send : %s(%d) - 대회 : %d, 블루 : %d, 레드 : %d, 남은 시간 : %d", __FUNCTION__, pUser->GetPublicID().c_str(), 
				m_TournamentRoundData.m_dwTourIndex, m_TournamentRoundData.m_dwBlueIndex, m_TournamentRoundData.m_dwRedIndex , dwGapTime );
		}

		pUser->RelayPacket( kPacket5 );
	}

	ModeType eModeType = GetSelectIndexToMode( m_iSelectMode, m_iSelectMap );
	if( eModeType == MT_TEAM_SURVIVAL_AI )
	{
		SP2Packet kPacketAi( STPK_MACRO_COMMAND );
		PACKET_GUARD_VOID( kPacketAi.Write(MACRO_AI_LEVEL_CHANGE) );
		PACKET_GUARD_VOID( kPacketAi.Write(m_iAILevel) );
		pUser->RelayPacket( kPacketAi );
	}

	m_NodeSync.Update( BattleRoomSync::BRS_CHANGEINFO );
}

void BattleRoomNode::UserInfoUpdate( const DWORD dwUserIndex, const int iGradeLevel, const int iAbilityLevel,  const bool bSafetyLevel, const int iClientPort, const ioHashString &szTransferIP, const int iTransferPort  )
{
	vBattleRoomUser_iter iter, iEnd;
	iEnd = m_vUserNode.end();
	for( iter=m_vUserNode.begin() ; iter!=iEnd ; ++iter )
	{
		BattleRoomUser &kUser = *iter;
		if( kUser.m_dwUserIndex == dwUserIndex )
		{
			kUser.m_iGradeLevel   = iGradeLevel;
			kUser.m_iAbilityLevel = iAbilityLevel;
			kUser.m_bSafetyLevel  = bSafetyLevel;
			kUser.m_szTransferIP  = szTransferIP;
			kUser.m_iClientPort   = iClientPort;
			kUser.m_iTransferPort = iTransferPort;
			return;
		}
	}
	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "BattleRoomNode::UserInfoUpdate(%d) %d 유저 없음", GetIndex(), dwUserIndex );
}

void BattleRoomNode::UserUDPChange( const DWORD dwUserIndex, const ioHashString &szPublicID, const ioHashString &szPublicIP, const int iClientPort, 
								    const ioHashString &szPrivateIP, const ioHashString &szTransferIP, const int iTransferPort  )
{
	vBattleRoomUser_iter iter, iEnd;
	iEnd = m_vUserNode.end();
	for( iter=m_vUserNode.begin() ; iter!=iEnd ; ++iter )
	{
		BattleRoomUser &kUser = *iter;
		if( kUser.m_dwUserIndex == dwUserIndex )
		{
			kUser.m_szTransferIP  = szTransferIP;
			kUser.m_iTransferPort = iTransferPort;
			kUser.m_iClientPort   = iClientPort;
			
			if( !IsBattleModePlaying() ) // 룸에서 플레이 중이라면 이미 보냈다.
			{
				SP2Packet kPacket( STPK_CHANGE_UDP_INFO );
				PACKET_GUARD_VOID( kPacket.Write(szPublicID) );
				PACKET_GUARD_VOID( kPacket.Write(szPublicIP) );
				PACKET_GUARD_VOID( kPacket.Write(iClientPort) );
				PACKET_GUARD_VOID( kPacket.Write(szPrivateIP) );
				PACKET_GUARD_VOID( kPacket.Write(szTransferIP) );
				PACKET_GUARD_VOID( kPacket.Write(iTransferPort) );

				SendPacketTcp( kPacket, dwUserIndex );
			}			
			return;
		}
	}
}

void BattleRoomNode::UserP2PRelayInfo( const DWORD dwUserIndex, const DWORD dwRelayUserIndex, bool bRelay )
{
	BattleRoomUser &kRelayUser = GetUserNodeByIndex( dwRelayUserIndex );
	if( kRelayUser.m_szPublicID.IsEmpty() ) return;

	BattleRoomUser &kUser = GetUserNodeByIndex( dwUserIndex );
	if( kUser.m_szPublicID.IsEmpty() ) return;

	
	if( bRelay )
	{
		kUser.m_iServerRelayCount++;
	}
	else
	{
		kUser.m_iServerRelayCount--;				
	}
}

void BattleRoomNode::TournamentInfo( const DWORD dwUserIndex, const DWORD dwTeamIndex )
{
	if( GetBattleEventType() != BET_TOURNAMENT_BATTLE )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "BattleRoomNode::TournamentInfo None Type : %d - %d", GetIndex(), GetBattleEventType() );
	}
	else
	{
		if( m_TournamentRoundData.m_dwBlueIndex == dwTeamIndex )
		{
			if( IsTournamentTeam( dwUserIndex, TEAM_BLUE ) == false )
				m_TournamentRoundData.m_BlueUserIndex.push_back( dwUserIndex );
		}
		else if( m_TournamentRoundData.m_dwRedIndex == dwTeamIndex )
		{
			if( IsTournamentTeam( dwUserIndex, TEAM_RED ) == false )
				m_TournamentRoundData.m_RedUserIndex.push_back( dwUserIndex );
		}
	}
}

bool BattleRoomNode::IsTournamentTeam( DWORD dwUserIndex, TeamType eTeam )
{
	if( eTeam == TEAM_BLUE )
	{
		DWORDVec::iterator iter = m_TournamentRoundData.m_BlueUserIndex.begin();
		for(;iter != m_TournamentRoundData.m_BlueUserIndex.end();iter++)
		{
			DWORD &rkUserIndex = *iter;
			if( rkUserIndex == dwUserIndex )
				return true;
		}
	}
	else if( eTeam == TEAM_RED )
	{
		DWORDVec::iterator iter = m_TournamentRoundData.m_RedUserIndex.begin();
		for(;iter != m_TournamentRoundData.m_RedUserIndex.end();iter++)
		{
			DWORD &rkUserIndex = *iter;
			if( rkUserIndex == dwUserIndex )
				return true;
		}
	}
	return false;
}

bool BattleRoomNode::LeaveUser( const DWORD dwUserIndex, const ioHashString &szPublicID  )
{
	if( !RemoveUser( dwUserIndex ) ) return false;

	//마지막 유저가 퇴장하면 파티 해제.
	if( m_vUserNode.empty() )
	{
		if( GetBattleEventType() != BET_TOURNAMENT_BATTLE )
		{
			g_BattleRoomManager.RemoveBattleRoom( this );
		}
	}
	else
	{
		//이벤트방에서 방장이 나가면 일반 방으로 전환
		if( GetBattleEventType() == BET_BROADCAST_AFRICA ||
			GetBattleEventType() == BET_BROADCAST_MBC )      
		{
			if( szPublicID == m_OwnerUserID )
			{
				SetBattleEventType( BET_NORMAL );

				// 이벤트 타입이 변경되므로 모드 초기화하고 전체 동기화
				SetDefaultMode( BMT_TEAM_SURVIVAL );

				SP2Packet kPacket( STPK_BATTLEROOM_COMMAND );

				PACKET_GUARD_bool( kPacket.Write(BATTLEROOM_EVENT_TYPE_CHANGE) );
				PACKET_GUARD_bool( kPacket.Write(GetBattleEventType()) ); 
				PACKET_GUARD_bool( kPacket.Write(m_iSelectMode) );
				PACKET_GUARD_bool( kPacket.Write(m_iSelectMap) );
				PACKET_GUARD_bool( kPacket.Write(m_bSafetyLevelRoom) );

				SendPacketTcp( kPacket );
			}
		}

		if( IsRegularTournamentBattle() )
		{
			LOG.PrintTimeAndLog( 0, "[대회로그] %s 배틀룸 나가기 : %s(%d)", __FUNCTION__, szPublicID.c_str(), GetIndex() );
		}

		SP2Packet kPacket( STPK_BATTLEROOM_LEAVE );

		PACKET_GUARD_bool( kPacket.Write(szPublicID) );
		PACKET_GUARD_bool( kPacket.Write(GetRoomLevel()) );

		SendPacketTcp( kPacket );

		// 방장이 나가면 방장 교체
		CRASH_GUARD();
		if( szPublicID == m_OwnerUserID )
			SelectNewOwner();

		if( m_pBattleRoom )
			m_pBattleRoom->LeaveReserveUser( dwUserIndex );
	}
	m_NodeSync.Update( BattleRoomSync::BRS_CHANGEINFO );
	return true;
}

void BattleRoomNode::BattleEnterRandomTeam()
{
	if( !m_bRandomTeamMode ) return;

	// 팀을 다시 섞는다. 계급 순
	static vBattleRoomUser vUserList;
	vUserList.clear();

	vBattleRoomUser_iter iter, iEnd;
	iEnd = m_vUserNode.end();
	for( iter=m_vUserNode.begin() ; iter!=iEnd ; ++iter )
	{
		BattleRoomUser &kUser = *iter;

		if( kUser.m_bObserver )
		{
			ChangeTeamType( kUser.m_szPublicID, TEAM_NONE );
			continue;
		}

		vUserList.push_back( kUser );
	}

	std::sort( vUserList.begin(), vUserList.end(), BattleRoomUserSort() );
	int iBlueCount = m_iMaxPlayerBlue;
	int iRedCount  = m_iMaxPlayerRed;

	bool bSequenceValue = m_bRandomTeamSequence;
	m_bRandomTeamSequence = !m_bRandomTeamSequence;

	for(int i = 0;i < (int)vUserList.size();i++)
	{
		BattleRoomUser &kUser = vUserList[i];

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

void BattleRoomNode::BattleEnterRandomTeam( UserRankInfoList &rkUserRankInfoList )
{
	CRASH_GUARD();
	int i = 0;
	// 전투방에는 입장했지만 아직 룸에는 입장 못한 유저를 포함시킨다.
	// 서버이동중에 팀섞기가 완료되면 팀섞기에 해당 안되는 유저는 최초의 팀을 유지하므로 인원 불균형 발생.
	vBattleRoomUser_iter iter, iEnd;
	iEnd = m_vUserNode.end();
	for(iter = m_vUserNode.begin();iter!=iEnd;++iter)
	{
		BattleRoomUser &kUser = *iter;
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

	bool bSequenceValue = m_bRandomTeamSequence;
	m_bRandomTeamSequence = !m_bRandomTeamSequence;

	int iCheckCnt = 0;
	for( i = 0; i < iUserRankCnt; i++ )
	{
		if( IsObserver( rkUserRankInfoList[i].szName ) )
		{

			ChangeTeamType( rkUserRankInfoList[i].szName, TEAM_NONE );

			continue;
		}
		else if( bSequenceValue && iBlueTeamCount > 0 )
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

void BattleRoomNode::BattleEnterRoom( Room *pRoom )
{
	m_pBattleRoom = pRoom;

	vBattleRoomUser_iter iter, iEnd;
	iEnd = m_vUserNode.end();
	for( iter=m_vUserNode.begin() ; iter!=iEnd ; ++iter )
	{
		BattleRoomUser &kUser = *iter;
		UserParent *pItem = g_UserNodeManager.GetGlobalUserNode( kUser.m_dwUserIndex );
		if( pItem == NULL ) continue;

		if( pItem->IsUserOriginal() )
		{
			User *pUser = (User*)pItem;

			// 유저 로딩 상태 돌입
			SP2Packet kPacket( STPK_BATTLEROOM_COMMAND );
			PACKET_GUARD_VOID( kPacket.Write(BATTLEROOM_READY_GO_OK) );
			PACKET_GUARD_VOID( kPacket.Write(m_pBattleRoom->GetModeType()) );
			PACKET_GUARD_VOID( kPacket.Write(m_pBattleRoom->GetModeSubNum()) );
			PACKET_GUARD_VOID( kPacket.Write(m_pBattleRoom->GetModeMapNum()) );
			pUser->SendMessage( kPacket );

			pUser->EnterRoom( m_pBattleRoom );
		}
		else        
		{
			//타서버에 있는 유저들은 전부 서버 이동 시작
			UserCopyNode *pUser = (UserCopyNode*)pItem;
			m_pBattleRoom->EnterReserveUser( pUser->GetUserIndex() );
			SP2Packet kPacket( SSTPK_MOVING_ROOM_RESULT );
			PACKET_GUARD_VOID( kPacket.Write(SS_MOVING_ROOM_JOIN) );
			PACKET_GUARD_VOID( kPacket.Write(SS_MOVING_ROOM_JOIN_BATTLE) );
			PACKET_GUARD_VOID( kPacket.Write((int)m_pBattleRoom->GetModeType()) );
			PACKET_GUARD_VOID( kPacket.Write(m_pBattleRoom->GetRoomIndex()) );
			PACKET_GUARD_VOID( kPacket.Write(pUser->GetUserIndex()) );
			PACKET_GUARD_VOID( kPacket.Write(m_pBattleRoom->GetModeSubNum()) );
			PACKET_GUARD_VOID( kPacket.Write(m_pBattleRoom->GetModeMapNum()) );
			PACKET_GUARD_VOID( kPacket.Write((int)m_pBattleRoom->GetPlazaModeType()) );
			PACKET_GUARD_VOID( kPacket.Write(m_pBattleRoom->GetRoomNumber()) );

			pUser->SendMessage( kPacket );
		}
	}
}

void BattleRoomNode::CreateBattle( SP2Packet &rkPacket )
{	
	//
	PACKET_GUARD_VOID( rkPacket.Write(GetJoinUserCnt()) );

	int iUserCount = 0;
	vBattleRoomUser_iter iter, iEnd;
	iEnd = m_vUserNode.end();
	for(iter = m_vUserNode.begin();iter!=iEnd;++iter,++iUserCount)
	{
		BattleRoomUser &kUser = *iter;
		if( kUser.m_eTeamType == TEAM_NONE )
			kUser.m_eTeamType = CreateTeamType();

		PACKET_GUARD_VOID( rkPacket.Write(kUser.m_szPublicID) );
		PACKET_GUARD_VOID( rkPacket.Write((int)kUser.m_eTeamType) );
	}
}

TeamType BattleRoomNode::PlayRoomWantTeamType()
{
	// 모드 플레이 중 원하는 팀 타입
	if( m_pBattleRoom == NULL ) return TEAM_NONE;
	if( m_pBattleRoom->IsRoomEmpty() ) return TEAM_NONE;
	if( !m_pBattleRoom->IsRoomProcess() ) return TEAM_NONE;	

	ModeType eMode = m_pBattleRoom->GetModeType();

	if( eMode == MT_SURVIVAL || eMode == MT_BOSS || eMode == MT_GANGSI || eMode == MT_MONSTER_SURVIVAL ||
		eMode == MT_DUNGEON_A || eMode == MT_FIGHT_CLUB || eMode == MT_RAID ||
		Help::IsMonsterDungeonMode(eMode) )
	{
		return TEAM_NONE;
	}

	return m_pBattleRoom->GetNextTeamType();
}

TeamType BattleRoomNode::CreateTeamType()
{
	TeamType eWantTeam = PlayRoomWantTeamType();

	int iBlueCnt = 0;
	int iRedCnt  = 0;
	vBattleRoomUser_iter iter, iEnd;
	iEnd = m_vUserNode.end();
	for(iter = m_vUserNode.begin();iter!=iEnd;++iter)
	{
		BattleRoomUser &kUser = *iter;

		if( kUser.m_bObserver ) continue;

		if( kUser.m_eTeamType == TEAM_BLUE )
			iBlueCnt++;
		else if( kUser.m_eTeamType == TEAM_RED )
			iRedCnt++;
	}
	ModeType eModeType = GetSelectIndexToMode( m_iSelectMode, m_iSelectMap );
	if( eModeType == MT_TEAM_SURVIVAL_AI )
		return TEAM_BLUE;
	if( eWantTeam == TEAM_BLUE && iBlueCnt < m_iMaxPlayerBlue )
		return TEAM_BLUE;
	else if( eWantTeam == TEAM_RED && iRedCnt < m_iMaxPlayerRed )
		return TEAM_RED;
	else if( iRedCnt >= iBlueCnt && iBlueCnt < m_iMaxPlayerBlue )
		return TEAM_BLUE;
	else if( iRedCnt < m_iMaxPlayerRed )
		return TEAM_RED;
    return TEAM_BLUE;
}

void BattleRoomNode::CheckBattleJoin( BattleRoomUser &kCheckUser )
{
	// 파티 연습 상태 전송.
//	SP2Packet kPacket( STPK_BATTLEROOM_COMMAND );
//	kPacket << BATTLEROOM_UI_SHOW_OK << IsBattleModePlaying() << false;	
	
	UserParent *pUserParent = g_UserNodeManager.GetGlobalUserNode( kCheckUser.m_dwUserIndex );
	if( pUserParent == NULL ) 
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "BattleRoomNode::CheckBattleJoin 파티에 들어온 유저 없음(%d) : %s(%d)", GetIndex(), kCheckUser.m_szPublicID.c_str(), kCheckUser.m_dwUserIndex );		return;
	}
//	pUserParent->RelayPacket( kPacket );

	if( IsBattleModePlaying() )
	{
		// 플레이중이면 룸으로 이동시킨다.
		if( pUserParent->IsUserOriginal() )
		{
			User *pUser = (User*)pUserParent;
			// 룸 입장.
			SP2Packet kPacket1( STPK_MOVING_ROOM );
			PACKET_GUARD_VOID( kPacket1.Write(m_pBattleRoom->GetModeType()) );
			PACKET_GUARD_VOID( kPacket1.Write(m_pBattleRoom->GetModeSubNum()) );
			PACKET_GUARD_VOID( kPacket1.Write(m_pBattleRoom->GetModeMapNum()) );
			PACKET_GUARD_VOID( kPacket1.Write((int)m_pBattleRoom->GetPlazaModeType()) );

			pUser->SendMessage( kPacket1 );

			pUser->EnterRoom( m_pBattleRoom );
		}
		else
		{
			UserCopyNode *pUser = (UserCopyNode*)pUserParent;
			m_pBattleRoom->EnterReserveUser( pUser->GetUserIndex() );
			SP2Packet kPacket( SSTPK_MOVING_ROOM_RESULT );
			PACKET_GUARD_VOID( kPacket.Write(SS_MOVING_ROOM_JOIN) );
			PACKET_GUARD_VOID( kPacket.Write(SS_MOVING_ROOM_JOIN_BATTLE) );
			PACKET_GUARD_VOID( kPacket.Write((int)m_pBattleRoom->GetModeType()) );
			PACKET_GUARD_VOID( kPacket.Write(m_pBattleRoom->GetRoomIndex()) );
			PACKET_GUARD_VOID( kPacket.Write(pUser->GetUserIndex()) );
			PACKET_GUARD_VOID( kPacket.Write(m_pBattleRoom->GetModeSubNum()) );
			PACKET_GUARD_VOID( kPacket.Write(m_pBattleRoom->GetModeMapNum()) );
			PACKET_GUARD_VOID( kPacket.Write((int)m_pBattleRoom->GetPlazaModeType()) );
			PACKET_GUARD_VOID( kPacket.Write(m_pBattleRoom->GetRoomNumber()) );

			pUser->SendMessage( kPacket );
		}
	}
}

bool BattleRoomNode::IsBattleTeamChangeOK( TeamType eChangeTeam )
{
#ifdef BATTLE_MODE_BY_BCKIM // 2019-02-14 by bckim, 배틀 모드 추가	IsBattleTeamChangeOK
	if( GetSelectMode() == MT_BATTLE )
		return IsBattleTeamChangeOK_OnBattleMode(eChangeTeam);
#endif // BATTLE_MODE_BY_BCKIM // 2019-02-14 by bckim, 배틀 모드 추가

	int iTeamCount = 0;
	vBattleRoomUser_iter iter, iEnd;
	iEnd = m_vUserNode.end();
	for( iter=m_vUserNode.begin() ; iter!=iEnd ; ++iter )
	{
		BattleRoomUser &kUser = *iter;
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
		if( iTeamCount >= m_iMaxObserver )
			return false;
	}

	return true;
}

bool BattleRoomNode::IsBattleModePlaying()
{
	if( m_pBattleRoom && !m_pBattleRoom->IsRoomEmpty() )
		return true;
	return false;
}

bool BattleRoomNode::IsStartRoomEnterX() 
{
	if( m_bStartRoomEnterX == false ) return false;
	if( m_pBattleRoom == NULL ) return false;
	if( !m_pBattleRoom->IsRoomProcess() ) return false;
	if( GetPlayModeType() == MT_NONE ) return false;

	return true;
}

bool BattleRoomNode::IsUseExtraOption()
{
	if( m_bUseExtraOption == false ) return false;

	return true;
}

bool BattleRoomNode::IsNoChallenger()
{
	if( GetPlayModeType() == MT_FIGHT_CLUB )
		return m_bNoChallenger;

	return false;
}

bool BattleRoomNode::IsHidden(int iIndex)
{
	if( GetIndex() == iIndex )							return true;
	if( GetBattleEventType() == BET_BROADCAST_MBC )		return true;
	if( GetBattleEventType() == BET_TOURNAMENT_BATTLE ) return true;
	return false;
}

int BattleRoomNode::GetChangeCharType()
{
	if( !m_bUseExtraOption ) return 0;

	return m_iChangeCharType;
}

int BattleRoomNode::GetTeamAttackType()
{
	if( !m_bUseExtraOption ) return 0;

	return m_iTeamAttackType;
}

int BattleRoomNode::GetCoolTimeType()
{
	if( !m_bUseExtraOption ) return 0;

	return m_iCoolTimeType;
}

int BattleRoomNode::GetRedHPType()
{
	if( !m_bUseExtraOption ) return 0;

	return m_iRedHPType;
}

int BattleRoomNode::GetBlueHPType()
{
	if( !m_bUseExtraOption ) return 0;

	return m_iBlueHPType;
}

int BattleRoomNode::GetDropDamageType()
{
	if( !m_bUseExtraOption ) return 0;

	return m_iDropDamageType;
}

int BattleRoomNode::GetGravityType()
{
	if( !m_bUseExtraOption ) return 0;

	return m_iGravityType;
}

int BattleRoomNode::GetGetUpType()
{
	if( !m_bUseExtraOption ) return 0;

	return m_iGetUpType;
}

int BattleRoomNode::GetRedMoveSpeedType()
{
	if( !m_bUseExtraOption ) return 0;

	return m_iRedMoveSpeedType;
}

int BattleRoomNode::GetBlueMoveSpeedType()
{
	if( !m_bUseExtraOption ) return 0;

	return m_iBlueMoveSpeedType;
}

int BattleRoomNode::GetRedEquipType()
{
	if( !m_bUseExtraOption ) return 0;

	return m_iRedEquipType;
}

int BattleRoomNode::GetBlueEquipType()
{
	if( !m_bUseExtraOption ) return 0;

	return m_iBlueEquipType;
}

int BattleRoomNode::GetKOType()
{
	if( !m_bUseExtraOption ) return 0;

	return m_iKOType;
}

int BattleRoomNode::GetKOEffectType()
{
	if( !m_bUseExtraOption ) return 0;

	return m_iKOEffectType;
}

int BattleRoomNode::GetRedBlowType()
{
	if( !m_bUseExtraOption ) return 0;

	return m_iRedBlowType;
}

int BattleRoomNode::GetBlueBlowType()
{
	if( !m_bUseExtraOption ) return 0;

	return m_iBlueBlowType;
}

int BattleRoomNode::GetPreSetModeType()
{
	if( !m_bUseExtraOption ) return 0;

	return m_iPreSetModeType;
}

int BattleRoomNode::GetCatchModeRoundType()
{
	if( !m_bUseExtraOption ) return -1;

	return m_iCatchModeRoundType;
}

int BattleRoomNode::GetCatchModeRoundTimeType()
{
	if( !m_bUseExtraOption ) return -1;

	return m_iCatchModeRoundTimeType;
}

int BattleRoomNode::GetGrowthUseType()
{
	if( !m_bUseExtraOption ) return 0;

	return m_iGrowthUseType;
}

int BattleRoomNode::GetExtraItemUseType()
{
	if( !m_bUseExtraOption ) return 0;

	return m_iExtraItemUseType;
}

int BattleRoomNode::GetPlayModeType()
{
	if( !m_pBattleRoom || m_pBattleRoom->IsRoomEmpty() ) return MT_NONE;
	if( m_pBattleRoom->GetModeType() == MT_TRAINING ) return MT_NONE;
	if( m_pBattleRoom->GetModeType() == MT_HEADQUARTERS ) return MT_NONE;
	if( m_pBattleRoom->GetModeType() == MT_NONE ) return MT_NONE;
	if( m_pBattleRoom->GetModeType() == MT_PRACTICE ) return MT_NONE;

	return (int)m_pBattleRoom->GetModeType();
}

void BattleRoomNode::SetDefaultMode( int iSelectModeTerm )
{
	switch( iSelectModeTerm )
	{
	case BMT_UNDERWEAR:					//포로탈출
		m_iSelectMode = g_BattleRoomManager.GetBattleModeToIndex( MT_UNDERWEAR, false, (GetBattleEventType() == BET_BROADCAST_MBC) );
		m_iSelectMap  = -1;
		break;
	case BMT_CBT:					//포로탈출
		m_iSelectMode = g_BattleRoomManager.GetBattleModeToIndex( MT_CBT, false, (GetBattleEventType() == BET_BROADCAST_MBC) );
		m_iSelectMap  = -1;
		break;
	case BMT_CATCH:					//포로탈출
		m_iSelectMode = g_BattleRoomManager.GetBattleModeToIndex( MT_CATCH, false, (GetBattleEventType() == BET_BROADCAST_MBC) );
		m_iSelectMap  = -1;
		break;
	case BMT_CATCH_BOSS:            //포로탈출 - 보스
		m_iSelectMode = g_BattleRoomManager.GetBattleModeToIndex( MT_CATCH, false, (GetBattleEventType() == BET_BROADCAST_MBC) );
		m_iSelectMap  = -1;
		break;
	case BMT_CATCH_RUNNINGMAN:					//포로탈출 런닝맨
		m_iSelectMode = g_BattleRoomManager.GetBattleModeToIndex( MT_CATCH_RUNNINGMAN, false, (GetBattleEventType() == BET_BROADCAST_MBC) );
		m_iSelectMap  = -1;
		break;
	case BMT_CATCH_RUNNINGMAN_BOSS:            //포로탈출 런닝맨 - 보스
		m_iSelectMode = g_BattleRoomManager.GetBattleModeToIndex( MT_CATCH_RUNNINGMAN, false, (GetBattleEventType() == BET_BROADCAST_MBC) );
		m_iSelectMap  = -1;
		break;
	case BMT_STONE:                 //상징물
		m_iSelectMode = g_BattleRoomManager.GetBattleModeToIndex( MT_SYMBOL, false, (GetBattleEventType() == BET_BROADCAST_MBC) );
		m_iSelectMap  = -1;
		break;
	case BMT_STONE_BOSS:            //상징물 - 보스
		m_iSelectMode = g_BattleRoomManager.GetBattleModeToIndex( MT_SYMBOL, false, (GetBattleEventType() == BET_BROADCAST_MBC) );
		m_iSelectMap  = -1;
		break;
	case BMT_KING: 					//히든 크라운
		m_iSelectMode = g_BattleRoomManager.GetBattleModeToIndex( MT_KING, false, (GetBattleEventType() == BET_BROADCAST_MBC) );
		m_iSelectMap  = -1;
		break;
	case BMT_KING_BOSS: 			//히든 크라운 - 보스
		m_iSelectMode = g_BattleRoomManager.GetBattleModeToIndex( MT_KING, false, (GetBattleEventType() == BET_BROADCAST_MBC) );
		m_iSelectMap  = -1;
		break;
	case BMT_SURVIVAL:              //스틸&서바이벌
		m_iSelectMode = g_BattleRoomManager.GetBattleModeToIndex( MT_SURVIVAL, false, (GetBattleEventType() == BET_BROADCAST_MBC) );
		m_iSelectMap  = -1;
		break;
	case BMT_FIGHT_CLUB:            //파이트클럽
		m_iSelectMode = g_BattleRoomManager.GetBattleModeToIndex( MT_FIGHT_CLUB, false, (GetBattleEventType() == BET_BROADCAST_MBC) );
		m_iSelectMap  = -1;
		break;
	case BMT_TEAM_SURVIVAL:              //팀 데스매치
		m_iSelectMode = g_BattleRoomManager.GetBattleModeToIndex( MT_TEAM_SURVIVAL, false, (GetBattleEventType() == BET_BROADCAST_MBC) );
		m_iSelectMap  = -1;
		break;
	case BMT_TEAM_SURVIVAL_BOSS:              //팀 데스매치 - 보스
		m_iSelectMode = g_BattleRoomManager.GetBattleModeToIndex( MT_TEAM_SURVIVAL, false, (GetBattleEventType() == BET_BROADCAST_MBC) );
		m_iSelectMap  = -1;
		break;
	case BMT_TEAM_SURVIVAL_FIRST:        //팀 데스매치 - 초보
		m_iSelectMode = g_BattleRoomManager.GetBattleModeToIndex( MT_TEAM_SURVIVAL, true, (GetBattleEventType() == BET_BROADCAST_MBC) );
		m_iSelectMap  = -1;
		m_bSafetyLevelRoom = true;
		break;
	case BMT_TEAM_SURVIVAL_FIRST_BOSS:        //팀 데스매치 - 초보 - 보스
		m_iSelectMode = g_BattleRoomManager.GetBattleModeToIndex( MT_TEAM_SURVIVAL, true, (GetBattleEventType() == BET_BROADCAST_MBC) );
		m_iSelectMap  = -1;
		m_bSafetyLevelRoom = true;
		break;
	case BMT_BOSS:              //보스
		m_iSelectMode = g_BattleRoomManager.GetBattleModeToIndex( MT_BOSS, false, (GetBattleEventType() == BET_BROADCAST_MBC) );
		m_iSelectMap  = -1;
		break;
	case BMT_MONSTER_SURVIVAL_EASY:     //PvE - 랜덤 맵이 없다
	case BMT_MONSTER_SURVIVAL_NORMAL:
	case BMT_MONSTER_SURVIVAL_HARD:
		m_iSelectMode = g_BattleRoomManager.GetBattleModeToIndex( MT_MONSTER_SURVIVAL, false, (GetBattleEventType() == BET_BROADCAST_MBC) );
		m_iSelectMap  = ( iSelectModeTerm - BMT_MONSTER_SURVIVAL_EASY ) + 1;       
		break;
	case BMT_USER_CUSTOM:                 // 유저 모드 
		m_iSelectMode = g_BattleRoomManager.GetBattleModeToIndex( MT_CATCH, false, (GetBattleEventType() == BET_BROADCAST_MBC) );
		m_iSelectMap  = -1;
		break;
	case BMT_FOOTBALL:                 //축구
		m_iSelectMode = g_BattleRoomManager.GetBattleModeToIndex( MT_FOOTBALL, false, (GetBattleEventType() == BET_BROADCAST_MBC) );
		m_iSelectMap  = -1;
		break;
	case BMT_FOOTBALL_BOSS:            //축구보스
		m_iSelectMode = g_BattleRoomManager.GetBattleModeToIndex( MT_FOOTBALL, false, (GetBattleEventType() == BET_BROADCAST_MBC) );
		m_iSelectMap  = -1;
		break;
	case BMT_GANGSI:				   //강시
		m_iSelectMode = g_BattleRoomManager.GetBattleModeToIndex( MT_GANGSI, false, (GetBattleEventType() == BET_BROADCAST_MBC) );
		m_iSelectMap  = -1;
		break;
	case BMT_DUNGEON_A_EASY:     //PvE - 랜덤 맵이 없다
		m_iSelectMode = g_BattleRoomManager.GetBattleModeToIndex( MT_DUNGEON_A, false, (GetBattleEventType() == BET_BROADCAST_MBC) );
		m_iSelectMap  = ( iSelectModeTerm - BMT_DUNGEON_A_EASY ) + 1;       
		break;
	case BMT_TOWER_DEFENSE_EASY:     //타워 디펜스
	case BMT_TOWER_DEFENSE_NORMAL:
	case BMT_TOWER_DEFENSE_HARD:
	case BMT_TOWER_DEFENSE_CHALLENGE:
		m_iSelectMode = g_BattleRoomManager.GetBattleModeToIndex( MT_TOWER_DEFENSE, false, (GetBattleEventType() == BET_BROADCAST_MBC) );
		m_iSelectMap  = ( iSelectModeTerm - BMT_TOWER_DEFENSE_EASY ) + 1;       
		break;
	case BMT_DARK_XMAS_EASY:
	case BMT_DARK_XMAS_NORMAL:
	case BMT_DARK_XMAS_HARD:
	case BMT_DARK_XMAS_CHALLENGE:
		m_iSelectMode = g_BattleRoomManager.GetBattleModeToIndex( MT_DARK_XMAS, false, (GetBattleEventType() == BET_BROADCAST_MBC) );
		m_iSelectMap  = ( iSelectModeTerm - BMT_DARK_XMAS_EASY ) + 1;
		break;
	case BMT_FIRE_TEMPLE_EASY:
	case BMT_FIRE_TEMPLE_NORMAL:
	case BMT_FIRE_TEMPLE_HARD:
	case BMT_FIRE_TEMPLE_CHALLENGE:
		m_iSelectMode = g_BattleRoomManager.GetBattleModeToIndex( MT_FIRE_TEMPLE, false, (GetBattleEventType() == BET_BROADCAST_MBC) );
		m_iSelectMap  = ( iSelectModeTerm - BMT_FIRE_TEMPLE_EASY ) + 1;       
		break;
	case BMT_DOBULE_CROWN:
		m_iSelectMode = g_BattleRoomManager.GetBattleModeToIndex( MT_DOBULE_CROWN, false, (GetBattleEventType() == BET_BROADCAST_MBC) );
		m_iSelectMap  = -1;
		break;
	case BMT_DOBULE_CROWN_BOSS:
		m_iSelectMode = g_BattleRoomManager.GetBattleModeToIndex( MT_DOBULE_CROWN, false, (GetBattleEventType() == BET_BROADCAST_MBC) );
		m_iSelectMap  = -1;
		break;
	case BMT_FACTORY_EASY:    
	case BMT_FACTORY_NORMAL:
	case BMT_FACTORY_HARD:
	case BMT_FACTORY_CHALLENGE:
		m_iSelectMode = g_BattleRoomManager.GetBattleModeToIndex( MT_FACTORY, false, (GetBattleEventType() == BET_BROADCAST_MBC) );
		m_iSelectMap  = ( iSelectModeTerm - BMT_FACTORY_EASY ) + 1;       
		break;
	case BMT_TEAM_SURVIVAL_AI_EASY:		
	case BMT_TEAM_SURVIVAL_AI_HARD:
		if( iSelectModeTerm == BMT_TEAM_SURVIVAL_AI_EASY )
			m_iAILevel = 0;
		else
			m_iAILevel = 1;
		m_iSelectMode = g_BattleRoomManager.GetBattleModeToIndex( MT_TEAM_SURVIVAL_AI, false, (GetBattleEventType() == BET_BROADCAST_MBC) );
		m_iSelectMap  = 1;
		break;
	case BMT_RAID:
		m_iSelectMode = g_BattleRoomManager.GetBattleModeToIndex( MT_RAID, false, (GetBattleEventType() == BET_BROADCAST_MBC) );
		m_iSelectMap  = 1;       
		break;
	case BMT_SUCCESSION:	//일대일모드
		m_iSelectMode = g_BattleRoomManager.GetBattleModeToIndex( MT_SUCCESSION, false, (GetBattleEventType() == BET_BROADCAST_MBC) );
		m_iSelectMap  = -1;
		break;

#ifdef	FLAG_MODE_BY_BCKIM			// 2018-03-15 by bckim, 깃발모드 추가  BattleRoomNode::SetDefaultMode
	case BMT_FLAG:
		m_iSelectMode = g_BattleRoomManager.GetBattleModeToIndex( MT_FLAG, false, (GetBattleEventType() == BET_BROADCAST_MBC) );
		m_iSelectMap  = -1;
		break;
#endif
#ifdef ARENA_MODE_BY_BCKIM		// 2018-07-25 by bckim, 아레나 모드 추가	case BMT_ARENA:
	case BMT_ARENA:
		m_iSelectMode = g_BattleRoomManager.GetBattleModeToIndex( MT_ARENA, false, (GetBattleEventType() == BET_BROADCAST_MBC) );
		m_iSelectMap  = -1;
		break;
#endif 	// ARENA_MODE_BY_BCKIM	// End. 2018-07-25 by bckim, 아레나 모드 추가

#ifdef BATTLE_MODE_BY_BCKIM		// 2019-02-14 by bckim, 배틀 모드 추가
	case BMT_BATTLE_4P:
	case BMT_BATTLE_6P:
		m_iSelectMode = g_BattleRoomManager.GetBattleModeToIndex( MT_BATTLE, false, (GetBattleEventType() == BET_BROADCAST_MBC) );
		m_iSelectMap  = ( iSelectModeTerm - BMT_BATTLE_6P ) + 1;
		break;
#endif 	// BATTLE_MODE_BY_BCKIM		// End. 2019-02-14 by bckim, 배틀 모드 추가

#ifdef FARMING_MODE_BY_BCKIM		// 2019-07-03 by bckim, 파밍모드 추가
	case BMT_FARMING:
		m_iSelectMode = g_BattleRoomManager.GetBattleModeToIndex( MT_FARMING, false, (GetBattleEventType() == BET_BROADCAST_MBC) );
		m_iSelectMap  = -1;
		break;
#endif 	// FARMING_MODE_BY_BCKIM		// 2019-07-03 by bckim, 파밍모드 추가

	default:
		m_iSelectMode = -1;
		m_iSelectMap  = -1;
		break;
	}

	if( GetBattleEventType() == BET_BROADCAST_MBC )
		m_iSelectMap = 1;
}

int BattleRoomNode::GetSelectModeTerm()
{
	if( m_iSelectMode == -1 )
	{
		if( GetMaxPlayerBlue() != GetMaxPlayerRed() )
			return BMT_RANDOM_BOSS;
		else
			return BMT_RANDOM;
	}

	ModeType eSelectMode = GetSelectIndexToMode( GetSelectMode(), GetSelectMap() );
	if( eSelectMode == MT_NONE ) 
	{
		if( GetMaxPlayerBlue() != GetMaxPlayerRed() )
			return BMT_RANDOM_BOSS;
		else
			return BMT_RANDOM;
	}
	else if( eSelectMode == MT_CATCH )
	{
		if( IsUseExtraOption() )
			return BMT_USER_CUSTOM;
        else if( GetMaxPlayerBlue() != GetMaxPlayerRed() )
			return BMT_CATCH_BOSS;
		else
			return BMT_CATCH;
	}
	else if( eSelectMode == MT_UNDERWEAR )
	{
		return BMT_UNDERWEAR;
	}
	else if( eSelectMode == MT_CBT )
	{
		return BMT_CBT;
	}
	else if( eSelectMode == MT_CATCH_RUNNINGMAN )
	{
		if( IsUseExtraOption() )
			return BMT_USER_CUSTOM;
		else if( GetMaxPlayerBlue() != GetMaxPlayerRed() )
			return BMT_CATCH_RUNNINGMAN_BOSS;
		else
			return BMT_CATCH_RUNNINGMAN;
	}
	else if( eSelectMode == MT_SYMBOL )
	{
		if( GetMaxPlayerBlue() != GetMaxPlayerRed() )
			return BMT_STONE_BOSS;
		else
			return BMT_STONE;
	}    
	else if( eSelectMode == MT_KING )
	{
		if( GetMaxPlayerBlue() != GetMaxPlayerRed() )
			return BMT_KING_BOSS;
		else
			return BMT_KING;
	}
	else if( eSelectMode == MT_SURVIVAL )
	{
		return BMT_SURVIVAL;
	}
	else if( eSelectMode == MT_FIGHT_CLUB )
	{
		return BMT_FIGHT_CLUB;
	}
	else if( eSelectMode == MT_TEAM_SURVIVAL )
	{
		if( m_bSafetyLevelRoom )
		{
			if( GetMaxPlayerBlue() != GetMaxPlayerRed() )
				return BMT_TEAM_SURVIVAL_FIRST_BOSS;
			else
				return BMT_TEAM_SURVIVAL_FIRST;
		}
		else
		{
			if( IsUseExtraOption() )
				return BMT_USER_CUSTOM;
			else if( GetMaxPlayerBlue() != GetMaxPlayerRed() )
				return BMT_TEAM_SURVIVAL_BOSS;
			else
				return BMT_TEAM_SURVIVAL;
		}
	}
	else if( eSelectMode == MT_TEAM_SURVIVAL_AI )
	{
		if( m_iAILevel == 0 )
			return BMT_TEAM_SURVIVAL_AI_EASY;
		return BMT_TEAM_SURVIVAL_AI_HARD;
	}
	else if( eSelectMode == MT_BOSS )
	{
		return BMT_BOSS;
	}
	else if( eSelectMode == MT_MONSTER_SURVIVAL )
	{
		int iReturnBMT = BMT_MONSTER_SURVIVAL_EASY + ( m_iSelectMap - 1 );
		if( !COMPARE( iReturnBMT, BMT_MONSTER_SURVIVAL_EASY, BMT_MONSTER_SURVIVAL_HARD + 1 ) )
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "BattleRoomNode::GetSelectModeTerm Error : %d - %d", iReturnBMT, m_iSelectMap );
		else
			return iReturnBMT;
	}
	else if( eSelectMode == MT_FOOTBALL )
	{
		if( GetMaxPlayerBlue() != GetMaxPlayerRed() )
			return BMT_FOOTBALL_BOSS;
		else
			return BMT_FOOTBALL;
	}   
	else if( eSelectMode == MT_GANGSI )
	{
		return BMT_GANGSI;
	}
	else if( eSelectMode == MT_DUNGEON_A )
	{
		int iReturnBMT = BMT_DUNGEON_A_EASY + ( m_iSelectMap - 1 );
		return iReturnBMT;
	}
	else if( eSelectMode == MT_TOWER_DEFENSE )
	{
		int iReturnBMT = BMT_TOWER_DEFENSE_EASY + ( m_iSelectMap - 1 );
		if( !COMPARE( iReturnBMT, BMT_TOWER_DEFENSE_EASY, BMT_TOWER_DEFENSE_CHALLENGE + 1 ) )
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "BattleRoomNode::GetSelectModeTerm Error : %d - %d", iReturnBMT, m_iSelectMap );
		else
			return iReturnBMT;
	}
	else if( eSelectMode == MT_DARK_XMAS )
	{
		int iReturnBMT = BMT_DARK_XMAS_EASY + ( m_iSelectMap - 1 );
		if( !COMPARE( iReturnBMT, BMT_DARK_XMAS_EASY, BMT_DARK_XMAS_CHALLENGE + 1 ) )
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "BattleRoomNode::GetSelectModeTerm Error : %d - %d", iReturnBMT, m_iSelectMap );
		else
			return iReturnBMT;
	}
	else if( eSelectMode == MT_FIRE_TEMPLE )
	{
		int iReturnBMT = BMT_FIRE_TEMPLE_EASY + ( m_iSelectMap - 1 );
		if( !COMPARE( iReturnBMT, BMT_FIRE_TEMPLE_EASY, BMT_FIRE_TEMPLE_CHALLENGE + 1 ) )
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "BattleRoomNode::GetSelectModeTerm Error : %d - %d", iReturnBMT, m_iSelectMap );
		else
			return iReturnBMT;
	}
	else if( eSelectMode == MT_DOBULE_CROWN )
	{		
		if( GetMaxPlayerBlue() != GetMaxPlayerRed() )
			return BMT_DOBULE_CROWN_BOSS;
		else
			return BMT_DOBULE_CROWN;
	}
	else if( eSelectMode == MT_FACTORY )
	{
		int iReturnBMT = BMT_FACTORY_EASY + ( m_iSelectMap - 1 );
		if( !COMPARE( iReturnBMT, BMT_FACTORY_EASY, BMT_FACTORY_CHALLENGE + 1 ) )
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "BattleRoomNode::GetSelectModeTerm Error : %d - %d", iReturnBMT, m_iSelectMap );
		else
			return iReturnBMT;
	}
	else if( eSelectMode == MT_RAID )
	{
		int iReturnBMT = BMT_RAID;
		return iReturnBMT;
	}
	else if( eSelectMode == MT_SUCCESSION )
	{
		return BMT_SUCCESSION;
	}
#ifdef FLAG_MODE_BY_BCKIM
	else if( eSelectMode == MT_FLAG )
	{
		return BMT_FLAG;
	}
#endif // FLAG_MODE_BY_BCKIM

#ifdef ARENA_MODE_BY_BCKIM		// 2018-07-25 by bckim, 아레나 모드 추가	BattleRoomNode::GetSelectModeTerm()
	else if( eSelectMode == MT_ARENA )
	{
		return BMT_ARENA;
	}
#endif 	// ARENA_MODE_BY_BCKIM	// End. 2018-07-25 by bckim, 아레나 모드 추가

#ifdef BATTLE_MODE_BY_BCKIM		// 2019-02-14 by bckim, 배틀 모드 추가   BattleRoomNode::GetSelectModeTerm()
	else if( eSelectMode == MT_BATTLE )
	{
		int iReturnBMT = BMT_BATTLE_6P + ( m_iSelectMap - 1 );
		if( !COMPARE( iReturnBMT, BMT_BATTLE_6P, BMT_BATTLE_4P + 1 ) )
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "BattleRoomNode::GetSelectModeTerm Error : %d - %d", iReturnBMT, m_iSelectMap );
		else
			return iReturnBMT;
	}
#endif 	// BATTLE_MODE_BY_BCKIM	// End. 2019-02-14 by bckim, 배틀 모드 추가
#ifdef FARMING_MODE_BY_BCKIM		// 2019-07-03 by bckim, 파밍모드 추가
	else if( eSelectMode == MT_FARMING )
	{
		return BMT_FARMING;
	}
#endif 	// FARMING_MODE_BY_BCKIM		// 2019-07-03 by bckim, 파밍모드 추가

	return BMT_RANDOM;
}

bool BattleRoomNode::IsBattleTimeClose()
{
	if( !m_pBattleRoom || m_pBattleRoom->IsRoomEmpty() ) return false;
	if( m_pBattleRoom->GetModeType() == MT_TRAINING ) return false;
	if( m_pBattleRoom->GetModeType() == MT_HEADQUARTERS ) return false;
	if( m_pBattleRoom->GetModeType() == MT_HOUSE ) return false;
	if( !m_pBattleRoom->IsRoomProcess() ) return false;
	
	return m_pBattleRoom->IsTimeCloseRoom();
}

void BattleRoomNode::AddUser(const BattleRoomUser &kUser )
{
	m_vUserNode.push_back( kUser );
}

bool BattleRoomNode::RemoveUser( const DWORD dwUserIndex )
{
    vBattleRoomUser_iter iter, iEnd;
	iEnd = m_vUserNode.end();
	for(iter=m_vUserNode.begin();iter!=iEnd;++iter)
	{
		BattleRoomUser &kUser = *iter;
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

void BattleRoomNode::SelectNewOwner()
{
	m_OwnerUserID.Clear();
	if( m_vUserNode.empty() ) return;

	ioHashString szObserver;

    vBattleRoomUser_iter iter, iEnd;
	iEnd = m_vUserNode.end();
	for(iter=m_vUserNode.begin();iter!=iEnd;++iter)
	{
		BattleRoomUser &kUser = *iter;
		if( !kUser.m_bObserver )
		{
			m_OwnerUserID = kUser.m_szPublicID;
			break;
		}
		else if( szObserver.IsEmpty() )
		{
			szObserver = kUser.m_szPublicID;
		}
	}

	if( m_OwnerUserID.IsEmpty() && !szObserver.IsEmpty() )
	{
		m_OwnerUserID = szObserver;
		//방장이 교체되면 비번 제거
		SetPW( "" );
		SP2Packet kPacket( STPK_BATTLEROOM_INFO );
		FillBattleRoomInfo( kPacket );
		SendPacketTcp( kPacket );		
		m_NodeSync.Update( BattleRoomSync::BRS_CHANGEINFO );

		if( IsRegularTournamentBattle() )
		{
			LOG.PrintTimeAndLog( 0, "[대회로그] %s 옵저버가 방장이 됨: %s", __FUNCTION__, m_OwnerUserID.c_str() );
		}
	}
	else if( !m_OwnerUserID.IsEmpty() )
	{
		//방장이 교체되면 비번 제거
		SetPW( "" );
		SP2Packet kPacket( STPK_BATTLEROOM_INFO );
		FillBattleRoomInfo( kPacket );
		SendPacketTcp( kPacket );		
		m_NodeSync.Update( BattleRoomSync::BRS_CHANGEINFO );

		if( IsRegularTournamentBattle() )
		{
			LOG.PrintTimeAndLog( 0, "[대회로그] %s 방장이 바뀜: %s", __FUNCTION__, m_OwnerUserID.c_str() );
		}
	}
}

ModeType BattleRoomNode::GetSelectIndexToMode( int iModeIndex, int iMapIndex )
{
	ModeType eModeType = MT_NONE;
	int      iSubType  = 0;

	if( iModeIndex != -1 )
		g_BattleRoomManager.CheckBattleRandomMode( iModeIndex, iMapIndex, eModeType, iSubType );	
	return eModeType;
}

bool BattleRoomNode::CheckUserMode( int iModeIndex, int iMapIndex )
{
	if( iModeIndex != -1 )
		return g_BattleRoomManager.CheckBattleModeUserMode( iModeIndex, iMapIndex);	
	return false;
}

bool BattleRoomNode::IsFreeForAllMode( int iModeIndex, int iMapIndex )
{
	ModeType eModeType = GetSelectIndexToMode( iModeIndex, iMapIndex );
	if( eModeType == MT_SURVIVAL || eModeType == MT_MONSTER_SURVIVAL || eModeType == MT_DUNGEON_A || 
		eModeType == MT_FIGHT_CLUB || eModeType == MT_RAID ||
		Help::IsMonsterDungeonMode(eModeType) )
		return true;
	return false;
}

bool BattleRoomNode::IsSafetyModeCreate()
{
	vBattleRoomUser_iter iter, iEnd;
	iEnd = m_vUserNode.end();
	for(iter=m_vUserNode.begin();iter!=iEnd;++iter)
	{
		BattleRoomUser &kUser = *iter;
		if( !kUser.m_bSafetyLevel )
			return false;
	}
	return true;
}

bool BattleRoomNode::KickOutUser( ioHashString &rkName )
{
	CRASH_GUARD();
	vBattleRoomUser_iter iter, iEnd;
	iEnd = m_vUserNode.end();
	for(iter=m_vUserNode.begin();iter!=iEnd;++iter)
	{
		BattleRoomUser &kUser = *iter;
		if( kUser.m_szPublicID == rkName )
		{
			SP2Packet kPacket( STPK_MACRO_COMMAND );
			PACKET_GUARD_bool( kPacket.Write(MACRO_KICK_OUT) );
			PACKET_GUARD_bool( kPacket.Write(kUser.m_szPublicID) );

			SendPacketTcp( kPacket );			
			
			UserParent *pUserParent = g_UserNodeManager.GetGlobalUserNode( rkName );
			if( pUserParent )
			{
				if( pUserParent->IsUserOriginal() )
				{
					User *pUser = (User*)pUserParent;
					pUser->BattleRoomKickOut();
				}            
				else
				{
					UserCopyNode *pUser = (UserCopyNode*)pUserParent;
					SP2Packet kPacket( SSTPK_BATTLEROOM_KICK_OUT );
					PACKET_GUARD_bool( kPacket.Write(kUser.m_dwUserIndex) );
					pUser->SendMessage( kPacket );
				}
			}    
			return true;
		}
	}
	return false;
}

int BattleRoomNode::GetJoinUserCnt() const
{
	return m_vUserNode.size();
}

int BattleRoomNode::GetPlayUserCnt()
{
	int iPlayUserCnt = 0;
	vBattleRoomUser_iter iter, iEnd;
	iEnd = m_vUserNode.end();
	for(iter = m_vUserNode.begin();iter!=iEnd;++iter)
	{
		BattleRoomUser &kUser = *iter;

		if( kUser.m_bObserver ) continue;

		iPlayUserCnt++;
	}

	return iPlayUserCnt;
}

int BattleRoomNode::GetMaxPlayer() const
{
	return m_iMaxPlayerBlue + m_iMaxPlayerRed;
}

int BattleRoomNode::GetMaxPlayerBlue() const
{
	return m_iMaxPlayerBlue;
}

int BattleRoomNode::GetMaxPlayerRed() const
{
	return m_iMaxPlayerRed;
}

int BattleRoomNode::GetMaxObserver() const
{
	return m_iMaxObserver;
}

int BattleRoomNode::GetSelectMode() const
{
	return m_iSelectMode;
}

int BattleRoomNode::GetSelectMap() const
{
	return m_iSelectMap;
}

void BattleRoomNode::SetName( const ioHashString &rkName )
{
	if( m_szRoomName != rkName )
		m_NodeSync.Update( BattleRoomSync::BRS_CHANGEINFO );
	m_szRoomName = rkName;
}

void BattleRoomNode::SetPW( const ioHashString &rkPW )
{
	if( m_szRoomPW != rkPW )
		m_NodeSync.Update( BattleRoomSync::BRS_CHANGEINFO );
	m_szRoomPW = rkPW;
}

void BattleRoomNode::SetMaxPlayer( int iBluePlayer, int iRedPlayer, int iObserver )
{
#ifdef BATTLE_MODE_BY_BCKIM // 2019-02-14 by bckim, 배틀 모드 추가	IsBattleTeamChangeOK
	if( GetSelectMode() == MT_BATTLE && 1 == GetSelectMap() )
	{
		iBluePlayer = 3;
		iRedPlayer = 3;			
	}
	if( GetSelectMode() == MT_BATTLE && 2 == GetSelectMap() )
	{
		iBluePlayer = 2;
		iRedPlayer = 2;		
	}	
#endif // BATTLE_MODE_BY_BCKIM // 2019-02-14 by bckim, 배틀 모드 추가

	if( m_iMaxPlayerBlue != iBluePlayer || m_iMaxPlayerRed  != iRedPlayer || m_iMaxObserver != iObserver )
		m_NodeSync.Update( BattleRoomSync::BRS_CHANGEINFO );
	
	m_iMaxPlayerBlue = iBluePlayer;
    m_iMaxPlayerRed  = iRedPlayer;
	m_iMaxObserver = iObserver;
}

int BattleRoomNode::GetAbilityMatchLevel()
{
	if( m_bSafetyLevelRoom ) return 0;

	int iSize  = 0;
	int iLevel = 0;
	int iObserverSize = 0;
	int iObserverLevel = 0;

	vBattleRoomUser_iter iter, iEnd;
	iEnd = m_vUserNode.end();

	for(iter=m_vUserNode.begin();iter!=iEnd;++iter)
	{
		BattleRoomUser &kUser = *iter;
		// 옵저버 제외 레벨
		if( !g_UserNodeManager.IsDeveloper( kUser.m_szPublicID.c_str() ) && !kUser.m_bObserver )
		{
			iSize++;
			iLevel += kUser.m_iAbilityLevel;
		}
	}

	for(iter=m_vUserNode.begin();iter!=iEnd;++iter)
	{
		BattleRoomUser &kUser = *iter;
		// 옵저버 레벨
		if( !g_UserNodeManager.IsDeveloper( kUser.m_szPublicID.c_str() ) && kUser.m_bObserver )
		{
			iObserverSize++;
			iObserverLevel += kUser.m_iAbilityLevel;
		}
	}

	if( iSize <= 0 && iObserverSize > 0 )
	{
		iObserverLevel /= iObserverSize;
		iObserverLevel = min( max( iObserverLevel, 0 ), g_LevelMatchMgr.GetRoomEnterLevelMax() );
		return iObserverLevel;
	}
	else if( iSize <= 0 && iObserverSize <= 0 )
	{
		return 0;
	}

	iLevel /= iSize;
	iLevel = min( max( iLevel, 0 ), g_LevelMatchMgr.GetRoomEnterLevelMax() );
	return iLevel;
}

int BattleRoomNode::GetRoomLevel()
{
	int iMatchLevel = GetAbilityMatchLevel() - g_LevelMatchMgr.GetAddGradeLevel();
	iMatchLevel = min( max( iMatchLevel, 0 ), g_LevelMgr.GetMaxGradeLevel() );

	return iMatchLevel;
}

int BattleRoomNode::GetSortLevelPoint( int iMyLevel )
{
	int iSize = m_vUserNode.size();
	if( iSize == 0 )
		return BATTLE_ROOM_SORT_HALF_POINT;

	int iGapLevel = 5;
	int iLevel    = 0;
	vBattleRoomUser_iter iter, iEnd;
	iEnd = m_vUserNode.end();
	for(iter=m_vUserNode.begin();iter!=iEnd;++iter)
	{
		BattleRoomUser &kUser = *iter;
		iLevel += kUser.m_iAbilityLevel;
	}
	iLevel /= iSize;
	return ( abs( iLevel - iMyLevel ) / iGapLevel );		
}

int BattleRoomNode::GetBattleEventType()
{
	return m_iBattleEventType;
}

bool BattleRoomNode::IsLevelMatchIgnore()
{
	if( GetBattleEventType() == BET_BROADCAST_AFRICA ||
		GetBattleEventType() == BET_BROADCAST_MBC || 
		GetBattleEventType() == BET_TOURNAMENT_BATTLE )
		return true;
	return false;
}

void BattleRoomNode::SetBattleEventType( int iBattleEventType )
{
	m_iBattleEventType = iBattleEventType;
	if( m_OwnerUserID.IsEmpty() )
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "(%d) BattleRoom EventType - %d", GetIndex(), GetBattleEventType() );
	else
		LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "(%d) BattleRoom EventType - %d : %s", GetIndex(), GetBattleEventType(), m_OwnerUserID.c_str() );

	if( GetBattleEventType() == BET_BROADCAST_MBC || GetBattleEventType() == BET_TOURNAMENT_BATTLE )
	{
		m_bRandomTeamMode = false;
		m_bAutoModeStart  = false;
	}
}

void BattleRoomNode::SetTournamentBattle( DWORD dwTourIndex, DWORD dwBlueIndex, BYTE BlueTourPos, SHORT BluePosition, DWORD dwRedIndex, BYTE RedTourPos, SHORT RedPosition )
{
	m_TournamentRoundData.Init();

	m_TournamentRoundData.m_dwTourIndex = dwTourIndex;
	m_TournamentRoundData.m_bRegularTour= g_TournamentManager.IsRegularTournament( dwTourIndex );

	m_TournamentRoundData.m_dwBlueIndex = dwBlueIndex;
	m_TournamentRoundData.m_BlueTourPos = BlueTourPos;
	m_TournamentRoundData.m_BluePosition= BluePosition;

	m_TournamentRoundData.m_dwRedIndex  = dwRedIndex;
	m_TournamentRoundData.m_RedTourPos  = RedTourPos;
	m_TournamentRoundData.m_RedPosition = RedPosition;

	m_TournamentRoundData.m_dwInviteTimer = TIMEGETTIME();
}

void BattleRoomNode::SetTournamentTeamChange( DWORD dwNewTeam )
{
	if( m_TournamentRoundData.m_dwBlueIndex == 0 )
	{
		m_TournamentRoundData.m_dwBlueIndex = dwNewTeam;
	}
	else
	{
		m_TournamentRoundData.m_dwRedIndex  = dwNewTeam;
	}

	SP2Packet kPacket( STPK_BATTLEROOM_COMMAND );
	PACKET_GUARD_VOID( kPacket.Write(BATTLEROOM_TOURNAMENT_NEW_TEAM) );
	PACKET_GUARD_VOID( kPacket.Write(dwNewTeam) );
	SendPacketTcp( kPacket );
}

void BattleRoomNode::SyncPlayEnd( bool bAutoStart )
{
	SP2Packet kPacket( SSTPK_BATTLEROOM_SYNC );
	PACKET_GUARD_VOID( kPacket.Write(BattleRoomSync::BRS_PLAY) );
	PACKET_GUARD_VOID( kPacket.Write(GetIndex()) );
	PACKET_GUARD_VOID( kPacket.Write(GetSelectModeTerm()) );
	PACKET_GUARD_VOID( kPacket.Write(GetSelectMap()) );
	PACKET_GUARD_VOID( kPacket.Write(GetSelectMap()) );
	PACKET_GUARD_VOID( kPacket.Write(MT_NONE) );
	PACKET_GUARD_VOID( kPacket.Write(false) );
	PACKET_GUARD_VOID( kPacket.Write(m_bRandomTeamMode) );
	PACKET_GUARD_VOID( kPacket.Write(IsStartRoomEnterX()) );
	PACKET_GUARD_VOID( kPacket.Write(m_bUseExtraOption) );
	PACKET_GUARD_VOID( kPacket.Write(m_bNoChallenger) );
			/*
			<< m_bUseExtraOption << m_iChangeCharType
			<< m_iCoolTimeType << m_iRedHPType << m_iBlueHPType
			<< m_iDropDamageType << m_iGravityType
			<< m_iPreSetModeType << m_iTeamAttackType << m_iGetUpType
			<< m_iKOType << m_iRedBlowType << m_iBlueBlowType
			<< m_iRedMoveSpeedType << m_iBlueMoveSpeedType << m_iKOEffectType
			<< m_iRedEquipType << m_iBlueEquipType;
			*/

	g_ServerNodeManager.SendMessageToPartitions( kPacket );

/*	int iPlayUserCnt = GetPlayUserCnt();
	if( iPlayUserCnt <= 1 )
	{
		// 광장으로 이동
		vBattleRoomUser_iter iter, iEnd;
		iEnd = m_vUserNode.end();
		for(iter=m_vUserNode.begin();iter!=iEnd;++iter)
		{
			BattleRoomUser &kUser = *iter;
			User *pUser = g_UserNodeManager.GetUserNode( kUser.m_dwUserIndex );
			if( pUser )
			{
				pUser->PrivatelyLeaveRoomToTraining();
			}
			else
			{
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "BattleRoomNode::SyncPlayEnd(%d)(%s) 광장으로 보낼 유저가 이동중일 수 있다.", GetIndex(), kUser.m_szPublicID.c_str() );
			}
		}
		m_pBattleRoom = NULL;
	}
	else
*/	{
		// 연습 하기 생성
		SP2Packet kPacket( STPK_BATTLEROOM_COMMAND );
		PACKET_GUARD_VOID( kPacket.Write(BATTLEROOM_UI_SHOW_OK) );
		PACKET_GUARD_VOID( kPacket.Write(false) );
		PACKET_GUARD_VOID( kPacket.Write(bAutoStart) );
		SendPacketTcp( kPacket );
	}
}

void BattleRoomNode::SelectMode( const int iModeIndex )
{
	m_iSelectMode = iModeIndex;
}

void BattleRoomNode::SelectMap( const int iMapIndex )
{
	m_iSelectMap = iMapIndex;
}

TeamType BattleRoomNode::ChangeTeamType( const ioHashString &rkName, TeamType eTeam )
{
	CRASH_GUARD();
	TeamType eCurTeam = TEAM_NONE;

	vBattleRoomUser_iter iter, iEnd;
	iEnd = m_vUserNode.end();
	for(iter=m_vUserNode.begin();iter!=iEnd;++iter)
	{
		BattleRoomUser &kUser = *iter;
		if( kUser.m_szPublicID == rkName )
		{
			if( IsBattleModePlaying() )        // 모드 플레이중인 룸이면 유저도 팀을 바꾼다.
			{
				if( m_pBattleRoom->GetRoomStyle() == RSTYLE_BATTLEROOM && 
					m_pBattleRoom->IsPartyProcessEnd() )
				{
					kUser.m_eTeamType = eTeam;

					// 옵저버 전환
					if( eTeam == TEAM_NONE && !kUser.m_bObserver )
						kUser.m_bObserver = true;
					else if( eTeam != TEAM_NONE && kUser.m_bObserver )
						kUser.m_bObserver = false;

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

				// 옵저버 전환
				if( eTeam == TEAM_NONE && !kUser.m_bObserver )
					kUser.m_bObserver = true;
				else if( eTeam != TEAM_NONE && kUser.m_bObserver )
					kUser.m_bObserver = false;

				eCurTeam = eTeam;
			}

			break;
		}
	}

	return eCurTeam;
}

bool BattleRoomNode::IsObserver( const ioHashString &szPublicID )
{
	vBattleRoomUser_iter iter, iEnd;
	iEnd = m_vUserNode.end();
	for(iter=m_vUserNode.begin();iter!=iEnd;++iter)
	{
		BattleRoomUser &kUser = *iter;
		if( kUser.m_szPublicID == szPublicID )
			return kUser.m_bObserver;
	}

	return false;
}

void BattleRoomNode::SetPreSelectModeInfo( const ModeType eModeType, const int iSubType, const int iMapNum )
{
	m_PreModeType = eModeType;
	m_iPreSubType = iSubType;
	m_iPreMapNum  = iMapNum;
}

void BattleRoomNode::GetPreSelectModeInfo( ModeType &eModeType, int &iSubType, int &iMapNum )
{
	eModeType = m_PreModeType;
	iSubType = m_iPreSubType;
	iMapNum = m_iPreMapNum;
}

bool BattleRoomNode::IsReserveTimeOver()
{
	if( !m_vUserNode.empty() ) return false;

	if( GetBattleEventType() == BET_TOURNAMENT_BATTLE )
	{
		if( TIMEGETTIME() - m_TournamentRoundData.m_dwInviteTimer > TournamentRoundData::TOURNAMENT_ROOM_CLOSE_TIME )
			return true;
	}
	else
	{
		if( m_dwReserveTime == 0 ) return false;
		if( TIMEGETTIME() - m_dwReserveTime > CREATE_RESERVE_DELAY_TIME )
			return true;
	}
	return false;
}

void BattleRoomNode::FillBattleRoomInfo( SP2Packet &rkPacket )
{
	PACKET_GUARD_VOID( rkPacket.Write(m_dwIndex) );
	PACKET_GUARD_VOID( rkPacket.Write(m_szRoomName) );
	PACKET_GUARD_VOID( rkPacket.Write(m_OwnerUserID) );
	PACKET_GUARD_VOID( rkPacket.Write(GetJoinUserCnt()) );
	PACKET_GUARD_VOID( rkPacket.Write(GetPlayUserCnt()) );
	PACKET_GUARD_VOID( rkPacket.Write(m_iMaxPlayerBlue) );
	PACKET_GUARD_VOID( rkPacket.Write(m_iMaxPlayerRed) );
	PACKET_GUARD_VOID( rkPacket.Write(m_iMaxObserver) );
	PACKET_GUARD_VOID( rkPacket.Write(m_szRoomPW) );
	PACKET_GUARD_VOID( rkPacket.Write(GetRoomLevel()) );
	PACKET_GUARD_VOID( rkPacket.Write(m_bUseExtraOption) );
	PACKET_GUARD_VOID( rkPacket.Write(m_bNoChallenger) );
	PACKET_GUARD_VOID( rkPacket.Write(m_iChangeCharType) );
	PACKET_GUARD_VOID( rkPacket.Write(m_iCoolTimeType) );
	PACKET_GUARD_VOID( rkPacket.Write(m_iRedHPType) );
	PACKET_GUARD_VOID( rkPacket.Write(m_iBlueHPType) );
	PACKET_GUARD_VOID( rkPacket.Write(m_iDropDamageType) );
	PACKET_GUARD_VOID( rkPacket.Write(m_iGravityType) );
	PACKET_GUARD_VOID( rkPacket.Write(m_iPreSetModeType) );
	PACKET_GUARD_VOID( rkPacket.Write(m_iTeamAttackType) );
	PACKET_GUARD_VOID( rkPacket.Write(m_iGetUpType) );
	PACKET_GUARD_VOID( rkPacket.Write(m_iKOType) );
	PACKET_GUARD_VOID( rkPacket.Write(m_iRedBlowType) );
	PACKET_GUARD_VOID( rkPacket.Write(m_iBlueBlowType) );
	PACKET_GUARD_VOID( rkPacket.Write(m_iRedMoveSpeedType) );
	PACKET_GUARD_VOID( rkPacket.Write(m_iBlueMoveSpeedType) );
	PACKET_GUARD_VOID( rkPacket.Write(m_iKOEffectType) );
	PACKET_GUARD_VOID( rkPacket.Write(m_iRedEquipType) );
	PACKET_GUARD_VOID( rkPacket.Write(m_iBlueEquipType) );
	PACKET_GUARD_VOID( rkPacket.Write(m_iCatchModeRoundType) );
	PACKET_GUARD_VOID( rkPacket.Write(m_iCatchModeRoundTimeType) );
	PACKET_GUARD_VOID( rkPacket.Write(m_iGrowthUseType) );
	PACKET_GUARD_VOID( rkPacket.Write(m_iExtraItemUseType) );

	// 룸에 있는 경우와 룸에 없는 경우 클라이언트는 다른 처리를 한다. 
	// 룸에 있으면 룸으로 이동하고 룸에 없으면 로비의 전투방으로 이동한다.
	PACKET_GUARD_VOID( rkPacket.Write(IsBattleModePlaying()) );
}

void BattleRoomNode::FillBattleRoomInfoState( SP2Packet &rkPacket, UserParent *pUserParent, DWORD dwPrevBattleIndex )
{
	if( !pUserParent )
	{
		PACKET_GUARD_VOID( rkPacket.Write(0) );
		PACKET_GUARD_VOID( rkPacket.Write(GetBattleEventType()) );
		PACKET_GUARD_VOID( rkPacket.Write(GetTournamentIndex()) );
	}
	else
	{
		PACKET_GUARD_VOID( rkPacket.Write(g_BattleRoomManager.GetSortBattleRoomState( (BattleRoomParent*)this, pUserParent, dwPrevBattleIndex )) );
		PACKET_GUARD_VOID( rkPacket.Write(GetBattleEventType()) );
		PACKET_GUARD_VOID( rkPacket.Write(GetTournamentIndex()) );
	}
}

void BattleRoomNode::FillUserList( SP2Packet &rkPacket )
{
	PACKET_GUARD_VOID( rkPacket.Write(GetJoinUserCnt()) );

	vBattleRoomUser_iter iter, iEnd;
	iEnd = m_vUserNode.end();
	for(iter=m_vUserNode.begin();iter!=iEnd;++iter)
	{
		BattleRoomUser &kUser = *iter;
		PACKET_GUARD_VOID( rkPacket.Write(kUser.m_iGradeLevel) );
		PACKET_GUARD_VOID( rkPacket.Write(kUser.m_szPublicID) );
		PACKET_GUARD_VOID( rkPacket.Write(kUser.m_bObserver) );
		PACKET_GUARD_VOID( rkPacket.Write((int)kUser.m_eTeamType) );

		//길드 정보 & PING
		UserParent *pUserParent = g_UserNodeManager.GetGlobalUserNode( kUser.m_dwUserIndex );
		if( pUserParent )
		{
			PACKET_GUARD_VOID( rkPacket.Write(pUserParent->GetGuildIndex() ) );
			PACKET_GUARD_VOID( rkPacket.Write(pUserParent->GetGuildMark()) );
			PACKET_GUARD_VOID( rkPacket.Write(pUserParent->GetPingStep()) );
		}
		else
		{
			PACKET_GUARD_VOID( rkPacket.Write(0) );
			PACKET_GUARD_VOID( rkPacket.Write(0) );
			PACKET_GUARD_VOID( rkPacket.Write(0) );
		}
	}
}

void BattleRoomNode::FillUserInfo( SP2Packet &rkPacket, const BattleRoomUser &rkUser )
{
	PACKET_GUARD_VOID( rkPacket.Write(GetRoomLevel()) );
	PACKET_GUARD_VOID( rkPacket.Write(rkUser.m_szPublicID) );
	PACKET_GUARD_VOID( rkPacket.Write(rkUser.m_iGradeLevel) );
	PACKET_GUARD_VOID( rkPacket.Write(rkUser.m_iAbilityLevel) );
	PACKET_GUARD_VOID( rkPacket.Write(rkUser.m_bSafetyLevel) );
	PACKET_GUARD_VOID( rkPacket.Write(rkUser.m_bObserver) );
	PACKET_GUARD_VOID( rkPacket.Write((int)rkUser.m_eTeamType) );
	PACKET_GUARD_VOID( rkPacket.Write(rkUser.m_dwUserIndex) );
	PACKET_GUARD_VOID( rkPacket.Write(rkUser.m_szPublicIP) );
	PACKET_GUARD_VOID( rkPacket.Write(rkUser.m_iClientPort) );
	PACKET_GUARD_VOID( rkPacket.Write(rkUser.m_szPrivateIP) );
	PACKET_GUARD_VOID( rkPacket.Write(rkUser.m_szTransferIP) );
	PACKET_GUARD_VOID( rkPacket.Write(rkUser.m_iTransferPort) );

#ifdef BATTLE_MODE_BY_BCKIM		// 2019-02-14 by bckim, 배틀 모드 추가	// BattleRoomNode::FillUserInfo (m_eRunner_Select_OrderType) send 
	PACKET_GUARD_VOID( rkPacket.Write(rkUser.m_eSelect_Order) );
#endif //  BATTLE_MODE_BY_BCKIM	// End. 2019-02-14 by bckim, 배틀 모드 추가

	//길드 정보
	UserParent *pUserParent = g_UserNodeManager.GetGlobalUserNode( rkUser.m_dwUserIndex );
	if( pUserParent )
	{
		PACKET_GUARD_VOID( rkPacket.Write(pUserParent->GetGuildIndex()) );
		PACKET_GUARD_VOID( rkPacket.Write(pUserParent->GetGuildMark()) );
	}
	else
	{
		PACKET_GUARD_VOID( rkPacket.Write(0) );
		PACKET_GUARD_VOID( rkPacket.Write(0) );
	}
}

void BattleRoomNode::FillModeInfo( SP2Packet &rkPacket )
{
	// BMT 전송
	PACKET_GUARD_VOID( rkPacket.Write(GetSelectModeTerm()) );
	
	// 모드 정보
	enum{ BST_START = 0, BST_PLAYING = 1, BST_RESULT = 2, };
	if( m_pBattleRoom && m_pBattleRoom->IsRoomProcess() )
	{
		// 모드 플레이 정보 
		if( m_pBattleRoom->IsFinalRoundResult() )          // 게임 종료중
		{
			PACKET_GUARD_VOID( rkPacket.Write((int)BST_RESULT) );
			PACKET_GUARD_VOID( rkPacket.Write((int)m_pBattleRoom->GetModeType()) );
		}
		else
		{
			PACKET_GUARD_VOID( rkPacket.Write((int)BST_PLAYING) );
			PACKET_GUARD_VOID( rkPacket.Write((int)m_pBattleRoom->GetModeType()) );

			if( m_pBattleRoom->GetModeType() != MT_MONSTER_SURVIVAL && m_pBattleRoom->GetModeType() != MT_DUNGEON_A && 
				m_pBattleRoom->GetModeType() != MT_RAID &&
				!Help::IsMonsterDungeonMode(m_pBattleRoom->GetModeType()) )
			{
				if( m_pBattleRoom->GetModeType() == MT_SURVIVAL ||
					m_pBattleRoom->GetModeType() == MT_TEAM_SURVIVAL ||
					m_pBattleRoom->GetModeType() == MT_DOBULE_CROWN ||
					m_pBattleRoom->GetModeType() == MT_BOSS ||
					m_pBattleRoom->GetModeType() == MT_GANGSI ||
					m_pBattleRoom->GetModeType() == MT_TEAM_SURVIVAL_AI ||
					m_pBattleRoom->GetModeType() == MT_FIGHT_CLUB )
				{
					//남은 시간 전송
					PACKET_GUARD_VOID( rkPacket.Write(m_pBattleRoom->GetRemainPlayTime()) );
				}
				else
				{
					//팀 스코어 전송
					PACKET_GUARD_VOID( rkPacket.Write(m_pBattleRoom->GetBlueWinCnt()) );
					PACKET_GUARD_VOID( rkPacket.Write(m_pBattleRoom->GetRedWinCnt()) );
				}
			}
		}
	}
	else  
	{
		// 선택한 모드 정보(게임 시작전)
		PACKET_GUARD_VOID( rkPacket.Write((int)BST_START) );
		PACKET_GUARD_VOID( rkPacket.Write((int)GetSelectIndexToMode( GetSelectMode(), GetSelectMap() )) );
	}
}

void BattleRoomNode::FillRecord( SP2Packet &rkPacket )
{
	PACKET_GUARD_VOID( rkPacket.Write(m_iBlueWin) );
	PACKET_GUARD_VOID( rkPacket.Write(m_iBlueLose) );
	PACKET_GUARD_VOID( rkPacket.Write(m_iBlueVictories) );
	PACKET_GUARD_VOID( rkPacket.Write(m_iRedWin) );
	PACKET_GUARD_VOID( rkPacket.Write(m_iRedLose) );
	PACKET_GUARD_VOID( rkPacket.Write(m_iRedVictories) );
}

bool BattleRoomNode::IsFull()
{
	int iPlayCnt = GetPlayUserCnt();
	if( iPlayCnt >= GetMaxPlayer() )
		return true;

	return false;
}

bool BattleRoomNode::IsMapLimitPlayerFull()
{
	int iPlayCnt = GetPlayUserCnt();
	if( iPlayCnt >= g_BattleRoomManager.GetBattleMapToLimitPlayer( GetSelectMode(), GetSelectMap() ) )
		return true;

	return false;
}

bool BattleRoomNode::IsMapLimitGrade( int iGradeLevel )
{
	if( iGradeLevel < g_BattleRoomManager.GetBattleMapToLimitGrade( GetSelectMode(), GetSelectMap() ) )
		return true;
	return false;
}

bool BattleRoomNode::lsMapStartCoin( int iCoinCnt)
{
	if( iCoinCnt >= g_BattleRoomManager.GetBattleMapToStartCoin( GetSelectMode(), GetSelectMap() ) )
	{
		return true;
	}
	return false;
}

int BattleRoomNode::GetMapStartCoin()
{
	return g_BattleRoomManager.GetBattleMapToStartCoin( GetSelectMode(), GetSelectMap() );
}

bool BattleRoomNode::IsObserverFull()
{
	int iObserverCnt = 0;
	int iJoiner = GetJoinUserCnt();
	int iPlayer = GetPlayUserCnt();

	if( iJoiner > iPlayer )
		iObserverCnt = iJoiner - iPlayer;

	if( iObserverCnt >= m_iMaxObserver )
		return true;

	return false;
}

bool BattleRoomNode::IsEmptyBattleRoom()
{
	return m_vUserNode.empty();
}

void BattleRoomNode::InsertInviteUser( DWORD dwUserIndex )
{
	InviteUser kInviteUser;
	kInviteUser.m_dwInviteTime  = TIMEGETTIME();
	kInviteUser.m_dwUserIndex   = dwUserIndex;
	m_vInviteUser.push_back( kInviteUser );
}

bool BattleRoomNode::IsInviteUser( DWORD dwUserIndex )
{
	int iSize = (int)m_vInviteUser.size();
	for(int i = 0;i < iSize;i++)
	{
		InviteUser &kInviteUser = m_vInviteUser[i];
		if( kInviteUser.m_dwUserIndex == dwUserIndex )
		{
			if( TIMEGETTIME() - kInviteUser.m_dwInviteTime > 20000 )
			{
				m_vInviteUser.erase( m_vInviteUser.begin() + i );
				return false;
			}
			else
			{
				m_vInviteUser.erase( m_vInviteUser.begin() + i );
				return true;
			}
		}
	}
	return false;
}

BattleRoomUser &BattleRoomNode::GetUserNodeByIndex( DWORD dwUserIndex )
{
	vBattleRoomUser_iter iter, iEnd;
	iEnd = m_vUserNode.end();
	for(iter=m_vUserNode.begin();iter!=iEnd;++iter)
	{
		BattleRoomUser &kUser = *iter;
		if( kUser.m_dwUserIndex == dwUserIndex )
			return kUser;
	}

	static BattleRoomUser kReturn;
	return kReturn;
}

const BattleRoomUser &BattleRoomNode::GetUserNodeByArray( int iArray )
{
	if( COMPARE( iArray, 0, (int) m_vUserNode.size()) )
		return m_vUserNode[iArray];

	static BattleRoomUser kError;
	return kError;
}

UserParent *BattleRoomNode::GetUserNode( const ioHashString &szPublicID )
{
	CRASH_GUARD();
	vBattleRoomUser_iter iter, iEnd;
	iEnd = m_vUserNode.end();
	for(iter=m_vUserNode.begin();iter!=iEnd;++iter)
	{
		BattleRoomUser &kUser = *iter;
		if( kUser.m_szPublicID == szPublicID )
			return g_UserNodeManager.GetGlobalUserNode( kUser.m_dwUserIndex );
	}
	return NULL;
}

TeamType BattleRoomNode::GetUserTeam( const ioHashString &szPublicID )
{
	CRASH_GUARD();
	vBattleRoomUser_iter iter, iEnd;
	iEnd = m_vUserNode.end();
	for(iter=m_vUserNode.begin();iter!=iEnd;++iter)
	{
		BattleRoomUser &kUser = *iter;
		if( kUser.m_szPublicID == szPublicID )
			return kUser.m_eTeamType;
	}
	return TEAM_NONE;
}

int BattleRoomNode::GetUserTeamCount( TeamType eTeam )
{
	int iCount = 0;
	vBattleRoomUser_iter iter, iEnd;
	iEnd = m_vUserNode.end();
	for(iter=m_vUserNode.begin();iter!=iEnd;++iter)
	{
		BattleRoomUser &kUser = *iter;

		if( eTeam == TEAM_NONE && kUser.m_bObserver )
			iCount++;
		else if( eTeam != TEAM_NONE && kUser.m_eTeamType == eTeam )
			iCount++;
	}
	return iCount;
}

int BattleRoomNode::GetMaxPlayer( TeamType eTeam )
{
	if( eTeam == TEAM_BLUE )
		return m_iMaxPlayerBlue;
	return m_iMaxPlayerRed;
}

void BattleRoomNode::UpdateRecord( TeamType eWinTeam )
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

bool BattleRoomNode::IsChangeEnterSyncData()
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
	else if( !m_bRandomTeamMode )
		return true;
	else if( m_bStartRoomEnterX )
		return true;
	else if( !m_bAutoModeStart )
		return true;
	else if( m_bBadPingKick )
		return true;
	else if( m_bUseExtraOption )
		return true;
	else if( m_bNoChallenger )
		return true;
	else if( m_iChangeCharType > 0 )
		return true;
	else if( m_iTeamAttackType > 0 )
		return true;
	else if( m_iCoolTimeType > 0 )
		return true;
	else if( m_iRedHPType > 0 )
		return true;
	else if( m_iBlueHPType > 0 )
		return true;
	else if( m_iDropDamageType > 0 )
		return true;
	else if( m_iGravityType > 0 )
		return true;
	else if( m_iGetUpType > 0 )
		return true;
	else if( m_iRedMoveSpeedType > 0 )
		return true;
	else if( m_iBlueMoveSpeedType > 0 )
		return true;
	else if( m_iRedEquipType > 0 )
		return true;
	else if( m_iBlueEquipType > 0 )
		return true;
	else if( m_iKOType > 0 )
		return true;
	else if( m_iKOEffectType > 0 )
		return true;
	else if( m_iRedBlowType > 0 )
		return true;
	else if( m_iBlueBlowType > 0 )
		return true;
	else if( m_iCatchModeRoundType > 0 )
		return true;
	else if( m_iCatchModeRoundTimeType > 0 )
		return true;
	else if( m_iPreSetModeType > 0 )
		return true;
	else if( m_iGrowthUseType > 0 )
		return true;
	else if( m_iExtraItemUseType > 0 )
		return true;
	else if( m_iBattleEventType != BET_NORMAL )
		return true;

	return false;
}

void BattleRoomNode::CreateBoss()
{
	ioHashStringVec vOriginalUserName;
	ioHashStringVec vCopyUserName;

	vBattleRoomUser_iter iter;
	for(iter=m_vUserNode.begin();iter!=m_vUserNode.end();++iter)
	{
		BattleRoomUser &kUser = *iter;
		
		// 옵져버 패스
		if( kUser.m_bObserver ) continue;

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

		LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "BattleRoomNode::CreateBoss(%d) 동일 서버 - [%s]당첨!", GetIndex(), m_szBossName.c_str() );
	}
	else if( !vCopyUserName.empty() ) 
	{
		//타서버에 있는 유저중 랜덤
		int iSize = vCopyUserName.size();
		int rSelect = rand()%iSize;
		m_szBossName = vCopyUserName[rSelect];

		LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "BattleRoomNode::CreateBoss(%d) 타 서버 - [%s]당첨!", GetIndex(), m_szBossName.c_str() );
	}
	else
	{
		LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "BattleRoomNode::CreateBoss(%d) : 보스에 선택될 유저가 없다!!", GetIndex() );
	}
	vOriginalUserName.clear();
	vCopyUserName.clear();
}

const ioHashString &BattleRoomNode::GetBossName()
{
	return m_szBossName;
}

void BattleRoomNode::ClearBossName()
{
	m_szBossName.Clear();
}

void BattleRoomNode::CreateGangsi()
{
	ioHashStringVec vOriginalUserName;
	ioHashStringVec vCopyUserName;

	vBattleRoomUser_iter iter;
	for(iter=m_vUserNode.begin();iter!=m_vUserNode.end();++iter)
	{
		BattleRoomUser &kUser = *iter;

		// 옵져버 패스
		if( kUser.m_bObserver ) continue;

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

		LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "BattleRoomNode::CreateGangsi(%d) 동일 서버 - [%s]당첨!", GetIndex(), m_szGangsiName.c_str() );
	}
	else if( !vCopyUserName.empty() ) 
	{
		//타서버에 있는 유저중 랜덤
		int iSize = vCopyUserName.size();
		int rSelect = rand()%iSize;
		m_szGangsiName = vCopyUserName[rSelect];

		LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "BattleRoomNode::CreateGangsi(%d) 타 서버 - [%s]당첨!", GetIndex(), m_szGangsiName.c_str() );
	}
	else
	{
		LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "BattleRoomNode::CreateGangsi(%d) : 보스에 선택될 유저가 없다!!", GetIndex() );
	}
	vOriginalUserName.clear();
	vCopyUserName.clear();
}

const ioHashString &BattleRoomNode::GetGangsiName()
{
	return m_szGangsiName;
}

void BattleRoomNode::ClearGangsi()
{
	m_szGangsiName.Clear();
}
	
void BattleRoomNode::SendPacketTcp( SP2Packet &rkPacket, const DWORD dwUserIndex )
{
	vBattleRoomUser_iter iter, iEnd;
	iEnd = m_vUserNode.end();
	for(iter=m_vUserNode.begin();iter!=iEnd;++iter)
	{
		BattleRoomUser &kUser = *iter;
		if( kUser.m_dwUserIndex == dwUserIndex ) continue;

		UserParent *pUser = g_UserNodeManager.GetGlobalUserNode( kUser.m_dwUserIndex );
		if( pUser )
			pUser->RelayPacket( rkPacket );
	}
}

void BattleRoomNode::SendPacketTcpUser( SP2Packet &rkPacket, const ioHashString &rkSenderName )
{
	CRASH_GUARD();
	vBattleRoomUser_iter iter, iEnd;
	iEnd = m_vUserNode.end();
	for(iter=m_vUserNode.begin();iter!=iEnd;++iter)
	{
		BattleRoomUser &kUser = *iter;
		if( kUser.m_szPublicID == rkSenderName ) 
		{
			UserParent *pUser = g_UserNodeManager.GetGlobalUserNode( rkSenderName );
			if( pUser )
				pUser->RelayPacket( rkPacket );
			return;
		}		
	}
}

void BattleRoomNode::SendPacketUdp( SP2Packet &rkPacket, const DWORD dwUserIndex /* = 0  */ )
{
	vBattleRoomUser_iter iter, iEnd;
	iEnd = m_vUserNode.end();
	for(iter=m_vUserNode.begin();iter!=iEnd;++iter)
	{
		BattleRoomUser &kUser = *iter;
		if( kUser.m_dwUserIndex == dwUserIndex ) continue;

		g_UDPNode.SendMessage( (char*)kUser.m_szPublicIP.c_str(), kUser.m_iClientPort, rkPacket );
	}
}

void BattleRoomNode::Process()
{
	m_NodeSync.Process();

	TournamentProcess();
}

void BattleRoomNode::TournamentProcess()
{
	if( GetBattleEventType() != BET_TOURNAMENT_BATTLE ) return;
	
	if( m_TournamentRoundData.m_bRegularTour )
	{
		// 정규 대회 프로세스

	}
	else
	{
		// 유저 대회 프로세스
		CustomTournamentRandomWinProcess();
	}
}

void BattleRoomNode::CustomTournamentRandomWinProcess()
{
	if( m_pBattleRoom ) return;		// 경기가 시작되었음
	if( m_TournamentRoundData.m_bSendResult ) return;    // 이미 결과가 전송되었음.
	if( TIMEGETTIME() - m_TournamentRoundData.m_dwInviteTimer < TournamentRoundData::CUSTOM_TOURNAMENT_RANDOM_WIN_TIME ) return;
	if( GetUserTeamCount( TEAM_BLUE ) != 0 || GetUserTeamCount( TEAM_RED ) != 0 ) return;          // 접속한 팀원이 있음.

	if( (BOOL)(rand() % 2) == TRUE )
	{
		SP2Packet kPacket( MSTPK_TOURNAMENT_BATTLE_RESULT );
		PACKET_GUARD_VOID( kPacket.Write(m_TournamentRoundData.m_dwTourIndex) );
		PACKET_GUARD_VOID( kPacket.Write(m_TournamentRoundData.m_dwBlueIndex) );
		PACKET_GUARD_VOID( kPacket.Write(m_TournamentRoundData.m_dwRedIndex) );
		PACKET_GUARD_VOID( kPacket.Write(m_TournamentRoundData.m_BlueTourPos + 1) );

		g_MainServer.SendMessage( kPacket );
	}
	else
	{
		SP2Packet kPacket( MSTPK_TOURNAMENT_BATTLE_RESULT );
		PACKET_GUARD_VOID( kPacket.Write(m_TournamentRoundData.m_dwTourIndex) );
		PACKET_GUARD_VOID( kPacket.Write(m_TournamentRoundData.m_dwRedIndex) );
		PACKET_GUARD_VOID( kPacket.Write(m_TournamentRoundData.m_dwBlueIndex) );
		PACKET_GUARD_VOID( kPacket.Write(m_TournamentRoundData.m_RedTourPos + 1) );

		g_MainServer.SendMessage( kPacket );
	}
	m_TournamentRoundData.m_bSendResult = true;
}

void BattleRoomNode::OnBattleRoomInfo( UserParent *pUser, int iPrevBattleIndex )
{
	if( pUser == NULL )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "BattleRoomNode::OnBattleRoomInfo Null User Pointer!!(%d)", GetIndex() );
		return;
	}

	SP2Packet kPacket( STPK_BATTLEROOM_JOIN_INFO );
	FillBattleRoomInfo( kPacket );
	FillBattleRoomInfoState( kPacket, pUser, iPrevBattleIndex );
	FillUserList( kPacket );
	FillModeInfo( kPacket );
	pUser->RelayPacket( kPacket );
}

bool BattleRoomNode::OnProcessPacket( SP2Packet &rkPacket, UserParent *pUser )
{
	switch( rkPacket.GetPacketID() )
	{
	case CTPK_MACRO_COMMAND:
		OnMacroCommand( rkPacket, pUser );
		return true;
	case CTPK_VOICE_INFO:
		OnVoiceInfo( rkPacket, pUser );
		return true;
	case CTPK_BATTLEROOM_INVITE:
		OnBattleRoomInvite( rkPacket, pUser );
		return true;
	case CTPK_BATTLEROOM_COMMAND:
		OnBattleRoomCommand( rkPacket, pUser );
		return true;
	}
	return false;
}

void BattleRoomNode::OnMacroCommand( SP2Packet &rkPacket, UserParent *pUser )
{
	if( pUser == NULL ) return;

	int command_type = 0;
	PACKET_GUARD_VOID( rkPacket.Read(command_type) );

	bool bCommandBlock = false;
	if( GetBattleEventType() == BET_TOURNAMENT_BATTLE )
		bCommandBlock = true;

#ifdef BATTLE_MODE_BY_BCKIM		// 2019-02-14 by bckim, 배틀 모드 추가
	bool bCommand_PlayerCnt = false;		
	if( GetSelectMode() == MT_BATTLE )
		bCommand_PlayerCnt = true;
#endif // BATTLE_MODE_BY_BCKIM	// End. 2019-02-14 by bckim, 배틀 모드 추가

	SP2Packet kPacket( STPK_MACRO_COMMAND );
	switch( command_type )
	{
	case MACRO_KICK_OUT:
		{
			CRASH_GUARD();
			if( !bCommandBlock && GetOwnerName() == pUser->GetPublicID() )
			{
				ioHashString rkName;
				PACKET_GUARD_VOID( rkPacket.Read(rkName) );

				if( !KickOutUser( rkName ) )
				{
					PACKET_GUARD_VOID( kPacket.Write(MACRO_KICK_OUT_FAILED) );
					PACKET_GUARD_VOID( kPacket.Write(rkName) );
					pUser->RelayPacket( kPacket );
				}
			}		
			else
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "관리자가 아닌데 강퇴 신호 보냄(%d) : %s - %d", GetIndex(), pUser->GetPublicID().c_str(), (int)bCommandBlock );
		}
		break;
	case MACRO_ROOM_NAME_PW:
		{
			CRASH_GUARD();
			if( !bCommandBlock && GetOwnerName() == pUser->GetPublicID() )
			{
				ioHashString rkName, rkPW;

				PACKET_GUARD_VOID( rkPacket.Read(rkName) );
				PACKET_GUARD_VOID( rkPacket.Read(rkPW) );

				SetName( rkName );
				SetPW( rkPW );
				PACKET_GUARD_VOID( kPacket.Write(MACRO_ROOM_NAME_PW) );
				PACKET_GUARD_VOID( kPacket.Write(rkName) );
				PACKET_GUARD_VOID( kPacket.Write(rkPW) );

				SendPacketTcp( kPacket, pUser->GetUserIndex() );
			}
			else
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "관리자가 아닌데 방제/비번 변경 신호 보냄(%d) : %s - %d", GetIndex(), pUser->GetPublicID().c_str(), (int)bCommandBlock );
		}
		break;
	case MACRO_PLAYER_CNT:
		{
			CRASH_GUARD();
			if( !bCommandBlock && GetOwnerName() == pUser->GetPublicID() )
			{
				int iBlueCnt = 0, iRedCnt = 0, iObserver = 0, iWantBlue = 0, iWantRed = 0, iWantObserver = 0;
				
				PACKET_GUARD_VOID( rkPacket.Read(iBlueCnt) );
				PACKET_GUARD_VOID( rkPacket.Read(iRedCnt) );
				PACKET_GUARD_VOID( rkPacket.Read(iObserver) );

				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "BEFORE>>>>>>>>iBlueCnt[%d],iRedCnt[%d],iObserver[%d]", iBlueCnt,iRedCnt,iObserver);


				iWantBlue = iBlueCnt;
				iWantRed  = iRedCnt;
				iWantObserver = iObserver;

				//아무나 초대가 되므로 변경하려는 팀원수가 작을 수 있다.
				//

#ifdef BATTLE_MODE_BY_BCKIM // 2019-02-14 by bckim, 배틀 모드 추가	IsBattleTeamChangeOK
				if( GetSelectMode() != MT_BATTLE )
#endif // BATTLE_MODE_BY_BCKIM // 2019-02-14 by bckim, 배틀 모드 추가
				{
					if( iBlueCnt < GetUserTeamCount( TEAM_BLUE ) )         
						iBlueCnt = GetUserTeamCount( TEAM_BLUE );
					if( iRedCnt < GetUserTeamCount( TEAM_RED ) )         
						iRedCnt = GetUserTeamCount( TEAM_RED );
					if( iObserver < GetUserTeamCount( TEAM_NONE ) )
						iObserver = GetUserTeamCount( TEAM_NONE );				
				}

				SetMaxPlayer( iBlueCnt, iRedCnt, iObserver );
				PACKET_GUARD_VOID( kPacket.Write(MACRO_PLAYER_CNT) );
				PACKET_GUARD_VOID( kPacket.Write(iBlueCnt) );
				PACKET_GUARD_VOID( kPacket.Write(iRedCnt) );
				PACKET_GUARD_VOID( kPacket.Write(iObserver) );

				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "AFTER>>>>>>>>iBlueCnt[%d],iRedCnt[%d],iObserver[%d]", iBlueCnt,iRedCnt,iObserver);

				SendPacketTcp( kPacket );

				if( iWantBlue != iBlueCnt )
				{
					LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "전투방 블루팀 인원 변경 안됨(%d) : %s - %d - %d", GetIndex(), pUser->GetPublicID().c_str(), iWantBlue, iBlueCnt );
				}

				if( iWantRed != iRedCnt )
				{
					LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "전투방 레드팀 인원 변경 안됨(%d) : %s - %d - %d", GetIndex(), pUser->GetPublicID().c_str(), iWantRed, iRedCnt );
				}

				if( iWantObserver != iObserver )
				{
					LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "전투방 옵저버 인원 변경 안됨(%d) : %s - %d - %d", GetIndex(), pUser->GetPublicID().c_str(), iWantObserver, iObserver );
				}
			}
			else
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "관리자가 아닌데 인원 변경 신호 보냄(%d) : %s - %d", GetIndex(), pUser->GetPublicID().c_str(), (int)bCommandBlock );
		}
		break;
	case MACRO_OPTION_CHANGE:
		{
			CRASH_GUARD();
			if( !bCommandBlock && GetOwnerName() == pUser->GetPublicID() )
			{

				bool bPrevRoomEnterX = m_bStartRoomEnterX;
				bool bPrevRandomTeamMode = m_bRandomTeamMode;
				bool bPrevuseExtraOption = m_bUseExtraOption;
				bool bPrevNoChallenger = m_bNoChallenger;
				bool bUseExtraOption;

				int iPrevTeamAttackType = m_iTeamAttackType;
				int iPrevChangeCharType = m_iChangeCharType;
				int iPrevCoolTimeType = m_iCoolTimeType;
				int iPreRedHPType = m_iRedHPType;
				int iPreBlueHPType = m_iBlueHPType;
				int iPreDropDamageType = m_iDropDamageType;
				int iPreGravityType = m_iGravityType;
				int iPreGetUpType = m_iGetUpType;
				int iPreRedMoveSpeedType = m_iRedMoveSpeedType;
				int iPreBlueMoveSpeedType = m_iBlueMoveSpeedType;
				int iPreKOType = m_iKOType;
				int iPreKOEffectType = m_iKOEffectType;
				int iPreRedBlowType = m_iRedBlowType;
				int iPreBlueBlowType = m_iBlueBlowType;
				int iPreRedEquipType = m_iRedEquipType;
				int iPreBlueEquipType = m_iBlueEquipType;
				int iPreCatchModeRoundType = m_iCatchModeRoundType;
				int iPreCatchModeRoundTimeType = m_iCatchModeRoundTimeType;
				int iPreGrowthUseType = m_iGrowthUseType;
				int iPreExtraItemUseType = m_iExtraItemUseType;


				int iPrePreSetModeType = m_iPreSetModeType;
				

				PACKET_GUARD_VOID( rkPacket.Read(m_bRandomTeamMode) );
				PACKET_GUARD_VOID( rkPacket.Read(m_bStartRoomEnterX) );
				PACKET_GUARD_VOID( rkPacket.Read(m_bAutoModeStart) );
				PACKET_GUARD_VOID( rkPacket.Read(m_bBadPingKick) );
				PACKET_GUARD_VOID( rkPacket.Read(bUseExtraOption) );
				PACKET_GUARD_VOID( rkPacket.Read(m_bNoChallenger) );
				PACKET_GUARD_VOID( rkPacket.Read(m_iChangeCharType) );
				PACKET_GUARD_VOID( rkPacket.Read(m_iCoolTimeType) );
				PACKET_GUARD_VOID( rkPacket.Read(m_iRedHPType) );
				PACKET_GUARD_VOID( rkPacket.Read(m_iBlueHPType) );
				PACKET_GUARD_VOID( rkPacket.Read(m_iDropDamageType) );
				PACKET_GUARD_VOID( rkPacket.Read(m_iGravityType) );
				PACKET_GUARD_VOID( rkPacket.Read(m_iPreSetModeType) );
				PACKET_GUARD_VOID( rkPacket.Read(m_iTeamAttackType) );
				PACKET_GUARD_VOID( rkPacket.Read(m_iGetUpType) );
				PACKET_GUARD_VOID( rkPacket.Read(m_iKOType) );
				PACKET_GUARD_VOID( rkPacket.Read(m_iRedBlowType) );
				PACKET_GUARD_VOID( rkPacket.Read(m_iBlueBlowType) );
				PACKET_GUARD_VOID( rkPacket.Read(m_iRedMoveSpeedType) );
				PACKET_GUARD_VOID( rkPacket.Read(m_iBlueMoveSpeedType) );
				PACKET_GUARD_VOID( rkPacket.Read(m_iKOEffectType) );
				PACKET_GUARD_VOID( rkPacket.Read(m_iRedEquipType) );
				PACKET_GUARD_VOID( rkPacket.Read(m_iBlueEquipType) );
				PACKET_GUARD_VOID( rkPacket.Read(m_iCatchModeRoundType) );
				PACKET_GUARD_VOID( rkPacket.Read(m_iCatchModeRoundTimeType) );
				PACKET_GUARD_VOID( rkPacket.Read(m_iGrowthUseType) );
				PACKET_GUARD_VOID( rkPacket.Read(m_iExtraItemUseType) );

				if(CheckUserMode( GetSelectMode(), GetSelectMap() ))
				{
					m_bUseExtraOption= bUseExtraOption;
				}

				PACKET_GUARD_VOID( kPacket.Write(MACRO_OPTION_CHANGE) );

#ifdef BATTLE_MODE_BY_BCKIM		// 2019-02-14 by bckim, 배틀 모드 추가
				if( GetSelectMode() == MT_BATTLE )
				{
					m_bRandomTeamMode = false;
					m_bStartRoomEnterX = true;
				}
#endif // BATTLE_MODE_BY_BCKIM	// End. 2019-02-14 by bckim, 배틀 모드 추가

				PACKET_GUARD_VOID( kPacket.Write(m_bRandomTeamMode) );
				PACKET_GUARD_VOID( kPacket.Write(m_bStartRoomEnterX) );
				PACKET_GUARD_VOID( kPacket.Write(m_bAutoModeStart) );
				PACKET_GUARD_VOID( kPacket.Write(m_bBadPingKick) );
				PACKET_GUARD_VOID( kPacket.Write(m_bUseExtraOption) );
				PACKET_GUARD_VOID( kPacket.Write(m_bNoChallenger) );
				PACKET_GUARD_VOID( kPacket.Write(m_iChangeCharType) );
				PACKET_GUARD_VOID( kPacket.Write(m_iCoolTimeType) );
				PACKET_GUARD_VOID( kPacket.Write(m_iRedHPType) );
				PACKET_GUARD_VOID( kPacket.Write(m_iBlueHPType) );
				PACKET_GUARD_VOID( kPacket.Write(m_iDropDamageType) );
				PACKET_GUARD_VOID( kPacket.Write(m_iGravityType) );
				PACKET_GUARD_VOID( kPacket.Write(m_iPreSetModeType) );
				PACKET_GUARD_VOID( kPacket.Write(m_iTeamAttackType) );
				PACKET_GUARD_VOID( kPacket.Write(m_iGetUpType) );
				PACKET_GUARD_VOID( kPacket.Write(m_iKOType) );
				PACKET_GUARD_VOID( kPacket.Write(m_iRedBlowType) );
				PACKET_GUARD_VOID( kPacket.Write(m_iBlueBlowType) );
				PACKET_GUARD_VOID( kPacket.Write(m_iRedMoveSpeedType) );
				PACKET_GUARD_VOID( kPacket.Write(m_iBlueMoveSpeedType) );
				PACKET_GUARD_VOID( kPacket.Write(m_iKOEffectType) );
				PACKET_GUARD_VOID( kPacket.Write(m_iRedEquipType) );
				PACKET_GUARD_VOID( kPacket.Write(m_iBlueEquipType) );
				PACKET_GUARD_VOID( kPacket.Write(m_iCatchModeRoundType) );
				PACKET_GUARD_VOID( kPacket.Write(m_iCatchModeRoundTimeType) );
				PACKET_GUARD_VOID( kPacket.Write(m_iGrowthUseType) );
				PACKET_GUARD_VOID( kPacket.Write(m_iExtraItemUseType) );

				SendPacketTcp( kPacket, pUser->GetUserIndex() );

				if( bPrevRoomEnterX != m_bStartRoomEnterX ||
					bPrevRandomTeamMode != m_bRandomTeamMode ||
					bPrevuseExtraOption != m_bUseExtraOption ||
					bPrevNoChallenger != m_bNoChallenger )
				{
					m_NodeSync.Update( BattleRoomSync::BRS_PLAY );
				}
			}
			else
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "관리자가 아닌데 옵션 변경 신호 보냄(%d) : %s - %d", GetIndex(), pUser->GetPublicID().c_str(), (int)bCommandBlock );
		}
		break;
	case MACRO_AI_LEVEL_CHANGE:
		{
			CRASH_GUARD();
			{
				if( !bCommandBlock && GetOwnerName() == pUser->GetPublicID() )
				{
					PACKET_GUARD_VOID( rkPacket.Read(m_iAILevel) );
					PACKET_GUARD_VOID( kPacket.Write(MACRO_AI_LEVEL_CHANGE) );
					PACKET_GUARD_VOID( kPacket.Write(m_iAILevel) );
					SendPacketTcp( kPacket, pUser->GetUserIndex() );
				}
				else
					LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "관리자가 아닌데 AI레벨 변경 신호 보냄(%d) : %s - %d", GetIndex(), pUser->GetPublicID().c_str(), (int)bCommandBlock );
			}
		}
		break;
	}
}

void BattleRoomNode::OnVoiceInfo( SP2Packet &rkPacket, UserParent *pUser )
{
	if( pUser == NULL ) return;

	int iType = 0;
	PACKET_GUARD_VOID( rkPacket.Read(iType) );

	if( !COMPARE( iType, ID_VOICE_ON, ID_VOICE_PERMIT + 1) ) return;

	ioHashString szReceiverID;
	PACKET_GUARD_VOID( rkPacket.Read(szReceiverID) );

	SP2Packet kReturnPacket( STPK_VOICE_INFO );
	PACKET_GUARD_VOID( kReturnPacket.Write(iType) );
	PACKET_GUARD_VOID( kReturnPacket.Write(pUser->GetPublicID()) );
	PACKET_GUARD_VOID( kReturnPacket.Write(GetUserTeam( pUser->GetPublicID() )) );

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

void BattleRoomNode::OnBattleRoomInvite( SP2Packet &rkPacket, UserParent *pUser )
{
	if( pUser == NULL ) return;

	int iSize = 0;
	ioHashString szInvitedID;
	PACKET_GUARD_VOID( rkPacket.Read(iSize) );
	PACKET_GUARD_VOID( rkPacket.Read(szInvitedID) );
	MAX_GUARD(iSize, 50);

	if( IsFull() )
	{
		SP2Packet kPacket( STPK_BATTLEROOM_INVITE_RESULT );
		PACKET_GUARD_VOID( kPacket.Write(BATTLEROOM_INVITE_USER_FULL) );
		pUser->RelayPacket( kPacket );
		return;
	}
	else
	{
		if( iSize == 1 )
		{
			DWORD dwLevel = 0, dwGuildIndex = 0, dwGuildMark = 0;
			PACKET_GUARD_VOID( rkPacket.Read(dwLevel) );
			PACKET_GUARD_VOID( rkPacket.Read(dwGuildIndex) );
			PACKET_GUARD_VOID( rkPacket.Read(dwGuildMark) );

			SP2Packet kPacket( STPK_BATTLEROOM_INVITE_RESULT );
			PACKET_GUARD_VOID( kPacket.Write(BATTLEROOM_INVITE_OK) );
			PACKET_GUARD_VOID( kPacket.Write(iSize) );
			PACKET_GUARD_VOID( kPacket.Write(szInvitedID) );
			PACKET_GUARD_VOID( kPacket.Write(dwLevel) );
			PACKET_GUARD_VOID( kPacket.Write(dwGuildIndex) );
			PACKET_GUARD_VOID( kPacket.Write(dwGuildMark) );
			pUser->RelayPacket( kPacket );		
		}		
		else
		{
			SP2Packet kPacket( STPK_BATTLEROOM_INVITE_RESULT );
			PACKET_GUARD_VOID( kPacket.Write(BATTLEROOM_INVITE_OK) );
			PACKET_GUARD_VOID( kPacket.Write(iSize) );
			pUser->RelayPacket( kPacket );		
		}
	}
	//	else if( IsBattleTimeClose() )
	//	{
	//		SP2Packet kPacket( STPK_BATTLEROOM_INVITE_RESULT );
	//		kPacket << BATTLEROOM_INVITE_TIME_CLOSE;
	//		pUser->RelayPacket( kPacket );
	//		return;
	//	}

	SP2Packet kPacket( STPK_BATTLEROOM_INVITE );
	FillBattleRoomInfo( kPacket );
	FillUserList( kPacket );		
	FillModeInfo( kPacket );
	
#ifdef USER_RECONFIRM_IN_SERVER_BY_BCKIM		// 2018-11-08 by bckim, 개발자 ID 검증 추가 	// 2018-11-21 bckim, 게임초대 예외처리 추가	
	// 첫번째 유저 전송
	if( g_UserNodeManager.IsDeveloper( szInvitedID.c_str() ) == false)
	{
		UserParent *pInviteUser = g_UserNodeManager.GetGlobalUserNode( szInvitedID );
		if( pInviteUser && pInviteUser->GetUserPos() != UP_BATTLE_ROOM )
		{
			pInviteUser->RelayPacket( kPacket );
			InsertInviteUser( pInviteUser->GetUserIndex() );
		}
	}
	else
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "USER_RECONFIRM_IN_SERVER[BATTLEROOM](%s to %s) ", pUser->GetPublicID().c_str(),szInvitedID.c_str());

	// 다음 유저 전송
	for(int i = 1;i < iSize;i++)
	{		
		PACKET_GUARD_VOID( rkPacket.Read(szInvitedID) );
		if( g_UserNodeManager.IsDeveloper( szInvitedID.c_str() ) == false)    
		{
			UserParent *pInviteUser = g_UserNodeManager.GetGlobalUserNode( szInvitedID );
			if( pInviteUser && pInviteUser->GetUserPos() != UP_BATTLE_ROOM )
			{
				pInviteUser->RelayPacket( kPacket );
				InsertInviteUser( pInviteUser->GetUserIndex() );
			}
		}
		else
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "USER_RECONFIRM_IN_SERVER[BATTLEROOM](%s to %s) ", pUser->GetPublicID().c_str(),szInvitedID.c_str());
	}	

#else 
	// 첫번째 유저 전송
	UserParent *pInviteUser = g_UserNodeManager.GetGlobalUserNode( szInvitedID );
	if( pInviteUser && pInviteUser->GetUserPos() != UP_BATTLE_ROOM )
	{
		pInviteUser->RelayPacket( kPacket );
		InsertInviteUser( pInviteUser->GetUserIndex() );
	}
	// 다음 유저 전송
	for(int i = 1;i < iSize;i++)
	{		
		PACKET_GUARD_VOID( rkPacket.Read(szInvitedID) );
		UserParent *pInviteUser = g_UserNodeManager.GetGlobalUserNode( szInvitedID );
		if( pInviteUser && pInviteUser->GetUserPos() != UP_BATTLE_ROOM )
		{
			pInviteUser->RelayPacket( kPacket );
			InsertInviteUser( pInviteUser->GetUserIndex() );
		}
	}	
#endif // USER_RECONFIRM_IN_SERVER_BY_BCKIM		// 2018-11-08 by bckim, 개발자 ID 검증 추가 	// 2018-11-21 bckim, 게임초대 예외처리 추가	
}

void BattleRoomNode::OnBattleRoomCommand( SP2Packet &rkPacket, UserParent *pUser )
{

#ifdef FLAG_MODE_BY_BCKIM_DEBUG		// 전투시작 임시 값 확인  BattleRoomNode::OnBattleRoomCommand
	LOG.PrintTimeAndLog(0,"[FLAG_MODE_BY_BCKIM:BattleRoomNode::OnBattleRoomCommand] A_NAME[%s]",pUser->GetPrivateID().c_str());
#endif // FLAG_MODE_BY_BCKIM_DEBUG


	if( pUser == NULL ) return;

	int iCommand = 0;
	PACKET_GUARD_VOID( rkPacket.Read(iCommand) );

	bool bCommandBlock = false;
	if( GetBattleEventType() == BET_TOURNAMENT_BATTLE )
		bCommandBlock = true;

	switch( iCommand )
	{
	case BATTLEROOM_TEAM_CHANGE:
		{
			int iTeamType = 0;
			PACKET_GUARD_VOID( rkPacket.Read(iTeamType) );

			if( !bCommandBlock && IsBattleTeamChangeOK( (TeamType)iTeamType ) )
			{
				if( IsObserver( pUser->GetPublicID() ) && m_bSafetyLevelRoom && !pUser->IsSafetyLevel() )
				{
					// 옵져버인데 팀으로 이동할려면 수준이 맞아야한다.
					SP2Packet kPacket( STPK_BATTLEROOM_COMMAND );
					PACKET_GUARD_VOID( kPacket.Write(BATTLEROOM_TEAM_CHANGE_NOT_LEVEL) );
					pUser->RelayPacket( kPacket );
				}
				else if( IsObserver( pUser->GetPublicID() ) && !g_LevelMatchMgr.IsPartyLevelJoin( GetAbilityMatchLevel(), pUser->GetKillDeathLevel(), JOIN_CHECK_MIN_LEVEL ) )
				{
					// 옵져버인데 팀으로 이동할려면 수준이 맞아야한다.
					SP2Packet kPacket( STPK_BATTLEROOM_COMMAND );
					PACKET_GUARD_VOID( kPacket.Write(BATTLEROOM_TEAM_CHANGE_NOT_LEVEL) );
					pUser->RelayPacket( kPacket );
				}
				else if( IsObserver( pUser->GetPublicID() ) && IsMapLimitPlayerFull() )
				{
					// 옵져버인데 팀으로 맵 최대 인원수가 맞아야한다.
					SP2Packet kPacket( STPK_BATTLEROOM_COMMAND );
					PACKET_GUARD_VOID( kPacket.Write(BATTLEROOM_TEAM_CHANGE_MAP_LIMIT) );
					pUser->RelayPacket( kPacket );
				}
				else
				{
					TeamType eCurTeam = ChangeTeamType( pUser->GetPublicID(), (TeamType)iTeamType );

					SP2Packet kPacket( STPK_BATTLEROOM_COMMAND );
					PACKET_GUARD_VOID( kPacket.Write(BATTLEROOM_TEAM_CHANGE_OK) );
					PACKET_GUARD_VOID( kPacket.Write(pUser->GetPublicID()) );
					PACKET_GUARD_VOID( kPacket.Write((int)eCurTeam) );
					SendPacketTcp( kPacket );

					m_NodeSync.Update( BattleRoomSync::BRS_CHANGEINFO );
				}
			}				
			else
			{
				SP2Packet kPacket( STPK_BATTLEROOM_COMMAND );
				PACKET_GUARD_VOID( kPacket.Write(BATTLEROOM_TEAM_CHANGE_FULL) );
				PACKET_GUARD_VOID( kPacket.Write(iTeamType) );

				pUser->RelayPacket( kPacket );
			}
		}
		break;
	case BATTLEROOM_CANCEL:
		{
			SP2Packet kPacket( STPK_BATTLEROOM_COMMAND );
			PACKET_GUARD_VOID( kPacket.Write(BATTLEROOM_CANCEL_OK) );

			SendPacketTcp( kPacket );				
		}
		break;
	case BATTLEROOM_MODE_SEL:
		{
			CRASH_GUARD();
			if( !bCommandBlock && GetOwnerName() == pUser->GetPublicID() )
			{
				int  iSelectMode = 0, iSelectMap = 0;
				bool bSafetyLevelRoom = false;

				PACKET_GUARD_VOID( rkPacket.Read(iSelectMode) );
				PACKET_GUARD_VOID( rkPacket.Read(iSelectMap) );
				PACKET_GUARD_VOID( rkPacket.Read(bSafetyLevelRoom) );

				if( bSafetyLevelRoom && !IsSafetyModeCreate() )
				{
					// 안전 레벨의 모드 설정 불가능
					SP2Packet kPacket( STPK_BATTLEROOM_COMMAND );
					PACKET_GUARD_VOID( kPacket.Write(BATTLEROOM_MODE_SEL_FAIL) );
					PACKET_GUARD_VOID( kPacket.Write(m_iSelectMode) );
					PACKET_GUARD_VOID( kPacket.Write(m_iSelectMap) );
					PACKET_GUARD_VOID( kPacket.Write(m_bSafetyLevelRoom) );

					SendPacketTcp( kPacket );
				}
				else
				{
					SP2Packet kPacket( STPK_BATTLEROOM_COMMAND );
					PACKET_GUARD_VOID( kPacket.Write(BATTLEROOM_MODE_SEL_OK) );
					PACKET_GUARD_VOID( kPacket.Write(iSelectMode) );
					PACKET_GUARD_VOID( kPacket.Write(iSelectMap) );
					PACKET_GUARD_VOID( kPacket.Write(bSafetyLevelRoom) );

					SendPacketTcp( kPacket );
					//AI 모드에서 유저를 강제로 블루로 바꿔줘야됨
					ModeType eModeType = GetSelectIndexToMode( iSelectMode, iSelectMap );
					m_bUseExtraOption = false;

					if( eModeType == MT_TEAM_SURVIVAL_AI )
					{						
						int iUserSize = m_vUserNode.size();
						for( int i = 0; i < iUserSize; ++i )
							m_vUserNode[i].m_eTeamType = TEAM_BLUE;
					}
					int  iPrevSelectMode		= m_iSelectMode;
					int  iPrevSelectMap         = m_iSelectMap;
					bool bPrevSafetyLevelRoom	= m_bSafetyLevelRoom;
					m_iSelectMode      = iSelectMode;
					m_iSelectMap       = iSelectMap;
					m_bSafetyLevelRoom = bSafetyLevelRoom;
					if( iPrevSelectMode != m_iSelectMode || bPrevSafetyLevelRoom != m_bSafetyLevelRoom || iPrevSelectMap != m_iSelectMap )
						m_NodeSync.Update( BattleRoomSync::BRS_SELECTMODE );
				}				
			}
		}
		break;
	case BATTLEROOM_START_COUNT:
		{
			SP2Packet kPacket( STPK_BATTLEROOM_COMMAND );
			PACKET_GUARD_VOID( kPacket.Write(BATTLEROOM_START_COUNT_OK) );
			SendPacketTcp( kPacket, pUser->GetUserIndex() );
		}
		break;
	case BATTLEROOM_STOP_COUNT:
		{			
			SP2Packet kPacket( STPK_BATTLEROOM_COMMAND );
			PACKET_GUARD_VOID( kPacket.Write(BATTLEROOM_STOP_COUNT_OK) );
			PACKET_GUARD_VOID( kPacket.Write(pUser->GetPublicID()) );
			SendPacketTcp( kPacket, pUser->GetUserIndex() );				
		}
		break;
	case BATTLEROOM_READY_GO:
		{

#ifdef BATTLE_MODE_BY_BCKIM		// 2019-02-14 by bckim, 배틀 모드 추가  ReadyGoBattleModeSetOrder3vs3  // ReadyGoBattleModeSetOrder2vs2 
			if( GetSelectMode() == MT_BATTLE )
				SendBattleModeSelectInfo(pUser);
#endif // BATTLE_MODE_BY_BCKIM		

			OnBattleRoomReadyGO();
		}
		break;
	case BATTLEROOM_RUNNER_SELECT:	// 2019-02-14 by bckim, 배틀 모드 추가
		{
#ifdef BATTLE_MODE_BY_BCKIM		// 2019-02-14 by bckim, 배틀 모드 추가  BATTLEROOM_RUNNER_SELECT  // BATTLEROOM_RUNNER_SELECT_OK 
			if( GetSelectMode() == MT_BATTLE && IsObserver( pUser->GetPublicID()) == false ) 
			{
				int iOrderNum = 0;
				PACKET_GUARD_VOID( rkPacket.Read(iOrderNum) );
				int iUserSize = m_vUserNode.size();
				for( int i = 0; i < iUserSize; ++i )
				{
					if( m_vUserNode[i].m_dwUserIndex == pUser->GetUserIndex())
						m_vUserNode[i].m_eSelect_Order = iOrderNum;
				}	

				SP2Packet kPacket( STPK_BATTLEROOM_COMMAND );
				PACKET_GUARD_VOID( kPacket.Write(BATTLEROOM_RUNNER_SELECT_OK) );
				PACKET_GUARD_VOID( kPacket.Write(pUser->GetPublicID()) );
				PACKET_GUARD_VOID( kPacket.Write(iOrderNum));
				SendPacketTcp( kPacket);	
#endif // BATTLE_MODE_BY_BCKIM	
			}
		}
		break;
	
	case BATTLEROOM_TOURNAMENT_START:
		{
			//
			if( GetBattleEventType() == BET_TOURNAMENT_BATTLE )
			{
				// Cheat!!!!
				bool bCheat = false;
				DWORD dwGapTime = TIMEGETTIME() - m_TournamentRoundData.m_dwInviteTimer;
				if( dwGapTime < TournamentRoundData::INVITE_DELAY_TIME - 60000 )
				{
					bCheat = true;
				}

				// 예비 엔트리 필요? ( 정규 리그만 예비 엔트리 )
				bool bChangeTeam = false;
				if( m_TournamentRoundData.m_bRegularTour )  
				{
					if( m_TournamentRoundData.m_BlueTourPos == 1 )        // 첫 라운드이면 예비 엔트리 요청
					{
						if( !m_TournamentRoundData.m_bChangeEntry )       // 이미 요청했으면 결과 처리
						{
							bChangeTeam = true;
						}
					}
				}

				// 양팀 확인하여 대전이나 결과 전송!!
				int iBlueUserCount = GetUserTeamCount( TEAM_BLUE );
				int iRedUserCount  = GetUserTeamCount( TEAM_RED );

				// 동수는 바로 시작한다.
				if( iBlueUserCount == iRedUserCount )
				{
					bCheat = false;
				}

				if( bCheat )
				{
					LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s - Cheat Tournament Battle Room BATTLEROOM_TOURNAMENT_START", pUser->GetPublicID().c_str() );
					// 
					SP2Packet kPacket( STPK_BATTLEROOM_COMMAND );
					PACKET_GUARD_VOID( kPacket.Write(BATTLEROOM_TOURNAMENT_START_CHEAT) );
					pUser->RelayPacket( kPacket );
				}
				else if( iBlueUserCount == 0 )
				{
					if( bChangeTeam )
					{
						// 예비 엔트리 요청
						m_TournamentRoundData.m_bChangeEntry = true;

						SP2Packet kPacket( MSTPK_TOURNAMENT_BATTLE_TEAM_CHANGE );
						PACKET_GUARD_VOID( kPacket.Write(m_TournamentRoundData.m_dwTourIndex) );
						PACKET_GUARD_VOID( kPacket.Write(m_TournamentRoundData.m_dwBlueIndex) );
						PACKET_GUARD_VOID( kPacket.Write(GetIndex()) );
						g_MainServer.SendMessage( kPacket );
						
						// 예비 엔트리 알림
						SP2Packet kPacket2( STPK_BATTLEROOM_COMMAND );
						PACKET_GUARD_VOID( kPacket2.Write(BATTLEROOM_TOURNAMENT_DELETE_TEAM) );
						PACKET_GUARD_VOID( kPacket2.Write(m_TournamentRoundData.m_dwBlueIndex) );
						PACKET_GUARD_VOID( kPacket2.Write(TournamentRoundData::INVITE_DELAY_TIME) );
						SendPacketTcp( kPacket2 );

						// 팀 초기화
						m_TournamentRoundData.m_dwBlueIndex = 0;
						m_TournamentRoundData.m_BlueUserIndex.clear();

						// 시간 초기화
						m_TournamentRoundData.m_dwInviteTimer = TIMEGETTIME();						

						if( IsRegularTournamentBattle() )
						{							
							LOG.PrintTimeAndLog( 0, "[대회로그] %s - %s - 블루팀 예비엔트리 요청 : 배틀룸 : %d, 참가하지 않은 팀 : %d", __FUNCTION__, 
								pUser->GetPublicID().c_str(), GetIndex(), m_TournamentRoundData.m_dwBlueIndex );
						}
					}
					else if( m_TournamentRoundData.m_bSendResult == false )
					{
						//레드팀 부전승
						TournamentUnearnedWinCommand( TEAM_RED, pUser->GetPublicID() );
					}
					else
					{
						LOG.PrintTimeAndLog( 0, "[대회로그] %s - %s - 로직 에러 : 배틀룸 : %d, 참가하지 않은 블루 팀 : %d", __FUNCTION__,
							pUser->GetPublicID().c_str(), GetIndex(), m_TournamentRoundData.m_dwBlueIndex );
					}
				}
				else if( iRedUserCount == 0 )
				{
					if( bChangeTeam )
					{
						// 예비 엔트리 요청
						m_TournamentRoundData.m_bChangeEntry = true;

						SP2Packet kPacket( MSTPK_TOURNAMENT_BATTLE_TEAM_CHANGE );
						PACKET_GUARD_VOID( kPacket.Write(m_TournamentRoundData.m_dwTourIndex) );
						PACKET_GUARD_VOID( kPacket.Write(m_TournamentRoundData.m_dwRedIndex) );
						PACKET_GUARD_VOID( kPacket.Write(GetIndex()) );
						g_MainServer.SendMessage( kPacket );

						// 예비 엔트리 알림
						SP2Packet kPacket2( STPK_BATTLEROOM_COMMAND );
						PACKET_GUARD_VOID( kPacket2.Write(BATTLEROOM_TOURNAMENT_DELETE_TEAM) );
						PACKET_GUARD_VOID( kPacket2.Write(m_TournamentRoundData.m_dwRedIndex) );
						PACKET_GUARD_VOID( kPacket2.Write(TournamentRoundData::INVITE_DELAY_TIME) );
						SendPacketTcp( kPacket2 );

						m_TournamentRoundData.m_dwRedIndex = 0;
						m_TournamentRoundData.m_RedUserIndex.clear();

						// 시간 초기화
						m_TournamentRoundData.m_dwInviteTimer = TIMEGETTIME();

						if( IsRegularTournamentBattle() )
						{
							LOG.PrintTimeAndLog( 0, "[대회로그] %s 레드팀 예비엔트리 요청 : 배틀룸 : %d, 참가하지 않은 팀 : %d", __FUNCTION__, GetIndex(), m_TournamentRoundData.m_dwBlueIndex );
						}
					}
					else if( m_TournamentRoundData.m_bSendResult == false )
					{
						//블루팀 부전승
						TournamentUnearnedWinCommand( TEAM_BLUE, pUser->GetPublicID() );
					}
					else
					{
						LOG.PrintTimeAndLog( 0, "[대회로그] %s - %s - 로직 에러 : 배틀룸 : %d, 참가하지 않은 레드 팀 : %d", __FUNCTION__, pUser->GetPublicID().c_str(), GetIndex(), m_TournamentRoundData.m_dwRedIndex );
					}
				}
				else 
				{
					// 전투 시작
					SP2Packet kPacket( STPK_BATTLEROOM_COMMAND );
					PACKET_GUARD_VOID( kPacket.Write(BATTLEROOM_START_COUNT_OK) );
					SendPacketTcp( kPacket );

					if( IsRegularTournamentBattle() )
					{
						LOG.PrintTimeAndLog( 0, "[대회로그] %s 카운트 시작 : 배틀룸 : %d", __FUNCTION__, GetIndex() );
					}
				}				
			}
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s - BattleRoom Tournament Start %d", pUser->GetPublicID().c_str(), GetBattleEventType() );
		}
		break;
	}
}

void BattleRoomNode::OnBattleRoomReadyGO()
{
	// 경기 시작전 대회모드 부전승 체크
	int iBlueUserCount = GetUserTeamCount( TEAM_BLUE );
	int iRedUserCount  = GetUserTeamCount( TEAM_RED );

	if( IsRegularTournamentBattle() )
	{
		if( iBlueUserCount == 0 && 0 < iRedUserCount )
		{
			//레드팀 부전승
			TournamentUnearnedWinCommand( TEAM_RED, "래디된 상태에서의 부전승" );
			
			return;
		}
		else if( iRedUserCount == 0 && 0 < iBlueUserCount )
		{
			//블루팀 부전승
			TournamentUnearnedWinCommand( TEAM_BLUE, "래디된 상태에서의 부전승" );
			return;
		}
	}

	ioHashString szTitle;
	if( m_pBattleRoom && !m_pBattleRoom->IsRoomEmpty() && m_pBattleRoom->GetRoomStyle() == RSTYLE_BATTLEROOM )
	{
		// 룸에 있는 경우
		int iMapNum = -1;
		int iSubType = -1;
		ModeType eModeType = MT_NONE;
		int iModeIndex = GetSelectMode();
		int iMapIndex  = GetSelectMap();

		bool bPrevSafetyRoom    = m_pBattleRoom->IsSafetyLevelRoom();
		bool bPrevBroadcastRoom = m_pBattleRoom->IsBroadcastRoom();
        m_pBattleRoom->SetSafetyRoom( m_bSafetyLevelRoom );
		m_pBattleRoom->SetBroadcastRoom( (GetBattleEventType() == BET_BROADCAST_MBC) );
		m_pBattleRoom->SetTournamentRoom( (GetBattleEventType() == BET_TOURNAMENT_BATTLE), m_TournamentRoundData.m_dwTourIndex );
		m_pBattleRoom->SetRoomStyle( RSTYLE_BATTLEROOM );
		
		if( bPrevSafetyRoom != m_pBattleRoom->IsSafetyLevelRoom() ||
			bPrevBroadcastRoom != m_pBattleRoom->IsBroadcastRoom() )
		{
			m_pBattleRoom->InitModeTypeList();
		}

		if( iModeIndex == -1 )	
		{
			eModeType = m_pBattleRoom->SelectNextMode( MT_NONE );
		}
		else					//기타
		{
			bool bSuccess = g_BattleRoomManager.CheckBattleRandomMode( iModeIndex, iMapIndex, eModeType, iSubType );
			if( bSuccess )
			{
				eModeType = m_pBattleRoom->SelectNextMode( eModeType, iSubType );
			}
			else
			{
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "BattleRoomNode::OnBattleRoomCommand() - SelectIndex is not Exist(%d): %d", GetIndex(), iModeIndex );
				eModeType = m_pBattleRoom->SelectNextMode( MT_NONE );
			}
		}

		// 옵션 세팅
		if( m_bRandomTeamMode )
		{
			m_pBattleRoom->NextShamBattleRandomTeam( eModeType );
		}

		m_pBattleRoom->SetModeType( eModeType, GetCatchModeRoundType(), GetCatchModeRoundTimeType() );
		{
			SP2Packet kPacket( STPK_BATTLEROOM_COMMAND );
			PACKET_GUARD_VOID( kPacket.Write(BATTLEROOM_READY_GO_OK) );
			PACKET_GUARD_VOID( kPacket.Write(m_pBattleRoom->GetModeType()) );
			PACKET_GUARD_VOID( kPacket.Write(m_pBattleRoom->GetModeSubNum()) );
			PACKET_GUARD_VOID( kPacket.Write(m_pBattleRoom->GetModeMapNum()) );
			SendPacketTcp( kPacket );

			if( IsRegularTournamentBattle() )
			{
				LOG.PrintTimeAndLog( 0, "[대회로그] %s 게임 모드 시작 : 배틀룸 : %d", __FUNCTION__, GetIndex() );
			}
		}

		if( eModeType == MT_BOSS )
			CreateBoss();
		else if( eModeType == MT_GANGSI )
			CreateGangsi();
		else if ( eModeType == MT_TEAM_SURVIVAL_AI )
			SetAIMode( m_pBattleRoom );

		//m_pBattleRoom->CheckUseFightNPC();
		m_pBattleRoom->CreateNextShamBattle();

		eModeType = m_pBattleRoom->GetModeType();
		iSubType = m_pBattleRoom->GetModeSubNum();
		iMapNum = m_pBattleRoom->GetModeMapNum();

		if( m_pBattleRoom && CheckUseFightNPC() && eModeType == MT_FIGHT_CLUB )
		{
			m_pBattleRoom->CheckUseFightNPC();
		}

		SetPreSelectModeInfo( eModeType, iSubType, iMapNum );

		LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "BattleRoomNode::OnBattleRoomCommand() Already ReadyGO(%d) : %d", GetIndex(), m_pBattleRoom->GetRoomIndex() );
		m_vInviteUser.clear();

		//160913 HRYOON 포탈로그 추가
		//HRYOON0825 게임방 입장로그
		//전투 시작시 양팀 인원, 양팀 최대 인원, 팀섞기 옵션, 공개방 체크유무, 유저모드체크유무
		if( eModeType == MT_CATCH  )
		{
			g_LogDBClient.OnInsertEnterBattleRoomLog( m_pBattleRoom->GetRoomIndex(), GetUserTeamCount(TEAM_BLUE), GetUserTeamCount(TEAM_RED), GetMaxPlayerBlue(), GetMaxPlayerRed(), m_bRandomTeamMode, IsPassword(), IsUseExtraOption());
		}


	}
	else
	{
		Room *pRoom = g_RoomNodeManager.CreateNewRoom();
		if( pRoom )
		{	
			int iMapNum = -1;
			int iSubType = -1;
			ModeType eModeType = MT_NONE;
			int iModeIndex = GetSelectMode();
			int iMapIndex  = GetSelectMap();

            pRoom->SetSafetyRoom( m_bSafetyLevelRoom );
			pRoom->SetBroadcastRoom( (GetBattleEventType() == BET_BROADCAST_MBC) );
			pRoom->SetTournamentRoom( (GetBattleEventType() == BET_TOURNAMENT_BATTLE), m_TournamentRoundData.m_dwTourIndex );
			pRoom->SetRoomStyle( RSTYLE_BATTLEROOM );
			pRoom->InitModeTypeList();

			//앞선 플레이 정보 반영
			int iPreMapNum, iPreSubType;
			ModeType ePreModeType;

			GetPreSelectModeInfo( ePreModeType, iPreSubType, iPreMapNum );
			pRoom->SetPreSelectModeInfo( ePreModeType, iPreSubType, iPreMapNum );
			//

			if( iModeIndex == -1 )	//랭킹
			{
				eModeType = pRoom->SelectNextMode( MT_NONE );
			}
			else					//기타
			{
				bool bSuccess = g_BattleRoomManager.CheckBattleRandomMode( iModeIndex, iMapIndex, eModeType, iSubType );
				if( bSuccess )
				{
					eModeType = pRoom->SelectNextMode( eModeType, iSubType );
				}
				else
				{
					LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "BattleRoomNode::OnBattleRoomCommand() - SelectIndex is not Exist(%d): %d", GetIndex(), iModeIndex );
					eModeType = pRoom->SelectNextMode( MT_NONE );
				}
			}

			pRoom->SetModeType( eModeType, GetCatchModeRoundType(), GetCatchModeRoundTimeType() );
			if( eModeType != MT_SURVIVAL && eModeType != MT_BOSS && eModeType != MT_GANGSI && eModeType != MT_MONSTER_SURVIVAL && 
				eModeType != MT_DUNGEON_A && eModeType != MT_FIGHT_CLUB && eModeType != MT_RAID && 
				!Help::IsMonsterDungeonMode(eModeType) )
				BattleEnterRandomTeam();
			if( eModeType == MT_BOSS )
				CreateBoss();
			else if( eModeType == MT_GANGSI )
				CreateGangsi();
			else if ( eModeType == MT_TEAM_SURVIVAL_AI )
 				SetAIMode( pRoom );

			BattleEnterRoom( pRoom );

			eModeType = pRoom->GetModeType();
			iSubType = pRoom->GetModeSubNum();
			iMapNum = pRoom->GetModeMapNum();

			if( m_pBattleRoom && CheckUseFightNPC() && eModeType == MT_FIGHT_CLUB )
			{
				m_pBattleRoom->CheckUseFightNPC();
			}

			SetPreSelectModeInfo( eModeType, iSubType, iMapNum );

			if( IsRegularTournamentBattle() )
			{
				LOG.PrintTimeAndLog( 0, "[대회로그] %s 게임 모드 시작 : 배틀룸 : %d", __FUNCTION__, GetIndex() );
			}
			else
			{
				LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "BattleRoomNode::OnBattleRoomCommand() New ReadyGO(%d) : %d", GetIndex(), m_pBattleRoom->GetRoomIndex() );
			}
			//160913 HRYOON 포탈로그 추가
			//HRYOON0825 게임방 입장로그
			//전투 시작시 양팀 인원, 양팀 최대 인원, 팀섞기 옵션, 공개방 체크유무, 유저모드체크유무
			if( eModeType == MT_CATCH  )
			{
				g_LogDBClient.OnInsertEnterBattleRoomLog( m_pBattleRoom->GetRoomIndex(), GetUserTeamCount(TEAM_BLUE), GetUserTeamCount(TEAM_RED), GetMaxPlayerBlue(), GetMaxPlayerRed(), m_bRandomTeamMode, IsPassword(), IsUseExtraOption());
			}

		}
		else
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"BATTLEROOM_READY_GO : 잔여 룸이 없다(%d)", GetIndex() );
	}

	// 2019-02-14 by bckim, 배틀 모드 추가
	if( m_pBattleRoom && m_pBattleRoom->GetModeType() == MT_BATTLE)
	{
		vBattleRoomUser_iter iter, iEnd;
		iEnd = m_vUserNode.end();
		for( iter=m_vUserNode.begin() ; iter!=iEnd ; ++iter )
		{
			BattleRoomUser &kUser = *iter;
			//m_pBattleRoom->SetBattleModeOrder(kUser.m_dwUserIndex , kUser.m_eSelect_Order);		
		}
	}
	// End. 2019-02-14 by bckim, 배틀 모드 추가

	m_NodeSync.Update( BattleRoomSync::BRS_PLAY );                  // 플레이 갱신
}

bool BattleRoomNode::CheckUseFightNPC()
{
	int iPlayUserCnt = 0;

	vBattleRoomUser_iter iter, iEnd;
	iEnd = m_vUserNode.end();
	for( iter=m_vUserNode.begin() ; iter!=iEnd ; ++iter )
	{
		BattleRoomUser &kUser = *iter;

		if( kUser.m_bObserver )
			continue;

		iPlayUserCnt++;
	}

	if( iPlayUserCnt == 1 )
		return true;

	return false;
}


bool BattleRoomNode::IsRegularTournamentBattle()
{
	if( GetBattleEventType() == BET_TOURNAMENT_BATTLE && m_TournamentRoundData.m_dwTourIndex == TOURNAMENT_REGULAR_INDEX )
		return true;
	
	return false;
}

void BattleRoomNode::TournamentUnearnedWinCommand( TeamType eUnearnedWinTeam,  const ioHashString& szUserName )
{
	switch( eUnearnedWinTeam )
	{
	case TEAM_RED:
		{
			// 레드팀 부전승
			SP2Packet kPacket( MSTPK_TOURNAMENT_BATTLE_RESULT );
			PACKET_GUARD_VOID( kPacket.Write(m_TournamentRoundData.m_dwTourIndex) );
			PACKET_GUARD_VOID( kPacket.Write(m_TournamentRoundData.m_dwRedIndex) );
			PACKET_GUARD_VOID( kPacket.Write(m_TournamentRoundData.m_dwBlueIndex) );
			PACKET_GUARD_VOID( kPacket.Write(m_TournamentRoundData.m_RedTourPos + 1) );
			g_MainServer.SendMessage( kPacket );

			// 부전승 알림
			SP2Packet kPacket2( STPK_BATTLEROOM_COMMAND );
			PACKET_GUARD_VOID( kPacket2.Write(BATTLEROOM_TOURNAMENT_DRAW_BYE) );
			SendPacketTcp( kPacket2 );

			m_TournamentRoundData.m_bSendResult = true;

			if( IsRegularTournamentBattle() )
			{
				LOG.PrintTimeAndLog( 0, "[대회로그]  %s - %s 레드팀 부전승 : 배틀룸 : %d", __FUNCTION__, szUserName.c_str(), GetIndex() );
			}
		}
		break;
	case TEAM_BLUE:
		{
			// 블루팀 부전승
			SP2Packet kPacket( MSTPK_TOURNAMENT_BATTLE_RESULT );
			PACKET_GUARD_VOID( kPacket.Write(m_TournamentRoundData.m_dwTourIndex) );
			PACKET_GUARD_VOID( kPacket.Write(m_TournamentRoundData.m_dwBlueIndex) );
			PACKET_GUARD_VOID( kPacket.Write(m_TournamentRoundData.m_dwRedIndex) );
			PACKET_GUARD_VOID( kPacket.Write(m_TournamentRoundData.m_BlueTourPos + 1) );
			g_MainServer.SendMessage( kPacket );

			// 부전승 알림
			SP2Packet kPacket2( STPK_BATTLEROOM_COMMAND );
			PACKET_GUARD_VOID( kPacket2.Write(BATTLEROOM_TOURNAMENT_DRAW_BYE) );
			SendPacketTcp( kPacket2 );

			m_TournamentRoundData.m_bSendResult = true;

			if( IsRegularTournamentBattle() )
			{
				LOG.PrintTimeAndLog( 0, "[대회로그]  %s - %s 블루팀 부전승 : 배틀룸 : %d", __FUNCTION__, szUserName.c_str(), GetIndex() );
			}
		}
		break;
	default:
		{
			LOG.PrintTimeAndLog( 0, "%s - not unknown team type", __FUNCTION__ );
		}
	}
}

void BattleRoomNode::SetAIMode( Room* pRoom )
{
	if( !pRoom )
		return;

	TeamSurvivalAIMode* pMode = ToTeamSurvivalAIMode( pRoom->GetModeInfo() );
	if( pMode )
		pMode->SetAILevel( m_iAILevel );
}

// 2019-02-14 by bckim, 배틀 모드 추가
int BattleRoomNode::IsExistOrderType(TeamType team, int eType)
{

	int cnt = 0;
	int iUserSize = m_vBMCopyUserNode.size();
	for( int i = 0; i < iUserSize; ++i )
	{
		if( m_vBMCopyUserNode[i].m_bObserver == false && m_vBMCopyUserNode[i].m_eTeamType == team && m_vBMCopyUserNode[i].m_iResult_Order == eType )
			cnt++;
	}
	return cnt;

		/*
	int cnt = 0;
	int iUserSize = m_vUserNode.size();
	for( int i = 0; i < iUserSize; ++i )
	{
		if( m_vUserNode[i].m_bObserver == false && m_vUserNode[i].m_eTeamType == team && m_vUserNode[i].m_eSelect_Order == eType )
			cnt++;
	}
	return cnt;
	*/
}

bool BattleRoomNode::SetEmptyOrder(TeamType team, int i_maxcnt, int source)
{
	for(int i=0; i<i_maxcnt; i++)
	{
		int i_Cnt = IsExistOrderType(team, i+1);
		if( i_Cnt == 0 )
		{
			SetUserOrderTypeAtoB(team, source ,i+1 );
			return true;
		}
	}
	return false;
}

bool BattleRoomNode::SetUserOrderTypeAtoB(TeamType Team, int type_a, int type_b )
{
	int iUserSize = m_vBMCopyUserNode.size();
	for( int i = 0; i < iUserSize; ++i )
	{
		if( m_vBMCopyUserNode[i].m_eTeamType == Team && m_vBMCopyUserNode[i].m_bObserver == false &&  m_vBMCopyUserNode[i].m_iResult_Order == type_a)
		{
			m_vBMCopyUserNode[i].m_iResult_Order = type_b;
			return true;
		}
	}
	return false;

/*
	int iUserSize = m_vUserNode.size();
	for( int i = 0; i < iUserSize; ++i )
	{
		if( m_vUserNode[i].m_eTeamType == Team && m_vUserNode[i].m_bObserver == false &&  m_vUserNode[i].m_eSelect_Order == type_a)
		{
			m_vUserNode[i].m_eSelect_Order = type_b;
			return true;
		}
	}
	return false;
	*/
}

int BattleRoomNode::AllRndSetType(TeamType Team, int type)
{

	int i_Increase = 1;
	int iUserSize = m_vBMCopyUserNode.size();
	for( int i = 0; i < iUserSize; ++i )
	{
		if( m_vBMCopyUserNode[i].m_eTeamType == Team && m_vBMCopyUserNode[i].m_bObserver == false &&  m_vBMCopyUserNode[i].m_iResult_Order == type)
		{
			m_vBMCopyUserNode[i].m_iResult_Order = i_Increase;
			i_Increase++;			
		}
	}
	return i_Increase;

	/*
	int i_Increase = 1;
	int iUserSize = m_vUserNode.size();
	for( int i = 0; i < iUserSize; ++i )
	{
		if( m_vUserNode[i].m_eTeamType == Team && m_vUserNode[i].m_bObserver == false &&  m_vUserNode[i].m_eSelect_Order == type)
		{
			m_vUserNode[i].m_eSelect_Order = i_Increase;
			i_Increase++;			
		}
	}
	return i_Increase;
	*/
}

bool BattleRoomNode::ReadyGoBattleModeSetOrder2vs2(TeamType Team)
{
	int blueCNT = 3;
	for( int order = 0; order<blueCNT; order++)
	{
		int i_OrderTypeCnt = IsExistOrderType(Team,order);
		switch(i_OrderTypeCnt)
		{
		case 0:	break;
		case 1:
			{
				if(order == 0)// 0인 자리 찾아서 넣어
				{	
					SetEmptyOrder(Team,blueCNT,order);
				}
			}
			break;
		case 2:// 자동 랜덤 분배 
			AllRndSetType(Team,order);
			break;
		}
	}
	return true;
}

bool BattleRoomNode::ReadyGoBattleModeSetOrder3vs3(TeamType Team)
{
	int blueCNT = 4;
	for( int order = 0; order<blueCNT; order++)
	{
		int i_OrderTypeCnt = IsExistOrderType(Team,order);
		switch(i_OrderTypeCnt)
		{
		case 0:
			break;
		case 1:
			{
				if(order == 0)// 0인 자리 찾아서 넣어
					SetEmptyOrder(Team,blueCNT,order);
			}
			break;
		case 2:
			{	
				if(order == 0 )
				{
					SetEmptyOrder(Team,blueCNT,order);
					SetEmptyOrder(Team,blueCNT,order);
					break;
				}						
				SetEmptyOrder(Team,blueCNT,order);
			}
			break;
		case 3:	//	 자동 랜덤 분배 	
			AllRndSetType(Team,order);
			break;
		}
	}
	return true;
}

void BattleRoomNode::SendBattleModeSelectInfo(UserParent *pUser)
{

#ifdef BATTLE_MODE_BY_BCKIM_DEBUG 	// 2019-02-14 by bckim, 배틀 모드 추가  BATTLEROOM_RUNNER_SELECT  // BATTLEROOM_RUNNER_SELECT_OK 
	LOG.PrintTimeAndLog(0,"[BATTLE_MODE_BY_BCKIM:BattleRoomNode::OnBattleRoomReadyGO] A_NAME[%s]",pUser->GetPrivateID().c_str());
	{// 검증 
		vBattleRoomUser_iter iter, iEnd;
		iEnd = m_vUserNode.end();
		for( iter=m_vUserNode.begin() ; iter!=iEnd ; ++iter )
		{
			BattleRoomUser &kUser = *iter;
			UserParent *pUser = g_UserNodeManager.GetGlobalUserNode( kUser.m_dwUserIndex );
			if( pUser && 	kUser.m_bObserver == false )
			{		
				if(kUser.m_eTeamType == TEAM_BLUE )
					LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"[ORDER[BEFORE]>>>>Clent[%s]TEAM[BLUE]ORDER[%d]",pUser->GetPublicID().c_str(),kUser.m_eSelect_Order);
				if(kUser.m_eTeamType == TEAM_RED )
					LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"[ORDER[BEFORE]>>>>Clent[%s]TEAM[_RED]ORDER[%d]",pUser->GetPublicID().c_str(),kUser.m_eSelect_Order);
			}
		}
	} 	// End. 검증 
#endif //  BATTLE_MODE_BY_BCKIM_DEBUG		// 2019-02-14 by bckim, 배틀 모드 추가

	if( GetSelectMode() != MT_BATTLE )
		return;


	//////////////////////////////////////////////////////////////////////////

	
	
	m_vBMCopyUserNode.clear();
	vBattleRoomUser_iter iter2, iEnd2;
	iEnd2 = m_vUserNode.end();
	for( iter2=m_vUserNode.begin() ; iter2!=iEnd2 ; ++iter2 )
	{
		BMRoomUser CopyUserNode;
		CopyUserNode.m_dwUserIndex		= iter2->m_dwUserIndex;
		CopyUserNode.m_eTeamType		= iter2->m_eTeamType;
		CopyUserNode.m_bObserver		= iter2->m_bObserver;
		CopyUserNode.m_iResult_Order	= iter2->m_eSelect_Order;

		if( 2 == GetSelectMap() && CopyUserNode.m_iResult_Order > BATTLE_ORDER_SECOND )		// 2:2
			CopyUserNode.m_iResult_Order = BATTLE_ORDER_RANDOM;
		m_vBMCopyUserNode.push_back(CopyUserNode);
	}

	io_random_shuffle( m_vBMCopyUserNode.begin(), m_vBMCopyUserNode.end() );

	if( 1 == GetSelectMap() )
	{
		ReadyGoBattleModeSetOrder3vs3(TEAM_BLUE);
		ReadyGoBattleModeSetOrder3vs3(TEAM_RED);				
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"[MT_BATTLE]>>>> 3vs3 SET ORDER !!!! ");
	}
	if( 2 == GetSelectMap() )
	{
		ReadyGoBattleModeSetOrder2vs2(TEAM_BLUE);
		ReadyGoBattleModeSetOrder2vs2(TEAM_RED);
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"[MT_BATTLE]>>>> 2vs2 SET ORDER !!!! ");
	}	

	vBattleRoomUser_iter iter3, iEnd3;
	iEnd3 = m_vUserNode.end();
	for( iter3=m_vUserNode.begin() ; iter3!=iEnd3 ; ++iter3 )
	{
		int tempsize= m_vBMCopyUserNode.size();
		for( int i=0; i<tempsize; i++)
		{
			if( m_vBMCopyUserNode[i].m_dwUserIndex == 	iter3->m_dwUserIndex )
				iter3->m_eSelect_Order = m_vBMCopyUserNode[i].m_iResult_Order;
		}
	}
	


	int iUserSize = m_vUserNode.size();
	SP2Packet kPacket( STPK_BATTLEROOM_COMMAND );
	PACKET_GUARD_VOID( kPacket.Write(BATTLEROOM_RUNNER_SELECT_DONE) );
	PACKET_GUARD_VOID( kPacket.Write(iUserSize));

	vBattleRoomUser_iter iter, iEnd;
	iEnd = m_vUserNode.end();
	for( iter=m_vUserNode.begin() ; iter!=iEnd ; ++iter )
	{
		BattleRoomUser &kUser = *iter;
		UserParent *pUser = g_UserNodeManager.GetGlobalUserNode( kUser.m_dwUserIndex );
		if( pUser )
		{ 	// 순서  user 저장 
			PACKET_GUARD_VOID( kPacket.Write(pUser->GetPublicID()) );
			PACKET_GUARD_VOID( kPacket.Write(kUser.m_eSelect_Order));	

#ifdef BATTLE_MODE_BY_BCKIM_DEBUG 	// 2019-02-14 by bckim, 배틀 모드 추가  BATTLEROOM_RUNNER_SELECT  // BATTLEROOM_RUNNER_SELECT_OK 
			if(kUser.m_eTeamType == TEAM_BLUE )
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"[BATTLEROOM_RUNNER_SELECT_DONE>>Clent[%s]TEAM[BLUE]ORDER[%d]",pUser->GetPublicID().c_str(),kUser.m_eSelect_Order);
			if(kUser.m_eTeamType == TEAM_RED )
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"[BATTLEROOM_RUNNER_SELECT_DONE>>Clent[%s]TEAM[_RED]ORDER[%d]",pUser->GetPublicID().c_str(),kUser.m_eSelect_Order);
#endif

			if( pUser->IsUserOriginal())
				pUser->SetBattleModeOrder(kUser.m_eSelect_Order);
			else
			{
				UserCopyNode *pCopyUser = static_cast< UserCopyNode * >( pUser );
				if( pCopyUser )
				{
					pCopyUser->SetBattleModeOrder(kUser.m_eSelect_Order);
					SP2Packet kPacket( SSTPK_BATTLE_MODE_ORDER );
					PACKET_GUARD_VOID( kPacket.Write( pCopyUser->GetUserIndex() ) );
					PACKET_GUARD_VOID( kPacket.Write( pCopyUser->GetBattleModeOrder()) );
					pCopyUser->SendMessage( kPacket );
					LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"[SSTPK_BATTLE_MODE_ORDER>>Clent[%s] ORDER[%d][%d]",pCopyUser->GetPublicID().c_str(),pCopyUser->GetBattleModeOrder(),kUser.m_eSelect_Order);
				}
			}
		}
	}

	SendPacketTcp( kPacket);	

#ifdef BATTLE_MODE_BY_BCKIM_DEBUG 	// 2019-02-14 by bckim, 배틀 모드 추가  BATTLEROOM_RUNNER_SELECT  // BATTLEROOM_RUNNER_SELECT_OK 
	{	// 검증 
		vBattleRoomUser_iter iter, iEnd;
		iEnd = m_vUserNode.end();
		for( iter=m_vUserNode.begin() ; iter!=iEnd ; ++iter )
		{
			BattleRoomUser &kUser = *iter;
			UserParent *pUser = g_UserNodeManager.GetGlobalUserNode( kUser.m_dwUserIndex );
			if( pUser && 	kUser.m_bObserver == false )
			{		
				if(kUser.m_eTeamType == TEAM_BLUE )				
					LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"[ORDER[AFTER]>>>>Clent[%s]TEAM[BLUE]ORDER[%d]",pUser->GetPublicID().c_str(),kUser.m_eSelect_Order);
				if(kUser.m_eTeamType == TEAM_RED )
					LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"[ORDER[AFTER]>>>>Clent[%s]TEAM[_RED]ORDER[%d]",pUser->GetPublicID().c_str(),kUser.m_eSelect_Order);				
			}
		}
	} 	// End. 검증 			
#endif //  BATTLE_MODE_BY_BCKIM		// 2019-02-14 by bckim, 배틀 모드 추가

	return;
}

bool BattleRoomNode::IsBattleTeamChangeOK_OnBattleMode( TeamType eChangeTeam )
{
	if( GetSelectMode() != MT_BATTLE )
		return false;	

	int iTeamCount = 0;
	vBattleRoomUser_iter iter, iEnd;
	iEnd = m_vUserNode.end();
	for( iter=m_vUserNode.begin() ; iter!=iEnd ; ++iter )
	{
		BattleRoomUser &kUser = *iter;
		if( kUser.m_eTeamType == eChangeTeam )
			iTeamCount++;
	}

#ifdef BATTLE_MODE_BY_BCKIM_DEBUG	// 2019-02-14 by bckim, 배틀 모드 추가	IsBattleTeamChangeOK
	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[BATTLE_MODE_BY_BCKIM] IsBattleTeamChangeOK [%d][%d]",eChangeTeam,iTeamCount);
#endif  // BATTLE_MODE_BY_BCKIM_DEBUG

	if( eChangeTeam == TEAM_BLUE )
	{
		if( iTeamCount >= m_iMaxPlayerBlue+1 )
			return false;
	}
	else if( eChangeTeam == TEAM_RED )
	{
		if( iTeamCount >= m_iMaxPlayerRed+1 )
			return false;
	}
	else if( eChangeTeam == TEAM_NONE )
	{
		if( iTeamCount >= m_iMaxObserver )
			return false;
	}
	return true;
}

// End. 2019-02-14 by bckim, 배틀 모드 추가