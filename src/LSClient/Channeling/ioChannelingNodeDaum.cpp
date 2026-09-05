#include "stdafx.h"
#include "..\SP2Packet.h"
#include "../ioMyInfo.h"
#include ".\iochannelingnodedaum.h"
#include <strsafe.h>

ioChannelingNodeDaum::ioChannelingNodeDaum(void)
{
}

ioChannelingNodeDaum::~ioChannelingNodeDaum(void)
{
}

ChannelingType ioChannelingNodeDaum::GetType()
{
	return CNT_DAUM;
}

const char * ioChannelingNodeDaum::GetININame()
{
	return "config/sp2_channeling_daum.ini";
}

const char * ioChannelingNodeDaum::GetBillingErrorMent(ioHashString &sServerError)
{
	return "DAUM Billing Error : ";
}
const char * ioChannelingNodeDaum::GetLogoSetName()
{
	return "LogoDefault";
}

bool ioChannelingNodeDaum::OnRecieveGetCash( SP2Packet &rkReceivePacket )
{
	int iCash           = 0;
	int iPurchasedCash  = 0;
	rkReceivePacket >> iCash;
	rkReceivePacket >> iPurchasedCash;

#if defined( USE_GA )
	if( g_App.GetGAFirstCash() == false )
	{
		// GOLD_CHARGE
		int iChargeCash = iPurchasedCash - g_MyInfo.GetPurchasedCash();
		if( iChargeCash > 0 )
		{
			/*
			g_HttpMng.GA_ItemHitTracking( g_MyInfo.GetUserIndex() 
				, "Charge"
				, iChargeCash
				, 1
				, 0
				, "Gold"
				, "%2FGOLD%2FCHARGE"
				, true );
			*/

			// Gold charge page view ·Î ÀüÈ¯
			g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), "%2FGOLD%2FCHARGE", 4, "", iChargeCash );
		}
	}
	else
	{
		g_App.SetGAFirstCash( false );
	}
#endif

	g_MyInfo.SetCash( iCash );
	g_MyInfo.SetPurchasedCash( iPurchasedCash );
	g_MyInfo.SetChannelingCash( 0 );

	//LOG.PrintTimeAndLog( 0 , "Daum Get Cash Info   %d     %d", iCash, iPurchasedCash );

	return true;
}

void ioChannelingNodeDaum::GetFillCashURL( OUT ioHashString &rsFullURL , IN const ioHashString &rsShortURL )
{
	rsFullURL = rsShortURL;
}

void ioChannelingNodeDaum::GetLoginURL( OUT ioHashString &rsFullURL, IN const ioHashString &rsShortURL, IN const ioHashString &rsLoginKey )
{
	char szFullURL[MAX_PATH*2]="";
	StringCbPrintf( szFullURL, sizeof( szFullURL ), rsShortURL.c_str(), rsLoginKey.c_str() );
	rsFullURL = szFullURL;

#ifdef _DEBUG
	LOG.PrintTimeAndLog( 0, "%s | %s | %s |", __FUNCTION__ , szFullURL, rsShortURL.c_str(), rsLoginKey.c_str() );
#endif
}

int ioChannelingNodeDaum::GetCashPageWidth()
{
	return 570;
}

int ioChannelingNodeDaum::GetCashPageHeight()
{
	return 650;
}

void ioChannelingNodeDaum::OnCashPageHide()
{
	SP2Packet kPacket( CTPK_GET_CASH );
	TCPNetwork::SendToServer( kPacket );
	TCPNetwork::MouseBusy( true );
}

bool ioChannelingNodeDaum::IsCreateProcessFillCashWeb()
{
	return false;
}