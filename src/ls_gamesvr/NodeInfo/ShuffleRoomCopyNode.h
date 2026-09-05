#ifndef _ShuffleRoomCopyNode_h_
#define _ShuffleRoomCopyNode_h_

#include "ShuffleRoomParent.h"
#include "CopyNodeParent.h"

class ShuffleRoomCopyNode : public ShuffleRoomParent, public CopyNodeParent					 
{
protected:
	// 기본 정보
	DWORD        m_dwIndex;

	ioHashString m_szRoomName;
	ioHashString m_OwnerUserID;
	int          m_iCurPlayer;
	int          m_iCurJoiner;
	int          m_iMaxPlayerBlue;
	int			 m_iMaxPlayerRed;
	int          m_iAbilityMatchLevel;
	int          m_iRoomLevel;
	int          m_iModeType;
	
	//
	int          m_iMapLimitPlayer;
	int          m_iMapLimitGrade;

protected:
	void InitData();

public:
	void ApplySyncPlay( SP2Packet &rkPacket );
	void ApplySyncChangeInfo( SP2Packet &rkPacket );
	void ApplySyncCreate( SP2Packet &rkPacket );

public:
	virtual void OnCreate( ServerNode *pCreator );
	virtual void OnDestroy();

public:
	virtual const DWORD GetIndex(){ return m_dwIndex; }
	virtual void EnterUser( const DWORD dwUserIndex, const ioHashString &szPublicID, const int iGradeLevel, const int iAbilityLevel,
							const ioHashString &szPublicIP, const ioHashString &szPrivateIP, const ioHashString &szTransferIP, const int iClientPort, const int iTransferPort );
	virtual bool LeaveUser( const DWORD dwUserIndex, const ioHashString &szPublicID );
	virtual bool ReJoinTry( const DWORD dwUserIndex, const ioHashString &szPublicID );
	virtual void SendPacketTcp( SP2Packet &rkPacket, const DWORD dwUserIndex = 0 );
	virtual void SendPacketTcpUser( SP2Packet &rkPacket, const ioHashString &rkSenderName );
	virtual void SendPacketUdp( SP2Packet &rkPacket, const DWORD dwUserIndex = 0 );
	virtual void UserInfoUpdate( const DWORD dwUserIndex, const int iGradeLevel, const int iAbilityLevel, const int iClientPort, const ioHashString &szTransferIP, const int iTransferPort );
	virtual void UserUDPChange( const DWORD dwUserIndex, const ioHashString &szPublicID, const ioHashString &szPublicIP, const int iClientPort,
						     	const ioHashString &szPrivateIP, const ioHashString &szTransferIP, const int iTransferPort );	
	virtual void UserP2PRelayInfo( const DWORD dwUserIndex, const DWORD dwRelayUserIndex, bool bRelay );
	virtual const bool IsOriginal(){ return false; }

	virtual int	 GetJoinUserCnt() const { return m_iCurJoiner; }
	virtual int	 GetPlayUserCnt() { return m_iCurPlayer; }
	virtual int  GetMaxPlayer() const { return m_iMaxPlayerBlue + m_iMaxPlayerRed; }
	virtual int  GetMaxPlayerBlue() const { return m_iMaxPlayerBlue; }
	virtual int  GetMaxPlayerRed() const { return m_iMaxPlayerRed; }
	virtual ioHashString GetName() const { return m_szRoomName; }
	virtual ioHashString GetOwnerName() const { return m_OwnerUserID; }
	virtual bool IsFull() { return ( m_iCurPlayer >= GetMaxPlayer() ); }
	virtual bool IsEmptyShuffleRoom();
	virtual int  GetAbilityMatchLevel(){ return m_iAbilityMatchLevel; }
	virtual int  GetRoomLevel(){ return m_iRoomLevel; }
	virtual bool IsShuffleModePlaying();
	virtual int  GetPlayModeType();
	virtual int  GetSortLevelPoint( int iMyLevel );

public:
	virtual void SyncPlayEnd( bool bAutoStart );

public:
	void SetIndex( DWORD dwIndex ){ m_dwIndex = dwIndex; }
	void SetJoinUserCnt( int iJoinUserCnt ){ m_iCurJoiner = iJoinUserCnt; }
	void SetPlayUserCnt( int iPlayUserCnt ){ m_iCurPlayer = iPlayUserCnt; }
	void SetMaxPlayer( int iBluePlayer, int iRedPlayer );

public:        //패킷 처리
	virtual bool OnProcessPacket( SP2Packet &rkPacket, UserParent *pUser );

public:
	ShuffleRoomCopyNode();
	virtual ~ShuffleRoomCopyNode();
};

typedef std::vector< ShuffleRoomCopyNode * > vShuffleRoomCopyNode;
typedef vShuffleRoomCopyNode::iterator vShuffleRoomCopyNode_iter;
#endif
