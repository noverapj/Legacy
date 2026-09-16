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
	return ioLocalManager::LCT_CHINA;
}

const char * ioLocalChina::GetTextDirName()
{
	return "cn";
}

const char * ioLocalChina::GetMemTextList()
{
	return ChinaLanguage::GetMemTextList();
}

const char * ioLocalChina::GetFontFileName()
{
	return "chinese.ttf";
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

bool ioLocalChina::IsShowFirstIDWnd()
{
	return false;
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

bool ioLocalChina::IsSendGashaponList()
{
	return false;
}

void ioLocalChina::GetChangedString( OUT char *szCheckString )
{
	// 아무일도 하지 않음
}

bool ioLocalChina::IsCheckKorean()
{
	return true;
}

bool ioLocalChina::IsMyHomePage()
{
	return true;
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

const char * ioLocalChina::GetBillingErrorMent(ioHashString &sServerError)
{
	return "ASIASOFT Billing Error : ";
}