#include "stdafx.h"
#include ".\iolocalparent.h"
#include "..\NodeInfo\User.h"

ioLocalParent::ioLocalParent(void)
{
}

ioLocalParent::~ioLocalParent(void)
{
}

bool ioLocalParent::IsRightLicense()
{
	SYSTEMTIME st;
	GetLocalTime( &st );
	int iDate = (st.wYear * 10000) + (st.wMonth * 100) + st.wDay;

	if( iDate >= GetLicenseDate() )
		return false;

	return true;
}

bool ioLocalParent::UpdateOutputCash( User *pUser, SP2Packet &rkRecievePacket, int iPayAmt, DWORD dwErrorPacketID, DWORD dwErrorPacketType )
{
	if( !pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s pUser == NULL.", __FUNCTION__ );
		return false;
	}

	int iChannelingType = 0;
	int iTotalCash     = 0;
	int iPurchasedCash = 0;
	rkRecievePacket >> iChannelingType;
	rkRecievePacket >> iTotalCash;
	rkRecievePacket >> iPurchasedCash;
	pUser->SetCash( iTotalCash );
	pUser->SetPurchasedCash( iPurchasedCash );

	return true;
}