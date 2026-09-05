#ifndef __ioClientSocket_h__
#define __ioClientSocket_h__

#define WM_CLIENTSOCKET    (WM_USER + 13)
#define WM_CLIENTOTPSOCKET (WM_USER + 14)
#define WM_LOGINSOCKET	   (WM_USER + 15)
#define WM_MIGRATIONSOCKET (WM_USER + 16)

class ioClientSocket : public ioClientTCPSocket
{
protected:
	DWORD   m_dwProcID;

public:
	virtual void ConnectProcess();
	virtual void DisconnectProcess();
	virtual void ProcessPacket( CPacket &rkPacket );
	virtual bool CheckNS( CPacket &rkPacket );

public:
	DWORD GetProcID(){ return m_dwProcID; }

public:
	ioClientSocket( DWORD dwProcID );
	virtual ~ioClientSocket();
};
////////////////////////////////////////////////////////////////////////////
class FSM;
class ioTCPClientSecurity : public NetworkSecurity
{
	FSM m_SndState;
	FSM m_RcvState;

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

public:
	ioTCPClientSecurity();
	virtual ~ioTCPClientSecurity();
};

#endif // __ioClientSocket_h__