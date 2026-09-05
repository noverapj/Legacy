// cBaseQueueElem.cpp: implementation of the cBaseQueueElem class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "cSystem.h"
#include "cBaseQueueElem.h"
#include "../include/cSingleton.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

cBaseQueueElem::cBaseQueueElem()
{
	Init();
}

cBaseQueueElem::~cBaseQueueElem()
{
	Destroy();
}

void cBaseQueueElem::Init()
{
	m_buffer.Create(cSingleton<cSystem>::GetInstance()->GetPacket());
}

void cBaseQueueElem::Destroy()
{
}

//////////////////////////////////////////////////////////////////////
// operation
//////////////////////////////////////////////////////////////////////

BOOL cBaseQueueElem::CopyData(const uint8* buffer, const uint32 length)
{
	if(!m_buffer.Copy(buffer, length))
	{
		m_buffer.Create(length);
		m_buffer.Copy(buffer, length);
	}
	return TRUE;
}
