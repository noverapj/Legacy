#include "../stdafx.h"
#include ".\iolocalkorea.h"
#include "../resource.h"
#include "../util/HttpManager.h"
#include "../Util/HelpFunc.h"
#include "../MainFrm.h"
#include <strsafe.h>
#include "../StringManager/Safesprintf.h"
//#include "stringmanager\iostringmanager.h"
#include "../StringManager\ioStringManager.h"


#include ".\ioLocalIndonesia.h"
#include "../resource.h"
#include <strsafe.h>
#include "../Util/HttpApp.h"
#include "../StringManager/ioHashString.h"
#include <mmsystem.h>
#include "../SetupAndStart.h"

ioLocalKorea::ioLocalKorea(void)
{
	ZeroMemory( m_szRegKey, sizeof( m_szRegKey ) );
	ZeroMemory( m_szStartURL, sizeof( m_szStartURL ) );
}

ioLocalKorea::~ioLocalKorea(void)
{
}

ioLocalManager::LocalType ioLocalKorea::GetType()
{
	return ioLocalManager::LCT_KOREA;
}

const char * ioLocalKorea::GetTextListFileName()
{
	return "text.txt";
}

const char * ioLocalKorea::GetMemTextList()
{
	return "|EXE_CMainFrame::OnCreate_1|\r\n\r\n로스트사가가 이미 실행 중 입니다.\r\n\r\n     잠시만 기다려 주세요.\r\n\r\n\r\n\r\n  장시간 게임이 실행되지않으면\r\n\r\n   리부팅후 다시 실행해 주세요.|"
		   "|EXE_CMainFrame::OnCreate_2|로스트사가 업데이트 중입니다. 잠시만 기다려 주세요.|"
		   "|EXE_CMainFrame::ParseCmdAndStart_1|인자값이 잘못되었습니다.\r\n잠시후 다시 이용해 주세요.(%1)\r\n(오류번호:104)|"
		   "|EXE_CMainFrame::ParseCmdAndStart_2|시작데이터값이 잘못되었습니다.\r\n잠시후 다시 이용해 주세요.(%1/%2)\r\n(오류번호:106)|"
		   "|EXE_CMainFrame::SetupAndStart_1|새롭게 설정한 폴더명이 잘못되었습니다.\r\n잠시후 다시 이용해 주세요.(%1)\r\n(오류번호:105)|"
		   "|EXE_CSetupAndStart::SetupAndStartGame_1|인자값이 변경되었습니다.\r\n잠시후 다시 이용해 주세요.(%1)\r\n(오류번호:101)|"
		   "|EXE_CSetupAndStart::SetupAndStartGame_2|자동업데이트 설치가 실패하였습니다.\r\n잠시후 다시 이용해 주세요.(%1:%2)\r\n(오류번호:102)|"
		   "|EXE_CSetupAndStart::SetupAndStartGame_3|패치정보파일 설치가 실패하였습니다.\r\n잠시후 다시 이용해 주세요.(%1:%2)\r\n(오류번호:103)|"
		   "|EXE_CSetupAndStart::SetupAndStartGame_4|실행중인 서버가 없거나 서버가 혼잡합니다.\r\n잠시후 다시 이용해 주세요.(%1)\r\n(오류번호:107)|"
		   "|EXE_ioErrorDlg::ShowErrorDlg_2|--- 시스템 사양 ---\r\n%1\r\n\r\n--- 에러 정보 ---\r\n%2|"
		   "|EXE_SetupFolderDlg::OnBnClickedChangeFolder_1|로스트사가를 설치할 폴더를 선택해 주세요.|"
		   "|EXE_SetupFolderDlg::OnBnClickedChangeFolder_2|설치 경로명이 비어 있습니다.\r\n\r\n     다시 설정해 주세요.|"
		   "|EXE_SetupFolderDlg::OnBnClickedChangeFolder_3|설치 경로는 100자를 넘을 수 없습니다.\r\n\r\n              다시 설정해 주세요.|"
		   "|EXE_SetupFolderDlg::OnBnClickedChangeFolder_4|폴더를 만들 수 없는 경로입니다.\r\n\r\n       다시 설정해 주세요.|"
		   "|EXE_SetupFolderDlg::OnBnClickedSetup_1|로스트사가 설치 준비중입니다. 잠시만 기다려주세요.|"
		   "|EXE_ioLocalKorea::GetLoginResultPostData_1|로그인 블럭된 아이피 입니다(-1).|"
		   "|EXE_ioLocalKorea::GetLoginResultPostData_2|아이디 또는 패스워드 인증 실패 했습니다(-2).|"
		   "|EXE_ioLocalKorea::GetLoginResultPostData_3|아이디 또는 패스워드가 틀립니다(-3).|"
		   "|EXE_ioLocalKorea::GetLoginResultPostData_4|채널링 회원(-4).|"
		   "|EXE_ioLocalKorea::GetLoginResultPostData_5|개인정보 제공 동의 필요합니다(-5).|"
		   "|EXE_ioLocalKorea::GetLoginResultPostData_6|셧다운제 대상 유저(0시 ~ 6시) 입니다(-6).|"
		   "|EXE_ioLocalKorea::GetLoginResultPostData_7|선택적 셧다운제 대상 유저입니다(-7).|"
		   "|EXE_ioLocalKorea::GetLoginResultPostData_8|서버 점검 중 입니다(-8).|"
		   "|EXE_ioLocalKorea::GetLoginResultPostData_9|차단 상태로 게임 접속 불가 입니다(-9).|"
		   "|EXE_ioLocalKorea::GetLoginResultPostData_10|실행중인 게임서버 없습니다(-10).|"
		   "|EXE_ioLocalKorea::GetLoginResultPostData_11|풀 접속 상태로 게임 실행 불가(-11).|"
		   "|EXE_ioLocalKorea::GetLoginResultPostData_12|내부 에러(-12)|"
		   "|EXE_ioLocalKorea::GetLoginResultPostData_13|내부 에러(-13)|"
		   "|EXE_ioLocalKorea::GetLoginResultPostData_14|로그인 테이블 유저 정보 없습니다(-14).|"
		   "|EXE_ioLocalKorea::GetLoginResultPostData_15|정의 되CMainFrame::ParseCmdAndStart지 않은 zone 타입 입니다(-15).|"
		   "|EXE_ioLocalKorea::GetLoginResultPostData_16|아이디 혹은 비밀번호가 깁니다 다시 입력해주세요.|";
}

bool ioLocalKorea::GetNewCmdLine( IN const char *szCmd, OUT char *szNewCmd, IN int iNewCmdSize )
{
	// txt 파일에서 start 정보를 읽어옴.
	char szStartData[4096]; // asp 파일의 결과 같은 4096바이트를 넘지 않는다.
	if( !ReadURLData( (LPCTSTR) m_szStartURL, szNewCmd, iNewCmdSize, szStartData, sizeof( szStartData ) ) )
		return false;

	// parsing
	if( strcmp( szStartData, "" ) == 0 )
	{
		StringCbCopy( szNewCmd, iNewCmdSize, "StartData is empty." );
		return false;
	}
	CString szCmdValue = szCmd;
	CString szCmd2 = szCmd;

	//웹에서 불필요한 데이터 삭제
	szCmdValue = szCmdValue.Mid( szCmdValue.Find(START_WEB_DIVIDE_TOKEN) + 2, szCmdValue.ReverseFind(START_WEB_DIVIDE_TOKEN) - szCmdValue.Find(START_WEB_DIVIDE_TOKEN) - 2);

	CString szKeyValue;
	CString szIPKeyValue;
	CString szGameServerID;
	CString szcpTypeValue;
	CString szOPTValue;
	CString szCheckURL;
	int iPos = szCmdValue.Find(',');
	szKeyValue = szCmdValue.Left(iPos);

	szCmdValue = szCmdValue.Mid( szCmdValue.Find(',') + 1);

	iPos = szCmdValue.Find(',');
	szIPKeyValue = szCmdValue.Left(iPos);

	szCmdValue = szCmdValue.Mid( szCmdValue.Find(',') + 1);

	iPos = szCmdValue.Find(',');
	szGameServerID = szCmdValue.Left(iPos);

	szCmdValue = szCmdValue.Mid( szCmdValue.Find(',') + 1);

	iPos = szCmdValue.Find(',');
	szcpTypeValue = szCmdValue.Left(iPos);
	szOPTValue = szCmdValue.Right(szCmdValue.GetLength() - iPos - 1);

	if( -1 != szCmd2.Find("beta") )
		szCheckURL = "http://beta.www.lostsaga.com/play/webmanagerCheck.asp?c=set&v=";
	/*else if( -1 != szCmd2.Find("QA") )
		szCheckURL = "http://172.30.82.190/play/webmanagerCheck.asp?c=set&v=";
	else if( -1 != szCmd2.Find("Dev") )
		szCheckURL = "http://172.30.82.191/play/webmanagerCheck.asp?c=set&v=";*/
	else
		szCheckURL = "http://www.lostsaga.com/play/webmanagerCheck.asp?c=set&v=";
	g_HttpMgr.SendGetData(szCheckURL,szKeyValue,false);

	char szAfterServerIDValue[MAX_PATH*2]="";
	ZeroMemory( szAfterServerIDValue, sizeof( szAfterServerIDValue ) );

	char szGameServerIDData[MAX_PATH*4]="";
	ZeroMemory( szGameServerIDData, sizeof( szGameServerIDData ) );

	enum 
	{ 
		OGP_TYPE_CHECK_VALUE		= 9,  // key value가 생략되어 있어 9번째임
		OGP_TYPE_VALUE_ZERO			= 11,
		OGP_TYPE_GAMESERVER_ID_LIST = 12, // key value가 생략되어 있어 11번째임, 앞으로는 통일성을 위해서 한국과 동일하게 처리 필요.
		OGP_TYPE_PASS				= 10,
		OPTION_TYPE_FULLZIP_VER		= 13,
		OPTION_VALUE_ONE			= 14,
		OPTION_TYPE_PARAMETER_VER	= 15,
		OGP_TYPE_END				= 16
	};

	enum ExpandType
	{
		ET_CHECK      = 0,
		ET_GARBAGE_1  = 1,
		ET_GARBAGE_2  = 2,
		ET_GARBAGE_3  = 3,
		ET_OPTIMIZE   = 4,
		ET_INI_NAME   = 5,
		ET_CHANNELING = 6,
		ET_OTP        = 7,
		ET_LOGIN      = 8,
		ET_LOCAL      = 9,
		ET_MAX,
	};

	// parsing main token
	for (int i = 0; i < OGP_TYPE_END ; i++)
	{
		char *pPos = NULL;
		if( i == 0 )
			pPos = strtok( szStartData, START_TOKEN );
		else
			pPos = strtok( NULL, START_TOKEN );

		if( pPos == NULL )
			break;

		switch(i)
		{
		case OGP_TYPE_CHECK_VALUE :
			{
				StringCbCat( szNewCmd, iNewCmdSize, START_TOKEN );

				CString szCopyedString = pPos;
				int iCurPos = 0;
				for (int i = 0; i < ET_MAX ; i++)
				{
					CString szResult = szCopyedString.Tokenize( START_EXPAND_TOKEN, iCurPos);
					if(szResult.IsEmpty()) 
					{
						continue;
					}
				
					if( i == ET_CHANNELING )
					{
						StringCbCat( szNewCmd, iNewCmdSize, szcpTypeValue );
					}
					else if( i == ET_OTP )
					{
						StringCbCat( szNewCmd, iNewCmdSize, szOPTValue );
					}
					else
					{
						StringCbCat( szNewCmd, iNewCmdSize, szResult );
					}
					if(ET_LOCAL != i)
					{
						StringCbCat( szNewCmd, iNewCmdSize, START_EXPAND_TOKEN );
					}
				}
	//			StringCbCat( szNewCmd, iNewCmdSize, pPos );
				StringCbCat( szNewCmd, iNewCmdSize, START_TOKEN );
				StringCbCat( szNewCmd, iNewCmdSize, szKeyValue );
			}
			break;
		case OGP_TYPE_PASS :
			continue;
			break;
		case OGP_TYPE_VALUE_ZERO:
			StringCbCopy( szGameServerIDData, sizeof( szGameServerIDData ), pPos );	
			break;
		case OGP_TYPE_GAMESERVER_ID_LIST:
			StringCbCat( szAfterServerIDValue, sizeof( szAfterServerIDValue ), START_TOKEN );
			StringCbCat( szAfterServerIDValue, sizeof( szAfterServerIDValue ), szIPKeyValue );
			StringCbCat( szAfterServerIDValue, sizeof( szAfterServerIDValue ), START_TOKEN );
			StringCbCat( szAfterServerIDValue, sizeof( szAfterServerIDValue ), szGameServerID );
			break;
		case OPTION_TYPE_FULLZIP_VER:
			StringCbCat( szAfterServerIDValue, sizeof( szAfterServerIDValue ), START_TOKEN );
			StringCbCat( szAfterServerIDValue, sizeof( szAfterServerIDValue ), pPos );
			break;
		case OPTION_VALUE_ONE: //기존 인자값과 맞춰주기 위해 수정
			StringCbCat( szAfterServerIDValue, sizeof( szAfterServerIDValue ), START_TOKEN );
			StringCbCat( szAfterServerIDValue, sizeof( szAfterServerIDValue ), pPos );
			break;
		case OPTION_TYPE_PARAMETER_VER:
			char version[MAX_PATH];
			StringCbCat( szAfterServerIDValue, sizeof( szAfterServerIDValue ), START_TOKEN );
			StringCbCat( szAfterServerIDValue, sizeof( szAfterServerIDValue ), itoa( PARAMETER_VERSION, version, 10 ) );
			break;
		default:
			if( i != 0 )
				StringCbCat( szNewCmd, iNewCmdSize, START_TOKEN );
			StringCbCat( szNewCmd, iNewCmdSize, pPos );
			break;
		}
	}

	char szServerID[MAX_PATH];
	ZeroMemory( szServerID, sizeof( szServerID ) );
	ParseServerIDList( szGameServerIDData, szServerID, sizeof( szServerID ) );

	StringCbCat( szNewCmd, iNewCmdSize, START_TOKEN );
	StringCbCat( szNewCmd, iNewCmdSize, szServerID );

	// save data add
	StringCbCat( szNewCmd, iNewCmdSize, szAfterServerIDValue );

	return true;
}

bool ioLocalKorea::IsRightGameServerID( const CString &rszGameServerID )
{
	return true;
}

const char * ioLocalKorea::GetRegKey()
{
	return "Software\\SP2Client\\";
}

DWORD ioLocalKorea::GetResourceIDBack()
{
	return IDB_BACK;
}

DWORD ioLocalKorea::GetResourceIDChange()
{
	return IDB_CHANGEBTN;
}

DWORD ioLocalKorea::GetResourceIDSetup()
{
	return IDB_SETUPBTN;
}

DWORD ioLocalKorea::GetResourceIDErrorBack()
{
	return IDB_ERROR_BACK;
}

DWORD ioLocalKorea::GetResourceIDErrorExit()
{
	return IDB_EXITBTN;
}

DWORD ioLocalKorea::GetResourceIDErrorSolution()
{
	return IDB_SOLUTIONBTN;
}

const char * ioLocalKorea::GetErrorSolutionURL()
{
	return "http://www.lostsaga.com/common/rgfaq.asp?errnumber=%d";
}

void ioLocalKorea::SetRegKey( const char *szReg )
{
	StringCbCopy( m_szRegKey, sizeof( m_szRegKey ), szReg );
}

void ioLocalKorea::SetStartURL( const char *szURL )
{
	StringCbCopy( m_szStartURL, sizeof( m_szStartURL ), szURL );
}

bool ioLocalKorea::IsLoginDlg()
{
/*
#ifdef SRC_KOR
	return true;
#endif*/
	return false;
}

bool ioLocalKorea::GetLoginResultPostData( OUT char *szError, IN int iErrorSize, const char *szURL, const char *szID, const char *szPW )
{
	bool bResult = false;

	// 빈값
	char szData[MAX_PATH] = {0,};
	char szReturn[2048] = {0,}; 
	char szTemp[2048] = {0, };

	strcpy(szData, "userID=");
	strcat(szData, szID);
	strcat(szData, "&");
	strcat(szData, "userPWD=");
	strcat(szData, szPW);

	if(g_HttpMgr.GetResultPostData(szURL, szData, szReturn, 1024, true))
	{
		char pos[2048] =  {0, };
 		memcpy(pos, szReturn, sizeof(szReturn));
		//로그인 로그 관련 스크립트 제거
		strtok(pos, "?");
		int iSize = strlen(pos) - 1;
		int nResult = atoi(pos + iSize);
		if(nResult == 0)
		{
			SafeSprintf( szError, sizeof( char )*MAX_PATH, STR(16));
			bResult = false;
		}
		else if(nResult == 1)
		{
			strcpy_s(szError, sizeof(szReturn), szReturn + 2 + iSize);
			bResult = true;
		}
		else if(nResult < 16)
		{
			int index = abs(nResult);
			SafeSprintf( szError, sizeof( char )*MAX_PATH, STR(index));
			bResult = false;
		}
	}

	return bResult;
}

const char * ioLocalKorea::GetFontName()
{
	return "돋움";
}

