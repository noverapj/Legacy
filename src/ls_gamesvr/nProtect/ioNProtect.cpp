#include "stdafx.h"
#include ".\ionprotect.h"

#ifdef NPROTECT

#ifdef NPROTECT_CSAUTH3
	#include "../nProtect/ggsrv30.h"
#else
	#include "../nProtect/ggsrv25.h"
#endif

#ifdef NPROTECT_CSAUTH3
	void __stdcall CS3LogCallback( int nLogType, char *szLog )
	{
		if( nLogType == LOG_UPDATE )
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "this is update log : %s\n", szLog );
		else if( nLogType == LOG_NORMAL )
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "this is normal log : %s\n", szLog);
		//else if( nLogType == LOG_DEBUG ) // 라이브에 사용하지 말것
		//	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "this is debug log : %s\n", szLog );
	}

	void __stdcall CS3UpdateInfoCallback( int nUpdateType, int nBefore, int nAfter )
	{
		if( nUpdateType == UPDATE_PROTOCOL )
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "this is protocol update notification : %d -> %d \n", nBefore, nAfter);
		else if( nUpdateType == UPDATE_GGVERSION )
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "this is ggversion update notification : %d -> %d \n", nBefore, nAfter);
	}
#endif

ioNProtect *ioNProtect::sg_Instance = NULL;

ioNProtect::ioNProtect(void)
{
	m_bUse          = false;
	m_dwProcessTime = 0;
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

bool ioNProtect::Start( bool bUse )
{
	m_bUse = bUse;

	if( !m_bUse )
		return true;

	__try
	{
#ifdef NPROTECT_CSAUTH3
	 UINT32 uReturn = InitCSAuth3("./nProtect/");
	 if( uReturn != ERROR_SUCCESS )
	 {
		 LOG.PrintTimeAndLog(LOG_DEBUG_LEVEL, "ioNProtect::Start : Error : InitCSAuth3() %d", uReturn );
		 return false;
	 }
	 LOG.PrintTimeAndLog(LOG_DEBUG_LEVEL, "ioNProtect::Start : Success : InitCSAuth3() %d", uReturn );

//	 SetCallbackFunction( CALLBACK_LOG, (PVOID)CS3LogCallback );
//	 SetCallbackFunction( CALLBACK_UPDATE, (PVOID)CS3UpdateInfoCallback );
#else
		DWORD dwRet = InitGameguardAuth("./nProtect/", 50, true, 0x03);
		if( dwRet != ERROR_SUCCESS )
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioNProtect::Start : Error : InitGameguardAuth() %d", dwRet );
			return false;
		}
		SetUpdateCondition(30, 50); // 30분동안 50% 이상
#endif
	}
	__except (ExceptCallBack (GetExceptionInformation()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioNProtect::Start : Crash : %d", GetLastError() );
		return false;
	}

	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioNProtect::Start : Success." );
	return true;
}

void ioNProtect::End()
{
	if( m_bUse )
	{
#ifdef NPROTECT_CSAUTH3
		CloseCSAuth3();
#else 
		CleanupGameguardAuth();
#endif
	}
}

void ioNProtect::Process()
{
	if( TIMEGETTIME() - m_dwProcessTime < UPDATE_ALGORITHM_TIME  )
		return;
	m_dwProcessTime = TIMEGETTIME();

	FUNCTION_TIME_CHECKER( 100000.0f, 0 );   

	if( m_bUse )
	{
#ifndef NPROTECT_CSAUTH3
		GGAuthUpdateTimer();
#endif
	}
}

#ifndef NPROTECT_CSAUTH3
GGAUTHS_API void NpLog(int mode, char* msg)
{
	//if(mode & (NPLOG_DEBUG | NPLOG_ERROR) )  // 디버그가 필요하면 NPLOG_DEBUG 로그도 같이 확인
	if(mode & NPLOG_ERROR ) 
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, msg );
};

GGAUTHS_API void GGAuthUpdateCallback(PGG_UPREPORT report)
{
	if( report )
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "GGAuth version update [%s] : [%ld] -> [%ld] \n", report->nType==1?"GameGuard Ver":"Protocol Num",  report->dwBefore, report->dwNext );
};
#endif


#endif 