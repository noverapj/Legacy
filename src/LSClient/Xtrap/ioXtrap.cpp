#include "stdafx.h"
#include ".\ioxtrap.h"

#ifdef XTRAP

#include "../WndID.h"
#include "../ioApplication.h"
#include "../GUI/ioSP2GUIManager.h"
#include "../GUI/DisconnectWnd.h"
#include "../io3DEngine/ioStringManager.h"

#include <strsafe.h>
#include "../Local/ioLocalManager.h"

#include "Xtrap4Client.h"
#include "XTrap4ClientExt1.h"
#include "../Local/ioLocalUS.h"

// 패치 URL이 변경되면 다시 변경해야 함
#define XTRAP_ID_STR    "660970B47849DB33C5156E984ACFEF628633E4C9A9CC12F8E5CCF2E41D264668D7508833E87986C58341C5C9C04B9BD404814CA12A605CD5A7F3852A7C2D9CDE0F7D04245246E40BAA66194DDA5776406029F86FCC6AE149EE6D83D9BDF39F6498A5A4F0ED1E44027AB6BC6CD95B3CFB1D1F26A4E0AB8BE5B6E81EF3476B89"
#define XTRAP_ID_STR_US "660970B4784BDB4FC23D6D984ACFE862B1436F4900D90C117B3D2831463DE83FD31DE6C9350A59A30A437D5655BC70E4D2E11C20720727A57829D7B0ACAB54B30F7D04245246E404A4770450DE443902E615B347471B286642EC3EC9F7365271A737F4FD1D6FBBEFF32B"
                
// Xtrap 함수에서 크래쉬가 발생하는 경우 확인이 어려우므로 함수 시작과 끝에 로그를 남김
// Xtrap 함수가 계속적으로 호출되는 함수라면 로그량이 많아지므로 로그를 남기지 말것.
// 로그이름을 유저가 인지하지 못하도록 그냥 check라고만 정의

extern LONG __stdcall ExceptCallBack ( EXCEPTION_POINTERS * pExPtrs );

ioXtrap *ioXtrap::sg_Instance = NULL;

ioXtrap::ioXtrap(void)
{
	// XTRAP_PROTECT_PE | XTRAP_PROTECT_TEXT | XTRAP_PROTECT_EXCEPT_VIRUS가 주로사용하는 기본 옵션이며, 해킹이 심한 경우 Xtrap과 협의후 RData와 EData 늘려 갈 수 있음. FilePatchOpiont은 Xtrap 호환성 테스트를 받아야함.
	m_dwMethod = XTRAP_PROTECT_PE | XTRAP_PROTECT_TEXT | XTRAP_PROTECT_EXCEPT_VIRUS; // 서버, 클라이언트, X-Protect 값이 동일해야 함.
	m_bPayment = false;
	m_dwPreTime= 0;
}

ioXtrap::~ioXtrap(void)
{
}

bool ioXtrap::Start()
{
	FILE *pFile = fopen( "info\\pp.log", "wt" ); // LOG 셋팅전이기 때문에 fopen으로 기록한다.
	if( pFile )
	{
		fprintf( pFile, "Check - 1 - start:%d\n", timeGetTime() );
		fflush( pFile );
	}
	DWORD dwResult = 0;
	__try
	{
		// 한국
		XTrap_C_Start( XTRAP_ID_STR, NULL );

		// 미국
		//XTrap_C_Start( XTRAP_ID_STR_US, NULL );
		//ioLocalUS::SetEU( false );

		// 유럽
		//XTrap_C_Start( XTRAP_ID_STR_US, NULL );
		//ioLocalUS::SetEU( true );


		dwResult = XTrap_CE1_Func5_Init();

		if( dwResult != XTRAP_API_RETURN_OK )
		{
			fprintf( pFile, "Check - 1 - error:%d:%d\n", dwResult, GetLastError() );
			fflush( pFile );
			fclose( pFile );
			return false;
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER ) // 처음시작시 이므로 ""만 한다
	{
		fprintf( pFile, "Check - 1 - error:%d\n", GetLastError() );
		fflush( pFile );
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

void ioXtrap::KeepAlive()
{
	LOG.PrintTimeAndLog( 0, "Check - 2 - start");
	__try
	{
		XTrap_C_KeepAlive();
	}
	__except(ExceptCallBack( GetExceptionInformation() ) )
	{
		LOG.PrintTimeAndLog( 0, "Check - 2 - error : %d", GetLastError() );
		return;
	}
	LOG.PrintTimeAndLog( 0, "Check - 2 - end");
}

void ioXtrap::SetUserInfo( IN LPCSTR lpUserID, IN LPCSTR lpServerName, IN LPCSTR lpCharacterName, IN LPCSTR lpCharacterClass, IN DWORD dwCharacterLevel )
{
	LOG.PrintTimeAndLog( 0, "Check - 3 - start");
	__try
	{
		XTrap_C_SetUserInfoEx( lpUserID, lpServerName, lpCharacterName, lpCharacterClass, dwCharacterLevel, NULL );
	}
	__except(ExceptCallBack( GetExceptionInformation() ) )
	{
		LOG.PrintTimeAndLog( 0, "Check - 3 - error : %d", GetLastError() );
		return;
	}
	LOG.PrintTimeAndLog( 0, "Check - 3 - end");
}

ioXtrap &ioXtrap::GetInstance()
{
	if( sg_Instance == NULL )
		sg_Instance = new ioXtrap;

	return (*sg_Instance);
}

void ioXtrap::ReleaseInstance()
{
	if( sg_Instance )
		delete sg_Instance;
	
	sg_Instance = NULL;
}

bool ioXtrap::Step2( IN BYTE *pPacketBuf, OUT BYTE *pOutPacketBuf )
{
	DWORD dwResult = 0;
	__try
	{
		dwResult = XTrap_CS_Step2( pPacketBuf, pOutPacketBuf, m_dwMethod );
	}
	__except(ExceptCallBack( GetExceptionInformation() ) )
	{
		LOG.PrintTimeAndLog( 0, "Check - 4 - error : %d", GetLastError() );
		return false;
	}
	if( dwResult != XTRAP_API_RETURN_OK )
	{
		LOG.PrintTimeAndLog(0, "ioXtrap::Step2 : Fail : %d", dwResult );

		bool bNotifyGUI = false;
		if( ioSP2GUIManager::GetSingletonPtr() )
		{
			DisconnectWnd *pDisWnd = static_cast<DisconnectWnd*> ( g_GUIMgr.FindWnd(DISCONNECT_WND) );
			if( pDisWnd )
			{
				pDisWnd->SetType( DisconnectWnd::TYPE_XTRAP_EXIT );
				pDisWnd->ShowWnd();
				bNotifyGUI = true;
			}
		}

		if( !bNotifyGUI )
		{
			MBox( NULL, STR(1), STR(2) );

			LOG.PrintTimeAndLog( 0, "ExitProgram - 25" );
			g_App.SetExitProgram();
		}
		return false;
	}

	return true;
}

bool ioXtrap::StartPayment()
{
	if( m_bPayment )
			return true;
		DWORD dwResult = 0;
		__try
		{
			dwResult = XTrap_CE1_Func6_Payment();
		}
		__except(ExceptCallBack( GetExceptionInformation() ) )
		{
			LOG.PrintTimeAndLog( 0, "Check - 5 - error : %d", GetLastError() );
			return false;
		}
		if( dwResult != XTRAP_API_RETURN_OK )
		{
			LOG.PrintTimeAndLog(0, "ioXtrap::StartPayment : Fail : %d", dwResult );
			return false;
		}
		m_bPayment = true;
	
	return true;
}

bool ioXtrap::EndPayment()
{
	if( !m_bPayment )
		return true;
	DWORD dwResult = 0;
	__try
	{
		dwResult = XTrap_CE1_Func7_Payment();
	}
	__except(ExceptCallBack( GetExceptionInformation() ) )
	{
		LOG.PrintTimeAndLog( 0, "Check - 6 - error : %d", GetLastError() );
		return false;
	}
	if( dwResult != XTRAP_API_RETURN_OK )
	{
		LOG.PrintTimeAndLog(0, "ioXtrap::EndPayment : Fail : %d", dwResult );
		return false;
	}
	m_bPayment = false;

	return true;
}

void ioXtrap::Process()
{
	if( ( timeGetTime() - m_dwPreTime ) < MAX_PROCESS_MS )
		return;
	m_dwPreTime = timeGetTime();
	XTrap_C_CallbackAlive( MAX_PROCESS_MS );
}
#endif // XTRAP