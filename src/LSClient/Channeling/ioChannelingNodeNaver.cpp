#include "stdafx.h"
#include "..\SP2Packet.h"
#include "../ioMyInfo.h"
#include "../NetworkWrappingFunc.h"
#include ".\ioChannelingNodeNaver.h"
#include <strsafe.h>

ioChannelingNodeNaver::ioChannelingNodeNaver(void)
{
	
}

ioChannelingNodeNaver::~ioChannelingNodeNaver(void)
{
}

ChannelingType ioChannelingNodeNaver::GetType()
{
	return CNT_NAVER;
}

const char * ioChannelingNodeNaver::GetININame()
{
	return "config/sp2_channeling_naver.ini";
}

const char * ioChannelingNodeNaver::GetLogoSetName()
{
	return "LogoDefault";
}


bool ioChannelingNodeNaver::OnRecieveGetCash( SP2Packet &rkReceivePacket )
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

	//LOG.PrintTimeAndLog( 0 , "Naver Get Cash Info   %d     %d", iCash, iPurchasedCash );

	return true;
}

void ioChannelingNodeNaver::GetFillCashURL( OUT ioHashString &rsFullURL , IN const ioHashString &rsShortURL )
{
	rsFullURL = rsShortURL;
}

void ioChannelingNodeNaver::GetLoginURL( OUT ioHashString &rsFullURL, IN const ioHashString &rsShortURL, IN const ioHashString &rsLoginKey )
{
	char szFullURL[MAX_PATH*2]="";
	StringCbPrintf( szFullURL, sizeof( szFullURL ), rsShortURL.c_str(), rsLoginKey.c_str() );
	rsFullURL = szFullURL;

#ifdef _DEBUG
	LOG.PrintTimeAndLog( 0, "%s | %s | %s |", __FUNCTION__ , szFullURL, rsShortURL.c_str(), rsLoginKey.c_str() );
#endif
}

int ioChannelingNodeNaver::GetCashPageWidth()
{
	return 0;
}

int ioChannelingNodeNaver::GetCashPageHeight()
{
	return 0;
}

void ioChannelingNodeNaver::OnCashPageHide()
{
}

bool ioChannelingNodeNaver::IsCreateProcessFillCashWeb()
{
	return true;
}