#include "../stdafx.h"
#include "../resource.h"
#include ".\iolocalJapan.h"
#include "../HelpFunc.h"
#include "ioLocalJapanLanguage.h"

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

void ioLocalJapan::GetPacPassword( OUT char *szPassword, IN const int iPasswordSize, IN const int iPasswordType /*=0*/  )
{
	if( iPasswordType == 0 )
	{
		// password : EDgei%^df930%#fj!_=]
		char szEncPassWord[MAX_PASSWORD]={ -69, 91, 120, 111, 52, 4, 50, -62, 32, 30, 51, 23, 99, -123, 10, 75, 124, 85, 34, 66 };
		EncryptDecryptData( szPassword, iPasswordSize, szEncPassWord, MAX_PASSWORD, true );
	}
	else if( iPasswordType == 1 )
	{
		// password : @7$gjTRreie][!323O++
		char szEncPassWord[MAX_PASSWORD]={ -66, 40, 59, 109, 55, 117, 62, -44, 35, 78, 101, 122, 29, -121, 95, 19, 110, 69, 52, 52 };
		EncryptDecryptData( szPassword, iPasswordSize, szEncPassWord, MAX_PASSWORD, true );
	}
}

DWORD ioLocalJapan::GetResourceIDBackBmp()
{
	return IDB_BACK_JP;
}

DWORD ioLocalJapan::GetResourceIDOptimizeBtnBmp()
{
	return IDB_OPTIMIZE_BTN_JP;
}

DWORD ioLocalJapan::GetResourceIDStartBtnBmp()
{
	return IDB_START_JP;
}

DWORD ioLocalJapan::GetResourceIDErrorBackBtnBmp()
{
	return IDB_ERROR_BACK_JP;
}

DWORD ioLocalJapan::GetResourceIDErrorSolutionBtnBmp()
{
	return IDB_SOLUTIONBTN_JP;
}

DWORD ioLocalJapan::GetResourceIDErrorExitBtnBmp()
{
	return IDB_EXITBTN_JP;
}

const char * ioLocalJapan::GetRegKey()
{
	return "Software\\SP2ClientJP\\";
}

const char * ioLocalJapan::GetFontName()
{
	return FONT_NAME_JP;
}

void ioLocalJapan::CreateShortcuts( const CString &rszRootDir, const CString &rszShortcutName, const CString &rszLoginURL )
{
	Help::CreateShortcutOnDeskTop( rszRootDir, rszShortcutName, rszLoginURL );
	Help::CreateMenuIcon( rszShortcutName );

	CString szClientPath = "lostsaga.exe";
	CString szFullPath;
	Help::SetFullPath( rszRootDir, szClientPath, szFullPath);

	char szVersionName[MAX_PATH*2]="";
	Help::GetFileVesrion((LPCTSTR)szFullPath, szVersionName, sizeof(szVersionName), false );
	Help::CreateUninstallRegInfo( rszRootDir, szVersionName );
}

const char * ioLocalJapan::GetErrorSolutionURL()
{
	return "http://www.lostsaga.jp";
}