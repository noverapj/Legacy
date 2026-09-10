#include "stdafx.h"

#include "ioChannelingNodeHappyTalk.h"
// #include "..\SP2Packet.h"
// #include "../ioMyInfo.h"
// #include "../NetworkWrappingFunc.h"
// #include ".\ioChannelingNodeHappyTalk.h"
// #include <strsafe.h>


ioChannelingNodeHappyTalk::ioChannelingNodeHappyTalk(void)
{
}

ioChannelingNodeHappyTalk::~ioChannelingNodeHappyTalk(void)
{
}

ChannelingType ioChannelingNodeHappyTalk::GetType()
{
	return CNT_HAPPYTALK;
}

const char * ioChannelingNodeHappyTalk::GetININame()
{
	return "config/sp2_channeling_valofe.ini";
}

const char * ioChannelingNodeHappyTalk::GetLogoSetName()
{
	return "LogoDefault";
}

bool ioChannelingNodeHappyTalk::OnRecieveGetCash( SP2Packet &rkReceivePacket )
{
	int iCash          = 0;
	int iPurchasedCash = 0;
	rkReceivePacket >> iCash;
	rkReceivePacket >> iPurchasedCash;
	g_MyInfo.SetCash( iCash );
	g_MyInfo.SetPurchasedCash( iPurchasedCash );
	g_MyInfo.SetChannelingCash( 0 );

	//LOG.PrintTimeAndLog( 0 , "HappyTalk Get Cash Info   %d     %d", iCash, iPurchasedCash );

	return true;
}

void ioChannelingNodeHappyTalk::GetFillCashURL( OUT ioHashString &rsFullURL , IN const ioHashString &rsShortURL )
{
	rsFullURL = rsShortURL;

#ifdef NEXONLOG
	LOG.PrintTimeAndLog( 0, "%s | %s |", __FUNCTION__ , rsFullURL.c_str(), rsShortURL.c_str() );
#endif
}

void ioChannelingNodeHappyTalk::GetLoginURL( OUT ioHashString &rsFullURL, IN const ioHashString &rsShortURL, IN const ioHashString &rsLoginKey )
{
	char szFullURL[MAX_PATH*2]="";
	StringCbPrintf( szFullURL, sizeof( szFullURL ), rsShortURL.c_str(), rsLoginKey.c_str() );
	rsFullURL = szFullURL;

#ifdef NEXONLOG
	LOG.PrintTimeAndLog( 0, "%s | %s | %s |", __FUNCTION__ , szFullURL, rsShortURL.c_str(), rsLoginKey.c_str() );
#endif
}

int ioChannelingNodeHappyTalk::GetCashPageWidth()
{
	return 550; 
}

int ioChannelingNodeHappyTalk::GetCashPageHeight()
{
	return 580;
}

void ioChannelingNodeHappyTalk::OnCashPageHide()
{
	SP2Packet kPacket( CTPK_GET_CASH );
	TCPNetwork::SendToServer( kPacket );
	TCPNetwork::MouseBusy( true );
}

bool ioChannelingNodeHappyTalk::IsCreateProcessFillCashWeb()
{
	return false;
}

const char * ioChannelingNodeHappyTalk::GetBillingErrorMent(ioHashString &sServerError)
{
	if( sServerError == "{\"resultCode\":-1,\"resultValue\":0}" )
		return "DB에러";
	else if( sServerError == "{\"resultCode\":-2,\"resultValue\":0}" )
		return "한도 초과로 구매/선물이 불가합니다.";
	else if( sServerError == "{\"resultCode\":-3,\"resultValue\":0}" )
		return "아이디 없음";
	else if( sServerError == "{\"resultCode\":-4,\"resultValue\":0}" )
		return "내용 불일치";
	else if( sServerError == "{\"resultCode\":-999,\"resultValue\":0}" )
		return "System Excetion";
	else if( sServerError == "{\"resultCode\":-110,\"resultValue\":0}" )
		return "선물 한도금액 초과";
	else if( sServerError == "{\"resultCode\":-108,\"resultValue\":0}" )
		return "14세미만은 선물이 불가합니다.";
	else if( sServerError == "{\"resultCode\":-106,\"resultValue\":0}" )
		return "선물 나이 에러(음수 또는 NULL)";
	else if( sServerError == "{\"resultCode\":-1002,\"resultValue\":0}" )
		return "넥슨캐시 사용이 불가능하도록 설정되어 있습니다.#넥슨포털 내정보의 넥슨캐시 보안설정#메뉴에서 설정을 변경해주세요.";
	else if( sServerError == "{\"resultCode\":-1202,\"resultValue\":0}" )
		return "선물한도 체크실패";
	else if( sServerError == "{\"resultCode\":-1100,\"resultValue\":0}" )
		return "사용자 정보조회 중 오류";
	else if( sServerError == "{\"resultCode\":-1104,\"resultValue\":0}" )
		return "존재하지 않는 사용자";	
	else if( sServerError == "{\"resultCode\":-1000,\"resultValue\":0}" )
		return "한도 초과로 구매/선물이 불가합니다.";
	else if( sServerError == "{\"resultCode\":-9900,\"resultValue\":0}" )
		return "정의되지 않은 오류";
	else if( sServerError == "{\"resultCode\":-9999,\"resultValue\":0}" )
		return "차감 성공/실패";

	return "Billing Error : ";
}

bool ioChannelingNodeHappyTalk::OnRecieveAddCash( SP2Packet &rkReceivePacket )
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
	g_MyInfo.SetPurchasedCash( iPurchasedCash );
	return true;
}