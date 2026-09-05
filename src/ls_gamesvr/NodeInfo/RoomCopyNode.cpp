#include "stdafx.h"

#include "../EtcHelpFunc.h"

#include "LevelMatchManager.h"
#include "RoomCopyNode.h"
#include "UserNodeManager.h"
#include "ServerNode.h"
#include "ioMyLevelMgr.h"

RoomCopyNode::RoomCopyNode()
{
	InitData();
}

RoomCopyNode::~RoomCopyNode()
{
}

void RoomCopyNode::OnCreate( ServerNode *pCreator )
{
	CopyNodeParent::OnCreate( pCreator );
	m_eCopyType = ROOM_TYPE;
	InitData();
}

void RoomCopyNode::OnDestroy()
{
	BlockNode::Reset();
	CopyNodeParent::OnDestroy();
}

void RoomCopyNode::InitData()
{
	// 기본 정보
	m_iRoomIndex = 0;
	m_iRoomStyle = RSTYLE_NONE;
	m_iRoomLevel = 0;
	m_iModeType  = MT_NONE;       
	m_iSubType   = 0;
	m_iMapNumber = 0;
	m_iMaxPlayer = 0;
	m_iCurPlayer = 0;
	m_iCurJoiner = 0;
	m_bSafetyLevelRoom = false;
	m_iTeamRatePoint   = 0;
	m_bTimeClose = false;
	m_ePlazaType = PT_NONE;

	// 추가 광장 정보
	m_iRoomNumber	= 0;
	m_iSubState		= 0;
	m_iMasterLevel	= 0;
	m_szRoomName.Clear();
	m_szMaster.Clear();
	m_szPassword.Clear();
}

bool RoomCopyNode::IsServerUserFull()
{
	if( m_pCreator == NULL ) return true;

	if( m_pCreator->GetUserNodeSize() > g_UserNodeManager.GetStableConnection() )
		return true;
	return false;
}

void RoomCopyNode::ApplySyncMode( SP2Packet &rkPacket )
{
	int iModeType, iSubType;
	rkPacket >> iModeType >> iSubType >> m_iMapNumber;
	m_iModeType = (ModeType)iModeType;
	m_iSubType = iSubType;
	m_bTimeClose = false;       //모드가 바뀌면 시간 초기화
}

void RoomCopyNode::ApplySyncCurUser( SP2Packet &rkPacket )
{
	int iModeType, iSubType;
	rkPacket >> iModeType >> iSubType >> m_iMapNumber >> m_iCurJoiner >> m_iCurPlayer >> m_iMaxPlayer >> m_iRoomLevel >> m_iTeamRatePoint;
	m_iModeType = (ModeType)iModeType;
	m_iSubType = iSubType;
}

void RoomCopyNode::ApplySyncPlazaInfo( SP2Packet &rkPacket )
{
	int iModeType, iSubType;
	rkPacket >> iModeType >> iSubType >> m_iMapNumber >> m_iCurJoiner >> m_iCurPlayer >> m_iMaxPlayer >> m_iRoomLevel >> m_iTeamRatePoint;
	rkPacket >> m_iRoomNumber >> m_iMasterLevel >> m_szRoomName >> m_szMaster >> m_szPassword >> m_iSubState;
	m_iModeType = (ModeType)iModeType;
	m_iSubType = iSubType;
}

void RoomCopyNode::ApplySyncCreate( SP2Packet &rkPacket )
{
	int iRoomStyle, iModeType, iSubType;
	rkPacket >> iRoomStyle >> m_bSafetyLevelRoom >> iModeType >> iSubType >> m_iMapNumber >> m_iCurJoiner >> m_iCurPlayer >> m_iMaxPlayer >> m_iRoomLevel >> m_iTeamRatePoint;
	m_iRoomStyle = (RoomStyle)iRoomStyle;
	m_iModeType  = (ModeType)iModeType;
	m_iSubType = iSubType;

	if( (ModeType)m_iModeType == MT_TRAINING )
	{
		int iPlazaType;
		rkPacket >> m_iRoomNumber >> m_iMasterLevel >> m_szRoomName >> m_szMaster >> m_szPassword >> iPlazaType;
		m_ePlazaType = (PlazaType)iPlazaType;
	}
}

int RoomCopyNode::GetRoomIndex()
{
	return m_iRoomIndex;
}

RoomStyle RoomCopyNode::GetRoomStyle()
{
	return m_iRoomStyle;
}

int RoomCopyNode::GetAverageLevel()
{
	if( m_bSafetyLevelRoom )
		return 0;

	return m_iRoomLevel;
}

int RoomCopyNode::GetGapLevel( int iMyLevel )
{
	return abs( GetAverageLevel() - iMyLevel );
}

ModeType RoomCopyNode::GetModeType()
{
	return m_iModeType;
}

int RoomCopyNode::GetModeSubNum()
{
	return m_iSubType;
}

int RoomCopyNode::GetModeMapNum()
{
	return m_iMapNumber;
}

int RoomCopyNode::GetMaxPlayer()
{
	return m_iMaxPlayer;
}

int RoomCopyNode::GetJoinUserCnt()
{
	return m_iCurJoiner;
}

int RoomCopyNode::GetPlayUserCnt()
{
	return m_iCurPlayer;
}

int RoomCopyNode::GetRoomNumber()
{
	return m_iRoomNumber;
}

int RoomCopyNode::GetMasterLevel()
{
	return m_iMasterLevel;
}

ioHashString RoomCopyNode::GetRoomName()
{
	return m_szRoomName;
}

ioHashString RoomCopyNode::GetMasterName()
{
	return m_szMaster;
}

bool RoomCopyNode::IsRoomMasterID()
{
	return (!m_szMaster.IsEmpty());
}

ioHashString RoomCopyNode::GetRoomPW()
{
	return m_szPassword;
}

bool RoomCopyNode::IsRoomPW()
{
	return (!m_szPassword.IsEmpty());
}

bool RoomCopyNode::IsRoomFull()
{
	if( GetPlayUserCnt() >= GetMaxPlayer() )
		return true;

	return false;
}

int RoomCopyNode::GetPlazaRoomLevel()
{
	int iMatchLevel = GetAverageLevel() - g_LevelMatchMgr.GetAddGradeLevel();
	return min( max( iMatchLevel, 0 ), g_LevelMgr.GetMaxGradeLevel() );
}

PlazaType RoomCopyNode::GetPlazaModeType() const
{ 
	if( m_iModeType != MT_TRAINING )
		return PT_NONE;
	return m_ePlazaType; 
}

void RoomCopyNode::OnPlazaRoomInfo( UserParent *pUser )
{
	if( pUser == NULL ) return;

	SP2Packet kPacket( SSTPK_PLAZAROOM_TRANSFER );
	kPacket << RoomParent::ROOM_INFO << GetRoomIndex() << pUser->GetUserIndex();
	SendMessage( kPacket );
}
