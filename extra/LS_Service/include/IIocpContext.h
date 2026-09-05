#pragma once

class IIocpContext 
{
public:
	IIocpContext()
	{}
	~IIocpContext()		
	{}

public:
	virtual BOOL IsActive() = 0;
	virtual void SetActive( BOOL active ) = 0;
	
	virtual void Disconnect() = 0;
	virtual	uint32 GetEntity() = 0;
	virtual uint32 GetIP() = 0;
};
