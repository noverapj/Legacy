#ifndef __USBillingServer_h__
#define __USBillingServer_h__

class CConnectNode;
class USBillingServer : public CConnectNode
{
protected:
	enum 
	{ 
		UPDATE_TIME = 10000, // 3분 connect() 함수 딜레이 때문
	};

protected:
	static USBillingServer *sg_Instance;
	DWORD  m_dwCurrentTime;
	bool   m_bSendAlive;

public:
	static USBillingServer &GetInstance();
	static void ReleaseInstance();

public:
	virtual void SessionClose( BOOL safely=TRUE );
	virtual void DispatchReceive(CPacket& packet, DWORD bytesTransferred);
	virtual bool SendMessage( CPacket &rkPacket );
	virtual void ReceivePacket( CPacket &packet );
	virtual void PacketParsing( CPacket &packet );

public:
	virtual void OnCreate();       //초기화
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
	void OnAlive( SP2Packet &rkPacket );
	void OnBalance( SP2Packet &rkPacket );
	void OnBuy( SP2Packet &rkPacket );
	void OnCancel( SP2Packet &rkPacket );
	void OnClose( SP2Packet &rkPacket );
	void OnHealthCheck( SP2Packet &rkPacket );
	void OnCheckPremium2( SP2Packet &rkPacket );
	void OnGSConnect( SP2Packet &rkPacket );

protected:
	USBillingServer( SOCKET s, DWORD dwSendBufSize, DWORD dwRecvBufSize );
	virtual ~USBillingServer();
};
#define g_USBillingServer USBillingServer::GetInstance()
#endif // __USBillingServer_h__