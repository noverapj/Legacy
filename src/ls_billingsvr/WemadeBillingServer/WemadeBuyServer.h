#ifndef __WemadeBuyServer_h__
#define __WemadeBuyServer_h__

class CConnectNode;
class WemadeBuyServer : public CConnectNode
{
protected:
	enum 
	{ 
		UPDATE_TIME = 180000, // 3분 connect() 함수 딜레이 때문
	};

	enum WemadeBuyServerTypes{
		ALIVE = 0, 
		BALANCE = 10,
		BUY = 20,
		PRESENT = 21,
		CANCEL = 22,
		SUBSCRIPTION_RETRACT = 24, 
		SUBSCRIPTION_RETRACT_CHECK = 25,
		SUBSCRIPTION_RETRACT_CANCEL = 26,
	};

protected:
	static WemadeBuyServer *sg_Instance;
	DWORD  m_dwCurrentTime;
	bool   m_bSendAlive;

protected:
	char m_szServerIP[MAX_PATH];
	int m_iSSPort;

public:
	static WemadeBuyServer &GetInstance();
	static void ReleaseInstance();

public:
//	virtual bool Dispatch( DWORD bytesTransferred, OVERLAPPED *ov, int &iReturnCode );
	virtual void SessionClose( BOOL safely=TRUE );
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
	void DispatchReceive( CPacket& packet, DWORD bytesTransferred );

public:
	void ProcessTime();

public:
	void OnAlive( SP2Packet &rkPacket );
	void OnBalance( SP2Packet &rkPacket );
	void OnBuy( SP2Packet &rkPacket );
	void OnPresent( SP2Packet &rkPacket );
	void OnCancel( SP2Packet &rkPacket );
	void OnClose( SP2Packet &rkPacket );
	void OnRetractCheck ( SP2Packet& rkPacket );
	void OnRetract ( SP2Packet& rkPacket );
	void OnRetractCancel ( SP2Packet& rkPacket );


protected:
	WemadeBuyServer( SOCKET s, DWORD dwSendBufSize, DWORD dwRecvBufSize );
	virtual ~WemadeBuyServer();
};
#define g_WemadeBuyServer WemadeBuyServer::GetInstance()
#endif // __WemadeBuyServer_h__