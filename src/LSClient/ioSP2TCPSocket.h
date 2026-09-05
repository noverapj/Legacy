#pragma once

#define WM_GAMESOCKET1  (WM_USER + 11)
#define WM_GAMESOCKET2  (WM_USER + 12)

class SP2Packet;
class ioSP2TCPSocket : public ioClientTCPSocket
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
	void InitState();

public:
	ioSP2TCPSocket( DWORD dwProcID );
	virtual ~ioSP2TCPSocket();
};

class ioTCPSocketManager : public Singleton< ioTCPSocketManager >
{
public:
	enum
	{
		CHANGE_NONE    = 0,
		CHANGE_RESERVE = 1,
		CHANGE_SERVER  = 2,
		CHANGE_CONNECT = 3,
		CHANGE_UDPCHECK= 4,
	};

	enum
	{
		MAX_WAITING_CHANGE = 90000,
	};
	
protected:
	ioSP2TCPSocket *m_pMainSocket;
	ioSP2TCPSocket *m_pChangeSocket;

	ioHashString    m_szServerIP;
	int				m_iServerPort;

	DWORD m_dwState;
	HWND  m_hWnd;	
	
	int   m_iMovingValue;
	int   m_iServerIndex;
	int   m_iRoomIndex;

	DWORD m_dwUDPHoleTime;
	DWORD m_dwChangeStartTime;
	bool  m_bTestZone;
	ioHashString m_szChangeState;
	ioHashString m_szChangeServerNumber;

protected:
	void SetServerNumber( const char *szServerIP );
	void SetChangeStateText( const char *szText );

public:
	bool Connect( HWND hWnd, const char *szIP, int iPort );
	bool MoveConnect( HWND hWnd, const char *szIP, int iPort );

public:
	bool ChangeServerProcess();
	bool IsChangeServer(){ return (m_dwState != CHANGE_NONE); }
	DWORD GetMoveState(){ return m_dwState; }
	int   GetMoveRoomIndex(){ return m_iRoomIndex; }
	void SetNagleAlgorithm( bool bOn );
	bool IsTestZone(){ return m_bTestZone; }
	bool GetChangeStateText( OUT ioHashString &rszStateText );

public:
	void ReserveServerMoving();
	void SetServerChange( const ioHashString &szServerIP, int iServerPort, int iMovingValue, int iServerIndex, int iRoomIndex );
	void ConnectProcess();	
	void CompleteChangeServer();         
	void ChangeServerStateEnd();
	
	// Socket Fun....
public:
	const char *GetConnectedIP();
	int         GetConnectedPort();
	bool        IsConnected();
	SOCKET      GetSocket();
	void		CloseClientTCP();

public:
	bool EvaluateSocket( DWORD dwProcID, WPARAM wParam, LPARAM lParam );

public:
	bool SendLoop( SP2Packet &rkPacket );

public:
	static ioTCPSocketManager& GetSingleton();

public:
	ioTCPSocketManager();
	virtual ~ioTCPSocketManager();
};
#define  g_TCPSocketMgr ioTCPSocketManager::GetSingleton()
//////////////////////////////////////////////////////////////////////////
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
