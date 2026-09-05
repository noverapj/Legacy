#include "stdafx.h"
#include "..\SP2Packet.h"
#include "../ioMyInfo.h"
#include "../NetworkWrappingFunc.h"
#include ".\ioChannelingNodeWemadeBuy.h"
#include <strsafe.h>


ioChannelingNodeWemadeBuy::ioChannelingNodeWemadeBuy(void)
{
}

ioChannelingNodeWemadeBuy::~ioChannelingNodeWemadeBuy(void)
{
}

ChannelingType ioChannelingNodeWemadeBuy::GetType()
{
	return CNT_WEMADEBUY;
}

const char * ioChannelingNodeWemadeBuy::GetININame()
{
	return "config/sp2_channeling_wemade.ini";
}

const char * ioChannelingNodeWemadeBuy::GetLogoSetName()
{
	return "LogoDefault";
}

bool ioChannelingNodeWemadeBuy::OnRecieveGetCash( SP2Packet &rkReceivePacket )
{
	int iCash          = 0;
	int iPurchasedCash = 0;
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

	//LOG.PrintTimeAndLog( 0 , "Wemade Get Cash Info   %d     %d", iCash, iPurchasedCash );
	return true;
}

void ioChannelingNodeWemadeBuy::GetFillCashURL( OUT ioHashString &rsFullURL , IN const ioHashString &rsShortURL )
{
	rsFullURL = rsShortURL;
}

void ioChannelingNodeWemadeBuy::GetLoginURL( OUT ioHashString &rsFullURL, IN const ioHashString &rsShortURL, IN const ioHashString &rsLoginKey )
{
	char szFullURL[MAX_PATH*2]="";
	StringCbPrintf( szFullURL, sizeof( szFullURL ), rsShortURL.c_str(), rsLoginKey.c_str() );
	rsFullURL = szFullURL;

#ifdef _DEBUG
	LOG.PrintTimeAndLog( 0, "%s | %s | %s |", __FUNCTION__ , szFullURL, rsShortURL.c_str(), rsLoginKey.c_str() );
#endif
}

int ioChannelingNodeWemadeBuy::GetCashPageWidth()
{
	return 550; 
}

int ioChannelingNodeWemadeBuy::GetCashPageHeight()
{
	return 580;
}

void ioChannelingNodeWemadeBuy::OnCashPageHide()
{
	SP2Packet kPacket( CTPK_GET_CASH );
	TCPNetwork::SendToServer( kPacket );
	TCPNetwork::MouseBusy( true );
}

bool ioChannelingNodeWemadeBuy::IsCreateProcessFillCashWeb()
{
	return false;
}

const char * ioChannelingNodeWemadeBuy::GetBillingErrorMent(ioHashString &sServerError)
{
	return "Wemade Billing Error : ";
}

bool ioChannelingNodeWemadeBuy::OnRecieveAddCash( SP2Packet &rkReceivePacket )
{
	int iCash          = 0;
	int iPurchasedCash = 0;
	rkReceivePacket >> iCash;
	rkReceivePacket >> iPurchasedCash;
	g_MyInfo.SetCash( iCash );
	g_MyInfo.SetPurchasedCash( iPurchasedCash );
	return true;
}