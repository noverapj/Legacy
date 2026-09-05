#pragma once

#include "FSM.h"
#include "NetworkSecurity.h"

class SecurityUDP : public NetworkSecurity
{
public:
	SecurityUDP(void);
	~SecurityUDP(void);

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

protected:
	FSM m_SndState;
	FSM m_RcvState;
	char m_recent_rcv_ip[16];
	int	 m_recent_rcv_port;
	char m_pre_recent_rcv_ip[16];
	int	 m_pre_recent_rcv_port;

};
