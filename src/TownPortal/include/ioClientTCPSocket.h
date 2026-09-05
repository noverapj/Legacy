

#ifndef _ioClientTCPSocket_h_
#define _ioClientTCPSocket_h_

class CPacket;
class SendBuffer;
class RecvBuffer;
class NetworkSecurity;

class TOWN_PORTAL_API ioClientTCPSocket
{
protected:
	SOCKET  m_Socket;
	HWND	m_hWnd;

	UINT	m_WndMsg;

	SendBuffer *m_pSndBuf;
	RecvBuffer *m_pRcvBuf;
	
	char	m_szConnectedIP[MAX_PATH];
	int		m_iConnectedPort;
	bool	m_bConnected;
	BOOL    m_bNoNagle;

public:
	bool InitClientTCP( HWND hWnd, UINT uMsg, const char *szIP, int iPort );
	void CloseClientTCP();
	bool ReConnect();
	HWND Wnd() const { return m_hWnd; }
	void Wnd(HWND val) { m_hWnd = val; }
public:
	bool EvaluateSocket( WPARAM wParam, LPARAM lParam );

	virtual bool SendLoop( CPacket &rkPacket );
	virtual bool RecvLoop( SOCKET cSocket );

protected:
	bool BufferSendLoop();

public:
	virtual void ConnectProcess() = 0;
	virtual void DisconnectProcess() = 0;
	virtual void ProcessPacket( CPacket &rkPacket ) = 0;

public:
	const char* GetConnectedIP() const;
	int  GetConnectedPort() const;
	bool IsConnected() const;

	SOCKET GetSocket() const;

public:
	void SetNagleAlgorithm( bool bOn );
	void SetRetransmission( char cMaxRT = 10 );

protected:
	NetworkSecurity *m_pNS;
	
public:
	void SetNS( NetworkSecurity *pNS );
	virtual bool CheckNS( CPacket &rkPacket );

public:
	ioClientTCPSocket();
	virtual ~ioClientTCPSocket();
};

#endif

