#include "stdafx.h"

#include "ShuffleRoomCopyNode.h"
#include "ShuffleRoomManager.h"
#include "ServerNode.h"

ShuffleRoomCopyNode::ShuffleRoomCopyNode()
{
	InitData();
}

ShuffleRoomCopyNode::~ShuffleRoomCopyNode()
{
}

void ShuffleRoomCopyNode::OnCreate( ServerNode *pCreator )
{
	CopyNodeParent::OnCreate( pCreator );
	m_eCopyType = SHUFFLEROOM_TYPE;
	InitData();
}

void ShuffleRoomCopyNode::OnDestroy()
{
	BlockNode::Reset();
	CopyNodeParent::OnDestroy();
}

void ShuffleRoomCopyNode::ApplySyncPlay( SP2Packet &rkPacket )
{
	rkPacket >> m_iModeType >> m_iCurShufflePhase >> m_bPhaseEnd;
}

void ShuffleRoomCopyNode::ApplySyncChangeInfo( SP2Packet &rkPacket )
{	
	rkPacket >> m_iModeType;

	rkPacket >> m_szRoomName >> m_OwnerUserID >> m_iCurJoiner >> m_iCurPlayer;
	rkPacket >> m_iMaxPlayerBlue >> m_iMaxPlayerRed >> m_iAbilityMatchLevel >> m_iRoomLevel;
}

void ShuffleRoomCopyNode::ApplySyncCreate( SP2Packet &rkPacket )
{
	rkPacket >> m_iModeType;

	rkPacket >> m_szRoomName >> m_OwnerUserID >> m_iCurJoiner >> m_iCurPlayer;
	rkPacket >> m_iMaxPlayerBlue >> m_iMaxPlayerRed >> m_iAbilityMatchLevel >> m_iRoomLevel >> m_dwCreateTime;
	rkPacket >> m_iCurShufflePhase >> m_bPhaseEnd;
}

void ShuffleRoomCopyNode::InitData()
{
	// 기본 정보
	m_dwIndex = 0;
	m_szRoomName.Clear();
	m_OwnerUserID.Clear();
	m_iCurPlayer = 0;
	m_iCurJoiner = 0;
	m_iMaxPlayerBlue = 0;
	m_iMaxPlayerRed = 0;
	m_iAbilityMatchLevel = 0;
	m_iRoomLevel = 0;
	m_iModeType  = MT_NONE;
}

void ShuffleRoomCopyNode::EnterUser( const DWORD dwUserIndex, const ioHashString &szPublicID, const int iGradeLevel, const int iAbilityLevel,
									 const ioHashString &szPublicIP, const ioHashString &szPrivateIP, const ioHashString &szTransferIP, const int iClientPort, const int iTransferPort )
{
	SP2Packet kPacket( SSTPK_SHUFFLEROOM_TRANSFER );
	kPacket << ShuffleRoomParent::ENTER_USER << GetIndex();
	kPacket << dwUserIndex << szPublicID << iGradeLevel << iAbilityLevel
			<< szPublicIP << szPrivateIP << szTransferIP << iClientPort << iTransferPort;
	ShuffleRoomCopyNode::SendMessage( kPacket );
}

bool ShuffleRoomCopyNode::LeaveUser( const DWORD dwUserIndex, const ioHashString &szPublicID )
{	
	SP2Packet kPacket( SSTPK_SHUFFLEROOM_TRANSFER );
	kPacket << ShuffleRoomParent::LEAVE_USER << GetIndex();
	kPacket << dwUserIndex << szPublicID;
	ShuffleRoomCopyNode::SendMessage( kPacket );
	return true;
}

bool ShuffleRoomCopyNode::ReJoinTry( const DWORD dwUserIndex, const ioHashString &szPublicID )
{
	SP2Packet kPacket( SSTPK_SHUFFLEROOM_TRANSFER );
	kPacket << ShuffleRoomParent::REJOIN_TRY << GetIndex();
	kPacket << dwUserIndex << szPublicID;
	ShuffleRoomCopyNode::SendMessage( kPacket );
	return true;
}

void ShuffleRoomCopyNode::SendPacketTcp( SP2Packet &rkPacket, DWORD dwUserIndex )
{
	SP2Packet kPacket( SSTPK_SHUFFLEROOM_TRANSFER );
	kPacket << ShuffleRoomParent::TRANSFER_PACKET << GetIndex() << dwUserIndex << rkPacket.GetPacketID();
	kPacket.SetDataAdd( (char*)rkPacket.GetData(), rkPacket.GetDataSize() );
	SendMessage( kPacket );
}

void ShuffleRoomCopyNode::SendPacketTcpUser( SP2Packet &rkPacket, const ioHashString &rkSenderName )
{
	SP2Packet kPacket( SSTPK_SHUFFLEROOM_TRANSFER );
	kPacket << ShuffleRoomParent::TRANSFER_PACKET_USER << GetIndex() << rkSenderName << rkPacket.GetPacketID();
	kPacket.SetDataAdd( (char*)rkPacket.GetData(), rkPacket.GetDataSize() );
	SendMessage( kPacket );
}

void ShuffleRoomCopyNode::SendPacketUdp( SP2Packet &rkPacket, DWORD dwUserIndex )
{
	SP2Packet kPacket( SSTPK_SHUFFLEROOM_TRANSFER );
	kPacket << ShuffleRoomParent::TRANSFER_PACKET_UDP << GetIndex() << dwUserIndex << rkPacket.GetPacketID();
	kPacket.SetDataAdd( (char*)rkPacket.GetData(), rkPacket.GetDataSize() );
	SendMessage( kPacket );
}

void ShuffleRoomCopyNode::UserInfoUpdate( const DWORD dwUserIndex, const int iGradeLevel, const int iAbilityLevel, const int iClientPort, const ioHashString &szTransferIP, const int iTransferPort  )
{
	SP2Packet kPacket( SSTPK_SHUFFLEROOM_TRANSFER );
	kPacket << ShuffleRoomParent::USER_INFO << GetIndex();
	kPacket << dwUserIndex << iGradeLevel << iAbilityLevel << iClientPort << szTransferIP << iTransferPort;
	ShuffleRoomCopyNode::SendMessage( kPacket );
}

void ShuffleRoomCopyNode::UserUDPChange( const DWORD dwUserIndex, const ioHashString &szPublicID, const ioHashString &szPublicIP, const int iClientPort, 
									    const ioHashString &szPrivateIP, const ioHashString &szTransferIP, const int iTransferPort  )
{
	SP2Packet kPacket( SSTPK_SHUFFLEROOM_TRANSFER );
	kPacket << ShuffleRoomParent::UDP_CHANGE << GetIndex();
	kPacket << dwUserIndex << szPublicID << szPublicIP << iClientPort << szPrivateIP << szTransferIP << iTransferPort;
	ShuffleRoomCopyNode::SendMessage( kPacket );
}

void ShuffleRoomCopyNode::UserP2PRelayInfo( const DWORD dwUserIndex, const DWORD dwRelayUserIndex, bool bRelay )
{
	SP2Packet kPacket( SSTPK_SHUFFLEROOM_TRANSFER );
	kPacket << ShuffleRoomParent::P2P_RELAY_INFO << GetIndex();
	kPacket << dwUserIndex << dwRelayUserIndex << bRelay;
	ShuffleRoomCopyNode::SendMessage( kPacket );
}

bool ShuffleRoomCopyNode::OnProcessPacket( SP2Packet &rkPacket, UserParent *pUser )
{
	if( pUser == NULL )
		return false;

	switch( rkPacket.GetPacketID() )
	{
	case CTPK_MACRO_COMMAND:
	case CTPK_VOICE_INFO:
	case CTPK_BATTLEROOM_COMMAND:
		{

#ifdef FLAG_MODE_BY_BCKIM_DEBUG		// 전투시작 임시 값 확인  ShuffleRoomCopyNode::OnProcessPacket
			LOG.PrintTimeAndLog(0,"[FLAG_MODE_BY_BCKIM:ShuffleRoomCopyNode::OnProcessPacket] A_NAME[%s]",pUser->GetPrivateID().c_str());
#endif // FLAG_MODE_BY_BCKIM_DEBUG

			SP2Packet kPacket( SSTPK_SHUFFLEROOM_TRANSFER );
			kPacket << ShuffleRoomParent::USER_PACKET_TRANSFER << GetIndex() << pUser->GetUserIndex() << rkPacket.GetPacketID();
			kPacket.SetDataAdd( (char*)rkPacket.GetData(), rkPacket.GetDataSize() );
			SendMessage( kPacket );
		}
		return true;
	}
	return false;
}

bool ShuffleRoomCopyNode::IsEmptyShuffleRoom()
{
	if( m_iCurJoiner == 0 )
		return true;
	return false;
}

bool ShuffleRoomCopyNode::IsShuffleModePlaying()
{
	if( GetPlayModeType() != MT_NONE )
		return true;
	return false;
}

int  ShuffleRoomCopyNode::GetPlayModeType()
{
	return m_iModeType;
}

int  ShuffleRoomCopyNode::GetSortLevelPoint( int iMyLevel )
{
	return abs( GetAbilityMatchLevel() - iMyLevel );
}

void ShuffleRoomCopyNode::SyncPlayEnd( bool bAutoStart )
{
	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ShuffleRoomCopyNode::SyncPlayEnd 복사 전투룸이 동기화 하려함" );
}

void ShuffleRoomCopyNode::SetMaxPlayer( int iBluePlayer, int iRedPlayer )
{
	m_iMaxPlayerBlue = iBluePlayer;
	m_iMaxPlayerRed  = iRedPlayer;
}