#include "stdafx.h"
#include ".\ionprotect.h"

#ifdef NPROTECT
#include "../NetworkWrappingFunc.h"
#include "..\ioApplication.h"
#include "..\io3DEngine\Safesprintf.h"
#include "..\ioMyInfo.h"
#include <strsafe.h>
#include "..\io3DEngine\ioStringManager.h"

#define USE_NPROTECT

#ifdef USE_NPROTECT
#ifdef NPROTECT_CSAUTH3

#if defined( SRC_TH )
CNPGameLib g_NProtectLib("LostSagaTH");
#elif defined( SRC_TW)
CNPGameLib g_NProtectLib("LostSagaTW");
#elif defined( SRC_SEA)
CNPGameLib g_NProtectLib("LostSagaSEA");
#elif defined( SRC_ID)
CNPGameLib g_NProtectLib("LostSagaID");
#else
CNPGameLib g_NProtectLib("LostSagaKR");
#endif

#else
CNPGameLib g_NProtectLib("LostSagaTW");
#endif
#endif // USE_NPROTECT
ioNProtect *ioNProtect::sg_Instance = NULL;

extern bool g_bHappenError;
extern int  g_iErrorNumber;
extern char g_szErrorLog[2048];

extern LONG __stdcall ExceptCallBack ( EXCEPTION_POINTERS * pExPtrs );

BOOL CALLBACK NPGameMonCallback(DWORD dwMsg, DWORD dwArg);

ioNProtect::ioNProtect(void)
{
	m_dwStartErrorType  = 0;
	m_iCheckCnt         = 0;
	m_bRunCallBack      = false;
	m_eCallBackType     = CBT_NONE;
	m_bMessageBox		= true;
}

ioNProtect::~ioNProtect(void)
{
}

ioNProtect & ioNProtect::GetInstance()
{
	if( sg_Instance == NULL )
		sg_Instance = new ioNProtect;

	return (*sg_Instance);
}

void ioNProtect::ReleaseInstance()
{
	if( sg_Instance )
		delete sg_Instance;

	sg_Instance = NULL;
}

bool ioNProtect::Start()
{
// try 사용할 수 없어 생략

#ifdef USE_NPROTECT
	FILE *pFile = fopen( "info/pp.log", "wt" ); // LOG 셋팅전이기 때문에 fopen으로 기록한다.
	if( pFile )
	{
		fprintf( pFile, "Check - 1 - start:%d\n", timeGetTime() );
		fflush( pFile );
	}

	//char szPath[MAX_PATH]= {0,};
	//GetCurrentDirectory( MAX_PATH, szPath);

	//SetModulePath(szPath);

	bool bSuccess = false;
	DWORD dwResult = g_NProtectLib.Init();
	if( dwResult == NPGAMEMON_SUCCESS )
	{
		LOG.PrintTimeAndLog(0, "[nProtect] - GameMon Success" );
		bSuccess = true;
	}
	else
		m_dwStartErrorType = dwResult;
	
	if( pFile )
	{
		m_dwStartErrorType = dwResult;
		LOG.PrintTimeAndLog(0, "[nProtect] - GameMon Error : %d", dwResult );
	}
	if( pFile )
		fclose( pFile );

	if( bSuccess )
		return true;

	return false;
#else // USE_NPROTECT
	return true;
#endif
}

void ioNProtect::GetStartErrorMsg( OUT char *szError, IN int iSize )
{
	if( m_dwStartErrorType == NPGAMEMON_ERROR_EXIST )
		SafeSprintf( szError, iSize, STR(1) );
	else if( m_dwStartErrorType == NPGAMEMON_ERROR_GAME_EXIST )
		SafeSprintf( szError, iSize, STR(2));		
	else if( m_dwStartErrorType == NPGAMEMON_ERROR_INIT )
		SafeSprintf( szError, iSize, STR(3));
	else if( m_dwStartErrorType == NPGAMEMON_ERROR_AUTH_GAMEGUARD || 
		     m_dwStartErrorType == NPGAMEMON_ERROR_NFOUND_GG      ||
		     m_dwStartErrorType == NPGAMEMON_ERROR_AUTH_INI       ||
		     m_dwStartErrorType == NPGAMEMON_ERROR_NFOUND_INI)
		SafeSprintf( szError, iSize, STR(4));
	else if( m_dwStartErrorType == NPGAMEMON_ERROR_CRYPTOAPI )
		SafeSprintf( szError, iSize, STR(5));		
	else if( m_dwStartErrorType == NPGAMEMON_ERROR_EXECUTE )
		SafeSprintf( szError, iSize, STR(6));
	else if( m_dwStartErrorType == NPGAMEMON_ERROR_ILLEGAL_PRG )
		SafeSprintf( szError, iSize, STR(7));			
	else if( m_dwStartErrorType == NPGMUP_ERROR_ABORT )
		SafeSprintf( szError, iSize, STR(8));	
	else if( m_dwStartErrorType == NPGMUP_ERROR_CONNECT )
		SafeSprintf( szError, iSize, STR(9));
	else if( m_dwStartErrorType == NPGAMEMON_ERROR_GAMEGUARD )
		SafeSprintf( szError, iSize, STR(10));
	else if( m_dwStartErrorType == NPGMUP_ERROR_PARAM )
		SafeSprintf( szError, iSize, STR(11));
	else if( m_dwStartErrorType == NPGMUP_ERROR_INIT )
		SafeSprintf( szError, iSize, STR(12));
	else if( m_dwStartErrorType == NPGMUP_ERROR_DOWNCFG )
		SafeSprintf( szError, iSize, STR(13));
	else if( m_dwStartErrorType == NPGMUP_ERROR_AUTH )
		SafeSprintf( szError, iSize, STR(14));
	else if( m_dwStartErrorType == NPGAMEMON_ERROR_NPSCAN )
		SafeSprintf( szError, iSize, STR(15));
	else if( m_dwStartErrorType == NPGG_ERROR_COLLISION )
		SafeSprintf( szError, iSize, STR(16));
	else
		SafeSprintf( szError, iSize, STR(17));
}

void ioNProtect::SetHwnd( HWND hWnd )
{
#ifdef USE_NPROTECT
	LOG.PrintTimeAndLog( 0, "Check - 2 - start");
	__try
	{
		g_NProtectLib.SetHwnd( hWnd );
	}
	__except(ExceptCallBack( GetExceptionInformation() ) )
	{
		LOG.PrintTimeAndLog( 0, "Check - 2 - error : %d", GetLastError() );
		return;
	}
	LOG.PrintTimeAndLog( 0, "Check - 2 - end");
#endif // USE_NPROTECT
}

void ioNProtect::SendPrivateID( const char *szPrivateID )
{
#ifdef USE_NPROTECT
	LOG.PrintTimeAndLog( 0, "Check - 3 - start");
	__try
	{
		g_NProtectLib.Send( szPrivateID );
	}
	__except(ExceptCallBack( GetExceptionInformation() ) )
	{
		LOG.PrintTimeAndLog( 0, "Check - 3 - error : %d", GetLastError() );
		return;
	}
	LOG.PrintTimeAndLog( 0, "Check - 3 - end");
#endif // USE_NPROTECT
}

void ioNProtect::Check( bool bDirectCheck )
{
#ifdef USE_NPROTECT
// try 사용할 수 없어 생략
	if( !bDirectCheck )
	{
		// udp sync에서 9초마다 호출하므로 총 81초 간격으로 nprotect check() 함수 실행  
		m_iCheckCnt++;
		if( m_iCheckCnt < MAX_CHECK_CNT )
			return;
		m_iCheckCnt = 0;
	}
	else
	{
		m_iCheckCnt = 0;
	}

	bool bError = false;	
	if( g_NProtectLib.Check() != NPGAMEMON_SUCCESS )
		bError = true;		

	if( bError )
	{
		char szError[MAX_PATH]="";
		StringCbPrintf( szError, sizeof( szError ), "GameGuard Fail Check.( PrivateID:%s:IP:%s)", g_MyInfo.GetPrivateID().c_str(), g_MyInfo.GetMyIP().c_str() );
		SP2Packet kPacket( LUPK_LOG );
		kPacket << "CheckError";
		kPacket << szError;
		kPacket << 328;  // 오류번호
		kPacket << true; // write db
		P2PNetwork::SendLog( kPacket );

		LOG.PrintTimeAndLog( 0, "ExitProgram - 23" );
		g_App.SetExitProgram();
	}
#endif // USE_NPROTECT
}


LPBYTE ioNProtect::GetHackInfo( OUT DWORD *dwSize )
{
#ifdef USE_NPROTECT
	__try
	{
		return g_NProtectLib.GetHackInfo( dwSize );
	}
	__except(ExceptCallBack( GetExceptionInformation() ) )
	{
		LOG.PrintTimeAndLog( 0, "Check - 5 - error : %d", GetLastError() );
		return NULL;
	}
#else // USE_NPROTECT
	return NULL;
#endif
}

#ifdef NPROTECT_CSAUTH3
void ioNProtect::OnRecieveCheck( BYTE *pPacket, DWORD dwPacketSize )
{
#ifdef USE_NPROTECT
	__try
	{
		if( dwPacketSize > NProtectPacket::MAX_PACKET_BUF )
		{
			LOG.PrintTimeAndLog( 0, "Check - 6 - error : size" );
			return;
		}

		g_NProtectLib.Auth3( pPacket, dwPacketSize, 0 );
	}
	__except(ExceptCallBack( GetExceptionInformation() ) )
	{
		LOG.PrintTimeAndLog( 0, "Check - 6 - error : %d", GetLastError() );
	}
#endif // USE_NPROTECT
}
#else
void ioNProtect::OnRecieveCheck( GG_AUTH_DATA &rkAuthData )
{
#ifdef USE_NPROTECT
	__try
	{
		g_NProtectLib.Auth2( &rkAuthData );
	}
	__except(ExceptCallBack( GetExceptionInformation() ) )
	{
		LOG.PrintTimeAndLog( 0, "Check - 6 - error : %d", GetLastError() );
	}
#endif // USE_NPROTECT
}
#endif

void ioNProtect::Process()
{
	if( m_eCallBackType == CBT_NONE )
		return;
	if( m_bRunCallBack )
		return;
	
	if( m_eCallBackType == CBT_ERROR_EXIT )
	{
		SP2Packet kPacket( LUPK_LOG );
		kPacket << "CheckError";
		kPacket << m_sErrorLog;
		kPacket << 328;  // 오류번호
		kPacket << true; // write db
		P2PNetwork::SendLog( kPacket );

		g_bHappenError = true;
		g_iErrorNumber = 328; // 오류번호
		StringCbCopy( g_szErrorLog, sizeof( g_szErrorLog ), m_sErrorLog.c_str() );

		LOG.PrintTimeAndLog( 0, "ExitProgram - 24" );
		g_App.SetExitProgram();
	}
	else if( m_eCallBackType == CBT_ERROR_LOG )
	{
		SP2Packet kPacket( LUPK_LOG );
		kPacket << "CheckError";
		kPacket << m_sErrorLog;
		kPacket << 330;  // 오류번호
		kPacket << true; // write db
		P2PNetwork::SendLog( kPacket );
	}
	else if( m_eCallBackType == CBT_ERROR_LOG2 )
	{
		SP2Packet kPacket( LUPK_LOG );
		kPacket << "CheckError";
		kPacket << m_sErrorLog;
		kPacket << 329;  // 오류번호
		kPacket << true; // write db
		P2PNetwork::SendLog( kPacket );
	}
	else if( m_eCallBackType == CBT_SEND_CHECK )
	{
		SP2Packet kPacket( CTPK_PROTECT_CHECK );
		kPacket << m_kCheckData;
		TCPNetwork::SendToServer( kPacket );
	}

	m_eCallBackType = CBT_NONE;
}

#ifdef NPROTECT_CSAUTH3
void ioNProtect::SetCheckData( const NProtectPacket &rkCheckData )
{
	memcpy( &m_kCheckData, &rkCheckData, rkCheckData.m_dwStructSize );
}
#endif

void ioNProtect::SetD3dDevice( IDirect3DDevice9 * pD3dDevice )
{
	if(pD3dDevice != NULL)
	{
		if(FALSE == SetD3DDeviceInfo(9, (LPVOID *)&pD3dDevice))
		{
			// 디바이스 셋팅 실패 종료처리
			LOG.PrintTimeAndLog( 0, "[%s] FAIL", __FUNCTION__ );
			g_App.SetExitProgram();
		}
	}
}

// callback 함수는 다른 쓰레드에서 동작하므로 동기화 처리함.
BOOL CALLBACK NPGameMonCallback(DWORD dwMsg, DWORD dwArg)
{
	g_ioNProtect.SetRunCallBack( true );

	char szErrorLog[MAX_PATH*2]="";
	bool bExit = false;
	if( dwMsg == NPGAMEMON_COMM_ERROR ||
	    dwMsg == NPGAMEMON_COMM_CLOSE )
	{
#if defined(SRC_TH) || defined(SRC_TW)
		SafeSprintf( szErrorLog, sizeof(szErrorLog), STR(1) );
#else
		SafeSprintf( szErrorLog, sizeof(szErrorLog), STR(1), g_MyInfo.GetPrivateID().c_str(), g_MyInfo.GetMyIP().c_str() );
#endif
		if(g_ioNProtect.GetMessageBox())
			Help::MessageboxShow(NULL, STR(1),"LostSaga - Warning",MB_OK);
		bExit = true;
	}
	else if( dwMsg == NPGAMEMON_INIT_ERROR )
	{
#if defined(SRC_TH) || defined(SRC_TW)
		SafeSprintf( szErrorLog, sizeof(szErrorLog), STR(2) );
#else
		SafeSprintf( szErrorLog, sizeof(szErrorLog), STR(2), g_MyInfo.GetPrivateID().c_str(), g_MyInfo.GetMyIP().c_str() );
#endif
		if(g_ioNProtect.GetMessageBox())
			Help::MessageboxShow(NULL, STR(2),"LostSaga - Warning",MB_OK);
		bExit = true;
	}
	else if( dwMsg == NPGAMEMON_SPEEDHACK )
	{
#if defined(SRC_TH) || defined(SRC_TW)
		SafeSprintf( szErrorLog, sizeof(szErrorLog), STR(3) );
#else
		SafeSprintf( szErrorLog, sizeof(szErrorLog), STR(3), g_MyInfo.GetPrivateID().c_str(), g_MyInfo.GetMyIP().c_str() );
#endif
		if(g_ioNProtect.GetMessageBox())
			Help::MessageboxShow(NULL, STR(3),"LostSaga - Warning",MB_OK);
		bExit = true;
	}
	else if( dwMsg == NPGAMEMON_GAMEHACK_KILLED )
	{
#if defined(SRC_TH) || defined(SRC_TW)
		SafeSprintf( szErrorLog, sizeof(szErrorLog), STR(4) );
#else
		SafeSprintf( szErrorLog, sizeof(szErrorLog), STR(4), g_MyInfo.GetPrivateID().c_str(), g_MyInfo.GetMyIP().c_str() );
#endif
		if(g_ioNProtect.GetMessageBox())
			Help::MessageboxShow(NULL, STR(4),"LostSaga - Warning",MB_OK);
		bExit = true;
	}
	else if( dwMsg == NPGAMEMON_GAMEHACK_DETECT )
	{
#if defined(SRC_TH) || defined(SRC_TW)
		SafeSprintf( szErrorLog, sizeof(szErrorLog), STR(5) );
#else
		SafeSprintf( szErrorLog, sizeof(szErrorLog), STR(5), g_MyInfo.GetPrivateID().c_str(), g_MyInfo.GetMyIP().c_str() );
#endif
		if(g_ioNProtect.GetMessageBox())
			Help::MessageboxShow(NULL, STR(5),"LostSaga - Warning",MB_OK);
		bExit = true;
	}
	else if( dwMsg == NPGAMEMON_GAMEHACK_DOUBT )
	{
#if defined(SRC_TH) || defined(SRC_TW)
		SafeSprintf( szErrorLog, sizeof(szErrorLog), STR(6) );
#endif
		/*SafeSprintf( szErrorLog, sizeof(szErrorLog), STR(6), g_MyInfo.GetPrivateID().c_str(), g_MyInfo.GetMyIP().c_str() );
		bExit = true;*/
		LOG.PrintTimeAndLog( 0, "ExitProgram - 25" );
		// 게임 종료 없이 로그만 전송
		SafeSprintf( szErrorLog, sizeof(szErrorLog), STR(6), g_MyInfo.GetPrivateID().c_str(), g_MyInfo.GetMyIP().c_str(), "/329/" );//329 게임핵 의심 종료안함
		g_ioNProtect.SetErrorLog( szErrorLog );
		g_ioNProtect.SetCallBackType( ioNProtect::CBT_ERROR_LOG2 );
	}
	else if( dwMsg == NPGAMEMON_GAMEHACK_REPORT )
	{
		// 게임 종료 없이 로그만 전송
		DWORD dwHackInfoSize = 0;
		LPBYTE pHackInfo = NULL;
		pHackInfo = g_ioNProtect.GetHackInfo( &dwHackInfoSize ); 
		if ( pHackInfo && dwHackInfoSize > 0 )
		{
			StringCbPrintf( szErrorLog, sizeof( szErrorLog ), "( PrivateID:%s|IP:%s) %s", g_MyInfo.GetPrivateID().c_str(), g_MyInfo.GetMyIP().c_str(), pHackInfo, "/330/" );
			g_ioNProtect.SetErrorLog( szErrorLog );
			g_ioNProtect.SetCallBackType( ioNProtect::CBT_ERROR_LOG );
		}
	}
#ifdef NPROTECT_CSAUTH3
	else if( dwMsg == NPGAMEMON_CHECK_CSAUTH3 )
	{
		PCSAuth3Data pCSAuth3 = (PCSAuth3Data) dwArg;
		if( pCSAuth3 )
		{
			if( pCSAuth3->dwPacketSize <= NProtectPacket::MAX_PACKET_BUF )
			{
				NProtectPacket kAuthData;
				kAuthData.m_dwStructSize = pCSAuth3->dwPacketSize + sizeof(DWORD); // sizeof(DWORD)는 사이즈 공간 
				memcpy( kAuthData.m_byNProtectPacket, pCSAuth3->bPacket, pCSAuth3->dwPacketSize );
				g_ioNProtect.SetCheckData( kAuthData );
				g_ioNProtect.SetCallBackType( ioNProtect::CBT_SEND_CHECK );
			}
		}
	}
#else
	else if( dwMsg == NPGAMEMON_CHECK_CSAUTH2 )
	{
		GG_AUTH_DATA kAuthData;
		memcpy(&kAuthData, (PVOID)dwArg, sizeof(GG_AUTH_DATA));

		g_ioNProtect.SetCheckData( kAuthData );
		g_ioNProtect.SetCallBackType( ioNProtect::CBT_SEND_CHECK );
	}
#endif

	if( bExit )
	{
		g_ioNProtect.SetErrorLog( szErrorLog );
		g_ioNProtect.SetCallBackType( ioNProtect::CBT_ERROR_EXIT );

		g_ioNProtect.SetRunCallBack( false );
		return false;
	}

	g_ioNProtect.SetRunCallBack( false );
	return true;
}

#endif // NPROTECT