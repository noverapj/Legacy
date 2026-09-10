#include "stdafx.h"
#include ".\iolocalChina.h"
#include "../resource.h"
#include "../ioMyInfo.h"
#include "../ioApplication.h"
#include "ioLocalChinaLanguage.h"

ioLocalChina::ioLocalChina(void)
{
}

ioLocalChina::~ioLocalChina(void)
{
}

ioLocalManager::LocalType ioLocalChina::GetType()
{
	return ioLocalManager::LCT_THAILAND;
}

const char * ioLocalChina::GetTextListFileName()
{
	return "text.txt";
}

const char * ioLocalChina::GetMemTextList()
{
	return ChinaLanguage::GetMemTextList(); 
}

DWORD ioLocalChina::GetResourceIDTitleCashBmp()
{
	return IDB_WEB_TITLE_CASH;
}

DWORD ioLocalChina::GetResourceIDTitleEntryBmp()
{
	return IDB_WEB_TITLE_ENTRY;
}

DWORD ioLocalChina::GetResourceIDTitleGuildBmp()
{
	return IDB_WEB_TITLE_GUILD;
}

DWORD ioLocalChina::GetResourceIDTitleGuildMarkBmp()
{
	return IDB_WEB_TITLE_GUILD_MARK;
}

DWORD ioLocalChina::GetResourceIDTitleHomeBmp()
{
	return IDB_WEB_TITLE_HOME;
}

DWORD ioLocalChina::GetResourceIDTitleBmp()
{
	return IDB_WINDOW_TITLE_LOSA;
}

void ioLocalChina::SetLoginData( const ioHashString &szBillingUserKey )
{

}

void ioLocalChina::FillLoginData( OUT SP2Packet &rkPacket )
{

}

void ioLocalChina::SendWindowMsg( int iMsgType )
{

}

void ioLocalChina::GetClassTypeForTutorialDropItem( int &riClassType )
{

}


int ioLocalChina::GetFirstIDMaxSize()
{
	return 12;
}

bool ioLocalChina::IsShowFirstIDWnd()
{
	return false;
}

void ioLocalChina::OpenFillCashPage()
{
	if( g_MyInfo.IsEntryFormality() )
		g_App.OpenFillCashPage();
	else
		g_App.OpenCashEntryPage();
}

const char * ioLocalChina::GetGuildMasterPostion()
{
	return GUILD_MASTER_POSTION_CH;
}

const char * ioLocalChina::GetGuildSecondMasterPosition()
{
	return GUILD_SECOND_MASTER_POSTION_CH;
}

const char * ioLocalChina::GetGuildGeneralPosition()
{
	return GUILD_GENERAL_POSTION_CH;
}

const char * ioLocalChina::GetGuildAdminPosition()
{
	return GUILD_GENERAL_POSTION_CH;
}

bool ioLocalChina::IsRightNewID( const char *szID )
{
	return true;
}

void ioLocalChina::SetTimeZone()
{
	// 유저PC설정이 다를 수 있으므로 정해진 [타임존]으로 변경
	_putenv("TZ=GMT-9"); 
	_tzset();
	//
}

bool ioLocalChina::IsSendGashaponList()
{
	return false;
}

void ioLocalChina::GetChangedString( OUT char *szCheckString )
{
	// 아무일도 하지 않음
}

bool ioLocalChina::IsExecuteLuncher()
{
	return false;
}

bool ioLocalChina::IsKoreaGameLimit()
{
	return false;
}

bool ioLocalChina::IsCheckKorean()
{
	return true;
}

bool ioLocalChina::IsMyHomePage()
{
	return true;
}

bool ioLocalChina::IsShowExtraGoodsList()
{
	return false;
}

bool ioLocalChina::IsShowRegulation()
{
	return true;
}

DWORD ioLocalChina::GetResourceIDErrorBack()
{
	return IDB_BACK_ERROR;
}

DWORD ioLocalChina::GetResourceIDErrorExitC()
{
	return IDB_EXIT_C;
}

DWORD ioLocalChina::GetResourceIDErrorExitN()
{
	return IDB_EXIT_N;
}

DWORD ioLocalChina::GetResourceIDErrorExitO()
{
	return IDB_EXIT_O;
}

DWORD ioLocalChina::GetResourceIDErrorSolutionC()
{
	return IDB_SOLUTION_C;
}

DWORD ioLocalChina::GetResourceIDErrorSolutionN()
{
	return IDB_SOLUTION_N;
}

DWORD ioLocalChina::GetResourceIDErrorSolutionO()
{
	return IDB_SOLUTION_O;
}

const char * ioLocalChina::GetErrorSolutionURL()
{
	return "http://www.lostsaga.com/common/rgfaq.asp?errnumber=%d";
}

bool ioLocalChina::SetPresentRecvDisableMsg( int iPresentType )
{
	return false;
}

int ioLocalChina::GetLimitGradeLevel()
{
	return -1;
}

const char * ioLocalChina::GetBillingErrorMent(ioHashString &sServerError)
{
	return "ASIASOFT Billing Error : ";
}

int ioLocalChina::GetLicenseDate()
{
	return 20121130;
}