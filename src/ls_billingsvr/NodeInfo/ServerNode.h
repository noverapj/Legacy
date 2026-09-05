#ifndef _ServerNode_h_
#define _ServerNode_h_


#define DEFAULT_SERVER_UPDATE_TIME              15000

class CConnectNode;
class SP2Packet;
class ServerNode : public CConnectNode
{
public:
	enum SessionState
	{
		SS_DISCONNECT	= 0,
		SS_CONNECT		= 1,
	};

protected:
	friend class ServerNodeManager;

	ioHashString m_szServerIP;
	int          m_iClientPort;	
	SessionState m_eSessionState;
	DWORD		 m_serverIndex;
	BOOL		 m_bSendLoginInfo;

public:
	void InitData();
	
public:
	virtual void ReceivePacket( CPacket &packet );
	virtual void PacketParsing( CPacket &packet );
	
public:
	void SetSessionState( ServerNode::SessionState eState ){ m_eSessionState = eState; }
	bool IsDisconnectState(){ return ( m_eSessionState == SS_DISCONNECT ); }
	bool IsConnectState(){ return ( m_eSessionState == SS_CONNECT ); }
	
public:
	virtual void OnCreate();       //√ ±‚»≠
	virtual void OnDestroy();
	virtual bool CheckNS( CPacket &rkPacket );	
	virtual int  GetConnectType();
	virtual void SessionClose( BOOL safely=TRUE );

public:
	const ioHashString &GetIP() { return m_szServerIP; }
	int GetClientPort() { return m_iClientPort; }
	BOOL GetSendLoginInfo() const { return m_bSendLoginInfo; }
	void SetSendLoginInfo(BOOL val) { m_bSendLoginInfo = val; }
public:
	void OnGetCash( SP2Packet &rkPacket );
	void OnOutputCash( SP2Packet &rkPacket );
	void OnServerIPPort( SP2Packet &rkPacket );
	void OnLogin( SP2Packet &rkPacket );
	void OnRefundCash( SP2Packet &rkPacket );
	void OnUserInfo( SP2Packet &rkPacket );
	void OnPCRoom( SP2Packet &rkPacket );
	void OnOTP( SP2Packet &rkPacket );
	void OnGetMileage( SP2Packet &rkPacket );
	void OnAddMileage( SP2Packet &rkPacket );
	void OnIPBonus( SP2Packet &rkPacket );
	void OnCancelCash( SP2Packet &rkPacket );
	void OnIPBonusOut( SP2Packet &rkPacket );
	void OnAddCash( SP2Packet &rkPacket );
	void OnFillCashUrl( SP2Packet &rkPacket );
	void OnClose( SP2Packet &rkPacket );
	void OnSubscriptionRetractCheck( SP2Packet& rkPacket );
	void OnSubscriptionRetract( SP2Packet& rkPacket );
	void OnSessionControl( SP2Packet& rkPakcet );
	void SendRequestLoginInfo();
	void OnLogout(SP2Packet &rkPacket );


protected:
	void OnSessionControlPacketParse(SP2Packet& rkPacket, int controlType);
	void OnSessionLogin(SP2Packet& rkPacket);
	void OnSessionLogout(SP2Packet& rkPacket);
	void OnSessionReConnect(SP2Packet& rkPacket);
	void OnSessionFirstLogin(SP2Packet& rkPacket);
	void OnSessionLogoutLog(SP2Packet& rkPacket);
	void OnGetCCUCount(SP2Packet& rkPacket);
	
protected:
	void OnDaumShutDownCheck( SP2Packet& rkPacket );
	void OnRequestTimeCash( SP2Packet& rkPacket );

public:
	virtual bool SendMessage( CPacket &rkPacket );

public:
	ServerNode();
	
protected:
	ServerNode( SOCKET s, DWORD dwSendBufSize, DWORD dwRecvBufSize );
	virtual ~ServerNode();
};

#endif
