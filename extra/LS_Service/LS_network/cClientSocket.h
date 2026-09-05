// cClientSocket.h: interface for the cClientSocket class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "cBaseThread.h"

class cClientSocket : public cBaseThread
{
public:
	cClientSocket();
	virtual ~cClientSocket();

	void Init();
	void Destroy();

public:
	BOOL Startup();
	void Cleanup();

	void CloseSocket();

public:
	SOCKET GetSocketHandle()						{	return m_socket;			}
	void SetSocketHandle( SOCKET socket )			{	m_socket	= socket;		}

	BOOL SetSocketOption();

public:
	BOOL Create();
	BOOL Connect( const TCHAR *IP = NULL, const uint16 port = 0 );
	BOOL Reconnect();

public:
	BOOL Send( const uint8 *buffer, const uint32 length ); 
	BOOL Receive( uint8* buffer, uint32& length );

protected:
	virtual	void OnConnected()		{}
	virtual	void OnDisconnected()	{}
	virtual	void OnReceived( uint8* buffer, const uint32 length )	{}

protected:
	virtual void Process();

protected:
	SOCKADDR_IN m_sockAddr;

	BOOL m_connected;
	SOCKET	m_socket;
};