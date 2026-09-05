// ConnectNode.h: interface for the CConnectNode class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "../ThreadModules/SuperParent.h"
#include "IOCP.h"
#include "Packet.h"
#include "SendIO.h"
#include "ReceiveIO.h"


class NetworkSecurity;

class IOCP_SOCKET_API CCommandNode
{
protected:
	bool m_active;

public:
	virtual void SessionClose( BOOL safely=TRUE )  {}
	virtual void PacketParsing( CPacket &packet ) {}
	virtual void PacketParsing( CPacket &packet, SOCKET socket ) {}
	virtual void ReceivePacket( CPacket &packet ) {}

public:
	virtual void OnCreate()
	{
		SetActive(true);
	}

	void SetActive(bool b)	{ m_active = b; }
	bool IsActive()			{ return m_active; }

public:
	CCommandNode() : m_active(false)	{}
	virtual ~CCommandNode()				{}
};

class IOCP_SOCKET_API CConnectNode : public SuperParent,
									 public NetworkParent,
									 public CCommandNode,
									 public CSendIO
{
protected:
	bool GetPeerIP(char* remoteIP, const int size, int& remotePort);
	bool WaitForPacketReceive();
	
	SOCKET		m_socket;
	char        m_public_ip[16];       //사용자 외부 아이피
	char        m_private_ip[16];      //사용자 내부 아이피
	int         m_client_port;
	bool        m_bSendBlock;

public:
	virtual void OnCreate();
	virtual void OnDestroy();
	
	virtual bool AfterCreate();

public:
	virtual bool  Dispatch( DWORD bytesTransferred, OVERLAPPED *ov, CPacket& packet );
	virtual void  DispatchReceive(CPacket& packet, DWORD bytesTransferred);

	virtual bool  SendMessage( CPacket &rkPacket, const BOOL immediatelySend = FALSE );
	virtual	bool  SendMessage( const char* buffer, const int size, const BOOL immediatelySend );

    virtual int  GetConnectType() = 0;

public:
	BOOL EnableSend(const LONG count);

	void CloseConnection();
	void ExceptionClose(int lasterr);
	
public:
	SOCKET GetSocket(){ return m_socket; }
	int    GetUDP_port();
	char  *GetPublicIP();
	char  *GetPrivateIP();
		
public:
	void SetSocket(SOCKET csocket);

	void SetSocketHandle(SOCKET socket)	{ m_socket = socket; }
	SOCKET GetSocketHandle()			{ return m_socket; }
	void SetIPMapping( const char *privateIP );
	void SetSendBlock( const bool b )	{ m_bSendBlock = b; }
	bool GetSendBlock()					{ return m_bSendBlock; }
	virtual void SetClientAddr(sockaddr_in client_addr);
	
protected:
	CReceiveIO	m_recvIO;
	NetworkSecurity *m_pNS;

public:
	void SetNS( NetworkSecurity *pNS );
	virtual bool CheckNS( CPacket &rkPacket );

public:
	void SetNagleAlgorithm( bool bOn );

public:
	CConnectNode( SOCKET s, DWORD dwSendBufSize, DWORD dwRecvBufSize );
	virtual ~CConnectNode();
};
