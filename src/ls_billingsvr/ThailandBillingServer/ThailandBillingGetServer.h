#ifndef __ThailandBillingGetServer_h__
#define __ThailandBillingGetServer_h__

class CConnectNode;
class ThailandBillingGetServer : public CConnectNode
{
protected:
	enum 
	{ 
		UPDATE_TIME = 180000, // 3분 connect() 함수 딜레이 때문
	};

protected:
	static ThailandBillingGetServer *sg_Instance;
	DWORD  m_dwCurrentTime;

public:
	static ThailandBillingGetServer &GetInstance();
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
	void OnBillingGet( SP2Packet &rkPacket );
	void OnClose( SP2Packet &rkPacket );
protected:
	ThailandBillingGetServer( SOCKET s, DWORD dwSendBufSize, DWORD dwRecvBufSize );
	virtual ~ThailandBillingGetServer();
};
#define g_ThailandBillingGetServer ThailandBillingGetServer::GetInstance()
#endif // __ThailandBillingGetServer_h__