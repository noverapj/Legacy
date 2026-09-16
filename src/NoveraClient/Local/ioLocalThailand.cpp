#include "stdafx.h"
#include ".\iolocalThailand.h"
#include "../resource.h"
#include "../ioMyInfo.h"
#include "../ioApplication.h"
#include "ioLocalThailandLanguage.h"
#include "../EtcHelpFunc.h"
#include "../ioBrowserManager.h"
#include "../Setting.h"

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

const char * ioLocalThailand::GetFontFileName()
{
	return "thailand.ttf";
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

void ioLocalThailand::FillLoginData( OUT SP2Packet &rkPacket )
{
	rkPacket << m_sLoginEncodePW;

	// 더이상 필요가 없으므로 삭제
	m_sLoginEncodePW.Clear();
}

bool ioLocalThailand::IsShowFirstIDWnd()
{
	return true;
}

const char * ioLocalThailand::GetGuildMasterPostion()
{
	return GUILD_MASTER_POSTION_TH;
}

const char * ioLocalThailand::GetGuildSecondMasterPosition()
{
	return GUILD_SECOND_MASTER_POSTION_TH;
}

const char * ioLocalThailand::GetGuildGeneralPosition()
{
	return GUILD_GENERAL_POSTION_TH;
}

const char * ioLocalThailand::GetGuildAdminPosition()
{
	return GUILD_GENERAL_POSTION_TH;
}

bool ioLocalThailand::IsSendGashaponList()
{
	return true;
}

void ioLocalThailand::GetChangedString( OUT char *szCheckString )
{
	// 아무일도 하지 않음
}

bool ioLocalThailand::IsCheckKorean()
{
	return false;
}

bool ioLocalThailand::IsMyHomePage()
{
	return true;
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

const char * ioLocalThailand::GetBillingErrorMent(ioHashString &sServerError)
{
	return "ASIASOFT Billing Error : ";
}

void ioLocalThailand::ExitAction()
{
	Help::ExcuteWeb( "http://games.playpark.com/" );
}


void ioLocalThailand::ShowHackShieldErrorDlg( const ioHashString &rsPrivateID )
{

}