#pragma once

#include "../Util/NetworkSecurity.h"
#include "../Encrypt/FSM.h"

class ioState 
{
public:
	ioState() 
	{
		InitState();
	};
	~ioState() {};
public:
	void InitState()
	{
		m_iState = MAGIC_TOKEN_FSM;
	}
	long GetState() const { return m_iState; }
	void SetState(long val) { InterlockedExchange(&m_iState,val);}
	long UpdateState()
	{ 
		return InterlockedIncrement(&m_iState);
	}

protected:
	volatile long m_iState; 
};

class IOCP_SOCKET_API ioUDPSecurity : public NetworkSecurity
{
	//FSM m_SndState;
	ioState m_SndState;
	FSM m_RcvState;
	DWORD m_recent_rcv_ip;
	int	 m_recent_rcv_port;
	DWORD m_pre_recent_rcv_ip;
	int	 m_pre_recent_rcv_port;
	int m_iState;
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
	long UpdateAndGetSndState();

	// Send 
public:
	virtual void PrepareMsg( CPacket &rkPacket );
	virtual void CompletionMsg( CPacket &rkPacket );

public:
	void RcvPeerInfo( sockaddr_in *peer_addr );
	void RcvPeerInfo( DWORD dwIp, int iPort );

public:
	ioUDPSecurity();
	virtual ~ioUDPSecurity();	
};