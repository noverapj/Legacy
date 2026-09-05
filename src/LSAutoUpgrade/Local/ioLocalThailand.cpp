#include "../stdafx.h"
#include "../resource.h"
#include "../HelpFunc.h"
#include ".\ioLocalThailand.h"
#include "../Util/md5.h"
#include <winsock.h>
#include "../Util/ioHttpManager.h"
#include <strsafe.h>
#include "../HttpApp.h"
#include "../HTTPManager.h"
#include "../StringManager/ioHashString.h"
#include "ioLocalThailandLanguage.h"
#include "../AutoUpgradeDlg.h"

ioLocalThailand::ioLocalThailand(void)
{	
	ZeroMemory( m_szRegKey, sizeof( m_szRegKey ) );
	ZeroMemory( m_szStartURL, sizeof( m_szStartURL ) );
	ZeroMemory( m_szLoginID, sizeof( m_szLoginID ) );
	ZeroMemory( m_szLoginEncodePW, sizeof( m_szLoginEncodePW ) );
	ZeroMemory( m_szOTPID, sizeof( m_szOTPID ) );
	ZeroMemory( m_szOTPEncodePW, sizeof( m_szOTPEncodePW ) );
}

ioLocalThailand::~ioLocalThailand(void)
{
	for(vDomainInfo::iterator iter = m_vDomainInfo.begin(); iter != m_vDomainInfo.end(); ++iter)
	{
		DomainInfo *pInfo = (*iter );
		if( !pInfo )
			continue;
		SAFEDELETE( pInfo );
	}
	m_vDomainInfo.clear();
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
	return  ThailandLanguage::GetMemTextList();
}

void ioLocalThailand::GetPacPassword( OUT char *szPassword, IN const int iPasswordSize, IN const int iPasswordType /*=0*/ )
{
#ifdef SHIPPING
	if( iPasswordType == 0 )
	{
		// password : K3$dls49YU#$#eoE3054
		char szEncPassWord[MAX_PASSWORD]={ -75, 44, 59, 110, 49, 82, 88, -97, 31, 114, 35, 3, 101, -61, 3, 100, 110, 58, 42, 43 };
		EncryptDecryptData( szPassword, iPasswordSize, szEncPassWord, MAX_PASSWORD, true );
	}
	else if( iPasswordType == 1 )
	{
		// password : -_495IUEVJdlsl++32ed
		char szEncPassWord[MAX_PASSWORD]={ -45, 64, 43, 51, 104, 104, 57, -29, 16, 109, 100, 75, 53, -54, 71, 10, 110, 56, 122, 123 };
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

DWORD ioLocalThailand::GetResourceIDBackBmp()
{
	return IDB_BACK_TH;
}

DWORD ioLocalThailand::GetResourceIDOptimizeBtnBmp()
{
	return IDB_OPTIMIZE_BTN_TH;
}

DWORD ioLocalThailand::GetResourceIDStartBtnBmp()
{
	return IDB_START_TH;
}

DWORD ioLocalThailand::GetResourceIDErrorBackBtnBmp()
{
	return IDB_ERROR_BACK_TH;
}

DWORD ioLocalThailand::GetResourceIDErrorSolutionBtnBmp()
{
	return IDB_SOLUTIONBTN_TH;
}

DWORD ioLocalThailand::GetResourceIDErrorExitBtnBmp()
{
	return IDB_EXITBTN_TH;
}

const char * ioLocalThailand::GetRegKey()
{
	return "Software\\SP2ClientTH\\";
}

const char * ioLocalThailand::GetFontName()
{
	return FONT_NAME_TH;
}

const char * ioLocalThailand::GetLoginFontName()
{
	return FONT_NAME_TH;
}

void ioLocalThailand::CreateShortcuts( const CString &rszRootDir, const CString &rszShortcutName, const CString &rszLoginURL )
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

const char * ioLocalThailand::GetErrorSolutionURL()
{
	return "http://ls.garena.in.th/";
}

bool ioLocalThailand::GetNewCmdLine( IN const char *szCmd, OUT char *szNewCmd, IN int iNewCmdSize )
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

	StringCbCat( szCurCmd, iNewCmdSize, m_szLoginTokenKey.c_str() );
	StringCbCat( szCurCmd, iNewCmdSize, CMD_SECTION_TOKEN );
	

	enum 
	{ 
		MAX_LOOP = 6, 
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

bool ioLocalThailand::GetLoginResultPostData( OUT char *szError, IN int iErrorSize, const char *szURL, const char *szID, const char *szPW )
{
	return true;
}

int ioLocalThailand::GetLoginKeyLiveTime()
{
	return ( LOGIN_KEY_LIVE_TIME*100 ); // 서버에서 로그인을 하므로 로그인키 생존 시간이 필요 없기때문에 아주길게 설정
}

void ioLocalThailand::SetLoginData( const char *szLoginID, const char *szLoginEncodePW )
{
	if( szLoginID == NULL )
		return;
	if( szLoginEncodePW == NULL )
		return;

	StringCbCopy( m_szLoginID, sizeof( m_szLoginID ), szLoginID );
	StringCbCopy( m_szLoginEncodePW, sizeof( m_szLoginEncodePW ), szLoginEncodePW );
}


void ioLocalThailand::SetOTPID( const char *szOTPID )
{
	if( szOTPID == NULL )
		return;
	StringCbCopy( m_szOTPID, sizeof( m_szOTPID ), szOTPID );
}

void ioLocalThailand::SetOTPEncodePW( const char *szOTPEncodePW )
{
	if( szOTPEncodePW == NULL )
		return;
	StringCbCopy( m_szOTPEncodePW, sizeof( m_szOTPEncodePW ), szOTPEncodePW );
}

bool ioLocalThailand::IsRightID( const char *szID )
{
	enum { MIN_LENGTH = 1,  };

	int iSize = strlen( szID );
	if ( iSize < MIN_LENGTH || iSize > GetMaxIDSize() )
		return false;

	for (int i=0; i<iSize; i++)
	{
		if ((!COMPARE(szID[i], 'A', 'Z'+1)) &&
			(!COMPARE(szID[i], 'a', 'z'+1)) &&
			(!COMPARE(szID[i], '0', '9'+1)) &&
			(szID[i]!='!') &&
			(szID[i]!='@') &&
			(szID[i]!='#') &&
			(szID[i]!='$') &&
			(szID[i]!='%') &&
			(szID[i]!='^') &&
			(szID[i]!='&') &&
			(szID[i]!='*') &&
			(szID[i]!='(') &&
			(szID[i]!=')') &&
			(szID[i]!='_') &&
			(szID[i]!='-') &&
			(szID[i]!='[') &&
			(szID[i]!=']') &&
			(szID[i]!='.') &&
			(szID[i]!=' ') 	)
		{
			return false;
		}
	}
	return true;
}

bool ioLocalThailand::IsRightPW( const char *szPW )
{
	enum { MIN_LENGTH = 4,  };

	int iSize = strlen( szPW );
	if ( iSize < MIN_LENGTH || iSize > GetMaxPWSize() )
		return false;

	for (int i=0; i<iSize; i++)
	{
		if ((!COMPARE(szPW[i], 'A', 'Z'+1)) &&
			(!COMPARE(szPW[i], 'a', 'z'+1)) &&
			(!COMPARE(szPW[i], '0', '9'+1)) &&
			(szPW[i]!='!') &&
			(szPW[i]!='@') &&
			(szPW[i]!='#') &&
			(szPW[i]!='$') &&
			(szPW[i]!='%') &&
			(szPW[i]!='^') &&
			(szPW[i]!='&') &&
			(szPW[i]!='*') &&
			(szPW[i]!='(') &&
			(szPW[i]!=')') &&
			(szPW[i]!='_') &&
			(szPW[i]!='-') &&
			(szPW[i]!='[') &&
			(szPW[i]!=']') &&
			(szPW[i]!=' ') 	)
		{
			return false;
		}
	}
	return true;
}

bool ioLocalThailand::IsRightOTP( const char *szOTP )
{
	int iSize = strlen( szOTP );
	if ( iSize != GetMaxOPTSize() )
		return false;

	for (int i=0; i<iSize; i++)
	{
		if ( !COMPARE(szOTP[i], '0', '9'+1) )
		{
			return false;
		}
	}
	return true;
}

const char * ioLocalThailand::GetConnectFailMent()
{
	return CONNECT_FAIL_TH;
}

const char * ioLocalThailand::GetLoginWrongIDMent()
{
	return "Wrong ID";
}

const char * ioLocalThailand::GetLoginWrongPWMent()
{
	return "Wrong Password";
}

const char * ioLocalThailand::GetEntryURL()
{
	return "http://ls.garena.in.th/";
}

const char * ioLocalThailand::GetFindIDURL()
{
	return "http://ls.garena.in.th/";
}

const char * ioLocalThailand::GetFindPWURL()
{
	return "http://ls.garena.in.th/";
}

const char * ioLocalThailand::GetLoginID()
{
	return m_szLoginID;
}

const char * ioLocalThailand::GetOTPID()
{
	return m_szOTPID;
}

void ioLocalThailand::SetDomainList()
{
/*
	enum { MAX_DOMAIN = 3, };
	char szDomainTitleList[MAX_DOMAIN][MAX_PATH]={  "@ID", "Play ID", "TCG" };
	char szDomainTokenList[MAX_DOMAIN][MAX_PATH]={ "THAS", "THPP", "THTC" };
	for (int i = 0; i < MAX_DOMAIN; i++)
	{
		DomainInfo *pInfo = new DomainInfo;
		if( !pInfo )
			continue;
		pInfo->m_sDomainTitle = szDomainTitleList[i];
		pInfo->m_sDomainToken = szDomainTokenList[i];
		m_vDomainInfo.push_back( pInfo );
	}*/

	
}

int ioLocalThailand::GetMaxDomain()
{
	return m_vDomainInfo.size();
}

void ioLocalThailand::GetDomainTitle( IN int iArray , OUT ioHashString &rsTitle )
{
	if( !COMPARE( iArray, 0, (int) m_vDomainInfo.size() ) )
		return;

	rsTitle = m_vDomainInfo[iArray]->m_sDomainTitle;
}

void ioLocalThailand::GetDomainToken( IN int iArray , OUT ioHashString &rsToken )
{
	if( !COMPARE( iArray, 0, (int) m_vDomainInfo.size() ) )
		return;

	rsToken = m_vDomainInfo[iArray]->m_sDomainToken;
}

bool ioLocalThailand::IsOTP()
{
/*
	if( strcmp( m_szOTPID, "" ) == 0 )
		return false;*/

	return true;
}


bool ioLocalThailand::DownloadStartIni( const CString & strStartINIURL, HTTPManager* pHttpMgr )
{
	enum { eFULL_PATH_SIZE = 1024, eDRIVE_PATH_SIZE = 24, eDIR_PATH_SIZE = 512, eFILE_PATH_SIZE = 256, eEXT_PATH_SIZE = 24};

	char szModuleName[1024] = {0,};
	GetModuleFileName(NULL, szModuleName, eFULL_PATH_SIZE);
	LOG.PrintTimeAndLog(0, "GetModuleFileName - %s", szModuleName);

	char szDrive[eDRIVE_PATH_SIZE] = {0,};
	char szDir[eDIR_PATH_SIZE] = {0,};
	_splitpath(szModuleName, szDrive, szDir, NULL, NULL);

	char * szInfoDir = "info\\";
	char * szININame = "start.ini";
	char * szDownName = "start_down.ini";

	CString strStartINIPath = szDrive;
	strStartINIPath += szDir;
	strStartINIPath += szInfoDir;

	CString strStartDownPath = strStartINIPath;

	strStartINIPath += szININame;
	strStartDownPath += szDownName;

	enum { eMIN_FILE_SIZE = 100, eMAX_DOWNLOAD_COUNT = 5, };

	{
		if(pHttpMgr)
		{

			DWORD dwFileSize = 0;
			bool bDownload = false;
			for(int downloadCount = 0; downloadCount < eMAX_DOWNLOAD_COUNT; ++downloadCount)
			{
				bDownload = pHttpMgr->DownLoadOneFile(strStartINIURL, 0, strStartDownPath, dwFileSize);
				if(bDownload)
				{
					//다운실패 간주
					if(dwFileSize < eMIN_FILE_SIZE)
					{
						continue;
					}
					else
					{
						// 정상 다운 같다.
						try
						{
							CFile::Remove(strStartINIPath);
							CFile::Rename(strStartDownPath, strStartINIPath);
						}
						catch( CFileException *e )
						{
							if(e)
							{
								TCHAR szError[1024];
								e->GetErrorMessage(szError, 1024);
								LOG.PrintTimeAndLog(0, "[%s], %s", __FUNCTION__, szError);
							}
						}
						break;
					}
				}
			}
			if(!bDownload)
			{
				MessageBox(NULL, "StartIni Download Fail.", "DOWNLOAD ERROR", MB_OK);
				LOG.PrintTimeAndLog(0, "CheckLoginValidation - StartIni Download Fail");
				return false; //fail down
			}
		}
		else
		{
			MessageBox(NULL, "httpMgr Isn't Exist", "DOWNLOAD ERROR", MB_OK);
			LOG.PrintTimeAndLog(0, "CheckLoginValidation - httpMgr Isn't Exist");
			return false;
		}
	}

	return true;
}

void ioLocalThailand::SetLoginTokenKey( ioHashString szToken )
{
	m_szLoginTokenKey = szToken;
}
