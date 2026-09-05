

#ifndef _ioUDPSocket_h_
#define _ioUDPSocket_h_

#include "ioPortalHelp.h"

class CPacket;
class NetworkSecurity;

class TOWN_PORTAL_API ioUDPSocket
{
protected:
	SOCKET  m_Socket;
	HWND	m_hWnd;
	UINT	m_WndMsg;

	char	m_RecentRcvIP[16];
	int		m_iRecentRcvPort;

	int		m_iMainPort;

	char	m_PreRecentRcvIP[16];
	int		m_iPreRecentRcvPort;

	char	m_szRcvBuf[MAX_BUFFER];
	char	m_szMyIP[MAX_PATH];

protected:
	void ClearIPInfo();

public:
	bool InitUDP( HWND hWnd, UINT uMsg, int iPort );
	void CloseUDP();

public:
	bool EvaluateSocket( WPARAM wParam, LPARAM lParam );
	bool SendLoop( const char *szIP, int iPort, CPacket &rkPacket );

protected:
	bool RecvLoop( SOCKET cSocket );
	virtual void ProcessPacket( CPacket &rkPacket, char *RcvIP, int iRcvPort ) = 0;

public:
	virtual void RcvPeerInfo( char *iip, int iPort );
	virtual void SocketErrorMsg( const char *szError );
	void GetRecentIPPort( char *szIP, int &iPort );
	const char* GetMyIP();

protected:
	NetworkSecurity *m_pNS;
	
public:
	void SetNS( NetworkSecurity *pNS );
	virtual bool CheckNS( CPacket &rkPacket );

public:
	ioUDPSocket();
	virtual ~ioUDPSocket();
};

#endif
