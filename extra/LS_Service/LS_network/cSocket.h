#pragma once

#include "cIocpOv.h"
#include "cDoubleOV.h"

class cSocket : public cDoubleOV
{
public:
	cSocket(void);
	~cSocket(void);

	void Init();
	void Destroy();

public:
	virtual void Disconnect();

public:
	BOOL CreateSocket();
	void CloseSocket();

	void Reset();
	BOOL Flush();
	BOOL SendRequest( uint8* buffer, uint32 length );
	BOOL ReceiveRequest();
	void SendCompleted( cIocpOv* iocpOv );

	void GetPeerAddress();

public:
	void CleanOverlapped( E_IO_OPERATION iocpOP );
	LPWSAOVERLAPPED	GetOverlapped( E_IO_OPERATION iocpOP );

	SOCKET GetSockHandle()	{ return m_socket; }
	uint8* GetWsaBuffer()	{ return (reinterpret_cast<uint8*>(m_olReceive.m_wsaBuf.buf)); }
	
protected:
	BOOL SendRequest( cIocpOv* sendOvl );
	
protected:
	SOCKET		m_socket;
	cIocpOv		m_olReceive;
	cOverlapped	m_olAccept;
	cOverlapped	m_olDisconnect;

	uint32		m_IP;
};

