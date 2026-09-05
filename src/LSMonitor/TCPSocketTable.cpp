#include "stdafx.h"
#include "LSMonitorDlg.h"
#include ".\tcpsockettable.h"

TCPSocketTable *TCPSocketTable::sg_Instance = NULL;

TCPSocketTable::TCPSocketTable(void)
{
}

TCPSocketTable::~TCPSocketTable(void)
{
	CloseAndRelease();
}

bool TCPSocketTable::AddAndInit( HWND hWnd, vZONEINFO &rvConnectInfo )
{
	const int iMax = (int) rvConnectInfo.size();
	m_hWnd = hWnd;
	for (int i = 0; i < iMax ; i++)
	{
		if(i >= MAX_TCPSOCKET_MAP)
		{
			LOG.PrintTimeAndLog(0, "Error - MAX_TCPSOCKET_MAP");
			return false;
		}

		ioSP2TCPSocket *pSocket = new ioSP2TCPSocket;
		m_TCPSocketMap.insert( TCPSocketMap::value_type( WM_TCPSOCKET1+i, pSocket));
		TCPSocketMap::iterator iter = m_TCPSocketMap.find( WM_TCPSOCKET1+i );
		if( iter != m_TCPSocketMap.end())
		{
			ioSP2TCPSocket *pSocket = iter->second;
			if(pSocket)
			{
				pSocket->InitState();
				if(!pSocket->InitClientTCP(hWnd, WM_TCPSOCKET1+i, rvConnectInfo[i].szIP, rvConnectInfo[i].iPort))
				{
					LOG.PrintTimeAndLog(0, "TCP Socket Failed(%d:%d)", WM_TCPSOCKET1+i, WSAGetLastError());
					return false;
				}
			}
		}
	}

	return true;
}
bool TCPSocketTable::AddLoginServer( HWND hWnd, vSERVERINFO &rvConnectInfo)
{
	m_hWnd = hWnd;
	const int iMax = (int) rvConnectInfo.size();
	for (int i = 0; i < iMax ; i++)
	{
		if(i >= MAX_TCPSOCKET_MAP)
		{
			LOG.PrintTimeAndLog(0, "Error - MAX_TCPSOCKET_MAP");
			return false;
		}

		ioSP2TCPSocket *pSocket = new ioSP2TCPSocket;
		m_TCPSocketMap.insert( TCPSocketMap::value_type( WM_TCPLOGIN1+i, pSocket));
		TCPSocketMap::iterator iter = m_TCPSocketMap.find( WM_TCPLOGIN1+i );
		if( iter != m_TCPSocketMap.end())
		{
			ioSP2TCPSocket *pSocket = iter->second;
			if(pSocket)
			{
				pSocket->InitState();
				if(!pSocket->InitClientTCP(hWnd, WM_TCPLOGIN1+i, rvConnectInfo[i].szIP, rvConnectInfo[i].iPort))
				{
					LOG.PrintTimeAndLog(0, "LoginTCP Socket Failed(%d:%d)", WM_TCPLOGIN1+i, WSAGetLastError());
					return false;
				}
			}
		}
	}

	return true;

}


bool TCPSocketTable::CheckAllClose()
{
	bool bAllClose = true;
	for (TCPSocketMap::iterator iter = m_TCPSocketMap.begin(); iter != m_TCPSocketMap.end(); ++iter)
	{
		ioSP2TCPSocket *pSocket = iter->second;
		if(pSocket)
		{
			if(pSocket->IsConnected())
			{
				pSocket->CloseClientTCP();
				bAllClose = false;
			}
		}
	}

	return bAllClose;
}

void TCPSocketTable::Reconnect(HWND hWnd)
{
	for (TCPSocketMap::iterator iter = m_TCPSocketMap.begin(); iter != m_TCPSocketMap.end(); ++iter)
	{
		ioSP2TCPSocket *pSocket = iter->second;
		if(pSocket)
		{
			if(!pSocket->IsConnected())
			{
				// 제 접속시 init 추가.
				pSocket->InitState();

				pSocket->InitClientTCP(hWnd, pSocket->GetWndMsg(), pSocket->GetConnectedIP(), pSocket->GetConnectedPort());
			}
		}
	}
}

void TCPSocketTable::CloseAndRelease()
{//kyg 여기서 종료감지 로그인서버 아이디인지만 판단해서 send해줌 도ㅓㅣㄹ드,ㅅ 
	for (TCPSocketMap::iterator iter = m_TCPSocketMap.begin(); iter != m_TCPSocketMap.end(); ++iter)
	{
		ioSP2TCPSocket *pSocket = iter->second;
		if(pSocket)
		{
			PostMessage( m_hWnd,WM_DISCONNETED,pSocket->GetWndMsg(),0);
			pSocket->CloseClientTCP();
			delete pSocket;
		}
	}
	m_TCPSocketMap.clear();
}

ioSP2TCPSocket* TCPSocketTable::GetTCPSocket( UINT wmdMsg )
{
	TCPSocketMap::iterator iter = m_TCPSocketMap.find( wmdMsg );

	if(iter != m_TCPSocketMap.end())
	{
		return iter->second;
	}

	return NULL;
}

bool TCPSocketTable::Init()
{
	if( !ioPortalHelp::InitWinSock() )
	{
		LOG.PrintTimeAndLog( 0, "TCPSocketTable::Init - InitSocket Failed" );
		return false;
	}

	return true;
}

void TCPSocketTable::Close()
{
	ioPortalHelp::CloseWinSock();
}

TCPSocketTable & TCPSocketTable::GetInstance()
{
	if( !sg_Instance )
		sg_Instance = new TCPSocketTable;

	return *sg_Instance;
}

void TCPSocketTable::ReleaseInstance()
{
	SAFEDELETE(sg_Instance);
}

bool TCPSocketTable::Send( UINT wmdMsg, CPacket &rkPacket )
{
	ioSP2TCPSocket* pSocket = GetTCPSocket(wmdMsg);

	if(!pSocket)
		return false;
	
	return pSocket->SendLoop(rkPacket);
}

int TCPSocketTable::GetSize()
{
	return (int) m_TCPSocketMap.size();
}

