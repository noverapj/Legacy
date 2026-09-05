#include "../stdafx.h"
#include "../resource.h"
#include "../HelpFunc.h"
#include ".\ioLocalLatin.h"
#include "../Util/md5.h"
#include <winsock.h>
#include "../Util/ioHttpManager.h"
#include <strsafe.h>
#include "../HttpApp.h"
#include "../StringManager/ioHashString.h"
#include "../AutoUpgradeDlg.h"
#include "ioLocalLatinLanguage.h"

ioLocalLatin::ioLocalLatin(void)
{
	ZeroMemory( m_szRegKey, sizeof( m_szRegKey ) );
	ZeroMemory( m_szStartURL, sizeof( m_szStartURL ) );
	ZeroMemory( m_szLoginID, sizeof( m_szLoginID ) );
	ZeroMemory( m_szLoginEncodePW, sizeof( m_szLoginEncodePW ) );
	ZeroMemory( m_szServerID, sizeof( m_szServerID ) );
	ZeroMemory( m_szUserType, sizeof( m_szUserType ) );
}

ioLocalLatin::~ioLocalLatin(void)
{
}

ioLocalManager::LocalType ioLocalLatin::GetType()
{
	return ioLocalManager::LCT_LATIN;
}

const char * ioLocalLatin::GetTextListFileName()
{
	return "text_eu.txt";
}

const char * ioLocalLatin::GetMemTextList()
{
	return LatinLanguage::GetMemTextList();
}

void ioLocalLatin::GetPacPassword( OUT char *szPassword, IN const int iPasswordSize, IN const int iPasswordType /*=0*/ )
{
	if( iPasswordType == 0 )
	{
		// password 0: dus!qhdaksl
		char szEncPassWord[MAX_PASSWORD]={ -102, 106, 108, 43, 44, 73, 8, -57, 45, 84, 108, 39, 70, -90, 108, 33, 93, 10, 31, 31 };
		EncryptDecryptData( szPassword, iPasswordSize, szEncPassWord, MAX_PASSWORD, true );
	}
	else if( iPasswordType == 1 )
	{
		// password 1: tkdjqqn!dlfEhrqkfhgo
		char szEncPassWord[MAX_PASSWORD]={ -118, 116, 123, 96, 44, 80, 2, -121, 34, 75, 102, 98, 46, -44, 29, 74, 59, 98, 120, 112 };
		EncryptDecryptData( szPassword, iPasswordSize, szEncPassWord, MAX_PASSWORD, true );
	}
}

DWORD ioLocalLatin::GetResourceIDBackBmp()
{
	return IDB_BACK_SA;
}

DWORD ioLocalLatin::GetResourceIDOptimizeBtnBmp()
{
	return IDB_OPTIMIZE_BTN_US;
}

DWORD ioLocalLatin::GetResourceIDStartBtnBmp()
{
	return IDB_START_SA;
}

DWORD ioLocalLatin::GetResourceIDErrorBackBtnBmp()
{
	return IDB_ERROR_BACK_SA;
}

DWORD ioLocalLatin::GetResourceIDErrorSolutionBtnBmp()
{
	return IDB_SOLUTIONBTN_SA;  
}

DWORD ioLocalLatin::GetResourceIDErrorExitBtnBmp()
{
	return IDB_EXITBTN_SA;
}

const char * ioLocalLatin::GetRegKey()
{
	return "Software\\SP2ClientEU\\";
}

const char * ioLocalLatin::GetFontName()
{
	return "Tahoma";
}

void ioLocalLatin::CreateShortcuts( const CString &rszRootDir, const CString &rszShortcutName, const CString &rszLoginURL )
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

const char * ioLocalLatin::GetErrorSolutionURL()
{
	return "http://www.softnyx.com/customercare/customercare.aspx?TopCode=12";
}

bool ioLocalLatin::GetNewCmdLine( IN const char *szCmd, OUT char *szNewCmd, IN int iNewCmdSize )
{
	char szCurCmd[4096]="";
	ZeroMemory( szCurCmd, sizeof( szCurCmd ) );
	StringCbCopy( szCurCmd, sizeof( szCurCmd ), szCmd );

	CAutoUpgradeDlg* pDlg = NULL;
	if(AfxGetApp())
		pDlg = (CAutoUpgradeDlg*) AfxGetApp()->GetMainWnd();

// 	if( pDlg && pDlg->IsGameServerID() )
// 	{
// 		string sGameServerID;
// 		pDlg->GetStrGameServerID( sGameServerID );
// 		if( pLocal )
// 			pLocal->SetServerID( sGameServerID.c_str() );
// 	}
// 	else
// 	{
// 		if( COMPARE( m_iServerIPPortIdx , 0, (int)m_vServerIPPort.size() ) )
// 		{
// 			if( pLocal )
// 				pLocal->SetServerID( m_vServerIPPort[m_iServerIPPortIdx].c_str() );
// 		}
// 	}

	enum 
	{ 
		MAX_LOOP           = 5, 
		TYPE_EXPAND_KEY    = 0, 
		TYPE_KEY_VALUE     = 1,
		TYPE_GAMESERVER_ID = 3,
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
//  			StringCbCat( szNewCmd, iNewCmdSize, CMD_EXPAND_SECTION_TOKEN );
//  			StringCbCat( szNewCmd, iNewCmdSize, m_szLoginID );
		}
		else if( i == TYPE_KEY_VALUE )
		{
			// 키값은 0으로 쓰레기 값이므로 복사하지 않는다.
			StringCbCat( szNewCmd, iNewCmdSize, CMD_SECTION_TOKEN );
			StringCbCat( szNewCmd, iNewCmdSize, m_szLoginEncodePW );
		}
		else if( i == TYPE_GAMESERVER_ID )
		{
			// 정상 접속이 되는 서버ID를 넣는다.
			StringCbCat( szNewCmd, iNewCmdSize, CMD_SECTION_TOKEN );

			if( pDlg && pDlg->IsGameServerID() )
			{
				std::string szServerID;
				pDlg->GetStrGameServerID(szServerID);
				SetServerID(szServerID.c_str());
				StringCbCat( szNewCmd, iNewCmdSize, m_szServerID );
			}
			else
			{
				StringCbCat( szNewCmd, iNewCmdSize, pPos );
			}
		}
		else 
		{
			StringCbCat( szNewCmd, iNewCmdSize, CMD_SECTION_TOKEN );
			StringCbCat( szNewCmd, iNewCmdSize, pPos );
		}
	}

	return true;
}

bool ioLocalLatin::GetLoginResultPostData( OUT char *szError, IN int iErrorSize, const char *szURL, const char *szID, const char *szPW )
{
	return true;
}

int ioLocalLatin::GetLoginKeyLiveTime()
{
	return ( LOGIN_KEY_LIVE_TIME*100 ); // 서버에서 로그인을 하므로 로그인키 생존 시간이 필요 없기때문에 아주길게 설정
}

void ioLocalLatin::SetLoginData( const char *szLoginID, const char *szLoginEncodePW )
{
	if( szLoginID == NULL )
		return;
	if( szLoginEncodePW == NULL )
		return;

	StringCbCopy( m_szLoginID, sizeof( m_szLoginID ), szLoginID );
	StringCbCopy( m_szLoginEncodePW, sizeof( m_szLoginEncodePW ), szLoginEncodePW );
}

void ioLocalLatin::SetServerID( const char *szServerID )
{
	if( szServerID == NULL )
		return;

	StringCbCopy( m_szServerID, sizeof( m_szServerID ), szServerID );
}

bool ioLocalLatin::IsRightID( const char *szID )
{
	enum { MIN_LENGTH = 3,  };

	int iSize = strlen( szID );
	if ( iSize < MIN_LENGTH || iSize > GetMaxIDSize() )
		return false;

	for (int i=0; i<iSize; i++)
	{
		if ((!COMPARE(szID[i], 'A', 'Z'+1)) &&
			(!COMPARE(szID[i], 'a', 'z'+1)) &&
			(!COMPARE(szID[i], '0', '9'+1)) &&
			(szID[i]!='-') &&
			(szID[i]!='_') &&
			(szID[i]!='@') &&
			(szID[i]!='.') &&
			(szID[i]!='+') &&
			(szID[i]!='|') ) // 구분자
		{
			return false;
		}
	}
	return true;
}

bool ioLocalLatin::IsRightPW( const char *szPW )
{
	enum { MIN_LENGTH = 3,  };

	int iSize = strlen( szPW );
	if ( iSize < MIN_LENGTH || iSize > GetMaxPWSize() )
		return false;

	for (int i=0; i<iSize; i++)
	{
		if ((!COMPARE(szPW[i], 'A', 'Z'+1)) &&
			(!COMPARE(szPW[i], 'a', 'z'+1)) &&
			(!COMPARE(szPW[i], '0', '9'+1)) )
		{
			return false;
		}
	}
	return true;
}

const char * ioLocalLatin::GetConnectFailMent()
{
	return "Cannot connect the server.\r\n\r\n Due to problem with Computer’s Network Program setup \r\n the connection to server failed.(TCP Connection Failed)\r\n(Error Code:227)";
}

const char * ioLocalLatin::GetLoginWrongIDMent()
{
	return "Wrong ID";
}

const char * ioLocalLatin::GetLoginWrongPWMent()
{
	return "Wrong Password";
}

const char * ioLocalLatin::GetServerFullMent()
{
	return "Game Server is full.\r\n(Error Code:228)";
}

const char * ioLocalLatin::GetEntryURL()
{
	return "http://lostsaga.z8games.com/landing/";
}

const char * ioLocalLatin::GetFindIDURL()
{
	return "http://www.z8games.com/lost_id_password.aspx";
}

const char *ioLocalLatin::GetLoginDlgURL()
{
	return "http://images.z8games.com/lsna/launcher/img/launcher_main.jpg";
}

const char * ioLocalLatin::GetFindPWURL()
{
	return ""; // facebook
}

void ioLocalLatin::SetUserType( const char *szUserType )
{
	StringCbCopy( m_szUserType, sizeof( m_szUserType ), szUserType );
}

void ioLocalLatin::FillLoginData( OUT SP2Packet &rkPacket )
{
	rkPacket << m_szUserType;
}


void ioLocalLatin::OnFacebookData( const string &rsEncryptData )
{
	enum { MAX_RANDOM_KEY = 20, };
	char szRandomKey[MAX_PATH*2]="";
	if( rsEncryptData.length() > MAX_RANDOM_KEY )
		StringCbCopyN( szRandomKey, sizeof( szRandomKey ), rsEncryptData.c_str(), MAX_RANDOM_KEY );
	char szEncode[MAX_PATH*2]="";
	int iEncodeCnt = 0;
	int iEncodPwLength = rsEncryptData.length();
	for (int i = 0; i < iEncodPwLength; i++)
	{
		if( i >= MAX_RANDOM_KEY )
		{
			szEncode[iEncodeCnt] = rsEncryptData.at(i);
			iEncodeCnt++;
			if( iEncodeCnt >= MAX_PATH*2 )
				break;
		}
	}

	char szFBIDToken[MAX_PATH*2]="";
	Help::Decode( szEncode, strlen( szEncode ), szFBIDToken, sizeof( szFBIDToken ), szRandomKey, strlen( szRandomKey ) );

	char szID[MAX_PATH]="";
	char szToken[MAX_PATH*2]="";

	enum 
	{ 
		MAX_LOOP           = 2,
		FB_ID_ARRAY        = 0,
		FB_TOKEN_ARRAY     = 1,
	};
	// parsing main token
	for (int i = 0; i < MAX_LOOP ; i++)
	{
		char *pPos = NULL;
		if( i == 0 )
			pPos = strtok( szFBIDToken, FACEBOOK_DATA_TOKEN );
		else
			pPos = strtok( NULL, FACEBOOK_DATA_TOKEN );

		if( pPos == NULL )
			break;

		if( i == FB_ID_ARRAY )
		{
			StringCbCopy( m_szLoginID, sizeof( m_szLoginID ), pPos );
		}
		else if( i == FB_TOKEN_ARRAY )
		{
			StringCbCopy( m_szLoginEncodePW, sizeof( m_szLoginEncodePW ), pPos );
		}
	}

	HWND hWnd = ::FindWindow( NULL, "LostSaga FB Login" );
	if(hWnd != NULL)
	{
		COPYDATASTRUCT cds;
		cds.dwData = 0;
		cds.cbData = 0;
		cds.lpData = NULL;
		::SendMessage(hWnd, WM_COPYDATA,0,(LPARAM)&cds);
	}

	SetUserType( "FB" );

	CAutoUpgradeDlg* pDlg = NULL;
	if(AfxGetApp())
		pDlg = (CAutoUpgradeDlg*) AfxGetApp()->GetMainWnd();
	if( pDlg )
		pDlg->SetLoginTimer();
}