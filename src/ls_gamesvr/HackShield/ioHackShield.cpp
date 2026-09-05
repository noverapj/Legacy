#include "stdafx.h"
#include ".\ioHackShield.h"
#include <strsafe.h>

#ifdef HACKSHIELD

ioHackShield *ioHackShield::sg_Instance = NULL;

ioHackShield::ioHackShield(void)
{
	m_bUse          = false;
	m_dwProcessTime = 0;
	m_hServer       = NULL;
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

bool ioHackShield::Start( bool bUse )
{
	m_bUse = bUse;

	if( !m_bUse )
		return true;

	__try
	{
		char szModuleFileName[MAX_PATH]="";
		char szModuleFilePath[MAX_PATH]="";
		GetModuleFileName(NULL, szModuleFilePath, MAX_PATH);
		char szDir[MAX_PATH]="";
		char szPath[MAX_PATH]="";
		_splitpath( szModuleFileName, szDir, szPath, NULL, NULL );
		StringCbPrintf( szModuleFilePath, sizeof( szModuleFilePath ), "%s%s", szDir, szPath );

		char szHsbFilePath[MAX_PATH]="";
		StringCbPrintf( szHsbFilePath, sizeof( szHsbFilePath ),  "%sHackShield\\AntiCpx.hsb", szModuleFilePath );

		m_hServer = _AhnHS_CreateServerObject ( szHsbFilePath ); 
		if ( m_hServer == ANTICPX_INVALID_HANDLE_VALUE ) 
		{
			printf ( "ERROR: _AhnHS_CreateServerObject\n" ); 
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s : Error %d", __FUNCTION__, m_hServer );
			return false; 
		}
	}
	__except (ExceptCallBack (GetExceptionInformation()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioHackShield::Start : Crash : %d", GetLastError() );
		return false;
	}

	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioHackShield::Start : Success." );
	return true;
}

void ioHackShield::End()
{
	if( !m_bUse )
		return;

	if ( m_hServer == ANTICPX_INVALID_HANDLE_VALUE ) 
		return;

	__try
	{
		_AhnHS_CloseServerHandle ( m_hServer );
		m_hServer = ANTICPX_INVALID_HANDLE_VALUE;
	}
	__except (ExceptCallBack (GetExceptionInformation()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s : Crash : %d", __FUNCTION__, GetLastError() );
		return;
	}
}

AHNHS_CLIENT_HANDLE ioHackShield::CreateClient()
{
	if( !m_bUse )
		return NULL;

	AHNHS_CLIENT_HANDLE hClient = NULL;
	__try
	{
		hClient =_AhnHS_CreateClientObject( m_hServer ); 
		if( hClient == ANTICPX_INVALID_HANDLE_VALUE )
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Error", __FUNCTION__ );
			return NULL;
		}
	}
	__except (ExceptCallBack (GetExceptionInformation()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s : Crash : %d", __FUNCTION__, GetLastError() );
		return NULL;
	}

	return hClient;
}

void ioHackShield::CloseClient( AHNHS_CLIENT_HANDLE hClient )
{
	if( !m_bUse )
		return;
	__try
	{
		_AhnHS_CloseClientHandle ( hClient );
		hClient = ANTICPX_INVALID_HANDLE_VALUE;
	}
	__except (ExceptCallBack (GetExceptionInformation()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s : Crash : %d", __FUNCTION__, GetLastError() );
	}
}

bool ioHackShield::MakeRequest( IN AHNHS_CLIENT_HANDLE hClient, OUT AHNHS_TRANS_BUFFER &rkBuf )
{
	if( !m_bUse )
		return true;

	__try
	{
		unsigned long ulRet = _AhnHS_MakeRequest ( hClient, &rkBuf );
		if ( ulRet != ERROR_SUCCESS ) 
		{ 
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Error %x", __FUNCTION__, ulRet );
			return false;
		}
	}
	__except (ExceptCallBack (GetExceptionInformation()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s : Crash : %d", __FUNCTION__, GetLastError() );
		return false;
	}

	return true;
}

bool ioHackShield::VerifyRespose( IN AHNHS_CLIENT_HANDLE hClient, IN AHNHS_TRANS_BUFFER &rkBuf )
{
	if( !m_bUse )
		return true;

	__try
	{
		ULONG ulLastError = 0;
		unsigned long ulRet = _AhnHS_VerifyResponseEx ( hClient, rkBuf.byBuffer, rkBuf.nLength, &ulLastError );
		if ( ulRet == ANTICPX_RECOMMAND_CLOSE_SESSION )
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Error %x : %x", __FUNCTION__, ulRet, ulLastError );
			return false;
		}
	}
	__except (ExceptCallBack (GetExceptionInformation()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s : Crash : %d", __FUNCTION__, GetLastError() );
		return false;
	}

	return true;
}

#endif 