#ifndef _ServerNode_h_
#define _ServerNode_h_


#define PING_CHECK_TIME              30000

class CConnectNode;
class SP2Packet;

class ServerNode : public CConnectNode
{
	friend class ServerNodeManager;

protected:
	DWORD        m_dwServerIndex;
	DWORD        m_dwNormalUserCount;
	DWORD        m_dwMgameUserCount;
	DWORD        m_dwDaumUserCount;
	DWORD        m_dwBuddyUserCount;
	DWORD        m_dwNaverUserCount;
	DWORD        m_dwToonilandUserCount;
	DWORD        m_dwNexonUserCount;
	DWORD		 m_dwHangameUserCount;
	DWORD		 m_dwValofeUserCount;
	DWORD        m_dwSteamUserCount;
	DWORD        m_dwHappyTukUserCount;

	DWORD        m_dwRoomCount;
	DWORD        m_dwPlazaCount;
	DWORD        m_dwBattleRoomCount;
	int          m_iSafetySurvivalRoomUserCount;
	int          m_iPlazaUserCount;
	int          m_iBattleRoomUserCount;
	int          m_iBattleRoomPlayUserCount;
	int          m_iLadderBattlePlayUserCount;

	int64		 m_serverID;
	ioHashString m_szServerName;
	ioHashString m_szPublicIP;
	ioHashString m_szPrivateIP;
	int          m_iServerPort;
	int          m_iClientPort;	
	
	DWORD        m_dwServerNoReactionTime;
	DWORD        m_dwServerPingSendTime;
	DWORD        m_dwSendPingTime;
	DWORD        m_dwPingMS;

	DWORDVec     m_vConnectServerIndex;           //연결된 서버 인덱스들...
	int          m_iDisconnectCheckCount;

	DWORD        m_dwDBQueryMS;
	BOOL        m_bRelayServerState;

public:
	virtual void SessionClose( BOOL safely=TRUE );
	virtual bool SendMessage( CPacket &rkPacket );
	virtual void ReceivePacket( CPacket &packet );
	virtual void PacketParsing( CPacket &packet );

	void OnNodeInfo( CPacket & packet );

public:
	virtual void OnCreate();       //초기화
	virtual bool AfterCreate();
	virtual void OnDestroy();
	virtual bool CheckNS( CPacket &rkPacket );	
	virtual int  GetConnectType();

protected:
	void InitData();
	
public:
	void SetServerIndex( DWORD dwServerIndex ){ m_dwServerIndex = dwServerIndex; }
	bool IsConnectServerIndex( DWORD dwServerIndex );
	void ApplyConnectServerIndex( SP2Packet &rkPacket );

public:
	void InitDisconnectCheckCount(){ m_iDisconnectCheckCount = 0; }
	bool IsDisconnectCheckOver();

public:
	void SendPingCheck();

public:
	inline const DWORD &GetServerIndex(){ return m_dwServerIndex; }
	inline DWORD GetUserCount() { return ( m_dwNormalUserCount + m_dwMgameUserCount + m_dwDaumUserCount + m_dwBuddyUserCount + m_dwNaverUserCount + m_dwToonilandUserCount + m_dwNexonUserCount + m_dwHangameUserCount + m_dwValofeUserCount); }
	DWORD  GetUserCountByChannelingType( ChannelingType eChannelingType );
	inline int   GetSafetySurvivalRoomUserCount(){ return m_iSafetySurvivalRoomUserCount; }
	inline int   GetPlazaUserCount(){ return m_iPlazaUserCount; }
	inline int   GetBattleRoomUserCount(){ return m_iBattleRoomUserCount; }
	inline int   GetBattleRoomPlayUserCount(){ return m_iBattleRoomPlayUserCount; }
	inline int   GetLadderBattlePlayUserCount(){ return m_iLadderBattlePlayUserCount; }
	inline ioHashString GetPrivateIP(){ return m_szPrivateIP; }
	inline ioHashString GetPublicIP(){ return m_szPublicIP; }
	inline int   GetClientPort(){ return m_iClientPort; }
	inline int   GetServerPort(){ return m_iServerPort; }
    WORD GetCurDelaySec(); 

public:
	void FillServerInfo( SP2Packet &rkPacket );

public:
	void OnClose( SP2Packet &packet );
	void OnServerInfo( SP2Packet &rkPacket );
	void OnServerUpdate( SP2Packet &rkPacket );
	void OnCreateGuildReg( SP2Packet &rkPacket );
	void OnGuildRankList( SP2Packet &rkPacket );
	void OnGuildInfo( SP2Packet &rkPacket );
	void OnGuildChangeJoiner( SP2Packet &rkPacket );
	void OnGuildEntryAgree( SP2Packet &rkPacket );
	void OnGuildLeave( SP2Packet &rkPacket );
	void OnGuildTitleChange( SP2Packet &rkPacket );
	void OnGuildSimpleInfo( SP2Packet &rkPacket );
	void OnGuildJoinUser( SP2Packet &rkPacket );
	void OnGuildMarkChange( SP2Packet &rkPacket );
	void OnGuildExist( SP2Packet &rkPacket );
	void OnGuildMarkBlockInfo( SP2Packet &rkPacket );
	void OnLadderModeResultUpdate( SP2Packet &rkPacket );
	void OnGuildTitleSync( SP2Packet &rkPacket );	
	void OnGuildAddLadderPoint( SP2Packet &rkPacket );
	void OnCampRoomBattleInfo( SP2Packet &rkPacket );
	void OnCampDataSync( SP2Packet &rkPacket );
	void OnCampEntryChange( SP2Packet &rkPacket );
	void OnGuildNameChange( SP2Packet &rkPacket );
	void OnNickNameChange( SP2Packet &rkPacket );
	void OnResultLoadCS3File( SP2Packet &rkPacket );
	void OnResultCS3FileVersion( SP2Packet &rkPacket );
	void OnResultUpdateClientVersion( SP2Packet &rkPacket );
	void OnDecreaseGuildUserCount( SP2Packet &rkPacket );
	void OnReqGuildRoomIndex( SP2Packet &rkPacket );
	void OnUpdateGuildRoomIndex( SP2Packet &rkPacket );
	void OnDeleteGuildRoomInfo( SP2Packet &rkPacket );
	void OnGuildRoomsInfo( SP2Packet &rkPacket );

	void OnCreateTradeReg( SP2Packet &rkPacket );
	void OnTradeList( SP2Packet &rkPacket );
	void OnTradeItem( SP2Packet &rkPacket );
	void OnTradeCancel( SP2Packet &rkPacket );

	void OnEventShopGoodsList( SP2Packet &rkPacket );
	void OnEventShopGoodsBuy( SP2Packet &rkPacket );
	void OnEventShopGoodsBuyResult( SP2Packet &rkPacket );
	void OnEventShopState( SP2Packet &rkPacket );
	void OnEventShopBuyUserClear( SP2Packet &rkPacket );
	
	void OnExtraItemGrowthMortmainCheck( SP2Packet &rkPacket );
	void OnServerPingCheck( SP2Packet &rkPacket );
	void OnExtraItemGrowthMortmainInfo( SP2Packet &rkPacket );
	void OnServerInfoAck( SP2Packet& rkPacket );

	void OnTournamentRegularInfo( SP2Packet &rkPacket );
	void OnTournamentMainInfo( SP2Packet &rkPacket );
	void OnTournamentListRequest( SP2Packet &rkPacket );
	void OnTournamentTeamCreate( SP2Packet &rkPacket );
	void OnTournamentTeamInfo( SP2Packet &rkPacket );
	void OnTournamentTeamDelete( SP2Packet &rkPacket );
	void OnTournamentTeamLadderPointAdd( SP2Packet &rkPacket );
	void OnTournamentScheduleInfo( SP2Packet &rkPacket );
	void OnTournamentRoundTeamData( SP2Packet &rkPacket );
	void OnTournamentRoundCreateBattleRoom( SP2Packet &rkPacket );
	void OnTournamentBattleResult( SP2Packet &rkPacket );
	void OnTournamentBattleTeamChange( SP2Packet &rkPacket );
	void OnTournamentCustomCreate( SP2Packet &rkPacket );
	void OnTournamentTeamAllocateList( SP2Packet &rkPacket );
	void OnTournamentTeamAllocateData( SP2Packet &rkPacket );
	void OnTournamentJoinConfirmCheck( SP2Packet &rkPacket );
	void OnTournamentJoinConfirmReg( SP2Packet &rkPacket );
	void OnTournamentAnnounceChange( SP2Packet &rkPacket );
	void OnTournamentTotalTeamList( SP2Packet &rkPacket );
	void OnTournamentCustomStateStart( SP2Packet &rkPacket );
	void OnTournamentCustomRewardList( SP2Packet &rkPacket );
	void OnTournamentCustomRewardRegCheck( SP2Packet &rkPacket );
	void OnTournamentCustomRewardRegUpdate( SP2Packet &rkPacket );
	void OnSuperGashponLimitCheck( SP2Packet &rkPacket );
	void OnSuperGashponLimitDecrease( SP2Packet &rkPacket );
	void OnSuperGashponLimitInfo( SP2Packet &rkPacket );
	void OnSuperGashponLimitReset( SP2Packet &rkPacket );
	void OnTournamentCheerDecision( SP2Packet &rkPacket );

	void OnEventNpcClose( SP2Packet &rkPacket );

	void OnTournamentMacro(SP2Packet &rkPacket);
	void TestTournamentNextStep();
	void TestTournamentEnd();

	//스패셜 삽
	void OnSpecialShopGoodsBuy( SP2Packet &rkPacket );
	void OnSpecialShopGoodsBuyResult( SP2Packet &rkPacket );

	void OnReqUserMatch( SP2Packet &rkPacket );
	void OnCancelUserMatch( SP2Packet &rkPacket );

	// 수련장 
	void OnPractice_IniList( SP2Packet &rkPacket );
	void OnPractice_Indexrank( SP2Packet &rkPacket );
	void OnPractice_UserUpdate( SP2Packet &rkPacket );

// 해외용 서버 라이센스 체크
#if defined( SRC_OVERSEAS )
	void OnSendLicenseAlert( SP2Packet &rkPacket );
#endif

public:
	ServerNode( SOCKET s=INVALID_SOCKET, DWORD dwSendBufSize=0, DWORD dwRecvBufSize=0 );
	virtual ~ServerNode();
};

#endif
