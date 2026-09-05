#include "stdafx.h"
#include "BonusCashInfo.h"

BonusCashInfo::BonusCashInfo()
{
	Init();
}

BonusCashInfo::~BonusCashInfo()
{
	Destroy();
}

void BonusCashInfo::Init()
{
	m_dwIndex			= 0;
	m_iTotalCash		= 0;
	m_iRemainingAmount	= 0;
	m_iExpirationDate	= 0;
	m_bActive			= TRUE;
}

void BonusCashInfo::Destroy()
{
}

void BonusCashInfo::Create(const DWORD dwIndex, const int iTotalCash, const int iRemainingAmount, DBTIMESTAMP& dts)
{
	m_dwIndex			= dwIndex;				// 테이블인덱스
	m_iTotalCash		= iTotalCash;			// 총 보너스 캐쉬
	m_iRemainingAmount	= iRemainingAmount;		// 보너스 캐쉬 잔액

	CTime cExpired(dts.year, dts.month, dts.day, dts.hour, dts.minute, 0);		
	
	m_iExpirationDate	= cExpired.GetTime();	// 만료기간
}

void BonusCashInfo::Create(const DWORD dwIndex, const int iTotalCash, const int iRemainingAmount, DWORD dwExpirateDate)
{
	m_dwIndex			= dwIndex;
	m_iTotalCash		= iTotalCash;
	m_iRemainingAmount	= iRemainingAmount;
	m_iExpirationDate	= dwExpirateDate;
}

void BonusCashInfo::ApplyCashInfo(SP2Packet& kPacket)
{
	PACKET_GUARD_VOID( kPacket.Read(m_dwIndex) );
	PACKET_GUARD_VOID( kPacket.Read(m_iTotalCash) );
	PACKET_GUARD_VOID( kPacket.Read(m_iRemainingAmount) );
	PACKET_GUARD_VOID( kPacket.Read(m_iExpirationDate) );
	PACKET_GUARD_VOID( kPacket.Read(m_bActive) );
}

void BonusCashInfo::FillCashInfo(SP2Packet& kPacket)
{
	PACKET_GUARD_VOID( kPacket.Write(m_dwIndex) );
	PACKET_GUARD_VOID( kPacket.Write(m_iTotalCash) );
	PACKET_GUARD_VOID( kPacket.Write(m_iRemainingAmount) );
	PACKET_GUARD_VOID( kPacket.Write(m_iExpirationDate) );
	PACKET_GUARD_VOID( kPacket.Write(m_bActive) );
}

int BonusCashInfo::GetRemainingAmount()
{
	return m_iRemainingAmount;			// 사용 가능 보너스 캐쉬
}

__int64 BonusCashInfo::GetExpirationDate()
{
	return m_iExpirationDate;
}
	
void BonusCashInfo::SetFlag(BOOL bVal)
{
	m_bActive	= bVal;
}

BOOL BonusCashInfo::IsAvailable()
{
	if( !m_bActive )
		return FALSE;

	if( IsExpired() )
		return FALSE;

	if( m_iRemainingAmount <= 0 )
		return FALSE;

	return TRUE;
}

BOOL BonusCashInfo::SpendCash(int iVal)
{
	if( IsAvailable() )
		return FALSE;

	m_iRemainingAmount -= iVal;

	return TRUE;
}

BOOL BonusCashInfo::IsExpired()
{
	CTime cCurTime			= CTime::GetCurrentTime();
	CTime cExpirationDate(m_iExpirationDate); 

	if( cCurTime > cExpirationDate )
		return TRUE;

	return FALSE;
}

DWORD BonusCashInfo::GetIndex()
{
	return m_dwIndex;
}

void BonusCashInfo::UpdateRemainingAmount(const int iVal)
{
	m_iRemainingAmount	= iVal;
}

int BonusCashInfo::GetTotalAmount()
{
	return m_iTotalCash;
}
//BOOL BonusCashInfo::IsActive()
//{
//	return m_bActive;
//}