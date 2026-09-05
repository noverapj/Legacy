#include "../stdafx.h"
#include "../resource.h"
#include "../HelpFunc.h"
#include ".\iolocaltaiwan.h"
#include "ioLocalTaiwanLanguage.h"
#include "..\AutoUpgradeDlg.h"

ioLocalTaiwan::ioLocalTaiwan(void)
{
	ZeroMemory( m_szRegKey, sizeof( m_szRegKey ) );
	ZeroMemory( m_szStartURL, sizeof( m_szStartURL ) );
	ZeroMemory( m_szLoginID, sizeof( m_szLoginID ) );
	ZeroMemory( m_szLoginEncodePW, sizeof( m_szLoginEncodePW ) );
	ZeroMemory( m_szServerID, sizeof( m_szServerID ) );
}

ioLocalTaiwan::~ioLocalTaiwan(void)
{
}

ioLocalManager::LocalType ioLocalTaiwan::GetType()
{
	return ioLocalManager::LCT_TAIWAN;
}

const char * ioLocalTaiwan::GetTextListFileName()
{
	return "text_tw.txt";
}

const char * ioLocalTaiwan::GetMemTextList()
{
	return TaiwanLanguage::GetMemTextList();
}

void ioLocalTaiwan::GetPacPassword( OUT char *szPassword, IN const int iPasswordSize, IN const int iPasswordType /*=0*/ )
{
	if( iPasswordType == 0 )
	{
		// password : iUT38#@49vnFdjf)(4sg
		char szEncPassWord[MAX_PASSWORD]={ -105, 74, 75, 57, 101, 2, 44, -110, 127, 81, 110, 97, 34, -52, 10, 8, 117, 62, 108, 120 };
		EncryptDecryptData( szPassword, iPasswordSize, szEncPassWord, MAX_PASSWORD, true );
	}
	else if( iPasswordType == 1 )
	{
		// password : Yi#weT%^903Unv0$2gfj
		char szEncPassWord[MAX_PASSWORD]={ -89, 118, 60, 125, 56, 117, 73, -8, 127, 23, 51, 114, 40, -48, 92, 5, 111, 109, 121, 117 };
		EncryptDecryptData( szPassword, iPasswordSize, szEncPassWord, MAX_PASSWORD, true );
	}
}

DWORD ioLocalTaiwan::GetResourceIDBackBmp()
{
	return IDB_BACK_TW;
}

DWORD ioLocalTaiwan::GetResourceIDOptimizeBtnBmp()
{
	return IDB_OPTIMIZE_BTN_TW;
}

DWORD ioLocalTaiwan::GetResourceIDStartBtnBmp()
{
	return IDB_START_TW;
}

DWORD ioLocalTaiwan::GetResourceIDErrorBackBtnBmp()
{
	return IDB_ERROR_BACK_TW;
}

DWORD ioLocalTaiwan::GetResourceIDErrorSolutionBtnBmp()
{
	return IDB_SOLUTIONBTN_TW;
}

DWORD ioLocalTaiwan::GetResourceIDErrorExitBtnBmp()
{
	return IDB_EXITBTN_TW;
}

const char * ioLocalTaiwan::GetRegKey()
{
	return "Software\\SP2ClientTW\\";
}

const char * ioLocalTaiwan::GetFontName()
{
	return FONT_NAME_TW;
}

void ioLocalTaiwan::CreateShortcuts( const CString &rszRootDir, const CString &rszShortcutName, const CString &rszLoginURL )
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

const char * ioLocalTaiwan::GetErrorSolutionURL()
{
	return "https://vfun.valofe.com/customer/support?service_code=ls_tw";
}

bool ioLocalTaiwan::GetNewCmdLine( IN const char *szCmd, OUT char *szNewCmd, IN int iNewCmdSize )
{
	char szCurCmd[4096]="";
	ZeroMemory( szCurCmd, sizeof( szCurCmd ) );
	StringCbCopy( szCurCmd, sizeof( szCurCmd ), szCmd );

	enum 
	{ 
		MAX_LOOP = 5, 
		TYPE_EXPAND_KEY = 0, 
		TYPE_KEY_VALUE  = 1,
	};

	// parsing main token
	for (int i = 0; i < MAX_LOOP ; i++)
	{
		char *pPos = NULL;
		if( i == 0 )
			pPos = strtok( szCurCmd, CMD_SECTION_TOKEN );
		else
			pPos = strtok( NULL, CMD_SECTION_TOKEN );

		if( pPos == NULL )
			break;

		if( i == TYPE_EXPAND_KEY )
		{
			StringCbCat( szNewCmd, iNewCmdSize, " " );  // cmd 라인 빈칸 입력 a.exe aaaa
			StringCbCat( szNewCmd, iNewCmdSize, pPos );
		}
		else 
		{
			StringCbCat( szNewCmd, iNewCmdSize, CMD_SECTION_TOKEN );
			StringCbCat( szNewCmd, iNewCmdSize, pPos );
		}
	}

	return true;
}

bool ioLocalTaiwan::GetLoginResultPostData( OUT char *szError, IN int iErrorSize, const char *szURL, const char *szID, const char *szPW )
{
	return true;
}

int ioLocalTaiwan::GetLoginKeyLiveTime()
{
	return ( LOGIN_KEY_LIVE_TIME*100 ); // 서버에서 로그인을 하므로 로그인키 생존 시간이 필요 없기때문에 아주길게 설정
}

void ioLocalTaiwan::SetLoginData( const char *szLoginID, const char *szLoginEncodePW )
{
	if( szLoginID == NULL )
		return;
	if( szLoginEncodePW == NULL )
		return;

	StringCbCopy( m_szLoginID, sizeof( m_szLoginID ), szLoginID );
	StringCbCopy( m_szLoginEncodePW, sizeof( m_szLoginEncodePW ), szLoginEncodePW );
}

void ioLocalTaiwan::SetServerID( const char *szServerID )
{
	if( szServerID == NULL )
		return;

	StringCbCopy( m_szServerID, sizeof( m_szServerID ), szServerID );
}

bool ioLocalTaiwan::IsRightID( const char *szID )
{
	enum { MIN_LENGTH = 1,  };

	int iSize = strlen( szID );
	if ( iSize < MIN_LENGTH || iSize > GetMaxIDSize() )
		return false;

	// 이메일이 로그인ID임 모든 이메일 형식을 허용함.
	return true;
}

bool ioLocalTaiwan::IsRightPW( const char *szPW )
{
	enum { MIN_LENGTH = 2,  };

	int iSize = strlen( szPW );
	if ( iSize < MIN_LENGTH || iSize > GetMaxPWSize() )
		return false;

	// 모두 허용
	return true;
}

const char * ioLocalTaiwan::GetConnectFailMent()
{
	return "Error Connect";
}

const char * ioLocalTaiwan::GetLoginWrongIDMent()
{
	return "Wrong ID";
}

const char * ioLocalTaiwan::GetLoginWrongPWMent()
{
	return "Wrong Password";
}

const char * ioLocalTaiwan::GetEntryURL()
{
	return "http://platform.garena.tw/register/";
}

const char * ioLocalTaiwan::GetFindIDURL()
{
	return "http://lostsaga.garena.tw/";
}

const char * ioLocalTaiwan::GetFindPWURL()
{
	return "http://platform.garena.tw/forgot/";
}

void ioLocalTaiwan::FillLoginData( OUT SP2Packet &rkPacket )
{
#ifdef MAC_ADDRESS 
	char szMacAddress[MAX_PATH]="";
	Help::GetMacAddress( szMacAddress, sizeof( szMacAddress ) );
	rkPacket << szMacAddress;
#endif
}