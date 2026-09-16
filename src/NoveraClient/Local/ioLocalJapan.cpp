#include "stdafx.h"
#include ".\iolocalJapan.h"
#include "../resource.h"
#include "../ioMyInfo.h"
#include "../ioApplication.h"
#include "ioLocalJapanLanguage.h"
#include "../Setting.h"
#include "../EtcHelpFunc.h"
#include "../ioBrowserManager.h"

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

const char * ioLocalJapan::GetTextDirName()
{
	return "jp";
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

void ioLocalJapan::FillLoginData( OUT SP2Packet &rkPacket )
{

}

bool ioLocalJapan::IsShowFirstIDWnd()
{
	return false;
}

const char * ioLocalJapan::GetGuildMasterPostion()
{
	return GUILD_MASTER_POSTION_JP;
}

const char * ioLocalJapan::GetGuildSecondMasterPosition()
{
	return GUILD_SECOND_MASTER_POSTION_JP;
}

const char * ioLocalJapan::GetGuildGeneralPosition()
{
	return GUILD_GENERAL_POSTION_JP;
}

const char * ioLocalJapan::GetGuildAdminPosition()
{
	return GUILD_GENERAL_POSTION_JP;
}

bool ioLocalJapan::IsSendGashaponList()
{
	return false;
}

void ioLocalJapan::GetChangedString( OUT char *szCheckString )
{
	// 아무일도 하지 않음
}

bool ioLocalJapan::IsCheckKorean()
{
	return true;
}

bool ioLocalJapan::IsMyHomePage()
{
	return true;
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

const char * ioLocalJapan::GetBillingErrorMent(ioHashString &sServerError)
{
	return "CJIJ Billing Error : ";
}