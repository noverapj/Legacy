#ifndef _MgrToolNode_h_
#define _MgrToolNode_h_

class CConnectNode;
class SP2Packet;
class MgrToolNode : public CConnectNode
{
	friend class MgrToolNodeManager;

	DWORD m_dwCurrentTime;
	ioHashString m_szGUID;			// 접속시 마다 새롭게 부여되는 유일한 id, node 식별용.
	ioHashString m_szMgrToolIP;
	ioHashString m_szID;			// 운영툴에서 인증한 아이디
	DWORD m_dwIndex;

public:
	static bool m_bUseSecurity;
	static int  m_iSecurityOneSecRecv;

public:
	virtual void SessionClose( BOOL safely=TRUE );
	virtual bool SendMessage( CPacket &rkPacket );
	virtual void ReceivePacket( CPacket &packet );
	virtual void PacketParsing( CPacket &packet );

public:
	virtual void OnCreate();       //초기화
	virtual void OnDestroy();
	virtual bool CheckNS( CPacket &rkPacket );	
	virtual int  GetConnectType();

public:
	static void LoadHackCheckValue();

public:
	bool IsGhostSocket();

protected:
	void InitData();

	void CreateGUID(OUT char *szGUID, IN int iSize);

public:
	void OnClose( SP2Packet &packet );
	void OnRequestNumConnect( SP2Packet &rkPacket );
	void OnAnnounce( SP2Packet &rkPacket );
	void OnUpdateClientVersion( SP2Packet &rkPacket );
	void OnLoadCS3File( SP2Packet &rkPacket );
	void OnMonitorIP( SP2Packet &rkPacket );
	void OnCS3FileVersion( SP2Packet &rkPacket );
	void OnServerInfoReq( SP2Packet &rkPacket );
	void OnMainServerExit( SP2Packet &rkPacket );
	void OnGameServerExit( SP2Packet &rkPacket );
	void OnReloadCloseINI( SP2Packet &rkPacket );
	void OnReloadGameServerINI( SP2Packet &rkPacket );
	void OnGameServerSetNagleRefCount( SP2Packet& rkPacket );
	void OnGameServerSetNagleTime( SP2Packet& rkPacket );
	void OnReciveToolDataSendGameServer( SP2Packet& rkPacket );
	void OnAdminCommand( SP2Packet& rkPacket );
	void OnResetEventShop( SP2Packet& rkPacket );
	void OnWhiteListReq( SP2Packet& rkPacket );
	void OnResetBingoNumber( SP2Packet& rkPacket );
	void OnResetOldMissionData( SP2Packet& rkPacket );

// 해외용 서버 라이센스 체크
#if defined( SRC_OVERSEAS )
	void OnRequestLicense( SP2Packet& rkPacket );
#endif

	inline const ioHashString& GetGUID() const	{ return m_szGUID; }
	inline const ioHashString& GetID() const	{ return m_szID; }
	inline const DWORD GetIndex() const			{ return m_dwIndex; }

protected:
	void SetID(ioHashString& szID);
	int AdminIDCheck(ioHashString& szUserID, ioHashString& szPassword);

	void OnAdminKickUser( const int iType, SP2Packet& rkPacket );
	void OnAdminAnnounce( const int iType, SP2Packet& rkPacket );
	void OnAdminItemInsert( const int iType, SP2Packet& rkPacket );
	void OnAdminEventInsert( const int iType, SP2Packet& rkPacket );
	void OnAdminUserBlock( const int iType, SP2Packet& rkPacket );
	void OnAdminUserUnblock( const int iType, SP2Packet& rkPacket );
	void OnAdminAuth( const int iType, SP2Packet& rkPacket );
	void OnAdminRenewalSecretShop(SP2Packet& rkPacket );
	void OnAdminRegistCompensation(SP2Packet& rkPacket );
	void OnAdminPracticeKickUserDelete(SP2Packet& rkPacket );

	void OnAdminMToolAuth( const int iType, SP2Packet& rkPacket );
	void OnAdminMToolKickUser( const int iType, SP2Packet& rkPacket );
	void OnAdminMToolUserBlock( const int iType, SP2Packet& rkPacket );
	void OnAdminMToolUserUnblock( const int iType, SP2Packet& rkPacket );

	void OnAdminMToolAnnounce( const int iType, SP2Packet& rkPacket );
	void OnAdminMToolItemInsert( const int iType, SP2Packet& rkPacket );
	void OnAdminMToolEventInsert( const int iType, SP2Packet& rkPacket );
	void OnAdminMToolPracticeKickUserDelete( SP2Packet& rkPacket );
	void OnMToolReciveToolDataSendGameServer( SP2Packet& rkPacket );
	void OnMToolMainServerExit( SP2Packet& rkPacket );
	void OnMToolGameServerExit( SP2Packet& rkPacket );
	void OnMToolReloadGameServerINI( SP2Packet& rkPacket );
	void OnMToolReloadCloseINI( SP2Packet& rkPacket );

public:
	void ApplyUserBlockDB( ioHashString& szUserID, LONG lSuccess );
	void ApplyChangeUserBlockDB(int ClientNumber, ioHashString& szUserID, LONG lSuccess );

public:
	MgrToolNode( SOCKET s=INVALID_SOCKET, DWORD dwSendBufSize=0, DWORD dwRecvBufSize=0 );
	virtual ~MgrToolNode();
};

#endif