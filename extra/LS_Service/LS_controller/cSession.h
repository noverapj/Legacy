#pragma once

#include "cBuffer.h"
#include "../common/Protocol.h"
#include "../include/BoostPooler.h"

class IIocpContext;

// 세션타입
enum SessionTypes
{
	SESSION_NORMAL = 0,
	SESSION_LOGIN,
	SESSION_PLAYER,
	SESSION_SERVER,
	SESSION_OPERATOR,
};

// 세션인터페이스
class ISession
{
public:
	ISession(void) : m_sessionType(SESSION_NORMAL), m_context(NULL)
	{}

public:
	void SetType(const SessionTypes type)	{ m_sessionType = type; }
	void SetContext(IIocpContext* context)	{ m_context = context; }

	SessionTypes GetType()					{ return m_sessionType;	}
	IIocpContext* GetContext()				{ return m_context; }

public:
	BOOL IsNormal()		{ return (SESSION_NORMAL == m_sessionType) ? TRUE : FALSE;	}
	BOOL IsWaiter()		{ return (SESSION_LOGIN == m_sessionType) ? TRUE : FALSE;	}
	BOOL IsPlayer()		{ return (SESSION_PLAYER == m_sessionType) ? TRUE : FALSE;	}
	BOOL IsServer()		{ return (SESSION_SERVER == m_sessionType) ? TRUE : FALSE;	}

public:
	virtual void OnConnect() {};
	virtual void OnDisconnect() {};

protected:
	SessionTypes m_sessionType;
	IIocpContext* m_context;
};

// 기본세션
class cSession : public ISession, public BoostPooler<cSession>
{
public:
	cSession(void) {}

public:
	virtual void OnConnect();
	virtual void OnDisconnect();

public:
	BOOL TransferBegin(const TCHAR* file, const uint32 length);
	BOOL TransferDo(const uint8* buffer, const uint32 length);
	void TransferWrite();
	void TransferEnd();
	BOOL DoingTransfer()				{ return m_transfer; }

	const TCHAR* GetFile()				{ return m_file; }
	const uint8* GetBuffer()			{ return m_buffer.GetBuffer(); }
	const uint32 GetLength()			{ return m_buffer.GetLength(); }
	const uint32 GetTransferLength()	{ return m_transferLength; }

public:
	BOOL m_transfer;
	TCHAR m_file[512];
	uint32 m_transferLength;
	cBuffer m_buffer;
};
