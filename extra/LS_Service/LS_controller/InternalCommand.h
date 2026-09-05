#pragma once

#include "cCommand.h"

// 서버에 접속됐을때 자동으로 호출되는 명령
class IConnect : public cCommand
{
protected:
	virtual void Self()
	{
		Debug(_T("I_CONNECT\n"));
	}
	virtual void Run(IIocpContext* context);
};

// 서버와 연결이 종료되었을때 자동으로 호출되는 명령
class IClose : public cCommand
{
protected:
	virtual void Self()
	{
		Debug(_T("I_CLOSE\n"));
	}
	virtual void Run(IIocpContext* context);
};

// Ping을 처리해야될 타이머 메세지
class IPing : public cCommand
{
public:
	IPing() : m_sequence(0)
	{}

protected:
	virtual void Self()
	{
		Debug(_T("I_PING\n"));
	}
	virtual void Run(const uint32 serverId);

protected:
	uint32 m_sequence;
};

// send buffer flush
class IFlush : public cCommand
{
public:
	IFlush()
	{}

protected:
	virtual void Self()
	{
		Debug(_T("I_FLUSH\n"));
	}
	virtual void Run(const uint32 serverId);
};

// 접속한 서버에서 주는 신호
class SPing : public cCommand
{
protected:
	virtual void Self()
	{
		Debug(_T("S_PING\n"));
	}
};








