#include "stdafx.h"
#include "Accessory.h"
#include "AccessoryManager.h"

Accessory::Accessory()
{
	Init();
}

Accessory::~Accessory()
{
	Destroy();
}

void Accessory::Init()
{
	m_dwIndex			= 0;
	m_iAccessoryCode	= 0;
	m_iValue1			= 0;
	m_iValue2			= 0;
	m_iPeriodType		= PCPT_TIME;
	m_iWearingClassType	= 0;
	m_iAccessoryValue	= 0;

	m_iComposeCode		= 0;
	m_iComposeValue		= 0;
}

void Accessory::Destroy()
{
	Init();
}

void Accessory::FillMoveData( SP2Packet &rkPacket )
{
	PACKET_GUARD_VOID(rkPacket.Write(m_dwIndex));
	PACKET_GUARD_VOID(rkPacket.Write(m_iAccessoryCode));
	PACKET_GUARD_VOID(rkPacket.Write(m_iWearingClassType))
	PACKET_GUARD_VOID(rkPacket.Write(m_iPeriodType));
	PACKET_GUARD_VOID(rkPacket.Write(m_iValue1));
	PACKET_GUARD_VOID(rkPacket.Write(m_iValue2));
	PACKET_GUARD_VOID(rkPacket.Write(m_iAccessoryValue));
	PACKET_GUARD_VOID(rkPacket.Write(m_iComposeCode));
	PACKET_GUARD_VOID(rkPacket.Write(m_iComposeValue));

	//LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[test][accessory] accessory fillmovedata : [index:%d code:%d value:%d]", m_dwIndex, m_iAccessoryCode, m_iAccessoryValue);
}

void Accessory::ApplyMoveData( SP2Packet &rkPacket )
{
	PACKET_GUARD_VOID(rkPacket.Read(m_dwIndex));
	PACKET_GUARD_VOID(rkPacket.Read(m_iAccessoryCode));
	PACKET_GUARD_VOID(rkPacket.Read(m_iWearingClassType));
	PACKET_GUARD_VOID(rkPacket.Read(m_iPeriodType));
	PACKET_GUARD_VOID(rkPacket.Read(m_iValue1));
	PACKET_GUARD_VOID(rkPacket.Read(m_iValue2));
	PACKET_GUARD_VOID(rkPacket.Read(m_iAccessoryValue));
	PACKET_GUARD_VOID(rkPacket.Read(m_iComposeCode));
	PACKET_GUARD_VOID(rkPacket.Read(m_iComposeValue));

	//LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[test][accessory] accessory applymovedata : [index:%d code:%d value:%d]", m_dwIndex, m_iAccessoryCode, m_iAccessoryValue);
}

void Accessory::GetAccessoryLimitDate(SYSTEMTIME& sysTime)
{
	if( 0 == m_iValue1 )
		return;

	sysTime.wYear = GetYear();
	sysTime.wMonth = GetMonth();
	sysTime.wDay = GetDay();
	sysTime.wHour = GetHour();
	sysTime.wMinute = GetMinute();
}

void Accessory::SetAccessoryValue( int iValue )
{
	m_iAccessoryValue = iValue; 

	bool bSign = g_AccessoryMgr.GetAccessorySign( m_iAccessoryCode );
	int iMaxValue = g_AccessoryMgr.GetAccessoryMaxValue( m_iAccessoryCode );
	if( bSign && iMaxValue > m_iAccessoryValue )
		m_iAccessoryValue = iMaxValue;
	else if( !bSign && iMaxValue < m_iAccessoryValue )
		m_iAccessoryValue = iMaxValue;
}