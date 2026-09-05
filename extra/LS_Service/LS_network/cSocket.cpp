#include "StdAfx.h"
#include "cSystem.h"
#include "cSocket.h"
#include "../include/cSingleton.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

cSocket::cSocket(void) : m_socket(INVALID_SOCKET), m_IP(0)
{
	Init();
}

cSocket::~cSocket(void)
{
	Destroy();
}

void cSocket::Init()
{
	m_olReceive.Alloc(cSingleton<cSystem>::GetInstance()->GetPacket());
	m_olReceive.SetOperation(E_IO_RECEIVE);
	m_olAccept.SetOperation(E_IO_ACCEPT);
	m_olDisconnect.SetOperation(E_IO_DISCONNECT);

	CreateSocket();
}

void cSocket::Destroy()
{
	CloseSocket();

	m_olReceive.Destroy();	
}

//////////////////////////////////////////////////////////////////////
// Operation 
//////////////////////////////////////////////////////////////////////

BOOL cSocket::CreateSocket()
{
	CloseSocket();

	m_socket = WSASocket(	AF_INET, 
							SOCK_STREAM, 
						    IPPROTO_TCP,
							NULL,
							NULL,
							WSA_FLAG_OVERLAPPED);
	if(m_socket == INVALID_SOCKET)
	{
		return FALSE;
	}


	BOOL sockOpt = TRUE;

	if(setsockopt(m_socket, 
					IPPROTO_TCP, 
					TCP_NODELAY, 
					reinterpret_cast<char*>(&sockOpt), 
					sizeof(sockOpt)) == SOCKET_ERROR) 
	{
		CloseSocket();
		return FALSE;
	}

	int32 sockBuffer = 0;
	if(setsockopt(	m_socket, 
					SOL_SOCKET, 
					SO_SNDBUF, 
					reinterpret_cast<char*>(&sockBuffer), 
					sizeof(sockBuffer)) == SOCKET_ERROR) 
	{
		CloseSocket();
		return FALSE;
	}

	//LINGER  lingerStruct;
	//lingerStruct.l_onoff	= 0;
	//lingerStruct.l_linger	= 0;	// TIME_WAIT : 0 sec

	//setsockopt(m_socket, 
	//			SOL_SOCKET, 
	//			SO_LINGER,
	//			reinterpret_cast<char*>(&lingerStruct), 
	//			sizeof(lingerStruct));

	/*
	if(setsockopt(	m_socket, 
					SOL_SOCKET, 
					SO_RCVBUF, 
					reinterpret_cast<char*>(&sockBuffer), 
					sizeof(sockBuffer)) == SOCKET_ERROR) 
	{
		CloseSocket();
		return FALSE;
	}
	*/

	return TRUE;
}


void cSocket::CloseSocket()
{
	if(m_socket != INVALID_SOCKET)
	{
		shutdown(m_socket, SD_BOTH);
		closesocket(m_socket);

		m_socket = INVALID_SOCKET;
	}
}

void cSocket::Disconnect()
{
	CleanOverlapped(E_IO_DISCONNECT);

	TransmitFile(	m_socket, 
					NULL, 
					0, 
					0, 
					&(m_olDisconnect.m_overlapped), 
					NULL,  
					TF_DISCONNECT | TF_REUSE_SOCKET);
}

BOOL cSocket::SendRequest(cIocpOv* sendOvl)
{
	DWORD dwBytes=0, flags=0;
	if(SOCKET_ERROR == WSASend(	
		m_socket,
		&(sendOvl->m_wsaBuf),
		1,
		&dwBytes,
		flags,
		&(sendOvl->m_overlapped),
		NULL))
	{
		DWORD error = WSAGetLastError();
		if(error != WSA_IO_PENDING)
		{
			Debug(_T("LS system :: send error(%lu)\n"), error);
			SendCompleted(sendOvl);
			return FALSE;
		}
	}
	return TRUE;
}

void cSocket::Reset()
{
	GetPeerAddress();
	cDoubleOV::Reset();
}

BOOL cSocket::Flush()
{
	if(FALSE == cDoubleOV::IsSending())
	{
		// 메모리풀에서 사용할 버퍼를 꺼낸다
		cIocpOv* sendOvl = cDoubleOV::Pop();
		if(!sendOvl) return TRUE;

		//Debug("LS system :: flush(%lu)\n", sendOvl->GetDataLength());
		sendOvl->SetOperation(E_IO_SEND);
		return SendRequest(sendOvl);
	}
	return FALSE;
}

BOOL cSocket::SendRequest(uint8* buffer, uint32 length)
{
	if(FALSE == cDoubleOV::Appendable(length))
	{
		if(FALSE == Flush())
		{
			return FALSE;
		}
	}

	if(FALSE == cDoubleOV::AppendBuffer(buffer, length))
	{
		Flush();
	}
	return TRUE;
}

BOOL cSocket::ReceiveRequest()
{
	DWORD flags=0, recvBytes=0;
		
	CleanOverlapped(E_IO_RECEIVE);

	int32 iRet = WSARecv(m_socket,
						static_cast<LPWSABUF>(&m_olReceive.m_wsaBuf),
						1,
						&recvBytes,
						&flags,
						(LPWSAOVERLAPPED)GetOverlapped(E_IO_RECEIVE),
						NULL);
	if(iRet == SOCKET_ERROR)
	{
		if(WSAGetLastError() != WSA_IO_PENDING)
		{
			Debug(_T("LS system :: receive error\n"));
			return FALSE;
		}
	}
	return TRUE;
}

void cSocket::CleanOverlapped(E_IO_OPERATION iocpOP)
{
	switch(iocpOP)
	{
	case E_IO_RECEIVE :
		m_olReceive.Clean();
		break;

	case E_IO_ACCEPT :
		m_olAccept.Clean();
		break;

	case E_IO_DISCONNECT :
		m_olDisconnect.Clean();
		break;
	}
}

LPWSAOVERLAPPED	cSocket::GetOverlapped(E_IO_OPERATION iocpOP)
{
	switch(iocpOP)
	{
	case E_IO_RECEIVE :
		return &(m_olReceive.m_overlapped);
		break;

	case E_IO_ACCEPT :
		return &(m_olAccept.m_overlapped);
		break;

	case E_IO_DISCONNECT :
		return &(m_olDisconnect.m_overlapped);
		break;
	}
	return NULL;
}

void cSocket::SendCompleted(cIocpOv* sendOvl)
{	
	sendOvl->Clean();
	cDoubleOV::Push(sendOvl);
}

void cSocket::GetPeerAddress()
{
	SOCKADDR_IN *localAddr, *remoteAddr;
	int localLen = 0, remoteLen = 0;

	GetAcceptExSockaddrs(GetWsaBuffer(), 
		0, 
		sizeof(SOCKADDR_IN) + 16,
		sizeof(SOCKADDR_IN) + 16, 
		(LPSOCKADDR*)&localAddr, 
		&localLen, 
		(LPSOCKADDR*)&remoteAddr, 
		&remoteLen); 

	m_IP = remoteAddr->sin_addr.S_un.S_addr;
}