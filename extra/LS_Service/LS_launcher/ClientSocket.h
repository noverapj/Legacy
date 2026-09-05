#pragma once

#include <afxsock.h>
#include <string>
#include "cBuffer.h"


const uint32 PACKET_BUFFER = 1024*4;

// CClientSocket 명령 대상입니다.

class CClientSocket : public CAsyncSocket
{
public:
	CClientSocket(HWND hwnd, CString& IP, UINT port);
	virtual ~CClientSocket();

public:
	BOOL Create();

	BOOL Connect();
	BOOL Reconnect();

	BOOL IsConnected()	{ return m_connected; }

protected:
	virtual void OnReceive(int nErrorCode);
	virtual void OnSend(int nErrorCode);
	virtual void OnConnect(int nErrorCode);
	virtual void OnClose(int nErrorCode);

	BOOL Parse(BYTE* buffer, UINT length);
	void Push(UINT command, BYTE* buffer, UINT length);

protected:
	HWND m_hwnd;

	CString m_IP;
	UINT m_port;
	BOOL m_connected;

	BYTE m_receive[PACKET_BUFFER];
	cBuffer m_storage, m_temporage;
};


