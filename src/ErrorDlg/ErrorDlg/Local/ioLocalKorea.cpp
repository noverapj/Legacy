#include "../stdafx.h"
#include ".\iolocalkorea.h"
#include "../resource.h"
//#include "../ioMyInfo.h"
//#include "../ioApplication.h"
//#include "../Channeling/ioChannelingNodeParent.h"
//#include "../Channeling/ioChannelingNodeManager.h"

ioLocalKorea::ioLocalKorea(void)
{
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
	return "|EXE_WinMain_1|보안프로그램에서 에러가 발생하였습니다.|" //Except Extracting Hangeul
		   "|EXE_WinMain_2|로스트사가가 이미 실행 중 입니다.|" //Except Extracting Hangeul
		   "|EXE_WinMain_3|    DirectX를 찾을수 없습니다\r\n최신버전의 DirectX를 설치해주세요.\r\n(오류번호:301)|" //Except Extracting Hangeul
		   "|EXE_WinMain_4|설치된 DirectX의 버전이 낮습니다(%s)\n최신버전의 DirectX를 설치해주세요\n그래픽카드의 드라이버도 최신버전인지 확인해주세요\n(오류번호:302)|" //Except Extracting Hangeul
		   "|EXE_WinMain_5|게임가드 에러 : %d|" //Except Extracting Hangeul
	       "|EXE_ErrorDlgProc_1|--- 시스템 사양 ---\r\n%s\r\n\r\n--- 에러 정보 ---\r\n%s|" //Except Extracting Hangeul
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

DWORD ioLocalKorea::GetResourceIDTitleCashBmp()
{
	return IDB_WEB_TITLE_CASH;
}

DWORD ioLocalKorea::GetResourceIDTitleEntryBmp()
{
	return IDB_WEB_TITLE_ENTRY;
}

DWORD ioLocalKorea::GetResourceIDTitleGuildBmp()
{
	return IDB_WEB_TITLE_GUILD;
}

DWORD ioLocalKorea::GetResourceIDTitleGuildMarkBmp()
{
	return IDB_WEB_TITLE_GUILD_MARK;
}

DWORD ioLocalKorea::GetResourceIDTitleHomeBmp()
{
	return IDB_WEB_TITLE_HOME;
}

DWORD ioLocalKorea::GetResourceIDTitleBmp()
{
	return IDB_WINDOW_TITLE_LOSA;
}

void ioLocalKorea::SetLoginData( const ioHashString &szBillingUserKey )
{

}

//void ioLocalKorea::FillLoginData( OUT SP2Packet &rkPacket )
//{
//
//}

void ioLocalKorea::GetClassTypeForTutorialDropItem( int &riClassType )
{

}


int ioLocalKorea::GetFirstIDMaxSize()
{
	return 12;
}

bool ioLocalKorea::IsShowFirstIDWnd()
{
	return false;
}

void ioLocalKorea::OpenFillCashPage()
{
	/*if( g_MyInfo.IsEntryFormality() )
		g_App.OpenFillCashPage();
	else
		g_App.OpenCashEntryPage();*/
}

const char * ioLocalKorea::GetGuildMasterPostion()
{
	return "길드장"; //Except Extracting Hangeul
}

const char * ioLocalKorea::GetGuildSecondMasterPosition()
{
	return "부길드장"; //Except Extracting Hangeul
}

const char * ioLocalKorea::GetGuildGeneralPosition()
{
	return "길드원"; //Except Extracting Hangeul
}

bool ioLocalKorea::IsRightNewID( const char *szID )
{
	return true;
}

void ioLocalKorea::SetTimeZone()
{
	// 유저PC설정이 다를 수 있으므로 정해진 [타임존]으로 변경
	_putenv("TZ=GMT-9"); 
	_tzset();
	//
}

bool ioLocalKorea::IsSendGashaponList()
{
	return false;
}

void ioLocalKorea::GetChangedString( OUT char *szCheckString )
{
	// 아무일도 하지 않음
}

bool ioLocalKorea::IsExecuteLuncher()
{
	return true;
}

bool ioLocalKorea::IsKoreaGameLimit()
{
	return true;
}

bool ioLocalKorea::IsCheckKorean()
{
	return true;
}

bool ioLocalKorea::IsMyHomePage()
{
	return true;
}

bool ioLocalKorea::IsShowExtraGoodsList()
{
	return false;
}

bool ioLocalKorea::IsShowRegulation()
{
	return true;
}

DWORD ioLocalKorea::GetResourceIDErrorBack()
{
	return IDB_BACK_ERROR;
}

DWORD ioLocalKorea::GetResourceIDErrorExitC()
{
	return IDB_EXIT_C;
}

DWORD ioLocalKorea::GetResourceIDErrorExitN()
{
	return IDB_EXIT_N;
}

DWORD ioLocalKorea::GetResourceIDErrorExitO()
{
	return IDB_EXIT_O;
}

DWORD ioLocalKorea::GetResourceIDErrorSolutionC()
{
	return IDB_SOLUTION_C;
}

DWORD ioLocalKorea::GetResourceIDErrorSolutionN()
{
	return IDB_SOLUTION_N;
}

DWORD ioLocalKorea::GetResourceIDErrorSolutionO()
{
	return IDB_SOLUTION_O;
}

const char * ioLocalKorea::GetErrorSolutionURL()
{
	return "http://www.lostsaga.com/common/rgfaq.asp?errnumber=%d";
}

bool ioLocalKorea::SetPresentRecvDisableMsg( int iPresentType )
{
	return false;
}

int ioLocalKorea::GetLimitGradeLevel()
{
	return -1;
}

const char * ioLocalKorea::GetBillingErrorMent()
{
	//ioChannelingNodeParent *pChannelingNode = g_ChannelingMgr.GetNode( g_MyInfo.GetChannelingType() );
	//if( pChannelingNode )
	//	return pChannelingNode->GetBillingErrorMent();

	return "Billing Error : ";
}

int ioLocalKorea::GetLicenseDate()
{
	return 30111131;
}

DWORD ioLocalKorea::GetCashSendTime()
{
	//ioChannelingNodeParent *pNode = g_ChannelingMgr.GetNode( g_MyInfo.GetChannelingType() );
	//if( pNode )
	//	return pNode->GetCashSendTime();

	return ioLocalParent::GetCashSendTime();
}

int ioLocalKorea::GetCashPageWidth()
{
	//ioChannelingNodeParent *pChannelingNode = g_ChannelingMgr.GetNode( g_MyInfo.GetChannelingType() );
	//if( pChannelingNode )
	//{
	//	return pChannelingNode->GetCashPageWidth();
	//}

	return 0;
}

int ioLocalKorea::GetCashPageHeight()
{
	//ioChannelingNodeParent *pChannelingNode = g_ChannelingMgr.GetNode( g_MyInfo.GetChannelingType() );
	//if( pChannelingNode )
	//{
	//	return pChannelingNode->GetCashPageHeight();
	//}

	return 0;
}

void ioLocalKorea::OnCashPageHide()
{
	//ioChannelingNodeParent *pChannelingNode = g_ChannelingMgr.GetNode( g_MyInfo.GetChannelingType() );
	//if( pChannelingNode )
	//	pChannelingNode->OnCashPageHide();
}