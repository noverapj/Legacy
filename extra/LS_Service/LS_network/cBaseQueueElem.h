#pragma once

#include "cBuffer.h"

class cBaseQueueElem  
{
public:
	cBaseQueueElem();
	virtual ~cBaseQueueElem();

	void Init();
	void Destroy();

public:
	BOOL CopyData(const uint8* buffer, const uint32 length);
	
	const uint32 GetLength()	{ return m_buffer.GetLength();	}
	const uint8* GetData()		{ return m_buffer.GetBuffer();	}	

protected:
	cBuffer	m_buffer;
};