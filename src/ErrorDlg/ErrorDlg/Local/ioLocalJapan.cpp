#include "../stdafx.h"
#include ".\iolocalJapan.h"
#include "../resource.h"
//#include "../ioMyInfo.h"
//#include "../ioApplication.h"
#include "ioLocalJapanLanguage.h"
//#include "../Setting.h"
//#include "../EtcHelpFunc.h"
//#include "../ioBrowserManager.h"

ioLocalJapan::ioLocalJapan(void)
{
}

ioLocalJapan::~ioLocalJapan(void)
{
}

ioLocalManager::LocalType ioLocalJapan::GetType()
{
	return ioLocalManager::LCT_JAPAN;
}

const char * ioLocalJapan::GetTextListFileName()
{
	return "text.txt";
}

const char * ioLocalJapan::GetMemTextList()
{
	return JapanLanguage::GetMemTextList(); 
}

DWORD ioLocalJapan::GetResourceIDTitleCashBmp()
{
	return IDB_WEB_TITLE_CASH_JP;
}

DWORD ioLocalJapan::GetResourceIDTitleEntryBmp()
{
	return IDB_WEB_TITLE_ENTRY_JP;
}

DWORD ioLocalJapan::GetResourceIDTitleGuildBmp()
{
	return IDB_WEB_TITLE_GUILD;
}

DWORD ioLocalJapan::GetResourceIDTitleGuildMarkBmp()
{
	return IDB_WEB_TITLE_GUILD_MARK;
}

DWORD ioLocalJapan::GetResourceIDTitleHomeBmp()
{
	return IDB_WEB_TITLE_HOME_JP;
}

DWORD ioLocalJapan::GetResourceIDTitleBmp()
{
	return IDB_WINDOW_TITLE_LOSA_JP;
}

void ioLocalJapan::SetLoginData( const ioHashString &szBillingUserKey )
{

}

//void ioLocalJapan::FillLoginData( OUT SP2Packet &rkPacket )
//{
//
//}

void ioLocalJapan::GetClassTypeForTutorialDropItem( int &riClassType )
{

}


int ioLocalJapan::GetFirstIDMaxSize()
{
	return 12;
}

bool ioLocalJapan::IsShowFirstIDWnd()
{
	return false;
}

void ioLocalJapan::OpenFillCashPage()
{
	//if( g_BrowserMgr.GetCashURL().IsEmpty() )
	//	return;

	//if( Setting::FullScreen() )
	//	ShowWindow( g_App.GetHWnd(), SW_MINIMIZE );

	//Help::ExcuteWeb( g_BrowserMgr.GetCashURL().c_str() );
}


bool ioLocalJapan::IsRightNewID( const char *szID )
{
	return true;
}

void ioLocalJapan::SetTimeZone()
{
	// 유저PC설정이 다를 수 있으므로 정해진 [타임존]으로 변경
	_putenv("TZ=GMT-9"); 
	_tzset();
	//
}

bool ioLocalJapan::IsSendGashaponList()
{
	return false;
}

void ioLocalJapan::GetChangedString( OUT char *szCheckString )
{
	// 아무일도 하지 않음
}

bool ioLocalJapan::IsExecuteLuncher()
{
	return false;
}

bool ioLocalJapan::IsKoreaGameLimit()
{
	return false;
}

bool ioLocalJapan::IsCheckKorean()
{
	return true;
}

bool ioLocalJapan::IsMyHomePage()
{
	return true;
}

bool ioLocalJapan::IsShowExtraGoodsList()
{
	return false;
}

bool ioLocalJapan::IsShowRegulation()
{
	return true;
}

DWORD ioLocalJapan::GetResourceIDErrorBack()
{
	return IDB_BACK_ERROR_JP;
}

DWORD ioLocalJapan::GetResourceIDErrorExitC()
{
	return IDB_EXIT_C_JP;
}

DWORD ioLocalJapan::GetResourceIDErrorExitN()
{
	return IDB_EXIT_N_JP;
}

DWORD ioLocalJapan::GetResourceIDErrorExitO()
{
	return IDB_EXIT_O_JP;
}

DWORD ioLocalJapan::GetResourceIDErrorSolutionC()
{
	return IDB_SOLUTION_C_JP;
}

DWORD ioLocalJapan::GetResourceIDErrorSolutionN()
{
	return IDB_SOLUTION_N_JP;
}

DWORD ioLocalJapan::GetResourceIDErrorSolutionO()
{
	return IDB_SOLUTION_O_JP;
}

const char * ioLocalJapan::GetErrorSolutionURL()
{
	return "http://www.lostsaga.jp";
}

bool ioLocalJapan::SetPresentRecvDisableMsg( int iPresentType )
{
	return false;
}

int ioLocalJapan::GetLimitGradeLevel()
{
	return -1;
}

const char * ioLocalJapan::GetBillingErrorMent()
{
	return "CJIJ Billing Error : ";
}

int ioLocalJapan::GetLicenseDate()
{
	return 20131130;
}