#include "stdafx.h"
#include "UserTitleInfo.h"

UserTitleInfo::UserTitleInfo()
{
	Init();
}

UserTitleInfo::~UserTitleInfo()
{
	Destroy();
}

void UserTitleInfo::Init()
{
	m_iPremiumSecond	= 0;
	m_eStatus			= TITLE_DISABLE;
}

void UserTitleInfo::Destroy()
{
}

void UserTitleInfo::FillData(SP2Packet &rkPacket)
{
	PACKET_GUARD_VOID( rkPacket.Write(m_dwTitleCode) );
	PACKET_GUARD_VOID( rkPacket.Write(m_iCurValue) );
	PACKET_GUARD_VOID( rkPacket.Write(m_iLevel) );
	PACKET_GUARD_VOID( rkPacket.Write(m_bPrimium) );
	PACKET_GUARD_VOID( rkPacket.Write(m_bEquip) );
	PACKET_GUARD_VOID( rkPacket.Write((int)m_eStatus) );
};


void UserTitleInfo::ApplyData(SP2Packet &rkPacket)
{
	int iStatus	= 0;

	PACKET_GUARD_VOID( rkPacket.Read(m_dwTitleCode) );
	PACKET_GUARD_VOID( rkPacket.Read(m_iCurValue) );
	PACKET_GUARD_VOID( rkPacket.Read(m_iLevel) );
	PACKET_GUARD_VOID( rkPacket.Read(m_bPrimium) );
	PACKET_GUARD_VOID( rkPacket.Read(m_bEquip) );
	PACKET_GUARD_VOID( rkPacket.Read(iStatus) );

	m_eStatus	= static_cast<TitleStatus>(iStatus);
}

void UserTitleInfo::Confirm()
{
	
}

TitleStatus UserTitleInfo::GetTitleStatus()
{
	return m_eStatus;
}

void UserTitleInfo::SetTitleStatus(TitleStatus eStatus)
{
	m_eStatus	= eStatus;
}

BOOL UserTitleInfo::IsActiveTitle()
{
	if( TITLE_DISABLE == GetTitleStatus() )
		return FALSE;

	return TRUE;
}

void UserTitleInfo::ActiveTitle()
{
	SetTitleStatus(TITLE_ACTIVE);
}

void UserTitleInfo::CreateTitle(const DWORD dwCode, const __int64 iValue, const int iLevel, const BOOL bPremium, const BOOL bEquip, const TitleStatus eStatus)
{
	SetTitleStatus(eStatus);
	CreateTitleInfo(dwCode, iValue, iLevel, bPremium, bEquip);
}

BOOL UserTitleInfo::IsNewData()
{
	if( TITLE_NEW == GetTitleStatus() )
		return TRUE;

	return FALSE;
}