#include "stdafx.h"
#include ".\ioXignCode.h"

#ifdef XIGNCODE
#include "../XignCode/zwave_sdk_helper.h"
#include "../NodeInfo/User.h"
#include "../Window.h"

ioXignCode *ioXignCode::sg_Instance = NULL;

BOOL WINAPI SendProc( xpvoid uid, PVOID meta, LPCSTR buf, SIZE_T size );
void WINAPI VerifyProc( xpvoid uid, xpvoid meta, int code, xctstr report );

ioXignCode::ioXignCode(void)
{
	m_pXignCode     = NULL;
	m_bUse          = false;
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

bool ioXignCode::Start( bool bUse )
{
	m_bUse = bUse;

	if( !m_bUse )
		return true;

	__try
	{
		CreateXigncodeServer CXProc = LoadHelperDll(NULL);
		if( !CXProc )
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s : Error 1: GetLastError %0xx", __FUNCTION__, GetLastError() );
			return false;
		}

		if( !CXProc( &m_pXignCode, SendProc, VerifyProc ) ) 
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s : Error 2: GetLastError %0xx", __FUNCTION__, GetLastError() );
			return false;
		}

		if( !m_pXignCode )
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s : Error 3: GetLastError %0xx", __FUNCTION__, GetLastError() );
			return false;
		}

		if( !m_pXignCode->OnBegin( MAX_XIGNCODE_PACKET_BUF ) )
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s : Error 4: GetLastError %0xx", __FUNCTION__, GetLastError() );
			return false;
		}
	}
	__except (ExceptCallBack (GetExceptionInformation()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioXignCode::Start : Crash : %0xx", GetLastError() );
		return false;
	}

	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioXignCode::Start : Success." );
	return true;
}

void ioXignCode::SetUserInformation( void *pUser, const char *szIP, const char *szPrivateID )
{
	if( pUser == NULL )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s pUser == NULL.", __FUNCTION__ );
		return;
	}

	if( szIP == NULL )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s szIP == NULL.", __FUNCTION__ );
		return;
	}

	if( szPrivateID == NULL )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s szPrivateID == NULL.", __FUNCTION__ );
		return;
	}

	UINT nAddr = inet_addr(szIP);

	if( m_pXignCode )
		m_pXignCode->SetUserInformationA( pUser, nAddr, szPrivateID );
}

void ioXignCode::End()
{
	if( m_pXignCode )
	{
		m_pXignCode->OnEnd();
		m_pXignCode->Release();
	}
}


void ioXignCode::OnAccept( void *pUser )
{
	if( pUser == NULL )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s pUser == NULL.", __FUNCTION__ );
		return;
	}
	if( m_pXignCode )
		m_pXignCode->OnAccept( pUser, NULL );
}

void ioXignCode::OnReceive( void *pUser, BYTE *pData )
{
	if( pUser == NULL )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s pUser == NULL.", __FUNCTION__ );
		return;
	}

	if( pData == NULL )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s pData == NULL.", __FUNCTION__ );
		return;
	}

	if( m_pXignCode )
		m_pXignCode->OnRecieve( pUser, (const char *)pData, MAX_XIGNCODE_PACKET_BUF );
}

void ioXignCode::OnDisconnect( void *pUser )
{
	if( pUser == NULL )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s pUser == NULL.", __FUNCTION__ );
		return;
	}

	if( m_pXignCode )
		m_pXignCode->OnDisconnect( pUser );
}

BOOL WINAPI SendProc( xpvoid uid, PVOID meta, LPCSTR buf, SIZE_T size )
{
	if( g_App.IsWantExit() || g_App.IsReserveLogOut() )
		return FALSE;

	User *pUser = static_cast<User*>( uid );
	if( pUser == NULL )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s pUser == NULL.", __FUNCTION__ );
		return FALSE;
	}

	XignCodePacket kXignCodePacket;
	memcpy( kXignCodePacket.m_XignCodePacket, buf, size );

	SP2Packet kPacket( STPK_PROTECT_CHECK );
	kPacket << kXignCodePacket;
	pUser->SendMessage( kPacket );
	return TRUE;
}

void WINAPI VerifyProc( xpvoid uid, xpvoid meta, int code, xctstr report )
{
	if( g_App.IsWantExit() || g_App.IsReserveLogOut() )
		return;

	User *pUser = static_cast<User*>( uid );
	if( pUser == NULL )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s pUser == NULL.", __FUNCTION__ );
		return;
	}

	if( code == Z_RTN_ERROR )
	{
		// 로그
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s : %d : %d : %s : %s", __FUNCTION__, uid, pUser->GetUserIndex(), pUser->GetPublicID().c_str(), report );
	}
	else if( code == Z_RTN_NONCLIENT )
	{
		// 로그
		// 종료
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s : %d : %d : %s : %s", __FUNCTION__, uid, pUser->GetUserIndex(), pUser->GetPublicID().c_str(), report );
		pUser->ExceptionClose( 0 );
	}
	else if( code == Z_RTN_BLACK_CODE )
	{
		// 로그
		// 종료
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s : %d : %d : %s : %s", __FUNCTION__, uid, pUser->GetUserIndex(), pUser->GetPublicID().c_str(), report );
		pUser->ExceptionClose( 0 );
	}
	else if( code == Z_RTN_TIMEOUT )
	{
		// 로그
		// 종료
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s : %d : %d : %s : %s", __FUNCTION__, uid, pUser->GetUserIndex(), pUser->GetPublicID().c_str(), report );
		pUser->ExceptionClose( 0 );
	}
}

#endif 