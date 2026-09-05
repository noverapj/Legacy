#include "stdafx.h"
#include "Costume.h"

Costume::Costume()
{
	Init();
}

Costume::~Costume()
{
	Destroy();
}

void Costume::Init()
{
	m_dwIndex			= 0;
	m_iCostumeCode		= 0;
	m_iValue1			= 0;
	m_iValue2			= 0;
	m_dwMaleCustom		= 0;
	m_dwFemaleCustom	= 0;
	m_iPeriodType		= PCPT_TIME;
	m_iWearingClassType	= 0;
}

void Costume::Destroy()
{
	Init();
}

void Costume::FillMoveData( SP2Packet &rkPacket )
{
	PACKET_GUARD_VOID(rkPacket.Write(m_dwIndex));
	PACKET_GUARD_VOID(rkPacket.Write(m_iCostumeCode));
	PACKET_GUARD_VOID(rkPacket.Write(m_iWearingClassType))
	PACKET_GUARD_VOID(rkPacket.Write(m_iPeriodType));
	PACKET_GUARD_VOID(rkPacket.Write(m_iValue1));
	PACKET_GUARD_VOID(rkPacket.Write(m_iValue2));
	PACKET_GUARD_VOID(rkPacket.Write(m_dwMaleCustom));
	PACKET_GUARD_VOID(rkPacket.Write(m_dwFemaleCustom));
}

void Costume::ApplyMoveData( SP2Packet &rkPacket )
{
	PACKET_GUARD_VOID(rkPacket.Read(m_dwIndex));
	PACKET_GUARD_VOID(rkPacket.Read(m_iCostumeCode));
	PACKET_GUARD_VOID(rkPacket.Read(m_iWearingClassType));
	PACKET_GUARD_VOID(rkPacket.Read(m_iPeriodType));
	PACKET_GUARD_VOID(rkPacket.Read(m_iValue1));
	PACKET_GUARD_VOID(rkPacket.Read(m_iValue2));
	PACKET_GUARD_VOID(rkPacket.Read(m_dwMaleCustom));
	PACKET_GUARD_VOID(rkPacket.Read(m_dwFemaleCustom));
}

void Costume::GetCostumeLimitDate(SYSTEMTIME& sysTime)
{
	if( 0 == m_iValue1 || 0 == m_iValue2 )
		return;

	sysTime.wYear = GetYear();
	sysTime.wMonth = GetMonth();
	sysTime.wDay = GetDay();
	sysTime.wHour = GetHour();
	sysTime.wMinute = GetMinute();
}