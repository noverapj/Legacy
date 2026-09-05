#pragma once

#define MAX_BYPASS_MAGIC_TOKEN           5
class FSM;

class ioServerSecurity : public NetworkSecurity
{
	FSM m_SndState;
	FSM m_RcvState;
	
	int m_iRcvCount;
	int m_iMaxRcvCheck;
	DWORD m_dwRcvCurTimer;
	
	SOCKET m_Socket;	
public:
	void InitDoSAttack( int iMaxRcvCount );
	void InitState( SOCKET csocket );
	inline int  GetRcvCount() const { return m_iRcvCount; }	
	// DoS Attack
public:
	virtual bool UpdateReceiveCount();
	
	// Packet CheckSum
protected:	
	void  EncryptMsg( CPacket &rkPacket );
	void  DecryptMsg( CPacket &rkPacket );
public:
	virtual bool IsCheckSum( CPacket &rkPacket );
	
	// Packet Replay
protected:
	int m_iCurMagicNum;
	inline void AddMagicNum() { m_iCurMagicNum++; }
	inline int GetMagicNum() const { return m_iCurMagicNum; }
	inline void ClearMagicNum() { m_iCurMagicNum = 0; }
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
	ioServerSecurity();
	virtual ~ioServerSecurity();	
};