#ifndef __NexonEUSessionServer_h_
#define __NexonEUSessionServer_h_

#include "../NexonEUDefine.h"
class CConnectNode;

class NexonEUSessionServer  : public CConnectNode 
{
protected:
	enum 
	{ 
		UPDATE_TIME = 30000, // 30초, 최소 1분내 1회 이상 요청이 있는걸로, 30초 에 1 회 alive 패킷 보냄
		ALIVE_TIME = 10000,
	};

	
protected:
	static NexonEUSessionServer *sg_Instance;
	
	EU_SESSION_HEADER	m_packetParse;
	DWORD				m_dwCurrentTime;
	bool				m_bSendAlive;

protected:
	char m_szServerIP[MAX_PATH];
	int m_iPort;
	int m_serverNo;

	

public:
	static NexonEUSessionServer &GetInstance();
	static void ReleaseInstance();

public:
	virtual void SessionClose( BOOL safely=TRUE );
	virtual void DispatchReceive(CPacket& packet, DWORD bytesTransferred);
	virtual bool SendMessage( CPacket &rkPacket );
	virtual void ReceivePacket( CPacket &packet );
	virtual void PacketParsing( CPacket &packet );
	virtual bool AfterCreate();

public:
	virtual void OnClose( SP2Packet &rkPacket );
	virtual void OnCreate();       //초기화
	virtual void OnDestroy();
	virtual bool CheckNS( CPacket &rkPacket );	
	virtual int  GetConnectType();

public:
	bool ConnectTo( bool bStart );
	void InitData();
	bool SendInitPacket( bool );

public:
	void ProcessTime();

	void OnInitialize( SP2Packet &rkPacket );
	void OnAlive( SP2Packet &rkPacket );
	void OnSessionResponse( SP2Packet &rkPacket );
	
	/*char  *GeEUTestPublicIP();
	bool IsEuTestMode();*/


public:
	bool m_bFirst;
	int	 m_iEUTestMode;
	char m_iEUTestPublicIP[MAX_PATH]; 

	NexonEUSessionServer( SOCKET s, DWORD dwSendBufSize, DWORD dwRecvBufSize );
	~NexonEUSessionServer(void);
};
#define g_NexonEUSessionServer NexonEUSessionServer::GetInstance()
#endif // __NexonEUSessionServer_h_
