#include "StdAfx.h"
#include "cServerSocketManager.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

cServerSocketManager::cServerSocketManager(void)
{
	Init();
}

cServerSocketManager::~cServerSocketManager(void)
{
	Destroy();
}

void cServerSocketManager::Init()
{
	m_sockets.reserve(8);
}

void cServerSocketManager::Destroy()
{
	for( uint32 i = 0 ; i < m_sockets.size() ; i++ )
	{
		cServerSocket* serverSocket = m_sockets[i];
		if( serverSocket )
		{
			delete serverSocket;
			serverSocket = NULL;
		}
	}
	m_sockets.clear();
}

//////////////////////////////////////////////////////////////////////
// Operation
//////////////////////////////////////////////////////////////////////

BOOL cServerSocketManager::AddSocket(const uint16 port, const int32 backlog, uint32& ID)
{
	cServerSocket* serverSocket = new cServerSocket;
	if( serverSocket )
	{
		if( !serverSocket->Startup() )
		{
			delete serverSocket;
			Information( _T("[%lu]LS system :: server socket startup(%lu)\r\n"), GetCurrentThreadId(), GetLastError() );
			return FALSE;
		}
		if( !serverSocket->CreateSocket() )
		{
			delete serverSocket;
			Information( _T("[%lu]LS system :: server socket create(%lu)\r\n"), GetCurrentThreadId(), GetLastError() );
			return FALSE;
		}
		if( !serverSocket->Bind(port) )
		{
			delete serverSocket;
			Information( _T("[%lu]LS system :: server socket bind(%lu)\r\n"), GetCurrentThreadId(), GetLastError() );
			return FALSE;
		}
		if( !serverSocket->Listen(backlog) )
		{
			delete serverSocket;
			Information( _T("[%lu]LS system :: server socket listen(%lu)\r\n"), GetCurrentThreadId(), GetLastError() );
			return FALSE;
		}

		ID = m_sockets.size();

		m_sockets.push_back( serverSocket );
		return TRUE;
	}
	return FALSE;
}

cServerSocket* cServerSocketManager::GetSocket(const uint32 ID)
{
	return (ID < m_sockets.size()) ? m_sockets.at(ID) : NULL;
}
