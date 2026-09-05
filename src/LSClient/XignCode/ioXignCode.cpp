#include "stdafx.h"
#include ".\ioXignCode.h"

#ifdef XIGNCODE
#include "../NetworkWrappingFunc.h"
#include "..\ioApplication.h"
#include "..\io3DEngine\Safesprintf.h"
#include "..\ioMyInfo.h"
#include <strsafe.h>
#include "..\io3DEngine\ioStringManager.h"

ioXignCode *ioXignCode::sg_Instance = NULL;

extern bool g_bHappenError;
extern int  g_iErrorNumber;
extern char g_szErrorLog[2048];

extern LONG __stdcall ExceptCallBack ( EXCEPTION_POINTERS * pExPtrs );

void CALLBACK OnProbeComplete( HANDLE CodeBox, CPACKETDATA ReceivedPacket, PACKETDATA Buffer, SIZE_T BufferSize, PVOID Context );

ioXignCode::ioXignCode(void)
{
	m_bRunCallBack      = false;
	m_eCallBackType     = CBT_NONE;
}

ioXignCode::~ioXignCode(void)
{
}

ioXignCode & ioXignCode::GetInstance()
{
	if( sg_Instance == NULL )
		sg_Instance = new ioXignCode;

	return (*sg_Instance);
}

void ioXignCode::ReleaseInstance()
{
	if( sg_Instance )
		delete sg_Instance;

	sg_Instance = NULL;
}

bool ioXignCode::Start()
{
	FILE *pFile = fopen( "info\\pp.log", "wt" ); // LOG 셋팅전이기 때문에 fopen으로 기록한다.
	if( pFile )
	{
		fprintf( pFile, "Check - 1 - start:%d\n", timeGetTime() );
		fflush( pFile );
	}

	__try
	{
		if( !ZCWAVE_SysInit() )
		{
			return false;
		}

		if( !ZCWAVE_SysEnter( "FX7cjiycIV3G", "XIGNCODE", ZSOPT_USESYSGUARD ) )
		{
			return false;
		}
	}
	__except(ExceptCallBack( GetExceptionInformation() ) )
	{
		
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


void ioXignCode::OnConnect()
{
	LOG.PrintTimeAndLog( 0, "Check - 2 - start");
	__try
	{
		ZCWAVE_Init();
	}
	__except(ExceptCallBack( GetExceptionInformation() ) )
	{
		LOG.PrintTimeAndLog( 0, "Check - 2 - error : %d", GetLastError() );
		return;
	}

	LOG.PrintTimeAndLog( 0, "Check - 2 - end");
}

void ioXignCode::OnDisconnect()
{
	LOG.PrintTimeAndLog( 0, "Check - 3 - start");
	__try
	{
		ZCWAVE_Cleanup();
	}
	__except(ExceptCallBack( GetExceptionInformation() ) )
	{
		LOG.PrintTimeAndLog( 0, "Check - 3 - error : %d", GetLastError() );
		return;
	}

	LOG.PrintTimeAndLog( 0, "Check - 3 - end");
}


void ioXignCode::Cleanup()
{
	LOG.PrintTimeAndLog( 0, "Check - 4 - start");
	__try
	{
		ZCWAVE_SysExit();
		ZCWAVE_SysCleanup();
	}
	__except(ExceptCallBack( GetExceptionInformation() ) )
	{
		LOG.PrintTimeAndLog( 0, "Check - 4 - error : %d", GetLastError() );
		return;
	}

	LOG.PrintTimeAndLog( 0, "Check - 4 - end");
}

// callback 함수는 다른 쓰레드에서 동작하므로 동기화 처리함.
void CALLBACK OnProbeComplete( HANDLE CodeBox, CPACKETDATA ReceivedPacket, PACKETDATA Buffer, SIZE_T BufferSize, PVOID Context )
{
	g_ioXignCode.SetRunCallBack( true );
	
	XignCodePacket kPacket;
	memcpy( kPacket.m_XignCodePacket, Buffer, BufferSize );
	g_ioXignCode.SetPacket( kPacket );
	g_ioXignCode.SetCallBackType( ioXignCode::CBT_SEND_CHECK );

	g_ioXignCode.SetRunCallBack( false );
}

void ioXignCode::SetPacket( const XignCodePacket &rkPacket )
{
	m_XignCodePacket = rkPacket;
}

void ioXignCode::OnRecieveCheck( XignCodePacket &rkPacket )
{
	__try
	{
		ZCWAVE_ProbeEx( (char*) rkPacket.m_XignCodePacket, sizeof( XignCodePacket ), OnProbeComplete, NULL );
	}
	__except(ExceptCallBack( GetExceptionInformation() ) )
	{
		LOG.PrintTimeAndLog( 0, "Check - 5 - error : %d", GetLastError() );
	}
}

void ioXignCode::Process()
{
	if( m_eCallBackType == CBT_NONE )
		return;
	if( m_bRunCallBack )
		return;
	
	if( m_eCallBackType == CBT_SEND_CHECK )
	{
		SP2Packet kPacket( CTPK_PROTECT_CHECK );
		kPacket << m_XignCodePacket;
		TCPNetwork::SendToServer( kPacket );
	}

	m_eCallBackType = CBT_NONE;
}


#endif // XIGNCODE