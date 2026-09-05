#ifndef __MgameBillingServer_h__
#define __MgameBillingServer_h__

class CConnectNode;
class MgameBillingServer : public CConnectNode
{
protected:
	enum 
	{ 
		UPDATE_TIME = 180000, // 3분 connect() 함수 딜레이 때문
	};

protected:
	static MgameBillingServer *sg_Instance;
	DWORD  m_dwCurrentTime;
	bool   m_bSendAlive;

protected:
	char m_szServerIP[MAX_PATH];
	int m_iSSPort;
public:
	static MgameBillingServer &GetInstance();
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
	void OnClose( SP2Packet &rkPacket );
	void OnCashAction( SP2Packet &rkPacket );


protected:
	MgameBillingServer( SOCKET s, DWORD dwSendBufSize, DWORD dwRecvBufSize );
	virtual ~MgameBillingServer();
};
#define g_MgameBillingServer MgameBillingServer::GetInstance()
#endif // __MgameBillingServer_h__