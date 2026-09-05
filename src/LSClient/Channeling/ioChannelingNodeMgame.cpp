#include "stdafx.h"
#include "..\SP2Packet.h"
#include "../ioMyInfo.h"
#include "../NetworkWrappingFunc.h"
#include ".\iochannelingnodemgame.h"
#include <strsafe.h>

ioChannelingNodeMgame::ioChannelingNodeMgame(void)
{
	
}

ioChannelingNodeMgame::~ioChannelingNodeMgame(void)
{
}

ChannelingType ioChannelingNodeMgame::GetType()
{
	return CNT_MGAME;
}

const char * ioChannelingNodeMgame::GetININame()
{
	return "config/sp2_channeling_mgame.ini";
}

const char * ioChannelingNodeMgame::GetLogoSetName()
{
	return "LogoMgame";
}


bool ioChannelingNodeMgame::OnRecieveGetCash( SP2Packet &rkReceivePacket )
{
	int iCash           = 0;
	int iChannelingCash = 0;
	rkReceivePacket >> iCash >> iChannelingCash;

#if defined( USE_GA )
	if( g_App.GetGAFirstCash() == false )
	{
		// GOLD_CHARGE
		int iChargeCash = iChannelingCash - g_MyInfo.GetPurchasedCash();
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

			// Gold charge page view 로 전환
			g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), "%2FGOLD%2FCHARGE", 4, "", iChargeCash );
		}
	}
	else
	{
		g_App.SetGAFirstCash( false );
	}
#endif

	g_MyInfo.SetCash( iCash );
	g_MyInfo.SetPurchasedCash( iCash ); // 캐쉬가 실재 구매한 캐쉬와 동일하다
	g_MyInfo.SetChannelingCash( iChannelingCash );

	//LOG.PrintTimeAndLog( 0 , "MGame Get Cash Info   %d", iCash );

	return true;
}

void ioChannelingNodeMgame::GetFillCashURL( OUT ioHashString &rsFullURL , IN const ioHashString &rsShortURL )
{
	char szPath[MAX_PATH*2]="";
	
	if( m_szCurrentDirectory.IsEmpty() )
	{
		::GetCurrentDirectory(MAX_PATH*2,szPath);
		m_szCurrentDirectory = szPath;
	}
	else
		StringCbCopy( szPath, sizeof( szPath ), m_szCurrentDirectory.c_str() );

	StringCbCat( szPath, sizeof( szPath ), "\\info\\cd" );

	char szOTP[MAX_PATH]="";
	FILE *pFile = fopen( szPath, "rb" );
	if( !pFile )
	{
		LOG.PrintTimeAndLog( 0, "Fail Mgame OTP : pFile == NULL." );
		return;
	}
	
	fread( szOTP, sizeof( szOTP ), 1, pFile );
	fclose( pFile );

	char szFullURL[MAX_PATH*2]="";
	StringCbPrintf( szFullURL, sizeof( szFullURL ), "%s%%0B%s%%0B%s%%0BMMALL_LOSTSAGA%%0B1%%0BLS,1,%s,MMALL_LOSTSAGA%%0B", rsShortURL.c_str(), g_MyInfo.GetChannelingUserID().c_str(), szOTP, g_MyInfo.GetPublicID().c_str() );
	rsFullURL = szFullURL;
}

void ioChannelingNodeMgame::GetLoginURL( OUT ioHashString &rsFullURL, IN const ioHashString &rsShortURL, IN const ioHashString &rsLoginKey )
{
	char szPath[MAX_PATH*2]="";

	if( m_szCurrentDirectory.IsEmpty() )
	{
		::GetCurrentDirectory(MAX_PATH*2,szPath);
		m_szCurrentDirectory = szPath;
	}
	else
		StringCbCopy( szPath, sizeof( szPath ), m_szCurrentDirectory.c_str() );

	StringCbCat( szPath, sizeof( szPath ), "\\info\\cdl" );

	char szMgameLoginKey[MAX_PATH]="";
	FILE *pFile = fopen( szPath, "rb" ); // channeling data login
	if( !pFile )
	{
		LOG.PrintTimeAndLog( 0, "Fail Mgame Login : pFile == NULL." );
		return;
	}

	fread( szMgameLoginKey, sizeof( szMgameLoginKey ), 1, pFile );
	fclose( pFile );
	//LOG.PrintTimeAndLog( 0, "szMgameLoginKey : %s ", szMgameLoginKey );
	char szFullURL[MAX_PATH*2]="";
	StringCbPrintf( szFullURL, sizeof( szFullURL ), rsShortURL.c_str(), rsLoginKey.c_str(), szMgameLoginKey );
	rsFullURL = szFullURL;

//#ifdef _DEBUG
	LOG.PrintTimeAndLog( 0, "%s | %s | %s | %s |", __FUNCTION__ , szFullURL, rsShortURL.c_str(), rsLoginKey.c_str(), szMgameLoginKey );
//#endif
}

int ioChannelingNodeMgame::GetCashPageWidth()
{
	return 700;
}

int ioChannelingNodeMgame::GetCashPageHeight()
{
	return 420;
}

void ioChannelingNodeMgame::OnCashPageHide()
{
	SP2Packet kPacket( CTPK_GET_CASH );
	TCPNetwork::SendToServer( kPacket );
	TCPNetwork::MouseBusy( true );

	if( !m_szCurrentDirectory.IsEmpty() )
		SetCurrentDirectory( m_szCurrentDirectory.c_str() );
}

bool ioChannelingNodeMgame::IsCreateProcessFillCashWeb()
{
	return false;
}