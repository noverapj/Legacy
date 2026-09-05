#ifndef __BillingRelayServer_h__
#define __BillingRelayServer_h__

class CConnectNode;
class SP2Packet;
class BillingRelayServer : public CConnectNode
{
protected:
	enum 
	{ 
		UPDATE_TIME = 15000,
	};
	
protected:
	static BillingRelayServer *sg_Instance;
	DWORD m_dwCurrentTime;

	std::vector<std::string> m_vServerIP;
	std::vector<int> m_vServerPort;

	ioHashString m_szBillingIP;
	int          m_iBillingPort;

protected:
	bool m_bReconnectState;

public:
	static BillingRelayServer &GetInstance();
	static void ReleaseInstance();

public:
	virtual void SessionClose( BOOL safely=TRUE );
	virtual bool SendMessage( CPacket &rkPacket );
	virtual void ReceivePacket( CPacket &packet );
	virtual void PacketParsing( CPacket &packet );

public:
	virtual void OnCreate();       //√ ±‚»≠
	virtual void OnDestroy();
	virtual bool CheckNS( CPacket &rkPacket );	
	virtual int  GetConnectType();
	virtual void OnClose();

public:
	bool ConnectTo();

protected:
	void InitData();

public:
	void ProcessTime();
	void ProcessFlush();
	
	void GenerateBillingServerInfo();
	void SetBillingServerInfo(std::vector<std::string>& vServerIP, std::vector<int>& vServerPort );
	int GetBillingServerCount() { return m_vServerIP.size(); }

public:
	void OnBillingPacketParsing( SP2Packet &rkPacket );

public:
	void SendUserInfo();


public://get/set
	bool GetReconnectState() const { return m_bReconnectState; }
	void SetReconnectState(bool val) { m_bReconnectState = val; }

public:
	inline ioHashString &GetBillingIP(){ return m_szBillingIP; }
	inline int GetBillingPort(){ return m_iBillingPort; }

public:
	BillingRelayServer( SOCKET s, DWORD dwSendBufSize, DWORD dwRecvBufSize );
	virtual ~BillingRelayServer(void);
};

#define g_BillingRelayServer BillingRelayServer::GetInstance()

#endif // __BillingRelayServer_h__
