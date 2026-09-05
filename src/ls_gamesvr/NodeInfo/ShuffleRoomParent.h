#ifndef _ShuffleRoomParent_h_
#define _ShuffleRoomParent_h_

#include "BlockNode.h"

class UserParent;
class SP2Packet;
class ShuffleRoomParent : public BlockNode
{
protected:
	DWORD m_dwCreateTime;
	int   m_iCurShufflePhase;
	bool  m_bPhaseEnd;

public:    
	enum
	{
		ENTER_USER = 100,
		LEAVE_USER,
		ROOM_INFO,
		USER_INFO,
		KICK_OUT,
		UDP_CHANGE,
		TRANSFER_PACKET,
		TRANSFER_PACKET_USER,
		USER_PACKET_TRANSFER,
		P2P_RELAY_INFO,
		TRANSFER_PACKET_UDP,
		REJOIN_TRY,
	};

	//가상함수의 향연
	public:
	virtual const DWORD GetIndex() = 0;
	virtual void EnterUser( const DWORD dwUserIndex, const ioHashString &szPublicID, const int iGradeLevel, const int iAbilityLevel,
							const ioHashString &szPublicIP, const ioHashString &szPrivateIP, const ioHashString &szTransferIP, const int iClientPort, const int iTransferPort ) = 0;
	virtual bool LeaveUser( const DWORD dwUserIndex, const ioHashString &szPublicID ) = 0;
	virtual bool ReJoinTry( const DWORD dwUserIndex, const ioHashString &szPublicID ) = 0;

	virtual void SendPacketTcp( SP2Packet &rkPacket, const DWORD dwUserIndex = 0 ) = 0;
	virtual void SendPacketTcpUser( SP2Packet &rkPacket, const ioHashString &rkSenderName ) = 0;
	virtual void SendPacketUdp( SP2Packet &rkPacket, const DWORD dwUserIndex = 0 ) = 0;
	virtual void UserInfoUpdate( const DWORD dwUserIndex, const int iGradeLevel, const int iAbilityLevel, const int iClientPort, const ioHashString &szTransferIP, const int iTransferPort ) = 0;
	virtual void UserUDPChange( const DWORD dwUserIndex, const ioHashString &szPublicID, const ioHashString &szPublicIP, const int iClientPort,
								const ioHashString &szPrivateIP, const ioHashString &szTransferIP, const int iTransferPort ) = 0;
	virtual void UserP2PRelayInfo( const DWORD dwUserIndex, const DWORD dwRelayUserIndex, bool bRelay ) = 0;
	virtual const bool IsOriginal() = 0;
	
    virtual int	 GetJoinUserCnt() const = 0;
	virtual int  GetPlayUserCnt() = 0;
	virtual int  GetMaxPlayer() const = 0;
	virtual int  GetMaxPlayerBlue() const = 0;
	virtual int  GetMaxPlayerRed() const = 0;
	virtual ioHashString GetName() const = 0;
	virtual ioHashString GetOwnerName() const = 0;
	virtual bool IsShuffleModePlaying() = 0;
	virtual bool IsFull() = 0;
	virtual bool IsEmptyShuffleRoom() = 0;
	virtual int  GetPlayModeType() = 0;
	virtual int  GetAbilityMatchLevel() = 0;
	virtual int  GetRoomLevel() = 0;
	virtual int  GetSortLevelPoint( int iMyLevel ) = 0;

public:
	virtual void SyncPlayEnd( bool bAutoStart ) = 0;

public:        //패킷 처리
	virtual bool OnProcessPacket( SP2Packet &rkPacket, UserParent *pUser ) = 0;

public:
	inline int  GetCreateTime() { return m_dwCreateTime; }
	inline int GetPhase() { return m_iCurShufflePhase; }
	inline bool IsBonusPhase() { return m_bPhaseEnd; }

	public:
	ShuffleRoomParent();
	virtual ~ShuffleRoomParent();
};
typedef std::vector< ShuffleRoomParent * > vShuffleRoomParent;
typedef vShuffleRoomParent::iterator vShuffleRoomParent_iter;
#endif