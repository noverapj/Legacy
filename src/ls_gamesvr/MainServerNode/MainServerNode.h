#ifndef _MainServerNode_h_
#define _MainServerNode_h_

#define DEFAULT_SERVER_UPDATE_TIME              20000

class CConnectNode;
class SP2Packet;


class MainServerNode : public CConnectNode
{
public:
	enum
	{
		RELOAD_HACK = 0,
		RELOAD_USERDISPERSION,
        RELOAD_PROCESS,
		RELOAD_CLASS,
		RELOAD_DECO,
		RELOAD_ETC,
		RELOAD_EVENT,
		RELOAD_QUEST,
		RELOAD_MODE,
		RELOAD_PRESENT,
		RELOAD_EXCAVATION,
		RELOAD_FISHING,
		RELOAD_ITEMCOMPOUND,
		RELOAD_EXTRAITEM,
		RELOAD_TRADE,
		RELOAD_LEVELMATCH,
		RELOAD_ITEMINIT_CONTROL,
		RELOAD_CONFIG,
		RELOAD_ALCHEMIC_MGR,
		RELOAD_PET,
		RELOAD_AWAKE,
		RELOAD_MISSION,
		RELOAD_ROLLBOOK,
		RELOAD_COSTUMESHOP,
		RELOAD_COSTUME,
		RELOAD_MEDAL,
		RELOAD_SETITEM,
		RELOAD_SUPERGASHAPON,
		RELOAD_ACCESSORY,
		RELOAD_RANKING,
		RELOAD_SUCCESSION_MODE,
		RELOAD_TIMEGATE,
	};

protected:
	typedef std::vector<int> VERSIONS;

	static MainServerNode *sg_Instance;
	DWORD m_dwCurrentTime;
	DWORD m_dwLastSendServerUpdateTime;

	int   m_iHeadquartersUserCount;
	int   m_iSafetySurvivalRoomUserCount;
	int   m_iPlazaUserCount;
	int   m_iBattleRoomUserCount;
	int   m_iLadderBattleUserCount;

	int   m_iTotalUserRegCount;

	VERSIONS m_vVersions;

	ioHashString m_szSendID;

	ioHashString m_szReserveCloseAnn;
	ioHashString m_szMainIP;
	int          m_iMainPort;

	int		m_iTopRankYear;
	int		m_iTopRankMonth;
	int		m_iTopRankDay;
	int		m_iTopRankHour;

public:
	static MainServerNode &GetInstance();
	static void ReleaseInstance();

public:
	virtual void SessionClose( BOOL safely=TRUE );
	virtual bool SendMessage( CPacket &rkPacket );
	virtual void ReceivePacket( CPacket &packet );
	virtual void PacketParsing( CPacket &packet );

	void OnRelayServerChangeState( SP2Packet & kPacket );

public:
	virtual void OnCreate();       //초기화
	virtual void OnDestroy();
	virtual bool CheckNS( CPacket &rkPacket );	
	virtual int  GetConnectType();

public:
	bool ConnectTo();

protected:
	void InitData();
	
public:
	void ProcessTime();

public:
	inline int GetHeadquartersUserCount()			{ return m_iHeadquartersUserCount; }
	inline int GetSafetySurvivalRoomUserCount()		{ return m_iSafetySurvivalRoomUserCount; }
	inline int GetPlazaUserCount()					{ return m_iPlazaUserCount; }
	inline int GetBattleRoomUserCount()				{ return m_iBattleRoomUserCount; }
	inline int GetLadderBattleUserCount()			{ return m_iLadderBattleUserCount; }
	inline int GetTotalUserRegCount()				{ return m_iTotalUserRegCount; }
	inline bool IsUseClientVersion()				{ return (m_vVersions.size() != 0) ? true : false; }
	inline int  GetClientVersion()
				{ 
					if(m_vVersions.empty()) return 0; 
					return m_vVersions[m_vVersions.size()-1]; 
				}

public:
	void MainServerNode::SendCountryCCU();
	void SendReserveCloseAnn( CConnectNode *pConnectUser );
	void SendNodeInfoRequest();

public:
	void OnMainServerConnect( SP2Packet &rkPacket );
	void OnServerIndex( SP2Packet &rkPacket );
	void OnAllServerList( SP2Packet &rkPacket );
	void OnNodeInfoResponse( SP2Packet & kPacket );

private:
	void OnAdminKickUser( SP2Packet &rkPacket );
	void OnAdminAnnounce( SP2Packet &rkPacket );
	void OnAdminItemInsert( SP2Packet &rkPacket );
	void OnAdminEventInsert( SP2Packet &rkPacket );
	void OnAdminUserBlock( SP2Packet &rkPacket );

public:
	void OnAdminCommand( SP2Packet &rkPacket );
	void OnClassPrice( SP2Packet &rkPacket );
	//void OnAnnounce( SP2Packet &rkPacket );
	void OnServerUpdate( SP2Packet &rkPacket );
	void OnTotalUserReg( SP2Packet &rkPacket );

	void OnGuildRankList( SP2Packet &rkPacket );
	void OnGuildInfo( SP2Packet &rkPacket );
	void OnGuildChangeJoiner( SP2Packet &rkPacket );
	void OnGuildSimpleInfo( SP2Packet &rkPacket );
	void OnGuildMarkBlockInfo( SP2Packet &rkPacket );
	void OnGuildTitleSync( SP2Packet &rkPacket );
	void OnGuildEntryAgreeResult( SP2Packet &rkPacket );
	void OnCampRoomBattleInfo( SP2Packet &rkPacket );
	void OnCampDataSync( SP2Packet &rkPacket );
	void OnCampBattleInfo( SP2Packet &rkPacket );
	void OnUserCampPointNRecordInit( SP2Packet &rkPacket );
	void OnCampInfluenceAlarm( SP2Packet &rkPacket );

	void OnLowConnectExit( SP2Packet &rkPacket );
	void OnUpdateClientVersion( SP2Packet &rkPacket );
	void OnLoadCS3File( SP2Packet &rkPacket );
	void OnGuildExist( SP2Packet &rkPacket );
	void OnCS3FileVersion( SP2Packet &rkPacket );
	void OnAutoCloseAnnounce( SP2Packet &rkPacket );

	void OnTradeList( SP2Packet &rkPacket );
	void OnTradeItemComplete( SP2Packet &rkPacket );
	void OnTradeItemCancel( SP2Packet &rkPacket );
	void OnTradeItemTimeOut( SP2Packet &rkPacket );

	void OnEventShopGoodsList( SP2Packet &rkPacket );
	void OnEventShopGoodsBuy( SP2Packet &rkPacket );

	void OnDBAgentExtend( SP2Packet &rkPacket );
	void OnGameServerReloadINI( SP2Packet &rkPacket );
	void OnGameServerOption( SP2Packet &rkPacket );
	void OnExtraItemGrowthCatalyst( SP2Packet &rkPacket );
	void OnExtraItemGrowthMortmainCheck( SP2Packet &rkPacket );
	void OnExtraItemGrowthMortmainInfo( SP2Packet &rkPacket );
	void OnServerInfoRequest( SP2Packet& rkPacket );
	void OnServerPingCheck( SP2Packet &rkPacket );

	void OnTournamentRegularInfo( SP2Packet &rkPacket );
	void OnTournamentMainInfo( SP2Packet &rkPacket );
	void OnTournamentListRequest( SP2Packet &rkPacket );
	void OnTournamentCreateTeam( SP2Packet &rkPacket );
	void OnTournamentTeamInfo( SP2Packet &rkPacket );
	void OnTournamentServerSync( SP2Packet &rkPacket );
	void OnTournamentEndProcess( SP2Packet &rkPacket );
	void OnTournamentScheduleInfo( SP2Packet &rkPacket );
	void OnTournamentTeamPositionSync( SP2Packet &rkPacket );
	void OnTournamentRoundTeamData( SP2Packet &rkPacket );
	void OnTournamentRoundStart( SP2Packet &rkPacket );
	void OnTournamentBattleRoomInvite( SP2Packet &rkPacket );
	void OnTournamentBattleTeamChange( SP2Packet &rkPacket );
	void OnTournamentCustomCreate( SP2Packet &rkPacket );
	void OnTournamentTeamAllocateList( SP2Packet &rkPacket );
	void OnTournamentTeamAllocateData( SP2Packet &rkPacket );
	void OnTournamentJoinConfirmCheck( SP2Packet &rkPacket );
	void OnTournamentAnnounceChange( SP2Packet &rkPacket );
	void OnTournamentTotalTeamList( SP2Packet &rkPacket );
	void OnTournamentCustomStateStart( SP2Packet &rkPacket );
	void OnTournamentCustomRewardList( SP2Packet &rkPacket );
	void OnTournamentCustomRewardRegCheck( SP2Packet &rkPacket );
	void OnTournamentCheerDecision( SP2Packet &rkPacket );
	void OnTournamentPrevChampSync( SP2Packet &rkPacket );
	void OnWhiteListRequest( SP2Packet &kPacket );
	void OnNpcEventRequest(SP2Packet &kPacket);
	void OnUpdateRelativeGrade( SP2Packet &rkPacket );
	void OnSuperGashponLimitCheckResult( SP2Packet &rkPacket );
	void OnSuperGashponLimitInfo( SP2Packet &rkPacket );

	void OnSpecialShopGoodsInfo( SP2Packet &rkPacket );
	void OnSpecialShopGoodsBuy( SP2Packet &rkPacket );
	void OnChangeSpecialShopState( SP2Packet &rkPacket );
	void OnSpecialShopGoodsBuyResult( SP2Packet &rkPacket );

	void OnResultGuildRoomReq( SP2Packet &rkPacket );
	void OnSendAllGuildRoomInfo(SP2Packet &rkPacket);

	void OnRegistCompensation(SP2Packet &rkPacket);

	void OnTradeGameSvrSync(SP2Packet &rkPacket);

	void OnReqSuccessionMatching(SP2Packet &rkPacket);
	void OnCancelSuccessionMatching(SP2Packet &rkPacket);

	void OnPractice_IniList(SP2Packet &rkPacket);
	void OnPractice_Index_Rank(SP2Packet &rkPacket);
	void OnPractice_UserUpdate(SP2Packet &rkPacket);
	void OnPractice_InitData(SP2Packet &rkPacket);

// 해외용 서버 라이센스 체크
#if defined( SRC_OVERSEAS )
	void OnRequestLicense( SP2Packet &rkPacket );
#endif


public:
	bool IsRightClientVersion( int iUserClientVersion );

	inline ioHashString &GetMainIP(){ return m_szMainIP; }
	inline int GetMainPort(){ return m_iMainPort; }

	ioHashString GetSendID(){ return m_szSendID; }
	void SetSendID(ioHashString szSendID)	{	m_szSendID = szSendID; }

protected:
	MainServerNode( SOCKET s, DWORD dwSendBufSize, DWORD dwRecvBufSize );
	virtual ~MainServerNode();
};
#define g_MainServer MainServerNode::GetInstance()
#endif