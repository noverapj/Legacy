#include "stdafx.h"

#include "BattleRoomCopyNode.h"
#include "BattleRoomManager.h"
#include "ServerNode.h"

BattleRoomCopyNode::BattleRoomCopyNode()
{
	InitData();
}

BattleRoomCopyNode::~BattleRoomCopyNode()
{
}

void BattleRoomCopyNode::OnCreate( ServerNode *pCreator )
{
	CopyNodeParent::OnCreate( pCreator );
	m_eCopyType = BATTLEROOM_TYPE;
	InitData();
}

void BattleRoomCopyNode::OnDestroy()
{
	BlockNode::Reset();
	CopyNodeParent::OnDestroy();
}

void BattleRoomCopyNode::ApplySyncSelectMode( SP2Packet &rkPacket )
{
	rkPacket >> m_iSelectModeTerm >> m_iSelectMode >> m_iSelectMap;

	m_iMapLimitPlayer = g_BattleRoomManager.GetBattleMapToLimitPlayer( m_iSelectMode, m_iSelectMap );
	m_iMapLimitGrade  = g_BattleRoomManager.GetBattleMapToLimitGrade( m_iSelectMode, m_iSelectMap );
	m_iMapStartCoinCnt  = g_BattleRoomManager.GetBattleMapToStartCoin( m_iSelectMode, m_iSelectMap );
}

void BattleRoomCopyNode::ApplySyncPlay( SP2Packet &rkPacket )
{
	rkPacket >> m_iSelectModeTerm >> m_iSelectMode >> m_iSelectMap >> m_iModeType >> m_bTimeClose >> m_bRandomTeamMode >> m_bStartRoomEnterX
			 >> m_bUseExtraOption >> m_bNoChallenger;

			 /*
			 >> m_bUseExtraOption >> m_iChangeCharType
			 >> m_iCoolTimeType >> m_iRedHPType >> m_iBlueHPType
			 >> m_iDropDamageType >> m_iGravityType
			 >> m_iPreSetModeType >> m_iTeamAttackType >> m_iGetUpType
			 >> m_iKOType >> m_iRedBlowType >> m_iBlueBlowType
			 >> m_iRedMoveSpeedType >> m_iBlueMoveSpeedType >> m_iKOEffectType
			 >> m_iRedEquipType >> m_iBlueEquipType;
			 */

	m_iMapLimitPlayer = g_BattleRoomManager.GetBattleMapToLimitPlayer( m_iSelectMode, m_iSelectMap );
	m_iMapLimitGrade  = g_BattleRoomManager.GetBattleMapToLimitGrade( m_iSelectMode, m_iSelectMap );
	m_iMapStartCoinCnt  = g_BattleRoomManager.GetBattleMapToStartCoin( m_iSelectMode, m_iSelectMap );
}

void BattleRoomCopyNode::ApplySyncChangeInfo( SP2Packet &rkPacket )
{	
	rkPacket >> m_iSelectModeTerm >> m_iSelectMode >> m_iSelectMap >> m_iModeType >> m_bTimeClose >> m_bRandomTeamMode >> m_bStartRoomEnterX
			 >> m_bUseExtraOption >> m_bNoChallenger;
			 /*
			 >> m_bUseExtraOption >> m_iChangeCharType
			 >> m_iCoolTimeType >> m_iRedHPType >> m_iBlueHPType
			 >> m_iDropDamageType >> m_iGravityType
			 >> m_iPreSetModeType >> m_iTeamAttackType >> m_iGetUpType
			 >> m_iKOType >> m_iRedBlowType >> m_iBlueBlowType
			 >> m_iRedMoveSpeedType >> m_iBlueMoveSpeedType >> m_iKOEffectType
			 >> m_iRedEquipType >> m_iBlueEquipType;
			 */
	rkPacket >> m_szRoomName >> m_szRoomPW >> m_OwnerUserID >> m_iCurJoiner >> m_iCurPlayer;
	rkPacket >> m_iMaxPlayerBlue >> m_iMaxPlayerRed >> m_iMaxObserver >> m_iAbilityMatchLevel >> m_iRoomLevel >> m_iBattleEventType;

	m_iMapLimitPlayer = g_BattleRoomManager.GetBattleMapToLimitPlayer( m_iSelectMode, m_iSelectMap );
	m_iMapLimitGrade  = g_BattleRoomManager.GetBattleMapToLimitGrade( m_iSelectMode, m_iSelectMap );
	m_iMapStartCoinCnt  = g_BattleRoomManager.GetBattleMapToStartCoin( m_iSelectMode, m_iSelectMap );
}

void BattleRoomCopyNode::ApplySyncCreate( SP2Packet &rkPacket )
{
	rkPacket >> m_dwTournamentIndex >> m_iSelectModeTerm >> m_iSelectMode >> m_iSelectMap >> m_iModeType >> m_bTimeClose >> m_bRandomTeamMode >> m_bStartRoomEnterX
			 >> m_bUseExtraOption >> m_bNoChallenger;
			 /*
			 >> m_bUseExtraOption >> m_iChangeCharType
			 >> m_iCoolTimeType >> m_iRedHPType >> m_iBlueHPType
			 >> m_iDropDamageType >> m_iGravityType
			 >> m_iPreSetModeType >> m_iTeamAttackType >> m_iGetUpType
			 >> m_iKOType >> m_iRedBlowType >> m_iBlueBlowType
			 >> m_iRedMoveSpeedType >> m_iBlueMoveSpeedType >> m_iKOEffectType
			 >> m_iRedEquipType >> m_iBlueEquipType;
			 */

	rkPacket >> m_szRoomName >> m_szRoomPW >> m_OwnerUserID >> m_iCurJoiner >> m_iCurPlayer;
	rkPacket >> m_iMaxPlayerBlue >> m_iMaxPlayerRed >> m_iMaxObserver >> m_iAbilityMatchLevel >> m_iRoomLevel >> m_iBattleEventType;

	m_iMapLimitPlayer = g_BattleRoomManager.GetBattleMapToLimitPlayer( m_iSelectMode, m_iSelectMap );
	m_iMapLimitGrade  = g_BattleRoomManager.GetBattleMapToLimitGrade( m_iSelectMode, m_iSelectMap );
	m_iMapStartCoinCnt  = g_BattleRoomManager.GetBattleMapToStartCoin( m_iSelectMode, m_iSelectMap );
}

void BattleRoomCopyNode::InitData()
{
	// 기본 정보
	m_dwIndex = 0;
	m_dwTournamentIndex = 0;
	m_szRoomName.Clear();
	m_szRoomPW.Clear();
	m_OwnerUserID.Clear();
	m_iCurPlayer = 0;
	m_iCurJoiner = 0;
	m_iMaxPlayerBlue = 0;
	m_iMaxPlayerRed = 0;
	m_iSelectMode = 0;
	m_iSelectMap = 0;
	m_iAbilityMatchLevel = 0;
	m_iRoomLevel = 0;
	m_iModeType  = MT_NONE;
	m_iSelectModeTerm = 0;
	m_bTimeClose = false;
	m_bRandomTeamMode  = true;
	m_bStartRoomEnterX = false;
	
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
	m_iRedEquipType = 0;
	m_iBlueEquipType = 0;
	m_iKOType = 0;
	m_iKOEffectType = 0;
	m_iRedBlowType = 0;
	m_iBlueBlowType = 0;

	m_iPreSetModeType = 0;

	m_iCatchModeRoundType = -1;
	m_iCatchModeRoundTimeType = -1;

	m_iGrowthUseType = 0;
	m_iExtraItemUseType = 0;

	m_iBattleEventType = BET_NORMAL;
	m_iMapLimitPlayer  = 0;
	m_iMapLimitGrade   = 0;
	m_iMapStartCoinCnt  = 0;
}

void BattleRoomCopyNode::EnterUser( const DWORD dwUserIndex, const ioHashString &szPublicID, const int iGradeLevel, const int iAbilityLevel, const bool bSafetyLevel, const bool bObserver,
								    const ioHashString &szPublicIP, const ioHashString &szPrivateIP, const ioHashString &szTransferIP, const int iClientPort, const int iTransferPort  )
{
	SP2Packet kPacket( SSTPK_BATTLEROOM_TRANSFER );
	kPacket << BattleRoomParent::ENTER_USER << GetIndex();
	kPacket << dwUserIndex << szPublicID << iGradeLevel << iAbilityLevel << bSafetyLevel << bObserver
		    << szPublicIP << szPrivateIP << szTransferIP << iClientPort << iTransferPort;
	BattleRoomCopyNode::SendMessage( kPacket );
}

bool BattleRoomCopyNode::LeaveUser( const DWORD dwUserIndex, const ioHashString &szPublicID )
{	
	SP2Packet kPacket( SSTPK_BATTLEROOM_TRANSFER );
	kPacket << BattleRoomParent::LEAVE_USER << GetIndex();
	kPacket << dwUserIndex << szPublicID;
	BattleRoomCopyNode::SendMessage( kPacket );
	return true;
}

void BattleRoomCopyNode::SendPacketTcp( SP2Packet &rkPacket, DWORD dwUserIndex )
{
	SP2Packet kPacket( SSTPK_BATTLEROOM_TRANSFER );
	kPacket << BattleRoomParent::TRANSFER_PACKET << GetIndex() << dwUserIndex << rkPacket.GetPacketID();
	kPacket.SetDataAdd( (char*)rkPacket.GetData(), rkPacket.GetDataSize() );
	SendMessage( kPacket );
}

void BattleRoomCopyNode::SendPacketTcpUser( SP2Packet &rkPacket, const ioHashString &rkSenderName )
{
	SP2Packet kPacket( SSTPK_BATTLEROOM_TRANSFER );
	kPacket << BattleRoomParent::TRANSFER_PACKET_USER << GetIndex() << rkSenderName << rkPacket.GetPacketID();
	kPacket.SetDataAdd( (char*)rkPacket.GetData(), rkPacket.GetDataSize() );
	SendMessage( kPacket );
}

void BattleRoomCopyNode::SendPacketUdp( SP2Packet &rkPacket, DWORD dwUserIndex )
{
	SP2Packet kPacket( SSTPK_BATTLEROOM_TRANSFER );
	kPacket << BattleRoomParent::TRANSFER_PACKET_UDP << GetIndex() << dwUserIndex << rkPacket.GetPacketID();
	kPacket.SetDataAdd( (char*)rkPacket.GetData(), rkPacket.GetDataSize() );
	SendMessage( kPacket );
}

void BattleRoomCopyNode::UserInfoUpdate( const DWORD dwUserIndex, const int iGradeLevel, const int iAbilityLevel, const bool bSafetyLevel, const int iClientPort, const ioHashString &szTransferIP, const int iTransferPort  )
{
	SP2Packet kPacket( SSTPK_BATTLEROOM_TRANSFER );
	kPacket << BattleRoomParent::USER_INFO << GetIndex();
	kPacket << dwUserIndex << iGradeLevel << iAbilityLevel << bSafetyLevel << iClientPort << szTransferIP << iTransferPort;
	BattleRoomCopyNode::SendMessage( kPacket );
}

void BattleRoomCopyNode::UserUDPChange( const DWORD dwUserIndex, const ioHashString &szPublicID, const ioHashString &szPublicIP, const int iClientPort, 
									    const ioHashString &szPrivateIP, const ioHashString &szTransferIP, const int iTransferPort  )
{
	SP2Packet kPacket( SSTPK_BATTLEROOM_TRANSFER );
	kPacket << BattleRoomParent::UDP_CHANGE << GetIndex();
	kPacket << dwUserIndex << szPublicID << szPublicIP << iClientPort << szPrivateIP << szTransferIP << iTransferPort;
	BattleRoomCopyNode::SendMessage( kPacket );
}

void BattleRoomCopyNode::UserP2PRelayInfo( const DWORD dwUserIndex, const DWORD dwRelayUserIndex, bool bRelay )
{
	SP2Packet kPacket( SSTPK_BATTLEROOM_TRANSFER );
	kPacket << BattleRoomParent::P2P_RELAY_INFO << GetIndex();
	kPacket << dwUserIndex << dwRelayUserIndex << bRelay;
	BattleRoomCopyNode::SendMessage( kPacket );
}

void BattleRoomCopyNode::TournamentInfo( const DWORD dwUserIndex, const DWORD dwTeamIndex )
{
	SP2Packet kPacket( SSTPK_BATTLEROOM_TRANSFER );
	kPacket << BattleRoomParent::TOURNAMENT_INFO << GetIndex();
	kPacket << dwUserIndex << dwTeamIndex;
	BattleRoomCopyNode::SendMessage( kPacket );
}

void BattleRoomCopyNode::OnBattleRoomInfo( UserParent *pUser, int iPrevBattleIndex )
{
	if( pUser == NULL )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "BattleRoomCopyNode::OnBattleRoomInfo Null User Pointer!!(%d)", GetIndex() );
		return;
	}
	SP2Packet kPacket( SSTPK_BATTLEROOM_TRANSFER );
	kPacket << BattleRoomParent::ROOM_INFO << GetIndex() << pUser->GetUserIndex() << iPrevBattleIndex;
	SendMessage( kPacket );
}

bool BattleRoomCopyNode::OnProcessPacket( SP2Packet &rkPacket, UserParent *pUser )
{
	if( pUser == NULL )
		return false;

	switch( rkPacket.GetPacketID() )
	{
	case CTPK_MACRO_COMMAND:
	case CTPK_VOICE_INFO:
	case CTPK_BATTLEROOM_INVITE:
	case CTPK_BATTLEROOM_COMMAND:
		{
			SP2Packet kPacket( SSTPK_BATTLEROOM_TRANSFER );
			kPacket << BattleRoomParent::USER_PACKET_TRANSFER << GetIndex() << pUser->GetUserIndex() << rkPacket.GetPacketID();
			kPacket.SetDataAdd( (char*)rkPacket.GetData(), rkPacket.GetDataSize() );
			SendMessage( kPacket );
		}
		return true;
	}
	return false;
}

bool BattleRoomCopyNode::IsBattleTimeClose()
{
	return m_bTimeClose;
}

bool BattleRoomCopyNode::IsStartRoomEnterX()
{
	if( !m_bStartRoomEnterX ) return false;
	if( MT_NONE == (ModeType)m_iModeType ) return false;

	return true;
}

bool BattleRoomCopyNode::IsUseExtraOption()
{
	if( !m_bUseExtraOption ) return false;

	return true;
}

bool BattleRoomCopyNode::IsNoChallenger()
{
	if( GetPlayModeType() == MT_FIGHT_CLUB )
		return m_bNoChallenger;

	return false;
}

bool BattleRoomCopyNode::IsHidden(int iIndex)
{
	if( IsBlocked())									return true;
	if( GetIndex() == iIndex )							return true;
	if( GetBattleEventType() == BET_BROADCAST_MBC )		return true;
	if( GetBattleEventType() == BET_TOURNAMENT_BATTLE ) return true;
	return false;
}

int BattleRoomCopyNode::GetChangeCharType()
{
	if( !m_bUseExtraOption ) return 0;

	return m_iChangeCharType;
}

int BattleRoomCopyNode::GetTeamAttackType()
{
	if( !m_bUseExtraOption ) return 0;

	return m_iTeamAttackType;
}

int BattleRoomCopyNode::GetCoolTimeType()
{
	if( !m_bUseExtraOption ) return 0;

	return m_iCoolTimeType;
}

int BattleRoomCopyNode::GetRedHPType()
{
	if( !m_bUseExtraOption ) return 0;

	return m_iRedHPType;
}

int BattleRoomCopyNode::GetBlueHPType()
{
	if( !m_bUseExtraOption ) return 0;

	return m_iBlueHPType;
}

int BattleRoomCopyNode::GetDropDamageType()
{
	if( !m_bUseExtraOption ) return 0;

	return m_iDropDamageType;
}

int BattleRoomCopyNode::GetGravityType()
{
	if( !m_bUseExtraOption ) return 0;

	return m_iGravityType;
}

int BattleRoomCopyNode::GetGetUpType()
{
	if( !m_bUseExtraOption ) return 0;

	return m_iGetUpType;
}

int BattleRoomCopyNode::GetRedMoveSpeedType()
{
	if( !m_bUseExtraOption ) return 0;

	return m_iRedMoveSpeedType;
}

int BattleRoomCopyNode::GetBlueMoveSpeedType()
{
	if( !m_bUseExtraOption ) return 0;

	return m_iBlueMoveSpeedType;
}

int BattleRoomCopyNode::GetRedEquipType()
{
	if( !m_bUseExtraOption ) return 0;

	return m_iRedEquipType;
}

int BattleRoomCopyNode::GetBlueEquipType()
{
	if( !m_bUseExtraOption ) return 0;

	return m_iBlueEquipType;
}

int BattleRoomCopyNode::GetKOType()
{
	if( !m_bUseExtraOption ) return 0;

	return m_iKOType;
}

int BattleRoomCopyNode::GetKOEffectType()
{
	if( !m_bUseExtraOption ) return 0;

	return m_iKOEffectType;
}

int BattleRoomCopyNode::GetRedBlowType()
{
	if( !m_bUseExtraOption ) return 0;

	return m_iRedBlowType;
}

int BattleRoomCopyNode::GetBlueBlowType()
{
	if( !m_bUseExtraOption ) return 0;

	return m_iBlueBlowType;
}

int BattleRoomCopyNode::GetPreSetModeType()
{
	if( !m_bUseExtraOption ) return 0;

	return m_iPreSetModeType;
}

int BattleRoomCopyNode::GetCatchModeRoundType()
{
	if( !m_bUseExtraOption ) return -1;

	return m_iCatchModeRoundType;
}

int BattleRoomCopyNode::GetCatchModeRoundTimeType()
{
	if( !m_bUseExtraOption ) return -1;

	return m_iCatchModeRoundTimeType;
}

int BattleRoomCopyNode::GetGrowthUseType()
{
	if( !m_bUseExtraOption ) return 0;

	return m_iGrowthUseType;
}

int BattleRoomCopyNode::GetExtraItemUseType()
{
	if( !m_bUseExtraOption ) return 0;

	return m_iExtraItemUseType;
}

bool BattleRoomCopyNode::IsEmptyBattleRoom()
{
	if( m_iCurJoiner == 0 )
		return true;
	return false;
}

bool BattleRoomCopyNode::IsBattleModePlaying()
{
	if( GetPlayModeType() != MT_NONE )
		return true;
	return false;
}

int  BattleRoomCopyNode::GetPlayModeType()
{
	return m_iModeType;
}

int  BattleRoomCopyNode::GetSelectModeTerm()
{
	return m_iSelectModeTerm;
}

int  BattleRoomCopyNode::GetSortLevelPoint( int iMyLevel )
{
	return abs( GetAbilityMatchLevel() - iMyLevel );
}

int BattleRoomCopyNode::GetBattleEventType()
{
	return m_iBattleEventType;
}

bool BattleRoomCopyNode::IsLevelMatchIgnore()
{
	if( GetBattleEventType() == BET_BROADCAST_AFRICA ||
		GetBattleEventType() == BET_BROADCAST_MBC ||
		GetBattleEventType() == BET_TOURNAMENT_BATTLE )
		return true;
	return false;
}

void BattleRoomCopyNode::SyncPlayEnd( bool bAutoStart )
{
	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "BattleRoomCopyNode::SyncPlayEnd 복사 전투룸이 동기화 하려함" );
}

void BattleRoomCopyNode::SetMaxPlayer( int iBluePlayer, int iRedPlayer, int iObserver )
{
	m_iMaxPlayerBlue = iBluePlayer;
	m_iMaxPlayerRed  = iRedPlayer;
	m_iMaxObserver = iObserver;
}

bool BattleRoomCopyNode::IsMapLimitPlayerFull()
{
	int iPlayCnt = GetPlayUserCnt();
	if( iPlayCnt >= m_iMapLimitPlayer )
		return true;

	return false;
}

bool BattleRoomCopyNode::IsMapLimitGrade( int iGradeLevel )
{
	if( iGradeLevel < m_iMapLimitGrade )
		return true;
	return false;
}

bool BattleRoomCopyNode::lsMapStartCoin( int iCoinCnt)
{
	if(iCoinCnt >= m_iMapStartCoinCnt)
	{
		return true;
	}
	return false;
}

int BattleRoomCopyNode::GetMapStartCoin()
{
	return m_iMapStartCoinCnt;
}

bool BattleRoomCopyNode::IsObserverFull()
{
	int iObserverCnt = 0;
	if( m_iCurJoiner > m_iCurPlayer )
		iObserverCnt = m_iCurJoiner - m_iCurPlayer;

	if( iObserverCnt >= m_iMaxObserver )
		return true;

	return false;
}
