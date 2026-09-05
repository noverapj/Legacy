

#include "stdafx.h"
#include "resource.h"

#include <TLHELP32.h> // for GetProcessIEPresentID()

#include "GUI/VoiceChatWnd.h"
#include "Version.h"

#include "Login.h"
#ifdef XTRAP
#include "Xtrap/ioXtrap.h"
#endif 
#include "Local/ioLocalUS.h"
#include "ioSP2TCPSocket.h"

#include "Channeling/ioChannelingNodeManager.h"
#include "Channeling/ioChannelingNodeParent.h"

#include <strsafe.h>
#ifdef NPROTECT
#include "nProtect/ioNProtect.h"
#endif 

#ifdef XIGNCODE
#include "XignCode/ioXignCode.h"
#endif 

extern bool g_bHappenError;
extern int  g_iErrorNumber;
extern char g_szErrorLog[2048];

static BOOL CALLBACK HandleFromPIDProc( HWND hwnd, LPARAM lParam )
{
	DWORD pid = 0;
	HANDLEToHWNDInfo* Info = (HANDLEToHWNDInfo*)lParam;
	if( !IsWindowVisible( hwnd ) ) 
		return TRUE;

	//핸들 값으로 PID 값을 조회
	GetWindowThreadProcessId( hwnd, &pid );

	if ( Info->m_dwProcessID != pid )
		return TRUE;

	Info->m_hHwnd = hwnd;
	return FALSE;
}

HWND GetHandleFromPID( DWORD pid )
{
	HANDLEToHWNDInfo Info;

	if (pid == 0) 
		return NULL;

	Info.m_dwProcessID = pid;
	Info.m_hHwnd       = NULL;

	EnumWindows( HandleFromPIDProc, (LPARAM)&Info );

	return Info.m_hHwnd;
}

CLogin::CLogin(void)
{
	memset(m_szLoginKey, 0, sizeof(m_szLoginKey));
	m_eLoginState = LS_NONE;
	m_bSingleMode = false;
	m_dwSentUDPMsgTime = 0;
	m_iCheckCnt        = 0;
	m_dwConnectPreTime = 0;
}

CLogin::~CLogin(void)
{
}

void CLogin::DebugWebLogin( OUT LPSTR &szCmdLine ) // OpenLog 전이므로 Log 사용하지 말것
{
#ifndef SHIPPING
	if(strncmp(szCmdLine, NORMAL_EXCUTE_CHECK, strlen(NORMAL_EXCUTE_CHECK)) == 0)
		return;
	FILE* fp = fopen("config/debug_connect.ini", "rt");
	if(fp == NULL) return;
	fclose(fp);

	// load
	char szURL[MAX_PATH];
	ioINILoader_e kLoader( "config/debug_connect.ini" , false);
	kLoader.SetTitle_e("debug_connect");
	kLoader.LoadString_e("login_url", "", szURL, MAX_PATH );

	PROCESS_INFORMATION IEProcessInfo;
	IELAUNCHURLINFO LunchInfo;
	ExcuteIE( IEProcessInfo, LunchInfo, szURL );

	//핸들 값을 가져오기 위해서 대기
	::SleepEx( 500, FALSE );

	DWORD dwPreTime  = timeGetTime();
	DWORD dwParentID = GetProcessIEParentID( IEProcessInfo.dwProcessId );

	//IE 탭의 부모 핸들
	HWND hParentWnd  = GetHandleFromPID( dwParentID );
	
	while( true )
	{
		::SleepEx( 500, FALSE );

		// activeX에서 값을 넣어줌
		kLoader.LoadString_e("login_key", "", m_szLoginKey, sizeof( m_szLoginKey ) );

		if( strcmp( m_szLoginKey, "" ) != 0 )
		{
			szCmdLine = m_szLoginKey;
			kLoader.SaveString( "login_key", "" );
			PostMessage(  hParentWnd, WM_CLOSE , 0, 0 );			
			break;
		}

		if( GetProcessVersion( dwParentID ) == NULL )
		{			
			PostMessage(  hParentWnd, WM_CLOSE , 0, 0 );			
			break;
		}

		if( timeGetTime() - dwPreTime >= 300000 )
		{			
			PostMessage(  hParentWnd, WM_CLOSE , 0, 0 );			
			break;
		}
	}
#endif
}
#ifdef NEXON_IP
bool CLogin::ParseCmd( IN const char *szCmdLine, OUT ioHashString &szKeyOnCmd, OUT ioHashString &szOptinosOnCmd, 
	OUT ioHashString &szIPKeyOnCmd, OUT ioHashString &szIPOnCmd, OUT int &iPortOnCmd )// OpenLog 전이므로 Log 사용하지 말것
#else
bool CLogin::ParseCmd( IN const char *szCmdLine, OUT ioHashString &szKeyOnCmd, OUT ioHashString &szOptinosOnCmd   
	,OUT ioHashString &szIPOnCmd, OUT int &iPortOnCmd )// OpenLog 전이므로 Log 사용하지 말것
#endif
{
	// cmd : check key option gameserverid
	enum CmdType 
	{
		CT_EXPAND = 0,
		CT_KEY    = 1,
		CT_OPTIONS= 2,
#ifdef NEXON_IP
		CT_MYIP   = 3,
		CT_IPPORT = 4,
#else
		CT_IPPORT = 3,
#endif
	};

	ioHashString szCheck;
	__int64 iIPAndPort = 0;

	std::string szCopiedCmd = szCmdLine; 
	std::string szDelims    = CMD_SECTION_TOKEN;
	StringVector vStringVector = ioStringConverter::Split( szCopiedCmd, szDelims );
	int iSize = vStringVector.size();

	// cmd parsing
	for(int i = 0; i < iSize; i++)
	{
		if( i == CT_EXPAND )
		{
			szCheck = vStringVector[i].c_str();
			ParseExpandValue( vStringVector[i] );
		}
		else if( i == CT_KEY )
			szKeyOnCmd = vStringVector[i].c_str();
		else if( i == CT_OPTIONS )
			szOptinosOnCmd = vStringVector[i].c_str();
#ifdef NEXON_IP
		else if( i == CT_MYIP )
		{
			szIPKeyOnCmd = vStringVector[i].c_str();
			
		}
#endif
		else if( i == CT_IPPORT )
		{
			iIPAndPort = _atoi64( vStringVector[i].c_str() );
			LOG.PrintTimeAndLog( 0, "[Login] %s)", vStringVector[i].c_str() );
		}		
	}
	
	unsigned char szIPAndPort[MAX_PATH];
	memset( szIPAndPort, 0, MAX_PATH );
	memcpy(szIPAndPort, &iIPAndPort, 8);

	char szIP[MAX_PATH];
	StringCbPrintf(szIP, sizeof( szIP ), "%d.%d.%d.%d", szIPAndPort[0], szIPAndPort[1], szIPAndPort[2], szIPAndPort[3]);
	szIPOnCmd = szIP;
	memcpy(&iPortOnCmd, &szIPAndPort[4], 4);

	// cmd check
	if(szCheck.IsEmpty() 
	|| szKeyOnCmd.IsEmpty()
	|| szOptinosOnCmd.IsEmpty() )
		return false;
#ifdef NPROTECT 
	_ENCSTR("NOMESSAGEBOX", szForProtect);
	if(strncmp(szCheck.c_str(), szForProtect, strlen(szForProtect) ))
	{
		g_ioNProtect.SetMessageBox(false);
		return true;
	}
#endif
	if(strncmp(szCheck.c_str(), NORMAL_EXCUTE_CHECK, strlen(NORMAL_EXCUTE_CHECK)) != 0)
		return false;

	return true;
}

void CLogin::ParseExpandValue( const std::string &rszExpandValue )
{
	// expand value안에 확장 타입이 있음
	// EDEW3940FVDP4950,10,20,30,1,autoupgrade_info.ini,300,39eidko2043049dkk
	// EDEW3940FVDP4950 : check value / 
	// 10 : 램 512M 이하에서 쓰레기 파일이 10메가를 넘으면 쓰레기 파일 정리 (10 숫자는 가변)
	// 20 : 램 512M~1G  에서 쓰레기 파일이 20메가를 넘으면 쓰레기 파일 정리 (20 숫자는 가변)
	// 30 : 램 1G   이상에서 쓰레기 파일이 30메가를 넘으면 쓰레기 파일 정리 (30 숫자는 가변)
	// 1  : 최적화를 클릭해도 최적화를 하지 않는 기준 용량 나타내면 1메가   (1  숫자는 가변)
	// autoupgrade_info.ini : ini 파일 이름 ( 채널링 마다 다르다 )
	// 300: ChannelingType 
	// 39eidko2043049dkk : mgame OTP 값.
	// 1 : 게시판 사용 가능 여부 1 : 사용 가능     0 : 사용 불가 ( 일반유저는 무조건 1 )
	// 119483910 : 로컬 타입 ( LocalType )
	// 1a1204966af942deb5d299bf19741f02 : OGP Key

	// parse
	enum { INI_NAME_POS = 5, OGP_KEY_POS = 10, };
	std::string szDelims = CMD_EXPAND_SECTION_TOKEN;
	StringVector vStringVector =  ioStringConverter::Split( rszExpandValue, szDelims );
	int iSize = vStringVector.size();
	ioHashString szBillingUserKey;
	for (int i = 0; i < iSize ; i++)
	{
		if( i == INI_NAME_POS )
		{
			ioHashString szININame = vStringVector[i].c_str();
			g_App.SetAutoupgradeInfoININame( szININame );
		}
		else if( i == OGP_KEY_POS )
		{
			szBillingUserKey = vStringVector[i].c_str();
		}
	}

	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	if( pLocal )
		pLocal->SetLoginData( szBillingUserKey );
}

bool CLogin::DeleteDirectoryInFile( const char *DirName, const char *FileExten )
{
	WIN32_FIND_DATA wfd;
	HANDLE hHandle = 0L;
	char fName[MAX_PATH] = "";
	char szPath[MAX_PATH] = "";
	strcat( szPath, DirName );
	strcat( szPath, FileExten );
	BOOL bLoopResult = true;

	memset( &wfd, 0, sizeof( WIN32_FIND_DATA ) );
	hHandle = FindFirstFile( szPath, &wfd );
	if( strcmp( wfd.cFileName, "" ) != 0 )
	{
		while( bLoopResult )
		{
			if( !(wfd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY) )
			{
				strcpy( fName, DirName );
				strcat( fName, wfd.cFileName );
				DeleteFile( fName );
			}
			bLoopResult = FindNextFile( hHandle, &wfd );
		}
	}
	return true;
}
#ifdef NEXON_IP
bool CLogin::LoginLoop( const ioHashString &szKey, const ioHashString &szIPKey )
#else
bool CLogin::LoginLoop( const ioHashString &szKey )
#endif
{
	MSG msg;
	m_dwConnectPreTime = timeGetTime();

	// 중간에 다운으로 프로그램이 종료되야하는 상황에서도 계속 루프가 돌아서.. 무한루프대신에 살아있는지로 체크
	while( !g_App.IsExitProgram() )
	{
		if( PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE ) )
		{
			if( !GetMessage( &msg, NULL, 0, 0 ) )
			{
				break;
			}

			TranslateMessage( &msg );
			DispatchMessage( &msg );
			continue;
		}
		else
		{
			switch(m_eLoginState)
			{
			case LS_RECEIVE_MSG_ERROR:
				return false;
			case LS_USER_CLICK_EXIT:
				return false;
			case LS_RECEIVE_MSG_NORMAL:
				return true;
			}

			DWORD dwPastTime = timeGetTime() - m_dwConnectPreTime;
		
			enum { MAX_WAIT_MS = 600000, }; // 10분

			if( !g_App.IsTCPConnected() && ( dwPastTime >= MAX_WAIT_MS ) ) // TCP 연결 실패
			{
				SendMsgToAutoUpgrade(WM_MY_MSG_C_ERROR);
				Setting::Initialize("error");
				g_bHappenError = true;
				g_iErrorNumber = 303; // 오류번호

				char szAllProcessName[MAX_PATH*2]="";
				Help::GetAllProcessName( szAllProcessName, sizeof( szAllProcessName ) );
				SafeSprintf( g_szErrorLog, sizeof(g_szErrorLog), STR(1), szAllProcessName );

				// 소켓통신이 안되는 구간이므로 ini 기록하고 나중에 전송.
				Help::SaveClientError( g_iErrorNumber, timeGetTime() - m_dwConnectPreTime, 0 );
				return false;
			}

			if( ( m_eLoginState == LS_SENT_MSG ) && ( dwPastTime >= MAX_WAIT_MS ) ) // TCP 응답 못 받음.
			{
				SendMsgToAutoUpgrade(WM_MY_MSG_C_ERROR);
				Setting::Initialize("error");
				g_bHappenError = true;
				g_iErrorNumber = 324; // 오류번호

				char szAllProcessName[MAX_PATH*2]="";
				Help::GetAllProcessName( szAllProcessName, sizeof( szAllProcessName ) );
				SafeSprintf( g_szErrorLog, sizeof(g_szErrorLog), STR(4), szAllProcessName );

				// 소켓통신이 안되는 구간이므로 ini 기록하고 나중에 전송.
				Help::SaveClientError( g_iErrorNumber, timeGetTime() - m_dwConnectPreTime, 0 );
				return false;
			}

			if( ( ( m_eLoginState == LS_SENT_UDP_MSG ) && ( dwPastTime >= MAX_WAIT_MS ) ) || !P2PNetwork::IsNetworkPlaying()  ) // UDP 실패
			{
				SendMsgToAutoUpgrade(WM_MY_MSG_C_ERROR);
				Setting::Initialize("error");
				g_bHappenError = true;
				g_iErrorNumber = 304;
				char szAllProcessName[MAX_PATH*2]="";
				Help::GetAllProcessName( szAllProcessName, sizeof( szAllProcessName ) );
				SafeSprintf( g_szErrorLog, sizeof(g_szErrorLog), STR(2), szAllProcessName );
				char szErrorLog[MAX_PATH*3]="";
				SafeSprintf(szErrorLog, sizeof(szErrorLog), STR(3), g_MyInfo.GetPublicID().c_str(), m_szMyIP.c_str(), m_iCheckCnt, szAllProcessName );
				SP2Packet kPacket( LUPK_LOG );
				kPacket << "CheckError";  // 로그 파일 타입
				kPacket << szErrorLog;
				kPacket << 304;  // 오류번호
				kPacket << true; // write db
				P2PNetwork::SendLog(kPacket);

				return false;
			}

			SendUDPMsg();
#ifdef NEXON_IP
			SendLogin(szKey, szIPKey);
#else
			SendLogin(szKey);
#endif
		}

		::SleepEx(1, FALSE);
	}

	if( g_App.IsExitProgram() )
		return false;

	return true;
}

void CLogin::SetAdminSettings()
{
	typedef struct OptionRegInfo
	{
		ioHashString szValue;
		int iMaxData;
	}OptionRegInfo;

	const OptionRegInfo OptionRegInfos[OT_MAX] = { 
		{"", -1},
		{"", -1},
		{"", -1},
		{"", -1},
		{"PreMode", 0}, // 아래에서 별도로 계산함.
		{"FullScreen", 1},
		{"ShowBoundBox", 1},
		{"ShowExtraInfo", 1},
		{"ShowMemoryUsage", 1},
		{"",-1},
		{"",-1},
		{"",-1},
		{"ModeSub", 50}, 
		{"ModeMap", 50}, 
	};

	for (int i = 0; i < OT_MAX ; i++)
	{
		if(i >= m_szOptions.Length())
			continue;
		
		if(OptionRegInfos[i].szValue.IsEmpty())
			continue;

		char szTempData[2];
		szTempData[0] = m_szOptions.At(i);
		szTempData[1] = '\0';

		int iData = GetNumber( szTempData );

		if(i == OT_GAME_MODE)
		{
			SetSetting(i, iData);
		}
		else if(COMPARE(iData, 0, OptionRegInfos[i].iMaxData+1))
		{
			SetSetting(i, iData);
		}
	}

	Setting::SetShowBoundSphere( false );
}

void CLogin::SetSetting( const int iType , const int iData )
{
	bool bData = ( (iData == 1) ? true : false );

	switch(iType)
	{
	case OT_GAME_MODE:
		Setting::SetReservedMode( iData );
		break;
	case OT_FULLSCREEN:
//		Setting::SetFullScreen( bData );
		break;
	case OT_DEBUG_SHOW_BOUND_BOX:
		Setting::SetShowBoundBox( bData );
		break;
	case OT_DEBUG_SHOW_EXTRA_INFO:
		Setting::SetShowExtraInfo( bData );
		break;
	case OT_DEBUG_MEMORY_USAGE:
		Setting::SetShowMemoryUsage( bData );
		break;
	case OT_GAME_MODE_SUB:
		Setting::SetModeSub( iData );
		break;
	case OT_GAME_MODE_MAP:
		Setting::SetModeMap( iData );
		break;
	}
}
#ifdef NEXON_IP
void CLogin::SendLogin( const ioHashString &szLoginKey, const ioHashString &szIPKey )
#else
void CLogin::SendLogin( const ioHashString &szLoginKey )
#endif
{
	if(m_eLoginState >= LS_SENT_MSG)
		return;

	if( g_App.IsTCPConnected() )
	{
		char szLocalIP[LENGTH_IP];
		g_App.GetMyLocalIP( szLocalIP );

		SP2Packet kPacket( CTPK_CONNECT );
#ifdef NEXON_IP
		kPacket << szLoginKey << szIPKey << szLocalIP << FILEVER << (int) ioLocalManager::GetLocalType();
		//LOG.PrintTimeAndLog( 0, "CLogin::SendLogin (szIPKey : %s)", szIPKey );
#else
		kPacket << szLoginKey << szLocalIP << FILEVER << (int) ioLocalManager::GetLocalType();
#endif
		ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
		if( pLocal )
		{
			pLocal->FillLoginData( kPacket );
		}
		
		TCPNetwork::SendToServer( kPacket );

		m_eLoginState = LS_SENT_MSG;
		SendMsgToAutoUpgrade(WM_MY_MSG_C_CONNECT);
		m_dwConnectPreTime = timeGetTime();

		StringCbCopy( m_szLoginKey, sizeof( m_szLoginKey ), szLoginKey.c_str() );
	}
}

void CLogin::OnConnect( SP2Packet &rkPacket )
{
	int iLoginResult = 0;
	ioHashString szID="";
	char szErrorLog[MAX_PATH]="";
	
	rkPacket >> iLoginResult >> szID;
	switch( iLoginResult )
	{
	case CONNECT_OK:
		SendMsgToAutoUpgrade(WM_MY_MSG_C_CONNECT_OK);
		if( !_OnConnectOK( szID, rkPacket ) )
		{
			iLoginResult = CONNECT_EXCEPT; // happen except
			g_iErrorNumber = 322;
			SafeSprintf( g_szErrorLog, sizeof(g_szErrorLog), STR(1) );
			StringCbPrintf(szErrorLog, sizeof(szErrorLog), "%s/Login Fail (block user connect) / Server : %s", m_szMyIP.c_str(), szID);
		}
		break;
	case CONNECT_ID_NOT:
		g_iErrorNumber = 305;
		SafeSprintf( g_szErrorLog, sizeof(g_szErrorLog), STR(2) );
		StringCbPrintf(szErrorLog, sizeof(szErrorLog), "%s/Login Fail (Not exist ID) / Server : %s", m_szMyIP.c_str(), szID);
		break;
	case CONNECT_PW_NOT:
		g_iErrorNumber = 306;
		SafeSprintf( g_szErrorLog, sizeof(g_szErrorLog), STR(3) );
		StringCbPrintf(szErrorLog, sizeof(szErrorLog), "%s/Login Fail (Wrong password) / Server : %s", m_szMyIP.c_str(), szID);
		break;
	case CONNECT_ID_ALREADY:
		ShowDisconnectDlg( g_App.GetInstance() );
		g_iErrorNumber = 307;
		SafeSprintf( g_szErrorLog, sizeof(g_szErrorLog), STR(4) );
		StringCbPrintf(szErrorLog, sizeof(szErrorLog), "%s/Login Fail (duplicated connect) / Server : %s", m_szMyIP.c_str(), szID);
		break;
	case CONNECT_EXCEPT:
		{
			g_iErrorNumber = 308;
			SafeSprintf( g_szErrorLog, sizeof(g_szErrorLog), STR(5) );

			// 파트너사 에러 출력
			bool bError = false;
			rkPacket >> bError;
			if( bError )
			{
				ioHashString sServerError;
				rkPacket >> sServerError;

				ioHashString sError = "Error : ";
				ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
				if( pLocal ) 
					sError = pLocal->GetBillingErrorMent(sServerError);
				char szAddError[MAX_PATH]="";

				ioChannelingNodeParent *pNode = g_ChannelingMgr.GetNode( g_MyInfo.GetChannelingType() );
				if( pNode && pNode->GetType() == CNT_NEXON )					
				{
					StringCbPrintf( szAddError, sizeof( szAddError ), "%s", sError.c_str() ); //Except Extracting Hangeul
					StringCbCat( g_szErrorLog, sizeof( g_szErrorLog ), szAddError );
				}
				else
				{
					StringCbPrintf( szAddError, sizeof( szAddError ), "\r\n\r\n%s%s", sError.c_str(), sServerError.c_str() ); //Except Extracting Hangeul
					StringCbCat( g_szErrorLog, sizeof( g_szErrorLog ), szAddError );
				}
			}

			StringCbPrintf(szErrorLog, sizeof(szErrorLog), "%s/Login Fail (occur exception) / Server : %s", m_szMyIP.c_str(), szID);
		}
		break;
	case CONNECT_EXPIRE_KEY:
		g_iErrorNumber = 309;
		SafeSprintf( g_szErrorLog, sizeof(g_szErrorLog), STR(6) );
		StringCbPrintf(szErrorLog, sizeof(szErrorLog), "%s/Login Fail (expire key) / Server : %s", m_szMyIP.c_str());
		break;
	case CONNECT_EXITING_SERVER:
		g_iErrorNumber = 310;
		SafeSprintf( g_szErrorLog, sizeof(g_szErrorLog), STR(7) );
		StringCbPrintf(szErrorLog, sizeof(szErrorLog), "%s/Login Fail (exiting server)", m_szMyIP.c_str());
		break;
	case CONNECT_WRONG_CLIENT_VER:
		g_iErrorNumber = 312;
		SafeSprintf( g_szErrorLog, sizeof(g_szErrorLog), STR(8) );
		StringCbPrintf(szErrorLog, sizeof(szErrorLog), "%s/Login Fail (wrong client version)", m_szMyIP.c_str());
		LOG.PrintTimeAndLog(0, "%S ",szID );
		break;
	case CONNECT_WRONG_LOCAL_VER:
		g_iErrorNumber = 323;
		SafeSprintf( g_szErrorLog, sizeof(g_szErrorLog), STR(9) );
		StringCbPrintf(szErrorLog, sizeof(szErrorLog), "%s/Login Fail (wrong client version)", m_szMyIP.c_str());
		break;
	case CONNECT_GAMESERVER_FULL:
		g_iErrorNumber = 329;
		SafeSprintf( g_szErrorLog, sizeof(g_szErrorLog), "Gameserver is full.\r\n\r\n(Error:329)" );
		StringCbPrintf(szErrorLog, sizeof(szErrorLog), "%s/Login Fail (Gameserver is full)", m_szMyIP.c_str());
		break;
	case CONNECT_BLOCK_IP:
		g_iErrorNumber = 329;
		SafeSprintf( g_szErrorLog, sizeof(g_szErrorLog), "ip is blocked.\r\n\r\n(Error:329)" );
		StringCbPrintf(szErrorLog, sizeof(szErrorLog), "%s/Login Fail (ip is blocked)", m_szMyIP.c_str());		
		break;
	case CONNECT_BLOCK_USER:
		g_iErrorNumber = 332;
		SafeSprintf( g_szErrorLog, sizeof(g_szErrorLog), "Account is blocked.\r\n\r\n(Error:332)" );
		StringCbPrintf(szErrorLog, sizeof(szErrorLog), "%s/Login Fail (Account is blocked)", m_szMyIP.c_str());		
		break;
	}

	if( iLoginResult == CONNECT_ID_ALREADY ) // 로그만 전송 후 리턴
	{
 		SP2Packet kPacket( LUPK_LOG );
 		kPacket << "CheckError";    // 로그 파일 타입
 		kPacket << szErrorLog;
 		kPacket << g_iErrorNumber;  // 오류번호
 		kPacket << true;            // write db
 		P2PNetwork::SendLog(kPacket);
		return; 
	}

	ZeroMemory( m_szLoginKey, sizeof( m_szLoginKey ) );

	if( iLoginResult != CONNECT_OK )
	{
		switch( iLoginResult )
		{
		case CONNECT_BLOCK_IP:
			SendMsgToAutoUpgrade(WM_MY_MSG_C_IP_BLOCK);
			break;

		case CONNECT_BLOCK_USER:
			SendMsgToAutoUpgrade(WM_MY_MSG_C_ACCOUNT_BLOCK);
			break;

		default:
			SendMsgToAutoUpgrade(WM_MY_MSG_C_ERROR);
			break;
		}
		
		m_eLoginState = LS_RECEIVE_MSG_ERROR;
		Setting::Initialize("error");
		g_bHappenError = true;

		SP2Packet kPacket( LUPK_LOG );
		kPacket << "CheckError";    // 로그 파일 타입
		kPacket << szErrorLog;
		kPacket << g_iErrorNumber;  // 오류번호
		kPacket << true;            // write db
		P2PNetwork::SendLog(kPacket);
	}

	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	if( pLocal )
	{
		int	iMsgType = ioLocalUS::OGP_LM_LOGIN_FAIL;
		if( iLoginResult == CONNECT_OK )
			iMsgType = ioLocalUS::OGP_LM_LOGIN_OK;

		pLocal->SendWindowMsg( iMsgType );
	}
}

void CLogin::OnDisconnectAlreadyID( SP2Packet &rkPacket )
{
	int iResult = 0;
	rkPacket >> iResult;
	
	if( iResult == DISCONNECT_ALREADY_ID_OK )
	{
		g_iErrorNumber = 307;
		SafeSprintf( g_szErrorLog, sizeof(g_szErrorLog), STR(1) );
	}
	else if( iResult == DISCONNECT_ALREADY_ID_EXCEPT )
	{
		g_iErrorNumber = 307;
		SafeSprintf( g_szErrorLog, sizeof(g_szErrorLog), STR(2) );
	}
	
	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	if( pLocal )
		pLocal->SendWindowMsg( ioLocalUS::OGP_LM_LOGIN_FAIL );

	SendMsgToAutoUpgrade(WM_MY_MSG_C_SUCCESS); // 자동업그레이드 에러창 없이
	m_eLoginState = LS_RECEIVE_MSG_ERROR;
	Setting::Initialize("error");
	g_bHappenError = true;
}

bool CLogin::_OnConnectOK( const ioHashString &szID, SP2Packet &rkPacket )
{
	DWORD dwPublicIDHashCode = 0;
	g_MyInfo.SetPrivateID(szID);
	g_MyInfo.SetUserData( rkPacket, dwPublicIDHashCode );	
	Setting::Initialize( g_MyInfo.GetPublicID() );
	LOG.PrintTimeAndLog(0, "Public ID - %s", g_MyInfo.GetPublicID().c_str() );
#ifdef XTRAP
	g_ioXtrap.SetUserInfo( szID.c_str(), g_TCPSocketMgr.GetConnectedIP(), g_MyInfo.GetPublicID().c_str(), STRFILEVER, g_MyInfo.GetGradeLevel() );
#endif 
#ifdef NPROTECT
	g_ioNProtect.SendPrivateID( szID.c_str() );
#endif 
#ifdef XIGNCODE
	g_ioXignCode.OnConnect();
#endif 
	if( g_MyInfo.GetBlockType() == BKT_BLOCK ||
		g_MyInfo.GetBlockType() == BKT_NONE )
	{
		LOG.PrintTimeAndLog( 0, "Block LogOut" );
		return false;
	}

#ifndef SHIPPING // ADMIN
	SetAdminSettings();
#endif

	g_BlackListManager.LoadBlackList();
	g_KeyManager.Load();
	g_VoiceDisableListManager.LoadList();

	//UDP HOLE PUNCHING.
	SP2Packet kSendPacket( CUPK_CONNECT );
	kSendPacket << g_MyInfo.GetPublicID();
	P2PNetwork::SendToServer( kSendPacket );

	m_dwSentUDPMsgTime = timeGetTime();
	m_eLoginState      = LS_SENT_UDP_MSG;
	m_iCheckCnt        = 1; 
	m_dwConnectPreTime = timeGetTime();

	//클베때 길드 마크들로 인해 길드 폴더를 최소 1회는 삭제해야한다. 
	if( Setting::GuildFolderDel() )
	{
		if( DeleteDirectoryInFile( "Guild\\", "*.*" ) )
		{
			Setting::SetGuildFolderDel( false );
			Setting::SaveSetting( g_MyInfo.GetPublicID() );		
		}
	}

	SendPCInfo();
	g_MyInfo.SendMgameShutDownDate();

	return true;
}

void CLogin::OnUDPConnect( SP2Packet &rkPacket )
{
	if(m_eLoginState == LS_RECEIVE_MSG_NORMAL )
		return;

	m_eLoginState = LS_RECEIVE_MSG_NORMAL;
	SendMsgToAutoUpgrade( WM_MY_MSG_C_SUCCESS );

	if( g_MyInfo.IsTutorialUser() == false )
	{
		switch( g_App.GetConnectedMoveMode() )
		{
		case ioApplication::CMM_LOBBY:
			{
				ioMyRoomMode::SetMyRoomType( MRT_LOBBY );
				g_App.SetNextStage( ioGameStage::GS_LOBBY, __FUNCTION__ );
			}
			break;
		case ioApplication::CMM_PLAZA:
			{
				ioMyRoomMode::SetMyRoomType( MRT_FIND_MODE );
				g_App.SetNextStage( ioGameStage::GS_FIND_MODE, __FUNCTION__ );
				g_App.SetModeType( 0, 0, 0, PT_NONE );

				SP2Packet kPacket( CTPK_SEARCH_PLAZA_ROOM );
				kPacket << -2 << false;
				TCPNetwork::SendToServer( kPacket );
				TCPNetwork::MouseBusy( true );
				g_App.SetStepByConnectTry( true );
			}
			break;
		case ioApplication::CMM_HEADQUARTER:
			{
				SP2Packet kPacket( CTPK_JOIN_HEADQUARTERS );
				kPacket << g_MyInfo.GetPublicID() << -1 << false;
				TCPNetwork::SendToServer( kPacket );

				ioMyRoomMode::SetMyRoomType( MRT_FIND_MODE );
				g_App.SetNextStage( ioGameStage::GS_FIND_MODE, __FUNCTION__ );
				g_App.SetModeType( 0, 0, 0, PT_NONE );
				g_App.SetStepByConnectTry( true );
			}
			break;
		}
	}
	m_iCheckCnt += 10000; 
}

//IE8 이후 부터 CreateProcess()로 IE를 런칭 시키는 경우에 실행 후 올바른 프로세스 아이디나 핸들 값을 얻을 수 없음
//참조 : http://blogs.msdn.com/b/ie8kr/archive/2009/02/27/ie8-loosely-coupled-ie-lcie.aspx

void CLogin::ExcuteIE( OUT PROCESS_INFORMATION &process_info ,IN const char *szURL )
{
	STARTUPINFO si;
	ZeroMemory( &si, sizeof(si) );
	si.cb = sizeof(STARTUPINFO);
	si.dwFlags = 0;

	ZeroMemory( &process_info, sizeof(process_info) );	

	char ProcessName[1024];
	Help::RegReadString(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\IEXPLORE.EXE"
		,"","",ProcessName,sizeof(ProcessName));

	char ExecProcess[1024];
	strcpy(ExecProcess, ProcessName);
	strcat(ExecProcess, " ");
	strcat(ExecProcess, szURL);  //검색내용

	CreateProcess(NULL,ExecProcess,NULL,NULL,FALSE,0,NULL,NULL,&si,&process_info);
}


// IE8 이후 버전 부터 사용 할 함수
void CLogin::ExcuteIE( OUT PROCESS_INFORMATION &IEProcessInfo, OUT IELAUNCHURLINFO& LunchInfo, IN const char *szURL )
{
	wchar_t wURL[ MAX_PATH ];
	MultiByteToWideChar( CP_ACP, 0, szURL, -1, wURL, MAX_PATH );

	LunchInfo.cbSize = sizeof(IELAUNCHURLINFO);
	LunchInfo.dwCreationFlags = NULL;
	IELaunchURL( wURL, &IEProcessInfo, &LunchInfo );
}

int CLogin::GetProcessIEParentID( DWORD pid )
{
	PROCESSENTRY32 Entry = { sizeof(PROCESSENTRY32) };
	Entry.dwSize = sizeof( Entry );
	HANDLE hSnapshot = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
	
	bool bSuccess = false;
	Process32First( hSnapshot, &Entry );
	do
	{
		if( Entry.th32ProcessID == pid )
		{
			bSuccess = true;
		}
	}
	while( Process32Next( hSnapshot, &Entry ) );

	if( bSuccess )
		return Entry.th32ParentProcessID;
	else
		return 0;
} 

bool CLogin::ProcessOptions( const ioHashString &szOptions )
{
	char szRootDir[MAX_PATH];
	char szFullPath[MAX_PATH]="";
	::GetCurrentDirectory(MAX_PATH,szRootDir);

#ifdef SHIPPING
	g_ResourceLoader.Initialize( true );

	memset( szFullPath, 0, sizeof(szFullPath));
	StringCbPrintf( szFullPath, sizeof( szFullPath ), "%s\\rs.iop" , szRootDir );

	if( !g_ResourceLoader.AddPackFile( szFullPath , "") )
		return false;

	memset( szFullPath, 0, sizeof(szFullPath));
	StringCbPrintf( szFullPath, sizeof( szFullPath ), "%s\\map.iop" , szRootDir );
		
	if( !g_ResourceLoader.AddPackFile( szFullPath , "_map") )
		return false;

#else // ADMIN

	m_szOptions = szOptions;

	if(szOptions.Length() > OT_READ_PAC && szOptions.At(OT_READ_PAC) == '1')
	{
		g_ResourceLoader.Initialize( true );

		memset( szFullPath, 0, sizeof(szFullPath));
		StringCbPrintf( szFullPath, sizeof( szFullPath ), "%s\\rs.iop" , szRootDir );
		if( !g_ResourceLoader.AddPackFile( szFullPath , "") )
			return false;

		memset( szFullPath, 0, sizeof(szFullPath));
		StringCbPrintf( szFullPath, sizeof( szFullPath ), "%s\\map.iop" , szRootDir );
		if( !g_ResourceLoader.AddPackFile( szFullPath , "_map") )
			return false;
	}
	else
	{
		g_ResourceLoader.Initialize( false );
	}

	if(szOptions.Length() > OT_SINGLE_MODE && szOptions.At(OT_SINGLE_MODE) == '1')
	{
		SendMsgToAutoUpgrade(WM_MY_MSG_C_SUCCESS);
		m_bSingleMode = true;
		return true;
	}
#endif // SHIPPING

	return true;
}

void CLogin::SendMsgToAutoUpgrade( DWORD dwWMTYPE )
{
	HWND hWnd = FindWindow(AUTPUPGRADE_CLASS_NAME,NULL);
	if(hWnd != NULL)
	{
		static COPYDATASTRUCT cds;
		cds.dwData = dwWMTYPE;
		cds.cbData = 0;
		cds.lpData = NULL;
		SendMessage(hWnd, WM_COPYDATA, 0,(LPARAM)&cds);
	}
}

void CLogin::SendUDPMsg()
{
	// udp 메세지는 실패할 수 있으므로 1초마다 보낸다.
	if(m_eLoginState != LS_SENT_UDP_MSG) return;
	if(timeGetTime() - m_dwSentUDPMsgTime < 1000) return;

	//UDP HOLE PUNCHING.
	SP2Packet kSendPacket( CUPK_CONNECT );
	kSendPacket << g_MyInfo.GetPublicID();
	P2PNetwork::SendToServer( kSendPacket );

	m_dwSentUDPMsgTime = timeGetTime();
	m_iCheckCnt++;
}

int CLogin::GetNumber(char *szOneOption )
{
	int iReturnValue = 0;

	if( IsCharAlpha( szOneOption[0] ) )
	{
		// a : 10, .... z: 35
		// A : 36, .... Z: 61
		if( IsCharLower( szOneOption[0]) )
			iReturnValue = szOneOption[0] - 87; // a = 97 이므로 10을 만들기 위해서
		else
			iReturnValue = szOneOption[0] - 29; // A = 65 이므로 36을 만들기 위해서

	}
	else
		iReturnValue = atoi(szOneOption);

	return iReturnValue;
}

void CLogin::SetMyIP( const char *szMyIP )
{
	m_szMyIP = szMyIP;
}

void CLogin::SetLoginState( LOGIN_STATE eLoginState )
{
	if( eLoginState == LS_USER_CLICK_EXIT )
	{
		if( m_eLoginState == LS_USER_CLICK_EXIT )
			return;

		if( m_eLoginState == LS_NONE     || 
			m_eLoginState == LS_SENT_MSG )
		{
			// 소켓통신이 안되는 구간이므로 ini 기록하고 나중에 전송.
			Help::SaveClientError( 325, (int) m_eLoginState, timeGetTime() - m_dwConnectPreTime );
		}
		else
		{
			char szErrorLog[MAX_PATH*3]="";
			StringCbPrintf(szErrorLog, sizeof(szErrorLog), "User Click Exit : %s/%s/%d/%d/%d", g_MyInfo.GetPublicID().c_str(), m_szMyIP.c_str(), m_iCheckCnt, (int) m_eLoginState, timeGetTime() - m_dwConnectPreTime ); //Except Extracting Hangeul

			SP2Packet kPacket( LUPK_LOG );
			kPacket << "CheckError";  // 로그 파일 타입
			kPacket << szErrorLog;
			kPacket << 325;  // 오류번호
			kPacket << true; // write db
			P2PNetwork::SendLog(kPacket);
		}
	}

	m_eLoginState = eLoginState;
}

BOOL CLogin::LoginProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch(uMsg)
	{
	case WM_INITDIALOG:
		{
			//----------창 중간에 띄우기 ---------------------
			RECT workrect;
			// Find how large the desktop work area is
			SystemParametersInfo(SPI_GETWORKAREA, 0, &workrect, 0);
			int workwidth  = workrect.right -  workrect.left;
			int workheight = workrect.bottom - workrect.top;

			// And how big the window is
			int winwidth  = 245;
			int winheight = 155;
			// Make sure it"s not bigger than the work area
			winwidth  = min(winwidth, workwidth);
			winheight = min(winheight, workheight);

			// Now center it
			SetWindowPos( hWnd,
						  HWND_TOP,
						  workrect.left + (workwidth-winwidth) / 2,
						  workrect.top + (workheight-winheight) / 2,
						  winwidth, winheight, 
						  SWP_SHOWWINDOW);
			//----------------------------------------------------------

			SetDlgItemText(  hWnd, IDC_DISCONNECT , STR(1) );
			SetForegroundWindow( hWnd );
		}
		return TRUE;
	case WM_COMMAND:
		switch( LOWORD(wParam) )
		{
		case ID_DISCONNECT:
			{
				char szLocalIP[LENGTH_IP]="";
				g_App.GetMyLocalIP( szLocalIP );
				SP2Packet kPacket( CTPK_DISCONNECT_ALREADY_ID );
				kPacket << m_szLoginKey << szLocalIP;
				TCPNetwork::SendToServer( kPacket );

				ZeroMemory( m_szLoginKey, sizeof( m_szLoginKey ) );
				DestroyWindow( hWnd );
			}
			break;
		case ID_NO_DISCONNECT:
			{
				ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
				if( pLocal )
					pLocal->SendWindowMsg( ioLocalUS::OGP_LM_LOGIN_FAIL );

				SendMsgToAutoUpgrade(WM_MY_MSG_C_SUCCESS); // 자동업그레이드 에러창 없이
				m_eLoginState = LS_RECEIVE_MSG_ERROR;

				ZeroMemory( m_szLoginKey, sizeof( m_szLoginKey ) );
				DestroyWindow( hWnd );
			}
			break;
		}
		return TRUE;
	}

	return FALSE;
}

BOOL CALLBACK DisconnectDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	return g_App.LoginProc( hDlg, message, wParam, lParam );
}


void CLogin::ShowDisconnectDlg( HINSTANCE hInstance )
{
	CreateDialog( hInstance, MAKEINTRESOURCE(IDD_DISCONNECT), NULL, DisconnectDlgProc ); // 모달리스
}

void CLogin::SendPCInfo()
{
	enum 
	{
		SEND_NONE   = 0,
		SEND_FIRST  = 1,
		SEND_SECOND = 2,
		SEND_GRADE_LEVEL = 15, 
	};

	// check
	/*Help::CreateDefaultSettingINI();
	ioINILoader_e kIniLoder("Save\\default\\setting.ini",false);
	kIniLoder.SetTitle_e("Setting");
	int iSendType = kIniLoder.LoadInt_e("PCInfo", SEND_NONE );
	if( iSendType == SEND_SECOND )
		return;
	else if( iSendType == SEND_FIRST )
	{
		if( g_MyInfo.GetGradeLevel() < SEND_GRADE_LEVEL )
			return;

		kIniLoder.SaveInt( "PCInfo", SEND_SECOND );
	}
	else
	{
		kIniLoder.SaveInt( "PCInfo", SEND_FIRST );
	}*/

	// web version
	char szTotalWebVersion[MAX_PATH]="";
	char szWebVersion[MAX_PATH]="";
	Help::RegReadString( HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Internet Explorer", "Version", "", szWebVersion, sizeof(szWebVersion) );
	StringCbCat( szTotalWebVersion, sizeof( szTotalWebVersion ), szWebVersion );
	StringCbCat( szTotalWebVersion, sizeof( szTotalWebVersion ), "|" );
	Help::RegReadString( HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Google Chrome", "Version", "", szWebVersion, sizeof(szWebVersion) );
	StringCbCat( szTotalWebVersion, sizeof( szTotalWebVersion ), szWebVersion );
	StringCbCat( szTotalWebVersion, sizeof( szTotalWebVersion ), "|" );
	Help::RegReadString( HKEY_LOCAL_MACHINE, "Software\\Mozilla\\Mozilla Firefox", "CurrentVersion", "", szWebVersion, sizeof(szWebVersion) );
	StringCbCat( szTotalWebVersion, sizeof( szTotalWebVersion ), szWebVersion );

	char szDesktopWH[MAX_PATH]="";
	StringCbPrintf( szDesktopWH, sizeof( szDesktopWH ), "%d*%d", g_App.GetDeskTopWidth(), g_App.GetDeskTopHeight() );
	char szGameWH[MAX_PATH]="";
	StringCbPrintf( szGameWH, sizeof( szGameWH ), "%d*%d", Setting::Width(), Setting::Height() );

	// send
	SP2Packet kPacket( CTPK_PCINFO );
	kPacket << Setting::GetOSVersion() << Setting::GetDXVersion() << Setting::GetCPUName() << Setting::GetSystemMemory() 
		    << Setting::GetGraphicCardName() << szTotalWebVersion << szDesktopWH << szGameWH << Setting::FullScreen() << Setting::GetHDDSerialNumber();
	TCPNetwork::SendToServer( kPacket );

}