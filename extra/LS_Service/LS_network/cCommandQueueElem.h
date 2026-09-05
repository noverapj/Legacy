#pragma once

#include "cBaseQueueElem.h"

class cCommandQueueElem : public cBaseQueueElem
{
public:
	cCommandQueueElem();
	virtual ~cCommandQueueElem();

	void Init();
	void Destroy();

public:
	BOOL CopyData(	const cIocpContext* iocpContext, 
					const uint32 command,
					const uint8* buffer = NULL, 
					const uint32 length = 0 );

	BOOL CopyData(	const uint32 ID, 
					const uint32 command,
					const uint8* buffer = NULL, 
					const uint32 length = 0 );

	void SetContext( cIocpContext* iocpContext )	{ m_context = iocpContext; }

public:
	IIocpContext* GetContext()	{ return m_context;						}
	const uint32 GetID()		{ return m_ID;							}
	const uint32 GetCommand()	{ return m_command;						}
	const uint32 GetLength()	{ return cBaseQueueElem::GetLength();	}	
	const uint8* GetData()		{ return cBaseQueueElem::GetData();		}	

private:
	uint32 m_ID;
	uint32 m_command;
	cIocpContext* m_context;
};