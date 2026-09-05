// cServerSocket.h: interface for the cServerSocket class.
//
//////////////////////////////////////////////////////////////////////

#pragma once


class cServerSocket  
{
public:
	cServerSocket();
	virtual ~cServerSocket();

	void Init();
	void Destroy();

public:
	BOOL Startup();
	void Cleanup();

	BOOL CreateSocket();
	void CloseSocket();

	BOOL Bind( const uint16 port, const TCHAR* IP = NULL );
	BOOL Listen( const int32 backlog = SOMAXCONN );

	BOOL AcceptRequest( SOCKET socket, PVOID outBuffer, LPOVERLAPPED overlapped );

public:
	void SetSocketHandle( SOCKET socket )	{	m_serverSocket = socket;		}
	SOCKET GetSocketHandle()				{	return m_serverSocket;			}

	void IncreaseAcceptingCount()			{	InterlockedIncrement( &m_acceptingCount );	}
	void DecreaseAcceptingCount()			{	InterlockedDecrement( &m_acceptingCount );	}

	LONG GetAcceptingCount()				{	return m_acceptingCount;		}
	LONG GetBacklog()						{	return m_backlog;				}

protected:
	LONG	m_acceptingCount;
	LONG	m_backlog;
	SOCKET	m_serverSocket;
};
