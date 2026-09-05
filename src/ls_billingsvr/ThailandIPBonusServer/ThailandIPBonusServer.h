#ifndef __ThailandIPBonusServer_h__
#define __ThailandIPBonusServer_h__

class CConnectNode;
class ThailandIPBonusServer : public CConnectNode
{
protected:
	enum 
	{ 
		UPDATE_TIME = 180000, // 3분 connect() 함수 딜레이 때문
	};

protected:
	static ThailandIPBonusServer *sg_Instance;
	DWORD  m_dwCurrentTime;

public:
	static ThailandIPBonusServer &GetInstance();
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
	void OnIPBonus( SP2Packet &rkPacket );
	void OnClose( SP2Packet &rkPacket );
protected:
	ThailandIPBonusServer( SOCKET s, DWORD dwSendBufSize, DWORD dwRecvBufSize );
	virtual ~ThailandIPBonusServer();
};
#define g_ThailandIPBonusServer ThailandIPBonusServer::GetInstance()
#endif // __ThailandIPBonusServer_h__