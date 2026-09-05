#pragma once

#include "FSM.h"
#include "NetworkSecurity.h"


class SecurityTCP : public NetworkSecurity
{
public:
	SecurityTCP(void);
	~SecurityTCP(void);

public:
	void InitState();
		
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

private:
	FSM m_SndState;
	FSM m_RcvState;
};
