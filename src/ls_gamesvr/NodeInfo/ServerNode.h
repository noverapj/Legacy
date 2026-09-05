#ifndef _ServerNode_h_
#define _ServerNode_h_

#include "UserCopyNode.h"
#include "RoomCopyNode.h"
#include "ShuffleRoomCopyNode.h"
#include "BattleRoomCopyNode.h"
#include "ChannelCopyNode.h"
#include "LadderTeamCopyNode.h"
#include <unordered_set>

#define FRIEND_LIST_MOVE_COUNT             50
#define SERVER_PING_CHECK_TIME             10000
#define SERVER_ROOM_CREATE_CLOSE_TIME      15000

class User;
class CConnectNode;
class SP2Packet;

class ServerNode : public CConnectNode
{
	friend class ServerNodeManager;

public:
	enum NodeTypes
	{
		NODE_TYPE_NONE = 0,
		NODE_TYPE_GAME,
		NODE_TYPE_LOGIN,
		NODE_TYPE_RELAY,
		NODE_TYPE_END
	};
	enum NodeRoles
	{
		NODE_ROLE_NONE = 0,
		NODE_ROLE_SERVER = 1, //내가 서버이다
		NODE_ROLE_CLIENT = 2, //내가 클라이언트이다.
		NODE_ROLE_END
	};

	enum SessionState
	{
		SS_DISCONNECT	= 0,
		SS_CONNECT		= 1,
	};

protected:
	NodeTypes	 m_eNodeType;
	NodeRoles	 m_eNodeRole;
	SessionState m_eSessionState;

	bool		 m_bBlockState;
	DWORD        m_dwServerIndex;
	ioHashString m_szServerIP;     // 사설IP
	ioHashString m_szClientMoveIP; // 공인IP
	TCHAR        m_szPublicIP[STR_IP_MAX]; //전송용 
	int          m_iServerPort;
	int          m_iClientPort;
	int          m_iUserCount;
	int			 m_iRelayIndex;
	int			 m_iPartitionIndex;
 
	DWORD        m_dwPingTIme;
	DWORD        m_dwRecvPingTime;
	std::vector<int> m_relayPorts;
	SendRelayInfo_ m_RelayInfo;

	typedef std::vector< SP2Packet > SP2PacketList;
	SP2PacketList m_ConnectWorkingPacket;
	bool          m_bConnectWorkComplete;
 
protected:			  // User Copy Node
	uUserCopyNode m_UserCopyNode;
	MemPooler< UserCopyNode >	m_UserMemNode;

protected:
	void InitUserMemoryPool();
	void ReleaseUserMemoryPool();
	void ReturnUserMemoryPool();
	UserCopyNode *CreateNewUser( DWORD dwIndex );
	UserCopyNode *GetUserNode( DWORD dwIndex );
	//UserCopyNode *GetUserNode( const ioHashString &szUserID );
	void RemoveUserNode( DWORD dwIndex );
	void RemoveUserNode( const ioHashString &szUserID );
	//int  ExceptionRemoveUserNode( DWORD dwIndex );

protected:            // Room Copy Node
	vRoomCopyNode m_RoomCopyNode;
	MemPooler< RoomCopyNode >	m_RoomMemNode;
	
	void InitRoomMemoryPool();
	void ReleaseRoomMemoryPool();
	void ReturnRoomMemoryPool();
	RoomCopyNode *CreateNewRoom( int iIndex );
	RoomCopyNode *GetRoomNode( int iIndex );
	void RemoveRoomNode( int iIndex );
	void RemoveRoomNode( RoomCopyNode *pRoomNode );
	void RemoveRoomCopyNodeAll();
	
protected:           // BattleRoom Copy Node
	vBattleRoomCopyNode m_BattleRoomCopyNode;
	MemPooler< BattleRoomCopyNode >	m_BattleRoomMemNode;

	void InitBattleRoomMemoryPool();
	void ReleaseBattleRoomMemoryPool();
	void ReturnBattleRoomMemoryPool();
	BattleRoomCopyNode *CreateNewBattleRoom( DWORD dwIndex );
	BattleRoomCopyNode *GetBattleRoomNode( DWORD dwIndex );
	void RemoveBattleRoomNode( DWORD dwIndex );
	void RemoveBattleRoomNode( BattleRoomCopyNode *pBattleRoomNode );
	void RemoveBattleRoomCopyNodeAll();

protected:           // ShuffleRoom Copy Node
	vShuffleRoomCopyNode m_ShuffleRoomCopyNode;
	MemPooler< ShuffleRoomCopyNode >	m_ShuffleRoomMemNode;

	void InitShuffleRoomMemoryPool();
	void ReleaseShuffleRoomMemoryPool();
	void ReturnShuffleRoomMemoryPool();
	ShuffleRoomCopyNode *CreateNewShuffleRoom( DWORD dwIndex );
	ShuffleRoomCopyNode *GetShuffleRoomNode( DWORD dwIndex );
	void RemoveShuffleRoomNode( DWORD dwIndex );
	void RemoveShuffleRoomNode( ShuffleRoomCopyNode *pShuffleRoomNode );
	void RemoveShuffleRoomCopyNodeAll();

protected:			// Channel Copy Node
	vChannelCopyNode m_ChannelCopyNode;
	MemPooler< ChannelCopyNode >	m_ChannelMemNode;

	void InitChannelMemoryPool();
	void ReleaseChannelMemoryPool();
	void ReturnChannelMemoryPool();
	ChannelCopyNode *CreateNewChannel( DWORD dwIndex );
	ChannelCopyNode *GetChannelNode( DWORD dwIndex );
	void RemoveChannelNode( DWORD dwIndex );
	void RemoveChannelNode( ChannelCopyNode *pChannelNode );

protected:         // LadderTeam Copy Node
	vLadderTeamCopyNode m_LadderTeamCopyNode;
	MemPooler< LadderTeamCopyNode >	m_LadderTeamMemNode;

	void InitLadderTeamMemoryPool();
	void ReleaseLadderTeamMemoryPool();
	void ReturnLadderTeamMemoryPool();
	LadderTeamCopyNode *CreateNewLadderTeam( DWORD dwIndex );
	LadderTeamCopyNode *GetLadderTeamNode( DWORD dwIndex );
	void RemoveLadderTeamNode( DWORD dwIndex );
	void RemoveLadderTeamNode( LadderTeamCopyNode *pLadderTeamNode );

public:
	virtual void SessionClose( BOOL safely=TRUE );
	virtual bool SendMessage( CPacket &rkPacket );
	virtual void ReceivePacket( CPacket &packet );
	virtual void PacketParsing( CPacket &packet );

protected:
	void OnRelayControl( SP2Packet &kPacket );
	void OnUserGhost( SP2Packet &kPacket );
	void OnHackAnnounce( SP2Packet &kPacket );
	void OnChangeAddress( SP2Packet &kPacket );

	void RequestRelayServerConnect( SP2Packet &kPacket );
	void RequestRelayServerConnect(const char* publicID);

	void OnRelayServerConnect( SP2Packet &kPacket );

public:
	void SetSessionState(ServerNode::SessionState eState)	{ m_eSessionState = eState; }
	void SetBlockState(const bool bBlockState);

	bool IsDisconnectState()		{ return ( m_eSessionState == SS_DISCONNECT ); }
	bool IsConnectState()			{ return ( m_eSessionState == SS_CONNECT ); }
	
	bool IsBusy();
	bool IsConnectWorkComplete()	{ return m_bConnectWorkComplete; }
	bool IsBlocked()				{ return m_bBlockState; }
	bool IsRoomCreateClose();
	bool IsFull();
	bool IsMyPartition(const int iPartitionIndex);

public:
	virtual void OnCreate();       //초기화
	virtual void OnDestroy();
	void OnSessionDestroy();
	virtual bool CheckNS( CPacket &rkPacket );	
	virtual int  GetConnectType();

	int GetUserCount()			{ return m_iUserCount; }
	void SetUserCount(int val)	{ m_iUserCount = val; }

	int GetNodeType()			{ return m_eNodeType; }
	int GetNodeRole()			{ return m_eNodeRole; }

	bool IsGameNode()			{ return (GetNodeType() == NODE_TYPE_GAME) ? true : false; }
	bool IsLoginNode()			{ return (GetNodeType() == NODE_TYPE_LOGIN) ? true : false; }
	bool IsRelayNode()			{ return (GetNodeType() == NODE_TYPE_RELAY) ? true : false; }

protected:
	void InitData();

	// 유저 관련 패킷이 도착하는거와 동시에 유저가 서버를 이동해버리면 유저 패킷이 손실되므로 이동된 서버로 재전송한다.
	// ServerNode에서 사용하는 대부분의 Get은 아래 함수를 사용.
	User *GetUserOriginalNode( DWORD dwUserIndex, SP2Packet &rkPacket );
	User *GetUserOriginalNode( const ioHashString &szPublicID, SP2Packet &rkPacket );
	User *GetUserOriginalNodeByPrivateID( const ioHashString &szPrivateID, SP2Packet &rkPacket );
	 
public:
	void SetNodeType( ServerNode::NodeTypes eType );
	void SetNodeRole( ServerNode::NodeRoles eType );
	void SetServerIndex( DWORD dwServerIndex ){ m_dwServerIndex = dwServerIndex; }

public:
	void RequestJoin();
	void RequestSync();
	void AfterJoinProcess();

	void ProcessPing();

public:
	const DWORD &GetServerIndex()			{ return m_dwServerIndex; }
	const ioHashString &GetServerIP()		{ return m_szServerIP; }          // 사설IP
	const ioHashString &GetClientMoveIP()	{ return m_szClientMoveIP; }      // 공인IP
	const int	&GetServerPort()			{ return m_iServerPort; }
	const int   &GetClientPort()			{ return m_iClientPort; }
	const int   GetUserNodeSize()			{ return m_UserCopyNode.size(); }

public:
	int GetPlazaRoomCount();
	int GetHeadquartersRoomCount();
	int GetBattleRoomCount();
	int GetLadderTeamCount( bool bHeroMode );
	int GetLadderHeroModeCount();
	int GetShuffleRoomCount();

public:
	int RelayServerIndex() const				{ return m_iRelayIndex; }
	void SetRelayServerIndex(int iRelayIndex)	{ m_iRelayIndex = iRelayIndex; }
	SendRelayInfo_& RelayInfo()					{ return m_RelayInfo; }
	TCHAR* SZPublicIP()							{ return m_szPublicIP; }

	int GetRelayServerPort(int num);

	void SetPartitionIndex(const int iPartitionIndex)	{ m_iPartitionIndex = iPartitionIndex; }
	int GetPartitionIndex()								{ return m_iPartitionIndex; }

public:
	void OnClose( SP2Packet &rkPacket );
	void OnConnectInfo( SP2Packet &rkPacket );
	void OnConnectSync( SP2Packet &rkPacket );
	void OnPing( SP2Packet &rkPacket );
	void OnRoomSync( SP2Packet &rkPacket );
	void OnMovingRoom( SP2Packet &rkPacket );
	void _OnMovingRoomOK( int iMoveType, SP2Packet &rkPacket );
	void _OnMovingRoomError( int iMoveType, SP2Packet &rkPacket );
	void OnMovingRoomResult( SP2Packet &rkPacket );
	void OnUserDataMove( SP2Packet &rkPacket );
	void OnUserInventoryMoveResult( SP2Packet &rkPacket );
	void OnUserExtraItemMoveResult( SP2Packet &rkPacket );
	void OnUserQuestMoveResult( SP2Packet &rkPacket );
	void OnUserDataMoveResult( SP2Packet &rkPacket );
	void OnUserSync( SP2Packet &rkPacket );
	void _OnMoveUserNode( DWORD dwUserIndex, SP2Packet &rkPacket );
	void OnUserMove( SP2Packet &rkPacket );
	void OnUserFriendMove( SP2Packet &rkPacket );
	void OnUserFriendMoveResult( SP2Packet &rkPacket );
	void OnFollowUser( SP2Packet &rkPacket );
	void OnUserPosIndex( SP2Packet &rkPacket );
	void OnChannelSync( SP2Packet &rkPacket );
	void OnChannelInvite( SP2Packet &rkPacket );
	void OnChannelChat( SP2Packet &rkPacket );
	void OnBattleRoomSync( SP2Packet &rkPacket );
	void OnBattleRoomTransfer( SP2Packet &rkPacket );
	void OnBattleRoomJoinResult( SP2Packet &rkPacket );
	void OnBattleRoomFollow( SP2Packet &rkPacket );
	void OnPlazaRoomTransfer( SP2Packet &rkPacket );
	void OnUserInfoRefresh( SP2Packet &rkPacket );
	void OnSimpleUserInfoRefresh( SP2Packet &rkPacket );
	void OnUserCharInfoRefresh( SP2Packet &rkPacket );
	void OnUserCharSubInfoRefresh( SP2Packet &rkPacket );
	void OnBattleRoomKickOut( SP2Packet &rkPacket );
	void OnOfflineMemo( SP2Packet &rkPacket );
	void OnReserveCreateRoom( SP2Packet &rkPacket );
	void OnReserveCreateBattleRoom( SP2Packet &rkPacket );
	void OnReserveCreateBattleRoomResult( SP2Packet &rkPacket );
	void OnExceptionBattleRoomLeave( SP2Packet &rkPacket );
	void OnFriendDelete( SP2Packet &rkPacket );
	void OnWholeChat( SP2Packet &rkPacket );
	void OnRainbowWholeChat( SP2Packet &rkPacket );
	void OnUserCharRentalAgree( SP2Packet &rkPacket );
	void OnUserAlchemicMoveResult( SP2Packet &rkPacket );
	void OnUserPetMoveResult( SP2Packet &rkPacket );
	void OnUserCharAwakeMoveResult( SP2Packet &rkPacket );

	//GUILD
	void OnCreateGuildResult( SP2Packet &rkPacket );
	void OnCreateGuildComplete( SP2Packet &rkPacket );
	void OnGuildEntryAgree( SP2Packet &rkPacket );
	void OnGuildMemberListEx( SP2Packet &rkPacket );
	void OnGuildInvitation( SP2Packet &rkPacket );
	void OnGuildMasterChange( SP2Packet &rkPacket );
	void OnGuildPositionChange( SP2Packet &rkPacket );
	void OnGuildKickOut( SP2Packet &rkPacket );
	void OnGuildMarkChange( SP2Packet &rkPacket );
	void OnGuildUserDelete( SP2Packet &rkPacket );
	void OnLadderTeamSync( SP2Packet &rkPacket );
	void OnLadderTeamTransfer( SP2Packet &rkPacket );
	void OnExceptionLadderTeamLeave( SP2Packet &rkPacket );
	void OnReserveCreateLadderTeam( SP2Packet &rkPacket );
	void OnReserveCreateLadderTeamResult( SP2Packet &rkPacket );
	void OnLadderTeamJoinResult( SP2Packet &rkPacket );
	void OnLadderTeamKickOut( SP2Packet &rkPacket );
	void OnLadderTeamFollow( SP2Packet &rkPacket );
	void OnLadderTeamEnterRoomUser( SP2Packet &rkPacket );
	void OnCampSeasonBonus( SP2Packet &rkPacket );
	void OnGuildNameChange( SP2Packet &rkPacket );
	void OnGuildNameChangeResult( SP2Packet &rkPacket );
	void OnPresentSelect( SP2Packet &rkPacket );
	void OnSubscriptionSelect( SP2Packet &rkPacket );

	// Trade
	void OnTradeCreate( SP2Packet &rkPacket );
	void OnTradeCreateComplete( SP2Packet &rkPacket );
	void OnTradeCreateFail( SP2Packet &rkPacket );
	void OnTradeItemComplete( SP2Packet &rkPacket );
	void OnTradeCancel( SP2Packet &rkPacket );
	void OnTradeTimeOut( SP2Packet &rkPacket );

	void OnTradeGetInfoOK( User *pUser, SP2Packet &rkPacket );
	void OnTradeGetInfoFail( User *pUser, SP2Packet &rkPacket );
	void OnTradeBuyComplete( User *pUser, SP2Packet &rkPacket );
	void OnTradeSellComplete( User *pUser, SP2Packet &rkPacket );

	void OnTradeCancelInfoOK( User *pUser, SP2Packet &rkPacket );
	void OnTradeCancelInfoFail( User *pUser, SP2Packet &rkPacket );
	void OnTradeCancelComplete( User *pUser, SP2Packet &rkPacket );


	//
	void OnUserHeroData( SP2Packet &rkPacket );
	void OnHeroMatchOtherInfo( SP2Packet &rkPacket );
	void OnEventItemInitialize( SP2Packet &rkPacket );

	//
	void OnJoinHeadquartersUser( SP2Packet &rkPacket );
	void OnHeadquartersInfo( SP2Packet &rkPacket );
	void OnHeadquartersRoomInfo( SP2Packet &rkPacket );
	void OnHeadquartersJoinAgree( SP2Packet &rkPacket );
	void OnLogoutRoomAlarm( SP2Packet &rkPacket );
	void OnDisconnectAlreadyID( SP2Packet &rkPacket );

	// 2020-06-05 by bckim, 하드시리얼 유저 제재
	void OnDisconnectHddSerialBlock( SP2Packet &rkPacket );
	// End. 2020-06-05 by bckim, 하드시리얼 유저 제재


	//
	void OnUDPRecvTimeOut( SP2Packet &rkPacket );
	void OnServerAlarmMent( SP2Packet &rkPacket );

	bool OnConnectWorkingPacket( SP2Packet &rkPacket );
	bool OnUserTransferTCP( SP2Packet &rkPacket );

	// web
	bool OnWebUDPParse( SP2Packet &rkPacket );
	// billing
	bool OnBillingParse( SP2Packet &rkPacket );

	//login
	void OnLoginConnect(SP2Packet &rkPacket);
	void OnLoginRequestStatus();
	void OnLoginBlockRequest(SP2Packet &rkPacket);
	void OnLoginPartitionRequest(SP2Packet &rkPacket);

	//
	void OnTournamentCreateTeam( SP2Packet &rkPacket );
	void OnTournamentTeamAgreeOK( SP2Packet &rkPacket );
	void OnTournamentTeamJoin( SP2Packet &rkPacket );
	void OnTournamentTeamLeave( SP2Packet &rkPacket );
	void OnCloverSend( SP2Packet& rkPacket );

	// 선물 전송
	void OnPresentInsert( SP2Packet &rkPacket );
	void OnPresentInsertByEtcItem( SP2Packet &rkPacket );

	// 셔플
	void OnShuffleRoomTransfer( SP2Packet &rkPacket );
	void OnShuffleRoomJoinResult( SP2Packet &rkPacket );
	void OnShuffleRoomSync( SP2Packet &rkPacket );
	void OnShuffleRoomKickOut( SP2Packet &rkPacket );
	void OnExceptionShuffleRoomLeave( SP2Packet &rkPacket );
	void OnShuffleRoomGlobalCreate( SP2Packet &rkPacket );

	// 펫
	void OnMoveNewPetData( SP2Packet &rkPacket );

	// 용병,ETC, 메달
	void OnMoveSoldierData( SP2Packet &rkPacket );
	void OnMoveEtcItemData( SP2Packet &rkPacket );
	void OnMoveMedalItemData( SP2Packet &rkPacket );

	//코스튬
	void OnMoveCostumeData( SP2Packet &rkPacket );
	void OnCostumeAdd(SP2Packet &rkPacket);

	//미션
	void OnMoveMissionData( SP2Packet &rkPacket );

	//출석부
	void OnMoveRollBookData( SP2Packet &rkPacket );

	//길드 본부
	void OnReserveCreateGuildRoom(SP2Packet &rkPacket);
	void OnChangeGuildRoomStatus(SP2Packet &rkPacket);

	//개인 본부
	void OnJoinPersonalHQUser(SP2Packet &rkPacket);
	void OnPersonalHQInfo( SP2Packet &rkPacket );
	void OnPersonalHQRoomInfo( SP2Packet &rkPacket );
	void OnPersonalHQJoinAgree(SP2Packet &rkPacket );
	void OnMovePersonalHQData(SP2Packet &rkPacket );
	void OnPersonalHQAddBlock(SP2Packet &rkPacket );

	//기간 캐쉬 박스
	void OnMoveTimeCashData( SP2Packet &rkPacket );
	void OnUpdateTimeCashInfo( SP2Packet &rkPacket );

	//칭호
	void OnTitleUpdate(SP2Packet &rkPacket );
	void OnMoveTitleData( SP2Packet &rkPacket );

	//보너스 캐쉬
	void OnMoveBonusCashData(SP2Packet &rkPacket );
	void OnResultBonusCashAdd(SP2Packet &rkPacket );
	void OnResultBonusCashUpdate(SP2Packet &rkPacket );

	//악세사리
	void OnMoveAccessroyData(SP2Packet &rkPacket );
	void OnAccessoryAdd(SP2Packet &rkPacket);

	// 오크통 개선
	void OnResultOakBarrelGetInfo( SP2Packet &rkPacket );
	void OnResultOakBarrelUpdateInfo( SP2Packet &rkPacket );
	void OnResultOakBarrelUpdateTime( SP2Packet &rkPacket );
	void OnResultOakBarrelUpdateLimitSword( SP2Packet &rkPacket );

	void OnResultUserMatchUpdate( SP2Packet &rkPacket );
	void OnResultUserMatchLog( SP2Packet &rkPacket );

	void OnSetLadderMatchTime( SP2Packet &rkPacket );

	void OnSetBattleModeOrder( SP2Packet &rkPacket );	// 2019-02-14 by bckim, 배틀 모드 추가

	void OnSuccessionUserInfo( SP2Packet &rkPacket );

	void OnMoveCustomMedalData( SP2Packet &rkPacket );
	void OnCustomMedalAdd( SP2Packet &rkPacket );

	void OnResultDiceGameGetInfo(SP2Packet &rkPacket);		// 2018-08-30 by bckim, 주사위 이벤트 추가
	void OnUserPracticeAdmission(SP2Packet &rkPacket);
	void OnUserPracticeTime(SP2Packet &rkPacket);
public:
	//서버 이동시 데이터 분할 전송.
	void SendDecoMoveData( int iMovingValue, User *pUser );			// 치장
	void SendExtraItemMoveData( int iMovingValue, User *pUser );	// 장비

public:
	bool IsSameBilling(DWORD serverIndex);// 이동 완료 후 

public:
	ServerNode( SOCKET s = INVALID_SOCKET, DWORD dwSendBufSize = 0, DWORD dwRecvBufSize = 0 );
	virtual ~ServerNode();
};

#endif
