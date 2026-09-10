#pragma once

#define WM_P2PSOCKET	(WM_USER + 14)
#define P2PSOCKET_PORT	14009 // 공유기 때문에 IP 주소 끝단을 더하므로 클라이언트에서는 14009~14263 까지 사용함 예) 192.168.1.111 -> 14009+111 -> 14120

class FSM;

class ioClientUDPSocket : public ioUDPSocket
{
public:
	virtual void ProcessPacket( CPacket &rkPacket, char *RcvIP, int iRcvPort );

public:
	virtual void RcvPeerInfo( char *iip, int iPort );
	virtual bool CheckNS( CPacket &rkPacket );
	
public:
	ioClientUDPSocket( int iSecurity );
	virtual ~ioClientUDPSocket();
};

//////////////////////////////////////////////////////////////////////////

class ioUDPClientSecurity : public NetworkSecurity
{
protected:
	FSM		m_SndState;
	FSM		m_RcvState;

	char m_recent_rcv_ip[16];
	int	 m_recent_rcv_port;

	char m_pre_recent_rcv_ip[16];
	int	 m_pre_recent_rcv_port;
	
	// Packet CheckSum
protected:
	void  EncryptMsg( CPacket &rkPacket );
	void  DecryptMsg( CPacket &rkPacket );

public:
	virtual bool IsCheckSum( CPacket &rkPacket );
	
	// Packet Replay	
public:
	virtual int	 GetSndState();
	virtual void UpdateSndState();
	virtual int  GetRcvState();
	virtual void UpdateRcvState();
	virtual bool CheckState( CPacket &rkPacket );
	
	// Send 
public:
	virtual void PrepareMsg( CPacket &rkPacket );
	virtual void CompletionMsg( CPacket &rkPacket );
	
public:
	void RcvPeerInfo( char *iip, int iPort );

public:
	ioUDPClientSecurity();
	virtual ~ioUDPClientSecurity();
};
