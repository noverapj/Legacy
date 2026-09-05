#pragma once

class IIocpContext;

class cCommand
{
public:
	cCommand(void)	{}
	~cCommand(void)	{}

public:
	BOOL Run(IIocpContext* context, const uint8* buffer, const uint32 payload);
	BOOL Run(const uint32 serverId, const uint8* buffer, const uint32 payload);

protected:
	virtual BOOL Verify(IIocpContext* pContext, uint8* buffer, const uint32 length)	{	return TRUE;	}
	virtual BOOL Verify(uint8* buffer, const uint32 length)							{	return TRUE;	}
	
	virtual void Self()								{}
	virtual void Run(IIocpContext* context)			{}
	virtual void Run(const uint32 serverId)			{}
};
