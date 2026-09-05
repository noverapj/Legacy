#ifndef __ToonilandBillingServer_h__
#define __ToonilandBillingServer_h__

class CConnectNode;
class ToonilandBillingServer : public CConnectNode
{
protected:
	enum 
	{ 
		UPDATE_TIME = 180000, // 3분 connect() 함수 딜레이 때문
	};

protected:
	static ToonilandBillingServer *sg_Instance;
	DWORD        m_dwCurrentTime;
	bool         m_bSendAlive;
	ioHashString m_sAliveCheckCPID;
protected:
	char m_szServerIP[MAX_PATH];
	int m_iSSPort;
	
public:
	static ToonilandBillingServer &GetInstance();
	static void ReleaseInstance();

public:
	virtual void SessionClose( BOOL safely=TRUE );
	virtual void DispatchReceive(CPacket& packet, DWORD bytesTransferred);
	virtual bool SendMessage( CPacket &rkPacket );
	virtual void ReceivePacket( CPacket &packet );
	virtual void PacketParsing( CPacket &packet );

public:
	virtual void OnCreate();       //초기화
	virtual bool AfterCreate();
	virtual void OnDestroy();
	virtual bool CheckNS( CPacket &rkPacket );	
	virtual int  GetConnectType();

public:
	bool ConnectTo( bool bStart );

protected:
	void InitData();

public:
	void ProcessTime();

public:
	void OnFindCash( SP2Packet &rkPacket );
	void OnBuyCash( SP2Packet &rkPacket );
	void OnSubscriptionRetractCash( SP2Packet &rkPacket );
	void OnClose( SP2Packet &rkPacket );


public:
	const ioHashString &GetAliveCheckCPID() const { return m_sAliveCheckCPID; }
	void SetSendAlive( bool bSendAlive ) { m_bSendAlive = bSendAlive; }

protected:
	ToonilandBillingServer( SOCKET s, DWORD dwSendBufSize, DWORD dwRecvBufSize );
	virtual ~ToonilandBillingServer();
};
#define g_ToonilandBillingServer ToonilandBillingServer::GetInstance()
#endif // __ToonilandBillingServer_h__