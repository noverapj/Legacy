#include "stdafx.h"
#include ".\ioHackShield.h"

#ifdef HACKSHIELD

#include "../NetworkWrappingFunc.h"
#include "..\ioApplication.h"
#include "..\io3DEngine\Safesprintf.h"
#include "..\ioMyInfo.h"
#include <strsafe.h>
#include "..\io3DEngine\ioStringManager.h"
#include "HSUpChk.h"
#include "..\Version.h"
#include "HShield.h"

ioHackShield *ioHackShield::sg_Instance = NULL;

extern bool g_bHappenError;
extern int  g_iErrorNumber;
extern char g_szErrorLog[2048];

extern LONG __stdcall ExceptCallBack ( EXCEPTION_POINTERS * pExPtrs );

int __stdcall HS_CallbackProc ( long iCode, long iParamSize, void* pParam );

ioHackShield::ioHackShield(void)
{
	m_dwStartErrorType  = 0;
	m_bRunCallBack      = false;
	m_eCallBackType     = CBT_NONE;

	ZeroMemory( m_szModuleFilePath, sizeof( m_szModuleFilePath ) );
	SetModuleFilePath();
}

ioHackShield::~ioHackShield(void)
{
}

ioHackShield & ioHackShield::GetInstance()
{
	if( sg_Instance == NULL )
		sg_Instance = new ioHackShield;

	return (*sg_Instance);
}

void ioHackShield::ReleaseInstance()
{
	if( sg_Instance )
		delete sg_Instance;

	sg_Instance = NULL;
}


bool ioHackShield::Update()
{
	FILE *pFile = fopen( "info\\pp.log", "wt" ); // LOG 셋팅전이기 때문에 fopen으로 기록한다.
	if( pFile )
	{
		fprintf( pFile, "Check - 1 - start:%d\n", timeGetTime() );
		fflush( pFile );
	}

	char szFullFilePath[MAX_PATH]=""; // 핵쉴드 폴더 위치를 지정합니다. 
	StringCbPrintf( szFullFilePath, sizeof( szFullFilePath ), "%sHShield", m_szModuleFilePath ); 

	__try
	{
		AHNHS_EXT_ERRORINFO HsExtError = {0,};
		StringCbCopy( HsExtError.szServer, sizeof( HsExtError.szServer ), "127.0.0.1" ); //모니터링주소 
		StringCbCopy( HsExtError.szGameVersion, sizeof( HsExtError.szGameVersion ), STRFILEVER ); //Game 버전

		// _AhnHS_HSUpdate 함수 호출 
 		DWORD dwRet = _AhnHS_HSUpdateEx( szFullFilePath, // 핵쉴드 폴더 경로 
 			                            1000 * 600,      // 업데이트 전체 타임아웃 
 										6055,            // 게임 코드
 										AHNHSUPDATE_CHKOPT_HOSTFILE| AHNHSUPDATE_CHKOPT_GAMECODE,
 										HsExtError,
 				                        1000 * 20 );     // 서버 연결 타임아웃 
 				                        
//		DWORD dwRet = _AhnHS_HSUpdate( szFullFilePath, 1000 * 5 );

		if( dwRet != ERROR_SUCCESS )
		{
			if( pFile )
			{
				fprintf( pFile, "Check - 1 - error:%d:%x\n", timeGetTime(), dwRet );
				fflush( pFile );
				fclose( pFile );
			}

			m_dwStartErrorType = dwRet;
		}

		if( dwRet == HSERROR_ENVFILE_NOTREAD )
			return false;
		else if( dwRet == HSERROR_ENVFILE_NOTWRITE )
			return false;
		else if( dwRet == HSERROR_NETWORK_CONNECT_FAIL )
			return false;
		else if( dwRet == HSERROR_HSUPDATE_TIMEOUT )
			return false;
		else if( dwRet == HSERROR_MISMATCH_ENVFILE )
			return false;
		else if( dwRet == HSERROR_HOSTFILE_MODIFICATION )
			return false;
		else if( dwRet != ERROR_SUCCESS )
			return false;
	}
	__except(ExceptCallBack( GetExceptionInformation() ) )
	{
		if( pFile )
		{
			fprintf( pFile, "Check - 1 - except:%d\n", timeGetTime() );
			fflush( pFile );
		}

		if( pFile )
			fclose( pFile );
		return false;
	}

	if( pFile )
	{
		fprintf( pFile, "Check - 1 - end:%d\n", timeGetTime() );
		fflush( pFile );
	}
	if( pFile )
		fclose( pFile );

	return true;
}

bool ioHackShield::Start()
{
	FILE *pFile = fopen( "info\\pp.log", "wt" ); // LOG 셋팅전이기 때문에 fopen으로 기록한다.
	if( pFile )
	{
		fprintf( pFile, "Check - 2 - start:%d\n", timeGetTime() );
		fflush( pFile );
	}

	char szFullFilePath[MAX_PATH]=""; 
	StringCbPrintf( szFullFilePath, sizeof( szFullFilePath ), "%sHShield\\EhSvc.dll", m_szModuleFilePath ); 
	DWORD dwOption = AHNHS_CHKOPT_ALL | AHNHS_CHKOPT_LOCAL_MEMORY_PROTECTION; 
	__try
	{
		int iRet = _AhnHS_Initialize ( szFullFilePath, 
			                           HS_CallbackProc, 
									   6055,                       // 게임 코드
									   "F48AA6C262FFA39014545B6D", // 라이선스 키 
			                           dwOption,                   
			                           AHNHS_SPEEDHACK_SENSING_RATIO_NORMAL ); 

		if ( iRet != HS_ERR_OK ) 
		{ 
			m_dwStartErrorType = iRet;
			if( pFile )
			{
				fprintf( pFile, "Check - 2 - error:%d:%x\n", timeGetTime(), iRet );
				fflush( pFile );
				fclose( pFile );
			}
			return false;
		} 

		iRet = _AhnHS_StartService(); 
		if ( iRet != HS_ERR_OK ) 
		{ 
			m_dwStartErrorType = iRet;
			if( pFile )
			{
				fprintf( pFile, "Check - 2 - error:%d:%x\n", timeGetTime(), iRet );
				fflush( pFile );
				fclose( pFile );
			}
			return false;
		} 
	}
	__except(ExceptCallBack( GetExceptionInformation() ) )
	{
		if( pFile )
		{
			fprintf( pFile, "Check - 2 - except:%d\n", timeGetTime() );
			fflush( pFile );
		}

		if( pFile )
			fclose( pFile );
		return false;
	}

	if( pFile )
	{
		fprintf( pFile, "Check - 2 - end:%d\n", timeGetTime() );
		fflush( pFile );
	}
	if( pFile )
		fclose( pFile );

	return true;
}

void ioHackShield::End()
{
	LOG.PrintTimeAndLog( 0, "Check - 3 - start");
	__try
	{
		int iRet = _AhnHS_StopService();
		if ( iRet != HS_ERR_OK ) 
		{
			LOG.PrintTimeAndLog( 0, "Check - 3 - error : %x", iRet );
		} 

		iRet = _AhnHS_Uninitialize(); 
		if ( iRet != HS_ERR_OK ) 
		{
			LOG.PrintTimeAndLog( 0, "Check - 3-1 - error : %x", iRet );
		} 
	}
	__except(ExceptCallBack( GetExceptionInformation() ) )
	{
		LOG.PrintTimeAndLog( 0, "Check - 3 - crash : %d", GetLastError() );
	}
	LOG.PrintTimeAndLog( 0, "Check - 3 - end");
}

bool ioHackShield::OnRecieveCheck( IN HackShieldPacket &rRecvBuf, OUT HackShieldPacket &rSendBuf )
{
	__try
	{
		unsigned long ulRet = _AhnHS_MakeResponse ( rRecvBuf.byBuffer, rRecvBuf.nLength, (_AHNHS_TRANS_BUFFER*)&rSendBuf ); 
		if ( ulRet != ERROR_SUCCESS ) 
		{ 
			LOG.PrintTimeAndLog( 0, "Check - 4 - error : %x", ulRet );
			return false;
		}
	}
	__except(ExceptCallBack( GetExceptionInformation() ) )
	{
		LOG.PrintTimeAndLog( 0, "Check - 4 - crash : %d", GetLastError() );
		return false;
	}

	return true;
}

void ioHackShield::ShowErrorDlg( const ioHashString &rsPrivateID )
{
	LOG.PrintTimeAndLog( 0, "Check - 3 - start");
	__try
	{
		char szFullFilePath[MAX_PATH]=""; 
		StringCbPrintf( szFullFilePath, sizeof( szFullFilePath ), "%sHShield", m_szModuleFilePath ); 
		_AhnHS_SendHsLog ( GetErrorCode(), rsPrivateID.c_str(), szFullFilePath ); 
	}
	__except(ExceptCallBack( GetExceptionInformation() ) )
	{
		LOG.PrintTimeAndLog( 0, "Check - 5 - crash : %d", GetLastError() );
	}
	LOG.PrintTimeAndLog( 0, "Check - 3 - end");
}

void ioHackShield::Process()
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

		LOG.PrintTimeAndLog( 0, "ExitProgram - 15" );
		g_App.SetExitProgram();
	}

	m_eCallBackType = CBT_NONE;
}

void ioHackShield::SetModuleFilePath()
{
	char szModuleFileName[MAX_PATH]="";
	GetModuleFileName(NULL, szModuleFileName, MAX_PATH);
	char szDir[MAX_PATH]="";
	char szPath[MAX_PATH]="";
	_splitpath( szModuleFileName, szDir, szPath, NULL, NULL );
	StringCbPrintf( m_szModuleFilePath, sizeof( m_szModuleFilePath ), "%s%s", szDir, szPath );
}


// callback 함수는 다른 쓰레드에서 동작하므로 동기화 처리함.
int __stdcall HS_CallbackProc( long iCode, long iParamSize, void* pParam )
{
	g_ioHackShield.SetRunCallBack( true );

	char szErrorLog[MAX_PATH*2]="";
	bool bExit = false;
	if( iCode == AHNHS_ENGINE_DETECT_GAME_HACK )
	{
		SafeSprintf( szErrorLog, sizeof(szErrorLog), STR(1), iCode, (LPTSTR) pParam, g_MyInfo.GetPrivateID().c_str(), g_MyInfo.GetMyIP().c_str() );
		bExit = true;
	}
	else if( iCode == AHNHS_ACTAPC_DETECT_AUTOMACRO )
	{
		SafeSprintf( szErrorLog, sizeof(szErrorLog), STR(2), iCode , g_MyInfo.GetPrivateID().c_str(), g_MyInfo.GetMyIP().c_str() );
		bExit = true;
	}
	else if( iCode == AHNHS_ACTAPC_DETECT_SPEEDHACK )
	{
		SafeSprintf( szErrorLog, sizeof(szErrorLog), STR(3), iCode, g_MyInfo.GetPrivateID().c_str(), g_MyInfo.GetMyIP().c_str() );
		bExit = true;
	}
	else if( iCode == AHNHS_ACTAPC_DETECT_KDTRACE ||
		     iCode == AHNHS_ACTAPC_DETECT_KDTRACE_CHANGED )
	{
		SafeSprintf( szErrorLog, sizeof(szErrorLog), STR(4), iCode, g_MyInfo.GetPrivateID().c_str(), g_MyInfo.GetMyIP().c_str() );
		bExit = true;
	}
	else if( iCode == AHNHS_ACTAPC_DETECT_DRIVERFAILED   ||
		     iCode == AHNHS_ACTAPC_DETECT_HOOKFUNCTION   ||
		     iCode == AHNHS_ACTAPC_DETECT_MODULE_CHANGE  ||
		     iCode == AHNHS_ACTAPC_DETECT_LMP_FAILED          ||
		     iCode == AHNHS_ACTAPC_DETECT_MEM_MODIFY_FROM_LMP ||
		     iCode == AHNHS_ACTAPC_DETECT_ENGINEFAILED        ||
		     iCode == AHNHS_ACTAPC_DETECT_CODEMISMATCH        ||
		     iCode == AHNHS_ACTAPC_DETECT_ANTIFREESERVER      ||
		     iCode == AHNHS_ACTAPC_DETECT_ABNORMAL_HACKSHIELD_STATUS ||
			 iCode == AHNHS_ACTAPC_DETECT_AUTOMOUSE                  ||
			 iCode == AHNHS_ACTAPC_DETECT_ALREADYHOOKED )
	{
		SafeSprintf( szErrorLog, sizeof(szErrorLog), STR(5), iCode, g_MyInfo.GetPrivateID().c_str(), g_MyInfo.GetMyIP().c_str() );
		bExit = true;
	}
	else if( iCode == AHNHS_ENGINE_DETECT_WINDOWED_HACK )
	{
		// 별도의 처리를 하지 않음
	}

	if( bExit )
	{
		g_ioHackShield.SetErrorCode( iCode );
		g_ioHackShield.SetErrorLog( szErrorLog );
		g_ioHackShield.SetCallBackType( ioHackShield::CBT_ERROR_EXIT );

		g_ioHackShield.SetRunCallBack( false );
		return false;
	}

	g_ioHackShield.SetRunCallBack( false );
	return true;
}

#endif // HACKSHIELD