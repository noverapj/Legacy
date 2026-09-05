#pragma once

#include "cBaseQueueElem.h"

class cConnectorQueueElem : public cBaseQueueElem
{
public:
	cConnectorQueueElem();
	virtual ~cConnectorQueueElem();

	void Init();
	void Destroy();

public:
	BOOL CopyData(	const uint32 ID,
					const uint32 command,
					const uint8* buffer = NULL, 
					const uint32 length = 0 );

	uint32 GetID()		{ return m_ID; }
	uint32 GetCommand()	{ return m_command; }

private:
	uint32 m_ID;
	uint32 m_command;
};