

#ifndef _ioSP2TCPSocket_h_
#define _ioSP2TCPSocket_h_

#define WM_TCPSOCKET (WM_USER + 11)
#define RIGHT_MSG_CHECK_VALUE "8302kdlso209f&3%#!%&&f"

class ioSP2TCPSocket : public ioClientTCPSocket
{
public:
	virtual void ConnectProcess();
	virtual void DisconnectProcess();
	virtual void ProcessPacket( CPacket &rkPacket );
	virtual bool CheckNS( CPacket &rkPacket );

public:
	ioSP2TCPSocket();
	virtual ~ioSP2TCPSocket();
};
//////////////////////////////////////////////////////////////////////////
class FSM;
class ioTCPClientSecurity : public NetworkSecurity
{
	FSM m_SndState;
	FSM m_RcvState;
		
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
	ioTCPClientSecurity();
	virtual ~ioTCPClientSecurity();
};
#endif