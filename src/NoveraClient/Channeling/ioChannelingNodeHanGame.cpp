#include "stdafx.h"
#include "..\SP2Packet.h"
#include "../ioMyInfo.h"
#include "../NetworkWrappingFunc.h"
#include ".\ioChannelingNodeHanGame.h"
#include <strsafe.h>

ioChannelingNodeHanGame::ioChannelingNodeHanGame(void)
{
	
}

ioChannelingNodeHanGame::~ioChannelingNodeHanGame(void)
{
}

ChannelingType ioChannelingNodeHanGame::GetType()
{
	return CNT_HANGAME;
}

const char * ioChannelingNodeHanGame::GetININame()
{
	return "config/sp2_channeling_hangame.ini";
}

const char * ioChannelingNodeHanGame::GetLogoSetName()
{
	return "LogoDefault";
}


bool ioChannelingNodeHanGame::OnRecieveGetCash( SP2Packet &rkReceivePacket )
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

	return true;
}

void ioChannelingNodeHanGame::GetFillCashURL( OUT ioHashString &rsFullURL , IN const ioHashString &rsShortURL )
{
	rsFullURL = rsShortURL;
}

void ioChannelingNodeHanGame::GetLoginURL( OUT ioHashString &rsFullURL, IN const ioHashString &rsShortURL, IN const ioHashString &rsLoginKey )
{
	char szFullURL[MAX_PATH*2]="";
	StringCbPrintf( szFullURL, sizeof( szFullURL ), rsShortURL.c_str(), rsLoginKey.c_str() );
	rsFullURL = szFullURL;

#ifdef _DEBUG
	LOG.PrintTimeAndLog( 0, "%s | %s | %s |", __FUNCTION__ , szFullURL, rsShortURL.c_str(), rsLoginKey.c_str() );
#endif
}

int ioChannelingNodeHanGame::GetCashPageWidth()
{
	return 0;
}

int ioChannelingNodeHanGame::GetCashPageHeight()
{
	return 0;
}

void ioChannelingNodeHanGame::OnCashPageHide()
{
}

bool ioChannelingNodeHanGame::IsCreateProcessFillCashWeb()
{
	return true;
}