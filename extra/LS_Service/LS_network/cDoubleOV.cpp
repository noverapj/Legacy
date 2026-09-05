#include "StdAfx.h"
#include "cSystem.h"
#include "cDoubleOV.h"
#include "../include/cSingleton.h"

enum WSASendingStates
{
	WSA_SENDING = 1,
	WSA_WAITING
};
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

cDoubleOV::cDoubleOV(void) : m_iterator(0), m_sending(WSA_WAITING)
{
	Init();
}

cDoubleOV::~cDoubleOV(void)
{
	Destroy();
}

void cDoubleOV::Init()
{
	uint32 length = cSingleton<cSystem>::GetInstance()->GetPacket() * 4;
	m_iocpOV[0].Alloc( length );
	m_iocpOV[1].Alloc( length );
}

void cDoubleOV::Destroy()
{
}

//////////////////////////////////////////////////////////////////////
// operation
//////////////////////////////////////////////////////////////////////

void cDoubleOV::Reset()
{
	m_iterator = 0;
	m_iocpOV[0].Clean();
	m_iocpOV[1].Clean();
}

uint32 cDoubleOV::GetDataLength()
{
	return m_iocpOV[GetIterator()].GetDataLength();
}

uint32 cDoubleOV::GetBufferLength()
{
	return m_iocpOV[GetIterator()].GetBufferLength();
}

BOOL cDoubleOV::IsSending()
{
	return (WSA_SENDING == m_sending) ? TRUE : FALSE;
}

void cDoubleOV::Alloc(const uint32 bufferLength)
{
	m_iocpOV[GetIterator()].Alloc(bufferLength);
}

void cDoubleOV::CopyBuffer(const uint8* buffer, const uint32 bufferLength)
{
	m_iocpOV[GetIterator()].CopyBuffer(buffer, bufferLength);
}

BOOL cDoubleOV::AppendBuffer(const uint8* buffer, const uint32 bufferLength)
{
	uint32 it = GetIterator();

	m_iocpOV[it].AppendBuffer(buffer, bufferLength);
	if(m_iocpOV[it].GetDataLength() >= cSingleton<cSystem>::GetInstance()->GetPacket())
	{
		// 보낼 때가 됨
		return FALSE;
	}
	return TRUE;
}

BOOL cDoubleOV::Appendable(const uint32 bufferLength)
{
	uint32 it = GetIterator();

	uint32 dataLength = m_iocpOV[it].GetDataLength();
	uint32 maxLength = m_iocpOV[it].GetBufferLength();
	if((dataLength+bufferLength) >= maxLength)
	{
		// 패킷이 오버됨
		return FALSE;
	}
	return TRUE;
}

cIocpOv* cDoubleOV::Pop()
{
	if(WSA_WAITING == m_sending)
	{
		cIocpOv* iocpOvl = &(m_iocpOV[m_iterator]);
		if(iocpOvl->GetDataLength() > 0)
		{
			if(0 == m_iterator)
			{
				InterlockedExchange(&m_iterator, 1);
			}
			else 
			{
				InterlockedExchange(&m_iterator, 0);
			}

			InterlockedCompareExchange(&m_sending, WSA_SENDING, WSA_WAITING);
			return iocpOvl;
		}
	}
	return NULL;
}

void cDoubleOV::Push(cIocpOv* iocpOvl)
{
	iocpOvl->Clean();

	InterlockedCompareExchange(&m_sending, WSA_WAITING, WSA_SENDING);
}

uint32 cDoubleOV::GetIterator()
{
	return m_iterator;
}
