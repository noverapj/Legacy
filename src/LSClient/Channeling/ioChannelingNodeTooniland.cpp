#include "stdafx.h"
#include "..\SP2Packet.h"
#include "../ioMyInfo.h"
#include "../NetworkWrappingFunc.h"
#include ".\ioChannelingNodeTooniland.h"
#include <strsafe.h>

ioChannelingNodeTooniland::ioChannelingNodeTooniland(void)
{
	
}

ioChannelingNodeTooniland::~ioChannelingNodeTooniland(void)
{
}

ChannelingType ioChannelingNodeTooniland::GetType()
{
	return CNT_TOONILAND;
}

const char * ioChannelingNodeTooniland::GetININame()
{
	return "config/sp2_channeling_tooniland.ini";
}

const char * ioChannelingNodeTooniland::GetLogoSetName()
{
	return "LogoDefault";
}


bool ioChannelingNodeTooniland::OnRecieveGetCash( SP2Packet &rkReceivePacket )
{
	int iCash           = 0;
	int iPurchasedCash = 0;
	rkReceivePacket >> iCash >> iPurchasedCash;

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

	//LOG.PrintTimeAndLog( 0 , "Tooniland Get Cash Info   %d     %d", iCash, iPurchasedCash );

	return true;
}

void ioChannelingNodeTooniland::GetFillCashURL( OUT ioHashString &rsFullURL , IN const ioHashString &rsShortURL )
{
	rsFullURL = rsShortURL;
}

void ioChannelingNodeTooniland::GetLoginURL( OUT ioHashString &rsFullURL, IN const ioHashString &rsShortURL, IN const ioHashString &rsLoginKey )
{
	char szFullURL[MAX_PATH*2]="";
	StringCbPrintf( szFullURL, sizeof( szFullURL ), rsShortURL.c_str(), rsLoginKey.c_str() );
	rsFullURL = szFullURL;

#ifdef _DEBUG
	LOG.PrintTimeAndLog( 0, "%s | %s | %s |", __FUNCTION__ , szFullURL, rsShortURL.c_str(), rsLoginKey.c_str() );
#endif
}

int ioChannelingNodeTooniland::GetCashPageWidth()
{
	return 378;
}

int ioChannelingNodeTooniland::GetCashPageHeight()
{
	return 482;
}

void ioChannelingNodeTooniland::OnCashPageHide()
{
	SP2Packet kPacket( CTPK_GET_CASH );
	TCPNetwork::SendToServer( kPacket );
	TCPNetwork::MouseBusy( true );
}

bool ioChannelingNodeTooniland::IsCreateProcessFillCashWeb()
{
	return false;
}