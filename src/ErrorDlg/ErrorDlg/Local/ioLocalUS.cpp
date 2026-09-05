#include "../stdafx.h"
#include ".\iolocalus.h"
#include "../resource.h"
//#include "../SP2Packet.h"
//#include "../Setting.h"
//#include "../ioApplication.h"
//#include "../WndID.h"
//#include "../GUI/ioSP2GUIManager.h"
//#include "../ioBrowserManager.h"

ioLocalUS::ioLocalUS(void)
{
}

ioLocalUS::~ioLocalUS(void)
{

}

ioLocalManager::LocalType ioLocalUS::GetType()
{
	return ioLocalManager::LCT_US;
}

const char * ioLocalUS::GetTextListFileName()
{
	return "text_us.txt";
}

const char *ioLocalUS::GetMemTextList()
{
	return "|EXE_WinMain_1|An error occurred from the security program.|"//Except Extracting Hangeul
		   "|EXE_WinMain_2|Lost Saga is already running.|"//Except Extracting Hangeul
		   "|EXE_WinMain_3|    Cannot detect DirectX\r\nPlease install the latest version of DirectX.\r\n(error code: 301)|"//Except Extracting Hangeul
		   "|EXE_WinMain_4|The installed DirectX is not up-to-date (%s).\nPlease install the latest version of DirectX.\nMake sure that you have the latest version of the graphics card driver installed.\n(error code: 302)|"//Except Extracting Hangeul
		   "|EXE_ErrorDlgProc_1|--- System Requirements ---\r\n%s\r\n\r\n--- Error Information ---\r\n%s|"//Except Extracting Hangeul
	       "|EXE_ioNProtect::GetStartErrorMsg_1|게임가드가 실행 중 입니다. 잠시 후나 재부팅 후에 다시 실행해보시기 바랍니다.|" //Except Extracting Hangeul
		   "|EXE_ioNProtect::GetStartErrorMsg_2|게임이 중복 실행되었거나 게임가드가 이미 실행 중 입니다. 게임 종료 후 다시 실행해보시기 바랍니다.|" //Except Extracting Hangeul
		   "|EXE_ioNProtect::GetStartErrorMsg_3|게임가드 초기화 에러입니다. 재부팅 후 다시 실행해보거나 충돌할 수 있는 다른 프로그램들을 종료한 후 실행해 보시기 바랍니다.|" //Except Extracting Hangeul
		   "|EXE_ioNProtect::GetStartErrorMsg_4|게임가드 파일이 없거나 변조되었습니다. 게임가드 셋업 파일을 설치해보시기 바랍니다.|" //Except Extracting Hangeul
		   "|EXE_ioNProtect::GetStartErrorMsg_5|윈도우의 일부 시스템 파일이 손상되었습니다. 인터넷 익스플로러(IE)를 다시 설치해보시기 바랍니다.|" //Except Extracting Hangeul
		   "|EXE_ioNProtect::GetStartErrorMsg_6|게임가드 실행에 실패했습니다. 게임가드 셋업 파일을 다시 설치해보시기 바랍니다.|" //Except Extracting Hangeul
		   "|EXE_ioNProtect::GetStartErrorMsg_7|불법 프로그램이 발견되었습니다. 불필요한 프로그램을 종료한 후 다시 실행해보시기 바랍니다.|" //Except Extracting Hangeul
		   "|EXE_ioNProtect::GetStartErrorMsg_8|게임가드 업데이트를 취소하셨습니다. 접속이 계속 되지 않을 경우 인터넷 및 개인 방화벽 설정을 조정해 보시기 바랍니다.|" //Except Extracting Hangeul
		   "|EXE_ioNProtect::GetStartErrorMsg_9|게임가드 업데이트 서버 접속에 실패하였습니다. 잠시 후 다시 접속하거나, 네트웍 상태를 점검해봅니다.|" //Except Extracting Hangeul
		   "|EXE_ioNProtect::GetStartErrorMsg_10|게임가드 초기화 에러 또는 구버젼의 게임가드 파일입니다. 게임가드 셋업파일을 다시 설치하고 게임을 실행해봅니다.|" //Except Extracting Hangeul
		   "|EXE_ioNProtect::GetStartErrorMsg_11|ini 파일이 없거나 변조되었습니다. 게임가드 셋업 파일을 설치하면 해결할 수 있습니다.|" //Except Extracting Hangeul
		   "|EXE_ioNProtect::GetStartErrorMsg_12|npgmup.des 초기화 에러입니다. 게임가드폴더를 삭제후 다시 게임실행을 해봅니다.|" //Except Extracting Hangeul
		   "|EXE_ioNProtect::GetStartErrorMsg_13|게임가드 업데이트 서버 접속에 실패하였습니다. 잠시 후 재시도 해보거나, 개인 방화벽이 있다면 설정을 조정해 보시기 바랍니다.|" //Except Extracting Hangeul
		   "|EXE_ioNProtect::GetStartErrorMsg_14|게임가드 업데이트를 완료하지 못 했습니다. 바이러스 백신을 일시 중시 시킨 후 재시도 해보시거나, PC 관리 프로그램을 사용하시면 설정을 조정해 보시기 바랍니다.|" //Except Extracting Hangeul
		   "|EXE_ioNProtect::GetStartErrorMsg_15|바이러스 및 해킹툴 검사 모듈 로딩에 실패 했습니다. 메모리 부족이거나 바이러스에 의한 감염일 수 있습니다.|" //Except Extracting Hangeul
		   "|EXE_ioNProtect::GetStartErrorMsg_16|게임가드와 충돌 프로그램이 발견되었습니다.|" //Except Extracting Hangeul
		   "|EXE_ioNProtect::GetStartErrorMsg_17|게임가드 실행 중 에러가 발생하였습니다. 게임 폴더 안의 GameGuard 폴더에 있는 *.erl 파일들을 Game1@inca.co.kr로 첨부하여 메일 보내주시기 바랍니다.|"; //Except Extracting Hangeul  
}

DWORD ioLocalUS::GetResourceIDTitleCashBmp()
{
	return IDB_WEB_TITLE_CASH_US;
}

DWORD ioLocalUS::GetResourceIDTitleEntryBmp()
{
	return IDB_WEB_TITLE_ENTRY_US;
}

DWORD ioLocalUS::GetResourceIDTitleGuildBmp()
{
	return IDB_WEB_TITLE_GUILD_US;
}

DWORD ioLocalUS::GetResourceIDTitleGuildMarkBmp()
{
	return IDB_WEB_TITLE_GUILD_MARK_US;
}

DWORD ioLocalUS::GetResourceIDTitleHomeBmp()
{
	return IDB_WEB_TITLE_HOME_US;
}

DWORD ioLocalUS::GetResourceIDTitleBmp()
{
	return IDB_WINDOW_TITLE_LOSA;
}

void ioLocalUS::SetLoginData( const ioHashString &szBillingUserKey )
{
	m_sLoginEncodePW = szBillingUserKey;
}

//void ioLocalUS::FillLoginData( SP2Packet &rkPacket )
//{
//	rkPacket << m_sLoginEncodePW;
//
//	// 더이상 필요가 없으므로 삭제
//	m_sLoginEncodePW.Clear();
//}


void ioLocalUS::GetClassTypeForTutorialDropItem( int &riClassType )
{
	enum { CLASS_TYPE_SHADOW = 12, };
	riClassType = CLASS_TYPE_SHADOW;
}


int ioLocalUS::GetFirstIDMaxSize()
{
	return 12;
}

bool ioLocalUS::IsShowFirstIDWnd()
{
	return true;
}

void ioLocalUS::OpenFillCashPage()
{
	//if( g_BrowserMgr.GetCashURL().IsEmpty() )
	//	return;

	//char szFullURL[MAX_PATH*2]="";
	//StringCbPrintf( szFullURL, sizeof( szFullURL ), "%s?userid=%s&loginToken=%s", g_BrowserMgr.GetCashURL().c_str(), g_MyInfo.GetPrivateID().c_str(), m_sWebLoginKey.c_str() );

	//if( Setting::FullScreen() )
	//	ShowWindow( g_App.GetHWnd(), SW_MINIMIZE );

	//Help::ExcuteWeb( szFullURL );
}

const char * ioLocalUS::GetGuildMasterPostion()
{
	return "Leader"; //Except Extracting Hangeul
}

const char * ioLocalUS::GetGuildSecondMasterPosition()
{
	return "Officer"; //Except Extracting Hangeul
}

const char * ioLocalUS::GetGuildGeneralPosition()
{
	return "Member"; //Except Extracting Hangeul
}

bool ioLocalUS::IsRightNewID( const char *szID )
{
	int size = strlen(szID);
	for (int i=0; i<size; i++)
	{
		if ((!COMPARE(szID[i], 'A', 'Z'+1)) &&
			(!COMPARE(szID[i], 'a', 'z'+1)) &&
			(!COMPARE(szID[i], '0', '9'+1)) &&
					  szID[i] != '!' &&
			          szID[i] != '$' &&
					  szID[i] != '^' &&
					  szID[i] != '*' &&
					  szID[i] != '(' &&
					  szID[i] != ')' &&
					  szID[i] != '_' &&
					  szID[i] != '-' &&
					  szID[i] != '=' &&
					  szID[i] != '+' &&
					  szID[i] != '|' &&
					  szID[i] != '[' &&
					  szID[i] != ']' &&
					  szID[i] != '{' &&
					  szID[i] != '}' &&
					  szID[i] != '<' &&
					  szID[i] != '>' &&
					  szID[i] != '?' &&
					  szID[i] != '~' &&
					  szID[i] != '.' &&
					  szID[i] != ',' )
		{
			return false;
		}
	}
	return true;
}

void ioLocalUS::SetTimeZone()
{
	// 유저PC설정이 다를 수 있으므로 정해진 [타임존]으로 변경
	putenv("TZ=GMT+7"); // 미국 // 써머타임이면 : TZ=GMT+7 | 써머타임 아니면 : TZ=GMT+8
	_tzset();
	//
}

bool ioLocalUS::IsSendGashaponList()
{
	return true;
}

void ioLocalUS::GetChangedString( OUT char *szCheckString )
{
	strlwr( szCheckString );
}

bool ioLocalUS::IsExecuteLuncher()
{
	return false;
}

bool ioLocalUS::IsKoreaGameLimit()
{
	return false;
}

bool ioLocalUS::IsCheckKorean()
{
	return false;
}

bool ioLocalUS::IsMyHomePage()
{
	return false;
}

bool ioLocalUS::IsShowExtraGoodsList()
{
	return true;
}

bool ioLocalUS::IsShowRegulation()
{
	return false;
}

DWORD ioLocalUS::GetResourceIDErrorBack()
{
	return IDB_BACK_ERROR_US;
}

DWORD ioLocalUS::GetResourceIDErrorExitC()
{
	return IDB_EXIT_C_US;
}

DWORD ioLocalUS::GetResourceIDErrorExitN()
{
	return IDB_EXIT_N_US;
}

DWORD ioLocalUS::GetResourceIDErrorExitO()
{
	return IDB_EXIT_O_US;
}

DWORD ioLocalUS::GetResourceIDErrorSolutionC()
{
	return IDB_SOLUTION_C_US;
}

DWORD ioLocalUS::GetResourceIDErrorSolutionN()
{
	return IDB_SOLUTION_N_US;
}

DWORD ioLocalUS::GetResourceIDErrorSolutionO()
{
	return IDB_SOLUTION_O_US;
}

const char * ioLocalUS::GetErrorSolutionURL()
{
	return "http://lostsaga.wemadeusa.com";
}

bool ioLocalUS::SetPresentRecvDisableMsg( int iPresentType )
{
	return false;
}

int ioLocalUS::GetLimitGradeLevel()
{
	return -1;
}

const char * ioLocalUS::GetBillingErrorMent()
{
	return "WemadeUSA Error : ";
}

int ioLocalUS::GetLicenseDate()
{
	return 20131130;
}

void ioLocalUS::SetWebLoginKey( const ioHashString &rsWebLoginKey )
{
	m_sWebLoginKey = rsWebLoginKey;
}
