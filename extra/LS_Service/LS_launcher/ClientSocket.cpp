// ClientSocket.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "LS_launcher.h"
#include "ClientSocket.h"
#include "cHeader.h"
#include "WinMessage.h"




// CClientSocket

CClientSocket::CClientSocket(HWND hwnd, CString& IP, UINT port) : m_connected(FALSE)
{
	m_hwnd	= hwnd;
	m_IP	= IP;
	m_port	= port;

	m_storage.Create(PACKET_BUFFER*3);
	m_temporage.Create(PACKET_BUFFER*3);
}

CClientSocket::~CClientSocket()
{
}


// CClientSocket 멤버 함수
void CClientSocket::OnReceive(int nErrorCode)
{
	//TRACE("OnReceive\n");
	if(0 == nErrorCode)
	{
		int bytes = Receive(m_receive, sizeof(m_receive));
		if(0 != bytes)
		{
			m_temporage.Copy(m_storage.GetBuffer(), m_storage.GetLength());
			m_temporage.Append(m_receive, bytes);
		}

		// 패킷 파싱
		if(Parse(m_temporage.GetBuffer(), m_temporage.GetLength()))
		{
			m_storage.Erase();
		}
		m_temporage.Erase();
	}
}

BOOL CClientSocket::Parse(BYTE* buffer, UINT length)
{
	// 패킷의 헤더길이 검사
	if(length < sizeof(cHeader))	
	{
		// 헤더보다 작은 패킷이 들어왔을 경우 저장하고 끝난다
		m_storage.Copy(buffer, length);
		return FALSE;
	}

	// 헤더로 페이로드 길이 확인
	cHeader* header = reinterpret_cast<cHeader*>(buffer);
	//if(!header->IsPerfect())
	//{
	//	// 잘못된 헤더가 들어왔을때 패킷을 drop한다
	//	TRACE("[%lu]invalid packet : %lu\n", GetCurrentThreadId());
	//	return FALSE;
	//}

	if(header->GetPayload() > PACKET_BUFFER)
	{
		TRACE("[%lu]too large payload : %lu\n", GetCurrentThreadId(), header->GetPayload());
		return TRUE;
	}

	// 패킷 헤더에 따른 길이 검사
	UINT packetLength = sizeof(cHeader) + header->GetPayload();
	if(length < packetLength) // 패킷길이가 잘려서 들어왔다
	{
		TRACE("[%lu]short than payload : %lu\n", GetCurrentThreadId(), packetLength);
		m_storage.Copy(buffer, length);
		return FALSE;
	}
	else if(length == packetLength) // 패킷길이가 정확하게 들어왔다
	{
		Push(	
			header->GetCommand(),
			buffer+sizeof(cHeader), 
			packetLength);
		return TRUE;
	}
	else if(length > packetLength)	// 패킷길이가 붙어서 들어왔다
	{
		Push(	
			header->GetCommand(),
			buffer+sizeof(cHeader), 
			packetLength);

			// 다시 한번 패킷 검사
		return Parse(buffer+packetLength, length-packetLength);
	}
	return TRUE;
}

void CClientSocket::Push(UINT nCommand, BYTE* buffer, UINT length)
{
	SendMessage(m_hwnd, IDC_RECEIVE, (WPARAM)nCommand, (LPARAM)buffer);
}

void CClientSocket::OnSend(int nErrorCode)
{
	//TRACE("OnSend\n");
}

void CClientSocket::OnConnect(int nErrorCode)
{
	//TRACE("OnConnect\n");
	
	if(0 == nErrorCode)
	{
		m_connected = TRUE;
		SendMessage(m_hwnd, IDC_CONNECT, (WPARAM)0, (LPARAM)0);
	}
	else
	{
		SendMessage(m_hwnd, IDC_DISCONNECT, (WPARAM)0, (LPARAM)0);
	}
}

void CClientSocket::OnClose(int nErrorCode)
{
	//TRACE("OnClose\n");
	m_connected = FALSE;
	CAsyncSocket::Close();
	CAsyncSocket::Create(0, SOCK_STREAM, FD_READ | FD_WRITE | FD_CONNECT | FD_CLOSE);

	SendMessage(m_hwnd, IDC_DISCONNECT, (WPARAM)0, (LPARAM)0);
}

BOOL CClientSocket::Create()
{
	CAsyncSocket::Close();
	return CAsyncSocket::Create(0, SOCK_STREAM, FD_READ | FD_WRITE | FD_CONNECT | FD_CLOSE);
}

BOOL CClientSocket::Connect()
{
	if(m_connected)
		return TRUE;
	
	return CAsyncSocket::Connect(m_IP, m_port);
}

BOOL CClientSocket::Reconnect()
{
	if(!Create())
	{
		return FALSE;
	}

	return Connect();
}

