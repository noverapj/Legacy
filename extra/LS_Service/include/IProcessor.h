#pragma once


#include "../include/IIocpContext.h"


class IProcessor
{
public:
	IProcessor(void) : m_count(NULL) {}
	virtual ~IProcessor(void) {}

	void SetCountPointer( volatile LONG& count ) { m_count = &count; }

public:
	virtual void Process( IIocpContext* context, const uint32 command, const uint8* buffer, const uint32 length ) = 0;
	virtual void Process( const uint32 serverId, const uint32 command, const uint8* buffer, const uint32 length ) = 0;

protected:
	volatile LONG* m_count; 
};
