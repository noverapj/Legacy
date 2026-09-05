#include "../stdafx.h"
#include "../resource.h"
#include "../HelpFunc.h"
#include ".\ioLocalChina.h"
#include "../Util/md5.h"
#include <winsock.h>
#include "../Util/ioHttpManager.h"
#include <strsafe.h>
#include "../HttpApp.h"
#include "../StringManager/ioHashString.h"
#include "ioLocalChinaLanguage.h"
#include "../AutoUpgradeDlg.h"

ioLocalChina::ioLocalChina(void)
{	
	ZeroMemory( m_szRegKey, sizeof( m_szRegKey ) );
	ZeroMemory( m_szStartURL, sizeof( m_szStartURL ) );
	ZeroMemory( m_szLoginID, sizeof( m_szLoginID ) );
	ZeroMemory( m_szLoginEncodePW, sizeof( m_szLoginEncodePW ) );
}

ioLocalChina::~ioLocalChina(void)
{
}

ioLocalManager::LocalType ioLocalChina::GetType()
{
	return ioLocalManager::LCT_CHINA;
}

const char * ioLocalChina::GetTextListFileName()
{
	return "text.txt";
}

const char * ioLocalChina::GetMemTextList()
{
	return  ChinaLanguage::GetMemTextList();
}

void ioLocalChina::GetPacPassword( OUT char *szPassword, IN const int iPasswordSize, IN const int iPasswordType /*=0*/ )
{
#ifdef SHIPPING
	if( iPasswordType == 0 )
	{
		// password : -)4TRfkl-41$%dgkrm05
		char szEncPassWord[MAX_PASSWORD]={ -45, 54, 43, 94, 15, 71, 7, -54, 107, 19, 49, 3, 99, -62, 11, 74, 47, 103, 47, 42 };
		EncryptDecryptData( szPassword, iPasswordSize, szEncPassWord, MAX_PASSWORD, true );
	}
	else if( iPasswordType == 1 )
	{
		// password : |059rtuGReowo@##tkg0
		char szEncPassWord[MAX_PASSWORD]={ -126, 47, 42, 51, 47, 85, 25, -31, 20, 66, 111, 80, 41, -26, 79, 2, 41, 97, 120, 47 };
		EncryptDecryptData( szPassword, iPasswordSize, szEncPassWord, MAX_PASSWORD, true );
	}
#else
	// 테스트때문에 디버그는 한국것 추가
	if( iPasswordType == 0 )
	{
		// password 0: iosuccess#@
		char szEncPassWord[MAX_PASSWORD]={ -105, 112, 108, 127, 62, 66, 9, -43, 53, 4, 64, 39, 70, -90, 108, 33, 93, 10, 31, 31 };
		EncryptDecryptData( szPassword, iPasswordSize, szEncPassWord, MAX_PASSWORD, true );
	}
	else if( iPasswordType == 1 )
	{
		// password 1: XrFrI0%3BF%!0Dcx$30-
		char szEncPassWord[MAX_PASSWORD]={ -90, 109, 89, 120, 20, 17, 73, -107, 4, 97, 37, 6, 118, -30, 15, 89, 121, 57, 47, 50 };
		EncryptDecryptData( szPassword, iPasswordSize, szEncPassWord, MAX_PASSWORD, true );
	}
#endif
}

DWORD ioLocalChina::GetResourceIDBackBmp()
{
	return IDB_BACK_ID;
}

DWORD ioLocalChina::GetResourceIDOptimizeBtnBmp()
{
	return IDB_OPTIMIZE_BTN_ID;
}

DWORD ioLocalChina::GetResourceIDStartBtnBmp()
{
	return IDB_START_ID;
}

DWORD ioLocalChina::GetResourceIDErrorBackBtnBmp()
{
	return IDB_ERROR_BACK_ID;
}

DWORD ioLocalChina::GetResourceIDErrorSolutionBtnBmp()
{
	return IDB_SOLUTIONBTN_ID;
}

DWORD ioLocalChina::GetResourceIDErrorExitBtnBmp()
{
	return IDB_EXITBTN_ID;
}

const char * ioLocalChina::GetRegKey()
{
	return "Software\\SP2ClientCH\\";
}

const char * ioLocalChina::GetFontName()
{
	return FONT_NAME_CH;
}

void ioLocalChina::CreateShortcuts( const CString &rszRootDir, const CString &rszShortcutName, const CString &rszLoginURL )
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

const char * ioLocalChina::GetErrorSolutionURL()
{
	return "http://forum.gemscool.com/forum-143.html";
}

bool ioLocalChina::GetNewCmdLine( IN const char *szCmd, OUT char *szNewCmd, IN int iNewCmdSize )
{
	if( strcmp( m_szLoginID, "" ) == 0 )
	{
		StringCbCopy( szNewCmd, iNewCmdSize, "LoginID is empty." );
		return false;
	}

	if( strcmp( m_szLoginEncodePW, "" ) == 0 )
	{
		StringCbCopy( szNewCmd, iNewCmdSize, "LoginPW is empty." );
		return false;
	}

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
			StringCbCat( szNewCmd, iNewCmdSize, CMD_EXPAND_SECTION_TOKEN );
			StringCbCat( szNewCmd, iNewCmdSize, m_szLoginEncodePW );
		}
		else if( i == TYPE_KEY_VALUE )
		{
			// 인도네시아 키값은 0으로 쓰레기 값이므로 복사하지 않는다.
			StringCbCat( szNewCmd, iNewCmdSize, CMD_SECTION_TOKEN );
			StringCbCat( szNewCmd, iNewCmdSize, m_szLoginID );
		}
		else 
		{
			StringCbCat( szNewCmd, iNewCmdSize, CMD_SECTION_TOKEN );
			StringCbCat( szNewCmd, iNewCmdSize, pPos );
		}
	}

	return true;
}

bool ioLocalChina::GetLoginResultPostData( OUT char *szError, IN int iErrorSize, const char *szURL, const char *szID, const char *szPW )
{
	return true;
}

int ioLocalChina::GetLoginKeyLiveTime()
{
	return ( LOGIN_KEY_LIVE_TIME*100 ); // 서버에서 로그인을 하므로 로그인키 생존 시간이 필요 없기때문에 아주길게 설정
}

void ioLocalChina::SetLoginData( const char *szLoginID, const char *szLoginEncodePW )
{
	if( szLoginID == NULL )
		return;
	if( szLoginEncodePW == NULL )
		return;

	StringCbCopy( m_szLoginID, sizeof( m_szLoginID ), szLoginID );
	StringCbCopy( m_szLoginEncodePW, sizeof( m_szLoginEncodePW ), szLoginEncodePW );
}

bool ioLocalChina::IsRightID( const char *szID )
{
	enum { MIN_LENGTH = 6,  };

	int iSize = strlen( szID );
	if ( iSize < MIN_LENGTH || iSize > GetMaxIDSize() )
		return false;

	for (int i=0; i<iSize; i++)
	{
		if ((!COMPARE(szID[i], 'A', 'Z'+1)) &&
			(!COMPARE(szID[i], 'a', 'z'+1)) &&
			(!COMPARE(szID[i], '0', '9'+1)) &&
			(szID[i]!='-') &&
			(szID[i]!='_') )
		{
			return false;
		}
	}
	return true;
}

bool ioLocalChina::IsRightPW( const char *szPW )
{
	enum { MIN_LENGTH = 6,  };

	int iSize = strlen( szPW );
	if ( iSize < MIN_LENGTH || iSize > GetMaxPWSize() )
		return false;

	for (int i=0; i<iSize; i++)
	{
		if ((!COMPARE(szPW[i], 'A', 'Z'+1)) &&
			(!COMPARE(szPW[i], 'a', 'z'+1)) &&
			(!COMPARE(szPW[i], '0', '9'+1)) &&
			(szPW[i]!='-') &&
			(szPW[i]!='_') )
		{
			return false;
		}
	}
	return true;
}

const char * ioLocalChina::GetConnectFailMent()
{
	return "Tidak dapat tersambung ke server.\r\n\r\nKarena pilihan program jaringan \r\n server tidak dapat tersambung.(Koneksi TCP gagal)";
}

const char * ioLocalChina::GetLoginWrongIDMent()
{
	return "Wrong ID";
}

const char * ioLocalChina::GetLoginWrongPWMent()
{
	return "Wrong Password";
}

const char * ioLocalChina::GetEntryURL()
{
	return "http://lostsaga.gemscool.com/index.php?act=1";
}

const char * ioLocalChina::GetFindIDURL()
{
	return "http://www.gemscool.com/registration/index.php";
}

const char * ioLocalChina::GetFindPWURL()
{
	return "http://www.gemscool.com/forgot/index.php";
}