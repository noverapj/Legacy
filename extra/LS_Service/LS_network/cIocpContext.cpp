// cIocpContext.cpp: implementation of the cIocpContext class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "cSystem.h"
#include "cIocpContext.h"
#include "../include/cSingleton.h"


uint32 cIocpContext::ENTITY = 0;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

cIocpContext::cIocpContext() : m_active(FALSE), m_associated(FALSE), m_portId(0), m_sequence(1)
{
	Init();
}

cIocpContext::~cIocpContext()
{
	Destroy();
}

void cIocpContext::Init()
{
	SetEntity(++ENTITY);
	CreateStorage(cSingleton<cSystem>::GetInstance()->GetPacket() * 2);
}

void cIocpContext::Destroy()
{
}

void cIocpContext::Reset()
{
	SetActive(TRUE);
	SetSequence(1);
	cStorage::CleanStorage();
	cSocket::Reset();
}

void cIocpContext::Disconnect()
{
	if(IsActive())
	{
		SetActive(FALSE);
		cSocket::Disconnect();
	}
}