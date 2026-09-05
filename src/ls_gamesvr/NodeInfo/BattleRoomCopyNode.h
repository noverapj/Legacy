#ifndef _BattleRoomCopyNode_h_
#define _BattleRoomCopyNode_h_

#include "BattleRoomParent.h"
#include "CopyNodeParent.h"

class BattleRoomCopyNode : public BattleRoomParent,
						   public CopyNodeParent					 
{
protected:
	// 기본 정보
	DWORD        m_dwIndex;
	DWORD        m_dwTournamentIndex;

	ioHashString m_szRoomName;
	ioHashString m_szRoomPW; 
	ioHashString m_OwnerUserID;
	int          m_iCurPlayer;
	int          m_iCurJoiner;
	int          m_iMaxPlayerBlue;
	int			 m_iMaxPlayerRed;
	int			 m_iMaxObserver;
	int   	     m_iSelectMode;			//클라이언트에서 선택한 인덱스 값
	int          m_iSelectMap;
	int          m_iAbilityMatchLevel;
	int          m_iRoomLevel;
	int          m_iModeType;
	int          m_iSelectModeTerm;
	bool         m_bTimeClose;
	bool         m_bRandomTeamMode;
	bool         m_bStartRoomEnterX;
	
	int          m_iBattleEventType;

	//
	int          m_iMapLimitPlayer;
	int          m_iMapLimitGrade;
	int          m_iMapStartCoinCnt;

	bool		 m_bUseExtraOption;
	bool		 m_bNoChallenger;

	int m_iTeamAttackType;
	int m_iChangeCharType;
	int m_iCoolTimeType;
	int m_iRedHPType;
	int m_iBlueHPType;
	int m_iDropDamageType;
	int m_iGravityType;
	int m_iGetUpType;
	int m_iKOType;
	int m_iKOEffectType;
	int m_iRedBlowType;
	int m_iBlueBlowType;
	int m_iRedMoveSpeedType;
	int m_iBlueMoveSpeedType;
	int m_iRedEquipType;
	int m_iBlueEquipType;

	int m_iPreSetModeType;

	int m_iCatchModeRoundType;
	int m_iCatchModeRoundTimeType;

	int m_iGrowthUseType;
	int m_iExtraItemUseType;


protected:
	void InitData();

public:
	void ApplySyncSelectMode( SP2Packet &rkPacket );
	void ApplySyncPlay( SP2Packet &rkPacket );
	void ApplySyncChangeInfo( SP2Packet &rkPacket );
	void ApplySyncCreate( SP2Packet &rkPacket );

public:
	virtual void OnCreate( ServerNode *pCreator );
	virtual void OnDestroy();

public:
	virtual const DWORD GetIndex(){ return m_dwIndex; }
	virtual void EnterUser( const DWORD dwUserIndex, const ioHashString &szPublicID, const int iGradeLevel, const int iAbilityLevel, const bool bSafetyLevel, const bool bObserver,
							const ioHashString &szPublicIP, const ioHashString &szPrivateIP, const ioHashString &szTransferIP, const int iClientPort, const int iTransferPort );
	virtual bool LeaveUser( const DWORD dwUserIndex, const ioHashString &szPublicID );
	virtual void SendPacketTcp( SP2Packet &rkPacket, const DWORD dwUserIndex = 0 );
	virtual void SendPacketTcpUser( SP2Packet &rkPacket, const ioHashString &rkSenderName );
	virtual void SendPacketUdp( SP2Packet &rkPacket, const DWORD dwUserIndex = 0 );
	virtual void UserInfoUpdate( const DWORD dwUserIndex, const int iGradeLevel, const int iAbilityLevel, const bool bSafetyLevel, const int iClientPort, const ioHashString &szTransferIP, const int iTransferPort );
	virtual void UserUDPChange( const DWORD dwUserIndex, const ioHashString &szPublicID, const ioHashString &szPublicIP, const int iClientPort,
						     	const ioHashString &szPrivateIP, const ioHashString &szTransferIP, const int iTransferPort );	
	virtual void UserP2PRelayInfo( const DWORD dwUserIndex, const DWORD dwRelayUserIndex, bool bRelay );
	virtual void TournamentInfo( const DWORD dwUserIndex, const DWORD dwTeamIndex );
	virtual const bool IsOriginal(){ return false; }

	virtual int	 GetJoinUserCnt() const { return m_iCurJoiner; }
	virtual int	 GetPlayUserCnt() { return m_iCurPlayer; }
	virtual int  GetMaxPlayer() const { return m_iMaxPlayerBlue + m_iMaxPlayerRed; }
	virtual int  GetMaxPlayerBlue() const { return m_iMaxPlayerBlue; }
	virtual int  GetMaxPlayerRed() const { return m_iMaxPlayerRed; }
	virtual int  GetMaxObserver() const { return m_iMaxObserver; }
	virtual ioHashString GetName() const { return m_szRoomName; }
	virtual	ioHashString GetPW() const { return m_szRoomPW; }
	virtual ioHashString GetOwnerName() const { return m_OwnerUserID; }
	virtual bool IsFull() { return ( m_iCurPlayer >= GetMaxPlayer() ); }
	virtual bool IsMapLimitPlayerFull();
	virtual bool IsMapLimitGrade( int iGradeLevel );
	virtual bool lsMapStartCoin( int iCoinCnt);
	virtual int GetMapStartCoin();

	virtual bool IsObserverFull();
	virtual bool IsRandomTeamMode(){ return m_bRandomTeamMode; }
	virtual bool IsStartRoomEnterX();
	virtual bool IsEmptyBattleRoom();
	virtual bool IsPassword(){ return !m_szRoomPW.IsEmpty(); }
	virtual int  GetAbilityMatchLevel(){ return m_iAbilityMatchLevel; }
	virtual int  GetRoomLevel(){ return m_iRoomLevel; }
	virtual bool IsBattleTimeClose();
	virtual bool IsBattleModePlaying();
	virtual int  GetPlayModeType();
	virtual int  GetSelectModeTerm();
	virtual int  GetSortLevelPoint( int iMyLevel );
	virtual int  GetBattleEventType();
	virtual bool IsLevelMatchIgnore();
	virtual DWORD GetTournamentIndex(){ return m_dwTournamentIndex; }

	virtual bool IsNoChallenger();
	virtual bool IsHidden(int iIndex);

	// Custom Option
	virtual bool IsUseExtraOption();

	virtual int GetTeamAttackType();
	virtual int GetChangeCharType();
	virtual int GetCoolTimeType();
	virtual int GetRedHPType();
	virtual int GetBlueHPType();
	virtual int GetDropDamageType();
	virtual int GetGravityType();
	virtual int GetGetUpType();
	virtual int GetKOType();
	virtual int GetKOEffectType();
	virtual int GetRedBlowType();
	virtual int GetBlueBlowType();
	virtual int GetRedMoveSpeedType();
	virtual int GetBlueMoveSpeedType();
	virtual int GetRedEquipType();
	virtual int GetBlueEquipType();

	virtual int GetPreSetModeType();

	virtual int GetCatchModeRoundType();
	virtual int GetCatchModeRoundTimeType();

	virtual int GetGrowthUseType();
	virtual int GetExtraItemUseType();


public:
	virtual void SyncPlayEnd( bool bAutoStart );

public:
	void SetIndex( DWORD dwIndex ){ m_dwIndex = dwIndex; }
	void SetJoinUserCnt( int iJoinUserCnt ){ m_iCurJoiner = iJoinUserCnt; }
	void SetPlayUserCnt( int iPlayUserCnt ){ m_iCurPlayer = iPlayUserCnt; }
	void SetMaxPlayer( int iBluePlayer, int iRedPlayer, int iObserver );
	void SetTimeClose( bool bTimeClose ){ m_bTimeClose = bTimeClose; }
	void SetStartRoomEnterX( bool bEnterX ){ m_bStartRoomEnterX = bEnterX; }
	void SetNoChallenger( bool bNoChallenger ){ m_bNoChallenger = bNoChallenger; }

public:        //패킷 처리
	virtual void OnBattleRoomInfo( UserParent *pUser, int iPrevBattleIndex );
	virtual bool OnProcessPacket( SP2Packet &rkPacket, UserParent *pUser );

public:
	BattleRoomCopyNode();
	virtual ~BattleRoomCopyNode();
};

typedef std::vector< BattleRoomCopyNode * > vBattleRoomCopyNode;
typedef vBattleRoomCopyNode::iterator vBattleRoomCopyNode_iter;
#endif
