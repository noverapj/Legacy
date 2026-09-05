#pragma once

// Operation 분류
typedef enum E_IO_OPERATION
{
	E_IO_NONE = 0,
	E_IO_ACCEPT,
	E_IO_RECEIVE,
	E_IO_SEND,
	E_IO_DISCONNECT
};

// Overlapped구조체와 Operation설정
class cOverlapped
{
public:
	cOverlapped(void)	{ Clean(); }

public:	
	void Clean()		{ ZeroMemory( &m_overlapped, sizeof(WSAOVERLAPPED) ); }

	void SetOperation( E_IO_OPERATION operation )	{ m_operation = operation; }
	uint8 GetOperation()							{ return m_operation; }

public:
	WSAOVERLAPPED m_overlapped;
	uint8 m_operation;
};
