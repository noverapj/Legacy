#include "../stdafx.h"
#include ".\iolocalThailand.h"
#include "../resource.h"
//#include "../ioMyInfo.h"
//#include "../ioApplication.h"
#include "ioLocalThailandLanguage.h"
//#include "../EtcHelpFunc.h"
//#include "../ioBrowserManager.h"
//#include "../Setting.h"

ioLocalThailand::ioLocalThailand(void)
{
}

ioLocalThailand::~ioLocalThailand(void)
{
}

ioLocalManager::LocalType ioLocalThailand::GetType()
{
	return ioLocalManager::LCT_THAILAND;
}

const char * ioLocalThailand::GetTextListFileName()
{
	return "text.txt";
}

const char * ioLocalThailand::GetMemTextList()
{
	return ThailandLanguage::GetMemTextList(); 
}

DWORD ioLocalThailand::GetResourceIDTitleCashBmp()
{
	return IDB_WEB_TITLE_CASH_TH;
}

DWORD ioLocalThailand::GetResourceIDTitleEntryBmp()
{
	return IDB_WEB_TITLE_ENTRY_TH;
}

DWORD ioLocalThailand::GetResourceIDTitleGuildBmp()
{
	return IDB_WEB_TITLE_GUILD_TH;
}

DWORD ioLocalThailand::GetResourceIDTitleGuildMarkBmp()
{
	return IDB_WEB_TITLE_GUILD_MARK_TH;
}

DWORD ioLocalThailand::GetResourceIDTitleHomeBmp()
{
	return IDB_WEB_TITLE_HOME_TH;
}

DWORD ioLocalThailand::GetResourceIDTitleBmp()
{
	return IDB_WINDOW_TITLE_LOSA;
}

void ioLocalThailand::SetLoginData( const ioHashString &szBillingUserKey )
{
	m_sLoginEncodePW = szBillingUserKey;
}

//void ioLocalThailand::FillLoginData( OUT SP2Packet &rkPacket )
//{
//	rkPacket << m_sLoginEncodePW;
//
//	// 더이상 필요가 없으므로 삭제
//	m_sLoginEncodePW.Clear();
//}

void ioLocalThailand::GetClassTypeForTutorialDropItem( int &riClassType )
{

}


int ioLocalThailand::GetFirstIDMaxSize()
{
	return 12;
}

bool ioLocalThailand::IsShowFirstIDWnd()
{
	return true;
}

void ioLocalThailand::OpenFillCashPage()
{
	//if( Setting::FullScreen() )
	//	ShowWindow( g_App.GetHWnd(), SW_MINIMIZE );

	//Help::ExcuteWeb( g_BrowserMgr.GetCashURL().c_str() );
}

bool ioLocalThailand::IsRightNewID( const char *szID )
{
	return true;
}

void ioLocalThailand::SetTimeZone()
{
	// 유저PC설정이 다를 수 있으므로 정해진 [타임존]으로 변경
	_putenv("TZ=GMT-7"); 
	_tzset();
	//
}

bool ioLocalThailand::IsSendGashaponList()
{
	return true;
}

void ioLocalThailand::GetChangedString( OUT char *szCheckString )
{
	// 아무일도 하지 않음
}

bool ioLocalThailand::IsExecuteLuncher()
{
	return false;
}

bool ioLocalThailand::IsKoreaGameLimit()
{
	return true;
}

bool ioLocalThailand::IsCheckKorean()
{
	return false;
}

bool ioLocalThailand::IsMyHomePage()
{
	return true;
}

bool ioLocalThailand::IsShowExtraGoodsList()
{
	return false;
}

bool ioLocalThailand::IsShowRegulation()
{
	return true;
}

DWORD ioLocalThailand::GetResourceIDErrorBack()
{
	return IDB_BACK_ERROR_TH;
}

DWORD ioLocalThailand::GetResourceIDErrorExitC()
{
	return IDB_EXIT_C_TH;
}

DWORD ioLocalThailand::GetResourceIDErrorExitN()
{
	return IDB_EXIT_N_TH;
}

DWORD ioLocalThailand::GetResourceIDErrorExitO()
{
	return IDB_EXIT_O_TH;
}

DWORD ioLocalThailand::GetResourceIDErrorSolutionC()
{
	return IDB_SOLUTION_C_TH;
}

DWORD ioLocalThailand::GetResourceIDErrorSolutionN()
{
	return IDB_SOLUTION_N_TH;
}

DWORD ioLocalThailand::GetResourceIDErrorSolutionO()
{
	return IDB_SOLUTION_O_TH;
}

const char * ioLocalThailand::GetErrorSolutionURL()
{
	return "http://www.lostsaga.in.th";
}

bool ioLocalThailand::SetPresentRecvDisableMsg( int iPresentType )
{
	return false;
}

int ioLocalThailand::GetLimitGradeLevel()
{
	return -1;
}

const char * ioLocalThailand::GetBillingErrorMent()
{
	return "ASIASOFT Billing Error : ";
}

int ioLocalThailand::GetLicenseDate()
{
	return 20131130;
}

void ioLocalThailand::ExitAction()
{
//	Help::ExcuteWeb( "http://games.playpark.com/" );
}


void ioLocalThailand::ShowHackShieldErrorDlg( const ioHashString &rsPrivateID )
{
#ifdef HACKSHIELD
	g_ioHackShield.ShowErrorDlg( rsPrivateID );
#endif
}